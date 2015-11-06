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


void BGBBTJ_BC7_EncodeBlock_VecSwapRGB(int *clra, int *clrb)
{
	int t;
	t=clra[0]; clra[0]=clrb[0]; clrb[0]=t;
	t=clra[1]; clra[1]=clrb[1]; clrb[1]=t;
	t=clra[2]; clra[2]=clrb[2]; clrb[2]=t;
}

void BGBBTJ_BC7_EncodeBlock_VecSwapRGBA(int *clra, int *clrb)
{
	int t;
	t=clra[0]; clra[0]=clrb[0]; clrb[0]=t;
	t=clra[1]; clra[1]=clrb[1]; clrb[1]=t;
	t=clra[2]; clra[2]=clrb[2]; clrb[2]=t;
	t=clra[3]; clra[3]=clrb[3]; clrb[3]=t;
}

void BGBBTJ_BC7_EncodeBlock_VecSwapR(int *clra, int *clrb)
	{ int t; t=clra[0]; clra[0]=clrb[0]; clrb[0]=t; }
void BGBBTJ_BC7_EncodeBlock_VecSwapG(int *clra, int *clrb)
	{ int t; t=clra[1]; clra[1]=clrb[1]; clrb[1]=t; }
void BGBBTJ_BC7_EncodeBlock_VecSwapB(int *clra, int *clrb)
	{ int t; t=clra[2]; clra[2]=clrb[2]; clrb[2]=t; }
void BGBBTJ_BC7_EncodeBlock_VecSwapA(int *clra, int *clrb)
	{ int t; t=clra[3]; clra[3]=clrb[3]; clrb[3]=t; }

void BGBBTJ_BC7_EncodeBlock_VecInvertPixels(
	byte *pxv, int n, int min, int max)
{
	int i, j, k;
	
	for(i=0; i<n; i++)
		{ pxv[i]=max-(pxv[i]-min); }
}

void BGBBTJ_BC7_EncodeBlock_Mode4(byte *block,
	byte *pxy, byte *pxa,
	int *min, int *max,
	int mcy, int ncy,
	int mca, int nca)
{
	static const char idxtab[16]=
		{ 0,0,0,0, 0,0,1,1, 2,2,3,3, 3,3,3,3 };
	static const char idxtab2[16]=
		{ 0,0,0,0, 0,1,2,3, 4,5,6,7, 7,7,7,7 };
	
	BGBBTJ_BitStream bits;

	int p0, p1, p2, p3, p4, p5, p6, p7;
	int l0, l1, l2, l3a, l3b;
	int avg[4], acy, aca;
	int acr, acg, acb, ixb;
	int cr, cg, cb, ca, cy;
	int i, j, k, l;

	avg[0]=(min[0]+max[0])>>1;
	avg[1]=(min[1]+max[1])>>1;
	avg[2]=(min[2]+max[2])>>1;
	avg[3]=(min[3]+max[3])>>1;	
	acy=(mcy+ncy)>>1;
	aca=(mca+nca)>>1;
	
	ixb=1;

	BGBBTJ_BitsLE_ClearSetupWrite(&bits, block, 16);
	BGBBTJ_BitsLE_WriteBits(&bits, 16, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, 0, 2);
	BGBBTJ_BitsLE_WriteBits(&bits, ixb, 1);

	if(pxy[0]>=acy)
	{
		BGBBTJ_BC7_EncodeBlock_VecSwapRGB(min, max);
		BGBBTJ_BC7_EncodeBlock_VecInvertPixels(pxy, 16, mcy, ncy);
	}

	if(pxa[0]>=aca)
	{
		BGBBTJ_BC7_EncodeBlock_VecSwapA(min, max);
		BGBBTJ_BC7_EncodeBlock_VecInvertPixels(pxa, 16, mca, nca);
	}

	BGBBTJ_BitsLE_WriteBits(&bits, (min[0])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (max[0])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (min[1])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (max[1])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (min[2])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (max[2])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (min[3])>>2, 6);
	BGBBTJ_BitsLE_WriteBits(&bits, (max[3])>>2, 6);

//	l0=49152/(nca-aca+1);		//Fix-Point Scale (Alpha)
	l0=32768/(nca-aca+1);		//Fix-Point Scale (Alpha)
	l1=32768/(ncy-acy+1);		//Fix-Point Scale (Luma)
	l2=65536-2048;
	l3a=65536;

	if(ixb)
	{
		p0=idxtab[((pxa[0]-aca)*l0+l3a)>>13];
		BGBBTJ_BitsLE_WriteBit(&bits, p0);
		for(i=1; i<16; i++)
		{
			p0=idxtab[((pxa[i]-aca)*l0+l3a)>>13];
			BGBBTJ_BitsLE_Write2Bits(&bits, p0);
		}

		p0=idxtab2[((pxy[0]-acy)*l1+l3a)>>13];
		BGBBTJ_BitsLE_Write2Bits(&bits, p0);
		for(i=1; i<16; i++)
		{
			p0=idxtab2[((pxy[i]-acy)*l1+l3a)>>13];
			BGBBTJ_BitsLE_Write3Bits(&bits, p0);
		}
	}else
	{
		p0=idxtab[((pxy[0]-acy)*l1+l3a)>>13];
		BGBBTJ_BitsLE_WriteBit(&bits, p0);
		for(i=1; i<16; i++)
		{
			p0=idxtab[((pxy[i]-acy)*l1+l3a)>>13];
			BGBBTJ_BitsLE_Write2Bits(&bits, p0);
		}

		p0=idxtab2[((pxa[0]-aca)*l0+l3a)>>13];
		BGBBTJ_BitsLE_Write2Bits(&bits, p0);
		for(i=1; i<16; i++)
		{
			p0=idxtab2[((pxa[i]-aca)*l0+l3a)>>13];
			BGBBTJ_BitsLE_Write3Bits(&bits, p0);
		}
	}

	BGBBTJ_BitsLE_FlushBits(&bits);
}

void BGBBTJ_BC7_EncodeBlock_Mode5(byte *block,
	byte *pxy, byte *pxa,
	int *min, int *max,
	int mcy, int ncy,
	int mca, int nca)
{
	static const char idxtab[16]=
		{ 0,0,0,0, 0,0,1,1, 2,2,3,3, 3,3,3,3 };
	
	BGBBTJ_BitStream bits;

	int p0, p1, p2, p3, p4, p5, p6, p7;
	int l0, l1, l2, l3a, l3b;
	int avg[4], acy, aca;
	int acr, acg, acb;
	int cr, cg, cb, ca, cy;
	int i, j, k, l;

	avg[0]=(min[0]+max[0])>>1;
	avg[1]=(min[1]+max[1])>>1;
	avg[2]=(min[2]+max[2])>>1;
	avg[3]=(min[3]+max[3])>>1;	
	acy=(mcy+ncy)>>1;
	aca=(mca+nca)>>1;
	
	BGBBTJ_BitsLE_ClearSetupWrite(&bits, block, 16);
	BGBBTJ_BitsLE_WriteBits(&bits, 32, 6);
	BGBBTJ_BitsLE_WriteBits(&bits, 0, 2);

	if(pxy[0]>=acy)
	{
		BGBBTJ_BC7_EncodeBlock_VecSwapRGB(min, max);
		BGBBTJ_BC7_EncodeBlock_VecInvertPixels(pxy, 16, mcy, ncy);
	}

	if(pxa[0]>=aca)
	{
		BGBBTJ_BC7_EncodeBlock_VecSwapA(min, max);
		BGBBTJ_BC7_EncodeBlock_VecInvertPixels(pxa, 16, mca, nca);
	}

	BGBBTJ_BitsLE_WriteBits(&bits, (min[0])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (max[0])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (min[1])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (max[1])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (min[2])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (max[2])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, min[3], 8);
	BGBBTJ_BitsLE_WriteBits(&bits, max[3], 8);

//	l0=49152/(nca-aca+1);		//Fix-Point Scale (Alpha)
	l0=32768/(nca-aca+1);		//Fix-Point Scale (Alpha)
	l1=32768/(ncy-acy+1);		//Fix-Point Scale (Luma)
	l2=65536-2048;
	l3a=65536;

	p0=idxtab[((pxy[0]-acy)*l1+l3a)>>13];
	BGBBTJ_BitsLE_WriteBit(&bits, p0);
	for(i=1; i<16; i++)
	{
		p0=idxtab[((pxy[i]-acy)*l1+l3a)>>13];
		BGBBTJ_BitsLE_Write2Bits(&bits, p0);
	}

	p0=idxtab[((pxa[0]-aca)*l0+l3a)>>13];
	BGBBTJ_BitsLE_WriteBit(&bits, p0);
	for(i=1; i<16; i++)
	{
		p0=idxtab[((pxa[i]-aca)*l0+l3a)>>13];
		BGBBTJ_BitsLE_Write2Bits(&bits, p0);
	}

	BGBBTJ_BitsLE_FlushBits(&bits);
}

void BGBBTJ_BC7_EncodeBlock_Mode6(byte *block,
	byte *pxy,
	int *min, int *max,
	int mcy, int ncy)
{
	static const char idxtab[32]=
		{  0,  0,  0,  0,  0,  0,  0,  0,
		   0,  1,  2,  3,  4,  5,  6,  7,
		   8,  9, 10, 11, 12, 13, 14, 15,
		  15, 15, 15, 15, 15, 15, 15, 15};
	
	BGBBTJ_BitStream bits;

	int p0, p1, p2, p3, p4, p5, p6, p7;
	int l0, l1, l2, l3a, l3b;
	int acy, aca;
	int acr, acg, acb;
	int cr, cg, cb, ca, cy;
	int i, j, k, l;

	acy=(mcy+ncy)>>1;

	BGBBTJ_BitsLE_ClearSetupWrite(&bits, block, 16);
	BGBBTJ_BitsLE_WriteBits(&bits, 64, 7);

	if(pxy[0]>=acy)
	{
		BGBBTJ_BC7_EncodeBlock_VecSwapRGBA(min, max);
		BGBBTJ_BC7_EncodeBlock_VecInvertPixels(pxy, 16, mcy, ncy);
	}

	BGBBTJ_BitsLE_WriteBits(&bits, (min[0])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (max[0])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (min[1])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (max[1])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (min[2])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (max[2])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (min[3])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (max[3])>>1, 7);
	BGBBTJ_BitsLE_WriteBit(&bits, min[1]);
	BGBBTJ_BitsLE_WriteBit(&bits, max[1]);

//	l0=49152/(nca-aca+1);		//Fix-Point Scale (Alpha)
	l1=32768/(ncy-acy+1);		//Fix-Point Scale (Luma)
//	l1=(32768-512)/(ncy-acy+1);		//Fix-Point Scale (Luma)
//	l2=65536-2048;
	l3a=65536-1024;
	l3b=65536-1024+512;

	p0=idxtab[((pxy[0]-acy)*l1+l3a)>>12];
	p1=idxtab[((pxy[1]-acy)*l1+l3b)>>12];
	p2=idxtab[((pxy[2]-acy)*l1+l3a)>>12];
	p3=idxtab[((pxy[3]-acy)*l1+l3b)>>12];
	if(p0>7)p0=7;
	BGBBTJ_BitsLE_Write3Bits(&bits, p0);
	BGBBTJ_BitsLE_Write12Bits(&bits, p1|(p2<<4)|(p3<<8));

	p0=idxtab[((pxy[ 4]-acy)*l1+l3b)>>12];
	p1=idxtab[((pxy[ 5]-acy)*l1+l3a)>>12];
	p2=idxtab[((pxy[ 6]-acy)*l1+l3b)>>12];
	p3=idxtab[((pxy[ 7]-acy)*l1+l3a)>>12];
	BGBBTJ_BitsLE_Write16Bits(&bits, p0|(p1<<4)|(p2<<8)|(p3<<12));
	p0=idxtab[((pxy[ 8]-acy)*l1+l3a)>>12];
	p1=idxtab[((pxy[ 9]-acy)*l1+l3b)>>12];
	p2=idxtab[((pxy[10]-acy)*l1+l3a)>>12];
	p3=idxtab[((pxy[11]-acy)*l1+l3b)>>12];
	BGBBTJ_BitsLE_Write16Bits(&bits, p0|(p1<<4)|(p2<<8)|(p3<<12));
	p0=idxtab[((pxy[12]-acy)*l1+l3b)>>12];
	p1=idxtab[((pxy[13]-acy)*l1+l3a)>>12];
	p2=idxtab[((pxy[14]-acy)*l1+l3b)>>12];
	p3=idxtab[((pxy[15]-acy)*l1+l3a)>>12];
	BGBBTJ_BitsLE_Write16Bits(&bits, p0|(p1<<4)|(p2<<8)|(p3<<12));

	BGBBTJ_BitsLE_FlushBits(&bits);
}


void BGBBTJ_BC7_EncodeBlock_Mode1(byte *block,
	byte *pxy0, byte *pxy1, int *min, int *max,
	int *rmcy, int *rncy, int part);

void BGBBTJ_BC7_EncodeBlock_Mode3(byte *block,
	byte *pxy0, byte *pxy1, int *min, int *max,
	int *rmcy, int *rncy, int part);

void BGBBTJ_BC7_EncodeBlock_Mode7(byte *block,
	byte *pxy0, byte *pxy1, int *min, int *max,
	int *rmcy, int *rncy, int part);

void BGBBTJ_BC7_EncodeBlockI(byte *block,
	byte *pxy, byte *pxy2, byte *pxa,
	int *min, int *max,
	int *mcy, int *ncy,
	int mca, int nca, int pn)
{
	int dy, du, dv, da, duv;
	
	dy=ncy[0]-mcy[0];
	du=ncy[3]-mcy[3];
	dv=ncy[4]-mcy[4];
	duv=du+dv;
	da=nca-mca;
	
	if((mca==nca) && (mca==255))
	{
		if(pn>=0)
		{
			if(dy>48)
			{
				BGBBTJ_BC7_EncodeBlock_Mode1(block, pxy2, pxy2,
					min+4, max+4, mcy+1, ncy+1, pn);
				return;
			}

			BGBBTJ_BC7_EncodeBlock_Mode3(block, pxy2, pxy2,
				min+4, max+4, mcy+1, ncy+1, pn);
			return;
		}
	
		BGBBTJ_BC7_EncodeBlock_Mode6(block, pxy, min, max, mcy[0], ncy[0]);
		return;
	}

	if((pn>=0) && (duv>64) && (dy<duv))
	{
		BGBBTJ_BC7_EncodeBlock_Mode7(block, pxy2, pxy2,
			min+4, max+4, mcy+1, ncy+1, pn);
		return;
	}

	if(da<32)
	{
		BGBBTJ_BC7_EncodeBlock_Mode6(block, pxy, min, max, mcy[0], ncy[0]);
		return;
	}
	
	if(dy>64)
	{
		BGBBTJ_BC7_EncodeBlock_Mode4(block, pxy, pxa,
			min, max, mcy[0], ncy[0], mca, nca);
		return;
	}

	if(1)
	{
		BGBBTJ_BC7_EncodeBlock_Mode5(block, pxy, pxa,
			min, max, mcy[0], ncy[0], mca, nca);
		return;
	}
}

void BGBBTJ_BC7_EncodeBlock(byte *block,
	byte *rgba, int xstride, int ystride, int pfb)
{
	byte pxy[16], pxy2[16], pxa[16];
	int min[16], max[16];
	int mcy[8], ncy[8];
	int dy, du, dv, da, duv, pn;
	int mca, nca;
	
	BGBBTJ_BC7_EncodeBlock_SplitMinMaxClrYA2(
		pxy, pxy2, pxa, min, max, mcy, ncy, &mca, &nca, &pn,
		rgba, xstride, ystride, pfb);
	
	BGBBTJ_BC7_EncodeBlockI(block, pxy, pxy2, pxa, min, max,
		mcy, ncy, mca, nca, pn);
}

void BGBBTJ_BC7_EncodeBlockYuva4204(byte *block,
	byte *ybuf, byte *ubuf, byte *vbuf, byte *abuf,
	int xystride, int yystride,
	int xustride, int yustride, int pfb)
{
	byte pxy[16], pxy2[16], pxa[16];
	int min[16], max[16];
	int mcy[8], ncy[8];
	int dy, du, dv, da, duv, pn;
	int mca, nca;
	
	BGBBTJ_BC7_EncodeBlock_SplitMinMaxClrYA2_Yuva4204(
		pxy, pxy2, pxa, min, max, mcy, ncy, &mca, &nca, &pn,
		ybuf, ubuf, vbuf, abuf,
		xystride, yystride,
		xustride, yustride, pfb);
	
	BGBBTJ_BC7_EncodeBlockI(block, pxy, pxy2, pxa, min, max,
		mcy, ncy, mca, nca, pn);
}

void BGBBTJ_BC7_EncodeBlockBest(byte *block,
	byte *rgba, int xstride, int ystride, int pfb)
{
	BGBBTJ_BC7_EncodeBlock(block, rgba, xstride, ystride, pfb);
}

void BGBBTJ_BC7_EncodeBlockEdge(byte *block,
	byte *rgba, int xstride, int ystride,
	int xfrac, int yfrac, int pfb)
{
	byte tblk[16*4];
	byte cr, cg, cb, ca;
	byte *cs, *ct;
	int i, j, xn;

	xn=xfrac*xstride;
	for(i=0; i<yfrac; i++)
	{
		cs=rgba+i*ystride;
		ct=tblk+i*4*4;
		if(xstride==4)
		{
			for(j=0; j<xfrac; j++)
			{
				cr=*cs++; cg=*cs++;
				cb=*cs++; ca=*cs++;
				*ct++=cr; *ct++=cg;
				*ct++=cb; *ct++=ca;
			}
			for(; j<4; j++)
			{
				*ct++=cr; *ct++=cg;
				*ct++=cb; *ct++=ca;
			}
			continue;
		}
		if(xstride==3)
		{
			ca=255;
			for(j=0; j<xfrac; j++)
			{
				cr=*cs++; cg=*cs++; cb=*cs++; 
				*ct++=cr; *ct++=cg;
				*ct++=cb; *ct++=ca;
			}
			for(; j<4; j++)
			{
				*ct++=cr; *ct++=cg;
				*ct++=cb; *ct++=ca;
			}
			continue;
		}
		for(j=0; j<xn; j++)
			*ct++=*cs++;
	}

	for(; i<4; i++)
	{
		ct=tblk+i*4*4;
		for(j=0; j<4; j++)
		{
			*ct++=cr; *ct++=cg;
			*ct++=cb; *ct++=ca;
		}
	}
	
	BGBBTJ_BC7_EncodeBlockBest(block, tblk, 4, 4*4, pfb);
}

void BGBBTJ_BC7_EncodeBlockEdgeYuva4204(byte *block,
//	byte *rgba, int xstride, int ystride,
	byte *ybuf, byte *ubuf, byte *vbuf, byte *abuf,
	int xystride, int yystride, int xustride, int yustride,
	int xfrac, int yfrac, int pfb)
{
	byte tyblk[16];
	byte tablk[16];
	byte tublk[4];
	byte tvblk[4];
	byte cr, cg, cb, ca;
	byte *csy, *cty;
	byte *csu, *ctu;
	byte *csv, *ctv;
	byte *csa, *cta;
	int i, j, xn;

	memset(tyblk, ybuf[0], 16);
	memset(tublk, ybuf[0], 4);
	memset(tvblk, vbuf[0], 4);

	if(abuf)
		{ memset(tablk, abuf[0], 16); }
	else
		{ memset(tablk, 255, 16); }

#if 0
	xn=xfrac*xstride;
	for(i=0; i<yfrac; i++)
	{
		cs=rgba+i*ystride;
		ct=tblk+i*4*4;

		for(j=0; j<xn; j++)
			*ct++=*cs++;
	}

	for(; i<4; i++)
	{
		ct=tblk+i*4*4;
		for(j=0; j<4; j++)
		{
			*ct++=cr; *ct++=cg;
			*ct++=cb; *ct++=ca;
		}
	}
#endif
	
	BGBBTJ_BC7_EncodeBlockYuva4204(block,
		tyblk, tublk, tvblk, tablk, 1, 4, 1, 2, pfb);
}

#if 0
// void BGBBTJ_BC7_EncodeImageBC7(byte *block,
	byte *rgba, int xs, int ys, int stride, int pfb)
{
	int xs1, ys1, xs2, ys2;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	xs2=(xs+3)>>2; ys2=(ys+3)>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BC7_EncodeBlock(
			block+(i*xs2+j)*16,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride, pfb);
	}
}
#endif

void BGBBTJ_BC7_EncodeImageBestBC7(byte *block,
	byte *rgba, int xs, int ys, int stride, int pfb)
{
	int stat[16];
	byte *rgba2, *rgba3;
	int xstr, ystr;
	int xs1, ys1, xs2, ys2, xf, yf;
	int i, j, k;
	
	BGBBTJ_BC7_PartitionInit();
	
	for(i=0; i<16; i++)
		stat[i]=0;
	
	rgba2=rgba; xstr=stride; ystr=xs*stride;
	if(ys<0)
	{
		ys=-ys; ystr=-xs*stride;
		rgba2=rgba+((ys-1)*xs*stride);
	}
	
	xs1=xs>>2; ys1=ys>>2;
	xs2=(xs+3)>>2; ys2=(ys+3)>>2;
	xf=xs&3; yf=ys&3;
	for(i=0; i<ys1; i++)
	{
		rgba3=rgba2+(i*4*ystr);

		for(j=0; j<xs1; j++)
		{
			BGBBTJ_BC7_EncodeBlockBest(
				block+(i*xs2+j)*16,
				rgba3+(j*4*xstr),
				stride, ystr, pfb);
		
//			k=BGBBTJ_BC7_GetBlockMode(block+(i*xs2+j)*16);
//			stat[k]++;
		}

		if(xf)
		{
			BGBBTJ_BC7_EncodeBlockEdge(
				block+(i*xs2+j)*16,
				rgba3+(j*4*xstr),
				xstr, ystr, xf, 4, pfb);
//			k=BGBBTJ_BC7_GetBlockMode(block+(i*xs2+j)*16);
//			stat[k]++;
		}
	}

	if(yf)
	{
		rgba3=rgba2+(i*4*ystr);
		for(j=0; j<xs1; j++)
		{
			BGBBTJ_BC7_EncodeBlockEdge(
				block+(i*xs2+j)*16,
				rgba3+(j*4*xstr),
				xstr, ystr, 4, yf, pfb);
		}
		if(xf)
		{
			BGBBTJ_BC7_EncodeBlockEdge(
				block+(i*xs2+j)*16,
				rgba3+(j*4*xstr),
				xstr, ystr, xf, yf, pfb);
		}
	}

//	printf("BGBBTJ_BC7_EncodeImageBestBC7: "
//			"Stats=(0:%d 1:%d 2:%d 3:%d  4:%d 5:%d 6:%d 7:%d  8:%d)\n",
//		stat[0], stat[1], stat[2], stat[3],
//		stat[4], stat[5], stat[6], stat[7], stat[8]);
}

void BGBBTJ_BC7_StatImageBC7(byte *block,
	int xs, int ys)
{
	int stat[16];
	int xs1, ys1, xs2, ys2, xf, yf;
	int i, j, k, n;
	
	for(i=0; i<16; i++)
		stat[i]=0;
	

	xs1=xs>>2; ys1=ys>>2;
	xs2=(xs+3)>>2; ys2=(ys+3)>>2;
	xf=xs&3; yf=ys&3;
	n=xs2*ys2;
	for(i=0; i<n; i++)
	{
		k=BGBBTJ_BC7_GetBlockMode(block+i*16);
		stat[k]++;
	}

	printf("BGBBTJ_BC7_EncodeImageBestBC7: "
			"Stats=(0:%d 1:%d 2:%d 3:%d  4:%d 5:%d 6:%d 7:%d  8:%d)\n",
		stat[0], stat[1], stat[2], stat[3],
		stat[4], stat[5], stat[6], stat[7], stat[8]);
}

#if 1
void BGBBTJ_BC7_EncodeImageYuvaBC7(byte *block,
	byte *ybuf,	byte *ubuf,	byte *vbuf,	byte *abuf,
	int xs, int ys, int xystr, int xustr, int pfb)
{
	byte *ybuf2, *ubuf2, *vbuf2, *abuf2;
	byte *ybuf3, *ubuf3, *vbuf3, *abuf3;
	int xstr, xastr, yystr, yustr, yastr;
	int xs1, ys1, xs2, ys2, xf, yf;
	int i, j, k;
	
	BGBBTJ_BC7_PartitionInit();
	
	ybuf2=ybuf;	ubuf2=ubuf;
	vbuf2=vbuf;	abuf2=abuf;
	yystr=xs*xystr; yustr=((xs+1)>>1)*xustr;
	xastr=abuf?xystr:0;
	yastr=abuf?yystr:0;

	if(ys<0)
	{
		ys=-ys;
		ybuf2=ybuf+((ys-1)*yystr);
		abuf2=abuf+((ys-1)*yastr);
		ubuf2=ubuf+(((ys-1)>>1)*yustr);
		vbuf2=vbuf+(((ys-1)>>1)*yustr);

		yystr=-xs*xystr;
		yustr=-((xs+1)>>1)*xustr;
		xastr=abuf?xystr:0;
		yastr=abuf?yystr:0;
	}
		
	xs1=xs>>2; ys1=ys>>2;
	xs2=(xs+3)>>2; ys2=(ys+3)>>2;
	xf=xs&3; yf=ys&3;
	for(i=0; i<ys1; i++)
	{
		ybuf3=ybuf2+(i*4*yystr);
		abuf3=abuf2+(i*4*yastr);
		ubuf3=ubuf2+(i*2*yustr);
		vbuf3=vbuf2+(i*2*yustr);

		for(j=0; j<xs1; j++)
		{
			BGBBTJ_BC7_EncodeBlockYuva4204(
				block+(i*xs2+j)*16,
				ybuf3+(j*4*xystr),	ubuf3+(j*2*xustr),
				vbuf3+(j*2*xustr),	abuf3+(j*4*xastr),
				xystr, yystr, xustr, yustr,
				pfb);
		}

		if(xf)
		{
			BGBBTJ_BC7_EncodeBlockEdgeYuva4204(
				block+(i*xs2+j)*16,
				ybuf3+(j*4*xystr),	ubuf3+(j*2*xustr),
				vbuf3+(j*2*xustr),	abuf3+(j*4*xastr),
				xystr, yystr, xustr, yustr,
				xf, 4, pfb);
		}
	}

	if(yf)
	{
		ybuf3=ybuf2+(i*4*yystr);
		abuf3=abuf2+(i*4*yastr);
		ubuf3=ubuf2+(i*2*yustr);
		vbuf3=vbuf2+(i*2*yustr);

		for(j=0; j<xs1; j++)
		{
			BGBBTJ_BC7_EncodeBlockEdgeYuva4204(
				block+(i*xs2+j)*16,
				ybuf3+(j*4*xystr),	ubuf3+(j*2*xustr),
				vbuf3+(j*2*xustr),	abuf3+(j*4*xastr),
				xystr, yystr, xustr, yustr,
				4, yf, pfb);
		}
		if(xf)
		{
			BGBBTJ_BC7_EncodeBlockEdgeYuva4204(
				block+(i*xs2+j)*16,
				ybuf3+(j*4*xystr),	ubuf3+(j*2*xustr),
				vbuf3+(j*2*xustr),	abuf3+(j*4*xastr),
				xystr, yystr, xustr, yustr,
				xf, yf, pfb);
		}
	}
}

void BGBBTJ_BC7_EncodeImagePlaneMipHalfSample(
	byte *oybuf, byte *iybuf, int xs, int ys)
{
	byte *ct, *cs;
	int xs1, ys1;
	int p0, p1, p2, p3;
	int i, j, k, l;
	
	xs1=(xs+1)>>1;
	ys1=(ys+1)>>1;
	ct=oybuf; cs=iybuf;
	for(i=0; i<ys1; i++)
	{
		ct=oybuf+i*xs1; cs=iybuf+(i*2*xs);
		for(j=0; j<xs1; j++)
		{
			p0=cs[0];		p1=cs[1];
			p2=cs[xs+0];	p3=cs[xs+1];
			k=(p0+p1+p2+p3)>>2;
			cs+=2;	*ct++=k;
		}
	}
}

void BGBBTJ_BC7_EncodeImageMipYuvaBC7(byte *block,
	byte *ybuf,	byte *ubuf,	byte *vbuf,	byte *abuf,
	int xs, int ys, int xystr, int xustr, int pfb)
{
	byte *tybuf, *tubuf, *tvbuf, *tabuf;
	byte *ct;
	int xs1, ys1, xs2, ys2, xs3, ys3, vf;
	
	xs1=xs; ys1=ys; vf=0;
	if(xs1<0)xs1=-xs;
	if(ys1<0)
		{ ys1=-ys; vf=1; }
	
	tybuf=malloc(xs1*ys1*4);
	tubuf=tybuf+xs1*ys1;
	tvbuf=tubuf+xs1*ys1;
	tabuf=tvbuf+xs1*ys1;
	if(!abuf)tabuf=NULL;

//	xs1=xs;	ys1=ys;
	xs2=(xs1+3)>>2;	ys2=(ys1+3)>>2;
	xs3=(xs1+1)>>1;	ys3=(ys1+1)>>1;
	ct=block;
	BGBBTJ_BC7_EncodeImageYuvaBC7(ct, ybuf, ubuf, vbuf, abuf,
		xs1, vf?(-ys1):ys1, xystr, xustr, pfb);
	ct+=xs2*ys2*16;

	BGBBTJ_BC7_EncodeImagePlaneMipHalfSample(tybuf, ybuf, xs1, ys1);
	if(abuf)BGBBTJ_BC7_EncodeImagePlaneMipHalfSample(tabuf, abuf, xs1, ys1);
	BGBBTJ_BC7_EncodeImagePlaneMipHalfSample(tubuf, ubuf, xs3, ys3);
	BGBBTJ_BC7_EncodeImagePlaneMipHalfSample(tvbuf, vbuf, xs3, ys3);
	xs1=xs3; ys1=ys3;
	
	while((xs1>1) || (ys1>1))
	{
		xs2=(xs1+3)>>2;	ys2=(ys1+3)>>2;
		xs3=(xs1+1)>>1;	ys3=(ys1+1)>>1;
		BGBBTJ_BC7_EncodeImageYuvaBC7(ct, tybuf, tubuf, tvbuf, tabuf,
			xs1, vf?(-ys1):ys1, xystr, xustr, pfb);
		ct+=xs2*ys2*16;
		BGBBTJ_BC7_EncodeImagePlaneMipHalfSample(tybuf, tybuf, xs1, ys1);
		if(abuf)
			BGBBTJ_BC7_EncodeImagePlaneMipHalfSample(tabuf, tabuf, xs1, ys1);
		BGBBTJ_BC7_EncodeImagePlaneMipHalfSample(tubuf, tubuf, xs3, ys3);
		BGBBTJ_BC7_EncodeImagePlaneMipHalfSample(tvbuf, tvbuf, xs3, ys3);
		xs1=xs3; ys1=ys3;
	}
	
	free(tybuf);
}

#endif
