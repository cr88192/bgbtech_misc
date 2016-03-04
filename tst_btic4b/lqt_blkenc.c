/*
 * 8x8 blocks: Nominal 384 Bits, Ext=512 bits
 * 
 * Bytes 0-15: Color Vector
 *    0: Tag
 *    4: Cy:U16
 *    6: Cu:U16
 *    8: Cv:U16
 *   10: Dy:U16
 *   12: Du:U16
 *   14: Dv:U16
 * Bytes 16-39: Y Data (3 bpp)
 * Bytes 40-43: U Dava (2 bpp)
 * Bytes 44-47: V Dava (2 bpp)
 */

force_inline int lqtvq_fmin(int x, int y)
{
	return((x<y)?x:y);
#if 0
	int m, z;
	m=(x-y)>>31;
	z=(x&m)|(y&(~m));
	return(z);
#endif
}

force_inline int lqtvq_fmax(int x, int y)
{
	return((x>y)?x:y);
#if 0
	int m, z;
	m=(x-y)>>31;
	z=(x&(~m))|(y&(m));
	return(z);
#endif
}

force_inline int lqtvq_clamp255(int x)
{
	return((x<0)?0:((x>255)?255:x));
}

force_inline int lqtvq_clamp65535(int x)
{
	return((x<0)?0:((x>65535)?65535:x));
}

void LQTVQ_SplitIbufBGRA(byte *ibuf, int ystr,
	int *ybuf, int *ubuf, int *vbuf,
	int *mcyuv, int *ncyuv)
{
	byte *cs0, *cs1;
	int *cty0, *cty1, *ctu, *ctv;
	int i0, i1, i2, i3;
	int cr0, cr1, cr2, cr3;
	int cg0, cg1, cg2, cg3;
	int cb0, cb1, cb2, cb3;
	int cy0, cy1, cy2, cy3;
	int mcy, mcu, mcv, ncy, ncu, ncv;
	int cr, cg, cb;
	int cy, cu, cv;
	
	int i, j, k;
	
	mcy=255; mcu=255; mcv=255;
	ncy=  0; ncu=  0; ncv=  0;
	ctu=ubuf;	ctv=vbuf;
	cty0=ybuf;	cty1=ybuf+8;
	for(i=0; i<4; i++)
	{
		cs0=ibuf+(i*2+0)*ystr;
		cs1=ibuf+(i*2+1)*ystr;
		for(j=0; j<4; j++)
		{
			i0=((u32 *)cs0)[0];		i1=((u32 *)cs0)[1];
			i2=((u32 *)cs1)[0];		i3=((u32 *)cs1)[1];
			cr0=(byte)(i0>>16);	cg0=(byte)(i0>> 8);	cb0=(byte)(i0    );
			cr1=(byte)(i1>>16);	cg1=(byte)(i1>> 8);	cb1=(byte)(i1    );
			cr2=(byte)(i2>>16);	cg2=(byte)(i2>> 8);	cb2=(byte)(i2    );
			cr3=(byte)(i3>>16);	cg3=(byte)(i3>> 8);	cb3=(byte)(i3    );
			cs0+=8;		cs1+=8;

			cg=cg0;
			cr=cr0;
			cb=cb0;

			cy0=cg0;	cy1=cg1;
			cy2=cg2;	cy3=cg3;
			cu=((cb-cg)>>1)+128;
			cv=((cr-cg)>>1)+128;

			cty0[0]=cy0; 	cty0[1]=cy1;
			cty1[0]=cy2; 	cty1[1]=cy3;
			*ctu++=cu;		*ctv++=cv;
			cty0+=2;		cty1+=2;

#if 1
			if(cy0<mcy)mcy=cy0;
			if(cy0>ncy)ncy=cy0;
			if(cy3<mcy)mcy=cy3;
			if(cy3>ncy)ncy=cy3;
			if(cy1<mcy)mcy=cy1;
			if(cy1>ncy)ncy=cy1;
			if(cy2<mcy)mcy=cy2;
			if(cy2>ncy)ncy=cy2;
			if(cu<mcu)mcu=cu;
			if(cu>ncu)ncu=cu;
			if(cv<mcv)mcv=cv;
			if(cv>ncv)ncv=cv;
#endif

#if 0
			mcy=lqtvq_fmin(mcy, cy0);
			mcy=lqtvq_fmin(mcy, cy3);
			mcy=lqtvq_fmin(mcy, cy1);
			mcy=lqtvq_fmin(mcy, cy2);

			ncy=lqtvq_fmax(ncy, cy0);
			ncy=lqtvq_fmax(ncy, cy3);
			ncy=lqtvq_fmax(ncy, cy1);
			ncy=lqtvq_fmax(ncy, cy2);

			mcu=lqtvq_fmin(mcu, cu);
			mcv=lqtvq_fmin(mcv, cv);

			ncu=lqtvq_fmax(ncu, cu);
			ncv=lqtvq_fmax(ncv, cv);
#endif
		}
		cty0+=8;
		cty1+=8;
	}
	
	mcyuv[0]=mcy;	mcyuv[1]=mcu;	mcyuv[2]=mcv;
	ncyuv[0]=ncy;	ncyuv[1]=ncu;	ncyuv[2]=ncv;
}

void LQTVQ_SplitIbufRGB30(byte *ibuf, int ystr,
	int *ybuf, int *ubuf, int *vbuf,
	int *mcyuv, int *ncyuv)
{
	byte *cs0, *cs1;
	int *cty0, *cty1, *ctu, *ctv;
	int i0, i1, i2, i3;
	int cr0, cr1, cr2, cr3;
	int cg0, cg1, cg2, cg3;
	int cb0, cb1, cb2, cb3;
	int cy0, cy1, cy2, cy3;
	int mcy, mcu, mcv, ncy, ncu, ncv;
	int cr, cg, cb;
	int cy, cu, cv;
	
	int i, j, k;
	
	mcy=255; mcu=255; mcv=255;
	ncy=  0; ncu=  0; ncv=  0;
	ctu=ubuf;	ctv=vbuf;
	cty0=ybuf;	cty1=ybuf+8;
	for(i=0; i<4; i++)
	{
		cs0=ibuf+(i*2+0)*ystr;
		cs1=ibuf+(i*2+1)*ystr;
		for(j=0; j<4; j++)
		{
			i0=((u32 *)cs0)[0];		i1=((u32 *)cs0)[1];
			i2=((u32 *)cs1)[0];		i3=((u32 *)cs1)[1];
			cs0+=8;		cs1+=8;

			cr0=(i0>>20)&1023;	cg0=(i0>>8)&1023;	cb0=(i0)&1023;
			cr1=(i1>>20)&1023;	cg1=(i1>>8)&1023;	cb1=(i1)&1023;
			cr2=(i2>>20)&1023;	cg2=(i2>>8)&1023;	cb2=(i2)&1023;
			cr3=(i3>>20)&1023;	cg3=(i3>>8)&1023;	cb3=(i3)&1023;

			cg=cg0;
			cr=cr0;
			cb=cb0;

			cy0=cg0;	cy1=cg1;
			cy2=cg2;	cy3=cg3;

			cu=((cb-cg)>>1)+512;
			cv=((cr-cg)>>1)+512;

			cty0[0]=cy0; 	cty0[1]=cy1;
			cty1[0]=cy2; 	cty1[1]=cy3;
			*ctu++=cu;		*ctv++=cv;
			cty0+=2;		cty1+=2;
			
			mcy=lqtvq_fmin(mcy, cy0);
			mcy=lqtvq_fmin(mcy, cy3);
			mcy=lqtvq_fmin(mcy, cy1);
			mcy=lqtvq_fmin(mcy, cy2);

			ncy=lqtvq_fmax(ncy, cy0);
			ncy=lqtvq_fmax(ncy, cy3);
			ncy=lqtvq_fmax(ncy, cy1);
			ncy=lqtvq_fmax(ncy, cy2);

			mcu=lqtvq_fmin(mcu, cu);
			mcv=lqtvq_fmin(mcv, cv);

			ncu=lqtvq_fmax(ncu, cu);
			ncv=lqtvq_fmax(ncv, cv);
		}
		cty0+=8;
		cty1+=8;
	}
	
	mcyuv[0]=mcy;	mcyuv[1]=mcu;	mcyuv[2]=mcv;
	ncyuv[0]=ncy;	ncyuv[1]=ncu;	ncyuv[2]=ncv;
}

void LQTVQ_SplitIbufRGBA48(byte *ibuf, int ystr,
	int *ybuf, int *ubuf, int *vbuf,
	int *mcyuv, int *ncyuv)
{
	byte *cs0, *cs1;
	int *cty0, *cty1, *ctu, *ctv;
	int i0, i1, i2, i3;
	int cr0, cr1, cr2, cr3;
	int cg0, cg1, cg2, cg3;
	int cb0, cb1, cb2, cb3;
	int cy0, cy1, cy2, cy3;
	int mcy, mcu, mcv, ncy, ncu, ncv;
	int cr, cg, cb;
	int cy, cu, cv;
	
	int i, j, k;
	
	mcy=255; mcu=255; mcv=255;
	ncy=  0; ncu=  0; ncv=  0;
	ctu=ubuf;	ctv=vbuf;
	cty0=ybuf;	cty1=ybuf+8;
	for(i=0; i<4; i++)
	{
		cs0=ibuf+(i*2+0)*ystr;
		cs1=ibuf+(i*2+1)*ystr;
		for(j=0; j<4; j++)
		{
			cr0=((u16 *)cs0)[0]; cg0=((u16 *)cs0)[1]; cb0=((u16 *)cs0)[2];
			cr1=((u16 *)cs0)[3]; cg1=((u16 *)cs0)[4]; cb1=((u16 *)cs0)[5];
			cr2=((u16 *)cs1)[0]; cg2=((u16 *)cs1)[1]; cb2=((u16 *)cs1)[2];
			cr3=((u16 *)cs1)[3]; cg3=((u16 *)cs1)[4]; cb3=((u16 *)cs1)[5];
			cs0+=12;	cs1+=12;

			cg=cg0;
			cr=cr0;
			cb=cb0;
			
			cy0=cg0;	cy1=cg1;
			cy2=cg2;	cy3=cg3;

			cu=((cb-cg)>>1)+32768;
			cv=((cr-cg)>>1)+32768;

			cty0[0]=cy0; 	cty0[1]=cy1;
			cty1[0]=cy2; 	cty1[1]=cy3;
			*ctu++=cu;		*ctv++=cv;
			cty0+=2;		cty1+=2;
			
			mcy=lqtvq_fmin(mcy, cy0);
			mcy=lqtvq_fmin(mcy, cy3);
			mcy=lqtvq_fmin(mcy, cy1);
			mcy=lqtvq_fmin(mcy, cy2);

			ncy=lqtvq_fmax(ncy, cy0);
			ncy=lqtvq_fmax(ncy, cy3);
			ncy=lqtvq_fmax(ncy, cy1);
			ncy=lqtvq_fmax(ncy, cy2);

			mcu=lqtvq_fmin(mcu, cu);
			mcv=lqtvq_fmin(mcv, cv);

			ncu=lqtvq_fmax(ncu, cu);
			ncv=lqtvq_fmax(ncv, cv);
		}
		cty0+=8;
		cty1+=8;
	}
	
	mcyuv[0]=mcy;	mcyuv[1]=mcu;	mcyuv[2]=mcv;
	ncyuv[0]=ncy;	ncyuv[1]=ncu;	ncyuv[2]=ncv;
}

int lqtvq_fsctab[256];
byte lqtvq_fsctab_init=0;

void LQTVQ_InitScTables()
{
	int i;

	if(lqtvq_fsctab_init)
		return;
	lqtvq_fsctab_init=1;

	for(i=0; i<256; i++)
	{
//		lqtvq_fsctab[i]=(32768-1024)/(i+1);
//		lqtvq_fsctab[i]=32768/(i+1);
		lqtvq_fsctab[i]=8388608/(i+1);
	}
}

force_inline void LQTVQ_EncUVBits4x4x2(
	byte *oblk, int *ubuf, int lsu, int acu)
{
	static const int lc2=8388608+4095;
	static const int lc3=8388608-4095;
	u32 i2;

	i2=        (((ubuf[ 0]-acu)*lsu+lc2)>>22);
	i2=(i2<<2)|(((ubuf[ 1]-acu)*lsu+lc3)>>22);
	i2=(i2<<2)|(((ubuf[ 2]-acu)*lsu+lc2)>>22);
	i2=(i2<<2)|(((ubuf[ 3]-acu)*lsu+lc3)>>22);
	i2=(i2<<2)|(((ubuf[ 4]-acu)*lsu+lc3)>>22);
	i2=(i2<<2)|(((ubuf[ 5]-acu)*lsu+lc2)>>22);
	i2=(i2<<2)|(((ubuf[ 6]-acu)*lsu+lc3)>>22);
	i2=(i2<<2)|(((ubuf[ 7]-acu)*lsu+lc2)>>22);
	i2=(i2<<2)|(((ubuf[ 8]-acu)*lsu+lc2)>>22);
	i2=(i2<<2)|(((ubuf[ 9]-acu)*lsu+lc3)>>22);
	i2=(i2<<2)|(((ubuf[10]-acu)*lsu+lc2)>>22);
	i2=(i2<<2)|(((ubuf[11]-acu)*lsu+lc3)>>22);
	i2=(i2<<2)|(((ubuf[12]-acu)*lsu+lc3)>>22);
	i2=(i2<<2)|(((ubuf[13]-acu)*lsu+lc2)>>22);
	i2=(i2<<2)|(((ubuf[14]-acu)*lsu+lc3)>>22);
	i2=(i2<<2)|(((ubuf[15]-acu)*lsu+lc2)>>22);
	*(u32 *)oblk=i2;
}

force_inline void LQTVQ_EncYBits2x2x2(
	byte *oblk, int *ybuf, int lsy, int acy)
{
	static const int lc0=8388608+4095;
	static const int lc1=8388608-4095;
	int y0, y1, y2, y3;
	u32 i0, i1;
	int i;

	y0=(ybuf[ 0]+ybuf[ 2]+ybuf[16]+ybuf[18])>>2;
	y1=(ybuf[ 4]+ybuf[ 6]+ybuf[20]+ybuf[22])>>2;
	y2=(ybuf[32]+ybuf[34]+ybuf[48]+ybuf[50])>>2;
	y3=(ybuf[36]+ybuf[38]+ybuf[52]+ybuf[54])>>2;

	i0=        (((y0-acy)*lsy+lc0)>>22);
	i0=(i0<<2)|(((y1-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|(((y2-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|(((y3-acy)*lsy+lc1)>>22);
	*(u32 *)oblk=i0;
}

force_inline void LQTVQ_EncYBits4x4x2(
	byte *oblk, int *ybuf, int lsy, int acy)
{
	static const int lc0=8388608+4095;
	static const int lc1=8388608-4095;
	u32 i0, i1;
	int i;

	i0=        (((ybuf[ 0]-acy)*lsy+lc0)>>22);
	i0=(i0<<2)|(((ybuf[ 2]-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|(((ybuf[ 4]-acy)*lsy+lc0)>>22);
	i0=(i0<<2)|(((ybuf[ 6]-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|(((ybuf[16]-acy)*lsy+lc0)>>22);
	i0=(i0<<2)|(((ybuf[18]-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|(((ybuf[20]-acy)*lsy+lc0)>>22);
	i0=(i0<<2)|(((ybuf[22]-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|(((ybuf[32]-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|(((ybuf[34]-acy)*lsy+lc0)>>22);
	i0=(i0<<2)|(((ybuf[36]-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|(((ybuf[38]-acy)*lsy+lc0)>>22);
	i0=(i0<<2)|(((ybuf[48]-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|(((ybuf[50]-acy)*lsy+lc0)>>22);
	i0=(i0<<2)|(((ybuf[52]-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|(((ybuf[54]-acy)*lsy+lc0)>>22);
	*(u32 *)oblk=i0;
}

force_inline void LQTVQ_EncYBits4x8x2(
	byte *oblk, int *ybuf, int lsy, int acy)
{
	static const int lc0=8388608+4095;
	static const int lc1=8388608-4095;
	int *csy;
	u32 i0, i1;
	int i;

	csy=ybuf;
	for(i=0; i<2; i++)
	{
		i0=        (((csy[ 0]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|(((csy[ 2]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|(((csy[ 4]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|(((csy[ 6]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|(((csy[ 8]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|(((csy[10]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|(((csy[12]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|(((csy[14]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|(((csy[16]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|(((csy[18]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|(((csy[20]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|(((csy[22]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|(((csy[24]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|(((csy[26]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|(((csy[28]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|(((csy[30]-acy)*lsy+lc0)>>22);
		*(u32 *)(oblk+(1-i)*4)=i0;
		csy+=32;
	}
}

force_inline void LQTVQ_EncYBits8x8x2(
	byte *oblk, int *ybuf, int lsy, int acy)
{
	static const int lc0=8388608+4095;
	static const int lc1=8388608-4095;
	int *csy;
	u32 i0, i1;
	int i;

	csy=ybuf;
	for(i=0; i<4; i++)
	{
//		csy=ybuf+i*16;
		i0=        (((csy[ 0]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|(((csy[ 1]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|(((csy[ 2]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|(((csy[ 3]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|(((csy[ 4]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|(((csy[ 5]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|(((csy[ 6]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|(((csy[ 7]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|(((csy[ 8]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|(((csy[ 9]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|(((csy[10]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|(((csy[11]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|(((csy[12]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|(((csy[13]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|(((csy[14]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|(((csy[15]-acy)*lsy+lc0)>>22);
		*(u32 *)(oblk+(3-i)*4)=i0;
		csy+=16;
	}
}

force_inline void LQTVQ_EncYBits8x8x3(
	byte *oblk, int *ybuf, int lsy, int acy)
{
	static const int lc0=8388608+4095;
	static const int lc1=8388608-4095;
	int *csy, *csye;
	u32 i0, i1;
	int i;

	csy=ybuf; csye=ybuf+64;
//	for(i=0; i<4; i++)
//	while(csy<csye)
	for(i=3; i>=0; i--)
	{
		csy=ybuf+i*16;
		i0=        (((csy[ 0]-acy)*lsy+lc0)>>21);
		i0=(i0<<3)|(((csy[ 1]-acy)*lsy+lc1)>>21);
		i0=(i0<<3)|(((csy[ 2]-acy)*lsy+lc0)>>21);
		i0=(i0<<3)|(((csy[ 3]-acy)*lsy+lc1)>>21);
		i0=(i0<<3)|(((csy[ 4]-acy)*lsy+lc0)>>21);
		i0=(i0<<3)|(((csy[ 5]-acy)*lsy+lc1)>>21);
		i0=(i0<<3)|(((csy[ 6]-acy)*lsy+lc0)>>21);
		i0=(i0<<3)|(((csy[ 7]-acy)*lsy+lc1)>>21);
		i1=        (((csy[ 8]-acy)*lsy+lc1)>>21);
		i1=(i1<<3)|(((csy[ 9]-acy)*lsy+lc0)>>21);
		i1=(i1<<3)|(((csy[10]-acy)*lsy+lc1)>>21);
		i1=(i1<<3)|(((csy[11]-acy)*lsy+lc0)>>21);
		i1=(i1<<3)|(((csy[12]-acy)*lsy+lc1)>>21);
		i1=(i1<<3)|(((csy[13]-acy)*lsy+lc0)>>21);
		i1=(i1<<3)|(((csy[14]-acy)*lsy+lc1)>>21);
		i1=(i1<<3)|(((csy[15]-acy)*lsy+lc0)>>21);
		*(u32 *)(oblk+(3-i)*6+0)=i1;
		*(u32 *)(oblk+(3-i)*6+3)=i0;
//		csy+=16;
	}
}

force_inline void LQTVQ_EncYBits8x8x4(
	byte *oblk, int *ybuf, int lsy, int acy)
{
	static const int lc0=8388608+4095;
	static const int lc1=8388608-4095;
	int *csy, *csye;
	u32 i0, i1;
	int i;

	csy=ybuf; csye=ybuf+64;
	for(i=0; i<4; i++)
//	while(csy<csye)
	{
		i0=        (((csy[ 0]-acy)*lsy+lc0)>>20);
		i0=(i0<<4)|(((csy[ 1]-acy)*lsy+lc1)>>20);
		i0=(i0<<4)|(((csy[ 2]-acy)*lsy+lc0)>>20);
		i0=(i0<<4)|(((csy[ 3]-acy)*lsy+lc1)>>20);
		i0=(i0<<4)|(((csy[ 4]-acy)*lsy+lc0)>>20);
		i0=(i0<<4)|(((csy[ 5]-acy)*lsy+lc1)>>20);
		i0=(i0<<4)|(((csy[ 6]-acy)*lsy+lc0)>>20);
		i0=(i0<<4)|(((csy[ 7]-acy)*lsy+lc1)>>20);
		i1=        (((csy[ 8]-acy)*lsy+lc1)>>20);
		i1=(i1<<4)|(((csy[ 9]-acy)*lsy+lc0)>>20);
		i1=(i1<<4)|(((csy[10]-acy)*lsy+lc1)>>20);
		i1=(i1<<4)|(((csy[11]-acy)*lsy+lc0)>>20);
		i1=(i1<<4)|(((csy[12]-acy)*lsy+lc1)>>20);
		i1=(i1<<4)|(((csy[13]-acy)*lsy+lc0)>>20);
		i1=(i1<<4)|(((csy[14]-acy)*lsy+lc1)>>20);
		i1=(i1<<4)|(((csy[15]-acy)*lsy+lc0)>>20);
		*(u32 *)(oblk+(3-i)*8+4)=i0;
		*(u32 *)(oblk+(3-i)*8+0)=i1;
		csy+=16;
	}
}

void LQTVQ_EncBlock0Inner(
	BT4A_Context *ctx,
	byte *blkbuf,
	int *ybuf, int *ubuf, int *vbuf,
	int *mcyuv, int *ncyuv)
{
//	int *csy;
	int mcy, mcu, mcv, ncy, ncu, ncv;
	int acy, acu, acv, dcy, dcu, dcv;
	int ls0, ls0b, ls1, ls2, lc0, lc1, lc2, lc3;
	int i0, i1, i2, i3;
	int i;
	
	mcy=mcyuv[0];	mcu=mcyuv[1];	mcv=mcyuv[2];
	ncy=ncyuv[0];	ncu=ncyuv[1];	ncv=ncyuv[2];

//	mcu=128;	ncu=128;
//	mcv=128;	ncv=128;

	acy=(mcy+ncy)>>1;
	acu=(mcu+ncu)>>1;
	acv=(mcv+ncv)>>1;
	dcy=ncy-mcy;
	dcu=ncu-mcu;
	dcv=ncv-mcv;

//	*(u32 *)(blkbuf+ 0)=  0x0B;

	*(u16 *)(blkbuf+ 4)=acy;
	*(u16 *)(blkbuf+ 6)=acu;
	*(u16 *)(blkbuf+ 8)=acv;
	*(u16 *)(blkbuf+10)=dcy;
	*(u16 *)(blkbuf+12)=dcu;
	*(u16 *)(blkbuf+14)=dcv;

	if(((ncy-acy)|(ncu-acu)|(ncv-acv))>>8)
	{
		ls0=8388608/(ncy-acy+1);
		ls1=8388608/(ncu-acu+1);
		ls2=8388608/(ncv-acv+1);
	}else
	{
		ls0=lqtvq_fsctab[ncy-acy];
		ls1=lqtvq_fsctab[ncu-acu];
		ls2=lqtvq_fsctab[ncv-acv];
	}
	
//	ls0=(ls0*248)>>8;
//	ls0b=(ls0*248)>>8;
//	ls0b=(ls0*240)>>8;
	ls0b=ls0;
	ls0=(ls0*231)>>8;
//	ls1=(ls1*248)>>8;
//	ls2=(ls2*248)>>8;
	ls1=(ls1*230)>>8;
	ls2=(ls2*230)>>8;
	
	lc0=8388608+4095;
	lc1=8388608-4095;

	lc2=8388608+4095;
	lc3=8388608-4095;
	
//	if((dcu+dcv)>ctx->qduv_flat)
	if((dcu|dcv)>ctx->qduv_flat)
	{
		if(dcy<ctx->qdy_8x8x2)
		{
			*(u32 *)(blkbuf+ 0)=  0x18;
			LQTVQ_EncYBits8x8x2(blkbuf+16, ybuf, ls0, acy);
			LQTVQ_EncUVBits4x4x2(blkbuf+32, ubuf, ls1, acu);
			LQTVQ_EncUVBits4x4x2(blkbuf+36, vbuf, ls2, acv);
		}else
		{
			*(u32 *)(blkbuf+ 0)=  0x1C;
			LQTVQ_EncYBits8x8x3(blkbuf+16, ybuf, ls0, acy);
			LQTVQ_EncUVBits4x4x2(blkbuf+40, ubuf, ls1, acu);
			LQTVQ_EncUVBits4x4x2(blkbuf+44, vbuf, ls2, acv);
		}
	}else
	{
		if(dcy<ctx->qdy_flat)
		{
			*(u32 *)(blkbuf+ 0)=  0x00;
		}else if(dcy<ctx->qdy_2x2x2)
		{
			*(u32 *)(blkbuf+ 0)=  0x09;
			LQTVQ_EncYBits2x2x2(blkbuf+16, ybuf, ls0, acy);
		}else if(dcy<ctx->qdy_4x4x2)
		{
			*(u32 *)(blkbuf+ 0)=  0x0C;
			LQTVQ_EncYBits4x4x2(blkbuf+16, ybuf, ls0, acy);
		}else if(dcy<ctx->qdy_8x8x2)
		{
			*(u32 *)(blkbuf+ 0)=  0x0F;
			LQTVQ_EncYBits8x8x2(blkbuf+16, ybuf, ls0, acy);
		}else if(dcy<ctx->qdy_8x8x3)
		{
			*(u32 *)(blkbuf+ 0)=  0x13;
			LQTVQ_EncYBits8x8x3(blkbuf+16, ybuf, ls0, acy);
		}else
		{
			*(u32 *)(blkbuf+ 0)=  0x17;
			LQTVQ_EncYBits8x8x4(blkbuf+16, ybuf, ls0b, acy);
		}
	}
}

void LQTVQ_EncBlockBGRA(BT4A_Context *ctx,
	byte *blkbuf, byte *ibuf, int ystr)
{
	int ybuf[64], ubuf[16], vbuf[16];
	int mcyuv[4], ncyuv[4];
	
	LQTVQ_SplitIbufBGRA(ibuf, ystr, ybuf, ubuf, vbuf, mcyuv, ncyuv);
	LQTVQ_EncBlock0Inner(ctx, blkbuf, ybuf, ubuf, vbuf, mcyuv, ncyuv);
}

void LQTVQ_EncImageBGRA(BT4A_Context *ctx,
	byte *blks, byte *ibuf, int xs, int ys)
{
	byte *cs, *ct;
	int xs1, ys1, ystr;
	int bi;
	int i, j, k;
	
	LQTVQ_InitScTables();
	
	ystr=xs*4;
	xs1=xs>>3;
	ys1=ys>>3;
	
	for(i=0; i<ys1; i++)
	{
		cs=ibuf+(i*8)*ystr;
		ct=blks+((i*xs1)<<6);
		for(j=0; j<xs1; j++)
		{
			LQTVQ_EncBlockBGRA(ctx, ct, cs, ystr);
			ct+=64; cs+=32;
		}
	}
}
