#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <time.h>

#include "repak_core.h"

#define TGVLZ_NOMAIN
#include "tgvlz1.c"

#include "tkulzd.c"
#include "tkulze.c"

#include "btrlz0_dec.c"
#include "btrlz0_enc.c"

#include "bt5b_decode.c"
#include "bt5b_encode.c"
#include "bt1h_targa.c"

#if 0
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
#endif

int FRGL_TimeMS()
{
#if 0
#ifdef _WIN32
	static unsigned int init;
	unsigned int t;

	t=timeGetTime();
	if(!init)init=t;

	return((unsigned int)(t-init));
#endif

#ifdef __EMSCRIPTEN__
	struct timeval	tp;
	static int      secbase; 

	gettimeofday(&tp, NULL);  
	if(!secbase)secbase=tp.tv_sec;
	return(((tp.tv_sec-secbase)*1000)+tp.tv_usec/1000);
#endif
#endif

#ifndef linux
	static int init;
	int t;

	t=clock();
	t*=CLOCKS_PER_SEC/1000.0;
//	t=FRGL_TimeMS();

	if(!init)init=t;

	return((unsigned int)(t-init));
#endif

#ifdef linux
	struct timeval	tp;
	static int      secbase; 

	gettimeofday(&tp, NULL);  
	if(!secbase)secbase=tp.tv_sec;
	return(((tp.tv_sec-secbase)*1000)+tp.tv_usec/1000);
#endif
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

double checkrmse16(u16 *ibuf1, u16 *ibuf2, int xs, int ys)
{
	double e, er, eg, eb;
	int cr0, cg0, cb0;
	int cr1, cg1, cb1;
	int dr, dg, db;
	int n, i0, i1;
	int i, j, k;
	
	er=0; eg=0; eb=0;
	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
	{
//		k=(i*xs+j)*4;
//		cr0=ibuf1[k+0];	cg0=ibuf1[k+1];	cb0=ibuf1[k+2];
//		cr1=ibuf2[k+0];	cg1=ibuf2[k+1];	cb1=ibuf2[k+2];

		k=(i*xs+j);
		i0=ibuf1[k];
		i1=ibuf2[k];
		cr0=(i0>>10)&31;	cg0=(i0>> 5)&31;	cb0=(i0>> 0)&31;
		cr1=(i1>>10)&31;	cg1=(i1>> 5)&31;	cb1=(i1>> 0)&31;

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




int main(int argc, char *argv[])
{
	BTIC5B_EncodeContext *ectx;
	BTIC5B_DecodeContext *dctx;
	BGBBTJ_BITMAPINFOHEADER *bmi;
	char *ifn, *ofn;
	byte *obuf;
	u16 *ibuf, *i2buf;
	int xs, ys, pix, osz, offs, nblk;
	int t0, t1, t2, t0e, cnt, qf, mod;
	int i, j, k;

	ifn=NULL;
	ofn=NULL;
	qf=90;
//	mod=0;
	mod=1;

	for(i=1; i<argc; i++)
	{
		if(argv[i][0]=='-')
		{
			if(!strcmp(argv[i], "-q"))
			{
				qf=atoi(argv[i+1]);
				i++;
				continue;
			}

			if(!strncmp(argv[i], "-q=", 3))
			{
				qf=atoi(argv[i]+3);
				i++;
				continue;
			}

			if(!strcmp(argv[i], "-e"))
			{
				mod=1;
			}

			if(!strcmp(argv[i], "-d"))
			{
				mod=2;
			}

			if(!strcmp(argv[i], "-t"))
			{
				mod=0;
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
		printf("No input file\n");
		printf("Usage: %s infile outfile args*\n", argv[0]);
		printf("  -e      Encode image from TGA\n");
		printf("  -d      Decode image to TGA\n");
		printf("  -t      Test Encode then Decode\n");
		printf("  -q val  Quality Level (0..100)\n");
		printf("  -q=val  Quality Level (0..100) (Alternate)\n");
		return(-1);
	}

	if((mod==0) || (mod==1))
	{
		ibuf=BTIC1H_Img_LoadTGA555(ifn, &xs, &ys);

		if(!ibuf)
		{
			printf("Failed Load %s\n", ifn);
			return(-1);
		}

		obuf=malloc(1<<22);
		i2buf=malloc((xs+4)*(ys+4)*2);
		pix=xs*ys;
		
		ectx=BTIC5B_AllocEncodeContext();
		BTIC5B_EncodeInitContext(ectx, xs, ys, qf);
	//	osz=BTIC5B_EncodeImageBasic(ectx, obuf, 1<<22, ibuf, xs);
		osz=BTIC5B_EncodeFrameImage(ectx,
			obuf+0x40, 1<<22, ibuf, qf, BTIC4B_CLRS_RGB555);
		
		printf("osz=%d %.3fbpp\n", osz, (osz*8.0)/pix);

		nblk=ectx->bxs*ectx->bys;
		printf("cd15=%d(%.2f) cd5=%d(%.2f) cdp=%d(%.2f)\n",
			ectx->stat_cd15,	(100.0*ectx->stat_cd15)/nblk,
			ectx->stat_cd5,		(100.0*ectx->stat_cd5)/nblk,
			ectx->stat_cdp,		(100.0*ectx->stat_cdp)/nblk);
		printf("flat=%d(%.2f) px2=%d(%.2f) px4=%d(%.2f) pxp6=%d(%.2f) tot=%d\n",
			ectx->stat_flat,	(100.0*ectx->stat_flat)/nblk,
			ectx->stat_px2,		(100.0*ectx->stat_px2)/nblk,
			ectx->stat_px4,		(100.0*ectx->stat_px4)/nblk,
			ectx->stat_pxp6,	(100.0*ectx->stat_pxp6)/nblk,
			nblk);
		
		obuf[0]='B';
		obuf[1]='M';
		btpic_setu32(obuf+2, osz+0x40);
		btpic_setu32(obuf+10, 0x40);
		offs=0x40;
		
		bmi=(BGBBTJ_BITMAPINFOHEADER *)(obuf+14);

		bmi->biSize=sizeof(BGBBTJ_BITMAPINFOHEADER);
		bmi->biWidth=xs;
		bmi->biHeight=ys;
		bmi->biPlanes=1;
		bmi->biBitCount=16;
		bmi->biCompression=BGBBTJ_FCC_bt5b;
		bmi->biSizeImage=osz;
		bmi->biXPelsPerMeter=2835;
		bmi->biYPelsPerMeter=2835;
				
		if(mod==1)
		{
			TgvLz_StoreFile(ofn, obuf, osz+0x40);
		}
	}

	if(mod==2)
	{
		obuf=TgvLz_LoadFile(ifn, &osz);
		
		if(!obuf)
		{
			printf("Failed Load %s\n", ifn);
			return(-1);
		}
		
		if((obuf[0]!='B') || (obuf[1]!='M'))
		{
			printf("BMP Sig Failed, %s\n", ifn);
			return(-1);
		}
		
		offs=btpic_getu32(obuf+10);
		
//		osz=osz-0x40;
		osz=osz-offs;

		bmi=(BGBBTJ_BITMAPINFOHEADER *)(obuf+14);

		if(bmi->biCompression!=BGBBTJ_FCC_bt5b)
		{
			printf("BMP biCompression!=bt5b, %s\n", ifn);
			return(-1);
		}

		osz=bmi->biSizeImage;

		xs=bmi->biWidth;
		ys=bmi->biHeight;

		i2buf=malloc((xs+4)*(ys+4)*2);
	}
	
	if((mod==0) || (mod==2))
	{
		dctx=BTIC5B_AllocDecodeContext();
//		BTIC5B_DecodeFrame(dctx, obuf+0x40, osz, i2buf, xs);
		BTIC5B_DecodeFrame(dctx, obuf+offs, osz, i2buf, xs);

		if(mod==0)
			checkrmse16(ibuf, i2buf, xs, ys);
		
	//	for(i=0; i<256; i++)
	//		i2buf[i]=dctx->pal[i];
	
		if(ofn)
			BTIC1H_Img_SaveTGA555(ofn, i2buf, xs, ys);
	}
	
	if(mod==0)
	{
		t0=FRGL_TimeMS();
		t1=t0; cnt=0;
		t0e=t0+(10*1000);
		while(t1<t0e)
		{
			cnt++;
			BTIC5B_DecodeFrame(dctx, obuf, osz, i2buf, xs);
			t1=FRGL_TimeMS();
		}
		
		printf("%d, %f MPix/s", cnt, (((s64)cnt)*pix)/(1000.0*(t1-t0)));
	}
}
