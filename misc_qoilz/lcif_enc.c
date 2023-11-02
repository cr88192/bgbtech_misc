#define LCIF_HASHSZ		4096
#define LCIF_HASHMSK	4095

#define LCIF_SDEPTH		29
#define LCIF_HROVMSK	31

int LCIF_CheckMatchLz4(byte *str1, byte *str2, int nl)
{
	byte *cs1, *cs1e, *cs1e0, *cs2;

#ifdef LCIF_CASTDEREF
	if((*(u32 *)str1)!=(*(u32 *)str2))
		return(0);
	cs1=str1+4;
	cs2=str2+4;
	cs1e0=str1+(nl-8);
	cs1e=str1+nl;
	while(cs1<cs1e0)
	{
		if((*(u64 *)cs1)!=(*(u64 *)cs2))
			break;
		cs1+=8; cs2+=8;
	}
#else
	if(	(str1[0]!=str2[0]) ||
		(str1[1]!=str2[1]) ||
		(str1[2]!=str2[2]) ||
		(str1[3]!=str2[3]) )
			return(0);
	cs1=str1+4;
	cs2=str2+4;
	cs1e=str1+nl;
#endif

	while(cs1<cs1e)
	{
		if((*cs1)!=(*cs2))
			break;
		cs1++; cs2++;
	}
	return(cs1-str1);
}

int LCIF_EncLiHash4(byte *cs)
{
	u32 v0, v1, v2, v3;
	u64 h;
	
#ifdef LCIF_CASTDEREF
	v0=((u32 *)cs)[0];
	v1=((u32 *)cs)[1];
	v2=((u32 *)cs)[2];
	v3=((u32 *)cs)[3];
#else
	memcpy(&v0, cs+ 0, 4);
	memcpy(&v1, cs+ 4, 4);
	memcpy(&v2, cs+ 8, 4);
	memcpy(&v3, cs+12, 4);
#endif

	h=v0;
	h=h*65521+v1;
	h=h*65521+v2;
	h=h*65521+v3;
	h=h*65521+(h>>16);
	h=h*65521+(h>>16);
	return((h>>16)&LCIF_HASHMSK);
}

byte *LCIF_EncodeColorPlane(byte *dstbuf, byte *img, int xs, int ys)
{
	byte pixtab[64*4];
#ifdef LCIF_MALLOCHASH
	int *hofs;
	byte *hrov;
#else
	int hofs[LCIF_HASHSZ];
	byte hrov[LCIF_HASHSZ];
#endif
	byte *cs, *ct, *cs1, *cse, *imgbuf;
	int cr, cg, cb, ca;
	int dr, dg, db;
	int crl, cgl, cbl, cal;
	int n, run, bl, bd;
	int i, j, k, h, hli, hr, nl;

#ifdef LCIF_MALLOCHASH
	hofs=malloc(LCIF_HASHSZ*sizeof(int));
	hrov=malloc(LCIF_HASHSZ);
#endif
	
	n=xs*ys;
	cs=img; cse=img+n*4;
	
	ct=dstbuf;
	
	for(i=0; i<256; i++)
	{
		pixtab[i]=0;
	}

	for(i=0; i<LCIF_HASHSZ; i++)
	{
		hofs[i]=0;
		hrov[i]=0;
	}

	crl=0;	cgl=0;
	cbl=0;	cal=255;
	run=0;
	while(cs<cse)
	{
		hli=LCIF_EncLiHash4(cs);
		hr=hrov[hli];

		cr=cs[0];
		cg=cs[1];
		cb=cs[2];
		ca=cs[3];
		cs+=4;
		
		if((cr==crl) && (cg==cgl) && (cb==cbl) && (ca==cal))
		{
			run++;
			continue;
		}
		
		if(run)
		{
#if 1
			while(run>=1027)
			{
				*ct++=0xFB;
				*ct++=0xFF;
				*ct++=0x00;
				*ct++=0x01;
				run-=1027;
//				crl=-255;	cgl=-255;
//				cbl=-255;	cal=-255;
			}
		
			if(run>=240)
			{
				j=run-4;
				*ct++=0xF8|(j>>8);
				*ct++=j;
				*ct++=0x00;
				*ct++=0x01;
				run=0;
//				crl=-255;	cgl=-255;
//				cbl=-255;	cal=-255;
			}
#endif
			
			while(run>=64)
			{
				*ct++=0xFD;
				run-=64;
			}

			while(run>=48)
			{
				*ct++=0xEF;
				run-=48;
			}

			if(run>0)
			{
				*ct++=0xC0+(run-1);
			}
			run=0;
		}

//		if(doli&1)
		if(1)
//		if(0)
		{
			nl=cse-cs;
			if(nl>16384)
				nl=16384;
		
			bl=0;	bd=0;
			for(i=0; i<LCIF_SDEPTH; i++)
			{
				cs1=img+hofs[(hli+((hr-i)&LCIF_HROVMSK))&LCIF_HASHMSK];
				if((cs1>=(cs-4)) || (cs1<img))
					continue;
				
				j=LCIF_CheckMatchLz4(cs1, cs-4, nl);
//				if(j>nl)
//					j=nl;
				j=j>>2;
				k=((cs-4)-cs1)>>2;
				
				if(j>bl)
					{ bl=j; bd=k; }
			}
			
			j=bl;
			k=bd;
			
			if((k>=4096) && (j<6))
				j=0;
			if((k>=65536) && (j<7))
				j=0;
			
			if((j>=5) && (j<4099) && (k<1048560) && (k>0))
			{
//				printf("QOLI E: l=%d d=%d\n", j, k);
				j-=4;
				if((j<127) && (k<4096))
				{
					*ct++=0xF0|(j>>4);
					*ct++=(j<<4)|(k>>8);
					*ct++=k;
				}else
					if((j<1023) && (k<65535))
				{
					*ct++=0xF8|(j>>8);
					*ct++=j;
					*ct++=k>>8;
					*ct++=k;
				}else
				{
					*ct++=0xFC;
					*ct++=j>>4;
					*ct++=(j<<4)|(k>>16);
					*ct++=k>>8;
					*ct++=k;
				}
				j+=4;
				
				i=j-1;
				while(i--)
				{
					hli=LCIF_EncLiHash4(cs);
					hr=hrov[hli];
					cs+=4;
					hofs[(hli+hr)&LCIF_HASHMSK]=(cs-4)-img;
					hrov[hli]=(hr+1)&LCIF_HROVMSK;
				}
				
				crl=-255;	cgl=-255;
				cbl=-255;	cal=-255;
				continue;
			}
		}

		h=(cr*3+cg*5+cb*7+ca*11)&63;

		k=h<<2;
		if(	(cr==pixtab[k+0]) &&
			(cg==pixtab[k+1]) &&
			(cb==pixtab[k+2]) &&
			(ca==pixtab[k+3]))
		{
			*ct++=0x00+h;
			crl=cr;	cgl=cg;
			cbl=cb;	cal=ca;
			continue;
		}
		
		dr=cr-crl;
		dg=cg-cgl;
		db=cb-cbl;

#if 1
		if(	(dr>=-2) && (dr<=1) &&
			(dg>=-2) && (dg<=1) &&
			(db>=-2) && (db<=1) &&
			(ca==cal))
		{
			*ct++=0x40|((dr+2)<<4)|((dg+2)<<2)|((db+2)<<0);
			h=(cr*3+cg*5+cb*7+ca*11)&63;
			k=h<<2;
			pixtab[k+0]=cr;	pixtab[k+1]=cg;
			pixtab[k+2]=cb;	pixtab[k+3]=ca;
			hofs[(hli+hr)&LCIF_HASHMSK]=(cs-4)-img;
			hrov[hli]=(hr+1)&LCIF_HROVMSK;
			crl=cr;	cgl=cg;
			cbl=cb;	cal=ca;
			continue;
		}
#endif
		
//		dr-=dg;
//		db-=dg;

#if 1
		if(	(dr>=-32) && (dr<=31) &&
			(dg>=-8) && (dg<=7) &&
			(db>=-8) && (db<=7) &&
			(ca==cal))
		{
//			*ct++=0x80|(dg+32);
//			*ct++=((dr+8)<<4)|((db+8)<<0);
			*ct++=0x80|(dr+32);
			*ct++=((dg+8)<<4)|((db+8)<<0);

			h=(cr*3+cg*5+cb*7+ca*11)&63;
			k=h<<2;
			pixtab[k+0]=cr;	pixtab[k+1]=cg;
			pixtab[k+2]=cb;	pixtab[k+3]=ca;
			hofs[(hli+hr)&LCIF_HASHMSK]=(cs-4)-img;
			hrov[hli]=(hr+1)&LCIF_HROVMSK;
			crl=cr;	cgl=cg;
			cbl=cb;	cal=ca;
			continue;
		}
#endif
		
		if(ca==cal)
		{
			*ct++=0xFE;
			*ct++=cr;
			*ct++=cg;
			*ct++=cb;
		}else
		{
			*ct++=0xFF;
			*ct++=cr;
			*ct++=cg;
			*ct++=cb;
			*ct++=ca;
		}

		h=(cr*3+cg*5+cb*7+ca*11)&63;
		k=h<<2;
		pixtab[k+0]=cr;	pixtab[k+1]=cg;
		pixtab[k+2]=cb;	pixtab[k+3]=ca;
		hofs[(hli+hr)&LCIF_HASHMSK]=(cs-4)-img;
		hrov[hli]=(hr+1)&LCIF_HROVMSK;
		
		crl=cr;
		cgl=cg;
		cbl=cb;
		cal=ca;
	}

	if(run)
	{
		while(run>=48)
		{
			*ct++=0xEF;
			run-=48;
		}
		if(run>0)
		{
			*ct++=0xC0+(run-1);
		}
		run=0;
	}

#ifdef LCIF_MALLOCHASH
	free(hofs);
	free(hrov);
#endif

	return(ct);
}

byte *LCIF_EncodeBlockPlane(byte *dstbuf, u64 *img, int xs, int ys)
{
	u64 blktab[64];
	u64 *cs, *cse;
	byte *ct;
	u64 v, v0, v1;
	u32 vd;
	int flatrun, h, vb;
	int i, j, k;
	
	ct=dstbuf;
	cs=img;
	cse=img+xs*ys;
	
	LCIF_ExpandCell_Init();

	flatrun=0;
	while(cs<cse)
	{
		v=*cs++;
		if(!v)
		{
			flatrun++;
			continue;
		}
		
		if(flatrun)
		{
#if 1
			while(flatrun>=64)
			{
				*ct++=0xFD;
				flatrun-=64;
			}
#endif
			
			while(flatrun>=16)
			{
				*ct++=0x8F;
				flatrun-=16;
			}
			
			if(flatrun)
			{
				*ct++=0x80+(flatrun-1);
				flatrun=0;
			}
		}

		v1=(v<<5)^(v>>31);
		v1=(v1<<3)^(v1>>17);
		v1=(v1<<1)^(v1>>11);
		h=(v1^(v1>>7))&63;
		
		if(blktab[h]==v)
		{
			*ct++=0x00+h;
			continue;
		}
		
		vd=0;
		for(i=0; i<16; i++)
		{
			vd|=((v>>(i*3+1))&3)<<(i*2);
		}

		vb=	(((vd>>0)&3)<<0) |
			(((vd>>4)&3)<<2) |
			(((vd>>16)&3)<<2) |
			(((vd>>20)&3)<<2) ;

#if 1
		v1=LCIF_ExpandCell_2x2x2(vb);
		if(v==v1)
		{
			*ct++=0x90;			
			*ct++=vb;
			blktab[h]=v;
			continue;
		}
#endif

#if 1
		v1=LCIF_ExpandCell_4x4x2(vd);
		if(v==v1)
		{
			*ct++=0xB0;			
			*ct++=vd>> 0;		*ct++=vd>> 8;
			*ct++=vd>>16;		*ct++=vd>>24;			
			blktab[h]=v;
			continue;
		}
#endif

		if(1)
		{
			*ct++=0xE8;
			*ct++=v>> 0;		*ct++=v>> 8;
			*ct++=v>>16;		*ct++=v>>24;
			*ct++=v>>32;		*ct++=v>>40;
			*ct++=v>>48;		*ct++=v>>56;
			
			blktab[h]=v;
			continue;
		}
	}

	if(flatrun)
	{
		while(flatrun>=16)
		{
			*ct++=0x8F;
			flatrun-=16;
		}
		
		if(flatrun)
		{
			*ct++=0x80+(flatrun-1);
			flatrun=0;
		}
	}
	
	return(ct);
}

int LCIF_EncodeAdjustYuvRange(int *rcy, int *rcu, int *rcv)
{
	int cy, cu, cv;
	int cr, cg, cb;
	int rt;
	
	cy=*rcy;
	cu=*rcu;
	cv=*rcv;

	cb=((cu-128)<<1)+cy;
	cr=((cv-128)<<1)+cy;
	cg=((cy<<4)-5*cr-3*cb)>>3;
	rt=0;
	
	while(
		(cr<  1) || (cg<  1) || (cb<  1) ||
		(cr>254) || (cg>254) || (cb>254))
	{
//		cy=128+(((cy-128)*31)>>5);
		cy=128+(((cy-128)*63)>>6);
//		cy=128+(((cy-128)*127)>>7);
		cu=128+(((cu-128)*31)>>5);
		cv=128+(((cv-128)*31)>>5);

//		cu=128+(((cu-128)*15)>>4);
//		cv=128+(((cv-128)*15)>>4);

		cb=((cu-128)<<1)+cy;
		cr=((cv-128)<<1)+cy;
		cg=((cy<<4)-5*cr-3*cb)>>3;
		rt=1;
	}
	
	*rcy=cy;
	*rcu=cu;
	*rcv=cv;
	return(rt);
}

int LCIF_EncodeAdjustYuvRangeBound(
	int *rmcy, int *rmcu, int *rmcv,
	int *rncy, int *rncu, int *rncv)
{
	int rt, i;

	i=1; rt=0;
	while(i)
	{
		i=LCIF_EncodeAdjustYuvRange(rmcy, rmcu, rmcv);
		i|=LCIF_EncodeAdjustYuvRange(rncy, rncu, rncv);

		i|=LCIF_EncodeAdjustYuvRange(rmcy, rncu, rncv);
		i|=LCIF_EncodeAdjustYuvRange(rncy, rmcu, rmcv);

		i|=LCIF_EncodeAdjustYuvRange(rmcy, rmcu, rncv);
		i|=LCIF_EncodeAdjustYuvRange(rmcy, rncu, rmcv);
		i|=LCIF_EncodeAdjustYuvRange(rncy, rmcu, rncv);
		i|=LCIF_EncodeAdjustYuvRange(rncy, rncu, rmcv);
		rt=1;
	}
	return(rt);
}

int LCIF_EncodeCellBlockPixels(byte *pix, int xstr,
	u64 *blk, byte *yuv0, byte *yuv1, int errt)
{
	byte ybuf[16], y4buf[4];
	byte ubuf[16], u4buf[4];
	byte vbuf[16], v4buf[4];
	u64 pxy, pxyd;
	int pxu, pxv;
	int cr, cg, cb;
	int cy, cu, cv;
	int mcy, mcu, mcv;
	int ncy, ncu, ncv;
	int acy, acu, acv;
	int mu, mv, nu, nv, dy, du, dv, rcp;
	int dmu, dmv, dnu, dnv, errtx2;
	int x, y;
	int i, j, k;
	
	errtx2=errt*2;
	
	mcy= 512; mcu=128; mcv=128;
	ncy=-512; ncu=128; ncv=128;
	mu= 512; mv= 512;
	nu=-512; nv=-512;
	for(y=0; y<4; y++)
		for(x=0; x<4; x++)
	{
		k=(y*xstr+x)*4;
		cb=pix[k+0];
		cg=pix[k+1];
		cr=pix[k+2];
		
		cy=(8*cg+5*cr+3*cb)>>4;
		cu=((cb-cy)>>1)+128;
		cv=((cr-cy)>>1)+128;
		
		k=y*4+x;
		ybuf[k]=cy;
		ubuf[k]=cu;
		vbuf[k]=cv;
		
		if(cy<mcy)
			{ mcy=cy; mcu=cu; mcv=cv; }
		if(cy>ncy)
			{ ncy=cy; ncu=cu; ncv=cv; }
			
		if(cu<mu)	mu=cu;
		if(cv<mv)	mv=cv;
		if(cu>nu)	nu=cu;
		if(cv>nv)	nv=cv;
	}

	dy=ncy-mcy;
	du=ncu-mcu;
	dv=ncv-mcv;
	acy=(mcy+ncy)>>1;
	acu=(mcu+ncu)>>1;
	acv=(mcv+ncv)>>1;
	
	if((dy<errt) && (du<errt) && (dv<errt))
	{
		LCIF_EncodeAdjustYuvRange(&acy, &acu, &acv);
	
		yuv0[0]=acy;	yuv0[1]=acu;
		yuv0[2]=acv;	yuv0[3]=255;
		yuv1[0]=acy;	yuv1[1]=acu;
		yuv1[2]=acv;	yuv1[3]=255;
		*blk=0;
		return(0);
	}

	du=nu-mu;
	dv=nv-mv;
	acu=(mu+nu)>>1;
	acv=(mv+nv)>>1;
	
//	dmu=mu^mcu;
//	dmv=mv^mcv;
//	dnu=nu^ncu;
//	dnv=nv^ncv;

	dmu=mu-mcu;	dmv=mv-mcv;
	dnu=nu-ncu;	dnv=nv-ncv;
	if(dmu<0)dmu=-dmu;
	if(dmv<0)dmv=-dmv;
	if(dnu<0)dnu=-dnu;
	if(dnv<0)dnv=-dnv;

	y4buf[0]=(ybuf[ 0]+ybuf[ 1]+ybuf[ 4]+ybuf[ 5])>>2;
	y4buf[1]=(ybuf[ 2]+ybuf[ 3]+ybuf[ 6]+ybuf[ 7])>>2;
	y4buf[2]=(ybuf[ 8]+ybuf[ 9]+ybuf[12]+ybuf[13])>>2;
	y4buf[3]=(ybuf[10]+ybuf[11]+ybuf[14]+ybuf[15])>>2;

	u4buf[0]=(ubuf[ 0]+ubuf[ 1]+ubuf[ 4]+ubuf[ 5])>>2;
	u4buf[1]=(ubuf[ 2]+ubuf[ 3]+ubuf[ 6]+ubuf[ 7])>>2;
	u4buf[2]=(ubuf[ 8]+ubuf[ 9]+ubuf[12]+ubuf[13])>>2;
	u4buf[3]=(ubuf[10]+ubuf[11]+ubuf[14]+ubuf[15])>>2;

	v4buf[0]=(vbuf[ 0]+vbuf[ 1]+vbuf[ 4]+vbuf[ 5])>>2;
	v4buf[1]=(vbuf[ 2]+vbuf[ 3]+vbuf[ 6]+vbuf[ 7])>>2;
	v4buf[2]=(vbuf[ 8]+vbuf[ 9]+vbuf[12]+vbuf[13])>>2;
	v4buf[3]=(vbuf[10]+vbuf[11]+vbuf[14]+vbuf[15])>>2;

	if(dy<(errt*4))
	{
		ybuf[ 0]=y4buf[0];	ybuf[ 1]=y4buf[0];
		ybuf[ 2]=y4buf[1];	ybuf[ 3]=y4buf[1];
		ybuf[ 4]=y4buf[0];	ybuf[ 5]=y4buf[0];
		ybuf[ 6]=y4buf[1];	ybuf[ 7]=y4buf[1];
		ybuf[ 8]=y4buf[0];	ybuf[ 9]=y4buf[0];
		ybuf[10]=y4buf[1];	ybuf[11]=y4buf[1];
		ybuf[12]=y4buf[0];	ybuf[13]=y4buf[0];
		ybuf[14]=y4buf[1];	ybuf[15]=y4buf[1];
	}

	rcp=65536/(dy+1);
	
	pxy=0; pxyd=0;
	for(i=0; i<16; i++)
	{
		j=(((ybuf[i]-acy)*rcp+4096)>>(16-3))+4;
		if(j<0)	j=0;
		if(j>7)	j=7;
		k=(j&6)|(j>>2);

		pxy|=((u64)j)<<(i*3);
		pxyd|=((u64)k)<<(i*3);
	}
	pxyd|=0x6969ULL<<48;

	rcp=65536/(du+1);
	pxu=0;
	for(i=0; i<4; i++)
	{
		j=(((u4buf[i]-acu)*rcp)>>(16-2))+2;
		if(j<0)	j=0;
		if(j>3)	j=3;
		pxu|=j<<(i*2);
	}

	rcp=65536/(dv+1);
	pxv=0;
	for(i=0; i<4; i++)
	{
		j=(((v4buf[i]-acv)*rcp)>>(16-2))+2;
		if(j<0)	j=0;
		if(j>3)	j=3;
		pxv|=j<<(i*2);
	}
	
	pxy|=((u64)pxu)<<48;
	pxy|=((u64)pxv)<<56;
	
//	if(1)
	if((dmu<errtx2) && (dmv<errtx2) && (dnu<errtx2) && (dnv<errtx2))
//	if(0)
	{
//		LCIF_EncodeAdjustYuvRangeBound(
//			&mcy, &mcu, &mcv, &ncy, &ncu, &ncv);

		LCIF_EncodeAdjustYuvRange(&mcy, &mcu, &mcv);
		LCIF_EncodeAdjustYuvRange(&ncy, &ncu, &ncv);

#if 0
		if(pxyd==0x696900000000000ULL)
		{
			yuv0[0]=mcy;	yuv0[1]=mcu;
			yuv0[2]=mcv;	yuv0[3]=255;
			yuv1[0]=mcy;	yuv1[1]=mcu;
			yuv1[2]=mcv;	yuv1[3]=255;
			*blk=0;
			return(0);
		}
#endif
	
		yuv0[0]=mcy;	yuv0[1]=mcu;
		yuv0[2]=mcv;	yuv0[3]=255;
		yuv1[0]=ncy;	yuv1[1]=ncu;
		yuv1[2]=ncv;	yuv1[3]=255;
		*blk=pxyd;
		return(0);
	}

	LCIF_EncodeAdjustYuvRangeBound(
		&mcy, &mu, &mv, &ncy, &nu, &nv);

//	LCIF_EncodeAdjustYuvRange(&mcy, &mu, &mv);
//	LCIF_EncodeAdjustYuvRange(&ncy, &nu, &nv);
	
	yuv0[0]=mcy;	yuv0[1]=mu;
	yuv0[2]=mv;		yuv0[3]=255;
	yuv1[0]=ncy;	yuv1[1]=nu;
	yuv1[2]=nv;		yuv1[3]=255;
	*blk=pxy;
	return(0);
}

int LCIF_LossyCheckMatchYUVA(
	int cr0, int cg0, int cb0, int ca0,
	int cr1, int cg1, int cb1, int ca1,
	int errt)
{
	int dr, dg, db, da, d;

	if(	(cr0==cr1) && (cg0==cg1) &&
		(cb0==cb1) && (ca0==ca1))
			return(1);

	dr=cr0^cr1;
	dg=cg0^cg1;
	db=cb0^cb1;
	da=ca0^ca1;
	d=2*dg+dr+db+da;
	if(d<0)
		return(0);
	if(d<errt)
		return(1);
	return(0);
}

int LCIF_LossyCalcErrorYUVA(
	int cr0, int cg0, int cb0, int ca0,
	int cr1, int cg1, int cb1, int ca1)
{
	int dr, dg, db, da, d;

	if(	(cr0==cr1) && (cg0==cg1) &&
		(cb0==cb1) && (ca0==ca1))
			return(0);

	dr=cr0^cr1;
	dg=cg0^cg1;
	db=cb0^cb1;
	da=ca0^ca1;
	d=2*dg+dr+db+da;
	if(d<0)
		return(65536);
	return(d);
}

int LCIF_EncodeQuantizeColorPlane(
	byte *pix, u64 *blkbuf, int xs, int ys, int errt)
{
	byte pixtab[64*4];
	byte *cs0, *cs1;
	u64 *csb;
	int cy0, cu0, cv0, ca0;
	int cy1, cu1, cv1, ca1;
	int cy2, cu2, cv2, ca2;
	int cy3, cu3, cv3, ca3;
	int bi0, bi1, bd0, bd1, d0, d1, h0, h1;
	int x, y;
	int i, j, k;
	
	for(y=0; y<ys; y++)
	{
		cs0=pix+((y*2+0)*xs*4);
		cs1=pix+((y*2+1)*xs*4);
		csb=blkbuf+(y*xs);
		for(x=0; x<xs; x++)
		{
			cy0=cs0[0];	cu0=cs0[1];	cv0=cs0[2];	ca0=cs0[3];
			cy1=cs1[0];	cu1=cs1[1];	cv1=cs1[2];	ca1=cs1[3];
			
			bd0=65536;
			bd1=65536;
			for(i=0; i<64; i++)
			{
				cy2=pixtab[i*4+0];
				cu2=pixtab[i*4+1];
				cv2=pixtab[i*4+2];
				ca2=pixtab[i*4+3];
				d0=LCIF_LossyCalcErrorYUVA(
					cy0, cu0, cv0, ca0, cy2, cu2, cv2, ca2);
				d1=LCIF_LossyCalcErrorYUVA(
					cy1, cu1, cv1, ca1, cy2, cu2, cv2, ca2);
				if(d0<bd0)
					{ bd0=d0; bi0=i; }
				if(d1<bd1)
					{ bd1=d1; bi1=i; }
			}
			
			if((bd0<errt) && (bd1<errt))
			{
				cy2=pixtab[bi0*4+0];	cu2=pixtab[bi0*4+1];
				cv2=pixtab[bi0*4+2];	ca2=pixtab[bi0*4+3];
				cy3=pixtab[bi1*4+0];	cu3=pixtab[bi1*4+1];
				cv3=pixtab[bi1*4+2];	ca3=pixtab[bi1*4+3];

				cs0[0]=cy2;	cs0[1]=cu2;	cs0[2]=cv2;	cs0[3]=ca2;
				cs1[0]=cy3;	cs1[1]=cu3;	cs1[2]=cv3;	cs1[3]=ca3;
				
				if(bi0==bi1)
				{
					*csb=0;
				}
				
				cs0+=4;
				cs1+=4;
				csb++;
				continue;
			}

			h0=(cy0*3+cu0*5+cv0*7+ca0*11)&63;
			h1=(cy1*3+cu1*5+cv1*7+ca1*11)&63;
			
			pixtab[h0*4+0]=cy0;	pixtab[h0*4+1]=cu0;
			pixtab[h0*4+2]=cv0;	pixtab[h0*4+3]=ca0;
			
			pixtab[h1*4+0]=cy1;	pixtab[h1*4+1]=cu1;
			pixtab[h1*4+2]=cv1;	pixtab[h1*4+3]=ca1;
	
			cs0+=4;
			cs1+=4;
			csb++;
		}
	}
	return(0);
}

int LCIF_EncodeImageBuffer(byte *dstbuf, byte *img, int xs, int ys, int flags)
{
	byte *clrbuf;
	u64 *blkbuf;
	byte *ct, *ctr0, *ctr1;
	int xs2, ys2, errt;
	int x, y;
	
	dstbuf[ 0]='l';
	dstbuf[ 1]='c';
	dstbuf[ 2]='i';
	dstbuf[ 3]='f';
	
	dstbuf[ 7]=xs>>24;
	dstbuf[ 6]=xs>>16;
	dstbuf[ 5]=xs>> 8;
	dstbuf[ 4]=xs>> 0;

	dstbuf[11]=ys>>24;
	dstbuf[10]=ys>>16;
	dstbuf[ 9]=ys>> 8;
	dstbuf[ 8]=ys>> 0;

	dstbuf[12]=3;
	dstbuf[13]=0;
	
	ct=dstbuf+16;

	xs2=(xs+3)>>2;
	ys2=(ys+3)>>2;
	clrbuf=malloc(xs2*ys2*2*4);
	blkbuf=malloc(xs2*ys2*8);
	
	errt=(flags&0xE)*4+4;
	
	for(y=0; y<ys2; y++)
	{
		for(x=0; x<xs2; x++)
		{
			LCIF_EncodeCellBlockPixels(
				img+((((y*4)*xs)+(x*4))*4),
				xs,
				blkbuf+y*xs2+x,
				clrbuf+((y*2+0)*xs2+x)*4,
				clrbuf+((y*2+1)*xs2+x)*4,
				errt);
		}
	}

	LCIF_EncodeQuantizeColorPlane(clrbuf, blkbuf, xs2, ys2, errt);

	BTIC1H_Img_SaveTGA("lciftst_enccplane.tga", clrbuf, xs2, ys2*2);

	ctr0=ct;
	ct=LCIF_EncodeColorPlane(ct, clrbuf, xs2, ys2*2);
	*ct++=0x00;
	*ct++=0x00;

	ctr1=ct;
	ct=LCIF_EncodeBlockPlane(ct, blkbuf, xs2, ys2);
	*ct++=0x00;
	*ct++=0x00;
	
	printf("Color Plane %d\n", ctr1-ctr0);
	printf("Block Plane %d\n", ct-ctr1);

	printf("Enc Hash Color Plane %08X\n",
		LCIF_HashBuffer((byte *)clrbuf, xs2*ys2*2*4));
	printf("Enc Hash Block Plane %08X\n",
		LCIF_HashBuffer((byte *)blkbuf, xs2*ys2*8));

	printf("Total %d\n", ct-dstbuf);
	
	return(ct-dstbuf);
}
