static int lqtvq_encrice8[16*512];		//0-15=bits, 16-19=len
static byte lqtvq_initrice=0;

void LQTVQ_InitRice(void)
{
	int q;
	int i, j, k, k1, l;

	if(lqtvq_initrice)
		return;
		
	for(k=0; k<16; k++)
	{
		for(i=0; i<512; i++)
		{
			q=i>>k;
			l=q+k+1;
			
			k1=k;
			if(!q)
			{
				if(k1>0)k1--;
			}else if(q>1)
			{
				j=q;
				while(j>1)
					{ k1++; j=j>>1; }
				if(k1>15)k1=15;
			}
			
			if((l>=16) || (q>=8))
			{
				lqtvq_encrice8[(k<<9)|i]=0;
				continue;
			}
			
			j=((1<<q)-1)|((i&((1<<k)-1))<<(q+1));
			lqtvq_encrice8[(k<<9)|i]=j|(l<<16)|(k1<<20);
		}
	}
}

void LQTVQ_SetupWriteBits(BT4A_Context *ctx, byte *buf, int sz)
{
	ctx->ct=buf;
	ctx->bit_win=0;
	ctx->bit_pos=0;
}

byte *LQTVQ_EndWriteBits(BT4A_Context *ctx)
{
	u32 bw;
	int bp;
	int i;
	
	bp=ctx->bit_pos;
	bw=ctx->bit_win;

	i=(bp+7)>>3;
	*(u32 *)ctx->ct=bw;
	ctx->ct+=i;
	ctx->bit_win=(bw>>(i<<3));
	ctx->bit_pos=0;
	return(ctx->ct);
}

void LQTVQ_WriteNBitsNM(BT4A_Context *ctx,
	int bits, int len)
{
	u32 bw;
	int bp;
//	int i;
	
	bp=ctx->bit_pos;
//	bw=ctx->bit_win;
//	bw=bw|(bits<<bp);
	bw=ctx->bit_win|(bits<<bp);
	bp=bp+len;

//	i=bp>>3;
	*(u32 *)ctx->ct=bw;
//	ctx->ct+=i;
//	ctx->bit_win=(bw>>(i<<3));
	ctx->ct+=bp>>3;
	ctx->bit_win=(bw>>(bp&(~7)));
	ctx->bit_pos=bp&7;
}

void LQTVQ_WriteNBits(BT4A_Context *ctx,
	int bits, int len)
{
	u32 bw;
	int bp;
	int i;
	
	bits&=1<<(len-1);
	bp=ctx->bit_pos;
	bw=ctx->bit_win;
	bw=bw|(bits<<bp);
	bp=bp+len;

	i=bp>>3;
	*(u32 *)ctx->ct=bw;
	ctx->ct+=i;
	ctx->bit_win=(bw>>(i<<3));
	ctx->bit_pos=bp&7;
}

void LQTVQ_Write8BitsNM(BT4A_Context *ctx, int bits)
{
	u32 bw;
	bw=ctx->bit_win|(bits<<ctx->bit_pos);
	*ctx->ct++=bw;
	ctx->bit_win=(bw>>8);
}

void LQTVQ_Write16BitsNM(BT4A_Context *ctx, int bits)
{
	u32 bw;
	bw=ctx->bit_win|(bits<<ctx->bit_pos);
	*(u32 *)ctx->ct=bw;
	ctx->ct+=2;
	ctx->bit_win=(bw>>16);
}

void LQTVQ_Write24BitsNM(BT4A_Context *ctx, int bits)
{
	u32 bw;
	bw=ctx->bit_win|(bits<<ctx->bit_pos);
	*(u32 *)ctx->ct=bw;
	ctx->ct+=3;
	ctx->bit_win=(bw>>24);
}

void LQTVQ_Write32Bits(BT4A_Context *ctx, u32 bits)
{
	LQTVQ_Write16BitsNM(ctx, (u16)bits);
	LQTVQ_Write16BitsNM(ctx, bits>>16);
}

void LQTVQ_WriteAdRiceILL(BT4A_Context *ctx, int val, byte *rk)
{
	int i, j, k, l;

	k=*rk;
	i=val>>k;

#if 1
	if(i>=8)
	{
		i=8; j=val>>5; k+=3;
		while(j>0)
			{ i++; j=j>>3; k++; }
		if(k>=15)k=15;
		LQTVQ_WriteNBits(ctx, (1<<i)-1, i+1);
		LQTVQ_WriteNBits(ctx, val, 5+(i-8)*3);
		*rk=k;
		return;
	}
#endif
	
	LQTVQ_WriteNBits(ctx, (1<<i)-1, i+1);
	LQTVQ_WriteNBits(ctx, val, k);
	
	if(!i)
	{
		if(k>0)k--;
		*rk=k;
	}else if(k>1)
	{
		j=i;
		while(j>1)
			{ k++; j=j>>1; }
		if(k>15)k=15;
		*rk=k;
	}
}

void LQTVQ_WriteAdRiceLL(BT4A_Context *ctx, int val, byte *rk)
{
	int i, j, k, l;

#if 1
	if(!(val>>9))
	{
		j=lqtvq_encrice8[(*rk<<9)|val];
		if(j)
		{
			LQTVQ_WriteNBitsNM(ctx, (u16)j, (j>>16)&15);
			*rk=(j>>20)&15;
			return;
		}
	}
#endif

	LQTVQ_WriteAdRiceILL(ctx, val, rk);	
}

void LQTVQ_WriteAdRiceSymLL(BT4A_Context *ctx, int val, byte *rk)
{
	int i, j, k, l;

	j=lqtvq_encrice8[(*rk<<9)|val];
	if(j)
	{
		LQTVQ_WriteNBitsNM(ctx, (u16)j, (j>>16)&15);
		*rk=(j>>20)&15;
		return;
	}

	LQTVQ_WriteAdRiceILL(ctx, val, rk);	
}

void LQTVQ_WriteAdSRiceLL(BT4A_Context *ctx, int val, byte *rk)
{
	LQTVQ_WriteAdRiceLL(ctx, (val<<1)^(val>>31), rk);
}

void LQTVQ_ResetSmtfDefault(BT4A_Context *ctx, BT4A_SmtfState *st)
{
	int i;
	
	for(i=0; i<256; i++)
		{ st->tab[i]=i; st->idx[i]=i; }
	st->rov=0;	st->rk=2;
}

void LQTVQ_ResetSmtfReverse(BT4A_Context *ctx, BT4A_SmtfState *st)
{
	int i, j;
	
	for(i=0; i<256; i++)
		{ j=255-i; st->tab[i]=j; st->idx[j]=i; }
	st->rov=0;	st->rk=2;
}

int LQTVQ_EmitIndexSymbolSmtf(BT4A_Context *ctx,
	BT4A_SmtfState *st, int val)
{
	int i0, i1, i2, i3;
	int i;
	
	i=(byte)((st->idx[val])-(st->rov));
	if(!i)
		{ return(i); }
	if(i<32)
	{
		i0=(byte)(st->rov+i);		i1=(byte)(st->rov+i-1);
		i2=st->tab[i0];		i3=st->tab[i1];
		st->tab[i0]=i3;		st->tab[i1]=i2;
		st->idx[i2]=i1;		st->idx[i3]=i0;
		return(i);
	}

	i0=(byte)(st->rov+i);	i1=(byte)(st->rov-1);
	i2=st->tab[i0];		i3=st->tab[i1];
	st->tab[i0]=i3;		st->tab[i1]=i2;
	st->idx[i2]=i1;		st->idx[i3]=i0;
	st->rov--;
	return(i);
}

void LQTVQ_WriteSymbolSmtf(BT4A_Context *ctx,
	BT4A_SmtfState *st, int val)
{
	int i0, i1, i2, i3;
	int i;
	
	i=(byte)((st->idx[val])-(st->rov));
	if(!i)
	{
		LQTVQ_WriteAdRiceSymLL(ctx, i, &(st->rk));
		return;
	}
	if(i<32)
	{
		i0=(byte)(st->rov+i);		i1=(byte)(st->rov+i-1);
		i2=st->tab[i0];		i3=st->tab[i1];
		st->tab[i0]=i3;		st->tab[i1]=i2;
		st->idx[i2]=i1;		st->idx[i3]=i0;
		LQTVQ_WriteAdRiceSymLL(ctx, i, &(st->rk));
		return;
	}

	i0=(byte)(st->rov+i);	i1=(byte)(st->rov-1);
	i2=st->tab[i0];		i3=st->tab[i1];
	st->tab[i0]=i3;		st->tab[i1]=i2;
	st->idx[i2]=i1;		st->idx[i3]=i0;
	st->rov--;

	LQTVQ_WriteAdRiceSymLL(ctx, i, &(st->rk));
}
