/*
Block, 64-bit:
  (63:48): ColorB
  (47:32): ColorA
  (31:16): Pixel Bits
  (15: 8): ?
  ( 7: 0): Tag

  0000: Flat, ColorA
  0010: Flat, ColorB
  0001: 2x2x1
  0011: 4x4x1

 */

#if 0

#define		BTPIC_TWOCC(a, b)			((a)|((b)<<8))
#define		BTPIC_FOURCC(a, b, c, d)	((a)|((b)<<8)|((c)<<16)|((d)<<24))

#define		BTPIC_TCC_IX	BTPIC_TWOCC('I', 'X')
#define		BTPIC_TCC_PX	BTPIC_TWOCC('P', 'X')
#define		BTPIC_TCC_IZ	BTPIC_TWOCC('I', 'Z')
#define		BTPIC_TCC_PZ	BTPIC_TWOCC('P', 'Z')
#define		BTPIC_TCC_HX	BTPIC_TWOCC('H', 'X')
#define		BTPIC_TCC_PT	BTPIC_TWOCC('P', 'T')

#define		BTPIC_TCC_Z3	BTPIC_TWOCC('Z', '3')
#define		BTPIC_TCC_Z4	BTPIC_TWOCC('Z', '4')

#define BTPIC_QFL_PFRAME	0x100

#endif

#if 0
typedef	uint8_t		byte;
typedef	uint16_t	u16;
typedef	uint32_t	u32;
typedef	uint64_t	u64;

typedef	int8_t		sbyte;
typedef	int16_t		s16;
typedef	int32_t		s32;
typedef	int64_t		s64;

#define		unaligned	__unaligned

#define		btpic_getu16(p)		(*(unaligned u16 *)(p))
#define		btpic_getu32(p)		(*(unaligned u32 *)(p))
#define		btpic_getu64(p)		(*(unaligned u64 *)(p))

#define		btpic_setu16(p, v)	((*(unaligned u16 *)(p))=(v))
#define		btpic_setu32(p, v)	((*(unaligned u32 *)(p))=(v))
#define		btpic_setu64(p, v)	((*(unaligned u64 *)(p))=(v))

#endif

typedef struct BTIC5B_DecodeContext_s BTIC5B_DecodeContext;

struct BTIC5B_DecodeContext_s {
	u64 *blka;
	u64 *lblka;
	int bxs;
	int bys;

	int xs;
	int ys;
	int fl;
	
	u16 clra;
	u16 clrb;
	u16 clrc;
	byte clrdy;
	
	byte *zfbuf;		//LZ Frame temporary buffer
	int zfbsz;			//LZ Frame buffer size
};

#if 0
int BTPIC_DecodeBufferRP2(
	byte *ibuf, byte *obuf, int ibsz, int obsz)
{
	u32 tag;
	byte *cs, *ct, *cse, *cs1, *cs1e, *ct1e;
	int pl, pd;
	int rl, l, d;
	u64 t0, v0, v1;
	int t1, t2;
	
	cs=ibuf; cse=ibuf+ibsz;
	ct=obuf;
	pl=0; pd=0;
	
	while(1)
	{
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
//			W_RawCopyB(ct, cs, rl);
//			cs+=rl;
//			ct+=rl;

			cs1e=cs+rl;
			ct1e=ct+rl;
			while(ct<ct1e)
			{
				v0=((u64 *)cs)[0];
				v1=((u64 *)cs)[1];
				cs+=16;
				((u64 *)ct)[0]=v0;
				((u64 *)ct)[1]=v1;
				ct+=16;
			}			
			cs=cs1e;
			ct=ct1e;

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
			cs++;
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
//			W_RawCopyB(ct, cs, rl);
//			cs+=rl;
//			ct+=rl;

			cs1e=cs+rl;
			ct1e=ct+rl;
			while(ct<ct1e)
			{
				v0=((u64 *)cs)[0];
				v1=((u64 *)cs)[1];
				cs+=16;
				((u64 *)ct)[0]=v0;
				((u64 *)ct)[1]=v1;
				ct+=16;
			}			
			cs=cs1e;
			ct=ct1e;
			continue;
		}else
		{
			__debugbreak();
		}

		*(u64 *)ct=*(u64 *)cs;
		cs+=rl;
		ct+=rl;
		
		cs1=ct-d;
		ct1e=ct+l;
		
		if(d<16)
		{
			v0=((u64 *)cs1)[0];
			v1=((u64 *)cs1)[1];
			while(ct<ct1e)
			{
				((u64 *)ct)[0]=v0;
				((u64 *)ct)[1]=v1;
				ct+=d;
			}
		}else
		{
			while(ct<ct1e)
			{
				v0=((u64 *)cs1)[0];
				v1=((u64 *)cs1)[1];
				cs1+=16;
				((u64 *)ct)[0]=v0;
				((u64 *)ct)[1]=v1;
				ct+=16;
			}
		}
		
		ct=ct1e;
		
//		W_MatchCopy2(ct, l, d);
//		ct+=l;
	}
	
	return(ct-obuf);
}
#endif

BTIC5B_DecodeContext *BTIC5B_AllocDecodeContext(void)
{
	BTIC5B_DecodeContext *ctx;
	ctx=malloc(sizeof(BTIC5B_DecodeContext));
	memset(ctx, 0, sizeof(BTIC5B_DecodeContext));
	return(ctx);
}

void BTIC5B_DecodeImageBlock(u64 blk, u16 *dptr, int ystr)
{
	u16 *ct;
	u64 v;
	int clra, clrb, pix, tg;
	int t0, t1, t2, t3;

	tg=blk;
	clra=(blk>>32);
	clrb=(blk>>48);
	pix=blk>>16;
	ct=dptr;

	if(!(tg&1))
	{
		if(tg&0x02)		{ t0=(u16)clrb; } else { t0=(u16)clra; }
		v=t0|(t0<<16);			v=v|(v<<32);
		btpic_setu64(ct, v);	ct+=ystr;
		btpic_setu64(ct, v);	ct+=ystr;
		btpic_setu64(ct, v);	ct+=ystr;
		btpic_setu64(ct, v);
		return;
	}

	if(!(tg&2))
	{
		if(pix&0x0001)	{ t0=clrb; } else { t0=clra; }
		if(pix&0x0002)	{ t1=clrb; } else { t1=clra; }
		if(pix&0x0004)	{ t2=clrb; } else { t2=clra; }
		if(pix&0x0008)	{ t3=clrb; } else { t3=clra; }
		ct[0]=t0;	ct[1]=t0;	ct[2]=t1;	ct[3]=t1;
		ct+=ystr;
		ct[0]=t0;	ct[1]=t0;	ct[2]=t1;	ct[3]=t1;
		ct+=ystr;
		ct[0]=t2;	ct[1]=t2;	ct[2]=t3;	ct[3]=t3;
		ct+=ystr;
		ct[0]=t2;	ct[1]=t2;	ct[2]=t3;	ct[3]=t3;
		return;
	}

	if(!(tg&4))
	{
		if(pix&0x0001)	{ t0=clrb; } else { t0=clra; }
		if(pix&0x0002)	{ t1=clrb; } else { t1=clra; }
		if(pix&0x0004)	{ t2=clrb; } else { t2=clra; }
		if(pix&0x0008)	{ t3=clrb; } else { t3=clra; }
		ct[0]=t0;	ct[1]=t1;	ct[2]=t2;	ct[3]=t3;
		ct+=ystr;
		if(pix&0x0010)	{ t0=clrb; } else { t0=clra; }
		if(pix&0x0020)	{ t1=clrb; } else { t1=clra; }
		if(pix&0x0040)	{ t2=clrb; } else { t2=clra; }
		if(pix&0x0080)	{ t3=clrb; } else { t3=clra; }
		ct[0]=t0;	ct[1]=t1;	ct[2]=t2;	ct[3]=t3;
		ct+=ystr;
		if(pix&0x0100)	{ t0=clrb; } else { t0=clra; }
		if(pix&0x0200)	{ t1=clrb; } else { t1=clra; }
		if(pix&0x0400)	{ t2=clrb; } else { t2=clra; }
		if(pix&0x0800)	{ t3=clrb; } else { t3=clra; }
		ct[0]=t0;	ct[1]=t1;	ct[2]=t2;	ct[3]=t3;
		ct+=ystr;
		if(pix&0x1000)	{ t0=clrb; } else { t0=clra; }
		if(pix&0x2000)	{ t1=clrb; } else { t1=clra; }
		if(pix&0x4000)	{ t2=clrb; } else { t2=clra; }
		if(pix&0x8000)	{ t3=clrb; } else { t3=clra; }
		ct[0]=t0;	ct[1]=t1;	ct[2]=t2;	ct[3]=t3;
		return;
	}
}


void BTIC5B_DecodeBlockImage(u16 *dptr, u64 *blka,
	int bxs, int bys, int ystr, int clrs)
{
	u64 *cs, *cse;
	u16 *ct0, *ct1;
	u64 blk;
	int y;

	BTPIC0A_DecodeBlockImage(dptr, blka, bxs, bys, ystr, clrs);

#if 0
	cs=blka; ct0=dptr;
	for(y=0; y<bys; y++)
	{
		cse=cs+bxs;
		ct1=ct0;
		ct0+=ystr<<2;
		while(cs<cse)
		{
			blk=*cs;
//			if(!(blk&0x80))
			if(!(blk&0x08))
			{
				BTIC5B_DecodeImageBlock(*cs, ct1, ystr);
			}
			cs++;
			ct1+=4;
		}
	}
#endif
}

void BTIC5B_DecodeCopyBlocks(
	u64 *ct0, u64 *blka, u64 *lblka, int cnt,
	int dvx, int dvy, int bxs, int bys)
{
	u64 *ct, *csl;
	u64 blk;
	int n;

	if(blka==lblka)
		return;

	n=cnt;
	ct=ct0;
	if(dvx|dvy)
	{
		csl=lblka+(ct-blka)+(dvy*bxs)+dvx;
		while(n--)
		{
			blk=*csl++;
//			blk&=~0x80;
			blk&=~0x08;
			*ct++=blk;
		}
	}else
	{
		csl=lblka+(ct-blka);
		while(n--)
		{
			blk=*csl++;
			*ct++=blk;
		}
	}
}

#define	BT5B_MKRBG555(r,g,b)	((((r)&31)<<10)|(((g)&31)<<5)|(((b)&31)<<0))

u16 bt5b_dystep[8]={
	BT5B_MKRBG555( 0, 0, 0),	BT5B_MKRBG555( 2, 2, 2),
	BT5B_MKRBG555( 4, 4, 4),	BT5B_MKRBG555( 6, 6, 6),
	BT5B_MKRBG555( 8, 8, 8),	BT5B_MKRBG555(10,10,10),
	BT5B_MKRBG555(12,12,12),	BT5B_MKRBG555(14,14,14)
	};

char bt5b_pat6gen[8*4]={
	 2,  2,  2,  2,
	 4,  2, -2, -4,
	 4, -1,  1, -4,
	 4, -4,  4, -4,
	-2, -2, -2, -2,
	-4, -2,  2,  4,
	-4,  1, -1,  4,
	-4,  4, -4,  4
};

u16 bt5b_delta5[32];
u16 bt5b_delta9[512];

u16 bt5b_pat6[64];
u32 bt5b_pat6x2[64];
byte bt5b_pat6chn[64];
byte bt5b_pat6hix[16];

int bt5b_initdeltas=0;

int btpic_clamp(int v, int m, int n)
{
	if(v<m)
		return(m);
	if(v>n)
		return(n);
	return(v);
}

void BTIC5B_InitDeltas()
{
	const int ptshr=0;
	int i0, i1, i2, i3;
	int ph0, ph1, ph2, ph3;
	int pv0, pv1, pv2, pv3;
	int p0, p1, p2, p3;
	int px, px2;
	int i, j, k;
	if(bt5b_initdeltas)
		return;
	bt5b_initdeltas=1;
	
	for(i0=0; i0<3; i0++)
		for(i1=0; i1<3; i1++)
			for(i2=0; i2<3; i2++)
	{
		i=(i0*9)+(i1*3)+i2;
		j=BT5B_MKRBG555((i0-1)*2, (i1-1)*2, (i2-1)*2);
		bt5b_delta5[i]=j;
	}

	for(i0=0; i0<7; i0++)
		for(i1=0; i1<7; i1++)
			for(i2=0; i2<7; i2++)
	{
		i=(i0*49)+(i1*7)+i2;
		j=BT5B_MKRBG555((i0-3)*2, (i1-3)*2, (i2-3)*2);
		bt5b_delta9[i]=j;
	}

	for(i0=0; i0<8; i0++)
		for(i1=0; i1<8; i1++)
	{
		ph0=bt5b_pat6gen[i1*4+0];
		ph1=bt5b_pat6gen[i1*4+1];
		ph2=bt5b_pat6gen[i1*4+2];
		ph3=bt5b_pat6gen[i1*4+3];

		pv0=bt5b_pat6gen[i0*4+0];
		pv1=bt5b_pat6gen[i0*4+1];
		pv2=bt5b_pat6gen[i0*4+2];
		pv3=bt5b_pat6gen[i0*4+3];
	
		px=0;
		px2=0;
		
		p0=ph0+pv0;		p1=ph1+pv0;
		p2=ph2+pv0;		p3=ph3+pv0;
		if(p0>=0)px|=0x0001;
		if(p1>=0)px|=0x0002;
		if(p2>=0)px|=0x0004;
		if(p3>=0)px|=0x0008;
		
		p0=btpic_clamp((p0>>ptshr)+2, 0, 3);
		p1=btpic_clamp((p1>>ptshr)+2, 0, 3);
		p2=btpic_clamp((p2>>ptshr)+2, 0, 3);
		p3=btpic_clamp((p3>>ptshr)+2, 0, 3);
		px2=(p0<<0)|(p1<<2)|(p2<<4)|(p3<<6);

		p0=ph0+pv1;		p1=ph1+pv1;
		p2=ph2+pv1;		p3=ph3+pv1;
		if(p0>=0)px|=0x0010;
		if(p1>=0)px|=0x0020;
		if(p2>=0)px|=0x0040;
		if(p3>=0)px|=0x0080;
		
		p0=btpic_clamp((p0>>ptshr)+2, 0, 3);
		p1=btpic_clamp((p1>>ptshr)+2, 0, 3);
		p2=btpic_clamp((p2>>ptshr)+2, 0, 3);
		p3=btpic_clamp((p3>>ptshr)+2, 0, 3);
		px2|=(p0<< 8)|(p1<<10)|(p2<<12)|(p3<<14);

		p0=ph0+pv2;		p1=ph1+pv2;
		p2=ph2+pv2;		p3=ph3+pv2;
		if(p0>=0)px|=0x0100;
		if(p1>=0)px|=0x0200;
		if(p2>=0)px|=0x0400;
		if(p3>=0)px|=0x0800;
		
		p0=btpic_clamp((p0>>ptshr)+2, 0, 3);
		p1=btpic_clamp((p1>>ptshr)+2, 0, 3);
		p2=btpic_clamp((p2>>ptshr)+2, 0, 3);
		p3=btpic_clamp((p3>>ptshr)+2, 0, 3);
		px2|=(p0<<16)|(p1<<18)|(p2<<20)|(p3<<22);

		p0=ph0+pv3;		p1=ph1+pv3;
		p2=ph2+pv3;		p3=ph3+pv3;
		if(p0>=0)px|=0x1000;
		if(p1>=0)px|=0x2000;
		if(p2>=0)px|=0x4000;
		if(p3>=0)px|=0x8000;
		
		p0=btpic_clamp((p0>>ptshr)+2, 0, 3);
		p1=btpic_clamp((p1>>ptshr)+2, 0, 3);
		p2=btpic_clamp((p2>>ptshr)+2, 0, 3);
		p3=btpic_clamp((p3>>ptshr)+2, 0, 3);
		px2|=(p0<<24)|(p1<<26)|(p2<<28)|(p3<<30);
		
		bt5b_pat6[i0*8+i1]=px;
		bt5b_pat6x2[i0*8+i1]=px2;
	}

	for(i=0; i<16; i++)
		bt5b_pat6hix[i]=0xFF;

	for(i=0; i<64; i++)
	{
		px=bt5b_pat6[i];
		k=((px*4093)>>12)&15;
		bt5b_pat6chn[i]=bt5b_pat6hix[k];
		bt5b_pat6hix[k]=i;
	}
}

void BTIC5B_DecodeEndpoint(BTIC5B_DecodeContext *ctx, byte **rcs)
{
	u32 tgv;
	int ca, cb, cc, cdy, cdi, cdj, cdk, cdv, cdva, cdvb;
	byte *cs;

	cs=*rcs;
	tgv=btpic_getu32(cs);

	if(!(tgv&0x1))
	{
		cs+=2;
		cc=(tgv>>1)&0x7BDE;
		cdy=((tgv>>1)&1)|((tgv>>5)&2)|((tgv>>9)&4);
		ca=cc+bt5b_dystep[cdy];
		cb=cc-bt5b_dystep[cdy];

#if 0
//		if((cc&0x7BDE)!=cc)
//			printf("BT5B: Delta C Overflow\n");
		if((ca&0x7BDE)!=ca)
			printf("BT5B: Joined A Overflow\n");
		if((cb&0x7BDE)!=cb)
			printf("BT5B: Joined B Overflow\n");
#endif

		ctx->clra=ca;
		ctx->clrb=cb;
		ctx->clrc=cc;
		ctx->clrdy=cdy;
	}else if((tgv&0x7)!=0x7)
	{
		cdy=ctx->clrdy;

		cdi=(tgv>>3)&31;
		if(cdi>=0x1B)
		{
			if(cdi>=0x1E)
			{
				cs+=2;
				cdi=((tgv>>8)&255)|((cdi<<8)&1);
				cdv=bt5b_delta9[cdi];
			}else if(cdi>=0x1C)
			{
				cs+=3;
				cdi=((tgv>>8)&65535)|((cdi<<16)&1);
				cdj=cdi%360;
				cdk=cdi/360;
				cdva=bt5b_delta9[cdj];
				cdvb=bt5b_delta9[cdk];
			}else
			{
				cs+=3;
				cdv=(tgv>>9)&0x7FFF;
				cdva=cdv;
				cdvb=cdv;

//				cs+=1;
//				cdv=bt5b_delta5[cdi];
//				cdva=cdv;
//				cdvb=cdv;
			}
		}else
		{
			cs+=1;
			cdv=bt5b_delta5[cdi];
			cdva=cdv;
			cdvb=cdv;
		}

		if(cdy<128)
//		if(1)
		{
			cdy+=((tgv>>1)&3)-1;
			cc=ctx->clrc+cdv;
			ca=cc+bt5b_dystep[cdy];
			cb=cc-bt5b_dystep[cdy];

#if 0
			if((cc&0x7BDE)!=cc)
				printf("BT5B: Delta C Overflow\n");
			if((ca&0x7BDE)!=ca)
				printf("BT5B: Delta A Overflow\n");
			if((cb&0x7BDE)!=cb)
				printf("BT5B: Delta B Overflow\n");
#endif

			ctx->clra=ca;
			ctx->clrb=cb;
//			ctx->clrc=cc&0x7BDE;
			ctx->clrc=cc;
			ctx->clrdy=cdy;
		}else
		{
			ca=ctx->clra;
			cb=ctx->clrb;
			if(!(tgv&4))	ca+=cdva;
			if(!(tgv&2))	cb+=cdvb;
			ctx->clra=ca;
			ctx->clrb=cb;
		}
	}else
	{
		cs+=4;
		ca=(tgv>>17)&0x7FFF;
		cb=(tgv>> 2)&0x7FFE;

		ctx->clra=ca;
		ctx->clrb=cb;
		ctx->clrc=0;
		ctx->clrdy=255;
	}
	*rcs=cs;
}

void BTIC5B_DecodeFrameData(BTIC5B_DecodeContext *ctx,
	byte *cdat, int csz, int qfl)
{
	u64 *blka;
	u64 *lblka;
	int bxs, bys, pfrm;

	u64 *ct, *csl, *cte;
	byte *cs, *cse;
	u32 px;
	int ca, cb, cc, cdy, py, n;
	int dx, dy, dvx, dvy;
	u64 tgv, blk;
	int i, j, k;

	blka=ctx->blka;
	lblka=ctx->lblka;
	bxs=ctx->bxs;
	bys=ctx->bys;

	BTIC5B_InitDeltas();

	pfrm=0;
	if(qfl&BTPIC_QFL_PFRAME)
		pfrm=1;

	ca=0; cb=0;
	ct=blka; dvx=0; dvy=0;
	cs=cdat; cse=cdat+csz;
	cte=blka+(bxs*bys);
	while(cs<cse)
	{
		if(ct>=cte)
		{
			__debugbreak();
		}

		tgv=btpic_getu64(cs);

		if(!(tgv&1))	//zzzz_zzz0
		{
#if 1
			cs++;
			dx=(tgv>>2)&63;
			px=bt5b_pat6[dx];
//			px=bt5b_pat6x2[dx];
			
			if(tgv&2)
			{
				BTIC5B_DecodeEndpoint(ctx, &cs);
				ca=ctx->clra;	cb=ctx->clrb;
			}
			
			blk=(((u64)px)<<16)|(((u64)ca)<<32)|(((u64)cb)<<48)|3;
//			blk=(((u64)px)<<32)|
//				(((u64)(ca&0x7FFE))<< 3)|
//				(((u64)(cb&0x7FFE))<<17)|7;
			*ct++=blk;
			continue;
#endif

#if 0
			px=tgv&65535;
			cs+=2;
			
			px|=(px>>1)&1;
			
			BTIC5B_DecodeEndpoint(ctx, &cs);
			ca=ctx->clra;	cb=ctx->clrb;
	
			blk=(((u64)px)<<16)|(((u64)ca)<<32)|(((u64)cb)<<48)|3;
			*ct++=blk;
			continue;
#endif
		}

		if(!(tgv&2))	//zzzz_zz01
		{
#if 0
			if(!pfrm)
			{
				cs++;
				dx=(tgv>>2)&63;
				px=bt5b_pat6[dx];
				blk=(((u64)px)<<16)|(((u64)ca)<<32)|(((u64)cb)<<48)|3;
				*ct++=blk;
				continue;
			}
#endif

			cs++;
			dx=(tgv>>2)&7;
			dy=(tgv>>5)&7;

			if(dx==7)
			{
				n=dy+1;
				if(dy==7)
				{
					cs++;
					n=((tgv>>8)&255)+4;
				}
			}else if(dy==7)
			{
				dvx=0; dvy=0;
				n=dx+1;
			}else
			{
				dvx+=dx-3;
				dvy+=dy-3;
				n=1;
			}
			BTIC5B_DecodeCopyBlocks(
				ct, blka, lblka, n,
				dvx, dvy, bxs, bys);
			ct+=n;
			continue;
		}

		if(!(tgv&4))	//zzzz_z011
		{
			cs++;
			
			if(tgv&8)
			{
				BTIC5B_DecodeEndpoint(ctx, &cs);
				ca=ctx->clra;
				cb=ctx->clrb;
			}

			px=(tgv>>4)&15;
			
//			blk=(((u64)px)<<16)|(((u64)ca)<<32)|(((u64)cb)<<48)|3;
			blk=(((u64)ca)<<32)|(((u64)cb)<<48)|(px<<16)|1;
			*ct++=blk;
			continue;
		}

		if(!(tgv&8))	//zzzz_0111
		{
			cs++;
			n=((tgv>>6)&3)+1;

			if(tgv&16)
			{
				if(tgv&32)
				{
					while(n>0)
					{
						px=*cs++;
						blk=(((u64)ca)<<32)|(((u64)cb)<<48)|(px<<16)|5;
						*ct++=blk;
						n--;
					}
				}else
				{
					while(n>0)
					{
						py=*cs++;

						px=(py>>0)&15;
						blk=(((u64)ca)<<32)|(((u64)cb)<<48)|(px<<16)|1;
						*ct++=blk;

						px=(py>>4)&15;
						blk=(((u64)ca)<<32)|(((u64)cb)<<48)|(px<<16)|1;
						*ct++=blk;

						n--;
					}
				}
			}else
			{
				if(tgv&32)
				{
					BTIC5B_DecodeEndpoint(ctx, &cs);
					ca=ctx->clra;
					cb=ctx->clrb;
				}
				
				while(n>0)
				{
					px=btpic_getu16(cs);
					cs+=2;
					blk=(((u64)px)<<16)|(((u64)ca)<<32)|(((u64)cb)<<48)|3;
					*ct++=blk;
					n--;
				}
			}

			continue;
		}

		if(!(tgv&16))	//zzz0_1111
		{
			cs++;
			n=((tgv>>6)&3)+1;
			i=(tgv>>5)&1;

//			ca=ctx->clra;
//			cb=ctx->clrb;

			blk=(((u64)ca)<<32)|(((u64)cb)<<48)|(i<<1);
			while(n--)
				{ *ct++=blk; }
			continue;
		}

		if(!(tgv&32))	//zz01_1111
		{
			cs++;
			n=((tgv>>6)&3)+1;

			while(n--)
			{
				BTIC5B_DecodeEndpoint(ctx, &cs);
				ca=ctx->clra;
				cb=ctx->clrb;

				blk=(((u64)ca)<<32)|(((u64)cb)<<48)|0;
				*ct++=blk;
			}
			continue;
		}

		if(!(tgv&64))	//z011_1111
		{
			if(tgv&128)
			{
				BTIC5B_DecodeEndpoint(ctx, &cs);
				ca=ctx->clra;
				cb=ctx->clrb;
			}

			px=tgv>>8;
			blk=
				(((u64)(ca&0x7FFE))<< 3) |
				(((u64)(cb&0x7FFE))<<17) |
				(((u64)px)<<32);
			*ct++=blk;
			continue;

#if 0
			__debugbreak();

			cs++;
			n=16;
			i=(tgv>>7)&1;

//			ca=ctx->clra;
//			cb=ctx->clrb;

			blk=(((u64)ca)<<32)|(((u64)cb)<<48)|(i<<1);
			while(n--)
				{ *ct++=blk; }
			continue;
#endif
		}

		if(!(tgv&128))	//0111_1111
		{
			__debugbreak();
		}

		if(!(tgv&256))	//zzz0_1111_1111
		{
			__debugbreak();
		}

		if(!(tgv&512))	//zz01_1111_1111
		{
			__debugbreak();
		}

		if(!(tgv&1024))	//z011_1111_1111
		{
#if 1
			cs+=2;
			n=((tgv>>12)&15)*4+8;
			i=(tgv>>11)&1;

//			ca=ctx->clra;
//			cb=ctx->clrb;

			blk=(((u64)ca)<<32)|(((u64)cb)<<48)|(i<<1);
			while(n--)
				{ *ct++=blk; }
			continue;
#endif
		}

		__debugbreak();
	}
	
	k=-1;
}

int BTIC5B_DecodeFrame(BTIC5B_DecodeContext *ctx,
	byte *cdat, int csz, u16 *img, int ystr, int clrs)
{
	u64 *tblk;
	byte *cs, *cs0, *cs1, *cs2, *cse;
	byte *fdat;
	u32 fcc;
	int sz, dsz, fsz, ffl;
	int i, j, k, n;
	
	if(!csz)
		return(0);
	
	fdat=NULL; ffl=0;
	cs=cdat; cse=cdat+csz;
	while(cs<cse)
	{
		i=cs[0];
		if((i>>5)==0)
		{
			sz=((i&31)<<8)|cs[1];
			fcc=btpic_getu16(cs+2);
			cs0=cs+4;	cs1=cs+sz;
		}else if((i>>5)==1)
		{
			sz=((i&31)<<8)|cs[1];
			fcc=btpic_getu32(cs+2);
			cs0=cs+6;	cs1=cs+sz;
		}else if((i>>5)==2)
		{
			sz=((i&31)<<24)|(cs[1]<<16)|(cs[2]<<8)|cs[3];
			fcc=btpic_getu16(cs+4);
			cs0=cs+6;	cs1=cs+sz;
		}else if((i>>5)==3)
		{
			sz=((i&31)<<24)|(cs[1]<<16)|(cs[2]<<8)|cs[3];
			fcc=btpic_getu32(cs+4);
			cs0=cs+8;	cs1=cs+sz;
		}else if((i>>5)==4)
		{
			sz=((i&31)<<16)|(cs[1]<<8)|cs[2];
			fcc=btpic_getu16(cs+3);
			cs0=cs+5;	cs1=cs+sz;
		}else if((i>>5)==5)
		{
			sz=((i&31)<<16)|(cs[1]<<8)|cs[2];
			fcc=btpic_getu32(cs+3);
			cs0=cs+7;	cs1=cs+sz;
		}

		if((fcc==BTPIC_TCC_Z3) || (fcc==BTPIC_TCC_Z4))
		{
			k=btpic_getu32(cs0+0);
			dsz=k&0xFFFFFF;
			cs2=cs0+4;
			
			if(!(ctx->zfbuf))
			{
				i=65536;
				while(i<=(dsz+64))
					i=i+(i>>1);
				ctx->zfbuf=malloc(i);
				ctx->zfbsz=i;
			}
			
			if((dsz+64)>(ctx->zfbsz))
			{
				i=ctx->zfbsz;
				while(i<=(dsz+64))
					i=i+(i>>1);
				ctx->zfbuf=realloc(ctx->zfbuf, i);
				ctx->zfbsz=i;
			}

			if(fcc==BTPIC_TCC_Z3)
			{
				BTPIC_DecodeBufferRP2(
					cs2, ctx->zfbuf,
					cs1-cs2, ctx->zfbsz);
			}
			
//			BTIC5B_DecodeFrame(ctx, ctx->zfbuf, ctx->zfbsz, img, ystr, clrs);
			BTIC5B_DecodeFrame(ctx, ctx->zfbuf, dsz, img, ystr, clrs);
			
			cs=cs1;
			continue;
		}
		
		if(fcc==BTPIC_TCC_HX)
		{
			ctx->xs=btpic_getu16(cs0+0);
			ctx->ys=btpic_getu16(cs0+2);
			ctx->fl=btpic_getu16(cs0+4);
			ctx->bxs=(ctx->xs+3)>>2;
			ctx->bys=(ctx->ys+3)>>2;

			if(!ctx->blka)
			{
				if(ctx->fl&1)
				{
					ctx->blka=malloc(ctx->bxs*ctx->bys*2*sizeof(u64));
					ctx->lblka=ctx->blka;
				}else
				{
					ctx->blka=malloc(ctx->bxs*ctx->bys*2*sizeof(u64));
					ctx->lblka=malloc(ctx->bxs*ctx->bys*2*sizeof(u64));
				}
			}
			
			cs=cs1;
			continue;
		}

		if(	(fcc==BTPIC_TCC_IX) ||
			(fcc==BTPIC_TCC_PX)	)
		{
			fdat=cs0; fsz=cs1-cs0; ffl=0;
			if(fcc==BTPIC_TCC_PX)
				ffl|=BTPIC_QFL_PFRAME;
			cs=cs1;
			continue;
		}

		cs=cs1;
		continue;
	}
	
	if(fdat)
	{
		if(!ctx->blka)
			return(-1);
		
		tblk=ctx->blka;
		ctx->blka=ctx->lblka;
		ctx->lblka=tblk;
	
		BTIC5B_DecodeFrameData(ctx, fdat, fsz, ffl);
		
		if(img)
		{
			BTIC5B_DecodeBlockImage(
				img, ctx->blka,
				ctx->bxs, ctx->bys, ystr, clrs);
		}
	}
	
	return(0);
}