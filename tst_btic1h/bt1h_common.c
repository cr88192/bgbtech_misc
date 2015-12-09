/*
Copyright (C) 2015 by Brendan G Bohannon

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#define BT1H_USEHV			//use half-blocks
// #define BT1H_USEGRAD		//use gradient blocks
#define BT1H_USETSKIP		//use translation-skip
#define BT1H_TSKIP_MAX 4	//translation-skip max value

// #define BT1H_CHEAPYUV		//use cheaper YUV

#define BT1H_ENABLE_AX		//enable alpha extension

// #define BT1H_DEBUG_TRAPRANGE

typedef unsigned char byte;
typedef signed char sbyte;

typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;

typedef unsigned long long u64;
typedef signed long long s64;

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


#define RIFF_MAKETAG(a, b, c, d)	((a)+((b)<<8)+((c)<<16)+((d)<<24))
#define RIFF_UNHEX_NIBBLE(a)		( \
	(((a)>='0')&&((a)<='9'))?((a)-'0') : \
	(((a)>='A')&&((a)<='F'))?((a)-'A'+10) : \
	(((a)>='a')&&((a)<='f'))?((a)-'a'+10) : 0)
#define RIFF_HEX_NIBBLE(a)		(((a)<10)?((a)+'0'):((a)+'A'))

#define RIFF_MAKEHEXTAG(a)	RIFF_MAKETAG( \
					RIFF_HEX_NIBBLE(((a)>>12)&0xf), \
					RIFF_HEX_NIBBLE(((a)>>8)&0xf), \
					RIFF_HEX_NIBBLE(((a)>>4)&0xf), \
					RIFF_HEX_NIBBLE((a)&0xf) \
					)

#define BTIC1H_FCC_RGBA	RIFF_MAKETAG('R','G','B','A')
#define BTIC1H_FCC_RGB	RIFF_MAKETAG('R','G','B',' ')
#define BTIC1H_FCC_BGRA	RIFF_MAKETAG('B','G','R','A')
#define BTIC1H_FCC_BGR	RIFF_MAKETAG('B','G','R',' ')
#define BTIC1H_FCC_YUVA	RIFF_MAKETAG('Y','U','V','A')
#define BTIC1H_FCC_YUV	RIFF_MAKETAG('Y','U','V',' ')

#define BTIC1H_FCC_RGBX	RIFF_MAKETAG('R','G','B','X')
#define BTIC1H_FCC_BGRX	RIFF_MAKETAG('B','G','R','X')
#define BTIC1H_FCC_YUVX	RIFF_MAKETAG('Y','U','V','X')

#define BTIC1H_FCC_BTIC	RIFF_MAKETAG('B','T','I','C')
#define BTIC1H_FCC_bt1h	RIFF_MAKETAG('b','t','1','h')
#define BTIC1H_FCC_YUY2	RIFF_MAKETAG('Y','U','Y','2')
#define BTIC1H_FCC_UYVY	RIFF_MAKETAG('U','Y','V','Y')

#define	BTIC1H_PXF_RGBA			 0	//RGBA(32)
#define	BTIC1H_PXF_RGB			 1	//RGB(24)
#define	BTIC1H_PXF_BGRA			 3	//BGRA(32)
#define	BTIC1H_PXF_BGR			 4	//BGR(24)
#define	BTIC1H_PXF_YUVA			 5	//YUVA(32)
#define	BTIC1H_PXF_YUV			 6	//YUV(24)
#define	BTIC1H_PXF_Y			 7	//Y(8)
#define	BTIC1H_PXF_YA			 8	//YA(16)
#define	BTIC1H_PXF_YUV422		 9	//YYUV
#define	BTIC1H_PXF_YUV420		10	//YYU/YYV (YYYYUV)

#define	BTIC1H_PXF_YYYA			11
#define	BTIC1H_PXF_ARGB			12
#define	BTIC1H_PXF_ABGR			13
#define	BTIC1H_PXF_UYVY			14	//UYVY Ordering (4:2:2)
#define	BTIC1H_PXF_YUYV			15	//YUYV Ordering (4:2:2)

#define	BTIC1H_PXF_BC1			16	//BC1 / DXT1
#define	BTIC1H_PXF_BC2			17	//BC2 / DXT3
#define	BTIC1H_PXF_BC3			18	//BC3 / DXT5
#define	BTIC1H_PXF_BC4			19	//BC4
#define	BTIC1H_PXF_BC5			20	//BC5
#define	BTIC1H_PXF_BC6			21	//BC6H (Signed)
#define	BTIC1H_PXF_BC7			22	//BC7
#define	BTIC1H_PXF_BC1F			23	//BC1 / DXT1
#define	BTIC1H_PXF_BC3F			24	//BC3 / DXT5
#define	BTIC1H_PXF_BC1A			25	//BC1 / DXT1
#define	BTIC1H_PXF_BC3_UVAY		26	//DXT5-UVAY
#define	BTIC1H_PXF_BC7_SRGB		27	//BC7 (SRGBA)
#define	BTIC1H_PXF_BC6_UF16		28	//BC6H (Unsigned)
#define	BTIC1H_PXF_BC4A			29	//BC4 (Alpha Only)

#define	BTIC1H_PXF_RGBA_Q11_4	32	//RGBA(64 bit, fixed Q11.4)
#define	BTIC1H_PXF_RGB_Q11_4	33	//RGB(48 bit, fixed Q11.4)
#define	BTIC1H_PXF_RGBA_F32		34	//RGBA(128 bit, float32)
#define	BTIC1H_PXF_RGB_F32		35	//RGB(96 bit, float32)
#define	BTIC1H_PXF_RGBA_F16		36	//RGBA(64 bit, float16)
#define	BTIC1H_PXF_RGB_F16		37	//RGB(48 bit, float16)
#define	BTIC1H_PXF_RGBA_LS16	38	//RGBA(64 bit, log-scale)
#define	BTIC1H_PXF_RGB_LS16		39	//RGB(48 bit, log-scale)

#define	BTIC1H_PXF_RGBA_VF		40	//RGBA (Vertical Flip)
#define	BTIC1H_PXF_RGB_VF		41	//RGB (Vertical Flip)
#define	BTIC1H_PXF_RGBA_F16_VF	42	//RGBA(64 bit, float16)
#define	BTIC1H_PXF_RGB_F16_VF	43	//RGB(48 bit, float16)

#define	BTIC1H_PXF_RGBX			44	//RGBx(32)
#define	BTIC1H_PXF_BGRX			45	//BGRx(32)
#define	BTIC1H_PXF_RGBX_VF		46	//RGBx(32)
#define	BTIC1H_PXF_BGRX_VF		47	//BGRx(32)

#define	BTIC1H_PXF_BC1_VF		48	//BC1 / DXT1 (Vertical Flip)
#define	BTIC1H_PXF_BC2_VF		49	//BC2 / DXT3 (Vertical Flip)
#define	BTIC1H_PXF_BC3_VF		50	//BC3 / DXT5 (Vertical Flip)
#define	BTIC1H_PXF_BC4_VF		51	//BC4 (Vertical Flip)
#define	BTIC1H_PXF_BC5_VF		52	//BC5 (Vertical Flip)
#define	BTIC1H_PXF_BC6_VF		53	//BC6H (Signed) (Vertical Flip)
#define	BTIC1H_PXF_BC7_VF		54	//BC7 (Vertical Flip)
#define	BTIC1H_PXF_BC6_VF2		55	//BC6H (Signed) (Vertical Flip)
#define	BTIC1H_PXF_BC7_VF2		56	//BC7 (Vertical Flip)

#define	BTIC1H_PXF_MB5C			57	//MB5C
#define	BTIC1H_PXF_MB5C_VF		58	//BC7 (Vertical Flip)

#define	BTIC1H_PXF_BC6_UVF		60	//BC6H (Unsigned)

#define	BTIC1H_PXF_RGBX_F16		62	//RGBx(32)
#define	BTIC1H_PXF_BGRX_F16		63	//BGRx(32)

#define	BTIC1H_QFL_IFRAME			1048576
#define	BTIC1H_QFL_PFRAME			2097152

#ifndef BTIC1H_API
#define BTIC1H_API __declspec(dllexport)
#endif

typedef struct BTIC1H_Context_s BTIC1H_Context;

struct BTIC1H_Context_s {
u32 mark1;

byte *bs_ct;						//bitstream output
byte *bs_cte;						//bitstream output end

byte *bs_cs;						//bitstream input
byte *bs_cse;						//bitstream input end

u32 bs_win;							//bitstream window
int bs_pos;							//bitstream window position
int bs_bits;						//bitstream total bits

byte *blks;							//blocks buffer
byte *lblks;						//last-frame blocks buffer

byte cmdwin[16];					//command window
short cmdidx[256];					//command window index
byte cmdwpos;						//command window position

int xs, ys, clrs;					//resolution and colorspace
int xbsz, ybsz;						//image size in blocks
int nblks;							//image blocks count

int cy, cu, cv, cd;					//Current YUVD
int qfy, qfuv, qfd;					//Aurrent Quantization Factors
int absyuvbias;						//Absolute YUV Bias

int cdy, cdu, cdv;					//Dyuv
int qfdy, qfduv;					//Dyuf Quantization Factors

int qfay, qfauv, qfad;				//AbsYUVD Quantization Factors
int qfady, qfaduv;					//AbsDyuv Quantization Factors

byte flip;							//Vertical Flip
byte dyuv;

u32 mark2;

int fx_qfy, fx_qfuv, fx_qfd;		//Fixed Point Quant Recip (YUVD)
int fx_qfay, fx_qfauv, fx_qfad;		//Fixed Point Quant Recip (AbsYUVD)
int fx_qfdy, fx_qfduv;				//Fixed Point Quant Recip (Dyuv)
int fx_qfady, fx_qfaduv;			//Fixed Point Quant Recip (AbsDyuv)

int rk_cmdidx;						//
int rk_cmdabs;						//
int rk_cmdcnt;						//

int rk_parmxy;						//
int rk_parmvar;						//
int rk_parmval;						//
int rk_parmix;						//

int rk_dy;							//
int rk_duv;							//
int rk_ddy;							//
int rk_dduv;						//

int rk_ay;							//
int rk_auv;							//
int rk_ady;							//
int rk_aduv;						//

int rk_qfy;							//
int rk_qfuv;						//
int rk_qfdy;						//
int rk_qfduv;						//

int cnt_cmds;						//
int bits_cmdidx;					//
int bits_cmdabs;					//

int bits_dyuv;						//
int bits_dy;						//
int bits_duv;						//
int bits_ddy;						//
int bits_dduv;						//

int bits_pix4x4;					//
int bits_pix4x4x1;					//
int bits_pix4x4x3;					//
int bits_pix4x2;					//
int bits_pix2x2;					//
int bits_pix2x1;					//

u32 mark3;
};

typedef struct BTIC1H_BMPInfoHeader_s BTIC1H_BMPInfoHeader;
typedef struct BTIC1H_VidCodec_s BTIC1H_VidCodec;
typedef struct BTIC1H_VidCodecCTX_s BTIC1H_VidCodecCTX;

struct BTIC1H_BMPInfoHeader_s {
unsigned int biSize;
unsigned int biWidth;
unsigned int biHeight;
unsigned short biPlanes;
unsigned short biBitCount;
unsigned int biCompression;
unsigned int biSizeImage;
unsigned int biXPelsPerMeter;
unsigned int biYPelsPerMeter;
unsigned int biClrUsed;
unsigned int biClrImportant;
};

struct BTIC1H_VidCodec_s {
BTIC1H_VidCodec *next;
int *fcc;	//list of handlers, NULL -> try any

BTIC1H_VidCodecCTX *(*begin_decompress)(int fcc,
	BTIC1H_BMPInfoHeader *in, BTIC1H_BMPInfoHeader *out);
BTIC1H_VidCodecCTX *(*begin_compress)(int fcc,
	BTIC1H_BMPInfoHeader *in, BTIC1H_BMPInfoHeader *out);
int (*decompress_query)(int fcc,
	BTIC1H_BMPInfoHeader *in, BTIC1H_BMPInfoHeader *out);
int (*compress_query)(int fcc,
	BTIC1H_BMPInfoHeader *in, BTIC1H_BMPInfoHeader *out);
};

struct BTIC1H_VidCodecCTX_s {
void *data;

byte *vidStrd;
byte *audStrd;
int sz_vidStrd;
int sz_audStrd;
int viQuality;
int viNextIFrame;
int viFlags;

int (*decompress_frame)(BTIC1H_VidCodecCTX *ctx,
	void *src, void *dst, int ssz, int dsz);
int (*compress_frame)(BTIC1H_VidCodecCTX *ctx,
	void *src, void *dst, int dsz, int qfl, int clrs, int *rfl);
int (*end_decompress)(BTIC1H_VidCodecCTX *ctx);
int (*end_compress)(BTIC1H_VidCodecCTX *ctx);
int (*decompress_frame_clrs)(BTIC1H_VidCodecCTX *ctx,
	void *src, void *dst, int ssz, int dsz, int clrs);
};

void btjpg_puts(char *str);
