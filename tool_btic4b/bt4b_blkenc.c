/*
 * 8x8 blocks: Nominal 384 Bits, Ext=512 bits
 * 
 * Bytes 0-15: Color Vector
 *    0: Tag
 *    4: Cy:S16
 *    6: Cu:S16
 *    8: Cv:S16
 *   10: Dy:S16
 *   12: Du:S16
 *   14: Dv:S16
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

force_inline int lqtvq_clamp32767S(int x)
{
	return((x<(-32767))?(-32767):((x>32767)?32767:x));
}

void BTIC4B_SplitIbufBGRA(byte *ibuf, int ystr,
	int *ybuf, int *ubuf, int *vbuf, int *abuf,
	int *mcyuv, int *ncyuv)
{
	byte *cs0, *cs1;
	int *cty0, *cty1, *ctu, *ctv;
	int *cta0, *cta1;
	int i0, i1, i2, i3;
	int cr0, cr1, cr2, cr3;
	int cg0, cg1, cg2, cg3;
	int cb0, cb1, cb2, cb3;
	int cy0, cy1, cy2, cy3;
	int ca0, ca1, ca2, ca3;
	int mcy, mcu, mcv, ncy, ncu, ncv;
	int mca, nca;
	int cr, cg, cb;
	int cy, cu, cv;
	
	int i, j, k;
	
	mcy=255; mcu= 255; mcv= 255;
	ncy=  0; ncu=-255; ncv=-255;
	mca=255; nca=-255;
	ctu=ubuf;	ctv=vbuf;
	cty0=ybuf;	cty1=ybuf+8;
	cta0=abuf;	cta1=abuf+8;
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
			ca0=(byte)(i0>>24);	ca1=(byte)(i1>>24);
			ca2=(byte)(i2>>24);	ca3=(byte)(i3>>24);
			cs0+=8;		cs1+=8;

			cg=cg0;
			cr=cr0;
			cb=cb0;

			cy0=cg0;	cy1=cg1;
			cy2=cg2;	cy3=cg3;
//			cu=((cb-cg)>>1)+128;
//			cv=((cr-cg)>>1)+128;
			cu=cb-cg;
			cv=cr-cg;

			cty0[0]=cy0; 	cty0[1]=cy1;
			cty1[0]=cy2; 	cty1[1]=cy3;
			*ctu++=cu;		*ctv++=cv;
			cty0+=2;		cty1+=2;

#if 1
			cta0[0]=ca0; 	cta0[1]=ca1;
			cta1[0]=ca2; 	cta1[1]=ca3;
			cta0+=2;		cta1+=2;
#endif

#if 1
//			if((ca0|ca1|ca2|ca3) || (mcy>ncy))
			if(((ca0&ca1&ca2&ca3)==(ca0|ca1|ca2|ca3)) && (ca0>=8))
			{
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
			}else if((ca0+ca1+ca2+ca3)>=8)
			{
				if(ca0>=8)
				{	if(cy0<mcy)mcy=cy0;
					if(cy0>ncy)ncy=cy0;	}
				if(ca1>=8)
				{	if(cy3<mcy)mcy=cy3;
					if(cy3>ncy)ncy=cy3;	}
				if(ca0>=8)
				{	if(cy1<mcy)mcy=cy1;
					if(cy1>ncy)ncy=cy1;	}
				if(ca0>=8)
				{	if(cy2<mcy)mcy=cy2;
					if(cy2>ncy)ncy=cy2;	}
				if(cu<mcu)mcu=cu;
				if(cu>ncu)ncu=cu;
				if(cv<mcv)mcv=cv;
				if(cv>ncv)ncv=cv;
			}else
			{
			}
#endif

#if 1
			if(ca0<mca)mca=ca0;
			if(ca0>nca)nca=ca0;
			if(ca3<mca)mca=ca3;
			if(ca3>nca)nca=ca3;
			if(ca1<mca)mca=ca1;
			if(ca1>nca)nca=ca1;
			if(ca2<mca)mca=ca2;
			if(ca2>nca)nca=ca2;
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
		cty0+=8;	cty1+=8;
		cta0+=8;	cta1+=8;
	}
	
	if(mcy>ncy)
	{
		mcy=cy0; ncy=cy0;
		mcu=cu; ncu=cu;
		mcv=cv; ncv=cv;
	}
	
	mcyuv[0]=mcy;	mcyuv[1]=mcu;	mcyuv[2]=mcv;
	ncyuv[0]=ncy;	ncyuv[1]=ncu;	ncyuv[2]=ncv;
	mcyuv[3]=mca;	ncyuv[3]=nca;
}

void BTIC4B_SplitIbufBGRX(byte *ibuf, int ystr,
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
	int ca0, ca1, ca2, ca3;
	int mcy, mcu, mcv, ncy, ncu, ncv;
	int mca, nca;
	int cr, cg, cb;
	int cy, cu, cv;
	
	int i, j, k;
	
	mcy=255; mcu= 255; mcv= 255;
	ncy=  0; ncu=-255; ncv=-255;
	mca=255; nca=-255;
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
//			cu=((cb-cg)>>1)+128;
//			cv=((cr-cg)>>1)+128;
			cu=cb-cg;
			cv=cr-cg;

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
		cty0+=8;	cty1+=8;
	}
	
	if(mcy>ncy)
	{
		mcy=cy0; ncy=cy0;
		mcu=cu; ncu=cu;
		mcv=cv; ncv=cv;
	}
	
	mcyuv[0]=mcy;	mcyuv[1]=mcu;	mcyuv[2]=mcv;
	ncyuv[0]=ncy;	ncyuv[1]=ncu;	ncyuv[2]=ncv;
	mcyuv[3]=255;	ncyuv[3]=255;
}


void BTIC4B_SplitIbufRGB30(byte *ibuf, int ystr,
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
	
	mcy=1023; mcu= 1023; mcv= 1023;
	ncy=   0; ncu=-1023; ncv=-1023;
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

//			cu=((cb-cg)>>1)+512;
//			cv=((cr-cg)>>1)+512;
			cu=cb-cg;
			cv=cr-cg;

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
	mcyuv[3]=255;	ncyuv[3]=255;
}

void BTIC4B_SplitIbufRGB48(byte *ibuf, int ystr,
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
	
	mcy= 32767; mcu= 32767; mcv= 32767;
	ncy=-32767; ncu=-32767; ncv=-32767;
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

//			cg=cg0;
//			cr=cr0;
//			cb=cb0;

			cg=(cg0+cg1+cg2+cg3)>>2;
			cr=(cr0+cr1+cr2+cr3)>>2;
			cb=(cb0+cb1+cb2+cb3)>>2;
			
			cy0=cg0;	cy1=cg1;
			cy2=cg2;	cy3=cg3;

//			cu=((cb-cg)>>1)+32768;
//			cv=((cr-cg)>>1)+32768;
			cu=cb-cg;
			cv=cr-cg;

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
	mcyuv[3]=255;	ncyuv[3]=255;
}

int lqtvq_fsctab[256];
byte lqtvq_fsctab_init=0;

void BTIC4B_InitScTables()
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

#if 1

#if 1
//force_inline int BTIC4B_SAT1(int x)
//{
//	static const int tab[8]=
//		{0,0,0,0,1,1,1,1};
//	return(tab[x+3]);
//	return((x<0)?0:((x>1)?1:x));
//}

static const int lqtvq_sat1_tab[8]=
		{0,0,0,0, 1,1,1,1};
#define BTIC4B_SAT1(x)	lqtvq_sat1_tab[(x)+3]

static const int lqtvq_sat2_tab[8]=
		{0,0,0,1, 2,3,3,3};
#define BTIC4B_SAT2(x)	lqtvq_sat2_tab[(x)+2]

#else
#define BTIC4B_SAT1(x)	(x)
#define BTIC4B_SAT2(x)	(x)
#endif

// #define BTIC4B_BIAS1		8191
// #define BTIC4B_BIAS1		16383
#define BTIC4B_BIAS1			65535
// #define BTIC4B_BIAS1			131071
// #define BTIC4B_BIAS1			262143
// #define BTIC4B_BIAS1			((1<<20)-1)

force_inline void BTIC4B_EncUVBits2x2x1(
	byte *oblk, int *ubuf, int lsu, int acu)
{
	static const int lc2=8388608+4095;
	static const int lc3=8388608-4095;
	int u0, u1, u2, u3;
	u32 i2;

	u0=(ubuf[ 0]+ubuf[ 1]+ubuf[ 4]+ubuf[ 5])>>2;
	u1=(ubuf[ 2]+ubuf[ 3]+ubuf[ 6]+ubuf[ 7])>>2;
	u2=(ubuf[ 8]+ubuf[ 9]+ubuf[12]+ubuf[13])>>2;
	u3=(ubuf[10]+ubuf[11]+ubuf[14]+ubuf[15])>>2;
	i2=        BTIC4B_SAT1(((u3-acu)*lsu+lc2)>>23);
	i2=(i2<<1)|BTIC4B_SAT1(((u2-acu)*lsu+lc2)>>23);
	i2=(i2<<1)|BTIC4B_SAT1(((u1-acu)*lsu+lc2)>>23);
	i2=(i2<<1)|BTIC4B_SAT1(((u0-acu)*lsu+lc2)>>23);
	*(byte *)oblk=i2;
}

force_inline void BTIC4B_EncUVBits4x4x1(
	byte *oblk, int *ubuf, int lsu, int acu)
{
	static const int lc2=8388608+BTIC4B_BIAS1;
	static const int lc3=8388608-BTIC4B_BIAS1;
	u32 i2;

	i2=        BTIC4B_SAT1(((ubuf[15]-acu)*lsu+lc2)>>23);
	i2=(i2<<1)|BTIC4B_SAT1(((ubuf[14]-acu)*lsu+lc3)>>23);
	i2=(i2<<1)|BTIC4B_SAT1(((ubuf[13]-acu)*lsu+lc2)>>23);
	i2=(i2<<1)|BTIC4B_SAT1(((ubuf[12]-acu)*lsu+lc3)>>23);
	i2=(i2<<1)|BTIC4B_SAT1(((ubuf[11]-acu)*lsu+lc3)>>23);
	i2=(i2<<1)|BTIC4B_SAT1(((ubuf[10]-acu)*lsu+lc2)>>23);
	i2=(i2<<1)|BTIC4B_SAT1(((ubuf[ 9]-acu)*lsu+lc3)>>23);
	i2=(i2<<1)|BTIC4B_SAT1(((ubuf[ 8]-acu)*lsu+lc2)>>23);
	i2=(i2<<1)|BTIC4B_SAT1(((ubuf[ 7]-acu)*lsu+lc2)>>23);
	i2=(i2<<1)|BTIC4B_SAT1(((ubuf[ 6]-acu)*lsu+lc3)>>23);
	i2=(i2<<1)|BTIC4B_SAT1(((ubuf[ 5]-acu)*lsu+lc2)>>23);
	i2=(i2<<1)|BTIC4B_SAT1(((ubuf[ 4]-acu)*lsu+lc3)>>23);
	i2=(i2<<1)|BTIC4B_SAT1(((ubuf[ 3]-acu)*lsu+lc3)>>23);
	i2=(i2<<1)|BTIC4B_SAT1(((ubuf[ 2]-acu)*lsu+lc2)>>23);
	i2=(i2<<1)|BTIC4B_SAT1(((ubuf[ 1]-acu)*lsu+lc3)>>23);
	i2=(i2<<1)|BTIC4B_SAT1(((ubuf[ 0]-acu)*lsu+lc2)>>23);
	*(u16 *)oblk=i2;
}

force_inline void BTIC4B_EncYBits2x2x1(
	byte *oblk, int *ybuf, int lsy, int acy)
{
	static const int lc0=8388608+BTIC4B_BIAS1;
	static const int lc1=8388608-BTIC4B_BIAS1;
	int y0, y1, y2, y3;
	u32 i0, i1;
	int i;

	y0=(ybuf[ 0]+ybuf[ 2]+ybuf[16]+ybuf[18])>>2;
	y1=(ybuf[ 4]+ybuf[ 6]+ybuf[20]+ybuf[22])>>2;
	y2=(ybuf[32]+ybuf[34]+ybuf[48]+ybuf[50])>>2;
	y3=(ybuf[36]+ybuf[38]+ybuf[52]+ybuf[54])>>2;

	i0=        BTIC4B_SAT1(((y3-acy)*lsy+lc0)>>23);
	i0=(i0<<1)|BTIC4B_SAT1(((y2-acy)*lsy+lc1)>>23);
	i0=(i0<<1)|BTIC4B_SAT1(((y1-acy)*lsy+lc1)>>23);
	i0=(i0<<1)|BTIC4B_SAT1(((y0-acy)*lsy+lc1)>>23);
	*(byte *)oblk=i0;
}


force_inline void BTIC4B_EncYBits4x2x1(
	byte *oblk, int *ybuf, int lsy, int acy)
{
	static const int lc0=8388608+BTIC4B_BIAS1;
	static const int lc1=8388608-BTIC4B_BIAS1;
	u32 i0, i1;
	int i;

	i0=        BTIC4B_SAT1(((ybuf[38]-acy)*lsy+lc0)>>23);
	i0=(i0<<1)|BTIC4B_SAT1(((ybuf[36]-acy)*lsy+lc1)>>23);
	i0=(i0<<1)|BTIC4B_SAT1(((ybuf[34]-acy)*lsy+lc0)>>23);
	i0=(i0<<1)|BTIC4B_SAT1(((ybuf[32]-acy)*lsy+lc1)>>23);
	i0=(i0<<1)|BTIC4B_SAT1(((ybuf[ 6]-acy)*lsy+lc1)>>23);
	i0=(i0<<1)|BTIC4B_SAT1(((ybuf[ 4]-acy)*lsy+lc0)>>23);
	i0=(i0<<1)|BTIC4B_SAT1(((ybuf[ 2]-acy)*lsy+lc1)>>23);
	i0=(i0<<1)|BTIC4B_SAT1(((ybuf[ 0]-acy)*lsy+lc0)>>23);
	*(byte *)oblk=i0;
}

force_inline void BTIC4B_EncYBits2x4x1(
	byte *oblk, int *ybuf, int lsy, int acy)
{
	static const int lc0=8388608+BTIC4B_BIAS1;
	static const int lc1=8388608-BTIC4B_BIAS1;
	u32 i0, i1;
	int i;

	i0=        BTIC4B_SAT1(((ybuf[52]-acy)*lsy+lc0)>>23);
	i0=(i0<<1)|BTIC4B_SAT1(((ybuf[48]-acy)*lsy+lc0)>>23);
	i0=(i0<<1)|BTIC4B_SAT1(((ybuf[36]-acy)*lsy+lc0)>>23);
	i0=(i0<<1)|BTIC4B_SAT1(((ybuf[32]-acy)*lsy+lc0)>>23);
	i0=(i0<<1)|BTIC4B_SAT1(((ybuf[20]-acy)*lsy+lc1)>>23);
	i0=(i0<<1)|BTIC4B_SAT1(((ybuf[16]-acy)*lsy+lc1)>>23);
	i0=(i0<<1)|BTIC4B_SAT1(((ybuf[ 4]-acy)*lsy+lc1)>>23);
	i0=(i0<<1)|BTIC4B_SAT1(((ybuf[ 0]-acy)*lsy+lc1)>>23);
	*(byte *)oblk=i0;
}

force_inline void BTIC4B_EncYBits4x4x1(
	byte *oblk, int *ybuf, int lsy, int acy)
{
	static const int lc0=8388608+BTIC4B_BIAS1;
	static const int lc1=8388608-BTIC4B_BIAS1;
	u32 i0, i1;
	int i;

	i0=        BTIC4B_SAT1(((ybuf[54]-acy)*lsy+lc0)>>23);
	i0=(i0<<1)|BTIC4B_SAT1(((ybuf[52]-acy)*lsy+lc1)>>23);
	i0=(i0<<1)|BTIC4B_SAT1(((ybuf[50]-acy)*lsy+lc0)>>23);
	i0=(i0<<1)|BTIC4B_SAT1(((ybuf[48]-acy)*lsy+lc1)>>23);
	i0=(i0<<1)|BTIC4B_SAT1(((ybuf[38]-acy)*lsy+lc0)>>23);
	i0=(i0<<1)|BTIC4B_SAT1(((ybuf[36]-acy)*lsy+lc1)>>23);
	i0=(i0<<1)|BTIC4B_SAT1(((ybuf[34]-acy)*lsy+lc0)>>23);
	i0=(i0<<1)|BTIC4B_SAT1(((ybuf[32]-acy)*lsy+lc1)>>23);
	i0=(i0<<1)|BTIC4B_SAT1(((ybuf[22]-acy)*lsy+lc1)>>23);
	i0=(i0<<1)|BTIC4B_SAT1(((ybuf[20]-acy)*lsy+lc0)>>23);
	i0=(i0<<1)|BTIC4B_SAT1(((ybuf[18]-acy)*lsy+lc1)>>23);
	i0=(i0<<1)|BTIC4B_SAT1(((ybuf[16]-acy)*lsy+lc0)>>23);
	i0=(i0<<1)|BTIC4B_SAT1(((ybuf[ 6]-acy)*lsy+lc1)>>23);
	i0=(i0<<1)|BTIC4B_SAT1(((ybuf[ 4]-acy)*lsy+lc0)>>23);
	i0=(i0<<1)|BTIC4B_SAT1(((ybuf[ 2]-acy)*lsy+lc1)>>23);
	i0=(i0<<1)|BTIC4B_SAT1(((ybuf[ 0]-acy)*lsy+lc0)>>23);
	*(u16 *)oblk=i0;
}

force_inline void BTIC4B_EncYBits4x8x1(
	byte *oblk, int *ybuf, int lsy, int acy)
{
	static const int lc0=8388608+BTIC4B_BIAS1;
	static const int lc1=8388608-BTIC4B_BIAS1;
	int *csy;
	u32 i0, i1;
	int i;

	csy=ybuf;
	for(i=0; i<2; i++)
	{
		i0=        BTIC4B_SAT1(((csy[30]-acy)*lsy+lc0)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[28]-acy)*lsy+lc1)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[26]-acy)*lsy+lc0)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[24]-acy)*lsy+lc1)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[22]-acy)*lsy+lc1)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[20]-acy)*lsy+lc0)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[18]-acy)*lsy+lc1)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[16]-acy)*lsy+lc0)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[14]-acy)*lsy+lc0)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[12]-acy)*lsy+lc1)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[10]-acy)*lsy+lc0)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[ 8]-acy)*lsy+lc1)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[ 6]-acy)*lsy+lc1)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[ 4]-acy)*lsy+lc0)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[ 2]-acy)*lsy+lc1)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[ 0]-acy)*lsy+lc0)>>23);
		*(u16 *)(oblk+i*2)=i0;
		csy+=32;
	}
}

force_inline void BTIC4B_EncYBits8x4x1(
	byte *oblk, int *ybuf, int lsy, int acy)
{
	static const int lc0=8388608+BTIC4B_BIAS1;
	static const int lc1=8388608-BTIC4B_BIAS1;
	int *csy;
	u32 i0, i1;
	int i;

	csy=ybuf;
	for(i=0; i<2; i++)
	{
		i0=        BTIC4B_SAT1(((csy[23]-acy)*lsy+lc0)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[22]-acy)*lsy+lc1)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[21]-acy)*lsy+lc0)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[20]-acy)*lsy+lc1)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[19]-acy)*lsy+lc0)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[18]-acy)*lsy+lc1)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[17]-acy)*lsy+lc0)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[16]-acy)*lsy+lc1)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[ 7]-acy)*lsy+lc1)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[ 6]-acy)*lsy+lc0)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[ 5]-acy)*lsy+lc1)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[ 4]-acy)*lsy+lc0)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[ 3]-acy)*lsy+lc1)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[ 2]-acy)*lsy+lc0)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[ 1]-acy)*lsy+lc1)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[ 0]-acy)*lsy+lc0)>>23);
		*(u16 *)(oblk+i*2)=i0;
		csy+=32;
	}
}

force_inline void BTIC4B_EncYBits8x8x1(
	byte *oblk, int *ybuf, int lsy, int acy)
{
	static const int lc0=8388608+BTIC4B_BIAS1;
	static const int lc1=8388608-BTIC4B_BIAS1;
	int *csy;
	u32 i0, i1;
	int i;

	csy=ybuf;
	for(i=0; i<4; i++)
	{
		i0=        BTIC4B_SAT1(((csy[15]-acy)*lsy+lc0)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[14]-acy)*lsy+lc1)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[13]-acy)*lsy+lc0)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[12]-acy)*lsy+lc1)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[11]-acy)*lsy+lc0)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[10]-acy)*lsy+lc1)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[ 9]-acy)*lsy+lc0)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[ 8]-acy)*lsy+lc1)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[ 7]-acy)*lsy+lc1)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[ 6]-acy)*lsy+lc0)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[ 5]-acy)*lsy+lc1)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[ 4]-acy)*lsy+lc0)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[ 3]-acy)*lsy+lc1)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[ 2]-acy)*lsy+lc0)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[ 1]-acy)*lsy+lc1)>>23);
		i0=(i0<<1)|BTIC4B_SAT1(((csy[ 0]-acy)*lsy+lc0)>>23);
		*(u16 *)(oblk+i*2)=i0;
		csy+=16;
	}
}
#endif

#if 1
force_inline void BTIC4B_EncUVBits2x2x2(
	byte *oblk, int *ubuf, int lsu, int acu)
{
	static const int lc2=8388608+4095;
	static const int lc3=8388608-4095;
	int u0, u1, u2, u3;
	u32 i2;

	u0=(ubuf[ 0]+ubuf[ 1]+ubuf[ 4]+ubuf[ 5])>>2;
	u1=(ubuf[ 2]+ubuf[ 3]+ubuf[ 6]+ubuf[ 7])>>2;
	u2=(ubuf[ 8]+ubuf[ 9]+ubuf[12]+ubuf[13])>>2;
	u3=(ubuf[10]+ubuf[11]+ubuf[14]+ubuf[15])>>2;
	i2=        BTIC4B_SAT2(((u3-acu)*lsu+lc2)>>22);
	i2=(i2<<2)|BTIC4B_SAT2(((u2-acu)*lsu+lc2)>>22);
	i2=(i2<<2)|BTIC4B_SAT2(((u1-acu)*lsu+lc2)>>22);
	i2=(i2<<2)|BTIC4B_SAT2(((u0-acu)*lsu+lc2)>>22);
	*(byte *)oblk=i2;
}

force_inline void BTIC4B_EncUVBits4x4x2(
	byte *oblk, int *ubuf, int lsu, int acu)
{
	static const int lc2=8388608+4095;
	static const int lc3=8388608-4095;
	u32 i2;

	i2=        BTIC4B_SAT2(((ubuf[15]-acu)*lsu+lc2)>>22);
	i2=(i2<<2)|BTIC4B_SAT2(((ubuf[14]-acu)*lsu+lc3)>>22);
	i2=(i2<<2)|BTIC4B_SAT2(((ubuf[13]-acu)*lsu+lc2)>>22);
	i2=(i2<<2)|BTIC4B_SAT2(((ubuf[12]-acu)*lsu+lc3)>>22);
	i2=(i2<<2)|BTIC4B_SAT2(((ubuf[11]-acu)*lsu+lc3)>>22);
	i2=(i2<<2)|BTIC4B_SAT2(((ubuf[10]-acu)*lsu+lc2)>>22);
	i2=(i2<<2)|BTIC4B_SAT2(((ubuf[ 9]-acu)*lsu+lc3)>>22);
	i2=(i2<<2)|BTIC4B_SAT2(((ubuf[ 8]-acu)*lsu+lc2)>>22);
	i2=(i2<<2)|BTIC4B_SAT2(((ubuf[ 7]-acu)*lsu+lc2)>>22);
	i2=(i2<<2)|BTIC4B_SAT2(((ubuf[ 6]-acu)*lsu+lc3)>>22);
	i2=(i2<<2)|BTIC4B_SAT2(((ubuf[ 5]-acu)*lsu+lc2)>>22);
	i2=(i2<<2)|BTIC4B_SAT2(((ubuf[ 4]-acu)*lsu+lc3)>>22);
	i2=(i2<<2)|BTIC4B_SAT2(((ubuf[ 3]-acu)*lsu+lc3)>>22);
	i2=(i2<<2)|BTIC4B_SAT2(((ubuf[ 2]-acu)*lsu+lc2)>>22);
	i2=(i2<<2)|BTIC4B_SAT2(((ubuf[ 1]-acu)*lsu+lc3)>>22);
	i2=(i2<<2)|BTIC4B_SAT2(((ubuf[ 0]-acu)*lsu+lc2)>>22);
	*(u32 *)oblk=i2;
}

force_inline void BTIC4B_EncUVBits4x4x4(
	byte *oblk, int *ubuf, int lsu, int acu)
{
	static const int lc2=8388608+4095;
	static const int lc3=8388608-4095;
	u32 i0, i1;

	i1=        (((ubuf[15]-acu)*lsu+lc2)>>20);
	i1=(i1<<4)|(((ubuf[14]-acu)*lsu+lc3)>>20);
	i1=(i1<<4)|(((ubuf[13]-acu)*lsu+lc2)>>20);
	i1=(i1<<4)|(((ubuf[12]-acu)*lsu+lc3)>>20);
	i1=(i1<<4)|(((ubuf[11]-acu)*lsu+lc3)>>20);
	i1=(i1<<4)|(((ubuf[10]-acu)*lsu+lc2)>>20);
	i1=(i1<<4)|(((ubuf[ 9]-acu)*lsu+lc3)>>20);
	i1=(i1<<4)|(((ubuf[ 8]-acu)*lsu+lc2)>>20);
	i0=        (((ubuf[ 7]-acu)*lsu+lc2)>>20);
	i0=(i0<<4)|(((ubuf[ 6]-acu)*lsu+lc3)>>20);
	i0=(i0<<4)|(((ubuf[ 5]-acu)*lsu+lc2)>>20);
	i0=(i0<<4)|(((ubuf[ 4]-acu)*lsu+lc3)>>20);
	i0=(i0<<4)|(((ubuf[ 3]-acu)*lsu+lc3)>>20);
	i0=(i0<<4)|(((ubuf[ 2]-acu)*lsu+lc2)>>20);
	i0=(i0<<4)|(((ubuf[ 1]-acu)*lsu+lc3)>>20);
	i0=(i0<<4)|(((ubuf[ 0]-acu)*lsu+lc2)>>20);
	*(u32 *)(oblk+0)=i0;
	*(u32 *)(oblk+4)=i1;
}

force_inline void BTIC4B_EncYBits2x2x2(
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

	i0=        BTIC4B_SAT2(((y3-acy)*lsy+lc0)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((y2-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((y1-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((y0-acy)*lsy+lc1)>>22);
	*(u32 *)oblk=i0;
}


force_inline void BTIC4B_EncYBits4x2x2(
	byte *oblk, int *ybuf, int lsy, int acy)
{
	static const int lc0=8388608+4095;
	static const int lc1=8388608-4095;
	u32 i0, i1;
	int i;

	i0=        BTIC4B_SAT2(((ybuf[38]-acy)*lsy+lc0)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((ybuf[36]-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((ybuf[34]-acy)*lsy+lc0)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((ybuf[32]-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((ybuf[ 6]-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((ybuf[ 4]-acy)*lsy+lc0)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((ybuf[ 2]-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((ybuf[ 0]-acy)*lsy+lc0)>>22);

	*(u16 *)oblk=i0;
}

force_inline void BTIC4B_EncYBits2x4x2(
	byte *oblk, int *ybuf, int lsy, int acy)
{
	static const int lc0=8388608+4095;
	static const int lc1=8388608-4095;
	u32 i0, i1;
	int i;

	i0=        BTIC4B_SAT2(((ybuf[52]-acy)*lsy+lc0)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((ybuf[48]-acy)*lsy+lc0)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((ybuf[36]-acy)*lsy+lc0)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((ybuf[32]-acy)*lsy+lc0)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((ybuf[20]-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((ybuf[16]-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((ybuf[ 4]-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((ybuf[ 0]-acy)*lsy+lc1)>>22);
	*(u16 *)oblk=i0;
}

force_inline void BTIC4B_EncYBits4x4x2(
	byte *oblk, int *ybuf, int lsy, int acy)
{
	static const int lc0=8388608+4095;
	static const int lc1=8388608-4095;
	int y2buf[16];
	u32 i0, i1;
	int i;
	
#if 1
	y2buf[ 0]=(ybuf[ 0]+ybuf[ 1]+ybuf[ 8]+ybuf[ 9])>>2;
	y2buf[ 1]=(ybuf[ 2]+ybuf[ 3]+ybuf[10]+ybuf[11])>>2;
	y2buf[ 2]=(ybuf[ 4]+ybuf[ 5]+ybuf[12]+ybuf[13])>>2;
	y2buf[ 3]=(ybuf[ 6]+ybuf[ 7]+ybuf[14]+ybuf[15])>>2;
	y2buf[ 4]=(ybuf[16]+ybuf[17]+ybuf[24]+ybuf[25])>>2;
	y2buf[ 5]=(ybuf[18]+ybuf[19]+ybuf[26]+ybuf[27])>>2;
	y2buf[ 6]=(ybuf[20]+ybuf[21]+ybuf[28]+ybuf[29])>>2;
	y2buf[ 7]=(ybuf[22]+ybuf[23]+ybuf[30]+ybuf[31])>>2;
	y2buf[ 8]=(ybuf[32]+ybuf[33]+ybuf[40]+ybuf[41])>>2;
	y2buf[ 9]=(ybuf[34]+ybuf[35]+ybuf[42]+ybuf[43])>>2;
	y2buf[10]=(ybuf[36]+ybuf[37]+ybuf[44]+ybuf[45])>>2;
	y2buf[11]=(ybuf[38]+ybuf[39]+ybuf[46]+ybuf[47])>>2;
	y2buf[12]=(ybuf[48]+ybuf[49]+ybuf[56]+ybuf[56])>>2;
	y2buf[13]=(ybuf[50]+ybuf[51]+ybuf[58]+ybuf[59])>>2;
	y2buf[14]=(ybuf[52]+ybuf[53]+ybuf[60]+ybuf[61])>>2;
	y2buf[15]=(ybuf[54]+ybuf[55]+ybuf[62]+ybuf[62])>>2;

	i0=        BTIC4B_SAT2(((y2buf[15]-acy)*lsy+lc0)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((y2buf[14]-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((y2buf[13]-acy)*lsy+lc0)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((y2buf[12]-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((y2buf[11]-acy)*lsy+lc0)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((y2buf[10]-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((y2buf[ 9]-acy)*lsy+lc0)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((y2buf[ 8]-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((y2buf[ 7]-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((y2buf[ 6]-acy)*lsy+lc0)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((y2buf[ 5]-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((y2buf[ 4]-acy)*lsy+lc0)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((y2buf[ 3]-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((y2buf[ 2]-acy)*lsy+lc0)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((y2buf[ 1]-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((y2buf[ 0]-acy)*lsy+lc0)>>22);
	*(u32 *)oblk=i0;
#endif

#if 0
	i0=        BTIC4B_SAT2(((ybuf[54]-acy)*lsy+lc0)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((ybuf[52]-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((ybuf[50]-acy)*lsy+lc0)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((ybuf[48]-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((ybuf[38]-acy)*lsy+lc0)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((ybuf[36]-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((ybuf[34]-acy)*lsy+lc0)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((ybuf[32]-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((ybuf[22]-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((ybuf[20]-acy)*lsy+lc0)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((ybuf[18]-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((ybuf[16]-acy)*lsy+lc0)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((ybuf[ 6]-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((ybuf[ 4]-acy)*lsy+lc0)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((ybuf[ 2]-acy)*lsy+lc1)>>22);
	i0=(i0<<2)|BTIC4B_SAT2(((ybuf[ 0]-acy)*lsy+lc0)>>22);
	*(u32 *)oblk=i0;
#endif
}

force_inline void BTIC4B_EncYBits4x8x2(
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
		i0=        BTIC4B_SAT2(((csy[30]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[28]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[26]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[24]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[22]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[20]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[18]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[16]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[14]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[12]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[10]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[ 8]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[ 6]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[ 4]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[ 2]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[ 0]-acy)*lsy+lc0)>>22);
		*(u32 *)(oblk+i*4)=i0;
		csy+=32;
	}
}

force_inline void BTIC4B_EncYBits8x4x2(
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
		i0=        BTIC4B_SAT2(((csy[23]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[22]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[21]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[20]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[19]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[18]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[17]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[16]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[ 7]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[ 6]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[ 5]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[ 4]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[ 3]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[ 2]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[ 1]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[ 0]-acy)*lsy+lc0)>>22);
		*(u32 *)(oblk+i*4)=i0;
		csy+=32;
	}
}

force_inline void BTIC4B_EncYBits8x8x2(
	byte *oblk, int *ybuf, int lsy, int acy)
{
//	static const int lc0=8388608+4095;
//	static const int lc1=8388608-4095;
	static const int lc0=8388608+1023;
	static const int lc1=8388608-1023;
	int *csy;
	u32 i0, i1;
	int i;

	csy=ybuf;
	for(i=0; i<4; i++)
	{
		i0=        BTIC4B_SAT2(((csy[15]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[14]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[13]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[12]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[11]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[10]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[ 9]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[ 8]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[ 7]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[ 6]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[ 5]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[ 4]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[ 3]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[ 2]-acy)*lsy+lc0)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[ 1]-acy)*lsy+lc1)>>22);
		i0=(i0<<2)|BTIC4B_SAT2(((csy[ 0]-acy)*lsy+lc0)>>22);
		*(u32 *)(oblk+i*4)=i0;
		csy+=16;
	}
}

force_inline void BTIC4B_EncYBits8x8x3(
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
//	for(i=3; i>=0; i--)
	for(i=0; i<4; i++)
	{
//		csy=ybuf+i*16;

		i1=        (((csy[15]-acy)*lsy+lc1)>>21);
		i1=(i1<<3)|(((csy[14]-acy)*lsy+lc0)>>21);
		i1=(i1<<3)|(((csy[13]-acy)*lsy+lc1)>>21);
		i1=(i1<<3)|(((csy[12]-acy)*lsy+lc0)>>21);
		i1=(i1<<3)|(((csy[11]-acy)*lsy+lc1)>>21);
		i1=(i1<<3)|(((csy[10]-acy)*lsy+lc0)>>21);
		i1=(i1<<3)|(((csy[ 9]-acy)*lsy+lc1)>>21);
		i1=(i1<<3)|(((csy[ 8]-acy)*lsy+lc0)>>21);
		i0=        (((csy[ 7]-acy)*lsy+lc0)>>21);
		i0=(i0<<3)|(((csy[ 6]-acy)*lsy+lc1)>>21);
		i0=(i0<<3)|(((csy[ 5]-acy)*lsy+lc0)>>21);
		i0=(i0<<3)|(((csy[ 4]-acy)*lsy+lc1)>>21);
		i0=(i0<<3)|(((csy[ 3]-acy)*lsy+lc0)>>21);
		i0=(i0<<3)|(((csy[ 2]-acy)*lsy+lc1)>>21);
		i0=(i0<<3)|(((csy[ 1]-acy)*lsy+lc0)>>21);
		i0=(i0<<3)|(((csy[ 0]-acy)*lsy+lc1)>>21);
		*(u32 *)(oblk+i*6+0)=i0;
		*(u32 *)(oblk+i*6+3)=i1;
		csy+=16;
	}
}

force_inline void BTIC4B_EncYBits8x8x4(
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
		i1=        (((csy[15]-acy)*lsy+lc1)>>20);
		i1=(i1<<4)|(((csy[14]-acy)*lsy+lc0)>>20);
		i1=(i1<<4)|(((csy[13]-acy)*lsy+lc1)>>20);
		i1=(i1<<4)|(((csy[12]-acy)*lsy+lc0)>>20);
		i1=(i1<<4)|(((csy[11]-acy)*lsy+lc1)>>20);
		i1=(i1<<4)|(((csy[10]-acy)*lsy+lc0)>>20);
		i1=(i1<<4)|(((csy[ 9]-acy)*lsy+lc1)>>20);
		i1=(i1<<4)|(((csy[ 8]-acy)*lsy+lc0)>>20);
		i0=        (((csy[ 7]-acy)*lsy+lc0)>>20);
		i0=(i0<<4)|(((csy[ 6]-acy)*lsy+lc1)>>20);
		i0=(i0<<4)|(((csy[ 5]-acy)*lsy+lc0)>>20);
		i0=(i0<<4)|(((csy[ 4]-acy)*lsy+lc1)>>20);
		i0=(i0<<4)|(((csy[ 3]-acy)*lsy+lc0)>>20);
		i0=(i0<<4)|(((csy[ 2]-acy)*lsy+lc1)>>20);
		i0=(i0<<4)|(((csy[ 1]-acy)*lsy+lc0)>>20);
		i0=(i0<<4)|(((csy[ 0]-acy)*lsy+lc1)>>20);
		*(u32 *)(oblk+i*8+0)=i0;
		*(u32 *)(oblk+i*8+4)=i1;
		csy+=16;
	}
}
#endif

void BTIC4B_EncBlock0Inner(
	BTIC4B_Context *ctx,
	byte *blkbuf,
	int *ybuf, int *ubuf, int *vbuf,
	int *mcyuv, int *ncyuv)
{
//	int *csy;
	int aby[16];
	int amcy[4], ancy[4];
	int mbyl, nbyl, mbyr, nbyr, mby, nby;
	int mbyu, nbyu, mbyd, nbyd;
	int dbyh, dbyv, dcy1, dcy2, dbyhvp;
	int dey, dpey, dcenexp, alexp;
	int mcy, mcu, mcv, ncy, ncu, ncv;
	int acy, acu, acv, dcy, dcu, dcv;
	int mca, nca, dcuv;
	int ls0, ls0b, ls1, ls2, lc0, lc1, lc2, lc3;
	int l0, l1, l2, l3;
	int i0, i1, i2, i3;
	int i, j, k, l;
	
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

	dcenexp=0;

	mca=mcyuv[3];
	nca=ncyuv[3];
	alexp=((mca!=nca)&&(mca<240))||(mca>>8);

#if 1
	dey=0;
	for(i=0; i<2; i++)
		for(j=0; j<2; j++)
	{
		k=i*4*8+j*4;
		l0=(ybuf[k+ 0]+ybuf[k+ 1]+ybuf[k+ 8]+ybuf[k+ 9])>>2;
		l1=(ybuf[k+ 2]+ybuf[k+ 3]+ybuf[k+10]+ybuf[k+11])>>2;
		l2=(ybuf[k+16]+ybuf[k+17]+ybuf[k+24]+ybuf[k+25])>>2;
		l3=(ybuf[k+18]+ybuf[k+19]+ybuf[k+26]+ybuf[k+27])>>2;
		
		mby=l0; nby=l0;
		if(l3<mby) { mby=l3; }
		if(l3>nby) { nby=l3; }
		if(l1<mby) { mby=l1; }
		if(l1>nby) { nby=l1; }
		if(l2<mby) { mby=l2; }
		if(l2>nby) { nby=l2; }

#if 1
		lc0=l0-acy;	lc1=l1-acy;
		lc2=l2-acy;	lc3=l3-acy;
		dey+=lc0*lc0;	dey+=lc1*lc1;
		dey+=lc2*lc2;	dey+=lc3*lc3;
#endif

		k=i*2+j;
		amcy[k]=mby;
		ancy[k]=nby;
	}

#if 1
//	if(dcy>8)
	if(1)
	{
		dpey=(dcy*dcy*ctx->qdce_sc)>>4;

//		dpey=(dcy>>2)*(dcy>>2)*16;
//		dpey=(dcy>>1)*(dcy>>1)*16;
//		dpey=(dcy>>1)*(dcy>>2)*16;
		dcenexp=dey>dpey;
//		dey=dey/64;
//		dey=dey/(dcy>>1);
//		dcenexp=dey>(dcy>>1);
	}
#endif
	

	mbyl=amcy[0];	nbyl=ancy[0];
	mbyr=amcy[1];	nbyr=ancy[1];
	if(amcy[2]<mbyl) { mbyl=amcy[2]; }
	if(ancy[2]>nbyl) { nbyl=ancy[2]; }
	if(amcy[3]<mbyr) { mbyr=amcy[3]; }
	if(ancy[3]>nbyr) { nbyr=ancy[3]; }

	mbyu=amcy[0];	nbyu=ancy[0];
	mbyd=amcy[2];	nbyd=ancy[2];
	if(amcy[1]<mbyu) { mbyu=amcy[1]; }
	if(ancy[1]>nbyu) { nbyu=ancy[1]; }
	if(amcy[3]<mbyd) { mbyd=amcy[3]; }
	if(ancy[3]>nbyd) { nbyd=ancy[3]; }

	dbyh=nbyu-mbyu;	dbyv=nbyl-mbyl;
	j=nbyd-mbyd;	k=nbyr-mbyr;
	if(j>dbyh)dbyh=j;
	if(k>dbyv)dbyv=k;

	dcy1=(dcy*3)>>2;
//	dcy1=(dcy*7)>>3;
	dbyhvp=(dbyh<dcy1)|(dbyv<dcy1);
	dcy2=(dcy*5)>>3;
#endif

//	*(u32 *)(blkbuf+ 0)=  0x0B;
//	dcenexp=1;
//	dcenexp=0;

	if(dcenexp)
	{
//		k=1;
//		dcy=(dcy*3)>>2;
//		dcu=(dcu*3)>>2;
//		dcv=(dcv*3)>>2;
//		dcy=(dcy*7)>>3;
//		dcu=(dcu*7)>>3;
//		dcv=(dcv*7)>>3;
	}

	*(s16 *)(blkbuf+ 4)=acy;
	*(s16 *)(blkbuf+ 6)=acu;
	*(s16 *)(blkbuf+ 8)=acv;
	*(s16 *)(blkbuf+10)=dcy;
	*(s16 *)(blkbuf+12)=dcu;
	*(s16 *)(blkbuf+14)=dcv;

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
	
	dcuv=dcu|dcv;
//	if((dcu+dcv)>ctx->qduv_flat)
//	if((dcu|dcv)>ctx->qduv_flat)
	if(dcuv>ctx->qduv_flat)
	{
		if(dcuv<ctx->qduv_2x2)
		{
#if 1
			if(dcy<ctx->qdy_2x2x2)
			{
				*(u32 *)(blkbuf+ 0)=  0x14|0x00FF0000;
				BTIC4B_EncYBits2x2x2(blkbuf+16, ybuf, ls0, acy);
				BTIC4B_EncUVBits2x2x1(blkbuf+18, ubuf, ls1, acu);
				BTIC4B_EncUVBits2x2x1(blkbuf+19, vbuf, ls2, acv);
				blkbuf[17]=blkbuf[18]|(blkbuf[19]<<4);
				return;
			}
#endif

#if 1
			if(dcy<ctx->qdy_4x4x2)
			{
				*(u32 *)(blkbuf+ 0)=  0x15|0x00FF0000;
				BTIC4B_EncYBits4x4x2(blkbuf+16, ybuf, ls0, acy);
				BTIC4B_EncUVBits2x2x2(blkbuf+20, ubuf, ls1, acu);
				BTIC4B_EncUVBits2x2x2(blkbuf+21, vbuf, ls2, acv);
				return;
			}
#endif

#if 1
			if((dcy<ctx->qdy_8x8x2) || dcenexp)
			{
				*(u32 *)(blkbuf+ 0)=  0x17|0x00FF0000;
				BTIC4B_EncYBits8x8x2(blkbuf+16, ybuf, ls0, acy);
				BTIC4B_EncUVBits2x2x2(blkbuf+32, ubuf, ls1, acu);
				BTIC4B_EncUVBits2x2x2(blkbuf+33, vbuf, ls2, acv);
				return;
			}
#endif
		}else
		{
#if 1
			if(dcy<ctx->qdy_4x4x2)
			{
				*(u32 *)(blkbuf+ 0)=  0x16|0x00FF0000;
				BTIC4B_EncYBits4x4x2(blkbuf+16, ybuf, ls0, acy);
				BTIC4B_EncUVBits4x4x2(blkbuf+20, ubuf, ls1, acu);
				BTIC4B_EncUVBits4x4x2(blkbuf+24, vbuf, ls2, acv);
				return;
			}
#endif
		}
		
//		if((dcy<ctx->qdy_8x8x2) || dcenexp || alexp)
		if((dcy<ctx->qdy_8x8x2) || dcenexp)
//		if((dcy<ctx->qdy_8x8x2) || dcenexp ||
//			((ctx->qfl&127)<80))
		{
			*(u32 *)(blkbuf+ 0)=  0x18|0x00FF0000;
			BTIC4B_EncYBits8x8x2(blkbuf+16, ybuf, ls0, acy);
			BTIC4B_EncUVBits4x4x2(blkbuf+32, ubuf, ls1, acu);
			BTIC4B_EncUVBits4x4x2(blkbuf+36, vbuf, ls2, acv);
			return;
		}
		
//		if((dcy<ctx->qdy_8x8x3) || alexp)
		if(1)
		{
			*(u32 *)(blkbuf+ 0)=  0x1C|0x00FF0000;
			BTIC4B_EncYBits8x8x3(blkbuf+16, ybuf, ls0, acy);
			BTIC4B_EncUVBits4x4x2(blkbuf+40, ubuf, ls1, acu);
			BTIC4B_EncUVBits4x4x2(blkbuf+44, vbuf, ls2, acv);
			return;
		}

		*(u32 *)(blkbuf+ 0)=  0x1D|0x00FF0000;
		BTIC4B_EncYBits8x8x4(blkbuf+16, ybuf, ls0, acy);
		BTIC4B_EncUVBits4x4x4(blkbuf+48, ubuf, ls1, acu);
		BTIC4B_EncUVBits4x4x4(blkbuf+56, vbuf, ls2, acv);
		return;
	}else
	{
		if(dcy<ctx->qdy_flat)
		{
			*(u32 *)(blkbuf+ 0)=  0x00|0x00FF0000;
			return;
		}
		
		if(dcy<ctx->qdy_2x2x2)
		{
			if(dcenexp)
//			if(0)
			{
				*(u32 *)(blkbuf+ 0)=  0x01|0x00FF0000;
				BTIC4B_EncYBits2x2x1(blkbuf+16, ybuf, ls0, acy);
				return;
			}
		
			*(u32 *)(blkbuf+ 0)=  0x09|0x00FF0000;
			BTIC4B_EncYBits2x2x2(blkbuf+16, ybuf, ls0, acy);
			return;
		}
		
		if(dbyhvp && (dcy1<=ctx->qdy_2x2x2))
		{
			if(dbyv<=dbyh)
			{
				if(dcenexp)
//				if(0)
				{
					*(u32 *)(blkbuf+ 0)=  0x02|0x00FF0000;
					BTIC4B_EncYBits4x2x1(blkbuf+16, ybuf, ls0, acy);
					return;
				}

				*(u32 *)(blkbuf+ 0)=  0x0A|0x00FF0000;
				BTIC4B_EncYBits4x2x2(blkbuf+16, ybuf, ls0, acy);
			}else
			{
				if(dcenexp)
//				if(0)
				{
					*(u32 *)(blkbuf+ 0)=  0x03|0x00FF0000;
					BTIC4B_EncYBits2x4x1(blkbuf+16, ybuf, ls0, acy);
					return;
				}

				*(u32 *)(blkbuf+ 0)=  0x0B|0x00FF0000;
				BTIC4B_EncYBits2x4x2(blkbuf+16, ybuf, ls0, acy);
			}
			return;
		}

		if(dcy<ctx->qdy_4x4x2)
		{
			if(dcenexp)
//			if(0)
			{
				*(u32 *)(blkbuf+ 0)=  0x04|0x00FF0000;
				BTIC4B_EncYBits4x4x1(blkbuf+16, ybuf, ls0, acy);
				return;
			}

			*(u32 *)(blkbuf+ 0)=  0x0C|0x00FF0000;
			BTIC4B_EncYBits4x4x2(blkbuf+16, ybuf, ls0, acy);
			return;
		}

#if 1
		if(dbyhvp && (dcy2<=ctx->qdy_4x4x2))
		{
			if(dbyv<=dbyh)
			{
				if(dcenexp)
//				if(0)
				{
					*(u32 *)(blkbuf+ 0)=  0x05|0x00FF0000;
					BTIC4B_EncYBits8x4x1(blkbuf+16, ybuf, ls0, acy);
					return;
				}

				*(u32 *)(blkbuf+ 0)=  0x0D|0x00FF0000;
				BTIC4B_EncYBits8x4x2(blkbuf+16, ybuf, ls0, acy);
			}else
			{
				if(dcenexp)
//				if(0)
				{
					*(u32 *)(blkbuf+ 0)=  0x06|0x00FF0000;
					BTIC4B_EncYBits4x8x1(blkbuf+16, ybuf, ls0, acy);
					return;
				}

				*(u32 *)(blkbuf+ 0)=  0x0E|0x00FF0000;
				BTIC4B_EncYBits4x8x2(blkbuf+16, ybuf, ls0, acy);
			}
			return;
		}
#endif

		if(dcy<ctx->qdy_8x8x2)
		{
			if(dcenexp)
//			if(0)
			{
				*(u32 *)(blkbuf+ 0)=  0x07|0x00FF0000;
				BTIC4B_EncYBits8x8x1(blkbuf+16, ybuf, ls0, acy);
				return;
			}

			*(u32 *)(blkbuf+ 0)=  0x0F|0x00FF0000;
			BTIC4B_EncYBits8x8x2(blkbuf+16, ybuf, ls0, acy);
			return;
		}

//		if(dcenexp ||
//			((ctx->qfl&127)<80))
		if(dcenexp || alexp)
//		if(dcenexp)
//		if(0)
		{
			*(u32 *)(blkbuf+ 0)=  0x0F|0x00FF0000;
			BTIC4B_EncYBits8x8x2(blkbuf+16, ybuf, ls0, acy);
			return;
		}

#if 1
		if((dcy<ctx->qdy_8x8x3) ||
			((ctx->qfl&127)<80))
		{
			*(u32 *)(blkbuf+ 0)=  0x10|0x00FF0000;
			BTIC4B_EncYBits8x8x3(blkbuf+16, ybuf, ls0, acy);
			return;
		}
#endif
		
		if(1)
		{
			*(u32 *)(blkbuf+ 0)=  0x11|0x00FF0000;
			BTIC4B_EncYBits8x8x4(blkbuf+16, ybuf, ls0b, acy);
			return;
		}
	}
}

void BTIC4B_EncBlockAInner(
	BTIC4B_Context *ctx,
	byte *blkbuf, int *abuf, int mca, int nca)
{
	int l0, l1, l2, l3;
	int lc0, lc1, lc2, lc3;
	int aca, dca;
	int ls0, dey, dcenexp;
	int i, j, k;

	if(mca==255)
		return;
	
	if((mca==nca) && (mca>=0) && (mca<=255))
	{
		blkbuf[1]=0;
		blkbuf[2]=mca;
		blkbuf[3]=0;
		return;
	}

	if((mca>=240) && (nca<=255))
	{
		blkbuf[1]=0;
		blkbuf[2]=255;
		blkbuf[3]=0;
		return;
	}

	aca=(mca+nca)>>1;
	dca=nca-mca;

	if(dca<ctx->qdy_flat)
	{
		if((aca>=0) && (aca<=255))
		{
			blkbuf[1]=0;
			blkbuf[2]=aca;
			blkbuf[3]=0;
			return;
		}
		blkbuf[1]=8;	blkbuf[2]=0;	blkbuf[3]=1;
		*(u16 *)(blkbuf+48+0)=aca;
		*(u16 *)(blkbuf+48+2)=0;
		return;
	}


	dey=0;
	for(i=0; i<2; i++)
		for(j=0; j<2; j++)
	{
		k=i*4*8+j*4;
		l0=(abuf[k+ 0]+abuf[k+ 1]+abuf[k+ 8]+abuf[k+ 9])>>2;
		l1=(abuf[k+ 2]+abuf[k+ 3]+abuf[k+10]+abuf[k+11])>>2;
		l2=(abuf[k+16]+abuf[k+17]+abuf[k+24]+abuf[k+25])>>2;
		l3=(abuf[k+18]+abuf[k+19]+abuf[k+26]+abuf[k+27])>>2;

		lc0=l0-aca;	lc1=l1-aca;
		lc2=l2-aca;	lc3=l3-aca;
		dey+=lc0*lc0;	dey+=lc1*lc1;
		dey+=lc2*lc2;	dey+=lc3*lc3;
	}

#if 1
//	if(dcy>8)
	if(1)
	{
		i=(dca*dca*ctx->qdce_sc)>>4;
		dcenexp=dey>i;
	}
#endif

	blkbuf[1]=8;
	blkbuf[2]=0;
	blkbuf[3]=1;
	*(u16 *)(blkbuf+48+0)=aca;
	*(u16 *)(blkbuf+48+2)=dca;

	if((nca-aca)>>8)
		{ ls0=8388608/(nca-aca+1); }
	else
		{ ls0=lqtvq_fsctab[nca-aca]; }

#if 1
	if(dca<ctx->qdy_2x2x2)
	{
		if(dcenexp)
		{
			blkbuf[1]=0x01;
			BTIC4B_EncYBits2x2x1(blkbuf+48+8, abuf, ls0, aca);
			return;
		}

		blkbuf[1]=0x04;
		BTIC4B_EncYBits2x2x2(blkbuf+48+8, abuf, ls0, aca);
		return;
	}
	
	if(dca<ctx->qdy_4x4x2)
	{
		if(dcenexp)
		{
			blkbuf[1]=0x02;
			BTIC4B_EncYBits4x4x1(blkbuf+48+8, abuf, ls0, aca);
			return;
		}

		blkbuf[1]=0x05;
		BTIC4B_EncYBits4x4x2(blkbuf+48+8, abuf, ls0, aca);
		return;
	}
#endif

//	if(1)
	if(dcenexp)
	{
		blkbuf[1]=0x03;
		BTIC4B_EncYBits8x8x1(blkbuf+48+8, abuf, ls0, aca);
		return;
	}

	blkbuf[1]=0x0C;
	blkbuf[2]=aca;
	blkbuf[3]=dca;
	BTIC4B_EncYBits8x8x2(blkbuf+48, abuf, ls0, aca);
	return;
}

void BTIC4B_EncBlockBGRA(BTIC4B_Context *ctx,
	byte *blkbuf, byte *ibuf, int ystr)
{
	int ybuf[64], ubuf[16], vbuf[16], abuf[64];
	int mcyuv[4], ncyuv[4];
	
	BTIC4B_SplitIbufBGRA(ibuf, ystr,
		ybuf, ubuf, vbuf, abuf,
		mcyuv, ncyuv);
	BTIC4B_EncBlock0Inner(ctx, blkbuf, ybuf, ubuf, vbuf, mcyuv, ncyuv);
	BTIC4B_EncBlockAInner(ctx, blkbuf, abuf, mcyuv[3], ncyuv[3]);
}

void BTIC4B_EncBlockBGRX(BTIC4B_Context *ctx,
	byte *blkbuf, byte *ibuf, int ystr)
{
	int ybuf[64], ubuf[16], vbuf[16];
	int mcyuv[4], ncyuv[4];
	
	BTIC4B_SplitIbufBGRX(ibuf, ystr,
		ybuf, ubuf, vbuf,
		mcyuv, ncyuv);
	BTIC4B_EncBlock0Inner(ctx, blkbuf, ybuf, ubuf, vbuf, mcyuv, ncyuv);
}

void BTIC4B_EncBlockRGBA(BTIC4B_Context *ctx,
	byte *blkbuf, byte *ibuf, int ystr)
{
	byte tbuf[128];
	int cu, cv, du, dv;
	int i, j;

	BTIC4B_EncBlockBGRA(ctx, tbuf, ibuf, ystr);
	cu=*(s16 *)(tbuf+ 6);
	cv=*(s16 *)(tbuf+ 8);
	du=*(s16 *)(tbuf+12);
	dv=*(s16 *)(tbuf+14);
	*(s16 *)(tbuf+ 6)=cv;
	*(s16 *)(tbuf+ 8)=cu;
	*(s16 *)(tbuf+12)=dv;
	*(s16 *)(tbuf+14)=du;
	memcpy(blkbuf, tbuf, 64);
}

void BTIC4B_EncBlockRGBX(BTIC4B_Context *ctx,
	byte *blkbuf, byte *ibuf, int ystr)
{
	byte tbuf[128];
	int cu, cv, du, dv;
	int i, j;

	BTIC4B_EncBlockBGRX(ctx, tbuf, ibuf, ystr);
	cu=*(s16 *)(tbuf+ 6);
	cv=*(s16 *)(tbuf+ 8);
	du=*(s16 *)(tbuf+12);
	dv=*(s16 *)(tbuf+14);
	*(s16 *)(tbuf+ 6)=cv;
	*(s16 *)(tbuf+ 8)=cu;
	*(s16 *)(tbuf+12)=dv;
	*(s16 *)(tbuf+14)=du;
	memcpy(blkbuf, tbuf, 64);
}

void BTIC4B_EncImageBGRA(BTIC4B_Context *ctx,
	byte *blks, byte *ibuf, int xs, int ys)
{
	byte *cs, *ct;
	int xs1, ys1, ystr;
	int bi;
	int i, j, k;
	
	BTIC4B_InitScTables();
	
	ystr=xs*4;
	xs1=xs>>3;
	ys1=ys>>3;
	
	for(i=0; i<ys1; i++)
	{
		cs=ibuf+(i*8)*ystr;
		ct=blks+((i*xs1)<<6);
		for(j=0; j<xs1; j++)
		{
			BTIC4B_EncBlockBGRA(ctx, ct, cs, ystr);
			ct+=64; cs+=32;
		}
	}
}

void BTIC4B_EncImageClrs(BTIC4B_Context *ctx,
	byte *blks, byte *ibuf, int xs, int ys, int clrs)
{
	void (*EncBlock)(BTIC4B_Context *ctx,
		byte *blkbuf, byte *ibuf, int ystr);
	byte *cs, *ct;
	int xs1, ys1, ystr;
	int bi;
	int i, j, k;
	
	BTIC4B_InitScTables();
	
	switch(clrs&127)
	{
	case BTIC4B_CLRS_RGBA:
		EncBlock=BTIC4B_EncBlockRGBA;
		break;
	case BTIC4B_CLRS_BGRA:
		EncBlock=BTIC4B_EncBlockBGRA;
		break;
	case BTIC4B_CLRS_RGBX:
		EncBlock=BTIC4B_EncBlockRGBX;
		break;
	case BTIC4B_CLRS_BGRX:
		EncBlock=BTIC4B_EncBlockRGBX;
		break;
	}
	
	ystr=xs*4;
	xs1=xs>>3;
	ys1=ys>>3;
	
	for(i=0; i<ys1; i++)
	{
		cs=ibuf+(i*8)*ystr;
		ct=blks+((i*xs1)<<6);
		for(j=0; j<xs1; j++)
		{
			EncBlock(ctx, ct, cs, ystr);
			ct+=64; cs+=32;
		}
	}
}