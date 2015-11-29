#include <btlzazip.h>

int btlza_dbase1[64];
int btlza_dextra1[64];
int btlza_lbase1[32];
int btlza_lextra1[32];
int btlza_lbase2[32];
int btlza_lextra2[32];
int btlza_lbase3[64];
int btlza_lextra3[64];


int BTLZA_BitEnc_BalanceTree_r(
	short *nodes, short *nlen,
	int root, int h, int ml)
{
	int h0, h1, h2, h3;
	int l0, l1, l2;

	if(root<0)
	{
//		fprintf(stderr, "L");
		return(0);
	}

//	fprintf(stderr, "{");

	h1=BTLZA_BitEnc_BalanceTree_r(nodes, nlen, nodes[root*2+0], h+1, ml);
	h2=BTLZA_BitEnc_BalanceTree_r(nodes, nlen, nodes[root*2+1], h+1, ml);
	h0=((h1>h2)?h1:h2)+1;
	nlen[root]=h0;

	if((h+h0)<=ml)	//depth limit not exceeded
	{
//		fprintf(stderr, "}");
		return(h0);
	}

	//ok, so part of the tree is too deep
	if((h1+1)<h2)
	{
		l0=nodes[root*2+1];
//		if(l0<0)return(h0);	//can't rebalance leaves

		l1=nodes[l0*2+1];
		nodes[l0*2+1]=nodes[l0*2+0];
		nodes[l0*2+0]=nodes[root*2+0];
		nodes[root*2+0]=l0;
		nodes[root*2+1]=l1;
	}else if((h2+1)<h1)
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
//		fprintf(stderr, "bal}");
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

//	fprintf(stderr, "rebal}");

	return(h0);
}

void BTLZA_BitEnc_CalcLengths_r(short *nodes, byte *cl, int root, int h)
{
	if(root<0)
	{
		cl[(-root)-1]=h;
		return;
	}

	BTLZA_BitEnc_CalcLengths_r(nodes, cl, nodes[root*2+0], h+1);
	BTLZA_BitEnc_CalcLengths_r(nodes, cl, nodes[root*2+1], h+1);
}

int BTLZA_BitEnc_BuildLengths(int *stat, int nc, byte *cl, int ml)
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
		fprintf(stderr, "empty tree\n");
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

//		fprintf(stderr, "%d %d %d\n", cnts[j], cnts[k], cnts[nr]);

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
		k=BTLZA_BitEnc_BalanceTree_r(nodes, nlen, l, 0, ml);
	if(k>ml)
	{
		fprintf(stderr, "tree balance failure\n");
		fprintf(stderr, "tree depth %d, org %d, %d nodes\n", k, j, nn);
		return(-2);
	}

	BTLZA_BitEnc_CalcLengths_r(nodes, cl, l, 0);
	return(0);
}

int BTLZA_BitEnc_BuildLengthsAdjust(int *stat, int nc, byte *cl, int ml)
{
	int i, j;

	while(1)
	{
		j=BTLZA_BitEnc_BuildLengths(stat, nc, cl, ml);
		if(j<0)
			fprintf(stderr, "BTLZA_BitEnc_BuildLengthsAdjust: Huff Fail %d\n");

		for(i=0; i<nc; i++)
			if(stat[i] && !cl[i])
				break;
		if(i>=nc)break;

		fprintf(stderr, "BTLZA_BitEnc_BuildLengthsAdjust: Fiddle Adjust\n");
		for(i=0; i<nc; i++)
			stat[i]++;
		continue;
	}
	return(0);
}

void BTLZA_BitEnc_StatLZRun(BGBBTJ_BTLZA_Context *ctx,
	byte *cs, int *lstat, int *dstat)
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
		k=ctx->lbase[i]+(1<<ctx->lextra[i]);
		if((j>=ctx->lbase[i]) && (j<k))
		{
			lstat[257+i]++;
			break;
		}
	}

	if(i>=60)
	{
		fprintf(stderr, "BTLZA_BitEnc_StatLZRun: No Length %d\n", l);
	}

	j=d;
	for(i=0; i<64; i++)
	{
		k=btlza_dbase1[i]+(1<<btlza_dextra1[i]);
		if((j>=btlza_dbase1[i]) && (j<k))
		{
			dstat[i]++;
			break;
		}
	}

	if(i>=64)
	{
		fprintf(stderr, "BTLZA_BitEnc_StatLZRun: No Dist %d\n", l);
	}
}

void BTLZA_BitEnc_StatLZBuffer(BGBBTJ_BTLZA_Context *ctx,
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
			BTLZA_BitEnc_StatLZRun(ctx, s, lstat, dstat);
//			s+=3;
			if(s[0]==255)	{ s+=6; }
			else			{ s+=3; }
			continue;
		}

		lstat[*s++]++;
	}
	lstat[256]++;
}

void BTLZA_BitEnc_StatLengths(BGBBTJ_BTLZA_Context *ctx,
	byte *cl, int nc, int *stat)
{
	int i, j, l;

	l=-1;
	for(i=0; i<nc;)
	{
		for(j=0; ((i+j)<nc) && (cl[i+j]==0); j++);
		if(j>138)j=138;
		if(j>10) { stat[18]++; i+=j; l=0; continue; }
		if(j>2) { stat[17]++; i+=j; l=0; continue; }

		for(j=0; ((i+j)<nc) && (cl[i+j]==l); j++);
		if(j>6)j=6;
		if(j>2) { stat[16]++; i+=j; continue; }

		l=cl[i++];
		stat[l]++;
	}
}

void BTLZA_BitEnc_EncodeSymbol(BGBBTJ_BTLZA_Context *ctx, int v)
{
	if(!ctx->bs_ltab_len[v])
		fprintf(stderr, "BTLZA_BitEnc_EncodeSymbol: no symbol %d\n", v);

	BTLZA_BitEnc_WriteNBits(ctx, ctx->bs_ltab_code[v],
		ctx->bs_ltab_len[v]);
}

void BTLZA_BitEnc_EncodeLengths(BGBBTJ_BTLZA_Context *ctx, byte *cl, int nc)
{
	int i, j, l;

	i=0; l=-1;
	while(i<nc)
	{
		for(j=0; ((i+j)<nc) && (cl[i+j]==0); j++);
		if(j>10)
		{
			if(j>138)j=138;
			BTLZA_BitEnc_EncodeSymbol(ctx, 18);
			BTLZA_BitEnc_WriteNBits(ctx, j-11, 7);
			i+=j;
			l=0;

//			fprintf(stderr, "Z%d ", j);
			continue;
		}
		if(j>2)
		{
			BTLZA_BitEnc_EncodeSymbol(ctx, 17);
			BTLZA_BitEnc_Write3Bits(ctx, j-3);
			i+=j;
			l=0;

//			fprintf(stderr, "Z%d ", j);
			continue;
		}

		for(j=0; ((i+j)<nc) && (cl[i+j]==l); j++);
		if(j>2)
		{
			if(j>6)j=6;
			BTLZA_BitEnc_EncodeSymbol(ctx, 16);
			BTLZA_BitEnc_Write2Bits(ctx, j-3);
			i+=j;

//			fprintf(stderr, "R%d ", j);
			continue;
		}

		l=cl[i++];
		BTLZA_BitEnc_EncodeSymbol(ctx, l);

//		fprintf(stderr, "L%d ", l);

		if((l<0) || (l>15))fprintf(stderr, "problem: bad code length noted\n");
	}
//	fprintf(stderr, "\n");
}

void BTLZA_BitEnc_EncodeLZRun(BGBBTJ_BTLZA_Context *ctx, byte *cs)
{
	int l, d;
	int i, j, k;

	if(cs[0]==255)
	{
//		BTLZA_BitEnc_EncodeSymbol(ctx, 285);
//		if(pdz2_maxdist>32768)
//		{
//			k=cs[3]|(cs[4]<<8);
//			BTLZA_BitEnc_WriteNBits(k-3, 16);
//		}
		l=cs[1]|(cs[2]<<8);
		d=cs[3]|(cs[4]<<8)|(cs[5]<<16);
	}else
	{
		l=cs[0]+3;
		d=cs[1]|(cs[2]<<8);
	}

	j=l;
	for(i=0; i<64; i++)
	{
		k=ctx->lbase[i]+(1<<ctx->lextra[i]);
		if((j>=ctx->lbase[i]) && (j<k))
		{
			BTLZA_BitEnc_EncodeSymbol(ctx, 257+i);
			BTLZA_BitEnc_WriteNBits(ctx, j-ctx->lbase[i], ctx->lextra[i]);
			break;
		}
	}

	j=d;
	for(i=0; i<64; i++)
	{
		k=btlza_dbase1[i]+(1<<btlza_dextra1[i]);
		if((j>=btlza_dbase1[i]) && (j<k))
		{
			if(!ctx->bs_dtab_len[i])
				fprintf(stderr, "BTLZA_BitEnc_EncodeLZRun: "
					"no dist sym %d, %d\n", i, j);

			BTLZA_BitEnc_WriteNBits(ctx, ctx->bs_dtab_code[i],
				ctx->bs_dtab_len[i]);

			BTLZA_BitEnc_WriteNBits(ctx, j-btlza_dbase1[i], btlza_dextra1[i]);
			break;
		}
	}
}

void BTLZA_BitEnc_EncodeLZBuffer(BGBBTJ_BTLZA_Context *ctx,
	byte *tbuf, byte *mbuf, int n)
{
	byte *s;
	int i;

	s=tbuf;
	for(i=0; i<n; i++)
	{
		if(mbuf[i>>3]&(1<<(i&7)))
		{
			BTLZA_BitEnc_EncodeLZRun(ctx, s);
			if(s[0]==255)	{ s+=6; }
			else			{ s+=3; }
			continue;
		}

		BTLZA_BitEnc_EncodeSymbol(ctx, *s++);
	}
	BTLZA_BitEnc_EncodeSymbol(ctx, 256);
}

int BTLZA_BitEnc_EncodeBlockStatic(BGBBTJ_BTLZA_Context *ctx,
	byte *ibuf, int isz, int last)
{
	int n;

	n=BTLZA_Encode_LZCompressBuffer(ctx,
		ibuf, ctx->lz_tbuf, ctx->lz_mbuf, isz);
	BTLZA_BitEnc_WriteBit(ctx, last);
	BTLZA_BitEnc_Write2Bits(ctx, 1);
	BTLZA_BitDec_SetupStatic(ctx);
	BTLZA_BitEnc_EncodeLZBuffer(ctx, ctx->lz_tbuf, ctx->lz_mbuf, n);

	return(0);
}

int BTLZA_BitEnc_EncodeBlock(BGBBTJ_BTLZA_Context *ctx,
	byte *ibuf, int isz, int last)
{
	int i;

	i=isz;
	if(!ctx->lz_tbuf || (isz>ctx->lz_tsz))
	{
		if(ctx->lz_tbuf)btlza_free(ctx->lz_tbuf);
		if(ctx->lz_mbuf)btlza_free(ctx->lz_mbuf);
		ctx->lz_tsz=i; ctx->lz_msz=(i+7)/8;
		ctx->lz_tbuf=btlza_malloc(ctx->lz_tsz);
		ctx->lz_mbuf=btlza_malloc(ctx->lz_msz);
	}

	if(ctx->lz_maxdist>65536)
	{
		ctx->lbase=btlza_lbase3; ctx->lextra=btlza_lextra3;
		return(BTLZA_BitEnc_EncodeBlockBTLZH(ctx, ibuf, isz, last));
	}else
	{
		if(ctx->lz_maxdist>32768)
			{ ctx->lbase=btlza_lbase2; ctx->lextra=btlza_lextra2; }
		else
			{ ctx->lbase=btlza_lbase1; ctx->lextra=btlza_lextra1; }
		return(BTLZA_BitEnc_EncodeBlockDynamic(ctx, ibuf, isz, last));
	}
}

int BTLZA_BitEnc_EncodeBlockDynamic(BGBBTJ_BTLZA_Context *ctx,
	byte *ibuf, int isz, int last)
{
	static int lorder[]={
		16, 17, 18, 0, 8,7, 9,6, 10,5, 11,4, 12,3, 13,2, 14,1, 15};

//	static byte *tbuf=NULL, *mbuf=NULL;
//	static int tsz, msz;

	int lstat[320], dstat[64], hstat[24];
	byte lcl[320], dcl[64], hcl[24], hcl2[24];
	int lc, dc, hc;
	int i, j, n;

	n=BTLZA_Encode_LZCompressBuffer(ctx,
		ibuf, ctx->lz_tbuf, ctx->lz_mbuf, isz);
	BTLZA_BitEnc_StatLZBuffer(ctx,
		ctx->lz_tbuf, ctx->lz_mbuf, n, lstat, dstat);

	if(ctx->lz_sdepth<2)
	{
		for(i=0; i<256; i++)
			lstat[i]++;
		for(i=0; i<8; i++)
			dstat[i]++;
	}

//	BTLZA_BitEnc_BuildLengths(lstat, 288, lcl, 15);
//	BTLZA_BitEnc_BuildLengths(dstat, 32, dcl, 15);
	BTLZA_BitEnc_BuildLengthsAdjust(lstat, 288, lcl, 15);
	BTLZA_BitEnc_BuildLengthsAdjust(dstat, 32, dcl, 15);

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
//	BTLZA_BitEnc_BuildLengths(hstat, 24, hcl, 7);
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

	BTLZA_BitEnc_EncodeLZBuffer(ctx, ctx->lz_tbuf, ctx->lz_mbuf, n);
	return(0);
}

int BTLZA_BitEnc_EncodeBlockBTLZH(BGBBTJ_BTLZA_Context *ctx,
	byte *ibuf, int isz, int last)
{
	static int lorder[]={
		16, 17, 18, 0, 8,7, 9,6, 10,5, 11,4, 12,3, 13,2, 14,1, 15};

	int lstat[512], dstat[128], hstat[24];
	byte lcl[512], dcl[128], hcl[24], hcl2[24];
	int lc, dc, hc;
	int i, j, n;

	n=BTLZA_Encode_LZCompressBuffer(ctx,
		ibuf, ctx->lz_tbuf, ctx->lz_mbuf, isz);
	BTLZA_BitEnc_StatLZBuffer(ctx,
		ctx->lz_tbuf, ctx->lz_mbuf, n, lstat, dstat);

	if(ctx->lz_sdepth<2)
	{
		for(i=0; i<256; i++)
			lstat[i]++;
		for(i=0; i<8; i++)
			dstat[i]++;
	}

//	for(i=0; i<64; i++)
//		dstat[i]++;

//	BTLZA_BitEnc_BuildLengths(lstat, 288, lcl, 15);
//	BTLZA_BitEnc_BuildLengths(dstat, 32, dcl, 15);
	BTLZA_BitEnc_BuildLengthsAdjust(lstat, 320, lcl, 15);
	BTLZA_BitEnc_BuildLengthsAdjust(dstat, 64, dcl, 15);

	for(i=0; i<64; i++)if(dcl[i])break;
	if(i==64)for(i=0; i<64; i++)dcl[i]=6;

	for(lc=257; lc<320; lc++)
	{
		for(i=lc; i<320; i++)
			if(lcl[i])break;
		if(i==320)break;
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
//	BTLZA_BitEnc_BuildLengths(hstat, 24, hcl, 7);
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

	BTLZA_BitEnc_EncodeLZBuffer(ctx, ctx->lz_tbuf, ctx->lz_mbuf, n);

	return(0);
}

int BTLZA_BitEnc_EncodeBeginArithmetic(BGBBTJ_BTLZA_Context *ctx)
{
	BTLZA_BitEnc_WriteBit(ctx, 0);
	BTLZA_BitEnc_Write2Bits(ctx, 3);
	BTLZA_BitEnc_Write3Bits(ctx, 1);

	BTLZA_BitEnc_Write3Bits(ctx, 1);
	BTLZA_BitEnc_Write4Bits(ctx, 13-12);
	BTLZA_BitEnc_Write5Bits(ctx, 18);

	BTLZA_BitEnc_FlushBits(ctx);

	BTLZA_BitArith_SetupRawContextBits(ctx, 13);
	BTLZA_BitArith_SetupBeginEncode(ctx);
	ctx->BS_WriteByte=BTLZA_BitDec_WriteByteArithLE;
	
	return(0);
}

int BTLZA_BitEnc_EncodeStream_I(BGBBTJ_BTLZA_Context *ctx,
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

	BTLZA_Encode_CheckAllocWindow(ctx);

	if(ctx->bs_flags&BGBBTJ_ZFL_ARITH)
	{
		BTLZA_BitEnc_EncodeBeginArithmetic(ctx);
	}

	s=ibuf;
	i=isz;	k=0;

#ifdef BTLZA_ENC_CHUNK
	while(i>BTLZA_ENC_CHUNK)
	{
//		fprintf(stderr, "Encode Block %d\r", k++);

#if 0
		for(j=0; j<4096; j++)pdz2_hash[j]=0;
		pdz2_lbuf[0]=0;
		pdz2_wpos=0xFFFF;
#endif

		j=BTLZA_BitEnc_EncodeBlock(ctx, s, BTLZA_ENC_CHUNK, 0);
//		j=BTLZA_BitEnc_EncodeBlockStatic(ctx, s, BTLZA_BitEnc_CHUNK, 0);
		if(j<0)return(j);
		i-=BTLZA_ENC_CHUNK;
		s+=BTLZA_ENC_CHUNK;
	}
//	fprintf(stderr, "\n");
#endif

	j=BTLZA_BitEnc_EncodeBlock(ctx, s, i, 1);
//	j=BTLZA_BitEnc_EncodeBlockStatic(ctx, s, i, 1);
	if(j<0)return(j);

	BTLZA_BitEnc_FlushBits(ctx);
	return(ctx->ct-obuf);
}

BGBBTJ_API int BTLZA_BitEnc_EncodeStream32Lvl(
	byte *ibuf, byte *obuf, int isz, int osz, int l)
{
	static int sd[10]=
		{1, 16, 16, 64, 64, 256, 256, 1024, 4096, 16384};
	static int md[10]=
		{1, 4096, 8192, 4096, 8192, 8192, 16384, 16384, 32768, 32768};

	BGBBTJ_BTLZA_Context *ctx;
	int i;

	ctx=BTLZA_AllocContext();
	ctx->lz_sdepth=sd[l&15];
	ctx->lz_maxdist=md[l&15];
	ctx->lz_maxlen=258;
	ctx->lz_wsize=65536;
	ctx->lz_wmask=65535;
	ctx->bs_flags=l>>8;

	i=BTLZA_BitEnc_EncodeStream_I(ctx, ibuf, obuf, isz, osz);
	BTLZA_FreeContext(ctx);
	return(i);
}

BGBBTJ_API int BTLZA_BitEnc_EncodeStream64Lvl(
	byte *ibuf, byte *obuf,
	int isz, int osz, int lvl)
{
	static int sd[10]=
		{0, 1, 4, 16, 64, 256, 256, 1024, 4096, 16384};
	BGBBTJ_BTLZA_Context *ctx;
	int i;

	ctx=BTLZA_AllocContext();
	ctx->lz_sdepth=sd[lvl&15];
	ctx->lz_maxdist=65536-260;
	ctx->lz_maxlen=258;
	ctx->lz_wsize=65536;
	ctx->lz_wmask=65535;
	ctx->bs_flags=lvl>>8;

	i=BTLZA_BitEnc_EncodeStream_I(ctx, ibuf, obuf, isz, osz);
	BTLZA_FreeContext(ctx);
	return(i);
}

BGBBTJ_API int BTLZA_BitEnc_EncodeStreamXLvl(
	byte *ibuf, byte *obuf,
	int isz, int osz, int lvl)
{
	static int sd[10]={
		  0,   1,    4,   16,    64,
		256, 256, 1024, 4096, 16384};
	static int wsz[10]={
		1<<12, 1<<14, 1<<15, 1<<15, 1<<17,
		1<<18, 1<<19, 1<<19, 1<<20, 1<<20};
	static int msz[10]={
		256, 256, 256, 256, 4096,
		16384, 32768, 32768, 65536, 65536};
	BGBBTJ_BTLZA_Context *ctx;
	int i;

	if((lvl>>8)&BGBBTJ_ZFL_FASTENC)
	{
		i=BTLZA_BitEncF_EncodeStreamXLvl(
			ibuf, obuf, isz, osz, lvl);
		return(i);
	}

	ctx=BTLZA_AllocContext();
	ctx->lz_sdepth=sd[lvl&15];
//	ctx->lz_maxdist=262144-16384;
//	ctx->lz_maxlen=16384;
//	ctx->lz_wsize=262144;
//	ctx->lz_wmask=262144;
	ctx->bs_flags=lvl>>8;

//	ctx->lz_maxdist=(1<<20)-65536;
//	ctx->lz_maxlen=65536;
//	ctx->lz_wsize=(1<<20);
//	ctx->lz_wmask=(1<<20);

	ctx->lz_maxlen=msz[lvl&15];
	ctx->lz_wsize=wsz[lvl&15];
	ctx->lz_wmask=wsz[lvl&15];
	ctx->lz_maxdist=ctx->lz_wsize-ctx->lz_maxlen-4;

	i=BTLZA_BitEnc_EncodeStream_I(ctx, ibuf, obuf, isz, osz);
	BTLZA_FreeContext(ctx);
	return(i);
}

BGBBTJ_API int BTLZA_BitEnc_EncodeStream32LvlZl(
	byte *ibuf, byte *obuf,
	int isz, int osz, int lvl)
{
	int i, j, k, l;

	j=BTLZA_BitEnc_EncodeStream32Lvl(ibuf, obuf+2, isz, osz-6, lvl);

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

BGBBTJ_API int BTLZA_BitEnc_EncodeStream64LvlZl(
	byte *ibuf, byte *obuf,
	int isz, int osz, int lvl)
{
	int i, j, k, l;

	j=BTLZA_BitEnc_EncodeStream64Lvl(ibuf, obuf+2, isz, osz-6, lvl);

	k=(8<<12)|(9<<8)|(3<<6); l=k%31;
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

BGBBTJ_API int BTLZA_BitEnc_EncodeStreamXLvlZl(
	byte *ibuf, byte *obuf,
	int isz, int osz, int lvl)
{
	int i, j, k, l;

	j=BTLZA_BitEnc_EncodeStreamXLvl(ibuf, obuf+2, isz, osz-6, lvl);

	k=(12<<12)|(10<<8)|(3<<6); l=k%31;
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

BGBBTJ_API int BTLZA_BitEnc_EncodeStreamXLvlZlc(
	byte *ibuf, byte *obuf,
	int isz, int osz, int lvl)
{
	int i, j, k, l;

	j=BTLZA_BitEnc_EncodeStreamXLvl(ibuf, obuf+1, isz, osz-1, lvl);
	obuf[0]=0x8A;
	
	k=(obuf[0]<<8)+obuf[1];
	if(!(k%31))
		obuf[0]=0xBA;
	
//	obuf[0]=0xFA;
	return(j+1);
}

BGBBTJ_API int BTLZA_BitEnc_EncodeStreamXLvlZlTest(
	byte *ibuf, byte *obuf,
	int isz, int osz, int lvl)
{
	byte *tbuf;
	int i, j, k, l;

	i=BTLZA_BitEnc_EncodeStreamXLvlZl(ibuf, obuf, isz, osz, lvl);

	tbuf=malloc(isz+32768);
	BTLZA_DecodeStreamSzZl(obuf, tbuf, i, isz+32768, &j, 0);
	if(j!=isz)
	{
		fprintf(stderr, "BTLZA_BitEnc_EncodeStream64LvlZlTest: "
			"Size %d -> %d\n", isz, j);
		free(tbuf);
		return(-1);
	}
	
	for(k=0; k<isz; k++)
		if(tbuf[k]!=ibuf[k])
			break;
	if(k<isz)
	{
		fprintf(stderr, "BTLZA_BitEnc_EncodeStream64LvlZlTest: "
			"Error @%d/%d\n", k, isz);
		free(tbuf);
		return(-1);
	}
	
	free(tbuf);
	return(i);
}
