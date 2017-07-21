/*
FELZ32: LZ in terms of 32-bit DWORDs

tag:
  bits  0..15: md, match distance (DWORDs)
  bits 16..22: ml, match length (DWORDs)
  bits 23..29: rl, raw length (DWORDs)
  bits 30..31: al, tag/align

ml!=0:
  md=distance
  ml=length
  mr=raw
  al=align (0=DWORD aligned, others: byte offset)

ml==0, rl==0, al==0:
	md==0: EOB
	md!=0: long run of raw DWORDs


Standalone File Header:
	FOURCC magic0;	//'FeLZ'
	TWOCC magic1;	//'32'
	BYTE ver;		//1, format version
	BYTE resv;		//0, reserved
	DWORD csize;	//compressed size (includes header)
	DWORD dsize;	//decompressed size
 */

#ifdef __GUNC
#define HAS_STDINT
#define IS_GCC		//GCC (or GCC-like compiler)
#endif

#if _MSC_VER>=1600
#define HAS_STDINT
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>

#ifdef HAS_STDINT
#include <stdint.h>
#endif

#ifdef __linux
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mouse.h>
#include <SDL/SDL_keyboard.h>
#endif

#if defined(__linux) || defined(__EMSCRIPTEN__)
#include <sys/time.h>
#endif


#if defined(__x86_64__) || defined(_M_X64)
#ifndef X86_64
#define X86_64
#endif

#ifndef ARCH_64BIT
#define ARCH_64BIT
#endif
#endif

#if defined(__i386__) || defined(_M_IX86)
#ifndef X86
#define X86
#endif
#endif

#if defined(__arm__) || defined(_M_ARM)
#ifndef ARM
#define ARM
#endif
#endif

#ifdef __BIG_ENDIAN__
#define BIGENDIAN
#endif

#ifdef __LITTLE_ENDIAN__
#define LTLENDIAN
#endif

#if defined(X86) || defined(X86_64)
#ifndef LTLENDIAN
#define LTLENDIAN
#endif
#endif

#ifdef _MSC_VER
#define force_inline __forceinline
#define default_inline __inline
#endif

#ifndef __EMSCRIPTEN__
#ifdef __GNUC__
#define force_inline inline
#define default_inline inline
#endif
#endif

#ifndef force_inline
#define force_inline
#define default_inline
#endif

#ifdef HAS_STDINT

typedef uint8_t   byte;
typedef  int8_t  sbyte;
typedef uint16_t u16;
typedef  int16_t s16;
typedef uint32_t u32;
typedef  int32_t s32;
typedef uint64_t u64;
typedef  int64_t s64;

#else

typedef unsigned char byte;
typedef signed char sbyte;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
#if defined(__linux) && defined(__x86_64__)
typedef unsigned long u64;
typedef signed long s64;
#else
typedef unsigned long long u64;
typedef signed long long s64;
#endif

#endif

typedef struct BGBDT_FeLzContext_s BGBDT_FeLzContext;

struct BGBDT_FeLzContext_s {
u32 mark1;

byte use_lvm;		//use multi-level matching (log2)
byte use_lvb;		//enable byte matching

u32 *css;
u32 *cse;
//u32 lzhash[16384];
u32 lzhash[65536];
byte lzhrov[4096];
};

#if defined(X86) || defined(X86_64)

#if defined(IS_GCC) && defined(X86_64)
default_inline u32 bgbdt_felz_getu32(byte *ptr)
{
	u32 t;
	memcpy(&t, ptr, 4);
	return(t);
}

default_inline u64 bgbdt_felz_getu64(byte *ptr)
{
	u64 t;
	memcpy(&t, ptr, 8);
	return(t);
}

default_inline void bgbdt_felz_setu32(byte *ptr, u32 val)
	{ memcpy(ptr, &val, 4); }
default_inline void bgbdt_felz_setu64(byte *ptr, u64 val)
	{ memcpy(ptr, &val, 8); }

#define felz_getu32(ptr)			(bgbdt_felz_getu32(ptr))
#define felz_getu32le(ptr)			(bgbdt_felz_getu32(ptr))
#define felz_setu32le(ptr, val)		(bgbdt_felz_setu32(ptr, val))

#define felz_getu32lea(ptr)			(*(u32 *)(ptr))
#define felz_setu32lea(ptr, val)	(*(u32 *)(ptr)=(val))

#define felz_getu64(ptr)			(bgbdt_felz_getu64(ptr))
#define felz_getu64le(ptr)			(bgbdt_felz_getu64(ptr))
#define felz_setu64le(ptr, val)		(bgbdt_felz_setu64(ptr, val))

#else

#define felz_getu32(ptr)			(*(u32 *)(ptr))
#define felz_getu32le(ptr)			(*(u32 *)(ptr))
#define felz_setu32le(ptr, val)		(*(u32 *)(ptr)=(val))

#define felz_getu32lea(ptr)			(*(u32 *)(ptr))
#define felz_setu32lea(ptr, val)	(*(u32 *)(ptr)=(val))

#define felz_getu64(ptr)			(*(u64 *)(ptr))
#define felz_getu64le(ptr)			(*(u64 *)(ptr))
#define felz_setu64le(ptr, val)		(*(u64 *)(ptr)=(val))

#endif

#else

default_inline u32 bgbdt_felz_getu32(byte *ptr)
{
	u32 v;
#ifdef IS_GCC
	memcpy(&v, ptr, 4);
#else
#if defined(BIGENDIAN)
	v=ptr[3]|(ptr[2]<<8)|(ptr[1]<<16)|(ptr[0]<<24);
#elif defined(LTLENDIAN)
	v=ptr[0]|(ptr[1]<<8)|(ptr[2]<<16)|(ptr[3]<<24);
#else
	static int tv=1;
	if(*(byte *)(&tv)==1)
		{ v=ptr[0]|(ptr[1]<<8)|(ptr[2]<<16)|(ptr[3]<<24); }
	else
		{ v=ptr[3]|(ptr[2]<<8)|(ptr[1]<<16)|(ptr[0]<<24); }
#endif
#endif
	return(v);
}

default_inline u32 bgbdt_felz_getu32le(byte *ptr)
{
	u32 v;
#if defined(IS_GCC) && defined(LTLENDIAN)
	memcpy(&v, ptr, 4);
#else
	v=ptr[0]|(ptr[1]<<8)|(ptr[2]<<16)|(ptr[3]<<24);
#endif
	return(v);
}

default_inline void bgbdt_felz_setu32le(byte *ptr, u32 val)
{
#if defined(IS_GCC) && defined(LTLENDIAN)
	memcpy(ptr, &val, 4);
#else
	ptr[0]=val;		ptr[1]=val>>8;
	ptr[2]=val>>16;	ptr[3]=val>>24;
#endif
}

#define felz_getu32(ptr)			bgbdt_felz_getu32((byte *)(ptr))
#define felz_getu32le(ptr)			bgbdt_felz_getu32le((byte *)(ptr))
#define felz_setu32le(ptr, val)		bgbdt_felz_setu32le((byte *)(ptr), (val))

#define felz_getu32a(ptr)			(*(u32 *)(ptr))

#if defined(LTLENDIAN)
#define felz_getu32lea(ptr)			(*(u32 *)(ptr))
#define felz_setu32lea(ptr, val)	(*(u32 *)(ptr)=(val))
#elif defined(BIGENDIAN)
default_inline u32 bgbdt_felz_bswap32(u32 v)
{
	v=((v>> 8)&0x00FF00FF)|((v<< 8)&0xFF00FF00);
//	v=((v>>16)&0x0000FFFF)|((v<<16)&0xFFFF0000);
	v=((v>>16)           )|((v<<16)           );
	return(v);
}

#define felz_getu32lea(ptr)			bgbdt_felz_bswap32(*(u32 *)(ptr))
#define felz_setu32lea(ptr, val)	(*(u32 *)(ptr)=bgbdt_felz_bswap32(val))
#else
#define felz_getu32lea(ptr)			bgbdt_felz_getu32le((byte *)(ptr))
#define felz_setu32lea(ptr, val)	bgbdt_felz_setu32le((byte *)(ptr), (val))
#endif

#endif

int FRGL_TimeMS()
{
#ifdef _WIN32
	static unsigned int init;
	unsigned int t;

	t=timeGetTime();
	if(!init)init=t;

	return((unsigned int)(t-init));
#elif defined(__EMSCRIPTEN__) || defined(linux)
	struct timeval	tp;
	static int      secbase; 

	gettimeofday(&tp, NULL);  
	if(!secbase)secbase=tp.tv_sec;
	return(((tp.tv_sec-secbase)*1000)+tp.tv_usec/1000);
#else
	static int init;
	int t;

	t=clock();
	t*=CLOCKS_PER_SEC/1000.0;
	if(!init)init=t;
	return((unsigned int)(t-init));
#endif
}


int BGBDT_FeLz32_LookupMatchB(BGBDT_FeLzContext *ctx,
	u32 *cs, u32 *css, u32 *cse, int *rml, int *rmd);

int BGBDT_FeLz32_LookupMatchA(BGBDT_FeLzContext *ctx,
	u32 *cs, u32 *css, u32 *cse, int *rml, int *rmd)
{
	u32 *s, *se, *t, *te, *ct;
	byte *ctb, *tb;
	u32 v0, v, h, h1, o, o1, co;
	int hr, hm, sd, rt;
	int l, d, al, bl, bd, bal;
	int i, j, k;

	if(ctx->use_lvb)
	{
		v0=*cs;
		h=((v0*65521)>>16)&4095;
		hr=ctx->lzhrov[h];
		h1=h<<ctx->use_lvm;
		sd=1<<ctx->use_lvm;
		hm=sd-1;

		bl=0; bd=0;

		co=cs-css;

#if 0
		for(i=0; i<sd; i++)
		{
			o=ctx->lzhash[h1|((hr+i)&hm)]^v0;
			if(o&3)
				continue;

			o1=o>>2;
			if((o1>=co) || ((co-o1)>>16))
				continue;

			ct=css+o1;
			ctb=(byte *)ct;
			al=0;

			s=cs; se=s+126;
			if(cse<se)se=cse;
			tb=ctb;

			while(s<se)
			{
				v=felz_getu32(tb);
				if(*s!=v)
					break;
				s++; tb+=4;
			}
			
			l=s-cs;
			d=cs-ct;
			
			if((l>bl) && (d<65536))
				{ bl=l; bd=d; bal=al; }
		}
#endif

		for(i=0; i<sd; i++)
		{
			o=ctx->lzhash[h1|((hr+i)&hm)]^v0;

			o1=o>>2;
			if((o1>=co) || ((co-o1)>>16))
				continue;

			ct=css+o1;
			ctb=((byte *)css)+o;
			al=o&3;

			s=cs; se=s+126;
			if(cse<se)se=cse;
			tb=ctb;

			while(s<se)
			{
				v=felz_getu32(tb);
				if(*s!=v)
					break;
				s++; tb+=4;
			}
			
			l=s-cs;
			d=cs-ct;
			
			if((l>bl) && (d<65536))
				{ bl=l; bd=d; bal=al; }
		}
		
		rt=(bl>1) && (bd<65536);

		if(!rt)
		{
			hr=(hr-1)&hm;
			ctx->lzhrov[h]=hr;
			ctx->lzhash[h1|hr]=co^v0;
		}

		*rml=bl|(bal<<14);
		*rmd=bd;
		return(rt);
	}

	if(ctx->use_lvm)
	{
		v0=*cs;
		h=((v0*65521)>>16)&4095;
		hr=ctx->lzhrov[h];
		h1=h<<ctx->use_lvm;
		sd=1<<ctx->use_lvm;
		hm=sd-1;

		co=cs-css;

		bl=0; bd=0;
		for(i=0; i<sd; i++)
		{
			o=ctx->lzhash[h1|((hr+i)&hm)]^v0;
			if((o>=co) || ((co-o)>>16))
				continue;

			ct=ctx->css+o;

			s=cs; se=s+126;
			if(cse<se)se=cse;
			t=ct;

			while(s<se)
			{
				if(*s!=*t)
					break;
				s++; t++;
			}
			
			l=s-cs;
			d=cs-ct;
			
			if((l>bl) && (d<65536))
				{ bl=l; bd=d; }
		}
		
		rt=(bl>1) && (bd<65536);

		if(!rt)
		{
			hr=(hr-1)&hm;
			ctx->lzhrov[h]=hr;
			ctx->lzhash[h1|hr]=co^v0;
		}

		*rml=bl;
		*rmd=bd;
		return(rt);
	}
	
	return(BGBDT_FeLz32_LookupMatchB(ctx, cs, css, cse, rml, rmd));
}

int BGBDT_FeLz32_UpdateStringB(BGBDT_FeLzContext *ctx,
	u32 *cs, u32 *css, u32 *cse, int len);

int BGBDT_FeLz32_UpdateStringA(BGBDT_FeLzContext *ctx,
	u32 *cs, u32 *css, u32 *cse, int len)
{
	byte *csb;
	u32 v;
	int i, h, h1, hr, hm;

	if(ctx->use_lvb)
	{
		hm=(1<<ctx->use_lvm)-1;

		csb=(byte *)cs;
		i=len*4;
		while(i--)
		{
			v=felz_getu32(csb);
			h=((v*65521)>>16)&4095;
			h1=h<<ctx->use_lvm;

			hr=ctx->lzhrov[h];
			hr=(hr-1)&hm;
			ctx->lzhrov[h]=hr;
			ctx->lzhash[h1|hr]=(csb-((byte *)ctx->css))^v;
			csb++;
		}

		return(0);
	}

	if(ctx->use_lvm)
	{
		hm=(1<<ctx->use_lvm)-1;

		i=len;
		while(i--)
		{
			v=*cs;
			h=((v*65521)>>16)&4095;
			h1=h<<ctx->use_lvm;

			hr=ctx->lzhrov[h];
			hr=(hr-1)&hm;
			ctx->lzhrov[h]=hr;
			ctx->lzhash[h1|hr]=(cs-ctx->css)^v;
			cs++;
		}

		return(0);
	}

	return(BGBDT_FeLz32_UpdateStringB(ctx, cs, css, cse, len));
}


int BGBDT_FeLz32_LookupMatchB(BGBDT_FeLzContext *ctx,
	u32 *cs, u32 *css, u32 *cse, int *rml, int *rmd)
{
	u32 *s, *se, *t, *te, *ct;
	u32 v0, v, h, h1, o, co;
	int l, d;
	int i, j, k;

	v0=*cs;

//	h=(v0^(v0>>10)^(v0>>20))&1023;
//	h=(v0^(v0>>16))&1023;

//	h=((v0*65521)>>16)&255;
//	h=((v0*65521)>>16)&1023;
	h=((v0*65521)>>16)&2047;
//	h=((v0*65521)>>16)&4095;
//	h=((v0*65521)>>16)&16383;
//	h=((v0*65521)>>16)&65535;

	o=ctx->lzhash[h]^v0;
	co=cs-css;

	ctx->lzhash[h]=co^v0;

	if((o>=co) || ((co-o)>>16))
//	if((o>=co) || ((co-o)>>13))
//	if(((co-o)>>13)|(o>=co))
	{
		*rml=0; *rmd=0;
		return(0);
	}

	ct=css+o;
	
	s=cs; se=s+126;
	if(cse<se)se=cse;
	t=ct;

	while(s<se)
	{
		if(*s!=*t)
			break;
		s++; t++;
	}
	
	l=s-cs;
	d=cs-ct;
	*rml=l;
	*rmd=d;
	return((l>1) && (d<65536));
}

int BGBDT_FeLz32_UpdateStringB(BGBDT_FeLzContext *ctx,
	u32 *cs, u32 *css, u32 *cse, int len)
{
	byte *csb;
	u32 v;
	int i, h, h1, hr, hm;

//	return(0);

	i=len;
	while(i--)
	{
		v=*cs;
//		h=((v*65521)>>16)&255;
		h=((v*65521)>>16)&1023;
//		h=((v*65521)>>16)&4095;
//		h=((v*65521)>>16)&16383;
//		h=((v*65521)>>16)&65535;
		ctx->lzhash[h]=(cs-css)^v;
		cs++;
	}
	return(0);
}

int BGBDT_FeLz32_UpdateStringNop(BGBDT_FeLzContext *ctx,
	u32 *cs, u32 *css, u32 *cse, int len)
{
	return(0);
}

int BGBDT_FeLz32_EncodeBufferCtx(BGBDT_FeLzContext *ctx,
	u32 *ibuf, int ibsz, u32 *obuf, int obsz, int lvl)
{
	int (*LookupMatch)(BGBDT_FeLzContext *ctx,
		u32 *cs, u32 *css, u32 *cse, int *rml, int *rmd);
	int (*UpdateString)(BGBDT_FeLzContext *ctx,
		u32 *cs, u32 *css, u32 *cse, int len);

	u32 *cs, *cse, *csrb;
	u32 *ct, *cte;
	u32 tg;
	int ml, md, rl, al;
	int i, j, k;

	LookupMatch=BGBDT_FeLz32_LookupMatchA;
	UpdateString=BGBDT_FeLz32_UpdateStringA;

	switch(lvl)
	{
	case 0:
	case 1:
		LookupMatch=BGBDT_FeLz32_LookupMatchB;
//		UpdateString=BGBDT_FeLz32_UpdateStringB;
		UpdateString=BGBDT_FeLz32_UpdateStringNop;
		ctx->use_lvb=0;
		ctx->use_lvm=0;
		break;
	case 2:
		ctx->use_lvb=0;
		ctx->use_lvm=1;
		break;
	case 3:
		ctx->use_lvb=0;
		ctx->use_lvm=2;
		break;
	case 4:
		ctx->use_lvb=0;
		ctx->use_lvm=3;
		break;
	case 5:
		ctx->use_lvb=0;
		ctx->use_lvm=4;
		break;

	case 6:
		ctx->use_lvb=1;
		ctx->use_lvm=0;
		break;
	case 7:
		ctx->use_lvb=1;
		ctx->use_lvm=2;
		break;
	case 8:
		ctx->use_lvb=1;
		ctx->use_lvm=3;
		break;
	case 9:
		ctx->use_lvb=1;
		ctx->use_lvm=4;
		break;
	}

	if(ctx->use_lvm)
	{
		ml=4096<<ctx->use_lvm;
		for(i=0; i<ml; i++)
			{ ctx->lzhash[i]=0; }
		for(i=0; i<4096; i++)
			{ ctx->lzhrov[i]=0; }
	}else
	{
		for(i=0; i<1024; i++)
			{ ctx->lzhash[i]=0; }
	}
		
	ctx->css=ibuf;
	ctx->cse=ibuf+ibsz;
	
	cs=ibuf; cse=cs+ibsz;
	ct=obuf; cte=ct+obsz;
	csrb=cs;
	
	while(cs<cse)
	{	
		if(LookupMatch(ctx, cs, ibuf, cse, &ml, &md))
		{
			rl=cs-csrb;
			while(rl>=127)
			{
				if(rl>=65536)
					rl=65535;
				felz_setu32lea(ct, rl);
				ct++;
				i=rl;
//				while(i>=4)
				while((csrb+4)<=cs)
				{
					ct[0]=csrb[0];	ct[1]=csrb[1];
					ct[2]=csrb[2];	ct[3]=csrb[3];
					ct+=4; csrb+=4;
				}
//				while(i--)
				while(csrb<cs)
					{ *ct++=*csrb++; }
				rl=cs-csrb;
			}
			
			tg=md|(ml<<16)|(rl<<23);
			ml&=16383;

			felz_setu32le(ct, tg);
			ct++;
			i=rl;
			while((csrb+4)<=cs)
			{
				ct[0]=csrb[0];	ct[1]=csrb[1];
				ct[2]=csrb[2];	ct[3]=csrb[3];
				ct+=4; csrb+=4;
			}
			while(csrb<cs)
				{ *ct++=*csrb++; }
			if(lvl)
				UpdateString(ctx, cs, ibuf, cse, ml);
			cs+=ml;
			csrb=cs;
		}else
		{
//			UpdateString(ctx, cs, ibuf, cse, 1);
			cs++;
		}
	}

	rl=cs-csrb;
	while(rl>0)
	{
		if(rl>=65536)
			rl=65535;
		felz_setu32lea(ct, rl);
		ct++;
		i=rl;
		while(i--)
			{ *ct++=*csrb++; }
		rl=cs-csrb;
	}
	
	*ct++=0;
	
	return(ct-obuf);
}

int BGBDT_FeLz32_EncodeBuffer(
	u32 *ibuf, int ibsz, u32 *obuf, int obsz, int lvl)
{
	BGBDT_FeLzContext tctx;
	return(BGBDT_FeLz32_EncodeBufferCtx(&tctx, ibuf, ibsz, obuf, obsz, lvl));
}

force_inline void felz_memcpy_u32a(u32 *dst, u32 *src, int num)
{
	u32 *cs, *ct, *cte;

#if defined(X86_64) && !defined(__GNUC__)
	cs=src; ct=dst; cte=ct+num;
	while(ct<cte)
	{
		((u64 *)ct)[0]=((u64 *)cs)[0];
		((u64 *)ct)[1]=((u64 *)cs)[1];
		ct+=4; cs+=4;
	}
#else
	cs=src; ct=dst; cte=ct+num;
	while(ct<cte)
	{
		ct[0]=cs[0];	ct[1]=cs[1];
		ct[2]=cs[2];	ct[3]=cs[3];
		ct+=4; cs+=4;
	}
#endif
}

force_inline void felz_memset_u32a(u32 *dst, u32 val, int num)
{
	u32 *ct, *cte;
	
#if defined(X86_64) && !defined(__GNUC__)
	u64 lv;

	lv=(((u64)val)<<32)|val;
	ct=dst; cte=ct+num;
	while(ct<cte)
	{
		((u64 *)ct)[0]=lv;
		((u64 *)ct)[1]=lv;
		ct+=4;
	}
#else
	ct=dst; cte=ct+num;
	while(ct<cte)
	{
		ct[0]=val;	ct[1]=val;
		ct[2]=val;	ct[3]=val;
		ct+=4;
	}
#endif
}

force_inline void felz_memset3_u32a(u32 *dst, u32 v0, u32 v1, u32 v2, int num)
{
	u32 *ct, *cte;
	
	ct=dst; cte=ct+num;
	while(ct<cte)
		{ ct[0]=v0; ct[1]=v1; ct[2]=v2; ct+=3; }
}

int BGBDT_FeLz32_DecodeBuffer(
	u32 *ibuf, int ibsz, u32 *obuf, int obsz)
{
	u32 *cs, *cse, *cs1;
	u32 *ct, *ct1, *cte;
	byte *cs1b;
	u64 lv;
	u32 tg, v, v0, v1, v2;
	int ml, md, rl, al;
	int i, j, k;

	cs=ibuf; cse=cs+ibsz;
	ct=obuf; cte=ct+obsz;

	while(cs<cse)
	{
		tg=felz_getu32lea(cs); cs++;

		ml=(tg>>16)&127;
		md=tg&65535;

		if(ml)
		{
			rl=(tg>>23)&127;
			al=(tg>>30)&3;

			if(rl)
			{
				i=rl;
				felz_memcpy_u32a(ct, cs, rl);
				ct+=rl;		cs+=rl;
			}

			if(al)
			{
				cs1b=((byte *)(ct-md))+al;
				if(md<2)
				{
#if defined(LTLENDIAN) || defined(BIGENDIAN)
					switch(al)
					{
					case 3:
						v=cs1b[0]; v=v|(v<<8); v=v|(v<<16);
						felz_memset_u32a(ct, v, ml);
						ct+=ml;
						break;
					case 2:
#ifdef LTLENDIAN
						v=cs1b[0]|(cs1b[1]<<8); v=v|(v<<16);
#else
						v=cs1b[1]|(cs1b[0]<<8); v=v|(v<<16);
#endif
						felz_memset_u32a(ct, v, ml);
						ct+=ml;
						break;
					case 1:
#ifdef LTLENDIAN
						v=cs1b[0]|(cs1b[1]<<8)|(cs1b[2]<<16);
						v0=v|(v<<24);
						v1=(v>>8)|(v<<16);
						v2=(v>>16)|(v<<8);
#else
						v=cs1b[2]|(cs1b[1]<<8)|(cs1b[0]<<16);
						v0=(v<< 8)|(v>>16);
						v1=(v<<16)|(v>> 8);
						v2=(v<<24)|(v    );
#endif
						felz_memset3_u32a(ct, v0, v1, v2, ml);
						ct+=ml;
						break;
					default:
						break;
					}
#else
					i=ml;
					while(i--)
					{
						((byte *)ct)[0]=cs1b[0];
						((byte *)ct)[1]=cs1b[1];
						((byte *)ct)[2]=cs1b[2];
						((byte *)ct)[3]=cs1b[3];
						cs1b+=4;
						ct++;
					}
#endif
				}else
				{
#if defined(LTLENDIAN) || defined(BIGENDIAN)
					ct1=ct; ct+=ml;
					while(ct1<ct)
					{
						ct1[0]=felz_getu32(cs1b+ 0);
						ct1[1]=felz_getu32(cs1b+ 4);
						ct1[2]=felz_getu32(cs1b+ 8);
						ct1[3]=felz_getu32(cs1b+12);
						cs1b+=16; ct1+=4;
					}
#else
					i=ml;
					while(i--)
					{
						((byte *)ct)[0]=cs1b[0];
						((byte *)ct)[1]=cs1b[1];
						((byte *)ct)[2]=cs1b[2];
						((byte *)ct)[3]=cs1b[3];
						cs1b+=4; ct++;
					}
#endif
				}
			}else
			{
				cs1=ct-md;
				if(md>1)
				{
					felz_memcpy_u32a(ct, cs1, ml);
					ct+=ml;
				}else
				{
					felz_memset_u32a(ct, *cs1, ml);
					ct+=ml;
				}
			}
			continue;
		}
		
		if(!tg)
			break;
		
		if(!(tg&0xFFFF0000U))
		{
			felz_memcpy_u32a(ct, cs, md);
			ct+=md;		cs+=md;
			continue;
		}
		
//		FRGL_DBGBREAK_SOFT
	}
	
	return(ct-obuf);
}

byte *loadfile(char *path, int *rsz)
{
	byte *buf;
	FILE *fd;
	int sz, i;
	
	fd=fopen(path, "rb");
	if(!fd)
		return(NULL);

	fseek(fd, 0, 2);
	sz=ftell(fd);
	fseek(fd, 0, 0);
	buf=malloc(sz);
	i=fread(buf, 1, sz, fd);
	fclose(fd);
	
	*rsz=sz;
	return(buf);
}

int storefile(char *name, byte *ibuf, int isz)
{
	byte *tbuf;
	FILE *fd;
	int sz;

	fd=fopen(name, "wb");
	if(!fd)
	{
		printf("Fail Open Write %s\n", name);
		return(-1);
	}

	fwrite(ibuf, 1, isz, fd);
	fclose(fd);
	return(0);
}

int felz_memcpy32(void *dst, void *src, int len)
{
	u32 *cs, *cse, *ct;
	
	cs=src; cse=cs+(len+3)/4;
	ct=dst;
	
	while(cs<cse)
	{
//		*ct++=*cs++;
//		*ct++=*cs++;

		ct[0]=cs[0];
		ct[1]=cs[1];
		ct[2]=cs[2];
		ct[3]=cs[3];
//		ct+=2; cs+=2;
		ct+=4; cs+=4;
	}
	return(0);
}

int help(char *prg)
{
	printf("usage: %s opts* infile outfile\n", prg);
	printf("\t-d Decode\n");
	printf("\t-1 .. -9 Compression Level\n");
	return(0);
}

int main(int argc, char *argv[])
{
	byte *ibuf, *obuf, *tbuf;
	byte *cs, *cse;
	char *ifn, *ofn;
	byte dec, lvl;
	s64 li0;
	double f, g;
	int t0, t1, t2, t3;
	int bd0, bl0, bd1, bl1;
	int bl, bd;
	int sz, csz, dsz, h, rb;
	int i, j, k;
	
	ifn=NULL; ofn=NULL; dec=0; lvl=1;
	for(i=1; i<argc; i++)
	{
		if(argv[i][0]=='-')
		{
			if(!strcmp(argv[i], "-d"))
				{ dec=1; continue; }
			if(!strcmp(argv[i], "-t"))
				{ dec=2; continue; }

			if(!strcmp(argv[i], "-1"))
				{ lvl=1; continue; }
			if(!strcmp(argv[i], "-2"))
				{ lvl=2; continue; }
			if(!strcmp(argv[i], "-3"))
				{ lvl=3; continue; }
			if(!strcmp(argv[i], "-4"))
				{ lvl=4; continue; }
			if(!strcmp(argv[i], "-5"))
				{ lvl=5; continue; }
			if(!strcmp(argv[i], "-6"))
				{ lvl=6; continue; }
			if(!strcmp(argv[i], "-7"))
				{ lvl=7; continue; }
			if(!strcmp(argv[i], "-8"))
				{ lvl=8; continue; }
			if(!strcmp(argv[i], "-9"))
				{ lvl=9; continue; }
		
			continue;
		}
		if(!ifn)
			{ ifn=argv[i]; continue; }
		if(!ofn)
			{ ofn=argv[i]; continue; }
	}

	if(!ifn || !ofn)
	{
		help(argv[0]);
		return(-1);
	}
	
	if(dec==0)
	{
		ibuf=loadfile(ifn, &sz);
		obuf=malloc(sz*2);

		i=BGBDT_FeLz32_EncodeBuffer(
			(u32 *)ibuf, (sz+3)/4,
			(u32 *)(obuf+16), (2*sz+3)/4, lvl);
		csz=i*4+16;
		obuf[0]='F';	obuf[1]='e';
		obuf[2]='L';	obuf[3]='Z';
		obuf[4]='3';	obuf[5]='2';
		obuf[6]=1;		obuf[7]=0;
		felz_setu32le(obuf+ 8, csz);
		felz_setu32le(obuf+12, sz);
		
		printf("Enc %d -> %d (%.2f%%)\n", sz, csz, (100.0*csz)/(sz+1));
		
		if(ofn)
		{
			storefile(ofn, obuf, csz);
		}
		
		return(0);
	}

	if(dec==1)
	{
		ibuf=loadfile(ifn, &csz);

		if(memcmp(ibuf, "FeLZ32", 6) || (ibuf[6]!=1) || ibuf[7])
		{
			printf("FeLZ Magic Failed\n");
			return(-1);
		}

		csz=felz_getu32le(ibuf+ 8);
		dsz=felz_getu32le(ibuf+12);
		obuf=malloc(dsz+4096);
		
		BGBDT_FeLz32_DecodeBuffer(
			(u32 *)(ibuf+16), ((csz-16)+3)/4,
			(u32 *)(obuf), (dsz+3)/4);
		storefile(ofn, obuf, dsz);

		return(0);
	}

	if(dec==2)
	{
		ibuf=loadfile(ifn, &sz);
		obuf=malloc(sz*2);
		tbuf=malloc(sz*2);
		
		dsz=sz;

		i=BGBDT_FeLz32_EncodeBuffer(
			(u32 *)ibuf, (sz+3)/4,
			(u32 *)(obuf+16), (2*sz+3)/4, lvl);
		csz=i*4+16;
		obuf[0]='F';	obuf[1]='e';
		obuf[2]='L';	obuf[3]='Z';
		obuf[4]='3';	obuf[5]='2';
		obuf[6]=1;		obuf[7]=0;
		felz_setu32le(obuf+ 8, csz);
		felz_setu32le(obuf+12, sz);
		
		printf("Test %d -> %d (%.2f%%)\n", sz, csz, (100.0*csz)/(sz+1));
		
		if(ofn)
		{
			storefile(ofn, obuf, csz);
		}
		
//		printf("A0A\n");

		i=BGBDT_FeLz32_DecodeBuffer(
			(u32 *)(obuf+16), ((csz-16)+3)/4,
			(u32 *)(tbuf), (dsz+3)/4);

//		printf("A0B\n");
		
		if(i!=((dsz+3)/4))
		{
			printf("Size Mismatch %d -> %d\n", dsz, i*4);
		}
		
		if(memcmp(ibuf, tbuf, dsz))
		{
			printf("Data Mismatch\n");
			return(-1);
		}

		t0=FRGL_TimeMS();
		t2=t0+1000;
		t1=t0;

		printf("Encode:\n");
		li0=0;
		while((t1<t2) && (t1>=t0))
		{
			j=(sz+3)/4;
			i=BGBDT_FeLz32_EncodeBuffer(
				(u32 *)ibuf, j,
				(u32 *)(obuf+16), (2*sz+3)/4, lvl);
			li0+=j;

			f=(t1-t0)/(1000.0);
			g=(4*li0)/(f*1000000.0);
			printf("%.2fs %.2fMB/s  \r", f, g);
			t1=FRGL_TimeMS();
		}

		printf("\n");
		
		t0=FRGL_TimeMS();
		t2=t0+1000;
		t1=t0;
		
//		printf("A1\n");
		
		printf("Decode:\n");
		li0=0;
		while((t1<t2) && (t1>=t0))
		{
			i=BGBDT_FeLz32_DecodeBuffer(
				(u32 *)(obuf+16), ((csz-16)+3)/4,
				(u32 *)(tbuf), (dsz+3)/4);
			li0+=i;

			f=(t1-t0)/(1000.0);
			g=(4*li0)/(f*1000000.0);
			printf("%.2fs %.2fMB/s  \r", f, g);
			t1=FRGL_TimeMS();
		}

//		printf("A2\n");

		printf("\n");
		
		t0=FRGL_TimeMS();
		t2=t0+1000;
		t1=t0;
		printf("Memcpy:\n");
		li0=0;
		while((t1<t2) && (t1>=t0))
		{
			j=(dsz+3)/4;
//			memcpy(tbuf, ibuf, j*4);
			felz_memcpy32(tbuf, ibuf, j*4);
//			i=BGBDT_FeLz32_EncodeBuffer(
//				(u32 *)ibuf, j,
//				(u32 *)(obuf+16), (2*sz+3)/4, lvl);
			li0+=j;

			f=(t1-t0)/(1000.0);
			g=(4*li0)/(f*1000000.0);
			printf("%.2fs %.2fMB/s  \r", f, g);
			t1=FRGL_TimeMS();
		}
		printf("\n");

		return(0);
	}
}
