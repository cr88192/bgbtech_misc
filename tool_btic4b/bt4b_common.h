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

#define BTIC4B_CLRS_RGBA		0
#define BTIC4B_CLRS_BGRA		1
#define BTIC4B_CLRS_RGBX		2
#define BTIC4B_CLRS_BGRX		3

#ifdef _MSC_VER
#define BTIC4B_DBGTRAP		__debugbreak();
#else
#define BTIC4B_DBGTRAP		
#endif

#define BTIC4B_QFL_PFRAME		0x0100

#define BTIC4B_ERRS_GENERIC	-1
#define BTIC4B_ERRS_BADFCC	-16
#define BTIC4B_ERRS_BADIBUFSZ	-17
#define BTIC4B_ERRS_NOIMAGE	-18

typedef struct {
byte tab[256];
byte idx[256];
byte rov;
byte rk;
int cnt;
int bits;
}BTIC4B_SmtfState;

typedef struct {
byte *ct, *cs;
u32 bit_win;
int bit_pos;

int xs, ys;
int xsb, ysb, nblk, blksz;
int qfl;

byte *blks;
byte *lblks;

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

BTIC4B_SmtfState sm_cmd;
BTIC4B_SmtfState sm_mask;

int stat_bits;
// int stat_tbits;
int stat_yuvbits;
int stat_yuvcnt;
int stat_yuvcbits[8];
int stat_pixbits;

int yuv_cz[8];

}BTIC4B_Context;

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
