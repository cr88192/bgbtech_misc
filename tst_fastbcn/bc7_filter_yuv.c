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

// int clamp255(int i)
//	{ return((i<0)?0:((i>255)?255:i)); }
int clamp15(int i)
	{ return((i<0)?0:((i>15)?15:i)); }
int clamp31(int i)
	{ return((i<0)?0:((i>31)?31:i)); }

static int clamp252(int y)
	{ return((y<8)?8:((y>243)?243:y)); }

int btic1h_subfold(int a, int b)
{
	int c;
	c=a-b;
	c=(c<<1)^(c>>31);
	return(c);
}


void BT1H_EncodeBlockFilt_YUYV(
	byte *pxy,	byte *pxu,	byte *pxv, byte *pxa,
	byte *pmcy,	byte *pncy,	byte *pmca,	byte *pnca,
	byte *yuv, int xstride, int ystride)
{
	int l0, l1, l2, l3;
	int mcy, ncy;
	int cy0, cy1, cy2, cy3;
	int cu0, cv0, cu1, cv1;

	l1=ystride;
	cy0=yuv[0];	cy1=yuv[2];
	cy2=yuv[l1+0];	cy3=yuv[l1+2];
	cu0=yuv[1];	cv0=yuv[3];
	cu1=yuv[l1+1];	cv1=yuv[l1+3];
	pxy[0]=cy0;	pxy[1]=cy1;
	pxy[4]=cy2;	pxy[5]=cy3;		
	mcy=cy0;	ncy=cy0;
	if(cy3<mcy) { mcy=cy3; }
	if(cy3>ncy) { ncy=cy3; }
	if(cy1>ncy) { ncy=cy1; }
	if(cy1<mcy) { mcy=cy1; }
	if(cy2<mcy) { mcy=cy2; }
	if(cy2>ncy) { ncy=cy2; }
	pmcy[0]=mcy;	pncy[0]=ncy;
	pxu[0]=(cu0+cu1)>>1;
	pxv[0]=(cv0+cv1)>>1;

	cy0=yuv[4];	cy1=yuv[6];
	cy2=yuv[l1+4];	cy3=yuv[l1+6];
	cu0=yuv[5];	cv0=yuv[7];
	cu1=yuv[l1+5];	cv1=yuv[l1+7];
	pxy[2]=cy0;	pxy[3]=cy1;
	pxy[6]=cy2;	pxy[7]=cy3;
	mcy=cy0;	ncy=cy0;
	if(cy3<mcy) { mcy=cy3; }
	if(cy3>ncy) { ncy=cy3; }
	if(cy1>ncy) { ncy=cy1; }
	if(cy1<mcy) { mcy=cy1; }
	if(cy2<mcy) { mcy=cy2; }
	if(cy2>ncy) { ncy=cy2; }
	pmcy[1]=mcy;	pncy[1]=ncy;
	pxu[1]=(cu0+cu1)>>1;
	pxv[1]=(cv0+cv1)>>1;

	l0=2*ystride;
	l1=3*ystride;
	cy0=yuv[l0+0];	cy1=yuv[l0+2];
	cy2=yuv[l1+0];	cy3=yuv[l1+2];
	cu0=yuv[l0+1];	cv0=yuv[l0+3];
	cu1=yuv[l1+1];	cv1=yuv[l1+3];
	pxy[ 8]=cy0;	pxy[ 9]=cy1;
	pxy[12]=cy2;	pxy[13]=cy3;
	mcy=cy0;	ncy=cy0;
	if(cy3<mcy) { mcy=cy3; }
	if(cy3>ncy) { ncy=cy3; }
	if(cy1>ncy) { ncy=cy1; }
	if(cy1<mcy) { mcy=cy1; }
	if(cy2<mcy) { mcy=cy2; }
	if(cy2>ncy) { ncy=cy2; }
	pmcy[2]=mcy;	pncy[2]=ncy;
	pxu[2]=(cu0+cu1)>>1;
	pxv[2]=(cv0+cv1)>>1;

	cy0=yuv[l0+4];	cy1=yuv[l0+6];
	cy2=yuv[l1+4];	cy3=yuv[l1+6];
	cu0=yuv[l0+5];	cv0=yuv[l0+7];
	cu1=yuv[l1+5];	cv1=yuv[l1+7];
	pxy[10]=cy0;	pxy[11]=cy1;
	pxy[14]=cy2;	pxy[15]=cy3;
	mcy=cy0;	ncy=cy0;
	if(cy3<mcy) { mcy=cy3; }
	if(cy3>ncy) { ncy=cy3; }
	if(cy1>ncy) { ncy=cy1; }
	if(cy1<mcy) { mcy=cy1; }
	if(cy2<mcy) { mcy=cy2; }
	if(cy2>ncy) { ncy=cy2; }
	pmcy[3]=mcy;	pncy[3]=ncy;
	pxu[3]=(cu0+cu1)>>1;
	pxv[3]=(cv0+cv1)>>1;
	
	((u32 *)pxa)[0]=0xFFFFFFFF;
	((u32 *)pxa)[1]=0xFFFFFFFF;
	((u32 *)pxa)[2]=0xFFFFFFFF;
	((u32 *)pxa)[3]=0xFFFFFFFF;
	((u32 *)pmca)[0]=0xFFFFFFFF;
	((u32 *)pnca)[0]=0xFFFFFFFF;
}

void BT1H_EncodeBlockFilt_Yuva4204(
	byte *pxy,	byte *pxu,	byte *pxv, byte *pxa,
	byte *pmcy,	byte *pncy,	byte *pmca,	byte *pnca,
	byte *ybuf, byte *ubuf, byte *vbuf, byte *abuf,
	int xystride, int yystride,
	int xustride, int yustride, int pfb)
{
	int x0y, x1y, x2y, x3y;
	int y0y, y1y, y2y, y3y;

	int x0u, x1u;
	int y0u, y1u;

	int l0, l1, l2, l3;
	int mcy, ncy;
	int cy0, cy1, cy2, cy3;
	int cu0, cv0, cu1, cv1;

	x0y=0; x1y=xystride; x2y=2*x1y; x3y=3*x1y;
	y0y=0; y1y=yystride; y2y=2*y1y; y3y=3*y1y;

	x0u=0; x1u=xustride;
	y0u=0; y1u=yustride;

	cy0=ybuf[y0y+x0y];	cy1=ybuf[y0y+x1y];
	cy2=ybuf[y1y+x0y];	cy3=ybuf[y1y+x0y];
	cu0=ubuf[x0u];	cv0=vbuf[x0u];
	pxy[0]=cy0;	pxy[1]=cy1;
	pxy[4]=cy2;	pxy[5]=cy3;		
	mcy=cy0;	ncy=cy0;
	if(cy3<mcy) { mcy=cy3; }
	if(cy3>ncy) { ncy=cy3; }
	if(cy1>ncy) { ncy=cy1; }
	if(cy1<mcy) { mcy=cy1; }
	if(cy2<mcy) { mcy=cy2; }
	if(cy2>ncy) { ncy=cy2; }
	pmcy[0]=mcy;	pncy[0]=ncy;
	pxu[0]=cu0;		pxv[0]=cv0;

	cy0=ybuf[y0y+x2y];	cy1=ybuf[y0y+x3y];
	cy2=ybuf[y1y+x2y];	cy3=ybuf[y1y+x3y];
	cu0=ubuf[x1u];	cv0=vbuf[x1u];
	pxy[2]=cy0;	pxy[3]=cy1;
	pxy[6]=cy2;	pxy[7]=cy3;
	mcy=cy0;	ncy=cy0;
	if(cy3<mcy) { mcy=cy3; }
	if(cy3>ncy) { ncy=cy3; }
	if(cy1>ncy) { ncy=cy1; }
	if(cy1<mcy) { mcy=cy1; }
	if(cy2<mcy) { mcy=cy2; }
	if(cy2>ncy) { ncy=cy2; }
	pmcy[1]=mcy;	pncy[1]=ncy;
	pxu[1]=cu0;		pxv[1]=cv0;

	cy0=ybuf[y2y+x0y];	cy1=ybuf[y2y+x1y];
	cy2=ybuf[y3y+x0y];	cy3=ybuf[y3y+x1y];
	cu0=ubuf[y1u];	cv0=vbuf[y1u];
	pxy[ 8]=cy0;	pxy[ 9]=cy1;
	pxy[12]=cy2;	pxy[13]=cy3;
	mcy=cy0;	ncy=cy0;
	if(cy3<mcy) { mcy=cy3; }
	if(cy3>ncy) { ncy=cy3; }
	if(cy1>ncy) { ncy=cy1; }
	if(cy1<mcy) { mcy=cy1; }
	if(cy2<mcy) { mcy=cy2; }
	if(cy2>ncy) { ncy=cy2; }
	pmcy[2]=mcy;	pncy[2]=ncy;
	pxu[2]=cu0;		pxv[2]=cv0;

	cy0=ybuf[y2y+x2y];	cy1=ybuf[y2y+x3y];
	cy2=ybuf[y3y+x2y];	cy3=ybuf[y3y+x3y];
	cu0=ubuf[y1u+x1u];	cv0=vbuf[y1u+x1u];
	pxy[10]=cy0;	pxy[11]=cy1;
	pxy[14]=cy2;	pxy[15]=cy3;
	mcy=cy0;	ncy=cy0;
	if(cy3<mcy) { mcy=cy3; }
	if(cy3>ncy) { ncy=cy3; }
	if(cy1>ncy) { ncy=cy1; }
	if(cy1<mcy) { mcy=cy1; }
	if(cy2<mcy) { mcy=cy2; }
	if(cy2>ncy) { ncy=cy2; }
	pmcy[3]=mcy;	pncy[3]=ncy;
	pxu[3]=cu0;		pxv[3]=cv0;
	
	if(abuf)
	{
		cy0=abuf[y0y+x0y];	cy1=abuf[y0y+x1y];
		cy2=abuf[y1y+x0y];	cy3=abuf[y1y+x0y];
		pxa[0]=cy0;	pxa[1]=cy1;
		pxa[4]=cy2;	pxa[5]=cy3;		
		mcy=cy0;	ncy=cy0;
		if(cy3<mcy) { mcy=cy3; }
		if(cy3>ncy) { ncy=cy3; }
		if(cy1>ncy) { ncy=cy1; }
		if(cy1<mcy) { mcy=cy1; }
		if(cy2<mcy) { mcy=cy2; }
		if(cy2>ncy) { ncy=cy2; }
		pmca[0]=mcy;	pnca[0]=ncy;

		cy0=abuf[y0y+x2y];	cy1=abuf[y0y+x3y];
		cy2=abuf[y1y+x2y];	cy3=abuf[y1y+x3y];
		pxa[2]=cy0;	pxa[3]=cy1;
		pxa[6]=cy2;	pxa[7]=cy3;
		mcy=cy0;	ncy=cy0;
		if(cy3<mcy) { mcy=cy3; }
		if(cy3>ncy) { ncy=cy3; }
		if(cy1>ncy) { ncy=cy1; }
		if(cy1<mcy) { mcy=cy1; }
		if(cy2<mcy) { mcy=cy2; }
		if(cy2>ncy) { ncy=cy2; }
		pmca[1]=mcy;	pnca[1]=ncy;

		cy0=abuf[y2y+x0y];	cy1=abuf[y2y+x1y];
		cy2=abuf[y3y+x0y];	cy3=abuf[y3y+x1y];
		pxa[ 8]=cy0;	pxa[ 9]=cy1;
		pxa[12]=cy2;	pxa[13]=cy3;
		mcy=cy0;	ncy=cy0;
		if(cy3<mcy) { mcy=cy3; }
		if(cy3>ncy) { ncy=cy3; }
		if(cy1>ncy) { ncy=cy1; }
		if(cy1<mcy) { mcy=cy1; }
		if(cy2<mcy) { mcy=cy2; }
		if(cy2>ncy) { ncy=cy2; }
		pmca[2]=mcy;	pnca[2]=ncy;

		cy0=abuf[y2y+x2y];	cy1=abuf[y2y+x3y];
		cy2=abuf[y3y+x2y];	cy3=abuf[y3y+x3y];
		pxa[10]=cy0;	pxa[11]=cy1;
		pxa[14]=cy2;	pxa[15]=cy3;
		mcy=cy0;	ncy=cy0;
		if(cy3<mcy) { mcy=cy3; }
		if(cy3>ncy) { ncy=cy3; }
		if(cy1>ncy) { ncy=cy1; }
		if(cy1<mcy) { mcy=cy1; }
		if(cy2<mcy) { mcy=cy2; }
		if(cy2>ncy) { ncy=cy2; }
		pmca[3]=mcy;	pnca[3]=ncy;
	}else
	{
		((u32 *)pxa)[0]=0xFFFFFFFF;
		((u32 *)pxa)[1]=0xFFFFFFFF;
		((u32 *)pxa)[2]=0xFFFFFFFF;
		((u32 *)pxa)[3]=0xFFFFFFFF;
		((u32 *)pmca)[0]=0xFFFFFFFF;
		((u32 *)pnca)[0]=0xFFFFFFFF;
	}
}

#define BT1H_MIN(x, y) (x<y)?x:y

void BT1H_EncodeBlockFilt_RGBI(
	byte *pxy,	byte *pxu,	byte *pxv, byte *pxa,
	byte *pmcy,	byte *pncy,	byte *pmca,	byte *pnca,
	byte *rgb, int xstride, int ystride, int tflip)
{
	byte *rgb0, *rgb1, *rgb2, *rgb3;
	byte *pxy2, *pxa2;
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int l0, l1, l2, l3;
	int mcy, ncy, mca, nca;
	int cy0, cy1, cy2, cy3;
	int cu0, cu1, cu2, cu3;
	int cv0, cv1, cv2, cv3;	
	int cr0, cr1, cr2, cr3;
	int cg0, cg1, cg2, cg3;
	int cb0, cb1, cb2, cb3;
	int ca0, ca1, ca2, ca3;
	int cy, cu, cv;
	int cr, cg, cb, ca;
	int i, j, k, l;

#if 1
	if((xstride==4) && !(tflip&BC7_PFB_BGR))
	{
		for(i=0; i<2; i++)
			for(j=0; j<2; j++)
		{
			p0=i<<1; p1=p0+1; p2=(j<<3);
			pxy2=pxy+((p0<<2)+(j<<1));
			pxa2=pxa+((p0<<2)+(j<<1));
			rgb0=rgb+p0*ystride+p2;		rgb1=rgb+p1*ystride+p2;
			cr0=rgb0[0]; cg0=rgb0[1]; cb0=rgb0[2]; ca0=rgb0[3];
			cr1=rgb0[4]; cg1=rgb0[5]; cb1=rgb0[6]; ca1=rgb0[7];
			cr2=rgb1[0]; cg2=rgb1[1]; cb2=rgb1[2]; ca2=rgb1[3];
			cr3=rgb1[4]; cg3=rgb1[5]; cb3=rgb1[6]; ca3=rgb1[7];

			cr=(cr0+cr1+cr2+cr3)>>2;
			cg=(cg0+cg1+cg2+cg3)>>2;
			cb=(cb0+cb1+cb2+cb3)>>2;

			cy0=(77*cr0 + 150*cg0 + 29*cb0 + 127)>>8;
			cy1=(77*cr1 + 150*cg1 + 29*cb1 + 127)>>8;
			cy2=(77*cr2 + 150*cg2 + 29*cb2 + 127)>>8;
			cy3=(77*cr3 + 150*cg3 + 29*cb3 + 127)>>8;
			cu=((- 43*cr - 85*cg +128*cb + 127)>>8)+128;
			cv=(( 128*cr -107*cg - 21*cb + 127)>>8)+128;

			pxy2[0]=cy0;	pxy2[1]=cy1;	pxy2[4]=cy2;	pxy2[5]=cy3;
			pxa2[0]=ca0;	pxa2[1]=ca1;	pxa2[4]=ca2;	pxa2[5]=ca3;
			l0=(cy0<cy3)?cy0:cy3;	l1=(cy1<cy2)?cy1:cy2;
			l2=(cy0>cy3)?cy0:cy3;	l3=(cy1>cy2)?cy1:cy2;
			mcy=(l0<l1)?l0:l1;		ncy=(l2>l3)?l2:l3;
			k=i*2+j; pmcy[k]=mcy; pncy[k]=ncy; pxu[k]=cu; pxv[k]=cv;

//			if((ca0&ca1&ca2&ca3)==255)
//				{ pmca[k]=255; pnca[k]=255; continue; }
			l0=(ca0<cy3)?ca0:ca3;	l1=(ca1<ca2)?ca1:ca2;
			l2=(ca0>cy3)?ca0:ca3;	l3=(ca1>ca2)?ca1:ca2;
			mca=(l0<l1)?l0:l1;		nca=(l2>l3)?l2:l3;
			pmca[k]=mca; pnca[k]=nca;
		}
		return;
	}
#endif

#if 1
	if((xstride==4) && (tflip&BC7_PFB_BGR))
	{
		for(i=0; i<2; i++)
			for(j=0; j<2; j++)
		{
			p0=i<<1; p1=p0+1; p2=(j<<3);
			pxy2=pxy+((p0<<2)+(j<<1));
			pxa2=pxa+((p0<<2)+(j<<1));
			rgb0=rgb+p0*ystride+p2;		rgb1=rgb+p1*ystride+p2;
			cr0=rgb0[2]; cg0=rgb0[1]; cb0=rgb0[0]; ca0=rgb0[3];
			cr1=rgb0[6]; cg1=rgb0[5]; cb1=rgb0[4]; ca1=rgb0[7];
			cr2=rgb1[2]; cg2=rgb1[1]; cb2=rgb1[0]; ca2=rgb1[3];
			cr3=rgb1[6]; cg3=rgb1[5]; cb3=rgb1[4]; ca3=rgb1[7];

			cr=(cr0+cr1+cr2+cr3)>>2;
			cg=(cg0+cg1+cg2+cg3)>>2;
			cb=(cb0+cb1+cb2+cb3)>>2;

			cy0=(77*cr0 + 150*cg0 + 29*cb0 + 127)>>8;
			cy1=(77*cr1 + 150*cg1 + 29*cb1 + 127)>>8;
			cy2=(77*cr2 + 150*cg2 + 29*cb2 + 127)>>8;
			cy3=(77*cr3 + 150*cg3 + 29*cb3 + 127)>>8;
			cu=((- 43*cr - 85*cg +128*cb + 127)>>8)+128;
			cv=(( 128*cr -107*cg - 21*cb + 127)>>8)+128;

			pxy2[0]=cy0;	pxy2[1]=cy1;	pxy2[4]=cy2;	pxy2[5]=cy3;
			pxa2[0]=ca0;	pxa2[1]=ca1;	pxa2[4]=ca2;	pxa2[5]=ca3;
			l0=(cy0<cy3)?cy0:cy3;	l1=(cy1<cy2)?cy1:cy2;
			l2=(cy0>cy3)?cy0:cy3;	l3=(cy1>cy2)?cy1:cy2;
			mcy=(l0<l1)?l0:l1;		ncy=(l2>l3)?l2:l3;
			k=i*2+j; pmcy[k]=mcy; pncy[k]=ncy; pxu[k]=cu; pxv[k]=cv;

			l0=(ca0<cy3)?ca0:ca3;	l1=(ca1<ca2)?ca1:ca2;
			l2=(ca0>cy3)?ca0:ca3;	l3=(ca1>ca2)?ca1:ca2;
			mca=(l0<l1)?l0:l1;		nca=(l2>l3)?l2:l3;
			pmca[k]=mca; pnca[k]=nca;
		}
		return;
	}
#endif

	for(i=0; i<2; i++)
		for(j=0; j<2; j++)
	{
		if(xstride==4)
		{
			p0=(i<<1)+0; p1=(i<<1)+1; p2=j*8;
			rgb0=rgb+p0*ystride+p2;
			rgb1=rgb+p1*ystride+p2;

			if(tflip&BC7_PFB_BGR)
			{
				cr0=rgb0[2]; cg0=rgb0[1]; cb0=rgb0[0]; ca0=rgb0[3];
				cr1=rgb0[6]; cg1=rgb0[5]; cb1=rgb0[4]; ca1=rgb0[7];
				cr2=rgb1[2]; cg2=rgb1[1]; cb2=rgb1[0]; ca2=rgb1[3];
				cr3=rgb1[6]; cg3=rgb1[5]; cb3=rgb1[4]; ca3=rgb1[7];
			}else
			{
				cr0=rgb0[0]; cg0=rgb0[1]; cb0=rgb0[2]; ca0=rgb0[3];
				cr1=rgb0[4]; cg1=rgb0[5]; cb1=rgb0[6]; ca1=rgb0[7];
				cr2=rgb1[0]; cg2=rgb1[1]; cb2=rgb1[2]; ca2=rgb1[3];
				cr3=rgb1[4]; cg3=rgb1[5]; cb3=rgb1[6]; ca3=rgb1[7];
			}
		}else
		{
			p0=((i<<1)+0);	p1=((i<<1)+1);
			p2=(j*2+0)*xstride;
			p3=(j*2+1)*xstride;

			l0=p0*ystride+p2;	l1=p0*ystride+p3;
			l2=p1*ystride+p2;	l3=p1*ystride+p3;
			rgb0=rgb+l0;		rgb1=rgb+l1;
			rgb2=rgb+l2;		rgb3=rgb+l3;

			if(tflip&BC7_PFB_BGR)
			{
				cr0=rgb0[2]; cg0=rgb0[1]; cb0=rgb0[0];
				cr1=rgb1[2]; cg1=rgb1[1]; cb1=rgb1[0];
				cr2=rgb2[2]; cg2=rgb2[1]; cb2=rgb2[0];
				cr3=rgb3[2]; cg3=rgb3[1]; cb3=rgb3[0];
			}else
			{
				cr0=rgb0[0]; cg0=rgb0[1]; cb0=rgb0[2];
				cr1=rgb1[0]; cg1=rgb1[1]; cb1=rgb1[2];
				cr2=rgb2[0]; cg2=rgb2[1]; cb2=rgb2[2];
				cr3=rgb3[0]; cg3=rgb3[1]; cb3=rgb3[2];
			}
		}

		cr=(cr0+cr1+cr2+cr3)>>2;
		cg=(cg0+cg1+cg2+cg3)>>2;
		cb=(cb0+cb1+cb2+cb3)>>2;
		cy0=(77*cr0 + 150*cg0 + 29*cb0 + 127)>>8;
		cy1=(77*cr1 + 150*cg1 + 29*cb1 + 127)>>8;
		cy2=(77*cr2 + 150*cg2 + 29*cb2 + 127)>>8;
		cy3=(77*cr3 + 150*cg3 + 29*cb3 + 127)>>8;
		cu=((- 43*cr - 85*cg +128*cb + 127)>>8)+128;
		cv=(( 128*cr -107*cg - 21*cb + 127)>>8)+128;

		l2=(p0<<2)+(j<<1);
		l3=(p1<<2)+(j<<1);
		pxy[l2+0]=cy0;	pxy[l2+1]=cy1;
		pxy[l3+0]=cy2;	pxy[l3+1]=cy3;

		pxa[l2+0]=ca0;	pxa[l2+1]=ca1;
		pxa[l3+0]=ca2;	pxa[l3+1]=ca3;
		
		mcy=cy0;	ncy=cy0;
		if(cy3<mcy) { mcy=cy3; }
		if(cy3>ncy) { ncy=cy3; }
		if(cy1>ncy) { ncy=cy1; }
		if(cy1<mcy) { mcy=cy1; }
		if(cy2<mcy) { mcy=cy2; }
		if(cy2>ncy) { ncy=cy2; }

		mca=ca0;	nca=ca0;
		if(ca3<mca) { mca=cy3; }
		if(ca3>nca) { nca=cy3; }
		if(ca1>nca) { nca=cy1; }
		if(ca1<mca) { mca=cy1; }
		if(ca2<mca) { mca=cy2; }
		if(ca2>nca) { nca=cy2; }

		k=i*2+j;
		pmcy[k]=mcy;	pncy[k]=ncy;
		pxu[k]=cu;		pxv[k]=cv;
		pmca[k]=mca;	pnca[k]=nca;
	}
}

void BGBBTJ_BC7_EncodeBlock_SplitMinMaxClrYA2(
	byte *pxy, byte *pxy2, byte *pxa,
	int *min_rgba, int *max_rgba,
	int *rmcy, int *rncy,
	int *rmca, int *rnca,
	int *rpartn,
	byte *rgba, int xstride, int ystride, int pfb)
{
	static const int rcp4tab[16]={
		256, 256, 128, 85, 64, 51, 43, 37,
		 32,  28,  26, 23, 21, 20, 18, 17};

	byte pxu[4];
	byte pxv[4];
	byte pmcy[4], pmca[4];
	byte pncy[4], pnca[4];
	byte *tpart;
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int l0, l1, l2, l3, l4, l5, l6, l7;
	int mcy, ncy, mcyu, mcyv, ncyu, ncyv;
	int mca, nca, mcu, mcv, ncu, ncv;
	int dcu, dcv;

	int mcy0, ncy0, mcyu0, mcyv0, ncyu0, ncyv0;
	int mcy1, ncy1, mcyu1, mcyv1, ncyu1, ncyv1;
	int mcr0, mcg0, mcb0, ncr0, ncg0, ncb0;
	int mcr1, mcg1, mcb1, ncr1, ncg1, ncb1;
	int acu0, acv0, acu1, acv1, acn0, acn1;

	int acu, acv;
	int cy, cu, cv;
	int cy1, cu1, cv1;
	int mcr, mcg, mcb, ncr, ncg, ncb;
	int pn;
	int i, j, k, l;

	BT1H_EncodeBlockFilt_RGBI(pxy, pxu, pxv, pxa,
		pmcy, pncy, pmca, pnca,
		rgba, xstride, ystride, pfb);
	
	mcy=pmcy[0];	mcyu=pxu[0]; mcyv=pxv[0];
	ncy=pncy[0];	ncyu=pxu[0]; ncyv=pxv[0];
	if(pmcy[3]<mcy) { mcy=pmcy[3]; mcyu=pxu[3]; mcyv=pxv[3]; }
	if(pncy[3]>ncy) { ncy=pncy[3]; ncyu=pxu[3]; ncyv=pxv[3]; }
	if(pmcy[1]<mcy) { mcy=pmcy[1]; mcyu=pxu[1]; mcyv=pxv[1]; }
	if(pncy[1]>ncy) { ncy=pncy[1]; ncyu=pxu[1]; ncyv=pxv[1]; }
	if(pmcy[2]<mcy) { mcy=pmcy[2]; mcyu=pxu[2]; mcyv=pxv[2]; }
	if(pncy[2]>ncy) { ncy=pncy[2]; ncyu=pxu[2]; ncyv=pxv[2]; }

	mca=pmca[0];	nca=pnca[0];
	if(pmca[3]<mca) { mca=pmca[3]; }
	if(pnca[3]>nca) { nca=pnca[3]; }
	if(pmca[1]<mca) { mca=pmca[1]; }
	if(pnca[1]>nca) { nca=pnca[1]; }
	if(pmca[2]<mca) { mca=pmca[2]; }
	if(pnca[2]>nca) { nca=pnca[2]; }
	
	acu=(pxu[0]+pxu[1]+pxu[2]+pxu[3])>>2;
	acv=(pxv[0]+pxv[1]+pxv[2]+pxv[3])>>2;
	
//	mcyu=(mcyu+acu)>>1;	mcyv=(mcyv+acv)>>1;
//	ncyu=(ncyu+acu)>>1;	ncyv=(ncyv+acv)>>1;
	
	cy1=mcy; cu1=mcyu-128; cv1=mcyv-128;
	mcr=(256*cy1        +359*cv1+128)>>8;
	mcg=(256*cy1- 88*cu1-183*cv1+128)>>8;
	mcb=(256*cy1+454*cu1        +128)>>8;

	cy1=ncy; cu1=ncyu-128; cv1=ncyv-128;
	ncr=(256*cy1        +359*cv1+128)>>8;
	ncg=(256*cy1- 88*cu1-183*cv1+128)>>8;
	ncb=(256*cy1+454*cu1        +128)>>8;
	
	if((mcr|mcg|mcb)>>8)
		{ mcr=clamp255(mcr); mcg=clamp255(mcg); mcb=clamp255(mcb); }
	if((ncr|ncg|ncb)>>8)
		{ ncr=clamp255(ncr); ncg=clamp255(ncg); ncb=clamp255(ncb); }
	
	min_rgba[0]=mcr;	min_rgba[1]=mcg;
	min_rgba[2]=mcb;	min_rgba[3]=mca;

	max_rgba[0]=ncr;	max_rgba[1]=ncg;
	max_rgba[2]=ncb;	max_rgba[3]=nca;
	
	rmcy[0]=mcy;	rncy[0]=ncy;
	rmca[0]=mca;	rnca[0]=nca;
	

	if((pfb&BC7_PFB_NOPART) ||
		!rpartn || !bgbbtj_bc7_part_init)
	{
		if(rpartn)
			*rpartn=-1;
		return;
	}

#if 1
	p0=pxu[0];	p1=pxu[1];	p2=pxu[2];	p3=pxu[3];
	p4=pxv[0];	p5=pxv[1];	p6=pxv[2];	p7=pxv[3];
	l0=(p0<p3)?p0:p3;	l1=(p1<p2)?p1:p2;
	l2=(p0>p3)?p0:p3;	l3=(p1>p2)?p1:p2;
	l4=(p4<p7)?p4:p7;	l5=(p5<p6)?p5:p6;
	l6=(p4>p7)?p4:p7;	l7=(p5>p6)?p5:p6;
	mcu=(l0<l1)?l0:l1;	ncu=(l2>l3)?l2:l3;
	mcv=(l4<l5)?l4:l5;	ncv=(l6>l7)?l6:l7;

	pn=-1;

	dcu=ncu-mcu; dcv=ncv-mcv;

	if((dcu+dcv)>24)
	{
		if(dcu>dcv)
		{
			k=0;
			if(pxu[0]>acu)k|=0x0033;
			if(pxu[1]>acu)k|=0x00CC;
			if(pxu[2]>acu)k|=0x3300;
			if(pxu[3]>acu)k|=0xCC00;
			pn=bgbbtj_bc7_part2_idx[k];
		}else
		{
			k=0;
			if(pxv[0]>acv)k|=0x0033;
			if(pxv[1]>acv)k|=0x00CC;
			if(pxv[2]>acv)k|=0x3300;
			if(pxv[3]>acv)k|=0xCC00;
			pn=bgbbtj_bc7_part2_idx[k];
		}
	}
	*rpartn=pn;
	if(pn<0)
		return;

	memcpy(pxy2, pxy, 16);

	tpart=bgbbtj_bc7_partition2+(pn*16);

	mcy0=255; mcy1=255; ncy0=0; ncy1=0;
	acu0=0; acv0=0; acu1=0; acv1=0; acn0=0; acn1=1;
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=i*4+j;	l=(i&(~1))|(j>>1);
		cy=pxy[k];	cu=pxu[l];	cv=pxv[l];

		if(tpart[k])
		{
			if(cy<mcy1) { mcy1=cy; mcyu1=cu; mcyv1=cv; }
			if(cy>ncy1) { ncy1=cy; ncyu1=cu; ncyv1=cv; }
			acu1+=cu; acv1+=cv; acn1++;
		}else
		{
			if(cy<mcy0) { mcy0=cy; mcyu0=cu; mcyv0=cv; }
			if(cy>ncy0) { ncy0=cy; ncyu0=cu; ncyv0=cv; }
			acu0+=cu; acv0+=cv; acn0++;
		}
	}
	
	acu0=(acu0*rcp4tab[acn0]+128)>>8;
	acv0=(acv0*rcp4tab[acn0]+128)>>8;
	acu1=(acu1*rcp4tab[acn1]+128)>>8;
	acv1=(acv1*rcp4tab[acn1]+128)>>8;

//	mcyu0=(3*mcyu0+acu0)>>2;
//	ncyu0=(3*ncyu0+acu0)>>2;
//	mcyu1=(3*mcyu1+acu1)>>2;
//	ncyu1=(3*ncyu1+acu1)>>2;

	cy1=mcy0; cu1=mcyu0-128; cv1=mcyv0-128;
	mcr0=(256*cy1        +359*cv1+128)>>8;
	mcg0=(256*cy1- 88*cu1-183*cv1+128)>>8;
	mcb0=(256*cy1+454*cu1        +128)>>8;

	cy1=ncy0; cu1=ncyu0-128; cv1=ncyv0-128;
	ncr0=(256*cy1        +359*cv1+128)>>8;
	ncg0=(256*cy1- 88*cu1-183*cv1+128)>>8;
	ncb0=(256*cy1+454*cu1        +128)>>8;

	cy1=mcy1; cu1=mcyu1-128; cv1=mcyv1-128;
	mcr1=(256*cy1        +359*cv1+128)>>8;
	mcg1=(256*cy1- 88*cu1-183*cv1+128)>>8;
	mcb1=(256*cy1+454*cu1        +128)>>8;

	cy1=ncy1; cu1=ncyu1-128; cv1=ncyv1-128;
	ncr1=(256*cy1        +359*cv1+128)>>8;
	ncg1=(256*cy1- 88*cu1-183*cv1+128)>>8;
	ncb1=(256*cy1+454*cu1        +128)>>8;
	
	if((mcr0|mcg0|mcb0)>>8)
		{ mcr0=clamp255(mcr0); mcg0=clamp255(mcg0); mcb0=clamp255(mcb0); }
	if((ncr0|ncg0|ncb0)>>8)
		{ ncr0=clamp255(ncr0); ncg0=clamp255(ncg0); ncb0=clamp255(ncb0); }
	if((mcr1|mcg1|mcb1)>>8)
		{ mcr1=clamp255(mcr1); mcg1=clamp255(mcg1); mcb1=clamp255(mcb1); }
	if((ncr1|ncg1|ncb1)>>8)
		{ ncr1=clamp255(ncr1); ncg1=clamp255(ncg1); ncb1=clamp255(ncb1); }
	
	min_rgba[ 4]=mcr0;	min_rgba[ 5]=mcg0;
	min_rgba[ 6]=mcb0;	min_rgba[ 7]=mca;
	min_rgba[ 8]=mcr1;	min_rgba[ 9]=mcg1;
	min_rgba[10]=mcb1;	min_rgba[11]=mca;
	max_rgba[ 4]=ncr0;	max_rgba[ 5]=ncg0;
	max_rgba[ 6]=ncb0;	max_rgba[ 7]=nca;
	max_rgba[ 8]=ncr1;	max_rgba[ 9]=ncg1;
	max_rgba[10]=ncb1;	max_rgba[11]=nca;
	rmcy[1]=mcy0;	rncy[1]=ncy0;
	rmcy[2]=mcy1;	rncy[2]=ncy1;
#endif
}



void BGBBTJ_BC7_EncodeBlock_SplitMinMaxClrYA2_Yuva4204(
	byte *pxy, byte *pxy2, byte *pxa,
	int *min_rgba, int *max_rgba,
	int *rmcy, int *rncy,
	int *rmca, int *rnca, int *rpartn,
	byte *ybuf, byte *ubuf, byte *vbuf, byte *abuf,
	int xystride, int yystride,
	int xustride, int yustride, int pfb)
{
	static const int rcp4tab[16]={
		256, 256, 128, 85, 64, 51, 43, 37,
		 32,  28,  26, 23, 21, 20, 18, 17};

	byte pxu[4];
	byte pxv[4];
	byte pmcy[4], pmca[4];
	byte pncy[4], pnca[4];
	byte *tpart;
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int l0, l1, l2, l3, l4, l5, l6, l7;
	int mcy, ncy, mcyu, mcyv, ncyu, ncyv;
	int mca, nca, mcu, mcv, ncu, ncv;
	int dcu, dcv;

	int mcy0, ncy0, mcyu0, mcyv0, ncyu0, ncyv0;
	int mcy1, ncy1, mcyu1, mcyv1, ncyu1, ncyv1;
	int mcr0, mcg0, mcb0, ncr0, ncg0, ncb0;
	int mcr1, mcg1, mcb1, ncr1, ncg1, ncb1;
	int acu0, acv0, acu1, acv1, acn0, acn1;

	int acu, acv;
	int cy, cu, cv;
	int cy1, cu1, cv1;
	int mcr, mcg, mcb, ncr, ncg, ncb;
	int pn;
	int i, j, k, l;

	BT1H_EncodeBlockFilt_Yuva4204(pxy, pxu, pxv, pxa,
		pmcy, pncy, pmca, pnca,
		ybuf, ubuf, vbuf, abuf,
		xystride, yystride,
		xustride, yustride, pfb);
	
	mcy=pmcy[0];	mcyu=pxu[0]; mcyv=pxv[0];
	ncy=pncy[0];	ncyu=pxu[0]; ncyv=pxv[0];
	if(pmcy[3]<mcy) { mcy=pmcy[3]; mcyu=pxu[3]; mcyv=pxv[3]; }
	if(pncy[3]>ncy) { ncy=pncy[3]; ncyu=pxu[3]; ncyv=pxv[3]; }
	if(pmcy[1]<mcy) { mcy=pmcy[1]; mcyu=pxu[1]; mcyv=pxv[1]; }
	if(pncy[1]>ncy) { ncy=pncy[1]; ncyu=pxu[1]; ncyv=pxv[1]; }
	if(pmcy[2]<mcy) { mcy=pmcy[2]; mcyu=pxu[2]; mcyv=pxv[2]; }
	if(pncy[2]>ncy) { ncy=pncy[2]; ncyu=pxu[2]; ncyv=pxv[2]; }

	mca=pmca[0];	nca=pnca[0];
	if(pmca[3]<mca) { mca=pmca[3]; }
	if(pnca[3]>nca) { nca=pnca[3]; }
	if(pmca[1]<mca) { mca=pmca[1]; }
	if(pnca[1]>nca) { nca=pnca[1]; }
	if(pmca[2]<mca) { mca=pmca[2]; }
	if(pnca[2]>nca) { nca=pnca[2]; }
	
	acu=(pxu[0]+pxu[1]+pxu[2]+pxu[3])>>2;
	acv=(pxv[0]+pxv[1]+pxv[2]+pxv[3])>>2;
	
//	mcyu=(mcyu+acu)>>1;	mcyv=(mcyv+acv)>>1;
//	ncyu=(ncyu+acu)>>1;	ncyv=(ncyv+acv)>>1;
	
	cy1=mcy; cu1=mcyu-128; cv1=mcyv-128;
	mcr=(256*cy1        +359*cv1+128)>>8;
	mcg=(256*cy1- 88*cu1-183*cv1+128)>>8;
	mcb=(256*cy1+454*cu1        +128)>>8;

	cy1=ncy; cu1=ncyu-128; cv1=ncyv-128;
	ncr=(256*cy1        +359*cv1+128)>>8;
	ncg=(256*cy1- 88*cu1-183*cv1+128)>>8;
	ncb=(256*cy1+454*cu1        +128)>>8;
	
	if((mcr|mcg|mcb)>>8)
		{ mcr=clamp255(mcr); mcg=clamp255(mcg); mcb=clamp255(mcb); }
	if((ncr|ncg|ncb)>>8)
		{ ncr=clamp255(ncr); ncg=clamp255(ncg); ncb=clamp255(ncb); }
	
	min_rgba[0]=mcr;	min_rgba[1]=mcg;
	min_rgba[2]=mcb;	min_rgba[3]=mca;

	max_rgba[0]=ncr;	max_rgba[1]=ncg;
	max_rgba[2]=ncb;	max_rgba[3]=nca;
	
	rmcy[0]=mcy;	rncy[0]=ncy;
	rmca[0]=mca;	rnca[0]=nca;
	

	if((pfb&BC7_PFB_NOPART) ||
		!rpartn || !bgbbtj_bc7_part_init)
	{
		if(rpartn)
			*rpartn=-1;
		return;
	}

#if 1
	p0=pxu[0];	p1=pxu[1];	p2=pxu[2];	p3=pxu[3];
	p4=pxv[0];	p5=pxv[1];	p6=pxv[2];	p7=pxv[3];
	l0=(p0<p3)?p0:p3;	l1=(p1<p2)?p1:p2;
	l2=(p0>p3)?p0:p3;	l3=(p1>p2)?p1:p2;
	l4=(p4<p7)?p4:p7;	l5=(p5<p6)?p5:p6;
	l6=(p4>p7)?p4:p7;	l7=(p5>p6)?p5:p6;
	mcu=(l0<l1)?l0:l1;	ncu=(l2>l3)?l2:l3;
	mcv=(l4<l5)?l4:l5;	ncv=(l6>l7)?l6:l7;

	pn=-1;

	dcu=ncu-mcu; dcv=ncv-mcv;

	if((dcu+dcv)>24)
	{
		if(dcu>dcv)
		{
			k=0;
			if(pxu[0]>acu)k|=0x0033;
			if(pxu[1]>acu)k|=0x00CC;
			if(pxu[2]>acu)k|=0x3300;
			if(pxu[3]>acu)k|=0xCC00;
			pn=bgbbtj_bc7_part2_idx[k];
		}else
		{
			k=0;
			if(pxv[0]>acv)k|=0x0033;
			if(pxv[1]>acv)k|=0x00CC;
			if(pxv[2]>acv)k|=0x3300;
			if(pxv[3]>acv)k|=0xCC00;
			pn=bgbbtj_bc7_part2_idx[k];
		}
	}
	*rpartn=pn;
	if(pn<0)
		return;

	memcpy(pxy2, pxy, 16);

	tpart=bgbbtj_bc7_partition2+(pn*16);

	mcy0=255; mcy1=255; ncy0=0; ncy1=0;
	acu0=0; acv0=0; acu1=0; acv1=0; acn0=0; acn1=1;
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=i*4+j;	l=(i&(~1))|(j>>1);
		cy=pxy[k];	cu=pxu[l];	cv=pxv[l];

		if(tpart[k])
		{
			if(cy<mcy1) { mcy1=cy; mcyu1=cu; mcyv1=cv; }
			if(cy>ncy1) { ncy1=cy; ncyu1=cu; ncyv1=cv; }
			acu1+=cu; acv1+=cv; acn1++;
		}else
		{
			if(cy<mcy0) { mcy0=cy; mcyu0=cu; mcyv0=cv; }
			if(cy>ncy0) { ncy0=cy; ncyu0=cu; ncyv0=cv; }
			acu0+=cu; acv0+=cv; acn0++;
		}
	}
	
	acu0=(acu0*rcp4tab[acn0]+128)>>8;
	acv0=(acv0*rcp4tab[acn0]+128)>>8;
	acu1=(acu1*rcp4tab[acn1]+128)>>8;
	acv1=(acv1*rcp4tab[acn1]+128)>>8;

//	mcyu0=(3*mcyu0+acu0)>>2;
//	ncyu0=(3*ncyu0+acu0)>>2;
//	mcyu1=(3*mcyu1+acu1)>>2;
//	ncyu1=(3*ncyu1+acu1)>>2;

	cy1=mcy0; cu1=mcyu0-128; cv1=mcyv0-128;
	mcr0=(256*cy1        +359*cv1+128)>>8;
	mcg0=(256*cy1- 88*cu1-183*cv1+128)>>8;
	mcb0=(256*cy1+454*cu1        +128)>>8;

	cy1=ncy0; cu1=ncyu0-128; cv1=ncyv0-128;
	ncr0=(256*cy1        +359*cv1+128)>>8;
	ncg0=(256*cy1- 88*cu1-183*cv1+128)>>8;
	ncb0=(256*cy1+454*cu1        +128)>>8;

	cy1=mcy1; cu1=mcyu1-128; cv1=mcyv1-128;
	mcr1=(256*cy1        +359*cv1+128)>>8;
	mcg1=(256*cy1- 88*cu1-183*cv1+128)>>8;
	mcb1=(256*cy1+454*cu1        +128)>>8;

	cy1=ncy1; cu1=ncyu1-128; cv1=ncyv1-128;
	ncr1=(256*cy1        +359*cv1+128)>>8;
	ncg1=(256*cy1- 88*cu1-183*cv1+128)>>8;
	ncb1=(256*cy1+454*cu1        +128)>>8;
	
	if((mcr0|mcg0|mcb0)>>8)
		{ mcr0=clamp255(mcr0); mcg0=clamp255(mcg0); mcb0=clamp255(mcb0); }
	if((ncr0|ncg0|ncb0)>>8)
		{ ncr0=clamp255(ncr0); ncg0=clamp255(ncg0); ncb0=clamp255(ncb0); }
	if((mcr1|mcg1|mcb1)>>8)
		{ mcr1=clamp255(mcr1); mcg1=clamp255(mcg1); mcb1=clamp255(mcb1); }
	if((ncr1|ncg1|ncb1)>>8)
		{ ncr1=clamp255(ncr1); ncg1=clamp255(ncg1); ncb1=clamp255(ncb1); }
	
	min_rgba[ 4]=mcr0;	min_rgba[ 5]=mcg0;
	min_rgba[ 6]=mcb0;	min_rgba[ 7]=mca;
	min_rgba[ 8]=mcr1;	min_rgba[ 9]=mcg1;
	min_rgba[10]=mcb1;	min_rgba[11]=mca;
	max_rgba[ 4]=ncr0;	max_rgba[ 5]=ncg0;
	max_rgba[ 6]=ncb0;	max_rgba[ 7]=nca;
	max_rgba[ 8]=ncr1;	max_rgba[ 9]=ncg1;
	max_rgba[10]=ncb1;	max_rgba[11]=nca;
	rmcy[1]=mcy0;	rncy[1]=ncy0;
	rmcy[2]=mcy1;	rncy[2]=ncy1;
#endif
}
