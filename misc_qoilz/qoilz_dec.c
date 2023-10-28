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

byte *QOI_DecImageBuffer(byte *inbuf, int *rxs, int *rys)
{
	byte pixtab[64*4];
	byte *cs, *ct, *cte, *imgbuf;
	int cr, cg, cb, ca;
	int xs, ys, n;
	int i, j, k, h;
	
	if(	(inbuf[0]!='q') ||
		(inbuf[1]!='o') ||
		(inbuf[2]!='i') ||
		(inbuf[3]!='f') )
	{
		return(NULL);
	}

	xs=	(inbuf[ 4]<<24) |
		(inbuf[ 5]<<16) |
		(inbuf[ 6]<< 8) |
		(inbuf[ 7]<< 0) ;
	ys=	(inbuf[ 8]<<24) |
		(inbuf[ 9]<<16) |
		(inbuf[10]<< 8) |
		(inbuf[11]<< 0) ;
	cs=inbuf+14;

	n=xs*ys;
	imgbuf=malloc(n*4);
	ct=imgbuf;
	cte=ct+n*4;
	
	for(i=0; i<256; i++)
		pixtab[i]=0;
	
	cr=0;	cg=0;
	cb=0;	ca=255;
	while(ct<cte)
	{
		j=*cs++;
		i=j>>6;

		if(i==0)
		{
			k=j&63;
			k=k<<2;
			cr=pixtab[k+0];	cg=pixtab[k+1];
			cb=pixtab[k+2];	ca=pixtab[k+3];
			ct[0]=cr;	ct[1]=cg;
			ct[2]=cb;	ct[3]=ca;
			ct+=4;
			continue;
		}

		if(i==1)
		{
			cr+=((j>>4)&3)-2;
			cg+=((j>>2)&3)-2;
			cb+=((j>>0)&3)-2;
			h=(cr*3+cg*5+cb*7+ca*11)&63;
			k=h<<2;
			pixtab[k+0]=cr;	pixtab[k+1]=cg;
			pixtab[k+2]=cb;	pixtab[k+3]=ca;
			ct[0]=cr;	ct[1]=cg;
			ct[2]=cb;	ct[3]=ca;
			ct+=4;
			continue;
		}

		if(i==2)
		{
			k=(j&63)-32;
			cr+=k;
			cg+=k;
			cb+=k;

			j=*cs++;
			cr+=((j>>4)&15)-8;
			cb+=((j>>0)&15)-8;

			h=(cr*3+cg*5+cb*7+ca*11)&63;
			k=h<<2;
			pixtab[k+0]=cr;	pixtab[k+1]=cg;
			pixtab[k+2]=cb;	pixtab[k+3]=ca;
			ct[0]=cr;	ct[1]=cg;
			ct[2]=cb;	ct[3]=ca;
			ct+=4;
			continue;
		}

		k=j&63;
		if(k<62)
		{
			k++;
			while(k--)
			{
				ct[0]=cr;	ct[1]=cg;
				ct[2]=cb;	ct[3]=ca;
				ct+=4;
			}
			continue;
		}
		
		if(j==0xFE)
		{
			cr=cs[0];
			cg=cs[1];
			cb=cs[2];
			cs+=3;

			h=(cr*3+cg*5+cb*7+ca*11)&63;
			k=h<<2;
			pixtab[k+0]=cr;	pixtab[k+1]=cg;
			pixtab[k+2]=cb;	pixtab[k+3]=ca;
			ct[0]=cr;	ct[1]=cg;
			ct[2]=cb;	ct[3]=ca;
			ct+=4;
			continue;
		}

		
		if(j==0xFF)
		{
			cr=cs[0];
			cg=cs[1];
			cb=cs[2];
			ca=cs[3];
			cs+=4;

			h=(cr*3+cg*5+cb*7+ca*11)&63;
			k=h<<2;
			pixtab[k+0]=cr;	pixtab[k+1]=cg;
			pixtab[k+2]=cb;	pixtab[k+3]=ca;
			ct[0]=cr;	ct[1]=cg;
			ct[2]=cb;	ct[3]=ca;
			ct+=4;
			continue;
		}
	}
	
	*rxs=xs;
	*rys=ys;
	return(imgbuf);
}

void QOILZ_LzMemCpy(byte *dst, byte *src, int sz)
{
	byte *cs, *ct, *cte;
	int i, d;
	
	d=dst-src;

#if 1
	if(d<=0)
	{
		if(d==0)
			return;
		memmove(dst, src, sz);
		return;
	}

	if(d>sz)
	{
		memcpy(dst, src, sz);
		return;
	}
#endif
	
	if(d>=8)
//	if(0)
	{
		cs=src; ct=dst; cte=ct+sz;
		while(ct<cte)
		{
			memcpy(ct, cs, 8);
			ct+=8; cs+=8;
		}
	}else
	{
		cs=src; ct=dst; cte=ct+sz;
		for(i=0; i<sz; i++)
			ct[i]=cs[i];
#if 0
		while(ct<cte)
		{
			*(u64 *)ct=*(u64 *)cs;
//			memcpy(ct, cs, 8);
			ct+=8; cs+=8;
		}
#endif
	}
}

u32 QOILZ_HashBuffer(byte *buf, int sz)
{
	u32 *bufi;
	u64 s0, s1;
	u32 v;
	int i, j, k, n;
	
	bufi=(u32 *)buf;
	n=sz/4; s0=1; s1=1;
	for(i=0; i<n; i++)
	{
		v=bufi[i];
		s0+=v;
		s1+=s0;
	}
	
	s0=((u32)s0)+(s0>>32);
	s1=((u32)s1)+(s1>>32);
	s0=((u32)s0)+(s0>>32);
	s1=((u32)s1)+(s1>>32);
	v=s0^s1;

	return(v);
}

int QOILZ_UnpackLz4(byte *dst, byte *src, int dsz, int ssz)
{
	byte *ct, *cte, *cs, *cse;
	int tg, l, d, r;
	int i;
	
	ct=dst; cte=dst+dsz; cs=src; cse=src+ssz;
	while(ct<cte)
	{
		tg=*cs++;
		r=(tg>>4)&15;
		l=(tg&15)+4;
		
		if(r>=15)
		{
			i=*cs++;
			while(i==255)
			{
				r+=255;
				i=*cs++;
			}
			r+=i;
		}
		
		QOILZ_LzMemCpy(ct, cs, r);
		ct+=r;
		cs+=r;
		if((ct>=cte) || (cs>=cse))
			break;
		d=cs[0]|(cs[1]<<8);
		cs+=2;
		
		if(!d)
		{
			printf("QOILZ_UnpackLz4: EOB, Dist=0\n");
			break;
		}

		if(l>=19)
		{
			i=*cs++;
			while(i==255)
			{
				l+=255;
				i=*cs++;
			}
			l+=i;
		}

//		if((ct-dst)<1024)
//			printf("D: r=%d d=%d l=%d\n", r, d, l);

		QOILZ_LzMemCpy(ct, ct-d, l);
		ct+=l;
	}
	
	if((cs!=cse) || (ct!=cte))
	{
		printf("QOILZ_UnpackLz4: Size Issue dst=%d/%d src=%d/%d",
			ct-dst, dsz,
			cs-src, ssz);
	}
	
	return(cs-src);
}

byte *QOILZ_DecImageBuffer(byte *inbuf, int *rxs, int *rys)
{
	byte *tbuf, *i1buf;
	int sz0, sz1, sz2, xs, ys;

	if(	(inbuf[0]=='q') &&
		(inbuf[1]=='o') &&
		(inbuf[2]=='i') &&
		(inbuf[3]=='f') )
	{
		return(QOI_DecImageBuffer(inbuf, rxs, rys));
	}

	if(	(inbuf[0]!='q') ||
		(inbuf[1]!='o') ||
		(inbuf[2]!='l') ||
		(inbuf[3]!='4') )
	{
		return(NULL);
	}
	
	sz0=(inbuf[16]<<24) |
		(inbuf[17]<<16) |
		(inbuf[18]<< 8) |
		(inbuf[19]<< 0) ;

	sz2=(inbuf[20]<<24) |
		(inbuf[21]<<16) |
		(inbuf[22]<< 8) |
		(inbuf[23]<< 0) ;

	sz1=14+sz0;
	tbuf=malloc(sz1+256);
	memcpy(tbuf, inbuf, 14);
	tbuf[2]='i';
	tbuf[3]='f';
	
	QOILZ_UnpackLz4(tbuf+14, inbuf+24, sz0, sz2);

	printf("Dec Hash %08X\n", QOILZ_HashBuffer(tbuf+14, sz0));

	i1buf=QOI_DecImageBuffer(tbuf, &xs, &ys);
	if(!i1buf)
		return(NULL);
	
	*rxs=xs;
	*rys=ys;
	return(i1buf);
}

u16 *QOILZ_DecImageBuffer555(byte *inbuf, int *rxs, int *rys)
{
	byte *i1buf;
	u16 *i2buf, *ct;
	byte *cs, *cse;
	int cr, cg, cb;
	int x, y, xs, ys, n, v;
	
	i1buf=QOILZ_DecImageBuffer(inbuf, &xs, &ys);
	if(!i1buf)
		return(NULL);
		
	n=xs*ys;
	i2buf=malloc(xs*ys*2);
	
	cs=i1buf; cse=i1buf+n*4;
	ct=i2buf;

#if 0
	while(cs<cse)
	{
		cr=cs[0];	cg=cs[1];
		cb=cs[2];	cs+=4;
		
		cr>>=3;	cg>>=3;	cb>>=3;
		v=(cr<10)|(cg<<5)|cb;
		*ct++=v;
	}
#endif

#if 1
	for(y=0; y<ys; y++)
	{
		cs=i1buf+(ys-y-1)*xs*4;
		for(x=0; x<xs; x++)
		{
			cr=cs[0];	cg=cs[1];
			cb=cs[2];	cs+=4;
			
			cr>>=3;	cg>>=3;	cb>>=3;
			v=(cr<<10)|(cg<<5)|cb;
			*ct++=v;
		}
	}
#endif
	
	free(i1buf);
	
	*rxs=xs;
	*rys=ys;
	return(i2buf);
}
