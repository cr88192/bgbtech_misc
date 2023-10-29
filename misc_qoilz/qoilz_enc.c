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

int QOILZ_CheckMatchLz4(byte *str1, byte *str2, int nl);

int QOI_EncLiHash4(byte *cs)
{
	u32 v0, v1, v2, v3;
	u64 h;
	
	v0=((u32 *)cs)[0];
	v1=((u32 *)cs)[1];
	v2=((u32 *)cs)[2];
	v3=((u32 *)cs)[3];
	h=v0;
	h=h*65521+v1;
	h=h*65521+v2;
	h=h*65521+v3;
	h=h*65521+(h>>16);
	h=h*65521+(h>>16);
//	return((h>>16)&255);
	return((h>>16)&4095);
}

int QOI_EncodeImageBuffer(byte *dstbuf, byte *img, int xs, int ys, int doli)
{
	byte pixtab[64*4];
	int hofs[4096];
	byte hrov[4096];
	byte *cs, *ct, *cs1, *cse, *imgbuf;
	int cr, cg, cb, ca;
	int dr, dg, db;
	int crl, cgl, cbl, cal;
	int n, run, bl, bd;
	int i, j, k, h, hli, hr;
	
	dstbuf[ 0]='q';
	dstbuf[ 1]='o';
	dstbuf[ 2]='i';
	dstbuf[ 3]='f';
	
	if(doli&1)
	{
		dstbuf[ 2]='l';
		dstbuf[ 3]='i';
	}

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
	
	for(i=0; i<256; i++)
	{
//		pixtab[i*4+0]=0;
//		pixtab[i*4+1]=0;
//		pixtab[i*4+2]=0;
//		pixtab[i*4+3]=0;
		pixtab[i]=0;
//		hofs[i]=0;
	}

	for(i=0; i<4096; i++)
	{
		hofs[i]=0;
		hrov[i]=0;
	}
	
	crl=0;	cgl=0;
	cbl=0;	cal=255;
	run=0;
	while(cs<cse)
	{
//		hli=((j*251+1)>>8)&255;
		hli=QOI_EncLiHash4(cs);
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
			if(doli&1)
			{
				while(run>=48)
				{
					*ct++=0xEF;
					run-=48;
				}
			}else
			{
				while(run>=62)
				{
					*ct++=0xFD;
					run-=62;
				}
			}
			if(run>0)
			{
				*ct++=0xC0+(run-1);
			}
			run=0;
		}

		if(doli&1)
		{
			bl=0;	bd=0;
			for(i=0; i<32; i++)
			{
				cs1=img+hofs[(hli+((hr-i)&31))&4095];
				if((cs1>=(cs-4)) || (cs1<img))
					continue;
				
				j=QOILZ_CheckMatchLz4(cs1, cs-4, 16384);
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
			
//			j=0;
			
//			if((j>=5) && (j<132) && (k<4080) && (k>0))
//			if((j>=5) && (j<1027) && (k<65520) && (k>0))
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
					hli=QOI_EncLiHash4(cs);
					hr=hrov[hli];
//					cr=cs[0];
//					cg=cs[1];
//					cb=cs[2];
//					ca=cs[3];
					cs+=4;
					
//					j=cr*3+cg*5+cb*7+ca*11;
//					hli=((j*251+1)>>8)&255;
//					hofs[hli]=(cs-4)-img;
					hofs[(hli+hr)&4095]=(cs-4)-img;
					hrov[hli]=(hr+1)&31;
				}
				
//				cs+=(j-1)*4;
//				crl=cr;
//				cgl=cg;
//				cbl=cb;
//				cal=ca;
				
				crl=cs[-4];
				cgl=cs[-3];
				cbl=cs[-2];
				cal=cs[-1];
				continue;
			}
		}

//		j=cr*3+cg*5+cb*7+ca*11;
//		hli=((j*251+1)>>8)&255;

//		j=cr*3+cg*5+cb*7+ca*11;
//		h=j&63;
//		hli=j&255;
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
//			hofs[hli]=(cs-4)-img;
			hofs[(hli+hr)&4095]=(cs-4)-img;
			hrov[hli]=(hr+1)&31;
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
//			hofs[hli]=(cs-4)-img;
			hofs[(hli+hr)&4095]=(cs-4)-img;
			hrov[hli]=(hr+1)&31;
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
//		hofs[hli]=(cs-4)-img;
		hofs[(hli+hr)&4095]=(cs-4)-img;
		hrov[hli]=(hr+1)&31;
		
		crl=cr;
		cgl=cg;
		cbl=cb;
		cal=ca;
	}

	if(run)
	{
		if(doli&1)
		{
			while(run>=48)
			{
				*ct++=0xEF;
				run-=48;
			}
		}else
		{
			while(run>=62)
			{
				*ct++=0xFD;
				run-=62;
			}
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
	
	sz=QOI_EncodeImageBuffer(dstbuf, ibuf, xs, ys, 0);
	free(ibuf);
	return(sz);
}


// #define QOILZ4_ROVMSK	255
#define QOILZ4_ROVMSK	63
// #define QOILZ4_ROVMSK	31

// #define QOILZ4_HASHSZ	4096
// #define QOILZ4_HASHSZ	8192
// #define QOILZ4_HASHSZ	16384
#define QOILZ4_HASHSZ	65536
#define QOILZ4_HASHMSK	(QOILZ4_HASHSZ-1)
// #define QOILZ4_HASHSHR	18
#define QOILZ4_HASHSHR	16

#define QOILZ4_HASH1MSK QOILZ4_HASHMSK
// #define QOILZ4_HASH1MSK	(QOILZ4_HASHMSK&(~QOILZ4_ROVMSK))

#define QOILZ4_MALLOCHASH

// #define QOILZ4_SDEPTH	32
#define QOILZ4_SDEPTH	56

int QOILZ_CheckMatchLz4(byte *str1, byte *str2, int nl)
{
	byte *cs1, *cs1e, *cs2;

	if((*(u32 *)str1)!=(*(u32 *)str2))
		return(0);
	cs1=str1+4;
	cs2=str2+4;
	cs1e=str1+nl;
	while(cs1<cs1e)
	{
		if((*(u64 *)cs1)!=(*(u64 *)cs2))
			break;
		cs1+=8; cs2+=8;
	}

	while(cs1<cs1e)
	{
		if((*cs1)!=(*cs2))
			break;
		cs1++; cs2++;
	}
	return(cs1-str1);
}

int QOILZ_PackBufferLz4(byte *dstbuf, byte *srcbuf, int sz)
{
//	byte *hash[QOILZ4_HASHSZ];
#ifdef QOILZ4_MALLOCHASH
	int *hofs;
	byte *hrov;
#else
	int hofs[QOILZ4_HASHSZ];
	byte hrov[QOILZ4_HASHSZ];
#endif
	byte *cs, *cse, *csn, *ct, *cs1, *csr;
	int i, j, k, h, d, l, nl, r, tg, bd, bl, hr, h1;
	int bdl, bll, cf;

#ifdef QOILZ4_MALLOCHASH
	hofs=malloc(QOILZ4_HASHSZ*sizeof(int));
	hrov=malloc(QOILZ4_HASHSZ);
#endif
	
	for(i=0; i<QOILZ4_HASHSZ; i++)
	{
//		hash[i]=srcbuf;
		hofs[i]=0;
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
		h=((h*65521+1)>>QOILZ4_HASHSHR)&QOILZ4_HASH1MSK;
		
		bd=0; bl=0;
		hr=hrov[h];
		
//		for(i=0; i<32; i++)
		for(i=0, j=0; i<QOILZ4_SDEPTH; i++, j+=7)
		{
//			h1=(h+((hr-i-1)&15)*31)&4095;
//			h1=(h+((hr-(i+0)*7)&QOILZ4_ROVMSK))&QOILZ4_HASHMSK;
			h1=(h+((hr-j)&QOILZ4_ROVMSK))&QOILZ4_HASHMSK;
//			cs1=hash[h1];
			cs1=srcbuf+hofs[h1];
//			d=csn-cs1; l=0;
//			if((d>0) && (d<65520))
//			if(1)
			if(*cs1==*csn)
			{
				nl=cse-cs;
				d=csn-cs1;
//				nl=4096;
//				if((cse-cs)<nl)
//					nl=cse-csn;

				if(nl>4096)
					nl=4096;

//				for(l=0; l<nl; l++)
//					if(csn[l]!=cs1[l])
//						break;
				l=QOILZ_CheckMatchLz4(csn, cs1, nl);
	//			if(l>d)
	//				l=0;

				if((l>bl) && (d<65520) && (l>4) && (d>0))
				{
					bl=l;
					bd=d;
				}
			}
		}

		d=bd;
		l=bl;

#if 1
		r=cs-csr;
		cf=3;
		if(r>=15)
			{ cf+=1+(r>>8); }
		if(l>=19)
			{ cf+=1+(l>>8); }

		if(d>4096)
			if(l<8)
				l=0;
		if(cf>=l)
			l=0;
#endif

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
			h=((h*65521+1)>>QOILZ4_HASHSHR)&QOILZ4_HASH1MSK;

			hr=hrov[h];

//			hash[h]=cs;
			h1=(h+hr)&QOILZ4_HASHMSK;
//			hash[h1]=cs;
			hofs[h1]=cs-srcbuf;
			hrov[h]=(hr+7)&QOILZ4_ROVMSK;
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
			h=((h*65521+1)>>QOILZ4_HASHSHR)&QOILZ4_HASH1MSK;

//			hash[h]=csr;

			hr=hrov[h];
			h1=(h+hr)&QOILZ4_HASHMSK;
//			hash[h1]=cs;
			hofs[h1]=cs-srcbuf;
			hrov[h]=(hr+7)&QOILZ4_ROVMSK;

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

#ifdef QOILZ4_MALLOCHASH
	free(hofs);
	free(hrov);
#endif
	
	return(ct-dstbuf);
}

int QOILZ_EncodeImageBuffer(byte *dstbuf, byte *img, int xs, int ys)
{
	byte *t1buf, *t2buf, *t3buf;
	int sz1, sz2, sz3, sz0, sz;
	
	t1buf=malloc(2*xs*ys);
	t2buf=malloc(2*xs*ys);
	t3buf=malloc(2*xs*ys);
	sz1=QOI_EncodeImageBuffer(t1buf, img, xs, ys, 0);

	sz3=QOI_EncodeImageBuffer(t3buf, img, xs, ys, 1);
	
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
//	if(1)
	{
		if((sz2*1.25)<sz3)
//		if(0)
		{
			memcpy(dstbuf, t2buf, sz2);
			sz=sz2;
		}else
		{
			memcpy(dstbuf, t3buf, sz3);
			sz=sz3;
		}
	}else
	{
		if((sz3*1.10)<sz1)
		{
			memcpy(dstbuf, t3buf, sz3);
			sz=sz3;
		}else
		{
			memcpy(dstbuf, t1buf, sz1);
			sz=sz1;
		}
	}

	free(t1buf);
	free(t2buf);
	free(t3buf);
	
	printf("QOILZ_EncodeImageBuffer: QOI=%d LZ=%d LI=%d Pick=%d\n",
		sz1, sz2, sz3, sz);
	
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
