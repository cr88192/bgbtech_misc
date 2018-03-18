typedef unsigned char byte;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

#define force_inline	__forceinline


// #define BTFLZH_DLHASH_SZ	4096
// #define BTFLZH_DLHASH_LVL	16

// #define BTFLZH_DLHASH_SZ	1024
// #define BTFLZH_DLHASH_SZ	4096
// #define BTFLZH_DLHASH_LVL	64

#define BTFLZH_DLHASH_SZ	16384
#define BTFLZH_DLHASH_LVL	16

#define BTFLZH_CDHASH_SZ	16384

// #define BTFLZH_CDHASH_SZ	4096
// #define BTFLZH_CDHASH_CD	65536
#define BTFLZH_CDHASH_CD	131072
// #define BTFLZH_CDHASH_SD	512
#define BTFLZH_CDHASH_SD	1024

// #define BTFLZH_DLHASH_SZ	256
// #define BTFLZH_DLHASH_LVL	256

typedef struct BtFLZH_EncodeCtx_s BtFLZH_EncodeCtx;

struct BtFLZH_EncodeCtx_s {
byte *bits_ptr;
u32 bits_win;
byte bits_pos;

byte *ibuf_strt;
byte *ibuf_end;
byte *ibuf_ptr;

byte is_stat;
byte prio_speed;

u32 htab_lt[BTFLZH_HTAB_LUSZ];	//Lookup T
u32 htab_ll[BTFLZH_HTAB_LUSZ];	//Lookup L
u32 htab_ld[BTFLZH_HTAB_LUSZ];	//Lookup D

u32 htab_st[256];	//Search T
u32 htab_sl[256];	//Search L
u32 htab_sd[256];	//Search D

int hstat_st[256];	//Stats for T
int hstat_sl[256];	//Stats for L
int hstat_sd[256];	//Stats for D

int stat_stf[32];

byte stf_v[32];		//Swap-to-Front Val
byte stf_i[32];		//Swap-to-Front Index

// byte **dlhash_ptr;	//direct lookup hash
u32 *dlhash_idx;	//direct lookup hash
byte *dlhash_rov;	//direct lookup hash rov
int dlhash_sz;
int dlhash_lvl;
int maxdist;

u32 *chhash_idx;	//chain hash index
u32 *chhash_chn;	//chain hash chains
int chhash_sz;		//chain hash size
int chhash_cd;		//chain hash chain depth
int chhash_sd;		//chain hash search depth

byte *tbdat_strt;
byte *tbdat_end;
byte *tbdat_ptr;

int stat_tagbits;
int stat_litbits;
int stat_distbits;
int stat_ricebits;
int stat_litxtra;
int stat_lenxtra;
int stat_distxtra;
int stat_ricextra;

int stat_totrun_cnt;
u64 stat_totrun_lit;
u64 stat_totrun_run;
u64 stat_totrun_dist;
int stat_totrun_lit_fail16;
int stat_totrun_faildfl;

u16 stat_hti[4][12];
};

force_inline void BtFLZH_WriteNBits(BtFLZH_EncodeCtx *ctx, int val, int cnt)
{
	int p;
	
//	val&=btflzh_pow2masktab[cnt];
	
	if(ctx->is_stat)
		return;
	
	if((val&((1<<cnt)-1))!=val)
		__debugbreak();
		
	if(cnt>24)
		__debugbreak();
	
	p=ctx->bits_pos;
	
	if(ctx->bits_win>>p)
		__debugbreak();
	
	ctx->bits_win|=val<<p;
	p+=cnt;
	(*(u32 *)ctx->bits_ptr)=ctx->bits_win;
	ctx->bits_win>>=p&(~7);
	ctx->bits_ptr+=p>>3;
	ctx->bits_pos=p&7;
}

force_inline void BtFLZH_EncodeHuffTag(BtFLZH_EncodeCtx *ctx, int sym)
{
	int v, l, b;

	if(ctx->is_stat)
	{
		ctx->hstat_st[sym]++;
		return;
	}
	
	b=ctx->htab_st[sym];
	v=b&65535;
	l=(b>>16)&15;
	BtFLZH_WriteNBits(ctx, v, l);
	ctx->stat_tagbits+=l;
}

force_inline void BtFLZH_EncodeHuffLit(BtFLZH_EncodeCtx *ctx, int sym)
{
	int v, l, b;
	
	if(ctx->is_stat)
	{
		ctx->hstat_sl[sym]++;
		return;
	}
	
	b=ctx->htab_sl[sym];
	v=b&65535;
	l=(b>>16)&15;
	BtFLZH_WriteNBits(ctx, v, l);
	ctx->stat_litbits+=l;
}

force_inline void BtFLZH_EncodeHuffDist(BtFLZH_EncodeCtx *ctx, int sym)
{
	int v, l, b;
	
	if(ctx->is_stat)
	{
		ctx->hstat_sd[sym]++;
		return;
	}
	
	b=ctx->htab_sd[sym];
	v=b&65535;
	l=(b>>16)&15;
	BtFLZH_WriteNBits(ctx, v, l);
	ctx->stat_distbits+=l;
}

force_inline void BtFLZH_EncodeEmitRice(BtFLZH_EncodeCtx *ctx,
	int val, int rk)
{
	int q, f;
	
	q=val>>rk;
	f=val&((1<<rk)-1);
	
	BtFLZH_WriteNBits(ctx, (1<<q)-1, q+1);
	BtFLZH_WriteNBits(ctx, f, rk);
	ctx->stat_ricebits+=q+rk+1;
}

void BtFLZH_EncodeEmitStfRice(BtFLZH_EncodeCtx *ctx,
	int sym, int rk)
{
	int j0, k0, j1, k1;

#if 0
	BtFLZH_WriteNBits(ctx, sym, 5);
	return;
#endif

	if(sym>>5)
		__debugbreak();
	
	ctx->stat_stf[sym]++;
	
	j0=ctx->stf_i[sym];
//	j1=(j0*3)/4;
//	j1=(j0*13)/16;
	j1=BTFLZH_STFSWAP(j0);

	k0=ctx->stf_v[j0];	k1=ctx->stf_v[j1];
	ctx->stf_v[j0]=k1;	ctx->stf_v[j1]=k0;
	ctx->stf_i[k1]=j0;	ctx->stf_i[k0]=j1;
	
	BtFLZH_EncodeEmitRice(ctx, j0, rk);
}

void BtFLZH_EncodeEmitHuffLengths(BtFLZH_EncodeCtx *ctx, byte *clbuf)
{
//	byte stf[32];
//	byte stfi[32];
	int i, j, k, l, ll, rk;

//	memset(obuf, 0, 256);
	memcpy(ctx->stf_v, btflzh_initstf, 32);

	for(i=0; i<32; i++)
		{ ctx->stf_i[ctx->stf_v[i]]=i; }

	rk=BTFLZH_STF_RK;

	ll=-1; i=0;
	while(i<256)
	{
		l=clbuf[i];

		if((l&15)!=l)
			__debugbreak();
		
		for(j=i+1; j<256; j++)
			if(clbuf[j]!=l)break;
		k=j-i;

#if 1
		if(l==0)
		{
			if((j>=256) && (k>3))
			{
				BtFLZH_EncodeEmitStfRice(ctx, 19, rk);
				break;
			}
		}

		if(l==ll)
		{
			if((k>=2) && (k<=9))
			{
				BtFLZH_EncodeEmitStfRice(ctx, 16, rk);
				BtFLZH_WriteNBits(ctx, k-2, 3);
				ctx->stat_ricextra+=3;
				i+=k;
				continue;
			}

			if(k>=10)
			{
				if(k>41)k=41;
				BtFLZH_EncodeEmitStfRice(ctx, 17, rk);
				BtFLZH_WriteNBits(ctx, k-10, 5);
				ctx->stat_ricextra+=5;
				i+=k;
				continue;
			}
		}

		if(l==0)
		{
			if((k>=2) && (k<=9))
			{
				BtFLZH_EncodeEmitStfRice(ctx, 18, rk);
				BtFLZH_WriteNBits(ctx, k-2, 3);
				ctx->stat_ricextra+=3;
				i+=k;
				continue;
			}
		}
#endif

		BtFLZH_EncodeEmitStfRice(ctx, l, rk);
		ll=l;
		i++;
	}
}


void BtFLZH_EncodeDistance(BtFLZH_EncodeCtx *ctx, int val)
{
	int b, m, l;
	int i;
	
	for(i=0; i<124; i++)
//	for(i=0; i<90; i++)
	{
		m=btflzh_distbase[i+1];
		if(m>val)
		{
			b=btflzh_distbase[i];
			l=btflzh_distxtra[i];
			BtFLZH_EncodeHuffDist(ctx, i);
			BtFLZH_WriteNBits(ctx, val-b, l);
			ctx->stat_distxtra+=l;
			return;
		}
	}
	
	__debugbreak();
}

void BtFLZH_EncodeTagRL(BtFLZH_EncodeCtx *ctx,
	int nrval, int nlval)
{
	int bl, m, ll, br, lr;
	int ir, il;
	int i;

#ifdef BTFLZH_LEN44
	for(ir=0; ir<16; ir++)
	{
		m=btflzh_len2base[ir+1];
		if(m>nrval)
		{
			br=btflzh_len2base[ir];
			lr=btflzh_len2xtra[ir];
			break;
		}
	}
	
	for(il=0; il<16; il++)
	{
		m=btflzh_len2base[il+1];
		if(m>nlval)
		{
			bl=btflzh_len2base[il];
			ll=btflzh_len2xtra[il];
			break;
		}
	}

	if((ir>=0) && (ir<16) && (il>0) && (il<16))
	{
		i=(ir<<4)+il;
		BtFLZH_EncodeHuffTag(ctx, i);
		BtFLZH_WriteNBits(ctx, nrval-br, lr);
		BtFLZH_WriteNBits(ctx, nlval-bl, ll);
		ctx->stat_litxtra+=lr;
		ctx->stat_lenxtra+=ll;
		return;
	}

#else
	for(ir=0; ir<8; ir++)
	{
		m=btflzh_rawbase[ir+1];
		if(m>nrval)
		{
			br=btflzh_rawbase[ir];
			lr=btflzh_rawxtra[ir];
			break;
		}
	}
	
	for(il=0; il<128; il++)
	{
		m=btflzh_distbase[il+1];
		if(m>nlval)
		{
			bl=btflzh_distbase[il];
			ll=btflzh_distxtra[il];
			break;
		}
	}

	if((ir>=0) && (ir<8) && (il>0) && (il<32))
	{
		i=(ir<<5)+il;
		BtFLZH_EncodeHuffTag(ctx, i);
		BtFLZH_WriteNBits(ctx, nrval-br, lr);
		BtFLZH_WriteNBits(ctx, nlval-bl, ll);
		ctx->stat_litxtra+=lr;
		ctx->stat_lenxtra+=ll;
		return;
	}
#endif

	BtFLZH_EncodeHuffTag(ctx, 0);
	BtFLZH_EncodeDistance(ctx, nrval);
	BtFLZH_EncodeDistance(ctx, nlval);
}

void BtFLZH_EncodePrefix(BtFLZH_EncodeCtx *ctx,
	int nr, int nl, int nd)
{
	BtFLZH_EncodeTagRL(ctx, nr, nl);
	if(nl!=2)
		BtFLZH_EncodeDistance(ctx, nd);
}


byte *BtFLZH_EncodeGetVli(byte *cs, int *rval)
{
	int i, j;
	
	i=*cs;
	if(i<0x80)
	{
		*rval=*cs++;
		return(cs);
	}

	if(i<0xC0)
	{
		j=(*cs++)&0x3F;
		j=(j<<8)|(*cs++);
		*rval=j;
		return(cs);
	}

	if(i<0xE0)
	{
		j=(*cs++)&0x1F;
		j=(j<<8)|(*cs++);
		j=(j<<8)|(*cs++);
		*rval=j;
		return(cs);
	}

	if(i<0xF0)
	{
		j=(*cs++)&0x0F;
		j=(j<<8)|(*cs++);
		j=(j<<8)|(*cs++);
		j=(j<<8)|(*cs++);
		*rval=j;
		return(cs);
	}

	if(i<0xF8)
	{
		j=(*cs++)&0x07;
		j=(j<<8)|(*cs++);
		j=(j<<8)|(*cs++);
		j=(j<<8)|(*cs++);
		j=(j<<8)|(*cs++);
		*rval=j;
		return(cs);
	}

	*rval=0;
	return(cs);
}

void BtFLZH_LzEntropyEncode(BtFLZH_EncodeCtx *ctx)
{
	byte *cs, *cse;
	int lnl, lnd;
	int nr, nl, nd, nl2, nd2;
	int i;
	
	cs=ctx->tbdat_strt;
	cse=ctx->tbdat_ptr;
	
	lnl=-1; lnd=-1;
	while(cs<cse)
	{
		cs=BtFLZH_EncodeGetVli(cs, &nr);
		cs=BtFLZH_EncodeGetVli(cs, &nl);
		cs=BtFLZH_EncodeGetVli(cs, &nd);

		nl2=nl;
		nd2=nd;

		ctx->stat_totrun_cnt++;
		ctx->stat_totrun_lit+=nr;
		ctx->stat_totrun_run+=nl;
		ctx->stat_totrun_dist+=nd;
		
		if(nr>=16)
			ctx->stat_totrun_lit_fail16++;

#if 1
//		if(nr<64)
		if(nr<BTFLZH_RAWLIM)
		{
			if(nl==lnl)
			{
				nl2=1;
				if(nd==lnd)
					nl2=2;
			}
			if(nd==lnd)
				nd2=0;
		}else
		{
			if(nl==lnl)
				nl2=1;
			if(nd==lnd)
				nd2=0;
			ctx->stat_totrun_faildfl++;
		}
#endif

		lnl=nl;
		lnd=nd;

		BtFLZH_EncodePrefix(ctx, nr, nl2, nd2);
		i=nr;
		while(i--)
			{ BtFLZH_EncodeHuffLit(ctx, *cs++); }
		
		if(!(nl2|nd2) && (cs<cse))
			__debugbreak();
		if(cs>cse)
			__debugbreak();
	}
}


int BtFLZH_EncBalanceTree_r(
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

	h1=BtFLZH_EncBalanceTree_r(nodes, nlen, nodes[root*2+0], h+1, ml);
	h2=BtFLZH_EncBalanceTree_r(nodes, nlen, nodes[root*2+1], h+1, ml);
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

void BtFLZH_EncCalcLengths_r(short *nodes, byte *cl, int root, int h)
{
	if(root<0)
	{
		cl[(-root)-1]=h;
		return;
	}

	BtFLZH_EncCalcLengths_r(nodes, cl, nodes[root*2+0], h+1);
	BtFLZH_EncCalcLengths_r(nodes, cl, nodes[root*2+1], h+1);
}

int BtFLZH_EncBuildLengths(int *stat, int nc, byte *cl, int ml)
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
		k=BtFLZH_EncBalanceTree_r(nodes, nlen, l, 0, ml);
	if(k>ml)
	{
		fprintf(stderr, "tree balance failure\n");
		fprintf(stderr, "tree depth %d, org %d, %d nodes\n", k, j, nn);
		return(-2);
	}

	BtFLZH_EncCalcLengths_r(nodes, cl, l, 0);
	return(0);
}

int BtFLZH_EncBuildLengthsAdjust(int *stat, int nc, byte *cl, int ml, int tn)
{
	int i, j;

	while(1)
	{
		j=BtFLZH_EncBuildLengths(stat, nc, cl, ml);
		if(j<0)
			fprintf(stderr, "BtFLZH_EncBuildLengthsAdjust: "
				"Huff Fail %d %d\n", j, tn);

		for(i=0; i<nc; i++)
			if(stat[i] && !cl[i])
				break;
		if(i>=nc)break;

		fprintf(stderr, "BtFLZH_EncBuildLengthsAdjust: Fiddle Adjust %d\n", tn);
		for(i=0; i<nc; i++)
			stat[i]++;
		continue;
	}
	return(0);
}

void BtFLZH_EncSetupTable(BtFLZH_EncodeCtx *ctx,
	int tn, byte *clbuf, int ti)
{
	u32 *ltab, *stab;
	int nb;
	int i;

	switch(tn)
	{
	case 1:
		ltab=ctx->htab_lt;
		stab=ctx->htab_st;
		break;
	case 2:
		ltab=ctx->htab_ll;
		stab=ctx->htab_sl;
		break;
	case 3:
		ltab=ctx->htab_ld;
		stab=ctx->htab_sd;
		break;
	}

	if(clbuf)
	{
		BtFLZH_SetupHuffLengths(NULL, clbuf, ltab, stab);
	}else
	{
		nb=8-ti;
		for(i=0; i<BTFLZH_HTAB_LUSZ; i++)
			{ ltab[i]=(i&((1<<nb)-1))|(nb<<16); }
		for(i=0; i<256; i++)
			{ stab[i]=0; }
		for(i=0; i<(1<<nb); i++)
			stab[i]=i|(nb<<16);
	}
}

void BtFLZH_EncEmitSetFixedTable(BtFLZH_EncodeCtx *ctx, int tn, int ti)
{
	byte clbuf[256];

	BtFLZH_WriteNBits(ctx, 2, 4);
	BtFLZH_WriteNBits(ctx, tn, 2);
	BtFLZH_WriteNBits(ctx, 0, 2);
	BtFLZH_WriteNBits(ctx, ti, 6);

	if(ti<8)
	{
		BtFLZH_EncSetupTable(ctx, tn, NULL, ti);
	}else
	{
		BtFLZH_DecodeFixedLengths(NULL, clbuf, ti);
		BtFLZH_EncSetupTable(ctx, tn, clbuf, ti);
	}
}

void BtFLZH_EncEmitSetHuffTable(BtFLZH_EncodeCtx *ctx,
	int tn, byte *clbuf)
{
	BtFLZH_WriteNBits(ctx, 2, 4);
	BtFLZH_WriteNBits(ctx, tn, 2);
	BtFLZH_WriteNBits(ctx, 1, 2);

	BtFLZH_EncodeEmitHuffLengths(ctx, clbuf);
	BtFLZH_EncSetupTable(ctx, tn, clbuf, 0);
}

void BtFLZH_EncEmitCSetHuffTable(BtFLZH_EncodeCtx *ctx,
	int tn, int *stat, byte *clbuf)
{
	int tbh_sl, tbr_sl, uh_sl;
	int nb;
	int i, j, k;

	j=0;
	for(i=0; i<256; i++)
		{ if(stat[i]>0)j=i; }

	nb=8;
	if(j<128)nb=7;
	if(j< 64)nb=6;
	if(j< 32)nb=5;
	if(j< 16)nb=4;
	if(j<  8)nb=3;
	if(j<  4)nb=2;
	if(j<  2)nb=1;

	ctx->is_stat=0;
	
	tbh_sl=0; tbr_sl=0;
	for(i=0; i<256; i++)
	{
		tbh_sl+=stat[i]*clbuf[i];
		tbr_sl+=stat[i]*nb;
	}

	if(ctx->prio_speed)
	{
		if(tn==2)
		{
//			uh_sl=(tbr_sl*0.85)>tbh_sl;
//			uh_sl=(tbr_sl*0.80)>tbh_sl;
//			uh_sl=(tbr_sl*0.87)>tbh_sl;
			uh_sl=(tbr_sl*0.83)>tbh_sl;
//			uh_sl=1;
		}else
		{
			uh_sl=(tbr_sl*0.90)>tbh_sl;
		}
	}else
	{
		uh_sl=(tbr_sl*0.97)>tbh_sl;
	}
	
	if(uh_sl)
	{
		BtFLZH_EncEmitSetHuffTable(ctx, tn, clbuf);
		ctx->stat_hti[tn][8]++;
	}
	else
	{
		BtFLZH_EncEmitSetFixedTable(ctx, tn, 8-nb);
		ctx->stat_hti[tn][8-nb]++;
	}
}

void BtFLZH_LzEntropyEncode2(BtFLZH_EncodeCtx *ctx)
{
	byte cl_st[256];
	byte cl_sl[256];
	byte cl_sd[256];
	int tbh_sl, tbr_sl, uh_sl;
	int nb;
	int i, j, k;

	for(i=0; i<256; i++)
	{
		ctx->hstat_st[i]=0;
		ctx->hstat_sl[i]=0;
		ctx->hstat_sd[i]=0;
	}

	ctx->is_stat=1;
	BtFLZH_LzEntropyEncode(ctx);
	
	BtFLZH_EncBuildLengthsAdjust(ctx->hstat_st, 256, cl_st, 15, 1);
	BtFLZH_EncBuildLengthsAdjust(ctx->hstat_sl, 256, cl_sl, 15, 2);
	BtFLZH_EncBuildLengthsAdjust(ctx->hstat_sd, 256, cl_sd, 15, 3);

	BtFLZH_EncEmitCSetHuffTable(ctx, 1, ctx->hstat_st, cl_st);
	BtFLZH_EncEmitCSetHuffTable(ctx, 2, ctx->hstat_sl, cl_sl);
	BtFLZH_EncEmitCSetHuffTable(ctx, 3, ctx->hstat_sd, cl_sd);

#if 0
	j=0;
	for(i=0; i<256; i++)
		{ if(ctx->hstat_sl[i]>0)j=i; }

	nb=8;
	if(j<128)nb=7;
	if(j< 64)nb=6;
	if(j< 32)nb=5;
	if(j< 16)nb=4;
	if(j<  8)nb=3;
	if(j<  4)nb=2;
	if(j<  2)nb=1;

	ctx->is_stat=0;
	
	tbh_sl=0; tbr_sl=0;
	for(i=0; i<256; i++)
	{
		tbh_sl+=ctx->hstat_sl[i]*cl_sl[i];
		tbr_sl+=ctx->hstat_sl[i]*nb;
	}

	if(ctx->prio_speed)
	{
//		uh_sl=(tbr_sl*0.85)>tbh_sl;
//		uh_sl=(tbr_sl*0.80)>tbh_sl;
//		uh_sl=(tbr_sl*0.87)>tbh_sl;
		uh_sl=(tbr_sl*0.83)>tbh_sl;
//		uh_sl=1;
	}else
	{
		uh_sl=(tbr_sl*0.97)>tbh_sl;
	}
	
	BtFLZH_EncEmitSetHuffTable(ctx, 1, cl_st);
	if(uh_sl)
		BtFLZH_EncEmitSetHuffTable(ctx, 2, cl_sl);
	else
		BtFLZH_EncEmitSetFixedTable(ctx, 2, 8-nb);
	BtFLZH_EncEmitSetHuffTable(ctx, 3, cl_sd);

//	BtFLZH_EncEmitSetFixedTable(ctx, 1, 0);
//	BtFLZH_EncEmitSetFixedTable(ctx, 2, 0);
//	BtFLZH_EncEmitSetFixedTable(ctx, 3, 0);
#endif

	BtFLZH_WriteNBits(ctx, 1, 4);
	BtFLZH_LzEntropyEncode(ctx);
}


void BtFLZH_SetupHashForEncode(BtFLZH_EncodeCtx *ctx,
	byte *ibuf, int ibsz)
{
	int i;

	ctx->ibuf_strt=ibuf;
	ctx->ibuf_end=ibuf+ibsz;
	
//	if(!ctx->dlhash_ptr)
	if(!ctx->dlhash_idx)
	{
//		ctx->dlhash_ptr=malloc(ctx->dlhash_sz*ctx->dlhash_lvl*sizeof(byte *));
		ctx->dlhash_idx=malloc(ctx->dlhash_sz*ctx->dlhash_lvl*sizeof(u32));
		ctx->dlhash_rov=malloc(ctx->dlhash_sz*sizeof(byte));
	}
	
//	memset(ctx->dlhash_ptr, 0, ctx->dlhash_sz*ctx->dlhash_lvl*sizeof(byte *));
	memset(ctx->dlhash_idx, 0, ctx->dlhash_sz*ctx->dlhash_lvl*sizeof(u32));
	memset(ctx->dlhash_rov, 0, ctx->dlhash_sz*sizeof(byte));
	
	if(!ctx->chhash_idx)
	{
		ctx->chhash_idx=malloc(ctx->chhash_sz*sizeof(u32));
		ctx->chhash_chn=malloc(ctx->chhash_cd*sizeof(u32));
	}

	memset(ctx->chhash_idx, 0, ctx->chhash_sz*sizeof(u32));
	memset(ctx->chhash_chn, 0, ctx->chhash_cd*sizeof(u32));
}

void BtFLZH_SetupTbDataForEncode(BtFLZH_EncodeCtx *ctx,
	byte *ibuf, int ibsz)
{
	int tsz;
	int i;

	tsz=ibsz*3;
	
	if(ctx->tbdat_strt)
	{
		i=ctx->tbdat_end-ctx->tbdat_strt;
		if(i<tsz)
		{
			free(ctx->tbdat_strt);
			ctx->tbdat_strt=NULL;
		}
	}
	
	if(!ctx->tbdat_strt)
	{
		ctx->tbdat_strt=malloc(tsz);
		ctx->tbdat_end=ctx->tbdat_strt+tsz;
	}
	
	ctx->tbdat_ptr=ctx->tbdat_strt;
}

void BtFLZH_EncodeHashForPos(BtFLZH_EncodeCtx *ctx, byte *pos)
{
	u32 dh, ch, hr, v;
	u32 cn, psi;
	
	v=*(u32 *)pos;
	dh=(v&0x00FFFFFF)*65521;
//	dh=(v&0xFFFFFFFF)*65521;
	ch=(v&0x00FFFFFF)*65521;
//	ph=ph*65521;
//	h=(h>>17)&(BTFLZH_DLHASH_SZ-1);
//	h=(h>>17)&(ctx->dlhash_sz-1);
//	h=(h>>16)&(ctx->dlhash_sz-1);
	dh=(dh>>18)&(ctx->dlhash_sz-1);
	ch=(ch>>18)&(ctx->chhash_sz-1);
	
	hr=ctx->dlhash_rov[dh];
//	hr=(hr-1)&(BTFLZH_DLHASH_LVL-1);
	hr=(hr-1)&(ctx->dlhash_lvl-1);
	ctx->dlhash_rov[dh]=hr;
//	ctx->dlhash_ptr[(h*BTFLZH_DLHASH_LVL)+hr]=pos;
//	ctx->dlhash_ptr[(h*ctx->dlhash_lvl)+hr]=pos;
	psi=pos-ctx->ibuf_strt;
	ctx->dlhash_idx[(dh*ctx->dlhash_lvl)+hr]=psi;

	cn=psi&(ctx->chhash_cd-1);
	ctx->chhash_chn[cn]=ctx->chhash_idx[ch];
	ctx->chhash_idx[ch]=psi;
}

void BtFLZH_EncodeHashForStr(BtFLZH_EncodeCtx *ctx, byte *pos, int n)
{
	int i;
	
	for(i=0; i<n; i++)
	{
		BtFLZH_EncodeHashForPos(ctx, pos+i);
	}
}

int BtFLZH_CheckMatchString(BtFLZH_EncodeCtx *ctx,
	byte *psrc, byte *psrce, byte *pdst)
{
	byte *ps, *pt, *pse, *pte;
	
	ps=psrc;
	pt=pdst;
	pse=psrce;

	if((*(u32 *)ps)!=(*(u32 *)pt))
		return(0);
	ps+=4; pt+=4;

#if 1
	while(ps<pse)
	{
		if(((u32 *)ps)[0]!=((u32 *)pt)[0])
			break;
		if(((u32 *)ps)[1]!=((u32 *)pt)[1])
			{ ps+=4; pt+=4; break; }
		if(((u32 *)ps)[2]!=((u32 *)pt)[2])
			{ ps+=8; pt+=8; break; }
		if(((u32 *)ps)[3]!=((u32 *)pt)[3])
			{ ps+=12; pt+=12; break; }
		ps+=16; pt+=16;
	}
#endif

#if 0
	while(ps<pse)
	{
		if((*(u32 *)ps)!=(*(u32 *)pt))
			break;
		ps+=4; pt+=4;
	}
#endif

	while(ps<pse)
	{
//		if(pt>=psrc)break;
		if((*ps)!=(*pt))break;
		ps++; pt++;
	}
	
//	if(ps>pse)
//		return(pse-psrc);
	
	return(ps-psrc);
}

int BtFLZH_LookupHashClassifyReject(BtFLZH_EncodeCtx *ctx,
	int l, int d)
{
	int i;

	if(l>=3)
	{
//		if((d<=0) || (d>ctx->maxdist))
//			__debugbreak();

//		if((d<=0) || (d>ctx->maxdist))
		if(d<=0)
			return(1);
		if(d>ctx->maxdist)
			__debugbreak();
	
		if(!ctx->prio_speed)
			return(0);

		i=1;

#if 0
		if(d>=0x80000)
			{ if(l<48)i=0; }
		else if(d>=0x10000)
			{ if(l<24)i=0; }
		else if(d>=0x4000)
			{ if(l<10)i=0; }
		else if(d>=0x2000)
			{ if(l<5)i=0; }
	//	else if(d>=0x1000)
	//		{ if(l<6)i=0; }
#endif

#if 1
		if(d>=0x200000)
			{ if(l<48)i=0; }
		else if(d>=0x80000)
			{ if(l<32)i=0; }
		else if(d>=0x10000)
			{ if(l<16)i=0; }
		else if(d>=0x4000)
			{ if(l<8)i=0; }
#endif

		return(!i);
	}

	return(1);
}

int BtFLZH_LookupHashString(BtFLZH_EncodeCtx *ctx,
	byte *pos, int *rl, int *rd)
{
	byte *pd1, *pse, *pss;
	u32 dh, ch, v, px;
	int bl, bd;
	int sd, sm, md;
	int l, d, n, nl, nd, hr;
	int i, j, k;
	
	*rl=0;
	*rd=0;
	
	if((pos+3)>=ctx->ibuf_end)
		return(0);
	
//	return(0);
	
	v=*(u32 *)pos;
	dh=(v&0x00FFFFFF)*65521;
//	dh=(v&0xFFFFFFFF)*65521;
	ch=(v&0x00FFFFFF)*65521;
//	h=h*65521;

//	h=(h>>17)&(BTFLZH_DLHASH_SZ-1);
//	h=(h>>17)&(ctx->dlhash_sz-1);
//	h=(h>>16)&(ctx->dlhash_sz-1);
	dh=(dh>>18)&(ctx->dlhash_sz-1);
	ch=(ch>>18)&(ctx->chhash_sz-1);
	
//	v=dh*BTFLZH_DLHASH_LVL;
	v=dh*ctx->dlhash_lvl;
	hr=ctx->dlhash_rov[dh];
	
	bl=0; bd=0;

//	nl=65536;
//	if((pos+nl)>ctx->ibuf_end)
//		nl=ctx->ibuf_end-pos;
	nl=ctx->ibuf_end-pos;
	if(nl>65536)
		nl=65536;
	pse=pos+nl;	

#if 1
	
	pss=ctx->ibuf_strt;
	nd=pos-pss;

#if 1
	for(i=1; i<=4; i++)
	{
		if(i>nd)break;
		pd1=pos-i;
		if(*(u32 *)pos==*(u32 *)pd1)
		{
			d=i;
			px=*(u32 *)pos;		
			for(l=i; l<nl; l+=i)
				if((*(u32 *)(pos+l))!=px)
					break;
			if(l>nl)l=nl;
			if(l>bl) { bl=l; bd=d; }
		}
	}
#endif

#endif
	if(bl<1024)
	{
		sd=ctx->dlhash_lvl; sm=sd-1;
		md=ctx->maxdist;
	//	for(i=0; i<BTFLZH_DLHASH_LVL; i++)
		for(i=0; i<sd; i++)
		{
	//		pd1=ctx->dlhash_ptr[v+((hr+i)&(BTFLZH_DLHASH_LVL-1))];
			j=v+((hr+i)&sm);

			k=ctx->dlhash_idx[j];
			if(k==0)break;
			pd1=pss+k;

//			pd1=ctx->dlhash_ptr[j];
	//		if(!pd1)continue;
//			if(!pd1)break;
			d=pos-pd1;
			
			if(d>md)
			{
//				ctx->dlhash_ptr[j]=NULL;
				ctx->dlhash_idx[j]=0;
				break;
			}
			
			l=BtFLZH_CheckMatchString(ctx, pos, pse, pd1);

			if(BtFLZH_LookupHashClassifyReject(ctx, l, d))
				l=0;

#if 0
			if(d>=0x80000)
				{ if(l<48)l=0; }
			else if(d>=0x10000)
				{ if(l<24)l=0; }
			else if(d>=0x4000)
				{ if(l<10)l=0; }
			else if(d>=0x2000)
				{ if(l<5)l=0; }
	//		else if(d>=0x1000)
	//			{ if(l<6)l=0; }
#endif

			if(l>bl)
				{ bl=l; bd=d; }
		}
	}

	if(bl<512)
//	if(0)
	{
		sm=ctx->chhash_cd-1;
		md=ctx->maxdist;
		i=ctx->chhash_idx[ch];
		n=ctx->chhash_sd;
		while(n--)
		{
			pd1=pss+i;
			d=pos-pd1;
			
			if(d>md)
				break;
			
			l=BtFLZH_CheckMatchString(ctx, pos, pse, pd1);

			if(BtFLZH_LookupHashClassifyReject(ctx, l, d))
				l=0;

#if 0
			if(d>=0x10000)
				{ if(l<24)l=0; }
			else if(d>=0x4000)
				{ if(l<10)l=0; }
			else if(d>=0x2000)
				{ if(l<5)l=0; }
#endif

			if(l>bl)
				{ bl=l; bd=d; }

			j=ctx->chhash_chn[i&sm];
			if(j>=i)break;
			i=j;
		}
	}
	
	if(bl>nl)	bl=nl;
	if(bl<3)	bl=0;

	*rl=bl;
	*rd=bd;
	return(bl>3);
}

byte *BtFLZH_EncodeEmitVli(byte *ct, int val)
{
	if(val<0)
		{ __debugbreak(); }

	if(val<0x80)
	{
		*ct++=val;
		return(ct);
	}

	if(val<0x4000)
	{
		*ct++=0x80|(val>>8);
		*ct++=val;
		return(ct);
	}

	if(val<0x200000)
	{
		*ct++=0xC0|(val>>16);
		*ct++=val>>8;
		*ct++=val;
		return(ct);
	}

	if(val<0x10000000)
	{
		*ct++=0xE0|(val>>24);
		*ct++=val>>16;
		*ct++=val>>8;
		*ct++=val;
		return(ct);
	}

	*ct++=0xF0;
	*ct++=val>>24;
	*ct++=val>>16;
	*ct++=val>>8;
	*ct++=val;
	return(ct);
}

void BtFLZH_LzStageEncode(BtFLZH_EncodeCtx *ctx,
	byte *ibuf, int ibsz)
{
	byte *cs, *cse, *ct;
	byte *cm;
	int i0, i1, i2, i3;
	int nr, nl, nd, nl1, nd1, nl2, nd2, nl3, nd3;
	int i, j, k;

	cs=ibuf; cse=ibuf+ibsz;
	ct=ctx->tbdat_strt;
	
	i0=0; i1=0; i2=0; i3=0;
	nl=0; nl1=0; nl2=0; nl3=0;
	nd=0; nd1=0; nd2=0; nd3=0;
	
	cm=cs;
	while(cs<cse)
	{
//		i=BtFLZH_LookupHashString(ctx, cs, &nl, &nd);
//		j=BtFLZH_LookupHashString(ctx, cs+1, &nl2, &nd2);

		nl=nl1; nd=nd1; i0=i1;
		nl1=nl2; nd1=nd2; i1=i2;
		nl2=nl3; nd2=nd3; i2=i3;
//		i1=BtFLZH_LookupHashString(ctx, cs+1, &nl1, &nd1);
//		i2=BtFLZH_LookupHashString(ctx, cs+2, &nl2, &nd2);
		i3=BtFLZH_LookupHashString(ctx, cs+3, &nl3, &nd3);

		i=i0;

//		i=BtFLZH_LookupHashString(ctx, cs, &nl, &nd);
		
		if(nl1 > (nl+2))	i=0;
		if(nl2 > (nl+4))	i=0;
		if(nl3 > (nl+6))	i=0;

		if(i>0)
		{
			if(!(nl|nd))
				__debugbreak();
			if((nd<=0) || (nd>ctx->maxdist))
				__debugbreak();

			nr=cs-cm;
			ct=BtFLZH_EncodeEmitVli(ct, nr);
			ct=BtFLZH_EncodeEmitVli(ct, nl);
			ct=BtFLZH_EncodeEmitVli(ct, nd);
						
			while(cm<cs)
				{ *ct++=*cm++; }
			BtFLZH_EncodeHashForStr(ctx, cs, nl);
			cs+=nl;
			cm=cs;

			i1=BtFLZH_LookupHashString(ctx, cs+0, &nl1, &nd1);
			i2=BtFLZH_LookupHashString(ctx, cs+1, &nl2, &nd2);
			i3=BtFLZH_LookupHashString(ctx, cs+2, &nl3, &nd3);
			continue;
		}

		BtFLZH_EncodeHashForPos(ctx, cs);
		cs++;
	}

	if(cs>cm)
	{
		nr=cs-cm;
		ct=BtFLZH_EncodeEmitVli(ct, nr);
		ct=BtFLZH_EncodeEmitVli(ct, 0);
		ct=BtFLZH_EncodeEmitVli(ct, 0);
		while(cm<cs)
			{ *ct++=*cm++; }
	}else
	{
		ct=BtFLZH_EncodeEmitVli(ct, 0);
		ct=BtFLZH_EncodeEmitVli(ct, 0);
		ct=BtFLZH_EncodeEmitVli(ct, 0);
	}

	ctx->tbdat_ptr=ct;
}

void BtFLZH_LzEncodeBlock(BtFLZH_EncodeCtx *ctx,
	byte *ibuf, int ibsz)
{
//	BtFLZH_SetupHashForEncode(ctx, ibuf, ibsz);
	BtFLZH_SetupTbDataForEncode(ctx, ibuf, ibsz);
	ctx->ibuf_end=ibuf+ibsz;
	
	BtFLZH_LzStageEncode(ctx, ibuf, ibsz);
	BtFLZH_LzEntropyEncode2(ctx);
}

int BtFLZH_LzEncodeStream(BtFLZH_EncodeCtx *ctx,
	byte *ibuf, int ibsz,
	byte *obuf, int obsz,
	int lvl)
{
	int stf_v[32], stf_c[32];
	byte *cs, *cse;
	u64 totbits;
	double totsc;
	int anr, anl, and, nr;
	int bn, bc;
	int i, j, k;
	
#if 0
	for(i=0; i<(12*8); i++)
	{
		if(btflzh_distbase[i+1]!=
			(btflzh_distbase[i]+
				(1<<btflzh_distxtra[i])))
					__debugbreak();
	}
#endif

#if 1
	for(i=0; i<256; i++)
	{
		j=	((i>>7)&0x01)|	((i>>5)&0x02)|
			((i>>3)&0x04)|	((i>>1)&0x08)|
			((i<<1)&0x10)|	((i<<3)&0x20)|
			((i<<5)&0x40)|	((i<<7)&0x80);
		if(btflzh_trans8tab[i]!=j)
			__debugbreak();
	}

	for(i=0; i<256; i++)
	{
		j=i; k=0;
		while(j&1) { j=j>>1; k++; }
		if(k!=btflzh_rqtab[i])
			__debugbreak();
	}

#endif

	ctx->dlhash_sz=BTFLZH_DLHASH_SZ;
	ctx->dlhash_lvl=BTFLZH_DLHASH_LVL;
	ctx->maxdist=1<<24;
//	ctx->maxdist=1<<22;
//	ctx->maxdist=1<<20;

	ctx->chhash_sz=BTFLZH_CDHASH_SZ;
	ctx->chhash_cd=BTFLZH_CDHASH_CD;
	ctx->chhash_sd=BTFLZH_CDHASH_SD;
	ctx->prio_speed=1;

	switch(lvl&15)
	{
	case 1:
		ctx->maxdist=1<<18;		ctx->prio_speed=1;
		ctx->dlhash_lvl=1;
		ctx->chhash_cd=32768;	ctx->chhash_sd=0;
		break;
	case 2:
		ctx->maxdist=1<<18;		ctx->prio_speed=1;
		ctx->dlhash_lvl=4;
		ctx->chhash_cd=32768;	ctx->chhash_sd=16;
		break;
	case 3:
		ctx->maxdist=1<<19;		ctx->prio_speed=1;
		ctx->dlhash_lvl=8;
		ctx->chhash_cd=65536;	ctx->chhash_sd=32;
		break;
	case 4:
		ctx->maxdist=1<<19;		ctx->prio_speed=1;
		ctx->dlhash_lvl=16;
		ctx->chhash_cd=65536;	ctx->chhash_sd=64;
		break;
	case 5:
		ctx->maxdist=1<<20;		ctx->prio_speed=1;
		ctx->dlhash_lvl=32;
		ctx->chhash_cd=131072;	ctx->chhash_sd=256;
		break;
	case 6:
		ctx->maxdist=1<<22;		ctx->prio_speed=1;
		ctx->dlhash_lvl=64;
		ctx->chhash_cd=131072;	ctx->chhash_sd=512;
		break;
	case 7:
		ctx->maxdist=1<<22;		ctx->prio_speed=1;
		ctx->dlhash_lvl=128;
		ctx->chhash_cd=131072;	ctx->chhash_sd=1024;
		break;
	case 8:
		ctx->maxdist=1<<24;		ctx->prio_speed=1;
		ctx->dlhash_lvl=256;
		ctx->chhash_cd=262144;	ctx->chhash_sd=2048;
		break;
	case 9:
		ctx->maxdist=1<<24;		ctx->prio_speed=1;
		ctx->dlhash_lvl=512;
		ctx->chhash_cd=524288;	ctx->chhash_sd=4096;
		break;
	
	default:
		break;
	}
	
	if(lvl&16)
		ctx->prio_speed=0;

	ctx->bits_ptr=obuf;
	ctx->bits_win=0;
	ctx->bits_pos=0;

	BtFLZH_SetupHashForEncode(ctx, ibuf, ibsz);

	bc=(ibsz+(1<<18)-1)>>18;
	bn=1;
	cs=ibuf; cse=ibuf+ibsz;
	while((cs+(1<<18))<cse)
	{
		printf("BtFLZH_LzEncodeStream: Block %d/%d\r", bn, bc);
		bn++;
		BtFLZH_LzEncodeBlock(ctx, cs, (1<<18));
		cs+=(1<<18);
	}

	if(cs<cse)
	{
		printf("BtFLZH_LzEncodeStream: Block %d/%d\n", bn, bc);
		bn++;
		BtFLZH_LzEncodeBlock(ctx, cs, cse-cs);
	}

	BtFLZH_WriteNBits(ctx, 0, 4);

//	BtFLZH_WriteNBits(ctx, 0, 4);
//	BtFLZH_WriteNBits(ctx, 0, 16);

	totbits=
		ctx->stat_tagbits+
		ctx->stat_litxtra+
		ctx->stat_lenxtra+
		ctx->stat_litbits+
		ctx->stat_distbits+
		ctx->stat_distxtra+
		ctx->stat_ricebits+
		ctx->stat_ricextra;
	totsc=100.0/totbits;

	printf("stat tagbits    %9d (%9.2fkB) %5.2f%%\n",
		ctx->stat_tagbits, ctx->stat_tagbits*(1.0/8192),
		ctx->stat_tagbits*totsc);
	printf("  stat litxtra  %9d (%9.2fkB) %5.2f%%\n",
		ctx->stat_litxtra, ctx->stat_litxtra*(1.0/8192),
		ctx->stat_litxtra*totsc);
	printf("  stat lenxtra  %9d (%9.2fkB) %5.2f%%\n",
		ctx->stat_lenxtra, ctx->stat_lenxtra*(1.0/8192),
		ctx->stat_lenxtra*totsc);
	printf("stat litbits    %9d (%9.2fkB) %5.2f%%\n",
		ctx->stat_litbits, ctx->stat_litbits*(1.0/8192),
		ctx->stat_litbits*totsc);
	printf("stat distbits   %9d (%9.2fkB) %5.2f%%\n",
		ctx->stat_distbits, ctx->stat_distbits*(1.0/8192),
		ctx->stat_distbits*totsc);
	printf("  stat distxtra %9d (%9.2fkB) %5.2f%%\n",
		ctx->stat_distxtra, ctx->stat_distxtra*(1.0/8192),
		ctx->stat_distxtra*totsc);
	printf("stat ricebits   %9d (%9.2fkB) %5.2f%%\n",
		ctx->stat_ricebits, ctx->stat_ricebits*(1.0/8192),
		ctx->stat_ricebits*totsc);
	printf("  stat ricextra %9d (%9.2fkB) %5.2f%%\n",
		ctx->stat_ricextra, ctx->stat_ricextra*(1.0/8192),
		ctx->stat_ricextra*totsc);

#if 0
	for(i=0; i<32; i++)
		{ stf_v[i]=i; stf_c[i]=ctx->stat_stf[i]; }
	for(i=0; i<32; i++)
		for(j=i+1; j<32; j++)
	{
		if(stf_c[j]>stf_c[i])
		{
			k=stf_v[i]; stf_v[i]=stf_v[j]; stf_v[j]=k;
			k=stf_c[i]; stf_c[i]=stf_c[j]; stf_c[j]=k;
		}
	}
	
	printf("STF: ");
	for(i=0; i<20; i++)
	{
//		printf(" %d", ctx->stat_stf[i]);
		printf(" %d:%d", stf_v[i], stf_c[i]);
	}
	printf("\n");
#endif

	nr=ctx->stat_totrun_cnt;
	anr=ctx->stat_totrun_lit/nr;
	anl=ctx->stat_totrun_run/nr;
	and=ctx->stat_totrun_dist/nr;
	printf("avg: nr=%d nl=%d nd=%d\n", anr, anl, and);

//	printf("fail16_nr=%2.2f%% fail_dfl=%2.2f%%\n",
//		ctx->stat_totrun_lit_fail16*(100.0/nr),
//		ctx->stat_totrun_faildfl*(100.0/nr));

	for(i=1; i<4; i++)
	{
		printf("htab idx %d:", i);
		for(j=0; j<9; j++)
		{
			printf(" %d", ctx->stat_hti[i][j]);
		}
		printf("\n");
	}

	if(ctx->bits_pos)
		ctx->bits_ptr++;
	return(ctx->bits_ptr-obuf);
}

BtFLZH_EncodeCtx *BtFLZH_AllocEncodeContext(void)
{
	BtFLZH_EncodeCtx *ctx;

	ctx=malloc(sizeof(BtFLZH_EncodeCtx));
	memset(ctx, 0, sizeof(BtFLZH_EncodeCtx));
	return(ctx);
}

void BtFLZH_FreeEncodeContext(BtFLZH_EncodeCtx *ctx)
{
//	if(ctx->dlhash_ptr)
//		free(ctx->dlhash_ptr);
	if(ctx->dlhash_idx)
		free(ctx->dlhash_idx);
	if(ctx->dlhash_rov)
		free(ctx->dlhash_rov);
	if(ctx->tbdat_strt)
		free(ctx->tbdat_strt);
		
	if(ctx->chhash_idx)
		free(ctx->chhash_idx);
	if(ctx->chhash_chn)
		free(ctx->chhash_chn);

	free(ctx);
}
