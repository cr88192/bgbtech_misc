#define TKDELZ_HTABSZ 4096
#define TKDELZ_HTABNB 12

//#define TKDELZ_HTABSZ 8192
//#define TKDELZ_HTABNB 13

#ifndef TKDELZ_BYTE
#define TKDELZ_BYTE
typedef unsigned char			byte;
typedef unsigned short		u16;
typedef unsigned int			u32;
typedef unsigned long long	u64;

typedef signed char			sbyte;
typedef signed short			s16;
typedef signed int			s32;
typedef signed long long		s64;
#endif

#ifdef _MSC_VER

#define tkdelz_getu16le(ptr)	(*(u16 *)(ptr))
#define tkdelz_getu32le(ptr)	(*(u32 *)(ptr))
#define tkdelz_getu64le(ptr)	(*(u64 *)(ptr))

#define tkdelz_setu16le(ptr, val)	(*(u16 *)(ptr)=(val))
#define tkdelz_setu32le(ptr, val)	(*(u32 *)(ptr)=(val))
#define tkdelz_setu64le(ptr, val)	(*(u64 *)(ptr)=(val))

#else

u16 tkdelz_getu16le(void *ptr)
	{ u16 v; memcpy(&v, ptr, 2); return(v); }
u32 tkdelz_getu32le(void *ptr)
	{ u32 v; memcpy(&v, ptr, 4); return(v); }
u64 tkdelz_getu64le(void *ptr)
	{ u64 v; memcpy(&v, ptr, 8); return(v); }

void tkdelz_setu16le(void *ptr, u16 val)
	{ memcpy(ptr, &val, 2); }
void tkdelz_setu32le(void *ptr, u32 val)
	{ memcpy(ptr, &val, 4); }
void tkdelz_setu64le(void *ptr, u64 val)
	{ memcpy(ptr, &val, 8); }

#endif

typedef struct TkDeLz_BihState_s TkDeLz_BihState;

struct TkDeLz_BihState_s {
	u32 magic1;

	byte *cs;
//	u32 win;
//	byte pos;
	byte status;

	byte *bbuf;
	int bbufsz;

	byte *csa0;
	byte *csa1;
	byte *csa2;
	byte *csa3;
	byte pos0;
	byte pos1;
	byte pos2;
	byte pos3;

	u16 htab[TKDELZ_HTABSZ];

	u32 win0;
	u32 win1;
	u32 win2;
	u32 win3;

	u32 hetab[256];
	byte hltab[256];
	int hstab[256];

	u32 magic2;

//	int bbpos;
//	byte bbuf[512];
//	TkDeLz_HufDecState *pctx;
};

#if 0
int TkDeLz_PeekBits(TkDeLz_BihState *ctx, int bits)
	{ return((ctx->win>>ctx->pos)&((1<<bits)-1)); }

void TkDeLz_SkipBits(TkDeLz_BihState *ctx, int bits)
{
	ctx->pos+=bits;
	while(ctx->pos>=8)
		{ ctx->win=(ctx->win>>8)|((*ctx->cs++)<<24); ctx->pos-=8; }
}

int TkDeLz_ReadBits(TkDeLz_BihState *ctx, int bits)
{
	int b;
	b=TkDeLz_PeekBits(ctx, bits);
	TkDeLz_SkipBits(ctx, bits);
	return(b);
}

int TkDeLz_BeginReadBits(TkDeLz_BihState *ctx, byte *ics)
{
	ctx->cs=ics;
	ctx->pos=32;
	TkDeLz_SkipBits(ctx, 0);
	return(0);
}

byte *TkDeLz_EndReadBits(TkDeLz_BihState *ctx)
{
	byte *cs;
	int p;
	cs=ctx->cs;
	p=ctx->pos;
	while(p<=24)
	{
		cs--;
		p+=8;
	}
	return(cs);
}

int TkDeLz_ReadPackedLengths(TkDeLz_BihState *ctx, byte *cls)
{
	byte *t, *te;
	int i, j, c, nz, zc, lc;
	
	t=cls; te=cls+256; lc=0;
	while(t<te)
	{
		c=TkDeLz_ReadBits(ctx, 4); zc=0; nz=0;
		if(c<14)
			{ *t++=c; lc=c; }
		else if(c==14)
			{ nz=TkDeLz_ReadBits(ctx, 4)+3; }
		else if(c==15)
		{
			j=TkDeLz_ReadBits(ctx, 2);
			if(j==0)
				{ nz=TkDeLz_ReadBits(ctx, 6)+19; }
			else if(j==1)
			{
				nz=TkDeLz_ReadBits(ctx, 6)+3; zc=lc;
				if(nz==3)
					{ nz=te-t; zc=0; }
			}
			else
				{ ctx->status=TKDELZ_STATUS_BADHT; }
		}
		while(nz>0)
			{ *t++=zc; nz--; }
	}
	
	if(t>te)
		{ ctx->status=TKDELZ_STATUS_BADHT; }
	return(0);
}

int TkDeLz_DecodeHuffSym(TkDeLz_BihState *ctx, u16 *htab)
{
	int b, c;
	b=(ctx->win>>ctx->pos)&(TKDELZ_HTABSZ-1);
	c=htab[b];
	if(!((c>>8)&15))
		{ __debugbreak(); }
	TkDeLz_SkipBits(ctx, (c>>8)&15);
	return(c&255);
}
#endif

int TkDeLz_ReadPackedLengths(TkDeLz_BihState *ctx, byte *cls)
{
	static byte lptab[256];
	byte *cs;
	int i, j, k, l;
	
	if(lptab[0xC3]!=0xDD)
	{
		for(i=0; i<14; i++)
			for(j=0; j<14; j++)
		{
			k=i*14+j;
			lptab[k]=(i<<4)|j;
		}
	}
	
	cs=ctx->cs;
	i=0; l=0;
	while(i<256)
	{
		j=*cs++;
		if(j<0xD0)
		{
			k=lptab[j];
			cls[i+0]=k>>4;
			cls[i+1]=k&15;
			l=k&15;
			i+=2;
			continue;
		}
		if((j>=0xD0) && (j<=0xDF))
		{
			k=(j-0xD0)*2+4;
			while((k--)>0)
				cls[i++]=0;
			continue;
		}
		if((j>=0xE0) && (j<=0xEF))
		{
			k=(j-0xE0)*2+4;
			while((k--)>0)
				cls[i++]=l;
			continue;
		}
		break;
	}
	ctx->cs=cs;
	return(0);
}

byte TkDeLz_TransposeByte(byte v)
{
	static const byte trans4[16]={
		0x0,0x8,0x4,0xC, 0x2,0xA,0x6,0xE,
		0x1,0x9,0x5,0xD, 0x3,0xB,0x7,0xF};
	return((trans4[v&15]<<4)|trans4[(v>>4)&15]);
}

u16 TkDeLz_TransposeWord(u16 v)
	{ return((TkDeLz_TransposeByte(v&255)<<8)|TkDeLz_TransposeByte(v>>8)); }

void TkDeLz_SetupTableLengths(TkDeLz_BihState *ctx, u16 *htab, byte *cls)
{
	int c, l, tc, ntc;
	int i, j, k;

	for(i=0; i<TKDELZ_HTABSZ; i++)
		htab[i]=0;
	
	c=0;
	for(l=1; l<(TKDELZ_HTABNB+1); l++)
	{
		for(i=0; i<256; i++)
			if(cls[i]==l)
		{
			ntc=1<<(TKDELZ_HTABNB-l);
			tc=c<<(TKDELZ_HTABNB-l);
			for(j=0; j<ntc; j++)
			{
				k=TkDeLz_TransposeWord(tc+j)>>(16-TKDELZ_HTABNB);
				htab[k]=(l<<8)|i;
			}
			c++;
		}
		c=c<<1;
	}
}

u32 TkDeLz_DecodeBihSym4x(TkDeLz_BihState *ctx)
{
	u16 *htab;
	byte *cs0, *cs1, *cs2, *cs3;
	u32 w0, w1, w2, w3;
	int c0, c1, c2, c3;
	int p0, p1, p2, p3;
	int b, c;

	htab=ctx->htab;
	cs0=ctx->csa0;					cs1=ctx->csa1;
	cs2=ctx->csa2;					cs3=ctx->csa3;
//	w0=*(u32 *)(cs0);				w1=*(u32 *)(cs1);
//	w2=*(u32 *)(cs2);				w3=*(u32 *)(cs3);
	w0=tkdelz_getu32le(cs0);		w1=tkdelz_getu32le(cs1);
	w2=tkdelz_getu32le(cs2);		w3=tkdelz_getu32le(cs3);
	p0=ctx->pos0;					p1=ctx->pos1;
	p2=ctx->pos2;					p3=ctx->pos3;
	c0=(w0>>p0)&(TKDELZ_HTABSZ-1);	c1=(w1>>p1)&(TKDELZ_HTABSZ-1);
	c2=(w2>>p2)&(TKDELZ_HTABSZ-1);	c3=(w3>>p3)&(TKDELZ_HTABSZ-1);
	c0=htab[c0];					c1=htab[c1];
	c2=htab[c2];					c3=htab[c3];
	p0+=(c0>>8)&15;					p1+=(c1>>8)&15;
	p2+=(c2>>8)&15;					p3+=(c3>>8)&15;
	ctx->csa0=cs0+(p0>>3);			ctx->csa1=cs1+(p1>>3);
	ctx->csa2=cs2+(p2>>3);			ctx->csa3=cs3+(p3>>3);
	ctx->pos0=p0&7;					ctx->pos1=p1&7;
	ctx->pos2=p2&7;					ctx->pos3=p3&7;
	c0&=255;						c1&=255;
	c2&=255;						c3&=255;
	w2=c0|(c1<<8);					w3=c2|(c3<<8);
	w0=w2|(w3<<16);
	return(w0);
}

u64 TkDeLz_DecodeBihSym8x(TkDeLz_BihState *ctx)
{
	u64 va, vb;
	va=TkDeLz_DecodeBihSym4x(ctx);
	vb=TkDeLz_DecodeBihSym4x(ctx);
	return(va|(vb<<32));
}

int TkDeLz_DecodeBihBlock(TkDeLz_BihState *ctx, byte *buf, int n)
{
	byte *cs, *ct;
	u64 v0, v1;
	u32 lb, lb1;
	int i, nn3;
	
	cs=ctx->cs;
//	lb=*(u32 *)cs;
	lb=tkdelz_getu32le(cs);
	cs+=4;
	
	ctx->csa0=cs;
	cs+=(lb>> 0)&255;
	ctx->csa1=cs;
	cs+=(lb>> 8)&255;
	ctx->csa2=cs;
	cs+=(lb>>16)&255;
	ctx->csa3=cs;
	cs+=(lb>>24)&255;

	ctx->cs=cs;
	
	ctx->pos0=0;	ctx->pos1=0;
	ctx->pos2=0;	ctx->pos3=0;
	ct=buf;

#if 1
	nn3=n&(~3);
	for(i=0; i<nn3; i+=4)
//	for(i=0; i<n; i+=4)
	{
		v0=TkDeLz_DecodeBihSym8x(ctx);
		v1=TkDeLz_DecodeBihSym8x(ctx);
		tkdelz_setu64le(ct+0, v0);
		tkdelz_setu64le(ct+8, v1);
		ct+=16;

#if 0
		lb=TkDeLz_DecodeBihSym4x(ctx);
		lb1=TkDeLz_DecodeBihSym4x(ctx);
		tkdelz_setu32le(ct+0, lb);
		tkdelz_setu32le(ct+4, lb1);

		lb=TkDeLz_DecodeBihSym4x(ctx);
		lb1=TkDeLz_DecodeBihSym4x(ctx);
		tkdelz_setu32le(ct+8, lb);
		tkdelz_setu32le(ct+12, lb1);
		ct+=16;
#endif
	}
#endif

#if 1
//	for(i=0; i<n; i++)
	for(; i<n; i++)
	{
		lb=TkDeLz_DecodeBihSym4x(ctx);
		tkdelz_setu32le(ct, lb);
//		*(u32 *)ct=lb;
		ct+=4;
	}
#endif

	return(0);
}

byte *TkDeLz_DecodeBihChunk(TkDeLz_BihState *ctx, byte *sptr)
{
	byte *ct, *cs;
	int i, j, k, n;
	
	cs=sptr;
//	j=*(u16 *)cs;
	j=tkdelz_getu16le(cs);
	cs+=2;
	ctx->cs=cs;
	
	if((j>>14)==0)
	{
		k=(j&16383)*4;
		memcpy(ctx->bbuf, cs, k);
		cs+=k;
		ctx->cs=cs;
		ctx->bbufsz=k;
		return(cs);
	}else
		if((j>>14)==1)
	{
//		TkDeLz_BeginReadBits(ctx, ctx->cs);
		TkDeLz_ReadPackedLengths(ctx, ctx->hltab);
		TkDeLz_SetupTableLengths(ctx, ctx->htab, ctx->hltab);
//		ctx->cs=TkDeLz_EndReadBits(ctx);
		n=j&16383;
		ctx->bbufsz=n*4;
	}else
		if((j>>14)==2)
	{
		/* Reuse prior table. */
		n=j&16383;
		ctx->bbufsz=n*4;
	}else
	{
		printf("TkDeLz_DecodeBihChunk: Bad Chunk\n");
		return(cs);
	}
	
	ct=ctx->bbuf;
	for(i=0; i<n; i+=128)
	{
		j=n-i;
		if(j>128)
			j=128;
		TkDeLz_DecodeBihBlock(ctx, ct, j);
		ct+=j*4;
	}

	return(ctx->cs);
}

int TkDeLz_InitBihContext(TkDeLz_BihState *ctx, int lg2sz)
{
	if((ctx->magic1!=0x13245768) || (ctx->magic2!=0x13245768))
	{
		memset(ctx, 0, sizeof(TkDeLz_BihState));
		ctx->magic1=0x13245768;
		ctx->magic2=0x13245768;
	}

	if(!ctx->bbuf)
	{
		ctx->bbuf=malloc(1<<lg2sz);
		memset(ctx->bbuf, 0, 1<<lg2sz);
	}

	return(0);
}



int TkDeLz_EncodeBihSym4x(TkDeLz_BihState *ctx, u32 lb)
{
	int b0, b1, b2, b3;
	int p0, p1, p2, p3;
	u32 w0, w1, w2, w3;
	
	b0=ctx->hetab[(lb>> 0)&255];
	b1=ctx->hetab[(lb>> 8)&255];
	b2=ctx->hetab[(lb>>16)&255];
	b3=ctx->hetab[(lb>>24)&255];
	p0=ctx->pos0;	p1=ctx->pos1;
	p2=ctx->pos2;	p3=ctx->pos3;
	w0=ctx->win0;	w1=ctx->win1;
	w2=ctx->win2;	w3=ctx->win3;
	w0|=(b0&0xFFFF)<<p0;
	w1|=(b1&0xFFFF)<<p1;
	w2|=(b2&0xFFFF)<<p2;
	w3|=(b3&0xFFFF)<<p3;
	p0+=b0>>16;		p1+=b1>>16;
	p2+=b2>>16;		p3+=b3>>16;
	
	while(p0>=8)
		{ *ctx->csa0++=w0; w0>>=8; p0-=8; }
	while(p1>=8)
		{ *ctx->csa1++=w1; w1>>=8; p1-=8; }
	while(p2>=8)
		{ *ctx->csa2++=w2; w2>>=8; p2-=8; }
	while(p3>=8)
		{ *ctx->csa3++=w3; w3>>=8; p3-=8; }

	ctx->pos0=p0;	ctx->pos1=p1;
	ctx->pos2=p2;	ctx->pos3=p3;
	ctx->win0=w0;	ctx->win1=w1;
	ctx->win2=w2;	ctx->win3=w3;
	
	return(0);
}

int TkDeLz_EncodeBihFlush4x(TkDeLz_BihState *ctx)
{
	int b0, b1, b2, b3;
	int p0, p1, p2, p3;
	u32 w0, w1, w2, w3;
	
	p0=ctx->pos0;	p1=ctx->pos1;
	p2=ctx->pos2;	p3=ctx->pos3;
	w0=ctx->win0;	w1=ctx->win1;
	w2=ctx->win2;	w3=ctx->win3;
	
	while(p0>0)
		{ *ctx->csa0++=w0; w0>>=8; p0-=8; }
	while(p1>0)
		{ *ctx->csa1++=w1; w1>>=8; p1-=8; }
	while(p2>0)
		{ *ctx->csa2++=w2; w2>>=8; p2-=8; }
	while(p3>0)
		{ *ctx->csa3++=w3; w3>>=8; p3-=8; }

	ctx->pos0=p0;	ctx->pos1=p1;
	ctx->pos2=p2;	ctx->pos3=p3;
	ctx->win0=w0;	ctx->win1=w1;
	ctx->win2=w2;	ctx->win3=w3;
	
	return(0);
}



int PDZ2_BalanceTree_r(short *nodes, short *nlen, int root, int h, int ml)
{
	int h0, h1, h2, h3;
	int l0, l1, l2;

	if(root<0)
	{
//		printf("L");
		return(0);
	}

//	printf("{");

	h1=PDZ2_BalanceTree_r(nodes, nlen, nodes[root*2+0], h+1, ml);
	h2=PDZ2_BalanceTree_r(nodes, nlen, nodes[root*2+1], h+1, ml);
	h0=((h1>h2)?h1:h2)+1;
	nlen[root]=h0;

	if((h+h0)<=ml)	//depth limit not exceeded
	{
//		printf("}");
		return(h0);
	}

	//ok, so part of the tree is too deep
//	if((h1+1)<h2)
	if(h1<h2)
	{
		l0=nodes[root*2+1];
//		if(l0<0)return(h0);	//can't rebalance leaves

		l1=nodes[l0*2+1];
		nodes[l0*2+1]=nodes[l0*2+0];
		nodes[l0*2+0]=nodes[root*2+0];
		nodes[root*2+0]=l0;
		nodes[root*2+1]=l1;
	}else
//		if((h2+1)<h1)
		if(h2<h1)
	{
		l0=nodes[root*2+0];
//		if(l0<0)return(h0);	//can't rebalance leaves

		l1=nodes[l0*2+0];
		nodes[l0*2+0]=nodes[l0*2+1];
		nodes[l0*2+1]=nodes[root*2+1];
		nodes[root*2+0]=l1;
		nodes[root*2+1]=l0;
	}else
	{
//		printf("bal}");
		//rotating would be ineffective or would make things worse...
		return(h0);
	}

	//recalc depth of modified sub-tree
	l1=nodes[l0*2+0];
	l2=nodes[l0*2+1];
	h1=(l1<0)?0:nlen[l1];
	h2=(l2<0)?0:nlen[l2];
	h3=((h1>h2)?h1:h2)+1;
	nlen[l0]=h3;

	//recalc height of root node
	l1=nodes[root*2+0];
	l2=nodes[root*2+1];
	h1=(l1<0)?0:nlen[l1];
	h2=(l2<0)?0:nlen[l2];
	h0=((h1>h2)?h1:h2)+1;
	nlen[root]=h0;

//	printf("rebal}");

	return(h0);
}

void PDZ2_CalcLengths_r(short *nodes, byte *cl, int root, int h)
{
	if(root<0)
	{
		cl[(-root)-1]=h;
		return;
	}

	PDZ2_CalcLengths_r(nodes, cl, nodes[root*2+0], h+1);
	PDZ2_CalcLengths_r(nodes, cl, nodes[root*2+1], h+1);
}

int PDZ2_BuildLengths(int *stat, int nc, byte *cl, int ml)
{
	static short nodes[1024], nlen[512];
	static short roots[512], clen[512];
	static int cnts[512];
	int nr, nn;
	int i, j, k, l;

	nr=0; nn=0;
	for(i=0; i<nc; i++)
	{
		if(!stat[i])continue;
		roots[nr]=-(i+1);
		cnts[nr]=stat[i];
		clen[nr]=0;
		nr++;
	}

	for(i=0; i<nc; i++)cl[i]=0;
	if(!nr)
	{
		printf("empty tree\n");
		return(-1);
	}


	while(nr>1)
	{
		if(cnts[0]>=cnts[1]) { j=0; k=1; }
			else { j=1; k=0; }
		for(i=2; i<nr; i++)
		{
			if(cnts[i]<=cnts[k])
			{
				j=k; k=i;
				continue;
			}
			if(cnts[i]<=cnts[j])
			{
				j=i;
				continue;
			}
		}

		nlen[nn]=((clen[j]>clen[k])?clen[j]:clen[k])+1;
		nodes[nn*2+0]=roots[j];
		nodes[nn*2+1]=roots[k];

		roots[nr]=nn;
		cnts[nr]=cnts[j]+cnts[k];
		clen[nr]=nlen[nn];

//		printf("%d %d %d\n", cnts[j], cnts[k], cnts[nr]);

		nn++; nr++;

		l=0;
		for(i=0; i<nr; i++)
		{
			if((i==j) || (i==k))continue;
			roots[l]=roots[i];
			cnts[l]=cnts[i];
			clen[l]=clen[i];
			l++;
		}
		nr=l;
	}

	l=roots[0];
	j=clen[0];
	k=j;

	i=4;
	while((i--) && (k>ml))
		k=PDZ2_BalanceTree_r(nodes, nlen, l, 0, ml);
	if(k>ml)
	{
		printf("tree balance failure\n");
		printf("tree depth %d, org %d, %d nodes\n", k, j, nn);
		return(-2);
	}

	PDZ2_CalcLengths_r(nodes, cl, l, 0);
	return(0);
}

int PDZ2_BuildLengthsAdjust(int *stat, int nc, byte *cl, int ml)
{
	int i, j;

	while(1)
	{
		j=PDZ2_BuildLengths(stat, nc, cl, ml);
		if(j<0)
			printf("PDZ2_BuildLengthsAdjust: Huff Fail %d\n", j);

		for(i=0; i<nc; i++)
			if(stat[i] && !cl[i])
				break;
		if(i>=nc)break;

		printf("PDZ2_BuildLengthsAdjust: Fiddle Adjust\n");
		for(i=0; i<nc; i++)
			stat[i]++;
		continue;
	}
	return(0);
}

int TkDeLz_BuildLengths(TkDeLz_BihState *ctx, int *stat, byte *cls)
{
	int i, j, k;

//	PDZ2_BuildLengthsAdjust(stat, 256, cls, 12);
	PDZ2_BuildLengthsAdjust(stat, 256, cls, TKDELZ_HTABNB);
	
//	for(i=0; i<256; i++)
//		cls[i]=8;

	return(0);
}

void TkDeLz_SetupEncTableLengths(TkDeLz_BihState *ctx, u32 *htab, byte *cls)
{
	int c, l, tc;
	int i, j, k;

	for(i=0; i<256; i++)
		htab[i]=0;
	
	c=0;
	for(l=1; l<14; l++)
	{
		for(i=0; i<256; i++)
			if(cls[i]==l)
		{
			tc=c<<(13-l);
			k=TkDeLz_TransposeWord(tc)>>3;
			htab[i]=(l<<16)|k;
			c++;
		}
		c=c<<1;
	}
	
//	if(c>8192)
//		__debugbreak();
}

int TkDeLz_EncodeBihPackedLengths(TkDeLz_BihState *ctx, byte *cls)
{
	byte *ct;
	int i, j, k, l;
	
	ct=ctx->cs; l=-1;
	i=0;
	while(i<256)
	{
#if 1
		for(j=i; j<256; j++)
			if(cls[j]!=0)
				break;
		k=(j-i)>>1;
		if(k>=2)
		{
			if(k>17)
				k=17;
			*ct++=0xD0+(k-2);
			i+=k*2;
			continue;
		}

		for(j=i; j<256; j++)
			if(cls[j]!=l)
				break;
		k=(j-i)>>1;
		if(k>=2)
		{
			if(k>17)
				k=17;
			*ct++=0xE0+(k-2);
			i+=k*2;
			continue;
		}
#endif

		k=cls[i+0];
		l=cls[i+1];
		*ct++=(k*14)+l;
		i+=2;
		continue;
	}
	ctx->cs=ct;
	
	return(0);
}

#if 0
int TkDeLz_SafeMemcpy(byte *dst, byte *src, int sz)
{
	byte *cs, *ct, *cse;
	
	cs=src; cse=cs+sz;
	ct=dst;
	
	while(cs<cse)
		{ *ct++=*cs++; }
}
#endif

int TkDeLz_EncodeBihBlock(TkDeLz_BihState *ctx, byte *buf, int n)
{
	byte *cs, *ct, *cs0, *cs1, *cs2, *cs3;
	int l0, l1, l2, l3;
	u32 lb;
	int i;
	
	cs=ctx->cs;
//	lb=*(u32 *)cs;
//	cs+=4;
	
	cs0=cs+  0+16+(0*32);
	cs1=cs+256+16+(1*32);
	cs2=cs+512+16+(2*32);
	cs3=cs+768+16+(3*32);
	ctx->csa0=cs0;
	ctx->csa1=cs1;
	ctx->csa2=cs2;
	ctx->csa3=cs3;
	
	ctx->pos0=0;	ctx->pos1=0;
	ctx->pos2=0;	ctx->pos3=0;
	ct=buf;
	
	for(i=0; i<n; i++)
	{
//		lb=*(u32 *)ct;
		lb=tkdelz_getu32le(ct);
		ct+=4;
		TkDeLz_EncodeBihSym4x(ctx, lb);
	}
	TkDeLz_EncodeBihFlush4x(ctx);

	l0=ctx->csa0-cs0;
	l1=ctx->csa1-cs1;
	l2=ctx->csa2-cs2;
	l3=ctx->csa3-cs3;
	lb=l0|(l1<<8)|(l2<<16)|(l3<<24);
	
	if((l0|l1|l2|l3)>>8)
	{
		printf("TkDeLz_EncodeBihBlock: Block Size Fail\n");
	}
	
//	*(u32 *)cs=lb;
	tkdelz_setu32le(cs, lb);
	cs+=4;
	memmove(cs, cs0, l0);	cs+=l0;
	memmove(cs, cs1, l1);	cs+=l1;
	memmove(cs, cs2, l2);	cs+=l2;
	memmove(cs, cs3, l3);	cs+=l3;

	ctx->cs=cs;

	return(0);
}

byte *TkDeLz_EncodeBihChunk(TkDeLz_BihState *ctx, byte *sptr,
	byte *tbuf, int tsz)
{
	byte hltab2[256];
	byte *cs, *ct;
	int i, j, k, n, tg, reuse;

	n=(tsz+3)/4;

	for(i=0; i<256; i++)
		{ ctx->hstab[i]=0; }
	for(i=0; i<(n*4); i++)
		ctx->hstab[tbuf[i]]++;

	for(i=0; i<256; i++)
	{
		if(ctx->hltab[i] && !ctx->hstab[i])
			ctx->hstab[i]=1;
	}

//	TkDeLz_BuildLengths(ctx, ctx->hstab, ctx->hltab);
	TkDeLz_BuildLengths(ctx, ctx->hstab, hltab2);

	reuse=0;
	j=0;	k=0;
	for(i=0; i<256; i++)
	{
		if(ctx->hstab[i] && !ctx->hltab[i])
			break;
		j+=ctx->hstab[i]*ctx->hltab[i];
		k+=ctx->hstab[i]*hltab2[i];
	}

	if((i>=256) && ((k*1.1)>=j))
//	if((i>=256) && ((k*1.05)>=j))
//	if((i>=256) && ((k*1.025)>=j))
	{
		reuse=1;
	}else
	{
		reuse=0;
		memcpy(ctx->hltab, hltab2, 256);
	}

	k=0;
	for(i=0; i<256; i++)
	{
		if(ctx->hstab[i] && !ctx->hltab[i])
			{ printf("TkDeLz_EncodeBihChunk: Missing Symbol"); }
	
		k+=ctx->hstab[i]*ctx->hltab[i];
	}
	k=(k+7)>>3;

	if((k*1.15)>=tsz)
//	if(1)
	{
		ct=sptr;
		tg=0x0000|n;
		tkdelz_setu16le(ct, tg);
		ct+=2;

		memcpy(ct, tbuf, n*4);
		ct+=n*4;

		return(ct);
	}

	ct=sptr;
	
	if(reuse)
	{
		tg=0x8000|n;
		tkdelz_setu16le(ct, tg);
		ct+=2;
		ctx->cs=ct;
	}else
	{
		tg=0x4000|n;
		tkdelz_setu16le(ct, tg);
		ct+=2;
		
		ctx->cs=ct;
		TkDeLz_EncodeBihPackedLengths(ctx, ctx->hltab);
		TkDeLz_SetupEncTableLengths(ctx, ctx->hetab, ctx->hltab);
	}

//	ct=ctx->cs;

	cs=tbuf;
	for(i=0; i<n; i+=128)
	{
		j=n-i;
		if(j>128)
			j=128;
		TkDeLz_EncodeBihBlock(ctx, tbuf+i*4, j);
	}

	ct=ctx->cs;
	return(ct);
}
