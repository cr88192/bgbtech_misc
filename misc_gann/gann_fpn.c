/*
 * S.E4.F3: FP8
 * S.E2.M1: FP4
 * S.E8.M7: FP16A (BF16)
 * S.E7.M8: FP16B
 */

byte gann_fp4to8[16]=
{
//	0x00, 0x30, 0x38, 0x3C,		//0.0. 0.5, 1.0, 1.5
//	0x40, 0x44, 0x48, 0x4C,		//2.0, 3.0, 4.0, 6.0
//	0x00, 0xB0, 0xB8, 0xBC,
//	0xC0, 0xC4, 0xC8, 0xCC

//	0x00, 0x28,	0x30, 0x34,		//0.00 0.25 0.50 0.75
//	0x38, 0x3C,	0x40, 0x44,		//1.00 1.50 2.00 3.00
//	0x80, 0xA8,	0xB0, 0xB4,		//0.00 0.25 0.50 0.75
//	0xB8, 0xBC,	0xC0, 0xC4,		//1.00 1.50 2.00 3.00

	0x00, 0x24, 0x28, 0x2C,		//0.00 0.19 0.25 0.38
	0x30, 0x34, 0x38, 0x3C,		//0.50 0.75 1.00 1.50
	0x00, 0xA4, 0xA8, 0xAC,		//0.00 0.19 0.25 0.38
	0xB0, 0xB4, 0xB8, 0xBC,		//0.50 0.75 1.00 1.50
};

byte gann_fp3to4[8]=
	{ 0, 2, 4, 7,  8, 10, 12, 15 };

// byte gann_fp3to8[8]=
//	{ 0x00, 0x38, 0x40, 0x4C, 0x80, 0xB8, 0xC0, 0xCC };

byte gann_fp8to4[256];
u16 gann_fp8to16[256];

u16 gann_fp16to8t[4096];

byte *gann_fp8mul8;
byte *gann_fp8add8;
byte *gann_fp4mul8;
byte *gann_fp4add8;

u16 *gann_fp8mul16;
float *gann_fp8mulf;

#ifdef __GNUC__
#define __debugbreak	__builtin_trap
#endif

byte GANN_MulFp8I(byte va, byte vb)
{
	byte fra, frb, frc;
	byte sga, sgb, sgc;
	sbyte exa, exb, exc;
	
	fra=8|(va&7); exa=(va>>3)&15; sga=(va>>7)&1;
	frb=8|(vb&7); exb=(vb>>3)&15; sgb=(vb>>7)&1;
	frc=fra*frb;
	exc=(exa+exb)-7;
	sgc=sga^sgb;
	frc>>=3;
	if(exc< 0)
		return(0x00);
	if(exc>15)
		return(0x7F|(sgc<<7));
	return((frc&7)|(exc<<3)|(sgc<<7));
}

byte GANN_AddFp8I(byte va, byte vb)
{
	byte fra, frb, frc;
	byte sga, sgb, sgc;
	sbyte exa, exb, exc;
	
	fra=8|(va&7); exa=(va>>3)&15; sga=(va>>7)&1;
	frb=8|(vb&7); exb=(vb>>3)&15; sgb=(vb>>7)&1;
	
	if(!(va&0x7F))
		fra=0;
	if(!(vb&0x7F))
		frb=0;
	
	if((va&0x7F)>=(vb&0x7F))
	{
		sgc=sga;
		exc=exa;
		if((exa-exb)>=4)
			return(va);
		if(sga==sgb)
			{ frc=fra+(frb>>(exa-exb)); }
		else
			{ frc=fra-(frb>>(exa-exb)); }
	}else
	{
		sgc=sgb;
		exc=exb;
		if((exb-exa)>=4)
			return(vb);
		if(sga==sgb)
			{ frc=frb+(fra>>(exb-exa)); }
		else
			{ frc=frb-(fra>>(exb-exa)); }
	}

	if(!frc)
		return(0);

	if(frc&0x80)
		{ sgc=!sgc; frc=-frc; }
	if(frc&0x10)
		{ exc++; frc=frc>>1; }
	if(!(frc&8))
		{ exc--; frc=frc<<1; }
	if(!(frc&8))
		{ exc--; frc=frc<<1; }
	if(!(frc&8))
		{ exc--; frc=frc<<1; }
	
	if(exc< 0)
		return(0x00);
	if(exc>15)
		return(0x7F|(sgc<<7));
	return((frc&7)|(exc<<3)|(sgc<<7));
}

byte GANN_CnvFp8ToFp4I(byte va)
{
	int ex, v;
	
	ex=(va>>3)&15;
	
	ex+=2;
	
	if(ex<6)
		return(0);
	if(ex>=10)
		return(((va>>4)&8)|7);
	v=	((va>>4)&8) |
		((ex-6)<<1) |
		((va>>2)&1) ;
	return(v);
}

u16 GANN_CnvFp8ToFp16I(byte va)
{
	int ex, v;

	if(!(va&0x7F))
		return(0x0000);
	ex=(va>>3)&15;
	ex+=(127-7);
	v=	((va<<8)&0x8000) |
		(ex<<7) |
		((va&7)<<4) ;
	return(v);
}

byte GANN_CnvFp16ToFp8I(u16 va)
{
	int sg, ex, fr, v;
	
	sg=(va>>15)&1;
	ex=(va>> 7)&255;
	fr=(va>> 0)&127;
	
	ex=ex-(127-7);

	fr=(fr+8)>>4;
	if(fr>7)
		fr=7;

	if(ex<0)
		return(0);
	if(ex>15)
		return(0x7F|(sg<<7));
	v=(sg<<7)|(ex<<3)|(fr&7);
	return(v);
}

u16 GANN_MulFp16I(u16 va, u16 vb)
{
	int fra, frb, frc;
	int sga, sgb, sgc;
	int exa, exb, exc;
	
	fra=128|(va&127); exa=(va>>7)&255; sga=(va>>15)&1;
	frb=128|(vb&127); exb=(vb>>7)&255; sgb=(vb>>15)&1;
	frc=fra*frb;
	exc=(exa+exb)-127;
	sgc=sga^sgb;
	frc>>=7;
	if(exc< 0)
		return(0x00);
	if(exc>255)
		return(0x7F80|(sgc<<15));
	return((frc&127)|(exc<<7)|(sgc<<15));
}

u16 GANN_AddFp16I(u16 va, u16 vb)
{
	int fra, frb, frc;
	int sga, sgb, sgc;
	int exa, exb, exc;
	
	fra=128|(va&127); exa=(va>>7)&255; sga=(va>>15)&1;
	frb=128|(vb&127); exb=(vb>>7)&255; sgb=(vb>>15)&1;
	
	if(!exa)
		fra=0;
	if(!exb)
		frb=0;
	
	if((va&0x7FFF)>=(vb&0x7FFF))
	{
		sgc=sga;
		exc=exa;
		if((exa-exb)>=8)
			return(va);
		if(sga==sgb)
			{ frc=fra+(frb>>(exa-exb)); }
		else
			{ frc=fra-(frb>>(exa-exb)); }
	}else
	{
		sgc=sgb;
		exc=exb;
		if((exb-exa)>=8)
			return(vb);
		if(sga==sgb)
			{ frc=frb+(fra>>(exb-exa)); }
		else
			{ frc=frb-(fra>>(exb-exa)); }
	}

	if(!frc)
		return(0x0000);

	if(frc<0)
		{ sgc=!sgc; frc=-frc; }
	if(frc&0x100)
		{ exc++; frc=frc>>1; }
	if(!(frc&0x80))
	{
		exc--; frc=frc<<1;
		if(!(frc&0x80))
		{
			exc--; frc=frc<<1;
			while(!(frc&0x80))
				{ exc--; frc=frc<<1; }
		}
	}
	
	if(exc<  0)
		return(0x0000);
	if(exc>255)
		return(0x7FFF|(sgc<<15));
	return((frc&127)|(exc<<7)|(sgc<<15));
}

u16 GANN_AddFp16J(u16 va, u16 vb)
{
	float x, y, z;
	u16 vc;
	*(u32 *)(&x)=((u32)va)<<16;
	*(u32 *)(&y)=((u32)vb)<<16;
	z=x+y;
	vc=(*(u32 *)(&z))>>16;
	return(vc);
}

float GANN_CnvFp16ToFloat(u16 va)
{
	float f;
	((u16 *)(&f))[1]=va;
	return(f);
}

u16 GANN_CnvFloatToFp16(float f)
{
	u16 v;
	v=((u16 *)(&f))[1];
	return(v);
}

int GANN_InitFp8Tab()
{
	int i, j, k;

	if(gann_fp8mul8)
		return(0);
	gann_fp8mul8=malloc(256*256);
	gann_fp8add8=malloc(256*256);

	gann_fp8mul16=malloc(256*256*2);
	gann_fp8mulf=malloc(256*256*sizeof(float));

	gann_fp4mul8=malloc(16*16);
	gann_fp4add8=malloc(16*16);
	
	for(i=0; i<256; i++)
		for(j=0; j<256; j++)
	{
		k=GANN_MulFp8I(i, j);
		gann_fp8mul8[i*256+j]=k;
		k=GANN_AddFp8I(i, j);
		gann_fp8add8[i*256+j]=k;
		
		k=GANN_MulFp16I(
			GANN_CnvFp8ToFp16I(i),
			GANN_CnvFp8ToFp16I(j));
		gann_fp8mul16[i*256+j]=k;
		gann_fp8mulf[i*256+j]=GANN_CnvFp16ToFloat(k);
	}
	
	for(i=0; i<16; i++)
		for(j=0; j<16; j++)
	{
		k=GANN_MulFp8I(gann_fp4to8[i], gann_fp4to8[j]);
		gann_fp4mul8[i*16+j]=k;
		k=GANN_AddFp8I(gann_fp4to8[i], gann_fp4to8[j]);
		gann_fp4add8[i*16+j]=k;
	}
	
	for(i=0; i<256; i++)
	{	
		gann_fp8to4[i]=GANN_CnvFp8ToFp4I(i);
		gann_fp8to16[i]=GANN_CnvFp8ToFp16I(i);
	}
	
	for(i=0; i<4096; i++)
	{	
		gann_fp16to8t[i]=GANN_CnvFp16ToFp8I(i<<4);
	}
	
	return(0);
}

byte GANN_WeightBit64X2(u64 bv, u64 wv0, u64 wv1)
{
	int i, j, k, b, w;
	
	k=0;
	for(i=0; i<64; i++)
	{
		b=(bv>>i)&1;
//		b=b*2-1;
		w=1+((wv0>>i)&1)*2;
		if((wv1>>i)&1)
			w=-w;
		k+=b*w;
	}

	if(k>=7)
		return(0x7);
	if(k<=-7)
		return(0xF);
	
//	if(k>=4)
//		return(0x4);
//	if(k<=-4)
//		return(0xC);
	if(k>=0)
		return(k);
	if(k<0)
		return(0x8+(-k));
	return(0);
	
//	if(k>0)
//		return(0x38);
//	if(k<0)
//		return(0xB8);
//	return(0x00);
}

byte GANN_WeightBit64X2_F8(u64 bv, u64 wv0, u64 wv1)
{
	int i, j, k, b, w;
	
	k=0;
	for(i=0; i<64; i++)
	{
		b=(bv>>i)&1;
		w=1+((wv0>>i)&1)*2;
		if((wv1>>i)&1)
			w=-w;
		k+=b*w;
	}

	if(k>=7)
		return(gann_fp4to8[0x7]);
	if(k<=-7)
		return(gann_fp4to8[0xF]);
	
	if(k>=0)
		return(gann_fp4to8[k]);
	if(k<0)
		return(gann_fp4to8[0x8+(-k)]);
	return(0);
}


u16 GANN_WeightBit64X2_F16A(u64 bv, u64 wv0, u64 wv1, u16 acc)
{
	int i, j, k, b, w, v;
	
	if(!bv)
		return(acc);
	
	k=0;
	for(i=0; i<64; i++)
	{
		b=(bv>>i)&1;
		w=1+((wv0>>i)&1)*2;
		if((wv1>>i)&1)
			w=-w;
		k+=b*w;
	}

	if(k>=7)			v=gann_fp4to8[0x7];
	else if(k<=-7)		v=gann_fp4to8[0xF];
	else if(k>0)		v=gann_fp4to8[k];
	else if(k<0)		v=gann_fp4to8[0x8+(-k)];
	else				v=0;

	v=gann_fp8to16[v];
	v=GANN_AddFp16J(v, acc);
//	v=GANN_AddFp16I(v, acc);

	return(v);
}


u16 GANN_WeightBit16X3A_F16A(u16 bv, u64 wv, u16 acc)
{
	static signed char wtab[8]= {
		0,  1,  3,  9,
		0, -1, -3, -9
	};
	int i, j, k, b, w, v, v0, v1;
	
	if(!bv)
		return(acc);
	
	k=0;
	for(i=0; i<16; i++)
	{
		b=(bv>>i)&1;
		w=wtab[(wv>>(i*3))&7];
//		w=1+((wv0>>i)&1)*2;
//		if((wv1>>i)&1)
//			w=-w;
		k+=b*w;
	}

	if(k>=7)			v=gann_fp4to8[0x7];
	else if(k<=-7)		v=gann_fp4to8[0xF];
	else if(k>0)		v=gann_fp4to8[k];
	else if(k<0)		v=gann_fp4to8[0x8+(-k)];
	else				v=0;

	v0=v;
//	v1=wv>>48;
	v1=wv>>56;
	v=gann_fp8mul16[v0*256+v1];

//	v=gann_fp8to16[v];
	v=GANN_AddFp16J(v, acc);
//	v=GANN_AddFp16I(v, acc);

	return(v);
}

byte GANN_WeightBit16X4(u64 bv, u64 wv)
{
	int v0, v1, v2, va;
	int i, j, k;
	
	va=0;
	for(i=0; i<16; i++)
	{
		v0=(bv>>(i*4))&15;
		v1=(wv>>(i*4))&15;
		v2=gann_fp4mul8[v0*16+v1];
		va=gann_fp8add8[va*256+v2];
	}
	return(gann_fp8to4[va]);
}


byte GANN_WeightBit16X4B(u64 bv, u64 wv)
{
	int v0, v1, v2, va;
	int i, j, k;
	
	va=(wv>>56)&255;
	for(i=0; i<16; i++)
	{
		v0=(bv>>(i*4))&15;
		v1=(wv>>(i*3))& 7;
		v1=gann_fp3to4[v1];
		v2=gann_fp4mul8[v0*16+v1];
		va=gann_fp8add8[va*256+v2];
	}
	
	if((wv>>52)&1)
		va=(va&0x80)|(((va&0x7F)>>1)+0x1F);
	if((wv>>52)&2)
	{
		if(va&0x80)
			va=0;
	}
	
	return(gann_fp8to4[va]);
}


byte GANN_WeightBit8X7B(u64 bv, u64 wv)
{
	int v0, v1, v2, va;
	int i, j, k;
	
	va=(wv>>56)&255;
	for(i=0; i<8; i++)
	{
		v0=(bv>>(i*8))&255;
		v1=(wv>>(i*7))&127;
		v1=v1<<1;
		v2=gann_fp8mul8[v0*256+v1];
		va=gann_fp8add8[va*256+v2];
	}
	
	if((wv>>52)&1)
		va=(va&0x80)|(((va&0x7F)>>1)+0x1F);
	if((wv>>52)&2)
	{
		if(va&0x80)
			va=0;
	}
	
	return(va);
}


u16 GANN_WeightBit8X8A16(u64 bv, u64 wv, u16 acc)
{
	float f;
	int v0, v1, v2, va;
	int i, j, k;
	
//	va=acc;
//	f=GANN_CnvFp16ToFloat(acc);
	*(u32 *)(&f)=((u32)acc)<<16;
	for(i=0; i<8; i++)
	{
		v0=(bv>>(i*8))&255;
		v1=(wv>>(i*8))&255;
//		v1=v1<<1;
//		v2=gann_fp8mul16[v0*256+v1];
//		va=gann_fp8add8[va*256+v2];
//		va=GANN_AddFp16I(va, v2);
		f+=gann_fp8mulf[v0*256+v1];
	}
	va=(*(u32 *)(&f))>>16;
//	va=GANN_CnvFloatToFp16(f);
	return(va);
}

u16 GANN_WeightBit8X7A16(u64 bv, u64 wv, u16 acc)
{
	int v0, v1, v2, va, vb, va0, va1;
	int i, j, k;
	
	va=(wv>>56)&255;
	va=gann_fp8to16[va];
	va=GANN_AddFp16I(va, acc);
	for(i=0; i<8; i++)
	{
		v0=(bv>>(i*8))&255;
//		v1=(wv>>(i*7))&127;
		v1=(wv>>(i*7))&0x7E;
		v1=v1<<1;
		v2=gann_fp8mul16[v0*256+v1];
		va=GANN_AddFp16J(va, v2);
//		va=GANN_AddFp16I(va, v2);

//		va1=GANN_AddFp16I(va, v2);
//		va0=GANN_AddFp16J(va, v2);
//		if(va0!=va1)
//			{ __debugbreak(); }
//		va=va0;
	}
	
	if((wv>> 7)&1)
		va=(va&0x8000)|(((va&0x7FFF)>>1)+0x1FFF);
	if((wv>> 0)&1)
	{
		if(va&0x8000)
			va=0;
	}
	
	return(va);
}

u64 GANN_WeakenBit8X8(u64 wv)
{
	u64 vc;
	int v0, v1;
	int i, j, k;
	
	vc=0;
	for(i=0; i<8; i++)
	{
		v1=(wv>>(i*8))&255;
		if(v1&0x7F)
			v1--;
		vc|=((u64)v1)<<(i*8);
	}
	return(vc);
}

u64 GANN_WeakenBit8X7(u64 wv)
{
	u64 vc;
	int v0, v1;
	int i, j, k;
	
	vc=0;
	for(i=0; i<8; i++)
	{
		v1=(wv>>(i*7))&127;
		if(v1&0x3F)
			v1--;
		vc|=((u64)v1)<<(i*7);
	}

	v1=(wv>>56)&255;
	if(v1&0x7F)
		v1--;
	vc|=((u64)v1)<<56;

	vc=(vc&(~0x0081))|(wv&0x0081);

	return(vc);
}
