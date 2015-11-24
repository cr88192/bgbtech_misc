#include <btlzazip.h>

extern u32 btlza_dbase[64];
extern int btlza_dextra[64];
extern u32 btlza_lbase[64];
extern int btlza_lextra[64];

extern int btlza_dbase1[64];
extern int btlza_dextra1[64];
extern int btlza_lbase1[32];
extern int btlza_lextra1[32];
extern int btlza_lbase2[32];
extern int btlza_lextra2[32];
extern int btlza_lbase3[64];
extern int btlza_lextra3[64];

void BTLZA_BitEncF_EncodeLZLength(BGBBTJ_BTLZA_Context *ctx, int l)
{
	int i, j, k;

	j=l;
	for(i=0; i<64; i++)
	{
		k=ctx->lbase[i]+(1<<ctx->lextra[i]);
//		if((j>=ctx->lbase[i]) && (j<k))
		if(j<k)
		{
			ctx->lzf_lstat[257+i]++;
			BTLZA_BitEnc_EncodeSymbol(ctx, 257+i);
			BTLZA_BitEnc_WriteNBits(ctx, j-ctx->lbase[i], ctx->lextra[i]);
			break;
		}
	}
}

void BTLZA_BitEncF_EncodeLZDist(BGBBTJ_BTLZA_Context *ctx, int d)
{
	int i, j, k;

	j=d;
	for(i=0; i<64; i++)
	{
		k=btlza_dbase1[i]+(1<<btlza_dextra1[i]);
//		if((j>=btlza_dbase1[i]) && (j<k))
		if(j<k)
		{
			if(!ctx->bs_dtab_len[i])
				printf("BTLZA_BitEnc_EncodeLZDist: "
					"no dist sym %d, %d\n", i, j);

			ctx->lzf_dstat[i]++;
			BTLZA_BitEnc_WriteNBits(ctx, ctx->bs_dtab_code[i],
				ctx->bs_dtab_len[i]);

			BTLZA_BitEnc_WriteNBits(ctx, j-btlza_dbase1[i], btlza_dextra1[i]);
			break;
		}
	}
}

void BTLZA_BitEncF_EncodeLZSpecial(BGBBTJ_BTLZA_Context *ctx, int d)
{
	int i, j, k;

	j=d;
	for(i=0; i<64; i++)
	{
		k=btlza_dbase1[i]+(1<<btlza_dextra1[i]);
//		if((j>=btlza_dbase1[i]) && (j<k))
		if(j<k)
		{
			ctx->lzf_lstat[318+i]++;
			BTLZA_BitEnc_EncodeSymbol(ctx, 318+i);

			BTLZA_BitEnc_WriteNBits(ctx, j-btlza_dbase1[i], btlza_dextra1[i]);
			break;
		}
	}
}

void BTLZA_BitEncF_EncodeLZRun(BGBBTJ_BTLZA_Context *ctx, int l, int d)
{
	int i, j, k;

#if 1
	if((ctx->lz_lastdist==d) || (ctx->lz_lastrun==l))
	{
		if((ctx->lz_lastdist==d) && (ctx->lz_lastrun==l))
		{
			BTLZA_BitEncF_EncodeLZSpecial(ctx, 1);
			return;
		}
		
		if(ctx->lz_lastrun==l)
		{
			ctx->lz_lastdist=d;
			BTLZA_BitEncF_EncodeLZSpecial(ctx, 2);
			BTLZA_BitEncF_EncodeLZDist(ctx, d);
			return;
		}

		if(ctx->lz_lastdist==d)
		{
			ctx->lz_lastrun=l;
			BTLZA_BitEncF_EncodeLZSpecial(ctx, 3);
			BTLZA_BitEncF_EncodeLZLength(ctx, l);
			return;
		}
	}
#endif

	ctx->lz_lastdist=d;
	ctx->lz_lastrun=l;

	j=l;
	for(i=0; i<64; i++)
	{
		k=ctx->lbase[i]+(1<<ctx->lextra[i]);
//		if((j>=ctx->lbase[i]) && (j<k))
		if(j<k)
		{
			ctx->lzf_lstat[257+i]++;
			BTLZA_BitEnc_EncodeSymbol(ctx, 257+i);
			BTLZA_BitEnc_WriteNBits(ctx, j-ctx->lbase[i], ctx->lextra[i]);
			break;
		}
	}

	j=d;
	for(i=0; i<64; i++)
	{
		k=btlza_dbase1[i]+(1<<btlza_dextra1[i]);
//		if((j>=btlza_dbase1[i]) && (j<k))
		if(j<k)
		{
			if(!ctx->bs_dtab_len[i])
				printf("BTLZA_BitEnc_EncodeLZRun: "
					"no dist sym %d, %d\n", i, j);

			ctx->lzf_dstat[i]++;
			BTLZA_BitEnc_WriteNBits(ctx, ctx->bs_dtab_code[i],
				ctx->bs_dtab_len[i]);

			BTLZA_BitEnc_WriteNBits(ctx, j-btlza_dbase1[i], btlza_dextra1[i]);
			break;
		}
	}
}

int BTLZA_BitEncF_LZEncodeBuffer(
	BGBBTJ_BTLZA_Context *ctx, byte *ibuf, int isz)
{
	byte *cs, *cse, *cs1, *ct1, *cse1;
	u32 h2;
	int i, j, k, l, h;
	
//	h2=0;
	cs=ibuf; cse=ibuf+isz;
//	h2=(cs[0])|(cs[1]<<8)|(cs[2]<<16)|(cs[3]<<24);
	h2=(cs[0]<<8)|(cs[1]<<16)|(cs[2]<<24);
	while(cs<cse)
	{
		k=0; l=0;
//		h=(((((cs[0]*251)+cs[1])*251+cs[2])*251)>>8)&4095;
//		h2=(h2>>8);
//		h=((h2*65521)>>16)&4095;
		h=((h2*65521)>>16)&16383;
//		h2=h2|(cs[3]<<24);
		h2=(h2<<8)|(cs[3]);
//		h2=(h2>>8)|(cs[3]<<24);

		if(ctx->lzf_hash[h])
		{
			cs1=cs; ct1=ctx->lzf_hash[h];
			k=cs1-ct1;
			if(k<ctx->lz_maxdist)
			{
				cse1=cs+ctx->lz_maxlen;
				if(cse<cse1)cse1=cse;
				while(cs1<cse1)
				{
					if(*cs1!=*ct1)
						break;
					cs1++; ct1++;
				}
				k=cs1-ct1;
				l=cs1-cs;
			}
		}

		if(l>=3)
		{
			BTLZA_BitEncF_EncodeLZRun(ctx, l, k);

			ctx->lzf_hash[h]=cs++;
			while(cs<cs1)
			{
//				h=(((((cs[0]*251)+cs[1])*251+cs[2])*251)>>8)&4095;
//				h2=(h2>>8);
//				h=((h2*65521)>>16)&4095;
				h=((h2*65521)>>16)&16383;
//				h2=h2|(cs[3]<<24);
				h2=(h2<<8)|(cs[3]);
//				h2=(h2>>8)|(cs[3]<<24);
				ctx->lzf_hash[h]=cs++;
			}
			continue;
		}
		
		ctx->lzf_lstat[*cs]++;
		ctx->lzf_hash[h]=cs;
		BTLZA_BitEnc_EncodeSymbol(ctx, *cs++);
	}
	BTLZA_BitEnc_EncodeSymbol(ctx, 256);
	return(0);
}

int BTLZA_BitEncF_EncodeBlockDynamic(BGBBTJ_BTLZA_Context *ctx,
	byte *ibuf, int isz, int last)
{
	static int lorder[]={
		16, 17, 18, 0, 8,7, 9,6, 10,5, 11,4, 12,3, 13,2, 14,1, 15};
	int hstat[24];
	byte lcl[320], dcl[64], hcl[24], hcl2[24];
	int lc, dc, hc;
	int i, j, n;

	for(i=0; i<286; i++)
		ctx->lzf_lstat[i]++;
	for(i=0; i<32; i++)
		ctx->lzf_dstat[i]++;

	BTLZA_BitEnc_BuildLengthsAdjust(ctx->lzf_lstat, 288, lcl, 15);
	BTLZA_BitEnc_BuildLengthsAdjust(ctx->lzf_dstat, 32, dcl, 15);

	for(i=0; i<32; i++)if(dcl[i])break;
	if(i==32)for(i=0; i<32; i++)dcl[i]=5;

	for(lc=257; lc<286; lc++)
	{
		for(i=lc; i<286; i++)
			if(lcl[i])break;
		if(i==286)break;
	}

	for(dc=1; dc<32; dc++)
	{
		for(i=dc; i<32; i++)
			if(dcl[i])break;
		if(i==32)break;
	}

	for(i=0; i<24; i++)hstat[i]=0;
	BTLZA_BitEnc_StatLengths(ctx, lcl, lc, hstat);
	BTLZA_BitEnc_StatLengths(ctx, dcl, dc, hstat);
	BTLZA_BitEnc_BuildLengthsAdjust(hstat, 24, hcl, 7);

	for(i=0; i<19; i++)hcl2[i]=hcl[lorder[i]];

	for(hc=4; hc<19; hc++)
	{
		for(i=hc; i<19; i++)
			if(hcl2[i])break;
		if(i==19)break;
	}

	BTLZA_BitEnc_WriteBit(ctx, last);
	BTLZA_BitEnc_Write2Bits(ctx, 2);

	BTLZA_BitEnc_Write5Bits(ctx, lc-257);
	BTLZA_BitEnc_Write5Bits(ctx, dc-1);
	BTLZA_BitEnc_Write4Bits(ctx, hc-4);

	for(i=0; i<hc; i++)
		BTLZA_BitEnc_Write3Bits(ctx, hcl[lorder[i]]);

	j=BTLZA_BitDec_SetupTable(hcl, 24,
		ctx->bs_ltab_code, ctx->bs_ltab_mask, ctx->bs_ltab_len,
		ctx->bs_ltab_idx, ctx->bs_ltab_next);
	if(j<0)return(j);

	BTLZA_BitEnc_EncodeLengths(ctx, lcl, lc);
	BTLZA_BitEnc_EncodeLengths(ctx, dcl, dc);

	j=BTLZA_BitDec_SetupTable(lcl, lc,
		ctx->bs_ltab_code, ctx->bs_ltab_mask, ctx->bs_ltab_len,
		ctx->bs_ltab_idx, ctx->bs_ltab_next);
	if(j<0)return(j);

	j=BTLZA_BitDec_SetupTable(dcl, dc,
		ctx->bs_dtab_code, ctx->bs_dtab_mask, ctx->bs_dtab_len,
		ctx->bs_dtab_idx, ctx->bs_dtab_next);
	if(j<0)return(j);

//	BTLZA_BitEnc_EncodeLZBuffer(ctx, ctx->lz_tbuf, ctx->lz_mbuf, n);
	n=BTLZA_BitEncF_LZEncodeBuffer(ctx, ibuf, isz);
	return(0);
}

int BTLZA_BitEncF_EncodeBlockBTLZH(BGBBTJ_BTLZA_Context *ctx,
	byte *ibuf, int isz, int last)
{
	static int lorder[]={
		16, 17, 18, 0, 8,7, 9,6, 10,5, 11,4, 12,3, 13,2, 14,1, 15};

	int hstat[24];
	byte lcl[512], dcl[128], hcl[24], hcl2[24];
	int lc, dc, hc;
	int i, j, n;

	for(i=0; i<336; i++)
		ctx->lzf_lstat[i]++;
	for(i=0; i<64; i++)
		ctx->lzf_dstat[i]++;

	BTLZA_BitEnc_BuildLengthsAdjust(ctx->lzf_lstat, 336, lcl, 15);
	BTLZA_BitEnc_BuildLengthsAdjust(ctx->lzf_dstat, 64, dcl, 15);

	for(i=0; i<64; i++)if(dcl[i])break;
	if(i==64)for(i=0; i<64; i++)dcl[i]=6;

	for(lc=257; lc<336; lc++)
	{
		for(i=lc; i<336; i++)
			if(lcl[i])break;
		if(i==336)break;
	}

	for(dc=1; dc<64; dc++)
	{
		for(i=dc; i<64; i++)
			if(dcl[i])break;
		if(i==64)break;
	}

	for(i=0; i<24; i++)hstat[i]=0;
	BTLZA_BitEnc_StatLengths(ctx, lcl, lc, hstat);
	BTLZA_BitEnc_StatLengths(ctx, dcl, dc, hstat);
	BTLZA_BitEnc_BuildLengthsAdjust(hstat, 24, hcl, 7);

	for(i=0; i<19; i++)hcl2[i]=hcl[lorder[i]];

	for(hc=4; hc<19; hc++)
	{
		for(i=hc; i<19; i++)
			if(hcl2[i])break;
		if(i==19)break;
	}

	BTLZA_BitEnc_WriteBit(ctx, last);
	BTLZA_BitEnc_Write2Bits(ctx, 3);
	BTLZA_BitEnc_Write3Bits(ctx, 2);

	BTLZA_BitEnc_Write3Bits(ctx, 0);

	BTLZA_BitEnc_Write7Bits(ctx, lc-257);
	BTLZA_BitEnc_Write6Bits(ctx, dc-1);
	BTLZA_BitEnc_Write5Bits(ctx, hc-4);

	for(i=0; i<hc; i++)
		BTLZA_BitEnc_Write3Bits(ctx, hcl[lorder[i]]);

	j=BTLZA_BitDec_SetupTable(hcl, 24,
		ctx->bs_ltab_code, ctx->bs_ltab_mask, ctx->bs_ltab_len,
		ctx->bs_ltab_idx, ctx->bs_ltab_next);
	if(j<0)return(j);

	BTLZA_BitEnc_EncodeLengths(ctx, lcl, lc);
	BTLZA_BitEnc_EncodeLengths(ctx, dcl, dc);

	j=BTLZA_BitDec_SetupTable(lcl, lc,
		ctx->bs_ltab_code, ctx->bs_ltab_mask, ctx->bs_ltab_len,
		ctx->bs_ltab_idx, ctx->bs_ltab_next);
	if(j<0)return(j);

	j=BTLZA_BitDec_SetupTable(dcl, dc,
		ctx->bs_dtab_code, ctx->bs_dtab_mask, ctx->bs_dtab_len,
		ctx->bs_dtab_idx, ctx->bs_dtab_next);
	if(j<0)return(j);

	n=BTLZA_BitEncF_LZEncodeBuffer(ctx, ibuf, isz);

	return(0);
}

int BTLZA_BitEncF_EncodeBlock(BGBBTJ_BTLZA_Context *ctx,
	byte *ibuf, int isz, int last)
{
	int i;

	i=isz;
//	if(!ctx->lz_tbuf || (isz>ctx->lz_tsz))
//	{
//		if(ctx->lz_tbuf)gcfree(ctx->lz_tbuf);
//		if(ctx->lz_mbuf)gcfree(ctx->lz_mbuf);
//		ctx->lz_tsz=i; ctx->lz_msz=(i+7)/8;
//		ctx->lz_tbuf=gcalloc(ctx->lz_tsz);
//		ctx->lz_mbuf=gcalloc(ctx->lz_msz);
//	}

	if(ctx->lz_maxdist>65536)
	{
		ctx->lbase=btlza_lbase3; ctx->lextra=btlza_lextra3;
		return(BTLZA_BitEncF_EncodeBlockBTLZH(ctx, ibuf, isz, last));
	}else
	{
		if(ctx->lz_maxdist>32768)
			{ ctx->lbase=btlza_lbase2; ctx->lextra=btlza_lextra2; }
		else
			{ ctx->lbase=btlza_lbase1; ctx->lextra=btlza_lextra1; }
		return(BTLZA_BitEnc_EncodeBlockDynamic(ctx, ibuf, isz, last));
	}
}

int BTLZA_BitEncF_EncodeStream_I(BGBBTJ_BTLZA_Context *ctx,
	byte *ibuf, byte *obuf, int isz, int osz)
{
	byte *s;
	int i, j, k;

	ctx->BS_WriteByte=BTLZA_BitEnc_WriteByteBasic;

	ctx->ibuf=ibuf;
	ctx->obuf=obuf;
	ctx->bs_pos=0;
	ctx->bs_win=0;

	ctx->ct=obuf;
	ctx->cte=obuf+osz;

//	BTLZA_Encode_CheckAllocWindow(ctx);

//	if(!ctx->lzf_hash)
//		{ ctx->lzf_hash=gcalloc(4096*sizeof(byte *)); }
//	memset(ctx->lzf_hash, 0, 4096*sizeof(byte *));

	if(!ctx->lzf_hash)
		{ ctx->lzf_hash=btlza_malloc(16384*sizeof(byte *)); }
	memset(ctx->lzf_hash, 0, 16384*sizeof(byte *));

	ctx->lz_lastdist=0;
	ctx->lz_lastrun=0;

#if 1
	for(i=0; i<512; i++)
		ctx->lzf_lstat[i]=0;
	for(i=0; i<128; i++)
		ctx->lzf_dstat[i]=0;
	for(i=0; i<286; i++)
		ctx->lzf_lstat[i]=16;
	for(i=0; i<32; i++)
		ctx->lzf_dstat[i]=4;
#endif

	s=ibuf;
	i=isz;	k=0;

#ifdef BTLZA_ENCF_CHUNK1
	if(i>BTLZA_ENCF_CHUNK1)
	{
		j=BTLZA_BitEncF_EncodeBlock(ctx, s, BTLZA_ENCF_CHUNK1, 0);
		if(j<0)return(j);
		i-=BTLZA_ENCF_CHUNK1;
		s+=BTLZA_ENCF_CHUNK1;
	}
//	printf("\n");
#endif

#ifdef BTLZA_ENCF_CHUNK2
	while(i>BTLZA_ENCF_CHUNK2)
	{
		j=BTLZA_BitEncF_EncodeBlock(ctx, s, BTLZA_ENCF_CHUNK2, 0);
		if(j<0)return(j);
		i-=BTLZA_ENCF_CHUNK2;
		s+=BTLZA_ENCF_CHUNK2;
	}
//	printf("\n");
#endif

	j=BTLZA_BitEncF_EncodeBlock(ctx, s, i, 1);
	if(j<0)return(j);

	BTLZA_BitEnc_FlushBits(ctx);
	return(ctx->ct-obuf);
}

BGBBTJ_API int BTLZA_BitEncF_EncodeStream32Lvl(
	byte *ibuf, byte *obuf, int isz, int osz, int l)
{
//	static int sd[10]=
//		{1, 16, 16, 64, 64, 256, 256, 1024, 4096, 16384};
//	static int md[10]=
//		{1, 4096, 8192, 4096, 8192, 8192, 16384, 16384, 32768, 32768};

	BGBBTJ_BTLZA_Context *ctx;
	int i;

	ctx=BTLZA_AllocContext();
//	ctx->lz_sdepth=sd[l&15];
//	ctx->lz_maxdist=md[l&15];
	ctx->lz_sdepth=1;
	ctx->lz_maxdist=32768;
	ctx->lz_maxlen=258;
	ctx->lz_wsize=65536;
	ctx->lz_wmask=65535;
	ctx->bs_flags=l>>8;

	i=BTLZA_BitEncF_EncodeStream_I(ctx, ibuf, obuf, isz, osz);
	BTLZA_FreeContext(ctx);
	return(i);
}

BGBBTJ_API int BTLZA_BitEncF_EncodeStream32LvlZl(
	byte *ibuf, byte *obuf,
	int isz, int osz, int lvl)
{
	int i, j, k, l;

	j=BTLZA_BitEncF_EncodeStream32Lvl(ibuf, obuf+2, isz, osz-6, lvl);

	k=(7<<12)|(8<<8)|(3<<6); l=k%31;
	if(l)k|=31-l;
	obuf[0]=k>>8; obuf[1]=k&0xFF;

	k=BGBBTJ_BufPNG_DataAdler32(ibuf, isz, 1);
	obuf[2+j+0]=(k>>24)&0xFF;
	obuf[2+j+1]=(k>>16)&0xFF;
	obuf[2+j+2]=(k>>8)&0xFF;
	obuf[2+j+3]=k&0xFF;

	j+=6;
	
	return(j);
}

BGBBTJ_API int BTLZA_BitEncF_EncodeStreamXLvl(
	byte *ibuf, byte *obuf, int isz, int osz, int l)
{
//	static int sd[10]=
//		{1, 16, 16, 64, 64, 256, 256, 1024, 4096, 16384};
//	static int md[10]=
//		{1, 4096, 8192, 4096, 8192, 8192, 16384, 16384, 32768, 32768};

	BGBBTJ_BTLZA_Context *ctx;
	int i;

	ctx=BTLZA_AllocContext();
//	ctx->lz_sdepth=sd[l&15];
//	ctx->lz_maxdist=md[l&15];
	ctx->lz_sdepth=1;
	ctx->lz_maxdist=1<<20;
	ctx->lz_maxlen=65536;
	ctx->lz_wsize=65536;
	ctx->lz_wmask=65535;
	ctx->bs_flags=l>>8;

	i=BTLZA_BitEncF_EncodeStream_I(ctx, ibuf, obuf, isz, osz);
	BTLZA_FreeContext(ctx);
	return(i);
}

BGBBTJ_API int BTLZA_BitEncF_EncodeStreamXLvlZl(
	byte *ibuf, byte *obuf,
	int isz, int osz, int lvl)
{
	int i, j, k, l;

	j=BTLZA_BitEncF_EncodeStreamXLvl(ibuf, obuf+2, isz, osz-6, lvl);

	k=(7<<12)|(10<<8)|(3<<6); l=k%31;
	if(l)k|=31-l;
	obuf[0]=k>>8; obuf[1]=k&0xFF;

	k=BGBBTJ_BufPNG_DataAdler32(ibuf, isz, 1);
	obuf[2+j+0]=(k>>24)&0xFF;
	obuf[2+j+1]=(k>>16)&0xFF;
	obuf[2+j+2]=(k>>8)&0xFF;
	obuf[2+j+3]=k&0xFF;

	j+=6;
	
	return(j);
}

BGBBTJ_API int BTLZA_BitEncF_EncodeStreamXLvlZlc(
	byte *ibuf, byte *obuf,
	int isz, int osz, int lvl)
{
	int i, j, k, l;

	j=BTLZA_BitEncF_EncodeStreamXLvl(ibuf, obuf+1, isz, osz-1, lvl);
	obuf[0]=0x8A;	
	return(j+1);
}
