#include <btlzazip.h>

u32 btlzh2_vbase[256]=
{
	0x0000, 0x0001, 0x0002, 0x0003,	0x0004, 0x0005, 0x0006, 0x0007, //00
	0x0008, 0x0009, 0x000A, 0x000B,	0x000C, 0x000D, 0x000E, 0x000F, //08
	0x0010, 0x0012, 0x0014, 0x0016,	0x0018, 0x001A, 0x001C, 0x001E, //10
	0x0020, 0x0024, 0x0028, 0x002C,	0x0030, 0x0034, 0x0038, 0x003C, //18
	0x0040, 0x0048, 0x0050, 0x0058,	0x0060, 0x0068, 0x0070, 0x0078, //20
	0x0080, 0x0090, 0x00A0, 0x00B0,	0x00C0, 0x00D0, 0x00E0, 0x00F0, //28
	0x0100, 0x0120, 0x0140, 0x0160,	0x0180, 0x01A0, 0x01C0, 0x01E0, //30
	0x0200, 0x0240, 0x0280, 0x02C0,	0x0300, 0x0340, 0x0380, 0x03C0, //38
	0x0400, 0x0480, 0x0500, 0x0580,	0x0600, 0x0680, 0x0700, 0x0780, //40
	0x0800, 0x0900, 0x0A00, 0x0B00,	0x0C00, 0x0D00, 0x0E00, 0x0F00, //48
	0x1000, 0x1200, 0x1400, 0x1600,	0x1800, 0x1A00, 0x1C00, 0x1E00, //50
	0x2000, 0x2400, 0x2800, 0x2C00,	0x3000, 0x3400, 0x3800, 0x3C00, //58
	0x4000, 0x4800, 0x5000, 0x5800,	0x6000, 0x6800, 0x7000, 0x7800, //60
	0x8000, 0x9000, 0xA000, 0xB000,	0xC000, 0xD000, 0xE000, 0xF000, //68
	0x00010000, 0x00012000, 0x00014000, 0x00016000, //70
	0x00018000, 0x0001A000, 0x0001C000, 0x0001E000, //74
	0x00020000, 0x00024000, 0x00028000, 0x0002C000, //78
	0x00030000, 0x00034000, 0x00038000, 0x0003C000, //7C
	0x00040000, 0x00048000, 0x00050000, 0x00058000, //80
	0x00060000, 0x00068000, 0x00070000, 0x00078000, //84
	0x00080000, 0x00090000, 0x000A0000, 0x000B0000, //88
	0x000C0000, 0x000D0000, 0x000E0000, 0x000F0000, //8C
	0x00100000, 0x00120000, 0x00140000, 0x00160000, //90
	0x00180000, 0x001A0000, 0x001C0000, 0x001E0000, //94
	0x00200000, 0x00240000, 0x00280000, 0x002C0000, //98
	0x00300000, 0x00340000, 0x00380000, 0x003C0000, //9C
	0x00400000, 0x00480000, 0x00500000, 0x00580000, //A0
	0x00600000, 0x00680000, 0x00700000, 0x00780000, //A4
	0x00800000, 0x00900000, 0x00A00000, 0x00B00000, //A8
	0x00C00000, 0x00D00000, 0x00E00000, 0x00F00000, //AC
	0x01000000, 0x01200000, 0x01400000, 0x01600000, //B0
	0x01800000, 0x01A00000, 0x01C00000, 0x01E00000, //B4
	0x02000000, 0x02400000, 0x02800000, 0x02C00000, //B8
	0x03000000, 0x03400000, 0x03800000, 0x03C00000, //BC
	0x04000000, 0x04800000, 0x05000000, 0x05800000, //C0
	0x06000000, 0x06800000, 0x07000000, 0x07800000, //C4
	0x08000000, 0x09000000, 0x0A000000, 0x0B000000, //C8
	0x0C000000, 0x0D000000, 0x0E000000, 0x0F000000, //CC
	0x10000000, 0x12000000, 0x14000000, 0x16000000, //D0
	0x18000000, 0x1A000000, 0x1C000000, 0x1E000000, //D4
	0x20000000, 0x24000000, 0x28000000, 0x2C000000, //D8
	0x30000000, 0x34000000, 0x38000000, 0x3C000000, //DC
	0x40000000, 0x48000000, 0x50000000, 0x58000000, //E0
	0x60000000, 0x68000000, 0x70000000, 0x78000000, //E4
	0x80000000, 0x90000000, 0xA0000000, 0xB0000000, //E8
	0xC0000000, 0xD0000000, 0xE0000000, 0xF0000000, //EC
};

int btlzh2_vextra[256]=
{
	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //0 
	 1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2, //1
	 3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4, //2
	 5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6, //3
	 7,  7,	 7,  7,	 7,  7,	 7,  7,	 8,  8,	 8,  8,	 8,  8,	 8,  8, //4
	 9,  9,	 9,  9,	 9,  9,	 9,  9, 10, 10, 10, 10, 10, 10, 10, 10, //5
	11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, //6
	13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, //7
	15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, //8
	17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, //9
	19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, //A
	21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22, //B
	23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, //C
	25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26, //D
	27, 27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 28, //E
	
	29, 29, 30, 30
};

force_inline int BTLZH2_Dec_ReadNBits(
	BGBBTJ_BTLZA_Context *ctx, int n)
{
	int i, j;

	i=(ctx->bs_win>>ctx->bs_pos)&((1<<n)-1);
	j=ctx->bs_pos+n;
	if(j>=8)
	{
		if(j>=16)
		{
			ctx->bs_win=(ctx->bs_win>>8)|((*ctx->cs++)<<24);
			ctx->bs_win=(ctx->bs_win>>8)|((*ctx->cs++)<<24);
			ctx->bs_pos=j-16;
		}else
		{
			ctx->bs_win=(ctx->bs_win>>8)|((*ctx->cs++)<<24);
			ctx->bs_pos=j-8;
		}
	}else
	{
		ctx->bs_pos=j;
	}
	return(i);
}

force_inline void BTLZH2_Dec_SkipNBits(
	BGBBTJ_BTLZA_Context *ctx, int n)
{
	int i;

	i=ctx->bs_pos+n;
	if(i>=8)
	{
		if(i>=16)
		{
			ctx->bs_win=(ctx->bs_win>>8)|((*ctx->cs++)<<24);
			ctx->bs_win=(ctx->bs_win>>8)|((*ctx->cs++)<<24);
			ctx->bs_pos=i-16;
		}else
		{
			ctx->bs_win=(ctx->bs_win>>8)|((*ctx->cs++)<<24);
			ctx->bs_pos=i-8;
		}
	}else
	{
		ctx->bs_pos=i;
	}
}

int BTLZH2_Dec_DecodeLzSymbol(BGBBTJ_BTLZA_Context *ctx)
{
	int i, j, k, j2, l, l2;

	i=ctx->bs_win>>ctx->bs_pos;
	k=ctx->bs_ltab_idx[i&0xFF];
	j=k&255;
	l=ctx->bs_ltab_len[j];

	if(l<=8)
	{
		BTLZH2_Dec_SkipNBits(ctx, l);
		return(j);
	}

	while((i&ctx->bs_ltab_mask[j])!=ctx->bs_ltab_code[j])
		j=ctx->bs_ltab_next[j];

	BTLZH2_Dec_SkipNBits(ctx, ctx->bs_ltab_len[j]);
	return(j);
}

int BTLZH2_Dec_DecodeDistanceSymbol(BGBBTJ_BTLZA_Context *ctx)
{
	int i, j, k, l;

	i=ctx->bs_win>>ctx->bs_pos;
	k=ctx->bs_dtab_idx[i&0xFF];
	j=k&255;
	l=(k>>24)&15;

	if(l<=8)
	{
		BTLZH2_Dec_SkipNBits(ctx, l);
		return(j);
	}

	while((i&ctx->bs_dtab_mask[j])!=ctx->bs_dtab_code[j])
		j=ctx->bs_dtab_next[j];

	BTLZH2_Dec_SkipNBits(ctx, ctx->bs_dtab_len[j]);
	return(j);
}


int BTLZH2_Dec_DecodeRawSymbol(BGBBTJ_BTLZA_Context *ctx)
{
	int i, j, k, j2, l, l2;

	i=ctx->bs_win>>ctx->bs_pos;
	k=ctx->bs_ltab_idx[i&0xFF];
	j=k&255;
	l=(k>>24)&15;

	if(l<=8)
	{
		BTLZH2_Dec_SkipNBits(ctx, l);
		return(j);
	}

	while((i&ctx->bs_ltab_mask[j])!=ctx->bs_ltab_code[j])
		j=ctx->bs_ltab_next[j];

	BTLZH2_Dec_SkipNBits(ctx, ctx->bs_ltab_len[j]);
	return(j);
}

int BTLZH2_Dec_TryDecodeRawSymbolPair(
	BGBBTJ_BTLZA_Context *ctx, byte *symab)
{
	int i, j, k, j2, l, l2;

	i=ctx->bs_win>>ctx->bs_pos;
	k=ctx->bs_ltab_idx[i&0xFF];
	j=k&255;
	l=ctx->bs_ltab_len[j];

	if(l<=8)
	{
		j2=(k>>9)&511;
		if(j2)
		{
			symab[0]=j;
			symab[1]=j2-1;
			l2=(k>>28)&15;
			BTLZH2_Dec_SkipNBits(ctx, l2);
			return(2);
		}

		symab[0]=j;
		BTLZH2_Dec_SkipNBits(ctx, l);
		return(1);
	}

	while((i&ctx->bs_ltab_mask[j])!=ctx->bs_ltab_code[j])
		j=ctx->bs_ltab_next[j];

	symab[0]=j;
	BTLZH2_Dec_SkipNBits(ctx, ctx->bs_ltab_len[j]);
	return(1);
}

int BTLZH2_Dec_DecodeRawSymbolPair(
	BGBBTJ_BTLZA_Context *ctx, byte *symab)
{
	int i;
	i=BTLZH2_Dec_TryDecodeRawSymbolPair(ctx, symab);
	if(i>=2)return(i);
	symab[1]=BTLZH2_Dec_DecodeRawSymbol(ctx);
	return(i+1);
}

int BTLZH2_Dec_TryDecodeRawSymbolQuad(
	BGBBTJ_BTLZA_Context *ctx, byte *symab)
{
	int i;
	i=BTLZH2_Dec_TryDecodeRawSymbolPair(ctx, symab);
	i+=BTLZH2_Dec_TryDecodeRawSymbolPair(ctx, symab+i);
	if(i<3)
		{ i+=BTLZH2_Dec_TryDecodeRawSymbolPair(ctx, symab+i); }
	return(i);
}

int BTLZH2_Dec_DecodeRawSymbolQuad(
	BGBBTJ_BTLZA_Context *ctx, byte *symab)
{
	int i;
	i=BTLZH2_Dec_TryDecodeRawSymbolQuad(ctx, symab);
	if(i>=4)return(i);
	symab[3]=BTLZH2_Dec_DecodeRawSymbol(ctx);
	return(i+1);
}

int BTLZH2_Dec_TryDecodeRawSymbolOct(
	BGBBTJ_BTLZA_Context *ctx, byte *symab)
{
	int i;
	i=BTLZH2_Dec_TryDecodeRawSymbolQuad(ctx, symab);
	i+=BTLZH2_Dec_TryDecodeRawSymbolQuad(ctx, symab+i);
	if(i<7)
		{ i+=BTLZH2_Dec_TryDecodeRawSymbolPair(ctx, symab+i); }
	return(i);
}

int BTLZH2_Dec_DecodeRawSymbolOct(
	BGBBTJ_BTLZA_Context *ctx, byte *symab)
{
	int i;
	i=BTLZH2_Dec_TryDecodeRawSymbolOct(ctx, symab);
	if(i>=8)return(i);
	symab[7]=BTLZH2_Dec_DecodeRawSymbol(ctx);
	return(i+1);
}

int BTLZH2_Dec_DecodeRawSymbolRun(
	BGBBTJ_BTLZA_Context *ctx, byte *syms, int len)
{
	byte *ct, *cte;

	ct=syms; cte=syms+len;
	while(ct<=(cte-8))
	{
		ct+=BTLZH2_Dec_TryDecodeRawSymbolOct(ctx, ct);
	}
	while(ct<=(cte-4))
	{
		ct+=BTLZH2_Dec_TryDecodeRawSymbolQuad(ctx, ct);
	}
	while(ct<=(cte-2))
	{
		ct+=BTLZH2_Dec_TryDecodeRawSymbolPair(ctx, ct);
	}
	if(ct<cte)
	{
		*ct=BTLZH2_Dec_DecodeRawSymbol(ctx);
	}
	return(0);
}

int BTLZH2_Dec_DecodeDistance(BGBBTJ_BTLZA_Context *ctx)
{

	int i, j, k;

	j=BTLZH2_Dec_DecodeDistanceSymbol(ctx);
	k=btlzh2_vbase[j]+BTLZH2_Dec_ReadNBits(ctx, btlzh2_vextra[j]);

	return(k);
}

int BTLZH2_Dec_DecodeCodeLengths(BGBBTJ_BTLZA_Context *ctx,
	byte *cl, int ncl)
{
	int i, j, k;

	i=0; k=0;
	while(i<ncl)
	{
		j=BTLZH2_Dec_DecodeClSymbol(ctx);
		if(j<0)return(j);

		if(j<16)
		{
			k=j;
			cl[i++]=j;
			continue;
		}

		if(j==16)
		{
			j=BTLZH2_Dec_Read2Bits(ctx)+3;
			while(j--)cl[i++]=k;
			continue;
		}

		if(j==17)
		{
			j=BTLZH2_Dec_Read3Bits(ctx)+3;
			while(j--)cl[i++]=0;
			k=0;
			continue;
		}
		if(j==18)
		{
			j=BTLZH2_Dec_ReadNBits(ctx, 7)+11;
			while(j--)cl[i++]=0;
			k=0;
			continue;
		}

		if(j==19)
		{
			j=BTLZH2_Dec_ReadBit(ctx)*2-1;
			k=k+j;
			cl[i++]=k;
			continue;
		}

		return(-7);
	}
	return(0);
}

int BTLZH2_Dec_DecodeHuffTable(BGBBTJ_BTLZA_Context *ctx)
{
	static int lorder[]={
		16, 17, 18, 19,  0,  8,  7,  9,
		 6, 10,  5, 11,  4, 12,  3, 13,
		 2, 14,  1, 15, 20, 21, 22, 23,
		24, 25, 26, 27, 28, 29, 30, 31};

	byte hcl[32], lcl[256], dcl[256], rcl[256], scl[256];
	int hm, hc, hrl, hll, hdl, hsl;
	int i, j;


	hm=BTLZH2_Dec_ReadNBits(ctx, 4);

//	hl=BTLZH2_Dec_ReadNBits(ctx, 5)+257;
//	hd=BTLZH2_Dec_ReadNBits(ctx, 5)+1;
	hc=BTLZH2_Dec_ReadNBits(ctx, 5)+1;
	ctx->bs_rhtab_n=0;

	for(i=0; i<32; i++)hcl[i]=0;
	for(i=0; i<hc; i++)
		hcl[lorder[i]]=BTLZH2_Dec_ReadNBits(ctx, 3);

	if(hm&1)
		{ hll=BTLZH2_Dec_ReadNBits(ctx, 8)+1; }
	if(hm&2)
		{ hdl=BTLZH2_Dec_ReadNBits(ctx, 8)+1; }
	if(hm&4)
		{ hrl=BTLZH2_Dec_ReadNBits(ctx, 8)+1; }
	if(hm&8)
		{ hsl=BTLZH2_Dec_ReadNBits(ctx, 8)+1; }

//	for(i=0; i<20; i++)printf("%d:%d ", i, hcl[i]);
//	printf("\n");

	j=BTLZA_BitDec_SetupTable(hcl, 32,
		ctx->bs_ltab_code, ctx->bs_ltab_mask, ctx->bs_ltab_len,
		ctx->bs_ltab_idx, ctx->bs_ltab_next);
	if(j<0)
	{
		printf("Codes Table Problem %d\n", j);
		for(i=0; i<20; i++)printf("%d:%d ", i, hcl[i]);
		printf("\n");
		return(j);
	}
	
	if(hm&1)
	{
		i=BTLZH2_Dec_DecodeCodeLengths(ctx, lcl, hll);
		if(i<0)return(i);

		j=BTLZH2_Dec_SetupTable(lcl, hll,
			ctx->bs_ltab_code, ctx->bs_ltab_mask, ctx->bs_ltab_len,
			ctx->bs_ltab_idx, ctx->bs_ltab_next);
		if(j<0)
		{
			printf("L Table Problem %d\n", j);
			for(i=0; i<hll; i++)printf("%d:%d ", i, lcl[i]);
			printf("\n");
			return(j);
		}
	}
	
	if(hm&2)
	{
		i=BTLZH2_Dec_DecodeCodeLengths(ctx, dcl, hdl);
		if(i<0)return(i);

		j=BTLZH2_Dec_SetupTable(dcl, hdl,
			ctx->bs_dtab_code, ctx->bs_dtab_mask, ctx->bs_dtab_len,
			ctx->bs_dtab_idx, ctx->bs_dtab_next);
		if(j<0)
		{
			printf("D Table Problem %d\n", j);
			for(i=0; i<hdl; i++)printf("%d:%d ", i, dcl[i]);
			printf("\n");
			return(j);
		}
	}

	if(hm&4)
	{
		i=BTLZH2_Dec_DecodeCodeLengths(ctx, rcl, hrl);
		if(i<0)return(i);

		j=BTLZH2_Dec_SetupTable(rcl, hrl,
			ctx->bs_rtab_code, ctx->bs_rtab_mask, ctx->bs_rtab_len,
			ctx->bs_rtab_idx, ctx->bs_rtab_next);
		if(j<0)
		{
			printf("R Table Problem %d\n", j);
			for(i=0; i<hrl; i++)printf("%d:%d ", i, rcl[i]);
			printf("\n");
			return(j);
		}
	}

	if(hm&8)
	{
		i=BTLZH2_Dec_DecodeCodeLengths(ctx, scl, hsl);
		if(i<0)return(i);

		j=BTLZH2_Dec_SetupTable(scl, hsl,
			ctx->bs_stab_code, ctx->bs_stab_mask, ctx->bs_stab_len,
			ctx->bs_stab_idx, ctx->bs_stab_next);
		if(j<0)
		{
			printf("S Table Problem %d\n", j);
			for(i=0; i<hsl; i++)printf("%d:%d ", i, scl[i]);
			printf("\n");
			return(j);
		}
	}

	return(0);
}
