#include <btlzazip.h>

extern u32 btlza_dbase[64];
extern int btlza_dextra[64];
extern u32 btlza_lbase[64];
extern int btlza_lextra[64];

void BTLZA_Encode_UpdateWindowByte(BGBBTJ_BTLZA_Context *ctx,
	byte *s)
{
	int h;

	h=(((s[0]*251+s[1])*251+s[2])*251)&0x3FFF;
//	h=(s[0]^(s[1]<<4)^(s[2]<<8))&0xFFF;
//	h=(s[0]^(s[1]<3)^(s[2]<<6))&0xFFF;
	ctx->lz_lbuf[ctx->lz_wpos]=ctx->lz_hash[h];
	ctx->lz_hash[h]=ctx->lz_wpos;
	ctx->lz_wbuf[ctx->lz_wpos]=*s;
	ctx->lz_wpos=(ctx->lz_wpos+1)&ctx->lz_wmask;
}

void BTLZA_Encode_UpdateWindowString(BGBBTJ_BTLZA_Context *ctx,
	byte *s, int l)
{
	int h;

	while(l--)
	{
		h=(((s[0]*251+s[1])*251+s[2])*251)&0x3FFF;
//		h=(s[0]^(s[1]<<4)^(s[2]<<8))&0xFFF;
//		h=(s[0]^(s[1]<<3)^(s[2]<<6))&0xFFF;
		ctx->lz_lbuf[ctx->lz_wpos]=ctx->lz_hash[h];
		ctx->lz_hash[h]=ctx->lz_wpos;
		ctx->lz_wbuf[ctx->lz_wpos]=*s++;
		ctx->lz_wpos=(ctx->lz_wpos+1)&ctx->lz_wmask;
	}
}

int BTLZA_Encode_LookupString(BGBBTJ_BTLZA_Context *ctx,
	byte *cs, byte *ce, int *rl, int *rd)
{
	byte *s, *t, *se;
	int i, j, l;
	int bl, bi, md, sd;

	se=cs+ctx->lz_maxlen;
	if(ce<se)se=ce;
	if((ce-cs)<3)return(0);

	md=ctx->lz_maxdist;
	sd=ctx->lz_sdepth;

	i=(((cs[0]*251+cs[1])*251+cs[2])*251)&0x3FFF;
//	i=(cs[0]^(cs[1]<<4)^(cs[2]<<8))&0xFFF;
	i=ctx->lz_hash[i];

#if 1
	bl=0; bi=0; l=2;
	while(l && (sd--))
	{
		j=ctx->lz_wpos-i;
		if(j<=0)j+=ctx->lz_wsize;
		if(j>=md)break;

//		if(i<(ctx->lz_wsize-258))
		if(i<(ctx->lz_wsize-ctx->lz_maxlen))
		{
			s=cs; t=ctx->lz_wbuf+i;
			while((s<se) && (*s==*t)) { s++; t++; }
			j=s-cs;
		}else
		{
			s=cs;
			for(j=0; ((*s++)==ctx->lz_wbuf[(i+j)&ctx->lz_wmask]) &&
				(s<se); j++);
		}

		if(j>bl)
		{
			bl=j; bi=i;

//			if(bl>=258)break;
			if(bl>=ctx->lz_maxlen)break;
			if(bl>64)
			{
				if(sd>256)sd=256;
				if(bl>128)if(sd>128)sd=128;
				if(bl>192)
				{
					if(sd>64)sd=64;
					if(bl>224)if(sd>16)sd=16;
//					if(bl>=256)if(sd>4)sd=4;
				}
			}
		}

		j=ctx->lz_lbuf[i];
		if(j>=i)l--;
		i=j;
	}
#endif

//	printf("ok\n");

	if(bl>2)
	{
		i=ctx->lz_wpos-bi;
		if(i<0)i+=ctx->lz_wsize;

		if((i>=65536) && (bl<6))
			return(0);
		if((i>=262144) && (bl<10))
			return(0);

		*rl=bl;
		*rd=i;

		return(1);
	}
	return(0);
}

int BTLZA_Encode_LZCompressBuffer(BGBBTJ_BTLZA_Context *ctx,
	byte *ibuf, byte *obuf, byte *mbuf, int sz)
{
	byte *cs, *ce, *ct, *mt;
	int l, d, l2, d2, mi, ni;
	int mp, n, pi, pj;
	int i, j;

//	if(ctx->lz_sdepth<2)
	if(ctx->lz_sdepth<1)
	{
		i=BTLZA_Encode_LZCompressBufferRLE(ctx, ibuf, obuf, mbuf, sz);
		return(i);
	}

	cs=ibuf;
	ce=ibuf+sz;
	ct=obuf; mt=mbuf;

	mp=0; n=0; pi=ctx->lz_wpos; pj=0;
	mi=BTLZA_Encode_LookupString(ctx, cs, ce, &l, &d);
	while(cs<ce)
	{
		j=((ce-cs)>=(ctx->lz_maxlen))?(ctx->lz_maxlen):(ce-cs);
		for(i=pj; i<j; i++)
			{ ctx->lz_wbuf[pi]=cs[i]; pi=(pi+1)&ctx->lz_wmask; pj++; }

		ni=BTLZA_Encode_LookupString(ctx, cs+1, ce, &l2, &d2);
		if(mi && ni && (l2>(l+1)))mi=0;

		if(mi)
		{
			*mt|=1<<(mp++);
			if(mp>=8) { mt++; mp-=8; }
			n++;

			if((l>=258) || (d>=65536))
			{
				*ct++=255;
				*ct++=l&0xFF;
				*ct++=(l>>8)&0xFF;
				*ct++=d&0xFF;
				*ct++=(d>>8)&0xFF;
				*ct++=(d>>16)&0xFF;
			}else
			{
				*ct++=l-3;
				*ct++=d&0xFF;
				*ct++=(d>>8)&0xFF;
			}

			BTLZA_Encode_UpdateWindowString(ctx, cs, l);
			cs+=l;
			pj-=l;

			j=((ce-cs)>=(ctx->lz_maxlen))?(ctx->lz_maxlen):(ce-cs);
			for(i=pj; i<j; i++)
				{ ctx->lz_wbuf[pi]=cs[i];
				pi=(pi+1)&ctx->lz_wmask; pj++; }

			mi=BTLZA_Encode_LookupString(ctx, cs, ce, &l, &d);
			continue;
		}

		*mt&=~(1<<(mp++));
		if(mp>=8) { mt++; mp-=8; }
		n++;

		BTLZA_Encode_UpdateWindowByte(ctx, cs);
		*ct++=*cs++;
		pj--;

		mi=ni; l=l2; d=d2+1;
	}

	if((ct-obuf)>sz)
	{
		printf("Deflate: LZ Buffer Overflow\n");
		*(int *)-1=-1;
	}

	return(n);
}

int BTLZA_Encode_LZCompressBufferRLE(BGBBTJ_BTLZA_Context *ctx,
	byte *ibuf, byte *obuf, byte *mbuf, int sz)
{
	byte *cs, *ce, *ct, *mt;
	int l, d, l2, d2, mi, ni, mxl;
	int mp, n, pi, pj;
	int i, j, lc;

	cs=ibuf;
	ce=ibuf+sz;
	ct=obuf; mt=mbuf;
	mxl=ctx->lz_maxlen;

	mp=0; n=0; pi=ctx->lz_wpos; pj=0; lc=-1;
	while(cs<ce)
	{
		j=ce-cs;
		if(j>mxl) { j=mxl; }
		for(i=0; i<j; i++)
			if(cs[i]!=lc)
				break;
		l=i; d=1;
//		l=0; d=1;
	
		if(l>=3)
		{
			*mt|=1<<(mp++);
			if(mp>=8) { mt++; mp-=8; }
			n++;

			if((l>=258) || (d>=65536))
			{
				*ct++=255;
				*ct++=l&0xFF;
				*ct++=(l>>8)&0xFF;
				*ct++=d&0xFF;
				*ct++=(d>>8)&0xFF;
				*ct++=(d>>16)&0xFF;
			}else
			{
				*ct++=l-3;
				*ct++=d&0xFF;
				*ct++=(d>>8)&0xFF;
			}

			cs+=l;
			continue;
		}

		*mt&=~(1<<(mp++));
		if(mp>=8) { mt++; mp-=8; }
		n++;

		lc=*cs++;
		*ct++=lc;
	}

	if((ct-obuf)>sz)
	{
		printf("Deflate: LZ Buffer Overflow\n");
		*(int *)-1=-1;
	}

	return(n);
}

void BTLZA_Encode_StatLZRun(BGBBTJ_BTLZA_Context *ctx,
	byte *cs, int *lstat, int *dstat)
{
	int i, j, k;

	j=cs[0]+3;
	if(j==258)
	{
		lstat[285]++;
	}else
	{
		for(i=0; i<29; i++)
		{
			k=btlza_lbase[i]+(1<<btlza_lextra[i]);
			if((j>=btlza_lbase[i]) && (j<k))
			{
				lstat[257+i]++;
				break;
			}
		}
	}

	j=cs[1]|(cs[2]<<8);
	for(i=0; i<64; i++)
	{
		k=btlza_dbase[i]+(1<<btlza_dextra[i]);
		if((j>=btlza_dbase[i]) && (j<k))
		{
			dstat[i]++;
			break;
		}
	}
}

void BTLZA_Encode_StatLZBuffer(BGBBTJ_BTLZA_Context *ctx,
	byte *tbuf, byte *mbuf, int n,
	int *lstat, int *dstat)
{
	byte *s;
	int i;

	for(i=0; i<320; i++)lstat[i]=0;
	for(i=0; i<64; i++)dstat[i]=0;

	s=tbuf;
	for(i=0; i<n; i++)
	{
		if(mbuf[i>>3]&(1<<(i&7)))
		{
			BTLZA_Encode_StatLZRun(ctx, s, lstat, dstat);
//			s+=3;
			if(s[0]==255)	{ s+=6; }
			else			{ s+=3; }
			continue;
		}

		lstat[*s++]++;
	}
	lstat[256]++;
}

#if 0
void BTLZA_Encode_EncodeSymbol(BGBBTJ_BTLZA_Context *ctx, int v)
{
	BTLZA_BitArith_EncodeSymbol9(ctx, v, ctx->mdl_lit, ctx->ctxmask);
}

void BTLZA_Encode_EncodeDistSymbol(BGBBTJ_BTLZA_Context *ctx, int v)
{
	BTLZA_BitArith_EncodeSymbol6(ctx, v, ctx->mdl_dist, 4095);
}

void BTLZA_Encode_EncodeLZRun(BGBBTJ_BTLZA_Context *ctx, byte *cs)
{
	int l, d;
	int i, j, k;

	if(cs[0]==255)
	{
		l=cs[1]|(cs[2]<<8);
		d=cs[3]|(cs[4]<<8)|(cs[5]<<16);
	}else
	{
		l=cs[0]+3;
		d=cs[1]|(cs[2]<<8);
	}

	j=l;
	for(i=0; i<60; i++)
	{
		k=btlza_lbase[i]+(1<<btlza_lextra[i]);
		if((j>=btlza_lbase[i]) && (j<k))
		{
			BTLZA_Encode_EncodeSymbol(ctx, 257+i);

			ctx->wctx&=4095;
			BTLZA_BitArith_OutputExtraBits(ctx,
				j-btlza_lbase[i], btlza_lextra[i],
				ctx->mdl_xbits, 4095);
//			BTLZA_Encode_WriteNBits(j-lbase[i], lextra[i]);
			break;
		}
	}

	j=d;
	for(i=0; i<64; i++)
	{
		k=btlza_dbase[i]+(1<<btlza_dextra[i]);
		if((j>=btlza_dbase[i]) && (j<k))
		{
			BTLZA_Encode_EncodeDistSymbol(ctx, i);
			BTLZA_BitArith_OutputExtraBits(ctx,
				j-btlza_dbase[i], btlza_dextra[i],
				ctx->mdl_xbits, 4095);
//			BTLZA_Encode_WriteNBits(j-dbase[i], dextra[i]);
			break;
		}
	}

	ctx->wctx=0;
}

void BTLZA_Encode_EncodeLZBuffer(BGBBTJ_BTLZA_Context *ctx,
	byte *tbuf, byte *mbuf, int n)
{
	byte *s;
	int i;

	s=tbuf;
	for(i=0; i<n; i++)
	{
		if(mbuf[i>>3]&(1<<(i&7)))
		{
			BTLZA_Encode_EncodeLZRun(ctx, s);
			if(s[0]==255)	{ s+=6; }
			else			{ s+=3; }
			continue;
		}

		BTLZA_Encode_EncodeSymbol(ctx, *s++);
	}
	BTLZA_Encode_EncodeSymbol(ctx, 256);
}

int BTLZA_Encode_EncodeBlock(BGBBTJ_BTLZA_Context *ctx,
	byte *ibuf, int isz)
{
	static int lorder[]={
		16, 17, 18, 0, 8,7, 9,6, 10,5, 11,4, 12,3, 13,2, 14,1, 15};

//	static byte *tbuf=NULL, *mbuf=NULL;
//	static int tsz, msz;

//	int lstat[288], dstat[32], hstat[24];
//	byte lcl[288], dcl[32], hcl[24], hcl2[24];
//	int lc, dc, hc;
	int fl, ll, ws, le;
	int i, j, n;

	i=isz;
	if(!ctx->lz_tbuf || (isz>ctx->lz_tsz))
	{
		if(ctx->lz_tbuf)btlza_free(ctx->lz_tbuf);
		if(ctx->lz_mbuf)btlza_free(ctx->lz_mbuf);
		ctx->lz_tsz=i; ctx->lz_msz=(i+7)/8;
		ctx->lz_tbuf=btlza_malloc(ctx->lz_tsz);
		ctx->lz_mbuf=btlza_malloc(ctx->lz_msz);
	}

	n=BTLZA_Encode_LZCompressBuffer(ctx,
		ibuf, ctx->lz_tbuf, ctx->lz_mbuf, isz);

#if 0
//	BTLZA_Encode_StatLZBuffer(ctx->lz_tbuf, ctx->lz_mbuf, n, lstat, dstat);

	if(ctx->lz_sdepth<2)
	{
		for(i=0; i<256; i++)
			lstat[i]++;
		for(i=0; i<8; i++)
			dstat[i]++;
	}

	BTLZA_Encode_BuildLengthsAdjust(lstat, 288, lcl, 15);
	BTLZA_Encode_BuildLengthsAdjust(dstat, 32, dcl, 15);

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
	BTLZA_Encode_StatLengths(lcl, lc, hstat);
	BTLZA_Encode_StatLengths(dcl, dc, hstat);
//	BTLZA_Encode_BuildLengths(hstat, 24, hcl, 7);
	BTLZA_Encode_BuildLengthsAdjust(hstat, 24, hcl, 7);

	for(i=0; i<19; i++)hcl2[i]=hcl[lorder[i]];

	for(hc=4; hc<19; hc++)
	{
		for(i=hc; i<19; i++)
			if(hcl2[i])break;
		if(i==19)break;
	}

	BTLZA_Encode_WriteBit(last);
	BTLZA_Encode_Write2Bits(2);

	BTLZA_Encode_Write5Bits(lc-257);
	BTLZA_Encode_Write5Bits(dc-1);
	BTLZA_Encode_Write4Bits(hc-4);

	for(i=0; i<hc; i++)
		BTLZA_Encode_Write3Bits(hcl[lorder[i]]);

	j=PDUNZ_SetupTable(hcl, 24,
		pdunz_ltab_code, pdunz_ltab_mask, pdunz_ltab_len,
		pdunz_ltab_idx, pdunz_ltab_next);
	if(j<0)return(j);

	BTLZA_Encode_EncodeLengths(lcl, lc);
	BTLZA_Encode_EncodeLengths(dcl, dc);

	j=PDUNZ_SetupTable(lcl, lc,
		pdunz_ltab_code, pdunz_ltab_mask, pdunz_ltab_len,
		pdunz_ltab_idx, pdunz_ltab_next);
	if(j<0)return(j);

	j=PDUNZ_SetupTable(dcl, dc,
		pdunz_dtab_code, pdunz_dtab_mask, pdunz_dtab_len,
		pdunz_dtab_idx, pdunz_dtab_next);
	if(j<0)return(j);

#endif

	ll=13;
	ws=18;

	*ctx->ct++=0xA6;

	fl=((ll-12)&15)|(((ws-12)&31)<<4);
	ctx->ct=BGBBTJ_EmitVLI(ctx->ct, fl);

	BTLZA_BitArith_SetupContextBits(ctx, ll);

	BTLZA_Encode_EncodeLZBuffer(ctx, ctx->lz_tbuf, ctx->lz_mbuf, n);

	BTLZA_BitArith_FlushWBits(ctx);

	return(0);
}
#endif

int BTLZA_Encode_CheckAllocWindow(BGBBTJ_BTLZA_Context *ctx)
{
	int i, j, k;

	if(!ctx->lz_wsize)
		{ ctx->lz_wsize=262144; }
	ctx->lz_wmask=ctx->lz_wsize-1;

	if(!ctx->lz_wbuf)
		{ ctx->lz_wbuf=btlza_malloc(ctx->lz_wsize); }
	if(!ctx->lz_lbuf)
		{ ctx->lz_lbuf=btlza_malloc(ctx->lz_wsize*sizeof(int)); }
	if(!ctx->lz_hash)
//		{ ctx->lz_hash=btlza_malloc(4096*sizeof(int)); }
		{ ctx->lz_hash=btlza_malloc(16384*sizeof(int)); }
	
	memset(ctx->lz_wbuf, 0, ctx->lz_wsize);

//	for(i=0; i<4096; i++)ctx->lz_hash[i]=0;
	for(i=0; i<16384; i++)ctx->lz_hash[i]=0;
	ctx->lz_lbuf[0]=0;
	ctx->lz_wpos=ctx->lz_wsize-1;
	
	return(0);
}

#if 0
int BTLZA_Encode_EncodeStream_I(BGBBTJ_BTLZA_Context *ctx,
	byte *ibuf, byte *obuf, int isz, int osz)
{
	byte *s;
	int i, j, k;

	ctx->ibuf=ibuf;
	ctx->obuf=obuf;
//	ctx->pos=0;
//	ctx->win=0;

	ctx->ct=obuf;
	ctx->cte=obuf+osz;

	BTLZA_Encode_CheckAllocWindow(ctx);

	s=ibuf;
	i=isz;	k=0;

#ifdef BTLZA_ENC_CHUNK
	while(i>BTLZA_ENC_CHUNK)
	{
//		printf("Encode Block %d\r", k++);

#if 1
		for(j=0; j<4096; j++)
			{ ctx->lz_hash[j]=0; }
		ctx->lz_lbuf[0]=0;
		ctx->lz_wpos=ctx->lz_wsize-1;
#endif

		j=BTLZA_Encode_EncodeBlock(ctx, s, BTLZA_ENC_CHUNK);
//		j=BTLZA_Encode_EncodeBlockStatic(s, BTLZA_ENC_CHUNK);
		if(j<0)return(j);
		i-=BTLZA_ENC_CHUNK;
		s+=BTLZA_ENC_CHUNK;
	}
//	printf("\n");
#endif

	j=BTLZA_Encode_EncodeBlock(ctx, s, i);
//	j=BTLZA_Encode_EncodeBlockStatic(s, i);
	if(j<0)return(j);

	*ctx->ct++=0;

//	BTLZA_BitArith_FlushWBits(ctx);
//	BTLZA_Encode_FlushBits();
	return(ctx->ct-obuf);
}

int BTLZA_Encode_EncodeStreamLvl(byte *ibuf, byte *obuf,
	int isz, int osz, int l)
{
	static int sd[10]=
		{0, 1, 4, 16, 64, 64, 256, 1024, 4096, 16384};
	static int md[10]=
		{1, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576};
	static int ml[10]=
		{256, 256, 256, 256, 256, 1024, 4096, 16384, 65536};

	BGBBTJ_BTLZA_Context *ctx;
	int i, ws;

	ctx=BTLZA_AllocContext();

	if(md[l]<65536)
	{
		if(ctx->lz_wbuf && (ctx->lz_wsize!=65536))
		{
			btlza_free(ctx->lz_wbuf);
			btlza_free(ctx->lz_lbuf);
			ctx->lz_wbuf=NULL;
			ctx->lz_lbuf=NULL;
		}
		
		ctx->lz_sdepth=sd[l];
		ctx->lz_maxdist=md[l];
		ctx->lz_maxlen=ml[l];
		ctx->lz_wsize=65536;
	}else
	{
		if(ctx->lz_wbuf && (ctx->lz_wsize!=md[l]))
		{
			btlza_free(ctx->lz_wbuf);
			btlza_free(ctx->lz_lbuf);
			ctx->lz_wbuf=NULL;
			ctx->lz_lbuf=NULL;
		}

		ctx->lz_sdepth=sd[l];
		ctx->lz_maxdist=md[l]-(ml[l]+4);
		ctx->lz_maxlen=ml[l];
		ctx->lz_wsize=md[l];
	}

	i=BTLZA_Encode_EncodeStream_I(ctx, ibuf, obuf, isz, osz);
	return(i);
}

int BTLZA_Encode_EncodeStreamLvlTest(byte *ibuf, byte *obuf,
	int isz, int osz, int lvl)
{
	byte *tbuf;
	int i, j, k, l;

	i=BTLZA_Encode_EncodeStreamLvl(ibuf, obuf, isz, osz, lvl);

	tbuf=malloc(isz+32768);
	BTLZA_Decode_DecodeStreamSz(obuf, tbuf, i, isz+32768, &j, 0);
	if(j!=isz)
	{
		printf("BTLZA_Encode_EncodeStreamLvlTest: "
			"Size %d -> %d\n", isz, j);
		free(tbuf);
		return(-1);
	}
	
	for(k=0; k<isz; k++)
		if(tbuf[k]!=ibuf[k])
			break;
	if(k<isz)
	{
		printf("BTLZA_Encode_EncodeStreamLvlTest: "
			"Error @%d/%d\n", k, isz);
		free(tbuf);
		return(-1);
	}
	
	free(tbuf);
	return(i);
}
#endif
