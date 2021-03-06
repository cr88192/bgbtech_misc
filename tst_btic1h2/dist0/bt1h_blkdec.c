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

void BTIC1H_DecodeBlockMB2B_PYUV_RGBI(byte *block,
	byte *rgba, int xstride, int ystride, int tflip)
{
	int cy, cu, cv, cd;
	int cy0, cy1, cy2, cy3;
	int cy0t, cy1t, cy2t, cy3t;
	int cu1, cv1, cu2, cv2, cuv2;
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

		cu2=454*cu1;	cv2=359*cv1;	cuv2=-88*cu1-183*cv1;
		cy0t=(cy0<<8)+128;	cy1t=(cy1<<8)+128;
		cy2t=(cy2<<8)+128;	cy3t=(cy3<<8)+128;
		cr0=(cy0t+cv2 )>>8;		cg0=(cy0t+cuv2)>>8;
		cb0=(cy0t+cu2 )>>8;		cr1=(cy1t+cv2 )>>8;
		cg1=(cy1t+cuv2)>>8;		cb1=(cy1t+cu2 )>>8;
		cr2=(cy2t+cv2 )>>8;		cg2=(cy2t+cuv2)>>8;
		cb2=(cy2t+cu2 )>>8;		cr3=(cy3t+cv2 )>>8;
		cg3=(cy3t+cuv2)>>8;		cb3=(cy3t+cu2 )>>8;

		if(tflip&1)
		{
			k=((i*2+0)*ystride)+((j*2+0)*xstride);
			rgba[k+0]=cb0;	rgba[k+1]=cg0;	rgba[k+2]=cr0;
			k=((i*2+0)*ystride)+((j*2+1)*xstride);
			rgba[k+0]=cb1;	rgba[k+1]=cg1;	rgba[k+2]=cr1;
			k=((i*2+1)*ystride)+((j*2+0)*xstride);
			rgba[k+0]=cb2;	rgba[k+1]=cg2;	rgba[k+2]=cr2;
			k=((i*2+1)*ystride)+((j*2+1)*xstride);
			rgba[k+0]=cb3;	rgba[k+1]=cg3;	rgba[k+2]=cr3;
		}else
		{
			k=((i*2+0)*ystride)+((j*2+0)*xstride);
			rgba[k+0]=cr0;	rgba[k+1]=cg0;	rgba[k+2]=cb0;
			k=((i*2+0)*ystride)+((j*2+1)*xstride);
			rgba[k+0]=cr1;	rgba[k+1]=cg1;	rgba[k+2]=cb1;
			k=((i*2+1)*ystride)+((j*2+0)*xstride);
			rgba[k+0]=cr2;	rgba[k+1]=cg2;	rgba[k+2]=cb2;
			k=((i*2+1)*ystride)+((j*2+1)*xstride);
			rgba[k+0]=cr3;	rgba[k+1]=cg3;	rgba[k+2]=cb3;
		}
	}
}

void BTIC1H_DecodeBlockMB2B_P420_RGBI(byte *block,
	byte *rgba, int xstride, int ystride, int tflip)
{
	byte clr[4][4*4];
	int clri[4][4];
	byte *ct;
	int cy0, cu0, cv0, cd0, cya0, cyb0;
	int cy1, cu1, cv1, cd1, cya1, cyb1;
	int cy2, cu2, cv2, cd2, cya2, cyb2;
	int cy3, cu3, cv3, cd3, cya3, cyb3;
	int cra0, cga0, cba0, crb0, cgb0, cbb0;
	int cra1, cga1, cba1, crb1, cgb1, cbb1;
	int cra2, cga2, cba2, crb2, cgb2, cbb2;
	int cra3, cga3, cba3, crb3, cgb3, cbb3;
	int cyat, cybt, cu0t, cv0t, cu1t, cv1t, cuvt;
	int ci, pxb;
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

	cyat=(cya0<<8)+128;		cybt=(cyb0<<8)+128;
	cu0t=cu0-128;			cv0t=cv0-128;
	cu1t=454*cu0t;			cv1t=359*cv0t;
	cuvt=0-88*cu0t-183*cv0t;
	cra0=(cyat+cv1t)>>8;	cga0=(cyat+cuvt)>>8;
	cba0=(cyat+cu1t)>>8;	crb0=(cybt+cv1t)>>8;
	cgb0=(cybt+cuvt)>>8;	cbb0=(cybt+cu1t)>>8;

	cyat=(cya1<<8)+128;		cybt=(cyb1<<8)+128;
	cu0t=cu1-128;			cv0t=cv1-128;
	cu1t=454*cu0t;			cv1t=359*cv0t;
	cuvt=0-88*cu0t-183*cv0t;
	cra1=(cyat+cv1t)>>8;	cga1=(cyat+cuvt)>>8;
	cba1=(cyat+cu1t)>>8;	crb1=(cybt+cv1t)>>8;
	cgb1=(cybt+cuvt)>>8;	cbb1=(cybt+cu1t)>>8;

	cyat=(cya2<<8)+128;		cybt=(cyb2<<8)+128;
	cu0t=cu2-128;			cv0t=cv2-128;
	cu1t=454*cu0t;			cv1t=359*cv0t;
	cuvt=0-88*cu0t-183*cv0t;
	cra2=(cyat+cv1t)>>8;	cga2=(cyat+cuvt)>>8;
	cba2=(cyat+cu1t)>>8;	crb2=(cybt+cv1t)>>8;
	cgb2=(cybt+cuvt)>>8;	cbb2=(cybt+cu1t)>>8;

	cyat=(cya3<<8)+128;		cybt=(cyb3<<8)+128;
	cu0t=cu3-128;			cv0t=cv3-128;
	cu1t=454*cu0t;			cv1t=359*cv0t;
	cuvt=0-88*cu0t-183*cv0t;
	cra3=(cyat+cv1t)>>8;	cga3=(cyat+cuvt)>>8;
	cba3=(cyat+cu1t)>>8;	crb3=(cybt+cv1t)>>8;
	cgb3=(cybt+cuvt)>>8;	cbb3=(cybt+cu1t)>>8;
	
	if((cra0|cga0|cba0|crb0|cgb0|cbb0)>>8)
	{	cra0=clamp255(cra0);	cga0=clamp255(cga0);
		cba0=clamp255(cba0);	crb0=clamp255(crb0);
		cgb0=clamp255(cgb0);	cbb0=clamp255(cbb0);	}
	if((cra1|cga1|cba1|crb1|cgb1|cbb1)>>8)
	{	cra1=clamp255(cra1);	cga1=clamp255(cga1);
		cba1=clamp255(cba1);	crb1=clamp255(crb1);
		cgb1=clamp255(cgb1);	cbb1=clamp255(cbb1);	}
	if((cra2|cga2|cba2|crb2|cgb2|cbb2)>>8)
	{	cra2=clamp255(cra2);	cga2=clamp255(cga2);
		cba2=clamp255(cba2);	crb2=clamp255(crb2);
		cgb2=clamp255(cgb2);	cbb2=clamp255(cbb2);	}
	if((cra3|cga3|cba3|crb3|cgb3|cbb3)>>8)
	{	cra3=clamp255(cra3);	cga3=clamp255(cga3);
		cba3=clamp255(cba3);	crb3=clamp255(crb3);
		cgb3=clamp255(cgb3);	cbb3=clamp255(cbb3);	}

	if(tflip&1)
	{
		clr[0][ 0]=cba0; clr[0][ 1]=cga0; clr[0][ 2]=cra0; clr[0][ 3]=255;
		clr[0][12]=cbb0; clr[0][13]=cgb0; clr[0][14]=crb0; clr[0][15]=255;
		clr[1][ 0]=cba1; clr[1][ 1]=cga1; clr[1][ 2]=cra1; clr[1][ 3]=255;
		clr[1][12]=cbb1; clr[1][13]=cgb1; clr[1][14]=crb1; clr[1][15]=255;
		clr[2][ 0]=cba2; clr[2][ 1]=cga2; clr[2][ 2]=cra2; clr[2][ 3]=255;
		clr[2][12]=cbb2; clr[2][13]=cgb2; clr[2][14]=crb2; clr[2][15]=255;
		clr[3][ 0]=cba3; clr[3][ 1]=cga3; clr[3][ 2]=cra3; clr[3][ 3]=255;
		clr[3][12]=cbb3; clr[3][13]=cgb3; clr[3][14]=crb3; clr[3][15]=255;
	}else
	{
		clr[0][ 0]=cra0; clr[0][ 1]=cga0; clr[0][ 2]=cba0; clr[0][ 3]=255;
		clr[0][12]=crb0; clr[0][13]=cgb0; clr[0][14]=cbb0; clr[0][15]=255;
		clr[1][ 0]=cra1; clr[1][ 1]=cga1; clr[1][ 2]=cba1; clr[1][ 3]=255;
		clr[1][12]=crb1; clr[1][13]=cgb1; clr[1][14]=cbb1; clr[1][15]=255;
		clr[2][ 0]=cra2; clr[2][ 1]=cga2; clr[2][ 2]=cba2; clr[2][ 3]=255;
		clr[2][12]=crb2; clr[2][13]=cgb2; clr[2][14]=cbb2; clr[2][15]=255;
		clr[3][ 0]=cra3; clr[3][ 1]=cga3; clr[3][ 2]=cba3; clr[3][ 3]=255;
		clr[3][12]=crb3; clr[3][13]=cgb3; clr[3][14]=cbb3; clr[3][15]=255;
	}

	for(i=0; i<4; i++)
	{
		clr[i][ 4]=(clr[i][0]*11+clr[i][12]*5)>>4;
		clr[i][ 5]=(clr[i][1]*11+clr[i][13]*5)>>4;
		clr[i][ 6]=(clr[i][2]*11+clr[i][14]*5)>>4;
		clr[i][ 7]=255;
		clr[i][ 8]=(clr[i][0]*5+clr[i][12]*11)>>4;
		clr[i][ 9]=(clr[i][1]*5+clr[i][13]*11)>>4;
		clr[i][10]=(clr[i][2]*5+clr[i][14]*11)>>4;
		clr[i][11]=255;
	
		clri[i][0]=*(u32 *)(clr[i]+ 0);
		clri[i][1]=*(u32 *)(clr[i]+ 4);
		clri[i][2]=*(u32 *)(clr[i]+ 8);
		clri[i][3]=*(u32 *)(clr[i]+12);
	}

	if(xstride==4)
	{
		ct=rgba;			pxb=block[12];
		*(u32 *)(ct+ 0)=clri[0][(pxb>>6)&3];
		*(u32 *)(ct+ 4)=clri[0][(pxb>>4)&3];
		*(u32 *)(ct+ 8)=clri[1][(pxb>>2)&3];
		*(u32 *)(ct+12)=clri[1][(pxb   )&3];
		ct=ct+ystride;		pxb=block[13];
		*(u32 *)(ct+ 0)=clri[0][(pxb>>6)&3];
		*(u32 *)(ct+ 4)=clri[0][(pxb>>4)&3];
		*(u32 *)(ct+ 8)=clri[1][(pxb>>2)&3];
		*(u32 *)(ct+12)=clri[1][(pxb   )&3];
		ct=ct+ystride;		pxb=block[14];
		*(u32 *)(ct+ 0)=clri[2][(pxb>>6)&3];
		*(u32 *)(ct+ 4)=clri[2][(pxb>>4)&3];
		*(u32 *)(ct+ 8)=clri[3][(pxb>>2)&3];
		*(u32 *)(ct+12)=clri[3][(pxb   )&3];
		ct=ct+ystride;		pxb=block[15];
		*(u32 *)(ct+ 0)=clri[2][(pxb>>6)&3];
		*(u32 *)(ct+ 4)=clri[2][(pxb>>4)&3];
		*(u32 *)(ct+ 8)=clri[3][(pxb>>2)&3];
		*(u32 *)(ct+12)=clri[3][(pxb   )&3];
		return;
	}
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		ci=(i&2)|(j>>1);
		k=i*ystride+j*xstride;
		l=((block[12+i]>>(6-2*j))&3)*4;
		rgba[k+0]=clr[ci][l+0];
		rgba[k+1]=clr[ci][l+1];
		rgba[k+2]=clr[ci][l+2];
	}
}

void BTIC1H_DecodeBlockMB2B_RGBI(byte *block,
	byte *rgba, int xstride, int ystride, int tflip)
{
	byte clr[8*4];
	u32 clri[8];
	u64 pxb2;
	byte *ct;
	int cy, cu, cv, cd, cya, cyb;
	int cy1, cu1, cv1;
	int cr, cg, cb;
	int cr1, cg1, cb1;
	int cr2, cg2, cb2;
	int bt, pxb, pxb1;
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

			cu1=cu-128; cv1=cv-128;
//			cr=65536*cy           +91881*cv1;
//			cg=65536*cy- 22554*cu1-46802*cv1;
//			cb=65536*cy+116130*cu1;
//			cr>>=16; cg>>=16; cb>>=16;
//			cr=(cr+32768)>>16;
//			cg=(cg+32768)>>16;
//			cb=(cb+32768)>>16;

			cr=(256*cy        +359*cv1+128)>>8;
			cg=(256*cy- 88*cu1-183*cv1+128)>>8;
			cb=(256*cy+454*cu1        +128)>>8;

			if((cr|cg|cb)>>8)
			{
				cr=clamp255(cr);
				cg=clamp255(cg);
				cb=clamp255(cb);
			}

			if(tflip&1)
			{
				clr[0]=cb;	clr[1]=cg;
				clr[2]=cr;	clr[3]=255;
			}else
			{
				clr[0]=cr;	clr[1]=cg;
				clr[2]=cb;	clr[3]=255;
			}

			l=*(u32 *)(clr+0);

			if(xstride==4)
			{
				ct=rgba;
				*(u32 *)(ct+ 0)=l;	*(u32 *)(ct+ 4)=l;
				*(u32 *)(ct+ 8)=l;	*(u32 *)(ct+12)=l;
				ct=ct+ystride;
				*(u32 *)(ct+ 0)=l;	*(u32 *)(ct+ 4)=l;
				*(u32 *)(ct+ 8)=l;	*(u32 *)(ct+12)=l;
				ct=ct+ystride;
				*(u32 *)(ct+ 0)=l;	*(u32 *)(ct+ 4)=l;
				*(u32 *)(ct+ 8)=l;	*(u32 *)(ct+12)=l;
				ct=ct+ystride;
				*(u32 *)(ct+ 0)=l;	*(u32 *)(ct+ 4)=l;
				*(u32 *)(ct+ 8)=l;	*(u32 *)(ct+12)=l;
				return;
			}

			for(i=0; i<4; i++)
				for(j=0; j<4; j++)
			{
				k=i*ystride+j*xstride;
				rgba[k+0]=cr;
				rgba[k+1]=cg;
				rgba[k+2]=cb;
//				rgba[k+3]=255;
//				*(u32 *)(rgba+k)=l;
			}
			return;
		}	

		if(block[4]==1)
			{ cd=block[5]; bt=1; }
		else if(block[4]==2)
			{ cd=block[5]; bt=2; }
		else if(block[4]==6)
			{ cd=block[5]; bt=6; }
		else if(block[4]==17)
		{
			BTIC1H_DecodeBlockMB2B_P420_RGBI(block, rgba,
				xstride, ystride, tflip);
			return;
		}else if(block[4]==7)
		{
			BTIC1H_DecodeBlockMB2B_PYUV_RGBI(block, rgba,
				xstride, ystride, tflip);
			return;
		}else
		{
		}
	}

	cy=block[0];
	cu=block[1];
	cv=block[2];
//	cd=block[3];

	cya=cy-(cd>>1);
	cyb=cya+cd;

	cy1=cya; cu1=cu-128; cv1=cv-128;
//	cr1=65536*cy1           +91881*cv1;
//	cg1=65536*cy1- 22554*cu1-46802*cv1;
//	cb1=65536*cy1+116130*cu1;
//	cr1>>=16; cg1>>=16; cb1>>=16;
//	cr1=(cr1+32768)>>16;
//	cg1=(cg1+32768)>>16;
//	cb1=(cb1+32768)>>16;

	cr1=(256*cy1        +359*cv1+128)>>8;
	cg1=(256*cy1- 88*cu1-183*cv1+128)>>8;
	cb1=(256*cy1+454*cu1        +128)>>8;
//	cr1=(16*cy1       +22*cv1+8)>>4;
//	cg1=(16*cy1- 6*cu1-11*cv1+8)>>4;
//	cb1=(16*cy1+28*cu1       +8)>>4;

	cy1=cyb;
//	cr2=65536*cy1           +91881*cv1;
//	cg2=65536*cy1- 22554*cu1-46802*cv1;
//	cb2=65536*cy1+116130*cu1;
//	cr2>>=16; cg2>>=16; cb2>>=16;
//	cr2=(cr2+32768)>>16;
//	cg2=(cg2+32768)>>16;
//	cb2=(cb2+32768)>>16;
	cr2=(256*cy1        +359*cv1+128)>>8;
	cg2=(256*cy1- 88*cu1-183*cv1+128)>>8;
	cb2=(256*cy1+454*cu1        +128)>>8;
//	cr2=(16*cy1       +22*cv1+8)>>4;
//	cg2=(16*cy1- 6*cu1-11*cv1+8)>>4;
//	cb2=(16*cy1+28*cu1       +8)>>4;

	if((cr1|cg1|cb1|cr2|cg2|cb2)>>8)
//	if(0)
	{
		cr1=clamp255(cr1);
		cg1=clamp255(cg1);
		cb1=clamp255(cb1);

		cr2=clamp255(cr2);
		cg2=clamp255(cg2);
		cb2=clamp255(cb2);
	}

	if(bt==6)
	{
		if(tflip&1)
		{
			clr[ 0]=cb1;	clr[ 1]=cg1;
			clr[ 2]=cr1;	clr[ 3]=255;
			clr[28]=cb2;	clr[29]=cg2;
			clr[30]=cr2;	clr[31]=255;
		}else
		{
			clr[ 0]=cr1;	clr[ 1]=cg1;
			clr[ 2]=cb1;	clr[ 3]=255;
			clr[28]=cr2;	clr[29]=cg2;
			clr[30]=cb2;	clr[31]=255;
		}

		clr[ 4]=(clr[0]*14+clr[28]*2)>>4;
		clr[ 5]=(clr[1]*14+clr[29]*2)>>4;
		clr[ 6]=(clr[2]*14+clr[30]*2)>>4;
		clr[ 7]=255;
		clr[ 8]=(clr[0]*11+clr[28]*5)>>4;
		clr[ 9]=(clr[1]*11+clr[29]*5)>>4;
		clr[10]=(clr[2]*11+clr[30]*5)>>4;
		clr[11]=255;
		clr[12]=(clr[0]* 9+clr[28]*7)>>4;
		clr[13]=(clr[1]* 9+clr[29]*7)>>4;
		clr[14]=(clr[2]* 9+clr[30]*7)>>4;
		clr[15]=255;
		clr[16]=(clr[0]* 7+clr[28]*9)>>4;
		clr[17]=(clr[1]* 7+clr[29]*9)>>4;
		clr[18]=(clr[2]* 7+clr[30]*9)>>4;
		clr[19]=255;
		clr[20]=(clr[0]* 5+clr[28]*11)>>4;
		clr[21]=(clr[1]* 5+clr[29]*11)>>4;
		clr[22]=(clr[2]* 5+clr[30]*11)>>4;
		clr[23]=255;
		clr[24]=(clr[0]* 2+clr[28]*14)>>4;
		clr[25]=(clr[1]* 2+clr[29]*14)>>4;
		clr[26]=(clr[2]* 2+clr[30]*14)>>4;
		clr[27]=255;
	
		clri[0]=*(u32 *)(clr+ 0);
		clri[1]=*(u32 *)(clr+ 4);
		clri[2]=*(u32 *)(clr+ 8);
		clri[3]=*(u32 *)(clr+12);
		clri[4]=*(u32 *)(clr+16);
		clri[5]=*(u32 *)(clr+20);
		clri[6]=*(u32 *)(clr+24);
		clri[7]=*(u32 *)(clr+28);
	}else
	{
		if(tflip&1)
		{
			clr[ 0]=cb1;	clr[ 1]=cg1;
			clr[ 2]=cr1;	clr[ 3]=255;
			clr[12]=cb2;	clr[13]=cg2;
			clr[14]=cr2;	clr[15]=255;
		}else
		{
			clr[ 0]=cr1;	clr[ 1]=cg1;
			clr[ 2]=cb1;	clr[ 3]=255;
			clr[12]=cr2;	clr[13]=cg2;
			clr[14]=cb2;	clr[15]=255;
		}

		clr[ 4]=(clr[0]*11+clr[12]*5)>>4;
		clr[ 5]=(clr[1]*11+clr[13]*5)>>4;
		clr[ 6]=(clr[2]*11+clr[14]*5)>>4;
		clr[ 7]=255;
		clr[ 8]=(clr[0]*5+clr[12]*11)>>4;
		clr[ 9]=(clr[1]*5+clr[13]*11)>>4;
		clr[10]=(clr[2]*5+clr[14]*11)>>4;
		clr[11]=255;
	
		clri[0]=*(u32 *)(clr+ 0);
		clri[1]=*(u32 *)(clr+ 4);
		clri[2]=*(u32 *)(clr+ 8);
		clri[3]=*(u32 *)(clr+12);
	}
	
	if(bt==0)
	{
		if(xstride==4)
		{
			ct=rgba;		pxb=block[4];
			*(u32 *)(ct+ 0)=clri[(pxb>>6)&3];
			*(u32 *)(ct+ 4)=clri[(pxb>>4)&3];
			*(u32 *)(ct+ 8)=clri[(pxb>>2)&3];
			*(u32 *)(ct+12)=clri[(pxb   )&3];
			ct=ct+ystride;		pxb=block[5];
			*(u32 *)(ct+ 0)=clri[(pxb>>6)&3];
			*(u32 *)(ct+ 4)=clri[(pxb>>4)&3];
			*(u32 *)(ct+ 8)=clri[(pxb>>2)&3];
			*(u32 *)(ct+12)=clri[(pxb   )&3];
			ct=ct+ystride;		pxb=block[6];
			*(u32 *)(ct+ 0)=clri[(pxb>>6)&3];
			*(u32 *)(ct+ 4)=clri[(pxb>>4)&3];
			*(u32 *)(ct+ 8)=clri[(pxb>>2)&3];
			*(u32 *)(ct+12)=clri[(pxb   )&3];
			ct=ct+ystride;		pxb=block[7];
			*(u32 *)(ct+ 0)=clri[(pxb>>6)&3];
			*(u32 *)(ct+ 4)=clri[(pxb>>4)&3];
			*(u32 *)(ct+ 8)=clri[(pxb>>2)&3];
			*(u32 *)(ct+12)=clri[(pxb   )&3];
			return;
		}
	
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride;
			l=((block[4+i]>>(6-2*j))&3)*4;
			rgba[k+0]=clr[l+0];
			rgba[k+1]=clr[l+1];
			rgba[k+2]=clr[l+2];
//			rgba[k+3]=clr[l+3];
//			l=((block[4+i]>>(6-2*j))&3);
//			*(u32 *)(rgba+k)=clri[l];
		}
	}else if(bt==1)
	{
		pxb=block[6];
		if(xstride==4)
		{
			i=clri[(pxb>>6)&3];
			j=clri[(pxb>>4)&3];
			k=clri[(pxb>>2)&3];
			l=clri[(pxb   )&3];
			
			ct=rgba;
			*(u32 *)(ct+ 0)=i;	*(u32 *)(ct+ 4)=i;
			*(u32 *)(ct+ 8)=j;	*(u32 *)(ct+12)=j;
			ct+=ystride;
			*(u32 *)(ct+ 0)=i;	*(u32 *)(ct+ 4)=i;
			*(u32 *)(ct+ 8)=j;	*(u32 *)(ct+12)=j;
			ct+=ystride;
			*(u32 *)(ct+ 0)=k;	*(u32 *)(ct+ 4)=k;
			*(u32 *)(ct+ 8)=l;	*(u32 *)(ct+12)=l;
			ct+=ystride;
			*(u32 *)(ct+ 0)=k;	*(u32 *)(ct+ 4)=k;
			*(u32 *)(ct+ 8)=l;	*(u32 *)(ct+12)=l;
			return;
		}
		
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride;
			l=2*(i>>1)+(j>>1);
			l=((pxb>>(6-2*l))&3)*4;
			rgba[k+0]=clr[l+0];
			rgba[k+1]=clr[l+1];
			rgba[k+2]=clr[l+2];
//			rgba[k+3]=clr[l+3];
//			l=((pxb>>(6-2*l))&3);
//			*(u32 *)(rgba+k)=clri[l];
		}
	}else if(bt==2)
	{
		pxb=(block[6]<<8)|block[7];
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride;
			l=(pxb>>((3-i)*4+(3-j)))&1;
//			l=(l?3:0)*4;
			l=(l<<3)|(l<<2);
			rgba[k+0]=clr[l+0];
			rgba[k+1]=clr[l+1];
			rgba[k+2]=clr[l+2];
//			rgba[k+3]=clr[l+3];
//			l=(l<<1)|l;
//			*(u32 *)(rgba+k)=clri[l];
		}
	}else if(bt==6)
	{
		if(xstride==4)
		{
			pxb=(block[10]<<16)|(block[11]<<8)|block[12];
			pxb1=(block[13]<<16)|(block[14]<<8)|block[15];

			ct=rgba;
			*(u32 *)(ct+ 0)=clri[(pxb>>21)&7];
			*(u32 *)(ct+ 4)=clri[(pxb>>18)&7];
			*(u32 *)(ct+ 8)=clri[(pxb>>15)&7];
			*(u32 *)(ct+12)=clri[(pxb>>12)&7];
			ct=ct+ystride;
			*(u32 *)(ct+ 0)=clri[(pxb>> 9)&7];
			*(u32 *)(ct+ 4)=clri[(pxb>> 6)&7];
			*(u32 *)(ct+ 8)=clri[(pxb>> 3)&7];
			*(u32 *)(ct+12)=clri[(pxb>> 0)&7];
			ct=ct+ystride;
			*(u32 *)(ct+ 0)=clri[(pxb1>>21)&7];
			*(u32 *)(ct+ 4)=clri[(pxb1>>18)&7];
			*(u32 *)(ct+ 8)=clri[(pxb1>>15)&7];
			*(u32 *)(ct+12)=clri[(pxb1>>12)&7];
			ct=ct+ystride;
			*(u32 *)(ct+ 0)=clri[(pxb1>> 9)&7];
			*(u32 *)(ct+ 4)=clri[(pxb1>> 6)&7];
			*(u32 *)(ct+ 8)=clri[(pxb1>> 3)&7];
			*(u32 *)(ct+12)=clri[(pxb1    )&7];
			return;
		}
	
		i=(block[10]<<8)|block[11];
		j=(block[12]<<8)|block[13];
		k=(block[14]<<8)|block[15];
		pxb2=(((u64)i)<<32)|(((u64)j)<<16)|k;
	
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride;
			l=(pxb2>>(45-3*(i*4+j)))&7;
			l=l<<2;
			rgba[k+0]=clr[l+0];
			rgba[k+1]=clr[l+1];
			rgba[k+2]=clr[l+2];
//			*(u32 *)(rgba+k)=clri[l];
		}
	}
}

void BTIC1H_DecodeBlockMB2B_RGB(byte *block,
	byte *rgba, int xstride, int ystride)
{
	BTIC1H_DecodeBlockMB2B_RGBI(block, rgba,
		xstride, ystride, 0);
}

void BTIC1H_DecodeBlockMB2B_BGR(byte *block,
	byte *rgba, int xstride, int ystride)
{
	BTIC1H_DecodeBlockMB2B_RGBI(block, rgba,
		xstride, ystride, 1);
}


void BTIC1H_DecodeBlockMB2B_PYUV_YUY(byte *block,
	byte *rgba, int xstride, int ystride)
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

		k=((i*2+0)*ystride)+(j*4);
		rgba[k+0]=cy0;	rgba[k+1]=cu;
		rgba[k+2]=cy1;	rgba[k+3]=cv;
		k=((i*2+1)*ystride)+(j*4);
		rgba[k+0]=cy2;	rgba[k+1]=cu;
		rgba[k+2]=cy3;	rgba[k+3]=cv;
		
//		k=((i*2+0)*ystride)+((j*2+0)*xstride);
//		rgba[k+0]=cy0;	rgba[k+1]=cu;
//		k=((i*2+0)*ystride)+((j*2+1)*xstride);
//		rgba[k+0]=cy1;	rgba[k+1]=cv;
//		k=((i*2+1)*ystride)+((j*2+0)*xstride);
//		rgba[k+0]=cy2;	rgba[k+1]=cu;
//		k=((i*2+1)*ystride)+((j*2+1)*xstride);
//		rgba[k+0]=cy3;	rgba[k+1]=cv;
	}
}

void BTIC1H_DecodeBlockMB2B_YUY(byte *block,
	byte *rgba, int xstride, int ystride)
{
	byte clr[8*4];
	u32 clri[8];
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

			clr[0]=cy;	clr[1]=cu;
			clr[2]=cy;	clr[3]=cv;
			l=*(u32 *)(clr+0);

			for(i=0; i<4; i++)
			{
				k=i*ystride;
				*(u32 *)(rgba+k+0)=l;
				*(u32 *)(rgba+k+1)=l;
			}
			return;
		}	

		if(block[4]==7)
		{
			BTIC1H_DecodeBlockMB2B_PYUV_YUY(block, rgba,
				xstride, ystride);
			return;
		}

		if(block[4]==1)
			{ cd=block[5]; bt=1; }
		if(block[4]==2)
			{ cd=block[5]; bt=2; }

		if(block[4]==6)
			{ cd=block[5]; bt=6; }
	}

	cy=block[0];
	cu=block[1];
	cv=block[2];
//	cd=block[3];

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
			k=i*ystride;
			l=(block[4+i]>>6)&3;
			rgba[k+0]=clr[l];	rgba[k+1]=cu;
			l=(block[4+i]>>4)&3;
			rgba[k+2]=clr[l];	rgba[k+3]=cv;
			l=(block[4+i]>>2)&3;
			rgba[k+4]=clr[l];	rgba[k+5]=cu;
			l=(block[4+i]   )&3;
			rgba[k+6]=clr[l];	rgba[k+7]=cv;
		}
	}else if(bt==1)
	{
		pxb=block[6];

		i0=(block[4+i]>>6)&3;
		i1=(block[4+i]>>4)&3;
		i2=(block[4+i]>>2)&3;
		i3=(block[4+i]   )&3;

		k=0*ystride;
		rgba[k+0]=clr[i0];	rgba[k+1]=cu;
		rgba[k+2]=clr[i0];	rgba[k+3]=cv;
		rgba[k+4]=clr[i1];	rgba[k+5]=cu;
		rgba[k+6]=clr[i1];	rgba[k+7]=cv;
		k=1*ystride;
		rgba[k+0]=clr[i0];	rgba[k+1]=cu;
		rgba[k+2]=clr[i0];	rgba[k+3]=cv;
		rgba[k+4]=clr[i1];	rgba[k+5]=cu;
		rgba[k+6]=clr[i1];	rgba[k+7]=cv;
		k=2*ystride;
		rgba[k+0]=clr[i2];	rgba[k+1]=cu;
		rgba[k+2]=clr[i2];	rgba[k+3]=cv;
		rgba[k+4]=clr[i3];	rgba[k+5]=cu;
		rgba[k+6]=clr[i3];	rgba[k+7]=cv;
		k=3*ystride;
		rgba[k+0]=clr[i2];	rgba[k+1]=cu;
		rgba[k+2]=clr[i2];	rgba[k+3]=cv;
		rgba[k+4]=clr[i3];	rgba[k+5]=cu;
		rgba[k+6]=clr[i3];	rgba[k+7]=cv;
	}else if(bt==2)
	{
		pxb=(block[6]<<8)|block[7];
		for(i=0; i<4; i++)
		{
			k=i*ystride;
			j=pxb>>((3-i)*4);
			i0=(j>>3)&1;	i1=(j>>2)&1;
			i2=(j>>1)&1;	i3=(j   )&1;
			rgba[k+0]=clr[i0];	rgba[k+1]=cu;
			rgba[k+2]=clr[i1];	rgba[k+3]=cv;
			rgba[k+4]=clr[i2];	rgba[k+5]=cu;
			rgba[k+6]=clr[i3];	rgba[k+7]=cv;
		}
	}else if(bt==6)
	{	
		i=(block[10]<<16)|(block[11]<<8)|block[12];
		j=(block[13]<<16)|(block[14]<<8)|block[15];

		k=0*ystride;
		i0=(i>>21)&7;	i1=(i>>18)&7;
		i2=(i>>15)&7;	i3=(i>>12)&7;
		rgba[k+0]=clr[i0];	rgba[k+1]=cu;
		rgba[k+2]=clr[i1];	rgba[k+3]=cv;
		rgba[k+4]=clr[i2];	rgba[k+5]=cu;
		rgba[k+6]=clr[i3];	rgba[k+7]=cv;

		k=1*ystride;
		i0=(i>> 9)&7;	i1=(i>> 6)&7;
		i2=(i>> 3)&7;	i3=(i    )&7;
		rgba[k+0]=clr[i0];	rgba[k+1]=cu;
		rgba[k+2]=clr[i1];	rgba[k+3]=cv;
		rgba[k+4]=clr[i2];	rgba[k+5]=cu;
		rgba[k+6]=clr[i3];	rgba[k+7]=cv;

		k=2*ystride;
		i0=(j>>21)&7;	i1=(j>>18)&7;
		i2=(j>>15)&7;	i3=(j>>12)&7;
		rgba[k+0]=clr[i0];	rgba[k+1]=cu;
		rgba[k+2]=clr[i1];	rgba[k+3]=cv;
		rgba[k+4]=clr[i2];	rgba[k+5]=cu;
		rgba[k+6]=clr[i3];	rgba[k+7]=cv;

		k=3*ystride;
		i0=(j>> 9)&7;	i1=(j>> 6)&7;
		i2=(j>> 3)&7;	i3=(j    )&7;
		rgba[k+0]=clr[i0];	rgba[k+1]=cu;
		rgba[k+2]=clr[i1];	rgba[k+3]=cv;
		rgba[k+4]=clr[i2];	rgba[k+5]=cu;
		rgba[k+6]=clr[i3];	rgba[k+7]=cv;
	}
}


void BTIC1H_DecodeImageMB2B(byte *block, int blkstride,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BTIC1H_DecodeBlockMB2B_RGB(
			block+(i*xs1+j)*blkstride,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride);
	}
}

void BTIC1H_DecodeImageMB2B_Clrs(byte *block, int blkstride,
	byte *rgba, int xs, int ys, int clrs)
{
	byte *rgba2;
	int xstr, ystr, tflip;
	int xs1, ys1, xs2, ys2;
	int i, j;
	
	if(clrs==BTIC1H_PXF_YUYV)
	{
		if(ys<0)
		{
			ys=-ys;
			xstr=2;
			ystr=-2*xs;
			rgba2=rgba+xs*(ys-1)*xstr;
		}else
		{
			xstr=2;
			ystr=2*xs;
			rgba2=rgba;
		}
	
		xs1=xs>>2; ys1=ys>>2;
		xs2=(xs+3)>>2; ys2=(ys+3)>>2;
		for(i=0; i<ys1; i++)
			for(j=0; j<xs1; j++)
		{
			BTIC1H_DecodeBlockMB2B_YUY(
				block+(i*xs2+j)*blkstride,
				rgba2+(i*4*ystr)+(j*4*xstr),
				xstr, ystr);
		}
		return;
	}
	
	if((clrs==BTIC1H_PXF_RGBA) || (clrs==BTIC1H_PXF_RGBX))
		{ xstr=4; ystr=xstr*xs; tflip=0; }
	if((clrs==BTIC1H_PXF_BGRA) || (clrs==BTIC1H_PXF_BGRX))
		{ xstr=4; ystr=xstr*xs; tflip=1; }
	if(clrs==BTIC1H_PXF_RGB)
		{ xstr=3; ystr=xstr*xs; tflip=0; }
	if(clrs==BTIC1H_PXF_BGR)
		{ xstr=3; ystr=xstr*xs; tflip=1; }
	
	if(ys<0)
	{
		ys=-ys;
		ystr=-xstr*xs;
		rgba2=rgba+xs*(ys-1)*xstr;
	}else
	{
		rgba2=rgba;
	}

	xs1=xs>>2; ys1=ys>>2;
	xs2=(xs+3)>>2; ys2=(ys+3)>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BTIC1H_DecodeBlockMB2B_RGBI(
			block+(i*xs2+j)*blkstride,
			rgba2+(i*4*ystr)+(j*4*xstr),
			xstr, ystr, tflip);
	}
}
