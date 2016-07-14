#if defined(_MSC_VER) || (_M_IX86_FP>=1)
#define BT4B_XMMINTRIN
#include <xmmintrin.h>
#include <emmintrin.h>
#endif

#ifndef BYTE_T
#define BYTE_T
typedef unsigned char byte;
#endif

#ifndef SBYTE_T
#define SBYTE_T
typedef signed char sbyte;
#endif

#ifndef PDLIB_INT_BITS_T
#define PDLIB_INT_BITS_T
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned long long u64;
typedef signed long long s64;
#endif

#ifndef UINT_T
#define UINT_T
typedef unsigned int uint;
#endif

#if !defined(LITTLEENDIAN) && !defined(BIGENDIAN)
#if defined(X86) || defined(X86_64) || defined(ARM)
#define LITTLEENDIAN
#endif
#endif

#if defined(_MSC_VER) && defined(BTEIFGL_DLL)
#ifndef BTIC4B_API
#define BTIC4B_API __declspec(dllexport)
#endif
#endif

#if defined(_MSC_VER) && defined(BTIC4B_DLL)
#ifndef BTIC4B_API
#define BTIC4B_API __declspec(dllexport)
#endif
#endif

#ifndef BTIC4B_API
#define BTIC4B_API
#endif

#ifdef _MSC_VER
#define force_inline __forceinline
#define default_inline __inline
#endif

#ifdef __GNUC__
#define force_inline inline
#define default_inline inline
#endif

#ifndef force_inline
#define force_inline
#define default_inline
#endif

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#ifndef M_TAU
#define M_TAU 6.283185307179586476925286766559
#endif

#define BTIC4B_TWOCC(a, b)			((a)|((b)<<8))
#define BTIC4B_FOURCC(a, b, c, d)	((a)|((b)<<8)|((c)<<16)|((d)<<24))

#define BTIC4B_TCC_I0		BTIC4B_TWOCC('I', '0')
#define BTIC4B_TCC_I1		BTIC4B_TWOCC('I', '1')
#define BTIC4B_TCC_I2		BTIC4B_TWOCC('I', '2')
#define BTIC4B_TCC_I3		BTIC4B_TWOCC('I', '3')

#define BTIC4B_TCC_AX		BTIC4B_TWOCC('A', 'X')
#define BTIC4B_TCC_HX		BTIC4B_TWOCC('H', 'X')

#define BTIC4B_FCC_BT4B		BTIC4B_FOURCC('B', 'T', '4', 'B')

#define BTIC4B_CLRS_RGBA		0x00
#define BTIC4B_CLRS_BGRA		0x01
#define BTIC4B_CLRS_RGBX		0x02
#define BTIC4B_CLRS_BGRX		0x03
#define BTIC4B_CLRS_RGB			0x04
#define BTIC4B_CLRS_BGR			0x05

#define BTIC4B_CLRS_BC1			0x08
#define BTIC4B_CLRS_BC3			0x09
#define BTIC4B_CLRS_BC6			0x0A
#define BTIC4B_CLRS_BC7			0x0B
#define BTIC4B_CLRS_BC1MIP		0x0C
#define BTIC4B_CLRS_BC3MIP		0x0D
#define BTIC4B_CLRS_BC6MIP		0x0E
#define BTIC4B_CLRS_BC7MIP		0x0F

#define BTIC4B_CLRT_GDBDR		0
#define BTIC4B_CLRT_RCT			1
#define BTIC4B_CLRT_YCBCR		2

#ifdef _MSC_VER
#define BTIC4B_DBGTRAP		__debugbreak();
#else
#define BTIC4B_DBGTRAP		
#endif

#define BTIC4B_QFL_PFRAME		0x0100
#define BTIC4B_QFL_IFRAME		0x0200
#define BTIC4B_QFL_USEPRED		0x0400
#define BTIC4B_QFL_USEBFQ		0x0800

#define BTIC4B_ERRS_GENERIC		-1
#define BTIC4B_ERRS_BADFCC		-16
#define BTIC4B_ERRS_BADIBUFSZ	-17
#define BTIC4B_ERRS_NOIMAGE		-18

typedef struct {
byte tab[256];
byte idx[256];
byte rov;
byte rk;
int cnt;
int bits;
}BTIC4B_SmtfState;

typedef struct BTIC4B_Context_s BTIC4B_Context;

struct BTIC4B_Context_s {
byte *ct, *cs;
u32 bit_win;
int bit_pos;

int xs, ys;
int xsb, ysb, nblk, blksz;
int qfl;

byte *blks;		//current frame blocks
byte *lblks;	//last frame blocks
byte *pblk;		//pixel blocks (mipmap decode)

int cy, cu, cv, dy, du, dv;
int qfy, qfuv, qfdy, qfduv;
int qy, quv, qdy, qduv;

int qdy_flat;
int qdy_2x2x2;
int qdy_4x4x2;
int qdy_8x8x2;
int qdy_8x8x3;
int qduv_flat;
int qduv_2x2;
int qdce_sc;
int qdcea_sc;

byte rk_cy, rk_cuv;
byte rk_dy, rk_duv;
byte rk_cnt, rk_misc;
byte cmask;				//current mask
byte imask;				//ideal mask
byte pred;				//predictor
byte imgt;				//image type
byte clrt;				//colorspace transform
byte pred_l8p;			//predictor needs to deal with LDR8/Skip

BTIC4B_SmtfState sm_cmd;
BTIC4B_SmtfState sm_mask;

int stat_bits;
// int stat_tbits;
int stat_yuvbits;
int stat_yuvcnt;
int stat_yuvcbits[8];
int stat_pixbits;

int yuv_cz[8];

void (*DecUpdatePred)(BTIC4B_Context *ctx,
	byte *ct, byte *blks);
void (*DecGetBlkPredClrs3)(BTIC4B_Context *ctx,
	byte *blka, byte *blkb, byte *blkc, int *rcyuv);

// used during image transform
void (*DecBlock)(BTIC4B_Context *ctx,
	byte *blkbuf, byte *ibuf, int ystr);
void (*DecBlockHalf)(BTIC4B_Context *ctx,
	byte *blkbuf, byte *ibuf, int ystr);

u32 (*ClrDec1)(int cy, int cu, int cv);
u32 (*ClrDec1A)(int cy, int cu, int cv, int ca);
void (*ClrDec4)(
	int cy0, int cy1, int cy2, int cy3, int cu, int cv,
	u32 *rpx0, u32 *rpx1);
void (*ClrDec4B)(
	int cy0, int cy1, int cy2, int cy3,
	int cu0, int cv0, int cu1, int cv1,
	u32 *rpx0, u32 *rpx1);
void (*ClrDec4C)(
	int cy0, int cy1, int cy2, int cy3,
	int cu0, int cv0, int cu1, int cv1,
	int cu2, int cv2, int cu3, int cv3,
	u32 *rpx0, u32 *rpx1);
void (*ClrDec2T)(int tag,
	int cy, int cu, int cv,
	int dy, int du, int dv,
	int *rr0, int *rg0, int *rb0,
	int *rr1, int *rg1, int *rb1);

//block format query
short bfq_qdy[32];		//dy cutoff
short bfq_qduv[32];		//duv cutoff
byte bfq_rqfl[32];		//require flags
byte bfq_exfl[32];		//exclude flags
byte bfq_cost[32];		//block cost

//BCn transcode
void (*BCnEncodeBlockGen)(byte *block,
	s16 *pxy, s16 *pxa, int *min, int *max,
	int mcy, int ncy, int mca, int nca);
void (*BCnEncodeBlockBits32)(byte *block,
	u32 pxy, u32 pxa, int *min, int *max);
void (*BCnEncodeBlockBits48)(byte *block,
	u64 pxy, u64 pxa, int *min, int *max);
void (*BCnEncodeBlockBits64)(byte *block,
	u64 pxy, int *min, int *max);
void (*BCnEncodeBlockFlat)(byte *block, int *avg);

};

BTIC4B_API int BTIC4B_DecodeImgBufferCtx(BTIC4B_Context *ctx,
	byte *cbuf, int cbsz, byte *ibuf,
	int xs, int ys, int clrfl);
BTIC4B_API byte *BTIC4B_BufBmpGetImg(byte *buf,
	int *rxs, int *rys,
	u32 *rfcc, int *risz);
BTIC4B_API int BTIC4B_DecodeImgBmpBufferCtx(BTIC4B_Context *ctx,
	byte *cbuf, int cbsz, byte *ibuf,
	int *rxs, int *rys, int clrfl);
BTIC4B_API int BTIC4B_DecodeImgBmpBuffer(byte *cbuf, int cbsz,
	byte *ibuf, int *rxs, int *rys, int clrfl);

BTIC4B_API int BTIC4B_EncodeImgBufferCtx(BTIC4B_Context *ctx,
	byte *obuf, int cbsz, byte *ibuf,
	int xs, int ys, int qfl, int clrs);
BTIC4B_API int BTIC4B_DumpStatsCtx(BTIC4B_Context *ctx);
BTIC4B_API int BTIC4B_EncFlattenBmp(byte *obuf,
	int xs, int ys, int fcc,
	byte *ibuf, int isz);
BTIC4B_API int BTIC4B_EncodeImgBmpBufferCtx(BTIC4B_Context *ctx,
	byte *obuf, int cbsz, byte *ibuf,
	int xs, int ys, int qfl, int clrs);
BTIC4B_API BTIC4B_Context *BTIC4B_AllocContext(void);
BTIC4B_API void BTIC4B_FreeContext(BTIC4B_Context *ctx);
BTIC4B_API int BTIC4B_EncodeImgBmpBuffer(
	byte *obuf, int cbsz, byte *ibuf,
	int xs, int ys, int qfl, int clrs);

