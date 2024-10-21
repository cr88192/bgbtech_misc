// #define TKUAC_QSPL_LIN

// #define TKUAC_QSPL_LINI
#define TKUAC_QSPL_HLINI


// #define TKUAC_QFXSC_MIN	16
#define TKUAC_QFXSC_MIN	64
#define TKUAC_QFXSC_MAX	4096

typedef struct TKuAC_EncState_s TKuAC_EncState;

struct TKuAC_EncState_s {
	byte *ct;
	u32 win;
	byte pos;
	byte status;
	
	byte pk_dc;
	byte pk_ac;
	byte mbform;
	
	int qfxsc;
	int qfxsbp;
	
	int tgtbsz;
	int tgthz;
	int tgtbnum;
	s64 curspos;

	s32 pdc_c;
	s32 pdc_s;
	s32 pdsbp_c;
	s32 pdsbp_s;

	s32 l_pdc_c;
	s32 l_pdc_s;
	s32 l_pdsbp_c;
	s32 l_pdsbp_s;

	s32 sbparm_c[8];
	s32 sbparm_s[2];

	s16 *sbufs;
	s16 *sbufe;

	byte cpt_dc[256];
	byte cpt_ac[256];
	byte cpi_dc[256];
	byte cpi_ac[256];
	byte qtab_c[64];
	byte qtab_s[64];
};

void TKuAC_WriteBits(TKuAC_EncState *ctx, int val, int bits)
{
	ctx->win|=(val&((1U<<bits)-1))<<ctx->pos;
	ctx->pos+=bits;
	while(ctx->pos>=8)
	{
		*ctx->ct++=ctx->win;
		ctx->win>>=8;
		ctx->pos-=8;
	}
}

void TKuAC_WriteFlushBits(TKuAC_EncState *ctx)
{
	int p;

	p=ctx->pos;
	while(p>0)
	{
		*ctx->ct++=ctx->win;
		ctx->win>>=8;
		p-=8;
	}
	ctx->pos=0;
}

void TKuAC_WriteRice(TKuAC_EncState *ctx, int val, int kf)
{
	int q;
	
	q=val>>kf;
	if(q>=8)
	{
		TKuAC_WriteBits(ctx, 0xFF, 8);
		TKuAC_WriteBits(ctx, val, 8);
		return;
	}

	TKuAC_WriteBits(ctx, (1<<q)-1, q+1);
	TKuAC_WriteBits(ctx, val&((1<<kf)-1), kf);
}

void TKuAC_WriteAdRice(TKuAC_EncState *ctx, int val, byte *rkf)
{
	int q, kf;
	
	kf=*rkf;
	q=val>>kf;

	if(q>=8)
	{
		kf++; *rkf=kf;
		TKuAC_WriteBits(ctx, 0xFF, 8);
		TKuAC_WriteBits(ctx, val, 8);
		return;
	}

	TKuAC_WriteBits(ctx, (1<<q)-1, q+1);
	TKuAC_WriteBits(ctx, val&((1<<kf)-1), kf);
	if(!q && kf)
		{ kf--; *rkf=kf; }
	else if(q>1)
		{ kf++; *rkf=kf; }
}

void TKuAC_WriteAdRiceSTF(TKuAC_EncState *ctx, int val,
	byte *rkf, byte *stfvtab, byte *stfitab)
{
	int i0, i1, v0, v1;
	
	i0=stfitab[val];
//	i1=(i0*7)>>3;
	i1=(i0*15)>>4;
	v0=stfvtab[i0];	v1=stfvtab[i1];
	stfvtab[i0]=v1;	stfvtab[i1]=v0;
	stfitab[v1]=i0;	stfitab[v0]=i1;
	
	TKuAC_WriteAdRice(ctx, i0, rkf);
}

void TKuAC_WriteCoeff(TKuAC_EncState *ctx,
	int v, int z,
	byte *rkf, byte *stfvtab, byte *stfitab)
{
	int av, tz, p, b, b1, h;
	
	tz=z;
	while(tz>=8)
	{
		TKuAC_WriteAdRiceSTF(ctx, 0xE0, rkf, stfvtab, stfitab);
		tz-=8;
	}
	
	if(v<(-32767))
		v=-32767;
	if(v>( 32767))
		v= 32767;
	
	
	av=(v<<1)^(v>>31);
	
	if(av<4)
	{
		p=(tz<<5)|av;
		TKuAC_WriteAdRiceSTF(ctx, p, rkf, stfvtab, stfitab);
		return;
	}
	
	b=4;
	b1=b+1;
	h=(b1>>1)-1;
	p=(((2|b1)&3)<<h);
	while(p<=av)
	{
		b++;
		b1=b+1;
		h=(b1>>1)-1;
		p=(((2|b1)&3)<<h);
	}

	h=(b>>1)-1;
	p=(tz<<5)|b;
	TKuAC_WriteAdRiceSTF(ctx, p, rkf, stfvtab, stfitab);
	TKuAC_WriteBits(ctx, av&((1<<h)-1), h);
}

void TKuAC_WriteCoeffDc(TKuAC_EncState *ctx,
	int val)
{
	TKuAC_WriteCoeff(ctx, val, 0,
		&(ctx->pk_dc), ctx->cpt_dc, ctx->cpi_dc);
}

void TKuAC_WriteCoeffAc(TKuAC_EncState *ctx,
	int val, int zp)
{
	TKuAC_WriteCoeff(ctx, val, zp,
		&(ctx->pk_ac), ctx->cpt_ac, ctx->cpi_ac);
}

void TKuAC_WriteCoeffTag(TKuAC_EncState *ctx,
	int val, int zp)
{
	TKuAC_WriteCoeff(ctx, val, zp,
		&(ctx->pk_dc), ctx->cpt_dc, ctx->cpi_dc);
}

static const byte tkuac_zigzag[64]={
 0,  1,  5,  6, 14, 15, 27, 28,
 2,  4,  7, 13, 16, 26, 29, 42,
 3,  8, 12, 17, 25, 30, 41, 43,
 9, 11, 18, 24, 31, 40, 44, 53,
10, 19, 23, 32, 39, 45, 52, 54,
20, 22, 33, 38, 46, 51, 55, 60,
21, 34, 37, 47, 50, 56, 59, 61,
35, 36, 48, 49, 57, 58, 62, 63
};

int TKuAC_CalcQuantTrelDc(int dv, int q)
{
	int v0, v1, v2, v3;
	
	if(q==1)
		return(dv);
	
	if(dv<0)
	{
		return(-TKuAC_CalcQuantTrelDc(-dv, q));
	}
	
	v0=dv/q;
	v1=(dv+(q>>1))/q;

//	if((v0==0) || (v0==1) || (v0==-1))
//		return(0);
	if(v0==v1)
		return(v0);
	
	v2=(v0<<1)^(v0>>31);
	v3=(v1<<1)^(v1>>31);
	while((v2&v3)>=2)
		{ v2>>=1; v3>>=1; }
	if(v2==v3)
		return(v1);
	return(v0);
}

int TKuAC_CalcQuantTrel(int dv, int q)
{
	int v;
	
	v=TKuAC_CalcQuantTrelDc(dv, q);
	if((v==1) || (v==-1))
		return(0);
	return(v);
}

void TKuAC_WriteBlockCoeffs(TKuAC_EncState *ctx,
	s32 *blk, byte *qtab, s32 *rpdc)
{
	int pdc, q, v, z;
	int i, j, k;
	
	pdc=*rpdc;
	v=blk[0];
//	v=(v-pdc)/qtab[0];
	q=(qtab[0]*ctx->qfxsc)>>TKUAC_QFX_SHL;
	if(q<1)	q=1;
//	v=TKuAC_CalcQuantTrel(v-pdc, (qtab[0]*ctx->qfxsc)>>TKUAC_QFX_SHL);
	v=TKuAC_CalcQuantTrelDc(v-pdc, q);
	TKuAC_WriteCoeffDc(ctx, v);
	*rpdc=pdc+(v*qtab[0]);
	
//	printf("TKuAC_WriteBlockCoeffs: pdc=%d v=%d\n", pdc, v);
	
	z=0;
	for(i=1; i<64; i++)
	{
//		j=tkuac_zigzag[i];
		j=tkuac_izigzag[i];
//		v=blk[j]/qtab[j];

		q=(qtab[j]*ctx->qfxsc)>>TKUAC_QFX_SHL;
		if(q<1)	q=1;

		v=TKuAC_CalcQuantTrel(blk[j], q);
		
//		if(((v==1) || (v==-1)) && (qtab[j]>=5))
//			v=0;
		
		if(!v)
		{
			z++;
			continue;
		}
		TKuAC_WriteCoeffAc(ctx, v, z);
		z=0;
	}
	
	if(z)
	{
//		TKuAC_WriteCoeffAc(ctx, 0, z);
		TKuAC_WriteCoeffAc(ctx, 0, 0);
	}
}

void TKuAC_TransBH_Horz(s32 *oblk, s32 *iblk)
{
	s32 i0, i1, i2, i3, i4, i5, i6, i7;
	s32 s0, s1;
	s32 t0, t1, t2, t3;
	s32 u0, u1, u2, u3, u4, u5, u6, u7;
	
	i0=iblk[0];		i1=iblk[1];		i2=iblk[2];		i3=iblk[3];
	i4=iblk[4];		i5=iblk[5];		i6=iblk[6];		i7=iblk[7];

	u0=(i0+i1)>>1;
	u1=(i2+i3)>>1;
	u2=(i4+i5)>>1;
	u3=(i6+i7)>>1;
	u4=i0-i1;
	u5=i2-i3;
	u6=i4-i5;
	u7=i6-i7;

	t0=(u0+u1)>>1;
	t1=(u2+u3)>>1;
	t2=u0-u1;
	t3=u2-u3;
	
	s0=(t0+t1)>>1;
	s1=t0-t1;
	
	oblk[0]=s0;		oblk[1]=s1;		oblk[2]=t2;		oblk[3]=t3;
	oblk[4]=u4;		oblk[5]=u5;		oblk[6]=u6;		oblk[7]=u7;
}

void TKuAC_TransBH_Vert(s32 *oblk, s32 *iblk)
{
	s32 i0, i1, i2, i3, i4, i5, i6, i7;
	s32 s0, s1;
	s32 t0, t1, t2, t3;
	s32 u0, u1, u2, u3, u4, u5, u6, u7;
	
	i0=iblk[ 0];	i1=iblk[ 8];	i2=iblk[16];	i3=iblk[24];
	i4=iblk[32];	i5=iblk[40];	i6=iblk[48];	i7=iblk[56];

	u0=(i0+i1)>>1;
	u1=(i2+i3)>>1;
	u2=(i4+i5)>>1;
	u3=(i6+i7)>>1;
	u4=i0-i1;
	u5=i2-i3;
	u6=i4-i5;
	u7=i6-i7;

	t0=(u0+u1)>>1;
	t1=(u2+u3)>>1;
	t2=u0-u1;
	t3=u2-u3;
	
	s0=(t0+t1)>>1;
	s1=t0-t1;
	
	oblk[ 0]=s0;	oblk[ 8]=s1;	oblk[16]=t2;	oblk[24]=t3;
	oblk[32]=u4;	oblk[40]=u5;	oblk[48]=u6;	oblk[56]=u7;
}

void TKuAC_TransBH(s32 *oblk, s32 *iblk)
{
	s32 tblk[64];

	TKuAC_TransBH_Horz(tblk+ 0, iblk+ 0);
	TKuAC_TransBH_Horz(tblk+ 8, iblk+ 8);
	TKuAC_TransBH_Horz(tblk+16, iblk+16);
	TKuAC_TransBH_Horz(tblk+24, iblk+24);
	TKuAC_TransBH_Horz(tblk+32, iblk+32);
	TKuAC_TransBH_Horz(tblk+40, iblk+40);
	TKuAC_TransBH_Horz(tblk+48, iblk+48);
	TKuAC_TransBH_Horz(tblk+56, iblk+56);

	TKuAC_TransBH_Vert(oblk+ 0, tblk+ 0);
	TKuAC_TransBH_Vert(oblk+ 1, tblk+ 1);
	TKuAC_TransBH_Vert(oblk+ 2, tblk+ 2);
	TKuAC_TransBH_Vert(oblk+ 3, tblk+ 3);
	TKuAC_TransBH_Vert(oblk+ 4, tblk+ 4);
	TKuAC_TransBH_Vert(oblk+ 5, tblk+ 5);
	TKuAC_TransBH_Vert(oblk+ 6, tblk+ 6);
	TKuAC_TransBH_Vert(oblk+ 7, tblk+ 7);
}

int TKuAC_WriteCoeffSbpC(TKuAC_EncState *ctx,
	int val)
{
	int v, p, q;

	p=ctx->pdsbp_c;
	q=ctx->qfxsbp;
//	v=TKuAC_CalcQuantTrel(val-p, q);
	v=TKuAC_CalcQuantTrelDc(val-p, q);
	TKuAC_WriteCoeffDc(ctx, v);
	p=p+v*q;
	ctx->pdsbp_c=p;
	return(p);
}

int TKuAC_WriteCoeffSbpS(TKuAC_EncState *ctx,
	int val)
{
	int v, p, q;

	p=ctx->pdsbp_s;
	q=ctx->qfxsbp;
//	v=TKuAC_CalcQuantTrel(val-p, q);
	v=TKuAC_CalcQuantTrelDc(val-p, q);
	TKuAC_WriteCoeffDc(ctx, v);
	p=p+v*q;
	ctx->pdsbp_s=p;
	return(p);
}

void TKuAC_EncodeLastPredictors(TKuAC_EncState *ctx)
{
	int dca[4];
	int qf;
	
//	return;
	
	qf=16;

	ctx->qfxsbp=(ctx->qtab_c[0]*qf)>>TKUAC_QFX_SHL;
	if(ctx->qfxsbp<1)
		ctx->qfxsbp=1;

//	TKuAC_WriteCoeffTag(ctx, qf, 2);
	TKuAC_WriteCoeffTag(ctx, qf, 1);
	dca[0]=TKuAC_WriteCoeffSbpC(ctx, ctx->l_pdc_c);
	dca[1]=TKuAC_WriteCoeffSbpC(ctx, ctx->l_pdc_s);
	dca[2]=TKuAC_WriteCoeffSbpC(ctx, ctx->l_pdsbp_c);
	dca[3]=TKuAC_WriteCoeffSbpC(ctx, ctx->l_pdsbp_s);
	ctx->pdc_c=dca[0];
	ctx->pdc_s=dca[1];
	ctx->pdsbp_c=dca[2];
	ctx->pdsbp_s=dca[3];
}

void TKuAC_EncodeSbParm(TKuAC_EncState *ctx, s32 *sbpc, s32 *sbps)
{
	static int st_1p, st_1n, st_2n;

	int sbr0, sbr1, sbp0, sbp1, sbd0, sbd1, sbd2, sbd3;
	int qf, qfn;

#if 1
	qfn=0;
	qf=ctx->qfxsc;
	qf=(qf*7)/8;

//	qf*=2;
//	qf/=2;
//	qf/=4;
//	qf/=8;
	ctx->qfxsbp=(ctx->qtab_c[0]*qf)>>TKUAC_QFX_SHL;
	if(ctx->qfxsbp<1)
		ctx->qfxsbp=1;


	sbr0=sbpc[0];
	sbr1=sbpc[6];
#ifdef TKUAC_QSPL_LIN
	sbp0=((2*sbpc[1])-sbpc[2]);
	sbp1=((2*sbpc[5])-sbpc[4]);
#else
	sbp0=((3*sbpc[1])-sbpc[2])/2;
	sbp1=((3*sbpc[5])-sbpc[4])/2;
#endif

	sbd0=sbr0-sbp0;
	sbd1=sbr1-sbp1;
	sbd0=sbd0^(sbd0>>31);
	sbd1=sbd1^(sbd1>>31);
	
	if(sbd0>(8*ctx->qfxsbp))
		qfn=1;
	if(sbd1>(8*ctx->qfxsbp))
		qfn=1;

//	qfn=1;

	sbd0=sbpc[0]-ctx->sbparm_c[4];
	sbd1=sbpc[1]-ctx->sbparm_c[5];
	sbd1=sbpc[2]-ctx->sbparm_c[6];
	sbd3=sbps[0]-ctx->sbparm_s[1];
	sbd0=sbd0^(sbd0>>31);
	sbd1=sbd1^(sbd1>>31);
	sbd2=sbd2^(sbd2>>31);
	sbd3=sbd3^(sbd3>>31);

	if((ctx->sbparm_c[4]|ctx->sbparm_c[5]|ctx->sbparm_c[6])==0)
	{
		sbd0=sbd0<<3;
		sbd1=sbd1<<3;
		sbd2=sbd2<<3;
		sbd3=sbd3<<3;
	}

	if(	(sbd0<(2*ctx->qfxsbp)) &&
		(sbd1<(2*ctx->qfxsbp)) &&
		(sbd2<(2*ctx->qfxsbp)) &&
		(sbd3<(2*ctx->qfxsbp)))
	{
//		printf("TKuAC_EncodeSbParm: 2N %d\n", st_2n);
//		st_2n++;
	
		TKuAC_WriteCoeffTag(ctx, -qf, 2);

		ctx->sbparm_c[0]=ctx->sbparm_c[4];
		ctx->sbparm_c[1]=ctx->sbparm_c[5];
		ctx->sbparm_c[2]=ctx->sbparm_c[6];
		ctx->sbparm_s[0]=ctx->sbparm_s[1];

//		ctx->sbparm_c[2]=TKuAC_WriteCoeffSbpC(ctx, sbpc[2]);
		ctx->sbparm_c[3]=TKuAC_WriteCoeffSbpC(ctx, sbpc[3]);
		ctx->sbparm_c[4]=TKuAC_WriteCoeffSbpC(ctx, sbpc[4]);
		ctx->sbparm_c[5]=TKuAC_WriteCoeffSbpC(ctx, sbpc[5]);
		ctx->sbparm_c[6]=TKuAC_WriteCoeffSbpC(ctx, sbpc[6]);

		ctx->sbparm_s[1]=TKuAC_WriteCoeffSbpS(ctx, sbps[1]);
		return;
	}

//	TKuAC_WriteCoeffTag(ctx, qfn?(-qf):qf, 1);

	qfn=1;
	TKuAC_WriteCoeffTag(ctx, qf, 2);

	if(qfn)
		ctx->sbparm_c[0]=TKuAC_WriteCoeffSbpC(ctx, sbpc[0]);
	ctx->sbparm_c[1]=TKuAC_WriteCoeffSbpC(ctx, sbpc[1]);
	ctx->sbparm_c[2]=TKuAC_WriteCoeffSbpC(ctx, sbpc[2]);
	ctx->sbparm_c[3]=TKuAC_WriteCoeffSbpC(ctx, sbpc[3]);
	ctx->sbparm_c[4]=TKuAC_WriteCoeffSbpC(ctx, sbpc[4]);
	ctx->sbparm_c[5]=TKuAC_WriteCoeffSbpC(ctx, sbpc[5]);

	if(qfn)
	{
//		printf("TKuAC_EncodeSbParm: 1N %d\n", st_1n);
//		st_1n++;

//		ctx->sbparm_c[0]=TKuAC_WriteCoeffSbpC(ctx, sbpc[0]);
		ctx->sbparm_c[6]=TKuAC_WriteCoeffSbpC(ctx, sbpc[6]);
	}else
	{
//		printf("TKuAC_EncodeSbParm: 1P %d\n", st_1p);
//		st_1p++;

#ifdef TKUAC_QSPL_LIN
		ctx->sbparm_c[0]=((2*ctx->sbparm_c[1])-ctx->sbparm_c[2]);
		ctx->sbparm_c[6]=((2*ctx->sbparm_c[5])-ctx->sbparm_c[4]);
#else
		ctx->sbparm_c[0]=((3*ctx->sbparm_c[1])-ctx->sbparm_c[2])/2;
		ctx->sbparm_c[6]=((3*ctx->sbparm_c[5])-ctx->sbparm_c[4])/2;
#endif
	}

	ctx->sbparm_s[0]=TKuAC_WriteCoeffSbpS(ctx, sbps[0]);
	ctx->sbparm_s[1]=TKuAC_WriteCoeffSbpS(ctx, sbps[1]);
#endif
}

void TKuAC_EncodeSampleSubBlock(TKuAC_EncState *ctx, s16 *sblk)
{
	s32 dct0_c[10];
	s32 dct0_s[5];
	s32 blk_c[256];
	s32 blk_s[64];
	s16 *scs;
	s32 *sbpc;
	int ctc0, ctc1, ctc2, ctc3;
	int cts0, cts1, cts2, cts3;
	int dc0, dc1, dcs0, dcs1;
	int cts, dccl, dcsl;
	int i, j, k, l, sbo;
	
	sbo=(sblk-ctx->sbufs)>>1;
	for(i=0; i<7; i++)
	{
		l=0;
		for(j=0; j<64; j++)
		{
			k=((i-1)*64+j)-32;
			if((k+sbo)<0)
				continue;
			
			l+=(sblk[k*2+0]+sblk[k*2+1])/2;
		}
		dct0_c[i]=l/64;
	}

	for(i=0; i<2; i++)
	{
		l=0;
		for(j=0; j<256; j++)
		{
			k=(i*256+j)-128;
			if((k+sbo)<0)
				continue;
			
			l+=sblk[k*2+0]-sblk[k*2+1];
		}
		dct0_s[i]=l/256;
	}

	TKuAC_EncodeSbParm(ctx, dct0_c, dct0_s);

	dcs0=dct0_s[0];
	dcs1=dct0_s[1];
	sbpc=dct0_c;

//	dcs0=ctx->sbparm_s[0];
//	dcs1=ctx->sbparm_s[1];
//	sbpc=ctx->sbparm_c;

	scs=sblk;
	for(i=0; i<64; i++)
	{
		ctc0=sbpc[(i>>4)+0];
		ctc1=sbpc[(i>>4)+1];
		ctc2=sbpc[(i>>4)+2];
		ctc3=sbpc[(i>>4)+3];
#ifdef TKUAC_QSPL_HLINI
		k=(i&15); l=15-k;
#else
		k=(i&15)*2+1; l=32-k;
#endif
		dc0=((ctc1*(32+k))-(ctc0*k))>>5;
		dc1=((ctc2*(32+l))-(ctc3*l))>>5;

//		dc0=dct0_c[(i>>4)+1];
//		dc1=dct0_c[(i>>4)+2];

#ifdef TKUAC_QSPL_LINI
		dc0=sbpc[(i>>4)+1];
		dc1=sbpc[(i>>4)+2];
#endif

		k=(i&15)*2+1;
		dccl=((dc0*(32-k))+(dc1*k))>>5;

		k=i*2+1;
		dcsl=((dcs0*(128-k))+(dcs1*k))>>7;
	
		ctc0=(scs[0]+scs[1])/2; cts0=(scs[0]-scs[1]);
		ctc1=(scs[2]+scs[3])/2; cts1=(scs[2]-scs[3]);
		ctc2=(scs[4]+scs[5])/2; cts2=(scs[4]-scs[5]);
		ctc3=(scs[6]+scs[7])/2; cts3=(scs[6]-scs[7]);
		cts=(cts0+cts1+cts2+cts3)/4;
		scs+=8;
		
		ctc0-=dccl;
		ctc1-=dccl;
		ctc2-=dccl;
		ctc3-=dccl;
		cts-=dcsl;
		
		blk_c[i*4+0]=ctc0;	blk_c[i*4+1]=ctc1;
		blk_c[i*4+2]=ctc2;	blk_c[i*4+3]=ctc3;
		blk_s[i]=cts;
	}

#if 0
	for(k=0; k<4; k++)
	{
		l=0;
		for(i=255; i>=0; i--)
		{
			j=blk_c[i];
	//		l=((l*31)+blk_c[i])>>5;
	//		l=((l*63)+blk_c[i])>>6;
	//		l=((l*47)+j)/48;
//			l=((l*31)+j)/32;
			l=((l*63)+j)/64;
		}
		for(i=0; i<256; i++)
		{
			j=blk_c[i];
			blk_c[i]=j-l;
			l=((l*63)+j)/64;
//			l=((l*31)+j)>>5;
	//		l=((l*63)+j)>>6;
	//		l=((l*47)+j)/48;
		}
	}
#endif

	TKuAC_TransBH(blk_c+  0, blk_c+  0);
	TKuAC_TransBH(blk_c+ 64, blk_c+ 64);
	TKuAC_TransBH(blk_c+128, blk_c+128);
	TKuAC_TransBH(blk_c+192, blk_c+192);
	TKuAC_TransBH(blk_s+  0, blk_s+  0);
	
//	TKuAC_WriteCoeffTag(ctx, ctx->qfxsc, 0);
	TKuAC_WriteCoeffTag(ctx, -ctx->qfxsc, 1);
	TKuAC_WriteBlockCoeffs(ctx, blk_c+  0, ctx->qtab_c, &(ctx->pdc_c));
	TKuAC_WriteBlockCoeffs(ctx, blk_c+ 64, ctx->qtab_c, &(ctx->pdc_c));
	TKuAC_WriteBlockCoeffs(ctx, blk_c+128, ctx->qtab_c, &(ctx->pdc_c));
	TKuAC_WriteBlockCoeffs(ctx, blk_c+192, ctx->qtab_c, &(ctx->pdc_c));
	TKuAC_WriteBlockCoeffs(ctx, blk_s    , ctx->qtab_s, &(ctx->pdc_s));
}


void TKuAC_EncodeStartBitstream(TKuAC_EncState *ctx,
	byte *ct)
{
	int i, j, k, l;

	for(i=0; i<256; i++)
	{
		ctx->cpt_dc[i]=i;
		ctx->cpi_dc[i]=i;

		ctx->cpt_ac[i]=i;
		ctx->cpi_ac[i]=i;
	}

#if 1
	for(i=0; i<8; i++)
		for(j=0; j<32; j++)
	{
		k=tkuac_initperm_sprz3[i]|tkuac_initperm_sprr5[j];
		l=(i<<5)|j;
		ctx->cpt_ac[k]=l;
		ctx->cpi_ac[l]=k;
	}
#endif

	ctx->pk_dc=4;
	ctx->pk_ac=4;

	ctx->pdc_c=0;
	ctx->pdc_s=0;

	ctx->pdsbp_c=0;
	ctx->pdsbp_s=0;

	ctx->sbparm_c[0]=0;
	ctx->sbparm_c[1]=0;
	ctx->sbparm_c[2]=0;
	ctx->sbparm_c[3]=0;
	ctx->sbparm_c[4]=0;
	ctx->sbparm_c[5]=0;
	ctx->sbparm_c[6]=0;
	ctx->sbparm_c[7]=0;

	ctx->sbparm_s[0]=0;
	ctx->sbparm_s[1]=0;
	
	ctx->ct=ct;
}

int TKuAC_EncodeStreamBlockI(TKuAC_EncState *ctx,
	byte **rctblk, s16 **rcsblk)
{
	s16 *cs;
	byte *ct1, *ct0;
	int i, j, k, l, nl;
	
	cs=*rcsblk;
	ct0=*rctblk;
	ct0[0]='A';
	ct0[1]='B';
	TKuAC_EncodeStartBitstream(ctx, ct0+4);
	
//	ctx->qfxsc=1024;

	TKuAC_EncodeLastPredictors(ctx);
	
//	TKuAC_EncodeSampleSubBlock(ctx, cs+256*0);
//	TKuAC_EncodeSampleSubBlock(ctx, cs+256*2);
//	TKuAC_EncodeSampleSubBlock(ctx, cs+256*4);
//	TKuAC_EncodeSampleSubBlock(ctx, cs+256*6);

	for(i=0; i<ctx->tgtbnum; i++)
	{
		TKuAC_EncodeSampleSubBlock(ctx, cs);
		cs+=2*256;
	}

//	TKuAC_WriteCoeffTag(ctx, 0, 0);
	TKuAC_WriteCoeffTag(ctx, 0, 1);
	TKuAC_WriteFlushBits(ctx);

	ctx->l_pdc_c=ctx->pdc_c;
	ctx->l_pdc_s=ctx->pdc_s;
	ctx->l_pdsbp_c=ctx->pdsbp_c;
	ctx->l_pdsbp_s=ctx->pdsbp_s;

//	*rcsblk=cs+256*8;
	*rcsblk=cs;
	
	ct1=ctx->ct;
	l=ct1-ct0;
	nl=~l;
	ct0[2]=(nl>>0)&255;
	ct0[3]=(nl>>8)&255;

	*rctblk=ct1;
	return(l);
}

void TKuAC_EncodeStreamBlock(TKuAC_EncState *ctx,
	byte **rctblk, s16 **rcsblk)
{
	static int qainh;
	byte *rct0;
	s16 *rcs0;
	int lpdc0, lpdc1, lpdc2, lpdc3;
	int l, q0, q1;
	
	rct0=*rctblk;
	rcs0=*rcsblk;

	lpdc0=ctx->l_pdc_c;		lpdc1=ctx->l_pdc_s;
	lpdc2=ctx->l_pdsbp_c;	lpdc3=ctx->l_pdsbp_s;

	l=TKuAC_EncodeStreamBlockI(ctx, rctblk, rcsblk);

	if((l<(ctx->tgtbsz*0.5)) && (ctx->qfxsc>TKUAC_QFXSC_MIN) && !(qainh&1))
	{
		*rctblk=rct0;	*rcsblk=rcs0;
		ctx->l_pdc_c=lpdc0;		ctx->l_pdc_s=lpdc1;
		ctx->l_pdsbp_c=lpdc2;	ctx->l_pdsbp_s=lpdc3;

		ctx->qfxsc=ctx->qfxsc*(1.0/1.8);
		if(ctx->qfxsc<TKUAC_QFXSC_MIN)
			ctx->qfxsc=TKUAC_QFXSC_MIN;
		l=TKuAC_EncodeStreamBlockI(ctx, rctblk, rcsblk);
		qainh|=2;
		return;
	}

	if((l>(ctx->tgtbsz*1.5)) && (ctx->qfxsc<TKUAC_QFXSC_MAX) && !(qainh&2))
	{
		*rctblk=rct0;	*rcsblk=rcs0;
		ctx->l_pdc_c=lpdc0;		ctx->l_pdc_s=lpdc1;
		ctx->l_pdsbp_c=lpdc2;	ctx->l_pdsbp_s=lpdc3;
		ctx->qfxsc=ctx->qfxsc*1.8;
		l=TKuAC_EncodeStreamBlockI(ctx, rctblk, rcsblk);
		qainh|=1;
		return;
	}

	qainh&=~0x03;

	if((l<(ctx->tgtbsz*0.75)) && (ctx->qfxsc>TKUAC_QFXSC_MIN) && !(qainh&4))
	{
		*rctblk=rct0;	*rcsblk=rcs0;
		ctx->l_pdc_c=lpdc0;		ctx->l_pdc_s=lpdc1;
		ctx->l_pdsbp_c=lpdc2;	ctx->l_pdsbp_s=lpdc3;
		ctx->qfxsc=ctx->qfxsc*(1.0/1.2);
		if(ctx->qfxsc<TKUAC_QFXSC_MIN)
			ctx->qfxsc=TKUAC_QFXSC_MIN;
		l=TKuAC_EncodeStreamBlockI(ctx, rctblk, rcsblk);
		qainh|=8;
		return;
	}

	if((l>(ctx->tgtbsz*1.25)) && (ctx->qfxsc<TKUAC_QFXSC_MAX) && !(qainh&8))
	{
		*rctblk=rct0;	*rcsblk=rcs0;
		ctx->l_pdc_c=lpdc0;		ctx->l_pdc_s=lpdc1;
		ctx->l_pdsbp_c=lpdc2;	ctx->l_pdsbp_s=lpdc3;

		ctx->qfxsc=ctx->qfxsc*1.2;
		l=TKuAC_EncodeStreamBlockI(ctx, rctblk, rcsblk);
		qainh|=4;
		return;
	}

	qainh&=~0x0C;

	if((l<(ctx->tgtbsz*0.9)) && (ctx->qfxsc>TKUAC_QFXSC_MIN))
	{
		*rctblk=rct0;
		*rcsblk=rcs0;
		ctx->l_pdc_c=lpdc0;		ctx->l_pdc_s=lpdc1;
		ctx->l_pdsbp_c=lpdc2;	ctx->l_pdsbp_s=lpdc3;

		q0=ctx->qfxsc;
		q1=q0*(1.0/1.05);
		if(q1==q0)
			q1=q0-1;
		ctx->qfxsc=q1;

		if(ctx->qfxsc<TKUAC_QFXSC_MIN)
			ctx->qfxsc=TKUAC_QFXSC_MIN;
		
		l=TKuAC_EncodeStreamBlockI(ctx, rctblk, rcsblk);
		return;
	}

	if((l>(ctx->tgtbsz*1.1)) && (ctx->qfxsc<TKUAC_QFXSC_MAX))
	{
		*rctblk=rct0;
		*rcsblk=rcs0;
		ctx->l_pdc_c=lpdc0;		ctx->l_pdc_s=lpdc1;
		ctx->l_pdsbp_c=lpdc2;	ctx->l_pdsbp_s=lpdc3;

		q0=ctx->qfxsc;
		q1=q0*1.05;
		if(q1==q0)
			q1=q0+1;
		ctx->qfxsc=q1;

		l=TKuAC_EncodeStreamBlockI(ctx, rctblk, rcsblk);
		return;
	}

	if((l<(ctx->tgtbsz*0.95)) && (ctx->qfxsc>TKUAC_QFXSC_MIN))
	{
		*rctblk=rct0;
		*rcsblk=rcs0;
		ctx->l_pdc_c=lpdc0;		ctx->l_pdc_s=lpdc1;
		ctx->l_pdsbp_c=lpdc2;	ctx->l_pdsbp_s=lpdc3;

		q0=ctx->qfxsc;
		q1=q0*(1.0/1.02);
		if(q1==q0)
			q1=q0-1;
		ctx->qfxsc=q1;

		if(ctx->qfxsc<TKUAC_QFXSC_MIN)
			ctx->qfxsc=TKUAC_QFXSC_MIN;
		
		l=TKuAC_EncodeStreamBlockI(ctx, rctblk, rcsblk);
		return;
	}

	if((l>(ctx->tgtbsz*1.05)) && (ctx->qfxsc<TKUAC_QFXSC_MAX))
	{
		*rctblk=rct0;
		*rcsblk=rcs0;
		ctx->l_pdc_c=lpdc0;		ctx->l_pdc_s=lpdc1;
		ctx->l_pdsbp_c=lpdc2;	ctx->l_pdsbp_s=lpdc3;

		q0=ctx->qfxsc;
		q1=q0*1.02;
		if(q1==q0)
			q1=q0+1;
		ctx->qfxsc=q1;

		l=TKuAC_EncodeStreamBlockI(ctx, rctblk, rcsblk);
		return;
	}
}

void TKuAC_EmitSyncFcc(byte **rct, u32 fcc)
{
	byte *ct;

	ct=*rct;
	ct[0]=  (fcc>> 0) &255;
	ct[1]=(~(fcc>> 0))&255;
	ct[2]=  (fcc>> 8) &255;
	ct[3]=(~(fcc>> 8))&255;
	ct[4]=  (fcc>>16) &255;
	ct[5]=(~(fcc>>16))&255;
	ct[6]=  (fcc>>24) &255;
	ct[7]=(~(fcc>>24))&255;
	*rct=ct+8;
}

void TKuAC_EmitTagTcc(byte **rct, int tcc, int len, byte *buf)
{
	byte *ct;
	int nl;
	
	nl=~(len+4);
	ct=*rct;
	ct[0]=(tcc>>0)&255;
	ct[1]=(tcc>>8)&255;
	ct[2]=(nl>>0)&255;
	ct[3]=(nl>>8)&255;
	if(buf)
		{ memcpy(ct+4, buf, len); }
	else
		{ memset(ct+4, 0, len); }
	*rct=ct+len+4;
}

void TKuAC_EncodeStreamParmHead(TKuAC_EncState *ctx,
	byte **rctblk, int qfl)
{
	byte tb[256];
	byte *ct;
	float qsc;
	int i, j, k, l, am, hz;
	int cx, cy;

	if((qfl&255)>=200)
	{
		am=0;
		for(i=0; i<64; i++)
		{
			ctx->qtab_c[i]=1;
			ctx->qtab_s[i]=1;
		}
	}else
	{
//		k=100-(qfl&127);
		k=100-5;
//		qsc=k*0.25;
		qsc=k*0.90;

		for(cy=0; cy<8; cy++)
		{
			for(cx=0; cx<8; cx++)
			{
				k=1.0+pow(((cx+1)/4.0)*((cy+1)/4.0), 0.667)*qsc;
//				k=1.0+pow(((cx+1)/4.0)*((cy+1)/4.0), 0.750)*qsc;
				if(k>255)
					k=255;
				i=cy*8+cx;
				ctx->qtab_c[i]=k;
				ctx->qtab_s[i]=k;
				printf("%3d ", k);
			}
			printf("\n");
		}
	}
	
	hz=ctx->tgthz;
	
//	ctx->tgtbsz=((qfl&255)*(4096/8))/62.5;
	ctx->tgtbsz=(qfl&255)*((4096/8)/(hz/256.0));
	ctx->qfxsc=1024;
	ctx->tgtbnum=4;
	
//	while((ctx->tgtbsz<512) && (ctx->tgtbnum<32))
	while((ctx->tgtbsz<1024) && (ctx->tgtbnum<32))
	{
		ctx->tgtbnum*=2;
		ctx->tgtbsz*=2;
	}
	
	ct=*rctblk;
	TKuAC_EmitSyncFcc(&ct, TKUAC_FCC_tkua);

	memset(tb, 0, 64);
	tb[0]=(ctx->tgthz>> 0)&255;
	tb[1]=(ctx->tgthz>> 8)&255;
	tb[2]=(ctx->tgthz>>16)&255;

	tb[4]=(ctx->curspos>> 0)&255;
	tb[5]=(ctx->curspos>> 8)&255;
	tb[6]=(ctx->curspos>>16)&255;
	tb[7]=(ctx->curspos>>24)&255;
	tb[8]=(ctx->curspos>>32)&255;
	tb[9]=(ctx->curspos>>40)&255;

	TKuAC_EmitTagTcc(&ct, TKUAC_TCC_hp, 16, tb);

	TKuAC_EmitTagTcc(&ct, TKUAC_TCC_qc, 64, ctx->qtab_c);
	TKuAC_EmitTagTcc(&ct, TKUAC_TCC_qs, 64, ctx->qtab_s);
	
	*rctblk=ct;
}
