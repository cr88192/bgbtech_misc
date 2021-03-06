/*
Copyright (C) 2015 by Brendan G Bohannon

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "bt1h_common.c"

#include "bt1h_rice.c"
#include "bt1h_decrice.c"
#include "bt1h_blkenc.c"
#include "bt1h_blkdec.c"
#include "bt1h_encode.c"
#include "bt1h_decode.c"
#include "bt1h_targa.c"

// #include "bt2d_dct.c"
// #include "bt2d_encode.c"

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

int main(int argc, char *argv[])
{
	byte *ibuf, *yibuf, *obuf, *blks, *yibuf2;
	byte *tbuf, *ct1, *ibuf2;
	BTIC1H_Context *ctx;
	FILE *tfd;
	double f, g, h, e;
	int xs, ys, xs1, ys1;
	int cr, cg, cb, cy, cu, cv, cu1, cv1;
	int t0, t1, t2, t3;
	int n, n1, nf, ncf, qf;
	int i, j, k, l;

//	BTIC1H_InitCamera();

//	ibuf=BTIC1H_Img_LoadTGA("StreetHollandSD.tga", &xs, &ys);
	ibuf2=BTIC1H_Img_LoadTGA("StreetHollandSD_q45.tga", &xs1, &ys1);
//	ibuf=BTIC1H_Img_LoadTGA("3917969_f260.tga", &xs, &ys);

	ibuf=BTIC1H_Img_LoadTGA("screencap0.tga", &xs, &ys);
	ibuf2=BTIC1H_Img_LoadTGA("screencap0.tga", &xs1, &ys1);

	yibuf=malloc(xs*ys*2);

	if(!ibuf || !ibuf2)
	{
		printf("failed load test images\n");
		return(-1);
	}

	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
	{
		cr=ibuf[(i*xs+j)*4+0];
		cg=ibuf[(i*xs+j)*4+1];
		cb=ibuf[(i*xs+j)*4+2];
		
		cy=(( 19595*cr + 38470*cg + 7471*cb + 32768)>>16)+0;
		cu=((-11056*cr - 21712*cg +32768*cb + 32768)>>16)+128;
		cv=(( 32768*cr - 27440*cg - 5328*cb + 32768)>>16)+128;
		
//		cu=128;		cv=128;
		
		cy=clamp255(cy);
		cu=clamp255(cu);
		cv=clamp255(cv);
		
//		k=j&(~1);
		yibuf[(i*xs+j)*2+0]=cy;
//		yibuf[(i*xs+k)*2+1]=cu;
//		yibuf[(i*xs+k)*2+3]=cv;
//		yibuf[(i*xs+j)*2+1]=(j&1)?cv:cu;
//		yibuf[(i*xs+j)*2+1]=(yibuf[(i*xs+j)*2+1]+((j&1)?cv:cu))>>1;
//		yibuf[(i*xs+j)*2+3]=(j&1)?cu:cv;

		if(j&1)
		{
			k=j&(~1);
			yibuf[(i*xs+k)*2+1]=(yibuf[(i*xs+k)*2+1]+cu)>>1;
			yibuf[(i*xs+k)*2+3]=(yibuf[(i*xs+k)*2+3]+cv)>>1;
		}else
		{
			yibuf[(i*xs+j)*2+1]=cu;
			yibuf[(i*xs+j)*2+3]=cv;
		}
	}

//	tbuf=BTIC1H_GetCameraFBuf();
//	if(tbuf)
//	{
//		usleep(1000000);
//		memcpy(yibuf, tbuf, xs*ys*2);
//	}

	obuf=malloc(xs*ys*4);
//	blks=malloc(xs*ys*2);

	tbuf=malloc(1<<20);

	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
	{
		k=j&(~1);
		cy=yibuf[(i*xs+j)*2+0];
		cu=yibuf[(i*xs+k)*2+1];
		cv=yibuf[(i*xs+k)*2+3];

		cu1=cu-128; cv1=cv-128;
		cr=65536*cy           +91881*cv1;
		cg=65536*cy- 22554*cu1-46802*cv1;
		cb=65536*cy+116130*cu1;
//		cr>>=16; cg>>=16; cb>>=16;

		cr=(cr+32768)>>16;
		cg=(cg+32768)>>16;
		cb=(cb+32768)>>16;

		cr=clamp255(cr);
		cg=clamp255(cg);
		cb=clamp255(cb);

		obuf[(i*xs+j)*4+0]=cr;
		obuf[(i*xs+j)*4+1]=cg;
		obuf[(i*xs+j)*4+2]=cb;
	}

	printf("QCLRS: ");
	checkrmse(ibuf, obuf, xs, ys);

	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
	{
#if 1
		cr=ibuf[(i*xs+j)*4+0];
		cg=ibuf[(i*xs+j)*4+1];
		cb=ibuf[(i*xs+j)*4+2];

//		cy=(2*cg+cb+cr)>>2;
//		cu=((cb-cy)>>1)+128;
//		cv=((cr-cy)>>1)+128;

		cy=((cg<<2)+3*cr+cb+4)>>3;
//		cy=((cg<<2)+2*cr+2*cb+4)>>3;
		cu=((2*cb-cr-cg+2)>>2)+128;
		cv=((2*cr-cb-cg+2)>>2)+128;
//		cu=((4*cb-3*cr-1*cg+4)>>3)+128;
//		cv=((4*cr-2*cb-2*cg+4)>>3)+128;

		cy=clamp255(cy);
		cu=clamp255(cu);
		cv=clamp255(cv);
#endif

//		k=j&(~1);
//		cy=yibuf[(i*xs+j)*2+0];
//		cu=yibuf[(i*xs+k)*2+1];
//		cv=yibuf[(i*xs+k)*2+3];

		cu1=cu-128; cv1=cv-128;
		cr=65536*cy           +91881*cv1;
		cg=65536*cy- 22554*cu1-46802*cv1;
		cb=65536*cy+116130*cu1;
		cr>>=16; cg>>=16; cb>>=16;

//		cr=(cr+32768)>>16;
//		cg=(cg+32768)>>16;
//		cb=(cb+32768)>>16;

//		cu1=cu-128; cv1=cv-128;
//		cb=cy+cu1*2;
//		cr=cy+cv1*2;
//		cg=(4*cy-cb-cr)>>1;

		cr=clamp255(cr);
		cg=clamp255(cg);
		cb=clamp255(cb);

		obuf[(i*xs+j)*4+0]=cr;
		obuf[(i*xs+j)*4+1]=cg;
		obuf[(i*xs+j)*4+2]=cb;
	}

	printf("QCLRS2: ");
	checkrmse(ibuf, obuf, xs, ys);

	memset(obuf, 0, xs*ys*4);

//	qf=65;
//	qf=85;
//	qf=95;
	qf=90;
	
	qf=qf|BTIC1H_QFL_IFRAME;
	
	xs1=xs>>2; ys1=ys>>2; n=xs1*ys1;
	blks=malloc(n*32);

	ctx=BTIC1H_AllocContext();
	ctx->xs=xs;
	ctx->ys=ys;
	ctx->flip=0;

	t0=clock(); t1=t0; nf=0; ncf=0;
	while((t1>=t0) && (t1<(t0+(1*CLOCKS_PER_SEC))))
	{
		yibuf2=yibuf;

//		ct1=BTIC2D_EncodeBlocksBufferYUY2(tbuf, 1<<20, yibuf2, xs, ys, 85);
		
//		BTIC1H_EncodeImageYUY2(blks, 32, yibuf2, xs, ys, qf);
//		ct1=BTIC1H_EncodeBlocksBuffer(tbuf, blks, NULL, n,
//			32, 1<<20, &n1, qf);

		i=BTIC1H_EncodeCtx(ctx, ibuf, tbuf, 1<<20,
			xs, ys, qf, BTIC1H_PXF_RGBX);

//		i=BTIC1H_EncodeCtx(ctx, yibuf2, tbuf, 1<<20,
//			xs, ys, qf, BTIC1H_PXF_YUYV);
		ct1=tbuf+i;

		nf++; t1=clock();
		
//		if(!(nf&15))
		if(1)
		{
			t2=t1-t0;
			f=(t2)/((double)CLOCKS_PER_SEC);
			printf("fc=%d dt=%f fps=%f Mpix=%f, ncf=%d cfps=%f\r",
				nf, f, nf/f, (nf/f)*(xs*ys*(1.0/1000000)),
				ncf, (ncf/f)*(xs*ys*(1.0/1000000)));
		}
	}
	printf("\n");
	
	BTIC1H_DumpGeneralEncodeStats();
	
	h=((ct1-tbuf)*8.0)/(xs*ys);
	printf("Output Size %d / %d (%.3f bpp)\n", ct1-tbuf, n*7,
		h);
	
	tfd=fopen("tst1g_raw0.dat", "wb");
	if(tfd)
	{
		fwrite(tbuf, 1, ct1-tbuf, tfd);
		fclose(tfd);
	}

	memset(blks, 0, n*32);

#if 1
	t0=clock(); t1=t0; nf=0; ncf=0;
	while((t1>=t0) && (t1<(t0+(1*CLOCKS_PER_SEC))))
	{
		
//		BTIC1H_EncodeImageYUY2(blks, yibuf2, xs, ys, 85);
//		ct1=BTIC1H_EncodeBlocksBuffer(tbuf, blks, NULL, n, 16, 1<<20, &n1);

//		BTIC1H_DecodeBlocksBuffer(tbuf, blks, NULL, n, 32,
//			xs1, ys1, ct1-tbuf);
//		BTIC1H_DecodeImageMB2B(blks, 32, obuf, xs, ys, 4);

		BTIC1H_DecodeCtx(ctx, tbuf, obuf, ct1-tbuf,
			xs*ys*4, &xs1, &ys1, BTIC1H_PXF_RGBA);

		nf++; t1=clock();
		
//		if(!(nf&15))
		if(1)
		{
			t2=t1-t0;
			f=(t2)/((double)CLOCKS_PER_SEC);
			printf("fc=%d dt=%f fps=%f Mpix=%f, ncf=%d cfps=%f\r",
				nf, f, nf/f, (nf/f)*(xs*ys*(1.0/1000000)),
				ncf, (ncf/f)*(xs*ys*(1.0/1000000)));
		}
	}
	printf("\n");
#endif

#if 1
	memset(blks, 0, n*32);
//	BTIC1H_DecodeBlocksBuffer(tbuf, blks, NULL, n, 32, xs1, ys1, ct1-tbuf);
//	BTIC1H_DecodeImageMB2B(blks, 32, obuf, xs, ys, 4);

	BTIC1H_DecodeCtx(ctx, tbuf, obuf, ct1-tbuf,
		xs*ys*4, &xs1, &ys1, BTIC1H_PXF_RGBA);
	
	BTIC1H_Img_SaveTGA("tst1g_out0.tga", obuf, xs, ys);

	printf("Q_JPG: ");
	checkrmse(ibuf, ibuf2, xs, ys);
	
	printf("QBT1H: ");
	e=checkrmse(ibuf, obuf, xs, ys);
	printf("Ebit: %f\n", h*e);
#endif

	return(0);
}
