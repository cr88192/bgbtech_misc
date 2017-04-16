/*
Copyright (C) 2016 by Brendan G Bohannon

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

int BTIC4B_Img_StoreHDR_RGBE(FILE *fd, byte *buf, int w, int h)
{
//	fprintf(fd, "#?RADIANCE\n");
	fprintf(fd, "#?RGBE\n");
	fprintf(fd, "BTIC4B RGBE Export\n");
	fprintf(fd, "FORMAT=32-bit_rle_rgbe\n\n");
	fprintf(fd, "-Y %d +X %d\n", h, w);
	fwrite(buf, 1, w*h*4, fd);
	return(0);
}

int BTIC4B_Img_SaveHDR_RGBE(
	char *name, byte *buf, int w, int h)
{
	FILE *fd;
	
	fd=fopen(name, "wb");
	if(!fd)return(-1);
	BTIC4B_Img_StoreHDR_RGBE(fd, buf, w, h);
	fclose(fd);
	return(0);
}

u32 btic4b_img_rgb48torgbe(u16 cr, u16 cg, u16 cb)
{
	u32 px1;
//	int cr, cg, cb;
	int fr, fg, fb;
	int er, eg, eb, em;

//	cr=(px    )&0x7FF;	cg=(px>>11)&0x7FF;	cb=(px>>22)&0x3FF;
//	cr=(cr<<4)|(cr>>7);	cg=(cg<<4)|(cg>>7);	cb=(cb<<5)|(cb>>5);
	fr=(cr&0x3FF)|0x400; fg=(cg&0x3FF)|0x400; fb=(cb&0x3FF)|0x400;
	er=(cr>>10)&31;	eg=(cg>>10)&31;	eb=(cb>>10)&31;
	
	em=0;
	if((er>em) && (er<31))em=eg;
	if((er>em) && (eg<31))em=er;
	if((eb>em) && (eb<31))em=eb;
	
	if(em<1)
		{ return(0x71808080); }	
	fr=fr>>((em-er)+3);
	fg=fg>>((em-eg)+3);
	fb=fb>>((em-eb)+3);
	em=em+(127-15);
	fr=fr&255;	fg=fg&255;	fb=fb&255;
	
	px1=(em<<24)|(fb<<16)|(fg<<8)|fr;
	return(px1);
}

u32 btic4b_img_r11ftorgbe(u32 px)
{
	u32 px1;
	int cr, cg, cb;
	int fr, fg, fb;
	int er, eg, eb, em;

	cr=(px    )&0x7FF;	cg=(px>>11)&0x7FF;	cb=(px>>22)&0x3FF;
	cr=(cr<<4)|(cr>>7);	cg=(cg<<4)|(cg>>7);	cb=(cb<<5)|(cb>>5);
	fr=(cr&0x3FF)|0x400; fg=(cg&0x3FF)|0x400; fb=(cb&0x3FF)|0x400;
	er=cr>>10;	eg=cg>>10;	eb=cb>>10;

	em=0;
	if((er>em) && (er<31))em=eg;
	if((er>em) && (eg<31))em=er;
	if((eb>em) && (eb<31))em=eb;

	if(em<1)
		{ return(0x71808080); }

	fr=fr>>((em-er)+3);
	fg=fg>>((em-eg)+3);
	fb=fb>>((em-eb)+3);
	em=em+(127-15);

	fr=fr&255;	fg=fg&255;	fb=fb&255;
	
	px1=(em<<24)|(fb<<16)|(fg<<8)|fr;
	return(px1);
}

u32 btic4b_img_rgbetor11f(u32 px)
{
	u32 px1;
	int cr, cg, cb;
	int fr, fg, fb;
	int er, eg, eb, em;
	
	fr=(px    )&255;	fg=(px>> 8)&255;
	fb=(px>>16)&255;	em=(px>>24)&255;
	
	er=em-(127-15);
	eg=em-(127-15);
	eb=em-(127-15);
	
	fr<<=3;
	fg<<=3;
	fb<<=3;

	if(fr)
		{ while(!(fr>>10)) { fr<<=1; er--; } }
	else
		{ er=0; }
	if(fg)
		{ while(!(fg>>10)) { fg<<=1; eg--; } }
	else
		{ eg=0; }
	if(fb)
		{ while(!(fb>>10)) { fb<<=1; eb--; } }
	else
		{ eb=0; }
//	if((er<1)||(er>14)) { fr=0; er=0; }
//	if((eg<1)||(eg>14)) { fg=0; eg=0; }
//	if((eb<1)||(eb>14)) { fb=0; eb=0; }

	if(er<1) { fr=0; er=0; }
	if(eg<1) { fg=0; eg=0; }
	if(eb<1) { fb=0; eb=0; }

	if(er>30) { fr=0x3FF; er=30; }
	if(eg>30) { fg=0x3FF; eg=30; }
	if(eb>30) { fb=0x3FF; eb=30; }

	cr=(er<<10)|(fr&0x3FF);
	cg=(eg<<10)|(fg&0x3FF);
	cb=(eb<<10)|(fb&0x3FF);
	
	px1=((cb>>5)<<22)|((cg>>4)<<11)|(cr>>4);
	return(px1);
}

int BTIC4B_Img_SaveHDR_R11F(
	char *name, byte *buf, int w, int h)
{
	byte *buf1;
	u32 px, px1;
//	int cr, cg, cb;
//	int fr, fg, fb;
//	int er, eg, eb, em;
	int i, j, k;
	
	buf1=malloc(w*h*4);
	for(i=0; i<h; i++)
		for(j=0; j<w; j++)
	{
		px=((u32 *)buf)[i*w+j];

		px1=btic4b_img_r11ftorgbe(px);
		k=h-i-1;
		((u32 *)buf1)[k*w+j]=px1;
	}
	
	BTIC4B_Img_SaveHDR_RGBE(name, buf1, w, h);
	free(buf1);
	return(0);
}

int BTIC4B_Img_SaveTGA_R11F(
	char *name, byte *buf, int w, int h)
{
	byte *buf1;
	u32 px, px1;
	int i, j, k;
	
	buf1=malloc(w*h*4);
	for(i=0; i<h; i++)
		for(j=0; j<w; j++)
	{
		px=((u32 *)buf)[i*w+j];

		px1=btic4b_img_r11ftorgbe(px);
//		k=h-i-1;
		((u32 *)buf1)[i*w+j]=px1;
	}
	
	BTIC1H_Img_SaveTGA(name, buf1, w, h);
	free(buf1);
	return(0);
}

int BTIC4B_Img_SaveTGA_R11F_RGB(
	char *name, byte *buf, int w, int h)
{
	byte *buf1;
	u32 px, px1;
	int i, j, k;
	
	buf1=malloc(w*h*4);
	for(i=0; i<h; i++)
		for(j=0; j<w; j++)
	{
		px=((u32 *)buf)[i*w+j];

		px1=btic4b_img_r11ftorgbe(px);
//		k=h-i-1;
		((u32 *)buf1)[i*w+j]=px1;
		buf1[(i*w+j)*4+3]=255;
	}
	
	BTIC1H_Img_SaveTGA(name, buf1, w, h);
	free(buf1);
	return(0);
}

int BTIC4B_Img_SaveTGA_R11F_E(
	char *name, byte *buf, int w, int h)
{
	byte *buf1;
	u32 px, px1;
	int m, n, a;
	int i, j, k;
	
	buf1=malloc(w*h*4);
	
	m=256; n=-1;
	for(i=0; i<h; i++)
		for(j=0; j<w; j++)
	{
		px=((u32 *)buf)[i*w+j];
		px1=btic4b_img_r11ftorgbe(px);
		k=(px1>>24)&255;
		if(k<m)m=k;
		if(k>n)n=k;
	}

	a=(m+n)/2;

	for(i=0; i<h; i++)
		for(j=0; j<w; j++)
	{
		px=((u32 *)buf)[i*w+j];

		px1=btic4b_img_r11ftorgbe(px);
//		k=h-i-1;
//		((u32 *)buf1)[i*w+j]=px1;

		k=(px1>>24)&255;
		
		k=(k-a)*(127/(n-a+1))+128;
		
		buf1[(i*w+j)*4+0]=k;
		buf1[(i*w+j)*4+1]=k;
		buf1[(i*w+j)*4+2]=k;
		buf1[(i*w+j)*4+3]=255;
	}
	
	BTIC1H_Img_SaveTGA(name, buf1, w, h);
	free(buf1);
	return(0);
}

char *btic4b_img_sgets(char *t, char *s)
{
	while(*s && (*s!='\n'))
		{ *t++=*s++; }
	*t++=0;
	if(*s=='\n')s++;
	return(s);
}

char **btic4b_img_ssplit(char *s)
{
	static char *aa[64];
	static char ab[1024];
	char *t, *t0;
	char **at;
	
	at=aa; t=ab;
	while(*s)
	{
		while(*s && (*s<=' '))
			s++;
		if(!*s)
			break;
		t0=t;
		while(*s>' ')
			*t++=*s++;
		*t++=0;
		*at++=t0;
	}
	*at++=NULL;
	return(aa);
}

byte *BTIC4B_Img_ReadHDR_RGBE(FILE *fd, int *rw, int *rh)
{
	byte tb0[4096];
	byte tb[256];
	byte *buf, *buf1;
	byte *cs, *ct, *cte;
	char *s, *t;
	char **a;
	int xs, ys, n, v, ofs, xf, yf;
	int i1, j1;
	int i, j, k, l;
	
	fread(tb0, 1, 4096, fd);
	s=(char *)tb0;
	
	while(*s && *s!='\n')
	{
		s=btic4b_img_sgets((char *)tb, s);
	}
	if(*s=='\n')s++;

	s=btic4b_img_sgets((char *)tb, s);
	ofs=s-((char *)tb0);
	
	a=btic4b_img_ssplit((char *)tb);
//	ys=atoi(a[1]);
//	xs=atoi(a[3]);
//	xf=0; yf=1;
//	if(a[0][0]=='+')yf=0;
//	if(a[2][0]=='-')xf=1;
	
	xs=0; ys=0;
	
	if(!strcmp(a[0], "+X")) { xs=atoi(a[1]); xf=0; }
	if(!strcmp(a[0], "-X")) { xs=atoi(a[1]); xf=1; }
	if(!strcmp(a[0], "+Y")) { ys=atoi(a[1]); yf=0; }
	if(!strcmp(a[0], "-Y")) { ys=atoi(a[1]); yf=1; }

	if(!strcmp(a[2], "+X")) { xs=atoi(a[3]); xf=0; }
	if(!strcmp(a[2], "-X")) { xs=atoi(a[3]); xf=1; }
	if(!strcmp(a[2], "+Y")) { ys=atoi(a[3]); yf=0; }
	if(!strcmp(a[2], "-Y")) { ys=atoi(a[3]); yf=1; }
	
	*rw=xs;
	*rh=ys;
	
	buf=malloc(xs*ys*4);	
	fseek(fd, ofs, 0);
	fread(buf, 1, xs*ys*4, fd);

	buf1=malloc(xs*ys*4);
	
	n=xs*ys;
	cs=buf; ct=buf1; cte=buf1+n*4;
	while(ct<cte)
	{
		if((cs[0]==1) && (cs[1]==1) && (cs[2]==1))
		{
			i=cs[3]; cs+=4;
			while((i>0) && (ct<cte))
			{
				*(u32 *)ct=*(u32 *)(ct-4);
				ct+=4; i--;
			}
			continue;
		}

		if((cs[0]==2) && (cs[1]==2) && (cs[2]<128))
		{
			l=(cs[2]<<8)|cs[3]; cs+=4;
//			l=xs;
			
			for(i=0; i<4; i++)
			{
				j=0;
				while(j<l)
				{
					k=*cs++;
					if(k>128)
					{
						k=k&127;
						v=*cs++;
						while(k-- && ((ct+(j*4+i))<cte))
							{ ct[j*4+i]=v; j++;	}
					}else
					{
						while(k-- && ((ct+(j*4+i))<cte))
							{ ct[j*4+i]=*cs++; j++;	}
					}
				}
			}

			ct+=l*4;
			continue;
		}

		*(u32 *)ct=*(u32 *)(cs);
		cs+=4; ct+=4;
	}
	
	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
	{
		i1=yf?(ys-i-1):i;
		j1=xf?(xs-j-1):j;
		v=((u32 *)buf1)[i1*xs+j1];
		((u32 *)buf)[i*xs+j]=v;
	}
	
	free(buf1);

	return(buf);
}

byte *BTIC4B_Img_LoadHDR_RGBE(
	char *name, int *rw, int *rh)
{
	byte *buf;
	FILE *fd;
	
	fd=fopen(name, "rb");
	if(!fd)return(NULL);
	buf=BTIC4B_Img_ReadHDR_RGBE(fd, rw, rh);
	fclose(fd);
	return(buf);
}

byte *BTIC4B_Img_LoadHDR_R11F(
	char *name, int *rw, int *rh)
{
	byte *buf, *buf1;
	u32 px, px1;
	int w, h;
	int i, j, k;
	
	buf=BTIC4B_Img_LoadHDR_RGBE(name, &w, &h);
	*rw=w; *rh=h;
	
	buf1=malloc(w*h*4);
	for(i=0; i<h; i++)
		for(j=0; j<w; j++)
	{
		px=((u32 *)buf)[i*w+j];

		px1=btic4b_img_rgbetor11f(px);
//		k=h-i-1;
		((u32 *)buf1)[i*w+j]=px1;
	}
	
	free(buf);
	return(buf1);
}

byte *BTIC4B_Img_LoadTGA_R11F(
	char *name, int *rw, int *rh)
{
	byte *buf, *buf1;
	u32 px, px1;
	int w, h;
	int i, j, k;
	
	buf=BTIC1H_Img_LoadTGA(name, &w, &h);
	*rw=w; *rh=h;
	
	buf1=malloc(w*h*4);
	for(i=0; i<h; i++)
		for(j=0; j<w; j++)
	{
		px=((u32 *)buf)[i*w+j];

		px1=btic4b_img_rgbetor11f(px);
//		k=h-i-1;
		((u32 *)buf1)[i*w+j]=px1;
	}
	
	free(buf);
	return(buf1);
}



byte *BTIC4B_Img_DecodeHDR_RGBE(byte *ibuf, int isz, int *rw, int *rh)
{
//	byte tb0[4096];
	byte tb[256];
	byte *buf, *buf1;
	byte *cs, *ct, *cte;
	char *s, *t;
	char **a;
	int xs, ys, n, v, ofs, xf, yf;
	int i1, j1;
	int i, j, k, l;
	
//	fread(tb0, 1, 4096, fd);
//	memcpy(tb0, ibuf, 4096);
//	s=(char *)tb0;
	s=(char *)ibuf;
	
	while(*s && *s!='\n')
	{
		s=btic4b_img_sgets((char *)tb, s);
	}
	if(*s=='\n')s++;

	s=btic4b_img_sgets((char *)tb, s);
//	ofs=s-((char *)tb0);
	ofs=s-((char *)ibuf);
	
	a=btic4b_img_ssplit((char *)tb);
//	ys=atoi(a[1]);
//	xs=atoi(a[3]);
//	xf=0; yf=1;
//	if(a[0][0]=='+')yf=0;
//	if(a[2][0]=='-')xf=1;
	
	xs=0; ys=0;
	
	if(!strcmp(a[0], "+X")) { xs=atoi(a[1]); xf=0; }
	if(!strcmp(a[0], "-X")) { xs=atoi(a[1]); xf=1; }
	if(!strcmp(a[0], "+Y")) { ys=atoi(a[1]); yf=0; }
	if(!strcmp(a[0], "-Y")) { ys=atoi(a[1]); yf=1; }

	if(!strcmp(a[2], "+X")) { xs=atoi(a[3]); xf=0; }
	if(!strcmp(a[2], "-X")) { xs=atoi(a[3]); xf=1; }
	if(!strcmp(a[2], "+Y")) { ys=atoi(a[3]); yf=0; }
	if(!strcmp(a[2], "-Y")) { ys=atoi(a[3]); yf=1; }
	
	*rw=xs;
	*rh=ys;
	
	buf=malloc(xs*ys*4);	
//	fseek(fd, ofs, 0);
//	fread(buf, 1, xs*ys*4, fd);
	l=xs*ys*4;
	if((ofs+l)>isz)
		l=isz-ofs;
	memcpy(buf, ibuf+ofs, l);

	buf1=malloc(xs*ys*4);
	
	n=xs*ys;
	cs=buf; ct=buf1; cte=buf1+n*4;
	while(ct<cte)
	{
		if((cs[0]==1) && (cs[1]==1) && (cs[2]==1))
		{
			i=cs[3]; cs+=4;
			while((i>0) && (ct<cte))
			{
				*(u32 *)ct=*(u32 *)(ct-4);
				ct+=4; i--;
			}
			continue;
		}

		if((cs[0]==2) && (cs[1]==2) && (cs[2]<128))
		{
			l=(cs[2]<<8)|cs[3]; cs+=4;
//			l=xs;
			
			for(i=0; i<4; i++)
			{
				j=0;
				while(j<l)
				{
					k=*cs++;
					if(k>128)
					{
						k=k&127;
						v=*cs++;
						while(k-- && ((ct+(j*4+i))<cte))
							{ ct[j*4+i]=v; j++;	}
					}else
					{
						while(k-- && ((ct+(j*4+i))<cte))
							{ ct[j*4+i]=*cs++; j++;	}
					}
				}
			}

			ct+=l*4;
			continue;
		}

		*(u32 *)ct=*(u32 *)(cs);
		cs+=4; ct+=4;
	}
	
	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
	{
		i1=yf?(ys-i-1):i;
		j1=xf?(xs-j-1):j;
		v=((u32 *)buf1)[i1*xs+j1];
		((u32 *)buf)[i*xs+j]=v;
	}
	
	free(buf1);

	return(buf);
}

int BTIC4B_ImgRGBe_CheckValidUp(u32 px)
{
	int e, r, g, b, c;
	
	e=(px>>24)&255;		b=(px>>16)&255;
	g=(px>> 8)&255;		r=(px    )&255;
	
	if(e<0x71)
		return(0);
	if(e>0x8E)
		return(0);
	c=r|g|b;
	if(!(c&0x80))
		return(0);
	return(1);
}

int BTIC4B_ImgRGBe_CheckValidSp(u32 px)
{
	int e, r, g, b, c;
	
	e=(px>>24)&255;		b=(px>>16)&255;
	g=(px>> 8)&255;		r=(px    )&255;
	
	if(!(e&31) || (e&31)==31)
		return(0);
	c=r|g|b;
	if(!(c&0x80))
		return(0);
	return(1);
}

BTIC4B_API int BTIC4B_Img_CheckRGBeP(byte *buf, int w, int h)
{
	u32 px;
	int d, m;
	int i, j, k;
	
	d=(w>h)?h:w; m=3;
	for(i=0; i<d; i++)
	{
		j=h-i-1;
		px=((u32 *)buf)[i*w+i];
		if(!BTIC4B_ImgRGBe_CheckValidUp(px))m&=~1;
		if(!BTIC4B_ImgRGBe_CheckValidSp(px))m&=~2;
		px=((u32 *)buf)[j*w+i];
		if(!BTIC4B_ImgRGBe_CheckValidUp(px))m&=~1;
		if(!BTIC4B_ImgRGBe_CheckValidSp(px))m&=~2;
		
		if(!m)break;
	}
	return(m);
}
