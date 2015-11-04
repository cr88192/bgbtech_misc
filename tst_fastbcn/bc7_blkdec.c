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

#if 0
// int BGBBTJ_BC7_CalcBlockError(
	byte *rgba0, int xstride0, int ystride0,
	byte *rgba1, int xstride1, int ystride1)
{
	int e, dr, dg, db, da;
	int p0, p1;
	int i, j, k;
	
	e=0;
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		p0=i*ystride0+j*xstride0;
		p1=i*ystride1+j*xstride1;
		dr=rgba0[p0+0]-rgba1[p1+0];
		dg=rgba0[p0+1]-rgba1[p1+1];
		db=rgba0[p0+2]-rgba1[p1+2];
		da=rgba0[p0+3]-rgba1[p1+3];
		e+=dr*dr+dg*dg+db*db+da*da;
	}
	return(e);
}
#endif

int BGBBTJ_BC7_DecodeBlock_ReadMode(BGBBTJ_BitStream *bits)
{
	int i, j;

	i=BGBBTJ_BitsLE_Peek8Bits(bits);
	if(i)
	{
		if(i&15)
		{
			if(i&3)		{ j=(i&1)?0:1; }
			else		{ j=(i&4)?2:3; }
		}else
		{
			if(i&63)	{ j=(i&16)?4:5; }
			else		{ j=(i&64)?6:7; }
		}
		BGBBTJ_BitsLE_ReadBits(bits, j+1);
		return(j);
	}else
	{
		BGBBTJ_BitsLE_Read8Bits(bits);
//		i=BGBBTJ_BitsLE_Peek8Bits(bits);
#if 1
		i=8; j=BGBBTJ_BitsLE_ReadBit(bits);
		while(!j && (i<32))
			{ i++; j=BGBBTJ_BitsLE_ReadBit(bits); }
		return(i);
#endif
	}

#if 0
	i=0; j=BGBBTJ_BitsLE_ReadBit(bits);
	while(!j && (i<32))
		{ i++; j=BGBBTJ_BitsLE_ReadBit(bits); }
	return(i);
#endif
}

int bgbbtj_bc7_interpolate(int e0, int e1, int idx, int prec)
{
	int i, w;

	switch(prec)
	{
	case 2:
		w=bgbbtj_bc7_weights2[idx];
		i=(((64-w)*e0+w*e1+32)>>6);
		break;
	case 3:
		w=bgbbtj_bc7_weights3[idx];
		i=(((64-w)*e0+w*e1+32)>>6);
		break;
	case 4:
		w=bgbbtj_bc7_weights4[idx];
		i=(((64-w)*e0+w*e1+32)>>6);
		break;
	default:
		i=0; break;
	}
	return(i);
}

void BGBBTJ_BC7_DecodeBlock_Mode4(
	BGBBTJ_BitStream *bits,
	byte *rgba, int xstride, int ystride, int flags)
{
	byte blkb[16*4];
	byte clrb[8*4];
	int rot, idxm;
	int r0, r1, g0, g1, b0, b1, a0, a1;
	int p;
	int i, j, k, l;
	
	rot=BGBBTJ_BitsLE_Read2Bits(bits);
	idxm=BGBBTJ_BitsLE_ReadBit(bits);

	r0=BGBBTJ_BitsLE_Read5Bits(bits);
	r1=BGBBTJ_BitsLE_Read5Bits(bits);
	g0=BGBBTJ_BitsLE_Read5Bits(bits);
	g1=BGBBTJ_BitsLE_Read5Bits(bits);
	b0=BGBBTJ_BitsLE_Read5Bits(bits);
	b1=BGBBTJ_BitsLE_Read5Bits(bits);
	a0=BGBBTJ_BitsLE_Read6Bits(bits);
	a1=BGBBTJ_BitsLE_Read6Bits(bits);
	
	r0=(r0<<3)|(r0>>2); g0=(g0<<3)|(g0>>2);
	b0=(b0<<3)|(b0>>2); a0=(a0<<2)|(a0>>4);
	r1=(r1<<3)|(r1>>2); g1=(g1<<3)|(g1>>2);
	b1=(b1<<3)|(b1>>2); a1=(a1<<2)|(a1>>4);
	
	if(idxm==0)
	{
		for(i=0; i<4; i++)
		{
			clrb[i*4+0]=bgbbtj_bc7_interpolate(r0, r1, i, 2);
			clrb[i*4+1]=bgbbtj_bc7_interpolate(g0, g1, i, 2);
			clrb[i*4+2]=bgbbtj_bc7_interpolate(b0, b1, i, 2);
		}

		for(i=0; i<8; i++)
		{
			clrb[i*4+3]=bgbbtj_bc7_interpolate(a0, a1, i, 3);
		}
		
		p=BGBBTJ_BitsLE_ReadBit(bits);
		blkb[0*4+0]=clrb[p*4+0];
		blkb[0*4+1]=clrb[p*4+1];
		blkb[0*4+2]=clrb[p*4+2];
		for(i=1; i<16; i++)
		{
			p=BGBBTJ_BitsLE_Read2Bits(bits);
			blkb[i*4+0]=clrb[p*4+0];
			blkb[i*4+1]=clrb[p*4+1];
			blkb[i*4+2]=clrb[p*4+2];
		}

		p=BGBBTJ_BitsLE_Read2Bits(bits);
		blkb[0*4+3]=clrb[p*4+3];
		for(i=1; i<16; i++)
		{
			p=BGBBTJ_BitsLE_Read3Bits(bits);
			blkb[i*4+3]=clrb[p*4+3];
		}
	}else
	{
		for(i=0; i<8; i++)
		{
			clrb[i*4+0]=bgbbtj_bc7_interpolate(r0, r1, i, 3);
			clrb[i*4+1]=bgbbtj_bc7_interpolate(g0, g1, i, 3);
			clrb[i*4+2]=bgbbtj_bc7_interpolate(b0, b1, i, 3);
		}

		for(i=0; i<4; i++)
		{
			clrb[i*4+3]=bgbbtj_bc7_interpolate(a0, a1, i, 2);
		}
		
		p=BGBBTJ_BitsLE_ReadBit(bits);
		blkb[0*4+3]=clrb[p*4+3];
		for(i=1; i<16; i++)
		{
			p=BGBBTJ_BitsLE_Read2Bits(bits);
			blkb[i*4+3]=clrb[p*4+3];
		}

		p=BGBBTJ_BitsLE_Read2Bits(bits);
		blkb[0*4+0]=clrb[p*4+0];
		blkb[0*4+1]=clrb[p*4+1];
		blkb[0*4+2]=clrb[p*4+2];
		for(i=1; i<16; i++)
		{
			p=BGBBTJ_BitsLE_Read3Bits(bits);
			blkb[i*4+0]=clrb[p*4+0];
			blkb[i*4+1]=clrb[p*4+1];
			blkb[i*4+2]=clrb[p*4+2];
		}
	}
	
	switch(rot)
	{
	case 0: break;
	case 1: case 2: case 3:
		k=rot-1;
		for(i=0; i<16; i++)
			{ j=blkb[i*4+k]; blkb[i*4+k]=blkb[i*4+3]; blkb[i*4+3]=j; }
		break;
	default:
		break;
	}

	if(flags&1)
	{
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride; l=(i*4+j)*4;
			rgba[k+2]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+0]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
	}else
	{
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride; l=(i*4+j)*4;
			rgba[k+0]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+2]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
	}
}

void BGBBTJ_BC7_DecodeBlock_Mode5(
	BGBBTJ_BitStream *bits,
	byte *rgba, int xstride, int ystride, int flags)
{
	byte blkb[16*4];
	byte clrb[8*4];
	byte *bp0, *bp1;
	int rot, idxm;
	int r0, r1, g0, g1, b0, b1, a0, a1;
	int p, p0, p1, p2, p3;
	int i0, i1, i2, i3;
	int i, j, k, l;
	
	rot=BGBBTJ_BitsLE_Read2Bits(bits);
#if 0
	r0=BGBBTJ_BitsLE_Read7Bits(bits);
	r1=BGBBTJ_BitsLE_Read7Bits(bits);
	g0=BGBBTJ_BitsLE_Read7Bits(bits);
	g1=BGBBTJ_BitsLE_Read7Bits(bits);
	b0=BGBBTJ_BitsLE_Read7Bits(bits);
	b1=BGBBTJ_BitsLE_Read7Bits(bits);
	a0=BGBBTJ_BitsLE_Read8Bits(bits);
	a1=BGBBTJ_BitsLE_Read8Bits(bits);
#endif
#if 1
	p0=BGBBTJ_BitsLE_Read14Bits(bits);
	p1=BGBBTJ_BitsLE_Read14Bits(bits);
	p2=BGBBTJ_BitsLE_Read14Bits(bits);
	p3=BGBBTJ_BitsLE_Read16Bits(bits);
	r0=p0&127; r1=(p0>>7);	g0=p1&127; g1=(p1>>7);
	b0=p2&127; b1=(p2>>7);	a0=p3&255; a1=(p3>>8);
#endif

	r0=(r0<<1)|(r0>>6); r1=(r1<<1)|(r1>>6);
	g0=(g0<<1)|(g0>>6); g1=(g1<<1)|(g1>>6);
	b0=(b0<<1)|(b0>>6); b1=(b1<<1)|(b1>>6);

	if((r0==r1) && (g0==g1) && (b0==b1) && (a0==a1))
	{
		if(flags&1)
		{
#if defined(X86) || defined(X86_64)
			p0=(a0<<24)|(r0<<16)|(g0<<8)|(b0);
			for(i=0; i<4; i++)
			{
				j=i*ystride;
				i0=j+0*xstride;		i1=j+1*xstride;
				i2=j+2*xstride;		i3=j+3*xstride;
				*(u32 *)(rgba+i0)=p0;	*(u32 *)(rgba+i1)=p0;
				*(u32 *)(rgba+i2)=p0;	*(u32 *)(rgba+i3)=p0;
			}
#else
			for(i=0; i<4; i++)
			{
				j=i*ystride;
				i0=j+0*xstride;		i1=j+1*xstride;
				i2=j+2*xstride;		i3=j+3*xstride;
				rgba[i0+2]=r0; rgba[i0+1]=g0;
				rgba[i0+0]=b0; rgba[i0+3]=a0;
				rgba[i1+2]=r0; rgba[i1+1]=g0;
				rgba[i1+0]=b0; rgba[i1+3]=a0;
				rgba[i2+2]=r0; rgba[i2+1]=g0;
				rgba[i2+0]=b0; rgba[i2+3]=a0;
				rgba[i3+2]=r0; rgba[i3+1]=g0;
				rgba[i3+0]=b0; rgba[i3+3]=a0;
			}
#endif
		}else
		{
			for(i=0; i<4; i++)
			{
				j=i*ystride;
				i0=j+0*xstride;		i1=j+1*xstride;
				i2=j+2*xstride;		i3=j+3*xstride;
				rgba[i0+0]=r0; rgba[i0+1]=g0;
				rgba[i0+2]=b0; rgba[i0+3]=a0;
				rgba[i1+0]=r0; rgba[i1+1]=g0;
				rgba[i1+2]=b0; rgba[i1+3]=a0;
				rgba[i2+0]=r0; rgba[i2+1]=g0;
				rgba[i2+2]=b0; rgba[i2+3]=a0;
				rgba[i3+0]=r0; rgba[i3+1]=g0;
				rgba[i3+2]=b0; rgba[i3+3]=a0;
			}
		}
		return;
	}

	for(i=0; i<4; i++)
	{
		clrb[i*4+0]=bgbbtj_bc7_interpolate(r0, r1, i, 2);
		clrb[i*4+1]=bgbbtj_bc7_interpolate(g0, g1, i, 2);
		clrb[i*4+2]=bgbbtj_bc7_interpolate(b0, b1, i, 2);
		clrb[i*4+3]=bgbbtj_bc7_interpolate(a0, a1, i, 2);
	}
	
	if((r0==r1) && (g0==g1) && (b0==b1))
	{
		for(i=0; i<16; i++)
		{
			blkb[i*4+0]=r0;
			blkb[i*4+1]=g0;
			blkb[i*4+2]=b0;
		}
		if(a0!=a1)
			{ p=BGBBTJ_BitsLE_ReadBits(bits, 31); }
	}else
	{
#if 1
		p=BGBBTJ_BitsLE_Read7Bits(bits);
		p0=(p&1)*4;			p1=((p>>1)&3)*4;
		p2=((p>>3)&3)*4;	p3=((p>>5)&3)*4;
		i0=0*4;		i1=1*4;
		i2=2*4;		i3=3*4;
		blkb[i0+0]=clrb[p0+0];	blkb[i0+1]=clrb[p0+1];
		blkb[i0+2]=clrb[p0+2];	blkb[i1+0]=clrb[p1+0];
		blkb[i1+1]=clrb[p1+1];	blkb[i1+2]=clrb[p1+2];
		blkb[i2+0]=clrb[p2+0];	blkb[i2+1]=clrb[p2+1];
		blkb[i2+2]=clrb[p2+2];	blkb[i3+0]=clrb[p3+0];
		blkb[i3+1]=clrb[p3+1];	blkb[i3+2]=clrb[p3+2];
		for(i=1; i<4; i++)
		{
			p=BGBBTJ_BitsLE_Read8Bits(bits);
			p0=(p&3)*4;			p1=((p>>2)&3)*4;
			p2=((p>>4)&3)*4;	p3=((p>>6)&3)*4;
			i0=(i*4+0)*4;		i1=(i*4+1)*4;
			i2=(i*4+2)*4;		i3=(i*4+3)*4;
			blkb[i0+0]=clrb[p0+0];	blkb[i0+1]=clrb[p0+1];
			blkb[i0+2]=clrb[p0+2];	blkb[i1+0]=clrb[p1+0];
			blkb[i1+1]=clrb[p1+1];	blkb[i1+2]=clrb[p1+2];
			blkb[i2+0]=clrb[p2+0];	blkb[i2+1]=clrb[p2+1];
			blkb[i2+2]=clrb[p2+2];	blkb[i3+0]=clrb[p3+0];
			blkb[i3+1]=clrb[p3+1];	blkb[i3+2]=clrb[p3+2];
		}
#endif
#if 0
		p=BGBBTJ_BitsLE_ReadBit(bits);
		blkb[0*4+0]=clrb[p*4+0];
		blkb[0*4+1]=clrb[p*4+1];
		blkb[0*4+2]=clrb[p*4+2];
		for(i=1; i<16; i++)
		{
			p=BGBBTJ_BitsLE_Read2Bits(bits);
			blkb[i*4+0]=clrb[p*4+0];
			blkb[i*4+1]=clrb[p*4+1];
			blkb[i*4+2]=clrb[p*4+2];
		}
#endif
	}

	if(a0==a1)
	{
		for(i=0; i<4; i++)
		{
			blkb[i*16+0*4+3]=a0;
			blkb[i*16+1*4+3]=a0;
			blkb[i*16+2*4+3]=a0;
			blkb[i*16+3*4+3]=a0;
		}

//		for(i=0; i<16; i++)
//		{
//			blkb[i*4+3]=a0;
//		}
//		p=BGBBTJ_BitsLE_ReadBits(bits, 31);
	}else
	{
#if 1
		p=BGBBTJ_BitsLE_Read7Bits(bits);
		p0=(p&1)*4;			p1=((p>>1)&3)*4;
		p2=((p>>3)&3)*4;	p3=((p>>5)&3)*4;
		i0=0*4;		i1=1*4;
		i2=2*4;		i3=3*4;
		blkb[i0+3]=clrb[p0+3];	blkb[i1+3]=clrb[p1+3];
		blkb[i2+3]=clrb[p2+3];	blkb[i3+3]=clrb[p3+3];
		for(i=1; i<4; i++)
		{
			p=BGBBTJ_BitsLE_Read8Bits(bits);
			p0=(p&3)*4;			p1=((p>>2)&3)*4;
			p2=((p>>4)&3)*4;	p3=((p>>6)&3)*4;
			i0=(i*4+0)*4;		i1=(i*4+1)*4;
			i2=(i*4+2)*4;		i3=(i*4+3)*4;
			blkb[i0+3]=clrb[p0+3];	blkb[i1+3]=clrb[p1+3];
			blkb[i2+3]=clrb[p2+3];	blkb[i3+3]=clrb[p3+3];
		}
#endif
#if 0
		p=BGBBTJ_BitsLE_ReadBit(bits);
		blkb[0*4+3]=clrb[p*4+3];
		for(i=1; i<16; i++)
		{
			p=BGBBTJ_BitsLE_Read2Bits(bits);
			blkb[i*4+3]=clrb[p*4+3];
		}
#endif
	}
	
	switch(rot)
	{
	case 0: break;
	case 1: case 2: case 3:
		k=rot-1;
		for(i=0; i<16; i++)
			{ j=blkb[i*4+k]; blkb[i*4+k]=blkb[i*4+3]; blkb[i*4+3]=j; }
		break;
	default:
		break;
	}

	if(flags&1)
	{
#if defined(X86) || defined(X86_64)
		if(xstride==4)
		{
			for(i=0; i<4; i++)
			{
				bp0=blkb+i*16;
				bp1=rgba+(i*ystride);
				p0=((int *)bp0)[0];		p1=((int *)bp0)[1];
				p2=((int *)bp0)[2];		p3=((int *)bp0)[3];
				p0=(p0&0xFF00FF00)|((p0>>16)&255)|((p0&255)<<16);
				p1=(p1&0xFF00FF00)|((p1>>16)&255)|((p1&255)<<16);
				p2=(p2&0xFF00FF00)|((p2>>16)&255)|((p2&255)<<16);
				p3=(p3&0xFF00FF00)|((p3>>16)&255)|((p3&255)<<16);
//				p0=(bp0[ 0]<<16)|(bp[ 1]<<8)|(bp[ 2])|(bp[ 3]<<24);
//				p1=(bp0[ 4]<<16)|(bp[ 5]<<8)|(bp[ 6])|(bp[ 7]<<24);
//				p2=(bp0[ 8]<<16)|(bp[ 9]<<8)|(bp[10])|(bp[11]<<24);
//				p3=(bp0[12]<<16)|(bp[13]<<8)|(bp[14])|(bp[15]<<24);
				((int *)bp1)[0]=p0;		((int *)bp1)[1]=p1;
				((int *)bp1)[2]=p2;		((int *)bp1)[3]=p3;
			}
			return;
		}
#endif

#if 1
		for(i=0; i<4; i++)
		{
			k=i*ystride+0*xstride; l=(i*4+0)*4;
			rgba[k+2]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+0]=blkb[l+2]; rgba[k+3]=blkb[l+3];
			k=i*ystride+1*xstride; l=(i*4+1)*4;
			rgba[k+2]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+0]=blkb[l+2]; rgba[k+3]=blkb[l+3];
			k=i*ystride+2*xstride; l=(i*4+2)*4;
			rgba[k+2]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+0]=blkb[l+2]; rgba[k+3]=blkb[l+3];
			k=i*ystride+3*xstride; l=(i*4+3)*4;
			rgba[k+2]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+0]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
#endif

#if 0
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride; l=(i*4+j)*4;
			rgba[k+2]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+0]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
#endif
	}else
	{
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride; l=(i*4+j)*4;
//			k=i*ystride+j*xstride; l=(j*4+i)*4;
			rgba[k+0]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+2]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
	}
}

void BGBBTJ_BC7_DecodeBlock_Mode6(
	BGBBTJ_BitStream *bits,
	byte *rgba, int xstride, int ystride, int flags)
{
	byte blkb[16*4];
	byte clrb[16*4];
	int rot, idxm;
	int r0, r1, g0, g1, b0, b1, a0, a1;
	int p;
	int i, j, k, l;
	
	r0=BGBBTJ_BitsLE_Read7Bits(bits);
	r1=BGBBTJ_BitsLE_Read7Bits(bits);
	g0=BGBBTJ_BitsLE_Read7Bits(bits);
	g1=BGBBTJ_BitsLE_Read7Bits(bits);
	b0=BGBBTJ_BitsLE_Read7Bits(bits);
	b1=BGBBTJ_BitsLE_Read7Bits(bits);
	a0=BGBBTJ_BitsLE_Read7Bits(bits);
	a1=BGBBTJ_BitsLE_Read7Bits(bits);

	j=BGBBTJ_BitsLE_ReadBit(bits);
	k=BGBBTJ_BitsLE_ReadBit(bits);
	r0=(r0<<1)|j; g0=(g0<<1)|j; b0=(b0<<1)|j; a0=(a0<<1)|j;
	r1=(r1<<1)|k; g1=(g1<<1)|k; b1=(b1<<1)|k; a1=(a1<<1)|k;
	
	for(i=0; i<16; i++)
	{
		clrb[i*4+0]=bgbbtj_bc7_interpolate(r0, r1, i, 4);
		clrb[i*4+1]=bgbbtj_bc7_interpolate(g0, g1, i, 4);
		clrb[i*4+2]=bgbbtj_bc7_interpolate(b0, b1, i, 4);
		clrb[i*4+3]=bgbbtj_bc7_interpolate(a0, a1, i, 4);
	}
		
	p=BGBBTJ_BitsLE_Read3Bits(bits);
	blkb[0*4+0]=clrb[p*4+0]; blkb[0*4+1]=clrb[p*4+1];
	blkb[0*4+2]=clrb[p*4+2]; blkb[0*4+3]=clrb[p*4+3];
	for(i=1; i<16; i++)
	{
		p=BGBBTJ_BitsLE_Read4Bits(bits);
		blkb[i*4+0]=clrb[p*4+0]; blkb[i*4+1]=clrb[p*4+1];
		blkb[i*4+2]=clrb[p*4+2]; blkb[i*4+3]=clrb[p*4+3];
	}
	
	if(flags&1)
	{
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride; l=(i*4+j)*4;
			rgba[k+2]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+0]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
	}else
	{
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride; l=(i*4+j)*4;
//			k=i*ystride+j*xstride; l=(j*4+i)*4;
			rgba[k+0]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+2]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
	}
}

void BGBBTJ_BC7_DecodeBlock_Mode0(
	BGBBTJ_BitStream *bits,
	byte *rgba, int xstride, int ystride, int flags)
{
	byte blkb[16*4];
	byte clrb[32*4];
	int part, an2, an3;
	int r0, r1, g0, g1, b0, b1, a0, a1;
	int r2, r3, g2, g3, b2, b3, a2, a3;
	int r4, r5, g4, g5, b4, b5, a4, a5;
	int p0, p1, p2, p3, p4, p5;
	int i, j, k, l, p;

	part=BGBBTJ_BitsLE_Read4Bits(bits);
	
	r0=BGBBTJ_BitsLE_Read4Bits(bits);
	r1=BGBBTJ_BitsLE_Read4Bits(bits);
	r2=BGBBTJ_BitsLE_Read4Bits(bits);
	r3=BGBBTJ_BitsLE_Read4Bits(bits);
	r4=BGBBTJ_BitsLE_Read4Bits(bits);
	r5=BGBBTJ_BitsLE_Read4Bits(bits);
	g0=BGBBTJ_BitsLE_Read4Bits(bits);
	g1=BGBBTJ_BitsLE_Read4Bits(bits);
	g2=BGBBTJ_BitsLE_Read4Bits(bits);
	g3=BGBBTJ_BitsLE_Read4Bits(bits);
	g4=BGBBTJ_BitsLE_Read4Bits(bits);
	g5=BGBBTJ_BitsLE_Read4Bits(bits);
	b0=BGBBTJ_BitsLE_Read4Bits(bits);
	b1=BGBBTJ_BitsLE_Read4Bits(bits);
	b2=BGBBTJ_BitsLE_Read4Bits(bits);
	b3=BGBBTJ_BitsLE_Read4Bits(bits);
	b4=BGBBTJ_BitsLE_Read4Bits(bits);
	b5=BGBBTJ_BitsLE_Read4Bits(bits);

	p0=BGBBTJ_BitsLE_ReadBit(bits);
	p1=BGBBTJ_BitsLE_ReadBit(bits);
	p2=BGBBTJ_BitsLE_ReadBit(bits);
	p3=BGBBTJ_BitsLE_ReadBit(bits);
	p4=BGBBTJ_BitsLE_ReadBit(bits);
	p5=BGBBTJ_BitsLE_ReadBit(bits);

	r0=(r0<<1)|p0; g0=(g0<<1)|p0; b0=(b0<<1)|p0;
	r1=(r1<<1)|p1; g1=(g1<<1)|p1; b1=(b1<<1)|p1;
	r2=(r2<<1)|p2; g2=(g2<<1)|p2; b2=(b2<<1)|p2;
	r3=(r3<<1)|p3; g3=(g3<<1)|p3; b3=(b3<<1)|p3;
	r4=(r4<<1)|p4; g4=(g4<<1)|p4; b4=(b4<<1)|p4;
	r5=(r5<<1)|p5; g5=(g5<<1)|p5; b5=(b5<<1)|p5;

	r0=(r0<<3)|(r0>>2); g0=(g0<<3)|(g0>>2); b0=(b0<<3)|(b0>>2);
	r1=(r1<<3)|(r1>>2); g1=(g1<<3)|(g1>>2); b1=(b1<<3)|(b1>>2);
	r2=(r2<<3)|(r2>>2); g2=(g2<<3)|(g2>>2); b2=(b2<<3)|(b2>>2);
	r3=(r3<<3)|(r3>>2); g3=(g3<<3)|(g3>>2); b3=(b3<<3)|(b3>>2);
	r4=(r4<<3)|(r4>>2); g4=(g4<<3)|(g4>>2); b4=(b4<<3)|(b4>>2);
	r5=(r5<<3)|(r5>>2); g5=(g5<<3)|(g5>>2); b5=(b5<<3)|(b5>>2);
	
	for(i=0; i<8; i++)
	{
		clrb[(0+i)*4+0]=bgbbtj_bc7_interpolate(r0, r1, i, 3);
		clrb[(0+i)*4+1]=bgbbtj_bc7_interpolate(g0, g1, i, 3);
		clrb[(0+i)*4+2]=bgbbtj_bc7_interpolate(b0, b1, i, 3);
		clrb[(0+i)*4+3]=255;
		clrb[(8+i)*4+0]=bgbbtj_bc7_interpolate(r2, r3, i, 3);
		clrb[(8+i)*4+1]=bgbbtj_bc7_interpolate(g2, g3, i, 3);
		clrb[(8+i)*4+2]=bgbbtj_bc7_interpolate(b2, b3, i, 3);
		clrb[(8+i)*4+3]=255;
		clrb[(16+i)*4+0]=bgbbtj_bc7_interpolate(r4, r5, i, 3);
		clrb[(16+i)*4+1]=bgbbtj_bc7_interpolate(g4, g5, i, 3);
		clrb[(16+i)*4+2]=bgbbtj_bc7_interpolate(b4, b5, i, 3);
		clrb[(16+i)*4+3]=255;
	}
	
	an2=bgbbtj_bc7_anchor3a[part];
	an3=bgbbtj_bc7_anchor3b[part];
	
	p=BGBBTJ_BitsLE_Read2Bits(bits);
	blkb[0*4+0]=clrb[p*4+0]; blkb[0*4+1]=clrb[p*4+1];
	blkb[0*4+2]=clrb[p*4+2]; blkb[0*4+3]=clrb[p*4+3];
	for(i=1; i<16; i++)
	{
		j=bgbbtj_bc7_partition3[(part<<4)+i]*8;
		if((i==an2) || (i==an3))
			{ p=BGBBTJ_BitsLE_Read2Bits(bits)+j; }
		else
			{ p=BGBBTJ_BitsLE_Read3Bits(bits)+j; }
		blkb[i*4+0]=clrb[p*4+0]; blkb[i*4+1]=clrb[p*4+1];
		blkb[i*4+2]=clrb[p*4+2]; blkb[i*4+3]=clrb[p*4+3];
	}
	
	if(flags&1)
	{
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride; l=(i*4+j)*4;
			rgba[k+2]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+0]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
	}else
	{
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride; l=(i*4+j)*4;
//			k=i*ystride+j*xstride; l=(j*4+i)*4;
			rgba[k+0]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+2]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
	}
}

void BGBBTJ_BC7_DecodeBlock_Mode1(
	BGBBTJ_BitStream *bits,
	byte *rgba, int xstride, int ystride, int flags)
{
	byte blkb[16*4];
	byte clrb[16*4];
	int part, an2;
	int r0, r1, g0, g1, b0, b1, a0, a1;
	int r2, r3, g2, g3, b2, b3, a2, a3;
	int p0, p1, p2, p3;
	int i, j, k, l, p;

	part=BGBBTJ_BitsLE_Read6Bits(bits);
	
	r0=BGBBTJ_BitsLE_Read6Bits(bits);
	r1=BGBBTJ_BitsLE_Read6Bits(bits);
	r2=BGBBTJ_BitsLE_Read6Bits(bits);
	r3=BGBBTJ_BitsLE_Read6Bits(bits);
	g0=BGBBTJ_BitsLE_Read6Bits(bits);
	g1=BGBBTJ_BitsLE_Read6Bits(bits);
	g2=BGBBTJ_BitsLE_Read6Bits(bits);
	g3=BGBBTJ_BitsLE_Read6Bits(bits);
	b0=BGBBTJ_BitsLE_Read6Bits(bits);
	b1=BGBBTJ_BitsLE_Read6Bits(bits);
	b2=BGBBTJ_BitsLE_Read6Bits(bits);
	b3=BGBBTJ_BitsLE_Read6Bits(bits);

	p0=BGBBTJ_BitsLE_ReadBit(bits);
	p1=BGBBTJ_BitsLE_ReadBit(bits);

	r0=(r0<<1)|p0; g0=(g0<<1)|p0; b0=(b0<<1)|p0;
	r1=(r1<<1)|p0; g1=(g1<<1)|p0; b1=(b1<<1)|p0;
	r2=(r2<<1)|p1; g2=(g2<<1)|p1; b2=(b2<<1)|p1;
	r3=(r3<<1)|p1; g3=(g3<<1)|p1; b3=(b3<<1)|p1;

	r0=(r0<<1)|(r0>>6); g0=(g0<<1)|(g0>>6); b0=(b0<<1)|(b0>>6);
	r1=(r1<<1)|(r1>>6); g1=(g1<<1)|(g1>>6); b1=(b1<<1)|(b1>>6);
	r2=(r2<<1)|(r2>>6); g2=(g2<<1)|(g2>>6); b2=(b2<<1)|(b2>>6);
	r3=(r3<<1)|(r3>>6); g3=(g3<<1)|(g3>>6); b3=(b3<<1)|(b3>>6);
	
	for(i=0; i<8; i++)
	{
		clrb[(0+i)*4+0]=bgbbtj_bc7_interpolate(r0, r1, i, 3);
		clrb[(0+i)*4+1]=bgbbtj_bc7_interpolate(g0, g1, i, 3);
		clrb[(0+i)*4+2]=bgbbtj_bc7_interpolate(b0, b1, i, 3);
		clrb[(0+i)*4+3]=255;
		clrb[(8+i)*4+0]=bgbbtj_bc7_interpolate(r2, r3, i, 3);
		clrb[(8+i)*4+1]=bgbbtj_bc7_interpolate(g2, g3, i, 3);
		clrb[(8+i)*4+2]=bgbbtj_bc7_interpolate(b2, b3, i, 3);
		clrb[(8+i)*4+3]=255;
	}
	
	an2=bgbbtj_bc7_anchor2[part];
	
	p=BGBBTJ_BitsLE_Read2Bits(bits);
	blkb[0*4+0]=clrb[p*4+0]; blkb[0*4+1]=clrb[p*4+1];
	blkb[0*4+2]=clrb[p*4+2]; blkb[0*4+3]=clrb[p*4+3];
	for(i=1; i<16; i++)
	{
		j=bgbbtj_bc7_partition2[(part<<4)+i]*8;
		if(i==an2)
			{ p=BGBBTJ_BitsLE_Read2Bits(bits)+j; }
		else
			{ p=BGBBTJ_BitsLE_Read3Bits(bits)+j; }
		blkb[i*4+0]=clrb[p*4+0]; blkb[i*4+1]=clrb[p*4+1];
		blkb[i*4+2]=clrb[p*4+2]; blkb[i*4+3]=clrb[p*4+3];
	}
	
	if(flags&1)
	{
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride; l=(i*4+j)*4;
			rgba[k+2]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+0]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
	}else
	{
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride; l=(i*4+j)*4;
//			k=i*ystride+j*xstride; l=(j*4+i)*4;
			rgba[k+0]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+2]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
	}
}

void BGBBTJ_BC7_DecodeBlock_Mode2(
	BGBBTJ_BitStream *bits,
	byte *rgba, int xstride, int ystride, int flags)
{
	byte blkb[16*4];
	byte clrb[16*4];
	int part, an2, an3;
	int r0, r1, g0, g1, b0, b1, a0, a1;
	int r2, r3, g2, g3, b2, b3, a2, a3;
	int r4, r5, g4, g5, b4, b5, a4, a5;
	int p0, p1, p2, p3;
	int i, j, k, l, p;

	part=BGBBTJ_BitsLE_Read6Bits(bits);
	
	r0=BGBBTJ_BitsLE_Read5Bits(bits);
	r1=BGBBTJ_BitsLE_Read5Bits(bits);
	r2=BGBBTJ_BitsLE_Read5Bits(bits);
	r3=BGBBTJ_BitsLE_Read5Bits(bits);
	r4=BGBBTJ_BitsLE_Read5Bits(bits);
	r5=BGBBTJ_BitsLE_Read5Bits(bits);
	g0=BGBBTJ_BitsLE_Read5Bits(bits);
	g1=BGBBTJ_BitsLE_Read5Bits(bits);
	g2=BGBBTJ_BitsLE_Read5Bits(bits);
	g3=BGBBTJ_BitsLE_Read5Bits(bits);
	g4=BGBBTJ_BitsLE_Read5Bits(bits);
	g5=BGBBTJ_BitsLE_Read5Bits(bits);
	b0=BGBBTJ_BitsLE_Read5Bits(bits);
	b1=BGBBTJ_BitsLE_Read5Bits(bits);
	b2=BGBBTJ_BitsLE_Read5Bits(bits);
	b3=BGBBTJ_BitsLE_Read5Bits(bits);
	b4=BGBBTJ_BitsLE_Read5Bits(bits);
	b5=BGBBTJ_BitsLE_Read5Bits(bits);

	r0=(r0<<3)|(r0>>2); g0=(g0<<3)|(g0>>2); b0=(b0<<3)|(b0>>2);
	r1=(r1<<3)|(r1>>2); g1=(g1<<3)|(g1>>2); b1=(b1<<3)|(b1>>2);
	r2=(r2<<3)|(r2>>2); g2=(g2<<3)|(g2>>2); b2=(b2<<3)|(b2>>2);
	r3=(r3<<3)|(r3>>2); g3=(g3<<3)|(g3>>2); b3=(b3<<3)|(b3>>2);
	r4=(r4<<3)|(r4>>2); g4=(g4<<3)|(g4>>2); b4=(b4<<3)|(b4>>2);
	r5=(r5<<3)|(r5>>2); g5=(g5<<3)|(g5>>2); b5=(b5<<3)|(b5>>2);

	for(i=0; i<4; i++)
	{
		clrb[(0+i)*4+0]=bgbbtj_bc7_interpolate(r0, r1, i, 2);
		clrb[(0+i)*4+1]=bgbbtj_bc7_interpolate(g0, g1, i, 2);
		clrb[(0+i)*4+2]=bgbbtj_bc7_interpolate(b0, b1, i, 2);
		clrb[(0+i)*4+3]=255;
		clrb[(4+i)*4+0]=bgbbtj_bc7_interpolate(r2, r3, i, 2);
		clrb[(4+i)*4+1]=bgbbtj_bc7_interpolate(g2, g3, i, 2);
		clrb[(4+i)*4+2]=bgbbtj_bc7_interpolate(b2, b3, i, 2);
		clrb[(4+i)*4+3]=255;
		clrb[(8+i)*4+0]=bgbbtj_bc7_interpolate(r4, r5, i, 2);
		clrb[(8+i)*4+1]=bgbbtj_bc7_interpolate(g4, g5, i, 2);
		clrb[(8+i)*4+2]=bgbbtj_bc7_interpolate(b4, b5, i, 2);
		clrb[(8+i)*4+3]=255;
	}
	
	an2=bgbbtj_bc7_anchor3a[part];
	an3=bgbbtj_bc7_anchor3b[part];
	
	p=BGBBTJ_BitsLE_ReadBit(bits);
	blkb[0*4+0]=clrb[p*4+0]; blkb[0*4+1]=clrb[p*4+1];
	blkb[0*4+2]=clrb[p*4+2]; blkb[0*4+3]=clrb[p*4+3];
	for(i=1; i<16; i++)
	{
		j=bgbbtj_bc7_partition3[(part<<4)+i]*4;
		if((i==an2) || (i==an3))
			{ p=BGBBTJ_BitsLE_ReadBit(bits)+j; }
		else
			{ p=BGBBTJ_BitsLE_Read2Bits(bits)+j; }
		blkb[i*4+0]=clrb[p*4+0]; blkb[i*4+1]=clrb[p*4+1];
		blkb[i*4+2]=clrb[p*4+2]; blkb[i*4+3]=clrb[p*4+3];
	}
	
	if(flags&1)
	{
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride; l=(i*4+j)*4;
			rgba[k+2]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+0]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
	}else
	{
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride; l=(i*4+j)*4;
//			k=i*ystride+j*xstride; l=(j*4+i)*4;
			rgba[k+0]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+2]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
	}
}

void BGBBTJ_BC7_DecodeBlock_Mode3(
	BGBBTJ_BitStream *bits,
	byte *rgba, int xstride, int ystride, int flags)
{
	byte blkb[16*4];
	byte clrb[16*4];
	int part, an2;
	int r0, r1, g0, g1, b0, b1, a0, a1;
	int r2, r3, g2, g3, b2, b3, a2, a3;
	int p0, p1, p2, p3;
	int i, j, k, l, p;

	part=BGBBTJ_BitsLE_Read6Bits(bits);
	
	r0=BGBBTJ_BitsLE_Read7Bits(bits);
	r1=BGBBTJ_BitsLE_Read7Bits(bits);
	r2=BGBBTJ_BitsLE_Read7Bits(bits);
	r3=BGBBTJ_BitsLE_Read7Bits(bits);
	g0=BGBBTJ_BitsLE_Read7Bits(bits);
	g1=BGBBTJ_BitsLE_Read7Bits(bits);
	g2=BGBBTJ_BitsLE_Read7Bits(bits);
	g3=BGBBTJ_BitsLE_Read7Bits(bits);
	b0=BGBBTJ_BitsLE_Read7Bits(bits);
	b1=BGBBTJ_BitsLE_Read7Bits(bits);
	b2=BGBBTJ_BitsLE_Read7Bits(bits);
	b3=BGBBTJ_BitsLE_Read7Bits(bits);

	p0=BGBBTJ_BitsLE_ReadBit(bits);
	p1=BGBBTJ_BitsLE_ReadBit(bits);
	p2=BGBBTJ_BitsLE_ReadBit(bits);
	p3=BGBBTJ_BitsLE_ReadBit(bits);
	r0=(r0<<1)|p0; g0=(g0<<1)|p0; b0=(b0<<1)|p0;
	r1=(r1<<1)|p1; g1=(g1<<1)|p1; b1=(b1<<1)|p1;
	r2=(r2<<1)|p2; g2=(g2<<1)|p2; b2=(b2<<1)|p2;
	r3=(r3<<1)|p3; g3=(g3<<1)|p3; b3=(b3<<1)|p3;
	
	for(i=0; i<4; i++)
	{
		clrb[(0+i)*4+0]=bgbbtj_bc7_interpolate(r0, r1, i, 2);
		clrb[(0+i)*4+1]=bgbbtj_bc7_interpolate(g0, g1, i, 2);
		clrb[(0+i)*4+2]=bgbbtj_bc7_interpolate(b0, b1, i, 2);
		clrb[(0+i)*4+3]=255;
		clrb[(8+i)*4+0]=bgbbtj_bc7_interpolate(r2, r3, i, 2);
		clrb[(8+i)*4+1]=bgbbtj_bc7_interpolate(g2, g3, i, 2);
		clrb[(8+i)*4+2]=bgbbtj_bc7_interpolate(b2, b3, i, 2);
		clrb[(8+i)*4+3]=255;
	}
	
	an2=bgbbtj_bc7_anchor2[part];
	
	p=BGBBTJ_BitsLE_ReadBit(bits);
	blkb[0*4+0]=clrb[p*4+0]; blkb[0*4+1]=clrb[p*4+1];
	blkb[0*4+2]=clrb[p*4+2]; blkb[0*4+3]=clrb[p*4+3];
	for(i=1; i<16; i++)
	{
		j=bgbbtj_bc7_partition2[(part<<4)+i]*8;
		if(i==an2)
			{ p=BGBBTJ_BitsLE_ReadBit(bits)+j; }
		else
			{ p=BGBBTJ_BitsLE_Read2Bits(bits)+j; }
		blkb[i*4+0]=clrb[p*4+0]; blkb[i*4+1]=clrb[p*4+1];
		blkb[i*4+2]=clrb[p*4+2]; blkb[i*4+3]=clrb[p*4+3];
	}
	
	if(flags&1)
	{
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride; l=(i*4+j)*4;
			rgba[k+2]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+0]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
	}else
	{
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride; l=(i*4+j)*4;
//			k=i*ystride+j*xstride; l=(j*4+i)*4;
			rgba[k+0]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+2]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
	}
}

void BGBBTJ_BC7_DecodeBlock_Mode7(
	BGBBTJ_BitStream *bits,
	byte *rgba, int xstride, int ystride, int flags)
{
	byte blkb[16*4];
	byte clrb[16*4];
	int part, an2;
	int r0, r1, g0, g1, b0, b1, a0, a1;
	int r2, r3, g2, g3, b2, b3, a2, a3;
	int p0, p1, p2, p3;
	int i, j, k, l, p;

	part=BGBBTJ_BitsLE_Read6Bits(bits);
	
	r0=BGBBTJ_BitsLE_Read5Bits(bits);
	r1=BGBBTJ_BitsLE_Read5Bits(bits);
	r2=BGBBTJ_BitsLE_Read5Bits(bits);
	r3=BGBBTJ_BitsLE_Read5Bits(bits);
	g0=BGBBTJ_BitsLE_Read5Bits(bits);
	g1=BGBBTJ_BitsLE_Read5Bits(bits);
	g2=BGBBTJ_BitsLE_Read5Bits(bits);
	g3=BGBBTJ_BitsLE_Read5Bits(bits);
	b0=BGBBTJ_BitsLE_Read5Bits(bits);
	b1=BGBBTJ_BitsLE_Read5Bits(bits);
	b2=BGBBTJ_BitsLE_Read5Bits(bits);
	b3=BGBBTJ_BitsLE_Read5Bits(bits);
	a0=BGBBTJ_BitsLE_Read5Bits(bits);
	a1=BGBBTJ_BitsLE_Read5Bits(bits);
	a2=BGBBTJ_BitsLE_Read5Bits(bits);
	a3=BGBBTJ_BitsLE_Read5Bits(bits);

	p0=BGBBTJ_BitsLE_ReadBit(bits);
	p1=BGBBTJ_BitsLE_ReadBit(bits);
	p2=BGBBTJ_BitsLE_ReadBit(bits);
	p3=BGBBTJ_BitsLE_ReadBit(bits);

	r0=(r0<<1)|p0; g0=(g0<<1)|p0; b0=(b0<<1)|p0; a0=(a0<<1)|p0;
	r1=(r1<<1)|p1; g1=(g1<<1)|p1; b1=(b1<<1)|p1; a1=(a1<<1)|p1;
	r2=(r2<<1)|p2; g2=(g2<<1)|p2; b2=(b2<<1)|p2; a2=(a2<<1)|p2;
	r3=(r3<<1)|p3; g3=(g3<<1)|p3; b3=(b3<<1)|p3; a3=(a3<<1)|p3;

	r0=(r0<<2)|(r0>>4); g0=(g0<<2)|(g0>>4);
	b0=(b0<<2)|(b0>>4); a0=(a0<<2)|(a0>>4);
	r1=(r1<<2)|(r1>>4); g1=(g1<<2)|(g1>>4);
	b1=(b1<<2)|(b1>>4); a1=(a1<<2)|(a1>>4);
	r2=(r2<<2)|(r2>>4); g2=(g2<<2)|(g2>>4);
	b2=(b2<<2)|(b2>>4); a2=(a2<<2)|(a2>>4);
	r3=(r3<<2)|(r3>>4); g3=(g3<<2)|(g3>>4);
	b3=(b3<<2)|(b3>>4); a3=(a3<<2)|(a3>>4);

	for(i=0; i<4; i++)
	{
		clrb[(0+i)*4+0]=bgbbtj_bc7_interpolate(r0, r1, i, 2);
		clrb[(0+i)*4+1]=bgbbtj_bc7_interpolate(g0, g1, i, 2);
		clrb[(0+i)*4+2]=bgbbtj_bc7_interpolate(b0, b1, i, 2);
		clrb[(0+i)*4+3]=bgbbtj_bc7_interpolate(a0, a1, i, 2);
		clrb[(8+i)*4+0]=bgbbtj_bc7_interpolate(r2, r3, i, 2);
		clrb[(8+i)*4+1]=bgbbtj_bc7_interpolate(g2, g3, i, 2);
		clrb[(8+i)*4+2]=bgbbtj_bc7_interpolate(b2, b3, i, 2);
		clrb[(8+i)*4+3]=bgbbtj_bc7_interpolate(a2, a3, i, 2);
	}
	
	an2=bgbbtj_bc7_anchor2[part];
	
	p=BGBBTJ_BitsLE_ReadBit(bits);
	blkb[0*4+0]=clrb[p*4+0]; blkb[0*4+1]=clrb[p*4+1];
	blkb[0*4+2]=clrb[p*4+2]; blkb[0*4+3]=clrb[p*4+3];
	for(i=1; i<16; i++)
	{
		j=bgbbtj_bc7_partition2[(part<<4)+i]*8;
		if(i==an2)
			{ p=BGBBTJ_BitsLE_ReadBit(bits)+j; }
		else
			{ p=BGBBTJ_BitsLE_Read2Bits(bits)+j; }
		blkb[i*4+0]=clrb[p*4+0]; blkb[i*4+1]=clrb[p*4+1];
		blkb[i*4+2]=clrb[p*4+2]; blkb[i*4+3]=clrb[p*4+3];
	}
	
	if(flags&1)
	{
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride; l=(i*4+j)*4;
			rgba[k+2]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+0]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
	}else
	{
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride; l=(i*4+j)*4;
//			k=i*ystride+j*xstride; l=(j*4+i)*4;
			rgba[k+0]=blkb[l+0]; rgba[k+1]=blkb[l+1];
			rgba[k+2]=blkb[l+2]; rgba[k+3]=blkb[l+3];
		}
	}
}

void BGBBTJ_BC7_DecodeBlock_Default(
	BGBBTJ_BitStream *bits,
	byte *rgba, int xstride, int ystride, int flags)
{
	int i, j;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		rgba[i*ystride+j*xstride+0]=0;
		rgba[i*ystride+j*xstride+1]=0;
		rgba[i*ystride+j*xstride+2]=0;
		rgba[i*ystride+j*xstride+3]=0;
	}
}

int BGBBTJ_BC7_GetBlockMode(byte *block)
{
	int mode;
	int i;
	
	i=block[0];
	if((i&1)==1)		//Mode 0
	{
		mode=0;
	}else if((i&  3)==  2)		//Mode 1
	{
		mode=1;
	}else if((i&  7)==  4)		//Mode 2
	{
		mode=2;
	}else if((i& 15)==  8)		//Mode 3
	{
		mode=3;
	}else if((i& 31)== 16)		//Mode 4
	{
		mode=4;
	}else if((i& 63)== 32)		//Mode 5
	{
		mode=5;
	}else if((i&127)== 64)		//Mode 6
	{
		mode=6;
	}else if((i&255)==128)		//Mode 7
	{
		mode=7;
	}else
	{
		mode=8;
	}
	return(mode);
}

void BGBBTJ_BC7_DecodeBlock(byte *block,
	byte *rgba, int xstride, int ystride, int flags)
{
	BGBBTJ_BitStream bits;
	int mode;

	BGBBTJ_BitsLE_ClearSetupRead(&bits, block, 16);
	mode=BGBBTJ_BC7_DecodeBlock_ReadMode(&bits);
	switch(mode)
	{
	case 0:
		BGBBTJ_BC7_DecodeBlock_Mode0(&bits, rgba, xstride, ystride, flags);
		break;
	case 1:
		BGBBTJ_BC7_DecodeBlock_Mode1(&bits, rgba, xstride, ystride, flags);
		break;
	case 2:
		BGBBTJ_BC7_DecodeBlock_Mode2(&bits, rgba, xstride, ystride, flags);
		break;
	case 3:
		BGBBTJ_BC7_DecodeBlock_Mode3(&bits, rgba, xstride, ystride, flags);
		break;
	case 4:
		BGBBTJ_BC7_DecodeBlock_Mode4(&bits, rgba, xstride, ystride, flags);
		break;
	case 5:
		BGBBTJ_BC7_DecodeBlock_Mode5(&bits, rgba, xstride, ystride, flags);
		break;
	case 6:
		BGBBTJ_BC7_DecodeBlock_Mode6(&bits, rgba, xstride, ystride, flags);
		break;
	case 7:
		BGBBTJ_BC7_DecodeBlock_Mode7(&bits, rgba, xstride, ystride, flags);
		break;
	default:
		BGBBTJ_BC7_DecodeBlock_Default(&bits, rgba, xstride, ystride, flags);
		break;
	}
}

void BGBBTJ_BC7_DecodeBlockEdge(byte *block,
	byte *rgba, int xstride, int ystride,
	int xfrac, int yfrac, int pfb)
{
	byte tblk[16*4];
	byte *cs, *ct;
	int i, j, xn;

	BGBBTJ_BC7_DecodeBlock(block, tblk, xstride, 4*xstride, pfb);

	xn=xfrac*xstride;
	for(i=0; i<yfrac; i++)
	{
		cs=tblk+i*4*xstride;
		ct=rgba+i*ystride;
		for(j=0; j<xn; j++)
			*ct++=*cs++;
	}
}

void BGBBTJ_BC7_DecodeImageBC7I(byte *block,
	byte *rgba, int xs, int ys, int stride, int pfb)
{
	byte *rgba2, *rgba3;
	int xstr, ystr;
	int xs1, ys1, xs2, ys2, xf, yf;
	int i, j;

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
			BGBBTJ_BC7_DecodeBlock(
				block+(i*xs2+j)*16,
				rgba3+(j*4*xstr),
				stride, xs*stride, 0);
		}

		if(xf)
		{
			BGBBTJ_BC7_DecodeBlockEdge(
				block+(i*xs2+j)*16,
				rgba3+(j*4*xstr),
				xstr, ystr, xf, 4, 0);
		}
	}

	if(yf)
	{
		rgba3=rgba2+(i*4*ystr);
		for(j=0; j<xs1; j++)
		{
			BGBBTJ_BC7_DecodeBlockEdge(
				block+(i*xs2+j)*16,
				rgba3+(j*4*xstr),
				xstr, ystr, 4, yf, 0);
		}
		if(xf)
		{
			BGBBTJ_BC7_DecodeBlockEdge(
				block+(i*xs2+j)*16,
				rgba3+(j*4*xstr),
				xstr, ystr, xf, yf, 0);
		}
	}
}

void BGBBTJ_BC7_DecodeImageBC7(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	BGBBTJ_BC7_DecodeImageBC7I(block, rgba, xs, ys, stride, 0);
}

void BGBBTJ_BC7_DecodeImageBC7_BGRA(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	BGBBTJ_BC7_DecodeImageBC7I(block, rgba, xs, ys, stride, 1);
}
