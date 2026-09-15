#include "_PluginAPI.h"
#include "src/Graphics.h"
#include "src/Gui.h"
#include "src/PackedCol.h"
#include "src/Window.h"
#include "TerminalScreen.h"
#include "Textures.h"

const PackedCol DefaultPalette[16] = {
  PackedCol_Make(0xF0, 0xF0, 0xF0, 0xFF), // white
  PackedCol_Make(0xF2, 0xB2, 0x33, 0xFF), // orange
  PackedCol_Make(0xE5, 0x7F, 0xD8, 0xFF), // magenta
  PackedCol_Make(0x99, 0xB2, 0xF2, 0xFF), // light blue
  PackedCol_Make(0xDE, 0xDE, 0x6C, 0xFF), // yellow
  PackedCol_Make(0x7F, 0xCC, 0x19, 0xFF), // lime
  PackedCol_Make(0xF2, 0xB2, 0xCC, 0xFF), // pink
  PackedCol_Make(0x4C, 0x4C, 0x4C, 0xFF), // gray
  PackedCol_Make(0x99, 0x99, 0x99, 0xFF), // light gray
  PackedCol_Make(0x4C, 0x99, 0xB2, 0xFF), // cyan
  PackedCol_Make(0xB2, 0x66, 0xE5, 0xFF), // purple
  PackedCol_Make(0x33, 0x66, 0xCC, 0xFF), // blue
  PackedCol_Make(0x7F, 0x66, 0x4C, 0xFF), // brown
  PackedCol_Make(0x57, 0xA6, 0x4E, 0xFF), // green
  PackedCol_Make(0xCC, 0x4C, 0x4C, 0xFF), // red
  PackedCol_Make(0x19, 0x19, 0x19, 0xFF), // black
};

static struct TerminalScreen {
  Screen_Body
  const struct TerminalBuffer *buffer;
  const PackedCol *palette;
  int width, height;
  int x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, x6, y6, scw, sch, sp;
} TerminalScreen_Instance;

#define TERM_BORDER_QUADS 8
#define TERM_CHAR_QUADS 2

#define VERTS_PER_QUAD 4
#define TERM_BORDER_VERTS (TERM_BORDER_QUADS * VERTS_PER_QUAD)
#define TERM_CHAR_VERTS (TERM_CHAR_QUADS * VERTS_PER_QUAD)

#define CalcMaxVertices(w, h) (TERM_BORDER_VERTS + (w) * (h) * TERM_CHAR_VERTS);

static void Init(void *screen) {
  struct TerminalScreen *s = (struct TerminalScreen *) screen;
  
  int width = s->buffer->width, height = s->buffer->height;
  s->width = width;
  s->height = height;
  
  s->maxVertices = CalcMaxVertices(width, height);
}

static void Free(void *screen) {}

static void ContextRecreated(void *screen) {
  struct TerminalScreen *s = (struct TerminalScreen *) screen;
  
  s->vb = Gfx_CreateDynamicVb(VERTEX_FORMAT_TEXTURED, s->maxVertices);
}

static void ContextLost(void *screen) {
  struct TerminalScreen *s = (struct TerminalScreen *) screen;
  
  Gfx_DeleteDynamicVb(&s->vb);
}

#define TERM_BORDER 12
#define TERM_PADDING 2
#define TOTAL_PADDING ((TERM_BORDER + TERM_PADDING) * 2)
#define SCALE_MARGIN 20

#define CHAR_WIDTH 6
#define CHAR_HEIGHT 9

// TODO allow custom font aspect ratios other than 6:9

static void Layout(void *screen) {
  struct TerminalScreen *s = (struct TerminalScreen *) screen;
  
  int win_width = Window_Main.Width, win_height = Window_Main.Height;
  int gui_width = s->width * CHAR_WIDTH + TOTAL_PADDING;
  int gui_height = s->height * CHAR_HEIGHT + TOTAL_PADDING;
  
  int scale_horz = win_width / (gui_width + SCALE_MARGIN);
  int scale_vert = win_height / (gui_height + SCALE_MARGIN);
  int scale = scale_horz < scale_vert ? scale_horz : scale_vert;
  scale = scale < 1 ? 1 : scale;
  
  int sw = gui_width * scale, sh = gui_height * scale;
  int sb = TERM_BORDER * scale, sp = TERM_PADDING * scale;
  int scw = CHAR_WIDTH * scale, sch = CHAR_HEIGHT * scale;
  int x1 = (win_width - sw) / 2, y1 = (win_height - sh) / 2;
  int x2 = x1 + sw, y2 = y1 + sh;
  int x3 = x1 + sb, y3 = y1 + sb;
  int x4 = x2 - sb, y4 = y2 - sb;
  int x5 = x3 + sp, y5 = y3 + sp;
  int x6 = x4 - sp, y6 = y4 - sp;
  
  s->x1 = x1; s->y1 = y1;
  s->x2 = x2; s->y2 = y2;
  s->x3 = x3; s->y3 = y3;
  s->x4 = x4; s->y4 = y4;
  s->x5 = x5; s->y5 = y5;
  s->x6 = x6; s->y6 = y6;
  s->scw = scw; s->sch = sch;
  s->sp = sp;
}

// Border UVs
#define buv0 0.0f
#define buv1 (12/64.0f)
#define buv2 (24/64.0f)
#define buv3 (36/64.0f)

#define white PACKEDCOL_WHITE

// Font UVs
#define bguv0 (250/256.0f)
#define bguv1 (252/256.0f)

#define QUAD(x1,y1,x2,y2,u1,v1,u2,v2,c) \
  v->x = (x1); v->y = (y1); v->z = 0; v->Col = (c); v->U = (u1); v->V = (v1); v++; \
  v->x = (x2); v->y = (y1); v->z = 0; v->Col = (c); v->U = (u2); v->V = (v1); v++; \
  v->x = (x2); v->y = (y2); v->z = 0; v->Col = (c); v->U = (u2); v->V = (v2); v++; \
  v->x = (x1); v->y = (y2); v->z = 0; v->Col = (c); v->U = (u1); v->V = (v2); v++;

// TODO: Cool idea, what if you didn't need to rebuild the mesh
// just the parts that changed during blit
// barring 1. ContextLost requires full rebuild
// 2. Layout requires full rebuild

// would it be efficient to keep a cached copy of the terminal and just offset it for resizing?
// like I could create an in-memory screen to make this more efficient?

static void BuildMesh(void *screen) {
  struct TerminalScreen *s = (struct TerminalScreen *) screen;
  
  int width = s->buffer->width, height = s->buffer->height;
  if (s->width != width || s->height != height) {
    s->width = width;
    s->height = height;
    
    s->maxVertices = CalcMaxVertices(width, height);
    
    Gfx_DeleteDynamicVb(&s->vb);
    s->vb = Gfx_CreateDynamicVb(VERTEX_FORMAT_TEXTURED, s->maxVertices);
    
    s->VTABLE->Layout(s);
  }
  
  int x1 = s->x1, y1 = s->y1;
  int x2 = s->x2, y2 = s->y2;
  int x3 = s->x3, y3 = s->y3;
  int x4 = s->x4, y4 = s->y4;
  int x5 = s->x5, y5 = s->y5;
  //int x6 = s->x6, y6 = s->y6;
  int scw = s->scw, sch = s->sch, sp = s->sp;
  
  struct VertexTextured *v = Gfx_LockDynamicVb(s->vb, VERTEX_FORMAT_TEXTURED, s->maxVertices);
  
  // Border
  QUAD(x1,y1,x3,y3,buv0,buv0,buv1,buv1,white) // top-left
  QUAD(x3,y1,x4,y3,buv1,buv0,buv2,buv1,white) // top
  QUAD(x4,y1,x2,y3,buv2,buv0,buv3,buv1,white) // top-right
  QUAD(x4,y3,x2,y4,buv2,buv1,buv3,buv2,white) // right 
  QUAD(x4,y4,x2,y2,buv2,buv2,buv3,buv3,white) // bottom-right
  QUAD(x3,y4,x4,y2,buv1,buv2,buv2,buv3,white) // bottom
  QUAD(x1,y4,x3,y2,buv0,buv2,buv1,buv3,white) // bottom-left
  QUAD(x1,y3,x3,y4,buv0,buv1,buv1,buv2,white) // left
  
  // TODO: implement background padding stretching better
  
  // Window
  cc_uint8 *data = s->buffer->data;
  const PackedCol *pal = s->palette;
  
  int cy = y5;
  for (int y = 0; y < height; y++) {
    int cx = x5;
    for (int x = 0; x < width; x++) {
      // Terminal Padding Background Stretch
      int _x1 = cx, _y1 = cy, _x2 = cx+scw, _y2 = cy+sch;
      if (x == 0) { _x1 -= sp; }
      if (y == 0) { _y1 -= sp; }
      if (x == width-1) { _x2 += sp; }
      if (y == height-1) { _y2 += sp; }
      
      int ch = (int) *data++;
      PackedCol forecol = pal[*data++];
      PackedCol backcol = pal[*data++];
      
      int chx = 1 + (ch & 0xF) * (CHAR_WIDTH + 2);
      int chy = 1 + (ch >> 4) * (CHAR_HEIGHT + 2);
      
      float u1 = chx / 256.0f;
      float v1 = chy / 256.0f;
      float u2 = u1 + (CHAR_WIDTH / 256.0f);
      float v2 = v1 + (CHAR_HEIGHT / 256.0f);
      
      QUAD(_x1,_y1,_x2,_y2,bguv0,bguv0,bguv1,bguv1,backcol) // char background
      QUAD(cx,cy,cx+scw,cy+sch,u1,v1,u2,v2,forecol) // char foreground
      
      cx += scw;
    }
    cy += sch;
  }
  
  Gfx_UnlockDynamicVb(s->vb);
}

static void Render(void *screen, float delta) {
  struct TerminalScreen *s = (struct TerminalScreen *) screen;
  
  Gfx_SetVertexFormat(VERTEX_FORMAT_TEXTURED);
  Gfx_BindDynamicVb(s->vb);
  
  Gfx_BindTexture(border_tex);
  Gfx_DrawVb_IndexedTris_Range(TERM_BORDER_VERTS, 0, DRAW_HINT_SPRITE);
  
  Gfx_BindTexture(font_tex);
  Gfx_DrawVb_IndexedTris_Range(s->maxVertices - TERM_BORDER_VERTS, TERM_BORDER_VERTS, DRAW_HINT_SPRITE);
}

// TODO check how the inventory GUIs handles/implements clicking off of the GUI
// and check with the computercraft mod if it actually implements that.

// all default user input events are absorbed
// key events are passthrough for testing purposes
static void Update(void *screen, float delta) {}
static int KeyDown(void *screen, int key, struct InputDevice *device) { return 0; }
static void KeyUp(void *screen, int key, struct InputDevice *device) {}
static int KeyPress(void *screen, char keyChar) { return 0; }
static int TextChanged(void *screen, const cc_string *str) { return 0; }
static int PointerDown(void *screen, int id, int x, int y) { return 1; } 
static void PointerUp(void *screen, int id, int x, int y) {}
static int PointerMove(void *screen, int id, int x, int y) { return 1; }
static int MouseScroll(void *screen, float delta) { return 1; }

static const struct ScreenVTABLE TerminalScreen_VTABLE = {
  Init, Update, Free,
  Render, BuildMesh,
  KeyDown, KeyUp, KeyPress, TextChanged,
  PointerDown, PointerUp, PointerMove, MouseScroll,
  Layout, ContextLost, ContextRecreated
};

#define GUI_PRIORITY_TERMINAL 24

void TerminalScreen_Show(const struct TerminalBuffer *buffer, const PackedCol *palette) {
  struct TerminalScreen *s = &TerminalScreen_Instance;
  s->VTABLE = &TerminalScreen_VTABLE;
  s->grabsInput = true;
  s->closable = true;
  s->buffer = buffer;
  s->palette = palette;
  
  Gui_Add((struct Screen *) s, GUI_PRIORITY_TERMINAL);
}

void TerminalScreen_Hide(void) {
  Gui_Remove((struct Screen *) &TerminalScreen_Instance);
}

void TerminalScreen_Redraw(void) {
  struct TerminalScreen *s = &TerminalScreen_Instance;
  s->dirty = true;
}

