#define BTLZA_BITARITH_LOWER		0x00000000
#define BTLZA_BITARITH_UPPER		0xFFFFFFFF
// #define BTLZA_BITARITH_WBITS		10
#define BTLZA_BITARITH_WBITS		8
#define BTLZA_BITARITH_WMASK		((1<<BTLZA_BITARITH_WBITS)-1)
#define BTLZA_BITARITH_WASHR		5
// #define BTLZA_BITARITH_WASHR		4
// #define BTLZA_BITARITH_WASHR		6

#define BTLZA_BITARITH_DATABITS		9	//W: arithmetic coder bits/symbol
#define BTLZA_BITARITH_CTXBITS		13
#define BTLZA_BITARITH_CTXMASK		((1<<BTLZA_BITARITH_CTXBITS)-1)

#define BTLZA_ENC_CHUNK	262144

// #define BTLZA_ENCF_CHUNK1	32768		//first chunk (fast encoder)
// #define BTLZA_ENCF_CHUNK2	131072		//other chunks (fast encoder)
#define BTLZA_ENCF_CHUNK1	16384		//first chunk (fast encoder)
#define BTLZA_ENCF_CHUNK2	65536		//other chunks (fast encoder)

#define BTLZA_ENCFL_ARITH		(BGBBTJ_ZFL_ARITH<<8)

typedef byte btlza_prob;

typedef struct BGBBTJ_BTLZA_Context_s BGBBTJ_BTLZA_Context;

struct BGBBTJ_BTLZA_VtBitIO_s
{
int (*BS_ReadByte)(BGBBTJ_BTLZA_Context *ctx);
int (*BS_WriteByte)(BGBBTJ_BTLZA_Context *ctx, int val);
};

struct BGBBTJ_BTLZA_Context_s
{
BGBBTJ_BTLZA_Context *next;
byte *ibuf;		//input buffer
byte *obuf;		//output buffer
byte *cs;		//current pos in bitstream (input)
byte *ct;		//current pos in bitstream (output)
byte *cse;		//current end pos in bitstream (input)
byte *cte;		//current end pos in bitstream (output)

u32 rmin;		//window lower range
u32 rmax;		//window upper range
u32 rval;		//window decode value
// u32 range;		//window decode range

//Arithmetic State
int wctx;
btlza_prob *mdl_lit;		//literal model
btlza_prob *mdl_dist;		//distances model
btlza_prob *mdl_xbits;	//extra bits model
btlza_prob *mdl_rbits;	//raw bits model

int ctxbits;	//context bits
int ctxmask;	//context mask

int ctxbits_lit;	//context bits (extra bits)
int ctxmask_lit;	//context mask (extra bits)

int ctxbits_dist;	//context bits (extra bits)
int ctxmask_dist;	//context mask (extra bits)

int ctxbits_xbits;	//context bits (extra bits)
int ctxmask_xbits;	//context mask (extra bits)
int ctxcnt_xbits;	//context mask (extra bits)

int ctxbits_raw;	//context bits (raw bits)
int ctxmask_raw;	//context mask (raw bits)

//Bitstream
u32 bs_win;		//bit window
int bs_pos;		//bit window position
int bs_flags;	//decoder flags

//Bitstream Functions
int (*BS_ReadByte)(BGBBTJ_BTLZA_Context *ctx);
int (*BS_DecodeSymbol)(BGBBTJ_BTLZA_Context *ctx);
int (*BS_DecodeDistanceSymbol)(BGBBTJ_BTLZA_Context *ctx);
int (*BS_DecodeClSymbol)(BGBBTJ_BTLZA_Context *ctx);
int (*BS_ReadExtraNBits)(BGBBTJ_BTLZA_Context *ctx, int n);
int (*BS_ReadNBits)(BGBBTJ_BTLZA_Context *ctx, int n);
void (*BS_SkipNBits)(BGBBTJ_BTLZA_Context *ctx, int n);
int (*BS_ReadBit)(BGBBTJ_BTLZA_Context *ctx);

void (*BS_WriteByte)(BGBBTJ_BTLZA_Context *ctx, int val);
void (*BS_EncodeSymbol)(BGBBTJ_BTLZA_Context *ctx, int sym);
void (*BS_EncodeDistanceSymbol)(BGBBTJ_BTLZA_Context *ctx, int sym);
void (*BS_EncodeClSymbol)(BGBBTJ_BTLZA_Context *ctx, int sym);
int (*BS_WriteExtraNBits)(BGBBTJ_BTLZA_Context *ctx, int v, int n);
int (*BS_WriteNBits)(BGBBTJ_BTLZA_Context *ctx, int v, int n);
int (*BS_WriteBit)(BGBBTJ_BTLZA_Context *ctx, int v);

int *lbase, *lextra;

u32 bs_ltab_idx[256];	//literal index table
u32 bs_dtab_idx[256];	//distance index table

u16 bs_ltab_code[384];	//literal codes
u16 bs_ltab_mask[384];	//literal code masks
u16 bs_ltab_next[384];	//literal table code chains
byte bs_ltab_len[384];	//literal code lengths

u16 bs_dtab_code[256];	//distance codes
u16 bs_dtab_mask[256];	//distance code masks
u16 bs_dtab_next[256];	//distance table code chains
byte bs_dtab_len[256];	//distance code lengths

#if 1
u32 bs_rtab_idx[256];	//literal index table
u32 bs_stab_idx[256];	//distance index table

u16 bs_rtab_code[256];	//distance codes
u16 bs_rtab_mask[256];	//distance code masks
u16 bs_rtab_next[256];	//distance table code chains
byte bs_rtab_len[256];	//distance code lengths

u16 bs_stab_code[256];	//distance codes
u16 bs_stab_mask[256];	//distance code masks
u16 bs_stab_next[256];	//distance table code chains
byte bs_stab_len[256];	//distance code lengths
#endif

// u32 bs_ltab_pidx[4096];	//literal pair index table

int bs_ltab_hint_next;

//Ring Huff

byte *bs_ltab_rhuff;	//literal table ring
byte *bs_dtab_rhuff;	//distance table ring

byte bs_rhtab_sz;		//size of allocated ringhuff
byte bs_rhtab_n;		//number of ringhuff tables
byte bs_rhtab_lrov;		//ringhuff literal rover
byte bs_rhtab_drov;		//ringhuff distance rover

u32 *bs_ltab_idx2[8];	//literal index table
u32 *bs_dtab_idx2[8];	//distance index table

u16 *bs_ltab_code2[8];	//literal codes
u16 *bs_ltab_mask2[8];	//literal code masks
u16 *bs_ltab_next2[8];	//literal table code chains
byte *bs_ltab_len2[8];	//literal code lengths

u16 *bs_dtab_code2[8];	//distance codes
u16 *bs_dtab_mask2[8];	//distance code masks
u16 *bs_dtab_next2[8];	//distance table code chains
byte *bs_dtab_len2[8];	//distance code lengths

//LZ77 state
byte *lz_wbuf;		//window buffer
int *lz_lbuf;		//link buffer
int *lz_hash;		//hash table
int lz_wpos;		//window position
int lz_wsize;		//window size
int lz_wmask;		//window mask

int lz_sdepth;	//search depth
int lz_maxdist;	//max distance
int lz_maxlen;	//max match length

byte *lz_tbuf;
byte *lz_mbuf;
int lz_tsz;
int lz_msz;

int lz_lastdist;	//last distance seen
int lz_lastrun;		//last run length seen

//LZ77 Fast State
byte **lzf_hash;
int lzf_lstat[512];
int lzf_dstat[256];
int lzf_hstat[24];

};
