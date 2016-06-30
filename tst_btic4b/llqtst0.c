#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#ifndef BYTE_T
#define BYTE_T
typedef unsigned char byte;
#endif

#ifndef SBYTE_T
#define SBYTE_T
typedef signed char sbyte;
#endif

#ifndef PDLIB_INT_BITS_T
#define PDLIB_INT_BITS_T
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned long long u64;
typedef signed long long s64;
#endif

#ifndef UINT_T
#define UINT_T
typedef unsigned int uint;
#endif

#if !defined(LITTLEENDIAN) && !defined(BIGENDIAN)
#if defined(X86) || defined(X86_64) || defined(ARM)
#define LITTLEENDIAN
#endif
#endif


#ifdef _MSC_VER
#define force_inline __forceinline
#define default_inline __inline
#endif

#ifdef __GNUC__
#define force_inline inline
#define default_inline inline
#endif

#ifndef force_inline
#define force_inline
#define default_inline
#endif

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#ifndef M_TAU
#define M_TAU 6.283185307179586476925286766559
#endif

#define LQTVQ_TWOCC(a, b)			((a)|((b)<<8))
#define LQTVQ_FOURCC(a, b, c, d)	((a)|((b)<<8)|((c)<<16)|((d)<<24))

#define LQTVQ_TCC_I0		LQTVQ_TWOCC('I', '0')
#define LQTVQ_TCC_I1		LQTVQ_TWOCC('I', '1')
#define LQTVQ_TCC_I2		LQTVQ_TWOCC('I', '2')
#define LQTVQ_TCC_I3		LQTVQ_TWOCC('I', '3')

#define LQTVQ_TCC_AX		LQTVQ_TWOCC('A', 'X')
#define LQTVQ_TCC_HX		LQTVQ_TWOCC('H', 'X')

#define LQTVQ_FCC_BT4B		LQTVQ_FOURCC('B', 'T', '4', 'B')

#define BT4A_QFL_PFRAME		0x0100

typedef struct {
byte tab[256];
byte idx[256];
byte rov;
byte rk;
int cnt;
int bits;
}BT4A_SmtfState;

typedef struct {
byte *ct, *cs;
u32 bit_win;
int bit_pos;

int xs, ys;
int xsb, ysb, nblk, blksz;
int qfl;

byte *blks;
byte *lblks;

int cy, cu, cv, dy, du, dv;
int qfy, qfuv, qfdy, qfduv;
int qy, quv, qdy, qduv;

int qdy_flat;
int qdy_2x2x2;
int qdy_4x4x2;
int qdy_8x8x2;
int qdy_8x8x3;
int qduv_flat;
int qduv_2x2;
int qdce_sc;

byte rk_cy, rk_cuv;
byte rk_dy, rk_duv;
byte rk_cnt, rk_misc;
byte cmask;				//current mask
byte imask;				//ideal mask
byte pred;				//predictor

BT4A_SmtfState sm_cmd;
BT4A_SmtfState sm_mask;

int stat_bits;
// int stat_tbits;
int stat_yuvbits;
int stat_yuvcnt;
int stat_yuvcbits[8];
int stat_pixbits;

int yuv_cz[8];

}BT4A_Context;

#include "lqt_thread.c"

#include "lqt_blkenc.c"
#include "lqt_bitwrite.c"
#include "lqt_encode.c"
#include "lqt_thslice.c"

#include "lqt_blkdec.c"
#include "lqt_bitread.c"
#include "lqt_decode.c"

#include "bt1h_targa.c"

int tst_llq0(byte *ibuf, byte *obuf, int xs, int ys)
{
	byte *ybuf, *ubuf, *vbuf;
	byte *cs0, *cs1, *cs0e, *cty0, *cty1, *ctu, *ctv;
	byte cr0, cr1, cr2, cr3;
	byte cg0, cg1, cg2, cg3;
	byte cb0, cb1, cb2, cb3;
	byte cy0, cy1, cy2, cy3;
	byte cr, cg, cb;
	byte cy, cu, cv;
	byte lcy, lcu, lcv;
	int xs1, ys1, ystr;
	int i, j, k;
	
	ystr=xs*4;
	
	xs1=xs>>1; ys1=ys>>1;
	ybuf=obuf;
	ubuf=ybuf+xs*ys;
	vbuf=ubuf+xs1*ys1;
	
	for(i=0; i<ys1; i++)
	{
		cs0=ibuf+(i*2+0)*ystr;
		cs1=ibuf+(i*2+1)*ystr;
		cs0e=cs0+ystr;
		cty0=ybuf+(i*2+0)*xs;
		cty1=ybuf+(i*2+1)*xs;
		ctu=ubuf+i*xs1;
		ctv=vbuf+i*xs1;
		
		while(cs0<cs0e)
		{
			cb0=cs0[0];	cg0=cs0[1];	cr0=cs0[2];
			cb1=cs0[4];	cg1=cs0[5];	cr1=cs0[6];
			cb2=cs1[0];	cg2=cs1[1];	cr2=cs1[2];
			cb3=cs1[4];	cg3=cs1[5];	cr3=cs1[6];
			cs0+=8;		cs1+=8;
			
			cr=(cr0+cr1+cr2+cr3)>>2;
			cg=(cg0+cg1+cg2+cg3)>>2;
			cb=(cb0+cb1+cb2+cb3)>>2;
			
			cy0=cg0;	cy1=cg1;
			cy2=cg2;	cy3=cg3;
			cy=(3*cr+4*cg+cb)>>3;
			cu=((cb-cy)>>1)+128;
			cv=((cr-cy)>>1)+128;
			
			cty0[0]=cy0; cty0[1]=cy1;
			cty1[0]=cy2; cty1[1]=cy3;
			*ctu++=cu;		*ctv++=cv;
			cty0+=2;		cty1+=2;
		}
	}
	return(0);
}

u32 bit_htab[512];
byte *bit_ct;
u32 bit_win;
int bit_pos;

void llq0_EmitSym(int sym)
{
	int ht, hc, hl;
	int i;
	
	ht=bit_htab[sym];
	hc=(u16)ht;
	hl=(ht>>16)&15;
	bit_win|=hc<<bit_pos;
	bit_pos+=hl;

#if 0
	while(bit_pos>=8)
	{
		*bit_ct++=bit_win;
		bit_win=bit_win>>8;
		bit_pos-=8;
	}
#endif

#if 1
	i=bit_pos>>3;
	*(u32 *)bit_ct=bit_win;
	bit_ct+=i;
	bit_win>>=i<<3;
	bit_pos=bit_pos&7;
#endif
}

void llq0_EmitSymOrZ(int *rzc, int val)
{
	int zc, uval;
	
	zc=*rzc;

#if 1
	if(!val)
	{
		*rzc=zc+1;
		return;
	}
		
	if(zc>0)
	{
		while(zc>=256)
		{
			llq0_EmitSym(0);
			llq0_EmitSym(255);
			zc-=255;
		}
		
		if(zc>1)
		{
			llq0_EmitSym(0);
			llq0_EmitSym(zc);
		}else
		{
			llq0_EmitSym(1);
		}
		*rzc=0;
	}
#endif

	uval=(val<<1)^(val>>31);
	llq0_EmitSym(uval+1);
}

int tst_llqe0(byte *ibuf, byte *obuf, int xs, int ys)
{
	byte *csy0, *csy1, *csy0e, *csu, *csv;
	byte *ybuf, *ubuf, *vbuf;

	byte cy0, cy1, cy2, cy3;
	byte cy, cu, cv;
	
	int ly0, ly1, lu, lv;
	int dy0, dy1, dy2, dy3, du, dv;
	int xs1, ys1, ystr, qsc, qsf;
	int zc;
	int i, j, k;

	bit_ct=obuf;
	bit_win=0;
	bit_pos=0;

	xs1=xs>>1; ys1=ys>>1;
	ybuf=ibuf;
	ubuf=ybuf+xs*ys;
	vbuf=ubuf+xs1*ys1;
	
	qsc=9;
	qsf=256/qsc;
	zc=0;
	
	for(i=0; i<ys1; i++)
	{
		csy0=ybuf+(i*2+0)*xs;
		csy1=ybuf+(i*2+1)*xs;
		csy0e=csy0+xs;
		csu=ubuf+i*xs1;
		csv=vbuf+i*xs1;
		
		ly0=0;		ly1=0;
		lu=0;		lv=0;
		
		while(csy0<csy0e)
		{
			cy0=csy0[0]; cy1=csy0[1];
			cy2=csy1[0]; cy3=csy1[1];
			cu=*csu++;	cv=*csv++;
			csy0+=2;
			csy1+=2;
			
			dy0=cy0-ly0;
			dy2=cy2-ly1;
			dy0=(dy0*qsf)>>8;
			dy2=(dy2*qsf)>>8;
			ly0+=dy0*qsc;
			ly1+=dy2*qsc;

			dy1=cy1-ly0;
			dy3=cy3-ly1;
			dy1=(dy1*qsf)>>8;
			dy3=(dy3*qsf)>>8;
			ly0+=dy1*qsc;
			ly1+=dy3*qsc;

			du=cu-lu;
			dv=cv-lv;
			du=(du*qsf)>>8;
			dv=(dv*qsf)>>8;
			lu+=du*qsc;
			lv+=dv*qsc;

#if 1
			llq0_EmitSymOrZ(&zc, dy0);
			llq0_EmitSymOrZ(&zc, dy1);
			llq0_EmitSymOrZ(&zc, dy2);
			llq0_EmitSymOrZ(&zc, dy3);
			llq0_EmitSymOrZ(&zc, du);
			llq0_EmitSymOrZ(&zc, dv);
#endif
		}
	}

	llq0_EmitSym(0);
	llq0_EmitSym(0);
	
	return(bit_ct-obuf);
}

void cmp_rmse(byte *ibuf0, byte *ibuf1, int xs, int ys)
{
	double er, eg, eb;
	int dr, dg, db;
	int i, j, k, n;
	
	n=xs*ys; er=0; eg=0; eb=0;
	for(i=0; i<n; i++)
	{
		dr=ibuf0[i*4+0]-ibuf1[i*4+0];
		dg=ibuf0[i*4+1]-ibuf1[i*4+1];
		db=ibuf0[i*4+2]-ibuf1[i*4+2];
		er=er+dr*dr;
		eg=eg+dg*dg;
		eb=eb+db*db;
	}
	
	printf("RMSE: r=%.3f g=%.3f b=%.3f avg=%.3f\n",
		sqrt(er/n),
		sqrt(eg/n),
		sqrt(eb/n),
		sqrt((er+eg+eb)/(3*n)));
}

void cmp_blkrmse(byte *ibuf0, byte *ibuf1, byte *blks, int xs, int ys)
{
	static int btcost[32]={
	  1,   4,   8,   8,  16,  32,  32,  64,		//00-07
	 -1,   8,  16,  16,  32,  64,  64, 128,		//08-0F
	192, 256,  -1, 192,  16,  48,  96, 144,		//10-17
	192, 448, 256, 384, 256, 384, 384, 640};	//18-1F
	static char *btname[32]={
	  "Flat", "2x2x1", "4x2x1", "2x4x1",	//00-03
	 "4x4x1", "8x4x1", "4x8x1", "8x8x1",	//04-07
	  "R-08", "2x2x2", "4x2x2", "2x4x2", 	//08-0B
	 "4x4x2", "8x4x2", "4x8x2", "8x8x2",	//0C-0F
	 "8x8x3",			"8x8x4",			//10/11
	 "R-12",			"Y8x8x2+UV4x4x2",	//12/13
	 "Y2x2x2+UV2x2x1", 	"Y4x4x2+UV2x2x2",	//14/15
	 "Y4x4x2+UV4x4x2",	"Y8x8x2+UV2x2x2",	//16/1F
	 "Y8x8x2+UV4x4x2",	"Y8x8x3+UV8x8x2",	//18/1F
	 "Y8x8x2+UV4x8x2",  "Y8x8x2+UV8x8x2",	//1A/1F
	 "Y8x8x3+UV4x4x2",	"Y8x8x4+UV4x4x4",	//1C/1F
	 "Y8x8x3+UV4x8x3",	"Y8x8x4+UV8x8x3"};	//1E/1F

	static int abtcost[16]={
	  1,   4,  16,  64,   8,  32,  48,  64,		//00-07
	  1,  -1,  64,  64, 128, 128, 192, 256};	//18-1F
	static char *abtname[16]={
	  "Flat", "2x2x1", "4x4x1", "8x8x1",	//00-03
	 "2x2x2", "4x4x2", "4x4x3", "4x8x2",	//04-07
	 "Flat2", "RSV09", "4x8x2", "8x4x2", 	//08-0B
	 "8x8x2", "8x8x2", "8x8x3", "8x8x4"};	//0C-0F

	double btte[64];
	double bttea[64];
	int btn[64];

	double abtte[64];
	int abtn[64];

	byte *cs0, *cs1;
	double er, eg, eb, ea, e;
	int dr, dg, db, da;
	int xs1, ys1, ystr;
	int i, j, k, l, n;
	
	ystr=xs*4;
	xs1=xs>>3;
	ys1=ys>>3;
	
	for(i=0; i<64; i++)
	{
		btte[i]=0;
		bttea[i]=0;
		btn[i]=0;

		abtte[i]=0;
		abtn[i]=0;
	}
	
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		cs0=ibuf0+i*8*ystr+j*8*4;
		cs1=ibuf1+i*8*ystr+j*8*4;
		
		er=0; eg=0; eb=0; ea=0;
		for(k=0; k<8; k++)
			for(l=0; l<8; l++)
		{
			dr=cs0[k*ystr+l*4+0]-cs1[k*ystr+l*4+0];
			dg=cs0[k*ystr+l*4+1]-cs1[k*ystr+l*4+1];
			db=cs0[k*ystr+l*4+2]-cs1[k*ystr+l*4+2];
			da=cs0[k*ystr+l*4+3]-cs1[k*ystr+l*4+3];
			er=er+dr*dr;
			eg=eg+dg*dg;
			eb=eb+db*db;
			ea=ea+da*da;
		}
		e=(er+eg+eb)/(3*64);

		k=i*xs1+j;
		l=blks[k*64+0];
		btte[l]+=e;
		bttea[l]+=ea/64;
		btn[l]++;

		l=blks[k*64+1];
		abtte[l]+=ea/64;
		abtn[l]++;
	}

	for(i=0; i<64; i++)
	{
		if(!btn[i])
			continue;

		j=sqrt(btte[i]/btn[i])*100;
		k=sqrt(bttea[i]/btn[i])*10;
		printf("B-RMSE: %02X: n=%6d avge=%3d.%02d ae=%3d.%01d,"
			" Tc=%6d (%-14s)\n",
			i, btn[i], j/100, j%100, k/10, k%10,
			(btn[i]*btcost[i]+7)/8, btname[i]);
	}

	for(i=0; i<64; i++)
	{
		if(!abtn[i])
			continue;

		j=sqrt(abtte[i]/abtn[i])*100;
		printf("AB-RMSE: %02X: n=%6d ae=%3d.%01d,"
			" Tc=%6d (%-14s)\n",
			i, abtn[i], j/100, j%100,
			(abtn[i]*abtcost[i]+7)/8, abtname[i]);
	}

	n=xs*ys; er=0; eg=0; eb=0; ea=0;
	for(i=0; i<n; i++)
	{
		dr=ibuf0[i*4+0]-ibuf1[i*4+0];
		dg=ibuf0[i*4+1]-ibuf1[i*4+1];
		db=ibuf0[i*4+2]-ibuf1[i*4+2];
		da=ibuf0[i*4+3]-ibuf1[i*4+3];
		er=er+dr*dr;
		eg=eg+dg*dg;
		eb=eb+db*db;
		ea=ea+da*da;
	}
	
	printf("RMSE: r=%.2f g=%.2f b=%.2f a=%.2f avg=%.3f\n",
		sqrt(er/n),
		sqrt(eg/n),
		sqrt(eb/n),
		sqrt(ea/n),
		sqrt((er+eg+eb)/(3*n)));
}

int dump_bmp(char *name, int xs, int ys, int fcc, byte *ibuf, int isz)
{
	byte *tbuf;
	FILE *fd;
	int sz;

	fd=fopen(name, "wb");
	if(!fd)
	{
		printf("Fail Open %s\n", name);
		return(-1);
	}

	tbuf=malloc(isz+256);
	sz=flatten_bmp(tbuf, xs, ys, fcc, ibuf, isz);
	fwrite(tbuf, 1, sz, fd);
	free(tbuf);
	fclose(fd);
	return(0);
}

int flatten_bmp(byte *obuf,
	int xs, int ys, int fcc,
	byte *ibuf, int isz)
{
	byte hbuf[256];
	int sz1, sz2, sz3, dpm;
	
	sz1=54+isz;
	sz2=54;
	sz3=40;
	dpm=2880;
	
	hbuf[0x00]='B';		hbuf[0x01]='M';
	hbuf[0x02]=sz1;		hbuf[0x03]=sz1>>8;
	hbuf[0x04]=sz1>>16;	hbuf[0x05]=sz1>>24;
	hbuf[0x06]=0;		hbuf[0x07]=0;
	hbuf[0x08]=0;		hbuf[0x09]=0;
	hbuf[0x0A]=sz2;		hbuf[0x0B]=sz2>>8;
	hbuf[0x0C]=sz2>>16;	hbuf[0x0D]=sz2>>24;

	hbuf[0x0E]=sz3;		hbuf[0x0F]=sz3>>8;
	hbuf[0x10]=sz3>>16;	hbuf[0x11]=sz3>>24;
	hbuf[0x12]=xs;		hbuf[0x13]=xs>>8;
	hbuf[0x14]=xs>>16;	hbuf[0x15]=xs>>24;
	hbuf[0x16]=ys;		hbuf[0x17]=ys>>8;
	hbuf[0x18]=ys>>16;	hbuf[0x19]=ys>>24;

	hbuf[0x1A]=1;		hbuf[0x1B]=0;
	hbuf[0x1C]=32;		hbuf[0x1D]=0;
	hbuf[0x1E]=fcc;		hbuf[0x1F]=fcc>>8;
	hbuf[0x20]=fcc>>16;	hbuf[0x21]=fcc>>24;
	hbuf[0x22]=isz;		hbuf[0x23]=isz>>8;
	hbuf[0x24]=isz>>16;	hbuf[0x25]=isz>>24;
	hbuf[0x26]=dpm;		hbuf[0x27]=dpm>>8;
	hbuf[0x28]=dpm>>16;	hbuf[0x29]=dpm>>24;
	hbuf[0x2A]=dpm;		hbuf[0x2B]=dpm>>8;
	hbuf[0x2C]=dpm>>16;	hbuf[0x2D]=dpm>>24;

	hbuf[0x2E]=0;		hbuf[0x2F]=0;
	hbuf[0x30]=0;		hbuf[0x31]=0;

	hbuf[0x32]=0;		hbuf[0x33]=0;
	hbuf[0x34]=0;		hbuf[0x35]=0;
	
	memcpy(obuf, hbuf, sz2);
	memcpy(obuf+sz2, ibuf, isz);
	return(sz2+isz);
	
//	fwrite(hbuf, 1, sz2, fd);
//	fwrite(ibuf, 1, isz, fd);
//	fclose(fd);
//	return(0);
}

int main()
{
	BT4A_Context tctx;
	BT4A_Context *ctx;
	byte *tbuf0, *tbuf1, *tbuf2, *cbuf;
	double dt, mpxf;
	int t0, t1, t2, t3, t0e;
	int xs, ys, nf, sz;
	int i, j, k;
	
//	xs=3840;
//	ys=2160;

//	tbuf0=BTIC1H_Img_LoadTGA("DSC00602_1.tga", &xs, &ys);
	tbuf0=BTIC1H_Img_LoadTGA("yazil0_1.tga", &xs, &ys);
	
//	tbuf0=malloc(xs*ys*4);
	tbuf1=malloc(xs*ys*8);
	tbuf2=malloc(xs*ys*8);
//	cbuf=malloc(xs*ys*8);
	cbuf=malloc(1<<24);
	
	for(i=0; i<512; i++)
	{
		bit_htab[i]=i|(9<<16);
	}

#if 0
	k=xs*ys;
	for(i=0; i<k; i++)
	{
		tbuf0[i*4+0]=rand();
		tbuf0[i*4+1]=rand();
		tbuf0[i*4+2]=rand();
		tbuf0[i*4+3]=rand();
	}
#endif
	
	mpxf=(xs*ys)/1000000.0;
	
	ctx=&tctx;

#if 0
	printf("Encode Test:\n");
	t0=clock(); t1=t0; t0e=t0+(10*CLOCKS_PER_SEC);
	nf=0; sz=0;
	while(t1<t0e)
	{
		memset(ctx, 0, sizeof(BT4A_Context));
		ctx->blks=tbuf2;

#if 1
//		LQTVQ_SetupContextQf(ctx, 75);
		LQTVQ_SetupContextQf(ctx, 60);
		LQTVQ_EncImageBGRA(ctx, tbuf2, tbuf0, xs, ys);
		sz=LQTVQ_EncImgBlocks(ctx, cbuf, tbuf2, NULL, xs, ys, 75);
#endif

//		sz=LQTVQ_EncImgBufFastBGRA(ctx, tbuf1, tbuf0, xs, ys, 75);

//		sz=LQTVQ_EncImageThreadsBGRA(ctx,
//			tbuf1, xs*ys*8, tbuf0, xs, ys, 75);

//		LQTVQ_EncImageThreadsBGRA(ctx, tbuf1, tbuf0, xs, ys);
	
//		LQTVQ_EncImageBGRA(tbuf1, tbuf0, xs, ys);
//		tst_llq0(tbuf0, tbuf1, xs, ys);
//		sz=tst_llqe0(tbuf1, tbuf2, xs, ys);
		nf++;
		t1=clock();
		t2=t1-t0;
		dt=t2/((double)CLOCKS_PER_SEC);
		printf("%d %.2fs %.2ffps %.2fMpix/s sz=%dKiB (%.2fbpp)\r", nf, dt,
			nf/dt, mpxf*(nf/dt), ((sz+512)>>10), (sz*8.0)/(xs*ys));
	}
	printf("\n");
#endif

#if 1
	memset(ctx, 0, sizeof(BT4A_Context));
//	ctx->blks=tbuf2;

//	sz=LQTVQ_EncodeImgBufferCtx(ctx, cbuf, 1<<24, tbuf0, xs, ys, 40, 0);
//	sz=LQTVQ_EncodeImgBufferCtx(ctx, cbuf, 1<<24, tbuf0, xs, ys, 60, 0);
	sz=LQTVQ_EncodeImgBufferCtx(ctx, cbuf, 1<<24, tbuf0, xs, ys, 75, 0);
//	sz=LQTVQ_EncodeImgBufferCtx(ctx, cbuf, 1<<24, tbuf0, xs, ys, 85, 0);

//	LQTVQ_SetupContextQf(ctx, 75);
//	LQTVQ_SetupContextQf(ctx, 40);
//	LQTVQ_EncImageBGRA(ctx, tbuf2, tbuf0, xs, ys);
//	sz=LQTVQ_EncImgBlocks(ctx, cbuf, tbuf2, NULL, xs, ys, 40);

	LQTVQ_DumpStatsCtx(ctx);

	dump_bmp("bt4at0_bt4b.bmp", xs, ys,
		LQTVQ_FCC_BT4B, cbuf, sz);

	printf("sz=%dKiB (%.2fbpp)\n", ((sz+512)>>10), (sz*8.0)/(xs*ys));
#endif

#if 1
	memset(ctx, 0, sizeof(BT4A_Context));

	printf("Decode Test:\n");
	t0=clock(); t1=t0; t0e=t0+(10*CLOCKS_PER_SEC);
	nf=0;
	while(t1<t0e)
	{
//		memset(ctx, 0, sizeof(BT4A_Context));
//		ctx->blks=tbuf2;

//		LQTVQ_DecImgBlocks(ctx, cbuf, sz,
//			tbuf2, NULL, xs, ys);
//		LQTVQ_DecImageBGRA(tbuf2, tbuf1, xs, ys);

		LQTVQ_DecodeImgBufferCtx(ctx, cbuf, sz, tbuf1, xs, ys, 0);

		nf++;
		t1=clock();
		t2=t1-t0;
		dt=t2/((double)CLOCKS_PER_SEC);
		printf("%d %.2fs %.2ffps %.2fMpix/s\r", nf, dt,
			nf/dt, mpxf*(nf/dt));
	}
	printf("\n");
#endif

#if 1
	memset(ctx, 0, sizeof(BT4A_Context));
	memset(tbuf1, 0, xs*ys*8);
	memset(tbuf2, 0, xs*ys*8);

	LQTVQ_DecodeImgBufferCtx(ctx, cbuf, sz, tbuf1, xs, ys, 0);
	cmp_blkrmse(tbuf0, tbuf1, ctx->blks, xs, ys);

//	LQTVQ_DecImgBlocks(ctx, cbuf, sz,
//		tbuf2, NULL, xs, ys);
//	LQTVQ_DecImageBGRA(tbuf2, tbuf1, xs, ys);
//	cmp_rmse(tbuf0, tbuf1, xs, ys);
//	cmp_blkrmse(tbuf0, tbuf1, tbuf2, xs, ys);
	BTIC1H_Img_SaveTGA("bt4at0.tga", tbuf1, xs, ys);
#endif

//	LQTVQ_DecImageBGRA(tbuf1, tbuf2, xs, ys);
//	cmp_rmse(tbuf0, tbuf2, xs, ys);

//	BTIC1H_Img_SaveTGA("bt4at0.tga", tbuf2, xs, ys);
}
