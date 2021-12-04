#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <time.h>

#include "repak_core.h"

#define TGVLZ_NOMAIN
#include "tgvlz1.c"

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
	char *ifn, *ofn;
	byte *obuf;
	u16 *ibuf, *i2buf;
	int xs, ys, pix, osz, nblk;
	int t0, t1, t2, t0e, cnt, qf;
	int i, j, k;

	ifn=NULL;
	ofn=NULL;
	qf=90;

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

	ibuf=BTIC1H_Img_LoadTGA555(ifn, &xs, &ys);

	obuf=malloc(1<<22);
	i2buf=malloc((xs+4)*(ys+4)*2);
	pix=xs*ys;
	
	ectx=BTIC5B_AllocEncodeContext();
	BTIC5B_EncodeInitContext(ectx, xs, ys, qf);
//	osz=BTIC5B_EncodeImageBasic(ectx, obuf, 1<<22, ibuf, xs);
	osz=BTIC5B_EncodeFrameImage(ectx, obuf, 1<<22, ibuf, qf, BTIC4B_CLRS_RGB555);
	
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
	
	dctx=BTIC5B_AllocDecodeContext();
	BTIC5B_DecodeFrame(dctx, obuf, osz, i2buf, xs);

	checkrmse16(ibuf, i2buf, xs, ys);
	
//	for(i=0; i<256; i++)
//		i2buf[i]=dctx->pal[i];
	
	if(ofn)
		BTIC1H_Img_SaveTGA555(ofn, i2buf, xs, ys);
		
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
