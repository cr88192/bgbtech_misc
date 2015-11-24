/*
Bitwise Arithmetic Coder
 */

#include <btlzazip.h>

// u16 bgbbtj_bitarith_divtab[65536];	//division table
byte bgbbtj_bitarith_transtab8[256];	//byte transpose table
u16 bgbbtj_bitarith_transtab9[512];	//byte transpose table

btlza_prob btlza_weightadjust[512];		//weight adjustment tables

void *bgbbtj_btlza_mutex=NULL;
BGBBTJ_BTLZA_Context *bgbbtj_btlza_freectx=NULL;

#if defined(BGBBTJ_DRV) || defined(BGBBTJ_NODY)

void BGBBTJ_BTLZA_Lock()
{
}

void BGBBTJ_BTLZA_Unlock()
{
}

#else

void BGBBTJ_BTLZA_Lock()
{
	thLockFastMutex(bgbbtj_btlza_mutex);
}

void BGBBTJ_BTLZA_Unlock()
{
	thUnlockFastMutex(bgbbtj_btlza_mutex);
}

#endif

void BTLZA_BitArith_Init()
{
	static int init=0;
	int i, j, k;

	if(init)return;
	init=1;

#if !defined(BGBBTJ_DRV) && !defined(BGBBTJ_NODY)
	bgbbtj_btlza_mutex=thFastMutex();
#endif

//	for(i=0; i<256; i++)
//		for(j=0; j<256; j++)
//	{
//		k=((i+1)<<BTLZA_BITARITH_WBITS)/(i+j+2);
//		bgbbtj_bitarith_divtab[i+(j<<8)]=k;
//	}

	for(i=0; i<256; i++)
	{
		j=((i&1)<<7)|((i&2)<<5)|((i&4)<<3)|((i&8)<<1);
		j|=((i&16)>>1)|((i&32)>>3)|((i&64)>>5)|((i&128)>>7);
		bgbbtj_bitarith_transtab8[i]=j;
	}

	for(i=0; i<512; i++)
	{
		j=((i&1)<<7)|((i&2)<<5)|((i&4)<<3)|((i&8)<<1);
		j|=((i&16)>>1)|((i&32)>>3)|((i&64)>>5)|((i&128)>>7);
		bgbbtj_bitarith_transtab9[i]=j;
	}
	
	for(i=0; i<256; i++)
	{
//		btlza_weightadjust[(0<<8)+i]=i+((256-i)>>6);
//		btlza_weightadjust[(1<<8)+i]=i-(i>>6);

//		btlza_weightadjust[(0<<8)+i]=i+((256-i)>>6);
//		btlza_weightadjust[(1<<8)+i]=i-(i>>6);

//		btlza_weightadjust[(0<<8)+i]=i+((256-i)/48);
//		btlza_weightadjust[(1<<8)+i]=i-(i/48);

//		btlza_weightadjust[(0<<8)+i]=i+sqrt(255-i)*0.5;
//		btlza_weightadjust[(1<<8)+i]=i-sqrt(i)*0.5;

//		btlza_weightadjust[(0<<8)+i]=i+2;
//		btlza_weightadjust[(1<<8)+i]=i-2;

		j=i+((256-i)>>5);
		k=i-(i>>5);
//		j=i+((256-i)>>6);
//		k=i-(i>>6);
//		if(j==i)j=j+1;
//		if(k==i)k=k-1;
		btlza_weightadjust[(0<<8)+i]=j;
		btlza_weightadjust[(1<<8)+i]=k;
	}
	btlza_weightadjust[(0<<8)+255]=255;
	btlza_weightadjust[(1<<8)+0]=0;
}

BGBBTJ_API BGBBTJ_BTLZA_Context *BTLZA_AllocContext(void)
{
	BGBBTJ_BTLZA_Context *ctx;
	int i, j;

	BTLZA_BitArith_Init();

	BGBBTJ_BTLZA_Lock();
	if(bgbbtj_btlza_freectx)
	{
		ctx=bgbbtj_btlza_freectx;
		bgbbtj_btlza_freectx=ctx->next;
		BGBBTJ_BTLZA_Unlock();
		return(ctx);
	}
	BGBBTJ_BTLZA_Unlock();
	
	ctx=btlza_malloc(sizeof(BGBBTJ_BTLZA_Context));
	ctx->cs=NULL; ctx->ct=NULL;
	
	return(ctx);
}

BGBBTJ_API void BTLZA_FreeContext(
	BGBBTJ_BTLZA_Context *ctx)
{
#if 1
	ctx->rmin=0;	ctx->rmax=0;
	ctx->rval=0;	ctx->wctx=0;
	ctx->cs=NULL;	ctx->ct=NULL;

	BGBBTJ_BTLZA_Lock();
	ctx->next=bgbbtj_btlza_freectx;
	bgbbtj_btlza_freectx=ctx;
	BGBBTJ_BTLZA_Unlock();
#endif

#if 0
	if(ctx->lz_wbuf)btlza_free(ctx->lz_wbuf);
	if(ctx->lz_lbuf)btlza_free(ctx->lz_lbuf);
	if(ctx->lz_hash)btlza_free(ctx->lz_hash);

	if(ctx->lz_tbuf)btlza_free(ctx->lz_tbuf);
	if(ctx->lz_mbuf)btlza_free(ctx->lz_mbuf);

	if(ctx->mdl_lit)btlza_free(ctx->mdl_lit);
	if(ctx->mdl_dist)btlza_free(ctx->mdl_dist);
	if(ctx->mdl_xbits)btlza_free(ctx->mdl_xbits);
	btlza_free(ctx);
#endif
}

int BTLZA_BitArith_InputByte(BGBBTJ_BTLZA_Context *ctx)
{
//	if(ctx->cs>ctx->cse)
//		return(0);
	return(*ctx->cs++);
}

void BTLZA_BitArith_OutputByte(BGBBTJ_BTLZA_Context *ctx, int i)
{
//	if(ctx->ct>ctx->cte)
//		return;
	*ctx->ct++=i;
}

void BTLZA_BitArith_NormalizeOutput(BGBBTJ_BTLZA_Context *ctx)
{
	while(!((ctx->rmin^ctx->rmax)>>24))
//	while((ctx->rmax-ctx->rmin)<16777216)
	{
		BTLZA_BitArith_OutputByte(ctx, ctx->rmin>>24);
		ctx->rmin<<=8;
		ctx->rmax<<=8;
		ctx->rmax|=255;
	}
}

void BTLZA_BitArith_NormalizeInput(BGBBTJ_BTLZA_Context *ctx)
{
#if 0
	while(!((ctx->rmin^ctx->rmax)>>24))
	{
		ctx->rmin<<=8;
		ctx->rmax<<=8;
		ctx->rmax|=255;
		ctx->rval<<=8;
		ctx->rval|=BTLZA_BitArith_InputByte(ctx);
	}
#endif

	while(!((ctx->rmin^ctx->rmax)>>24))
//	while((ctx->rmax-ctx->rmin)<16777216)
	{
		ctx->rmin<<=8;
		ctx->rmax=(ctx->rmax<<8)|255;
		ctx->rval=(ctx->rval<<8)|(*ctx->cs++);
	}

}

void BTLZA_BitArith_FlushWBits(BGBBTJ_BTLZA_Context *ctx)
{
	while((ctx->rmin!=BTLZA_BITARITH_LOWER) ||
		(ctx->rmax!=BTLZA_BITARITH_UPPER))
	{
		BTLZA_BitArith_OutputByte(ctx, ctx->rmin>>24);

		ctx->rmin<<=8;
		ctx->rmax<<=8;
		ctx->rmax|=255;
	}
}


void BTLZA_BitArith_OutputFixedBit(BGBBTJ_BTLZA_Context *ctx, int i)
{
	u32 r, r2, v;
	int j;

	r=ctx->rmax-ctx->rmin;
	v=ctx->rmin+(r>>1);

	if(i)ctx->rmin=v+1;
	else ctx->rmax=v;
	BTLZA_BitArith_NormalizeOutput(ctx);
}

int BTLZA_BitArith_InputFixedBit(BGBBTJ_BTLZA_Context *ctx)
{
	u32 r, r2, v, i;

	r=ctx->rmax-ctx->rmin;
	v=ctx->rmin+(r>>1);

	i=(ctx->rval>v);
	if(i)ctx->rmin=v+1;
	else ctx->rmax=v;

	BTLZA_BitArith_NormalizeInput(ctx);
	return(i);
}

void BTLZA_BitArith_OutputBit(BGBBTJ_BTLZA_Context *ctx,
	int i, u32 w)
{
	u32 r, r2, v;
	int j;

	r=ctx->rmax-ctx->rmin;
	v=ctx->rmin+(r>>BTLZA_BITARITH_WBITS)*w;

	if(i)ctx->rmin=v+1;
	else ctx->rmax=v;

	BTLZA_BitArith_NormalizeOutput(ctx);
}

int BTLZA_BitArith_InputBit(BGBBTJ_BTLZA_Context *ctx, u32 w)
{
	u32 r, r2, v, i;

	r=ctx->rmax-ctx->rmin;
	v=ctx->rmin+(r>>BTLZA_BITARITH_WBITS)*w;

	i=(ctx->rval>v);
	if(i)ctx->rmin=v+1;
	else ctx->rmax=v;

	BTLZA_BitArith_NormalizeInput(ctx);
	return(i);
}

void BTLZA_BitArith_OutputModelBit(BGBBTJ_BTLZA_Context *ctx,
	int i, byte *mctx)
{
	u32 r, v, w;
	int j;

	r=ctx->rmax-ctx->rmin; w=*mctx;
	v=ctx->rmin+(r>>BTLZA_BITARITH_WBITS)*w;

	if(i)
	{
		ctx->rmin=v+1;
//		w=w+(((1<<BTLZA_BITARITH_WBITS)-w)>>BTLZA_BITARITH_WASHR);
//		w=w-(w>>BTLZA_BITARITH_WASHR);
//		w=w-(w+(1<<(BTLZA_BITARITH_WASHR-1))>>BTLZA_BITARITH_WASHR);
		w=btlza_weightadjust[256+w];
		*mctx=w;
	}else
	{
		ctx->rmax=v;
//		w=w-(w>>BTLZA_BITARITH_WASHR);
//		w=w+(((1<<BTLZA_BITARITH_WBITS)-w)>>BTLZA_BITARITH_WASHR);
//		w=w+(((1<<BTLZA_BITARITH_WBITS)+
//			(1<<(BTLZA_BITARITH_WASHR-1))-w)>>BTLZA_BITARITH_WASHR);
		w=btlza_weightadjust[w];
		*mctx=w;
	}

	BTLZA_BitArith_NormalizeOutput(ctx);
}

int BTLZA_BitArith_InputModelBit(BGBBTJ_BTLZA_Context *ctx, byte *mctx)
{
	u32 r, v, w;
	int i, j, k;

	r=ctx->rmax-ctx->rmin; w=*mctx;
	v=ctx->rmin+(r>>BTLZA_BITARITH_WBITS)*w;

	i=(ctx->rval>v);
	if(i)
	{
		ctx->rmin=v+1;
//		ctx->rval-=v;
//		w=w+(((1<<BTLZA_BITARITH_WBITS)-w)>>BTLZA_BITARITH_WASHR);
//		w=w-(w>>BTLZA_BITARITH_WASHR);
//		w=w-(w+(1<<(BTLZA_BITARITH_WASHR-1))>>BTLZA_BITARITH_WASHR);
		w=btlza_weightadjust[256+w];
		*mctx=w;
	}else
	{
		ctx->rmax=v;
//		w=w-(w>>BTLZA_BITARITH_WASHR);
//		w=w+(((1<<BTLZA_BITARITH_WBITS)-w)>>BTLZA_BITARITH_WASHR);
//		w=w+(((1<<BTLZA_BITARITH_WBITS)+
//			(1<<(BTLZA_BITARITH_WASHR-1))-w)>>BTLZA_BITARITH_WASHR);
		w=btlza_weightadjust[w];
		*mctx=w;
	}

	BTLZA_BitArith_NormalizeInput(ctx);
	return(i);
}

void BTLZA_BitArith_OutputFixedBits(BGBBTJ_BTLZA_Context *ctx,
	int v, int n)
{
	int i, j, k, l;

	i=n;
	while(i--)
		{ BTLZA_BitArith_OutputFixedBit(ctx, (v>>i)&1); }
}

int BTLZA_BitArith_InputFixedBits(BGBBTJ_BTLZA_Context *ctx, int n)
{
	int i, j, k, l;

	i=n; j=0;
	while(i--)
	{
		k=BTLZA_BitArith_InputFixedBit(ctx);
		j=(j<<1)|k;
	}
	return(j);
}

int BTLZA_BitArith_InputContextFixedBits(BGBBTJ_BTLZA_Context *ctx, int n)
{
	int i, j, k, l;

	i=n; j=ctx->wctx;
	while(i--)
	{
		k=BTLZA_BitArith_InputFixedBit(ctx);
		j=(j<<1)|k;
	}
	ctx->wctx=j;
	return(j&((1<<n)-1));
}

int BTLZA_BitArith_InputContextFixedBitsLE(BGBBTJ_BTLZA_Context *ctx, int n)
{
	int i, j, k, l;

	i=n; j=ctx->wctx; l=0;
	while(i--)
	{
		k=BTLZA_BitArith_InputFixedBit(ctx);
		j=(j<<1)|k;
		l|=k<<i;
	}
	ctx->wctx=j;
	return(l);
}

void BTLZA_BitArith_OutputContextModelBits(BGBBTJ_BTLZA_Context *ctx,
	int v, int n, btlza_prob *mdl, int ctxmask)
{
	int i, j, k, l;

	i=n; j=ctx->wctx; k=ctxmask;
	while(i--)
	{
		l=(v>>i)&1;
		BTLZA_BitArith_OutputModelBit(ctx, l, mdl+btlza_ctxindex(j, k));
		j=(j<<1)|l;
	}
	ctx->wctx=j;
}

int BTLZA_BitArith_InputContextModelBits(BGBBTJ_BTLZA_Context *ctx,
	int n, btlza_prob *mdl, int ctxmask)
{
	int i, j, k, l;

	i=n; j=ctx->wctx; k=ctxmask;
	while(i--)
	{
		l=BTLZA_BitArith_InputModelBit(ctx, mdl+btlza_ctxindex(j, k));
		j=(j<<1)|l;
	}
	ctx->wctx=j;
	return(j&((1<<n)-1));
}

void BTLZA_BitArith_OutputExtraBits(BGBBTJ_BTLZA_Context *ctx,
	int v, int n, btlza_prob *mdl, int ctxmask)
{
	switch(n)
	{
	case 0:
		break;
	case 1: case 2:
	case 3: case 4:
		BTLZA_BitArith_OutputContextModelBits(ctx,
			v, n, mdl, ctxmask);
		break;
	default:
		BTLZA_BitArith_OutputContextModelBits(ctx,
			v>>(n-4), 4, mdl, ctxmask);
		BTLZA_BitArith_OutputFixedBits(ctx, v, n-4);
		break;
	}
}

void BTLZA_BitArith_OutputContextModelBitsLE(BGBBTJ_BTLZA_Context *ctx,
	int v, int n, btlza_prob *mdl, int ctxmask)
{
	int i, j, k, l;

	i=n; j=ctx->wctx;
//	while(i--)
	for(i=0; i<n; i++)
	{
		l=(v>>i)&1;
		BTLZA_BitArith_OutputModelBit(ctx, l,
			mdl+btlza_ctxindex(j, ctxmask));
		j=(j<<1)|l;
	}
	ctx->wctx=j;
}

int BTLZA_BitArith_InputContextModelBitsLE(BGBBTJ_BTLZA_Context *ctx,
	int n, btlza_prob *mdl, int ctxmask)
{
	int i, j, k, l;
	int v;

	i=n; j=ctx->wctx; v=0;
	while(i--)
	{
		l=BTLZA_BitArith_InputModelBit(ctx, mdl+
			btlza_ctxindex(j, ctxmask));
		j=(j<<1)|l;
		v|=(l<<i);
	}
	ctx->wctx=j;
	return(v);
}

int BTLZA_BitArith_InputExtraBits(BGBBTJ_BTLZA_Context *ctx,
	int n, btlza_prob *mdl, int ctxmask)
{
	int i, j;
	int v;

	switch(n)
	{
	case 0: v=0; break;
	case 1: case 2:
	case 3: case 4:
//		ctx->wctx&=ctxmask;
		v=BTLZA_BitArith_InputContextModelBits(ctx,
			n, mdl, ctxmask);
		break;
	default:
//		ctx->wctx&=ctxmask;
		i=BTLZA_BitArith_InputContextModelBits(ctx,
			4, mdl, ctxmask);
		j=BTLZA_BitArith_InputFixedBits(ctx, n-4);
		v=(i<<(n-4))|j;
		break;
	}
	return(v);
}

int BTLZA_BitArith_SetupEncode(BGBBTJ_BTLZA_Context *ctx, byte *out, int sz)
{
	ctx->ct=out;
	ctx->cte=out+sz;
	ctx->rmin=BTLZA_BITARITH_LOWER;
	ctx->rmax=BTLZA_BITARITH_UPPER;
	return(0);
}

int BTLZA_BitArith_SetupDecode(BGBBTJ_BTLZA_Context *ctx, byte *in, int sz)
{
	int i;

	ctx->cs=in;
	ctx->cse=in+sz;
	ctx->rmin=BTLZA_BITARITH_LOWER;
	ctx->rmax=BTLZA_BITARITH_UPPER;
//	ctx->range=BTLZA_BITARITH_UPPER;

	ctx->rval=BTLZA_BITARITH_LOWER;
	for(i=0; i<4; i++)
		ctx->rval=(ctx->rval<<8)|BTLZA_BitArith_InputByte(ctx);
	return(0);
}

int BTLZA_BitArith_SetupBeginEncode(BGBBTJ_BTLZA_Context *ctx)
{
	ctx->rmin=BTLZA_BITARITH_LOWER;
	ctx->rmax=BTLZA_BITARITH_UPPER;
	return(0);
}

int BTLZA_BitArith_SetupBeginDecode(BGBBTJ_BTLZA_Context *ctx)
{
	int i;

	if(ctx->rmax)
	{
		return(0);
	}

	ctx->rmin=BTLZA_BITARITH_LOWER;
	ctx->rmax=BTLZA_BITARITH_UPPER;
//	ctx->range=BTLZA_BITARITH_UPPER;

	ctx->rval=BTLZA_BITARITH_LOWER;
	for(i=0; i<4; i++)
		ctx->rval=(ctx->rval<<8)|BTLZA_BitArith_InputByte(ctx);
	return(0);
}

int BTLZA_BitArith_SetupEndDecode(BGBBTJ_BTLZA_Context *ctx)
{
	ctx->rmin=0;
	ctx->rmax=0;
	return(0);
}

int btlza_ctxindex(int ctx, int mask)
{
	int i;

//	i=((ctx>>8)*251)>>8;
//	return((ctx^(i<<8))&mask);
	
	i=((ctx>>16)*251)>>8;
	return((ctx^(i<<16))&mask);

//	return(ctx&mask);
}

void BTLZA_BitArith_EncodeSymbol6(BGBBTJ_BTLZA_Context *ctx, int v,
	btlza_prob *mdl, int ctxmask)
{
	int i, j, k, l;

	i=6; j=ctx->wctx; k=ctxmask;
	while(i--)
	{
		l=(v>>i)&1;
		BTLZA_BitArith_OutputModelBit(ctx, l, mdl+btlza_ctxindex(j, k));
		j=(j<<1)|l;
	}
	ctx->wctx=j;
}

void BTLZA_BitArith_EncodeSymbol8(BGBBTJ_BTLZA_Context *ctx, int v,
	btlza_prob *mdl, int ctxmask)
{
	int i, j, k, l;

	i=8; j=ctx->wctx; k=ctxmask;
	while(i--)
	{
		l=(v>>i)&1;
		BTLZA_BitArith_OutputModelBit(ctx, l, mdl+btlza_ctxindex(j, k));
		j=(j<<1)|l;
	}
	ctx->wctx=j;
}

void BTLZA_BitArith_EncodeSymbol9(BGBBTJ_BTLZA_Context *ctx, int v,
	btlza_prob *mdl, int ctxmask)
{
	int i, j, k, l;

	i=9; j=ctx->wctx; k=ctxmask;
	while(i--)
	{
		l=(v>>i)&1;
		BTLZA_BitArith_OutputModelBit(ctx, l, mdl+btlza_ctxindex(j, k));
		j=(j<<1)|l;
	}
	ctx->wctx=j;
}

void BTLZA_BitArith_EncodeSymbol8LE(BGBBTJ_BTLZA_Context *ctx, int v,
	btlza_prob *mdl, int ctxmask)
{
	int i, j, k, l;

	i=8; j=ctx->wctx; k=ctxmask;
//	while(i--)
	for(i=0; i<8; i++)
	{
		l=(v>>i)&1;
		BTLZA_BitArith_OutputModelBit(ctx, l, mdl+btlza_ctxindex(j, k));
		j=(j<<1)|l;
	}
	ctx->wctx=j;
}

int BTLZA_BitArith_DecodeSymbol6(BGBBTJ_BTLZA_Context *ctx,
	btlza_prob *mdl, int ctxmask)
{
	int i, j, k, v;

	j=ctx->wctx; k=ctxmask;
	i=BTLZA_BitArith_InputModelBit(ctx, mdl+btlza_ctxindex(j, k)); j=(j<<1)|i;
	i=BTLZA_BitArith_InputModelBit(ctx, mdl+btlza_ctxindex(j, k)); j=(j<<1)|i;
	i=BTLZA_BitArith_InputModelBit(ctx, mdl+btlza_ctxindex(j, k)); j=(j<<1)|i;
	i=BTLZA_BitArith_InputModelBit(ctx, mdl+btlza_ctxindex(j, k)); j=(j<<1)|i;
	i=BTLZA_BitArith_InputModelBit(ctx, mdl+btlza_ctxindex(j, k)); j=(j<<1)|i;
	i=BTLZA_BitArith_InputModelBit(ctx, mdl+btlza_ctxindex(j, k)); j=(j<<1)|i;
	ctx->wctx=j;
	return(j&0x3F);
}

int BTLZA_BitArith_DecodeSymbol8(BGBBTJ_BTLZA_Context *ctx,
	btlza_prob *mdl, int ctxmask)
{
	int i, j, k, v;

	j=ctx->wctx; k=ctxmask;
	i=BTLZA_BitArith_InputModelBit(ctx, mdl+btlza_ctxindex(j, k)); j=(j<<1)|i;
	i=BTLZA_BitArith_InputModelBit(ctx, mdl+btlza_ctxindex(j, k)); j=(j<<1)|i;
	i=BTLZA_BitArith_InputModelBit(ctx, mdl+btlza_ctxindex(j, k)); j=(j<<1)|i;
	i=BTLZA_BitArith_InputModelBit(ctx, mdl+btlza_ctxindex(j, k)); j=(j<<1)|i;
	i=BTLZA_BitArith_InputModelBit(ctx, mdl+btlza_ctxindex(j, k)); j=(j<<1)|i;
	i=BTLZA_BitArith_InputModelBit(ctx, mdl+btlza_ctxindex(j, k)); j=(j<<1)|i;
	i=BTLZA_BitArith_InputModelBit(ctx, mdl+btlza_ctxindex(j, k)); j=(j<<1)|i;
	i=BTLZA_BitArith_InputModelBit(ctx, mdl+btlza_ctxindex(j, k)); j=(j<<1)|i;
	ctx->wctx=j;
	return(j&0xFF);
}

int BTLZA_BitArith_DecodeSymbol9(BGBBTJ_BTLZA_Context *ctx,
	btlza_prob *mdl, int ctxmask)
{
	int i, j, k, v;

	j=ctx->wctx; k=ctxmask;
	i=BTLZA_BitArith_InputModelBit(ctx, mdl+btlza_ctxindex(j, k)); j=(j<<1)|i;
	i=BTLZA_BitArith_InputModelBit(ctx, mdl+btlza_ctxindex(j, k)); j=(j<<1)|i;
	i=BTLZA_BitArith_InputModelBit(ctx, mdl+btlza_ctxindex(j, k)); j=(j<<1)|i;
	i=BTLZA_BitArith_InputModelBit(ctx, mdl+btlza_ctxindex(j, k)); j=(j<<1)|i;
	i=BTLZA_BitArith_InputModelBit(ctx, mdl+btlza_ctxindex(j, k)); j=(j<<1)|i;
	i=BTLZA_BitArith_InputModelBit(ctx, mdl+btlza_ctxindex(j, k)); j=(j<<1)|i;
	i=BTLZA_BitArith_InputModelBit(ctx, mdl+btlza_ctxindex(j, k)); j=(j<<1)|i;
	i=BTLZA_BitArith_InputModelBit(ctx, mdl+btlza_ctxindex(j, k)); j=(j<<1)|i;
	i=BTLZA_BitArith_InputModelBit(ctx, mdl+btlza_ctxindex(j, k)); j=(j<<1)|i;
	ctx->wctx=j;
	return(j&0x1FF);
}

int BTLZA_BitArith_DecodeSymbol8LE(BGBBTJ_BTLZA_Context *ctx,
	btlza_prob *mdl, int ctxmask)
{
	int i, j, k, v;

	j=ctx->wctx; k=ctxmask;
	i=BTLZA_BitArith_InputModelBit(ctx, mdl+btlza_ctxindex(j, k)); j=(j<<1)|i;
	i=BTLZA_BitArith_InputModelBit(ctx, mdl+btlza_ctxindex(j, k)); j=(j<<1)|i;
	i=BTLZA_BitArith_InputModelBit(ctx, mdl+btlza_ctxindex(j, k)); j=(j<<1)|i;
	i=BTLZA_BitArith_InputModelBit(ctx, mdl+btlza_ctxindex(j, k)); j=(j<<1)|i;
	i=BTLZA_BitArith_InputModelBit(ctx, mdl+btlza_ctxindex(j, k)); j=(j<<1)|i;
	i=BTLZA_BitArith_InputModelBit(ctx, mdl+btlza_ctxindex(j, k)); j=(j<<1)|i;
	i=BTLZA_BitArith_InputModelBit(ctx, mdl+btlza_ctxindex(j, k)); j=(j<<1)|i;
	i=BTLZA_BitArith_InputModelBit(ctx, mdl+btlza_ctxindex(j, k)); j=(j<<1)|i;
	ctx->wctx=j;
	return(bgbbtj_bitarith_transtab8[j&0xFF]);
}

void BTLZA_BitArith_ResetContextWeights(
	BGBBTJ_BTLZA_Context *ctx)
{
	int i, j;

	j=1<<ctx->ctxbits;
	for(i=0; i<j; i++)ctx->mdl_lit[i]=1<<(BTLZA_BITARITH_WBITS-1);

	for(i=0; i<4096; i++)
	{
		ctx->mdl_dist[i]=1<<(BTLZA_BITARITH_WBITS-1);
		ctx->mdl_xbits[i]=1<<(BTLZA_BITARITH_WBITS-1);
	}

	ctx->wctx=0;
}

void BTLZA_BitArith_SetupContextBits(
	BGBBTJ_BTLZA_Context *ctx, int bits)
{
	int i, j;

	BTLZA_BitArith_Init();

	if(ctx->mdl_lit && (ctx->ctxbits==bits))
	{
		BTLZA_BitArith_ResetContextWeights(ctx);
		return;
	}

	if(ctx->mdl_lit && (ctx->ctxbits!=bits))
		{ btlza_free(ctx->mdl_lit); ctx->mdl_lit=NULL; }
	
	j=1<<bits;
	if(!ctx->mdl_lit)
		{ ctx->mdl_lit=btlza_malloc(j*sizeof(btlza_prob)); }
	ctx->ctxbits=bits;
	ctx->ctxmask=(1<<bits)-1;
	ctx->wctx=0;

	j=1<<12;
	if(!ctx->mdl_dist)
		{ ctx->mdl_dist=btlza_malloc(j*sizeof(btlza_prob)); }
	if(!ctx->mdl_xbits)
		{ ctx->mdl_xbits=btlza_malloc(j*sizeof(btlza_prob)); }

	BTLZA_BitArith_ResetContextWeights(ctx);
}


void BTLZA_BitArith_ResetRawContextWeights(
	BGBBTJ_BTLZA_Context *ctx)
{
	int i, j;

	j=1<<ctx->ctxbits_raw;
	for(i=0; i<j; i++)ctx->mdl_rbits[i]=1<<(BTLZA_BITARITH_WBITS-1);

	ctx->wctx=0;
}

void BTLZA_BitArith_SetupRawContextBits(
	BGBBTJ_BTLZA_Context *ctx, int bits)
{
	int i, j;

	BTLZA_BitArith_Init();

	if(ctx->mdl_rbits && (ctx->ctxbits_raw==bits))
	{
		BTLZA_BitArith_ResetRawContextWeights(ctx);
		return;
	}

	if(ctx->mdl_rbits && (ctx->ctxbits_raw!=bits))
		{ btlza_free(ctx->mdl_rbits); ctx->mdl_rbits=NULL; }
	
	j=1<<bits;
	if(!ctx->mdl_rbits)
		{ ctx->mdl_rbits=btlza_malloc(j*sizeof(btlza_prob)); }
	ctx->ctxbits_raw=bits;
	ctx->ctxmask_raw=(1<<bits)-1;
	ctx->wctx=0;

	BTLZA_BitArith_ResetRawContextWeights(ctx);
}

#if 0
BGBBTJ_API int BTLZA_BitArith_EncodeDataCtx(
	BGBBTJ_BTLZA_Context *ctx,
	byte *ibuf, int isz, byte *obuf, int osz, int flags)
{
	int i, j, k, l, ll, le;
	byte *cs, *ct, *cse, *cte, *s2;

	if(isz<0)return(-1);
	if(isz<=0)return(0);

	BTLZA_BitArith_Init();

	BTLZA_BitArith_SetupContextBits(ctx,
		BTLZA_BITARITH_CTXBITS);

	ct=obuf; cte=obuf+osz;
	le=flags&32; ll=flags&31;

	le=1;

	//Emit Header
	*ct++=0xBA;		//magic byte
	ct=BGBBTJ_EmitVLI(ct, ((ll-12)&15)|(le?16:0));
	ct=BGBBTJ_EmitVLI(ct, isz);

	BTLZA_BitArith_SetupEncode(ctx, ct, cte-ct);

	cs=ibuf;
	cse=ibuf+isz;
	i=0;

	while(cs<cse)
	{
		BTLZA_BitArith_EncodeSymbol8(ctx, *cs++,
			ctx->mdl_lit, ctx->ctxmask);
	}
	BTLZA_BitArith_FlushWBits(ctx);

	i=ctx->ct-obuf;
	return(i);
}

BGBBTJ_API int BTLZA_BitArith_DecodeDataCtx(
	BGBBTJ_BTLZA_Context *ctx,
	byte *ibuf, int isz, byte *obuf, int osz)
{
	int i, j, k, l, fl, ll, osz1, le;
	byte *cs, *ct, *cse, *cte, *s2;

	if(isz<=0)return(0);

	BTLZA_BitArith_Init();

	cs=ibuf; cse=ibuf+isz;

	if(*cs!=0xBA)
		return(-1);
	cs++;

	cs=BGBBTJ_DecodeVLI(cs, &fl);

	if(fl&(~31))
		return(-1);

	BTLZA_BitArith_SetupContextBits(ctx, 12+(fl&15));
	le=(fl&16)?1:0;

	cs=BGBBTJ_DecodeVLI(cs, &osz1);
	if(osz1>osz)return(-1);

	BTLZA_BitArith_SetupDecode(ctx, cs, cse-cs);

	ct=obuf;
	cte=obuf+osz1;
	i=0;

	while(ct<cte)
	{
		*ct++=BTLZA_BitArith_DecodeSymbol8(ctx,
			ctx->mdl_lit, ctx->ctxmask);
	}
	i=ct-obuf;
	return(i);
}

BGBBTJ_API int BTLZA_BitArith_EncodeData(
	byte *ibuf, int isz, byte *obuf, int osz)
{
	BGBBTJ_BTLZA_Context *ctx;
	int i, j;
	
//	ctx=btlza_malloc(sizeof(BGBBTJ_BTLZA_Context));
//	ctx->cs=NULL; ctx->ct=NULL;
	ctx=BTLZA_AllocContext();
	i=BTLZA_BitArith_EncodeDataCtx(ctx, ibuf, isz, obuf, osz, 13);
	BTLZA_FreeContext(ctx);
	return(i);
}

BGBBTJ_API int BTLZA_BitArith_DecodeData(
	byte *ibuf, int isz, byte *obuf, int osz)
{
	BGBBTJ_BTLZA_Context *ctx;
	int i, j;
	
	ctx=BTLZA_AllocContext();
	i=BTLZA_BitArith_DecodeDataCtx(ctx, ibuf, isz, obuf, osz);
	BTLZA_FreeContext(ctx);
	return(i);
}

BGBBTJ_API int BTLZA_BitArith_EncodeTestData(
	byte *ibuf, int isz, byte *obuf, int osz)
{
	BGBBTJ_BTLZA_Context *ctx;
	byte *tbuf;
	int i, j, k;

	if(isz<0)return(-1);
	if(isz<=0)return(0);
	
	ctx=BTLZA_AllocContext();
	i=BTLZA_BitArith_EncodeDataCtx(ctx, ibuf, isz, obuf, osz, 13);

	tbuf=malloc(2*isz);
	j=BTLZA_BitArith_DecodeDataCtx(ctx, obuf, i, tbuf, 2*isz);
	
	if(j!=isz)
	{
		printf("BTLZA_BitArith_EncodeTestData: Size Check %d->%d\n",
			isz, j);
	}
	
	for(k=0; k<j; k++)
	{
		if(ibuf[k]!=tbuf[k])
		{
			printf("BTLZA_BitArith_EncodeTestData: "
				"Decode Fail At %d\n", k);
			break;
		}
	}

	if(k>=j)
	{
//		printf("BTLZA_BitArith_EncodeTestData: OK\n");
	}else
	{
		printf("BTLZA_BitArith_EncodeTestData: Fail\n");
		i=-1;
	}

	free(tbuf);

	BTLZA_FreeContext(ctx);
	
	return(i);
}
#endif
