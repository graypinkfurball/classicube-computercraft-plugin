#include "src/PluginAPI.h"
#include "src/Chat.h"
#include "src/Core.h"
#include "src/Game.h"
#include "src/Event.h"
#include "src/String_.h"

static void OnPackChanged(void *obj) {
  cc_string msg = String_FromConst("OnPackChanged");
  Chat_Add(&msg);
}

static void OnFileChanged(void *obj, struct Stream *stream, const cc_string *name) {
  Chat_Add1("OnFileChanged %s", name);
}

static void OnInit(void) {
  cc_string msg = String_FromConst("Test Plugin Load");
  Chat_Add(&msg);
  
  Event_Register_(&TextureEvents.PackChanged, NULL, OnPackChanged);
  Event_Register_(&TextureEvents.FileChanged, NULL, OnFileChanged);
}

static void OnFree(void) {
  Event_Unregister_(&TextureEvents.PackChanged, NULL, OnPackChanged);
  Event_Unregister_(&TextureEvents.FileChanged, NULL, OnFileChanged);
}

PLUGIN_EXPORT int Plugin_ApiVersion = GAME_API_VER;
PLUGIN_EXPORT struct IGameComponent Plugin_Component = { OnInit, OnFree };

