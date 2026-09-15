#ifndef TERMINAL_SCREEN_H
#define TERMINAL_SCREEN_H
// TODO should I prefer opaque `struct PackedCol;` forward-declaration over including? size of TerminalPalette will not be known, will that be an issue?
#include "_PluginAPI.h"
#include "src/PackedCol.h"

extern const PackedCol DefaultPalette[16];

struct TerminalBuffer { int width, height; cc_uint8 *data; };

void TerminalScreen_Show(const struct TerminalBuffer *buffer, const PackedCol *palette);
void TerminalScreen_Hide(void);
void TerminalScreen_Redraw(void);
#endif

