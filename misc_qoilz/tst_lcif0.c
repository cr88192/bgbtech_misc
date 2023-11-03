#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <time.h>


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


// #include "bt2f_decode.c"
// #include "bt2f_encode.c"
#include "bt1h_targa.c"

#include "qoilz_dec.c"
#include "qoilz_enc.c"

#include "lcif_dec.c"
#include "lcif_enc.c"

byte *TgvLz_LoadFile(char *name, int *rsz)
{
	byte *buf;
	FILE *fd;
	int sz, i;
	
	fd=fopen(name, "rb");
	if(!fd)
	{
		return(NULL);
	}
	fseek(fd, 0, 2);
	sz=ftell(fd);
	fseek(fd, 0, 0);
	buf=malloc(sz+24);
	i=fread(buf, 1, sz, fd);
	fclose(fd);
	
	if(i!=sz)
	{
		if(i>0)
		{
			sz=i;
		}else
		{
			free(buf);
			return(NULL);
		}
	}

	memset(buf+sz, 0, 16);
	
	*rsz=sz;
	return(buf);
}

int TgvLz_StoreFile(char *name, void *buf, int sz)
{
	FILE *fd;
	
	fd=fopen(name, "wb");
	if(!fd)
		return(-1);
	
	fwrite(buf, 1, sz, fd);
	fclose(fd);
	return(0);
}

int clamp255(int x)
{
	if(x<0)return(0);
	if(x>255)return(255);
	return(x);
}

int clamp15(int x)
{
	if(x<0)return(0);
	if(x>15)return(15);
	return(x);
}


double checkrmse(byte *ibuf1, byte *ibuf2, int xs, int ys)
{
	double e, er, eg, eb;
	int cr0, cg0, cb0;
	int cr1, cg1, cb1;
	int dr, dg, db;
	int n;
	int i, j, k;
	
	er=0; eg=0; eb=0;
	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
	{
		k=(i*xs+j)*4;
		cr0=ibuf1[k+0];	cg0=ibuf1[k+1];	cb0=ibuf1[k+2];
		cr1=ibuf2[k+0];	cg1=ibuf2[k+1];	cb1=ibuf2[k+2];
		dr=cr1-cr0;		dg=cg1-cg0;		db=cb1-cb0;
		er+=dr*dr;
		eg+=dg*dg;
		eb+=db*db;
	}
	
	e=(er+eg+eb)/3;
	n=xs*ys;
	printf("RMSE: Er=%.3f Eg=%.3f Eb=%.3f Eavg=%.3f\n",
		sqrt(er/n), sqrt(eg/n), sqrt(eb/n), sqrt(e/n));

	e=sqrt(e/n);
	return(e);
}

/* Switch R and B components. */
int QOI_ImageFlipRB(byte *obuf, byte *ibuf, int xs, int ys)
{
	int i0, i1, i2, i3;
	int i, n;

	n=xs*ys;
	for(i=0; i<n; i++)
	{
		i0=ibuf[i*4+0];		i1=ibuf[i*4+1];
		i2=ibuf[i*4+2];		i3=ibuf[i*4+3];
		obuf[i*4+0]=i2;		obuf[i*4+1]=i1;
		obuf[i*4+2]=i0;		obuf[i*4+3]=i3;
	}
	return(0);
}

/* Flip image vertically */
int QOI_ImageFlipVert(byte *obuf, byte *ibuf, int xs, int ys)
{
	byte *cs0, *cs1;
	int i0, i1, i2, i3;
	int x, y, ys1;
	int i, j, n;

	if(ibuf==obuf)
	{
		ys1=ys/2;
		for(y=0; y<ys1; y++)
		{
			cs0=ibuf+(y*xs*4);
			cs1=ibuf+((ys-y-1)*xs*4);
			for(x=0; x<(xs<<2); x++)
			{
				i0=cs0[x];
				i1=cs1[x];
				cs0[x]=i1;
				cs1[x]=i0;
			}
		}
		return(0);
	}

	for(y=0; y<ys; y++)
	{
		cs0=ibuf+(y*xs*4);
		cs1=obuf+((ys-y-1)*xs*4);
		for(x=0; x<(xs<<2); x++)
		{
			i0=cs0[x];
			cs1[x]=i0;
		}
	}
	return(0);
}

int main(int argc, char *argv[])
{
	char tb[64];
	byte *ibuf, *obuf, *iimg;
	byte *cbuf;
	char *ifn, *ofn;
	int xs, ys, sz, isz, xs1, ys1;
	int t0, t1, t2;
	int efmt, flg, qlvl;
	double f, g;
	int i, j, k, n;
	
	ifn=NULL;
	ofn=NULL;
	efmt=0;
	qlvl=0;
	for(i=1; i<argc; i++)
	{
		if(argv[i][0]=='-')
		{
			if(argv[i][1]=='e')
			{
				if(!strcmp(argv[i]+2, "qoi"))
					efmt=1;
				if(!strcmp(argv[i]+2, "qolz"))
					efmt=2;
				if(!strcmp(argv[i]+2, "qoli"))
					efmt=3;
				if(!strcmp(argv[i]+2, "lcif"))
					efmt=4;
				if(!strcmp(argv[i]+2, "tga"))
					efmt=5;
				continue;
			}
			if(argv[i][1]=='q')
			{
				if((argv[i][2]>='0') && (argv[i][2]<='7'))
					qlvl=argv[i][2]-'0';
			}
			continue;
		}
		
		if(!ifn)
		{
			ifn=argv[i];
			continue;
		}
		
		if(!ofn)
		{
			ofn=argv[i];
			continue;
		}
	}
	
	if(!ifn)
	{
		printf("%s [infile] [outfile] [options]\n", argv[0]);
		printf("  -eqoi   Use QOI output\n");
		printf("  -eqolz  Use QOLZ output\n");
		printf("  -eqoli  Use QOLI output\n");
		printf("  -elcif  Use LCIF output\n");
		printf("  -etga   Use TGA output\n");
		printf("  -q0     Best | Lossless Quality\n");
		printf("  -q1     Best Lossy Quality\n");
		printf("  -q7     Worst Lossy Quality\n");
		printf("\n");
		printf("This will try to detect the format of the input file\n");
		return(0);
	}
	
	if(ifn)
	{
		iimg=TgvLz_LoadFile(ifn, &isz);
		ibuf=NULL;

		if(!iimg)
		{
			printf("Couldn't read %s\n", ifn);
		}else
			if((iimg[0]=='q') && (iimg[1]=='o'))
		{
			ibuf=QOILZ_DecImageBuffer(iimg, &xs, &ys);
			if(ibuf)
				QOI_ImageFlipVert(ibuf, ibuf, xs, ys);
		}else if(
			(iimg[0]=='l') && (iimg[1]=='c') &&
			(iimg[2]=='i') && (iimg[3]=='f'))
		{
			xs=0; ys=0;
			LCIF_DecImageBuffer(NULL, iimg, &xs, &ys);
			if(xs>0)
			{
				ibuf=malloc(xs*ys*4);
				LCIF_DecImageBuffer(ibuf, iimg, &xs, &ys);
				QOI_ImageFlipRB(ibuf, ibuf, xs, ys);
//				QOI_ImageFlipVert(ibuf, ibuf, xs, ys);
			}
		}else if(BTIC1H_Img_DecodeProbeIsTGA(iimg, NULL, NULL))
		{
			ibuf=BTIC1H_Img_DecodeTGA(iimg, &xs, &ys);
		}else
		{
			printf("Image Format, Detect Fail %s\n", ifn);
		}
		
		if(iimg)
			free(iimg);
	}
	
	if(ibuf && ofn && efmt)
	{
		cbuf=NULL;

		if((efmt==1) || (efmt==3))
		{
			QOI_ImageFlipVert(ibuf, ibuf, xs, ys);

			cbuf=malloc(xs*ys*4);
			flg=(qlvl<<1)|(efmt==3);
			sz=QOI_EncodeImageBuffer(cbuf, ibuf, xs, ys, flg);
			TgvLz_StoreFile(ofn, cbuf, sz);
		}else
			if(efmt==2)
		{
			QOI_ImageFlipVert(ibuf, ibuf, xs, ys);

			cbuf=malloc(xs*ys*4);
			flg=(qlvl<<1);
			sz=QOILZ_EncodeImageBuffer(cbuf, ibuf, xs, ys, flg);
			TgvLz_StoreFile(ofn, cbuf, sz);
		}else
			if(efmt==4)
		{
			cbuf=malloc(xs*ys*4);
			flg=(qlvl<<1);
			QOI_ImageFlipRB(ibuf, ibuf, xs, ys);
			sz=LCIF_EncodeImageBuffer(cbuf, ibuf, xs, ys, flg);
			TgvLz_StoreFile(ofn, cbuf, sz);
		}else
			if(efmt==5)
		{
			BTIC1H_Img_SaveTGA(ofn, ibuf, xs, ys);
		}

		if(cbuf)
			free(cbuf);
		free(ibuf);
		return(0);
	}
	
	if(ibuf && !ofn)
	{
	//	infn=argv[1];
	//	ibuf=BTIC1H_Img_LoadTGA(ifn, &xs, &ys);

		cbuf=malloc(xs*ys*4);
		obuf=malloc(xs*ys*4);

#if 0
		QOI_ImageFlipRB(ibuf, ibuf, xs, ys);
		
		sz=LCIF_EncodeImageBuffer(cbuf, ibuf, xs, ys, 0);
		LCIF_DecImageBuffer(obuf, cbuf, &xs1, &ys1);
		
		QOI_ImageFlipRB(obuf, obuf, xs, ys);
		BTIC1H_Img_SaveTGA("lciftst0.tga", obuf, xs, ys);
		
		for(i=0; i<8; i++)
		{
			printf("Loss=%d\n", i);

			sz=LCIF_EncodeImageBuffer(cbuf, ibuf, xs, ys, i<<1);
			LCIF_DecImageBuffer(obuf, cbuf, &xs1, &ys1);

			QOI_ImageFlipRB(obuf, obuf, xs, ys);
			sprintf(tb, "lciftst0_q%u.tga", i);
			BTIC1H_Img_SaveTGA(tb, obuf, xs, ys);
		}
#endif
		
#if 1
		flg=qlvl<<1;
		if(efmt==3)
			flg|=1;
		if((efmt==1) || (efmt==3))
			{ sz=QOI_EncodeImageBuffer(cbuf, ibuf, xs, ys, flg); }
		if(efmt==2)
			{ sz=QOILZ_EncodeImageBuffer(cbuf, ibuf, xs, ys, flg); }
		if(efmt==4)
			{ sz=LCIF_EncodeImageBuffer(cbuf, ibuf, xs, ys, qlvl<<1); }
		printf("Test Size=%d\n", sz);
		
		t0=clock();
		
		n=1024;
		for(i=0; i<n; i++)
		{
			if(cbuf[0]=='q' && cbuf[1]=='o')
				QOI_DecImageBufferFlat(obuf, cbuf, &xs1, &ys1, 0);
			if(cbuf[0]=='l' && cbuf[1]=='c')
				LCIF_DecImageBuffer(obuf, cbuf, &xs1, &ys1);
		}
		t1=clock();
		
		t2=t1-t0;
		f=(1.0*t2)/CLOCKS_PER_SEC;
		g=(xs*ys*((u64)n))/f;
		
		printf("%fs  %f Mpix/s\n", f, g/1000000.0);
#endif

		return(0);
	}
}
