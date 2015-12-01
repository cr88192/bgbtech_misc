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

	for(i=0; i<384; i++)lstat[i]=0;
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

void BTLZA_BitEnc_StatLengthsRh(BGBBTJ_BTLZA_Context *ctx,
	byte *cl, byte *lcl, int nc, int *stat)
{
	int i, j, l;

	l=-1;
	for(i=0; i<nc;)
	{
//		for(j=0; ((i+j)<nc) && (cl[i+j]==0); j++);
		for(j=0; ((i+j)<nc) && (cl[i+j]==lcl[i+j]); j++);
		if(j>138)j=138;
		if(j>10) { stat[18]++; i+=j; l=lcl[i-1]; continue; }
		if(j>2) { stat[17]++; i+=j; l=lcl[i-1]; continue; }

		for(j=0; ((i+j)<nc) && (cl[i+j]==l); j++);
		if(j>6)j=6;
		if(j>2) { stat[16]++; i+=j; continue; }

#if 1
		if((cl[i]==(lcl[i]+1)) || (cl[i]==(lcl[i]-1)))
		{
			stat[19]++;
			l=cl[i++];
			continue;
		}
#endif

		l=cl[i++];
		stat[l]++;
	}
}

void BTLZA_BitEnc_EncodeSymbol(BGBBTJ_BTLZA_Context *ctx, int v)
{
	ctx->BS_EncodeSymbol(ctx, v);
}

void BTLZA_BitEnc_EncodeDistanceSymbol(BGBBTJ_BTLZA_Context *ctx, int v)
{
	ctx->BS_EncodeDistanceSymbol(ctx, v);
}

void BTLZA_BitEnc_EncodeClSymbol(BGBBTJ_BTLZA_Context *ctx, int v)
{
	ctx->BS_EncodeClSymbol(ctx, v);
}

void BTLZA_BitEnc_EncodeSymbolBasic(BGBBTJ_BTLZA_Context *ctx, int v)
{
	if(!ctx->bs_ltab_len[v])
		fprintf(stderr, "BTLZA_BitEnc_EncodeSymbolBasic: No Symbol %d\n", v);

	BTLZA_BitEnc_WriteNBits(ctx, ctx->bs_ltab_code[v],
		ctx->bs_ltab_len[v]);
}

void BTLZA_BitEnc_EncodeDistanceSymbolBasic(
	BGBBTJ_BTLZA_Context *ctx, int v)
{
	if(!ctx->bs_dtab_len[v])
		fprintf(stderr, "BTLZA_BitEnc_EncodeDistanceSymbol: "
			"No symbol %d\n", v);

	BTLZA_BitEnc_WriteNBits(ctx, ctx->bs_dtab_code[v],
		ctx->bs_dtab_len[v]);
}

void BTLZA_BitEnc_EncodeSymbolRingHuff(
	BGBBTJ_BTLZA_Context *ctx, int v)
{
	int rh;

	rh=(ctx->bs_rhtab_lrov++)&7;

	if(!ctx->bs_ltab_len2[rh][v])
		fprintf(stderr, "BTLZA_BitEnc_EncodeSymbolRingHuff: "
			"No symbol %d\n", v);

	BTLZA_BitEnc_WriteNBits(ctx, ctx->bs_ltab_code2[rh][v],
		ctx->bs_ltab_len2[rh][v]);
}

void BTLZA_BitEnc_EncodeDistanceSymbolRingHuff(
	BGBBTJ_BTLZA_Context *ctx, int v)
{
	int rh;

	rh=(ctx->bs_rhtab_drov++)&7;

	if(!ctx->bs_dtab_len2[rh][v])
		fprintf(stderr, "BTLZA_BitEnc_EncodeDistanceSymbolRingHuff: "
			"No symbol %d\n", v);

	BTLZA_BitEnc_WriteNBits(ctx, ctx->bs_dtab_code2[rh][v],
		ctx->bs_dtab_len2[rh][v]);
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
			BTLZA_BitEnc_EncodeClSymbol(ctx, 18);
			BTLZA_BitEnc_WriteNBits(ctx, j-11, 7);
			i+=j;
			l=0;

//			fprintf(stderr, "Z%d ", j);
			continue;
		}
		if(j>2)
		{
			BTLZA_BitEnc_EncodeClSymbol(ctx, 17);
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
			BTLZA_BitEnc_EncodeClSymbol(ctx, 16);
			BTLZA_BitEnc_Write2Bits(ctx, j-3);
			i+=j;

//			fprintf(stderr, "R%d ", j);
			continue;
		}

		l=cl[i++];
		BTLZA_BitEnc_EncodeClSymbol(ctx, l);

//		fprintf(stderr, "L%d ", l);

		if((l<0) || (l>15))fprintf(stderr,
			"problem: bad code length noted\n");
	}
//	fprintf(stderr, "\n");
}

void BTLZA_BitEnc_EncodeLengthsRh(
	BGBBTJ_BTLZA_Context *ctx, byte *cl, byte *lcl, int nc)
{
	int i, j, l;

	i=0; l=-1;
	while(i<nc)
	{
//		for(j=0; ((i+j)<nc) && (cl[i+j]==0); j++);
		for(j=0; ((i+j)<nc) && (cl[i+j]==lcl[i+j]); j++);
		if(j>10)
		{
			if(j>138)j=138;
			BTLZA_BitEnc_EncodeClSymbol(ctx, 18);
			BTLZA_BitEnc_WriteNBits(ctx, j-11, 7);
			i+=j;
			l=cl[i-1];

//			fprintf(stderr, "Z%d ", j);
			continue;
		}
		if(j>2)
		{
			BTLZA_BitEnc_EncodeClSymbol(ctx, 17);
			BTLZA_BitEnc_Write3Bits(ctx, j-3);
			i+=j;
			l=cl[i-1];

//			fprintf(stderr, "Z%d ", j);
			continue;
		}

		for(j=0; ((i+j)<nc) && (cl[i+j]==l); j++);
		if(j>2)
		{
			if(j>6)j=6;
			BTLZA_BitEnc_EncodeClSymbol(ctx, 16);
			BTLZA_BitEnc_Write2Bits(ctx, j-3);
			i+=j;

//			fprintf(stderr, "R%d ", j);
			continue;
		}

#if 1
		if((cl[i]==(lcl[i]+1)) ||
			(cl[i]==(lcl[i]-1)))
		{
			j=cl[i]-lcl[i];
			j=(j+1)>>1;
			BTLZA_BitEnc_EncodeClSymbol(ctx, 19);
			BTLZA_BitEnc_WriteBit(ctx, j);

			l=cl[i++];
			continue;
		}
#endif

		l=cl[i++];
		BTLZA_BitEnc_EncodeClSymbol(ctx, l);

//		fprintf(stderr, "L%d ", l);

		if((l<0) || (l>15))fprintf(stderr,
			"problem: bad code length noted\n");
	}
//	fprintf(stderr, "\n");
}

void BTLZA_BitEnc_EncodeLZRun(BGBBTJ_BTLZA_Context *ctx, byte *cs)
{
	int rh;
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
//			if(!ctx->bs_dtab_len[i])
//				fprintf(stderr, "BTLZA_BitEnc_EncodeLZRun: "
//					"no dist sym %d, %d\n", i, j);

//			BTLZA_BitEnc_WriteNBits(ctx, ctx->bs_dtab_code[i],
//				ctx->bs_dtab_len[i]);

//			rh=(ctx->bs_rhtab_drov++)&7;
//			BTLZA_BitEnc_WriteNBits(ctx, ctx->bs_dtab_code2[rh][i],
//				ctx->bs_dtab_len2[rh][i]);

			BTLZA_BitEnc_EncodeDistanceSymbol(ctx, i);

			BTLZA_BitEnc_WriteNBits(ctx,
				j-btlza_dbase1[i], btlza_dextra1[i]);
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

	ctx->BS_EncodeSymbol=
		BTLZA_BitEnc_EncodeSymbolBasic;
	ctx->BS_EncodeDistanceSymbol=
		BTLZA_BitEnc_EncodeDistanceSymbolBasic;
	ctx->BS_EncodeClSymbol=
		BTLZA_BitEnc_EncodeSymbolBasic;

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

	int lstat[384], dstat[64], hstat[24];
	byte lcl[384], dcl[64], hcl[24], hcl2[24];
	int lc, dc, hc;
	int i, j, n;

	ctx->BS_EncodeSymbol=
		BTLZA_BitEnc_EncodeSymbolBasic;
	ctx->BS_EncodeDistanceSymbol=
		BTLZA_BitEnc_EncodeDistanceSymbolBasic;
	ctx->BS_EncodeClSymbol=
		BTLZA_BitEnc_EncodeSymbolBasic;

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

int bgbrasw_log2f8(int val)
{
	int i, j, k;
	
	i=val<<8; j=0;
	while(i>=512)
		{ i=i>>1; j++; }
	k=(j<<8)|(i&255);
	return(k);
}

int bgbrasw_exp2f8(int val)
{
	int i, j, k;
	
	i=256|(val&255); j=(val>>8);
	while(j--)
		i=i<<1;
	k=(i>>8);
	return(k);
}

int bgbrasw_log2f12(int val)
{
	int i, j, k;
	
	i=val<<12; j=0;
	while(i>=8192)
		{ i=i>>1; j++; }
	k=(j<<12)|(i&4095);
	return(k);
}

int bgbrasw_exp2f12(int val)
{
	int i, j, k;
	
	i=4096|(val&4095); j=(val>>12);
	while(j--)
		i=i<<1;
	k=(i>>12);
	return(k);
}

int BTLZA_BitEnc_EncodeBlockBTLZH(BGBBTJ_BTLZA_Context *ctx,
	byte *ibuf, int isz, int last)
{
	static int lorder[]={
		16, 17, 18, 0, 8,7, 9,6, 10,5, 11,4, 12,3, 13,2, 14,1, 15, 19,
		20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};

	int lstat[512], dstat[128], hstat[32];
	int lstat2[512], dstat2[128], hstat2[32];
	int lstat3[384], dstat3[64];
//	byte lcl[512], dcl[128], hcl[24], hcl2[24];
	byte lcl[384*8], dcl[64*8], hcl[32], hcl2[32];
	int tlcl[384], tlcl2[384], tdcl[128];
	int hnrh, tcnt;
	int lc, dc, hc;
	int i, j, k, n;

	ctx->BS_EncodeSymbol=
		BTLZA_BitEnc_EncodeSymbolBasic;
	ctx->BS_EncodeDistanceSymbol=
		BTLZA_BitEnc_EncodeDistanceSymbolBasic;
	ctx->BS_EncodeClSymbol=
		BTLZA_BitEnc_EncodeSymbolBasic;

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

#if 1
	for(i=0; i<384; i++)
		{ ctx->lzf_lstat[i]=lstat[i]; }
	for(i=0; i<64; i++)
		{ ctx->lzf_dstat[i]=dstat[i]; }
#endif

	hnrh=0;
	if(ctx->bs_flags&BGBBTJ_ZFL_RINGHUFF)
	{
//		hnrh=4;
		hnrh=2;
		ctx->bs_rhtab_lrov=0;
		ctx->bs_rhtab_drov=0;
	}

//	for(i=0; i<64; i++)
//		dstat[i]++;

//	BTLZA_BitEnc_BuildLengths(lstat, 288, lcl, 15);
//	BTLZA_BitEnc_BuildLengths(dstat, 32, dcl, 15);
//	BTLZA_BitEnc_BuildLengthsAdjust(lstat, 320, lcl, 15);
//	BTLZA_BitEnc_BuildLengthsAdjust(dstat, 64, dcl, 15);

//	for(i=0; i<384; i++)
//		{ lstat2[i]=lstat[i]; }

	if(hnrh>1)
	{
		ctx->BS_EncodeSymbol=
			BTLZA_BitEnc_EncodeSymbolRingHuff;
		ctx->BS_EncodeDistanceSymbol=
			BTLZA_BitEnc_EncodeDistanceSymbolRingHuff;

#if 1
		tcnt=0;
		for(i=0; i<384; i++)
			tcnt+=lstat[i];

		k=tcnt;
		for(i=0; i<384; i++)
		{
			lstat3[i]=lstat[i]*(65536.0/(tcnt+1));
//			if(lstat[i] && (lstat3[i]<256))
//				lstat3[i]=256;
			if(lstat[i] && !lstat3[i])
				lstat3[i]=1;
//			if(lstat[i] && (lstat3[i]<16))
//				lstat3[i]=16;
//			tlcl[i]=(16<<8)-bgbrasw_log2f8(lstat3[i]);
//			tlcl[i]=bgbrasw_log2f8(lstat3[i]);
			tlcl[i]=bgbrasw_log2f12(lstat3[i]);
//			tlcl2[i]=0;
//			tlcl2[i]=tlcl[i]&255;

			k=k*65521;
			tlcl2[i]=(k>>16)&255;
		}
#endif

		for(i=0; i<384; i++)
			{ lstat2[i]=lstat[i]; }
		for(i=0; i<64; i++)
			{ dstat2[i]=dstat[i]; }

		for(i=0; i<hnrh; i++)
		{
#if 1
			for(j=0; j<384; j++)
			{
//				k=tlcl[j]&255;
//				k=tlcl[j]&4095;
//				if((k<64) || (k>=192))
//				if((k<96) || (k>=160))
//				if((k<112) || (k>=144))
//				if(1)
//				if((k<96*16) || (k>=160*16))
				if((k<85*16) || (k>=171*16))
				{
					lstat2[j]=lstat3[j];
					continue;
				}
			
				tlcl2[j]+=tlcl[j]&255;
//				k=(tlcl[i]>>8)+(tlcl2[i]>>8);
//				k=tlcl[i]+tlcl2[i];
//				k=(tlcl[j]&(~255))+tlcl2[j];
//				tlcl2[j]=tlcl2[j]&255;
//				k=1<<k;
//				k=k+(((i^j)&1)?32:(-32));
//				k=k+(((i^j)&1)?16:(-16));
				k=tlcl[j];
//				k=k+(((i^j)&1)?512:(-512));
//				k=k+(((i^j)&1)?1024:(-1024));
				k=k+(((i^j)&1)?2048:(-2048));
//				k=bgbrasw_exp2f8(k);
				k=bgbrasw_exp2f12(k);
				
				lstat2[j]=(lstat3[j]+k)>>1;
//				lstat2[j]=(3*lstat3[j]+k)>>2;
//				lstat2[j]=lstat3[j];

//				if(lstat[j] && !lstat2[j])
//					lstat2[j]=1;
			}

//			for(i=0; i<64; i++)
//				{ dstat2[i]=dstat[i]; }
#endif

			j=BTLZA_BitEnc_BuildLengthsAdjust(
				lstat2, 384, lcl+i*384, 15);
			j=BTLZA_BitEnc_BuildLengthsAdjust(
				dstat2, 64, dcl+i*64, 15);

#if 1
//			for(j=0; j<320; j++)
//				{ lstat2[i]+=lstat2[i]>>8; }
			for(j=0; j<64; j++)
				{ dstat2[i]+=dstat2[i]>>8; }
#endif
		}
	}else
	{
		BTLZA_BitEnc_BuildLengthsAdjust(lstat, 384, lcl, 15);
		BTLZA_BitEnc_BuildLengthsAdjust(dstat, 64, dcl, 15);
	}

	for(i=0; i<64; i++)if(dcl[i])break;
	if(i==64)for(i=0; i<64; i++)dcl[i]=6;

//	for(lc=257; lc<320; lc++)
	for(lc=257; lc<384; lc++)
	{
//		for(i=lc; i<320; i++)
		for(i=lc; i<384; i++)
			if(lcl[i])break;
		if(i==384)break;
	}

	for(dc=1; dc<64; dc++)
	{
		for(i=dc; i<64; i++)
			if(dcl[i])break;
		if(i==64)break;
	}

	for(i=0; i<24; i++)hstat[i]=0;

	if(hnrh>1)
	{
		BTLZA_BitEnc_StatLengths(ctx, lcl, lc, hstat);
		BTLZA_BitEnc_StatLengths(ctx, dcl, dc, hstat);

		for(i=1; i<hnrh; i++)
		{
			BTLZA_BitEnc_StatLengthsRh(ctx,
				lcl+i*384, lcl+(i-1)*384, lc, hstat);
			BTLZA_BitEnc_StatLengthsRh(ctx,
				dcl+i*64, dcl+(i-1)*64, dc, hstat);
		}
	}else
	{
		BTLZA_BitEnc_StatLengths(ctx, lcl, lc, hstat);
		BTLZA_BitEnc_StatLengths(ctx, dcl, dc, hstat);
	}

//	BTLZA_BitEnc_StatLengths(ctx, lcl, lc, hstat);
//	BTLZA_BitEnc_StatLengths(ctx, dcl, dc, hstat);
//	BTLZA_BitEnc_BuildLengths(hstat, 24, hcl, 7);
	i=BTLZA_BitEnc_BuildLengthsAdjust(hstat, 24, hcl, 7);
	if(i<0)
	{
		printf("BTLZA_BitEnc_EncodeBlockBTLZH: Build Length Error %d\n", i);
		return(i);
	}

	for(i=0; i<24; i++)hcl2[i]=hcl[lorder[i]];

	for(hc=4; hc<24; hc++)
	{
		for(i=hc; i<24; i++)
			if(hcl2[i])break;
		if(i==24)break;
	}

	BTLZA_BitEnc_WriteBit(ctx, last);
	BTLZA_BitEnc_Write2Bits(ctx, 3);
	BTLZA_BitEnc_Write3Bits(ctx, 2);

	switch(hnrh)
	{
	case 0: case 1:	i=0; break;
	case 2:				i=1; break;
	case 4:				i=2; break;
	case 8:				i=3; break;
	default:			i=0; break;
	}
	BTLZA_BitEnc_Write3Bits(ctx, i);
	
//	BTLZA_BitEnc_Write3Bits(ctx, 0);
//	BTLZA_BitEnc_Write3Bits(ctx, hnrh);

	BTLZA_BitEnc_Write7Bits(ctx, lc-257);
	BTLZA_BitEnc_Write6Bits(ctx, dc-1);
	BTLZA_BitEnc_Write5Bits(ctx, hc-4);

	for(i=0; i<hc; i++)
		BTLZA_BitEnc_Write3Bits(ctx, hcl[lorder[i]]);

	j=BTLZA_BitDec_SetupTable(hcl, 24,
		ctx->bs_ltab_code, ctx->bs_ltab_mask, ctx->bs_ltab_len,
		ctx->bs_ltab_idx, ctx->bs_ltab_next);
	if(j<0)return(j);

//	BTLZA_BitEnc_EncodeLengths(ctx, lcl, lc);
//	BTLZA_BitEnc_EncodeLengths(ctx, dcl, dc);

	if(hnrh>1)
	{
		BTLZA_BitEnc_EncodeLengths(ctx, lcl, lc);
		BTLZA_BitEnc_EncodeLengths(ctx, dcl, dc);

		for(i=1; i<hnrh; i++)
		{
			BTLZA_BitEnc_EncodeLengthsRh(ctx,
				lcl+i*384, lcl+(i-1)*384, lc);
			BTLZA_BitEnc_EncodeLengthsRh(ctx,
				dcl+i*64, dcl+(i-1)*64, dc);
		}

		BTLZA_BitDec_CheckSetupRingHuffTables(ctx, hnrh);

		for(j=0; j<hnrh; j++)
		{
			i=BTLZA_BitDec_SetupTable(lcl+(j*384), lc,
				ctx->bs_ltab_code2[j], ctx->bs_ltab_mask2[j],
				ctx->bs_ltab_len2[j],
				ctx->bs_ltab_idx2[j], ctx->bs_ltab_next2[j]);
			if(i<0)
				return(i);

			i=BTLZA_BitDec_SetupTable(dcl+(j*64), dc,
				ctx->bs_dtab_code2[j], ctx->bs_dtab_mask2[j],
				ctx->bs_dtab_len2[j],
				ctx->bs_dtab_idx2[j], ctx->bs_dtab_next2[j]);
			if(i<0)
				return(i);
		}
	}else
	{
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
	}


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
	int tcnt;
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

	if(ctx->bs_flags&BGBBTJ_ZFL_PRELOAD)
	{
		k=ctx->lz_wsize;
		BTLZA_Encode_UpdateWindowString(ctx, ibuf-k, k);
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

#if 0
	tcnt=0;
	for(i=0; i<384; i++)
		tcnt+=ctx->lzf_lstat[i];

	for(i=0; i<384; i++)
	{
		if(!ctx->lzf_lstat[i])
			continue;
	
		j=ctx->lzf_lstat[i]*(65536.0/(tcnt+1));
		if(ctx->lzf_lstat[i] && !j)
			j=1;
		k=4096-bgbrasw_log2f8(j);
		if(k>(4*256))
			continue;
		printf("%d:%.2f ", i, k/256.0);
	}
	printf("  \n");
#endif

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

static int btlza_xlvl_sd[10]={
	  0,   1,    4,   16,    64,
	256, 256, 1024, 4096, 16384};
static int btlza_xlvl_msz[10]={
	256, 256, 256, 256, 4096,
	16384, 32768, 32768, 65536, 65536};
static int btlza_xlvl_wsz[10]={
	1<<12, 1<<14, 1<<15, 1<<15, 1<<17,
	1<<18, 1<<19, 1<<19, 1<<20, 1<<20};

BGBBTJ_API int BTLZA_BitEnc_EncodeXLvlWinSize(int lvl)
{
	return(btlza_xlvl_wsz[lvl&15]);
}

BGBBTJ_API int BTLZA_BitEnc_EncodeStreamXLvl(
	byte *ibuf, byte *obuf,
	int isz, int osz, int lvl)
{
	BGBBTJ_BTLZA_Context *ctx;
	int i;

	if((lvl>>8)&BGBBTJ_ZFL_FASTENC)
	{
		i=BTLZA_BitEncF_EncodeStreamXLvl(
			ibuf, obuf, isz, osz, lvl);
		return(i);
	}

	ctx=BTLZA_AllocContext();
	ctx->lz_sdepth=btlza_xlvl_sd[lvl&15];
//	ctx->lz_maxdist=262144-16384;
//	ctx->lz_maxlen=16384;
//	ctx->lz_wsize=262144;
//	ctx->lz_wmask=262144;
	ctx->bs_flags=lvl>>8;

//	ctx->lz_maxdist=(1<<20)-65536;
//	ctx->lz_maxlen=65536;
//	ctx->lz_wsize=(1<<20);
//	ctx->lz_wmask=(1<<20);

	ctx->lz_maxlen=btlza_xlvl_msz[lvl&15];
	ctx->lz_wsize=btlza_xlvl_wsz[lvl&15];
	ctx->lz_wmask=btlza_xlvl_wsz[lvl&15]-1;
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
	int wsz;
	int i, j, k, l;

	i=BTLZA_BitEnc_EncodeStreamXLvlZl(ibuf, obuf, isz, osz, lvl);

	wsz=BTLZA_BitEnc_EncodeXLvlWinSize(lvl);

	tbuf=malloc(wsz+isz+32768);
	memcpy(tbuf, ibuf-wsz, wsz);
	BTLZA_DecodeStreamSzZl(obuf, tbuf+wsz, i, isz+32768, &j, 0);
	if(j!=isz)
	{
		fprintf(stderr, "BTLZA_BitEnc_EncodeStream64LvlZlTest: "
			"Size %d -> %d\n", isz, j);
		free(tbuf);
		return(-1);
	}
	
	for(k=0; k<isz; k++)
		if(tbuf[wsz+k]!=ibuf[k])
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
