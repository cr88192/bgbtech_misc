/*
 * OpenEXR (.exr) image decoder
 * Copyright (c)Jimmy Christensen
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
	
/**
 * @file
 * OpenEXR decoder
 * @author Jimmy Christensen
 *
 * For more information on the OpenEXR format, visit:
 *  http://openexr.com/
 *
 * exr_flt2uint() and exr_halflt2uint() is credited to Reimar Döffinger.
 * exr_half2float() is credited to Aaftab Munshi, Dan Ginsburg, Dave Shreiner.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <stdint.h>
#include <stdarg.h>


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

#include "bt1h_targa.c"

// #include <float.h>
// #include <zlib.h>

// #include "libavutil/imgutils.h"
// #include "libavutil/intfloat.h"
// #include "libavutil/opt.h"

// #include "avcodec.h"
// #include "bytestream.h"
// #include "get_bits.h"
// #include "internal.h"
// #include "mathops.h"
// #include "thread.h"

#define FFMIN(x, y) (((x)<(y))?(x):(y))
#define FFMAX(x, y) (((x)>(y))?(x):(y))

#define FFMIN3(x, y, z) FFMIN(x, FFMIN(y, z))

#define AV_RL64(p) 	(*(uint64_t *)(p))
#define AV_RL32(p) 	(*(uint32_t *)(p))
#define AV_RL16(p) 	(*(uint16_t *)(p))
#define AV_RB8(p) 	(*(uint8_t *)(p))
#define AV_RL24(p) 	((*(uint32_t *)(p))&0xFFFFFF)

typedef struct GetByteContext {
	const uint8_t *buffer, *buffer_end, *buffer_start;
} GetByteContext;


#define DEF(type, name, bytes, read, write)									\
static type bytestream_get_ ## name(const uint8_t **b)						\
{																			\
	(*b) += bytes;															\
	return read(*b - bytes);												\
}																			\
static type bytestream2_get_ ## name ## u(GetByteContext *g) 				\
{																			\
	return bytestream_get_ ## name(&g->buffer);							\
}																			\
static type bytestream2_get_ ## name(GetByteContext *g)					\
{																			\
	if (g->buffer_end - g->buffer < bytes) {								\
		g->buffer = g->buffer_end;											\
		return 0;															\
	}																		\
	return bytestream2_get_ ## name ## u(g);								\
}																			\
static type bytestream2_peek_ ## name(GetByteContext *g)					\
{																			\
	if (g->buffer_end - g->buffer < bytes)									\
		return 0;															\
	return read(g->buffer);												\
}

#if 1
DEF(uint64_t,	 le64, 8, AV_RL64, AV_WL64)
DEF(unsigned int, le32, 4, AV_RL32, AV_WL32)
DEF(unsigned int, le24, 3, AV_RL24, AV_WL24)
DEF(unsigned int, le16, 2, AV_RL16, AV_WL16)
DEF(uint64_t,	 be64, 8, AV_RB64, AV_WB64)
DEF(unsigned int, be32, 4, AV_RB32, AV_WB32)
DEF(unsigned int, be24, 3, AV_RB24, AV_WB24)
DEF(unsigned int, be16, 2, AV_RB16, AV_WB16)
DEF(unsigned int, byte, 1, AV_RB8 , AV_WB8)
#endif

#	define bytestream2_get_ne16  bytestream2_get_le16
#	define bytestream2_get_ne24  bytestream2_get_le24
#	define bytestream2_get_ne32  bytestream2_get_le32
#	define bytestream2_get_ne64  bytestream2_get_le64
#	define bytestream2_get_ne16u bytestream2_get_le16u
#	define bytestream2_get_ne24u bytestream2_get_le24u
#	define bytestream2_get_ne32u bytestream2_get_le32u
#	define bytestream2_get_ne64u bytestream2_get_le64u
#	define bytestream2_put_ne16  bytestream2_put_le16
#	define bytestream2_put_ne24  bytestream2_put_le24
#	define bytestream2_put_ne32  bytestream2_put_le32
#	define bytestream2_put_ne64  bytestream2_put_le64
#	define bytestream2_peek_ne16 bytestream2_peek_le16
#	define bytestream2_peek_ne24 bytestream2_peek_le24
#	define bytestream2_peek_ne32 bytestream2_peek_le32
#	define bytestream2_peek_ne64 bytestream2_peek_le64

#define av_always_inline

static av_always_inline void bytestream2_init(
	GetByteContext *g, const uint8_t *buf, int buf_size)
{
//	av_assert0(buf_size >= 0);
	g->buffer		= buf;
	g->buffer_start = buf;
	g->buffer_end	= buf + buf_size;
}

static av_always_inline unsigned int bytestream2_get_bytes_left(
	GetByteContext *g)
{
	return g->buffer_end - g->buffer;
}


static av_always_inline void bytestream2_skip(
	GetByteContext *g, unsigned int size)
{
	g->buffer += FFMIN(g->buffer_end - g->buffer, size);
}

static av_always_inline void bytestream2_skipu(
	GetByteContext *g, unsigned int size)
{
	g->buffer += size;
}

static av_always_inline int bytestream2_tell(GetByteContext *g)
{
	return (int)(g->buffer - g->buffer_start);
}

static av_always_inline int bytestream2_size(GetByteContext *g)
{
	return (int)(g->buffer_end - g->buffer_start);
}

#define AVERROR(e) e

#ifndef EINVAL
#define EINVAL -1
#endif

#ifndef ENOMEM
#define ENOMEM -2
#endif

static av_always_inline int bytestream2_seek(
	GetByteContext *g, int offset, int whence)
{
	switch (whence) {
	case SEEK_CUR:
		offset	 = av_clip(offset, -(g->buffer - g->buffer_start),
							 g->buffer_end - g->buffer);
		g->buffer += offset;
		break;
	case SEEK_END:
		offset	= av_clip(offset, -(g->buffer_end - g->buffer_start), 0);
		g->buffer = g->buffer_end + offset;
		break;
	case SEEK_SET:
		offset	= av_clip(offset, 0, g->buffer_end - g->buffer_start);
		g->buffer = g->buffer_start + offset;
		break;
	default:
		return AVERROR(EINVAL);
	}
	return bytestream2_tell(g);
}


static av_always_inline unsigned int bytestream2_get_buffer(
	GetByteContext *g, uint8_t *dst, unsigned int size)
{
	int size2 = FFMIN(g->buffer_end - g->buffer, size);
	memcpy(dst, g->buffer, size2);
	g->buffer += size2;
	return size2;
}

static av_always_inline unsigned int bytestream2_get_bufferu(
	GetByteContext *g, uint8_t *dst, unsigned int size)
{
	memcpy(dst, g->buffer, size);
	g->buffer += size;
	return size;
}

static av_always_inline unsigned int bytestream_get_buffer(
	const uint8_t **b, uint8_t *dst, unsigned int size)
{
	memcpy(dst, *b, size);
	(*b) += size;
	return size;
}


typedef struct GetBitContext {
	const uint8_t *buffer, *buffer_end;
	int index;
	int size_in_bits;
	int size_in_bits_plus8;
} GetBitContext;

static int get_bits_count(const GetBitContext *s)
	{ return s->index; }

static unsigned int get_bits(GetBitContext *s, int n)
{
	unsigned int w;
	int tmp;
	int i, j, k;
	
	i=(s->index>>3);
	w=*(unsigned int *)(s->buffer+i);
	w=	((w<<24)&0xFF000000)|
		((w<< 8)&0x00FF0000)|
		((w>> 8)&0x0000FF00)|
		((w>>24)&0x000000FF);
	
	tmp=w>>(32-(s->index&7)-n);
	tmp&=(1<<n)-1;
	s->index+=n;
	return(tmp);
	
	
//	OPEN_READER(re, s);
//	av_assert2(n>0 && n<=25);
//	UPDATE_CACHE(re, s);
//	tmp = SHOW_UBITS(re, s, n);
//	LAST_SKIP_BITS(re, s, n);
//	CLOSE_READER(re, s);
//	return tmp;
}

#define AVERROR_INVALIDDATA -1
#define AVERROR_PATCHWELCOME -2

#define AV_LOG_ERROR -1
#define AV_LOG_WARNING -2
#define AV_LOG_INFO -3
#define AV_LOG_DEBUG -4

static int init_get_bits(
	GetBitContext *s, const uint8_t *buffer, int bit_size)
{
	int buffer_size;
	int ret = 0;

	if (bit_size >= INT_MAX - 7 || bit_size < 0 || !buffer) {
		bit_size	= 0;
		buffer	  = NULL;
		ret		 = AVERROR_INVALIDDATA;
	}

	buffer_size = (bit_size + 7) >> 3;

	s->buffer			 = buffer;
	s->size_in_bits	   = bit_size;
	s->size_in_bits_plus8 = bit_size + 8;
	s->buffer_end		 = buffer + buffer_size;
	s->index			  = 0;

	return ret;
}

static int init_get_bits8(
	GetBitContext *s, const uint8_t *buffer, int byte_size)
{
	if (byte_size > INT_MAX / 8 || byte_size < 0)
		byte_size = -1;
	return init_get_bits(s, buffer, byte_size * 8);
}


typedef struct {
void *priv_data;
int width;
int height;
int pix_fmt;
int thread_count;
}AVCodecContext;

typedef struct {
uint8_t *data[16];
int linesize[16];
int pict_type;
}AVFrame;

typedef struct {
int nb_components;
}AVPixFmtDescriptor;

typedef struct {
uint8_t *data;
int size;
}AVPacket;

#define AV_PIX_FMT_RGBA64		1
#define AV_PIX_FMT_RGB48		2

#define AV_PICTURE_TYPE_I		1
#define AV_PICTURE_TYPE_P		2

void *av_malloc(int sz)
{
	void *ptr;

	ptr=malloc(sz);
	memset(ptr, 0, sz);
	return(ptr);
}

void av_free(void *ptr)
{
	free(ptr);
}

void av_freep(void **ptr)
{
	if(ptr && *ptr)
	{
		free(*ptr);
		*ptr=NULL;
	}
}

void av_fast_padded_malloc(void **rptr, int *rsz, int sz)
{
	void *ptr;

	ptr=malloc(sz);
	memset(ptr, 0, sz);

	*rsz=sz;
	*rptr=ptr;
}

void av_log(AVCodecContext *avctx, int msgty, char *msg, ...)
{
	va_list lst;
	
	va_start(lst, msg);
	vprintf(msg, lst);
	va_end(lst);
}

void avpriv_report_missing_feature(
	AVCodecContext *avctx, char *msg, ...)
{
	va_list lst;
	
	va_start(lst, msg);
	vprintf(msg, lst);
	va_end(lst);
}

#define av_realloc(ptr, sz)		realloc(ptr, sz)
#define av_mallocz_array(esz, sz)			av_malloc((esz)*(sz))

#define av_assert(cc)
#define av_assert0(cc)


AVPixFmtDescriptor *av_pix_fmt_desc_get(int pixf)
{
	AVPixFmtDescriptor *tmp;
	
	tmp=av_malloc(sizeof(AVPixFmtDescriptor));
	tmp->nb_components=3;
	if(pixf==AV_PIX_FMT_RGBA64)
		tmp->nb_components=4;

	return(tmp);
}

int ff_set_dimensions(AVCodecContext *avctx, int width, int height)
{
	return(0);
}


enum ExrCompr {
	EXR_RAW,
	EXR_RLE,
	EXR_ZIP1,
	EXR_ZIP16,
	EXR_PIZ,
	EXR_PXR24,
	EXR_B44,
	EXR_B44A,
	EXR_UNKN,
};

enum ExrPixelType {
	EXR_UINT,
	EXR_HALF,
	EXR_FLOAT,
	EXR_UNKNOWN,
};

typedef struct EXRChannel {
	int xsub, ysub;
	enum ExrPixelType pixel_type;
} EXRChannel;

typedef struct EXRThreadData {
	uint8_t *uncompressed_data;
	int uncompressed_size;

	uint8_t *tmp;
	int tmp_size;

	uint8_t *bitmap;
	uint16_t *lut;
} EXRThreadData;

typedef struct EXRContext {
//	AVClass *class;
	AVFrame *picture;
	AVCodecContext *avctx;

	enum ExrCompr compression;
	enum ExrPixelType pixel_type;
	int channel_offsets[4]; // 0 = red, 1 = green, 2 = blue and 3 = alpha
	const AVPixFmtDescriptor *desc;

	int w, h;
	uint32_t xmax, xmin;
	uint32_t ymax, ymin;
	uint32_t xdelta, ydelta;
	int ysize;

	uint64_t scan_line_size;
	int scan_lines_per_block;

	GetByteContext gb;
	const uint8_t *buf;
	int buf_size;

	EXRChannel *channels;
	int nb_channels;

	EXRThreadData *thread_data;

	const char *layer;

	float gamma;
	uint16_t gamma_table[65536];
} EXRContext;

/* -15 stored using a single precision bias of 127 */
#define HALF_FLOAT_MIN_BIASED_EXP_AS_SINGLE_FP_EXP 0x38000000

/* max exponent value in single precision that will be converted
 * to Inf or Nan when stored as a half-float */
#define HALF_FLOAT_MAX_BIASED_EXP_AS_SINGLE_FP_EXP 0x47800000

/* 255 is the max exponent biased value */
#define FLOAT_MAX_BIASED_EXP (0xFF << 23)

#define HALF_FLOAT_MAX_BIASED_EXP (0x1F << 10)

union av_intfloat32 {
float f;
unsigned int i;
};

/**
 * Convert a half float as a uint16_t into a full float.
 *
 * @param hf half float as uint16_t
 *
 * @return float value
 */
static union av_intfloat32 exr_half2float(uint16_t hf)
{
	unsigned int sign = (unsigned int) (hf >> 15);
	unsigned int mantissa = (unsigned int) (hf & ((1 << 10) - 1));
	unsigned int exp = (unsigned int) (hf & HALF_FLOAT_MAX_BIASED_EXP);
	union av_intfloat32 f;

	if (exp == HALF_FLOAT_MAX_BIASED_EXP) {
		// we have a half-float NaN or Inf
		// half-float NaNs will be converted to a single precision NaN
		// half-float Infs will be converted to a single precision Inf
		exp = FLOAT_MAX_BIASED_EXP;
		if (mantissa)
			mantissa = (1 << 23) - 1;	// set all bits to indicate a NaN
	} else if (exp == 0x0) {
		// convert half-float zero/denorm to single precision value
		if (mantissa) {
			mantissa <<= 1;
			exp = HALF_FLOAT_MIN_BIASED_EXP_AS_SINGLE_FP_EXP;
			// check for leading 1 in denorm mantissa
			while ((mantissa & (1 << 10))) {
				// for every leading 0, decrement single precision exponent by 1
				// and shift half-float mantissa value to the left
				mantissa <<= 1;
				exp -= (1 << 23);
			}
			// clamp the mantissa to 10-bits
			mantissa &= ((1 << 10) - 1);
			// shift left to generate single-precision mantissa of 23-bits
			mantissa <<= 13;
		}
	} else {
		// shift left to generate single-precision mantissa of 23-bits
		mantissa <<= 13;
		// generate single precision biased exponent value
		exp = (exp << 13) + HALF_FLOAT_MIN_BIASED_EXP_AS_SINGLE_FP_EXP;
	}

	f.i = (sign << 31) | exp | mantissa;

	return f;
}

float exr_half2floatb(uint16_t hf)
{
	union av_intfloat32 f;
	f=exr_half2float(hf);
	return(f.f);
}

/**
 * Convert from 32-bit float as uint32_t to uint16_t.
 *
 * @param v 32-bit float
 *
 * @return normalized 16-bit unsigned int
 */
uint16_t exr_flt2uint(uint32_t v)
{
	unsigned int exp = v >> 23;
	// "HACK": negative values result in exp<  0, so clipping them to 0
	// is also handled by this condition, avoids explicit check for sign bit.
	if (exp <= 127 + 7 - 24) // we would shift out all bits anyway
		return 0;
	if (exp >= 127)
		return 0xffff;
	v &= 0x007fffff;
	return (v + (1 << 23)) >> (127 + 7 - exp);
}

/**
 * Convert from 16-bit float as uint16_t to uint16_t.
 *
 * @param v 16-bit float
 *
 * @return normalized 16-bit unsigned int
 */
uint16_t exr_halflt2uint(uint16_t v)
{
	unsigned exp = 14 - (v >> 10);
	if (exp >= 14) {
		if (exp == 14)
			return (v >> 9) & 1;
		else
			return (v & 0x8000) ? 0 : 0xffff;
	}
	v <<= 6;
	return (v + (1 << 16)) >> (exp + 1);
}

static void predictor(uint8_t *src, int size)
{
	uint8_t *t= src + 1;
	uint8_t *stop = src + size;

	while (t < stop) {
		int d = (int) t[-1] + (int) t[0] - 128;
		t[0] = d;
		++t;
	}
}

static void reorder_pixels(uint8_t *src, uint8_t *dst, int size)
{
	const int8_t *t1 = src;
	const int8_t *t2 = src + (size + 1) / 2;
	int8_t *s= dst;
	int8_t *stop= s + size;

	while (1) {
		if (s < stop)
			*(s++) = *(t1++);
		else
			break;

		if (s < stop)
			*(s++) = *(t2++);
		else
			break;
	}
}

static int zip_uncompress(const uint8_t *src, int compressed_size,
						  int uncompressed_size, EXRThreadData *td)
{
	unsigned long dest_len = uncompressed_size;

//	if (uncompress(td->tmp, &dest_len, src, compressed_size) != Z_OK ||
//		dest_len != uncompressed_size)
		return AVERROR_INVALIDDATA;

	predictor(td->tmp, uncompressed_size);
	reorder_pixels(td->tmp, td->uncompressed_data, uncompressed_size);

	return 0;
}

static int rle_uncompress(const uint8_t *src, int compressed_size,
						  int uncompressed_size, EXRThreadData *td)
{
	uint8_t *d= td->tmp;
	const int8_t *s = src;
	int ssize= compressed_size;
	int dsize= uncompressed_size;
	uint8_t *dend= d + dsize;
	int count;

	while (ssize > 0) {
		count = *s++;

		if (count < 0) {
			count = -count;

			if ((dsize -= count) < 0 ||
				(ssize -= count + 1) < 0)
				return AVERROR_INVALIDDATA;

			while (count--)
				*d++ = *s++;
		} else {
			count++;

			if ((dsize -= count) < 0 ||
				(ssize -= 2) < 0)
				return AVERROR_INVALIDDATA;

			while (count--)
				*d++ = *s;

			s++;
		}
	}

	if (dend != d)
		return AVERROR_INVALIDDATA;

	predictor(td->tmp, uncompressed_size);
	reorder_pixels(td->tmp, td->uncompressed_data, uncompressed_size);

	return 0;
}

#define USHORT_RANGE (1 << 16)
#define BITMAP_SIZE  (1 << 13)

static uint16_t reverse_lut(const uint8_t *bitmap, uint16_t *lut)
{
	int i, k = 0;

	for (i = 0; i < USHORT_RANGE; i++)
		if ((i == 0) || (bitmap[i >> 3] & (1 << (i & 7))))
			lut[k++] = i;

	i = k - 1;

	memset(lut + k, 0, (USHORT_RANGE - k) * 2);

	return i;
}

static void apply_lut(const uint16_t *lut, uint16_t *dst, int dsize)
{
	int i;

	for (i = 0; i < dsize; ++i)
		dst[i] = lut[dst[i]];
}

#define HUF_ENCBITS 16  // literal (value) bit length
#define HUF_DECBITS 14  // decoding bit size (>= 8)

#define HUF_ENCSIZE ((1 << HUF_ENCBITS) + 1)  // encoding table size
#define HUF_DECSIZE (1 << HUF_DECBITS)		// decoding table size
#define HUF_DECMASK (HUF_DECSIZE - 1)

typedef struct HufDec {
	int len;
	int lit;
	int *p;
} HufDec;

static void huf_canonical_code_table(uint64_t *hcode)
{
	uint64_t c, n[59] = { 0 };
	int i;

	for (i = 0; i < HUF_ENCSIZE; ++i)
		n[hcode[i]] += 1;

	c = 0;
	for (i = 58; i > 0; --i) {
		uint64_t nc = ((c + n[i]) >> 1);
		n[i] = c;
		c= nc;
	}

	for (i = 0; i < HUF_ENCSIZE; ++i) {
		int l = hcode[i];

		if (l > 0)
			hcode[i] = l | (n[l]++ << 6);
	}
}

#define SHORT_ZEROCODE_RUN  59
#define LONG_ZEROCODE_RUN	63
#define SHORTEST_LONG_RUN	(2 + LONG_ZEROCODE_RUN - SHORT_ZEROCODE_RUN)
#define LONGEST_LONG_RUN	(255 + SHORTEST_LONG_RUN)

static int huf_unpack_enc_table(GetByteContext *gb,
								int32_t im, int32_t iM, uint64_t *hcode)
{
	GetBitContext gbit;
	int ret = init_get_bits8(&gbit, gb->buffer, bytestream2_get_bytes_left(gb));
	if (ret < 0)
		return ret;

	for (; im <= iM; im++) {
		uint64_t l = hcode[im] = get_bits(&gbit, 6);

		if (l == LONG_ZEROCODE_RUN) {
			int zerun = get_bits(&gbit, 8) + SHORTEST_LONG_RUN;

			if (im + zerun > iM + 1)
				return AVERROR_INVALIDDATA;

			while (zerun--)
				hcode[im++] = 0;

			im--;
		} else if (l >= SHORT_ZEROCODE_RUN) {
			int zerun = l - SHORT_ZEROCODE_RUN + 2;

			if (im + zerun > iM + 1)
				return AVERROR_INVALIDDATA;

			while (zerun--)
				hcode[im++] = 0;

			im--;
		}
	}

	bytestream2_skip(gb, (get_bits_count(&gbit) + 7) / 8);
	huf_canonical_code_table(hcode);

	return 0;
}

static int huf_build_dec_table(const uint64_t *hcode, int im,
								int iM, HufDec *hdecod)
{
	for (; im <= iM; im++) {
		uint64_t c = hcode[im] >> 6;
		int i, l = hcode[im] & 63;

		if (c >> l)
			return AVERROR_INVALIDDATA;

		if (l > HUF_DECBITS) {
			HufDec *pl = hdecod + (c >> (l - HUF_DECBITS));
			if (pl->len)
				return AVERROR_INVALIDDATA;

			pl->lit++;

			pl->p = av_realloc(pl->p, pl->lit * sizeof(int));
			if (!pl->p)
				return AVERROR(ENOMEM);

			pl->p[pl->lit - 1] = im;
		} else if (l) {
			HufDec *pl = hdecod + (c << (HUF_DECBITS - l));

			for (i = 1 << (HUF_DECBITS - l); i > 0; i--, pl++) {
				if (pl->len || pl->p)
					return AVERROR_INVALIDDATA;
				pl->len = l;
				pl->lit = im;
			}
		}
	}

	return 0;
}

#define get_char(c, lc, gb)											 \
{																		\
		c= (c << 8) | bytestream2_get_byte(gb);					  \
		lc += 8;														\
}

#define get_code(po, rlc, c, lc, gb, out, oe)							\
{																		\
		if (po == rlc) {												\
			if (lc < 8)												 \
				get_char(c, lc, gb);									\
			lc -= 8;													\
																		\
			cs = c >> lc;												\
																		\
			if (out + cs > oe)										  \
				return AVERROR_INVALIDDATA;							 \
																		\
			s = out[-1];												\
																		\
			while (cs-- > 0)											\
				*out++ = s;											 \
		} else if (out < oe) {										  \
			*out++ = po;												\
		} else {														\
			return AVERROR_INVALIDDATA;								 \
		}																\
}

static int huf_decode(const uint64_t *hcode, const HufDec *hdecod,
					  GetByteContext *gb, int nbits,
					  int rlc, int no, uint16_t *out)
{
	uint64_t c= 0;
	uint16_t *outb= out;
	uint16_t *oe= out + no;
	const uint8_t *ie = gb->buffer + (nbits + 7) / 8; // input byte size
	uint8_t cs, s;
	int i, lc = 0;

	while (gb->buffer < ie) {
		get_char(c, lc, gb);

		while (lc >= HUF_DECBITS) {
			const HufDec pl = hdecod[(c >> (lc - HUF_DECBITS)) & HUF_DECMASK];

			if (pl.len) {
				lc -= pl.len;
				get_code(pl.lit, rlc, c, lc, gb, out, oe);
			} else {
				int j;

				if (!pl.p)
					return AVERROR_INVALIDDATA;

				for (j = 0; j < pl.lit; j++) {
					int l = hcode[pl.p[j]] & 63;

					while (lc < l && bytestream2_get_bytes_left(gb) > 0)
						get_char(c, lc, gb);

					if (lc >= l) {
						if ((hcode[pl.p[j]] >> 6) ==
							((c >> (lc - l)) & ((1LL << l) - 1))) {
							lc -= l;
							get_code(pl.p[j], rlc, c, lc, gb, out, oe);
							break;
						}
					}
				}

				if (j == pl.lit)
					return AVERROR_INVALIDDATA;
			}
		}
	}

	i= (8 - nbits) & 7;
	c >>= i;
	lc -= i;

	while (lc > 0) {
		const HufDec pl = hdecod[(c << (HUF_DECBITS - lc)) & HUF_DECMASK];

		if (pl.len) {
			lc -= pl.len;
			get_code(pl.lit, rlc, c, lc, gb, out, oe);
		} else {
			return AVERROR_INVALIDDATA;
		}
	}

	if (out - outb != no)
		return AVERROR_INVALIDDATA;
	return 0;
}

static int huf_uncompress(GetByteContext *gb,
						  uint16_t *dst, int dst_size)
{
	int32_t src_size, im, iM;
	uint32_t nBits;
	uint64_t *freq;
	HufDec *hdec;
	int ret, i;

	src_size = bytestream2_get_le32(gb);
	im= bytestream2_get_le32(gb);
	iM= bytestream2_get_le32(gb);
	bytestream2_skip(gb, 4);
	nBits = bytestream2_get_le32(gb);
	if (im < 0 || im >= HUF_ENCSIZE ||
		iM < 0 || iM >= HUF_ENCSIZE ||
		src_size < 0)
		return AVERROR_INVALIDDATA;

	bytestream2_skip(gb, 4);

	freq = av_mallocz_array(HUF_ENCSIZE, sizeof(*freq));
	hdec = av_mallocz_array(HUF_DECSIZE, sizeof(*hdec));
	if (!freq || !hdec) {
		ret = AVERROR(ENOMEM);
		goto fail;
	}

//	goto fail;

	if ((ret = huf_unpack_enc_table(gb, im, iM, freq)) < 0)
		goto fail;

	if (nBits > 8 * bytestream2_get_bytes_left(gb)) {
		ret = AVERROR_INVALIDDATA;
		goto fail;
	}

//	goto fail;

	if ((ret = huf_build_dec_table(freq, im, iM, hdec)) < 0)
		goto fail;
	ret = huf_decode(freq, hdec, gb, nBits, iM, dst_size, dst);

fail:
	for (i = 0; i < HUF_DECSIZE; i++)
		if (hdec)
			av_freep(&hdec[i].p);

	av_free(freq);
	av_free(hdec);

	return ret;
}

void wdec14(uint16_t l, uint16_t h, uint16_t *a, uint16_t *b)
{
	int16_t ls = l;
	int16_t hs = h;
	int hi= hs;
	int ai= ls + (hi & 1) + (hi >> 1);
	int16_t as = ai;
	int16_t bs = ai - hi;

	*a = as;
	*b = bs;
}

#define NBITS	  16
#define A_OFFSET  (1 << (NBITS - 1))
#define MOD_MASK  ((1 << NBITS) - 1)

void wdec16(uint16_t l, uint16_t h, uint16_t *a, uint16_t *b)
{
	int m  = l;
	int d  = h;
	int bb = (m - (d >> 1)) & MOD_MASK;
	int aa = (d + bb - A_OFFSET) & MOD_MASK;
	*b = bb;
	*a = aa;
}

static void wav_decode(uint16_t *in, int nx, int ox,
						int ny, int oy, uint16_t mx)
{
	int w14 = (mx < (1 << 14));
	int n= (nx > ny) ? ny : nx;
	int p= 1;
	int p2;

	while (p <= n)
		p <<= 1;

	p >>= 1;
	p2  = p;
	p >>= 1;

	while (p >= 1) {
		uint16_t *py = in;
		uint16_t *ey = in + oy * (ny - p2);
		uint16_t i00, i01, i10, i11;
		int oy1 = oy * p;
		int oy2 = oy * p2;
		int ox1 = ox * p;
		int ox2 = ox * p2;

		for (; py <= ey; py += oy2) {
			uint16_t *px = py;
			uint16_t *ex = py + ox * (nx - p2);

			for (; px <= ex; px += ox2) {
				uint16_t *p01 = px + ox1;
				uint16_t *p10 = px + oy1;
				uint16_t *p11 = p10 + ox1;

				if (w14) {
					wdec14(*px, *p10, &i00, &i10);
					wdec14(*p01, *p11, &i01, &i11);
					wdec14(i00, i01, px, p01);
					wdec14(i10, i11, p10, p11);
				} else {
					wdec16(*px, *p10, &i00, &i10);
					wdec16(*p01, *p11, &i01, &i11);
					wdec16(i00, i01, px, p01);
					wdec16(i10, i11, p10, p11);
				}
			}

			if (nx & p) {
				uint16_t *p10 = px + oy1;

				if (w14)
					wdec14(*px, *p10, &i00, p10);
				else
					wdec16(*px, *p10, &i00, p10);

				*px = i00;
			}
		}

		if (ny & p) {
			uint16_t *px = py;
			uint16_t *ex = py + ox * (nx - p2);

			for (; px <= ex; px += ox2) {
				uint16_t *p01 = px + ox1;

				if (w14)
					wdec14(*px, *p01, &i00, p01);
				else
					wdec16(*px, *p01, &i00, p01);

				*px = i00;
			}
		}

		p2  = p;
		p >>= 1;
	}
}

static int piz_uncompress(EXRContext *s, const uint8_t *src, int ssize,
						  int dsize, EXRThreadData *td)
{
	GetByteContext gb;
	uint16_t maxval, min_non_zero, max_non_zero;
	uint16_t *ptr;
	uint16_t *tmp = (uint16_t *)td->tmp;
	uint8_t *out;
	int ret, i, j;

	if (!td->bitmap)
		td->bitmap = av_malloc(BITMAP_SIZE);
	if (!td->lut)
		td->lut = av_malloc(1 << 17);
	if (!td->bitmap || !td->lut) {
		av_freep(&td->bitmap);
		av_freep(&td->lut);
		return AVERROR(ENOMEM);
	}

	bytestream2_init(&gb, src, ssize);
	min_non_zero = bytestream2_get_le16(&gb);
	max_non_zero = bytestream2_get_le16(&gb);

	if (max_non_zero >= BITMAP_SIZE)
		return AVERROR_INVALIDDATA;

	memset(td->bitmap, 0, FFMIN(min_non_zero, BITMAP_SIZE));
	if (min_non_zero <= max_non_zero)
		bytestream2_get_buffer(&gb, td->bitmap + min_non_zero,
								max_non_zero - min_non_zero + 1);
	memset(td->bitmap + max_non_zero, 0, BITMAP_SIZE - max_non_zero);

	maxval = reverse_lut(td->bitmap, td->lut);

	ret = huf_uncompress(&gb, tmp, dsize / sizeof(uint16_t));
	if (ret)
		return ret;

	ptr = tmp;
	for (i = 0; i < s->nb_channels; i++) {
		EXRChannel *channel = &s->channels[i];
		int size = channel->pixel_type;

		for (j = 0; j < size; j++)
			wav_decode(ptr + j, s->xdelta, size, s->ysize,
						s->xdelta * size, maxval);
		ptr += s->xdelta * s->ysize * size;
	}

	apply_lut(td->lut, tmp, dsize / sizeof(uint16_t));

	out = td->uncompressed_data;
	for (i = 0; i < s->ysize; i++)
		for (j = 0; j < s->nb_channels; j++) {
			uint16_t *in = tmp + j * s->xdelta * s->ysize + i * s->xdelta;
			memcpy(out, in, s->xdelta * 2);
			out += s->xdelta * 2;
		}

	return 0;
}

static int pxr24_uncompress(EXRContext *s, const uint8_t *src,
							int compressed_size, int uncompressed_size,
							EXRThreadData *td)
{
	unsigned long dest_len = uncompressed_size;
	const uint8_t *in = td->tmp;
	uint8_t *out;
	int c, i, j;

//	if (uncompress(td->tmp, &dest_len, src, compressed_size) != Z_OK ||
//		dest_len != uncompressed_size)
		return AVERROR_INVALIDDATA;

	out = td->uncompressed_data;
	for (i = 0; i < s->ysize; i++)
		for (c = 0; c < s->nb_channels; c++) {
			EXRChannel *channel = &s->channels[c];
			const uint8_t *ptr[4];
			uint32_t pixel = 0;

			switch (channel->pixel_type) {
			case EXR_FLOAT:
				ptr[0] = in;
				ptr[1] = ptr[0] + s->xdelta;
				ptr[2] = ptr[1] + s->xdelta;
				in= ptr[2] + s->xdelta;

				for (j = 0; j < s->xdelta; ++j) {
					uint32_t diff = (*(ptr[0]++) << 24) |
									(*(ptr[1]++) << 16) |
									(*(ptr[2]++) << 8);
					pixel += diff;
//					bytestream_put_le32(&out, pixel);
					*(uint32_t *)out=pixel;
					out+=4;
				}
				break;
			case EXR_HALF:
				ptr[0] = in;
				ptr[1] = ptr[0] + s->xdelta;
				in= ptr[1] + s->xdelta;
				for (j = 0; j < s->xdelta; j++) {
					uint32_t diff = (*(ptr[0]++) << 8) | *(ptr[1]++);

					pixel += diff;
//					bytestream_put_le16(&out, pixel);
					*(uint16_t *)out=pixel;
					out+=2;
				}
				break;
			default:
				return AVERROR_INVALIDDATA;
			}
		}

	return 0;
}

static int decode_block(AVCodecContext *avctx, void *tdata,
						int jobnr, int threadnr)
{
	EXRContext *s = avctx->priv_data;
	AVFrame *const p = s->picture;
	EXRThreadData *td = &s->thread_data[threadnr];
	const uint8_t *channel_buffer[4] = { 0 };
	const uint8_t *buf = s->buf;
	uint64_t line_offset, uncompressed_size;
	uint32_t xdelta = s->xdelta;
	uint16_t *ptr_x;
	uint8_t *ptr;
	uint32_t data_size, line;
	const uint8_t *src;
	int axmax = (avctx->width - (s->xmax + 1)) * 2 * s->desc->nb_components;
	int bxmin = s->xmin * 2 * s->desc->nb_components;
	int i, x, buf_size = s->buf_size;
	float one_gamma = 1.0f / s->gamma;
	int ret;

	line_offset = AV_RL64(s->gb.buffer + jobnr * 8);
	// Check if the buffer has the required bytes needed from the offset
	if (line_offset > buf_size - 8)
		return AVERROR_INVALIDDATA;

	src  = buf + line_offset + 8;
	line = AV_RL32(src - 8);
	if (line < s->ymin || line > s->ymax)
		return AVERROR_INVALIDDATA;

	data_size = AV_RL32(src - 4);
	if (data_size <= 0 || data_size > buf_size)
		return AVERROR_INVALIDDATA;

	s->ysize= FFMIN(s->scan_lines_per_block, s->ymax - line + 1);
	uncompressed_size = s->scan_line_size * s->ysize;
	if ((s->compression == EXR_RAW && (data_size != uncompressed_size ||
								 line_offset > buf_size - uncompressed_size)) ||
		(s->compression != EXR_RAW && (data_size > uncompressed_size ||
								 line_offset > buf_size - data_size))) {
		return AVERROR_INVALIDDATA;
	}

	if (data_size < uncompressed_size) {
		av_fast_padded_malloc(&td->uncompressed_data,
							  &td->uncompressed_size, uncompressed_size);
		av_fast_padded_malloc(&td->tmp, &td->tmp_size, uncompressed_size);
		if (!td->uncompressed_data || !td->tmp)
			return AVERROR(ENOMEM);

		ret = AVERROR_INVALIDDATA;
		switch (s->compression) {
		case EXR_ZIP1:
		case EXR_ZIP16:
			ret = zip_uncompress(src, data_size, uncompressed_size, td);
			break;
		case EXR_PIZ:
			ret = piz_uncompress(s, src, data_size, uncompressed_size, td);
			break;
		case EXR_PXR24:
			ret = pxr24_uncompress(s, src, data_size, uncompressed_size, td);
			break;
		case EXR_RLE:
			ret = rle_uncompress(src, data_size, uncompressed_size, td);
		}
		if (ret < 0) {
			av_log(avctx, AV_LOG_ERROR, "decode_block() failed.\n");
			return ret;
		}
		src = td->uncompressed_data;
	}

	channel_buffer[0] = src + xdelta * s->channel_offsets[0];
	channel_buffer[1] = src + xdelta * s->channel_offsets[1];
	channel_buffer[2] = src + xdelta * s->channel_offsets[2];
	if (s->channel_offsets[3] >= 0)
		channel_buffer[3] = src + xdelta * s->channel_offsets[3];

	ptr = p->data[0] + line * p->linesize[0];
	for (i = 0;
		 i < s->scan_lines_per_block && line + i <= s->ymax;
		 i++, ptr += p->linesize[0]) {
		const uint8_t *r, *g, *b, *a;

		r = channel_buffer[0];
		g = channel_buffer[1];
		b = channel_buffer[2];
		if (channel_buffer[3])
			a = channel_buffer[3];

		ptr_x = (uint16_t *) ptr;

		// Zero out the start if xmin is not 0
		memset(ptr_x, 0, bxmin);
		ptr_x += s->xmin * s->desc->nb_components;
		if (s->pixel_type == EXR_FLOAT) {
			// 32-bit
			for (x = 0; x < xdelta; x++) {
				union av_intfloat32 t;
				t.i = bytestream_get_le32(&r);
				if (t.f > 0.0f)  /* avoid negative values */
					t.f = powf(t.f, one_gamma);
				*ptr_x++ = exr_flt2uint(t.i);

				t.i = bytestream_get_le32(&g);
				if (t.f > 0.0f)
					t.f = powf(t.f, one_gamma);
				*ptr_x++ = exr_flt2uint(t.i);

				t.i = bytestream_get_le32(&b);
				if (t.f > 0.0f)
					t.f = powf(t.f, one_gamma);
				*ptr_x++ = exr_flt2uint(t.i);
				if (channel_buffer[3])
					*ptr_x++ = exr_flt2uint(bytestream_get_le32(&a));
			}
		} else {
			// 16-bit
			for (x = 0; x < xdelta; x++) {
//				*ptr_x++ = s->gamma_table[bytestream_get_le16(&r)];
//				*ptr_x++ = s->gamma_table[bytestream_get_le16(&g)];
//				*ptr_x++ = s->gamma_table[bytestream_get_le16(&b)];

				*ptr_x++ = bytestream_get_le16(&r);
				*ptr_x++ = bytestream_get_le16(&g);
				*ptr_x++ = bytestream_get_le16(&b);

				if (channel_buffer[3])
					*ptr_x++ = exr_halflt2uint(bytestream_get_le16(&a));
			}
		}

		// Zero out the end if xmax+1 is not w
		memset(ptr_x, 0, axmax);

		channel_buffer[0] += s->scan_line_size;
		channel_buffer[1] += s->scan_line_size;
		channel_buffer[2] += s->scan_line_size;
		if (channel_buffer[3])
			channel_buffer[3] += s->scan_line_size;
	}

	return 0;
}

/**
 * Check if the variable name corresponds to its data type.
 *
 * @param s			  the EXRContext
 * @param value_name	 name of the variable to check
 * @param value_type	 type of the variable to check
 * @param minimum_length minimum length of the variable data
 *
 * @return bytes to read containing variable data
 *		 -1 if variable is not found
 *		 0 if buffer ended prematurely
 */
static int check_header_variable(EXRContext *s,
								 const char *value_name,
								 const char *value_type,
								 unsigned int minimum_length)
{
	int var_size = -1;

	if (bytestream2_get_bytes_left(&s->gb) >= minimum_length &&
		!strcmp(s->gb.buffer, value_name)) {
		// found value_name, jump to value_type (null terminated strings)
		s->gb.buffer += strlen(value_name) + 1;
		if (!strcmp(s->gb.buffer, value_type)) {
			s->gb.buffer += strlen(value_type) + 1;
			var_size = bytestream2_get_le32(&s->gb);
			// don't go read past boundaries
			if (var_size > bytestream2_get_bytes_left(&s->gb))
				var_size = 0;
		} else {
			// value_type not found, reset the buffer
			s->gb.buffer -= strlen(value_name) + 1;
			av_log(s->avctx, AV_LOG_WARNING,
					"Unknown data type %s for header variable %s.\n",
					value_type, value_name);
		}
	}

	return var_size;
}

static int decode_header(EXRContext *s)
{
	int current_channel_offset = 0;
	int magic_number, version, flags, i;

	s->xmin= ~0;
	s->xmax= ~0;
	s->ymin= ~0;
	s->ymax= ~0;
	s->xdelta= ~0;
	s->ydelta= ~0;
	s->channel_offsets[0] = -1;
	s->channel_offsets[1] = -1;
	s->channel_offsets[2] = -1;
	s->channel_offsets[3] = -1;
	s->pixel_type= EXR_UNKNOWN;
	s->compression= EXR_UNKN;
	s->nb_channels= 0;
	s->w	= 0;
	s->h	= 0;

	if (bytestream2_get_bytes_left(&s->gb) < 10) {
		av_log(s->avctx, AV_LOG_ERROR, "Header too short to parse.\n");
		return AVERROR_INVALIDDATA;
	}

	magic_number = bytestream2_get_le32(&s->gb);
	if (magic_number != 20000630) {
		/* As per documentation of OpenEXR, it is supposed to be
		 * int 20000630 little-endian */
		av_log(s->avctx, AV_LOG_ERROR, "Wrong magic number %d.\n", magic_number);
		return AVERROR_INVALIDDATA;
	}

	version = bytestream2_get_byte(&s->gb);
	if (version != 2) {
		avpriv_report_missing_feature(s->avctx, "Version %d", version);
		return AVERROR_PATCHWELCOME;
	}

	flags = bytestream2_get_le24(&s->gb);
	if (flags & 0x02) {
		avpriv_report_missing_feature(s->avctx, "Tile support");
		return AVERROR_PATCHWELCOME;
	}

	// Parse the header
	while (bytestream2_get_bytes_left(&s->gb) > 0 && *s->gb.buffer) {
		int var_size;
		if ((var_size = check_header_variable(s, "channels",
											  "chlist", 38)) >= 0) {
			GetByteContext ch_gb;
			if (!var_size)
				return AVERROR_INVALIDDATA;

			bytestream2_init(&ch_gb, s->gb.buffer, var_size);

			while (bytestream2_get_bytes_left(&ch_gb) >= 19) {
				EXRChannel *channel;
				enum ExrPixelType current_pixel_type;
				int channel_index = -1;
				int xsub, ysub;

				if (strcmp(s->layer, "") != 0) {
					if (strncmp(ch_gb.buffer,
							s->layer, strlen(s->layer)) == 0) {
						ch_gb.buffer += strlen(s->layer);
						if (*ch_gb.buffer == '.')
							ch_gb.buffer++;		 /* skip dot if not given */
						av_log(s->avctx, AV_LOG_INFO,
								"Layer %s.%s matched.\n",
								s->layer, ch_gb.buffer);
					}
				}

				if (!strcmp(ch_gb.buffer, "R") ||
					!strcmp(ch_gb.buffer, "X") ||
					!strcmp(ch_gb.buffer, "U"))
					channel_index = 0;
				else if (!strcmp(ch_gb.buffer, "G") ||
						 !strcmp(ch_gb.buffer, "Y") ||
						 !strcmp(ch_gb.buffer, "V"))
					channel_index = 1;
				else if (!strcmp(ch_gb.buffer, "B") ||
						 !strcmp(ch_gb.buffer, "Z") ||
						 !strcmp(ch_gb.buffer, "W"))
					channel_index = 2;
				else if (!strcmp(ch_gb.buffer, "A"))
					channel_index = 3;
				else
					av_log(s->avctx, AV_LOG_WARNING,
							"Unsupported channel %.256s.\n", ch_gb.buffer);

				/* skip until you get a 0 */
				while (bytestream2_get_bytes_left(&ch_gb) > 0 &&
						bytestream2_get_byte(&ch_gb))
					continue;

				if (bytestream2_get_bytes_left(&ch_gb) < 4) {
					av_log(s->avctx, AV_LOG_ERROR, "Incomplete header.\n");
					return AVERROR_INVALIDDATA;
				}

				current_pixel_type = bytestream2_get_le32(&ch_gb);
				if (current_pixel_type >= EXR_UNKNOWN) {
					avpriv_report_missing_feature(s->avctx,
												  "Pixel type %d.\n",
												  current_pixel_type);
					return AVERROR_PATCHWELCOME;
				}

				bytestream2_skip(&ch_gb, 4);
				xsub = bytestream2_get_le32(&ch_gb);
				ysub = bytestream2_get_le32(&ch_gb);
				if (xsub != 1 || ysub != 1) {
					avpriv_report_missing_feature(s->avctx,
												  "Subsampling %dx%d",
												  xsub, ysub);
					return AVERROR_PATCHWELCOME;
				}

				if (channel_index >= 0) {
					if (s->pixel_type != EXR_UNKNOWN &&
						s->pixel_type != current_pixel_type) {
						av_log(s->avctx, AV_LOG_ERROR,
								"RGB channels not of the same depth.\n");
						return AVERROR_INVALIDDATA;
					}
					s->pixel_type		= current_pixel_type;
					s->channel_offsets[channel_index] = current_channel_offset;
				}

				s->channels = av_realloc(s->channels,
										 ++s->nb_channels * sizeof(EXRChannel));
				if (!s->channels)
					return AVERROR(ENOMEM);
				channel= &s->channels[s->nb_channels - 1];
				channel->pixel_type = current_pixel_type;
				channel->xsub= xsub;
				channel->ysub= ysub;

				current_channel_offset += 1 << current_pixel_type;
			}

			/* Check if all channels are set with an offset or if the channels
			 * are causing an overflow  */
			if (FFMIN3(s->channel_offsets[0],
						s->channel_offsets[1],
						s->channel_offsets[2]) < 0) {
				if (s->channel_offsets[0] < 0)
					av_log(s->avctx, AV_LOG_ERROR,
						"Missing red channel.\n");
				if (s->channel_offsets[1] < 0)
					av_log(s->avctx, AV_LOG_ERROR,
						"Missing green channel.\n");
				if (s->channel_offsets[2] < 0)
					av_log(s->avctx, AV_LOG_ERROR,
						"Missing blue channel.\n");
				return AVERROR_INVALIDDATA;
			}

			// skip one last byte and update main gb
			s->gb.buffer = ch_gb.buffer + 1;
			continue;
		} else if ((var_size = check_header_variable(s,
			"dataWindow", "box2i", 31)) >= 0) {
			if (!var_size)
				return AVERROR_INVALIDDATA;

			s->xmin= bytestream2_get_le32(&s->gb);
			s->ymin= bytestream2_get_le32(&s->gb);
			s->xmax= bytestream2_get_le32(&s->gb);
			s->ymax= bytestream2_get_le32(&s->gb);
			s->xdelta = (s->xmax - s->xmin) + 1;
			s->ydelta = (s->ymax - s->ymin) + 1;

			continue;
		} else if ((var_size = check_header_variable(s,
				"displayWindow", "box2i", 34)) >= 0) {
			if (!var_size)
				return AVERROR_INVALIDDATA;

			bytestream2_skip(&s->gb, 8);
			s->w = bytestream2_get_le32(&s->gb) + 1;
			s->h = bytestream2_get_le32(&s->gb) + 1;

			continue;
		} else if ((var_size = check_header_variable(s,
				"lineOrder", "lineOrder", 25)) >= 0) {
			int line_order;
			if (!var_size)
				return AVERROR_INVALIDDATA;

			line_order = bytestream2_get_byte(&s->gb);
			av_log(s->avctx, AV_LOG_DEBUG, "line order: %d.\n", line_order);
			if (line_order > 2) {
				av_log(s->avctx, AV_LOG_ERROR, "Unknown line order.\n");
				return AVERROR_INVALIDDATA;
			}

			continue;
		} else if ((var_size = check_header_variable(s,
				"pixelAspectRatio", "float", 31)) >= 0) {
			if (!var_size)
				return AVERROR_INVALIDDATA;

//			ff_set_sar(s->avctx,
//						av_d2q(av_int2float(
//							bytestream2_get_le32(&s->gb)), 255));
//			s->gb.buffer+=4;
			bytestream2_get_le32(&s->gb);

			continue;
		} else if ((var_size = check_header_variable(s,
			"compression", "compression", 29)) >= 0) {
			if (!var_size)
				return AVERROR_INVALIDDATA;

			if (s->compression == EXR_UNKN)
				s->compression = bytestream2_get_byte(&s->gb);
			else
				av_log(s->avctx, AV_LOG_WARNING,
						"Found more than one compression attribute.\n");

			continue;
		}

		// Check if there are enough bytes for a header
		if (bytestream2_get_bytes_left(&s->gb) <= 9) {
			av_log(s->avctx, AV_LOG_ERROR, "Incomplete header\n");
			return AVERROR_INVALIDDATA;
		}

		// Process unknown variables
		for (i = 0; i < 2; i++) // value_name and value_type
			while (bytestream2_get_byte(&s->gb) != 0);

		// Skip variable length
		bytestream2_skip(&s->gb, bytestream2_get_le32(&s->gb));
	}

	if (s->compression == EXR_UNKN) {
		av_log(s->avctx, AV_LOG_ERROR, "Missing compression attribute.\n");
		return AVERROR_INVALIDDATA;
	}
	s->scan_line_size = s->xdelta * current_channel_offset;

	if (bytestream2_get_bytes_left(&s->gb) <= 0) {
		av_log(s->avctx, AV_LOG_ERROR, "Incomplete frame.\n");
		return AVERROR_INVALIDDATA;
	}

	// aaand we are done
	bytestream2_skip(&s->gb, 1);
	return 0;
}

static int decode_frame(AVCodecContext *avctx, void *data,
						int *got_frame, AVPacket *avpkt)
{
	EXRContext *s = avctx->priv_data;
//	ThreadFrame frame = { .f = data };
	AVFrame *picture = data;
	uint8_t *ptr;

	int y, ret;
	int out_line_size;
	int scan_line_blocks;

	bytestream2_init(&s->gb, avpkt->data, avpkt->size);

	if ((ret = decode_header(s)) < 0)
		return ret;

	if(!avctx->width)
	{
		avctx->width=s->xdelta;
		avctx->height=s->ydelta;
		picture->linesize[0]=s->xdelta*
			((s->channel_offsets[3]>=0)?8:6);
	}

	switch (s->pixel_type) {
	case EXR_FLOAT:
	case EXR_HALF:
		if (s->channel_offsets[3] >= 0)
			avctx->pix_fmt = AV_PIX_FMT_RGBA64;
		else
			avctx->pix_fmt = AV_PIX_FMT_RGB48;
		break;
	case EXR_UINT:
//		avpriv_request_sample(avctx, "32-bit unsigned int");
		return AVERROR_PATCHWELCOME;
	default:
		av_log(avctx, AV_LOG_ERROR, "Missing channel list.\n");
		return AVERROR_INVALIDDATA;
	}

	switch (s->compression) {
	case EXR_RAW:
	case EXR_RLE:
	case EXR_ZIP1:
		s->scan_lines_per_block = 1;
		break;
	case EXR_PXR24:
	case EXR_ZIP16:
		s->scan_lines_per_block = 16;
		break;
	case EXR_PIZ:
		s->scan_lines_per_block = 32;
		break;
	default:
		avpriv_report_missing_feature(avctx, "Compression %d", s->compression);
		return AVERROR_PATCHWELCOME;
	}

	/* Verify the xmin, xmax, ymin, ymax and xdelta before setting
	 * the actual image size. */
	if (s->xmin > s->xmax				  ||
		s->ymin > s->ymax				  ||
		s->xdelta != s->xmax - s->xmin + 1 ||
		s->xmax >= s->w					||
		s->ymax >= s->h) {
		av_log(avctx, AV_LOG_ERROR, "Wrong or missing size information.\n");
		return AVERROR_INVALIDDATA;
	}

	if ((ret = ff_set_dimensions(avctx, s->w, s->h)) < 0)
		return ret;

	s->desc= av_pix_fmt_desc_get(avctx->pix_fmt);
	if (!s->desc)
		return AVERROR_INVALIDDATA;
	out_line_size= avctx->width * 2 * s->desc->nb_components;
	scan_line_blocks = (s->ydelta + s->scan_lines_per_block - 1) /
						s->scan_lines_per_block;

//	if ((ret = ff_thread_get_buffer(avctx, &frame, 0)) < 0)
//		return ret;

	if (bytestream2_get_bytes_left(&s->gb) < scan_line_blocks * 8)
		return AVERROR_INVALIDDATA;

	// save pointer we are going to use in decode_block
	s->buf= avpkt->data;
	s->buf_size = avpkt->size;
	ptr= picture->data[0];

	// Zero out the start if ymin is not 0
	for (y = 0; y < s->ymin; y++) {
		memset(ptr, 0, out_line_size);
		ptr += picture->linesize[0];
	}

	s->picture = picture;
//	avctx->execute2(avctx, decode_block, s->thread_data, NULL, scan_line_blocks);

	for(y=0; y<scan_line_blocks; y++)
		decode_block(avctx, s->thread_data, y, 0);

	// Zero out the end if ymax+1 is not h
	for (y = s->ymax + 1; y < avctx->height; y++) {
		memset(ptr, 0, out_line_size);
		ptr += picture->linesize[0];
	}

	picture->pict_type = AV_PICTURE_TYPE_I;
	*got_frame = 1;

	return avpkt->size;
}

static int decode_init(AVCodecContext *avctx)
{
	EXRContext *s = avctx->priv_data;
	uint32_t i;
	union av_intfloat32 t;
	float one_gamma = 1.0f / s->gamma;

	s->avctx= avctx;

	if (one_gamma > 0.9999f && one_gamma < 1.0001f) {
		for (i = 0; i < 65536; ++i)
			s->gamma_table[i] = exr_halflt2uint(i);
	} else {
		for (i = 0; i < 65536; ++i) {
			t = exr_half2float(i);
			/* If negative value we reuse half value */
			if (t.f <= 0.0f) {
				s->gamma_table[i] = exr_halflt2uint(i);
			} else {
				t.f = powf(t.f, one_gamma);
				s->gamma_table[i] = exr_flt2uint(t.i);
			}
		}
	}

	// allocate thread data, used for non EXR_RAW compreesion types
	s->thread_data = av_mallocz_array(avctx->thread_count, sizeof(EXRThreadData));
	if (!s->thread_data)
		return AVERROR_INVALIDDATA;

	return 0;
}

int decode_image_buffer(
	void *ibuf, int ibsz,
	uint16_t *obuf, int *rxs, int *rys)
{
	AVCodecContext tavctx;
	AVPacket tavpkt;
	AVFrame tavfrm;
	EXRContext texrctx;
	AVCodecContext *avctx;
	AVPacket *avpkt;
	AVFrame *avfrm;
	EXRContext *exrctx;
	int i;
	
	avctx=&tavctx;
	avpkt=&tavpkt;
	avfrm=&tavfrm;
	exrctx=&texrctx;
	
	memset(avctx, 0, sizeof(AVCodecContext));
	memset(avpkt, 0, sizeof(AVPacket));
	memset(avfrm, 0, sizeof(AVFrame));
	memset(exrctx, 0, sizeof(EXRContext));

	avctx->priv_data=exrctx;
	exrctx->layer="";
	exrctx->gamma=1.0;
	
	avpkt->data=ibuf;
	avpkt->size=ibsz;
	
	avfrm->data[0]=(uint8_t *)obuf;
	avfrm->linesize[0]=0;
	
	avctx->thread_count=4;
	
	decode_init(avctx);
	
	i=0;
	decode_frame(avctx, avfrm, &i, avpkt);
	
	if(rxs)
		*rxs=avctx->width;
	if(rys)
		*rys=avctx->height;
	return(i);
}

int clamp255(int x)
{
	if(x<0)return(0);
	if(x>255)return(255);
	return(x);
}

int main(int argc, char *argv[])
{
	int ofst[1024];
	byte *ibuf;
	u16 *hfbuf;
	byte *obuf;
	byte *cs, *cs1;
	char *an, *aty, *chn;
	FILE *ifd;
	double f, g;
	int cmp, xs, ys;
	int isz, asz;
	int cr, cg, cb, cm, csc;
	int i, j, k, l;
	
	ifd=fopen(argv[1], "rb");
	if(!ifd)
		return(-1);
	
	fseek(ifd, 0, 2);
	isz=ftell(ifd);
	fseek(ifd, 0, 0);
	
	ibuf=malloc(isz);
	fread(ibuf, 1, isz, ifd);
	fclose(ifd);

	hfbuf=malloc(3840*2160*8);
	
	i=decode_image_buffer(ibuf, isz, hfbuf, &xs, &ys);
	
	printf("Ret=%d Xs=%d Ys=%d\n", i, xs, ys);

	cm=0; g=0;
	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
	{
		cr=hfbuf[(i*xs+j)*3+0];
		cg=hfbuf[(i*xs+j)*3+1];
		cb=hfbuf[(i*xs+j)*3+2];
		
		if(cr>=0x8000)cr=0;
		if(cg>=0x8000)cg=0;
		if(cb>=0x8000)cb=0;
		
		f=exr_half2floatb(cr);
//		g=g+f*f;
		g=g+f;
		f=exr_half2floatb(cg);
//		g=g+f*f;
		g=g+f;
		f=exr_half2floatb(cb);
//		g=g+f*f;
		g=g+f;
		
		if(cr>cm)cm=cr;
		if(cg>cm)cm=cg;
		if(cb>cm)cm=cb;
	}
	
	g=sqrt(g/(xs*ys*3));
//	g=g/(xs*ys*3);
	
	printf("CMax=%d %f\n", cm, g);
	csc=(1<<24)/cm;
	
	obuf=malloc(xs*ys*4);
	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
	{
//		k=(i*xs+j)*3;
		k=((ys-i-1)*xs+j)*3;
		cr=hfbuf[k+0];
		cg=hfbuf[k+1];
		cb=hfbuf[k+2];

		if(cr>=0x8000)cr=0;
		if(cg>=0x8000)cg=0;
		if(cb>=0x8000)cb=0;
		
//		obuf[(i*xs+j)*4+0]=cr>>8;
//		obuf[(i*xs+j)*4+1]=cg>>8;
//		obuf[(i*xs+j)*4+2]=cb>>8;

		obuf[(i*xs+j)*4+0]=clamp255(exr_half2floatb(cr)*256/(0.7*g));
		obuf[(i*xs+j)*4+1]=clamp255(exr_half2floatb(cg)*256/(0.7*g));
		obuf[(i*xs+j)*4+2]=clamp255(exr_half2floatb(cb)*256/(0.7*g));

//		obuf[(i*xs+j)*4+0]=clamp255((cr*csc)>>16);
//		obuf[(i*xs+j)*4+1]=clamp255((cg*csc)>>16);
//		obuf[(i*xs+j)*4+2]=clamp255((cb*csc)>>16);

		obuf[(i*xs+j)*4+3]=255;
	}
	
	BTIC1H_Img_SaveTGA("lavexr_out0.tga", obuf, xs, ys);
}
