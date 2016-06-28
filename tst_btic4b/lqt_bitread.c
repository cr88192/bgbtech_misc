void LQTVQ_SetupReadBits(BT4A_Context *ctx, byte *buf, int sz)
{
	ctx->cs=buf;
//	ctx->bit_win=0;
	ctx->bit_pos=0;
	ctx->bit_win=*(u32 *)(ctx->cs);
}

byte *LQTVQ_EndReadBits(BT4A_Context *ctx)
{
	u32 bw;
	int bp;
	int i;
	
	bp=ctx->bit_pos;
	bw=ctx->bit_win;
	i=(bp+7)>>3;
	ctx->cs+=i;
	ctx->bit_win=(bw>>(i<<3));
	ctx->bit_pos=0;
	return(ctx->cs);
}

int LQTVQ_ReadNBitsNM(BT4A_Context *ctx, int len)
{
	u32 bw;
	int bp;
	int bits;
	
	bp=ctx->bit_pos;
	bw=ctx->bit_win;

	bits=bw>>bp;
	bp=bp+len;

	ctx->cs+=bp>>3;
	ctx->bit_win=*(u32 *)ctx->cs;
	ctx->bit_pos=bp&7;
	return(bits);
}

int LQTVQ_ReadNBits(BT4A_Context *ctx, int len)
{
	u32 bw;
	int bp;
	int bits;
	
	bp=ctx->bit_pos;
	bw=ctx->bit_win;

	bits=(bw>>bp)&((1<<len)-1);
	bp=bp+len;

	ctx->cs+=bp>>3;
	ctx->bit_win=*(u32 *)ctx->cs;
	ctx->bit_pos=bp&7;
	return(bits);
}

#if 0
int LQTVQ_Read8BitsNM(BT4A_Context *ctx)
{
	return(LQTVQ_ReadNBits(ctx, 8));
}

int LQTVQ_Read16BitsNM(BT4A_Context *ctx)
{
	return(LQTVQ_ReadNBits(ctx, 16));
}

int LQTVQ_Read24BitsNM(BT4A_Context *ctx)
{
	return(LQTVQ_ReadNBits(ctx, 24));
}

int LQTVQ_Read16Bits(BT4A_Context *ctx)
{
	return(LQTVQ_ReadNBits(ctx, 16));
}

int LQTVQ_Read24Bits(BT4A_Context *ctx)
{
	return(LQTVQ_ReadNBits(ctx, 24));
}

u32 LQTVQ_Read32Bits(BT4A_Context *ctx)
{
	int l, h;
	
	l=LQTVQ_Read16Bits(ctx);
	h=LQTVQ_Read16BitsNM(ctx);
	return((h<<16)|l);
}
#endif

#if 1
int LQTVQ_Read8BitsNM(BT4A_Context *ctx)
{
	int bits;
	bits=ctx->bit_win>>ctx->bit_pos;
	ctx->cs++;
	ctx->bit_win=*(u32 *)ctx->cs;
	return(bits);
}

int LQTVQ_Read16BitsNM(BT4A_Context *ctx)
{
	int bits;
	bits=ctx->bit_win>>ctx->bit_pos;
	ctx->cs+=2;
	ctx->bit_win=*(u32 *)ctx->cs;
	return(bits);
}

int LQTVQ_Read24BitsNM(BT4A_Context *ctx)
{
	int bits;
	bits=ctx->bit_win>>ctx->bit_pos;
	ctx->cs+=3;
	ctx->bit_win=*(u32 *)ctx->cs;
	return(bits);
}

int LQTVQ_Read8Bits(BT4A_Context *ctx)
{
	return((byte)LQTVQ_Read8BitsNM(ctx));
}

int LQTVQ_Read16Bits(BT4A_Context *ctx)
{
	return((u16)LQTVQ_Read16BitsNM(ctx));
}

int LQTVQ_Read24Bits(BT4A_Context *ctx)
{
	return(LQTVQ_Read24BitsNM(ctx)&16777215);
}

u32 LQTVQ_Read32Bits(BT4A_Context *ctx)
{
	u32 bits, bw1, m;

	bits=ctx->bit_win>>ctx->bit_pos;
	ctx->cs+=4;
	bw1=*(u32 *)ctx->cs;
	ctx->bit_win=bw1;
	m=~((ctx->bit_pos-1)>>31);
	bits|=(bw1<<(32-ctx->bit_pos))&m;
	return(bits);
}

#if 0
u32 LQTVQ_Read32Bits(BT4A_Context *ctx)
{
	int l, h;
	
	l=LQTVQ_Read16Bits(ctx);
	h=LQTVQ_Read16BitsNM(ctx);
	return((h<<16)|l);
}
#endif

#endif

u64 LQTVQ_Read64Bits(BT4A_Context *ctx)
{
	u32 l, m, h;

//	l=LQTVQ_Read24Bits(ctx);
//	m=LQTVQ_Read24Bits(ctx);
//	h=LQTVQ_Read16BitsNM(ctx);
//	return((((u64)h)<<48)|(((u64)m)<<24)|l);
	
	l=LQTVQ_Read32Bits(ctx);
	h=LQTVQ_Read32Bits(ctx);
	return((((u64)h)<<32)|l);
}

void LQTVQ_SkipNBits(BT4A_Context *ctx, int len)
{
	int bp;
	bp=ctx->bit_pos+len;
	ctx->cs+=bp>>3;
	ctx->bit_win=*(u32 *)ctx->cs;
	ctx->bit_pos=bp&7;
}

void LQTVQ_Skip8Bits(BT4A_Context *ctx)
{
	ctx->cs++;
	ctx->bit_win=*(u32 *)ctx->cs;
}

int LQTVQ_PeekNBitsNM(BT4A_Context *ctx, int len)
{
	u32 bw;
	int bp, bits;
	
	bp=ctx->bit_pos;
	bw=ctx->bit_win;
	bits=bw>>bp;
	return(bits);
}

//force_inline
int LQTVQ_Peek8Bits(BT4A_Context *ctx)
{
	u32 bw;
	int bp, bits;
	
	bp=ctx->bit_pos;
	bw=ctx->bit_win;
	bits=(byte)(bw>>bp);
	return(bits);
}

int LQTVQ_ReadRiceQ(BT4A_Context *ctx)
{
	int v, b;
	int i, j, k, l;

	b=LQTVQ_Peek8Bits(ctx);
	if(b!=255)
	{
		v=lqtvq_decriceq8[b];
		LQTVQ_SkipNBits(ctx, v+1);
		return(v);
	}

	LQTVQ_Skip8Bits(ctx);
	v=8;
	b=LQTVQ_Peek8Bits(ctx);
	while(b==255)
		{ LQTVQ_Skip8Bits(ctx); v+=8; b=LQTVQ_Peek8Bits(ctx); }
	i=lqtvq_decriceq8[b];
	LQTVQ_SkipNBits(ctx, i+1);
	return(v+i);
}

int LQTVQ_ReadAdRiceILL(BT4A_Context *ctx, byte *rk)
{
	int q, k, b, v;
	int j;
	
	k=*rk;
	q=LQTVQ_ReadRiceQ(ctx);

#if 1
	if(q>=8)
	{
		v=LQTVQ_ReadNBits(ctx, 5+(q-8)*3);
		k=k+3+(q-8);
		if(k>15)k=15;
		*rk=k;
		return(v);
	}
#endif
	
	b=LQTVQ_ReadNBits(ctx, k);
	v=(q<<k)|b;

#if 0
	if(!q)
	{
		if(k>0)k--;
		*rk=k;
	}else if(q>1)
	{
		j=q;
		while(j>1)
			{ k++; j=j>>1; }
		if(k>15)k=15;
		*rk=k;
	}
#endif

	*rk=lqtvq_decricenk8[(k<<4)|q];
	return(v);
}

int LQTVQ_ReadAdRiceLL(BT4A_Context *ctx, byte *rk)
{
	int v, b;
	int i, j, k, l;

#if 1
	b=LQTVQ_Peek8Bits(ctx);
	j=lqtvq_decrice8[(*rk<<8)|b];
	
	if(j)
	{
//		i=(u16)j;
		LQTVQ_SkipNBits(ctx, (j>>16)&15);
		*rk=(j>>20)&15;
//		return(i);
		return((u16)j);
	}
#endif

	i=LQTVQ_ReadAdRiceILL(ctx, rk);
	return(i);
}

int LQTVQ_ReadAdSRiceLL(BT4A_Context *ctx, byte *rk)
{
	int v;
	
	v=LQTVQ_ReadAdRiceLL(ctx, rk);
	v=(v>>1)^((v<<31)>>31);
	return(v);
}

#if 1
int LQTVQ_DecodeSymbolIndexSmtf(BT4A_Context *ctx,
	BT4A_SmtfState *st, int i)
{
	int i0, i1, i2, i3;

	if(!i)
	{
		i0=(byte)(st->rov+i);
		i2=st->tab[i0];
		return(i2);
	}

	if(i<32)
	{
		i0=(byte)(st->rov+i);		i1=(byte)(st->rov+i-1);
		i2=st->tab[i0];		i3=st->tab[i1];
		st->tab[i0]=i3;		st->tab[i1]=i2;
//		st->idx[i2]=i1;		st->idx[i3]=i0;
		return(i2);
	}

	i0=(byte)(st->rov+i);	i1=(byte)(st->rov-1);
	i2=st->tab[i0];		i3=st->tab[i1];
	st->tab[i0]=i3;		st->tab[i1]=i2;
//	st->idx[i2]=i1;		st->idx[i3]=i0;
	st->rov--;
	return(i2);
}
#endif

int LQTVQ_ReadSymbolSmtf(BT4A_Context *ctx,
	BT4A_SmtfState *st)
{
	int i0, i1, i2, i3;
	int i;

#ifdef LQTVQ_DBG_NOSMTF
	i=LQTVQ_ReadAdRiceLL(ctx, &(st->rk));
	return(i);
#endif

#if 1
	i=LQTVQ_ReadAdRiceLL(ctx, &(st->rk));
	if(!i)
	{
		i0=(byte)(st->rov+i);
		i2=st->tab[i0];
		return(i2);
	}

	if(i<32)
	{
		i0=(byte)(st->rov+i);		i1=(byte)(st->rov+i-1);
		i2=st->tab[i0];		i3=st->tab[i1];
		st->tab[i0]=i3;		st->tab[i1]=i2;
//		st->idx[i2]=i1;		st->idx[i3]=i0;
		return(i2);
	}

	i0=(byte)(st->rov+i);	i1=(byte)(st->rov-1);
	i2=st->tab[i0];		i3=st->tab[i1];
	st->tab[i0]=i3;		st->tab[i1]=i2;
//	st->idx[i2]=i1;		st->idx[i3]=i0;
	st->rov--;
	return(i2);
#endif
}
