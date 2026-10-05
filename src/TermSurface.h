#ifndef TERM_SURFACE_H
#define TERM_SURFACE_H
#include "_PluginAPI.h"
#include "src/Core.h"
#include "src/PackedCol.h"

#define TERMPAL_MAX_COLORS 256

struct TermPalette { PackedCol *colors; int count; }

void TermPalette_Init(struct TermPalette *pal, int count);
void TermPalette_Free(struct TermPalette *pal);

void TermPalette_SetColor(struct TermPalette *pal, cc_uint8 col, cc_uint8 red, cc_uint8 green, cc_uint8 blue);

#define TERMBUF_MAX_DIMS 256

struct TermBuffer { cc_uint8 *text, *fgcols, *bgcols; int width, height; };

void TermBuffer_Init(struct TermBuffer *buf, int width, int height);
void TermBuffer_Free(struct TermBuffer *buf);

void TermBuffer_Clear(struct TermBuffer *buf, cc_uint8 bgcol);
void TermBuffer_ClearLine(struct TermBuffer *buf, int y, cc_uint8 bgcol);
void TermBuffer_WriteLine(struct TermBuffer *buf, int x, int y, cc_uint8 *text, cc_uint8 fgcol, cc_uint8 bgcol);
void TermBuffer_BlitLine(struct TermBuffer *buf, int x, int y, cc_uint8 *text, cc_uint8 *fgcols, cc_uint8 *bgcols);

// TODO redefinable font character sizes, this probably shouldnt be defined here (maybe make a BitmapFont.h or smth)
#define CHAR_WIDTH 6
#define CHAR_HEIGHT 9

#define TERMSUR_TEXTURE_MAX_DIMS 1024
#define TERMSUR_MAX_WIDTH (TERM_TEXTURE_MAX_DIMS / CHAR_WIDTH)
#define TERMSUR_MAX_HEIGHT (TERM_TEXURE_MAX_DIMS / CHAR_HEIGHT)

// TODO blinking cursor
struct TermSurface {
  TermPalette *pal;
  TermBuffer *buf;

  // texture rendering
  GfxResourceID
};

void TermSurface_Init(struct TermSurface *sur, struct TermPalette *pal, struct TermBuffer *buf);
void TermSurface_Free(struct TermSurface *sur);


// ah shit, here we go again.
// changing palette will require a redraw.

void TerminalPalette_Alloc(struct TerminalPalette *pal, int count);
void TerminalBuffer_Alloc(struct TerminalBuffer *buf, int width, int height);

TerminalBuffer is exposed and helper methods for it
but TerminalSurface manages its own copy and provides wrappers for updating the surface
or you can grab the buffer, manipulate it directly and then redraw
if this was multithreaded, then you'd need to Lock and Unlock the buffer.

void TerminalScreen_Show(const struct TerminalBuffer *buffer, const PackedCol *palette);
void TerminalScreen_Hide(void);
void TerminalScreen_Redraw(void);
#endif
