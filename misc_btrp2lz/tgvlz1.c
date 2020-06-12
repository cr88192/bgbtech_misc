/*
BtRP2 is a yet another byte-based LZ77 variant.

Its design takes some inspiration from the EA RefPack format, but changes were made in an effort to improve compression ratio and decoder performance. Another goal is to try to avoid becomming excessively complicated.


BtRP2 (Transposed, LE):
*                   dddddddd-dlllrrr0	(l=3..10, d=0..511, r=0..7)
*          dddddddd-dddddlll-lllrrr01	(l=4..67, d=0..8191)
* dddddddd-dddddddd-dlllllll-llrrr011	(l=4..515, d=0..131071)
*                            rrrr0111	(Raw Bytes, r=(r+1)*8, 8..128)
*                          * rrr01111	(Long Match)
*                            rr011111	(r=1..3 bytes, 0=EOB)
*                   rrrrrrrr-r0111111 	(Long Raw, r=(r+1)*8, 8..4096)
** d: Distance
** l: Match Length
** r: Literal Length

Values are encoded in little-endian order, with tag bits located in the LSB. Bits will be contiguous within the value, with shift-and-mask being used to extract individual elements.

Long Match will encode length and distance using variable-length encodings directly following the initial tag byte.

Length VLN:
          lllllll0,   4..  131
 llllllll-llllll01, 132..16383

Distance VLN:
          dddddddd-ddddddd0, 32K (0..32767)
 dddddddd-dddddddd-dddddd01,  4M

While there are both more space efficient and faster ways to handle Length/Distance VLNs (such as via a combined encoding), this encoding is "reasonable" and the Long Match case appears to be relatively less common. The above encoding can be trivially extended to support larger values.

Note the lack of length/distance predictors:
While predictors can be useful, their effectiveness in byte-oriented encoders is limited, and supporting these cases tends to have a detrimental effect on performance (they make more sense in Entropy-coded designs).


Simple File Format:
*  0: 'RP2A'
*  4: Compressed Size
*  8: Uncompressed Size
* 12: Checksum
* Compressed data starts at 16.

This format will not attempt to deal with chunking or streaming.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>

typedef unsigned char byte;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef struct TgvLz_Context_s TgvLz_Context;

//#define TKELZ_HASH_SZ	1024
#define TKELZ_HASH_SZ	4096
#define TKELZ_HASH_DN	64
#define TKELZ_HASH_DN1	63

#define TKELZ_CHHASH_SZ	16384
#define TKELZ_CHAIN_SZ	65536

#ifdef _MSC_VER
#define force_inline	__forceinline
#define	debug_break		__debugbreak();
#else
#define force_inline	
#define	debug_break		
#endif

struct TgvLz_Context_s {
byte *hash[TKELZ_HASH_SZ*TKELZ_HASH_DN];
byte hrov[TKELZ_HASH_SZ];
byte *cs;
byte *ct;
int maxlen;
int maxdist;
u32	csum;

byte	*chn_base;
byte	*chn_ptrs[TKELZ_CHAIN_SZ];
u16		chn_next[TKELZ_CHAIN_SZ];
u16		chn_hash[TKELZ_CHHASH_SZ];

char *tstName;
int (*EncodeBuffer)(TgvLz_Context *ctx,
	byte *ibuf, byte *obuf, int ibsz, int obsz);
int (*DecodeBuffer)(
	byte *ibuf, byte *obuf, int ibsz, int obsz);

int stat_len[24];
int stat_dist[24];
int stat_rlen[24];

byte *pred_cs1;
byte *pred_cs2;
int pred_l1, pred_d1;
int pred_l2, pred_d2;
};

byte *TgvLz_LoadFile(char *name, int *rsz)
{
	byte *buf;
	FILE *fd;
	int sz;
	
	fd=fopen(name, "rb");
	if(!fd)
	{
		return(NULL);
	}
	fseek(fd, 0, 2);
	sz=ftell(fd);
	fseek(fd, 0, 0);
	buf=malloc(sz+24);
	fread(buf, 1, sz, fd);
	fclose(fd);
	
	*(u64 *)(buf+sz+0)=0;
	*(u64 *)(buf+sz+8)=0;
	
	*rsz=sz;
	return(buf);
}

int TgvLz_StoreFile(char *name, void *buf, int sz)
{
	FILE *fd;
	
	fd=fopen(name, "wb");
	if(!fd)
		return(-1);
	
	fwrite(buf, 1, sz, fd);
	fclose(fd);
	return(0);
}

int TgvLz_CalcHash(byte *cs)
{
	int h;
	
	h=*(u32 *)cs;
	h=((h*4093)>>12)&(TKELZ_HASH_SZ-1);
	return(h);
}

int TgvLz_CalcHashB(byte *cs)
{
	int h;
	
	h=*(u32 *)cs;
	h^=h>>16;
	h=((h*4093)>>12)&(TKELZ_CHHASH_SZ-1);
	return(h);
}

int TgvLz_CheckMatch(byte *s0, byte *s1, byte *cse)
{
	byte *cs, *ct;
	
	cs=s0; ct=s1;
	while((cs+4)<=cse)
	{
		if(*(u32 *)cs!=*(u32 *)ct)
			break;
		cs+=4; ct+=4;
	}
	while(cs<cse)
	{
		if(*cs!=*ct)
			break;
		cs++; ct++;
	}
	
	return(cs-s0);
}

int TgvLz_HashByte(TgvLz_Context *ctx, byte *cs)
{
	int h, hr, rv;
	int i;

	rv=(cs-ctx->chn_base)&(TKELZ_CHAIN_SZ-1);

	h=TgvLz_CalcHash(cs);
	
	hr=ctx->hrov[h];
	hr=(hr-1)&TKELZ_HASH_DN1;
	ctx->hrov[h]=hr;
	ctx->hash[(h*TKELZ_HASH_DN)+hr]=cs;

	h=TgvLz_CalcHashB(cs);

	ctx->chn_ptrs[rv]=cs;
	ctx->chn_next[rv]=ctx->chn_hash[h];
	ctx->chn_hash[h]=rv;

	return(0);
}

int TgvLz_HashStr(TgvLz_Context *ctx, byte *cs, int l)
{
	byte *cse;
	
	cse=cs+l;
	while(cs<cse)
	{
		TgvLz_HashByte(ctx, cs);
		cs++;
	}
	return(0);
}

int TgvLz_LookupMatch(TgvLz_Context *ctx,
	byte *str, byte *stre,
	int *rl, int *rd)
{
	byte *cs1, *cse;
	int h, hr, bl, bd;
	int i, j, k, l, d, n, ml, md, ld;

	if(str==ctx->pred_cs2)
	{
		bl=ctx->pred_l2;
		bd=ctx->pred_d2;

		*rl=bl;
		*rd=bd;
		return((bl>=4) || ((bl>=3) && (bd<512)));
	}

	if(str==ctx->pred_cs1)
	{
		bl=ctx->pred_l1;
		bd=ctx->pred_d1;

		*rl=bl;
		*rd=bd;
		return((bl>=4) || ((bl>=3) && (bd<512)));
	}

	ml=ctx->maxlen;
	md=ctx->maxdist;
//	cse=str+259;
//	cse=str+1023;
	cse=str+ml;
	if(cse>stre)
		cse=stre;
	
	h=TgvLz_CalcHash(str);
	hr=ctx->hrov[h];
	
	bl=0; bd=0;
	
	for(i=0; i<TKELZ_HASH_DN; i++)
	{
		cs1=ctx->hash[(h*TKELZ_HASH_DN)+((hr+i)&TKELZ_HASH_DN1)];
		if(!cs1)continue;
		l=TgvLz_CheckMatch(str, cs1, cse);
		d=str-cs1;
//		if((l>bl) && (d<65536))
//		if((l>bl) && (d<131072))
		if((l>bl) && (d<md))
			{ bl=l; bd=d; }
	}

//	ctx->chn_ptrs[rv]=cs;
//	ctx->chn_next[rv]=ctx->chn_hash[h];
//	ctx->chn_hash[h]=rv;

	h=TgvLz_CalcHashB(str);

	i=ctx->chn_hash[h]; n=1024; ld=0;
	while(n--)
	{
		cs1=ctx->chn_ptrs[i];
		if(!cs1)continue;

		d=str-cs1;
		if(d<=ld)
			break;
		ld=d;
		if(d>md)
			break;

		l=TgvLz_CheckMatch(str, cs1, cse);		
		if((l>bl) && (d<md))
			{ bl=l; bd=d; }
		i=ctx->chn_next[i];
	}

	ctx->pred_cs2=ctx->pred_cs1;
	ctx->pred_l2=ctx->pred_l1;
	ctx->pred_d2=ctx->pred_d1;
	ctx->pred_cs1=str;
	ctx->pred_l1=bl;
	ctx->pred_d1=bd;

	*rl=bl;
	*rd=bd;
	return((bl>=4) || ((bl>=3) && (bd<512)));
}

int tgvlz_log2u(int val)
{
	int i, v;
	
	v=val; i=0;
	while(v)
		{ i++; v=v>>1; }
	return(i);
}

int TgvLz_EstRawCost(int rl)
{
	int i, j, c;

	c=rl;
	i=rl;
	while(i>=4096)
		{ c+=2; i-=4096; }
	if(i>(128+7))
		{ c+=2; i=i&7; }
	if(i>=8)
		{ c+=1; i=i&7; }

//	c=(rl>>3)+rl;
//	c=(rl>>7)+rl+(rl>=8);
	return(c);
}

int TgvLz_EstMatchCost(int rl, int bl, int bd)
{
	int c;

//	c=(rl>>3)+rl;
//	c=(rl>>7)+rl+(rl>=8);
	c=TgvLz_EstRawCost(rl);
	if((bl<=10) && (bd<512))
		{ c+=2; }
	else if((bl<=67) && (bd<8192))
		{ c+=3; }
	else if((bl<=515) && (bd<131072))
		{ c+=4; }
	else if((bl<=16383) && (bd<(1<<22)))
	{
		c+=1+((bl<128)?1:2)+((bd<32768)?2:3);
	}
	return(c);
}

int TgvLz_LookupMatchB(TgvLz_Context *ctx,
	byte *cs, byte *cse, byte *lcs,
	int *rl, int *rd)
{
	int l, d, l1, d1;
	int rl0, rl1, rl2;
	int mc0, mc1;
	int i, j, k;

	i=TgvLz_LookupMatch(ctx, cs, cse, &l, &d);
	rl0=cs-lcs;
	rl1=rl0+1;	rl2=rl0+2;
	
	mc0=TgvLz_EstMatchCost(rl0, l, d);
	
	mc1=TgvLz_EstRawCost(rl0+l);
	
	if(mc0>=mc1)
	{
		/* Skip match if it costs more than it saves. */
		i=0;
	}
	
	if(i)
	{
		/* Check if a better match is just around the corner. */

		j=TgvLz_LookupMatch(ctx, cs+1, cse, &l1, &d1);
		if(j && (l1>(l+1)) && ((rl0>>3)==(rl1>>3)))
			i=0;

		mc1=TgvLz_EstMatchCost(rl1, l1, d1);
//		if(j && (l1>(l+1)) && (mc1<=mc0))
//		if(j && (l1>(l+0)) && (mc1<=mc0))
		if(j && ((mc1-l1)<=(mc0-l)))
			i=0;

//		if(j && (l1>(l+1)))
//		if(j && (l1>(l+2)))
//			i=0;

		if(i)
		{
			j=TgvLz_LookupMatch(ctx, cs+2, cse, &l1, &d1);

//			if(j && (l1>(l+4)) && ((rl0>>3)==(rl2>>3)))
//				i=0;

			mc1=TgvLz_EstMatchCost(rl2, l1, d1);
//			if(j && (l1>(l+0)) && (mc1<=mc0))
			if(j && ((mc1-l1)<(mc0-l)))
//			if(j && ((mc1-l1)<=(mc0-l)))
				i=0;

//			if(j && (l1>(l+2)))
//			if(j && (l1>(l+3)))
//			if(j && (l1>(l+4)))
//			if(j && (l1>(l+5)))
//				i=0;
		}

#if 0
		if(i)
		{
//			if((d>256) && (l<6))
//			if((d>128) && (l<4))
//				i=0;
			if((d>511) && (l<4))
//			if((d>511) && (l<5))
				i=0;
//			if((d>4096) && (l<10))
//			if((d>8191) && (l<8))
			if((d>8191) && (l<7))
//			if((d>8191) && (l<6))
				i=0;

//			if((d>32767) && (l<9))
//			if((d>32767) && (l<8))
//				i=0;

//			if((d>131071) && (l<9))
			if((d>131071) && (l<10))
//			if((d>131071) && (l<11))
				i=0;
		}
#endif
	}
	
	*rl=l;
	*rd=d;
	return(i);
}

void TgvLz_MatchCopy(byte *dst, byte *src, int sz)
{
	byte *cs, *ct, *cte;
	u64 v;
	int d;
	
	d=dst-src;
	if(d<8)
	{
		if(d==1)
		{
			v=*src;
			v=v|(v<<8);
			v=v|(v<<16);
			v=v|(v<<32);

			ct=dst; cte=dst+sz;
			while(ct<cte)
			{
				*(u64 *)ct=v;
				ct+=8;
			}
		}else
			if(d==2)
		{
			v=*(u16 *)src;
			v=v|(v<<16);
			v=v|(v<<32);

			ct=dst; cte=dst+sz;
			while(ct<cte)
			{
				*(u64 *)ct=v;
				ct+=8;
			}
		}else
			if(d==4)
		{
			v=*(u32 *)src;
			v=v|(v<<32);

			ct=dst; cte=dst+sz;
			while(ct<cte)
			{
				*(u64 *)ct=v;
				ct+=8;
			}
		}else
		{
			v=*(u64 *)src;
			ct=dst; cte=dst+sz;
			while(ct<cte)
			{
				*(u64 *)ct=v;
				ct+=d;
			}

#if 0
			cs=src;
			ct=dst; cte=dst+sz;
			while(ct<cte)
				{ *ct++=*cs++; }
#endif
		}
	}else
	{
		cs=src;
		ct=dst; cte=dst+sz;
		while(ct<cte)
		{
			*(u64 *)ct=*(u64 *)cs;
			ct+=8; cs+=8;
		}
	}
}

#if 1
force_inline void TgvLz_MatchCopy2(byte *dst, int sz, int d)
{
	byte *cs, *ct, *cte;
	u64 v, v1;
	
	if(d<8)
	{
		if(d==1)
		{
//			v=*src;
			v=*(dst-d);
			v=v|(v<<8);
			v=v|(v<<16);
			v=v|(v<<32);

			ct=dst; cte=dst+sz;
			while(ct<cte)
			{
				*(u64 *)ct=v;
				ct+=8;
			}
		}else
			if(d==2)
		{
//			v=*(u16 *)src;
			v=*(u16 *)(dst-d);
			v=v|(v<<16);
			v=v|(v<<32);

			ct=dst; cte=dst+sz;
			while(ct<cte)
			{
				*(u64 *)ct=v;
				ct+=8;
			}
		}else
			if(d==4)
		{
//			v=*(u32 *)src;
			v=*(u32 *)(dst-d);
			v=v|(v<<32);

			ct=dst; cte=dst+sz;
			while(ct<cte)
			{
				*(u64 *)ct=v;
				ct+=8;
			}
		}else
		{
//			v=*(u64 *)src;
			v=*(u64 *)(dst-d);
			ct=dst; cte=dst+sz;
			while(ct<cte)
			{
				*(u64 *)ct=v;
				ct+=d;
			}
		}
	}else
		if(sz<=16)
	{
		cs=dst-d;
		((u64 *)dst)[0]=((u64 *)cs)[0];
		((u64 *)dst)[1]=((u64 *)cs)[1];
	}else
	{
//		cs=src;
		cs=dst-d;
		ct=dst; cte=dst+sz;
		while(ct<cte)
		{
#if 0
			*(u64 *)ct=*(u64 *)cs;
			ct+=8; cs+=8;
#endif
#if 1
			((u64 *)ct)[0]=((u64 *)cs)[0];
			((u64 *)ct)[1]=((u64 *)cs)[1];
			ct+=16; cs+=16;
#endif
		}
	}
}
#endif

#if 0
force_inline void TgvLz_MatchCopy2(byte *dst, int sz, int d)
{
	byte *cs, *ct, *cte;
	u64 v, v1;
	
	if(d<16)
	{
		if(d<8)
		{
			if(d==1)
			{
	//			v=*src;
				v=*(dst-d);
				v=v|(v<<8);
				v=v|(v<<16);
				v=v|(v<<32);

				ct=dst; cte=dst+sz;
				while(ct<cte)
				{
					*(u64 *)ct=v;
					ct+=8;
				}
			}else
				if(d==2)
			{
	//			v=*(u16 *)src;
				v=*(u16 *)(dst-d);
				v=v|(v<<16);
				v=v|(v<<32);

				ct=dst; cte=dst+sz;
				while(ct<cte)
				{
					*(u64 *)ct=v;
					ct+=8;
				}
			}else
				if(d==4)
			{
	//			v=*(u32 *)src;
				v=*(u32 *)(dst-d);
				v=v|(v<<32);

				ct=dst; cte=dst+sz;
				while(ct<cte)
				{
					*(u64 *)ct=v;
					ct+=8;
				}
			}else
			{
	//			v=*(u64 *)src;
				v=*(u64 *)(dst-d);
				ct=dst; cte=dst+sz;
				while(ct<cte)
				{
					*(u64 *)ct=v;
					ct+=d;
				}
			}
		}else
			if(sz<=16)
		{
			cs=dst-d;
			((u64 *)dst)[0]=((u64 *)cs)[0];
			((u64 *)dst)[1]=((u64 *)cs)[1];
		}else
		{
			cs=dst-d;
			ct=dst; cte=dst+sz;
			while(ct<cte)
			{
				((u64 *)ct)[0]=((u64 *)cs)[0];
				((u64 *)ct)[1]=((u64 *)cs)[1];
				ct+=16; cs+=16;
			}
		}
	}else
		if(sz<=16)
	{
		cs=dst-d;
		v=((u64 *)cs)[0];
		v1=((u64 *)cs)[1];
		((u64 *)dst)[0]=v;
		((u64 *)dst)[1]=v1;
	}else
	{
//		cs=src;
		cs=dst-d;
		ct=dst; cte=dst+sz;
		while(ct<cte)
		{
			v=((u64 *)cs)[0];
			v1=((u64 *)cs)[1];
			((u64 *)ct)[0]=v;
			((u64 *)ct)[1]=v1;
			ct+=16; cs+=16;
		}
	}
}
#endif

force_inline void TgvLz_RawCopy(byte *dst, byte *src, int sz)
{
	byte *cs, *ct, *cte;

	if(sz>8)
	{
		cs=src;
		ct=dst; cte=dst+sz;
		while(ct<cte)
		{
#if 0
			*(u64 *)ct=*(u64 *)cs;
			ct+=8; cs+=8;
#endif
#if 1
			((u64 *)ct)[0]=((u64 *)cs)[0];
			((u64 *)ct)[1]=((u64 *)cs)[1];
			ct+=16; cs+=16;
#endif
		}
	}else
		if(sz)
	{
		*(u64 *)dst=*(u64 *)src;
	}
}

force_inline void TgvLz_RawCopyB(byte *dst, byte *src, int sz)
{
	byte *cs, *ct, *cte;

	cs=src;
	ct=dst; cte=dst+sz;
	while(ct<cte)
	{
#if 0
		*(u64 *)ct=*(u64 *)cs;
		ct+=8; cs+=8;
#endif
#if 1
		((u64 *)ct)[0]=((u64 *)cs)[0];
		((u64 *)ct)[1]=((u64 *)cs)[1];
		ct+=16; cs+=16;
#endif
	}
}

int TgvLz_EncodeBufferRP2(TgvLz_Context *ctx,
	byte *ibuf, byte *obuf, int ibsz, int obsz)
{
	byte *cs, *cse, *lcs;
	byte *ct;
	u32 v;
	int pl, pd;
	int l, d, rl, l1, d1;
	int i, j, k;
	
	ctx->chn_base=ibuf;
	cs=ibuf; cse=ibuf+ibsz;
	ct=obuf;
	
	pd=0; pl=0;
	
	lcs=cs;
	while(cs<cse)
	{
//		i=TgvLz_LookupMatch(ctx, cs, cse, &l, &d);
		i=TgvLz_LookupMatchB(ctx, cs, cse, lcs, &l, &d);

		if(!i)
		{
			TgvLz_HashByte(ctx, cs);
			cs++;
			continue;
		}
			
		rl=cs-lcs;

		ctx->stat_rlen[tgvlz_log2u(rl)]++;
		ctx->stat_len[tgvlz_log2u(l)]++;
		ctx->stat_dist[tgvlz_log2u(d)]++;

#if 1
//		while(rl>128)
		while(rl>(128+7))
		{
			j=(rl>>3)-1;
			if(j>511)j=511;
			
			k=(j+1)*8;

			v=0x3F+(j<<7);
			*ct++=v;
			*ct++=v>>8;

			memcpy(ct, lcs, k);
			ct+=k;	lcs+=k;	rl-=k;
		}
#endif

		while(rl>=8)
		{
			j=(rl>>3)-1;
			if(j>15)j=15;
			
			k=(j+1)*8;
			*ct++=0x07+(j<<4);
			memcpy(ct, lcs, k);
			ct+=k;	lcs+=k;	rl-=k;
		}

#if 1
		if((rl<8) && (l<=10) && (d<=511))
		{
			d1=d;
			l1=l-3;
			v=(d1<<7)|(l1<<4)|(rl<<1)|0;
			*ct++=v;
			*ct++=v>>8;
			memcpy(ct, lcs, rl);
			ct+=rl;
		}
		else
#endif
#if 1
		if((rl<8) && (l<=67) && (d<=8191))
		{
			d1=d;
			l1=l-4;

			v=(d1<<11)|(l1<<5)|(rl<<2)|1;
			*ct++=v;
			*ct++=v>>8;
			*ct++=v>>16;
			memcpy(ct, lcs, rl);
			ct+=rl;
		}
		else
#endif
#if 1
		if((rl<8) && (l<=515) && (d<=131071))
		{
			d1=d;
			l1=l-4;

			v=(d1<<15)|(l1<<6)|(rl<<3)|3;
			*ct++=v;
			*ct++=v>>8;
			*ct++=v>>16;
			*ct++=v>>24;

			memcpy(ct, lcs, rl);
			ct+=rl;
		}
		else
#endif
#if 1
		if((rl<8) && (l<=0x3FFF) && (d<=0x3FFFFF))
		{
			*ct++=0x0F|(rl<<5);
			d1=d;
			l1=l-4;

			if(l1<128)
			{
				*ct++=(l1<<1);
			}else
			{
				v=(l1<<2)|1;
				*ct++=v;
				*ct++=v>>8;
			}

			if(d1<32768)
			{
				v=d1<<1;
				*ct++=v;
				*ct++=v>>8;
			}else
			{
				v=(d1<<2)|1;
				*ct++=v;
				*ct++=v>>8;
				*ct++=v>>16;
			}

			memcpy(ct, lcs, rl);
			ct+=rl;
		}
		else
#endif
		{
			debug_break
		}
		
		pl=l; pd=d;
		
		TgvLz_HashStr(ctx, cs, l);
		cs+=l;
		lcs=cs;
	}

	rl=cs-lcs;
	
	while(rl>8)
	{
		j=(rl>>3)-1;
		if(j>15)j=15;
		
		k=(j+1)*8;
		*ct++=0x07+(j<<4);
		memcpy(ct, lcs, k);
		ct+=k;	rl-=k;	lcs+=k;
	}

	while(rl)
	{
		j=rl;
		if(j>3)j=3;

		*ct++=0x1F+(j<<6);
		memcpy(ct, lcs, j);
		ct+=j;	rl-=j;	lcs+=j;
	}

	*ct++=0x1F;
	
	return(ct-obuf);
}

int TgvLz_DecodeBufferRP2(
	byte *ibuf, byte *obuf, int ibsz, int obsz)
{
	u32 tag;
	byte *cs, *ct, *cse;
//	int pl, pd;
	int rl, l, d;
	u64 t0;
	int t1, t2;
	
	cs=ibuf; cse=ibuf+ibsz;
	ct=obuf;
	rl=0; l=0; d=0;
	
	while(1)
	{
//		t0=*(u32 *)cs;
		t0=*(u64 *)cs;
		if(!(t0&0x01))
		{
			cs+=2;
			rl=(t0>>1)&7;
			l=((t0>>4)&7)+3;
			d=(t0>>7)&511;
		}else
			if(!(t0&0x02))
		{
			cs+=3;
			rl=(t0>>2)&7;
			l=((t0>>5)&63)+4;
			d=(t0>>11)&8191;
		}else
			if(!(t0&0x04))
		{
			cs+=4;
			rl=(t0>>3)&7;
			l=((t0>>6)&511)+4;
			d=(t0>>15)&131071;
		}else
			if(!(t0&0x08))
		{
			cs++;
			t1=(t0>>4)&15;
			rl=(t1+1)*8;
			TgvLz_RawCopyB(ct, cs, rl);
			cs+=rl;
			ct+=rl;
			continue;
		}else
			if(!(t0&0x10))
		{
			/* Long Match */
			cs++;
			rl=(t0>>5)&7;
			t1=t0>>8;
			if(!(t1&1))
				{ l=((t1>>1)&0x007F)+4; cs+=1; t2=t0>>16; }
			else
				{ l=((t1>>2)&0x3FFF)+4; cs+=2; t2=t0>>24; }
			if(!(t2&1))
				{ d=((t2>>1)&0x007FFF); cs+=2; }
			else
				{ d=((t2>>2)&0x3FFFFF); cs+=3; }
		}else
			if(!(t0&0x20))
		{
			rl=(t0>>6)&3;
			if(!rl)break;
			*(u32 *)ct=*(u32 *)cs;
			cs+=rl;
			ct+=rl;
			continue;
		}else
			if(!(t0&0x40))
		{
			/* Long Raw */
			cs+=2;
			t1=(t0>>7)&511;
			rl=(t1+1)*8;
			TgvLz_RawCopyB(ct, cs, rl);
			cs+=rl;
			ct+=rl;
			continue;
		}else
		{
			debug_break
		}

		*(u64 *)ct=*(u64 *)cs;
		cs+=rl;
		ct+=rl;
		TgvLz_MatchCopy2(ct, l, d);
		ct+=l;
	}
	
	return(ct-obuf);
}

u32 TgvLz_CalculateImagePel4BChecksum(byte *buf, int size)
{
	byte *cs, *cse;
//	u32 v;
	u32 v0, v1, v2, v3;
	u64 acc_lo, acc_hi;
	u32 csum;
	
	cs=buf;
	cse=cs+size;
	acc_lo=1;
	acc_hi=0;
#if 0
	while(cs<cse)
	{
		v=*(u32 *)cs;
		acc_lo=acc_lo+v;

		acc_hi=acc_hi+acc_lo;
		cs+=4;
	}
#endif
#if 1
	while(cs<cse)
	{
		v0=((u32 *)cs)[0];	v1=((u32 *)cs)[1];
		v2=((u32 *)cs)[2];	v3=((u32 *)cs)[3];
		acc_lo=acc_lo+v0;	acc_hi=acc_hi+acc_lo;
		acc_lo=acc_lo+v1;	acc_hi=acc_hi+acc_lo;
		acc_lo=acc_lo+v2;	acc_hi=acc_hi+acc_lo;
		acc_lo=acc_lo+v3;	acc_hi=acc_hi+acc_lo;
		cs+=16;
	}
#endif
	acc_lo=((u32)acc_lo)+(acc_lo>>32);
	acc_lo=((u32)acc_lo)+(acc_lo>>32);
	acc_hi=((u32)acc_hi)+(acc_hi>>32);
	acc_hi=((u32)acc_hi)+(acc_hi>>32);
	csum=(u32)(acc_lo^acc_hi);
	return(csum);
}

int TgvLz_DoEncode(TgvLz_Context *ctx,
	byte *ibuf, byte *obuf, int isz)
{
	long long ttsz;
	byte *obuf2;
	double f, g;
	int t0, t1, te;
	int csum1, csum2;
	int osz, osz2;
	int i, j, k;

//	obuf2=malloc(isz*2);

	osz=ctx->EncodeBuffer(ctx, ibuf, obuf, isz, 2*isz);
	
//	printf("%s: %d -> %d\n", ctx->tstName, isz, osz);

	csum1=TgvLz_CalculateImagePel4BChecksum(ibuf, isz);	
	ctx->csum=csum1;
	return(osz);
}

int TgvLz_DoTest(TgvLz_Context *ctx,
	byte *ibuf, byte *obuf, int isz)
{
	long long ttsz;
	byte *obuf2;
	double f, g;
	int t0, t1, te;
	int csum1, csum2;
	int osz, osz2;
	int i, j, k;

	obuf2=malloc(isz*2);

	osz=ctx->EncodeBuffer(ctx, ibuf, obuf, isz, 2*isz);
	
	printf("%s: %d -> %d\n", ctx->tstName, isz, osz);

#if 0
	printf("Stat Len:\n");
	for(i=0; i<2; i++)
	{
		k=i*8;
		printf("%5d: ", k);
		for(j=0; j<8; j++)
		{
			printf("%5d ", ctx->stat_len[i*8+j]);
		}
		printf("\n");
	}

	printf("Stat Dist:\n");
	for(i=0; i<3; i++)
	{
		k=i*8;
		printf("%5d: ", k);
		for(j=0; j<8; j++)
		{
			printf("%5d ", ctx->stat_dist[i*8+j]);
		}
		printf("\n");
	}

	printf("Stat Raw:\n");
	for(i=0; i<2; i++)
	{
		k=i*8;
		printf("%5d: ", k);
		for(j=0; j<8; j++)
		{
			printf("%5d ", ctx->stat_rlen[i*8+j]);
		}
		printf("\n");
	}
#endif

#if 1
	osz2=ctx->DecodeBuffer(obuf, obuf2, osz, isz*2);
	
	if(osz2!=isz)
	{
		printf("%s: Size mismatch\n", ctx->tstName);
	}else
	{
		if(memcmp(ibuf, obuf2, isz))
		{
			printf("%s: Data mismatch\n", ctx->tstName);
		}else
		{
			printf("%s: Data OK\n", ctx->tstName);
		}
	}

	*(u64 *)(obuf2+isz+0)=0;
	*(u64 *)(obuf2+isz+8)=0;

	csum1=TgvLz_CalculateImagePel4BChecksum(ibuf, isz);
	csum2=TgvLz_CalculateImagePel4BChecksum(obuf2, isz);
	
	if(csum1!=csum2)
	{
		printf("%s: Checksum %08X->%08X\n", ctx->tstName, csum1, csum2);
	}
	
	ctx->csum=csum2;
	
	t0=clock();
	te=t0+(10*CLOCKS_PER_SEC);
	t1=t0;
	ttsz=0;
	while(t1<te)
	{
		ttsz+=ctx->DecodeBuffer(obuf, obuf2, osz, isz*2);
		t1=clock();
	}
	
	f=(t1-t0)/((double)CLOCKS_PER_SEC);
	printf("%s: %lldB %fs %.2f MB/s\n",
		ctx->tstName, ttsz,
		f, (ttsz/f)*(1.0/1048576));
#endif

	free(obuf2);
	return(osz);
}

int main(int argc, char *argv[])
{
	TgvLz_Context *ctx;
	long long ttsz;
	byte *ibuf;
	byte *obuf;
	byte *obuf2;
	char *ifn, *ofn;
	double f, g;
	int t0, t1, te;
	int isz, csz, osz, osz2, dsz, dsum, csum, mode;
	int i, j, k;
	
	ifn=NULL;
	ofn=NULL;
	
	mode=0;
	for(i=1; i<argc; i++)
	{
		if(argv[i][0]=='-')
		{
			if(!strcmp(argv[i], "-z"))
				mode=1;
			if(!strcmp(argv[i], "-d"))
				mode=2;
			if(!strcmp(argv[i], "-b"))
				mode=3;
			if(!strcmp(argv[i], "-t"))
				mode=4;
			continue;
		}
		
		if(!ifn)
		{
			ifn=argv[i];
			continue;
		}

		if(!ofn)
		{
			ofn=argv[i];
			continue;
		}
	}
	
	if((mode==0) && !ifn)
	{
		printf("usage: %s [opts*] infile [outfile]\n", argv[0]);
		printf("\t-z\t\tEncode\n");
		printf("\t-d\t\tDecode\n");
		printf("\t-b\t\tBenchmark\n");
	}
	
	ibuf=NULL;

	if(ifn)
	{
		ibuf=TgvLz_LoadFile(ifn, &isz);
	}else
	{
		printf("%s: no input file\n", argv[0]);
		return(-1);
	}
	
//	ctx1=malloc(sizeof(TgvLz_Context));
//	ctx2=malloc(sizeof(TgvLz_Context));
	ctx=malloc(sizeof(TgvLz_Context));
	
//	obuf=malloc(isz*2);

//	obuf2=malloc(isz*2);

	memset(ctx, 0, sizeof(TgvLz_Context));
//	ctx->ct=obuf;	
	ctx->maxlen=16383;
	ctx->maxdist=(1<<22)-1;

	ctx->EncodeBuffer=TgvLz_EncodeBufferRP2;
	ctx->DecodeBuffer=TgvLz_DecodeBufferRP2;
	ctx->tstName="RP2";

	if((mode==3) || ((mode==0) && !ofn))
	{
		obuf=malloc(isz*2);
		osz=TgvLz_DoTest(ctx, ibuf, obuf+16, isz);
		
	//	csum=TgvLz_CalculateImagePel4BChecksum(ibuf, isz);
		
		memcpy(obuf, "RP2A", 4);
		*(u32 *)(obuf+4)=osz;
		*(u32 *)(obuf+8)=isz;
	//	*(u32 *)(obuf+12)=csum;
		*(u32 *)(obuf+12)=ctx->csum;

		if(ofn)
		{
			TgvLz_StoreFile(ofn, obuf, osz+16);
		}
		return(0);
	}
	
	if((mode==1) || ((mode==0) && ofn))
	{
		obuf=malloc(isz*2);
		osz=TgvLz_DoEncode(ctx, ibuf, obuf+16, isz);
		
		memcpy(obuf, "RP2A", 4);
		*(u32 *)(obuf+4)=osz;
		*(u32 *)(obuf+8)=isz;
		*(u32 *)(obuf+12)=ctx->csum;

		if(ofn)
		{
			TgvLz_StoreFile(ofn, obuf, osz+16);
		}
		return(0);
	}

	if(((mode==2) && ofn) || (mode==4))
	{
		if(memcmp(ibuf, "RP2A", 4))
		{
			printf("Magic Fail\n");
			return(0);
		}
		
		osz=*(u32 *)(ibuf+4);
		dsz=*(u32 *)(ibuf+8);
		dsum=*(u32 *)(ibuf+12);

		obuf=malloc(dsz*2);
		osz2=ctx->DecodeBuffer(ibuf+16, obuf, osz, dsz*2);
		*(u64 *)(obuf+osz2+0)=0;
		*(u64 *)(obuf+osz2+8)=0;
		
		csum=TgvLz_CalculateImagePel4BChecksum(obuf, osz2);

		if(osz2!=dsz)
		{
			printf("Size mismatch\n");
		}else
		{
			if(csum!=dsum)
			{
				printf("Checksum mismatch %08X->%08X\n", dsum, csum);
			}else
			{
				if(mode==4)
				{
					printf("OK\n");
				}
			}
		}

		if(ofn)
		{
			TgvLz_StoreFile(ofn, obuf, osz2);
		}
		return(0);
	}

	return(0);
}
