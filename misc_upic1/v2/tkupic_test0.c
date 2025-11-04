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

#include "bt1h_targa.c"

#include "tkupic_dec.c"
#include "tkupic_enc.c"

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

void check_haarrev()
{
	int c, d, a, b;
	int i, j, k, l;
	
	for(i=0; i<256; i++)
		for(j=0; j<256; j++)
	{
		c=(i+j)/2;
		d=i-j;
		
		b=c-(d>>1);
		a=b+d;
		if((a!=i) || (b!=j))
		{
			printf("check_haarrev: fail, %d %d -> %d %d\n", i, j, a, b);
			return;
		}
	}
}

void check_clrsrev()
{
	int cy, cu, cv, cr, cg, cb;
	int i, j, k, l;
	
	for(i=0; i<256; i++)
		for(j=0; j<256; j++)
			for(k=0; k<256; k++)
	{
#if 1
		cy=j+((i-k)>>2);
		cu=k-cy;
		cv=i-cy;
		
		cr=cv+cy;
		cb=cu+cy;
		cg=cy-((cr-cb)>>2);
#endif

#if 0
//		cy=(2*j+i+k)>>2;
//		cy=(4*j+3*i+k)/8;
		cy=(j>>1)+((3*i+k)>>3);

//		cy=j+(3*i+k-4*j)/8;
//		cy=j+((3*i+k-4*j)>>3);
//		cy=2*j-((3*i+k)>>3);
//		cy=(2*j+i+k)/4;
		cu=k-cy;
		cv=i-cy;
		
		cr=cv+cy;
		cb=cu+cy;

		cg=(8*cy-3*cr-cb)/4;

//		cg=cy*2-((3*cr+cb)>>3);

//		cg=(cy+((3*cr+cb)>>3))>>1;
//		cg=(cy+((3*cr+cb)/8))>>1;
//		cg=(8*cy+3*cr+cb)/16;
//		cg=(16*cy-6*cr-2*cb)/8;
//		cg=cy-((cu+cv)>>2);
//		cg=(4*cy-cr-cb)/2;
		
//		if(cg<0)
//			cg=0;

//		cg=cy-((3*cr+cb)>>2);
//		cg=cy-((3*cr+cb)>>3);
//		cg=2*cy-((3*cr+cb)>>1);

//		cg=(4*cy-cr-cb)/2;
//		cg=2*cy-(cr+cb)/2;
//		cg=cy-((cr+cb)>>2);
#endif

#if 0
		cy=(2*j+i+k)>>2;
		cu=k-j;
		cv=i-j;
		
//		cg=(4*cy-cu-cv)/2;
//		cg=2*cy-(cu>>1)-(cv>>1);
		cg=cy-((cu+cv)>>2);
		cr=cv+cg;
		cb=cu+cg;
#endif

		if((cr!=i) || (cg!=j) || (cb!=k))
		{
			printf("check_clrsrev: fail, %d %d %d -> %d %d %d\n",
				i, j, k, cr, cg, cb);
			return;
		}
	}
}

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

int print_usage(char *arg0)
{
	printf("Usage: %s infile outfile [opts*]\n", arg0);
	printf("  -q lvl      Quality in percentage, 100=lossless\n");
	printf("  -e          Encode Image\n");
	printf("  -d          Decode Image\n");
}

int main(int argc, char *argv[])
{
	TKuPI_DecState t_dctx;
	TKuPI_EncState t_ectx;
	TKuPI_DecState *dctx;
	TKuPI_EncState *ectx;
	char *ifn, *ofn;
	byte *ibuf, *cbuf, *obuf;
	long long tpix;
	int t0, t1, t2, t0e;
	int xs, ys, sz, qfl, md;
	int i, j, k;

//	check_haarrev();
//	check_clrsrev();

//	ifn="StreetHolland.tga";

	md=0;
	qfl=75;
//	qfl=100;

	ifn=NULL;
	ofn=NULL;
	for(i=1; i<argc; i++)
	{
		if(argv[i][0]=='-')
		{
			if(!strcmp(argv[i], "-e"))
			{
				md=1;
				continue;
			}

			if(!strcmp(argv[i], "-d"))
			{
				md=2;
				continue;
			}

			if(!strcmp(argv[i], "-q"))
			{
				qfl=(qfl&(~127))|(atoi(argv[i+1])&127);
				i++;
				continue;
			}
			continue;
		}
		if(!ifn)
			{ ifn=argv[i]; continue; }
		if(!ofn)
			{ ofn=argv[i]; continue; }
	}

	if(md==1)
	{
		if(!ifn || !ofn)
		{
			print_usage(argv[0]);
			return(0);
		}
	
		ibuf=BTIC1H_Img_LoadTGA_BGRA(ifn, &xs, &ys);
		if(!ibuf)
		{
			printf("Fail read TGA %s\n", ifn);
			return(0);
		}
		
		ectx=&t_ectx;
		
//		cbuf=malloc(1<<24);
		cbuf=malloc(1024+xs*ys*6);

		sz=TKuPI_EncodeImageBuffer(ectx, cbuf, 1<<20,
			ibuf, xs, ys, qfl);
			
		TgvLz_StoreFile(ofn, cbuf, sz);
		return(0);
	}

	if(md==2)
	{
		if(!ifn || !ofn)
		{
			print_usage(argv[0]);
			return(0);
		}
	
		cbuf=TgvLz_LoadFile(ifn, &sz);
		if(!cbuf)
		{
			printf("Fail load %s\n", ifn);
		}

		dctx=&t_dctx;

		TKuPI_DecodeImageBuffer(dctx, cbuf, sz, NULL, 0, 0);
		xs=dctx->img_xs;
		ys=dctx->img_ys;
		
		if((xs<=0) || (ys<=0))
		{
			printf("Bad Image %s\n", ifn);
		}

		obuf=malloc(xs*ys*4);
		
		TKuPI_DecodeImageBuffer(dctx, cbuf, sz, obuf, xs, ys);

		BTIC1H_Img_SaveTGA_BGRA(ofn, obuf, xs, ys);
	}

	if(md==0)
	{
		if(!ifn)
		{
			print_usage(argv[0]);
			return(0);
		}
	
		ibuf=BTIC1H_Img_LoadTGA_BGRA(ifn, &xs, &ys);
		if(!ibuf)
		{
			printf("Fail read TGA %s\n", ifn);
			return(0);
		}
		
		dctx=&t_dctx;
		ectx=&t_ectx;
		
//		cbuf=malloc(1<<24);
		cbuf=malloc(1024+xs*ys*6);

		sz=TKuPI_EncodeImageBuffer(ectx, cbuf, 1<<20,
			ibuf, xs, ys, qfl);
			
		if(ofn)
		{
			TgvLz_StoreFile(ofn, cbuf, sz);
		}
			
		printf("Enc Sz=%d\n", sz);
		
		obuf=malloc(xs*ys*4);
		
		TKuPI_DecodeImageBuffer(dctx, cbuf, sz, obuf, xs, ys);

		checkrmse(ibuf, obuf, xs, ys);

		BTIC1H_Img_SaveTGA_BGRA("test0a.tga", obuf, xs, ys);
		
		tpix=0;
		t0=clock();
		t1=t0;
		t0e=t0+10*CLOCKS_PER_SEC;
		while(t1<t0e)
		{
			TKuPI_DecodeImageBuffer(dctx, cbuf, sz, obuf, xs, ys);
			tpix+=xs*ys;
			t1=clock();
		}
		
		printf("%f Mpix/sec\n", tpix/((1000000.0*(t1-t0))/CLOCKS_PER_SEC));
	}
}
