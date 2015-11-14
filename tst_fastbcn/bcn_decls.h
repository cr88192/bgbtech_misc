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

#define BC7_PFB_BGR			1		//BGR Ordering
#define BC7_PFB_NOAX		2		//No Alpha
#define BC7_PFB_NOPART		4		//No Partitions

#define BCN_PFB_FL_BGR		0x01	//BGR Ordering
#define BCN_PFB_FL_NOAX		0x02	//No Alpha
#define BCN_PFB_FL_NOPART	0x04	//No Partitions
#define BCN_PFB_FL_YUV		0x08	//YUV Colorspace

#define BCN_PFB_STR_MASK	0x30	//Stride=4-(Str&3);
#define BCN_PFB_PCF_MASK	0xC0

#define BCN_PFB_STR_4		0x00
#define BCN_PFB_STR_3		0x10
#define BCN_PFB_STR_2		0x20
#define BCN_PFB_STR_1		0x30

#define BCN_PFB_PCF_BYTE	0x00
#define BCN_PFB_PCF_RESV	0x40
#define BCN_PFB_PCF_HFLOAT	0x80
#define BCN_PFB_PCF_FLOAT	0xC0

#define BCN_PFB_RGBA		0x00
#define BCN_PFB_BGRA		0x01
#define BCN_PFB_RGBX		0x02
#define BCN_PFB_BGRX		0x03

#define BCN_PFB_RGB			0x10
#define BCN_PFB_BGR			0x11

#define BCN_PFB_HFRGBA		0x80
#define BCN_PFB_HFBGRA		0x81
#define BCN_PFB_HFRGBX		0x82
#define BCN_PFB_HFBGRX		0x83

#define BCN_PFB_HFRGB		0x90
#define BCN_PFB_HFBGR		0x91

#define BCN_PFB_FRGBA		0xC0
#define BCN_PFB_FBGRA		0xC1
#define BCN_PFB_FRGBX		0xC2
#define BCN_PFB_FBGRX		0xC3

#define BCN_PFB_FRGB		0xD0
#define BCN_PFB_FBGR		0xD1


#define BCN_BCF_UNKNOWN		0x00
#define BCN_BCF_BC1			0x01
#define BCN_BCF_BC2			0x02
#define BCN_BCF_BC3			0x03
#define BCN_BCF_BC4			0x04
#define BCN_BCF_BC5			0x05
#define BCN_BCF_BC6			0x06
#define BCN_BCF_BC7			0x07


#ifndef BYTE_T
#define BYTE_T
typedef unsigned char byte;
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

// typedef unsigned short u16;
// typedef unsigned int u32;

typedef unsigned short ushort;
typedef unsigned int uint;

typedef struct {
byte *cs, *cse;
byte *ct, *cte;
u32 win;
int pos;
}BGBBTJ_BitStream;
