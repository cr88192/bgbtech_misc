#include <btlzazip.h>

void BTLZA_BitEnc_WriteByteBasic(BGBBTJ_BTLZA_Context *ctx, int v)
{
	*ctx->ct++=v;
}

void BTLZA_BitEnc_WriteByteEscape(BGBBTJ_BTLZA_Context *ctx, int v)
{
	if(v==255)
	{
		*ctx->ct++=0xFF;
		*ctx->ct++=0x00;
		return;
	}

	*ctx->ct++=v;
}

void BTLZA_BitDec_WriteByteArithLE(BGBBTJ_BTLZA_Context *ctx, int v)
{
	BTLZA_BitArith_EncodeSymbol8LE(ctx,
		v, ctx->mdl_rbits, ctx->ctxmask_raw);
//	return(*ctx->cs++);
}

void BTLZA_BitDec_WriteByteArithBE(BGBBTJ_BTLZA_Context *ctx, int v)
{
	BTLZA_BitArith_EncodeSymbol8(ctx,
		v, ctx->mdl_rbits, ctx->ctxmask_raw);
//	return(*ctx->cs++);
}


void BTLZA_BitEnc_WriteByte(BGBBTJ_BTLZA_Context *ctx, int v)
	{ ctx->BS_WriteByte(ctx, v); }

void BTLZA_BitEnc_WriteBit(BGBBTJ_BTLZA_Context *ctx, int i)
{
	i&=1;
	ctx->bs_win|=i<<ctx->bs_pos;
	ctx->bs_pos++;
	if(ctx->bs_pos>=8)
	{
		BTLZA_BitEnc_WriteByte(ctx, ctx->bs_win&0xFF);
		ctx->bs_win>>=8;
		ctx->bs_pos-=8;
	}
}

#if 0
void BTLZA_BitEnc_Write2Bits(BGBBTJ_BTLZA_Context *ctx, int i)
{
	i&=3;
	ctx->bs_win|=i<<ctx->bs_pos;
	ctx->bs_pos+=2;
	if(ctx->bs_pos>=8)
	{
		BTLZA_BitEnc_WriteByte(ctx, ctx->bs_win&0xFF);
		ctx->bs_win>>=8;
		ctx->bs_pos-=8;
	}
}

void BTLZA_BitEnc_Write3Bits(BGBBTJ_BTLZA_Context *ctx, int i)
{
	i&=7;
	ctx->bs_win|=i<<ctx->bs_pos;
	ctx->bs_pos+=3;
	if(ctx->bs_pos>=8)
	{
		BTLZA_BitEnc_WriteByte(ctx, ctx->bs_win&0xFF);
		ctx->bs_win>>=8;
		ctx->bs_pos-=8;
	}
}

void BTLZA_BitEnc_Write4Bits(BGBBTJ_BTLZA_Context *ctx, int i)
{
	i&=15;
	ctx->bs_win|=i<<ctx->bs_pos;
	ctx->bs_pos+=4;
	if(ctx->bs_pos>=8)
	{
		BTLZA_BitEnc_WriteByte(ctx, ctx->bs_win&0xFF);
		ctx->bs_win>>=8;
		ctx->bs_pos-=8;
	}
}

void BTLZA_BitEnc_Write5Bits(BGBBTJ_BTLZA_Context *ctx, int i)
{
	i&=31;
	ctx->bs_win|=i<<ctx->bs_pos;
	ctx->bs_pos+=5;
	if(ctx->bs_pos>=8)
	{
		BTLZA_BitEnc_WriteByte(ctx, ctx->bs_win&0xFF);
		ctx->bs_win>>=8;
		ctx->bs_pos-=8;
	}
}

void BTLZA_BitEnc_Write6Bits(BGBBTJ_BTLZA_Context *ctx, int i)
	{ BTLZA_BitEnc_WriteNBits(ctx, i, 6); }
void BTLZA_BitEnc_Write7Bits(BGBBTJ_BTLZA_Context *ctx, int i)
	{ BTLZA_BitEnc_WriteNBits(ctx, i, 7); }
#endif

void BTLZA_BitEnc_Write2Bits(BGBBTJ_BTLZA_Context *ctx, int i)
	{ BTLZA_BitEnc_WriteNBits(ctx, i, 2); }
void BTLZA_BitEnc_Write3Bits(BGBBTJ_BTLZA_Context *ctx, int i)
	{ BTLZA_BitEnc_WriteNBits(ctx, i, 3); }
void BTLZA_BitEnc_Write4Bits(BGBBTJ_BTLZA_Context *ctx, int i)
	{ BTLZA_BitEnc_WriteNBits(ctx, i, 4); }
void BTLZA_BitEnc_Write5Bits(BGBBTJ_BTLZA_Context *ctx, int i)
	{ BTLZA_BitEnc_WriteNBits(ctx, i, 5); }
void BTLZA_BitEnc_Write6Bits(BGBBTJ_BTLZA_Context *ctx, int i)
	{ BTLZA_BitEnc_WriteNBits(ctx, i, 6); }
void BTLZA_BitEnc_Write7Bits(BGBBTJ_BTLZA_Context *ctx, int i)
	{ BTLZA_BitEnc_WriteNBits(ctx, i, 7); }

void BTLZA_BitEnc_WriteNBits(BGBBTJ_BTLZA_Context *ctx, int i, int n)
{
	i&=((1<<n)-1);
	ctx->bs_win|=i<<ctx->bs_pos;
	ctx->bs_pos+=n;
#if 0
	while(ctx->bs_pos>=8)
	{
		BTLZA_BitEnc_WriteByte(ctx, ctx->bs_win&0xFF);
		ctx->bs_win>>=8;
		ctx->bs_pos-=8;
	}
#endif
#if 1
	if(ctx->bs_pos>=8)
	{
		BTLZA_BitEnc_WriteByte(ctx, ctx->bs_win&0xFF);
		ctx->bs_win>>=8;
		ctx->bs_pos-=8;

		if(ctx->bs_pos>=8)
		{
			BTLZA_BitEnc_WriteByte(ctx, ctx->bs_win&0xFF);
			ctx->bs_win>>=8;
			ctx->bs_pos-=8;

			while(ctx->bs_pos>=8)
			{
				BTLZA_BitEnc_WriteByte(ctx, ctx->bs_win&0xFF);
				ctx->bs_win>>=8;
				ctx->bs_pos-=8;
			}
		}
	}
#endif
}

void BTLZA_BitEnc_FlushBits(BGBBTJ_BTLZA_Context *ctx)
{
	while(ctx->bs_pos>0)
	{
		BTLZA_BitEnc_WriteByte(ctx, ctx->bs_win&0xFF);
		ctx->bs_win>>=8;
		ctx->bs_pos-=8;
	}
	ctx->bs_pos=0;
	ctx->bs_win=0;
	
	if(ctx->rmax)
	{
		BTLZA_BitArith_FlushWBits(ctx);
	}
}

int BTLZA_BitDec_ReadByteBasic(BGBBTJ_BTLZA_Context *ctx)
{
	return(*ctx->cs++);
}

int BTLZA_BitDec_ReadByteEscape(BGBBTJ_BTLZA_Context *ctx)
{
	int i, j;

	i=*ctx->cs++;
	if(i==0xFF)
	{
		j=*ctx->cs++;
		if(j==0x00)
			{ /* No Op */ }
		else
			{ *(int *)-1=-1; }
	}
	return(i);
}

int BTLZA_BitDec_ReadByteArithLE(BGBBTJ_BTLZA_Context *ctx)
{
	return(BTLZA_BitArith_DecodeSymbol8LE(ctx,
		ctx->mdl_rbits, ctx->ctxmask_raw));
//	return(*ctx->cs++);
}

int BTLZA_BitDec_ReadByteArithBE(BGBBTJ_BTLZA_Context *ctx)
{
	return(BTLZA_BitArith_DecodeSymbol8(ctx,
		ctx->mdl_rbits, ctx->ctxmask_raw));
//	return(*ctx->cs++);
}

int BTLZA_BitDec_ReadBitBasic(BGBBTJ_BTLZA_Context *ctx)
{
	int i;

	i=(ctx->bs_win>>ctx->bs_pos)&1;
	ctx->bs_pos++;
	if(ctx->bs_pos>=8)
	{
		ctx->bs_win=(ctx->bs_win>>8)|(BTLZA_BitDec_ReadByte(ctx)<<24);
		ctx->bs_pos-=8;
	}
	return(i);
}

#if 0
int BTLZA_BitDec_Read2Bits(BGBBTJ_BTLZA_Context *ctx)
{
	int i;
	i=(ctx->bs_win>>ctx->bs_pos)&3;
	ctx->bs_pos+=2;
	if(ctx->bs_pos>=8)
	{
		ctx->bs_win=(ctx->bs_win>>8)|(BTLZA_BitDec_ReadByte(ctx)<<24);
		ctx->bs_pos-=8;
	}
	return(i);
}

int BTLZA_BitDec_Read3Bits(BGBBTJ_BTLZA_Context *ctx)
{
	int i;
	i=(ctx->bs_win>>ctx->bs_pos)&7;
	ctx->bs_pos+=3;
	if(ctx->bs_pos>=8)
	{
		ctx->bs_win=(ctx->bs_win>>8)|(BTLZA_BitDec_ReadByte(ctx)<<24);
		ctx->bs_pos-=8;
	}
	return(i);
}

int BTLZA_BitDec_Read4Bits(BGBBTJ_BTLZA_Context *ctx)
{
	int i;
	i=(ctx->bs_win>>ctx->bs_pos)&15;
	ctx->bs_pos+=4;
	if(ctx->bs_pos>=8)
	{
		ctx->bs_win=(ctx->bs_win>>8)|(BTLZA_BitDec_ReadByte(ctx)<<24);
		ctx->bs_pos-=8;
	}
	return(i);
}

int BTLZA_BitDec_Read5Bits(BGBBTJ_BTLZA_Context *ctx)
{
	int i;
	i=(ctx->bs_win>>ctx->bs_pos)&31;
	ctx->bs_pos+=5;
	if(ctx->bs_pos>=8)
	{
		ctx->bs_win=(ctx->bs_win>>8)|(BTLZA_BitDec_ReadByte(ctx)<<24);
		ctx->bs_pos-=8;
	}
	return(i);
}
#endif

void BTLZA_BitDec_ReadAdjust(BGBBTJ_BTLZA_Context *ctx)
{
	while(ctx->bs_pos>=8)
	{
		ctx->bs_win=(ctx->bs_win>>8)|(BTLZA_BitDec_ReadByte(ctx)<<24);
		ctx->bs_pos-=8;
	}
}

int BTLZA_BitDec_ReadNBitsBasic(BGBBTJ_BTLZA_Context *ctx, int n)
{
	int i;
	i=(ctx->bs_win>>ctx->bs_pos)&((1<<n)-1);
	ctx->bs_pos+=n;
	if(ctx->bs_pos>=8)
	{
		ctx->bs_win=(ctx->bs_win>>8)|(BTLZA_BitDec_ReadByte(ctx)<<24);
		ctx->bs_pos-=8;
		if(ctx->bs_pos>=8)
		{
			ctx->bs_win=(ctx->bs_win>>8)|(BTLZA_BitDec_ReadByte(ctx)<<24);
			ctx->bs_pos-=8;
			while(ctx->bs_pos>=8)
			{
				ctx->bs_win=(ctx->bs_win>>8)|(BTLZA_BitDec_ReadByte(ctx)<<24);
				ctx->bs_pos-=8;
			}
		}
	}
	return(i);
}

void BTLZA_BitDec_SkipNBitsBasic(BGBBTJ_BTLZA_Context *ctx, int n)
{
	ctx->bs_pos+=n;
	if(ctx->bs_pos>=8)
	{
		ctx->bs_win=(ctx->bs_win>>8)|(BTLZA_BitDec_ReadByte(ctx)<<24);
		ctx->bs_pos-=8;
		if(ctx->bs_pos>=8)
		{
			ctx->bs_win=(ctx->bs_win>>8)|(BTLZA_BitDec_ReadByte(ctx)<<24);
			ctx->bs_pos-=8;
			while(ctx->bs_pos>=8)
			{
				ctx->bs_win=(ctx->bs_win>>8)|(BTLZA_BitDec_ReadByte(ctx)<<24);
				ctx->bs_pos-=8;
			}
		}
	}
}

int BTLZA_BitDec_ReadByte(BGBBTJ_BTLZA_Context *ctx)
	{ return(ctx->BS_ReadByte(ctx)); }

int BTLZA_BitDec_ReadBit(BGBBTJ_BTLZA_Context *ctx)
	{ return(ctx->BS_ReadBit(ctx)); }

int BTLZA_BitDec_ReadNBits(BGBBTJ_BTLZA_Context *ctx, int n)
	{ return(ctx->BS_ReadNBits(ctx, n)); }

void BTLZA_BitDec_SkipNBits(BGBBTJ_BTLZA_Context *ctx, int n)
	{ ctx->BS_SkipNBits(ctx, n); }

int BTLZA_BitDec_Read2Bits(BGBBTJ_BTLZA_Context *ctx)
	{ return(BTLZA_BitDec_ReadNBits(ctx, 2)); }

int BTLZA_BitDec_Read3Bits(BGBBTJ_BTLZA_Context *ctx)
	{ return(BTLZA_BitDec_ReadNBits(ctx, 3)); }

int BTLZA_BitDec_Read4Bits(BGBBTJ_BTLZA_Context *ctx)
	{ return(BTLZA_BitDec_ReadNBits(ctx, 4)); }

int BTLZA_BitDec_Read5Bits(BGBBTJ_BTLZA_Context *ctx)
	{ return(BTLZA_BitDec_ReadNBits(ctx, 5)); }

int BTLZA_BitDec_Read6Bits(BGBBTJ_BTLZA_Context *ctx)
	{ return(BTLZA_BitDec_ReadNBits(ctx, 6)); }

int BTLZA_BitDec_Read7Bits(BGBBTJ_BTLZA_Context *ctx)
	{ return(BTLZA_BitDec_ReadNBits(ctx, 7)); }

int BTLZA_BitDec_ReadExtraNBits(BGBBTJ_BTLZA_Context *ctx, int n)
	{ return(ctx->BS_ReadExtraNBits(ctx, n)); }


int BTLZA_BitDec_ReadRawNBits(BGBBTJ_BTLZA_Context *ctx, int n)
{
	int v;
	v=BTLZA_BitArith_InputContextModelBitsLE(ctx,
		n, ctx->mdl_rbits, ctx->ctxmask_raw);
	return(v);
}

int BTLZA_BitDec_ReadExtraNBitsBasic(BGBBTJ_BTLZA_Context *ctx, int n)
	{ return(BTLZA_BitDec_ReadNBitsBasic(ctx, n)); }

int BTLZA_BitArith_ReadExtraNBitsModel(BGBBTJ_BTLZA_Context *ctx, int n)
{
	int i, j;
	int v;

	if(n<=ctx->ctxcnt_xbits)
	{
		v=BTLZA_BitArith_InputContextModelBitsLE(ctx,
			n, ctx->mdl_xbits, ctx->ctxmask_xbits);
		return(v);
	}

	v=BTLZA_BitArith_InputContextFixedBitsLE(ctx, n-ctx->ctxcnt_xbits);
	j=BTLZA_BitArith_InputContextModelBitsLE(ctx,
			ctx->ctxcnt_xbits, ctx->mdl_xbits, ctx->ctxmask_xbits);
	v=v|(j<<(n-ctx->ctxcnt_xbits));
	return(v);
}


int BTLZA_BitDec_ReadLiteralBit(BGBBTJ_BTLZA_Context *ctx)
{
	int i, j, k;
	
	j=ctx->wctx;
	i=BTLZA_BitArith_InputModelBit(ctx,
		ctx->mdl_lit+(ctx->wctx&ctx->ctxmask_lit));
	ctx->wctx=(ctx->wctx<<1)|i;
	return(i);

//	return(BTLZA_BitDec_ReadBit(ctx));
}

int BTLZA_BitDec_ReadDistanceBit(BGBBTJ_BTLZA_Context *ctx)
{
	int i, j, k;
	
	j=ctx->wctx;
	i=BTLZA_BitArith_InputModelBit(ctx,
		ctx->mdl_dist+(ctx->wctx&ctx->ctxmask_dist));
	ctx->wctx=(ctx->wctx<<1)|i;
	return(i);

//	return(BTLZA_BitDec_ReadBit(ctx));
}

int BTLZA_BitDec_ReadRawBit(BGBBTJ_BTLZA_Context *ctx)
{
	int i, j, k;
	
	j=ctx->wctx;
	i=BTLZA_BitArith_InputModelBit(ctx,
		ctx->mdl_rbits+(ctx->wctx&ctx->ctxmask_raw));
	ctx->wctx=(ctx->wctx<<1)|i;
	return(i);

//	return(BTLZA_BitDec_ReadBit(ctx));
}

int BTLZA_BitDec_PeekWord(BGBBTJ_BTLZA_Context *ctx)
{
	int i;
	i=(ctx->bs_win>>ctx->bs_pos)&65535;
	return(i);
}

void BTLZA_BitDec_AlignByte(BGBBTJ_BTLZA_Context *ctx)
{
	while(ctx->bs_pos>0)
	{
		ctx->bs_win=(ctx->bs_win>>8)|(BTLZA_BitDec_ReadByte(ctx)<<24);
		ctx->bs_pos-=8;
	}
	ctx->bs_pos=0;
}

int BTLZA_BitDec_ReadAlignedByte(BGBBTJ_BTLZA_Context *ctx)
{
	int i;
	i=ctx->bs_win&0xFF;
	ctx->bs_win=(ctx->bs_win>>8)|(BTLZA_BitDec_ReadByte(ctx)<<24);
	return(i);
}

int BTLZA_BitDec_ReadAlignedWord(BGBBTJ_BTLZA_Context *ctx)
{
	int i;
	i=ctx->bs_win&0xFFFF;
	ctx->bs_win=(ctx->bs_win>>8)|(BTLZA_BitDec_ReadByte(ctx)<<24);
	ctx->bs_win=(ctx->bs_win>>8)|(BTLZA_BitDec_ReadByte(ctx)<<24);
	return(i);
}
