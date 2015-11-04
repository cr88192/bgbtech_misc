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

#include "bcn_decls.h"
#include "bcn_bitstream.c"

#include "bc7_common.c"
#include "bc7_blkdec.c"
#include "bc7_filter_cmy.c"
#include "bc7_filter_yuv.c"
#include "bc7_blkenc.c"
#include "bc7_blkenc_p2.c"

#include "bt1h_targa.c"

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

int staterrorblocks(byte *ibuf, byte *obuf,
	byte *block, int xs, int ys, int stride)
{
	int stat[16];
	int estat[16];
	int xs1, ys1, xs2, ys2;
	int e;
	int i, j, k;
	
	for(i=0; i<16; i++)
	{
		stat[i]=0;
		estat[i]=0;
	}
	
	xs1=xs>>2; ys1=ys>>2;
	xs2=(xs+3)>>2; ys2=(ys+3)>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		e=BGBBTJ_BC7_CalcBlockError(
			ibuf+(i*4*xs+j*4)*stride, stride, xs*stride,
			obuf+(i*4*xs+j*4)*stride, stride, xs*stride);
	
//		BGBBTJ_BC7_EncodeBlock(
//			block+(i*xs2+j)*16,
//			rgba+(i*4*xs+j*4)*stride,
//			stride, xs*stride, pfb);

		k=BGBBTJ_BC7_GetBlockMode(block+(i*xs2+j)*16);
		stat[k]++;
		estat[k]+=sqrt(e/16);
	}

	printf("BGBBTJ_BC7_EncodeImageBestBC7:\n"
			"  0:%6d/%2.2f\n"
			"  1:%6d/%2.2f\n"
			"  2:%6d/%2.2f\n"
			"  3:%6d/%2.2f\n"
			"  4:%6d/%2.2f\n"
			"  5:%6d/%2.2f\n"
			"  6:%6d/%2.2f\n"
			"  7:%6d/%2.2f\n"
			"  8:%6d/%2.2f\n",
		stat[0], estat[0]/(stat[0]+1.0),
		stat[1], estat[1]/(stat[1]+1.0),
		stat[2], estat[2]/(stat[2]+1.0),
		stat[3], estat[3]/(stat[3]+1.0),
		stat[4], estat[4]/(stat[4]+1.0),
		stat[5], estat[5]/(stat[5]+1.0),
		stat[6], estat[6]/(stat[6]+1.0),
		stat[7], estat[7]/(stat[7]+1.0),
		stat[8], estat[8]/(stat[8]+1.0));
}

int main()
{
	byte *ibuf, *yibuf, *obuf, *blks, *yibuf2;
	byte *tbuf, *tbuf1, *ct1, *ibuf2;
	FILE *tfd;
	double f, g, h, e;
	int xs, ys, xs1, ys1;
	int cr, cg, cb, cy, cu, cv, cu1, cv1;
	int t0, t1, t2, t3;
	int n, n1, nf, ncf, qf;
	int i, j, k, l;

//	BTIC1H_InitCamera();

//	ibuf=BTIC1H_Img_LoadTGA("StreetHollandSD.tga", &xs, &ys);
//	ibuf2=BTIC1H_Img_LoadTGA("StreetHollandSD_q45.tga", &xs1, &ys1);
//	ibuf=BTIC1H_Img_LoadTGA("3917969_f260.tga", &xs, &ys);

//	ibuf=BTIC1H_Img_LoadTGA("screencap0.tga", &xs, &ys);
//	ibuf2=BTIC1H_Img_LoadTGA("screencap0.tga", &xs1, &ys1);

//	ibuf=BTIC1H_Img_LoadTGA("MLP_FIM1.tga", &xs, &ys);
//	ibuf2=BTIC1H_Img_LoadTGA("MLP_FIM1.tga", &xs1, &ys1);

	ibuf=BTIC1H_Img_LoadTGA("Chem0.tga", &xs, &ys);
	ibuf2=BTIC1H_Img_LoadTGA("Chem0.tga", &xs1, &ys1);

//	yibuf=malloc(xs*ys*2);

	if(!ibuf || !ibuf2)
	{
		printf("failed load test images\n");
		return(-1);
	}

	obuf=malloc(xs*ys*8);
//	blks=malloc(xs*ys*2);

	tbuf=malloc(1<<20);
	tbuf1=malloc(1<<20);

	BGBBTJ_BC7_PartitionInit();

	t0=clock(); t1=t0; nf=0; ncf=0;
	while((t1>=t0) && (t1<(t0+(15*CLOCKS_PER_SEC))))
	{
		BGBBTJ_BC7_EncodeImageBestBC7(tbuf, ibuf, xs, ys, 4, 0);

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

	BGBBTJ_BC7_StatImageBC7(tbuf, xs, ys);

	xs1=(xs+3)>>2;
	ys1=(ys+3)>>2;
	n=xs1*ys1;

	tfd=fopen("bc7_raw0.dat", "wb");
	if(tfd)
	{
		fwrite(tbuf, 1, n*16, tfd);
		fclose(tfd);
	}


	BGBBTJ_BC7_DecodeImageBC7(tbuf, obuf, xs, ys, 4);

	staterrorblocks(ibuf, obuf, tbuf, xs, ys, 4);

	printf("CLRS I: ");
	checkrmse(ibuf, ibuf, xs, ys);

	printf("CLRS 7: ");
	checkrmse(ibuf, obuf, xs, ys);

	BTIC1H_Img_SaveTGA("tst1g_out0.tga", obuf, xs, ys);

}
