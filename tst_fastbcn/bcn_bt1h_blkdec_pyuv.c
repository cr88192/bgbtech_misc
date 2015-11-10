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

void BTIC1H_DecodeBlockMB2B_Alpha(byte *block,
	byte *rgba, int xstride, int ystride, int tflip)
{
	byte clr[16];
	byte *ct;
	int i0, i1, i2, i3;
	int cd, bt, cy, cya, cyb;
	int ci, pxb;
	int i, j, k, l;

	i=block[1];
	cd=i; bt=6;
	
	if(i==0)
	{
		if(block[4]==0)
		{
			if(xstride==4)
			{
				for(i=0; i<4; i++)
				{
					ct=rgba+(i*ystride);
					ct[ 0]=255;	ct[ 4]=255;
					ct[ 8]=255;	ct[12]=255;
				}
				return;
			}

			for(i=0; i<4; i++)
				for(j=0; j<4; j++)
					{ rgba[i*ystride+j*xstride]=255; }
			return;
		}

		if(block[4]==7)
		{
			cy=block[0];

			if(xstride==4)
			{
				for(i=0; i<4; i++)
				{
					ct=rgba+(i*ystride);
					ct[ 0]=cy;	ct[ 4]=cy;
					ct[ 8]=cy;	ct[12]=cy;
				}
				return;
			}

			for(i=0; i<4; i++)
				for(j=0; j<4; j++)
			{
				k=i*ystride+j*xstride;
				rgba[k]=cy;
			}
			return;
		}	

		if(i==1)
			{ cd=block[2]; bt=1; }
		else if(i==2)
			{ cd=block[2]; bt=2; }
		else if(i==6)
			{ cd=block[2]; bt=0; }
	}

	cy=block[0];

	cya=cy-(cd>>1);
	cyb=cya+cd;

	if(bt==6)
	{
		clr[0]=clamp255(cya);
		clr[7]=clamp255(cyb);
		clr[1]=(clr[0]*14+clr[7]* 2)>>4;
		clr[2]=(clr[0]*11+clr[7]* 5)>>4;
		clr[3]=(clr[0]* 9+clr[7]* 7)>>4;
		clr[4]=(clr[0]* 7+clr[7]* 9)>>4;
		clr[5]=(clr[0]* 5+clr[7]*11)>>4;
		clr[6]=(clr[0]* 2+clr[7]*14)>>4;
	}else
	{
		clr[0]=clamp255(cya);
		clr[3]=clamp255(cyb);
		clr[1]=(clr[0]*11+clr[3]* 5)>>4;
		clr[2]=(clr[0]* 5+clr[3]*11)>>4;
	}
	
	if(bt==0)
	{
		for(i=0; i<4; i++)
		{
			ct=rgba+(i*ystride);
			i0=(block[4+i]>>6)&3;	i1=(block[4+i]>>4)&3;
			i2=(block[4+i]>>2)&3;	i3=(block[4+i]   )&3;
			ct[0]=clr[i0];	ct[ 4]=clr[i1];
			ct[8]=clr[i2];	ct[12]=clr[i3];
		}
		return;
	}else if(bt==1)
	{
		pxb=block[6];
		i0=(block[4+i]>>6)&3;	i1=(block[4+i]>>4)&3;
		i2=(block[4+i]>>2)&3;	i3=(block[4+i]   )&3;
		ct=rgba;
		ct[0]=clr[i0];  ct[ 4]=clr[i0];
		ct[8]=clr[i1];  ct[12]=clr[i1];
		ct=ct+ystride;
		ct[0]=clr[i0];  ct[ 4]=clr[i0];
		ct[8]=clr[i1];  ct[12]=clr[i1];
		ct=ct+ystride;
		ct[0]=clr[i2];  ct[ 4]=clr[i2];
		ct[8]=clr[i3];  ct[12]=clr[i3];
		ct=ct+ystride;
		ct[0]=clr[i2];  ct[ 4]=clr[i2];
		ct[8]=clr[i3];  ct[12]=clr[i3];
		return;
	}else if(bt==2)
	{
		pxb=(block[6]<<8)|block[7];
		for(i=0; i<4; i++)
		{
			ct=rgba+(i*ystride);
			j=pxb>>((3-i)*4);
			i0=(j>>3)&1;	i1=(j>>2)&1;
			i2=(j>>1)&1;	i3=(j   )&1;
			ct[0]=clr[i0];  ct[ 4]=clr[i1];
			ct[8]=clr[i2];  ct[12]=clr[i3];
		}
		return;
	}else if(bt==6)
	{	
		i=(block[2]<<16)|(block[3]<<8)|block[4];
		j=(block[5]<<16)|(block[6]<<8)|block[7];
		ct=rgba;
		i0=(i>>21)&7; i1=(i>>18)&7;  i2=(i>>15)&7; i3=(i>>12)&7;
		ct[0]=clr[i0]; ct[4]=clr[i1]; ct[8]=clr[i2]; ct[12]=clr[i3];
		ct=ct+ystride;
		i0=(i>>9)&7; i1=(i>>6)&7;  i2=(i>>3)&7; i3=i&7;
		ct[0]=clr[i0]; ct[4]=clr[i1]; ct[8]=clr[i2]; ct[12]=clr[i3];
		ct=ct+ystride;
		i0=(j>>21)&7; i1=(j>>18)&7;  i2=(j>>15)&7; i3=(j>>12)&7;
		ct[0]=clr[i0]; ct[4]=clr[i1]; ct[8]=clr[i2]; ct[12]=clr[i3];
		ct=ct+ystride;
		i0=(j>>9)&7; i1=(j>>6)&7;  i2=(j>>3)&7;  i3=j&7;
		ct[0]=clr[i0]; ct[4]=clr[i1]; ct[8]=clr[i2]; ct[12]=clr[i3];
		return;
	}
}

void BTIC1H_DecodeBlockMB2B_PYUV_PYUV(byte *block,
	byte *ybuf, byte *ubuf, byte *vbuf,
	int yystride, int yustride)
{
	int cy, cu, cv, cd;
	int cy0, cy1, cy2, cy3;
	int cu1, cv1;
	int cr, cg, cb;
	int cr0, cg0, cb0;
	int cr1, cg1, cb1;
	int cr2, cg2, cb2;
	int cr3, cg3, cb3;
	int bt, pxb, pxb1;
	int i, j, k, l;

	for(i=0; i<2; i++)
		for(j=0; j<2; j++)
	{
		cu=block[ 8+(i*2+j)];
		cv=block[12+(i*2+j)];
		cu1=cu-128; cv1=cv-128;

		k=i*8+j*2;
		cy0=block[16+k];	cy1=block[17+k];
		cy2=block[20+k];	cy3=block[21+k];

		k=((i*2+0)*yystride)+(j*2);
		ybuf[k+0]=cy0;	ybuf[k+1]=cy1;
		k=((i*2+1)*yystride)+(j*4);
		ybuf[k+0]=cy2;	ybuf[k+1]=cy3;
		k=(i*yustride)+j;
		ubuf[k]=cu;		vbuf[k]=cv;
	}
}

void BTIC1H_DecodeBlockMB2B_P420_PYUV(byte *block,
	byte *ybuf, byte *ubuf, byte *vbuf,
	int yystride, int yustride)
{
	byte clr[4][4];
	byte *ct;
	int cy0, cu0, cv0, cd0, cya0, cyb0;
	int cy1, cu1, cv1, cd1, cya1, cyb1;
	int cy2, cu2, cv2, cd2, cya2, cyb2;
	int cy3, cu3, cv3, cd3, cya3, cyb3;
	int i0, i1, i2, i3;
	int ci, pxb, cf;
	int i, j, k, l;
	
	cy0=block[ 0];	cu0=block[ 1];
	cv0=block[ 2];	cd0=block[ 5];
	cy1=block[ 8];	cu1=block[ 9];
	cv1=block[10];	cd1=block[11];
	cy2=block[16];	cu2=block[17];
	cv2=block[18];	cd2=block[19];
	cy3=block[20];	cu3=block[21];
	cv3=block[22];	cd3=block[23];

	cya0=cy0-(cd0>>1);	cyb0=cya0+cd0;
	cya1=cy1-(cd1>>1);	cyb1=cya1+cd1;
	cya2=cy2-(cd2>>1);	cyb2=cya2+cd2;
	cya3=cy3-(cd3>>1);	cyb3=cya3+cd3;

	clr[0][0]=clamp255(cya0);	clr[0][3]=clamp255(cyb0);
	clr[1][0]=clamp255(cya1);	clr[1][3]=clamp255(cyb1);
	clr[2][0]=clamp255(cya2);	clr[2][3]=clamp255(cyb2);
	clr[3][0]=clamp255(cya3);	clr[3][3]=clamp255(cyb3);
	
	for(i=0; i<4; i++)
	{
		clr[i][1]=(clr[i][0]*11+clr[i][3]*5)>>4;
		clr[i][2]=(clr[i][0]*5+clr[i][3]*11)>>4;
	}

	ct=ybuf; pxb=block[12];
	i0=(pxb>>6)&3; i1=(pxb>>4)&3;  i2=(pxb>>2)&3; i3=pxb&3;
	ct[0]=clr[0][i0]; ct[1]=clr[0][i1]; ct[2]=clr[1][i2]; ct[3]=clr[1][i3];
	ct=ct+yystride; pxb=block[13];
	i0=(pxb>>6)&3; i1=(pxb>>4)&3;  i2=(pxb>>2)&3; i3=pxb&3;
	ct[0]=clr[0][i0]; ct[1]=clr[0][i1]; ct[2]=clr[1][i2]; ct[3]=clr[1][i3];
	ct=ct+yystride; pxb=block[14];
	i0=(pxb>>6)&3; i1=(pxb>>4)&3;  i2=(pxb>>2)&3; i3=pxb&3;
	ct[0]=clr[2][i0]; ct[1]=clr[2][i1]; ct[2]=clr[3][i2]; ct[3]=clr[3][i3];
	ct=ct+yystride; pxb=block[15];
	i0=(pxb>>6)&3; i1=(pxb>>4)&3;  i2=(pxb>>2)&3; i3=pxb&3;
	ct[0]=clr[2][i0]; ct[1]=clr[2][i1]; ct[2]=clr[3][i2]; ct[3]=clr[3][i3];
	ct=ubuf;			ct[0]=cu0; ct[1]=cu1;
	ct=ct+yustride;		ct[0]=cu2; ct[1]=cu3;
	ct=vbuf;			ct[0]=cv0; ct[1]=cv1;
	ct=ct+yustride;		ct[0]=cv2; ct[1]=cv3;

	return;
}

void BTIC1H_DecodeBlockMB2B_PYUV(byte *block,
	byte *ybuf, byte *ubuf, byte *vbuf,
	int yystride, int yustride)
{
	byte clr[8*4];
//	u32 clri[8];
	u64 pxb2;
	byte *ct;
	int cy, cu, cv, cd, cya, cyb;
	int cy1, cu1, cv1;
	int cr, cg, cb;
	int cr1, cg1, cb1;
	int cr2, cg2, cb2;
	int bt, pxb, pxb1;
	int i0, i1, i2, i3;
	int i, j, k, l;
	
	i=block[3];
	cd=i; bt=0;
	
	if(i==0)
	{
		if(block[4]==0)
		{
			cy=block[0];
			cu=block[1];
			cv=block[2];

//			cu=128; cv=128;

			clr[0]=cy;	clr[1]=cy;
			clr[2]=cy;	clr[3]=cy;
			l=*(u32 *)(clr+0);

			ct=ybuf;
			for(i=0; i<4; i++)
				{ ((u32 *)ct)[0]=l; ct+=yystride; }
			ct=ubuf;			ct[0]=cu;	ct[1]=cu;
			ct=ct+yustride;		ct[0]=cu;	ct[1]=cu;
			ct=vbuf;			ct[0]=cv;	ct[1]=cv;
			ct=ct+yustride;		ct[0]=cv;	ct[1]=cv;
			return;
		}	

		bt=-1;

		if(block[4]==1)
			{ cd=block[5]; bt=1; }
		else if(block[4]==2)
			{ cd=block[5]; bt=2; }
		else if(block[4]==6)
			{ cd=block[5]; bt=6; }
		else if(block[4]==17)
		{
			BTIC1H_DecodeBlockMB2B_P420_PYUV(block,
				ybuf, ubuf, vbuf,
				yystride, yustride);
			return;
		}else if(block[4]==7)
		{
			BTIC1H_DecodeBlockMB2B_PYUV_PYUV(block,
				ybuf, ubuf, vbuf,
				yystride, yustride);
			return;
		}
	}

	cy=block[0];
	cu=block[1];
	cv=block[2];

	cya=cy-(cd>>1);
	cyb=cya+cd;

	if(bt==6)
	{
		clr[0]=clamp255(cya);
		clr[7]=clamp255(cyb);
		clr[1]=(clr[0]*14+clr[7]* 2)>>4;
		clr[2]=(clr[0]*11+clr[7]* 5)>>4;
		clr[3]=(clr[0]* 9+clr[7]* 7)>>4;
		clr[4]=(clr[0]* 7+clr[7]* 9)>>4;
		clr[5]=(clr[0]* 5+clr[7]*11)>>4;
		clr[6]=(clr[0]* 2+clr[7]*14)>>4;
	}else
	{
		clr[0]=clamp255(cya);
		clr[3]=clamp255(cyb);
		clr[ 1]=(clr[0]*11+clr[3]* 5)>>4;
		clr[ 2]=(clr[0]* 5+clr[3]*11)>>4;
	}
	
	if(bt==0)
	{
		for(i=0; i<4; i++)
		{
			ct=ybuf+(i*yystride);
			i0=(block[4+i]>>6)&3;	i1=(block[4+i]>>4)&3;
			i2=(block[4+i]>>2)&3;	i3=(block[4+i]   )&3;
			ct[0]=clr[i0]; ct[1]=clr[i1];
			ct[2]=clr[i2]; ct[3]=clr[i3];
		}
		ct=ubuf;			ct[0]=cu;	ct[1]=cu;
		ct=ct+yustride;		ct[0]=cu;	ct[1]=cu;
		ct=vbuf;			ct[0]=cv;	ct[1]=cv;
		ct=ct+yustride;		ct[0]=cv;	ct[1]=cv;
		return;
	}else if(bt==1)
	{
		pxb=block[6];

		i0=(block[4+i]>>6)&3;
		i1=(block[4+i]>>4)&3;
		i2=(block[4+i]>>2)&3;
		i3=(block[4+i]   )&3;

		ct=ybuf;
		ct[0]=clr[i0]; ct[1]=clr[i0];
		ct[2]=clr[i1]; ct[3]=clr[i1];
		ct=ct+yystride;
		ct[0]=clr[i0]; ct[1]=clr[i0];
		ct[2]=clr[i1]; ct[3]=clr[i1];
		ct=ct+yystride;
		ct[0]=clr[i2]; ct[1]=clr[i2];
		ct[2]=clr[i3]; ct[3]=clr[i3];
		ct=ct+yystride;
		ct[0]=clr[i2]; ct[1]=clr[i2];
		ct[2]=clr[i3]; ct[3]=clr[i3];

		ct=ubuf;			ct[0]=cu;	ct[1]=cu;
		ct=ct+yustride;		ct[0]=cu;	ct[1]=cu;
		ct=vbuf;			ct[0]=cv;	ct[1]=cv;
		ct=ct+yustride;		ct[0]=cv;	ct[1]=cv;
		return;
	}else if(bt==2)
	{
		pxb=(block[6]<<8)|block[7];
		for(i=0; i<4; i++)
		{
			ct=ybuf+(i*yystride);
			j=pxb>>((3-i)*4);
			i0=(j>>3)&1;	i1=(j>>2)&1;
			i2=(j>>1)&1;	i3=(j   )&1;
			ct[0]=clr[i0]; ct[1]=clr[i1];
			ct[2]=clr[i2]; ct[3]=clr[i3];
		}

		ct=ubuf;			ct[0]=cu;	ct[1]=cu;
		ct=ct+yustride;		ct[0]=cu;	ct[1]=cu;
		ct=vbuf;			ct[0]=cv;	ct[1]=cv;
		ct=ct+yustride;		ct[0]=cv;	ct[1]=cv;
		return;
	}else if(bt==6)
	{	
		i=(block[10]<<16)|(block[11]<<8)|block[12];
		j=(block[13]<<16)|(block[14]<<8)|block[15];

		ct=ybuf;
		i0=(i>>21)&7; i1=(i>>18)&7;  i2=(i>>15)&7; i3=(i>>12)&7;
		ct[0]=clr[i0]; ct[2]=clr[i1]; ct[4]=clr[i2]; ct[6]=clr[i3];
		ct=ct+yystride;
		i0=(i>>9)&7; i1=(i>>6)&7;  i2=(i>>3)&7; i3=i&7;
		ct[0]=clr[i0]; ct[2]=clr[i1]; ct[4]=clr[i2]; ct[6]=clr[i3];
		ct=ct+yystride;
		i0=(j>>21)&7; i1=(j>>18)&7;  i2=(j>>15)&7; i3=(j>>12)&7;
		ct[0]=clr[i0]; ct[2]=clr[i1]; ct[4]=clr[i2]; ct[6]=clr[i3];
		ct=ct+yystride;
		i0=(j>>9)&7; i1=(j>>6)&7;  i2=(j>>3)&7;  i3=j&7;
		ct[0]=clr[i0]; ct[2]=clr[i1]; ct[4]=clr[i2]; ct[6]=clr[i3];

		ct=ubuf;			ct[0]=cu;	ct[1]=cu;
		ct=ct+yustride;		ct[0]=cu;	ct[1]=cu;
		ct=vbuf;			ct[0]=cv;	ct[1]=cv;
		ct=ct+yustride;		ct[0]=cv;	ct[1]=cv;
		return;
	}
	
	if(1)
	{
		i0=255;		i1=0;		i2=255;		i3=0;
		ct=ybuf;		ct[0]=i0; ct[1]=i1; ct[2]=i1; ct[3]=i0;
		ct=ct+yystride;	ct[0]=i1; ct[1]=i0; ct[2]=i0; ct[3]=i1;
		ct=ct+yystride;	ct[0]=i0; ct[1]=i1; ct[2]=i1; ct[3]=i0;
		ct=ct+yystride;	ct[0]=i1; ct[1]=i0; ct[2]=i0; ct[3]=i1;
		ct=ubuf;			ct[0]=i2;	ct[1]=i3;
		ct=ct+yustride;		ct[0]=i3;	ct[1]=i2;
		ct=vbuf;			ct[0]=i3;	ct[1]=i2;
		ct=ct+yustride;		ct[0]=i2;	ct[1]=i3;
	}
}

void BTIC1H_DecodeImageMB2B_PYUV(byte *block, int blkstride,
	byte *ybuf, byte *ubuf, byte *vbuf, byte *abuf,
	int xs, int ys)
{
	byte *ybuf2, *ubuf2, *vbuf2, *abuf2;
	int xystr, xustr, yystr, yustr, tflip;
	int xs1, ys1, xs2, ys2, xs3, ys3, xf, yf;
	int i, j;
	
	xs3=(xs+1)>>1; ys3=(ys+1)>>1;
	xystr=1; xustr=1;

	if(ys<0)
	{
		ys=-ys;
		yystr=-xs;
		yustr=-xs3;
		ybuf2=ybuf+xs*(ys-1);
		abuf2=abuf+xs*(ys-1);

		ubuf2=ybuf+xs3*(ys3-1);
		vbuf2=vbuf+xs3*(ys3-1);
	}else
	{
		yystr=xs;	yustr=xs3;
		ybuf2=ybuf;	ubuf2=ubuf;
		vbuf2=vbuf;	abuf2=abuf;
	}
		
	xs1=xs>>2; ys1=ys>>2;
	xs2=(xs+3)>>2; ys2=(ys+3)>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BTIC1H_DecodeBlockMB2B_PYUV(
			block+(i*xs2+j)*blkstride,
			ybuf2+(i*4*yystr)+(j*4*xystr),
			ubuf2+(i*4*yustr)+(j*4*xustr),
			vbuf2+(i*4*yustr)+(j*4*xustr),
			yystr, yustr);
		BTIC1H_DecodeBlockMB2B_Alpha(
			block+(i*xs2+j)*blkstride+24,
			abuf2+(i*4*yystr)+(j*4*xystr),
			xystr, yystr, 0);
	}
}
