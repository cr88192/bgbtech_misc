typedef struct BTIC5B_EncodeContext_s BTIC5B_EncodeContext;

struct BTIC5B_EncodeContext_s {
	u64 *blka;
	u64 *lblka;
	int bxs;
	int bys;

	u16 *clra_a;
	u16 *clra_b;
	byte *clra_t;

	int xs;
	int ys;
	int fl;
	int ystr;

	u16 clra;
	u16 clrb;
	u16 clrc;
	u16 lclrc;
	byte clrdy;

	int ld_flat;
	int ld_2x2;
	int ld_skip;
	int ld_joint;

	int stat_cd15;
	int stat_cd5;
	int stat_cdp;
	
	int stat_flat;
	int stat_px2;
	int stat_px4;

	int stat_pxp6;

	u16		*imgbuf;

	byte *tfbuf;		//temp encoded frame buffer	
	byte *zfbuf;		//LZ Frame temporary buffer
	byte *zf2buf;		//LZ Frame temporary buffer
	int zfbsz;			//LZ Frame buffer size
};

#if 0
int btpic_clamp255(int x)
{
	if(x<0)return(x);
	if(x>255)return(255);
	return(x);
}
#endif

BTIC5B_EncodeContext *BTIC5B_AllocEncodeContext(void)
{
	BTIC5B_EncodeContext *ctx;
	ctx=malloc(sizeof(BTIC5B_EncodeContext));
	memset(ctx, 0, sizeof(BTIC5B_EncodeContext));
	return(ctx);
}

int BTIC5B_FreeEncodeContext(BTIC5B_EncodeContext *ctx)
{
	free(ctx);
	return(0);
}

int BTIC5B_EncodeContextSetupQuality(BTIC5B_EncodeContext *ctx, int qfl)
{
	int qf;

	qf=qfl&127;
	if(qf>100)qf=100;
//	ctx->ld_flat=100-qf;
	ctx->ld_flat=50-(qf/2);
//	ctx->ld_2x2=200-(qf*2);
	ctx->ld_2x2=100-qf;
//	ctx->ld_skip=200-(qf*2);
	ctx->ld_skip=100-qf;
//	ctx->ld_joint=320-(qf*2);
//	ctx->ld_joint=200-(qf*2);
	ctx->ld_joint=100-qf;
//	ctx->ld_joint=220-(qf*2);
	return(0);
}

int BTIC5B_EncodeInitContext(BTIC5B_EncodeContext *ctx,
	int xs, int ys, int qfl)
{
	int qf;
		
	ctx->xs=xs;
	ctx->ys=ys;
	ctx->bxs=(xs+3)>>2;
	ctx->bys=(ys+3)>>2;
	ctx->ystr=xs;
	ctx->fl=0;

	ctx->blka=malloc(ctx->bxs*ctx->bys*sizeof(u64));
	ctx->lblka=malloc(ctx->bxs*ctx->bys*sizeof(u64));
//	ctx->fl|=1;

	ctx->clra_a=malloc(ctx->bxs*ctx->bys*sizeof(u16));
	ctx->clra_b=malloc(ctx->bxs*ctx->bys*sizeof(u16));
	ctx->clra_t=malloc(ctx->bxs*ctx->bys*sizeof(byte));
	
	ctx->tfbuf=malloc(xs*ys*4);
	
	BTIC5B_EncodeContextSetupQuality(ctx, qfl);
	
	return(0);
}

static byte bt5b_ebitcnt[16]=
	{ 0, 1, 1, 2,  1, 2, 2, 3,  1, 2, 2, 3,  2, 3, 3, 4 };

void BTIC5B_InitDeltasEncode()
{
	u16 pbma[64];
	int x, y, xn1, xp1, yn1, yp1, ix, p;
	int pxn1, pxp1, pyn1, pyp1;
	int pxe, pbi, pbe;
	int t0, t1;

	u16 px1, px1b, pxm;
	u32 px2;
	int i, j, k, l;

	BTIC5B_InitDeltas();
	
	if(bt5b_pat6lut)
		return;
		
	bt5b_pat6lut=malloc(65536);
	memset(bt5b_pat6lut, 0xFF, 65536);

	t0=clock();

	/*
	 * Fill pattern lookup with 1-pixel flips.
	 * Generate a table of which pixels can flip.
	 */
	for(i=0; i<64; i++)
	{
		px1=bt5b_pat6[i];
		px2=bt5b_pat6x2[i];
		bt5b_pat6lut[px1]=i;

		pxm=0;		
		for(y=0; y<4; y++)
			for(x=0; x<4; x++)
		{
			ix=y*4+x;
			p=(px2>>(ix*2))&3;

			if((p==1) || (p==2))
			{
				px1b=px1^(1<<ix);
				pxm|=(1<<ix);
				if(bt5b_pat6lut[px1b]==0xFF)
					bt5b_pat6lut[px1b]=i;
			}

			xn1=(x>0)?(x-1):x;	xp1=(x<3)?(x+1):x;
			yn1=(y>0)?(y-1):y;	yp1=(y<3)?(y+1):y;
			
			pxn1=(px2>>((y*4+xn1)*2))&3;	pxp1=(px2>>((y*4+xp1)*2))&3;
			pyn1=(px2>>((yn1*4+x)*2))&3;	pyp1=(px2>>((yp1*4+x)*2))&3;

			if((pxn1!=pxp1) || (pyn1!=pyp1))
			{
				px1b=px1^(1<<ix);
				pxm|=(1<<ix);
				if(bt5b_pat6lut[px1b]==0xFF)
					bt5b_pat6lut[px1b]=i;
			}
		}
		pbma[i]=pxm;
	}

	/*
	 * Now, try filling in the lookup table with other flips.
	 */
#if 1
	for(j=0; j<65536; j++)
	{
		if(bt5b_pat6lut[j]!=0xFF)
			continue;

		pbi=0; pbe=999;

		for(i=0; i<64; i++)
		{
			pxm=pbma[i];
			px1=bt5b_pat6[i];
//			px2=bt5b_pat6x2[i];

			if((j^px1)&(~pxm))
				continue;

//			k=j&pxm;
//			if(k==l)
//				continue;

			pxe=j^px1;
			k=	bt5b_ebitcnt[(pxe   )&15]+bt5b_ebitcnt[(pxe>> 4)&15]+
				bt5b_ebitcnt[(pxe>>8)&15]+bt5b_ebitcnt[(pxe>>12)&15];
			if(k<pbe)
				{ pbi=i; pbe=k; }

//			l=k;
			
//			px1b=px1^k;

//			if(bt5b_pat6lut[px1b]==0xFF)
//				bt5b_pat6lut[px1b]=i;
		}

		bt5b_pat6lut[j]=pbi;
	}
#endif

#if 0
	for(i=0; i<64; i++)
	{
		px1=bt5b_pat6[i];
		px2=bt5b_pat6x2[i];
		pxm=pbma[i];
		
		l=-1;
		
		for(j=0; j<65536; j++)
		{
			k=j&pxm;
			if(k==l)
				continue;
			l=k;
			
			px1b=px1^k;

			if(bt5b_pat6lut[px1b]==0xFF)
				bt5b_pat6lut[px1b]=i;
		}
	}
#endif

	k=0;
	for(j=0; j<65536; j++)
	{
		if(bt5b_pat6lut[j]==0xFF)
			k++;
	}
	
	t1=clock();
	
	printf("BTIC5B_InitDeltasEncode: Took %d, Unset=%d\n", t1-t0, k);
}

int BTIC5B_EncodeImageBasic_Px4ToPx2(u16 px)
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

int BTIC5B_EncodeBlockBasic(BTIC5B_EncodeContext *ctx,
	u16 *iblk, int ystr, int *opx, int *oclra, int *oclrb)
{
	short pxy[16];
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

	int cya, cyb;
	int ia, ib, ic, ix, px;
	int x, y;
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
//		cr=iblk[i*ystr+j*4+0];
//		cg=iblk[i*ystr+j*4+1];
//		cb=iblk[i*ystr+j*4+2];

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
//		cr=iblk[i*ystr+j*4+0];
//		cg=iblk[i*ystr+j*4+1];
//		cb=iblk[i*ystr+j*4+2];
//		cy=(cr+2*cg+cb)/4;

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
	
//	if(!cya)cya++;
//	if(!cyb)cyb++;

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
//		cr=iblk[i*ystr+j*4+0];
//		cg=iblk[i*ystr+j*4+1];
//		cb=iblk[i*ystr+j*4+2];

		k=iblk[i*ystr+j];
		cr=(k>>10)&31;			cg=(k>> 5)&31;			cb=(k>> 0)&31;
		cr=(cr<<3)|(cr>>2);		cg=(cg<<3)|(cg>>2);		cb=(cb<<3)|(cb>>2);

//		cy=(cr+2*cg+cb)/4;

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
	
//	cr=(mr+nr)/2;
//	cg=(mg+ng)/2;
//	cb=(mb+nb)/2;

	cr=acr;
	cg=acg;
	cb=acb;
	
	ia=((mr>>3)<<10)|((mg>>3)<<5)|(mb>>3);
	ib=((nr>>3)<<10)|((ng>>3)<<5)|(nb>>3);
	ic=((cr>>3)<<10)|((cg>>3)<<5)|(cb>>3);
	
//	ia=BTIC5B_EncodeBlockRGB2Index(ctx, mr, mg, mb);
//	ib=BTIC5B_EncodeBlockRGB2Index(ctx, nr, ng, nb);
//	ic=BTIC5B_EncodeBlockRGB2Index(ctx, acr, acg, acb);

//	ix=(ia<<4)|ib;
	
//	cya=(3*my+1*ny)/4;
//	cyb=(1*my+3*ny)/4;

//	cya=(5*my+3*ny)/8;
//	cyb=(3*my+5*ny)/8;

	cya=(5*my+3*ny+8*acy)/16;
	cyb=(3*my+5*ny+8*acy)/16;
	
	px=0;
	if(pxy[ 0]>cya)px|=0x0001;
	if(pxy[ 1]>cyb)px|=0x0002;
	if(pxy[ 2]>cya)px|=0x0004;
	if(pxy[ 3]>cyb)px|=0x0008;

	if(pxy[ 4]>cyb)px|=0x0010;
	if(pxy[ 5]>cya)px|=0x0020;
	if(pxy[ 6]>cyb)px|=0x0040;
	if(pxy[ 7]>cya)px|=0x0080;

	if(pxy[ 8]>cya)px|=0x0100;
	if(pxy[ 9]>cyb)px|=0x0200;
	if(pxy[10]>cya)px|=0x0400;
	if(pxy[11]>cyb)px|=0x0800;

	if(pxy[12]>cyb)px|=0x1000;
	if(pxy[13]>cya)px|=0x2000;
	if(pxy[14]>cyb)px|=0x4000;
	if(pxy[15]>cya)px|=0x8000;

	if((ny-my)<ctx->ld_2x2)
	{
		px=0;
		for(y=0; y<2; y++)
			for(x=0; x<2; x++)
		{
			l=	pxy[(y*2+0)*4+(x*2+0)] + pxy[(y*2+0)*4+(x*2+1)] +
				pxy[(y*2+1)*4+(x*2+0)] + pxy[(y*2+1)*4+(x*2+1)] ;
			l/=4;
			if(l>=acy)
				px|=0x0033<<(y*8+x*2);
		}
	}

	if((ny-my)<ctx->ld_flat)
//	if(1)
	{
		ia=ic;
		ib=ic;
		px=0;
	}

	*opx=px;
	*oclra=ia;
	*oclrb=ib;
	return(0);
}

int	BTIC5B_EncodeGetColorDist16(u16 ca, u16 cb)
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

//	d=(d*85)>>8;

//	d=(dr*dr)+(dg*dg)+(db*db);
//	d=(d*148)>>8;
//	d=(d*148)>>12;
	return(d);
}

int BTIC5B_EncodeCheckCanSkipBlock(BTIC5B_EncodeContext *ctx,
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

//	px0=(blk0>>32)&65535;	ca0=(blk0>>16)&255;		cb0=(blk0>>24)&255;
//	px1=(blk1>>32)&65535;	ca1=(blk1>>16)&255;		cb1=(blk1>>24)&255;
//	ca2=ctx->pal[ca0];		ca3=ctx->pal[ca1];
//	cb2=ctx->pal[cb0];		cb3=ctx->pal[cb1];

	px0=(blk0>>32)&65535;	ca0=(blk0>> 0)&65535;	cb0=(blk0>>16)&65535;
	px1=(blk1>>32)&65535;	ca1=(blk1>> 0)&65535;	cb1=(blk1>>16)&65535;
	
//	d0=BTIC5B_EncodeGetColorDist16(ca2, cb3);
//	d1=BTIC5B_EncodeGetColorDist16(ca3, cb2);

	d0=BTIC5B_EncodeGetColorDist16(ca0, cb1);
	d1=BTIC5B_EncodeGetColorDist16(ca1, cb0);

	if((d0<ctx->ld_skip) && (d1<ctx->ld_skip))
		return(1);

#if 1
	d2=BTIC5B_EncodeGetColorDist16(ca0, ca1);
	d3=BTIC5B_EncodeGetColorDist16(cb0, cb1);

//	if((d0>=ctx->ld_skip) && (d1>=ctx->ld_skip))
//	if(	(d0>=ctx->ld_skip) && (d1>=ctx->ld_skip) &&
//		(d2>=ctx->ld_skip) && (d3>=ctx->ld_skip))
//			return(0);
	
	blk2=(((u64)px0)<<16)|(((u64)ca0)<<32)|(((u64)cb0)<<48)|3;
	blk3=(((u64)px1)<<16)|(((u64)ca1)<<32)|(((u64)cb1)<<48)|3;
	BTIC5B_DecodeImageBlock(blk2, tblka0, 4);
	BTIC5B_DecodeImageBlock(blk3, tblka1, 4);
	
	d0=0;
	for(i=0; i<16; i++)
		{ d0+=BTIC5B_EncodeGetColorDist16(tblka0[i], tblka1[i]); }
	d0=d0/16;
	
	if(d0<ctx->ld_skip)
//	if(d0<ctx->ld_flat)
		return(1);
#endif

	return(0);
}

int BTIC5B_EncodeEndpoint_RGBD15(BTIC5B_EncodeContext *ctx,
	int clra, int clrb)
{
	int cra, cga, cba, cya;
	int crb, cgb, cbb, cyb;
	int cr, cg, cb, cy;
	int cv, cv0, cv1;
	int i, j, n;
	
	cra=(clra>>10)&31;	cga=(clra>> 5)&31;	cba=(clra>> 0)&31;
	crb=(clrb>>10)&31;	cgb=(clrb>> 5)&31;	cbb=(clrb>> 0)&31;
	cr=(cra+crb)/2;		cg=(cga+cgb)/2;		cb=(cba+cbb)/2;
	
	cya=(2*cga+cra+cba)/4;
	cyb=(2*cgb+crb+cbb)/4;
	cy=(cya-cyb)/2;
	
	if(cy<0)
		cy=-cy;
	
	if(cr<31)cr+=1;
	if(cg<31)cg+=1;
	if(cb<31)cb+=1;
	
	cr=(cr&0x1E)|((cy>>3)&1);
	cg=(cg&0x1E)|((cy>>2)&1);
	cb=(cb&0x1E)|((cy>>1)&1);
	cv=(cr<<10)|(cg<<5)|cb;
	
	cya=(cv&0x7BDE)+bt5b_dystep[(cy>>1)&7];
	cyb=(cv&0x7BDE)-bt5b_dystep[(cy>>1)&7];

	j=cya|cyb;
	if((j&0x7BDE)!=j)
	{
		cv0=cv;
		
		for(i=0; i<16; i++)
		{
			j=cya|cyb;
			if((j&0x7BDE)==j)
				break;
			
			cv1=cv;
		
			if((((cya&cyb)&0x8420) || (i>8)) && (cy>0))
			{
				cy--;

				cr=(cr&0x1E)|((cy>>3)&1);
				cg=(cg&0x1E)|((cy>>2)&1);
				cb=(cb&0x1E)|((cy>>1)&1);
				cv=(cr<<10)|(cg<<5)|cb;
				
				cya=(cv&0x7BDE)+bt5b_dystep[(cy>>1)&7];
				cyb=(cv&0x7BDE)-bt5b_dystep[(cy>>1)&7];
				continue;
			}

			if(cya&0x8000)	cv-=0x0800;
			if(cya&0x0400)	cv-=0x0040;
			if(cya&0x0020)	cv-=0x0002;

			cya=(cv&0x7BDE)+bt5b_dystep[(cy>>1)&7];
			cyb=(cv&0x7BDE)-bt5b_dystep[(cy>>1)&7];

			j=cya|cyb;
			if((j&0x7BDE)==j)
				break;

			if(cyb&0x8000)	cv+=0x0800;
			if(cyb&0x0400)	cv+=0x0040;
			if(cyb&0x0020)	cv+=0x0002;

			cya=(cv&0x7BDE)+bt5b_dystep[(cy>>1)&7];
			cyb=(cv&0x7BDE)-bt5b_dystep[(cy>>1)&7];

			j=cya|cyb;
			if((j&0x7BDE)==j)
				break;
				
//			cv=cv1;
			if(cy>0)
				cy--;

			cr=(cr&0x1E)|((cy>>3)&1);
			cg=(cg&0x1E)|((cy>>2)&1);
			cb=(cb&0x1E)|((cy>>1)&1);
			cv=(cr<<10)|(cg<<5)|cb;
			
			cya=(cv&0x7BDE)+bt5b_dystep[(cy>>1)&7];
			cyb=(cv&0x7BDE)-bt5b_dystep[(cy>>1)&7];
		}
	}

	j=cya|cyb;
//	if(	((cya&0x7BDE)!=cya) ||
//		((cyb&0x7BDE)!=cyb))
	if((j&0x7BDE)!=j)
	{
		printf("Encode RGBD5: Overflow %04X %04X\n", clra, clrb);
		cv&=0x7BDE;
	}
	
	return(cv);
}

int BTPIC_DecodeEndpoint_RGBD15(int rdv, int *rca, int *rcb)
{
	int cc, ca, cb, cdy, j;

	cc=rdv&0x7BDE;
	cdy=((rdv>>0)&1)|((rdv>>4)&2)|((rdv>>8)&4);
	ca=cc+bt5b_dystep[cdy];
	cb=cc-bt5b_dystep[cdy];
	
//	if(((ca&0x7BDE)!=ca))

	j=ca|cb;
	if((j&0x7BDE)!=j)
	{
		printf("BT5B-E: Decode RGBD5: Overflow %04X %04X\n", ca, cb);
//		cv&=0x7BDE;
	}
	
	*rca=ca;
	*rcb=cb;
	return(0);
}

int BTIC5B_EncodeEndpoint_FindDeltaRGBD15(
	BTIC5B_EncodeContext *ctx,
	int cda, int cdy, int lcda, int lcdy, int lcda2)
{
	int cra, cga, cba, cya;
	int crb, cgb, cbb, cyb;
	int dr, dg, db;
	int dy, dv, cdp;
	
	if((lcdy<0) || (lcdy>=8))
		return(-1);
	
	dy=(cdy-lcdy)+1;
	if((dy<0) || (dy>2))
//	if(dy!=1)
		return(-1);

#if 0
	cya=(lcda*2)-lcda2;
	if(cya==cda)
	{
		dv=0x1B;
		dv=dv*4+dy;
		return(dv);
	}
#endif

	cra=(cda >>10)&31;	cga=(cda >> 5)&31;	cba=(cda >> 0)&31;
	crb=(lcda>>10)&31;	cgb=(lcda>> 5)&31;	cbb=(lcda>> 0)&31;

	dr=(cra-crb)>>1;
	dg=(cga-cgb)>>1;
	db=(cba-cbb)>>1;
	dr+=1;	dg+=1;	db+=1;
//	if((dr<0) || (dg<0) || (db<0))
//		return(-1);
//	if((dr>2) || (dg>2) || (db>2))
//		return(-1);

	if(	(dr<0) || (dg<0) || (db<0) ||
		(dr>2) || (dg>2) || (db>2) )
	{
		cya=(lcda*2)-lcda2;
		if((cya==cda) && ((cya&0x7BDE)==cya))
		{
			dv=0x1B;
			dv=dv*4+dy;
			return(dv);
		}

		return(-1);
	}

	dv=(dr*9)+(dg*3)+db;
	if((dv<0) || (dv>0x1A))
		return(-1);

//	cdp=lcda+bt5b_delta5[dv];
	cdp=(lcda&0x7BDE)+bt5b_delta5[dv];
	if((cdp&0x7BDE)!=cdp)
		return(-1);

	dv=dv*4+dy;
	return(dv);
}

int BTIC5B_EncodeEndpointSkipP(BTIC5B_EncodeContext *ctx,
	int clra, int clrb)
{
	int lcda, lcdy, lcda2, cda, cdy, cd;
	int cdd, dab, clr2a, clr2b;
	int cd2a, clr3a, clr3b, cd2d;

	if((clra==ctx->clra) && (clrb==ctx->clrb))
		return(1);

//	return(0);

#if 1
	lcda=ctx->clrc;
	lcda2=ctx->lclrc;
	lcdy=ctx->clrdy;

	cda=BTIC5B_EncodeEndpoint_RGBD15(ctx, clra, clrb);
	cdy=((cda>>0)&1)|((cda>>4)&2)|((cda>>8)&4);

	cd=BTIC5B_EncodeEndpoint_FindDeltaRGBD15(ctx,
		cda&0x7BDE, cdy, lcda, lcdy, lcda2);

	dab=BTIC5B_EncodeGetColorDist16(clra, clrb);

	BTPIC_DecodeEndpoint_RGBD15(cda, &clr2a, &clr2b);
	cdd =	BTIC5B_EncodeGetColorDist16(clra, clr2a) +
			BTIC5B_EncodeGetColorDist16(clrb, clr2b);

	cd2a=lcda+bt5b_delta5[cd>>2];
	clr3a=cd2a+bt5b_dystep[cdy];
	clr3b=cd2a-bt5b_dystep[cdy];

	cd2d =	BTIC5B_EncodeGetColorDist16(clra, clr3a) +
			BTIC5B_EncodeGetColorDist16(clrb, clr3b);

//	if((cdd<ctx->ld_joint) || (dab<ctx->ld_flat) || (clra==clrb))
	if(ctx->ld_flat>=4)
	{
//		if((cd>=0) && (lcdy<8) && (cd2d<ctx->ld_flat))
		if((cd>=0) && (lcdy<8))
		{
			if(cd==((1*9+1*3+1)*4+1))
				return(1);
		}
	}
#endif

	return(0);
}

int BTIC5B_EncodeEndpoint(BTIC5B_EncodeContext *ctx,
	byte **rct, int clra, int clrb)
{
	byte *ct;
	int cd, cda, cdd, cpa, dab, cdy;
	int clr2a, clr2b;
	int cd2a, clr3a, clr3b, cd2d;
	int lcda, lcdy, lcda2;
	
	ct=*rct;
	
	cda=BTIC5B_EncodeEndpoint_RGBD15(ctx, clra, clrb);
	cdy=((cda>>0)&1)|((cda>>4)&2)|((cda>>8)&4);

	dab=BTIC5B_EncodeGetColorDist16(clra, clrb);

	BTPIC_DecodeEndpoint_RGBD15(cda, &clr2a, &clr2b);
	cdd =	BTIC5B_EncodeGetColorDist16(clra, clr2a) +
			BTIC5B_EncodeGetColorDist16(clrb, clr2b);

	lcda=ctx->clrc;
	lcda2=ctx->lclrc;
	lcdy=ctx->clrdy;

	cd=BTIC5B_EncodeEndpoint_FindDeltaRGBD15(ctx,
		cda&0x7BDE, cdy, lcda, lcdy, lcda2);

	cd2a=lcda+bt5b_delta5[cd>>2];
	if((cd>>2)==0x1B)
		cd2a=2*lcda-lcda2;
	
	clr3a=cd2a+bt5b_dystep[cdy];
	clr3b=cd2a-bt5b_dystep[cdy];

	if((cd2a&0x7BDE)!=cd2a)		cd=-1;
	if((clr3a&0x7BDE)!=clr3a)	cd=-1;
	if((clr3b&0x7BDE)!=clr3b)	cd=-1;

	cd2d =	BTIC5B_EncodeGetColorDist16(clra, clr3a) +
			BTIC5B_EncodeGetColorDist16(clrb, clr3b);

//	if(1)
//	if(0)
//	if(cdd<ctx->ld_joint)
//	if((cdd<ctx->ld_joint) || (dab<ctx->ld_flat) || (clra==clrb))
//	if(((cdd/2)<ctx->ld_joint) || (dab<ctx->ld_flat) || (clra==clrb))
//	if(ctx->ld_flat>=8)
	if(ctx->ld_flat>=4)
	{
//		if((cd>=0) && (lcdy<8) && (cd2d<ctx->ld_flat))
		if((cd>=0) && (lcdy<8))
//		if(0)
		{
			*ct++=(cd<<1)|1;

			ctx->lclrc=ctx->clrc;
//			ctx->lclrc=lcda;
			
			ctx->clra=clr3a;
			ctx->clrb=clr3b;
//			ctx->clrc=cd2a&0x7BDE;
			ctx->clrc=cd2a;
			ctx->clrdy=cdy;
			ctx->stat_cd5++;
		}else
		{
			ctx->lclrc=ctx->clrc;
//			ctx->lclrc=lcda;
		
			cpa=cda<<1;
			*ct++=cpa>>0;
			*ct++=cpa>>8;
			ctx->clra=clr2a;
			ctx->clrb=clr2b;
			ctx->clrc=cda&0x7BDE;
			ctx->clrdy=cdy;
			ctx->stat_cd15++;
		}

	}else
	{
		cpa=((clra&0x7FFF)<<17)|((clrb&0x7FFE)<<2)|7;
		*ct++=cpa>> 0;
		*ct++=cpa>> 8;
		*ct++=cpa>>16;
		*ct++=cpa>>24;

		ctx->clra=clra;
		ctx->clrb=clrb;
		ctx->clrc=0;
		ctx->clrdy=255;
		ctx->stat_cdp++;
	}

	*rct=ct;
	return(0);
}

int BTPIC_QuantizeEndpoint(BTIC5B_EncodeContext *ctx,
	int lca, int lcb, int *rca, int *rcb, int *rctag)
{
	int ca, cb, dla, dlb, dl, dab;

	*rctag=0;
	ca=*rca;
	cb=*rcb;
	
	if((lca<0) || (lcb<0))
		return(0);
	
	dab=BTIC5B_EncodeGetColorDist16(ca, cb);
	dla=BTIC5B_EncodeGetColorDist16(ca, lca);
	dlb=BTIC5B_EncodeGetColorDist16(cb, lcb);
	dl=dla+dlb;
	
//	if((dla<=4) && (dlb<=4))
//	if((dl*3)<ctx->ld_flat)
	if((dl*2)<ctx->ld_flat)
//	if((dl*1)<ctx->ld_flat)
//	if(((dl*2)<ctx->ld_flat) || (dab<ctx->ld_flat))
//	if(0)
	{
		ca=lca;
		cb=lcb;

		*rctag=1;
		*rca=ca;
		*rcb=cb;
		return(1);
	}

#if 0
	if((ca&0x7BDE)==(lca&0x7BDE))
		ca=lca;
	if((cb&0x7BDE)==(lcb&0x7BDE))
		cb=lcb;
	*rca=ca;
	*rcb=cb;
#endif

	return(0);
}

int BTIC5B_EncodeCheckBlockPat6(BTIC5B_EncodeContext *ctx,
	u16 px, u16 ca, u16 cb)
{
	int cd, ce;
	int i, j, k, px1, pxe, pbi, pbe;
	
	i=bt5b_pat6lut[px];
	if(i!=0xFF)
	{
		px1=bt5b_pat6[i];
		if(px1==px)
			return(i);
		
		pbi=i;

		pxe=px^px1;
		pbe=	bt5b_ebitcnt[(pxe   )&15]+bt5b_ebitcnt[(pxe>> 4)&15]+
				bt5b_ebitcnt[(pxe>>8)&15]+bt5b_ebitcnt[(pxe>>12)&15];

		cd=BTIC5B_EncodeGetColorDist16(ca, cb);
		ce=pbe*(cd>>4);

		if(ce<ctx->ld_2x2)
			return(pbi);
		
		return(-1);
	}
	
	pbi=0; pbe=999;
	for(i=0; i<64; i++)
	{
		px1=bt5b_pat6[i];
		if(px1==px)
			return(i);
		pxe=px^px1;
		j=	bt5b_ebitcnt[(pxe   )&15]+bt5b_ebitcnt[(pxe>> 4)&15]+
			bt5b_ebitcnt[(pxe>>8)&15]+bt5b_ebitcnt[(pxe>>12)&15];
		if(j<pbe)
			{ pbi=i; pbe=j; }
	}

	bt5b_pat6lut[px]=pbi;

	cd=BTIC5B_EncodeGetColorDist16(ca, cb);
	ce=pbe*(cd>>4);

	if(ce<ctx->ld_2x2)
		return(pbi);
	
	return(-1);
}

static int btic5b_mvpat[(5*4)*2] = 
{
 1, 0,   0, 1,  -1,  0,  0, -1,
 1, 1,  -1, 1,  -1, -1,  1, -1,
 2, 0,   0, 2,  -2,  0,  0, -2,
 2, 1,  -1, 2,  -2, -1,  1, -2,
 2, 2,  -2, 2,  -2, -2,  2, -2,
};

int BTIC5B_EncodeImageBasic(BTIC5B_EncodeContext *ctx,
	byte *obuf, int obsz, u16 *img, int ystr, int qfl, int clrs)
{
	u64 *blka, *lblka;
	u16 *clra_a, *clra_b;
	byte *clra_t;
	byte *ct;
	u16 *cs;
	int px, px1, px2, px3;
	int i0, i1, i2, i3;
	s16 p2x0, p2x1, p2x2, p2x3;
	u64 blk, blk0, blk1, blk2, blk3;
	u32 ui;
	int ca, cb, lca, lcb, lcc;
	int ca1, ca2, ca3, cb1, cb2, cb3;
	int bxs, bys;
	int nfca, nfcb, nfcc, nfcfl;
	int dvx, dvy, dvx1, dvy1, ldvx, ldvy, dvofs, dskip;
	int x, y, n, mm, mcm, mdm, m2x, mfc;
	int i, j, k, k1;
		
	BTIC5B_EncodeContextSetupQuality(ctx, qfl);

//	BTIC5B_InitDeltas();
	BTIC5B_InitDeltasEncode();

	blka=ctx->blka;
	lblka=ctx->lblka;
	
	clra_a=ctx->clra_a;
	clra_b=ctx->clra_b;
	clra_t=ctx->clra_t;
	
	if(qfl&BTPIC_QFL_PFRAME)
	{
		bxs=ctx->bxs;
		bys=ctx->bys;

		blka=ctx->blka;
		lblka=ctx->lblka;
		memcpy(lblka, blka, bxs*bys*sizeof(u64));
	}else
	{
		blka=ctx->blka;
		lblka=NULL;
	}
	
	bxs=ctx->bxs;	bys=ctx->bys;	n=0;
	for(y=0; y<bys; y++)
	{
		cs=img+(y*4)*ystr;
		for(x=0; x<bxs; x++)
		{
			BTIC5B_EncodeBlockBasic(ctx, cs, ystr, &px, &cb, &ca);
			cs+=4;
			
			ca=ca&0x7FFF;
			cb=cb&0x7FFF;
			
			px=~px;
			if(ca==cb)
				px=0;
			
			blk=(((u64)px)<<32)|
				(((u64)cb)<<16)|
				(((u64)ca)<< 0);
			blka[n]=blk;

			clra_a[n]=ca;
			clra_b[n]=cb;

			n++;
		}
	}
	
	n=bxs*bys;

#if 0
	lca=0;	lcb=0;
	for(i=0; i<n; i++)
	{
		ca=clra_a[i];
		cb=clra_b[i];
		px=blka[i]>>32;

		BTPIC_QuantizeEndpoint(ctx, lca, lcb, &ca, &cb, &j);
		clra_a[i]=ca;
		clra_b[i]=cb;
		clra_t[i]=j;
		lca=ca;
		lcb=cb;

		blk=(((u64)px)<<32)|
			(((u64)cb)<<16)|
			(((u64)ca)<< 0);
		blka[i]=blk;
	}
#endif

#if 1
	if(lblka)
	{
		dvx=0; dvy=0;
		ldvx=0; ldvy=0;
		for(i=0; i<n; i++)
		{
			blk=blka[i];

			blk1=lblka[i];
			if(BTIC5B_EncodeCheckCanSkipBlock(ctx, blk1, blk))
			{
//				blka[i]=blk1;
				dvx=0;		dvy=0;
				blka[i]=0x8000|((dvy&255)<<16)|((dvx&255)<<0);
				ldvx=0;		ldvy=0;
				continue;
			}

			dvofs=dvy*bxs+dvx;
			k=i+dvofs;
			if((k<0) || (k>=n))
			{
				dvx=0;		dvy=0;
				dvofs=0;
				k=i;
			}

			blk1=lblka[k];
			if(BTIC5B_EncodeCheckCanSkipBlock(ctx, blk1, blk))
			{
//				blka[i]=blk1;
				blka[i]=0x8000|((dvy&255)<<16)|((dvx&255)<<0);
				ldvx=dvx;	ldvy=dvy;
				continue;
			}
			
			if(ctx->fl&1)
//			if(1)
				continue;
			
			for(j=0; j<(5*4); j++)
			{
				dvx1=ldvx+btic5b_mvpat[j*2+0];
				dvy1=ldvy+btic5b_mvpat[j*2+1];
				
				if(((signed char)(dvx1*8))!=(dvx1*8))
					continue;
				if(((signed char)(dvy1*8))!=(dvy1*8))
					continue;

				dvofs=dvy1*bxs+dvx1;
				k=i+dvofs;
				if((k<0) || (k>=n))
					continue;

				x=(dvx1-ldvx)+3;
				y=(dvy1-ldvy)+3;
				if((x<0) || (x>6))
					continue;
				if((y<0) || (y>6))
					continue;

				blk1=lblka[k];
				if(BTIC5B_EncodeCheckCanSkipBlock(ctx, blk1, blk))
				{
					blka[i]=0x8000|((dvy1&255)<<16)|((dvx1&255)<<0);
					dvx=dvx1;		dvy=dvy1;
					ldvx=dvx1;		ldvy=dvy1;
					break;
				}
			}
		}
	}
#endif

	dvx=0; dvy=0;
	ldvx=0;	ldvy=0;
	ct=obuf; lca=-1; lcb=-1; lcc=-1;
	nfca=0; nfcb=0; nfcc=0;
	for(i=0; i<n; i++)
	{
#if 1
		blk0=blka[i+0];		blk1=blka[i+1];
		blk2=blka[i+2];		blk3=blka[i+3];
//		px =(blk0>>32)&65535;	ca =(blk0>>16)&255;		cb =(blk0>>24)&255;
//		px1=(blk1>>32)&65535;	ca1=(blk1>>16)&255;		cb1=(blk1>>24)&255;
//		px2=(blk2>>32)&65535;	ca2=(blk2>>16)&255;		cb2=(blk2>>24)&255;
//		px3=(blk3>>32)&65535;	ca3=(blk3>>16)&255;		cb3=(blk3>>24)&255;

		px =(blk0>>32)&65535;	ca =(blk0>> 0)&65535;	cb =(blk0>>16)&65535;
		px1=(blk1>>32)&65535;	ca1=(blk1>> 0)&65535;	cb1=(blk1>>16)&65535;
		px2=(blk2>>32)&65535;	ca2=(blk2>> 0)&65535;	cb2=(blk2>>16)&65535;
		px3=(blk3>>32)&65535;	ca3=(blk3>> 0)&65535;	cb3=(blk3>>16)&65535;
#endif

		BTPIC_QuantizeEndpoint(ctx, lca, lcb, &ca, &cb, &j);
		BTPIC_QuantizeEndpoint(ctx, ca , cb , &ca1, &cb1, &j);
		BTPIC_QuantizeEndpoint(ctx, ca1, cb1, &ca2, &cb2, &j);
		BTPIC_QuantizeEndpoint(ctx, ca2, cb2, &ca3, &cb3, &j);

//		if((ca==255) || (cb==255))
//			{ __debugbreak(); }

		dskip=0;

#if 1
//		if(lblka)
		if(lblka && (blk0&0x8000))
		{
			dvx=(signed char)(blk0    );
			dvy=(signed char)(blk0>>16);
		
			dvofs=dvy*bxs+dvx;
			for(j=0; (i+j)<n; j++)
			{
				blk2=blka[i+j];

				k=i+j+dvofs;
				if((k<0) || (k>=n))
					break;

				if(blk2&0x8000)
				{
					dvx1=(signed char)(blk2    );
					dvy1=(signed char)(blk2>>16);
					if((dvx1!=dvx) || (dvy1!=dvy))
						break;
				}else
				{
//					break;

					if(dvofs)
						break;
				
					blk3=lblka[k];
					if(blk2!=blk3)
						break;
				}

				blk3=lblka[k];
				blka[i+j]=blk3;
			}
			dskip=j;

//			if(!(blk0&0x8000))
//				dskip=0;
		}else if(lblka)
		{
			if((ldvx==0) && (ldvy==0))
			{
				for(j=0; (i+j)<n; j++)
				{
					blk2=blka[i+j];
					blk3=lblka[i+j];
					if(blk2!=blk3)
						break;
				}
				dskip=j;
			}
		}
#endif

//		if(px&1)
//			{ j=ca; ca=cb; cb=j; px=~px; }
		
		nfcfl=0;
		if(!px)
		{
			if(lca!=lcb)
			{
				if((ca==lca) && nfcb)	nfcfl=1;
				if((ca==lcb) && nfca)	nfcfl=1;

				if((ca!=lca) && (ca!=lcb))
//				if((ca!=lca) && (cb!=lcb))
					nfcfl=1;
			}else
			{
				if(ca!=lca)
					nfcfl=1;
			}

//			nfcfl=1;
		}else
		{
			nfcfl=1;
		}
		
		if(dskip)
			nfcfl=1;

		if((i+4)>=n)
			nfcfl=1;

#if 1
		if(nfcfl)
		{
			if(nfca)
			{
				j=nfca;
//				while(j>=16)
//					{ *ct++=0x3F; j-=16; }

#if 1
				while(j>=8)
//				if(j>8)
				{
					k=(j-8)/4;
					if(k>15)k=15;
					*ct++=0xFF;
					*ct++=0x03|(k<<4);
					j-=(k*4)+8;
				}
#endif

				while(j>=4)
					{ *ct++=0xCF; j-=4; }
				if(j)
					{ *ct++=0x0F|((j-1)<<6); }
				nfca=0;
			}

			if(nfcb)
			{
				j=nfcb;

//				while(j>=16)
//					{ *ct++=0xBF; j-=16; }

				while(j>=4)
					{ *ct++=0xEF; j-=4; }
				if(j)
					{ *ct++=0x2F|((j-1)<<6); }
				nfcb=0;
			}
		}
#endif

		if(dskip)
		{
			if(nfca || nfcb)
				printf("BT5B: Skip and in run\n");

			if((dvx!=ldvx) || (dvy!=ldvy))
			{
				if((dvx==0) && (dvy==0))
				{
					k=dskip-1;
					if(k>6)k=6;
					*ct++=0xE1|(k<<2);
					i+=k;

					ldvx=0;
					ldvy=0;
					continue;
				}
				
				x=(dvx-ldvx)+3;
				y=(dvy-ldvy)+3;
				if((x<0) || (y<0) || (x>6) || (y>6))
				{
					__debugbreak();
				}

				*ct++=0x01|(x<<2)|(y<<5);
				ldvx=dvx;
				ldvy=dvy;
				continue;
			}

			j=dskip;
			if(j>=1)
			{
				k=j;
				while(k>=256)
				{
					*ct++=0xFD;
					*ct++=256-4;
					k-=256;
				}

				if(k>0)
				{
					k1=k-1;
					if(k>=8)
					{
						*ct++=0xFD;
						*ct++=k-4;
					}else
					{
						*ct++=0x1D|(k1<<5);
					}
				}

				i+=j-1;
				continue;
			}
		}

		mfc=0;
		if(ca ==cb )	mfc|=1;
		if(ca1==cb1)	mfc|=2;
		if(ca2==cb2)	mfc|=4;
		if(ca3==cb3)	mfc|=8;

		mm=0;
		if((ca ==lca) && (cb ==lcb))	mm|=1;
		if((ca1==lca) && (cb1==lcb))	mm|=2;
		if((ca2==lca) && (cb2==lcb))	mm|=4;
		if((ca3==lca) && (cb3==lcb))	mm|=8;

		mcm=1;
		if((ca1==ca) && (cb1==cb))	mcm|=2;
		if((ca2==ca) && (cb2==cb))	mcm|=4;
		if((ca3==ca) && (cb3==cb))	mcm|=8;

		mdm=0;
		if((ca ==lca) && (cb ==lcb))	mdm|=1;
		if((ca1==ca ) && (cb1==cb ))	mdm|=2;
		if((ca2==ca1) && (cb2==cb1))	mdm|=4;
		if((ca3==ca2) && (cb3==cb2))	mdm|=8;

		if((i+4)>=n)
		{
			mfc=0;
			mm=0;
			mcm=0;
			mdm=0;
		}

#if 1
		if(!px)
		{
			ctx->stat_flat++;

#if 1
//			if(ca==lca)
			if((ca==lca) && !nfcb && mcm)
				{ nfca++; continue; }
//			if(ca==lcb)
			if((ca==lcb) && !nfca && mcm)
//			if((cb==lcb) && !nfca && mcm)
				{ nfcb++; continue; }
#endif

			if(nfca || nfcb)
				printf("BT5B: Still in run, A\n");

#if 1
//			if((mfc==15) && (mcm!=15))
			if((mfc==15) && (mdm==15))
			{
				ctx->stat_flat+=3;
				i+=3;
				*ct++=0x1F|(3<<6);
				BTIC5B_EncodeEndpoint(ctx, &ct, ca, cb);
				BTIC5B_EncodeEndpoint(ctx, &ct, ca1, cb1);
				BTIC5B_EncodeEndpoint(ctx, &ct, ca2, cb2);
				BTIC5B_EncodeEndpoint(ctx, &ct, ca3, cb3);
//				lca=ca3;
//				lcb=cb3;

				lca=ctx->clra;
				lcb=ctx->clrb;

				continue;
			}

//			if((mfc==7) && (mcm!=7))
			if((mfc==7) && (mdm==7))
			{
				ctx->stat_flat+=2;
				i+=2;
				*ct++=0x1F|(2<<6);
				BTIC5B_EncodeEndpoint(ctx, &ct, ca, cb);
				BTIC5B_EncodeEndpoint(ctx, &ct, ca1, cb1);
				BTIC5B_EncodeEndpoint(ctx, &ct, ca2, cb2);
//				lca=ca2;
//				lcb=cb2;
				lca=ctx->clra;
				lcb=ctx->clrb;
				continue;
			}

//			if((mfc==3) && (mcm!=3))
			if((mfc==3) && (mdm==3))
			{
				ctx->stat_flat++;
				i++;
				*ct++=0x1F|(1<<6);
				BTIC5B_EncodeEndpoint(ctx, &ct, ca, cb);
				BTIC5B_EncodeEndpoint(ctx, &ct, ca1, cb1);
//				lca=ca1;
//				lcb=cb1;
				lca=ctx->clra;
				lcb=ctx->clrb;
				continue;
			}
#endif

#if 1
			*ct++=0x0B;
			BTIC5B_EncodeEndpoint(ctx, &ct, ca, cb);
//			lca=ca;
//			lcb=cb;
			lca=ctx->clra;
			lcb=ctx->clrb;
			continue;
#endif

		}
#endif

		if(nfca || nfcb)
			printf("BT5B: Still in run, B\n");

#if 1

		m2x=0;
		p2x0=BTIC5B_EncodeImageBasic_Px4ToPx2(px);
		p2x1=BTIC5B_EncodeImageBasic_Px4ToPx2(px1);
		p2x2=BTIC5B_EncodeImageBasic_Px4ToPx2(px2);
		p2x3=BTIC5B_EncodeImageBasic_Px4ToPx2(px3);
		if(p2x0>=0)m2x|=1;
		if(p2x1>=0)m2x|=2;
		if(p2x2>=0)m2x|=4;
		if(p2x3>=0)m2x|=8;
		m2x=m2x&mm;

#if 1
		if(m2x&1)
		{
#if 1
			if((m2x&15)==15)
			{
				ctx->stat_px2+=4;

				i+=3;
//				*ct++=0x17|(2<<5);
				*ct++=0x17|(1<<6);
				*ct++=p2x0|(p2x1<<4);
				*ct++=p2x2|(p2x3<<4);
				continue;
			}
#endif

			ctx->stat_px2++;
			*ct++=0x03|(p2x0<<4);
			continue;
		}
#endif

#if 1
		if(mm&1)
		{
#if 1
			j=BTIC5B_EncodeCheckBlockPat6(ctx, px, ca, cb);
			if(j>=0)
			{
				ctx->stat_pxp6++;
//				*ct++=0x01|(j<<2);
				*ct++=0x00|(j<<2);
				continue;
			}
#endif

			if((mm&15)==15)
			{
				ctx->stat_px4+=4;

				i+=3;
				*ct++=0x07|(3<<6);
				*ct++=px;	*ct++=px>>8;
				*ct++=px1;	*ct++=px1>>8;
				*ct++=px2;	*ct++=px2>>8;
				*ct++=px3;	*ct++=px3>>8;
				continue;
			}
			if((mm&7)==7)
			{
				ctx->stat_px4+=3;

				i+=2;
				*ct++=0x07|(2<<6);
				*ct++=px;	*ct++=px>>8;
				*ct++=px1;	*ct++=px1>>8;
				*ct++=px2;	*ct++=px2>>8;
				continue;
			}
			if((mm&3)==3)
			{
				ctx->stat_px4+=2;

				i++;
				*ct++=0x07|(1<<6);
				*ct++=px;	*ct++=px>>8;
				*ct++=px1;	*ct++=px1>>8;
				continue;				
			}

			ctx->stat_px4++;
			*ct++=0x07|(0<<6);
			*ct++=px;	*ct++=px>>8;
			continue;
		}
#endif

#endif

#if 1
		p2x0=BTIC5B_EncodeImageBasic_Px4ToPx2(px);
		if(p2x0>=0)
//		if(0)
		{
			ctx->stat_px2++;

			if(BTIC5B_EncodeEndpointSkipP(ctx, ca, cb))
			{
				*ct++=0x03|(p2x0<<4);
				continue;
			}else
			{
				*ct++=0x0B|(p2x0<<4);
				BTIC5B_EncodeEndpoint(ctx, &ct, ca, cb);
//				lca=ca;
//				lcb=cb;
				lca=ctx->clra;
				lcb=ctx->clrb;
				continue;
			}
		}
#endif

//		if(!lblka)
		if(1)
		{
			i0=BTIC5B_EncodeCheckBlockPat6(ctx, px , ca , cb );
//			i1=BTIC5B_EncodeCheckBlockPat6(ctx, px1, ca1, cb1);
//			i2=BTIC5B_EncodeCheckBlockPat6(ctx, px2, ca2, cb2);
//			i3=BTIC5B_EncodeCheckBlockPat6(ctx, px3, ca3, cb3);

#if 1
			if(i0>=0)
			{
				ctx->stat_pxp6++;

				if(BTIC5B_EncodeEndpointSkipP(ctx, ca, cb))
//				if(0)
//				if(1)
				{
					*ct++=(i0<<2)|0;
				}else
				{
					*ct++=(i0<<2)|2;
					BTIC5B_EncodeEndpoint(ctx, &ct, ca, cb);
//					lca=ca;
//					lcb=cb;
					lca=ctx->clra;
					lcb=ctx->clrb;
				}
				continue;
			}
#endif

		}

#if 0
		if(1)
		{
			ctx->stat_px4++;

			*ct++=(px>>0)&0xFE;
			*ct++=(px>>8)&0xFF;
			BTIC5B_EncodeEndpoint(ctx, &ct, ca, cb);
			lca=ca;
			lcb=cb;
			continue;
		}
#endif

#if 1
		if(1)
		{
			ctx->stat_px4++;

			if(BTIC5B_EncodeEndpointSkipP(ctx, ca, cb))
//			if(0)
			{
				*ct++=0x07;
			}else
			{
				*ct++=0x27;
				BTIC5B_EncodeEndpoint(ctx, &ct, ca, cb);
//				lca=ca;
//				lcb=cb;
				lca=ctx->clra;
				lcb=ctx->clrb;
			}

			*ct++=(px>>0)&0xFF;
			*ct++=(px>>8)&0xFF;
			continue;
		}
#endif
	}
	
	return(ct-obuf);
}

byte *BTIC5B_EncodeEmitTwoCC(byte *ict, int tcc, void *buf, int sz)
{
	byte *ct;
	int sz1;
	
	ct=ict;
	
	if((sz+4)<8192)
	{
		sz1=sz+4;
		*ct++=0x00|(sz1>>8);
		*ct++=sz1;
		*ct++=tcc;
		*ct++=tcc>>8;
		if(buf)
			{ memcpy(ct, buf, sz); }
		else
			{ memset(ct, 0, sz); }
		ct+=sz;
		return(ct);
	}

	if((sz+5)<2097152)
	{
		sz1=sz+5;
		*ct++=0x80|(sz1>>16);
		*ct++=(sz1>>8);
		*ct++=sz1;
		*ct++=tcc;
		*ct++=tcc>>8;
		if(buf)
			{ memcpy(ct, buf, sz); }
		else
			{ memset(ct, 0, sz); }
		ct+=sz;
		return(ct);
	}

	if((sz+6)<536870912)
	{
		sz1=sz+5;
		*ct++=0x40|(sz1>>24);
		*ct++=(sz1>>16);
		*ct++=(sz1>>8);
		*ct++=sz1;
		*ct++=tcc;
		*ct++=tcc>>8;
		if(buf)
			{ memcpy(ct, buf, sz); }
		else
			{ memset(ct, 0, sz); }
		ct+=sz;
		return(ct);
	}
	
	return(NULL);
}

#if 0
int BTIC5B_EncodeFrameImageA(BTIC5B_EncodeContext *ctx,
	byte *obuf, int obsz, u16 *ibuf, int qfl, int clrs)
{
	byte tbuf[256];
	byte *ct;

	int tfsz;

	tfsz=BTIC5B_EncodeImageBasic(ctx, ctx->tfbuf, 1<<22,
		ibuf, ctx->ystr, qfl, clrs);

	ct=obuf;
	
	if(!(qfl&BTPIC_QFL_PFRAME))
	{
		ct=BTIC5B_EncodeEmitTwoCC(ct, BTPIC_TCC_IX, ctx->tfbuf, tfsz);

//		ct=BTIC5B_EncodeEmitTwoCC(ct, BTPIC_TCC_PT, ctx->pal, 512);

		btpic_setu16(tbuf+0, ctx->xs);
		btpic_setu16(tbuf+2, ctx->ys);
		btpic_setu16(tbuf+4, ctx->fl);
		ct=BTIC5B_EncodeEmitTwoCC(ct, BTPIC_TCC_HX, tbuf, 8);
	}else
	{
		ct=BTIC5B_EncodeEmitTwoCC(ct, BTPIC_TCC_PX, ctx->tfbuf, tfsz);
	}

	return(ct-obuf);
}
#endif

#if 1
int BTIC5B_EncodeFrameImageA(BTIC5B_EncodeContext *ctx,
	byte *obuf, int obsz, void *ibuf, int qfl, int clrs)
{
	byte tbuf[256];
	byte *ct;
	byte *ib8;
	u16 *ib16;
	u32 *ib32;
	int px, ystr, flip;
	int x, x1, y, y1;
	int i, j, k;

	int tfsz;

	if(!ctx->imgbuf)
	{
		ctx->imgbuf=malloc((ctx->bxs*4)*(ctx->bys*4)*2);
	}
	
	flip=0;
	if(qfl&BTIC4B_QFL_FLIP)
		flip=1;
	
	if(clrs==BTIC4B_CLRS_RGB555)
	{
		ystr=ctx->bxs*4;
		ib16=ibuf;

//		memcpy(ctx->imgbuf, ibuf, ctx->xs*ctx->ys*2);
		for(y=0; y<ctx->ys; y++)
		{
			y1=flip?(ctx->ys-y-1):y;
			memcpy(ctx->imgbuf+y*ystr, ib16+y1*ctx->xs, ctx->xs*2);

			if(ystr!=ctx->xs)
			{
				memcpy(
					ctx->imgbuf+y*ystr+ctx->xs,
					ctx->imgbuf+y*ystr,
					(ystr-ctx->xs)*2);
			}
		}

		for(y=ctx->ys; y<(ctx->bys*4); y++)
		{
			memcpy(
				ctx->imgbuf+(y  )*ystr,
				ctx->imgbuf+(y-1)*ystr,
				ystr*2);
		}
	}else
		if(	(clrs==BTIC4B_CLRS_RGBA) ||	(clrs==BTIC4B_CLRS_BGRA) ||
			(clrs==BTIC4B_CLRS_RGBX) ||	(clrs==BTIC4B_CLRS_BGRX) )
	{
		ystr=ctx->bxs*4;
		ib32=ibuf;
		for(y=0; y<ctx->ys; y++)
		{
			y1=flip?(ctx->ys-y-1):y;
			for(x=0; x<ctx->xs; x++)
			{
				px=ib32[y1*ctx->xs+x];
				if((clrs==BTIC4B_CLRS_RGBA) || (clrs==BTIC4B_CLRS_RGBX))
				{
					px=(px&0xFF00FF00)|
						((px<<16)&0x00FF0000)|
						((px>>16)&0x000000FF);
				}
				px=((px>>9)&0x7C00)|((px>>6)&0x03E0)|((px>>3)&0x001F);
				ctx->imgbuf[y*ystr+x]=px;
			}

			if(ystr!=ctx->xs)
			{
				memcpy(
					ctx->imgbuf+y*ystr+ctx->xs,
					ctx->imgbuf+y*ystr,
					(ystr-ctx->xs)*2);
			}	
		}

		for(y=ctx->ys; y<(ctx->bys*4); y++)
		{
			memcpy(
				ctx->imgbuf+(y  )*ystr,
				ctx->imgbuf+(y-1)*ystr,
				ystr*2);
		}
	}else
		if(	(clrs==BTIC4B_CLRS_RGB) ||	(clrs==BTIC4B_CLRS_BGR) )
	{
		ystr=ctx->bxs*4;
		ib8=ibuf;
		for(y=0; y<ctx->ys; y++)
		{
			y1=flip?(ctx->ys-y-1):y;
			for(x=0; x<ctx->xs; x++)
			{
				px=btpic_getu32(ib8+(y1*ctx->xs+x)*3);
				if(clrs==BTIC4B_CLRS_RGB)
				{
					px=(px&0xFF00FF00)|
						((px<<16)&0x00FF0000)|
						((px>>16)&0x000000FF);
				}
				px=((px>>9)&0x7C00)|((px>>6)&0x03E0)|((px>>3)&0x001F);
				ctx->imgbuf[y*ystr+x]=px;
			}

			if(ystr!=ctx->xs)
			{
				memcpy(
					ctx->imgbuf+y*ystr+ctx->xs,
					ctx->imgbuf+y*ystr,
					(ystr-ctx->xs)*2);
			}	
		}

		for(y=ctx->ys; y<(ctx->bys*4); y++)
		{
			memcpy(
				ctx->imgbuf+(y  )*ystr,
				ctx->imgbuf+(y-1)*ystr,
				ystr*2);
		}
	}

//	tfsz=BTIC5B_EncodeImageBasic(ctx, ctx->tfbuf, 1<<22, ibuf,
//		ctx->ystr, qfl, clrs);

	tfsz=BTIC5B_EncodeImageBasic(ctx, ctx->tfbuf, 1<<22,
		ctx->imgbuf, ystr, qfl, clrs);

	ct=obuf;
	
	if(!(qfl&BTPIC_QFL_PFRAME))
	{
		ct=BTIC5B_EncodeEmitTwoCC(ct, BTPIC_TCC_IX, ctx->tfbuf, tfsz);

//		ct=BTIC5B_EncodeEmitTwoCC(ct, BTPIC_TCC_PT, ctx->pal, 512);

		btpic_setu16(tbuf+0, ctx->xs);
		btpic_setu16(tbuf+2, ctx->ys);
		btpic_setu16(tbuf+4, ctx->fl);
		ct=BTIC5B_EncodeEmitTwoCC(ct, BTPIC_TCC_HX, tbuf, 8);
	}else
	{
		ct=BTIC5B_EncodeEmitTwoCC(ct, BTPIC_TCC_PX, ctx->tfbuf, tfsz);
	}

	return(ct-obuf);
}
#endif

int BTIC5B_EncodeFrameImage(BTIC5B_EncodeContext *ctx,
	byte *obuf, int obsz, void *ibuf, int qfl, int clrs)
{
	TgvLz_Context *zctx;
	byte *tbuf, *t2buf;
	int tsz, t2sz, sz;

	tbuf=ctx->zfbuf;
	t2buf=ctx->zf2buf;
	if(!tbuf)
	{
		tbuf=malloc(obsz);
		t2buf=malloc(obsz);

		ctx->zfbuf=tbuf;
		ctx->zf2buf=t2buf;
	}

	tsz=BTIC5B_EncodeFrameImageA(ctx, tbuf, obsz, ibuf, qfl, clrs);

//	tsz=cram_repak_buf(ibuf, tbuf+4, isz, isz*2, vstat);
//	set_u32le(tbuf, 0x00000000|tsz);
	
	zctx=TgvLz_CreateContext();
	TgvLz_SetLevel(zctx, 4);
	t2sz=TgvLz_DoEncode(zctx, tbuf, t2buf+10, tsz);
	TgvLz_DestroyContext(zctx);
	sz=t2sz+10;
	t2buf[0]=0x40|(sz>>24);
	t2buf[1]=sz>>16;
	t2buf[2]=sz>> 8;
	t2buf[3]=sz>> 0;
	t2buf[4]='Z';
	t2buf[5]='3';
	set_u32le(t2buf+6, tsz);
	t2sz=sz;
	
	if((t2sz*1.3)<tsz)
//	if(0)
	{
		sz=t2sz;
		memcpy(obuf, t2buf, sz);
	}else
	{
		sz=tsz;
		memcpy(obuf, tbuf, sz);
	}
	
//	free(tbuf);
//	free(t2buf);
	return(sz);
}
