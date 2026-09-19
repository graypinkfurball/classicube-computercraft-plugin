#include "_PluginAPI.h"
#include "src/Chat.h"
#include "src/Core.h"
#include "src/Commands.h"
#include "src/Game.h"
#include "src/String_.h"
#include "TerminalScreen.h"
#include "Textures.h"

// for Mem_TryAlloc, Mem_Free
#include "src/Platform.h"

#define PLUGIN_VERSION "v0.14.0"

// OMG: Remember LocalPlayer VTable Render Hook trick

static struct TerminalBuffer buffer = { 0, 0, NULL };
static void InitBuffer(int width, int height) {
  int n = width*height*3;
  cc_uint8 *data = Mem_TryAlloc(1, n);
  if (!data) { Chat_Add2("&eFailed to allocate memory for terminal buffer (%i x %i)", &width, &height); return; }
  Mem_Free(buffer.data);
  for (int i = 0; i < n; i++) { data[i] = 0; }
  buffer.data = data;
  buffer.width = width;
  buffer.height = height;
}

static void TShow_Execute(const cc_string *args, int args_count) {
  TerminalScreen_Show(&buffer, DefaultPalette);
}

static void TResize_Execute(const cc_string *args, int args_count) {
  if (args_count < 2) { Chat_Add1("&eTwo arguments needed", NULL); return; }
  int width, height;
  if (!Convert_ParseInt(&args[0], &width) || width < 0 || width > 127) { Chat_Add1("&eWidth must be an integer between 0 and 127", NULL); return; }
  if (!Convert_ParseInt(&args[1], &height) || height < 0 || height > 127) { Chat_Add1("&eHeight must be an integer between 0 and 127", NULL); return; }
  InitBuffer(width, height);
  TerminalScreen_Redraw();
}

static int parseHex(char ch) {
  if (ch >= '0' && ch <= '9') { return ch - '0'; }
  if (ch >= 'A' && ch <= 'F') { return ch - 'A' + 10; }
  if (ch >= 'a' && ch <= 'f') { return ch - 'a' + 10; }
  return -1;
}

static cc_bool validateHexStr(const char *p, int len) {
  for (int i = 0; i < len; i++) { if (parseHex(*p++) == -1) { return false; } }
  return true;
}


static void TBlit_Execute(const cc_string *args, int args_count) {
  if (args_count < 5) { Chat_Add1("&eFive arguments needed", NULL); return; }
  int x, y;
  if (!Convert_ParseInt(&args[0], &x)) { Chat_Add1("&eX must be an integer", NULL); return; }
  if (!Convert_ParseInt(&args[1], &y)) { Chat_Add1("&eY must be an integer", NULL); return; }
  
  int len = args[2].length;
  if (args[3].length != len || args[4].length != len) { Chat_Add1("&etext, forecol and backcol must all be the same length", NULL); return; }
  if (len == 0) { Chat_Add1("&etext, forecol and backcol must be non-empty", NULL); return; }
  
  char *text = args[2].buffer, *forecol = args[3].buffer, *backcol = args[4].buffer;
  if (!validateHexStr(forecol, len) || !validateHexStr(backcol, len)) {
    Chat_Add1("&eforecol and backdol must consist of hexadecimal only", NULL); return;
  }
  
  cc_bool replace_underscores = false;
  if (args_count >= 6 && !Convert_ParseBool(&args[5], &replace_underscores)) { Chat_Add1("&ereplaceUnderscores must be 'true' or 'false'", NULL); return; }
  
  // skip text that falls on an off-screen line
  if (y < 0 || y >= buffer.height) { return; }
  
  // left-truncate text falling off left-side of buffer
  if (x < 0) {
    text += -x;
    forecol += -x;
    backcol += -x;
    len -= -x;
    x = 0;
    // skip if entire text falls off left-side of buffer
    if (len <= 0) { return; }
  }
  
  // right-truncate text falling off right-side of buffer
  if (x + len > buffer.width) {
    len = buffer.width - x;
    // skip if entire text falls off right-side of buffer
    if (len <= 0) { return; }
  }
  
  // remaining x and len always fall within a row of the buffer
  cc_uint8 *data = buffer.data + (buffer.width*y+x)*3;
  for (int i = 0; i < len; i++) {
    char ch = *text++;
    if (ch == '_' && replace_underscores) { ch = ' '; }
    *data++ = ch;
    *data++ = parseHex(*forecol++);
    *data++ = parseHex(*backcol++);
  }
  
  TerminalScreen_Redraw();
}

static struct ChatCommand TShow_Command = { "TShow", TShow_Execute, 0, { "&b/TShow"} };
static struct ChatCommand TResize_Command = { "TResize", TResize_Execute, 0, {"&b/TResize [width] [height]"} };
static struct ChatCommand TBlit_Command = { "TBlit", TBlit_Execute, 0, {"&b/TBlit [x] [y] [text] [forecol] [backcol] <replaceUnderscores?true/false>"} };

static void Plugin_Init(void) {
  LoadSymbols();
  
  Commands_Register(&TShow_Command);
  Commands_Register(&TResize_Command);
  Commands_Register(&TBlit_Command);
  
  Textures_Init();
  InitBuffer(51,19);
  
  Chat_Add1("Loaded Computercraft " PLUGIN_VERSION, NULL);
}

static void Plugin_Free(void) {
  Textures_Free();
}

PLUGIN_EXPORT int Plugin_ApiVersion = GAME_API_VER;
PLUGIN_EXPORT struct IGameComponent Plugin_Component = { Plugin_Init, Plugin_Free };

