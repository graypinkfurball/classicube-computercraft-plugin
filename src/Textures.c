#include "_PluginAPI.h"
#include "src/Bitmap.h"
#include "src/Chat.h" // for logging
#include "src/Core.h"
#include "src/Event.h"
#include "src/Graphics.h"
#include "src/Platform.h"
#include "src/Stream.h"
#include "src/String_.h"
#include "Textures.h"

// TODO implement PackChanged solution using a duplicated zip decoder
// TODO generalize interface to allow registering textures from other modules

// TODO move this
static void Log(cc_result res, const char *action, const cc_string *name) {
  Chat_Add3("&cError %e when %c '%s'", &res, action, name);
}

static cc_bool LoadTexture(GfxResourceID *tex, const char *name) {
  cc_string path;
  char path_buffer[64];
  String_InitArray(path, path_buffer);
  String_Format1(&path, "plugins/computercraft/%c", name);
  
  cc_result res;
  struct Stream stream;
  res = Stream_OpenFile(&stream, &path);
  if (res) { Log(res, "opening", &path); return false; }
  
  struct Bitmap bmp;
  res = Png_Decode(&bmp, &stream);
  stream.Close(&stream);
  if (res) { Log(res, "decoding", &path); return false; }
  
  Gfx_DeleteTexture(tex);
  *tex = Gfx_CreateTexture(&bmp, TEXTURE_FLAG_MANAGED, false);
  
  Mem_Free(bmp.scan0);
  return *tex != 0;
}

GfxResourceID border_tex;
GfxResourceID font_tex;

static cc_bool tex_loaded = false;
static void LoadTextures() {
  if (!LoadTexture(&border_tex, "border_normal.png")) { return; }
  if (!LoadTexture(&font_tex, "term_font.png")) { return; }
  tex_loaded = true;
  Chat_Add1("Textures loaded :3", NULL);
}

static void FreeTextures() {
  Gfx_DeleteTexture(&border_tex);
  Gfx_DeleteTexture(&font_tex);
}

static void ContextRecreated(void *obj) {
  if (!Gfx.ManagedTextures || !tex_loaded) { LoadTextures(); }
}

static void ContextLost(void *obj) {
  if (!Gfx.ManagedTextures) { FreeTextures(); }
}

void Textures_Init(void) {
  Event_Register_(&GfxEvents.ContextRecreated, NULL, ContextRecreated);
  Event_Register_(&GfxEvents.ContextLost, NULL, ContextLost);
  LoadTextures();
}

void Textures_Free(void) {
  Event_Unregister_(&GfxEvents.ContextRecreated, NULL, ContextRecreated);
  Event_Unregister_(&GfxEvents.ContextLost, NULL, ContextLost);
  FreeTextures();
}

