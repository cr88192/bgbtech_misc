#define GFXEDIT_TOOL_BOXSEL		0x00
#define GFXEDIT_TOOL_PASTESEL	0x01
#define GFXEDIT_TOOL_PASTEREP	0x02

#define GFXEDIT_TOOL_BRUSH		0x04
#define GFXEDIT_TOOL_ERASE		0x05

#define GFXEDIT_TOOL_PENCIL		0x08
#define GFXEDIT_TOOL_PICK		0x09

#define GFXEDIT_TOOL_BUCKET		0x0C
#define GFXEDIT_TOOL_LINE		0x0D
#define GFXEDIT_TOOL_BOX		0x0E

#define GFXEDIT_TOOL_MAX		0x10

#define GFXEDIT_CLRBOX_MIN4		0x40
#define GFXEDIT_CLRBOX_MAX4		0x50

#define GFXEDIT_CLRBOX_MIN5		0x40
#define GFXEDIT_CLRBOX_MAX5		0x60

typedef struct GfxEdit_Context_s GfxEdit_Context;
typedef struct GfxEdit_ConCmd_s GfxEdit_ConCmd;

typedef struct GfxEdit_UndoLevel_s GfxEdit_UndoLevel;
typedef struct GfxEdit_DrawLayer_s GfxEdit_DrawLayer;

struct GfxEdit_Context_s {
u16 *screen;	//320x200 window

int canvas_width;
int canvas_height;
byte *canvas_pixels;	//8/16/32
u32 *canvas_pal4;		//1/2/4/8
u16 *canvas_pal2;		//1/2/4/8
byte canvas_bpp;

char *palname;
char *imgname;
char *cwd;

int canvas_xorg;
int canvas_yorg;
int time_ms;
int con_time_ms;

byte sel_tool;
byte sel_color1;
byte sel_color2;
byte clr_trans;
char zoom;

s16 line_x0, line_y0;
s16 line_x1, line_y1;
s16 paste_x0, paste_y0;
s16 paste_x1, paste_y1;
s16 paste_xr, paste_yr;
s16 paste_xd, paste_yd;

byte redraw_tools_dirty;
byte redraw_img_dirty;
byte redraw_con_dirty;

byte keydown_shift;
byte keydown_ctrl;
byte undo_inhibit;

byte condown;
byte con_cur_xbase, con_cur_pos;
byte con_prn_pos;

byte *conbuf;
byte conline[256];

GfxEdit_UndoLevel *undo_free;
GfxEdit_UndoLevel *undo;
GfxEdit_UndoLevel *clip;
};

struct GfxEdit_ConCmd_s {
GfxEdit_ConCmd *next;
char *name;
char *hint;
void (*Func)(GfxEdit_Context *ctx, char **args);
};

struct GfxEdit_UndoLevel_s {
GfxEdit_UndoLevel *undo;
GfxEdit_UndoLevel *redo;
int x0, y0;
int x1, y1;
int xs, ys;
byte *pixels_old;
byte *pixels_new;
// u32 pal4_old[256];
// u32 pal4_new[256];

u32 *pal4_old;
u32 *pal4_new;
byte pal4_bpp_old;
byte pal4_bpp_new;

int pix_old;
int pix_new;
int flags;
};

struct GfxEdit_DrawLayer_s {
GfxEdit_DrawLayer *next;
};

void GfxEdit_SetupPalFinish(GfxEdit_Context *ctx);
void GfxEdit_SetupPalRGBI(GfxEdit_Context *ctx);
void GfxEdit_SetupPalRGBIx2(GfxEdit_Context *ctx);
void GfxEdit_SetupPalWeb216(GfxEdit_Context *ctx);
void GfxEdit_SetupPalMono(GfxEdit_Context *ctx);
void GfxEdit_SetupPalCga2b(GfxEdit_Context *ctx, int psub);

u16 GfxEdit_RGB24to555(u32 rgb);
u16 GfxEdit_RGBA32to555(u32 rgb);
u32 GfxEdit_RGB555to32(u16 px);
u16 GfxEdit_Avg4xRGB555(u16 c0, u16 c1, u16 c2, u16 c3);
byte GfxEdit_RGBA32toLuma(u32 rgb);

int gfxedit_log2up(int val);
int gfxedit_isqrtapx(int val);

void GfxEdit_ConPuts(GfxEdit_Context *ctx, char *str);
void GfxEdit_ConPrintf(GfxEdit_Context *ctx, char *str, ...);
void GfxEdit_ConRunCmd(GfxEdit_Context *ctx, char *str);
