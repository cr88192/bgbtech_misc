/*
 * Ye Olde FP Math code.
 * Primarily use FP8 (S.E4.F3) and FP16 (S.E5.M10)
 */

#ifdef __GNUC__
#define adnn2_memcpy4(ct, cs)	memcpy(ct, cs, 4)
#else
#define adnn2_memcpy4(ct, cs)	(*(volatile u32 *)(ct)=*(volatile u32 *)(cs))
#endif

u16 adnn2_fp8to16[256];
u32 adnn2_fp8to32[256];
float *adnn2_fp8to32f;

byte adnn2_fp16t10to8[1024];
byte adnn2_fp32t12to8[4096];

// byte adnn2_fp16t10to8a[1024];
// byte adnn2_fp16t10to8b[1024];

u16 adnn2_fp8mul16[256*256];

u32 adnn2_fp16to32hi[64];
u16 adnn2_fp32to16hi[512];

u16 adnn2_fp16t12ssqrt[4096];

u64 adnn2_fastrand_seed=0x31415927;

int adnn2_fastrand()
{
	u64 seed;
	int val;
	seed=adnn2_fastrand_seed;
	seed^=~(seed>>43);
	seed^=(seed>>11);
	seed^=(seed<<17);
	adnn2_fastrand_seed=seed;
	val=(seed>>13)^(seed>>29);
	return(val&32767);
}

/* random value, gaussian distribution, signed 8-bit range */
int adnn2_fastrand_gauss_s8()
{
	int x, y, z;
	z=adnn2_fastrand();
	x=((z>>0)&127)-64;
	y=((z>>7)&127)-64;
	z=x*y;
	return(z>>3);
}

/* random value, gaussian distribution, unsigned 8-bit range */
int adnn2_fastrand_gauss_u8()
{
	int x, y, z;
	z=adnn2_fastrand();
	x=(z>>0)&127;
	y=(z>>7)&127;
	z=x*y;
	return(z>>6);
}

int adnn2_fastrand_gauss_u2()
{
	return(adnn2_fastrand_gauss_u8()>>6);
}

u16 AdNn2_Fp8to16I(byte val)
{
	int ex, mt, sg, v;
	if(val==0x00)
		return(0x0000);
	if(val==0x80)
		return(0x7F00);
	ex=(val>>3)&15;
	mt=8|(val&7);
	sg=(val>>7)&1;
	ex=ex+(15-7);
	mt=mt<<7;
	v=(ex<<10)|(mt&1023)|(sg<<15);
	return(v);
}

u32 AdNn2_Fp16to32I(u16 val)
{
	u32 v;
	int ex, mt, sg;
	ex=(val>>10)&31;
	if(!ex)
		return(0);
	mt=1024|(val&1023);
	sg=(val>>15)&1;
	ex=ex+(127-15);
	mt=mt<<13;
	v=(ex<<23)|(mt&0x7FFFFF)|(sg<<31);
	return(v);
}

byte AdNn2_Fp16to8I(u16 val)
{
	u32 v, v1;
	int ex, mt, sg, ru;
	ex=(val>>10)&31;
	mt=1024|(val&1023);
	sg=(val>>15)&1;
	ex=ex-(15-7);
	ru=(mt>>6)&1;
	mt=mt>>7;
	if(ex<0)
		return(0x00);
	if(ex>15)
		return(0x7F|(sg<<7));
	v=(ex<<3)|(mt&7)|(sg<<7);
	if(v==0x80)		v=0;
	
	v1=v+1;
	if(ru && !((v^v1)&0x80))
		v=v1;

	return(v);
}

u16 AdNn2_Fp32to16I(u32 val)
{
	int ex, mt, sg, v;
	ex=(val>>23)&255;
	mt=0x800000|(val&0x7FFFFF);
	sg=(val>>31)&1;
	ex=ex-(127-15);
	mt=mt>>13;
	if(ex<0)
		return(0x0000);
	if(ex>=31)
		return(0x7C00|(sg<<15));
	v=(ex<<10)|(mt&1023)|(sg<<15);
	return(v);
}

u32 AdNn2_Fp8to32I(byte val)
{
	return(AdNn2_Fp16to32I(AdNn2_Fp8to16I(val)));
}

byte AdNn2_Fp32to8I(u32 val)
{
	return(AdNn2_Fp16to8I(AdNn2_Fp32to16I(val)));
}

byte AdNn2_Fp16to8(u16 val)
{
	return(adnn2_fp16t10to8[val>>6]);
}

byte AdNn2_Fp16to8Rs(u16 val)
{
	static u16 lfsr=0x67;
	u16 va, b;
//	b=(((lfsr>>5)^(lfsr>>13)^1)&1);
//	lfsr=(lfsr<<1)^b;

	lfsr=(lfsr<<1)^(~(lfsr>>13));
	lfsr^=lfsr<<11;
	lfsr^=lfsr>> 7;
	b=(lfsr>>8)&ADNN2_DO8RSM;

	va=val^(b<<6);
	return(adnn2_fp16t10to8[va>>6]);
}

u16 AdNn2_Fp32to16(u32 val)
{
	u16 vc;
	vc=adnn2_fp32to16hi[val>>23];
	if(vc)
		vc|=(val>>13)&1023;
	return(vc);
}

u16 AdNn2_F32to16(float v)
{
	u32 va;
	u16 vc;
//	memcpy(&va, &v, 4);
	adnn2_memcpy4(&va, &v);
	vc=adnn2_fp32to16hi[va>>23];
	if(vc)
		vc|=(va>>13)&1023;
	return(vc);
}

byte AdNn2_F32to8(float v)
{
	u32 va;
	byte vc;
//	memcpy(&va, &v, 4);
	adnn2_memcpy4(&va, &v);
	vc=adnn2_fp32t12to8[va>>20];
	return(vc);
}

float AdNn2_Fp8toF32(byte val)
{
	return(adnn2_fp8to32f[val]);
}

float AdNn2_Fp16toF32(u16 val)
{
	float f;
	u32 v;
	u16 tv;
//	if(!((val>>10)&31))
//		return(0);
	tv=val;
	v=adnn2_fp16to32hi[tv>>10];
	v|=((tv&1023)<<13);
//	memcpy(&f, &v, 4);
	adnn2_memcpy4(&f, &v);
	return(f);
}

u16 AdNn2_AddFp16(u16 va, u16 vb)
{
	u32 i_x, i_y, i_z;
	u16 vc;
	float x, y, z;
	
	i_x=adnn2_fp16to32hi[va>>10]|((va&1023)<<13);
	i_y=adnn2_fp16to32hi[vb>>10]|((vb&1023)<<13);
//	memcpy(&x, &i_x, 4);
	adnn2_memcpy4(&x, &i_x);
//	memcpy(&y, &i_y, 4);
	adnn2_memcpy4(&y, &i_y);
	z=x+y;
//	memcpy(&i_z, &z, 4);
	adnn2_memcpy4(&i_z, &z);
	vc=adnn2_fp32to16hi[i_z>>23];
	vc|=(i_z>>13)&1023;
	return(vc);
}

u16 AdNn2_MulFp16(u16 va, u16 vb)
{
	u32 i_x, i_y, i_z;
	u16 vc;
	float x, y, z;
	
	i_x=adnn2_fp16to32hi[va>>10]|((va&1023)<<13);
	i_y=adnn2_fp16to32hi[vb>>10]|((vb&1023)<<13);
//	memcpy(&x, &i_x, 4);
	adnn2_memcpy4(&x, &i_x);
//	memcpy(&y, &i_y, 4);
	adnn2_memcpy4(&y, &i_y);
	z=x*y;
//	memcpy(&i_z, &z, 4);
	adnn2_memcpy4(&i_z, &z);
	vc=adnn2_fp32to16hi[i_z>>23];
	vc|=(i_z>>13)&1023;
	return(vc);
}

float AdNn2_MulFp8toF32(byte va, byte vb)
{
	float f;
	
	f=adnn2_fp8to32f[va]*adnn2_fp8to32f[vb];
//	u32 v;
//	u16 tv;
//	tv=adnn2_fp8mul16[(va<<8)|vb];
//	v=adnn2_fp16to32hi[tv>>10]|((tv&1023)<<13);
//	memcpy(&f, &v, 4);
	return(f);
}

float AdNn2_MulFp16toF32(u16 va, u16 vb)
{
	float f;
	
	if(!((va>>10)&31) || !((vb>>10)&31))
		return(0);
	
	f=AdNn2_Fp16toF32(va)*AdNn2_Fp16toF32(vb);
	return(f);
}

u16 AdNn2_SSqrtFp16(u16 va)
{
	return(adnn2_fp16t12ssqrt[va>>4]);
//	if(!va)
//		return(0);
//	return((va&0x8000)|(((va&0x7FFF)>>1)+0x1E00));
}

u16 AdNn2_USqrtFp16(u16 va)
{
	if(va&0x8000)
		return(0x0000);
	return(adnn2_fp16t12ssqrt[va>>4]);
}

u16 AdNn2_SSqrt2Fp16(u16 va)
{
	u16 va1, va2;
	va1=adnn2_fp16t12ssqrt[va>>4];
	va2=adnn2_fp16t12ssqrt[va1>>4];
	return(va2);
}

u16 AdNn2_USqrt2Fp16(u16 va)
{
	u16 va1, va2;
	if(va&0x8000)
		return(0x0000);
	va1=adnn2_fp16t12ssqrt[va>>4];
	va2=adnn2_fp16t12ssqrt[va1>>4];
	return(va2);
}


u16 AdNn2_IdentFp16(u16 va)
{
	return(va);
}

u16 AdNn2_ReluFp16(u16 va)
{
	if(va&0x8000)
		return(0x0000);
	return(va);
}

u16 AdNn2_SaSSqrtFp16(u16 va)
{
	return((va&0x8000)|(((va&0x7FFF)>>1)+0x1E00));
}

u16 AdNn2_SbSSqrtFp16(u16 va)
{
	int vc;
	if(va&0x8000)
	{
		vc=adnn2_fp16t12ssqrt[va>>4];
		if(((vc>>10)&31)>4)
			vc-=4<<10;
//		return((va&0x8000)|(((va&0x7FFF)>>1)+0x0000));
	}
	return(adnn2_fp16t12ssqrt[va>>4]);
}

float AdNn2_SSqrtF32(float va)
{
	if(va<0)
		return(-sqrt(-va));
	return(sqrt(va));
}

/* Take the derivative of ssqrt(). */
float AdNn2_SSqrtDeriv(float fval)
{
	float afv, deriv;
	afv=fabs(fval);
//	if(afv<0.000001)
	if(afv<0.25)
		return(0.5);
	deriv=0.5/sqrt(afv);
	if(fval<0)
		deriv=-deriv;
	return(deriv);
}

float AdNn2_SSqrtDeriv2(float fval)
{
	float afv, deriv;
	afv=fabs(fval);
//	if(afv<0.000001)
//	if(afv<0.25)
//		return(0.5);
	deriv=0.5/sqrt(sqrt(1.0+afv));
	if(fval<0)
		deriv=-deriv;
	return(deriv);
}

/* Take the derivative of ssqrt() as a Binary16 number. */
u16 AdNn2_SSqrtDerivFp16(u16 val)
{
	return(AdNn2_F32to16(AdNn2_SSqrtDeriv(AdNn2_Fp16toF32(val))));
}

byte AdNn2_AdjustFp8scaleBias(byte val, int bi)
{
	int v1;
	
	if(!bi)
		return(val);
	
	if((val&0x7F)>=0x70)
		return(val-7);
//	if((val&0x7F)<=0x10)
//		return(val+7);
	
	v1=(val&0x7F)+bi;
	if((v1>=0x7F))
		return(val);
	if(v1<0x01)
		return(val^0x80);
	v1|=(val&0x80);
//	if(v1==0x80)v1=0x00;
	return(v1);
}

u16 AdNn2_AdjustFp16scaleBias(u16 val, int bi)
{
	static int frng;
	int v0, v1, vh;
	
	if(!bi)
		return(val);
	
	vh=val>>8;
	if(((vh&0x7F)>=0x70) && (bi>0))
		return(val-7);
//	if((vh&0x7F)<=0x10)
//		return(val+7);
	
	frng^=~(frng>>7);
	frng^=(frng<<17);
	
	if((!val) && (bi>0))
	{
		v1=0x0180|(frng&0x8000);
		return(v1);
	}
	
	v0=(val&0x7FFF);
//	v1=(val&0x7FFF)+bi;
//	v1=(val&0x7FFF)+(bi<<5)+((frng>>12)&31);
//	v1=(val&0x7FFF)+(bi<<5)+(((frng>>12)&31)-15);
//	v1=(val&0x7FFF)+(bi<<4)+(((frng>>12)&15)-7);
//	v1=(val&0x7FFF)+(bi<<3)+(((frng>>12)&7)-3);
	v1=(val&0x7FFF)+(bi<<2)+(((frng>>12)&3)-2);
//	v1=(val&0x7FFF)+bi;
	if((v1>=0x7C00))
		return(val);
//	if((v1<0x0400) && (v1<v0))
	if((v1<0x0100) && (v1<v0))
		return(val^0x8000);
	v1|=(val&0x8000);
	return(v1);
}

adnn2_wght AdNn2_AdjustWeightScaleBias(adnn2_wght val, int bi)
{
#ifdef ADNN2_WGHT16
	return(AdNn2_AdjustFp16scaleBias(val, bi));
#else
	return(AdNn2_AdjustFp8scaleBias(val, bi));
#endif
}

byte AdNn2_WeightToFp8Fast(adnn2_wght val)
{
#ifdef ADNN2_WGHT16
	return(val>>8);
#else
	return(val);
#endif
}

float AdNn2_WeightToF32(adnn2_wght wght)
{
#ifdef ADNN2_WGHT16
	return(AdNn2_Fp16toF32(wght));
#else
	return(AdNn2_Fp8toF32(wght));
#endif
}

adnn2_wght AdNn2_F32ToWeight(float val)
{
#ifdef ADNN2_WGHT16
	return(AdNn2_F32to16(val));
#else
	return(AdNn2_F32to8(val));
#endif
}

float AdNn2_MulWeightToF32(adnn2_wght wa, adnn2_wght wb)
{
#ifdef ADNN2_WGHT16
	return(AdNn2_MulFp16toF32(wa, wb));
#else
	return(AdNn2_MulFp8toF32(wa, wb));
#endif
}

adnn2_wght AdNn2_Fp8ToWeight(byte val)
{
#ifdef ADNN2_WGHT16
	return(adnn2_fp8to16[val]);
#else
	return(val);
#endif
}

adnn2_wght AdNn2_BreedWeight(adnn2_wght wa, adnn2_wght wb)
{
	int wf, nwf, wc, wd;
	int k;

	if(!((wa^wb)&ADNN2_WGHTSGNMSK))
	{
		wf=adnn2_fastrand()&255;
		nwf=256-wf;
		wc=((wa*nwf)+(wb*wf))>>8;
//		return(wc);

#if 0
//		wf=rand()&255;
		wf=(adnn2_fastrand()&511)-128;
		nwf=256-wf;
		wc=((wa*nwf)+(wb*wf))>>8;

		if(!((wa^wc)&ADNN2_WGHTSGNMSK))
		{
//			wf=rand()&255;
			wf=adnn2_fastrand()&255;
			nwf=256-wf;
			wc=((wa*nwf)+(wb*wf))>>8;
		}
#endif

#if 1
		if(!(adnn2_fastrand()&31))
		{
//			wd=wc+((adnn2_fastrand()&63)-31);
//			wd=wc+(adnn2_fastrand_gauss_s8()>>3);
			wd=wc+(adnn2_fastrand_gauss_s8()>>5);
			if(!((wc^wd)&ADNN2_WGHTSGNMSK))
				wc=wd;
		}
#endif

		return(wc);
	}else
	{
		if((wa&0x7FFF)>=(wb&0x7FFF))
		{
			wc=wa-((wb&0x7FFF)>>1);
		}else
		{
			wc=wb-((wa&0x7FFF)>>1);
		}

#if 1
		if(!(adnn2_fastrand()&31))
		{
//			wd=wc+((adnn2_fastrand()&63)-31);
//			wd=wc+(adnn2_fastrand_gauss_s8()>>2);
//			wd=wc+(adnn2_fastrand_gauss_s8()>>3);
			wd=wc+(adnn2_fastrand_gauss_s8()>>5);
			if(!((wc^wd)&ADNN2_WGHTSGNMSK))
				wc=wd;
		}
#endif

		return(wc);
	}
}

int AdNn2_Init()
{
	u32 i_x, i_y, i_z;
	float x, y, z;
	int i, j, k;
	
	if(adnn2_fp8to16[127]!=0)
		return(0);
	
	for(i=0; i<256; i++)
	{
		adnn2_fp8to16[i]=AdNn2_Fp8to16I(i);
		adnn2_fp8to32[i]=AdNn2_Fp8to32I(i);
	}
	
	adnn2_fp8to32f=(float *)adnn2_fp8to32;
	
	for(i=0; i<1024; i++)
	{
		adnn2_fp16t10to8[i]=AdNn2_Fp16to8I((i<<6)+31);

//		adnn2_fp16t10to8a[i]=AdNn2_Fp16to8I((i<<6)+ 0);
//		adnn2_fp16t10to8b[i]=AdNn2_Fp16to8I((i<<6)+63);
	}
	for(i=0; i<4096; i++)
		{ adnn2_fp32t12to8[i]=AdNn2_Fp32to8I(i<<20); }

	for(i=0; i<256; i++)
		for(j=0; j<256; j++)
	{
		i_x=adnn2_fp8to32[i];
		i_y=adnn2_fp8to32[j];
//		memcpy(&x, &i_x, 4);
		adnn2_memcpy4(&x, &i_x);
//		memcpy(&y, &i_y, 4);
		adnn2_memcpy4(&y, &i_y);
		z=x*y;
//		memcpy(&i_z, &z, 4);
		adnn2_memcpy4(&i_z, &z);
		adnn2_fp8mul16[i*256+j]=AdNn2_Fp32to16I(i_z);
	}

	for(i=0; i<64; i++)
	{
		adnn2_fp16to32hi[i]=AdNn2_Fp16to32I(i<<10);
	}

	for(i=0; i<512; i++)
	{
		adnn2_fp32to16hi[i]=AdNn2_Fp32to16I(i<<23);
	}
	
	for(i=0; i<4096; i++)
	{
		x=AdNn2_Fp16toF32((i<<4)+8);
		y=(x>=0)?(sqrt(x)):(-sqrt(-x));
		adnn2_fp16t12ssqrt[i]=AdNn2_F32to16(y);
	}
	
	x=3.14159;
	i=AdNn2_F32to8(x);
	y=AdNn2_Fp8toF32(i);
	printf("%02X %f %f\n", i, x, y);

	i=AdNn2_F32to16(x);
	j=AdNn2_Fp16to8(i);
	y=AdNn2_Fp16toF32(i);
	z=AdNn2_Fp8toF32(j);
	printf("%04X %f %f\n", i, x, y);
	printf("%02X %f %f\n", j, x, z);

	x=3.14159;
	i=AdNn2_F32to16(x);
	j=AdNn2_SSqrtFp16(i);
	y=AdNn2_Fp16toF32(i);
	z=AdNn2_Fp16toF32(j);
	printf("%02X %02X %f %f %f\n", i, j, x, y, z);

	x=-3.14159;
	i=AdNn2_F32to16(x);
	j=AdNn2_SSqrtFp16(i);
	y=AdNn2_Fp16toF32(i);
	z=AdNn2_Fp16toF32(j);
	printf("%02X %02X %f %f %f\n", i, j, x, y, z);

	x=1/3.14159;
	i=AdNn2_F32to16(x);
	j=AdNn2_SSqrtFp16(i);
	y=AdNn2_Fp16toF32(i);
	z=AdNn2_Fp16toF32(j);
	printf("%02X %02X %f %f %f\n", i, j, x, y, z);

	x=-1/3.14159;
	i=AdNn2_F32to16(x);
	j=AdNn2_SSqrtFp16(i);
	y=AdNn2_Fp16toF32(i);
	z=AdNn2_Fp16toF32(j);
	printf("%02X %02X %f %f %f\n", i, j, x, y, z);

	return(1);
}

