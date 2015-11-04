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

void BGBBTJ_BC7_EncodeBlock_VecInvertPixels_Part2A(
	byte *pxv, int n, int min, int max, int part)
{
	byte *tp;
	int i, j, k;

	tp=bgbbtj_bc7_partition2+(part*16);
	
	for(i=0; i<n; i++)
	{
		if(tp[i]==0)
			{ pxv[i]=max-(pxv[i]-min); }
	}
}

void BGBBTJ_BC7_EncodeBlock_VecInvertPixels_Part2B(
	byte *pxv, int n, int min, int max, int part)
{
	byte *tp;
	int i, j, k;

	tp=bgbbtj_bc7_partition2+(part*16);
	
	for(i=0; i<n; i++)
	{
		if(tp[i]==1)
			{ pxv[i]=max-(pxv[i]-min); }
	}
}

void BGBBTJ_BC7_EncodeBlock_Mode1(byte *block,
	byte *pxy0, byte *pxy1,
	int *min, int *max,
	int *rmcy, int *rncy,
	int part)
{
	static const char idxtab[16]=
		{ 0,0,0,0, 0,0,1,1, 2,2,3,3, 3,3,3,3 };
	static const char idxtab2[16]=
		{ 0,0,0,0, 0,1,2,3, 4,5,6,7, 7,7,7,7 };
	
	BGBBTJ_BitStream bits;

	int p0, p1, p2, p3, p4, p5, p6, p7;
	int l0, l1, l2, l3a, l3b;

	int min0[4], mcy0, mca0;
	int max0[4], ncy0, nca0;
	int avg0[4], acy0, aca0;
	int min1[4], mcy1, mca1;
	int max1[4], ncy1, nca1;
	int avg1[4], acy1, aca1;

	int acr, acg, acb, ank;
	int cr, cg, cb, ca, cy;
	int i, j, k, l;
	byte *tp;

	tp=bgbbtj_bc7_partition2+(part*16);	

	min0[0]=min[0];	min0[1]=min[1];
	min0[2]=min[2];	min0[3]=min[3];
	min1[0]=min[4];	min1[1]=min[5];
	min1[2]=min[6];	min1[3]=min[7];

	max0[0]=max[0];	max0[1]=max[1];
	max0[2]=max[2];	max0[3]=max[3];
	max1[0]=max[4];	max1[1]=max[5];
	max1[2]=max[6];	max1[3]=max[7];

	mcy0=rmcy[0];	mcy1=rmcy[1];
	ncy0=rncy[0];	ncy1=rncy[1];
	acy0=(mcy0+ncy0)>>1;
	acy1=(mcy1+ncy1)>>1;

	BGBBTJ_BitsLE_ClearSetupWrite(&bits, block, 16);
	BGBBTJ_BitsLE_WriteBits(&bits, 2, 2);
	BGBBTJ_BitsLE_WriteBits(&bits, part, 6);

	if(pxy0[0]>=acy0)
	{
		BGBBTJ_BC7_EncodeBlock_VecSwapRGBA(min0, max0);
		BGBBTJ_BC7_EncodeBlock_VecInvertPixels_Part2A(
			pxy0, 16, mcy0, ncy0, part);
	}

	i=bgbbtj_bc7_anchor2[part];
	if(pxy1[i]>=acy1)
	{
		BGBBTJ_BC7_EncodeBlock_VecSwapRGBA(min1, max1);
		BGBBTJ_BC7_EncodeBlock_VecInvertPixels_Part2B(
			pxy1, 16, mcy1, ncy1, part);
	}

	BGBBTJ_BitsLE_WriteBits(&bits, (min0[0])>>2, 6);
	BGBBTJ_BitsLE_WriteBits(&bits, (max0[0])>>2, 6);
	BGBBTJ_BitsLE_WriteBits(&bits, (min1[0])>>2, 6);
	BGBBTJ_BitsLE_WriteBits(&bits, (max1[0])>>2, 6);

	BGBBTJ_BitsLE_WriteBits(&bits, (min0[1])>>2, 6);
	BGBBTJ_BitsLE_WriteBits(&bits, (max0[1])>>2, 6);
	BGBBTJ_BitsLE_WriteBits(&bits, (min1[1])>>2, 6);
	BGBBTJ_BitsLE_WriteBits(&bits, (max1[1])>>2, 6);

	BGBBTJ_BitsLE_WriteBits(&bits, (min0[2])>>2, 6);
	BGBBTJ_BitsLE_WriteBits(&bits, (max0[2])>>2, 6);
	BGBBTJ_BitsLE_WriteBits(&bits, (min1[2])>>2, 6);
	BGBBTJ_BitsLE_WriteBits(&bits, (max1[2])>>2, 6);

	p0=(min0[0]&min0[1])|(min0[1]&min0[2])|(min0[0]&min0[2]);
	p1=(max0[0]&max0[1])|(max0[1]&max0[2])|(max0[0]&max0[2]);
	p2=(min1[0]&min1[1])|(min1[1]&min1[2])|(min1[0]&min1[2]);
	p3=(max1[0]&max1[1])|(max1[1]&max1[2])|(max1[0]&max1[2]);
	p0=p0&p2;	p1=p1&p3;
	BGBBTJ_BitsLE_WriteBit(&bits, p0>>1);
	BGBBTJ_BitsLE_WriteBit(&bits, p1>>1);

	l0=32768/(ncy0-acy0+1);		//Fix-Point Scale (Luma 0)
	l1=32768/(ncy1-acy1+1);		//Fix-Point Scale (Luma 1)
	l2=65536-2048;
	l3a=65536;
	ank=bgbbtj_bc7_anchor2[part];

	p0=idxtab2[((pxy0[0]-acy0)*l1+l3a)>>13];
	if(p0>3)p0=3;
	BGBBTJ_BitsLE_Write2Bits(&bits, p0);
	for(i=1; i<16; i++)
	{
		if(tp[i]==1)
		{
			if(i==ank)
			{
				p0=idxtab2[((pxy1[i]-acy1)*l1+l3a)>>13];
				if(p0>3)p0=3;
				BGBBTJ_BitsLE_Write2Bits(&bits, p0);
			}else
			{
				p0=idxtab2[((pxy1[i]-acy1)*l1+l3a)>>13];
				BGBBTJ_BitsLE_Write3Bits(&bits, p0);
			}
		}else
		{
			p0=idxtab2[((pxy0[i]-acy0)*l0+l3a)>>13];
			BGBBTJ_BitsLE_Write3Bits(&bits, p0);
		}
	}

	BGBBTJ_BitsLE_FlushBits(&bits);
}

void BGBBTJ_BC7_EncodeBlock_Mode3(byte *block,
	byte *pxy0, byte *pxy1,
	int *min, int *max,
	int *rmcy, int *rncy,
	int part)
{
	static const char idxtab[16]=
		{ 0,0,0,0, 0,0,1,1, 2,2,3,3, 3,3,3,3 };
	static const char idxtab2[16]=
		{ 0,0,0,0, 0,1,2,3, 4,5,6,7, 7,7,7,7 };
	
	BGBBTJ_BitStream bits;

	int min0[4], max0[4];
	int min1[4], max1[4];

	int p0, p1, p2, p3, p4, p5, p6, p7;
	int l0, l1, l2, l3a, l3b;
	int mcy0, ncy0;
	int mcy1, ncy1;
	int acy0, acy1;

	int ank;
	int i, j, k, l;
	byte *tp;

	min0[0]=min[0];	min0[1]=min[1];
	min0[2]=min[2];	min0[3]=min[3];
	min1[0]=min[4];	min1[1]=min[5];
	min1[2]=min[6];	min1[3]=min[7];

	max0[0]=max[0];	max0[1]=max[1];
	max0[2]=max[2];	max0[3]=max[3];
	max1[0]=max[4];	max1[1]=max[5];
	max1[2]=max[6];	max1[3]=max[7];

	mcy0=rmcy[0];	mcy1=rmcy[1];
	ncy0=rncy[0];	ncy1=rncy[1];
	acy0=(mcy0+ncy0)>>1;
	acy1=(mcy1+ncy1)>>1;

	BGBBTJ_BitsLE_ClearSetupWrite(&bits, block, 16);
	BGBBTJ_BitsLE_WriteBits(&bits, 8, 4);
	BGBBTJ_BitsLE_WriteBits(&bits, part, 6);

#if 1
	if(pxy0[0]>=acy0)
	{
		BGBBTJ_BC7_EncodeBlock_VecSwapRGBA(min0, max0);
		BGBBTJ_BC7_EncodeBlock_VecInvertPixels_Part2A(
			pxy0, 16, mcy0, ncy0, part);
	}

	i=bgbbtj_bc7_anchor2[part];
	if(pxy1[i]>=acy1)
	{
		BGBBTJ_BC7_EncodeBlock_VecSwapRGBA(min1, max1);
		BGBBTJ_BC7_EncodeBlock_VecInvertPixels_Part2B(
			pxy1, 16, mcy1, ncy1, part);
	}
#endif

	BGBBTJ_BitsLE_WriteBits(&bits, (min0[0])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (max0[0])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (min1[0])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (max1[0])>>1, 7);

	BGBBTJ_BitsLE_WriteBits(&bits, (min0[1])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (max0[1])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (min1[1])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (max1[1])>>1, 7);

	BGBBTJ_BitsLE_WriteBits(&bits, (min0[2])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (max0[2])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (min1[2])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (max1[2])>>1, 7);

	p0=(min0[0]&min0[1])|(min0[1]&min0[2])|(min0[0]&min0[2]);
	p1=(max0[0]&max0[1])|(max0[1]&max0[2])|(max0[0]&max0[2]);
	p2=(min1[0]&min1[1])|(min1[1]&min1[2])|(min1[0]&min1[2]);
	p3=(max1[0]&max1[1])|(max1[1]&max1[2])|(max1[0]&max1[2]);
	BGBBTJ_BitsLE_WriteBit(&bits, p0);
	BGBBTJ_BitsLE_WriteBit(&bits, p1);
	BGBBTJ_BitsLE_WriteBit(&bits, p2);
	BGBBTJ_BitsLE_WriteBit(&bits, p3);

	l0=32768/(ncy0-acy0+1);		//Fix-Point Scale (Luma 0)
	l1=32768/(ncy1-acy1+1);		//Fix-Point Scale (Luma 1)
	l2=65536-2048;
	l3a=65536;

	tp=bgbbtj_bc7_partition2+(part*16);
	ank=bgbbtj_bc7_anchor2[part];

	p0=idxtab[((pxy0[0]-acy0)*l1+l3a)>>13];
	if(p0>1)p0=1;
	BGBBTJ_BitsLE_WriteBit(&bits, p0);
	for(i=1; i<16; i++)
	{
		if(tp[i]==1)
		{
			if(i==ank)
			{
				p0=idxtab[((pxy1[i]-acy1)*l1+l3a)>>13];
				if(p0>1)p0=1;
				BGBBTJ_BitsLE_WriteBit(&bits, p0);
			}else
			{
				p0=idxtab[((pxy1[i]-acy1)*l1+l3a)>>13];
				BGBBTJ_BitsLE_Write2Bits(&bits, p0);
			}
		}else
		{
			p0=idxtab[((pxy0[i]-acy0)*l0+l3a)>>13];
			BGBBTJ_BitsLE_Write2Bits(&bits, p0);
		}
	}

	BGBBTJ_BitsLE_FlushBits(&bits);
}

void BGBBTJ_BC7_EncodeBlock_Mode7(byte *block,
	byte *pxy0, byte *pxy1,
	int *min, int *max,
	int *rmcy, int *rncy,
	int part)
{
	static const char idxtab[16]=
		{ 0,0,0,0, 0,0,1,1, 2,2,3,3, 3,3,3,3 };
	static const char idxtab2[16]=
		{ 0,0,0,0, 0,1,2,3, 4,5,6,7, 7,7,7,7 };
	
	BGBBTJ_BitStream bits;

	byte pxy[16], pxa[16];
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int l0, l1, l2, l3a, l3b;

	int min0[4], mcy0, mca0;
	int max0[4], ncy0, nca0;
	int avg0[4], acy0, aca0;
	int min1[4], mcy1, mca1;
	int max1[4], ncy1, nca1;
	int avg1[4], acy1, aca1;

	int acr, acg, acb, ank;
	int cr, cg, cb, ca, cy;
	int i, j, k, l;
	byte *tp;

	tp=bgbbtj_bc7_partition2+(part*16);	

	min0[0]=min[0];	min0[1]=min[1];
	min0[2]=min[2];	min0[3]=min[3];
	min1[0]=min[4];	min1[1]=min[5];
	min1[2]=min[6];	min1[3]=min[7];

	max0[0]=max[0];	max0[1]=max[1];
	max0[2]=max[2];	max0[3]=max[3];
	max1[0]=max[4];	max1[1]=max[5];
	max1[2]=max[6];	max1[3]=max[7];

	mcy0=rmcy[0];	mcy1=rmcy[1];
	ncy0=rncy[0];	ncy1=rncy[1];
	acy0=(mcy0+ncy0)>>1;
	acy1=(mcy1+ncy1)>>1;

	BGBBTJ_BitsLE_ClearSetupWrite(&bits, block, 16);
//	BGBBTJ_BitsLE_WriteBits(&bits, 64, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, 128, 8);
	BGBBTJ_BitsLE_WriteBits(&bits, part, 6);

	if(pxy0[0]>=acy0)
	{
		BGBBTJ_BC7_EncodeBlock_VecSwapRGBA(min0, max0);
		BGBBTJ_BC7_EncodeBlock_VecInvertPixels_Part2A(
			pxy0, 16, mcy0, ncy0, part);
	}

	i=bgbbtj_bc7_anchor2[part];
	if(pxy1[i]>=acy1)
	{
		BGBBTJ_BC7_EncodeBlock_VecSwapRGBA(min1, max1);
		BGBBTJ_BC7_EncodeBlock_VecInvertPixels_Part2B(
			pxy1, 16, mcy1, ncy1, part);
	}

	BGBBTJ_BitsLE_WriteBits(&bits, (min0[0])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (max0[0])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (min1[0])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (max1[0])>>3, 5);

	BGBBTJ_BitsLE_WriteBits(&bits, (min0[1])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (max0[1])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (min1[1])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (max1[1])>>3, 5);

	BGBBTJ_BitsLE_WriteBits(&bits, (min0[2])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (max0[2])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (min1[2])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (max1[2])>>3, 5);

	BGBBTJ_BitsLE_WriteBits(&bits, (min0[3])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (max0[3])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (min1[3])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (max1[3])>>3, 5);

	p0=(min0[0]&min0[1])|(min0[1]&min0[2])|(min0[0]&min0[2]);
	p1=(max0[0]&max0[1])|(max0[1]&max0[2])|(max0[0]&max0[2]);
	p2=(min1[0]&min1[1])|(min1[1]&min1[2])|(min1[0]&min1[2]);
	p3=(max1[0]&max1[1])|(max1[1]&max1[2])|(max1[0]&max1[2]);
	BGBBTJ_BitsLE_WriteBit(&bits, p0>>2);
	BGBBTJ_BitsLE_WriteBit(&bits, p1>>2);
	BGBBTJ_BitsLE_WriteBit(&bits, p2>>2);
	BGBBTJ_BitsLE_WriteBit(&bits, p3>>2);

	l0=32768/(ncy0-acy0+1);		//Fix-Point Scale (Luma 0)
	l1=32768/(ncy1-acy1+1);		//Fix-Point Scale (Luma 1)
	l2=65536-2048;
	l3a=65536;
	ank=bgbbtj_bc7_anchor2[part];

	p0=idxtab[((pxy[0]-acy0)*l1+l3a)>>13];
	BGBBTJ_BitsLE_WriteBit(&bits, p0);
	for(i=1; i<16; i++)
	{
		if(tp[i]==1)
		{
			if(i==ank)
			{
				p0=idxtab[((pxy1[i]-acy1)*l1+l3a)>>13];
				BGBBTJ_BitsLE_WriteBit(&bits, p0);
			}else
			{
				p0=idxtab[((pxy1[i]-acy1)*l1+l3a)>>13];
				BGBBTJ_BitsLE_Write2Bits(&bits, p0);
			}
		}else
		{
			p0=idxtab[((pxy0[i]-acy0)*l0+l3a)>>13];
			BGBBTJ_BitsLE_Write2Bits(&bits, p0);
		}
	}

	BGBBTJ_BitsLE_FlushBits(&bits);
}
