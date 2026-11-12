#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <time.h>

#include "repak_core.h"

#include "gfxedit_minrp2.c"

// #include "rp2post_h12d.c"
// #include "rp2post_h12e.c"

#include "btm_rp2stf.c"
#include "btm_rp2bitrc.c"

#include "rp2post_h12d.c"
#include "rp2post_h12e.c"

#include "rp2post_auto.c"

#include "bt6a_decode.c"
#include "bt6a_encode.c"
#include "bt1h_targa.c"

#if 1
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
	BTIC6A_EncodeContext *ectx;
	BTIC6A_DecodeContext *dctx;
	BGBBTJ_BITMAPINFOHEADER *bmi;
	char *ifn, *ofn;
	byte *obuf;
	byte *ibuf, *i2buf;
	int xs, ys, pix, osz, offs, nblk;
	int t0, t1, t2, t0e, cnt, qf, mod;
	int i, j, k;

	ifn=NULL;
	ofn=NULL;
	qf=90;
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

	if((mod==1) && !ofn)
		mod=0;

	if((mod==0) || (mod==1))
	{
		ibuf=BTIC1H_Img_LoadTGA(ifn, &xs, &ys);

		if(!ibuf)
		{
			printf("Failed Load %s\n", ifn);
			return(-1);
		}

		obuf=malloc(1<<22);
		i2buf=malloc((xs+4)*(ys+4)*4);
		pix=xs*ys;
		
		ectx=BTIC6A_AllocEncodeContext();
		BTIC6A_EncodeInitContext(ectx, xs, ys, qf);
		osz=BTIC6A_EncodeFrameImage(ectx,
			obuf+0x40, 1<<22, ibuf, qf, BTIC4B_CLRS_RGBA);
		
		if(osz<=0)
			return(-1);
		
		printf("osz=%d %.3fbpp\n", osz, (osz*8.0)/pix);

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
		bmi->biCompression=BGBBTJ_FCC_bt6a;
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

		if(bmi->biCompression!=BGBBTJ_FCC_bt6a)
		{
			printf("BMP biCompression!=bt6a, %s\n", ifn);
			return(-1);
		}

		osz=bmi->biSizeImage;

		xs=bmi->biWidth;
		ys=bmi->biHeight;

		i2buf=malloc((xs+4)*(ys+4)*4);
	}
	
	if((mod==0) || (mod==2))
	{
		dctx=BTIC6A_AllocDecodeContext();
//		BTIC6A_DecodeFrame(dctx, obuf+0x40, osz, i2buf, xs);
		BTIC6A_DecodeFrame(dctx, obuf+offs, osz, i2buf, xs*4,
			BTIC4B_CLRS_RGBA);

		if(mod==0)
			checkrmse(ibuf, i2buf, xs, ys);
		
	//	for(i=0; i<256; i++)
	//		i2buf[i]=dctx->pal[i];
	
		if(ofn)
			BTIC1H_Img_SaveTGA(ofn, i2buf, xs, ys);
	}

	if(mod==0)
	{
		BTIC6A_DecodeFrame(dctx, obuf+offs, osz, i2buf, xs*4,
			BTIC4B_CLRS_RGBA);
		BTIC1H_Img_SaveTGA("tst6a_rgba.tga", (byte *)i2buf, xs, ys);

		BTIC6A_DecodeFrame(dctx, obuf+offs, osz, i2buf, xs*4,
			BTIC4B_CLRS_BGRA);
		BTIC1H_Img_SaveTGA("tst6a_bgra.tga", (byte *)i2buf, xs, ys);

		BTIC6A_DecodeFrame(dctx, obuf+offs, osz, i2buf, xs*4,
			BTIC4B_CLRS_RGB);
		BTIC1H_Img_SaveTGA24("tst6a_rgba.tg", (byte *)i2buf, xs, ys);

		BTIC6A_DecodeFrame(dctx, obuf+offs, osz, i2buf, xs*4,
			BTIC4B_CLRS_BGR);
		BTIC1H_Img_SaveTGA24("tst6a_bgr.tga", (byte *)i2buf, xs, ys);
	}
	
	if(mod==0)
//	if(0)
	{
		t0=FRGL_TimeMS();
		t1=t0; cnt=0;
		t0e=t0+(10*1000);
		while(t1<t0e)
		{
			cnt++;
			BTIC6A_DecodeFrame(dctx, obuf+offs, osz, i2buf, xs*4, 
				BTIC4B_CLRS_RGB555);
			t1=FRGL_TimeMS();
		}
		
		printf("%d, %f MPix/s", cnt, (((s64)cnt)*pix)/(1000.0*(t1-t0)));
	}
}
