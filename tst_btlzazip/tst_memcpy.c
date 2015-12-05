#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>

#if defined(_WIN64) || (_M_IX86_FP>=2)
#include <xmmintrin.h>
#include <emmintrin.h>
#define HAVE_SSE2
#endif

#if defined(__x86_64__) || defined(_M_X64)
#ifndef X86_64
#define X86_64
#endif
#endif

#if defined(__i386__) || defined(_M_IX86)
#ifndef X86
#define X86
#endif
#endif

typedef unsigned char byte;
typedef unsigned int u32;
typedef signed long long s64;

void BTLZA_BitDec_MemCpy(byte *dst, byte *src, int len)
{
	byte *s, *t, *te;
#ifdef HAVE_SSE2
	__m128i d128i, d128j, d128k, d128l;
#endif
#ifdef X86_64
	s64 d;
#else
	int d;
#endif
	int i, j, k, l;

#if defined(X86) || defined(X86_64) || defined(ARM)
	s=src; t=dst; i=len;
	d=dst-src;
	te=dst+len;
#if 1
	if((d>=16) || (d<0))
	{
#ifdef HAVE_SSE2
// #if 0
#if 1
		if((i>=32) && ((d>=64) || (d<0)))
		{
			while(t<=(te-64))
			{
				d128i=_mm_loadu_si128((__m128i *)(s   ));
				d128j=_mm_loadu_si128((__m128i *)(s+16));
				d128k=_mm_loadu_si128((__m128i *)(s+32));
				d128l=_mm_loadu_si128((__m128i *)(s+48));
				_mm_storeu_si128((__m128i *)(t   ), d128i);
				_mm_storeu_si128((__m128i *)(t+16), d128j);
				_mm_storeu_si128((__m128i *)(t+32), d128k);
				_mm_storeu_si128((__m128i *)(t+48), d128l);
				t+=64; s+=64;
			}

			if(t<=(te-32))
			{
				d128i=_mm_loadu_si128((__m128i *)(s   ));
				d128j=_mm_loadu_si128((__m128i *)(s+16));
				_mm_storeu_si128((__m128i *)(t   ), d128i);
				_mm_storeu_si128((__m128i *)(t+16), d128j);
				t+=32; s+=32;
			}
		}
#endif
		while(t<=(te-16))
		{
			d128i=_mm_loadu_si128((__m128i *)s);
			_mm_storeu_si128((__m128i *)t, d128i);
			t+=16; s+=16;
		}
#else
		while(t<=(te-16))
		{
			((s64 *)t)[0]=((s64 *)s)[0];
			((s64 *)t)[1]=((s64 *)s)[1];
			t+=16; s+=16;
		}
#endif
		if(t<=(te-8))
		{
			*((s64 *)t)=*((s64 *)s);
			t+=8; s+=8;
		}
		if(t<=(te-4))
		{
			*((u32 *)t)=*((u32 *)s);
			t+=4; s+=4;
		}
		if(t<=(te-2))
		{
			*t++=*s++; *t++=*s++;
		}
		if(t<te)
			{ *t++=*s++; }
		return;
	}else if(d>=4)
	{
		while(i>=16)
		{
			((u32 *)t)[0]=((u32 *)s)[0];
			((u32 *)t)[1]=((u32 *)s)[1];
			((u32 *)t)[2]=((u32 *)s)[2];
			((u32 *)t)[3]=((u32 *)s)[3];
			t+=16; s+=16; i-=16;
		}
		if(i>=8)
		{
			((u32 *)t)[0]=((u32 *)s)[0];
			((u32 *)t)[1]=((u32 *)s)[1];
			t+=8; s+=8; i-=8;
		}
		if(i>=4)
			{ *((int *)t)=*((int *)s); t+=4; s+=4; i-=4; }
		if(i>=2)
			{ *t++=*s++; *t++=*s++; i-=2; }
		if(i) { *t++=*s++; }
		return;
	}else
	{
		while(i>=16)
		{
			t[ 0]=s[ 0];	t[ 1]=s[ 1];
			t[ 2]=s[ 2];	t[ 3]=s[ 3];
			t[ 4]=s[ 4];	t[ 5]=s[ 5];
			t[ 6]=s[ 6];	t[ 7]=s[ 7];
			t[ 8]=s[ 8];	t[ 9]=s[ 9];
			t[10]=s[10];	t[11]=s[11];
			t[12]=s[12];	t[13]=s[13];
			t[14]=s[14];	t[15]=s[15];
			t+=16; s+=16; i-=16;
		}
		if(i>=8)
		{
			t[ 0]=s[ 0];	t[ 1]=s[ 1];
			t[ 2]=s[ 2];	t[ 3]=s[ 3];
			t[ 4]=s[ 4];	t[ 5]=s[ 5];
			t[ 6]=s[ 6];	t[ 7]=s[ 7];
			t+=8; s+=8; i-=8;
		}
		if(i>=4)
		{
			t[ 0]=s[ 0];	t[ 1]=s[ 1];
			t[ 2]=s[ 2];	t[ 3]=s[ 3];
			t+=4; s+=4; i-=4;
		}
		if(i>=2)
		{
			*t++=*s++;	*t++=*s++;
			i-=2;
		}
		if(i)*t++=*s++;
//		while(i--)*t++=*s++;
		return;
	}
#endif
#else
	s=src; t=dst; i=len;
	while(i--)*t++=*s++;
#endif
}


int main()
{
	char *tbuf1, *tbuf2;
	char *ts1, *ts2;
	int t0, t1, t2;
	int i, j, k, n;
	
	tbuf1=malloc(1<<26);
	tbuf2=malloc(1<<26);
	
	ts1=tbuf1;
	ts2=tbuf2;
	
	t0=clock(); n=0;
	for(i=0; i<64; i++)
	{
		for(j=0; j<64; j++)
		{
			ts1=tbuf1+(j<<20);
			ts2=tbuf2+(j<<20);
//			memcpy(ts1, ts2, 1<<20);
//			memmove(ts1, ts2, 1<<20);
			BTLZA_BitDec_MemCpy(ts1, ts2, 1<<20);
			n++;
		}
		t1=clock();
		t2=t1-t0;
		printf("%.3fMB/sec\r", n/(t2/((double)CLOCKS_PER_SEC)));
	}
	printf("\n");
}