#include <btlzazip.h>

u32 btlza_dbase[64]=
{
	    0,     1,     2,     3,
	    4,     6,     8,    12,
	   16,    24,    32,    48,
	   64,    96,   128,   192,
	  256,   384,   512,   768,
	 1024,  1536,  2048,  3072,
	 4096,  6144,  8192, 12288,
	16384, 24576, 32768, 49152,
	0x00010000, 0x00018000, 0x00020000, 0x00030000, 
	0x00040000, 0x00060000, 0x00080000, 0x000C0000, 
	0x00100000, 0x00180000, 0x00200000, 0x00300000, 
	0x00400000, 0x00600000, 0x00800000, 0x00C00000, 
	0x01000000, 0x01800000, 0x02000000, 0x03000000, 
	0x04000000, 0x06000000, 0x08000000, 0x0C000000, 
	0x10000000, 0x18000000, 0x20000000, 0x30000000, 
	0x40000000, 0x60000000, 0x80000000, 0xC0000000
};

int btlza_dextra[64]=
{
	 0,  0,  0,  0,	 1,  1,  2,  2,
	 3,  3,  4,  4,	 5,  5,  6,  6,
	 7,  7,  8,  8,	 9,  9, 10, 10,
	11, 11, 12, 12,	13, 13, 14, 14,
	15, 15, 16, 16, 17, 17, 18, 18,
	19, 19, 20, 20, 21, 21, 22, 22,
	23, 23, 24, 24, 25, 25, 26, 26,
	27, 27, 28, 28, 29, 29, 30, 30
};

int btlza_lbase[64]=
{
	  3,   4,   5,   6,   7,   8,   9,  10,
	 11,  13,  15,  17,  19,  23,  27,  31,
	 35,  43,  51,  59,  67,  83,  99, 115,
	131, 163, 195, 227,
	0x0103, 0x0143, 0x0183, 0x01C3,
	0x0203, 0x0283, 0x0303, 0x0383,
	0x0403, 0x0503, 0x0603, 0x0703,
	0x0803, 0x0A03, 0x0C03, 0x0E03,
	0x1003, 0x1403, 0x1803, 0x1C03,
	0x2003, 0x2803, 0x3003, 0x3803,
	0x4003, 0x5003, 0x6003, 0x7003,
	0x8003, 0xA003, 0xC003, 0xE003,
	0x0000, 0x0000, 0x0000, 0x0000
};

int btlza_lextra[64]=
{
	 0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  2,  2,  2,  2,
	 3,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,  5,
	 6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  8,  9,  9,  9,  9,
	10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13,
	 0,  0,  0,  0
};

#if 0
int BTLZA_Decode_DecodeDistance(BGBBTJ_BTLZA_Context *ctx)
{

	int i, j, k;

	j=BTLZA_BitArith_DecodeSymbol6(ctx, ctx->mdl_dist, (1<<12)-1);
//	k=btlza_dbase[j]+BTLZA_BitArith_InputExtraBits(ctx,
//		btlza_dextra[j], ctx->mdl_dist, (1<<12)-1);
	k=btlza_dbase[j]+BTLZA_BitArith_InputExtraBits(ctx,
		btlza_dextra[j], ctx->mdl_xbits, 4095);

	return(k);
}

int BTLZA_Decode_DecodeRun(BGBBTJ_BTLZA_Context *ctx, int sym)
{
	char *s;
	int i, j, k;

	i=sym-257;
//	j=btlza_lbase[i]+BTLZA_BitArith_InputExtraBits(ctx,
//		btlza_lextra[i], ctx->mdl_lit, ctx->ctxmask);
	ctx->wctx&=4095;
	j=btlza_lbase[i]+BTLZA_BitArith_InputExtraBits(ctx,
		btlza_lextra[i], ctx->mdl_xbits, 4095);
	k=BTLZA_Decode_DecodeDistance(ctx);
	s=ctx->ct-k;
	i=j;
	while(i--)
		{ *ctx->ct++=*s++; }
//	ctx->wctx=
//		((*(ctx->ct-3))<<18)|
//		((*(ctx->ct-2))<< 9)|
//		 (*(ctx->ct-1));
//	ctx->wctx&=ctx->ctxmask;
	ctx->wctx=0;
	return(0);
}

int BTLZA_Decode_DecodeSymbol(BGBBTJ_BTLZA_Context *ctx)
{
	return(BTLZA_BitArith_DecodeSymbol9(
		ctx, ctx->mdl_lit, ctx->ctxmask));
}

int BTLZA_Decode_DecodeBlockData(BGBBTJ_BTLZA_Context *ctx)
{
	int i;

	while(1)
	{
		i=BTLZA_Decode_DecodeSymbol(ctx);
		if(i<0)return(i);

		if(i<256)
		{
			*ctx->ct++=i;
			continue;
		}
		if(i==256)break;
		i=BTLZA_Decode_DecodeRun(ctx, i);
		if(i<0)return(i);
	}

	return(0);
}

int BTLZA_Decode_DecodeBlock(BGBBTJ_BTLZA_Context *ctx)
{
	int fi, ty, l, nl, fl;
	int i;

//	fi=PDUNZ_ReadBit();
//	ty=PDUNZ_Read2Bits();

//	i=*ctx->cs++;
//	if(i!=0xB1)return(-1);

	ty=*ctx->cs++;
	if(!ty)return(1);

//	printf("blk %d %d\n", fi, ty);

	switch(ty)
	{
	case 0xA0:
		ctx->cs=BGBBTJ_DecodeVLI(ctx->cs, &l);
		while(l--)
		{
			i=*ctx->cs++;
			*ctx->ct++=i;
		}
		break;
	case 0xA6:
		ctx->cs=BGBBTJ_DecodeVLI(ctx->cs, &fl);
		BTLZA_BitArith_SetupContextBits(ctx, 12+(fl&15));
		BTLZA_BitArith_SetupDecode(ctx, ctx->cs, ctx->cse-ctx->cs);
		i=BTLZA_Decode_DecodeBlockData(ctx);
		if(i<0)return(i);
		break;
//	case 2:
//		i=PDUNZ_DecodeHeader();
//		if(i<0)return(i);
//		i=PDUNZ_DecodeBlockData();
//		if(i<0)return(i);
//		break;

//	case 3:
	default:
		return(-2);
		break;
	}

	return(0);
}

BGBBTJ_API int BTLZA_Decode_DecodeStreamSz(byte *ibuf, byte *obuf, int isz,
	int osz, int *rosz, int flags)
{
	BGBBTJ_BTLZA_Context *ctx;
	int i, ws;

	ctx=BTLZA_AllocContext();

	ctx->ibuf=ibuf;
	ctx->obuf=obuf;
	ctx->cs=ibuf;
	ctx->cse=ibuf+isz;
	ctx->ct=obuf;
	ctx->cte=obuf+osz;

	while(1)
	{
		i=BTLZA_Decode_DecodeBlock(ctx);
		if(i<0)return(i);
		if(i)break;
	}

	if(rosz)
		*rosz=ctx->ct-ctx->obuf;

	i=ctx->cs-ibuf;
	return(i);
}
#endif

BGBBTJ_API int BTLZA_DecodeStreamSzZl(byte *ibuf, byte *obuf, int isz,
	int osz, int *rosz, int flags)
{
	BGBBTJ_BTLZA_Context *ctx;
	byte *cs, *cse;
	int i, j, cm;
	
	if(!ibuf || !obuf)
		return(-1);
	
//	i=(ibuf[0]<<8)+ibuf[1];
//	cm=(i>>8)&15;
	
	cs=ibuf; cse=ibuf+isz;
	i=(*cs)>>4;
//	if((i>=0) && (i<=7))
//	if((i>=0) && (i<=14))
//	if((i!=8) || ((*cs)==0x89))
	if((i==8) || (i==11))
	{
		j=(cs[0]<<8)+cs[1];
		if(j%31)
		{
			if(i==11)
			{
				j=j&0x8FFF;
				if(j%31)
					return(-5);
			}
			
			i=*cs++;
			cm=i&15;
		}else
		{
			i=(cs[0]<<8)+cs[1]; cs+=2;
			cm=(i>>8)&15;
		}
	}else
	{
		i=(cs[0]<<8)+cs[1]; cs+=2;
		cm=(i>>8)&15;
		if(i%31)
			return(-4);
	}

	if(cm==9)
	{
//		i=PDUNZ_DecodeStreamSz(ibuf+2, obuf, isz-2,
//			osz, rosz, flags|BGBBTJ_ZFL_DEFLATE64);

		ctx=BTLZA_AllocContext();
		if(!ctx)return(-2);
//		i=BTLZA_BitDec_DecodeStreamSz(ctx, ibuf+2, obuf, isz-2,
//			osz, rosz, flags|BGBBTJ_ZFL_DEFLATE64);
		i=BTLZA_BitDec_DecodeStreamSz(ctx, cs, obuf, cse-cs,
			osz, rosz, flags|BGBBTJ_ZFL_DEFLATE64);
		BTLZA_FreeContext(ctx);
		return(i);
	}
	if(cm==8)
	{
//		i=PDUNZ_DecodeStreamSz(ibuf+2, obuf, isz-2, osz, rosz, flags);
		ctx=BTLZA_AllocContext();
		if(!ctx)return(-2);
//		i=BTLZA_BitDec_DecodeStreamSz(ctx,
//			ibuf+2, obuf, isz-2, osz, rosz, flags);
		i=BTLZA_BitDec_DecodeStreamSz(ctx,
			cs, obuf, cse-cs, osz, rosz, flags);
		BTLZA_FreeContext(ctx);
		return(i);
	}
	if(cm==10)
	{
//		i=PDUNZ_DecodeStreamSz(ibuf+2, obuf, isz-2, osz, rosz, flags);
		ctx=BTLZA_AllocContext();
		if(!ctx)return(-2);
//		i=BTLZA_BitDec_DecodeStreamSz(ctx,
//			ibuf+2, obuf, isz-2, osz, rosz, flags);
		i=BTLZA_BitDec_DecodeStreamSz(ctx,
			cs, obuf, cse-cs, osz, rosz, flags);
		BTLZA_FreeContext(ctx);
		return(i);
	}

	return(-3);
}

BGBBTJ_API int BTLZA_DecodeStreamZl(byte *ibuf, byte *obuf, int isz, int osz)
{
	return(BTLZA_DecodeStreamSzZl(ibuf, obuf, isz, osz, NULL, 0));
}
