#define BTMGL_FOURCC(a, b, c, d)	((a)|((b)<<8)|((c)<<16)|((d)<<24))
#define BTMGL_TWOCC(a, b)			((a)|((b)<<8))

#define TKUAC_FCC_tkua	BTMGL_FOURCC('t', 'k', 'u', 'a')

#define TKUAC_TCC_hp	BTMGL_TWOCC('h', 'p')
#define TKUAC_TCC_qc	BTMGL_TWOCC('q', 'c')
#define TKUAC_TCC_qs	BTMGL_TWOCC('q', 's')

// #define TKUAC_QFX_SHL	6
// #define TKUAC_QFX_SHL	5
#define TKUAC_QFX_SHL	4

typedef struct TKuAC_DecState_s TKuAC_DecState;

struct TKuAC_DecState_s {
	byte *cs;
	u32 win;
	byte pos;
	byte status;
	
	byte pk_dc;
	byte pk_ac;
	
	int qfxsc;
	int qfxsbp;
	int tgthz;
	
	s32 pdc_c;
	s32 pdc_s;

	s32 pdsbp_c;
	s32 pdsbp_s;
	
	s32 sbparm_c[8];
	s32 sbparm_s[2];

	byte cpt_dc[256];
	byte cpt_ac[256];
	byte qtab_c[64];
	byte qtab_s[64];
};

#if 0
int TKuAC_PeekBits(TKuAC_DecState *ctx, int bits)
{
	return((ctx->win>>ctx->pos)&((1<<bits)-1));
}

void TKuAC_SkipBits(TKuAC_DecState *ctx, int bits)
{
	ctx->pos+=bits;
	while(ctx->pos>=8)
		{ ctx->win=(ctx->win>>8)|((*ctx->cs++)<<24); ctx->pos-=8; }
}

void TKuAC_SetupReadBits(TKuAC_DecState *ctx, byte *buf)
{
	ctx->cs=buf;
	ctx->pos=32;
	TKuAC_SkipBits(ctx, 0);
}
#endif

#if 1
int TKuAC_PeekBits(TKuAC_DecState *ctx, int bits)
{
	u32 v;
	v=*(u32 *)(ctx->cs);
	return((v>>ctx->pos)&((1<<bits)-1));
}

void TKuAC_SkipBits(TKuAC_DecState *ctx, int bits)
{
	int p;

	p=ctx->pos+bits;
	ctx->cs+=p>>3;
	ctx->pos=p&7;
}

void TKuAC_SetupReadBits(TKuAC_DecState *ctx, byte *buf)
{
	ctx->cs=buf;
	ctx->pos=0;
}
#endif

#if 0
int TKuAC_ReadBits(TKuAC_DecState *ctx, int bits)
{
	int b;
	int p;
	u32 v;

	p=ctx->pos;
	v=*(u32 *)(ctx->cs);
	b=((v>>p)&((1<<bits)-1));

	p+=bits;
	ctx->cs+=p>>3;
	ctx->pos=p&7;
	return(b);
}
#endif

#if 1
int TKuAC_ReadBits(TKuAC_DecState *ctx, int bits)
{
	int b;
	b=TKuAC_PeekBits(ctx, bits);
	TKuAC_SkipBits(ctx, bits);
	return(b);
}
#endif

static const byte tkuac_riceqtab[256]={
0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,
0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6,
0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,
0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 7,
0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,
0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6,
0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,
0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 8
};

static u16 tkuac_ricefasttab[8*256];

int TKuAC_InitRiceFastTab()
{
	int kf, k1, b, r, v, q, l;
	
	if(tkuac_ricefasttab[255]!=0)
		return(0);
	
	for(kf=0; kf<8; kf++)
	{
		for(b=0; b<256; b++)
		{
			q=tkuac_riceqtab[b];
			
			l=q+kf+1;
			if(l>8)
			{
				tkuac_ricefasttab[(kf<<8)|b]=0x000F;
				continue;
			}
			
			k1=kf;
			if((q==0) && (k1>0))k1--;
			if((q>1) && (k1<7))k1++;

			r=(b>>(q+1))&((1<<kf)-1);
			v=((q<<kf)|r);

			tkuac_ricefasttab[(kf<<8)|b]=l|(k1<<4)|(v<<8);
		}
	}
	return(0);
}

int TKuAC_ReadRice(TKuAC_DecState *ctx, int kf)
{
	int b, q, r;

	b=TKuAC_PeekBits(ctx, 16);
	q=tkuac_riceqtab[b&0xFF];
	if(q==8)
	{
		TKuAC_SkipBits(ctx, 16);
		return(b>>8);
	}
	r=(b>>(q+1))&((1<<kf)-1);
	TKuAC_SkipBits(ctx, q+kf+1);
	return((q<<kf)|r);
}

#if 1
int TKuAC_ReadAdRice(TKuAC_DecState *ctx, byte *rkf)
{
	int b, q, r, v, kf;

	kf=*rkf;
	b=TKuAC_PeekBits(ctx, 16);

#if 1
	r=(kf<<8)|(b&255);
	r=tkuac_ricefasttab[r];
	q=(r&15);
	if(q<=8)
	{
		v=r>>8;
		*rkf=(r>>4)&15;
		TKuAC_SkipBits(ctx, q);
		return(v);
	}
#endif
	
	q=tkuac_riceqtab[b&0xFF];
	if(q==8)
	{
		kf++; *rkf=kf;
		TKuAC_SkipBits(ctx, 16);
		return(b>>8);
	}
	r=(b>>(q+1))&((1<<kf)-1);
	TKuAC_SkipBits(ctx, q+kf+1);
	v=(q<<kf)|r;
	if(!q && kf)
		{ kf--; *rkf=kf; }
	else if(q>1)
		{ kf++; *rkf=kf; }
	return(v);
}
#endif

int TKuAC_ReadAdRiceSTF(TKuAC_DecState *ctx, byte *rkf, byte *stftab)
{
	int i0, i1, v0, v1;
	
	i0=TKuAC_ReadAdRice(ctx, rkf);
	i1=(i0*15)>>4;
	v0=stftab[i0];	v1=stftab[i1];
	stftab[i0]=v1;	stftab[i1]=v0;
	return(v0);
}

int TKuAC_ReadCoeffB(TKuAC_DecState *ctx, int b)
{
	int h, f, v;

	if(b<4)
	{
		v=(b>>1)^(((s32)(b<<31))>>31);
		return(v);
	}

	h=(b>>1)-1;
	v=(((2|b)&3)<<h)|TKuAC_ReadBits(ctx, h);

	v=(v>>1)^(((s32)(v<<31))>>31);
	return(v);
}

int TKuAC_ReadCoeffDc(TKuAC_DecState *ctx)
{
	int b, h, f, v;

	b=TKuAC_ReadAdRiceSTF(ctx, &(ctx->pk_dc), ctx->cpt_dc);
	v=TKuAC_ReadCoeffB(ctx, b&31);
	return(v);
}

int TKuAC_ReadCoeffAc(TKuAC_DecState *ctx, int *rv, int *rz)
{
	int b, h, f, v;

	b=TKuAC_ReadAdRiceSTF(ctx, &(ctx->pk_ac), ctx->cpt_ac);
	if(!b)
		return(1);
	v=TKuAC_ReadCoeffB(ctx, b&31);
	*rz=(b>>5)&7;
	*rv=v;
	return(0);
}

int TKuAC_ReadCoeffTag(TKuAC_DecState *ctx, int *rv, int *rz)
{
	int b, h, f, v;

	b=TKuAC_ReadAdRiceSTF(ctx, &(ctx->pk_dc), ctx->cpt_dc);
	if(!b)
	{
		*rz=0;
		*rv=0;
		return(1);
	}
	v=TKuAC_ReadCoeffB(ctx, b&31);
	*rz=(b>>5)&7;
	*rv=v;
	return(0);
}

int TKuAC_ReadCoeffSbpC(TKuAC_DecState *ctx)
{
	int b, h, f, v;

	v=TKuAC_ReadCoeffDc(ctx);
	v=ctx->pdsbp_c+(v*ctx->qfxsbp);
	ctx->pdsbp_c=v;
	return(v);
}

int TKuAC_ReadCoeffSbpS(TKuAC_DecState *ctx)
{
	int b, h, f, v;

	v=TKuAC_ReadCoeffDc(ctx);
	v=ctx->pdsbp_s+(v*ctx->qfxsbp);
	ctx->pdsbp_s=v;
	return(v);
}

static const byte tkuac_izigzag[64]={
 0,  1,  8, 16,  9,  2,  3, 10,
17, 24, 32, 25, 18, 11,  4,  5,
12, 19, 26, 33, 40, 48, 41, 34,
27, 20, 13,  6,  7, 14, 21, 28,
35, 42, 49, 56, 57, 50, 43, 36,
29, 22, 15, 23, 30, 37, 44, 51,
58, 59, 52, 45, 38, 31, 39, 46,
53, 60, 61, 54, 47, 55, 62, 63
};

int TKuAC_ReadBlockCoeffs(TKuAC_DecState *ctx, s32 *blk, byte *qtab, s32 *rpdc)
{
	int b, q, pdc;
	int i, j, k, z, v, z0;

	memset(blk, 0, 64*sizeof(s32));

	pdc=*rpdc;
	v=TKuAC_ReadCoeffDc(ctx);
	q=((qtab[0]*ctx->qfxsc)>>TKUAC_QFX_SHL);
	if(q<1)q=1;
	z=pdc+(v*q);
	blk[0]=z;
	*rpdc=z;

	i=1;
	while(i<64)
	{
		if(TKuAC_ReadCoeffAc(ctx, &v, &z))
			break;
		i+=z;
		if(i>=64)
			__debugbreak();
		j=tkuac_izigzag[i];
		q=((qtab[j]*ctx->qfxsc)>>TKUAC_QFX_SHL);
		if(q<1)q=1;
		blk[j]=v*q;
		i++;
	}
	return(0);
}

#if 1
void TKuAC_TransIBH(s32 *oblk, s32 *iblk)
{
	s32 tblk[64];
	s32 *cs, *ct;

	s32 i0, i1, i2, i3, i4, i5, i6, i7;
	s32 s0, s1;
	s32 t0, t1, t2, t3;
	s32 u0, u1, u2, u3, u4, u5, u6, u7;
	
	int i;
	
	cs=iblk; ct=tblk;
	for(i=0; i<8; i++)
	{
		i0=cs[ 0];	i1=cs[ 8];	i2=cs[16];	i3=cs[24];
		i4=cs[32];	i5=cs[40];	i6=cs[48];	i7=cs[56];
		s1=i0-(i1>>1);	s0=s1+i1;
		t1=s0-(i2>>1);	t0=t1+i2;	t3=s1-(i3>>1);	t2=t3+i3;
		u1=t0-(i4>>1);	u0=u1+i4;	u3=t1-(i5>>1);	u2=u3+i5;
		u5=t2-(i6>>1);	u4=u5+i6;	u7=t3-(i7>>1);	u6=u7+i7;
		ct[ 0]=u0;	ct[ 8]=u1;	ct[16]=u2;	ct[24]=u3;
		ct[32]=u4;	ct[40]=u5;	ct[48]=u6;	ct[56]=u7;
		cs++;	ct++;
	}

	cs=tblk; ct=oblk;
	for(i=0; i<8; i++)
	{
		i0=cs[0];	i1=cs[1];	i2=cs[2];	i3=cs[3];
		i4=cs[4];	i5=cs[5];	i6=cs[6];	i7=cs[7];
		s1=i0-(i1>>1);	s0=s1+i1;
		t1=s0-(i2>>1);	t0=t1+i2;	t3=s1-(i3>>1);	t2=t3+i3;
		u1=t0-(i4>>1);	u0=u1+i4;	u3=t1-(i5>>1);	u2=u3+i5;
		u5=t2-(i6>>1);	u4=u5+i6;	u7=t3-(i7>>1);	u6=u7+i7;
		ct[0]=u0;	ct[1]=u1;	ct[2]=u2;	ct[3]=u3;
		ct[4]=u4;	ct[5]=u5;	ct[6]=u6;	ct[7]=u7;
		cs+=8;	ct+=8;
	}
}
#endif

int tkuac_clamp16s(int vi)
{
	int v;
	v=vi;
	if(vi<-32768)	v=-32768;
	if(vi> 32767)	v= 32767;
	return(v);
}

/* rzrz-rzrr */
static const byte tkuac_initperm_sprz3[8]={
	0x00, 0x04, 0x10, 0x14, 0x40, 0x44, 0x50, 0x54 };
static const byte tkuac_initperm_sprr5[32]={
	0x00, 0x01, 0x02, 0x03, 0x08, 0x09, 0x0A, 0x0B,
	0x20, 0x21, 0x22, 0x23, 0x28, 0x29, 0x2A, 0x2B,
	0x80, 0x81, 0x82, 0x83, 0x88, 0x89, 0x8A, 0x8B,
	0xA0, 0xA1, 0xA2, 0xA3, 0xA8, 0xA9, 0xAA, 0xAB };

int TKuAC_DecodeSetupReadBitstream(TKuAC_DecState *ctx,
	byte *csbuf)
{
	int i, j, k;
	
	for(i=0; i<256; i++)
		ctx->cpt_dc[i]=i;

	for(i=0; i<8; i++)
		for(j=0; j<32; j++)
	{
		k=tkuac_initperm_sprz3[i]|tkuac_initperm_sprr5[j];
		ctx->cpt_ac[k]=(i<<5)|j;
	}
	
	TKuAC_InitRiceFastTab();
	
	ctx->pk_dc=4;
	ctx->pk_ac=4;

	ctx->pdc_c=0;
	ctx->pdc_s=0;

	ctx->pdsbp_c=0;
	ctx->pdsbp_s=0;

	TKuAC_SetupReadBits(ctx, csbuf);

	return(0);
}

void TKuAC_DecodeSampleSubBlock(TKuAC_DecState *ctx, s16 *sblk)
{
	s32 blk_c[256];
	s32 blk_s[64];
	s32 *sbpc;
	s16 *sct;
	int ctc0, ctc1, ctc2, ctc3;
	int cts0, cts1, cts2, cts3;
	int dc0, dc1, dcs0, dcs1;
	int cts, ctsh, ct_l, ct_r;
	int dccl, dcsl;
	int i, j, k, l;
	
	sct=sblk;

	TKuAC_ReadBlockCoeffs(ctx, blk_c+  0, ctx->qtab_c, &(ctx->pdc_c));
	TKuAC_ReadBlockCoeffs(ctx, blk_c+ 64, ctx->qtab_c, &(ctx->pdc_c));
	TKuAC_ReadBlockCoeffs(ctx, blk_c+128, ctx->qtab_c, &(ctx->pdc_c));
	TKuAC_ReadBlockCoeffs(ctx, blk_c+192, ctx->qtab_c, &(ctx->pdc_c));
	TKuAC_ReadBlockCoeffs(ctx, blk_s    , ctx->qtab_s, &(ctx->pdc_s));

	TKuAC_TransIBH(blk_c+  0, blk_c+  0);
	TKuAC_TransIBH(blk_c+ 64, blk_c+ 64);
	TKuAC_TransIBH(blk_c+128, blk_c+128);
	TKuAC_TransIBH(blk_c+192, blk_c+192);
	TKuAC_TransIBH(blk_s+  0, blk_s+  0);

	dcs0=ctx->sbparm_s[0];
	dcs1=ctx->sbparm_s[1];
	sbpc=ctx->sbparm_c;


	for(i=0; i<64; i++)
	{
		j=i>>4;
		ctc0=sbpc[j+0];		ctc1=sbpc[j+1];
		ctc2=sbpc[j+2];		ctc3=sbpc[j+3];
		k=(i&15); l=15-k;
		dc0=((ctc1*(32+k))-(ctc0*k))>>5;
		dc1=((ctc2*(32+l))-(ctc3*l))>>5;

		k=(i&15)*2+1;
		dccl=((dc0*(32-k))+(dc1*k))>>5;

		k=i*2+1;
		dcsl=((dcs0*(128-k))+(dcs1*k))>>7;
	
		ctc0=blk_c[i*4+0];
		ctc1=blk_c[i*4+1];
		ctc2=blk_c[i*4+2];
		ctc3=blk_c[i*4+3];
		cts=blk_s[i];

#if 1
		ctc0+=dccl;
		ctc1+=dccl;
		ctc2+=dccl;
		ctc3+=dccl;
		cts+=dcsl;
#endif

#if 0
		ctc0=dccl;
		ctc1=dccl;
		ctc2=dccl;
		ctc3=dccl;
		cts=dcsl;
#endif

		ctsh=cts/2;
		
#if 1
		cts0=ctc0+ctsh;		cts1=ctc0-ctsh;
		cts2=ctc1+ctsh;		cts3=ctc1-ctsh;
		k=(cts0+32768)|(cts1+32768)|(cts2+32768)|(cts3+32768);
		if(k&(~0xFFFF))
		{	cts0=tkuac_clamp16s(cts0);
			cts1=tkuac_clamp16s(cts1);
			cts2=tkuac_clamp16s(cts2);
			cts3=tkuac_clamp16s(cts3);	}
		sct[0]=cts0;	sct[1]=cts1;
		sct[2]=cts2;	sct[3]=cts3;

		cts0=ctc2+ctsh;		cts1=ctc2-ctsh;
		cts2=ctc3+ctsh;		cts3=ctc3-ctsh;
		k=(cts0+32768)|(cts1+32768)|(cts2+32768)|(cts3+32768);
		if(k&(~0xFFFF))
		{	cts0=tkuac_clamp16s(cts0);
			cts1=tkuac_clamp16s(cts1);
			cts2=tkuac_clamp16s(cts2);
			cts3=tkuac_clamp16s(cts3);	}
		sct[4]=cts0;	sct[5]=cts1;
		sct[6]=cts2;	sct[7]=cts3;
#endif

		sct+=8;
		
	}
}

int TKuAC_CheckSyncFcc(byte *cs, u32 fcc)
{
	u32 fc0, fc1;
	
	fc0=(cs[0])|(cs[2]<<8)|(cs[4]<<16)|(cs[6]<<24);
	fc1=(cs[1])|(cs[3]<<8)|(cs[5]<<16)|(cs[7]<<24);
	
	if(fc0!=fcc)
		return(0);
	if(fc0!=(~fc1))
		return(0);
	return(1);
}

int TKuAC_DecodeStreamBlock(TKuAC_DecState *ctx,
	byte **rcs, s16 **rcta, byte *csbufe)
{
	byte *cs, *cse;
	s16 *sct;
	s32 dca[8];
	int nl, tcc, tv, tz;
	int i, j, k;
	
	cs=*rcs;
	if((cs[0]!='A') || (cs[1]!='B') || !(cs[3]&0x80))
	{
		while(cs<csbufe)
		{
			if(TKuAC_CheckSyncFcc(cs, TKUAC_FCC_tkua))
				break;
		}

		if(!(cs<csbufe))
			return(0);

		cs+=8;
		while(cs<csbufe)
		{
			if((cs[0]=='A') && (cs[1]=='B') && (cs[3]&0x80))
				break;
			tcc=cs[0]|(cs[1]<<8);
			nl=cs[2]|(cs[3]<<8);
			if((tcc&0x8080) || !(nl&0x8000))
				break;
			nl=(~nl)&0xFFFF;
			
			if(tcc==TKUAC_TCC_qc)
				memcpy(ctx->qtab_c, cs+4, 64);
			if(tcc==TKUAC_TCC_qs)
				memcpy(ctx->qtab_s, cs+4, 64);
			
			if(tcc==TKUAC_TCC_hp)
			{
				k=cs[4]|(cs[5]<<8)|(cs[6]<<16);
				ctx->tgthz=k;
			}
			
			cs+=nl;
		}
	}

	if((cs[0]!='A') || (cs[1]!='B') || !(cs[3]&0x80))
		return(-1);
	
	nl=cs[2]|(cs[3]<<8);
	nl=(~nl)&0xFFFF;
	cse=cs+nl;
	*rcs=cse;
	
	sct=*rcta;
//	*rcta=sct+256*8;
	
	memset(sct, 0, 256*8*2);
	
	ctx->pdc_c=0;			ctx->pdc_s=0;
	ctx->pdsbp_c=0;			ctx->pdsbp_s=0;
	ctx->sbparm_c[0]=0;		ctx->sbparm_c[1]=0;
	ctx->sbparm_c[2]=0;		ctx->sbparm_c[3]=0;
	ctx->sbparm_c[4]=0;		ctx->sbparm_c[5]=0;
	ctx->sbparm_c[6]=0;		ctx->sbparm_c[7]=0;
	ctx->sbparm_s[0]=0;		ctx->sbparm_s[1]=0;
	
	ctx->qfxsbp=1;
	ctx->qfxsc=64;
	
	TKuAC_DecodeSetupReadBitstream(ctx, cs+4);

	while(1)
	{
		TKuAC_ReadCoeffTag(ctx, &tv, &tz);

		if(tz==1)
		{
			if(tv>0)
			{
				k=tv;
				ctx->qfxsbp=(ctx->qtab_c[0]*k)>>TKUAC_QFX_SHL;
				if(ctx->qfxsbp<1)
					ctx->qfxsbp=1;
				dca[0]=TKuAC_ReadCoeffSbpC(ctx);
				dca[1]=TKuAC_ReadCoeffSbpC(ctx);
				dca[2]=TKuAC_ReadCoeffSbpC(ctx);
				dca[3]=TKuAC_ReadCoeffSbpC(ctx);
				ctx->pdc_c=dca[0];
				ctx->pdc_s=dca[1];
				ctx->pdsbp_c=dca[2];
				ctx->pdsbp_s=dca[3];
			}else
				if(tv<0)
			{
				ctx->qfxsc=-tv;
				TKuAC_DecodeSampleSubBlock(ctx, sct);
				sct+=256*2;
			}else
			{
				break;	/* EOB */
			}
			continue;
		}

		if(tz==2)
		{
			k=(tv<0)?(-tv):tv;
			ctx->qfxsbp=(ctx->qtab_c[0]*k)>>TKUAC_QFX_SHL;
			if(ctx->qfxsbp<1)
				ctx->qfxsbp=1;

			if(tv>0)
			{
				ctx->sbparm_c[0]=TKuAC_ReadCoeffSbpC(ctx);
				ctx->sbparm_c[1]=TKuAC_ReadCoeffSbpC(ctx);
				ctx->sbparm_c[2]=TKuAC_ReadCoeffSbpC(ctx);
				ctx->sbparm_c[3]=TKuAC_ReadCoeffSbpC(ctx);
				ctx->sbparm_c[4]=TKuAC_ReadCoeffSbpC(ctx);
				ctx->sbparm_c[5]=TKuAC_ReadCoeffSbpC(ctx);
				ctx->sbparm_c[6]=TKuAC_ReadCoeffSbpC(ctx);
				ctx->sbparm_s[0]=TKuAC_ReadCoeffSbpS(ctx);
				ctx->sbparm_s[1]=TKuAC_ReadCoeffSbpS(ctx);
				continue;
			}else if(tv<0)
			{
				ctx->sbparm_c[0]=ctx->sbparm_c[4];
				ctx->sbparm_c[1]=ctx->sbparm_c[5];
				ctx->sbparm_c[2]=ctx->sbparm_c[6];
				ctx->sbparm_c[3]=TKuAC_ReadCoeffSbpC(ctx);
				ctx->sbparm_c[4]=TKuAC_ReadCoeffSbpC(ctx);
				ctx->sbparm_c[5]=TKuAC_ReadCoeffSbpC(ctx);
				ctx->sbparm_c[6]=TKuAC_ReadCoeffSbpC(ctx);

				ctx->sbparm_s[0]=ctx->sbparm_s[1];
				ctx->sbparm_s[1]=TKuAC_ReadCoeffSbpS(ctx);
				continue;
			}
			continue;
		}
		/* unknown tag and/or parameter. */
	}

	*rcta=sct;
	return(1);
}
