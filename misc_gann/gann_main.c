#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>

#include "gann.h"
#include "gann_fpn.c"
#include "gann_maj.c"


// #define GANN_TEST_STATETABLE	//mock up, branch-predictor FSM


char *test0;

char gann_charset_dec[64];
char gann_charset_enc[256];

static byte gann_nnbuf[1<<24];
static u16 gann_brseq;
static int gann_bxid;

int GANN_Test_CacheMember(GANN_Context *ctx, GANN_Member *mm)
{
	if(mm->brseq!=gann_brseq)
	{
//		memset(gann_nnbuf, 0, 1<<16);
		GANN_GetMemberAsBytes(mm, gann_nnbuf);
		gann_brseq=mm->brseq;
	}
	return(0);
}

#if 0
int GANN_Test_RunMember0(GANN_Context *ctx, GANN_Member *mm,
	u64 *mbits)
{
//	static byte nbuf[1<<14];
//	static u16 brseq;
	u64 *qnb;
	u64 l0bv[16];
	u64 l1bv[16];
	u64 l2bv[16];
	u64 l3bv[16];
	u64 l4bv[4];
	u64 bv, wv, wv0, wv1;
	int len, ix, bx, bi, bd;
	int i, j, k, ev, pen, pen0, pen1;
	
#if 0
	if(mm->brseq!=brseq)
	{
		memset(nbuf, 0, 1<<14);
		GANN_GetMemberAsBytes(mm, nbuf);
		brseq=mm->brseq;
	}
	qnb=(u64 *)nbuf;
#endif

	GANN_Test_CacheMember(ctx, mm);
	qnb=(u64 *)gann_nnbuf;
	
	pen=0;
	pen0=0;
	pen1=0;

	for(i=0; i<16; i++)
	{
		l0bv[i]=0;
		l1bv[i]=0;
		l2bv[i]=0;
		l3bv[i]=0;
	}
	for(i=0; i<4; i++)
	{
		l4bv[i]=0;
	}
		
	for(i=0; i<256; i++)
	{
		bv=mbits[i>>4];
		wv0=qnb[i*2+0];
		wv1=qnb[i*2+1];

		k=GANN_WeightBit64X2(bv, wv0, wv1);
		if((k&7)==0)	pen0++;
		if((k&7)==7)	pen1++;

//		l0bv[i>>4]=
		l0bv[i&15]=
//			(l0bv[i>>4]<<4)|
			(l0bv[i&15]<<4)|
			k;
	}

	bx=512;
	for(i=0; i<256; i++)
	{
		bv=l0bv[i>>4];
		wv0=qnb[bx+i];
		
		k=GANN_WeightBit16X4B(bv, wv0);
		if((k&7)==0)	pen0++;
		if((k&7)==7)	pen1++;

//		l1bv[i>>4]=
		l1bv[i&15]=
//			(l1bv[i>>4]<<4)|
			(l1bv[i&15]<<4)|
			k;
	}

#if 1
	bx=768;
	for(i=0; i<256; i++)
	{
		bv=l1bv[i>>4];
		wv0=qnb[bx+i];

		k=GANN_WeightBit16X4B(bv, wv0);
		if((k&7)==0)	pen0++;
		if((k&7)==7)	pen1++;

//		l2bv[i>>4]=
		l2bv[i&15]=
//			(l2bv[i>>4]<<4)|
			(l2bv[i&15]<<4)|
			k;
	}
#endif

#if 1
	bx=1024;
	for(i=0; i<256; i++)
	{
		bv=l2bv[i>>4];
		wv0=qnb[bx+i];

		k=GANN_WeightBit16X4B(bv, wv0);
		if((k&7)==0)	pen0++;
		if((k&7)==7)	pen1++;

//		l3bv[i>>4]=
		l3bv[i&15]=
//			(l3bv[i>>4]<<4)|
			(l3bv[i&15]<<4)|
			k;
	}
#endif

//	bx=1024;
	bx=1280;
	for(i=0; i<64; i++)
	{
//		bv=l1bv[i>>2];
//		bv=l2bv[i>>2];
		bv=l3bv[i>>2];
		wv0=qnb[bx+i];

		k=GANN_WeightBit16X4B(bv, wv0);
		if((k&7)==0)	pen0++;
		if((k&7)==7)	pen1++;

//		l4bv[i>>4]=
		l4bv[i&3]=
//			(l4bv[i>>4]<<4)|
			(l4bv[i&3]<<4)|
			k;
	}
	
	gann_bxid=bx+64;
	
	bi=0; bd=-1;
	wv=0; wv0=0; wv1=0;
	for(i=0; i<64; i++)
	{
		bv=l4bv[i>>4];
		j=(bv>>((i&15)*4))&15;
		
		if(j&8)
			continue;

		if(j>bd)
		{
			bi=i; bd=j;
		}

		wv|=1LL<<i;			
		if((j&7)>=2)
			wv0|=1LL<<i;
		if((j&7)>=5)
			wv1|=1LL<<i;
	}

//	wv|=1LL<<bi;

	pen=(pen1>>1)+(pen0>>7);
	
	ctx->wv0=wv0;
	ctx->wv1=wv1;
	ctx->pen=pen;
	
//	return(wv);
	return(bi);
}
#endif

#define GANN_DOWEAKEN

#if 1
#define NEURS_I1		8
#define NEURS_H1		8
#define NEURS_H2		8
#define NEURS_H3		8
#define NEURS_O1		6

#define NEURT_H2_ENA
#define NEURT_H3_ENA

#define NEUR_I1SR		0
#define NEUR_H1SR		0
#define NEUR_H2SR		0
#define NEUR_H3SR		0
#endif

#if 0
#define NEURS_I1		10
#define NEURS_H1		10
#define NEURS_H2		10
#define NEURS_H3		10
#define NEURS_O1		6

#define NEURT_H2_ENA
// #define NEURT_H3_ENA

#define NEUR_I1SR		3
#define NEUR_H1SR		3
#define NEUR_H2SR		3
#define NEUR_H3SR		3

#endif


#if 0
#define NEURS_I1		11
#define NEURS_H1		11
#define NEURS_H2		11
#define NEURS_H3		11
#define NEURS_O1		6

#define NEURT_H2_ENA
#define NEURT_H3_ENA

#define NEUR_I1SR		6
#define NEUR_H1SR		6
#define NEUR_H2SR		6
#define NEUR_H3SR		6

#endif

#define NEURT_I1		(1<<NEURS_I1)
#define NEURT_H1		(1<<NEURS_H1)
#define NEURT_H2		(1<<NEURS_H2)
#define NEURT_H3		(1<<NEURS_H3)
#define NEURT_O1		(1<<NEURS_O1)

#define NEUR_I1SL		(NEURS_I1-NEUR_I1SR-3)
#define NEUR_H1SL		(NEURS_H1-NEUR_H1SR-3)
#define NEUR_H2SL		(NEURS_H2-NEUR_H2SR-3)
#define NEUR_H3SL		(NEURS_H3-NEUR_H3SR-3)

#define NEURV_I1		(NEURT_I1>>3)
#define NEURV_H1		(NEURT_H1>>3)
#define NEURV_H2		(NEURT_H2>>3)
#define NEURV_H3		(NEURT_H3>>3)
#define NEURV_O1		(NEURT_O1>>3)

#define NEURVM_I1		(NEURV_I1-1)
#define NEURVM_H1		(NEURV_H1-1)
#define NEURVM_H2		(NEURV_H2-1)
#define NEURVM_H3		(NEURV_H3-1)
#define NEURVM_O1		(NEURV_O1-1)

#define NEUR_I1SM		((1<<NEUR_I1SR)-1)
#define NEUR_H1SM		((1<<NEUR_H1SR)-1)
#define NEUR_H2SM		((1<<NEUR_H2SR)-1)
#define NEUR_H3SM		((1<<NEUR_H3SR)-1)

#if 1
int GANN_Test_RunMember0(GANN_Context *ctx, GANN_Member *mm,
	u64 *mbits)
{
	u64 *qnb;
	u64 l0bv[1024];
	u64 l1bv[1024];
	u64 l2bv[1024];
	u64 l3bv[1024];
	u64 l4bv[8];
	u64 bv, wv, wv0, wv1, wv2, wv3, bx0;
	int len, ix, bx, bi, bd, didweak, enaweak;
	int i, j, k, ev, pen, pen0, pen1;

#if 1
	
	GANN_Test_CacheMember(ctx, mm);

	qnb=(u64 *)gann_nnbuf;
	
	pen=0;
	pen0=0;
	pen1=0;

	for(i=0; i<NEURV_H1; i++)
	{
		l0bv[i]=0;
		l1bv[i]=0;
		l2bv[i]=0;
		l3bv[i]=0;
	}
	for(i=0; i<NEURV_O1; i++)
	{
		l4bv[i]=0;
	}
	
	enaweak=mm->uflag&1;
	didweak=0;
	
	bx=0;
//	for(i=0; i<1024; i++)
//	for(i=0; i<256; i++)
	for(i=0; i<NEURT_I1; i++)
	{
		k=0;
		bx0=bx;

#if 0
		for(j=0; j<64; j++)
		{
			bv=mbits[j];
			wv0=qnb[bx+0];
			wv1=qnb[bx+1];
			bx+=2;
			if(j>8)
				continue;
			k=GANN_WeightBit64X2_F16A(bv, wv0, wv1, k);
		}
#endif

#if 1
		for(j=0; j<32; j++)
		{
			bv=mbits[j];
			wv0=qnb[bx+0];
			wv1=qnb[bx+1];
			wv2=qnb[bx+2];
			wv3=qnb[bx+3];
			bx+=4;
//			if(j>8)
//				continue;
			k=GANN_WeightBit16X3A_F16A(bv>> 0, wv0, k);
			k=GANN_WeightBit16X3A_F16A(bv>>16, wv1, k);
			k=GANN_WeightBit16X3A_F16A(bv>>32, wv2, k);
			k=GANN_WeightBit16X3A_F16A(bv>>48, wv3, k);
		}
		
		ev=(wv3>>48)&255;
		ev=gann_fp8to16[ev];
		k=GANN_AddFp16J(ev, k);
#endif

		k=(k&0x8000)|(((k&0x7FFF)>>1)+0x1FFF);

		k=gann_fp16to8t[k>>4];
		if((k&0x7F)==0x00)	pen0++;
//		if((k&0x7F)==0x7F)	pen1++;
		if((k&0x7F)>=0x60)	pen1++;
		if((k&0x7F)==0x7F)	pen1+=3;

#ifdef GANN_DOWEAKEN
		if(((k&0x7F)>=0x60) && enaweak)
		{
			ctx->mstat_sk[7]++;
			didweak=1;

			for(j=0; j<32*4; j++)
			{
				wv0=qnb[bx0+j];
				ev=wv0>>56;
				if(ev&0x7F)
					ev--;
				wv1=(wv0&0x00FFFFFFFFFFFFFFULL)|(((u64)ev)<<56);
				qnb[bx0+j]=wv1;
			}
		}
#endif

//		l0bv[i&31]=(l0bv[i&31]<<8)|k;
		l0bv[i&NEURVM_I1]=(l0bv[i&NEURVM_I1]<<8)|k;
	}

//	for(i=0; i<1024; i++)
//	for(i=0; i<256; i++)
	for(i=0; i<NEURT_H1; i++)
	{
		bx0=bx;
		k=0;
//		for(j=0; j<127; j++)
//		for(j=0; j<31; j++)
		for(j=0; j<NEURVM_I1>>NEUR_I1SR; j++)
		{
//			bv=l0bv[j];
//			bv=l0bv[(j<<NEUR_I1SR)+(i&NEUR_I1SM)];
			bv=l0bv[j+((i&NEUR_I1SM)<<NEUR_I1SL)];
			wv=qnb[bx++];
			k=GANN_WeightBit8X8A16(bv, wv, k);
		}
//		bv=l0bv[j];
//		bv=l0bv[(j<<NEUR_I1SR)+(i&NEUR_I1SM)];
		bv=l0bv[j+((i&NEUR_I1SM)<<NEUR_I1SL)];
		wv=qnb[bx++];
		k=GANN_WeightBit8X7A16(bv, wv, k);
		
		k=gann_fp16to8t[k>>4];
		if((k&0x7F)==0x00)	pen0++;
//		if((k&0x7F)==0x7F)	pen1++;
		if((k&0x7F)>=0x60)	pen1++;
		if((k&0x7F)==0x7F)	pen1+=3;

#ifdef GANN_DOWEAKEN
		if(((k&0x7F)>=0x60) && enaweak)
//		if((k&0x7F)>=0x70)
//		if((k&0x7F)==0x7F)
		{
			ctx->mstat_sk[7]++;
			didweak=1;
		
//			for(j=0; j<31; j++)
			for(j=0; j<NEURVM_I1>>NEUR_I1SR; j++)
			{
				wv=qnb[bx0+j];
				wv=GANN_WeakenBit8X8(wv);
				qnb[bx0+j]=wv;
			}
			wv=qnb[bx0+j];
			wv=GANN_WeakenBit8X7(wv);
			qnb[bx0+j]=wv;
		}
#endif

//		l1bv[i&31]=
//			(l1bv[i&31]<<8)|
//			k;

		l1bv[i&NEURVM_H1]=(l1bv[i&NEURVM_H1]<<8)|k;
	}

#ifdef NEURT_H2_ENA
//	for(i=0; i<1024; i++)
//	for(i=0; i<256; i++)
	for(i=0; i<NEURT_H2; i++)
	{
		bx0=bx;
		k=0;
//		for(j=0; j<127; j++)
//		for(j=0; j<31; j++)
		for(j=0; j<NEURVM_H1>>NEUR_H1SR; j++)
		{
//			bv=l1bv[j];
//			bv=l1bv[(j<<NEUR_H1SR)|(i&NEUR_H1SM)];
			bv=l1bv[j+((i&NEUR_H1SM)<<NEUR_H1SL)];
			wv=qnb[bx++];
			k=GANN_WeightBit8X8A16(bv, wv, k);
		}
//		bv=l1bv[j];
//		bv=l1bv[(j<<NEUR_H1SR)|(i&NEUR_H1SM)];
		bv=l1bv[j+((i&NEUR_H1SM)<<NEUR_H1SL)];
		wv=qnb[bx++];
		k=GANN_WeightBit8X7A16(bv, wv, k);
		
		k=gann_fp16to8t[k>>4];
		if((k&0x7F)==0x00)	pen0++;
//		if((k&0x7F)==0x7F)	pen1++;
		if((k&0x7F)>=0x60)	pen1++;
		if((k&0x7F)==0x7F)	pen1+=3;

#ifdef GANN_DOWEAKEN
		if(((k&0x7F)>=0x60) && enaweak)
//		if((k&0x7F)>=0x60)
//		if((k&0x7F)>=0x70)
//		if((k&0x7F)==0x7F)
		{
			ctx->mstat_sk[7]++;
			didweak=1;

			for(j=0; j<NEURVM_H1>>NEUR_H1SR; j++)
			{
				wv=qnb[bx0+j];
				wv=GANN_WeakenBit8X8(wv);
				qnb[bx0+j]=wv;
			}
			wv=qnb[bx0+j];
			wv=GANN_WeakenBit8X7(wv);
			qnb[bx0+j]=wv;
		}
#endif

//		l2bv[i&31]=
//			(l2bv[i&31]<<8)|
//			k;

		l2bv[i&NEURVM_H2]=(l2bv[i&NEURVM_H2]<<8)|k;
	}
#else
	for(i=0; i<NEURV_H2; i++)
	{
		l2bv[i&NEURVM_H2]=l1bv[i&NEURVM_H1];
	}
#endif


#ifdef NEURT_H3_ENA
//	for(i=0; i<1024; i++)
//	for(i=0; i<256; i++)
	for(i=0; i<NEURT_H3; i++)
	{
		bx0=bx;
		k=0;
//		for(j=0; j<127; j++)
//		for(j=0; j<31; j++)
		for(j=0; j<NEURVM_H2>>NEUR_H2SR; j++)
		{
//			bv=l2bv[j];
//			bv=l2bv[(j<<NEUR_H2SR)|(i&NEUR_H2SM)];
			bv=l2bv[j+((i&NEUR_H2SM)<<NEUR_H2SL)];
			wv=qnb[bx++];
			k=GANN_WeightBit8X8A16(bv, wv, k);
		}
//		bv=l2bv[j];
//		bv=l2bv[(j<<NEUR_H2SR)|(i&NEUR_H2SM)];
		bv=l2bv[j+((i&NEUR_H2SM)<<NEUR_H2SL)];
		wv=qnb[bx++];
		k=GANN_WeightBit8X7A16(bv, wv, k);
		
		k=gann_fp16to8t[k>>4];
		if((k&0x7F)==0x00)	pen0++;
//		if((k&0x7F)==0x7F)	pen1++;
//		if((k&0x7F)>=0x60)	pen1++;
		if((k&0x7F)>=0x60)	pen1+=2;
		if((k&0x7F)==0x7F)	pen1+=5;

#ifdef GANN_DOWEAKEN
		if(((k&0x7F)>=0x60) && enaweak)
//		if((k&0x7F)>=0x60)
//		if((k&0x7F)>=0x70)
//		if((k&0x7F)==0x7F)
		{
			ctx->mstat_sk[7]++;
			didweak=1;

//			for(j=0; j<31; j++)
			for(j=0; j<NEURVM_H2>>NEUR_H2SR; j++)
			{
				wv=qnb[bx0+j];
				wv=GANN_WeakenBit8X8(wv);
				qnb[bx0+j]=wv;
			}
			wv=qnb[bx0+j];
			wv=GANN_WeakenBit8X7(wv);
			qnb[bx0+j]=wv;
		}
#endif

//		l3bv[i&31]=
//			(l3bv[i&31]<<8)|
//			k;

		l3bv[i&NEURVM_H3]=(l3bv[i&NEURVM_H3]<<8)|k;
	}
#else
	for(i=0; i<NEURV_H3; i++)
	{
		l3bv[i&NEURVM_H3]=l2bv[i&NEURVM_H2];
	}
#endif

//	for(i=0; i<64; i++)
	for(i=0; i<NEURT_O1; i++)
	{
		bx0=bx;
		k=0;
//		for(j=0; j<127; j++)
//		for(j=0; j<31; j++)
		for(j=0; j<NEURVM_H3>>NEUR_H3SR; j++)
		{
//			bv=l3bv[j];
//			bv=l1bv[j];
//			bv=l3bv[(j<<NEUR_H3SR)|(i&NEUR_H3SM)];
			bv=l3bv[j+((i&NEUR_H3SM)<<NEUR_H3SL)];
			wv=qnb[bx++];
			k=GANN_WeightBit8X8A16(bv, wv, k);
		}
//		bv=l3bv[j];
//		bv=l3bv[(j<<NEUR_H3SR)|(i&NEUR_H3SM)];
		bv=l3bv[j+((i&NEUR_H3SM)<<NEUR_H3SL)];
		wv=qnb[bx++];
		k=GANN_WeightBit8X7A16(bv, wv, k);
		
		k=gann_fp16to8t[k>>4];
		if((k&0x7F)==0x00)	pen0++;
//		if((k&0x7F)==0x7F)	pen1++;
		if((k&0x7F)>=0x60)	pen1+=4;
		if((k&0x7F)==0x7F)	pen1+=8;

#ifdef GANN_DOWEAKEN
		if(((k&0x7F)>=0x60) && enaweak)
//		if((k&0x7F)>=0x60)
//		if((k&0x7F)>=0x70)
//		if((k&0x7F)==0x7F)
		{
			ctx->mstat_sk[7]++;
			didweak=1;

//			for(j=0; j<31; j++)
			for(j=0; j<NEURVM_H3>>NEUR_H3SR; j++)
			{
				wv=qnb[bx0+j];
				wv=GANN_WeakenBit8X8(wv);
				qnb[bx0+j]=wv;
			}
			wv=qnb[bx0+j];
			wv=GANN_WeakenBit8X7(wv);
			qnb[bx0+j]=wv;
		}
#endif

//		l4bv[i&7]=
//			(l4bv[i&7]<<8)|
//			k;

		l4bv[i&NEURVM_O1]=(l4bv[i&NEURVM_O1]<<8)|k;
	}
	
	gann_bxid=bx;
	
	bi=0; bd=-1;
	wv=0; wv0=0; wv1=0;
	for(i=0; i<64; i++)
	{
		bv=l4bv[i>>3];
		j=(bv>>((i&7)*8))&255;
		
		if(j&0x80)
			continue;

		if(j>bd)
		{
			bi=i; bd=j;
		}

		wv|=1LL<<i;			
		if((j&0x7F)>=0x38)
			wv0|=1LL<<i;
		if((j&0x7F)>=0x48)
			wv1|=1LL<<i;
	}

//	if(pen1>0)
	if(didweak)
	{
		GANN_ModifyMemberAsBytes(mm, gann_nnbuf);
	}

//	wv|=1LL<<bi;

//	if((pen1>64) && (mm->curix>=4))
//	if((pen1>(NEURT_H1>>2)) && (mm->curix>=4))
	if((pen1>NEURT_H1) && (mm->curix>=2))
	{
		mm->uflag|=1;
	}

//	pen=(pen1>>1)+(pen0>>7);
	pen=(pen1<<2)+(pen0>>7);
	
	mm->wv0=wv0;
	mm->wv1=wv1;
	mm->pen=pen;

#endif

	
//	return(wv);
	return(bi);
}
#endif

int GANN_Test_TestMember0(GANN_Context *ctx, GANN_Member *mm)
{
//	static byte nbuf[1<<14];
//	u64 *qnb;
	u64 mbits[64];
//	u64 l0bv[16];
//	u64 l1bv[16];
//	u64 l2bv[16];
//	u64 l3bv[4];
	byte pgc[64];
	u64 bv, wv, wv0, wv1, mbcur;
	int len, lmask, ix, bx, bi, bd;
	int probe, terr, trng;
	int i, j, k, ev, pen;
	
//	GANN_GetMemberAsBytes(mm, nbuf);
//	qnb=(u64 *)nbuf;
	terr=0;

	trng=ctx->curgen;
	trng=(trng<<1)^(trng>>17);
	trng=(trng<<1)^(trng>>17);
	trng=(trng<<1)^(trng>>17);

	len=strlen(test0);
	
	lmask=1;
	while(lmask<len)
		lmask=lmask<<1;
	lmask--;
	
	mm->uflag=0;
	
//	for(probe=0; probe<32; probe++)
//	for(probe=0; probe<8; probe++)
//	for(probe=0; probe<16; probe++)
//	for(probe=0; probe<24; probe++)
//	for(probe=0; probe<12; probe++)
	for(probe=0; probe<6; probe++)
	{
		trng=(trng<<1)^(trng>>13);
//		ix=((trng*65521)>>16)&0xFFFF;
		ix=((trng*65521)>>16)&lmask;

//		ix=GANN_Rand16();
		while((ix+4)>=len)
			ix-=len;
		
//		if(probe<8)
//			ix=probe;
		
		pen=0;
		
		for(i=0; i<64; i++)
		{
			if((ix-i)<0)
			{
				mbits[i]=0;
				continue;
			}
		
			j=test0[ix-i-1];
			k=gann_charset_enc[j];
			mbits[i]=1ULL<<k;
		}

		j=test0[ix];
		k=gann_charset_enc[j];
		mbcur=1ULL<<k;

		bi=GANN_Test_RunMember0(ctx, mm, mbits);
		wv0=mm->wv0;
		wv1=mm->wv1;
		pen=mm->pen;

		wv|=1LL<<bi;
		
//		ev=pen>>2;
		ev=pen;

		if(wv&(wv-1))
			ev+=64;
			
		pgc[probe]=' '+bi;

		for(i=0; i<64; i++)
		{
			if(mbcur&(1LL<<i))
			{
				if(!(wv&(1LL<<i)))
				{
					ev+=256;
				}

				if(!(wv0&(1LL<<i)))
					{ ev+=128; }
				if(!(wv1&(1LL<<i)))
					{ ev+=32; }
			}else
			{
				if(wv&(1LL<<i))
					ev+=8;
				if(wv0&(1LL<<i))
					{ ev+=4; }
				if(wv1&(1LL<<i))
					{ ev+=4; }
			}
		}
		
		terr+=ev;
	}
	
//	if(	(pgc[0]==pgc[1]) &&
//		(pgc[1]==pgc[2]) &&
//		(pgc[2]==pgc[3]) &&
//		(pgc[3]==pgc[4]) &&
//		(pgc[4]==pgc[5]) &&
//		(pgc[5]==pgc[6]) &&
//		(pgc[6]==pgc[7]) )

	if(	(pgc[0]==pgc[1]) &&
		(pgc[1]==pgc[2]) &&
		(pgc[2]==pgc[3]) )
	{
		terr+=2048;
	}

#if 0
	for(i=0; i<64; i++)
		mbits[i]=0;

	for(i=0; i<8; i++)
	{
		bi=GANN_Test_RunMember0(ctx, mm, mbits);
		pgc[i]=gann_charset_dec[bi];
		for(k=63; k>0; k--)
			{ mbits[k]=mbits[k-1]; }
		mbits[0]=1LL<<bi;
	}

	if(	(pgc[0]==pgc[1]) &&
		(pgc[1]==pgc[2]) &&
		(pgc[2]==pgc[3]) )
	{
		terr+=2048;
	}

	if(	(pgc[0]==pgc[1]) ||
		(pgc[1]==pgc[2]) ||
		(pgc[2]==pgc[3]) )
			{ terr+=512; }

	if(	(pgc[0]==pgc[2]) ||
		(pgc[1]==pgc[3]) )
			{ terr+=512; }
	if(	(pgc[2]==pgc[4]) ||
		(pgc[3]==pgc[5]) )
			{ terr+=512; }
	if(	(pgc[4]==pgc[6]) ||
		(pgc[5]==pgc[7]) )
			{ terr+=512; }

	if(	(pgc[0]==pgc[3]) ||
		(pgc[1]==pgc[4]) ||
		(pgc[2]==pgc[5]) )
			{ terr+=384; }

	if(	(pgc[0]==pgc[4]) ||
		(pgc[1]==pgc[5]) ||
		(pgc[2]==pgc[6]) ||
		(pgc[3]==pgc[7]) )
			{ terr+=384; }
#endif

//	return(ev);
	return(terr);
}

int GANN_Test_TestStateTable0(GANN_Context *ctx, GANN_Member *mm)
{
	static u64 pattab[16]={
		0x0000000000000000ULL,		//0, 0000
		0xFFFFFFFFFFFFFFFFULL,		//1, 1111
		0x5555555555555555ULL,		//2, 0101
		0xAAAAAAAAAAAAAAAAULL,		//3, 1010
		0x6DB6DB6DB6DB6DB6ULL,		//4, 110110
		0x9249249249249249ULL,		//5, 001001
		0x1111111111111111ULL,		//6, 00010001
		0xEEEEEEEEEEEEEEEEULL,		//7, 11101110
		0xCCCCCCCCCCCCCCCCULL,		//8, 11001100
		0x0820820820820820ULL,		//9, 100000100000
		0x0842108421084210ULL,		//A, 1000010000
		0xF7BDEF7BDEF7BDEFULL,		//B, 0111101111
		0x5555555555555555ULL,		//C
		0xAAAAAAAAAAAAAAAAULL,		//D
		0x6DB6DB6DB6DB6DB6ULL,		//E
		0x9249249249249249ULL,		//F
	};
	static u64 modtab[16]={
		64,		64,		64,		64,
		63,		63,		64,		64,
		64,		60,		60,		60,
		64,		64,		64,		64
	};

//	u64 mbits[64];
	byte pgc[64];
	byte *sttab;
	u64 bv, wv, wv0, wv1, mbcur;
	int len, lmask, ix, bx, bi, bd;
	int probe, terr, trng;
	int i, j, k, l, p, ev, pen;
	
	GANN_GetMemberAsBytes(mm, gann_nnbuf);
//	qnb=(u64 *)nbuf;
	terr=0;

	trng=0x5555;
	if(ctx)
	{
		trng=ctx->curgen;
		trng=(trng<<1)^(trng>>17);
		trng=(trng<<1)^(trng>>17);
		trng=(trng<<1)^(trng>>17);
	}

	gann_bxid=8;
	
	sttab=gann_nnbuf;
	ix=trng&31;
	l=0;
	
	ev=0;
	for(i=0; i<64; i++)
	{
		ix=trng&31;
		trng=(trng<<1)^(trng>>17);

		for(j=24+((trng>>8)&31); j<modtab[i&15]; j++)
		{
			k=sttab[(ix<<1)|l];
			p=k&1;
			ix=(k>>1)&31;
			l=(pattab[i&15]>>j)&1;
		}

		for(j=0; j<modtab[i&15]; j++)
		{
			k=sttab[(ix<<1)|l];
			p=k&1;
			ix=(k>>1)&31;
			l=(pattab[i&15]>>j)&1;
			
			if(p!=l)
				ev++;
		}
	}
	return(ev);
}


byte *GANN_LoadFile(char *name, int *rsz)
{
	byte *buf;
	FILE *fd;
	int sz, i;
	
	fd=fopen(name, "rb");
	if(!fd)
	{
		return(NULL);
	}
	fseek(fd, 0, 2);
	sz=ftell(fd);
	fseek(fd, 0, 0);
	buf=malloc(sz+24);
	i=fread(buf, 1, sz, fd);
	fclose(fd);
	
	if(i!=sz)
	{
		if(i>0)
		{
			sz=i;
		}else
		{
			free(buf);
			return(NULL);
		}
	}

	memset(buf+sz, 0, 16);
	
	*rsz=sz;
	return(buf);
}

int GANN_StoreFile(char *name, void *buf, int sz)
{
	FILE *fd;
	
	fd=fopen(name, "wb");
	if(!fd)
		return(-1);
	
	fwrite(buf, 1, sz, fd);
	fclose(fd);
	return(0);
}

int gann_log2u(int val)
{
	int v, e;
	
	v=val; e=0;
	while(v>1)
		{ v=(v+1)>>1; e++; }
	return(e);
}

int gann_log2u_f8(int val)
{
	int v, e;
	
	if(val<8)
		return(val);
	
	v=val; e=1;
	while(v>=16)
		{ v=(v+1)>>1; e++; }
	v=(v&7)|(e<<3);
	return(v);
}

int gann_exp2u_f8(int val)
{
	int v, e;
	
	if(val<8)
		return(val);
	v=8|(val&7); e=(val>>3)&31;
	v=v<<(e-1);
	return(v);
}

int main(int argc, char *argv[])
{
	GANN_Context *ctx;
	GANN_Member *vm;
	u64 mbits[64];
	char tbuf[256];
	char tb1[256];
	byte *ibuf;
	int szme, is16b, ngen, szm1, bxid, szgen, szkeep;
	int i, j, k, l;

	GANN_InitMaj();
	GANN_InitFp8Tab();

	for(i=0; i<64; i++)
	{
		gann_charset_dec[i]=' '+i;
	}
	gann_charset_dec[0x3C]='~';

	for(i=0; i<256; i++)
	{
		k=i-' ';
		if(k<0)
		{
			k=' ';
			if(i=='\n')
				k=0x20;
		}
		if(k>=64)
			k-=32;
		if(i=='`')	k=0x3C;
		if(i=='~')	k=0x3C;
		gann_charset_enc[i]=k;
	}
	
	vm=malloc(sizeof(GANN_Member));
	memset(vm, 0, sizeof(GANN_Member));

#ifdef GANN_TEST_STATETABLE
	GANN_Test_TestStateTable0(NULL, vm);
#else
	k=GANN_Test_RunMember0(NULL, vm, mbits);
#endif
	bxid=gann_bxid;
	

	j=GANN_AddFp16J(0x3FAB, 0x4CDF);
	k=GANN_AddFp16I(0x3FAB, 0x4CDF);
	if(j!=k)
		{ __debugbreak(); }

#ifndef GANN_TEST_STATETABLE
//	test0=(char *)GANN_LoadFile("2021-09-09_Skimmer1B.txt", &k);
//	test0=(char *)GANN_LoadFile("2023-02-14_ShellbugHardMod.txt", &k);
	test0=(char *)GANN_LoadFile("allstories0.txt", &k);
#endif
	
	szme=256;
	while(szme<(bxid*8))
		szme=szme<<1;
	
	j=gann_log2u_f8(bxid*8);
	szme=gann_exp2u_f8(j);
	
	if(szme<(bxid*8))
		{ __debugbreak(); }
	
	is16b=GANN_MODE_MAJ7;
//	is16b=GANN_MODE_MAJ3;
//	is16b=GANN_MODE_HAMM;
//	szme=1<<14;
//	szme=1<<22;
//	szme=1<<19;
	ngen=(1<<16);
//	ngen=(1<<12);
//	ngen=(1<<10);
//	szgen=1<<8;		szkeep=1<<4;
//	szgen=1<<7;		szkeep=1<<3;
//	szgen=1<<6;		szkeep=1<<3;
//	szgen=1<<5;		szkeep=1<<3;
	szgen=1<<4;		szkeep=1<<2;

#ifdef GANN_TEST_STATETABLE
//	szgen=1<<8;		szkeep=1<<4;
	szgen=1<<6;		szkeep=1<<3;
#endif

//	ctx=GANN_AllocContext(256, szme, is16b);
	ctx=GANN_AllocContext(szgen, szme, is16b);

#ifdef GANN_TEST_STATETABLE
	ctx->TestMember=GANN_Test_TestStateTable0;
#else
	ctx->TestMember=GANN_Test_TestMember0;
#endif

//	k=GANN_Test_RunMember0(ctx, ctx->marr[0], mbits);
//	bxid=gann_bxid;

	printf("bxid=%d\n", bxid);

#if 0
	szm1=szme>>1;
	for(i=0; i<szgen; i++)
	{
		if(is16b&2)
		{
			for(j=0; j<(szm1-4); j+=4)
			{
//				ctx->marr[i]->vec[j+3]&=0xFF5FFFF555FFFULL;
			}
		}else
			if(is16b&1)
		{
			for(j=0; j<(szm1-4); j+=4)
			{
				ctx->marr[i]->vec[j+3]&=0xFF5FFFF555FFFULL;
			}
		}else
		{
			for(j=0; j<(szme-8); j+=8)
			{
//				ctx->marr[i]->vec[j+6]&=0x55555555FFFFFFFFULL;
//				ctx->marr[i]->vec[j+7]&=0xFF5555FFFFFFFFFFULL;

				ctx->marr[i]->vec[j+6]&=0xFF5555555FFFFFFFULL;
				ctx->marr[i]->vec[j+7]&=0xFFFF555FFFFFFFFFULL;
			}
		}
	}
#endif

	sprintf(tb1, "gann_%u", bxid);
	mkdir(tb1, 0777);
	
	for(i=0; i<8; i++)
	{
		sprintf(tb1, "gann_%u/gann_%u_a%u.dat", bxid, bxid, i);
		ibuf=GANN_LoadFile(tb1, &k);
		if(ibuf)
		{
			sprintf(tb1, "gann_%u/gann_%u_b%u.dat", bxid, bxid, i);
			GANN_StoreFile(tb1, ibuf, szme);
			free(ibuf);
		}

		sprintf(tb1, "gann_%u/gann_%u_%u.dat", bxid, bxid, i);
		ibuf=GANN_LoadFile(tb1, &k);
		if(ibuf)
		{
			sprintf(tb1, "gann_%u/gann_%u_a%u.dat", bxid, bxid, i);
			GANN_StoreFile(tb1, ibuf, szme);

			GANN_SetMemberAsBytes(ctx->marr[i+ 0], ibuf);
			GANN_SetMemberAsBytes(ctx->marr[i+ 8], ibuf);
			if(szgen>=32)
			{
				GANN_SetMemberAsBytes(ctx->marr[i+16], ibuf);
				GANN_SetMemberAsBytes(ctx->marr[i+24], ibuf);
			}
			free(ibuf);
		}
	}

	for(i=0; i<ngen; i++)
	{
		if((i&7)==7)
		{
			tb1[0]=0;
		
#ifndef GANN_TEST_STATETABLE
//			strncpy(tb1, test0, 16);
			for(j=0; j<16; j++)
			{
				k=test0[500+j];
				k=gann_charset_enc[k];
				tb1[j]=gann_charset_dec[k];
			}
			tb1[j]=0;
#endif

			printf(
				"mstat: X:%02X S:%02X C:%02X ID:%02X"
				"  D:%02X I:%02X %02X %02X  %s\n",
				gann_log2u_f8(ctx->mstat_sk[0]),
				gann_log2u_f8(ctx->mstat_sk[1]),
				gann_log2u_f8(ctx->mstat_sk[2]),
				gann_log2u_f8(ctx->mstat_sk[3]),
				gann_log2u_f8(ctx->mstat_sk[4]),
				gann_log2u_f8(ctx->mstat_sk[5]),
				gann_log2u_f8(ctx->mstat_sk[6]),
				gann_log2u_f8(ctx->mstat_sk[7]),
				tb1);

			ctx->mstat_sk[0]=0;		ctx->mstat_sk[1]=0;
			ctx->mstat_sk[2]=0;		ctx->mstat_sk[3]=0;
			ctx->mstat_sk[4]=0;		ctx->mstat_sk[5]=0;
			ctx->mstat_sk[6]=0;		ctx->mstat_sk[7]=0;
		}
	
		printf("%d/%d a:%d b:%d c:%d d:%d mrm:%02X/%02X  %s  \n", i, ngen,
			ctx->earr[0], ctx->earr[1],
			ctx->earr[2], ctx->earr[3],
			ctx->marr[0]->mrmb,
			ctx->marr[0]->mrmc,
			tbuf);
		fflush(stdout);

		ctx->curgen=i;
		GANN_TestMembers(ctx);
		GANN_SortMembers(ctx);
//		GANN_BreedMembers(ctx, 16);
		GANN_BreedMembers(ctx, szkeep);

		tbuf[0]=0;

#ifndef GANN_TEST_STATETABLE
		for(j=0; j<64; j++)
		{
//			mbits[j]=0;

			k=test0[500-j];
			mbits[j]=1LL<<gann_charset_enc[k];
		}
//		for(j=0; j<24; j++)
		for(j=0; j<16; j++)
		{
//			mbits[16]=GANN_Test_RunMember0(ctx, ctx->marr[0], mbits);
			k=GANN_Test_RunMember0(ctx, ctx->marr[0], mbits);
			for(l=63; l>0; l--)
				{ mbits[l]=mbits[l-1]; }
			mbits[0]=1LL<<k;
//			tbuf[j]=(' '+k);
			tbuf[j]=gann_charset_dec[k];
		}
		tbuf[j]=0;

		if(i>=32)
		{
			for(j=0; j<8; j++)
			{
				GANN_Test_CacheMember(ctx, ctx->marr[j]);
				sprintf(tb1, "gann_%u/gann_%u_%u.dat", bxid, bxid, j);
				GANN_StoreFile(tb1, gann_nnbuf, szme);
			}
		}
#endif
	}
	
	printf("\n");
	
#ifndef GANN_TEST_STATETABLE
	for(i=0; i<64; i++)
		mbits[i]=0;
	for(i=0; i<256; i++)
	{
//		mbits[16]=GANN_Test_RunMember0(ctx, ctx->marr[0], mbits);
		k=GANN_Test_RunMember0(ctx, ctx->marr[0], mbits);

		for(j=63; j>0; j--)
			{ mbits[j]=mbits[j-1]; }
		mbits[0]=1LL<<k;
		fputc(' '+k, stdout);
	}
	fputc('\n', stdout);
	fflush(stdout);
#endif

#ifdef GANN_TEST_STATETABLE
	for(i=0; i<64; i++)
	{
		printf("  6'h%02X: st1=6'h%02X;\n", i, gann_nnbuf[i]&63);
	}
#endif
}
