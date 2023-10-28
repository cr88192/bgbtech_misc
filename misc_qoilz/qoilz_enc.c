#ifndef BTIC2F_BYTE
#define BTIC2F_BYTE
typedef unsigned char			byte;
typedef unsigned short		u16;
typedef unsigned int			u32;
typedef unsigned long long	u64;

typedef signed char			sbyte;
typedef signed short			s16;
typedef signed int			s32;
typedef signed long long		s64;
#endif

int QOI_EncodeImageBuffer(byte *dstbuf, byte *img, int xs, int ys)
{
	byte pixtab[64*4];
	byte *cs, *ct, *cse, *imgbuf;
	int cr, cg, cb, ca;
	int dr, dg, db;
	int crl, cgl, cbl, cal;
	int n, run;
	int i, j, k, h;
	
	dstbuf[ 0]='q';
	dstbuf[ 1]='o';
	dstbuf[ 2]='i';
	dstbuf[ 3]='f';

	dstbuf[ 4]=xs>>24;
	dstbuf[ 5]=xs>>16;
	dstbuf[ 6]=xs>> 8;
	dstbuf[ 7]=xs>> 0;

	dstbuf[ 8]=ys>>24;
	dstbuf[ 9]=ys>>16;
	dstbuf[10]=ys>> 8;
	dstbuf[11]=ys>> 0;

	dstbuf[12]=4;
	dstbuf[13]=1;
	
	n=xs*ys;
	cs=img; cse=img+n*4;
	
	ct=dstbuf+14;
	
	crl=0;	cgl=0;
	cbl=0;	cal=255;
	run=0;
	while(cs<cse)
	{
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
			while(run>=62)
			{
				*ct++=0xFD;
				run-=62;
			}
			if(run>0)
			{
				*ct++=0xC0+(run-1);
			}
			run=0;
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
			crl=cr;	cgl=cg;
			cbl=cb;	cal=ca;
			continue;
		}
		
		dr-=dg;
		db-=dg;

		if(	(dg>=-32) && (dg<=31) &&
			(dr>=-8) && (dr<=7) &&
			(db>=-8) && (db<=7) &&
			(ca==cal))
		{
			*ct++=0x80|(dg+32);
			*ct++=((dr+8)<<4)|((db+8)<<0);
			h=(cr*3+cg*5+cb*7+ca*11)&63;
			k=h<<2;
			pixtab[k+0]=cr;	pixtab[k+1]=cg;
			pixtab[k+2]=cb;	pixtab[k+3]=ca;
			crl=cr;	cgl=cg;
			cbl=cb;	cal=ca;
			continue;
		}
		
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
		
		crl=cr;
		cgl=cg;
		cbl=cb;
		cal=ca;
	}

	if(run)
	{
		while(run>=62)
		{
			*ct++=0xFD;
			run-=62;
		}
		if(run>0)
		{
			*ct++=0xC0+(run-1);
		}
		run=0;
	}

	*ct++=0x00;
	*ct++=0x00;
	*ct++=0x00;
	*ct++=0x00;
	*ct++=0x00;
	*ct++=0x00;
	*ct++=0x00;
	*ct++=0x01;
	
	return(ct-dstbuf);
}

int QOI_EncodeImageBuffer555(byte *dstbuf, u16 *img, int xs, int ys)
{
	byte *ibuf, *ct;
	u16 *cs;
	int cr, cg, cb;
	int x, y, v, sz;
	
	ibuf=malloc(xs*ys*4);
	ct=ibuf;
	for(y=0; y<ys; y++)
	{
		cs=img+(ys-y-1)*xs;
		for(x=0; x<xs; x++)
		{
			v=*cs++;
			cr=(v>>10)&31;
			cg=(v>> 5)&31;
			cb=(v>> 0)&31;
			cr=(cr<<3)|(cr>>2);
			cg=(cg<<3)|(cg>>2);
			cb=(cb<<3)|(cb>>2);
			
			ct[0]=cr;
			ct[1]=cg;
			ct[2]=cb;
			ct[3]=255;
			ct+=4;
		}
	}
	
	sz=QOI_EncodeImageBuffer(dstbuf, ibuf, xs, ys);
	free(ibuf);
	return(sz);
}

int QOILZ_PackBufferLz4(byte *dstbuf, byte *srcbuf, int sz)
{
	byte *hash[4096];
	byte hrov[4096];
	byte *cs, *cse, *csn, *ct, *cs1, *csr;
	int i, j, k, h, d, l, nl, r, tg, bd, bl, hr, h1;
	int bdl, bll;
	
	for(i=0; i<4096; i++)
	{
		hash[i]=srcbuf;
		hrov[i]=0;
	}
	
	ct=dstbuf;
	cs=srcbuf;
	cse=srcbuf+sz;
	csr=cs;
	
	while(cs<cse)
	{
//		h=cs[0]+1;
//		h=h*251+cs[1];
//		h=h*251+cs[2];
//		h=h*251+cs[3];

//		csn=cs+1;
		csn=cs;
		h=csn[0]+1;
		h=h*251+csn[1];
		h=h*251+csn[2];
		h=h*251+csn[3];

//		h=((h*251+1)>>24)&255;
//		h=((h*251+1)>>20)&4095;
		h=((h*65521+1)>>20)&4095;
		
		bd=0; bl=0;
		hr=hrov[h];
		
		for(i=0; i<32; i++)
		{
//			h1=(h+((hr-i-1)&15)*31)&4095;
			h1=(h+((hr-(i+1)*7)&255))&4095;
			cs1=hash[h1];
			d=csn-cs1; l=0;
			if((d>0) && (d<65520))
			{
				nl=4096;
				if((cse-cs)<nl)
					nl=cse-csn;
				for(l=0; l<nl; l++)
					if(csn[l]!=cs1[l])
						break;
	//			if(l>d)
	//				l=0;

				if((l>4) && (l>bl))
				{
					bl=l;
					bd=d;
				}
			}
		}

		d=bd;
		l=bl;
		
//		d=bdl;
//		l=bll;
//		if((bl+1)>bll)
//			l=0;
		
		if((d<=0) || (l<4) || (d>=65520))
		{
			bdl=bd;
			bll=bl;

			h=cs[0]+1;
			h=h*251+cs[1];
			h=h*251+cs[2];
			h=h*251+cs[3];
//			h=((h*251+1)>>20)&4095;
			h=((h*65521+1)>>20)&4095;

			hr=hrov[h];

//			hash[h]=cs;
			h1=(h+hr)&4095;
			hash[h1]=cs;
			hrov[h]=(hr+7)&255;
			cs++;
			continue;
		}

		r=cs-csr;
		
//		if((cs-srcbuf)<1024)
//			printf("E: r=%d d=%d l=%d\n", r, d, l);
		
		tg=((r<<4)&0xF0)|((l-4)&0x0F);
		if(r>=15)
			tg|=0xF0;
		if(l>=19)
			tg|=0x0F;
		*ct++=tg;

		i=r;
		if(i>=15)
		{
			i-=15;
			while(i>=255)
			{
				*ct++=0xFF;
				i-=255;
			}
			*ct++=i;
		}
		
		memcpy(ct, csr, r);
		ct+=r;	csr+=r;
		
		*ct++=d;
		*ct++=d>>8;
		
		i=l;
		if(i>=19)
		{
			i-=19;
			while(i>=255)
			{
				*ct++=0xFF;
				i-=255;
			}
			*ct++=i;
		}
		
		cs+=l;
#if 1
		while(csr<cs)
		{
			h=csr[0]+1;
			h=h*251+csr[1];
			h=h*251+csr[2];
			h=h*251+csr[3];
//			h=((h*251+1)>>24)&255;
//			h=((h*251+1)>>20)&4095;
			h=((h*65521+1)>>20)&4095;

//			hash[h]=csr;

			hr=hrov[h];
			h1=(h+hr)&4095;
			hash[h1]=cs;
			hrov[h]=(hr+7)&255;

			csr++;
		}
#endif

		csr=cs;
		bdl=0;
		bll=0;
	}

	r=cs-csr;
	if(r>0)
	{
		l=4;
		tg=((r<<4)&0xF0)|((l-4)&15);
		if(r>=15)
			tg|=0xF0;
		if(l>=19)
			tg|=0x0F;
		*ct++=tg;

		i=r;
		if(i>=15)
		{
			i-=15;
			while(i>=255)
			{
				*ct++=0xFF;
				i-=255;
			}
			*ct++=i;
		}
		
		memcpy(ct, csr, r);
		ct+=r;
		csr=cs;
	}
	ct[0]=0;
	ct[1]=0;
	
	return(ct-dstbuf);
}

int QOILZ_EncodeImageBuffer(byte *dstbuf, byte *img, int xs, int ys)
{
	byte *t1buf, *t2buf;
	int sz1, sz2, sz0, sz;
	
	t1buf=malloc(2*xs*ys);
	t2buf=malloc(2*xs*ys);
	sz1=QOI_EncodeImageBuffer(t1buf, img, xs, ys);
	
	t2buf[0]='q';
	t2buf[1]='o';
	t2buf[2]='l';
	t2buf[3]='4';

	t2buf[ 4]=xs>>24;
	t2buf[ 5]=xs>>16;
	t2buf[ 6]=xs>> 8;
	t2buf[ 7]=xs>> 0;

	t2buf[ 8]=ys>>24;
	t2buf[ 9]=ys>>16;
	t2buf[10]=ys>> 8;
	t2buf[11]=ys>> 0;

	t2buf[12]=4;
	t2buf[13]=1;
	t2buf[14]=0;
	t2buf[15]=0;

	sz0=sz1-14;
	t2buf[16]=sz0>>24;
	t2buf[17]=sz0>>16;
	t2buf[18]=sz0>> 8;
	t2buf[19]=sz0>> 0;

	printf("Enc Hash %08X\n", QOILZ_HashBuffer(t1buf+14, sz0));
	
	sz2=QOILZ_PackBufferLz4(t2buf+24, t1buf+14, sz0);

	t2buf[20]=sz2>>24;
	t2buf[21]=sz2>>16;
	t2buf[22]=sz2>> 8;
	t2buf[23]=sz2>> 0;

	sz2+=24;
	
//	if(sz2<sz1)
	if((sz2*1.25)<sz1)
	{
		memcpy(dstbuf, t2buf, sz2);
		sz=sz2;
	}else
	{
		memcpy(dstbuf, t1buf, sz1);
		sz=sz1;
	}
	
	printf("QOILZ_EncodeImageBuffer: QOI=%d LZ=%d Pick=%d\n", sz1, sz2, sz);
	
	return(sz);
}


int QOILZ_EncodeImageBuffer555(byte *dstbuf, u16 *img, int xs, int ys)
{
	byte *ibuf, *ct;
	u16 *cs;
	int cr, cg, cb;
	int x, y, v, sz;
	
	ibuf=malloc(xs*ys*4);
	ct=ibuf;
	for(y=0; y<ys; y++)
	{
		cs=img+(ys-y-1)*xs;
		for(x=0; x<xs; x++)
		{
			v=*cs++;
			cr=(v>>10)&31;
			cg=(v>> 5)&31;
			cb=(v>> 0)&31;
			cr=(cr<<3)|(cr>>2);
			cg=(cg<<3)|(cg>>2);
			cb=(cb<<3)|(cb>>2);
			
			ct[0]=cr;
			ct[1]=cg;
			ct[2]=cb;
			ct[3]=255;
			ct+=4;
		}
	}
	
	sz=QOILZ_EncodeImageBuffer(dstbuf, ibuf, xs, ys);
	free(ibuf);
	return(sz);
}
