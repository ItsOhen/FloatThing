#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/managers/HookSystemManager.hpp>
#include <hyprland/src/managers/LayoutManager.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <src/SharedDefs.hpp>

const std::string PLUGIN_NAME = "Floatthings";
const std::string PLUGIN_DESC = "Float things based on title";
const std::string PLUGIN_VERSION = "0.1";

enum Mode {
  EXACT,
  STARTS_WITH,
  CONTAINS,
  UNKNOWN
};

struct Match {
  Mode mode;
  std::string title;
};

const Match matches[] = {
    {EXACT, "SomeTestTitle"},
    {EXACT, "SomeOtherTestTitle"},
    {STARTS_WITH, "Extension: (Bitwarden Password Manager) - Bitwarden — Zen Browser"},
    {CONTAINS, "Bitwarden Password Manager"},
    {UNKNOWN, ""}};

APICALL EXPORT std::string
PLUGIN_API_VERSION() {
  return HYPRLAND_API_VERSION;
}

inline HANDLE PHANDLE = nullptr;

static SP<HOOK_CALLBACK_FN> titlehook;

static void onWindowTitleChanged(void *hk, SCallbackInfo &info, std::any data) {
  auto w = std::any_cast<PHLWINDOW>(data);

  const auto floatwindow = [](PHLWINDOW w) {
    Log::logger->log(Log::INFO, std::format("[{}] floating {}", PLUGIN_NAME, w->m_title));
    w->m_isFloating = true;
    g_pLayoutManager->getCurrentLayout()->changeWindowFloatingMode(w);
  };

  for (auto match : matches) {
    switch (match.mode) {
    case EXACT:
      if (w->m_title == match.title)
        floatwindow(w);
      break;
    case STARTS_WITH:
      if (w->m_title.starts_with(match.title))
        floatwindow(w);
      break;
    case CONTAINS:
      if (w->m_title.contains(match.title))
        floatwindow(w);
      break;
    default:
      break;
    }
  }
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
  PHANDLE = handle;

  const std::string COMPOSITOR_HASH = __hyprland_api_get_hash();
  const std::string CLIENT_HASH = __hyprland_api_get_client_hash();

  if (COMPOSITOR_HASH != CLIENT_HASH) {
    throw std::runtime_error(std::format("[{}] Version mismatch.. {} {}", PLUGIN_NAME, COMPOSITOR_HASH, CLIENT_HASH));
  }

  titlehook = g_pHookSystem->hookDynamic("windowTitle", onWindowTitleChanged);

  HyprlandAPI::addNotification(PHANDLE, std::format("Loaded {}", PLUGIN_NAME),
                               CHyprColor{0.2, 1.0, 0.2, 1.0}, 5000);
  return {PLUGIN_NAME, PLUGIN_DESC, "Ergon", PLUGIN_VERSION};
}

APICALL EXPORT void PLUGIN_EXIT() {
  titlehook.reset();
  PHANDLE = nullptr;
}
