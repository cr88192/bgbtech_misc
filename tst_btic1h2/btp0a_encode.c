// #define BTPIC_QFL_PFRAME	0x100
#define BTPIC_QFL_PFRAME	BTIC4B_QFL_PFRAME

typedef struct BTPIC0A_EncodeContext_s BTPIC0A_EncodeContext;

struct BTPIC0A_EncodeContext_s {
	u64 *blka;
	u64 *lblka;
	int bxs;
	int bys;
	u16		pal[256];
	byte	cmatab[8*65536];
	byte	lutab[256];
	
	u32		clrstat[32768];
	u16		*imgbuf;

	int xs;
	int ys;
	int fl;
	int ystr;

	int ld_flat;
	int ld_2x2;
	int ld_skip;
	
	byte *tfbuf;		//temp encoded frame buffer

	byte *zfbuf;		//LZ Frame temporary buffer
	byte *zf2buf;		//LZ Frame temporary buffer
	int zfbsz;			//LZ Frame buffer size
};

int btpic_clamp255(int x)
{
	if(x<0)return(x);
	if(x>255)return(255);
	return(x);
}

BTPIC0A_EncodeContext *BTPIC0A_AllocEncodeContext(void)
{
	BTPIC0A_EncodeContext *ctx;
	ctx=malloc(sizeof(BTPIC0A_EncodeContext));
	memset(ctx, 0, sizeof(BTPIC0A_EncodeContext));
	return(ctx);
}

int BTPIC0A_FreeEncodeContext(BTPIC0A_EncodeContext *ctx)
{
	free(ctx);
	return(0);
}

int BTPIC_InitDefaultPalette(u16 *pal)
{
	int cy, cu, cv;
	int cr, cg, cb;
	int i, j, k;

	cr=255;
	cg=0;
	cb=255;
	cr>>=3;		cg>>=3;		cb>>=3;
	j=(cr<<10)|(cg<<5)|cb;
	for(i=0; i<256; i++)
		{ pal[i]=j; }

#if 1
//	for(i=0; i<32; i++)
	for(i=0; i<16; i++)
	{
		cr=(i&4)?191:0;
		cg=(i&2)?191:0;
		cb=(i&1)?191:0;
		if(i&8)
		{
			cr+=64;
			cg+=64;
			cb+=64;
		}
		
		if(i&16)
		{
			cr=255-cr;
			cg=255-cg;
			cb=255-cb;
		}
		
		cr>>=3;		cg>>=3;		cb>>=3;
		j=(cr<<10)|(cg<<5)|cb;
		pal[i]=j;

//		k=(i<<1)|(k>>3);
//		k=((i*480)>>8)+1;
		k=i*2+1;
		j=(k<<10)|(k<<5)|k;
		pal[16+i]=j;
	}
#endif

#if 1
	for(i=0; i<216; i++)
	{
		cr=((i/36)%6)*51;
		cg=((i/ 6)%6)*51;
		cb=((i   )%6)*51;

		cr>>=3;		cg>>=3;		cb>>=3;
		j=(cr<<10)|(cg<<5)|cb;
		pal[32+i]=j;
	}
#endif

#if 0
	for(i=0; i<216; i++)
	{
		cr=(((i/36)%6)+1)*36;
		cg=(((i/ 6)%6)+1)*36;
		cb=(((i   )%6)+1)*36;

//		cr=(((i/36)%6)+2)*28;
//		cg=(((i/ 6)%6)+2)*28;
//		cb=(((i   )%6)+2)*28;

		cr>>=3;		cg>>=3;		cb>>=3;
		j=(cr<<10)|(cg<<5)|cb;
		pal[32+i]=j;
	}
#endif

#if 0
	for(i=0; i<223; i++)
//	for(i=0; i<239; i++)
	{
//		cy=(i/25)*31;
		cy=(i/25)*32;
//		cy=(i/25)*28;
//		cv=(((i/5)%5)-2)*51+128;
//		cu=(((i  )%5)-2)*51+128;
		cv=(((i/5)%5)-2)*51;
		cu=(((i  )%5)-2)*51;

//		cv=(((i/5)%5)-2)*46;
//		cu=(((i  )%5)-2)*46;

//		cv=(((i/5)%5)-2)*48;
//		cu=(((i  )%5)-2)*48;

//		cv=(((i/5)%5)-2)*44;
//		cu=(((i  )%5)-2)*44;
//		cv=(((i/5)%5)-2)*32;
//		cu=(((i  )%5)-2)*32;
//		cv=(((i/5)%5)-2)*38;
//		cu=(((i  )%5)-2)*38;
	
		cg=cy-((cu+cv)>>2);
		cb=cg+cu;
		cr=cg+cv;

		cr=btpic_clamp255(cr);
		cg=btpic_clamp255(cg);
		cb=btpic_clamp255(cb);

//		cr=((i/36)%6)*51;
//		cg=((i/ 6)%6)*51;
//		cb=((i   )%6)*51;

		cr>>=3;		cg>>=3;		cb>>=3;
		j=(cr<<10)|(cg<<5)|cb;
//		pal[16+i]=j;
		pal[32+i]=j;
	}
#endif

#if 0
	for(i=0; i<252; i++)
	{
		cg=((i/36)%7)*43;
		cr=((i/ 6)%6)*51;
		cb=((i   )%6)*51;

		cr=btpic_clamp255(cr);
		cg=btpic_clamp255(cg);
		cb=btpic_clamp255(cb);

		cr>>=3;		cg>>=3;		cb>>=3;
		j=(cr<<10)|(cg<<5)|cb;
		pal[i]=j;
	}
#endif

	pal[255]=0xFC1F;

	return(0);
}

int BTPIC_InitMapTableForPal(u16 *pal, byte *tab, int bias)
{
	int cr0, cg0, cb0;
	int cr1, cg1, cb1;
	int dr, dg, db;
	int d, bd, bi;
	int i, j, k;

	for(i=0; i<32768; i++)
	{
		cr0=(i>>10)&31;
		cg0=(i>> 5)&31;
		cb0=(i>> 0)&31;
		
		cr0=(cr0<<3)|(cr0>>2);
		cg0=(cg0<<3)|(cg0>>2);
		cb0=(cb0<<3)|(cb0>>2);
		
		cr0+=bias;
		cg0+=bias;
		cb0+=bias;
		
		cr0=btpic_clamp255(cr0);
		cg0=btpic_clamp255(cg0);
		cb0=btpic_clamp255(cb0);
		
		bd=999999999; bi=0;
		for(j=0; j<255; j++)
		{
			k=pal[j];
			cr1=(k>>10)&31;
			cg1=(k>> 5)&31;
			cb1=(k>> 0)&31;
		
			cr1=(cr1<<3)|(cr1>>2);
			cg1=(cg1<<3)|(cg1>>2);
			cb1=(cb1<<3)|(cb1>>2);
			
			dr=cr1-cr0;
			dg=cg1-cg0;
			db=cb1-cb0;
			d=(5*dr*dr)+(8*dg*dg)+(3*db*db);
			if(d<bd)
				{ bi=j; bd=d; }
		}
		
		tab[0x0000|i]=bi;
		tab[0x8000|i]=255;
	}

	return(0);
}

int BTPIC_InitLumaTableForPal(u16 *pal, byte *tab)
{
	int cr, cg, cb, cy;
	int i, j, k;

	for(i=0; i<256; i++)
	{
		j=pal[i];
		cr=(j>>10)&31;
		cg=(j>> 5)&31;
		cb=(j>> 0)&31;
		cr=(cr<<3)|(cr>>2);
		cg=(cg<<3)|(cg>>2);
		cb=(cb<<3)|(cb>>2);
		cy=((128*cg)+(88*cr)+(40*cb))>>8;
		tab[i]=cy;
	}
	return(0);
}

int BTPIC_EncodeContextSetupQuality(BTPIC0A_EncodeContext *ctx, int qfl)
{
	int qf;

	qf=qfl&127;
	if(qf>100)qf=100;
	ctx->ld_flat=100-qf;
	ctx->ld_2x2=225-(qf*2);
	ctx->ld_skip=255-(qf*2);
	return(0);
}

int BTPIC_EncodeInitContext(BTPIC0A_EncodeContext *ctx,
	int xs, int ys, int qfl)
{
	int qf;

	BTPIC_InitDefaultPalette(ctx->pal);
//	BTPIC_InitMapTableForPal(ctx->pal, ctx->cmatab+(0<<16),  0);
//	BTPIC_InitMapTableForPal(ctx->pal, ctx->cmatab+(1<<16), 13);
//	BTPIC_InitMapTableForPal(ctx->pal, ctx->cmatab+(2<<16), 26);
//	BTPIC_InitMapTableForPal(ctx->pal, ctx->cmatab+(3<<16), 38);

//	BTPIC_InitMapTableForPal(ctx->pal, ctx->cmatab+(0<<16), -10);
//	BTPIC_InitMapTableForPal(ctx->pal, ctx->cmatab+(1<<16), - 4);
//	BTPIC_InitMapTableForPal(ctx->pal, ctx->cmatab+(2<<16),   4);
//	BTPIC_InitMapTableForPal(ctx->pal, ctx->cmatab+(3<<16),  10);

//	BTPIC_InitMapTableForPal(ctx->pal, ctx->cmatab+(0<<16), -5);
//	BTPIC_InitMapTableForPal(ctx->pal, ctx->cmatab+(1<<16), -2);
//	BTPIC_InitMapTableForPal(ctx->pal, ctx->cmatab+(2<<16),  2);
//	BTPIC_InitMapTableForPal(ctx->pal, ctx->cmatab+(3<<16),  5);

	BTPIC_InitMapTableForPal(ctx->pal, ctx->cmatab+(0<<16),  0);
	BTPIC_InitMapTableForPal(ctx->pal, ctx->cmatab+(1<<16), -1);
	BTPIC_InitMapTableForPal(ctx->pal, ctx->cmatab+(2<<16),  2);
	BTPIC_InitMapTableForPal(ctx->pal, ctx->cmatab+(3<<16), -2);
	BTPIC_InitMapTableForPal(ctx->pal, ctx->cmatab+(4<<16),  4);
	BTPIC_InitMapTableForPal(ctx->pal, ctx->cmatab+(5<<16), -4);
	BTPIC_InitMapTableForPal(ctx->pal, ctx->cmatab+(6<<16),  6);
	BTPIC_InitMapTableForPal(ctx->pal, ctx->cmatab+(7<<16), -6);

	BTPIC_InitLumaTableForPal(ctx->pal, ctx->lutab);
		
	ctx->xs=xs;
	ctx->ys=ys;
	ctx->bxs=(xs+3)>>2;
	ctx->bys=(ys+3)>>2;
	ctx->ystr=xs;
	ctx->fl=0;

	ctx->blka=malloc(ctx->bxs*ctx->bys*sizeof(u64));
	ctx->lblka=malloc(ctx->bxs*ctx->bys*sizeof(u64));
	ctx->fl|=1;
	
	ctx->tfbuf=malloc(xs*ys*4);
	
	BTPIC_EncodeContextSetupQuality(ctx, qfl);
	
	return(0);
}

s64 BTPIC_EncUpdPal_BBoxWeight(BTPIC0A_EncodeContext *ctx,
	int mins, int maxs)
{
	s64 wn;
	u32 *stat;
	int mr, mg, mb, nr, ng, nb;
	int cr, cg, cb, ix;
	
	mr=(mins>>10)&31;	mg=(mins>> 5)&31;	mb=(mins>> 0)&31;
	nr=(maxs>>10)&31;	ng=(maxs>> 5)&31;	nb=(maxs>> 0)&31;

	stat=ctx->clrstat; wn=0;
	for(cr=mr; cr<=nr; cr++)
		for(cg=mg; cg<=ng; cg++)
			for(cb=mb; cb<=nb; cb++)
	{
		ix=(cr<<10)|(cg<<5)|cb;
		wn+=stat[ix];
	}
	return(wn);
}

int BTPIC_EncUpdPal_BBoxCom(BTPIC0A_EncodeContext *ctx,
	int mins, int maxs)
{
	u32 *stat;
	s64 wcr, wcg, wcb, wn;
	s64 dcr, dcg, dcb, d;
	int mr, mg, mb, nr, ng, nb;
	int cr, cg, cb, ix;
	
	mr=(mins>>10)&31;	mg=(mins>> 5)&31;	mb=(mins>> 0)&31;
	nr=(maxs>>10)&31;	ng=(maxs>> 5)&31;	nb=(maxs>> 0)&31;

	stat=ctx->clrstat;
	wcr=0; wcg=0; wcb=0; wn=0;
	for(cr=mr; cr<=nr; cr++)
		for(cg=mg; cg<=ng; cg++)
			for(cb=mb; cb<=nb; cb++)
	{
		ix=(cr<<10)|(cg<<5)|cb;
		wcr+=((s64)cr)*stat[ix];
		wcg+=((s64)cg)*stat[ix];
		wcb+=((s64)cb)*stat[ix];
		wn+=stat[ix];
	}
	
	if(wn>0)
		{ wcr=wcr/wn; wcg=wcg/wn; wcb=wcb/wn; }
	
	if((wcr==mr) && (mr!=nr))wcr++;
	if((wcg==mg) && (mg!=ng))wcg++;
	if((wcb==mb) && (mb!=nb))wcb++;

	dcr=0; dcg=0; dcb=0;
	for(cr=mr; cr<=nr; cr++)
		for(cg=mg; cg<=ng; cg++)
			for(cb=mb; cb<=nb; cb++)
	{
		ix=(cr<<10)|(cg<<5)|cb;
//		d=(cr-wcr)*stat[ix]; d=d^(d>>63);	dcr+=d;
//		d=(cg-wcg)*stat[ix]; d=d^(d>>63);	dcg+=d;
//		d=(cb-wcb)*stat[ix]; d=d^(d>>63);	dcb+=d;

		d=(cr-wcr)*stat[ix]; d=d*d;	dcr+=d;
		d=(cg-wcg)*stat[ix]; d=d*d;	dcg+=d;
		d=(cb-wcb)*stat[ix]; d=d*d;	dcb+=d;
	}
	
	if((wcr<mr) || (wcg<mg) || (wcb<mb))
		{ __debugbreak(); }
	if((wcr>nr) || (wcg>ng) || (wcb>nb))
		{ __debugbreak(); }
	
	cr=wcr;	cg=wcg;	cb=wcb;
	ix=(cr<<10)|(cg<<5)|cb;
	if(dcr>dcg)
	{
		if(dcr>dcb)
			ix|=0x30000;
		else
			ix|=0x10000;
	}else
	{
		if(dcg>dcb)
			ix|=0x20000;
		else
			ix|=0x10000;
	}
	
//	*rcom=ix;
//	*rwn=wn;
	return(ix);
}

int BTPIC_EncodeUpdatePalette(BTPIC0A_EncodeContext *ctx, int qfl)
{
	int bbmins[258], bbmaxs[258];
	s64 bbwn[258];
	int nbb, bbm, bbn, bbc, bbm2, bbn2;
	int nosplit;

	s64 twn;
	int qf;
	u32 uj;
	int i, j, k, l;

	if(qfl&BTPIC_QFL_PFRAME)
		return(0);
	
	twn=0;
	for(i=0; i<32768; i++)
	{
		uj=ctx->clrstat[i];
		uj=(uj>>4)+1;
		ctx->clrstat[i]=uj;
		twn+=uj;
	}
		
	bbmins[0]=0x0000;
	bbmaxs[0]=0x7FFF;
	bbwn[0]=twn;
	nbb=1;

	while(nbb<255)
	{
		for(i=0; i<nbb; i++)
			for(j=i+1; j<nbb; j++)
		{
			if(bbwn[i]>bbwn[j])
			{
				twn=bbwn[i];	bbwn[i]=bbwn[j];		bbwn[j]=twn;
				twn=bbmins[i];	bbmins[i]=bbmins[j];	bbmins[j]=twn;
				twn=bbmaxs[i];	bbmaxs[i]=bbmaxs[j];	bbmaxs[j]=twn;
			}
		}
		
		bbm=bbmins[nbb-1];
		bbn=bbmaxs[nbb-1];
		bbc=BTPIC_EncUpdPal_BBoxCom(ctx, bbm, bbn);
		
		nosplit=0;
		switch((bbc>>16)&3)
		{
		case 0:
			__debugbreak();
			break;
		case 1:
			if((bbm&0x001F)>=(bbn&0x001F))
				{ nosplit=1; break; }
			if(bbc&0x001F)
			{
				bbm2=(bbm&0x7FE0)|(bbc&0x001F);
				bbn2=(bbn&0x7FE0)|((bbc&0x001F)-1);
			}else
			{
				bbm2=(bbm&0x7FE0)|((bbc&0x001F)+1);
				bbn2=(bbn&0x7FE0)|(bbc&0x001F);
			}
			break;
		case 2:
			if((bbm&0x03E0)>=(bbn&0x03E0))
				{ nosplit=1; break; }
			if(bbc&0x03E0)
			{
				bbm2=(bbm&0x7C1F)|(bbc&0x03E0);
				bbn2=(bbn&0x7C1F)|((bbc&0x03E0)-0x0020);
			}else
			{
				bbm2=(bbm&0x7C1F)|((bbc&0x03E0)+0x0020);
				bbn2=(bbn&0x7C1F)|(bbc&0x03E0);
			}
			break;
		case 3:
			if((bbm&0x7C00)>=(bbn&0x7C00))
				{ nosplit=1; break; }
			if(bbc&0x7C00)
			{
				bbm2=(bbm&0x03FF)|(bbc&0x7C00);
				bbn2=(bbn&0x03FF)|((bbc&0x7C00)-0x0400);
			}else
			{
				bbm2=(bbm&0x03FF)|((bbc&0x7C00)+0x0400);
				bbn2=(bbn&0x03FF)|(bbc&0x7C00);
			}
			break;
		}
		
		if(nosplit)
		{
			bbwn[nbb-1]=0;
			continue;
		}
		
		nbb++;
		bbmins[nbb-2]=bbm;
		bbmaxs[nbb-2]=bbn2;
		bbmins[nbb-1]=bbm2;
		bbmaxs[nbb-1]=bbn;
		bbwn[nbb-2]=BTPIC_EncUpdPal_BBoxWeight(ctx,
			bbmins[nbb-2], bbmaxs[nbb-2]);
		bbwn[nbb-1]=BTPIC_EncUpdPal_BBoxWeight(ctx,
			bbmins[nbb-1], bbmaxs[nbb-1]);
	}

	for(i=0; i<nbb; i++)
		for(j=i+1; j<nbb; j++)
	{
		if(bbwn[i]<bbwn[j])
		{
			twn=bbwn[i];	bbwn[i]=bbwn[j];		bbwn[j]=twn;
			twn=bbmins[i];	bbmins[i]=bbmins[j];	bbmins[j]=twn;
			twn=bbmaxs[i];	bbmaxs[i]=bbmaxs[j];	bbmaxs[j]=twn;
		}
	}

	for(i=0; i<255; i++)
	{
		bbm=bbmins[i];
		bbn=bbmaxs[i];
		bbc=BTPIC_EncUpdPal_BBoxCom(ctx, bbm, bbn);

		ctx->pal[i]=bbc;
	}

	BTPIC_InitMapTableForPal(ctx->pal, ctx->cmatab,  0);
	BTPIC_InitLumaTableForPal(ctx->pal, ctx->lutab);
		
	return(0);
}

int BTPIC_EncodeImageBasic_Px4ToPx2(u16 px)
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


int BTPIC_EncodeBlockRGB2Index(BTPIC0A_EncodeContext *ctx,
	int cr, int cg, int cb)
{
	int c;
	c=((cr>>3)<<10)|((cg>>3)<<5)|((cb>>3)<<0);
	return(ctx->cmatab[c]);
}

int BTPIC_EncodeBlockBasic(BTPIC0A_EncodeContext *ctx,
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
	
	if(!cya)cya++;
	if(!cyb)cyb++;

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
	
//	ia=((mr>>3)<<10)|((mg>>3)<<5)|(mb>>3);
//	ib=((nr>>3)<<10)|((ng>>3)<<5)|(nb>>3);
	
	ia=BTPIC_EncodeBlockRGB2Index(ctx, mr, mg, mb);
	ib=BTPIC_EncodeBlockRGB2Index(ctx, nr, ng, nb);
	ic=BTPIC_EncodeBlockRGB2Index(ctx, acr, acg, acb);
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

#if 0
int BTPIC_EncodeBlockBasic(BTPIC0A_EncodeContext *ctx,
	u16 *img, int ystr,
	int *rpx, int *rca, int *rcb)
{
	byte pxc[16];
	byte pxl[16];
	u16 *pal;
	byte *cmap;
	byte *lutab;
	u16 *cs;
	int ca, cb, cc, la, lb, lc, ld;
	int d, bd;
	int x, y, z, px, ix;
	int i, j, k, l;
	
	pal=ctx->pal;
	cmap=ctx->cmatab;
	lutab=ctx->lutab;
	
	ca=0;		cb=0;
	la=999;		lb=-999;
	
	cs=img; ix=0;
	for(y=0; y<4; y++)
	{
		for(x=0; x<4; x++)
		{
//			z=(x^y)&3;
			z=(x^y)&7;
			i=cs[x];
//			j=cmap[i];
			j=cmap[i|(z<<16)];
			k=lutab[j];
			pxc[ix]=j;
			pxl[ix]=k;
			ix++;
			
			if(k<la)	{ ca=j; la=k; }
			if(k>lb)	{ cb=j; lb=k; }
		}
		cs+=ystr;
	}

	ld=lb-la;
	lc=(la+lb)/2;
	px=0;

	if(ld<ctx->ld_flat)
//	if(0)
	{
		bd=999999; cc=ca;
		for(i=0; i<16; i++)
		{
			d=pxl[i]-lc;
			d=d*d;
			if(d<bd)
				{ bd=d; cc=pxc[i]; }
		}
		ca=cc;
		cb=cc;
		px=0;
	}else
		if(ld<ctx->ld_2x2)
//		if(0)
	{
		px=0;
		for(y=0; y<2; y++)
			for(x=0; x<2; x++)
		{
			l=	pxl[(y*2+0)*4+(x*2+0)] + pxl[(y*2+0)*4+(x*2+1)] +
				pxl[(y*2+1)*4+(x*2+0)] + pxl[(y*2+1)*4+(x*2+1)] ;
			l/=4;
			if(l>=lc)
				px|=0x0033<<(y*8+x*2);
		}
	}else
	{
		px=0;
		for(i=0; i<16; i++)
		{
			if(pxl[i]>=lc)
				px|=1<<i;
		}
	}
	
	*rpx=px;
	*rca=ca;
	*rcb=cb;

	return(0);
}
#endif


int	BTPIC_EncodeGetColorDist16(u16 ca, u16 cb)
{
	int cr0, cg0, cb0;
	int cr1, cg1, cb1;
	int dr, dg, db, d;
	
	cr0=((ca>>10)&31)<<3;	cg0=((ca>> 5)&31)<<3;	cb0=((ca>> 0)&31)<<3;
	cr1=((cb>>10)&31)<<3;	cg1=((cb>> 5)&31)<<3;	cb1=((cb>> 0)&31)<<3;
	dr=cr0-cr1;				dg=cg0-cg1;				db=cb0-cb1;
	d=(dr*dr)+(dg*dg)+(db*db);
	d=(d*148)>>8;
	return(d);
}

int BTPIC_EncodeCheckCanSkipBlock(BTPIC0A_EncodeContext *ctx,
	u64 blk0, u64 blk1)
{
	u16 px0, px1, px2, px3;
	int ca0, ca1, ca2, ca3, cb0, cb1, cb2, cb3;
	int d0, d1, d2, d3;

	if(blk0==blk1)
		return(1);

	px0=(blk0>>32)&65535;	ca0=(blk0>>16)&255;		cb0=(blk0>>24)&255;
	px1=(blk1>>32)&65535;	ca1=(blk1>>16)&255;		cb1=(blk1>>24)&255;
	ca2=ctx->pal[ca0];		ca3=ctx->pal[ca1];
	cb2=ctx->pal[cb0];		cb3=ctx->pal[cb1];
	
	d0=BTPIC_EncodeGetColorDist16(ca2, cb3);
	d1=BTPIC_EncodeGetColorDist16(ca3, cb2);
	if((d0<ctx->ld_skip) && (d1<ctx->ld_skip))
		return(1);
	
	return(0);
}

int BTPIC_EncodeImageBasic(BTPIC0A_EncodeContext *ctx,
	byte *obuf, int obsz, u16 *img, int ystr, int qfl, int clrs)
{
//	static u16 pxarr[65536];
//	static byte clarr[65536*2];
	u64 *blka, *lblka;
	byte *ct;
	u16 *cs;
	int px, px1, px2, px3;
	s16 p2x0, p2x1, p2x2, p2x3;
	u64 blk, blk0, blk1, blk2, blk3;
	u32 ui;
	int ca, cb, lca, lcb, lcc;
	int ca1, ca2, ca3, cb1, cb2, cb3;
	int bxs, bys, nfca, nfcb, nfcc, nfcfl;
	int dvx, dvy, dvofs, dskip;
	int x, y, n, mm, m2x;
	int i, j, k, k1;
		
	BTPIC_EncodeContextSetupQuality(ctx, qfl);

	if(1)
	{
		bxs=ctx->xs;
		bys=ctx->ys;
		for(y=0; y<bys; y++)
		{
			cs=img+y*ystr;
			for(x=0; x<bxs; x++)
			{
				i=cs[x];
				i&=32767;

				ui=ctx->clrstat[i];
				if(ui<((1LL<<32)-1))ui++;
				ctx->clrstat[i]=ui;
			}
		}
		BTPIC_EncodeUpdatePalette(ctx, qfl);
	}

	blka=ctx->blka;
	lblka=ctx->lblka;
	
	if(qfl&BTPIC_QFL_PFRAME)
	{
		bxs=ctx->bxs;
		bys=ctx->bys;

		blka=ctx->blka;
		lblka=ctx->lblka;
		memcpy(lblka, blka, bxs*bys*sizeof(u64));

//		blka=ctx->lblka;
//		lblka=ctx->blka;
//		ctx->blka=blka;
//		ctx->lblka=lblka;
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
			BTPIC_EncodeBlockBasic(ctx, cs, ystr, &px, &ca, &cb);
			cs+=4;
//			pxarr[n]=px;
//			clarr[n*2+0]=ca;
//			clarr[n*2+1]=cb;
			
			blk=(((u64)px)<<32)|
				(((u64)cb)<<24)|
				(((u64)ca)<<16);
			blka[n]=blk;

			n++;
		}
	}

#if 1
	dvx=0; dvy=0;
	for(i=0; i<n; i++)
	{
		blk=blka[i];
		if(lblka)
		{
			blk1=lblka[i];
			if(BTPIC_EncodeCheckCanSkipBlock(ctx, blk1, blk))
			{
				blka[i]=blk1;
			}
		}
	}
#endif

	dvx=0; dvy=0;
	ct=obuf; lca=-1; lcb=-1; lcc=-1;
	nfca=0; nfcb=0; nfcc=0;
	for(i=0; i<n; i++)
	{
//		px =pxarr[i];		ca =clarr[i*2+0];		cb =clarr[i*2+1];
//		px1=pxarr[(i+1)];	ca1=clarr[(i+1)*2+0];	cb1=clarr[(i+1)*2+1];
//		px2=pxarr[(i+2)];	ca2=clarr[(i+2)*2+0];	cb2=clarr[(i+2)*2+1];
//		px3=pxarr[(i+3)];	ca3=clarr[(i+3)*2+0];	cb3=clarr[(i+3)*2+1];

#if 1
		blk0=blka[i+0];		blk1=blka[i+1];
		blk2=blka[i+2];		blk3=blka[i+3];
		px =(blk0>>32)&65535;	ca =(blk0>>16)&255;		cb =(blk0>>24)&255;
		px1=(blk1>>32)&65535;	ca1=(blk1>>16)&255;		cb1=(blk1>>24)&255;
		px2=(blk2>>32)&65535;	ca2=(blk2>>16)&255;		cb2=(blk2>>24)&255;
		px3=(blk3>>32)&65535;	ca3=(blk3>>16)&255;		cb3=(blk3>>24)&255;
#endif

		if((ca==255) || (cb==255))
			{ __debugbreak(); }

		dskip=0;

#if 1
		if(lblka)
		{
			dvofs=dvy*bxs+dvx;
			for(j=0; (i+j)<n; j++)
			{
				k=i+j+dvofs;
				if((k<0) || (k>=n))		break;
				if(blka[i+j]!=lblka[k])
					break;
			}
//			dskip=(j>1)?(j-1):0;
			dskip=j;
		}
#endif

		if(px&1)
			{ j=ca; ca=cb; cb=j; px=~px; }
		
		nfcfl=0;
		if(!px)
		{
//			if(!((ca==lcc) && nfcc))
			if(!nfcc)
			{
				if((ca==lca) && nfcb)	nfcfl=1;
				if((ca==lcb) && nfca)	nfcfl=1;
			}else
			{
				if(ca!=lcc)	nfcfl=1;
			}
		}else
		{
			nfcfl=1;
		}
		
		if(dskip)
			nfcfl=1;

		if((i+1)>=n)
			nfcfl=1;
		
		if(nfcfl)
		{
			if(nfca)
			{
				j=nfca;
				while(j>4)
					{ *ct++=0xCF; j-=4; }
				if(j)
					{ *ct++=0x0F|((j-1)<<6); }
				nfca=0;
			}

			if(nfcb)
			{
				j=nfcb;
				while(j>4)
					{ *ct++=0xEF; j-=4; }
				if(j)
					{ *ct++=0x2F|((j-1)<<6); }
				nfcb=0;
			}

			if(nfcc)
			{
				if(lcc<0)
					{ __debugbreak(); }
			
				j=nfcc;
				if(lcc==ca)
				{
					if(j>4)
					{
						*ct++=0xD3;
						*ct++=lcc;
						j-=4;
						lca=lcc;
					}else
					{
						*ct++=0x13|((j-1)<<6);
						*ct++=lcc;
						j=0;
						lca=lcc;
					}
					while(j>4)
						{ *ct++=0xCF; j-=4; }
					if(j)
						{ *ct++=0x0F|((j-1)<<6); }
				}else
				{
					if(j>4)
					{
						*ct++=0xDB;
						*ct++=lcc;
						j-=4;
						lcb=lcc;
					}else
					{
						*ct++=0x1B|((j-1)<<6);
						*ct++=lcc;
						j=0;
						lcb=lcc;
					}
					while(j>=4)
						{ *ct++=0xEF; j-=4; }
					if(j)
						{ *ct++=0x2F|((j-1)<<6); }
				}
				nfcc=0;
			}
		}

		if(dskip)
		{
			j=dskip;
			if(j>=1)
			{
				k=j;
				while(k>=1024)
				{
					*ct++=0xC3;
					*ct++=0xFF;
					k-=1024;
				}

				if(k>0)
				{
					k1=k-1;
					if(k>=8)
//					if(1)
					{
						*ct++=0x03|(k1<<6);
						*ct++=k1>>2;
					}else
					{
						*ct++=0x1D|(k1<<5);
					}
				}

				i+=j-1;
				continue;
			}

#if 0
			for(j=0; j<7; j++)
			{
				k=i+j;
				if((k<0) || (k>=n))		break;
				if(blka[k]!=lblka[k])
					break;
			}

			if(j>1)
			{
				k1=j-1;
				*ct++=0xE1|(k1<<2);
				i+=k1;
				continue;
			}
#endif
		}

#if 1
		if(!px)
		{
			if(ca==lca)
				{ nfca++; continue; }
			if(ca==lcb)
				{ nfcb++; continue; }
			if((ca==lcc) && nfcc)
				{ nfcc++; continue; }

			lcc=ca;
			nfcc++;
			continue;
		}
#endif

#if 1
		if((ca==lcb) && (cb==lca))
			{ j=ca; ca=cb; cb=j; px=~px; }
		if((ca1==lcb) && (cb1==lca))
			{ j=ca1; ca1=cb1; cb1=j; px1=~px1; }
		if((ca2==lcb) && (cb2==lca))
			{ j=ca2; ca2=cb2; cb2=j; px2=~px2; }
		if((ca3==lcb) && (cb3==lca))
			{ j=ca3; ca1=cb3; cb3=j; px3=~px3; }

		mm=0;
		if((ca==lca) && (cb==lcb))		mm|=1;
		if((ca1==lca) && (cb1==lcb))	mm|=2;
		if((ca2==lca) && (cb2==lcb))	mm|=4;
		if((ca3==lca) && (cb3==lcb))	mm|=8;

		m2x=0;
		p2x0=BTPIC_EncodeImageBasic_Px4ToPx2(px);
		p2x1=BTPIC_EncodeImageBasic_Px4ToPx2(px1);
		p2x2=BTPIC_EncodeImageBasic_Px4ToPx2(px2);
		p2x3=BTPIC_EncodeImageBasic_Px4ToPx2(px3);
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
				i+=3;
				*ct++=0x63;
				*ct++=p2x0|(p2x1<<4);
				*ct++=p2x2|(p2x3<<4);
				continue;
			}
#endif

			*ct++=0x07|(p2x0<<4);
			continue;
		}
#endif

		if(mm&1)
		{
			if((mm&15)==15)
			{
				i+=3;
				*ct++=0xEB;
				*ct++=px;	*ct++=px>>8;
				*ct++=px1;	*ct++=px1>>8;
				*ct++=px2;	*ct++=px2>>8;
				*ct++=px3;	*ct++=px3>>8;
				continue;
			}
			if((mm&7)==7)
			{
				i+=2;
				*ct++=0xAB;
				*ct++=px;	*ct++=px>>8;
				*ct++=px1;	*ct++=px1>>8;
				*ct++=px2;	*ct++=px2>>8;
				continue;
			}
			if((mm&3)==3)
			{
				i++;
				*ct++=0x6B;
				*ct++=px;	*ct++=px>>8;
				*ct++=px1;	*ct++=px1>>8;
				continue;				
			}
			*ct++=0x2B;
			*ct++=px;	*ct++=px>>8;
			continue;
		}

#endif

		if(1)
		{
			if(px&1)
				{ j=ca; ca=cb; cb=j; px=~px; }

			*ct++=px;	*ct++=px>>8;
			*ct++=ca;	*ct++=cb;
			lca=ca;		lcb=cb;
			continue;
		}
	}
	
	return(ct-obuf);
}

byte *BTPIC_EncodeEmitTwoCC(byte *ict, int tcc, void *buf, int sz)
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

int BTPIC_EncodeFrameImageA(BTPIC0A_EncodeContext *ctx,
	byte *obuf, int obsz, void *ibuf, int qfl, int clrs)
{
	byte tbuf[256];
	byte *ct;
	byte *ib8;
	u16 *ib16;
	u32 *ib32;
	int px, ystr, flip;
	int x, y, y1;
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
				px=btic4b_getu32le(ib8+(y1*ctx->xs+x)*3);
				if(clrs==BTIC4B_CLRS_RGB)
				{
					px=(px&0xFF00FF00)|
						((px<<16)&0x00FF0000)|
						((px>>16)&0x000000FF);
				}
				px=((px>>9)&0x7C00)|((px>>6)&0x03E0)|((px>>3)&0x001F);
				ctx->imgbuf[y*ystr+x]=px;
			}
		}
	}

//	tfsz=BTPIC_EncodeImageBasic(ctx, ctx->tfbuf, 1<<22, ibuf,
//		ctx->ystr, qfl, clrs);

	tfsz=BTPIC_EncodeImageBasic(ctx, ctx->tfbuf, 1<<22,
		ctx->imgbuf, ystr, qfl, clrs);

	ct=obuf;
	
	if(!(qfl&BTPIC_QFL_PFRAME))
	{
		ct=BTPIC_EncodeEmitTwoCC(ct, BTPIC_TCC_IX, ctx->tfbuf, tfsz);

		ct=BTPIC_EncodeEmitTwoCC(ct, BTPIC_TCC_PT, ctx->pal, 512);

		btpic_setu16(tbuf+0, ctx->xs);
		btpic_setu16(tbuf+2, ctx->ys);
		btpic_setu16(tbuf+4, ctx->fl);
		ct=BTPIC_EncodeEmitTwoCC(ct, BTPIC_TCC_HX, tbuf, 8);
	}else
	{
		ct=BTPIC_EncodeEmitTwoCC(ct, BTPIC_TCC_PX, ctx->tfbuf, tfsz);
	}

	return(ct-obuf);
}

int BTPIC_EncodeFrameImage(BTPIC0A_EncodeContext *ctx,
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

	tsz=BTPIC_EncodeFrameImageA(ctx, tbuf, obsz, ibuf, qfl, clrs);

//	tsz=cram_repak_buf(ibuf, tbuf+4, isz, isz*2, vstat);
//	set_u32le(tbuf, 0x00000000|tsz);
	
	zctx=TgvLz_CreateContext();
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
