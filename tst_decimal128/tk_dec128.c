/*
Written 2025-11-07 by Brendan G Bohannon.

Do whatever with this, just assume Public Domain or MIT-0 or CC0 or whatever.
I take no responsibility for anything that may result from using this code.
*/

/*
Decimal128 DPD test stuff...
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <time.h>


#ifndef BTIC2F_BYTE
#define BTIC2F_BYTE
typedef unsigned char			byte;
typedef unsigned short		u16;
typedef unsigned int			u32;
typedef unsigned long long	u64;

typedef signed char			sbyte;
typedef signed short			s16;
typedef signed int			s32;
typedef signed long long		s64;
#endif

#define TKD128_MASK30	0x3FFFFFFFU
#define TKD128_MASK20	0x000FFFFFU
#define TKD128_MASK46	0x00003FFFFFFFFFFFULL

// #define TKD128_BIAS		6176
#define TKD128_BIAS		6143

typedef struct TKD128_Packed128_s TKD128_Packed128;
typedef struct TKD128_Unpacked4x30_s TKD128_Unpacked4x30;
typedef struct TKD128_Unpacked8x30_s TKD128_Unpacked8x30;

struct TKD128_Packed128_s {
u64 lo;
u64 hi;
};

struct TKD128_Unpacked4x30_s {
u32 m[4];
s16 exp;
byte sgn;
};

struct TKD128_Unpacked8x30_s {
u32 m[8];
s16 exp;
byte sgn;
};

char *TKD128_OprDumpAsString(
	TKD128_Unpacked4x30 val);

u16 tkd128_unpackdpd[1024];
u16 tkd128_packdpd[1024];
u32 tkd128_lut_rcp3[1024];
u32 tkd128_lut_sqrt3p[1024];
u32 tkd128_lut_sqrt3n[1024];

int TKD128_InitDPD()
{
	int d0, d1, d2;
	int i, j, k;
	
	if(tkd128_packdpd[999])
		return(0);
	
	for(i=0; i<1024; i++)
	{
		if(i&8)
		{
			if(((i>>1)&3)==0)
			{
				d0=(i>>7)&7;
				d1=(i>>4)&7;
				d2=8+(i&1);
			}else
				if(((i>>1)&3)==1)
			{
				d0=(i>>7)&7;
				d1=8+((i>>4)&1);
				d2=((i>>4)&6)|(i&1);
			}else
				if(((i>>1)&3)==2)
			{
				d0=8+((i>>7)&1);
				d1=(i>>4)&7;
				d2=((i>>7)&6)|(i&1);
			}else
			{
				if(((i>>5)&3)==0)
				{
					d0=8+((i>>7)&1);
					d1=8+((i>>4)&1);
					d2=((i>>7)&6)|(i&1);
				}else
					if(((i>>5)&3)==1)
				{
					d0=8+((i>>7)&1);
					d1=((i>>7)&6)+((i>>4)&1);
					d2=8+((i>>0)&1);
				}else
					if(((i>>5)&3)==2)
				{
					d0=((i>>7)&6)+((i>>7)&1);
					d1=8+((i>>4)&1);
					d2=8+((i>>0)&1);
				}else
				{
					d0=8+((i>>7)&1);
					d1=8+((i>>4)&1);
					d2=8+((i>>0)&1);
				}
			}
		}else
		{
			d0=(i>>7)&7;
			d1=(i>>4)&7;
			d2=(i>>0)&7;
		}
		
		j=d0*100+d1*10+d2;
		
		tkd128_unpackdpd[i]=j;
		if(!tkd128_packdpd[j])
			tkd128_packdpd[j]=i;
	}

	for(i=0; i<1024; i++)
	{
		j=10000000/((i+0.5)*0.01);
//		j=(j*100)+500;
		tkd128_lut_rcp3[i]=j;

		j=1000000*sqrt((i+0.5)*0.01);
		tkd128_lut_sqrt3p[i]=j;
		tkd128_lut_sqrt3n[i]=9999999-j;
	}

	return(1);
}

TKD128_Unpacked4x30 TKD128_UnpackValueX30(TKD128_Packed128 val)
{
	TKD128_Unpacked4x30 tmp;
	int mc, mcd, ex;
	
	tmp.m[0]=(val.lo>> 0)&TKD128_MASK30;
	tmp.m[1]=(val.lo>>30)&TKD128_MASK30;
	tmp.m[2]=((val.hi<< 4)|(val.lo>>60))&TKD128_MASK30;
	tmp.m[3]=(val.hi>>26)&TKD128_MASK20;

	mc=(val.hi>>46)&0x1FFFF;
	if((mc>>15)==3)
		{ mcd=8+(mc>>12)&1; ex=(mc&0xFFF)|(((mc>>13)&3)<<12); }
	else
		{ mcd=(mc>>12)&7; ex=(mc&0xFFF)|(((mc>>15)&3)<<12); }
	tmp.m[3]+=mcd*1000000;

	tmp.exp=ex;
	tmp.sgn=val.hi>>63;

	return(tmp);
}

TKD128_Packed128 TKD128_PackValueX30(TKD128_Unpacked4x30 val)
{
	TKD128_Packed128 tmp;
	int m3n, mcd, mc, ex;
	
	m3n=val.m[3]%1000000000;
	mcd=val.m[3]/1000000000;
	
	ex=val.exp;
	if((mcd>=8) || (ex>=12288))
		{ mc=0x18000|(ex&0xFFF)|((mcd&1)<<12)|(((ex>>12)&3)<<13); }
	else
		{ mc=(ex&0xFFF)|((mcd&7)<<12)|(((ex>>12)&3)<<15); }
	
	tmp.lo=val.m[0]|(((u64)(val.m[1]))<<30)|(((u64)(val.m[2]))<<60);
	tmp.hi=(val.m[2]>>4)|(((u64)m3n)<<26)|
		(((u64)mcd)<<46)|(((u64)val.sgn)<<63);
	
	return(tmp);
}


#if 1
void TKD128_UnpackValueX30R(
	TKD128_Unpacked4x30 *rtmp, TKD128_Packed128 *rval)
{
//	TKD128_Unpacked4x30 tmp;
	u64 lo, hi;
	int mc, mcd, ex;
	
	lo=rval->lo;
	hi=rval->hi;
	rtmp->m[0]=(lo>> 0)&TKD128_MASK30;
	rtmp->m[1]=(lo>>30)&TKD128_MASK30;
	rtmp->m[2]=((hi<< 4)|(lo>>60))&TKD128_MASK30;
	rtmp->m[3]=(hi>>26)&TKD128_MASK20;

	mc=(hi>>46)&0x1FFFF;
	if((mc>>15)==3)
	{
		mcd=8+(mc>>12)&1;
		ex=(mc&0xFFF)|(((mc>>13)&3)<<12);
	}else
	{
		mcd=(mc>>12)&7;
		ex=(mc&0xFFF)|(((mc>>15)&3)<<12);
	}
	rtmp->m[3]+=mcd*1000000;

	rtmp->exp=ex;
	rtmp->sgn=hi>>63;
}

void TKD128_PackValueX30R(
	TKD128_Unpacked4x30 *rval, TKD128_Packed128 *rtmp)
{
	u32 m0, m1, m2, m3;
	int m3n, mcd, mc, ex, sgn;
	
	m0=rval->m[0];
	m1=rval->m[1];
	m2=rval->m[2];
	m3=rval->m[3];
	
	m3n=m3%1000000000;
	mcd=m3/1000000000;
	sgn=rval->sgn;
	
	ex=rval->exp;
	if((mcd>=8) || (ex>=12288))
		{ mc=0x18000|(ex&0xFFF)|((mcd&1)<<12)|(((ex>>12)&3)<<13); }
	else
		{ mc=(ex&0xFFF)|((mcd&7)<<12)|(((ex>>12)&3)<<15); }

	rtmp->lo=m0|(((u64)(m1))<<30)|(((u64)(m2))<<60);
	rtmp->hi=(m2>>4)|(((u64)m3n)<<26)|
		(((u64)mcd)<<46)|(((u64)sgn)<<63);
}
#endif


u32 TKD128_UnpackBitsDPD30(u32 val)
{
	u32 v0, v1, v2, v;
	v0=tkd128_unpackdpd[(val>> 0)&1023];
	v1=tkd128_unpackdpd[(val>>10)&1023];
	v2=tkd128_unpackdpd[(val>>20)&1023];
	v=v0+(v1*1000)+(v2*1000000);
	return(v);
}

u32 TKD128_PackBitsDPD30(u32 val)
{
	u32 v0, v1, v2, v;
	v=val;
	v1=v/1000;
	v0=v-(v1*1000);
	v2=v1/1000;
	v1=v1-(v2*1000);
	v0=tkd128_packdpd[v0];
	v1=tkd128_packdpd[v1];
	v2=tkd128_packdpd[v2];
	v=v0|(v1<<10)|(v2<<20);
	return(v);
}

TKD128_Unpacked4x30 TKD128_UnpackValueDPD(TKD128_Packed128 val)
{
	TKD128_Unpacked4x30 tmp;
	int mc, mcd, ex;
	
	tmp.m[0]=TKD128_UnpackBitsDPD30((val.lo>> 0)&TKD128_MASK30);
	tmp.m[1]=TKD128_UnpackBitsDPD30((val.lo>>30)&TKD128_MASK30);
	tmp.m[2]=TKD128_UnpackBitsDPD30(((val.hi<< 4)|(val.lo>>60))&TKD128_MASK30);
	tmp.m[3]=TKD128_UnpackBitsDPD30((val.hi>>26)&TKD128_MASK20);

	mc=(val.hi>>46)&0x1FFFF;
	if((mc>>15)==3)
	{
		mcd=8+(mc>>12)&1;
		ex=(mc&0xFFF)|(((mc>>13)&3)<<12);
	}else
	{
		mcd=(mc>>12)&7;
		ex=(mc&0xFFF)|(((mc>>15)&3)<<12);
	}
	tmp.m[3]+=mcd*1000000;

	tmp.exp=ex;
	tmp.sgn=val.hi>>63;

	return(tmp);
}

TKD128_Packed128 TKD128_PackValueDPD(TKD128_Unpacked4x30 val)
{
	TKD128_Packed128 tmp;
	u32 m0, m1, m2, m3;
	int m3n, mcd, mc, ex;
	
	mcd=val.m[3]/1000000;
	m3n=val.m[3]%1000000;
	
	ex=val.exp;
	if((mcd>=8) || (ex>=12288))
		{ mc=0x18000|(ex&0xFFF)|((mcd&1)<<12)|(((ex>>12)&3)<<13); }
	else
		{ mc=(ex&0xFFF)|((mcd&7)<<12)|(((ex>>12)&3)<<15); }

	m0=TKD128_PackBitsDPD30(val.m[0]);
	m1=TKD128_PackBitsDPD30(val.m[1]);
	m2=TKD128_PackBitsDPD30(val.m[2]);
	m3=TKD128_PackBitsDPD30(m3n);
	tmp.lo=m0|(((u64)m1)<<30)|(((u64)m2)<<60);
	tmp.hi=(m2>>4)|(((u64)m3)<<26)|
		(((u64)mc)<<46)|(((u64)val.sgn)<<63);
	
	return(tmp);
}


int TKD128_ScaleLeftArray8(u32 *arr, int shl)
{
	u32 or_lo4;
	int sh;

	sh=shl;

	while(sh>=9)
	{
		sh-=9;
		arr[7]=arr[6];
		arr[6]=arr[5];
		arr[5]=arr[4];
		arr[4]=arr[3];
		arr[3]=arr[2];
		arr[2]=arr[1];
		arr[1]=arr[0];
		arr[0]=0;
	}

	or_lo4=arr[3]|arr[2]|arr[1]|arr[0];

	while(sh>=3)
	{
		sh-=3;
		arr[7]=((arr[7]%1000000)*1000)+(arr[6]/1000000);
		arr[6]=((arr[6]%1000000)*1000)+(arr[5]/1000000);
		arr[5]=((arr[5]%1000000)*1000)+(arr[4]/1000000);
		if(or_lo4)
		{
			arr[4]=((arr[4]%1000000)*1000)+(arr[3]/1000000);
			arr[3]=((arr[3]%1000000)*1000)+(arr[2]/1000000);
			arr[2]=((arr[2]%1000000)*1000)+(arr[1]/1000000);
			arr[1]=((arr[1]%1000000)*1000)+(arr[0]/1000000);
			arr[0]=((arr[0]%1000000)*1000);
		}else
		{
			arr[4]=((arr[4]%1000000)*1000);
		}
	}

	while(sh>=1)
	{
		sh--;
		arr[7]=((arr[7]%100000000)*10)+(arr[6]/100000000);
		arr[6]=((arr[6]%100000000)*10)+(arr[5]/100000000);
		arr[5]=((arr[5]%100000000)*10)+(arr[4]/100000000);
		if(or_lo4)
		{
			arr[4]=((arr[4]%100000000)*10)+(arr[3]/100000000);
			arr[3]=((arr[3]%100000000)*10)+(arr[2]/100000000);
			arr[2]=((arr[2]%100000000)*10)+(arr[1]/100000000);
			arr[1]=((arr[1]%100000000)*10)+(arr[0]/100000000);
			arr[0]=((arr[0]%100000000)*10);
		}else
		{
			arr[4]=((arr[4]%100000000)*10);
		}
	}
	return(0);
}


int TKD128_ScaleRightArray8(u32 *arr, int shl)
{
	int sh;

	sh=shl;

	while(sh>=9)
	{
		sh-=9;
		arr[0]=arr[1];
		arr[1]=arr[2];
		arr[2]=arr[3];
		arr[3]=arr[4];
		arr[4]=arr[5];
		arr[5]=arr[6];
		arr[6]=arr[7];
		arr[7]=0;
	}

	while(sh>=3)
	{
		sh-=3;
		arr[0]=((arr[1]%1000)*1000000)+(arr[0]/1000);
		arr[1]=((arr[2]%1000)*1000000)+(arr[1]/1000);
		arr[2]=((arr[3]%1000)*1000000)+(arr[2]/1000);
		arr[3]=((arr[4]%1000)*1000000)+(arr[3]/1000);
		arr[4]=((arr[5]%1000)*1000000)+(arr[4]/1000);
		arr[5]=((arr[6]%1000)*1000000)+(arr[5]/1000);
		arr[6]=((arr[7]%1000)*1000000)+(arr[6]/1000);
		arr[7]=(arr[7]/1000);
	}

#if 1
//	while(sh>=2)
	if(sh>=2)
	{
		sh-=2;
		arr[0]=((arr[1]%100)*10000000)+(arr[0]/100);
		arr[1]=((arr[2]%100)*10000000)+(arr[1]/100);
		arr[2]=((arr[3]%100)*10000000)+(arr[2]/100);
		arr[3]=((arr[4]%100)*10000000)+(arr[3]/100);
		arr[4]=((arr[5]%100)*10000000)+(arr[4]/100);
		arr[5]=((arr[6]%100)*10000000)+(arr[5]/100);
		arr[6]=((arr[7]%100)*10000000)+(arr[6]/100);
		arr[7]=(arr[7]/100);
	}
#endif

//	while(sh>=1)
	if(sh>=1)
	{
		sh--;
		arr[0]=((arr[1]%10)*100000000)+(arr[0]/10);
		arr[1]=((arr[2]%10)*100000000)+(arr[1]/10);
		arr[2]=((arr[3]%10)*100000000)+(arr[2]/10);
		arr[3]=((arr[4]%10)*100000000)+(arr[3]/10);
		arr[4]=((arr[5]%10)*100000000)+(arr[4]/10);
		arr[5]=((arr[6]%10)*100000000)+(arr[5]/10);
		arr[6]=((arr[7]%10)*100000000)+(arr[6]/10);
		arr[7]=(arr[7]/10);
	}
	return(0);
}

int TKD128_AddArray8(u32 *arr_a, u32 *arr_b, u32 *arr_c)
{
	u32 v, c;
	int i;
	
	c=0;
	for(i=0; i<8; i++)
	{
		v=arr_a[i]+arr_b[i]+c;
		c=0;
		if(v>=1000000000U)
			{ c=1; v-=1000000000U; }
		arr_c[i]=v;
	}
	return(0);
}

int TKD128_SubArray8(u32 *arr_a, u32 *arr_b, u32 *arr_c)
{
	u32 v, c;
	int i;
	
	c=0;
	for(i=0; i<8; i++)
	{
		v=arr_a[i]-arr_b[i]-c;
		c=0;
		if(v>=1000000000U)
			{ c=1; v+=1000000000U; }
		arr_c[i]=v;
	}
	return(c);
}

/* Take the 10s complement of the array. */
int TKD128_ComplementArray8(u32 *arr_a, u32 *arr_c)
{
	u32 v, c;
	int i;
	c=1;
	for(i=0; i<8; i++)
	{
		v=(999999999-arr_a[i])+c;
		c=0;
		if(v>=1000000000U)
			{ c=1; v+=1000000000U; }
		arr_c[i]=v;
	}
	return(0);
}

TKD128_Unpacked8x30 TKD128_Conv4to8x30(
	TKD128_Unpacked4x30 val)
{
	TKD128_Unpacked8x30 tmp;
	tmp.sgn=val.sgn;
	tmp.exp=val.exp;
	tmp.m[7]=val.m[3];
	tmp.m[6]=val.m[2];
	tmp.m[5]=val.m[1];
	tmp.m[4]=val.m[0];
	tmp.m[3]=0;
	tmp.m[2]=0;
	tmp.m[1]=0;
	tmp.m[0]=0;
	return(tmp);
}

int TKD128_OprCountLeadingZeroesVal30(u32 v)
{
	if(v>= 10000000)
	{	if(v>=100000000)
			return(0);
		return(1);	}
	if(v>=   100000)
	{	if(v>=  1000000)
			return(2);
		return(3);	}
	if(v>=    1000)
	{	if(v>=   10000)
			return(4);
		return(5);	}
	if(v>=     100)
		return(6);
	return(7);
}

int TKD128_OprCountLeadingZeroesArray8(u32 *m)
{
	u32 v;
	int i, n;
	
	v=m[7];
	if(v)
		return(TKD128_OprCountLeadingZeroesVal30(v));
	n=0;
	for(i=7; i>=0; i--)
	{
		if(m[i])break;
		n+=9;
	}	
	if(i<0)
		return(n);
	return(TKD128_OprCountLeadingZeroesVal30(m[i])+n);
}

TKD128_Unpacked4x30 TKD128_OprNormalize8to4x30(TKD128_Unpacked8x30 valc0)
{
	TKD128_Unpacked4x30 valc;
	u32 m0, m1, m2, m3;
	int n;

	while(valc0.m[7]>=10000000)
		{ valc0.exp++; TKD128_ScaleRightArray8(valc0.m, 1); }

	if(valc0.m[7]<1000000)
	{
		n=TKD128_OprCountLeadingZeroesArray8(valc0.m);
		n=n-2;
		if(n>valc0.exp)	n=valc0.exp;
		if(n>0)
			{ valc0.exp-=n; TKD128_ScaleLeftArray8(valc0.m, n); }
	}

#if 0
//	if(!valc0.m[7])
	if(1)
	{
		while(!valc0.m[7] && !valc0.m[6] && !valc0.m[5] && (valc0.exp>=18))
			{ valc0.exp-=18; TKD128_ScaleLeftArray8(valc0.m, 18); }
	//	while(!valc0.m[7] && !valc0.m[6] && (valc0.exp>=9))
		while(!valc0.m[7] && (valc0.m[6]<10000000) && (valc0.exp>=9))
			{ valc0.exp-=9; TKD128_ScaleLeftArray8(valc0.m, 9); }
		while(!valc0.m[7] && (valc0.exp>=6))
			{ valc0.exp-=6; TKD128_ScaleLeftArray8(valc0.m, 6); }
	}
	while((valc0.m[7]<=1000) && (valc0.exp>=3))
		{ valc0.exp-=3; TKD128_ScaleLeftArray8(valc0.m, 3); }
	while((valc0.m[7]<1000000) && (valc0.exp>=1))
		{ valc0.exp--; TKD128_ScaleLeftArray8(valc0.m, 1); }
#endif

	m0=valc0.m[4];
	m1=valc0.m[5];
	m2=valc0.m[6];
	m3=valc0.m[7];

	m0+=(valc0.m[3]>500000000);

	if(m0>=1000000000)
		{ m1++; m0-=1000000000; }
	if(m1>=1000000000)
		{ m2++; m1-=1000000000; }
	if(m2>=1000000000)
		{ m3++; m2-=1000000000; }
	
	valc.sgn=valc0.sgn;
	valc.exp=valc0.exp;
	valc.m[0]=m0;	valc.m[1]=m1;
	valc.m[2]=m2;	valc.m[3]=m3;
	
	return(valc);
}

TKD128_Unpacked4x30 TKD128_OprNormalize4x30(TKD128_Unpacked4x30 vala)
{
	TKD128_Unpacked8x30 vala0;
	TKD128_Unpacked4x30 valc;
	vala0=TKD128_Conv4to8x30(vala);
	valc=TKD128_OprNormalize8to4x30(vala0);
	return(valc);
}

TKD128_Unpacked4x30 TKD128_OprAdd(
	TKD128_Unpacked4x30 vala,
	TKD128_Unpacked4x30 valb)
{
	TKD128_Unpacked8x30 vala0, valb0, valc0;
	TKD128_Unpacked4x30 valc;
	int maxexp, flip;
	
	vala0=TKD128_Conv4to8x30(vala);
	valb0=TKD128_Conv4to8x30(valb);
	maxexp=vala0.exp;
	flip=0;
	if(valb0.exp>maxexp)
	{
		maxexp=valb0.exp;
		flip=1;
	}else
		if(vala0.exp==valb0.exp)
	{
		if(valb0.m[7]>vala0.m[7])
			{ flip=1; }
		else if(valb0.m[7]==vala0.m[7])
		{	if(valb0.m[6]>vala0.m[6])
				{ flip=1; }
			else if(valb0.m[6]==vala0.m[6])
			{	if(valb0.m[5]>vala0.m[5])
					{ flip=1; }
				else if(valb0.m[5]==vala0.m[5])
				{	if(valb0.m[4]>vala0.m[4])
						{ flip=1; }
				}
			}
		}
	}
	
	if(flip)
	{
		valc0=vala0;
		vala0=valb0;
		valb0=valc0;
	}

	TKD128_ScaleRightArray8(vala0.m, maxexp-vala0.exp);
	TKD128_ScaleRightArray8(valb0.m, maxexp-valb0.exp);
	vala0.exp=maxexp;
	valb0.exp=maxexp;
	valc0.exp=maxexp;
	valc0.sgn=vala0.sgn;
	
	if(vala0.sgn==valb0.sgn)
		{ TKD128_AddArray8(vala0.m, valb0.m, valc0.m); }
	else
	{
		flip=TKD128_SubArray8(vala0.m, valb0.m, valc0.m);
		if(flip)
		{
			valc0.sgn=!valc0.sgn;
			TKD128_ComplementArray8(valc0.m, valc0.m);
		}
	}

	valc=TKD128_OprNormalize8to4x30(valc0);
	return(valc);
}

TKD128_Unpacked4x30 TKD128_OprSub(
	TKD128_Unpacked4x30 vala,
	TKD128_Unpacked4x30 valb)
{
	TKD128_Unpacked4x30 valb1;
	valb1=valb;
	valb1.sgn=!valb.sgn;
	return(TKD128_OprAdd(vala, valb1));
}

TKD128_Unpacked4x30 TKD128_OprMul(
	TKD128_Unpacked4x30 vala,
	TKD128_Unpacked4x30 valb)
{
	TKD128_Unpacked8x30 valc0;
	TKD128_Unpacked4x30 valc;
	u64 m0_aa, m0_ab, m0_ac, m0_ad;
	u64 m0_ba, m0_bb, m0_bc, m0_bd;
	u64 m0_ca, m0_cb, m0_cc, m0_cd;
	u64 m0_da, m0_db, m0_dc, m0_dd;
	u64 m1a, m1b, m1c, m1d;
	
	valc0.exp=vala.exp+valb.exp-TKD128_BIAS;
	valc0.sgn=vala.sgn^valb.sgn;
	
	m0_aa=((u64)vala.m[3])*((u64)valb.m[3]);
	m0_ab=((u64)vala.m[3])*((u64)valb.m[2]);
	m0_ac=((u64)vala.m[3])*((u64)valb.m[1]);
	m0_ad=((u64)vala.m[3])*((u64)valb.m[0]);
	m0_ba=((u64)vala.m[2])*((u64)valb.m[3]);
	m0_bb=((u64)vala.m[2])*((u64)valb.m[2]);
	m0_bc=((u64)vala.m[2])*((u64)valb.m[1]);
	m0_bd=((u64)vala.m[2])*((u64)valb.m[0]);
	m0_ca=((u64)vala.m[1])*((u64)valb.m[3]);
	m0_cb=((u64)vala.m[1])*((u64)valb.m[2]);
	m0_cc=((u64)vala.m[1])*((u64)valb.m[1]);
	m0_cd=((u64)vala.m[1])*((u64)valb.m[0]);
	m0_da=((u64)vala.m[0])*((u64)valb.m[3]);
	m0_db=((u64)vala.m[0])*((u64)valb.m[2]);
	m0_dc=((u64)vala.m[0])*((u64)valb.m[1]);
	m0_dd=((u64)vala.m[0])*((u64)valb.m[0]);

/*
 * AA
 *  AB
 *  BA
 *   BB
 *   AC
 *   CA
 *    AD
 *    DA
 *    BC
 *    CB
 *     CC
 *     BD
 *     DB
 *      CD
 *      DC
 *       DD
 */

	m1a=m0_aa+
		(m0_ba/1000000000ULL) +
		(m0_ab/1000000000ULL) ;
	m1b=m0_bb+
		m0_ac+
		m0_ca+
		(m0_cb/1000000000ULL) +
		(m0_bc/1000000000ULL) +
		(m0_ad/1000000000ULL) +
		(m0_da/1000000000ULL) +
		((m0_ba%1000000000ULL)*1000000000ULL) +
		((m0_ab%1000000000ULL)*1000000000ULL) ;
	m1c=m0_cc+
		m0_bd+
		m0_db+
		(m0_dc/1000000000ULL) +
		(m0_cd/1000000000ULL) +
		((m0_ad%1000000000ULL)*1000000000ULL) +
		((m0_da%1000000000ULL)*1000000000ULL) +
		((m0_cb%1000000000ULL)*1000000000ULL) +
		((m0_bc%1000000000ULL)*1000000000ULL) ;
	
	m1d=m0_dd+
		((m0_dc%1000000000ULL)*1000000000ULL) +
		((m0_cd%1000000000ULL)*1000000000ULL) ;

	while(m1d>=1000000000000000000ULL)
		{ m1d-=1000000000000000000ULL; m1c++; }
	while(m1c>=1000000000000000000ULL)
		{ m1c-=1000000000000000000ULL; m1b++; }
	while(m1b>=1000000000000000000ULL)
		{ m1b-=1000000000000000000ULL; m1a++; }

	valc0.m[7]=m1a/1000000000ULL;
	valc0.m[6]=m1a%1000000000ULL;
	valc0.m[5]=m1b/1000000000ULL;
	valc0.m[4]=m1b%1000000000ULL;
	valc0.m[3]=m1c/1000000000ULL;
	valc0.m[2]=m1c%1000000000ULL;
	valc0.m[1]=m1d/1000000000ULL;
	valc0.m[0]=m1d%1000000000ULL;
	
	TKD128_ScaleLeftArray8(valc0.m, 3);

	valc=TKD128_OprNormalize8to4x30(valc0);
	return(valc);
}

TKD128_Unpacked4x30 TKD128_OprRecipApx(
	TKD128_Unpacked4x30 val)
{
	TKD128_Unpacked4x30 valc;
	u32 m3;
	int ix;
	
	valc.sgn=val.sgn;
	valc.exp=(2*TKD128_BIAS-1)-val.exp;
	ix=val.m[3]/10000;
	if(ix>=1024)	ix=1023;
	m3=tkd128_lut_rcp3[ix];
	while(m3>10000000)
		{ valc.exp++; m3/=10; }
	while(m3<1000000)
		{ valc.exp--; m3*=10; }
	valc.m[3]=m3;
	valc.m[2]=0;
	valc.m[1]=0;
	valc.m[0]=0;

	return(valc);
}

TKD128_Unpacked4x30 TKD128_OprSqrtApx(
	TKD128_Unpacked4x30 val)
{
	TKD128_Unpacked4x30 valc;
	u32 m3;
	int ix;
	
	valc.sgn=val.sgn;
	valc.exp=TKD128_BIAS+((val.exp-TKD128_BIAS)/2);
	
//	m3=val.m[3];
	ix=val.m[3]/10000;
	if(ix>=1024)	ix=1023;
	m3=tkd128_lut_sqrt3p[ix];
	if(valc.exp<TKD128_BIAS)
		m3=tkd128_lut_sqrt3n[ix];
//	while(m3>10000000)
//		{ valc.exp++; m3/=10; }
//	while(m3<1000000)
//		{ valc.exp--; m3*=10; }

	valc.m[3]=m3;
	valc.m[2]=0;
	valc.m[1]=0;
	valc.m[0]=0;

	return(valc);
}

int TKD128_OprCompare(
	TKD128_Unpacked4x30 vala,
	TKD128_Unpacked4x30 valb)
{
	if(vala.sgn!=valb.sgn)
	{
		if(!vala.sgn)
			return(1);
		return(-1);
	}

	if((vala.exp>0) && ((vala.m[3]<1000000) || (vala.m[3]>=10000000)))
		{ vala=TKD128_OprNormalize4x30(vala); }
	if((valb.exp>0) && ((valb.m[3]<1000000) || (valb.m[3]>=10000000)))
		{ valb=TKD128_OprNormalize4x30(valb); }

	if((vala.exp>=18432) || (valb.exp>=18432))
		{ /* NAN */ return(2); }

	if(vala.exp!=valb.exp)
	{
		if(vala.exp>valb.exp)
			return(1);
		return(-1);
	}
	if(vala.m[3]!=valb.m[3])
	{
		if(vala.m[3]>valb.m[3])
			return(1);
		return(-1);
	}
	if(vala.m[2]!=valb.m[2])
	{
		if(vala.m[2]>valb.m[2])
			return(1);
		return(-1);
	}
	if(vala.m[1]!=valb.m[1])
	{
		if(vala.m[1]>valb.m[1])
			return(1);
		return(-1);
	}
	if(vala.m[0]!=valb.m[0])
	{
		if(vala.m[0]>valb.m[0])
			return(1);
		return(-1);
	}
	return(0);
}

TKD128_Unpacked4x30 TKD128_OprRecip(
	TKD128_Unpacked4x30 val)
{
	static TKD128_Unpacked4x30 cnst_1p0  ={{0,0,0,1000000}, TKD128_BIAS  , 0};
	static TKD128_Unpacked4x30 cnst_1p5  ={{0,0,0,1500000}, TKD128_BIAS  , 0};
	static TKD128_Unpacked4x30 cnst_1p25 ={{0,0,0,1250000}, TKD128_BIAS  , 0};
	static TKD128_Unpacked4x30 cnst_1p125={{0,0,0,1125000}, TKD128_BIAS  , 0};
	static TKD128_Unpacked4x30 cnst_2p0  ={{0,0,0,2000000}, TKD128_BIAS  , 0};
	static TKD128_Unpacked4x30 cnst_3p0  ={{0,0,0,3000000}, TKD128_BIAS  , 0};
	static TKD128_Unpacked4x30 cnst_0p5  ={{0,0,0,5000000}, TKD128_BIAS-1, 0};
	static TKD128_Unpacked4x30 cnst_0p75 ={{0,0,0,7500000}, TKD128_BIAS-1, 0};
	static TKD128_Unpacked4x30 cnst_0p875={{0,0,0,8750000}, TKD128_BIAS-1, 0};

	TKD128_Unpacked4x30 valc, valb, valt;
	int d0, d1, d2, d3, de;
	int i;
	
	valc=TKD128_OprRecipApx(val);

//	printf("Rcp Apx-0:\n  %s\n  %s\n",
//		TKD128_OprDumpAsString(val),
//		TKD128_OprDumpAsString(valc));

	for(i=0; i<32; i++)
	{
		valb=TKD128_OprMul(val, valc);
		de=valb.exp-TKD128_BIAS;
		if(de>0)
			{ valc=TKD128_OprMul(valc, cnst_0p5); continue; }
		if(de<0)
		{
			if(de==-1)
			{
				if(valb.m[3]>=8750000)
					break;
				if(valb.m[3]>=7500000)
					{ valc=TKD128_OprMul(valc, cnst_1p25); continue; }
				if(valb.m[3]>=5000000)
					{ valc=TKD128_OprMul(valc, cnst_1p5); continue; }
			}
			valc=TKD128_OprMul(valc, cnst_2p0);
			continue;
		}

		if(valb.m[3]>=2000000)
			{ valc=TKD128_OprMul(valc, cnst_0p5); continue; }
		if(valb.m[3]<= 500000)
			{ valc=TKD128_OprMul(valc, cnst_2p0); continue; }

		if(valb.m[3]>=1500000)
			{ valc=TKD128_OprMul(valc, cnst_0p75); continue; }
		if(valb.m[3]<= 750000)
			{ valc=TKD128_OprMul(valc, cnst_1p25); continue; }

		if(valb.m[3]>=1250000)
			{ valc=TKD128_OprMul(valc, cnst_0p875); continue; }
		if(valb.m[3]<= 875000)
			{ valc=TKD128_OprMul(valc, cnst_1p125); continue; }

		break;
	}

//	printf("Rcp Apx-1:\n  %s\n  %s\n",
//		TKD128_OprDumpAsString(val),
//		TKD128_OprDumpAsString(valc));

	for(i=0; i<32; i++)
	{
		valb=TKD128_OprMul(val, valc);

//		de=TKD128_OprCompare(valb, cnst_1p0);
//		if(!de)
//			break;

		valt=TKD128_OprSub(cnst_2p0, valb);
		
		de=TKD128_OprCompare(valt, cnst_1p0);
		if(!de)
			break;

//		valt=TKD128_OprSub(cnst_3p0, valb);
//		valt=TKD128_OprMul(valt, cnst_0p5);

//		printf("Rcp Apx-I:\n  %s\n  %s\n",
//			TKD128_OprDumpAsString(valb),
//			TKD128_OprDumpAsString(valt));

		valc=TKD128_OprMul(valc, valt);
	}
	
//	printf("Rcp Apx-2:\n  %s\n  %s\n",
//		TKD128_OprDumpAsString(val),
//		TKD128_OprDumpAsString(valc));
	return(valc);
}

TKD128_Unpacked4x30 TKD128_OprDiv(
	TKD128_Unpacked4x30 vala,
	TKD128_Unpacked4x30 valb)
{
	TKD128_Unpacked4x30 valb1;
	valb1=TKD128_OprRecip(valb);
	return(TKD128_OprMul(vala, valb1));
}


TKD128_Unpacked4x30 TKD128_OprSqrt(
	TKD128_Unpacked4x30 val)
{
	static TKD128_Unpacked4x30 cnst_1p0  ={{0,0,0,1000000}, TKD128_BIAS  , 0};
	static TKD128_Unpacked4x30 cnst_1p5  ={{0,0,0,1500000}, TKD128_BIAS  , 0};
	static TKD128_Unpacked4x30 cnst_1p25 ={{0,0,0,1250000}, TKD128_BIAS  , 0};
	static TKD128_Unpacked4x30 cnst_1p125={{0,0,0,1125000}, TKD128_BIAS  , 0};
	static TKD128_Unpacked4x30 cnst_2p0  ={{0,0,0,2000000}, TKD128_BIAS  , 0};
	static TKD128_Unpacked4x30 cnst_3p0  ={{0,0,0,3000000}, TKD128_BIAS  , 0};
	static TKD128_Unpacked4x30 cnst_4p0  ={{0,0,0,4000000}, TKD128_BIAS  , 0};

	static TKD128_Unpacked4x30 cnst_0p5  ={{0,0,0,5000000}, TKD128_BIAS-1, 0};
	static TKD128_Unpacked4x30 cnst_0p496={{0,0,0,4960938}, TKD128_BIAS-1, 0};
	static TKD128_Unpacked4x30 cnst_0p499={{0,0,0,4990000}, TKD128_BIAS-1, 0};
	static TKD128_Unpacked4x30 cnst_0p375={{0,0,0,3750000}, TKD128_BIAS-1, 0};
	static TKD128_Unpacked4x30 cnst_0p33x={{0,0,0,3333333}, TKD128_BIAS-1, 0};
	static TKD128_Unpacked4x30 cnst_0p25 ={{0,0,0,2500000}, TKD128_BIAS-1, 0};
	static TKD128_Unpacked4x30 cnst_0p275={{0,0,0,2750000}, TKD128_BIAS-1, 0};
	static TKD128_Unpacked4x30 cnst_0p125={{0,0,0,1250000}, TKD128_BIAS-1, 0};
	static TKD128_Unpacked4x30 cnst_0p188={{0,0,0,1875000}, TKD128_BIAS-1, 0};
	static TKD128_Unpacked4x30 cnst_0p75 ={{0,0,0,7500000}, TKD128_BIAS-1, 0};
	static TKD128_Unpacked4x30 cnst_0p875={{0,0,0,8750000}, TKD128_BIAS-1, 0};

	TKD128_Unpacked4x30 valc, valb, valt;
	TKD128_Unpacked4x30 val_rcp, val_rcph;
	int d0, d1, d2, d3, de;
	int i;

	if(val.sgn)
	{
		valb=val;
		valb.sgn=0;
		valc=TKD128_OprSqrt(valb);
		valc.sgn=1;
		return(valc);
	}

//	val_p1=TKD128_OprAdd(cnst_1p0, val);
//	val_p2=TKD128_OprAdd(cnst_2p0, val);
//	val_p4=TKD128_OprAdd(cnst_4p0, val);
	
	valc=TKD128_OprSqrtApx(val);

	val_rcp=TKD128_OprRecipApx(valc);
//	val_rcph=TKD128_OprMul(val_rcp, cnst_0p25);
	val_rcph=TKD128_OprMul(val_rcp,  cnst_0p375);

//	printf("Sqrt Apx-0:\n  a=%s\n  c=%s\n  p1=%s\n",
//		TKD128_OprDumpAsString(val),
//		TKD128_OprDumpAsString(valc),
//		TKD128_OprDumpAsString(val_p1));

	for(i=0; i<4; i++)
	{
		valb=TKD128_OprMul(valc, valc);
		valt=TKD128_OprSub(val, valb);
		valt=TKD128_OprMul(valt, val_rcph);
		if(valt.exp<(valc.exp-2))
			break;
		valc=TKD128_OprAdd(valc, valt);
	}

	val_rcp=TKD128_OprRecipApx(valc);
	valb=TKD128_OprMul(valc, val_rcp);
	valt=TKD128_OprSub(cnst_2p0, valb);
	val_rcp=TKD128_OprMul(val_rcp, valt);

	val_rcph=TKD128_OprMul(val_rcp, cnst_0p5);

//	printf("Sqrt Apx-1:\n  %s\n  %s\n",
//		TKD128_OprDumpAsString(val),
//		TKD128_OprDumpAsString(valc));

//	for(i=0; i<512; i++)
	for(i=0; i<128; i++)
//	for(i=0; i<64; i++)
	{
		valb=TKD128_OprMul(valc, valc);
		valt=TKD128_OprSub(val, valb);
		valt=TKD128_OprMul(valt, val_rcph);
		if(valt.exp<(valc.exp-32))
			break;
		valc=TKD128_OprAdd(valc, valt);

#if 1
		valb=TKD128_OprMul(valc, val_rcp);
		valt=TKD128_OprSub(cnst_2p0, valb);
		val_rcp=TKD128_OprMul(val_rcp, valt);
		val_rcph=TKD128_OprMul(val_rcp, cnst_0p5);
#endif
	}

//	printf("Sqrt Apx-2:\n  %s\n  %s\n",
//		TKD128_OprDumpAsString(val),
//		TKD128_OprDumpAsString(valc));
	return(valc);
}


int TKD128_OprDump30ToString(char *ct, u32 v)
{
	u32 v0, v1, vt;
	int i;
	
	vt=v;
	for(i=0; i<9; i++)
	{
		v1=vt/10;
		v0=vt-(v1*10);
		vt=v1;
		ct[8-i]='0'+v0;
	}
	return(0);
}

char *TKD128_RAlloc(int sz)
{
	static char buf[4096];
	static u16 pos;
	char *ct;
	
	if((pos+sz)>=4096)
		pos=0;
	ct=buf+pos;
	pos+=sz;
	return(ct);
}

char *TKD128_RStrDup(char *str)
{
	char *ct;
	ct=TKD128_RAlloc(strlen(str)+1);
	strcpy(ct, str);
	return(ct);
}

char *TKD128_OprDumpAsString(
	TKD128_Unpacked4x30 val)
{
	static char tbuf[256], tb1[48];
	char *ct, *cs;
	int i, j, k, ex, exn;

	if((val.exp==0) && !val.m[3] && !val.m[2] && !val.m[1] && !val.m[0])
	{
		if(val.sgn)
			return("-0.0");
		return("0.0");
	}

	TKD128_OprDump30ToString(tb1+ 0, val.m[3]);
	TKD128_OprDump30ToString(tb1+ 9, val.m[2]);
	TKD128_OprDump30ToString(tb1+18, val.m[1]);
	TKD128_OprDump30ToString(tb1+27, val.m[0]);
	tb1[36]=0;
	
	ex=val.exp-TKD128_BIAS;
	exn=0;

	if((ex>32) || (ex<(-32)))
		exn=1;

	ct=tbuf;
	if(val.sgn)
		*ct++='-';
	if(exn)
	{
		*ct++='0';
		*ct++='.';
		strcpy(ct, tb1+2);
		ct+=strlen(ct);
		*ct++='E';
		sprintf(ct, "%+d", ex);
	}else
		if(ex<0)
	{
		k=(-ex)-1;
		*ct++='0';
		*ct++='.';
		while(k--)
			*ct++='0';
		strcpy(ct, tb1+2);
	}else
	{
		k=ex+1;
		cs=tb1+2;

		while(k && *cs=='0')
			{ *cs++; k--; }

		while(k && *cs)
			{ *ct++=*cs++; k--; }
		if(*cs)
		{
			*ct++='.';
			strcpy(ct, cs);
		}else
		{
			while(k--)
				*ct++='0';
			*ct++='.';
			*ct++='0';
			*ct=0;
		}
	}
	
	return(TKD128_RStrDup(tbuf));
}

u32 TKD128_OprParse30ToString(char *str)
{
	u32 v;
	int i;
	
	v=0;
	for(i=0; i<9; i++)
		{ v=(v*10)+(str[i]-'0'); }
	return(v);
}

TKD128_Unpacked4x30 TKD128_OprParseFromString(char *str)
{
	TKD128_Unpacked4x30 val;
	char tb[64];
	char *cs, *ct;
	int exh, exn, exsz, ex, exadj, sgn;
	
	cs=str;
	ct=tb;

	sgn=0;
	if(*cs=='+')
		{ cs++; sgn=0; }
	if(*cs=='-')
		{ cs++; sgn=1; }
	
	while(*cs=='0')
		cs++;
	
	*ct++='0';
	*ct++='0';
	
	exh=0;
	while((*cs>='0') && (*cs<='9') && (exh<34))
		{ *ct++=*cs++; exh++; }
	if(exh>=34)
	{
		while((*cs>='0') && (*cs<='9'))
			{ cs++; exh++; }
	}else
	{
		exn=0;
		exsz=0;
		if(*cs=='.')
		{
			cs++;
			
			if(!exh)
			{
				while(*cs=='0')
					{ cs++; exsz++; }
			}
			
			while((*cs>='0') && (*cs<='9') && ((exh+exn)<34))
				{ *ct++=*cs++; exn++; }
		}
		while((exh+exn)<34)
			{ *ct++='0'; exn++; }

		while((*cs>='0') && (*cs<='9'))
			{ cs++; }
	}
	*ct=0;
	
	exadj=0;
	if((*cs=='e') || (*cs=='E'))
	{
		if(cs[1]=='+')
			{ exadj=atoi(cs+2); }
		else
			{ exadj=atoi(cs+1); }
	}
	
	ex=TKD128_BIAS+exh+exadj-1-exsz;

	val.sgn=sgn;
	val.exp=ex;
	val.m[3]=TKD128_OprParse30ToString(tb+ 0);
	val.m[2]=TKD128_OprParse30ToString(tb+ 9);
	val.m[1]=TKD128_OprParse30ToString(tb+18);
	val.m[0]=TKD128_OprParse30ToString(tb+27);
	return(val);
}

TKD128_Packed128 TKD128_OprAddDPD(
	TKD128_Packed128 vala,
	TKD128_Packed128 valb)
{
	TKD128_Packed128 vpc;
	TKD128_Unpacked4x30 vna, vnb, vnc;

	vna=TKD128_UnpackValueDPD(vala);
	vnb=TKD128_UnpackValueDPD(valb);
	vnc=TKD128_OprAdd(vna, vnb);
	vpc=TKD128_PackValueDPD(vnc);
	return(vpc);
}

TKD128_Packed128 TKD128_OprMulDPD(
	TKD128_Packed128 vala,
	TKD128_Packed128 valb)
{
	TKD128_Packed128 vpc;
	TKD128_Unpacked4x30 vna, vnb, vnc;

	vna=TKD128_UnpackValueDPD(vala);
	vnb=TKD128_UnpackValueDPD(valb);
	vnc=TKD128_OprMul(vna, vnb);
	vpc=TKD128_PackValueDPD(vnc);
	return(vpc);
}

TKD128_Packed128 TKD128_OprSubDPD(
	TKD128_Packed128 vala,
	TKD128_Packed128 valb)
{
	TKD128_Packed128 vpc;
	TKD128_Unpacked4x30 vna, vnb, vnc;

	vna=TKD128_UnpackValueDPD(vala);
	vnb=TKD128_UnpackValueDPD(valb);
	vnc=TKD128_OprSub(vna, vnb);
	vpc=TKD128_PackValueDPD(vnc);
	return(vpc);
}

TKD128_Packed128 TKD128_OprDivDPD(
	TKD128_Packed128 vala,
	TKD128_Packed128 valb)
{
	TKD128_Packed128 vpc;
	TKD128_Unpacked4x30 vna, vnb, vnc;

	vna=TKD128_UnpackValueDPD(vala);
	vnb=TKD128_UnpackValueDPD(valb);
	vnc=TKD128_OprDiv(vna, vnb);
	vpc=TKD128_PackValueDPD(vnc);
	return(vpc);
}

int TKD128_OprCompareDPD(
	TKD128_Packed128 vala,
	TKD128_Packed128 valb)
{
	TKD128_Packed128 vpc;
	TKD128_Unpacked4x30 vna, vnb;
	int i;

	vna=TKD128_UnpackValueDPD(vala);
	vnb=TKD128_UnpackValueDPD(valb);
	i=TKD128_OprCompare(vna, vnb);
	return(i);
}

TKD128_Packed128 TKD128_OprSqrtDPD(TKD128_Packed128 vala)
{
	TKD128_Packed128 vpc;
	TKD128_Unpacked4x30 vna, vnb, vnc;

	vna=TKD128_UnpackValueDPD(vala);
	vnc=TKD128_OprSqrt(vna);
	vpc=TKD128_PackValueDPD(vnc);
	return(vpc);
}


TKD128_Packed128 TKD128_OprAddX30(
	TKD128_Packed128 vala,
	TKD128_Packed128 valb)
{
	TKD128_Packed128 vpc;
	TKD128_Unpacked4x30 vna, vnb, vnc;

//	vna=TKD128_UnpackValueX30(vala);
//	vnb=TKD128_UnpackValueX30(valb);
	TKD128_UnpackValueX30R(&vna, &vala);
	TKD128_UnpackValueX30R(&vnb, &valb);
	vnc=TKD128_OprAdd(vna, vnb);
//	vpc=TKD128_PackValueX30(vnc);
	TKD128_PackValueX30R(&vnc, &vpc);
	return(vpc);
}

TKD128_Packed128 TKD128_OprMulX30(
	TKD128_Packed128 vala,
	TKD128_Packed128 valb)
{
	TKD128_Packed128 vpc;
	TKD128_Unpacked4x30 vna, vnb, vnc;

//	vna=TKD128_UnpackValueX30(vala);
//	vnb=TKD128_UnpackValueX30(valb);
	TKD128_UnpackValueX30R(&vna, &vala);
	TKD128_UnpackValueX30R(&vnb, &valb);
	vnc=TKD128_OprMul(vna, vnb);
//	vpc=TKD128_PackValueX30(vnc);
	TKD128_PackValueX30R(&vnc, &vpc);
	return(vpc);
}

char *TKD128_OprDumpAsStringDPD(TKD128_Packed128 val)
{
	return(TKD128_OprDumpAsString(TKD128_UnpackValueDPD(val)));
}

TKD128_Packed128 TKD128_OprParseFromStringDPD(char *str)
{
	return(TKD128_PackValueDPD(TKD128_OprParseFromString(str)));
}

// #define TEST_PI_DPD_HI		0x4329F0D84E95AF97
// #define TEST_PI_DPD_LO		0x7B1E09EE2B4D7F06
// 2F3F 1D4C 5D9C 1E8A
// 5E7F 1A5B 2C3A 5E3A

//#define TEST_PI_DPD_HI		0x2200431857ACAD71ULL
// #define TEST_PI_DPD_LO		0xF6F1CAC9A334F3DBULL

#define TEST_PI_DPD_HI		0x2601934b9c0c0000ULL
#define TEST_PI_DPD_LO		0x0000000000000000ULL

// #define TEST_PI_DPD_HI		0x2e078c2aeb53b3fbULL
// #define TEST_PI_DPD_LO		0xb4e262d0dab5e680ULL


int main()
{
	TKD128_Packed128 vp0, vp1, vp2;
	TKD128_Unpacked4x30 vn0, vn1, vn2, vn3;
	u32 v0, v1, v2, v3;
	int t0, t1, t2, t0e;
	int i, n;
	
	TKD128_InitDPD();
	
	v0=0x12345678;

	v2=v0&1023;
	if(v2>=1000)	v2-=1000;
	v3=v0%1000;
	
	printf("A %d %d\n", v2, v3);
	
	v1=((((u64)v0)<<10)+(((u64)v0)<<4)+(((u64)v0)<<3))>>20;
	
//	v1=v0>>10;
//	v1=(v0+(v0>>6)+(v0>>7))>>10;
//	v1=v0/1000;
	v2=v0-v1*1000;
	if(v2>1000)
	{
		v1++;
		v2=v0-v1*1000;
	}
	printf("B %d (%X) %d %d\n", v2, v2, v1, v0/1000);
	
#if 1
	vp0.hi=TEST_PI_DPD_HI;
	vp0.lo=TEST_PI_DPD_LO;
	vn0=TKD128_UnpackValueDPD(vp0);
	printf("DPD Test:  %s\n",
		TKD128_OprDumpAsString(vn0));
#endif


	vn0.sgn=0;
	vn1.sgn=0;
	vn0.exp=TKD128_BIAS;
	vn1.exp=TKD128_BIAS;
	vn0.m[3]=  1234567;
	vn0.m[2]=890000000;
	vn0.m[1]=0;
	vn0.m[0]=0;

	vn1.m[3]=  3141592;
	vn1.m[2]=653589793;
	vn1.m[1]=238462643;
	vn1.m[0]=383279500;

	vp0=TKD128_PackValueDPD(vn0);
	vp1=TKD128_PackValueDPD(vn1);

	printf("%016llX %016llX\n", vp1.hi, vp1.lo);
	vn1=TKD128_UnpackValueDPD(vp1);
	
	for(i=0; i<16; i++)
	{
		printf("%02X ", ((byte *)(&vp1))[i]);
	}
	printf("\n");

	vn2=TKD128_OprParseFromString("987.654321");
	printf("Parse Test 1:  %s\n", TKD128_OprDumpAsString(vn2));
	vn3=TKD128_OprSqrt(vn2);
	printf("Parse Test 1 (Sqrt):  %s\n", TKD128_OprDumpAsString(vn3));

	vn2=TKD128_OprParseFromString("0.0707123");
	printf("Parse Test 2:  %s\n", TKD128_OprDumpAsString(vn2));

	t0=clock(); t0e=t0+CLOCKS_PER_SEC;
	t1=t0;	n=0;
	while(t1<t0e)
	{
		n++;
		for(i=0; i<10000; i++)
			vn2=TKD128_OprMul(vn0, vn1);
		t1=clock();
	}
	printf("MUL %d\n", n);

	printf("  %s\n  %s\n  %s\n",
		TKD128_OprDumpAsString(vn0),
		TKD128_OprDumpAsString(vn1),
		TKD128_OprDumpAsString(vn2));

	t0=clock(); t0e=t0+CLOCKS_PER_SEC;
	t1=t0;	n=0;
	while(t1<t0e)
	{
		n++;
		for(i=0; i<10000; i++)
			vn2=TKD128_OprAdd(vn0, vn1);
		t1=clock();
	}
	printf("ADD %d\n", n);
	printf("  %s\n  %s\n  %s\n",
		TKD128_OprDumpAsString(vn0),
		TKD128_OprDumpAsString(vn1),
		TKD128_OprDumpAsString(vn2));

	vn3=TKD128_OprSub(vn0, vn1);
	printf("SUB %d\n", n);
	printf("  %s\n  %s\n  %s\n",
		TKD128_OprDumpAsString(vn0),
		TKD128_OprDumpAsString(vn1),
		TKD128_OprDumpAsString(vn3));


	t0=clock(); t0e=t0+CLOCKS_PER_SEC;
	t1=t0;	n=0;
	while(t1<t0e)
	{
		n++;
		for(i=0; i<10000; i++)
			vn3=TKD128_OprDiv(vn0, vn1);
		t1=clock();
	}

//	vn3=TKD128_OprDiv(vn0, vn1);
	printf("DIV %d\n", n);
	printf("  %s\n  %s\n  %s\n",
		TKD128_OprDumpAsString(vn0),
		TKD128_OprDumpAsString(vn1),
		TKD128_OprDumpAsString(vn3));

#if 1
	t0=clock(); t0e=t0+CLOCKS_PER_SEC;
	t1=t0;	n=0;
	while(t1<t0e)
	{
		n++;
		for(i=0; i<10000; i++)
			vn3=TKD128_OprSqrt(vn1);
		t1=clock();
	}
#endif

//	vn3=TKD128_OprSqrtApx(vn1);
//	vn3=TKD128_OprSqrt(vn1);
	printf("SQRT %d\n", n);
	printf("  %s\n  %s\n",
		TKD128_OprDumpAsString(vn1),
		TKD128_OprDumpAsString(vn3));

//	vp0=vp2;

	t0=clock(); t0e=t0+CLOCKS_PER_SEC;
	t1=t0;	n=0;
	while(t1<t0e)
	{
		n++;
		for(i=0; i<10000; i++)
			vn2=TKD128_UnpackValueX30(vp0);
		t1=clock();
	}
	printf("UP X30 %d\n", n);

	t0=clock(); t0e=t0+CLOCKS_PER_SEC;
	t1=t0;	n=0;
	while(t1<t0e)
	{
		n++;
		for(i=0; i<10000; i++)
			vn2=TKD128_UnpackValueDPD(vp0);
		t1=clock();
	}
	printf("UP DPD %d\n", n);

	t0=clock(); t0e=t0+CLOCKS_PER_SEC;
	t1=t0;	n=0;
	while(t1<t0e)
	{
		n++;
		for(i=0; i<10000; i++)
			vp2=TKD128_PackValueX30(vn0);
		t1=clock();
	}
	printf("PK X30 %d\n", n);

	t0=clock(); t0e=t0+CLOCKS_PER_SEC;
	t1=t0;	n=0;
	while(t1<t0e)
	{
		n++;
		for(i=0; i<10000; i++)
			vp2=TKD128_PackValueDPD(vn0);
		t1=clock();
	}
	printf("PK DPD %d\n", n);


	t0=clock(); t0e=t0+CLOCKS_PER_SEC;
	t1=t0;	n=0;
	while(t1<t0e)
	{
		n++;
		for(i=0; i<10000; i++)
			vp2=TKD128_OprMulDPD(vp0, vp1);
		t1=clock();
	}
	printf("MUL(DPD) %d\n", n);

	t0=clock(); t0e=t0+CLOCKS_PER_SEC;
	t1=t0;	n=0;
	while(t1<t0e)
	{
		n++;
		for(i=0; i<10000; i++)
			vp2=TKD128_OprAddDPD(vp0, vp1);
		t1=clock();
	}
	printf("ADD(DPD) %d\n", n);


	vp0=TKD128_PackValueX30(vn0);
	vp1=TKD128_PackValueX30(vn1);

	t0=clock(); t0e=t0+CLOCKS_PER_SEC;
	t1=t0;	n=0;
	while(t1<t0e)
	{
		n++;
		for(i=0; i<10000; i++)
			vp2=TKD128_OprMulX30(vp0, vp1);
		t1=clock();
	}
	printf("MUL(X30) %d\n", n);

	t0=clock(); t0e=t0+CLOCKS_PER_SEC;
	t1=t0;	n=0;
	while(t1<t0e)
	{
		n++;
		for(i=0; i<10000; i++)
			vp2=TKD128_OprAddX30(vp0, vp1);
		t1=clock();
	}
	printf("ADD(X30) %d\n", n);
}

