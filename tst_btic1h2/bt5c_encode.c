typedef struct BTIC5C_EncodeContext_s BTIC5C_EncodeContext;

struct BTIC5C_EncodeContext_s {
	u64 *blka;
	u64 *lblka;
	
	byte *ct;			//current position

	int xs;				//image X size (pixels)
	int ys;				//image Y size (pixels)
	int bxs;			//image X size (4x4 blocks)
	int bys;			//image Y size (4x4 blocks)
	int cxs;			//image X size (16x16 blocks)
	int cys;			//image Y size (16x16 blocks)
	int fl;
	int qfl;
	
	short ld_flat;
	short ld_2x2;
	short ld_skip;
	short ld_joint;

	u32 clrab;			//Color A/B endpoints
	short li_skip;
	byte li_skipb;

	byte run_flatlo;
	byte run_flathi;

	byte run_222;
	byte run_221;
	byte run_441;
	byte rund_222[32];
	byte rund_221[32];
	u16 rund_441[16];

	byte clrhist_pos;	//color history position
	byte blkhist_pos;	//block history position

	u32 clrhist[16];	//color history
	u32 blkhist[16];	//block history

	int stat_clrty[8];
	int stat_blkty[16];
	int stat_bytes_clr;
	int stat_skipped;

	int stat_runs[8][4];
	int stat_longflat[4];

	byte *tfbuf;		//Frame temporary buffer
	int tfbsz;			//Frame buffer size
	
	byte *zfbuf;		//LZ Frame temporary buffer
	int zfbsz;			//LZ Frame buffer size
};

BTIC5C_EncodeContext *BTIC5C_AllocEncodeContext()
{
	BTIC5C_EncodeContext *ctx;
	ctx=malloc(sizeof(BTIC5C_EncodeContext));
	memset(ctx, 0, sizeof(BTIC5C_EncodeContext));
	return(ctx);
}

int BTIC5C_FreeEncodeContext(BTIC5C_EncodeContext *ctx)
{
	if(!ctx)
		return(-1);
	if(ctx->blka)
		free(ctx->blka);
	if(ctx->lblka)
		free(ctx->lblka);
	if(ctx->tfbuf)
		free(ctx->tfbuf);
	if(ctx->zfbuf)
		free(ctx->zfbuf);
	free(ctx);
	return(0);
}

#if 0
char bt5b_pat6gen[8*4]={
	 2,  2,  2,  2,
	 4,  2, -2, -4,
	 4, -1,  1, -4,
	 4, -4,  4, -4,
	-2, -2, -2, -2,
	-4, -2,  2,  4,
	-4,  1, -1,  4,
	-4,  4, -4,  4
};

u32 BTIC5C_InitDeltas_FixupPat6(u32 pat2, u16 pat1)
{
	int x, y, xn1, xp1, yn1, yp1, ix, p;
	int pxn1, pxp1, pyn1, pyp1;
	u32 patb;

//	patb=pat2;
	patb=0;

	for(y=0; y<4; y++)
		for(x=0; x<4; x++)
	{
		ix=y*4+x;
		p=(pat2>>(ix*2))&3;

		if((p==0) || (p==3))
		{
			patb|=p<<(ix*2);
			continue;
		}

		xn1=(x>0)?(x-1):x;	xp1=(x<3)?(x+1):x;
		yn1=(y>0)?(y-1):y;	yp1=(y<3)?(y+1):y;
		
		pxn1=(pat2>>((y*4+xn1)*2))&3;	pxp1=(pat2>>((y*4+xp1)*2))&3;
		pyn1=(pat2>>((yn1*4+x)*2))&3;	pyp1=(pat2>>((yp1*4+x)*2))&3;
		
		if((pxn1==0) && (pxp1==0) && (p==1))		p=0;
		if((pxn1==3) && (pxp1==3) && (p==2))		p=3;

		if((pyn1==0) && (pyp1==0) && (p==1))		p=0;
		if((pyn1==3) && (pyp1==3) && (p==2))		p=3;

		if((pxn1==1) && (pxp1==0) && (p==1))		p=0;
		if((pxn1==2) && (pxp1==3) && (p==2))		p=3;
		if((pxn1==0) && (pxp1==1) && (p==1))		p=0;
		if((pxn1==3) && (pxp1==2) && (p==2))		p=3;

		if((pyn1==1) && (pyp1==0) && (p==1))		p=0;
		if((pyn1==2) && (pyp1==3) && (p==2))		p=3;
		if((pyn1==0) && (pyp1==1) && (p==1))		p=0;
		if((pyn1==3) && (pyp1==2) && (p==2))		p=3;

		patb|=p<<(ix*2);
	}

	return(patb);
}
#endif

int BTIC5C_EncodeContextSetupQuality(BTIC5C_EncodeContext *ctx, int qfl)
{
	int qf;

	qf=qfl&127;
	if(qf>100)qf=100;
	ctx->ld_flat=50-(qf/2);
	ctx->ld_2x2=100-qf;
	ctx->ld_skip=100-qf;
	ctx->ld_joint=100-qf;
	return(0);
}

#if 0
int BTIC5C_EncodeGenTables()
{
	int cr, cg, cb, px, px1;
	int x, y, z, u, v;
	int i, j, k;

#if 0
	for(i=0; i<8; i++)
	{
		j=i*2;
		k=j+(j<<5)+(j<<10);
//		printf("%04X\n", k);
	}
	
	for(x=0; x<3; x++)
		for(y=0; y<3; y++)
			for(z=0; z<3; z++)
	{
		cr=0;	cg=0;	cb=0;
		if(x==1)	cr= 2;
		if(x==2)	cr=30;
		if(y==1)	cg= 2;
		if(y==2)	cb=30;
		if(z==1)	cb= 2;
		if(z==2)	cb=30;
		
		j=(cr<<10)|(cg<<5)|cb;
		k=(x*3+y)*3+z;
		if(!k)
			continue;

		printf("0x%04X%04X, \n", j, j);
	}
	for(x=0; x<6; x++)
	{
		if(x==0)	cr= 2;
		if(x==1)	cr=30;
		if(x==2)	cr= 4;
		if(x==3)	cr=28;
		if(x==4)	cr= 6;
		if(x==5)	cr=26;
		cg=cr; cb=cr;

		j=(cr<<10)|(cg<<5)|cb;
		printf("0x%04X%04X, \n", j, j);
	}
#endif

#if 0
	for(y=0; y<8; y++)
		for(x=0; x<8; x++)
	{
		px=0; px1=0;
		for(v=0; v<4; v++)
			for(u=0; u<4; u++)
		{
			j=	bt5b_pat6gen[x*4+u] +
				bt5b_pat6gen[y*4+v] ;
			
			k=v*4+u;
			if(j>=0)px1|=1<<k;
						
			j=j+2;
			if(j<0) j=0;
			if(j>3) j=3;
			px|=j<<(k*2);
		}
		
		px=BTIC5C_InitDeltas_FixupPat6(px, px1);
		
		printf("0x%08X, \n", px);
	}
#endif

	for(i=0; i<256; i++)
	{
		px=0;

#if 0
		if(i&0x01)	px|=0x0003;
		if(i&0x02)	px|=0x000C;
		if(i&0x04)	px|=0x0030;
		if(i&0x08)	px|=0x00C0;
		if(i&0x10)	px|=0x0300;
		if(i&0x20)	px|=0x0C00;
		if(i&0x40)	px|=0x3000;
		if(i&0x80)	px|=0xC000;
		printf("0x%04X, \n", px);
#endif

#if 0
		if((i&0x03)==0x01)	px|=0x00000505;
		if((i&0x03)==0x02)	px|=0x00000A0A;
		if((i&0x03)==0x03)	px|=0x00000F0F;
		if((i&0x0C)==0x04)	px|=0x00005050;
		if((i&0x0C)==0x08)	px|=0x0000A0A0;
		if((i&0x0C)==0x0C)	px|=0x0000F0F0;
		if((i&0x30)==0x10)	px|=0x05050000;
		if((i&0x30)==0x20)	px|=0x0A0A0000;
		if((i&0x30)==0x30)	px|=0x0F0F0000;
		if((i&0xC0)==0x40)	px|=0x50500000;
		if((i&0xC0)==0x80)	px|=0xA0A00000;
		if((i&0xC0)==0xC0)	px|=0xF0F00000;
		printf("0x%08X, ", px);
		if((i&3)==3)
			printf("\n");
#endif
	}
}
#endif

int BTIC5C_EncodeInitContext(BTIC5C_EncodeContext *ctx,
	int xs, int ys, int qfl)
{
	int qf;
		
	ctx->xs=xs;
	ctx->ys=ys;
	ctx->cxs=(xs+15)>>4;
	ctx->cys=(ys+15)>>4;
	ctx->bxs=ctx->cxs*4;
	ctx->bys=ctx->cys*4;

//	ctx->bxs=(xs+3)>>2;
//	ctx->bys=(ys+3)>>2;
//	ctx->ystr=xs;
	ctx->fl=0;

	ctx->blka=malloc(ctx->bxs*ctx->bys*sizeof(u64));
	ctx->lblka=malloc(ctx->bxs*ctx->bys*sizeof(u64));
//	ctx->fl|=1;

//	ctx->clra_a=malloc(ctx->bxs*ctx->bys*sizeof(u16));
//	ctx->clra_b=malloc(ctx->bxs*ctx->bys*sizeof(u16));
//	ctx->clra_t=malloc(ctx->bxs*ctx->bys*sizeof(byte));
	
	ctx->tfbuf=malloc(xs*ys*4);
	
	BTIC5C_EncodeContextSetupQuality(ctx, qfl);

	return(0);
}

int	BTIC5C_EncodeGetColorDist16(u16 ca, u16 cb)
{
	int cr0, cg0, cb0;
	int cr1, cg1, cb1;
	int dr, dg, db, d;
	
	cr0=((ca>>10)&31)<<3;	cg0=((ca>> 5)&31)<<3;	cb0=((ca>> 0)&31)<<3;
	cr1=((cb>>10)&31)<<3;	cg1=((cb>> 5)&31)<<3;	cb1=((cb>> 0)&31)<<3;
	dr=cr0-cr1;				dg=cg0-cg1;				db=cb0-cb1;
	
	dr=dr^(dr>>31);
	dg=dg^(dg>>31);
	db=db^(db>>31);
	d=dr+dg+db;
	return(d);
}

int	BTIC5C_EncodeGetColorPairDist16(u32 ca, u32 cb)
{
	int d0, d1, d;
	
	d0=BTIC5C_EncodeGetColorDist16(ca>> 0, cb>> 0);
	d1=BTIC5C_EncodeGetColorDist16(ca>>16, cb>>16);
	d=d0+d1;
	return(d);
}

int BTIC5C_EncodeCheckCanSkipBlock(BTIC5C_EncodeContext *ctx,
	u64 blk0, u64 blk1)
{
	u16 tblka0[16], tblka1[16];
	u16 px0, px1, px2, px3;
	u64 blk2, blk3;
	int ca0, ca1, ca2, ca3, cb0, cb1, cb2, cb3;
	int d0, d1, d2, d3;
	int i, j, k;

	if(blk0==blk1)
		return(1);

	px0=(blk0>>32)&65535;	ca0=(blk0>> 0)&65535;	cb0=(blk0>>16)&65535;
	px1=(blk1>>32)&65535;	ca1=(blk1>> 0)&65535;	cb1=(blk1>>16)&65535;
	
	d0=BTIC5C_EncodeGetColorDist16(ca0, cb1);
	d1=BTIC5C_EncodeGetColorDist16(ca1, cb0);

	if((d0<ctx->ld_skip) && (d1<ctx->ld_skip))
		return(1);

#if 1
	d2=BTIC5C_EncodeGetColorDist16(ca0, ca1);
	d3=BTIC5C_EncodeGetColorDist16(cb0, cb1);
	
//	blk2=(((u64)px0)<<16)|(((u64)ca0)<<32)|(((u64)cb0)<<48)|3;
//	blk3=(((u64)px1)<<16)|(((u64)ca1)<<32)|(((u64)cb1)<<48)|3;
	BTIC5C_UnpackCellB_RGB555(blk0, tblka0, 4);
	BTIC5C_UnpackCellB_RGB555(blk1, tblka1, 4);
	
	d0=0;
	for(i=0; i<16; i++)
		{ d0+=BTIC5C_EncodeGetColorDist16(tblka0[i], tblka1[i]); }
	d0=d0/16;
	
	if(d0<ctx->ld_skip)
		return(1);
#endif

	return(0);
}

int BTIC5C_Encode_CompareError442(u32 pxa, u32 pxb)
{
	int p0, p1, p2, pe;
	int i, j, k;
	
	if(pxa==pxb)
		return(0);
	
	pe=0;
	for(i=0; i<16; i++)
	{
		p0=(pxa>>(i*2))&3;
		p1=(pxb>>(i*2))&3;
		p2=p0-p1;
		if(p2<0)
			p2=-p2;
		pe+=p2;
	}
	return(pe);
}

int BTIC5C_Encode_LookupPat6(u32 pxa)
{
	u32 pxb;
	int i, j, k, d, bd, bi;
	
	bi=0; bd=999999;
	for(i=0; i<64; i++)
	{
		pxb=btic5c_blk_pat6tab[i];
		d=BTIC5C_Encode_CompareError442(pxa, pxb);
		if(d<bd)
			{ bi=i; bd=d; }
	}
	return(bi);
}

int BTIC5C_EncodeBlockIsFlatP(BTIC5C_EncodeContext *ctx, u64 blk)
{
	return((blk>>32)==0);
}

int BTIC5C_EncodeBlockIsFlatHiP(BTIC5C_EncodeContext *ctx, u64 blk)
{
	return((blk>>32)==0xFFFFFFFFU);
}

int BTIC5C_EncodeBlockIs221P(BTIC5C_EncodeContext *ctx, u64 blk)
{
	if(blk&0x8000)
		return((blk>>48)==1);
	return(0);
}

int BTIC5C_EncodeBlockIs222P(BTIC5C_EncodeContext *ctx, u64 blk)
{
	if(blk&0x8000)
		return((blk>>48)==2);
	return(0);
}

int BTIC5C_EncodeBlockIs441P(BTIC5C_EncodeContext *ctx, u64 blk)
{
	if(blk&0x8000)
		return((blk>>48)==4);
	return(0);
}

int BTIC5C_EncodeBlockIsPat6P(BTIC5C_EncodeContext *ctx, u64 blk)
{
	if(blk&0x8000)
		return((blk>>48)==3);
	return(0);
}

int BTIC5C_EncodeBlockIsPatHistP(BTIC5C_EncodeContext *ctx, u64 blk)
{
	if(blk&0x8000)
		return((blk>>48)==7);
	return(0);
}

int BTIC5C_EncodeBlockIs442P(BTIC5C_EncodeContext *ctx, u64 blk)
{
	if(!(blk&0x8000))
		return(1);
	return(0);
}

int BTIC5C_EncodeCheckBlock_WithColor(BTIC5C_EncodeContext *ctx,
	u64 blk0, u32 clr)
{
	u64 blk1;

	if(clr&0x8000)
		return(0);

	blk1=(blk0&0xFFFFFFFF80008000ULL)|
		(clr&0x7FFF7FFF);
	return(BTIC5C_EncodeCheckCanSkipBlock(ctx, blk0, blk1));
}

int BTIC5C_EncodeCheckBlock_WithPat442(BTIC5C_EncodeContext *ctx,
	u64 blk0, u32 pat)
{
	u64 blk1;
	blk1=(((u64)pat)<<32)|(blk0&0x7FFF7FFF);
	return(BTIC5C_EncodeCheckCanSkipBlock(ctx, blk0, blk1));
}

int BTIC5C_EncodeAddColorHist(BTIC5C_EncodeContext *ctx, u32 clr)
{
	int pos;
	pos=ctx->clrhist_pos;
	pos=(pos-1)&15;
	ctx->clrhist[pos]=clr;
	ctx->clrhist_pos=pos;
	return(0);
}

int BTIC5C_EncodeAddBlockHist442(BTIC5C_EncodeContext *ctx, u32 px)
{
	int pos;
	pos=ctx->blkhist_pos;
	pos=(pos-1)&15;
	ctx->blkhist[pos]=px;
	ctx->blkhist_pos=pos;
	return(0);
}

int BTIC5C_EncodeAddBlockHist441(BTIC5C_EncodeContext *ctx, u16 pi)
{
	u32 px;
	px=         btic5c_blk_pat421[(pi>>8)&0xFF];
	px=(px<<16)|btic5c_blk_pat421[(pi>>0)&0xFF];
	BTIC5C_EncodeAddBlockHist442(ctx, px);
	return(0);
}

int BTIC5C_EncodeAddBlockHist222(BTIC5C_EncodeContext *ctx, byte pi)
{
	u32 px;
	px=btic5c_blk_pat222[pi];
	BTIC5C_EncodeAddBlockHist442(ctx, px);
	return(0);
}

u16 BTIC5C_Encode_PackColorPairToRGBD(u32 clr)
{
	int cr0, cg0, cb0, px0;
	int cr1, cg1, cb1, px1;
	int cr2, cg2, cb2, px2, dr, dg, db, dy;
	
	px0=(clr>> 0)&0x7FFF;
	px1=(clr>>16)&0x7FFF;
	cr0=(px0>>10)&31;	cg0=(px0>> 5)&31;	cb0=(px0>> 0)&31;
	cr1=(px1>>10)&31;	cg1=(px1>> 5)&31;	cb1=(px1>> 0)&31;
	cr2=(cr0+cr1+1)/2;	cg2=(cg0+cg1+1)/2;	cb2=(cb0+cb1+1)/2;
	dr=cr1-cr0;			dg=cg1-cg0;			db=cb1-cb0;
	dr^=dr>>31;			dg^=dg>>31;			db^=db>>31;
//	if(dr<0)	dr=-dr;
//	if(dg<0)	dg=-dg;
//	if(db<0)	db=-db;
//	px2=(cr2<<10)|(cg2<<5)|cb2;
	dy=(dr+dg+db)/6;
	if(dy>7)	dy=7;

#if 1
	while(((cr2&(~1))-dy*2)< 0)		cr2++;
	while(((cg2&(~1))-dy*2)< 0)		cg2++;
	while(((cb2&(~1))-dy*2)< 0)		cb2++;
	while(((cr2&(~1))+dy*2)>31)		cr2--;
	while(((cg2&(~1))+dy*2)>31)		cg2--;
	while(((cb2&(~1))+dy*2)>31)		cb2--;
#endif

	px2=(cr2<<10)|(cg2<<5)|cb2;
	px2&=0x7BDE;

	if(dy&1)	px2|=0x0001;
	if(dy&2)	px2|=0x0020;
	if(dy&4)	px2|=0x0400;
	return(px2);
}

u32 BTIC5C_Encode_UnpackColorPairFromRGBD(u16 clr)
{
	u16 pxa, pxb, pxc;
	int px, dy;
	
	pxc=clr&0x7BDE;
	dy=((clr>>0)&1)|(((clr>>5)&1)<<1)|(((clr>>10)&1)<<2);
	pxa=(pxc+btic5c_clr_dyptab[dy])&0x7BDE;
	pxb=(pxc-btic5c_clr_dyptab[dy])&0x7BDE;
	px=(pxa<<16)|pxb;
	return(px);
}

int BTIC5C_EncodeBlockEndpoint(BTIC5C_EncodeContext *ctx, u64 blk)
{
	u32 ca, cb;
	u32 d_bc, h_bc, p_bc, h_bci, h_bcj, d_bci, d_bcj;
	int d_bi, d_bj, d_bd;
	int h_bi, h_bj, h_bd;
	int i, j, k, d;
	
	ca=blk&0x7FFF7FFF;

	d_bi=0; d_bd=999999;
	for(i=0; i<32; i++)
	{
		cb=(ctx->clrab+btic5c_clr_deltatab[i])&0x7BDE7BDEU;
//		cb=ctx->clrab+btic5c_clr_deltatab[i];
		d=BTIC5C_EncodeGetColorPairDist16(ca, cb);
		if(d<d_bd)
			{ d_bd=d; d_bi=i; d_bc=cb; }
	}

	h_bi=-1; h_bd=999999;
	for(i=0; i<16; i++)
	{
		j=(ctx->clrhist_pos+i)&15;
		cb=ctx->clrhist[j];
		if(cb&0x8000)
			continue;
		d=BTIC5C_EncodeGetColorPairDist16(ca, cb);
		if(d<h_bd)
			{ h_bd=d; h_bi=i; h_bc=cb; }
	}
	
	if(d_bd<h_bd)
	{
#if 1
		if(BTIC5C_EncodeCheckBlock_WithColor(ctx, blk, d_bc) &&
			!((ctx->clrab)&0x8000))
		{
			ctx->stat_clrty[1]++;
			ctx->stat_bytes_clr++;
			*ctx->ct++=0x01|(d_bi<<3);
			ctx->clrab=d_bc;
			BTIC5C_EncodeAddColorHist(ctx, d_bc);
			return(1);
		}
#endif

#if 1
		if((h_bi>=0) && BTIC5C_EncodeCheckBlock_WithColor(ctx, blk, h_bc))
		{
			ctx->stat_clrty[2]++;
			ctx->stat_bytes_clr++;
			*ctx->ct++=0x05|(h_bi<<4);
			ctx->clrab=h_bc;
			if(h_bi==15)
				{ ctx->clrhist_pos=(ctx->clrhist_pos-1)&15; }
			return(1);
		}
#endif
	}else
	{
#if 1
		if((h_bi>=0) && BTIC5C_EncodeCheckBlock_WithColor(ctx, blk, h_bc))
		{
			ctx->stat_clrty[2]++;
			ctx->stat_bytes_clr++;
			*ctx->ct++=0x05|(h_bi<<4);
			ctx->clrab=h_bc;
			if(h_bi==15)
				{ ctx->clrhist_pos=(ctx->clrhist_pos-1)&15; }
			return(1);
		}
#endif

#if 1
		if(BTIC5C_EncodeCheckBlock_WithColor(ctx, blk, d_bc) &&
			!((ctx->clrab)&0x8000))
		{
			ctx->stat_clrty[1]++;
			ctx->stat_bytes_clr++;
			*ctx->ct++=0x01|(d_bi<<3);
			ctx->clrab=d_bc;
			BTIC5C_EncodeAddColorHist(ctx, d_bc);
			return(1);
		}
#endif
	}
	
	k=BTIC5C_Encode_PackColorPairToRGBD(ca);
	p_bc=BTIC5C_Encode_UnpackColorPairFromRGBD(k);

	if(BTIC5C_EncodeCheckBlock_WithColor(ctx, blk, p_bc))
//	if(1)
//	if(0)
	{
		ctx->stat_clrty[0]++;
		ctx->stat_bytes_clr+=2;
		j=k<<1;
		*ctx->ct++=j>>0;
		*ctx->ct++=j>>8;
		ctx->clrab=p_bc;
		BTIC5C_EncodeAddColorHist(ctx, p_bc);
		return(1);
	}


	/* Attempt a Repeat2 */

	h_bi=-1; h_bd=999999;
	for(i=0; i<32; i++)
	{
		j=(ctx->clrhist_pos+i)&15;
		cb=ctx->clrhist[j];
		if(cb&0x8000)
			continue;
		if(i&16)	cb>>=16;
		cb&=0xFFFF;
		d=BTIC5C_EncodeGetColorDist16(ca&0xFFFF, cb);
		if(d<h_bd)
			{ h_bd=d; h_bi=i; h_bci=cb; }
	}

	h_bj=-1; h_bd=999999;
	for(i=0; i<32; i++)
	{
		j=(ctx->clrhist_pos+i)&15;
		cb=ctx->clrhist[j];
		if(cb&0x8000)
			continue;
		if(i&16)	cb>>=16;
		cb&=0xFFFF;
		d=BTIC5C_EncodeGetColorDist16((ca>>16)&0xFFFF, cb);
		if(d<h_bd)
			{ h_bd=d; h_bj=i; h_bcj=cb; }
	}

	h_bc=(h_bcj<<16)|h_bci;
	if((h_bi>=0) && (h_bj>=0) &&
		BTIC5C_EncodeCheckBlock_WithColor(ctx, blk, h_bc))
	{
		ctx->stat_clrty[4]++;
		ctx->stat_bytes_clr+=2;
		k=0x001D|(h_bi<<6)|(h_bj<<11);
		*ctx->ct++=k>> 0;
		*ctx->ct++=k>> 8;
		ctx->clrab=h_bc;
		BTIC5C_EncodeAddColorHist(ctx, h_bc);
		return(1);
	}

	/* Attempt Delta2, Split */

	d_bi=0; d_bd=999999;
	for(i=0; i<32; i++)
	{
		cb=((ctx->clrab)+btic5c_clr_deltatab[i])&0x7BDE7BDEU;
		d=BTIC5C_EncodeGetColorDist16(ca, cb);
		if(d<d_bd)
			{ d_bd=d; d_bi=i; d_bci=cb; }
	}

	d_bj=0; d_bd=999999;
	for(i=0; i<32; i++)
	{
		cb=((ctx->clrab)+btic5c_clr_deltatab[i])&0x7BDE7BDEU;
		d=BTIC5C_EncodeGetColorDist16(ca>>16, cb>>16);
		if(d<d_bd)
			{ d_bd=d; d_bj=i; d_bcj=cb; }
	}

	d_bc=(d_bcj&0xFFFF0000U)|(d_bci&0x0000FFFFU);
	if(BTIC5C_EncodeCheckBlock_WithColor(ctx, blk, d_bc) &&
		!((ctx->clrab)&0x8000))
	{
		ctx->stat_clrty[5]++;
		ctx->stat_bytes_clr+=2;
		k=0x000D|(d_bi<<6)|(d_bj<<11);
		*ctx->ct++=k>> 0;
		*ctx->ct++=k>> 8;
		ctx->clrab=d_bc;
		BTIC5C_EncodeAddColorHist(ctx, d_bc);
		
//		printf("E:%08X ", d_bc);
		
		return(1);
	}

	/* Attempt Delta2, Joint */

#if 0
	d_bi=0; d_bd=999999;
	for(i=0; i<32; i++)
	{
		cb=((ctx->clrab)+btic5c_clr_deltatab[i])&0x7BDE7BDEU;
//		d=BTIC5C_EncodeGetColorDist16(ca, cb);
		d=BTIC5C_EncodeGetColorPairDist16(ca, cb);
		if(d<d_bd)
			{ d_bd=d; d_bi=i; d_bci=cb; }
	}

	d_bj=0; d_bd=999999;
	for(i=0; i<32; i++)
	{
		cb=(d_bci+btic5c_clr_deltatab[i])&0x7BDE7BDEU;
//		d=BTIC5C_EncodeGetColorDist16(ca>>16, cb>>16);
		d=BTIC5C_EncodeGetColorPairDist16(ca, cb);
		if(d<d_bd)
			{ d_bd=d; d_bj=i; d_bcj=cb; }
	}
	d_bc=d_bcj;
#endif

#if 1
	d_bi=0; d_bd=999999;
	for(i=0; i<32; i++)
		for(j=i; j<32; j++)
	{
		cb=((ctx->clrab)+btic5c_clr_deltatab[i])&0x7BDE7BDEU;
		cb=(cb+btic5c_clr_deltatab[i])&0x7BDE7BDEU;
		d=BTIC5C_EncodeGetColorPairDist16(ca, cb);
		if(d<d_bd)
			{ d_bd=d; d_bi=i; d_bj=j; d_bc=cb; }
	}
#endif

	if(BTIC5C_EncodeCheckBlock_WithColor(ctx, blk, d_bc) &&
		!((ctx->clrab)&0x8000))
	{
		ctx->stat_clrty[5]++;
		ctx->stat_bytes_clr+=2;
		k=0x002D|(d_bi<<6)|(d_bj<<11);
		*ctx->ct++=k>> 0;
		*ctx->ct++=k>> 8;
		ctx->clrab=d_bc;
		BTIC5C_EncodeAddColorHist(ctx, d_bc);
		
//		printf("E:%08X ", d_bc);
		
		return(1);
	}

	/* Pair Fallback */

	ctx->stat_clrty[3]++;
	ctx->stat_bytes_clr+=4;
	k=	((ca<<1)&0xFFFE0000) | ((ca<<2)&0x0001FFFC) | 3;
	*ctx->ct++=k>> 0;
	*ctx->ct++=k>> 8;
	*ctx->ct++=k>>16;
	*ctx->ct++=k>>24;
	ctx->clrab=ca;
	BTIC5C_EncodeAddColorHist(ctx, ca);
	return(1);
}

int BTIC5C_EncodeFlushBlockRuns(BTIC5C_EncodeContext *ctx)
{
	int i, j, k;

	if(ctx->run_flatlo)
	{
		k=ctx->run_flatlo;
		ctx->stat_blkty[0]+=k;

		if(k>5)		ctx->stat_longflat[0]++;
		if(k>12)	ctx->stat_longflat[1]++;
		if(k>24)	ctx->stat_longflat[2]++;
		if(k>36)	ctx->stat_longflat[3]++;

#if 1
		while(k>=67)
		{
			ctx->stat_runs[7][1]++;
			*ctx->ct++=0x7F;
			*ctx->ct++=0xFC;
			k-=67;
		}
		if(k>5)
		{
			ctx->stat_runs[7][0]++;
			*ctx->ct++=0x7F;
			*ctx->ct++=(k-4)<<2;
			k=0;
		}
#endif

		while((k>=8) || (k==5))
		{
			ctx->stat_runs[0][3]++;
			*ctx->ct++=0xCF; k-=5;
		}
		while(k>=5)
		{
			ctx->stat_runs[0][3]++;
			*ctx->ct++=0xCF; k-=5;
		}
		if(k>=2)
		{
			ctx->stat_runs[0][k-2]++;
			*ctx->ct++=0x0F|((k-2)<<6); k=0;
		}
		while(k--)
			*ctx->ct++=0x03;
		ctx->run_flatlo=0;
	}
	if(ctx->run_flathi)
	{
		k=ctx->run_flathi;
		ctx->stat_blkty[1]+=k;

		if(k>5)		ctx->stat_longflat[0]++;
		if(k>12)	ctx->stat_longflat[1]++;
		if(k>24)	ctx->stat_longflat[2]++;
		if(k>36)	ctx->stat_longflat[3]++;

		while(k>=8)
		{
			ctx->stat_runs[1][3]++;
			*ctx->ct++=0xEF; k-=5;
		}
		while(k>=5)
		{
			ctx->stat_runs[1][3]++;
			*ctx->ct++=0xEF; k-=5;
		}
		if(k>=2)
		{
			ctx->stat_runs[1][k-2]++;
			*ctx->ct++=0x2F|((k-2)<<6); k=0;
		}
		while(k--)
			*ctx->ct++=0xF3;
		ctx->run_flathi=0;
	}

	if(ctx->run_222)
	{
		k=ctx->run_222;
		ctx->stat_blkty[2]+=k;

//		while(k>=5)
		while((k>=8) || (k==5))
		{
			ctx->stat_runs[2][3]++;
			*ctx->ct++=0xF7;
			*ctx->ct++=ctx->rund_222[0];
			*ctx->ct++=ctx->rund_222[1];
			*ctx->ct++=ctx->rund_222[2];
			*ctx->ct++=ctx->rund_222[3];
			*ctx->ct++=ctx->rund_222[4];
			memmove(ctx->rund_222, ctx->rund_222+5, 32-5);
			k-=5;
		}
		while(k>=3)
		{
			ctx->stat_runs[2][1]++;
			*ctx->ct++=0x77;
			*ctx->ct++=ctx->rund_222[0];
			*ctx->ct++=ctx->rund_222[1];
			*ctx->ct++=ctx->rund_222[2];
			memmove(ctx->rund_222, ctx->rund_222+3, 32-3);
			k-=3;
		}
		while(k>=2)
		{
			ctx->stat_runs[2][0]++;
			*ctx->ct++=0x37;
			*ctx->ct++=ctx->rund_222[0];
			*ctx->ct++=ctx->rund_222[1];
			memmove(ctx->rund_222, ctx->rund_222+2, 32-2);
			k-=2;
		}
		if(k)
		{
			*ctx->ct++=0x5F;
			*ctx->ct++=ctx->rund_222[0];
			BTIC5C_EncodeAddBlockHist222(ctx, ctx->rund_222[0]);
		}
		ctx->run_222=0;
	}

	if(ctx->run_221)
	{
		k=ctx->run_221;
		ctx->stat_blkty[3]+=k;

		while(k>=8)
		{
			ctx->stat_runs[3][3]++;
			*ctx->ct++=0xD7;
			*ctx->ct++=ctx->rund_221[0]|(ctx->rund_221[1]<<4);
			*ctx->ct++=ctx->rund_221[2]|(ctx->rund_221[3]<<4);
			*ctx->ct++=ctx->rund_221[4]|(ctx->rund_221[5]<<4);
			*ctx->ct++=ctx->rund_221[6]|(ctx->rund_221[7]<<4);
			memmove(ctx->rund_221, ctx->rund_221+8, 32-8);
			k-=8;
		}
		while(k>=6)
		{
			ctx->stat_runs[3][2]++;
			*ctx->ct++=0x97;
			*ctx->ct++=ctx->rund_221[0]|(ctx->rund_221[1]<<4);
			*ctx->ct++=ctx->rund_221[2]|(ctx->rund_221[3]<<4);
			*ctx->ct++=ctx->rund_221[4]|(ctx->rund_221[5]<<4);
			memmove(ctx->rund_222, ctx->rund_222+6, 32-6);
			k-=6;
		}
		while(k>=4)
		{
			ctx->stat_runs[3][1]++;
			*ctx->ct++=0x57;
			*ctx->ct++=ctx->rund_221[0]|(ctx->rund_221[1]<<4);
			*ctx->ct++=ctx->rund_221[2]|(ctx->rund_221[3]<<4);
			memmove(ctx->rund_222, ctx->rund_222+4, 32-4);
			k-=4;
		}
		while(k>=2)
		{
			ctx->stat_runs[3][0]++;
			*ctx->ct++=0x17;
			*ctx->ct++=ctx->rund_221[0]|(ctx->rund_221[1]<<4);
			memmove(ctx->rund_222, ctx->rund_222+2, 32-2);
			k-=2;
		}
		if(k)
		{
			*ctx->ct++=0x03|(ctx->rund_221[0]<<4);
		}
		ctx->run_221=0;
	}
	
	if(ctx->run_441)
	{
		k=ctx->run_441;
		ctx->stat_blkty[5]+=k;

		while((k>=8) || (k==5))
		{
			ctx->stat_runs[4][3]++;
			*ctx->ct++=0xC7;
			*ctx->ct++=(ctx->rund_441[0]>>0)&255;
			*ctx->ct++=(ctx->rund_441[0]>>8)&255;
			*ctx->ct++=(ctx->rund_441[1]>>0)&255;
			*ctx->ct++=(ctx->rund_441[1]>>8)&255;
			*ctx->ct++=(ctx->rund_441[2]>>0)&255;
			*ctx->ct++=(ctx->rund_441[2]>>8)&255;
			*ctx->ct++=(ctx->rund_441[3]>>0)&255;
			*ctx->ct++=(ctx->rund_441[3]>>8)&255;
			*ctx->ct++=(ctx->rund_441[4]>>0)&255;
			*ctx->ct++=(ctx->rund_441[4]>>8)&255;
			memmove(ctx->rund_441, ctx->rund_441+5, (16-5)*2);
			k-=5;
		}
		while(k>=3)
		{
			ctx->stat_runs[4][1]++;
			*ctx->ct++=0x47;
			*ctx->ct++=(ctx->rund_441[0]>>0)&255;
			*ctx->ct++=(ctx->rund_441[0]>>8)&255;
			*ctx->ct++=(ctx->rund_441[1]>>0)&255;
			*ctx->ct++=(ctx->rund_441[1]>>8)&255;
			*ctx->ct++=(ctx->rund_441[2]>>0)&255;
			*ctx->ct++=(ctx->rund_441[2]>>8)&255;
			memmove(ctx->rund_441, ctx->rund_441+3, (16-3)*2);
			k-=3;
		}
		while(k>=2)
		{
			ctx->stat_runs[4][0]++;
			*ctx->ct++=0x07;
			*ctx->ct++=(ctx->rund_441[0]>>0)&255;
			*ctx->ct++=(ctx->rund_441[0]>>8)&255;
			*ctx->ct++=(ctx->rund_441[1]>>0)&255;
			*ctx->ct++=(ctx->rund_441[1]>>8)&255;
			memmove(ctx->rund_441, ctx->rund_441+2, (16-2)*2);
			k-=2;
		}
		if(k)
		{
			if(ctx->run_441>1)
				j=-1;
			*ctx->ct++=0x1F;
			*ctx->ct++=(ctx->rund_441[0]>>0)&255;
			*ctx->ct++=(ctx->rund_441[0]>>8)&255;
			BTIC5C_EncodeAddBlockHist441(ctx, ctx->rund_441[0]);
		}
		ctx->run_441=0;
	}
	return(0);
}

int BTIC5C_EncodeExtendRunFlatLo(BTIC5C_EncodeContext *ctx)
{
	if(ctx->run_221 && (ctx->run_221<30))
	{
		if(ctx->rund_221[ctx->run_221-1]==0)
		{
			ctx->run_221--;
			BTIC5C_EncodeFlushBlockRuns(ctx);
			ctx->run_flatlo=2;
			return(0);
		}
		ctx->rund_221[ctx->run_221++]=0;
		return(0);
	}

	if(ctx->run_flathi || ctx->run_221 || ctx->run_222 || ctx->run_441)
		BTIC5C_EncodeFlushBlockRuns(ctx);
	if(ctx->run_flatlo>=201)
		BTIC5C_EncodeFlushBlockRuns(ctx);
	ctx->run_flatlo++;

	return(0);
}

int BTIC5C_EncodeExtendRunFlatHi(BTIC5C_EncodeContext *ctx)
{
	if(ctx->run_221 && (ctx->run_221<30))
	{
		if(ctx->rund_221[ctx->run_221-1]==15)
		{
			ctx->run_221--;
			BTIC5C_EncodeFlushBlockRuns(ctx);
			ctx->run_flathi=2;
			return(0);
		}
		ctx->rund_221[ctx->run_221++]=15;
		return(0);
	}

	if(ctx->run_flatlo || ctx->run_221 || ctx->run_222 || ctx->run_441)
		BTIC5C_EncodeFlushBlockRuns(ctx);
	if(ctx->run_flathi>=201)
		BTIC5C_EncodeFlushBlockRuns(ctx);
	ctx->run_flathi++;
	return(0);
}

int BTIC5C_EncodeExtendRun221(BTIC5C_EncodeContext *ctx, int pv)
{
	if(ctx->run_flatlo || ctx->run_flathi || ctx->run_222 || ctx->run_441)
		BTIC5C_EncodeFlushBlockRuns(ctx);
	if(ctx->run_221>=32)
		BTIC5C_EncodeFlushBlockRuns(ctx);
	ctx->rund_221[ctx->run_221++]=pv;
	return(0);
}

int BTIC5C_EncodeExtendRun222(BTIC5C_EncodeContext *ctx, int pv)
{
	if(ctx->run_flatlo || ctx->run_flathi || ctx->run_221 || ctx->run_441)
		BTIC5C_EncodeFlushBlockRuns(ctx);
	if(ctx->run_222>=32)
		BTIC5C_EncodeFlushBlockRuns(ctx);
	ctx->rund_222[ctx->run_222++]=pv;
	return(0);
}

int BTIC5C_EncodeExtendRun441(BTIC5C_EncodeContext *ctx, int pv)
{
	if(ctx->run_flatlo || ctx->run_flathi || ctx->run_221 || ctx->run_222)
		BTIC5C_EncodeFlushBlockRuns(ctx);
	if(ctx->run_441>=16)
		BTIC5C_EncodeFlushBlockRuns(ctx);
	ctx->rund_441[ctx->run_441++]=pv;
	return(0);
}

int BTIC5C_Encode_LookupHistPat442(BTIC5C_EncodeContext *ctx, u32 pxa)
{
	u32 pxb;
	int i, j, k, d, bd, bi, pos;
	
	pos=ctx->blkhist_pos;
	bi=-1; bd=999999;
	for(i=0; i<16; i++)
	{
		pxb=ctx->blkhist[(pos+i)&15];
		if(!pxb)
			continue;
		
		d=BTIC5C_Encode_CompareError442(pxa, pxb);
		if(d<bd)
			{ bi=i; bd=d; }
	}
	return(bi);
}

int BTIC5C_EncodeBlockBase(BTIC5C_EncodeContext *ctx,
	int c_bx, int c_by, u64 iblk, u64 lblk)
{
	u64 blk, blk0, blk1;
	u32 px, pxa, pxb;
	int dy, pos, histix;
	int i, j, k;

	blk=iblk;
	histix=-1;

#if 1
	if(BTIC5C_EncodeBlockIs442P(ctx, blk) &&
		!BTIC5C_EncodeBlockIsFlatP(ctx, blk))
	{
		pos=ctx->blkhist_pos;
		pxa=blk>>32;
		i=BTIC5C_Encode_LookupHistPat442(ctx, pxa);
		if(i>=0)
		{
			pxb=ctx->blkhist[(pos+i)&15];
			if(BTIC5C_EncodeCheckBlock_WithPat442(ctx, iblk, pxb))
				{ histix=i; }
		}
	}

	if(BTIC5C_EncodeBlockIs441P(ctx, blk))
	{
		pos=ctx->blkhist_pos;
//		pxa=blk>>32;

		dy=(blk>>32)&65535;
		pxa=          btic5c_blk_pat421[(dy>>8)&0xFF];
		pxa=(pxa<<16)|btic5c_blk_pat421[(dy>>0)&0xFF];

		i=BTIC5C_Encode_LookupHistPat442(ctx, pxa);
		if(i>=0)
		{
			pxb=ctx->blkhist[(pos+i)&15];
			if(BTIC5C_EncodeCheckBlock_WithPat442(ctx, iblk, pxb))
				{ histix=i; }
		}
	}

	if(BTIC5C_EncodeBlockIs222P(ctx, blk))
	{
		pos=ctx->blkhist_pos;
		dy=(blk>>32)&255;
		pxa=btic5c_blk_pat222[dy];

		i=BTIC5C_Encode_LookupHistPat442(ctx, pxa);
		if(i>=0)
		{
			pxb=ctx->blkhist[(pos+i)&15];
			if(BTIC5C_EncodeCheckBlock_WithPat442(ctx, iblk, pxb))
				{ histix=i; }
		}
	}
#endif

	if(BTIC5C_EncodeCheckBlock_WithColor(ctx, blk, ctx->clrab))
//	if(0)
	{
		if(BTIC5C_EncodeBlockIsFlatP(ctx, blk))
		{
			BTIC5C_EncodeExtendRunFlatLo(ctx);
			return(0);
		}

		if(BTIC5C_EncodeBlockIsFlatHiP(ctx, blk))
		{
			BTIC5C_EncodeExtendRunFlatHi(ctx);
			return(0);
		}

		if(BTIC5C_EncodeBlockIs221P(ctx, blk))
		{
			BTIC5C_EncodeExtendRun221(ctx, (blk>>32)&15);
			return(0);
		}

		if(BTIC5C_EncodeBlockIs222P(ctx, blk))
		{
			BTIC5C_EncodeExtendRun222(ctx, (blk>>32)&255);
			return(0);
		}

		if(BTIC5C_EncodeBlockIs441P(ctx, blk))
		{
			BTIC5C_EncodeExtendRun441(ctx, (blk>>32)&65535);
			return(0);
		}
		
		BTIC5C_EncodeFlushBlockRuns(ctx);

		if(BTIC5C_EncodeBlockIsPat6P(ctx, blk))
		{
			ctx->stat_blkty[6]++;
			*ctx->ct++=0x00|(((blk>>32)&63)<<2);
			return(0);
		}

//		if(BTIC5C_EncodeBlockIsPatHistP(ctx, blk))
		if(histix>=0)
		{
			ctx->stat_blkty[7]++;
//			dy=(blk>>32)&15;
			dy=histix;
			*ctx->ct++=0x05|(dy<<4);
			if(dy==15)
				ctx->blkhist_pos=(ctx->blkhist_pos-1)&15;
			return(0);
		}
	
//		if(BTIC5C_EncodeBlockIs221P(ctx, blk))
//		{
//			*ctx->ct++=0x03|(((blk>>32)&15)<<4);
//			return(0);
//		}

//		if(BTIC5C_EncodeBlockIs222P(ctx, blk))
//		{
//			*ctx->ct++=0x5F;
//			*ctx->ct++=((blk>>32)&255);
//			return(0);
//		}
//		if(BTIC5C_EncodeBlockIs441P(ctx, blk))
//		{
//			ctx->stat_blkty[5]++;
//			*ctx->ct++=0x1F;
//			*ctx->ct++=((blk>>32)&255);
//			*ctx->ct++=((blk>>40)&255);
//			return(0);
//		}
		if(BTIC5C_EncodeBlockIs442P(ctx, blk))
		{
			ctx->stat_blkty[4]++;

			*ctx->ct++=0x3F;
			*ctx->ct++=((blk>>32)&255);
			*ctx->ct++=((blk>>40)&255);
			*ctx->ct++=((blk>>48)&255);
			*ctx->ct++=((blk>>56)&255);
			BTIC5C_EncodeAddBlockHist442(ctx, blk>>32);
			return(0);
		}
	}

	BTIC5C_EncodeFlushBlockRuns(ctx);

	if(BTIC5C_EncodeBlockIsFlatP(ctx, blk))
	{
		ctx->stat_blkty[0]++;
		*ctx->ct++=0x0B;
		BTIC5C_EncodeBlockEndpoint(ctx, blk);
		return(0);
	}

	if(BTIC5C_EncodeBlockIsFlatHiP(ctx, blk))
	{
		ctx->stat_blkty[1]++;
		*ctx->ct++=0xFB;
		BTIC5C_EncodeBlockEndpoint(ctx, blk);
		return(0);
	}

	if(BTIC5C_EncodeBlockIsPat6P(ctx, blk))
	{
		ctx->stat_blkty[6]++;
		*ctx->ct++=0x02|(((blk>>32)&63)<<2);
		BTIC5C_EncodeBlockEndpoint(ctx, blk);
		return(0);
	}

	if(BTIC5C_EncodeBlockIs221P(ctx, blk))
	{
		ctx->stat_blkty[3]++;
		*ctx->ct++=0x0B|(((blk>>32)&15)<<4);
		BTIC5C_EncodeBlockEndpoint(ctx, blk);
		return(0);
	}

//	if(BTIC5C_EncodeBlockIsPatHistP(ctx, blk))
	if(histix>=0)
	{
		ctx->stat_blkty[7]++;
//		dy=(blk>>32)&15;
		dy=histix;
		*ctx->ct++=0x0D|(dy<<4);
		if(dy==15)
			ctx->blkhist_pos=(ctx->blkhist_pos-1)&15;
		BTIC5C_EncodeBlockEndpoint(ctx, blk);
		return(0);
	}

	if(BTIC5C_EncodeBlockIs222P(ctx, blk))
	{
		ctx->stat_blkty[2]++;
		*ctx->ct++=0xDF;
		*ctx->ct++=((blk>>32)&255);
		BTIC5C_EncodeBlockEndpoint(ctx, blk);
		BTIC5C_EncodeAddBlockHist222(ctx, (blk>>32)&255);
		return(0);
	}
	if(BTIC5C_EncodeBlockIs441P(ctx, blk))
	{
		ctx->stat_blkty[5]++;
		*ctx->ct++=0x9F;
		*ctx->ct++=((blk>>32)&255);
		*ctx->ct++=((blk>>40)&255);
		BTIC5C_EncodeBlockEndpoint(ctx, blk);
		BTIC5C_EncodeAddBlockHist441(ctx, (blk>>32)&65535);
		return(0);
	}
	if(BTIC5C_EncodeBlockIs442P(ctx, blk))
	{
		ctx->stat_blkty[4]++;
		*ctx->ct++=0xBF;
		*ctx->ct++=((blk>>32)&255);
		*ctx->ct++=((blk>>40)&255);
		*ctx->ct++=((blk>>48)&255);
		*ctx->ct++=((blk>>56)&255);
		BTIC5C_EncodeBlockEndpoint(ctx, blk);
		BTIC5C_EncodeAddBlockHist442(ctx, blk>>32);
		return(0);
	}

	return(0);
}

int BTIC5C_EncodeFlushBlock(BTIC5C_EncodeContext *ctx)
{
//	BTIC5C_EncodeFlushBlockRuns(ctx);
	return(0);
}

int BTIC5C_Encode_CompareErrorForBlockPair(u64 bxa, u64 bxb)
{
	u16 pixa[16], pixb[16];
	int i, d;

	BTIC5C_UnpackCellB_RGB555(bxa, pixa, 4);
	BTIC5C_UnpackCellB_RGB555(bxb, pixb, 4);
	
	d=0;
	for(i=0; i<16; i++)
		d+=BTIC5C_EncodeGetColorDist16(pixa[i], pixb[i]);
	return(d);
}

int BTIC5C_EncodeBlockSuper(
	BTIC5C_EncodeContext *ctx, int c_cx, int c_cy)
{
	static const byte xotab[16]=
		{ 0, 1, 1, 0,  0, 0, 1, 1,  2, 2, 3, 3,  3, 2, 2, 3 };
	static const byte yotab[16]=
		{ 0, 0, 1, 1,  2, 3, 3, 2,  2, 3, 3, 2,  1, 1, 0, 0 };
	u64 *blka, *lblka;
	u64 blk, lblk, sk_lb1, sk_lb2;
	int bd, bi, bins, d, d0, noskip, skbi, skiprun;
	int bbx, bby, bbxl, bbyl, bx, by, bxl, byl, bxs, bys, x, y;
	int i, j, k;
	
	blka=ctx->blka;
	lblka=ctx->lblka;
	bxs=ctx->bxs;
	bys=ctx->bys;
	bbx=c_cx*4;	bby=c_cy*4;

	if(ctx->qfl&BTPIC_QFL_PFRAME)
	{
		bi=0; bd=99999999; bins=1;
		for(i=0; i<31; i++)
		{
			bbxl=bbx+btic5c_skip_xoffs[i];
			bbyl=bby+btic5c_skip_yoffs[i];
			if((bbxl<0) || (bbyl<0))
				continue;
			if(((bbxl+4)>bxs) || ((bbyl+4)>bys))
				continue;

			d=0; noskip=0;
			for(y=0; y<4; y++)
				for(x=0; x<4; x++)
			{
				bx=bbx+x;	by=bby+y;
				bxl=bbxl+x;	byl=bbyl+y;
				blk=blka[by*bxs+bx];
				lblk=lblka[byl*bxs+bxl];
				d0=BTIC5C_Encode_CompareErrorForBlockPair(blk, lblk);
				if((d0>>4)>(4*ctx->ld_skip))
					noskip=1;
				d+=d0;
			}
//			if(noskip)
//				continue;
			if(d<bd)
				{ bd=d; bi=i; bins=noskip; }
		}
		
		d=bd>>8;
//		if((d<(ctx->ld_skip*4)) && !bins)
		if((d<(ctx->ld_skip*2)) && !bins)
		{
			BTIC5C_EncodeFlushBlockRuns(ctx);
			
			if(	(ctx->li_skip==bi) &&
				((ctx->ct[-2]&7)==1) &&
				(ctx->ct[-1]<240))
			{
				/* Append onto prior skip. */
				ctx->stat_skipped+=16;
				ctx->ct[-1]+=16;
				return(0);
			}

			/* New Skip */
			ctx->stat_skipped+=16;
			*ctx->ct++=0x01|(bi<<3);
			*ctx->ct++=15;
			ctx->li_skip=bi;
			ctx->li_skipb=bi;
			return(0);
		}

		ctx->li_skipb=bi;
	}

	ctx->li_skip=-1;

	skbi=ctx->li_skipb;
	bbxl=bbx+btic5c_skip_xoffs[skbi];
	bbyl=bby+btic5c_skip_yoffs[skbi];
	skiprun=0;
	sk_lb2=0;
	sk_lb1=0;

	for(i=0; i<16; i++)
	{
		bx=bbx+xotab[i];	by=bby+yotab[i];
		bxl=bbxl+xotab[i];	byl=bbyl+yotab[i];
		blk=blka[by*bxs+bx];

		lblk=~blk;
		if(	(bxl>=0) && (byl>0) &&
			(bxl<bxs) && (byl<bys) &&
			(skbi>=0) && (skbi<31))
				lblk=lblka[byl*bxs+bxl];

#if 0
		if(ctx->qfl&BTPIC_QFL_PFRAME)
		{
			if(i && BTIC5C_EncodeCheckCanSkipBlock(ctx, blk, lblk))
			{
				sk_lb2=sk_lb1;
				sk_lb1=blk;
				skiprun++;
				continue;
			}
			
			if(skiprun)
			{
				if(skiprun>2)
				{
					ctx->stat_skipped+=skiprun;
					*ctx->ct++=0x01|(skbi<<3);
					*ctx->ct++=skiprun-1;
					skiprun=0;
				}else
				{
					if(skiprun==2)
						BTIC5C_EncodeBlockBase(ctx, bx, by, sk_lb2, ~sk_lb2);
					BTIC5C_EncodeBlockBase(ctx, bx, by, sk_lb1, ~sk_lb1);
					skiprun=0;
				}
			}
		}
#endif
		
		BTIC5C_EncodeBlockBase(ctx, bx, by, blk, lblk);
	}

	if(skiprun>2)
	{
		*ctx->ct++=0x01|(skbi<<3);
		*ctx->ct++=skiprun-1;
		skiprun=0;
	}else
		if(skiprun)
	{
		if(skiprun==2)
			BTIC5C_EncodeBlockBase(ctx, bx, by, sk_lb2, ~sk_lb2);
		BTIC5C_EncodeBlockBase(ctx, bx, by, sk_lb1, ~sk_lb1);
		skiprun=0;
	}

	BTIC5C_EncodeFlushBlock(ctx);
	return(0);
}

int BTIC5C_EncodeBlockPlane(
	BTIC5C_EncodeContext *ctx, byte *odat)
{
	int cx, cy, cxs, cys;
	int i, j, k;
	
	for(i=0; i<16; i++)
	{
		ctx->clrhist[i]=0x0000FFFF;
		ctx->blkhist[i]=0x00000000;
	}
	ctx->clrab=0x0000FFFF;
	ctx->li_skip=-1;
	ctx->li_skipb=255;
	
	ctx->ct=odat;
	cxs=ctx->cxs;
	cys=ctx->cys;
	for(cy=0; cy<cys; cy++)
		for(cx=0; cx<cxs; cx++)
	{
		BTIC5C_EncodeBlockSuper(ctx, cx, cy);
	}
	BTIC5C_EncodeFlushBlockRuns(ctx);
	return((ctx->ct)-odat);
}

int BTIC5C_EncodeImageBasic_Px4ToPx2(u16 px)
{
	int px1;
	px1=0;
	if((px&0x0033)==0x0033)px1|=1;
		else if((px&0x0033)!=0x0000)px1=-1;
	if((px&0x00CC)==0x00CC)px1|=2;
		else if((px&0x00CC)!=0x0000)px1=-1;
	if((px&0x3300)==0x3300)px1|=4;
		else if((px&0x3300)!=0x0000)px1=-1;
	if((px&0xCC00)==0xCC00)px1|=8;
		else if((px&0xCC00)!=0x0000)px1=-1;
	return(px1);
}

u64 BTIC5C_EncodeBlockBasic(BTIC5C_EncodeContext *ctx, u16 *iblk, int ystr)
{
	short pxy[16];
	byte picnt[4];

	u64 tblk;

	int min, max;
	int acr, acg, acb, acy, acy_c, acy_m, acy_y;
	int cr, cg, cb, cy;
	int mr, mg, mb, my;
	int nr, ng, nb, ny;

	int mr2, mg2, mb2, my2;
	int nr2, ng2, nb2, ny2;

	int cy_c, cy_m, cy_y;
	int my_c, my_m, my_y;
	int ny_c, ny_m, ny_y;
	int ax, dy;

	int cya, cyb, cyc, cyhi, cylo;
	int ia, ib, ic, ix, px442, px441, px222, px221, px6, px442b;
	int use1bpp, useflat;
	int x, y, z;
	int i, j, k, l;
	
	my=1024; ny=-1024;
	my_c=1024; ny_c=-1024;
	my_m=1024; ny_m=-1024;
	my_y=1024; ny_y=-1024;
	acr=0; acg=0; acb=0; acy=0;
	acy_c=0; acy_m=0; acy_y=0;
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=iblk[i*ystr+j];
		cr=(k>>10)&31;			cg=(k>> 5)&31;			cb=(k>> 0)&31;
		cr=(cr<<3)|(cr>>2);		cg=(cg<<3)|(cg>>2);		cb=(cb<<3)|(cb>>2);

		cy=(cr+2*cg+cb)/4;

		cy_c=(1*cr+4*cg+3*cb)/8;
		cy_m=(4*cr+1*cg+3*cb)/8;
		cy_y=(3*cr+4*cg+1*cb)/8;

		if(cy<my) { my=cy; }
		if(cy>ny) { ny=cy; }
		if(cy_c<my_c) { my_c=cy_c; }
		if(cy_c>ny_c) { ny_c=cy_c; }
		if(cy_m<my_m) { my_m=cy_m; }
		if(cy_m>ny_m) { ny_m=cy_m; }
		if(cy_y<my_y) { my_y=cy_y; }
		if(cy_y>ny_y) { ny_y=cy_y; }
		
		acr+=cr;	acg+=cg;
		acb+=cb;	acy+=cy;
		acy_c+=cy_c;
		acy_m+=cy_m;
		acy_y+=cy_y;
	}
	
	acr=acr/16;	acg=acg/16;
	acb=acb/16;	acy=acy/16;

	acy_c=acy_c/16;
	acy_m=acy_m/16;
	acy_y=acy_y/16;

	ax=0; dy=ny-my;
	i=ny_c-my_c; if(i>dy) { ax=1; dy=i; acy=acy_c; }
	i=ny_m-my_m; if(i>dy) { ax=2; dy=i; acy=acy_m; }
	i=ny_y-my_y; if(i>dy) { ax=3; dy=i; acy=acy_y; }

	mr=0; mg=0; mb=0; my=0; cya=0;
	nr=0; ng=0; nb=0; ny=0; cyb=0;
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=iblk[i*ystr+j];
		cr=(k>>10)&31;			cg=(k>> 5)&31;			cb=(k>> 0)&31;
		cr=(cr<<3)|(cr>>2);		cg=(cg<<3)|(cg>>2);		cb=(cb<<3)|(cb>>2);

		switch(ax)
		{
		case 0: cy=(cr+2*cg+cb)/4; break;
		case 1: cy=(1*cr+4*cg+3*cb)/8; break;
		case 2: cy=(4*cr+1*cg+3*cb)/8; break;
		case 3: cy=(3*cr+4*cg+1*cb)/8; break;
		}

		pxy[i*4+j]=cy;
		
		if(cy<acy)
			{ mr+=cr; mg+=cg; mb+=cb; my+=cy; cya++; }
		else
			{ nr+=cr; ng+=cg; nb+=cb; ny+=cy; cyb++; }
	}

	if(!cya)
		{ mr=nr; mg=ng; mb=nb; my=ny; cya=cyb; }
	if(!cyb)
		{ nr=mr; ng=mg; nb=mb; ny=my; cyb=cya; }

	mr=mr/cya; mg=mg/cya;
	mb=mb/cya; my=my/cya;

	nr=nr/cyb; ng=ng/cyb;
	nb=nb/cyb; ny=ny/cyb;

	my2=1024; ny2=-1024;
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=iblk[i*ystr+j];
		cr=(k>>10)&31;			cg=(k>> 5)&31;			cb=(k>> 0)&31;
		cr=(cr<<3)|(cr>>2);		cg=(cg<<3)|(cg>>2);		cb=(cb<<3)|(cb>>2);

		switch(ax)
		{
		case 0: cy=(cr+2*cg+cb)/4; break;
		case 1: cy=(1*cr+4*cg+3*cb)/8; break;
		case 2: cy=(4*cr+1*cg+3*cb)/8; break;
		case 3: cy=(3*cr+4*cg+1*cb)/8; break;
		}
		
		pxy[i*4+j]=cy;
		if(cy<my2)
			{ my2=cy; mr2=cr; mg2=cg; mb2=cb; }
		if(cy>ny2)
			{ ny2=cy; nr2=cr; ng2=cg; nb2=cb; }
	}
	
	mr=(mr+mr2)/2;
	mg=(mg+mg2)/2;
	mb=(mb+mb2)/2;
	my=(my+my2)/2;

	nr=(nr+nr2)/2;
	ng=(ng+ng2)/2;
	nb=(nb+nb2)/2;
	ny=(ny+ny2)/2;
	
	cr=acr;
	cg=acg;
	cb=acb;
	
	ia=((mr>>3)<<10)|((mg>>3)<<5)|(mb>>3);
	ib=((nr>>3)<<10)|((ng>>3)<<5)|(nb>>3);
	ic=((cr>>3)<<10)|((cg>>3)<<5)|(cb>>3);

	cya=(5*my+3*ny+8*acy)/16;
	cyb=(3*my+5*ny+8*acy)/16;
	cyhi=(5*ny+3*my)/8;
	cylo=(5*my+3*ny)/8;

	picnt[0]=0;
	picnt[1]=0;
	picnt[2]=0;
	picnt[3]=0;

	px442=0;

	for(y=0; y<4; y++)
		for(x=0; x<4; x++)
	{
		z=y*4+x;
		cy=pxy[z];
		cyc=((x^y)&1)?cya:cyb;
		if(cy>cyc)
			{ k=2; if(cy>cyhi) k=3; }
		else
			{ k=1; if(cy<cylo) k=0; }
		px442|=k<<(z*2);
		picnt[k]++;
	}
	
	px441=0;
	if(pxy[ 0]>cya)px441|=0x0001;
	if(pxy[ 1]>cyb)px441|=0x0002;
	if(pxy[ 2]>cya)px441|=0x0004;
	if(pxy[ 3]>cyb)px441|=0x0008;

	if(pxy[ 4]>cyb)px441|=0x0010;
	if(pxy[ 5]>cya)px441|=0x0020;
	if(pxy[ 6]>cyb)px441|=0x0040;
	if(pxy[ 7]>cya)px441|=0x0080;

	if(pxy[ 8]>cya)px441|=0x0100;
	if(pxy[ 9]>cyb)px441|=0x0200;
	if(pxy[10]>cya)px441|=0x0400;
	if(pxy[11]>cyb)px441|=0x0800;

	if(pxy[12]>cyb)px441|=0x1000;
	if(pxy[13]>cya)px441|=0x2000;
	if(pxy[14]>cyb)px441|=0x4000;
	if(pxy[15]>cya)px441|=0x8000;

	px221=0;
	px222=0;
	for(y=0; y<2; y++)
		for(x=0; x<2; x++)
	{
		l=	pxy[(y*2+0)*4+(x*2+0)] + pxy[(y*2+0)*4+(x*2+1)] +
			pxy[(y*2+1)*4+(x*2+0)] + pxy[(y*2+1)*4+(x*2+1)] ;
		l/=4;
		if(l>=acy)
		{
			px221|=1<<(y*2+x);
			k=2;
			if(l>cyhi) k=3;
		}else
		{
			k=1;
			if(l<cylo) k=0;
		}
		px222|=k<<((y*2+x)*2);
	}

	use1bpp=0;
	useflat=0;
	if((picnt[0]+picnt[3])>=(picnt[1]+picnt[2]))
		use1bpp=1;
	if((ny-my)<ctx->ld_flat)
		useflat=1;

	if(use1bpp && ((px441==0x0000) || (useflat && (px221==0))))
	{
		px442=0x00000000U;
		tblk=(ia<<0)|(ib<<16)|(((u64)px442)<<32);
		return(tblk);
	}

	if(use1bpp && ((px441==0xFFFF) || (useflat && (px221==15))))
	{
		px442=0xFFFFFFFFU;
		tblk=(ia<<0)|(ib<<16)|(((u64)px442)<<32);
		return(tblk);
	}

#if 1
//	if((ny-my)<ctx->ld_flat)
	if(useflat)
	{
		ia=ic;
		ib=ic;
		px441=0;
		px442=0;
		tblk=(ic<<0)|(ic<<16);
//		tblk|=0x0000000000008000ULL;
		return(tblk);
	}
#endif

	if(use1bpp && ((ny-my)<ctx->ld_2x2))
	{
		tblk=(ia<<0)|(ib<<16)|(((u64)px221)<<32);
		tblk|=0x0001000000008000ULL;
		return(tblk);
	}

	px6=BTIC5C_Encode_LookupPat6(px442);
	px442b=btic5c_blk_pat6tab[px6];
	k=BTIC5C_Encode_CompareError442(px442, px442b);
	k=(k*(ny-my))/(16*4);
	if(k<ctx->ld_flat)
	{
		tblk=(ia<<0)|(ib<<16)|(((u64)px6)<<32);
		tblk|=0x0003000000008000ULL;
		return(tblk);
	}

	if((ny-my)<ctx->ld_2x2)
//	if(0)
	{
//		if((picnt[0]+picnt[3])>=(picnt[1]+picnt[2]))
//		if(1)
		if(use1bpp)
		{
			tblk=(ia<<0)|(ib<<16)|(((u64)px221)<<32);
			tblk|=0x0001000000008000ULL;
			return(tblk);
		}else
		{
			tblk=(ia<<0)|(ib<<16)|(((u64)px222)<<32);
			tblk|=0x0002000000008000ULL;
			return(tblk);
		}
	}

//	if((picnt[0]+picnt[3])>=(picnt[1]+picnt[2]))
//	if(1)
//	if(0)
	if(use1bpp)
	{
		tblk=(ia<<0)|(ib<<16)|(((u64)px441)<<32);
		tblk|=0x0004000000008000ULL;
		return(tblk);
	}else
	{
		tblk=(ia<<0)|(ib<<16)|(((u64)px442)<<32);
		return(tblk);
	}
}

int BTIC5C_PackCellImage_RGB555(
	BTIC5C_EncodeContext *ctx,
	u64 *blka, int bxs, int bys,
	u16 *ibuf, int ibxs, int ibys, int ystr)
{
	u16 tpix[16];
	u16 *ct, *ct1;
	u64 tblk;
	int bx, by, bz, bxs0, bys0, c0;
	int i, j, k;
	
	bxs0=ibxs>>2;
	bys0=ibys>>2;
	ct=ibuf;
	for(by=0; by<bys0; by++)
	{
		bz=by*bxs; ct1=ct;
		for(bx=0; bx<bxs0; bx++)
		{
			tblk=BTIC5C_EncodeBlockBasic(ctx, ct1, ystr);
			blka[bz]=tblk;
			ct1+=4; bz++;
		}
		
		if(ibxs&3)
		{
			k=*ct1;
			for(i=0; i<16; i++)
				tpix[i]=k;
			memcpy(tpix+ 0, ct1+0*ystr, (ibxs&3)*2);
			memcpy(tpix+ 4, ct1+1*ystr, (ibxs&3)*2);
			memcpy(tpix+ 8, ct1+2*ystr, (ibxs&3)*2);
			memcpy(tpix+12, ct1+3*ystr, (ibxs&3)*2);
			tblk=BTIC5C_EncodeBlockBasic(ctx, tpix, 4);
			blka[bz]=tblk;
			bx++; bz++;
		}

		for(; bx<bxs; bx++)
			{ blka[bz]=tblk; bz++; }
		
		ct+=ystr<<2;
	}
	for(; by<bys; by++)
	{
		bz=by*bxs;
		for(bx=0; bx<bxs; bx++)
			{ blka[bz++]=tblk; }
	}
	return(0);
}

int BTIC5C_GetImageBlock_RGB24(
	u16 *blk,
	byte *ibuf, int ibxs, int ibys, int xstr, int ystrb, int flip)
{
	byte *cs, *cs1;
	int cr, cg, cb, px;
	int x, y;
	
	cs=ibuf;
	for(y=0; y<4; y++)
	{
		cs1=cs;
		for(x=0; x<4; x++)
		{
			if(flip&1)
				{ cr=cs1[0]; cg=cs1[1]; cb=cs1[2]; }
			else
				{ cr=cs1[2]; cg=cs1[1]; cb=cs1[0]; }
			cr>>=3;		cg>>=3;		cb>>=3;
			px=(cr<<10)|(cg<<5)|cb;
			blk[y*4+x]=px;
			cs1+=xstr;
		}
		cs+=ystrb;
	}
	return(0);
}


int BTIC5C_PackCellImage_RGB24(
	BTIC5C_EncodeContext *ctx,
	u64 *blka, int bxs, int bys,
	byte *ibuf, int ibxs, int ibys, int ystr, int clrs)
{
	u16 tpix[16];
	byte *ct, *ct1;
	u64 tblk;
	int xstr, ystrb, flip;
	int bx, by, bz, bxs0, bys0, c0;
	int i, j, k;
	
	flip=0;
	xstr=4;
	if(	(clrs==BTIC4B_CLRS_RGB) ||
		(clrs==BTIC4B_CLRS_BGR)	)
	{
		xstr=3;
	}

	if(clrs==BTIC4B_CLRS_RGBA)		flip=1;
	if(clrs==BTIC4B_CLRS_RGBX)		flip=1;
	if(clrs==BTIC4B_CLRS_RGB)		flip=1;

	ystrb=ystr*xstr;

	bxs0=ibxs>>2;
	bys0=ibys>>2;
	ct=ibuf;
	for(by=0; by<bys0; by++)
	{
		bz=by*bxs; ct1=ct;
		for(bx=0; bx<bxs0; bx++)
		{
			BTIC5C_GetImageBlock_RGB24(tpix,
				ct1, ibxs, ibys, xstr, ystrb, flip);
			tblk=BTIC5C_EncodeBlockBasic(ctx, tpix, 4);
//			tblk=BTIC5C_EncodeBlockBasic(ctx, ct1, ystr);
			blka[bz]=tblk;
			ct1+=4*xstr; bz++;
		}
		
		if(ibxs&3)
		{
			k=*ct1;
			for(i=0; i<16; i++)
				tpix[i]=k;
//			memcpy(tpix+ 0, ct1+0*ystr, (ibxs&3)*2);
//			memcpy(tpix+ 4, ct1+1*ystr, (ibxs&3)*2);
//			memcpy(tpix+ 8, ct1+2*ystr, (ibxs&3)*2);
//			memcpy(tpix+12, ct1+3*ystr, (ibxs&3)*2);
			tblk=BTIC5C_EncodeBlockBasic(ctx, tpix, 4);
			blka[bz]=tblk;
			bx++; bz++;
		}

		for(; bx<bxs; bx++)
			{ blka[bz]=tblk; bz++; }
		
		ct+=ystrb<<2;
	}
	for(; by<bys; by++)
	{
		bz=by*bxs;
		for(bx=0; bx<bxs; bx++)
			{ blka[bz++]=tblk; }
	}
	return(0);
}

byte *BTIC5C_EncodeEmitTwoCC(byte *ict, u16 tcc, void *buf, int sz)
{
	byte *ct;
	int sz1, nsz1;
	
	ct=ict;
	if((sz+8)<32768)
	{
		sz1=sz+4;
		nsz1=sz1^0xFFFF;
		*ct++=(tcc >>0)&0xFF;
		*ct++=(tcc >>8)&0xFF;
		*ct++=(nsz1>>0)&0xFF;
		*ct++=(nsz1>>8)&0xFF;
		memcpy(ct, buf, sz);
		ct+=sz;
		return(ct);
	}

	sz1=sz+8;
	nsz1=sz1^0xFFFFFFFF;
	*ct++=(tcc >> 0)&0xFF;
	*ct++=(tcc >> 8)&0xFF;
	*ct++=0;
	*ct++=0;
	*ct++=(nsz1>> 0)&0xFF;
	*ct++=(nsz1>> 8)&0xFF;
	*ct++=(nsz1>>16)&0xFF;
	*ct++=(nsz1>>24)&0xFF;
	memcpy(ct, buf, sz);
	ct+=sz;
	return(ct);	
}

byte *BTIC5C_EncodeEmitFourCC(byte *ict, u32 fcc, void *buf, int sz)
{
	byte *ct;
	int sz1, nsz1;

	ct=ict;
	sz1=sz+8;
	nsz1=sz1^0xFFFFFFFF;
	*ct++=(fcc >> 0)&0xFF;
	*ct++=(fcc >> 8)&0xFF;
	*ct++=(fcc >>16)&0xFF;
	*ct++=(fcc >>24)&0xFF;
	*ct++=(nsz1>> 0)&0xFF;
	*ct++=(nsz1>> 8)&0xFF;
	*ct++=(nsz1>>16)&0xFF;
	*ct++=(nsz1>>24)&0xFF;
	memcpy(ct, buf, sz);
	ct+=sz;
	return(ct);	
}

int BTIC5C_EncodeFrameImage(BTIC5C_EncodeContext *ctx,
	byte *obuf, int obsz, void *ibuf, int qfl, int clrs)
{
	byte thdb[64];
	u64 *blka;
	byte *ct;
	int tfsz, osz, osz2, tot_clr;
	int i, j, k;

	if(!ctx)
		return(-1);
	if(	(ctx->bxs<   1) || (ctx->bys<   1) ||
		(ctx->bxs>4096) || (ctx->bys>4096) )
			return(-1);
	if(!obuf || !ibuf)
		return(0);

	if(!ctx->blka)
	{
		ctx->blka=malloc(ctx->bxs*ctx->bys*8);
		memset(ctx->blka, 0, ctx->bxs*ctx->bys*8);
	}
	if(!ctx->lblka)
	{
		ctx->lblka=malloc(ctx->bxs*ctx->bys*8);
		memset(ctx->lblka, 0, ctx->bxs*ctx->bys*8);
	}

//	ctx->fl=qfl;
	ctx->qfl=qfl;
	BTIC5C_EncodeContextSetupQuality(ctx, qfl);

	memset(ctx->stat_clrty, 0, sizeof(ctx->stat_clrty));
	memset(ctx->stat_blkty, 0, sizeof(ctx->stat_blkty));
	memset(ctx->stat_longflat, 0, sizeof(ctx->stat_longflat));
	ctx->stat_bytes_clr=0;
	ctx->stat_skipped=0;

	if(qfl&BTPIC_QFL_PFRAME)
	{
		blka=ctx->lblka;
		ctx->lblka=ctx->blka;
		ctx->blka=blka;
	}

	if(	(clrs==BTIC4B_CLRS_RGBA) ||
		(clrs==BTIC4B_CLRS_BGRA) ||
		(clrs==BTIC4B_CLRS_RGBX) ||
		(clrs==BTIC4B_CLRS_BGRX) ||
		(clrs==BTIC4B_CLRS_RGB) ||
		(clrs==BTIC4B_CLRS_BGR))
	{
		BTIC5C_PackCellImage_RGB24(ctx, ctx->blka,
			ctx->bxs, ctx->bys,
			(byte *)ibuf, ctx->xs, ctx->ys, ctx->xs, clrs);
	}else
	{
		BTIC5C_PackCellImage_RGB555(ctx, ctx->blka,
			ctx->bxs, ctx->bys,
			ibuf, ctx->xs, ctx->ys, ctx->xs);
	}

	tfsz=BTIC5C_EncodeBlockPlane(ctx, ctx->tfbuf);
	if(tfsz<=0)
		return(0);

	tot_clr=	ctx->stat_clrty[0]+ctx->stat_clrty[1]+
				ctx->stat_clrty[2]+ctx->stat_clrty[3]+
				ctx->stat_clrty[4]+ctx->stat_clrty[6];
	k=(ctx->bxs*ctx->bys)-tot_clr;
		;
	printf(	"ClrTy: RGBD=%d Delta=%d Repeat=%d Pair=%d\n"
			"\tRepeat2=%d Delta2=%d None=%d\n",
		ctx->stat_clrty[0],
		ctx->stat_clrty[1],
		ctx->stat_clrty[2],
		ctx->stat_clrty[3],
		ctx->stat_clrty[4],
		ctx->stat_clrty[5],
		k);

	printf("BlkTy: FlatLo=%d FlatHi=%d 2x2x2=%d 2x2x1=%d\n"
			"\t4x4x2=%d 4x4x1=%d Pat6=%d Repeat=%d Skip=%d\n",
		ctx->stat_blkty[0],
		ctx->stat_blkty[1],
		ctx->stat_blkty[2],
		ctx->stat_blkty[3],
		ctx->stat_blkty[4],
		ctx->stat_blkty[5],
		ctx->stat_blkty[6],
		ctx->stat_blkty[7],
		ctx->stat_skipped);

	for(i=0; i<8; i++)
	{
		j=	ctx->stat_runs[i][0] + ctx->stat_runs[i][1] +
			ctx->stat_runs[i][2] + ctx->stat_runs[i][3] ;
		if(!j)
			continue;
		printf("Runs %d: %d %d %d %d\n",
			i,
			ctx->stat_runs[i][0], ctx->stat_runs[i][1],
			ctx->stat_runs[i][2], ctx->stat_runs[i][3]);
	}

	printf("  Longflat: %d %d %d %d\n",
			ctx->stat_longflat[0], ctx->stat_longflat[1],
			ctx->stat_longflat[2], ctx->stat_longflat[3]);

	if(tot_clr<=0)
		tot_clr=1;

	printf("Bytes Color %d (avg %.2f), %.2f%%\n",
		ctx->stat_bytes_clr,
		(1.0*ctx->stat_bytes_clr)/tot_clr,
		(100.0*ctx->stat_bytes_clr)/tfsz);

	ct=obuf;
	
	if(!(qfl&BTPIC_QFL_PFRAME))
	{
		thdb[0]=((ctx->xs)>>0)&255;
		thdb[1]=((ctx->xs)>>8)&255;
		thdb[2]=((ctx->ys)>>0)&255;
		thdb[3]=((ctx->ys)>>8)&255;
		thdb[4]=((ctx->fl)>>0)&255;
		thdb[5]=((ctx->fl)>>8)&255;

		ct=BTIC5C_EncodeEmitTwoCC(ct, BTPIC_TCC_HX, thdb, 6);
		ct=BTIC5C_EncodeEmitTwoCC(ct, BTPIC_TCC_IX, ctx->tfbuf, tfsz);
	}else
	{
		ct=BTIC5C_EncodeEmitTwoCC(ct, BTPIC_TCC_PX, ctx->tfbuf, tfsz);
	}

	osz=ct-obuf;

	if(!ctx->zfbuf)
	{
		k=osz+(osz>>3)+256;
		j=4096;
		while(j<k)
			j=j+(j>>1);
		ctx->zfbuf=malloc(j);
		ctx->zfbsz=j;
	}

	k=osz+(osz>>3)+256;
	if(k>ctx->zfbsz)
	{
		j=ctx->zfbsz;
		while(j<k)
			j=j+(j>>1);
		ctx->zfbuf=realloc(ctx->zfbuf, j);
		ctx->zfbsz=j;
	}

	osz2=GfxEdit_EncodeRP2(ctx->zfbuf+12, obuf, ctx->zfbsz, osz);
	k=osz2+12;
	j=~k;
	ctx->zfbuf[ 0]='Z';
	ctx->zfbuf[ 1]='3';
	ctx->zfbuf[ 2]=0;
	ctx->zfbuf[ 3]=0;
	ctx->zfbuf[ 4]=j  >> 0;
	ctx->zfbuf[ 5]=j  >> 8;
	ctx->zfbuf[ 6]=j  >>16;
	ctx->zfbuf[ 7]=j  >>24;
	ctx->zfbuf[ 8]=osz>> 0;
	ctx->zfbuf[ 9]=osz>> 8;
	ctx->zfbuf[10]=osz>>16;
	ctx->zfbuf[11]=osz>>24;
	osz2+=12;

	if((osz2*1.20)<osz)
	{
		memcpy(obuf, ctx->zfbuf, osz2);
		osz=osz2;
	}

	return(osz);
}
