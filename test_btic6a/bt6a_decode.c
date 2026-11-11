#define		BTPIC_TWOCC(a, b)			((a)|((b)<<8))
#define		BTPIC_FOURCC(a, b, c, d)	((a)|((b)<<8)|((c)<<16)|((d)<<24))

#define		BTPIC_TCC_IX	BTPIC_TWOCC('I', 'X')
#define		BTPIC_TCC_PX	BTPIC_TWOCC('P', 'X')
#define		BTPIC_TCC_IZ	BTPIC_TWOCC('I', 'Z')
#define		BTPIC_TCC_PZ	BTPIC_TWOCC('P', 'Z')
#define		BTPIC_TCC_HX	BTPIC_TWOCC('H', 'X')
#define		BTPIC_TCC_PT	BTPIC_TWOCC('P', 'T')

#define		BTPIC_TCC_Z3	BTPIC_TWOCC('Z', '3')
#define		BTPIC_TCC_Z4	BTPIC_TWOCC('Z', '4')
#define		BTPIC_TCC_Z5	BTPIC_TWOCC('Z', '5')
#define		BTPIC_TCC_Z6	BTPIC_TWOCC('Z', '6')
#define		BTPIC_TCC_Z7	BTPIC_TWOCC('Z', '7')

#ifndef BTPIC_QFL_PFRAME
// #define BTPIC_QFL_PFRAME	0x100
#define BTPIC_QFL_PFRAME	0x200
#endif

typedef	uint8_t		byte;
typedef	uint16_t	u16;
typedef	uint32_t	u32;
typedef	uint64_t	u64;

typedef	int8_t		sbyte;
typedef	int16_t		s16;
typedef	int32_t		s32;
typedef	int64_t		s64;

#ifdef _M_IX86
#define		unaligned
#endif

#ifndef unaligned
#define		unaligned	__unaligned
#endif

#ifndef btpic_getu16
#define		btpic_getu16(p)		(*(unaligned u16 *)(p))
#define		btpic_getu32(p)		(*(unaligned u32 *)(p))
#define		btpic_getu64(p)		(*(unaligned u64 *)(p))

#define		btpic_setu16(p, v)	((*(unaligned u16 *)(p))=(v))
#define		btpic_setu32(p, v)	((*(unaligned u32 *)(p))=(v))
#define		btpic_setu64(p, v)	((*(unaligned u64 *)(p))=(v))
#endif


typedef struct BTIC6A_DecodeContext_s BTIC6A_DecodeContext;

struct BTIC6A_DecodeContext_s {
	byte *cs;			//current position
	byte *lcs;			//lst position

//	byte cb_2x2_y[BTIC6A_CBSZ_2X*4];
//	byte cb_2x2_c[BTIC6A_CBSZ_2X*2];
//	u16 cb_4x4   [BTIC6A_CBSZ_4X*4];
//	u16 cb_8x8   [BTIC6A_CBSZ_8X*4];
//	u16 cb_16x16 [BTIC6A_CBSZ_16X*4];

	byte *cb_2x2_y;
	byte *cb_2x2_c;
	u16 *cb_4x4;
	u16 *cb_8x8;
	u16 *cb_16x16;

	u16 rov_2x2;
	u16 rov_4x4;
	u16 rov_8x8;
	u16 rov_16x16;

	u16 msk_2x2;
	u16 msk_4x4;
	u16 msk_8x8;
	u16 msk_16x16;

	u16 stf_2x2  [128];
	u16 stf_4x4  [128];
	u16 stf_8x8  [128];
	u16 stf_16x16[128];
	byte strov_2x2;
	byte strov_4x4;
	byte strov_8x8;
	byte strov_16x16;

	byte acc_zero_l3;
	byte acc_zero_l2;
	byte acc_zero_l1;
	byte acc_zero_l0;

	byte acc_dcom_l3;
	byte acc_dcom_l2;
	byte acc_dcom_l1;
	byte acc_dcom_l0;

	byte acc_skip_l2;
	byte acc_skip_l1;
	byte acc_skip_l0;


	byte *tfzbuf;
	byte *tfz2buf;
	int tfzsz;

	byte *cimg2x;
	byte *cimg;
	byte *pimg;
	int xs, ys;
	int xsr, ysr;
	int xsc, ysc;
	int mv_x, mv_y;
	int c_x, c_y;
	
	int clr_y;
	int clr_u;
	int clr_v;
};

/*
 * 0:
 *   0 3
 *   1 2
 * 1:
 *   2 3
 *   1 0
 * 2:
 *   2 1
 *   3 0
 * 3:
 *   0 1
 *   3 2

 */

static const byte bt6a_rotab[4*4]=
	{0, 2, 2, 0,   3, 3, 1, 1,   1, 1, 3, 3,   2, 0, 0, 2};
static const byte bt6a_rstab[4*4]=
	{3, 1, 2, 0,   2, 0, 2, 3,   0, 1, 3, 2,   0, 2, 1, 3};
static const byte bt6a_rttab[4*4]=
	{3, 2, 1, 0,   0, 1, 2, 3,   0, 1, 2, 3,   1, 0, 3, 2};

int btic6a_clamp255(int v)
{
	if(v<0)		return(0);
	if(v>255)	return(255);
	return(v);
}

BTIC6A_DecodeContext *BTIC6A_AllocDecodeContext()
{
	BTIC6A_DecodeContext *ctx;
	ctx=malloc(sizeof(BTIC6A_DecodeContext));
	memset(ctx, 0, sizeof(BTIC6A_DecodeContext));
	return(ctx);
}

int BTIC6A_DecodeCopyMacroblockSkip(BTIC6A_DecodeContext *ctx)
{
	byte *cimg, *pimg;
	u32 *cs, *ct;
	int mx, my, bx, by, bz, px, py, pz, xs, ys;
	int i, j, k;

	xs=ctx->xs;		ys=ctx->ys;
	bx=ctx->c_x*16;	by=ctx->c_y*16;
	mx=ctx->mv_x;	my=ctx->mv_y;
	px=bx+mx;		py=by+my;
	cimg=ctx->cimg;	pimg=ctx->pimg;
	bz=by*xs+bx;
	pz=py*xs+px;
	ct=((u32 *)cimg)+bz;
	cs=((u32 *)pimg)+pz;

	for(i=0; i<4; i++)
	{
		memcpy(ct, cs, 16*4);
		ct+=xs;	cs+=xs;
		memcpy(ct, cs, 16*4);
		ct+=xs;	cs+=xs;
		memcpy(ct, cs, 16*4);
		ct+=xs;	cs+=xs;
		memcpy(ct, cs, 16*4);
		ct+=xs;	cs+=xs;
	}
	return(0);
}

int BTIC6A_DecodeTransColorRGB24(BTIC6A_DecodeContext *ctx)
{
	int cr, cg, cb, cy, cu, cv, px;

	cy=ctx->clr_y;
	cu=ctx->clr_u;
	cv=ctx->clr_v;
	
	cb=((cu-128)*2)+cy;
	cr=((cv-128)*2)+cy;
	cg=(8*cy-3*cr-cb)>>2;
	
	if((cr|cg|cb)&(~255))
	{
		cr=btic6a_clamp255(cr);
		cg=btic6a_clamp255(cg);
		cb=btic6a_clamp255(cb);
	}
	
	px=(cr<<16)|(cg<<8)|cb|0xFF000000;
	return(px);
}

void BTIC6A_DecodeMacroblockPixelsZero(BTIC6A_DecodeContext *ctx)
{
	byte *cimg;
	u32 *ct;
	u32 px;
	int bx, by, bz, xs, ys;
	int x, y;

	px=BTIC6A_DecodeTransColorRGB24(ctx);

	xs=ctx->xs;		ys=ctx->ys;
	bx=ctx->c_x*16;	by=ctx->c_y*16;
	cimg=ctx->cimg;
	
	bz=(by*xs+bx)*4;
	ct=(u32 *)(cimg+bz);
	for(y=0; y<16; y++)
	{
		ct[ 0]=px; ct[ 1]=px; ct[ 2]=px; ct[ 3]=px;
		ct[ 4]=px; ct[ 5]=px; ct[ 6]=px; ct[ 7]=px;
		ct[ 8]=px; ct[ 9]=px; ct[10]=px; ct[11]=px;
		ct[12]=px; ct[13]=px; ct[14]=px; ct[15]=px;
		ct+=xs;
	}
}

void BTIC6A_DecodeBlock3PixelsIndex(BTIC6A_DecodeContext *ctx,
	byte *cibase, int ystride, int ixi, int rot)
{
	byte *cict[4];
	byte *ctb;
	u32 *ct;
	u32 px;
	byte *cs_y, *cs_c;
	int cy0, cy1, cy2, cy3, cu, cv, cu1, cv1;
	int cr0, cr1, cr2, cr3;
	int cg0, cg1, cg2, cg3;
	int cb0, cb1, cb2, cb3;
	int msk;
	int i, str1, pz;

	if(ixi==0xFFFF)
	{
		px=BTIC6A_DecodeTransColorRGB24(ctx);
		ct=(u32 *)cibase;
		str1=ystride>>2;
		ct[0]=px; ct[1]=px; ct+=str1;
		ct[0]=px; ct[1]=px;
		return;
	}

	msk=ctx->msk_2x2;
	cs_y=ctx->cb_2x2_y+((ixi&msk)*4);
	cs_c=ctx->cb_2x2_c+((ixi&msk)*2);
//	yst4=ystride;
//	xst4=4;
	
	cy0=cs_y[0];
	cy1=cs_y[1];
	cy2=cs_y[2];
	cy3=cs_y[3];
	cu=cs_c[0];
	cv=cs_c[1];
	
	if(!(ixi&0x4000))
	{
		cy0=((ctx->clr_y)+cy0)&255;
		cy1=(cy0+cy1)&255;
		cy2=(cy1+cy2)&255;
		cy3=(cy2+cy3)&255;
		cu=((ctx->clr_u)+cu)&255;
		cv=((ctx->clr_v)+cv)&255;
	}
	ctx->clr_y=cy3;
	ctx->clr_u=cu;
	ctx->clr_v=cv;
	
	cu1=((cu-128)*2);
	cv1=((cv-128)*2);
	cb0=cy0+cu1;	cr0=cy0+cv1;	cb1=cy1+cu1;	cr1=cy1+cv1;
	cb2=cy2+cu1;	cr2=cy2+cv1;	cb3=cy3+cu1;	cr3=cy3+cv1;
	cg0=(8*cy0-cr0*3-cb0)>>2;		cg1=(8*cy1-cr1*3-cb1)>>2;
	cg2=(8*cy2-cr2*3-cb2)>>2;		cg3=(8*cy3-cr3*3-cb3)>>2;
	
	if((cr0|cr1|cr2|cr3)&(~255))
	{	cr0=btic6a_clamp255(cr0);	cr1=btic6a_clamp255(cr1);
		cr2=btic6a_clamp255(cr2);	cr3=btic6a_clamp255(cr3);	}
	if((cg0|cg1|cg2|cg3)&(~255))
	{	cg0=btic6a_clamp255(cg0);	cg1=btic6a_clamp255(cg1);
		cg2=btic6a_clamp255(cg2);	cg3=btic6a_clamp255(cg3);	}
	if((cb0|cb1|cb2|cb3)&(~255))
	{	cb0=btic6a_clamp255(cb0);	cb1=btic6a_clamp255(cb1);
		cb2=btic6a_clamp255(cb2);	cb3=btic6a_clamp255(cb3);	}
	
	ctb=cibase+ystride;
	cict[bt6a_rotab[rot+ 0]]=cibase+0;
	cict[bt6a_rotab[rot+ 4]]=cibase+4;
	cict[bt6a_rotab[rot+ 8]]=ctb;
	cict[bt6a_rotab[rot+12]]=ctb+4;
	
	ctb=cict[0];
	ctb[0]=cb0; ctb[1]=cg0; ctb[2]=cr0; ctb[3]=255;
	ctb=cict[1];
	ctb[0]=cb1; ctb[1]=cg1; ctb[2]=cr1; ctb[3]=255;
	ctb=cict[2];
	ctb[0]=cb2; ctb[1]=cg2; ctb[2]=cr2; ctb[3]=255;
	ctb=cict[3];
	ctb[0]=cb3; ctb[1]=cg3; ctb[2]=cr3; ctb[3]=255;
}

void BTIC6A_DecodeBlock2PixelsIndex(BTIC6A_DecodeContext *ctx,
	byte *cibase, int ystride, int ixi, int rot)
{
	byte *cict[4];
	byte *ctb;
	u32 *ct;
	u32 px;
	u16 *csi;
	int i, str1, xst4, yst4;

	if(ixi==0xFFFF)
	{
		px=BTIC6A_DecodeTransColorRGB24(ctx);
		ct=(u32 *)cibase;
		str1=ystride>>2;
		ct[0]=px; ct[1]=px; ct[2]=px; ct[3]=px; ct+=str1;
		ct[0]=px; ct[1]=px; ct[2]=px; ct[3]=px; ct+=str1;
		ct[0]=px; ct[1]=px; ct[2]=px; ct[3]=px; ct+=str1;
		ct[0]=px; ct[1]=px; ct[2]=px; ct[3]=px;
		return;
	}

	csi=ctx->cb_4x4+((ixi&(ctx->msk_4x4))*4);
	yst4=ystride*2;
	xst4=2*4;

	ctb=cibase+yst4;
	cict[bt6a_rotab[rot+ 0]]=cibase+0;
	cict[bt6a_rotab[rot+ 4]]=cibase+xst4;
	cict[bt6a_rotab[rot+ 8]]=ctb;
	cict[bt6a_rotab[rot+12]]=ctb+xst4;
	
	BTIC6A_DecodeBlock3PixelsIndex(ctx,
		cict[0], ystride, csi[0], bt6a_rttab[rot+ 0]);
	BTIC6A_DecodeBlock3PixelsIndex(ctx,
		cict[1], ystride, csi[1], bt6a_rttab[rot+ 4]);
	BTIC6A_DecodeBlock3PixelsIndex(ctx,
		cict[2], ystride, csi[2], bt6a_rttab[rot+ 8]);
	BTIC6A_DecodeBlock3PixelsIndex(ctx,
		cict[3], ystride, csi[3], bt6a_rttab[rot+12]);
}

void BTIC6A_DecodeBlock1PixelsIndex(BTIC6A_DecodeContext *ctx,
	byte *cibase, int ystride, int ixi, int rot)
{
	byte *cict[4];
	byte *ctb;
	u32 *ct;
	u32 px;
	u16 *csi;
	int i, str1, xst4, yst4;

	if(ixi==0xFFFF)
	{
		px=BTIC6A_DecodeTransColorRGB24(ctx);
		ct=(u32 *)cibase;
		str1=ystride>>2;

		for(i=0; i<8; i++)
		{
			ct[0]=px; ct[1]=px; ct[2]=px; ct[3]=px;
			ct[4]=px; ct[5]=px; ct[6]=px; ct[7]=px;
			ct+=str1;
		}
		return;
	}

	csi=ctx->cb_8x8+((ixi&(ctx->msk_8x8))*4);
	yst4=ystride*4;
	xst4=4*4;
	
	ctb=cibase+yst4;
	cict[bt6a_rotab[rot+ 0]]=cibase+0;
	cict[bt6a_rotab[rot+ 4]]=cibase+xst4;
	cict[bt6a_rotab[rot+ 8]]=ctb;
	cict[bt6a_rotab[rot+12]]=ctb+xst4;
	
	BTIC6A_DecodeBlock2PixelsIndex(ctx,
		cict[0], ystride, csi[0], bt6a_rttab[rot+ 0]);
	BTIC6A_DecodeBlock2PixelsIndex(ctx,
		cict[1], ystride, csi[1], bt6a_rttab[rot+ 4]);
	BTIC6A_DecodeBlock2PixelsIndex(ctx,
		cict[2], ystride, csi[2], bt6a_rttab[rot+ 8]);
	BTIC6A_DecodeBlock2PixelsIndex(ctx,
		cict[3], ystride, csi[3], bt6a_rttab[rot+12]);
}

void BTIC6A_DecodeBlock0PixelsIndex(BTIC6A_DecodeContext *ctx,
	byte *cibase, int ystride, int ixi, int rot)
{
	byte *cict[4];
	byte *ctb;
	u32 *ct;
	u32 px;
	u16 *csi;
	int i, str1, xst4, yst4;

	if(ixi==0xFFFF)
	{
		px=BTIC6A_DecodeTransColorRGB24(ctx);
		ct=(u32 *)cibase;
		str1=ystride>>2;

		for(i=0; i<16; i++)
		{
			ct[ 0]=px; ct[ 1]=px; ct[ 2]=px; ct[ 3]=px;
			ct[ 4]=px; ct[ 5]=px; ct[ 6]=px; ct[ 7]=px;
			ct[ 8]=px; ct[ 9]=px; ct[10]=px; ct[11]=px;
			ct[12]=px; ct[13]=px; ct[14]=px; ct[15]=px;
			ct+=str1;
		}
		return;
	}

	csi=ctx->cb_16x16+((ixi&(ctx->msk_16x16))*4);
	yst4=ystride*8;
	xst4=8*4;
	
	ctb=cibase+yst4;
	cict[bt6a_rotab[rot+ 0]]=cibase+0;
	cict[bt6a_rotab[rot+ 4]]=cibase+xst4;
	cict[bt6a_rotab[rot+ 8]]=ctb;
	cict[bt6a_rotab[rot+12]]=ctb+xst4;
	
	BTIC6A_DecodeBlock1PixelsIndex(ctx,
		cict[0], ystride, csi[0], bt6a_rttab[rot+ 0]);
	BTIC6A_DecodeBlock1PixelsIndex(ctx,
		cict[1], ystride, csi[1], bt6a_rttab[rot+ 4]);
	BTIC6A_DecodeBlock1PixelsIndex(ctx,
		cict[2], ystride, csi[2], bt6a_rttab[rot+ 8]);
	BTIC6A_DecodeBlock1PixelsIndex(ctx,
		cict[3], ystride, csi[3], bt6a_rttab[rot+12]);
}

void BTIC6A_DecodeMacroblockPixelsIndex(BTIC6A_DecodeContext *ctx, int ixi)
{
	byte ctpix[16*16*4];
	byte *cict[4];
	byte *ctb;
	byte *cimg, *cibase;
	u32 *ctw, *csw;
	u16 *csi;
	int cr, cg, cb, cy, cu, cv;
	int ystride, yst8, xst8;
	int mx, my, bx, by, bz, px, py, pz, xs, ys;
	int x, y, xf, yf;
	
	if(ixi==0xFFFF)
	{
		BTIC6A_DecodeMacroblockPixelsZero(ctx);
		return;
	}

	xs=ctx->xs;		ys=ctx->ys;
	bx=ctx->c_x*16;	by=ctx->c_y*16;
	cimg=ctx->cimg;
	ystride=xs*4;
	
	cibase=cimg+(by*xs+bx)*4;
	BTIC6A_DecodeBlock0PixelsIndex(ctx, cibase, ystride, ixi, 0);
}

int BTIC6A_DecodeMacroblockL3(BTIC6A_DecodeContext *ctx, byte **rcs)
{
	byte *cs, *ct_y, *ct_c, *cs_y, *cs_c;
	u64 tg;
	int n, ix0, ix1, ix2, ix3;
	int i, j, k;
	
	if(ctx->acc_zero_l3)
	{
		ctx->acc_zero_l3--;
		return(0xFFFF);
	}
	
	cs=*rcs;
	if(ctx->acc_dcom_l3)
	{
		ix2=ctx->rov_2x2;
		ctx->rov_2x2=(ctx->rov_2x2+1)&0x3FFF;
		j=ix2&(ctx->msk_2x2);
		ct_y=ctx->cb_2x2_y+(j*4);
		ct_c=ctx->cb_2x2_c+(j*2);

		k=ctx->acc_dcom_l3;
//		ctx->acc_dcom_l3=(k>=4)?(k-4):0;
		ctx->acc_dcom_l3=(k>=8)?(k-4):0;

		if((k&3)==0)
		{
			ct_y[0]=cs[0];
			ct_y[1]=cs[1];
			ct_y[2]=cs[2];
			ct_y[3]=cs[3];
			ct_c[0]=cs[4];
			ct_c[1]=cs[5];
			cs+=6;
		}else
			if((k&3)==1)
		{
			ct_y[0]=((cs[0]&15)-8)<<2;
			ct_y[1]=((cs[0]>>4)-8)<<2;
			ct_y[2]=((cs[1]&15)-8)<<2;
			ct_y[3]=((cs[1]>>4)-8)<<2;
			ct_c[0]=((cs[2]&15)-8)<<2;
			ct_c[1]=((cs[2]>>4)-8)<<2;
			cs+=3;
		}else
			if((k&3)==2)
		{
#if 0
			ix2|=0x4000;
			ct_y[0]=cs[0];
			ct_y[1]=cs[1];
			ct_y[2]=cs[2];
			ct_y[3]=cs[3];
			ct_c[0]=cs[4];
			ct_c[1]=cs[5];
			cs+=6;
#endif

#if 1
			tg=btpic_getu32(cs);
			j=tg&3;
			ct_y[0]=(((tg>> 2)&31)-16)<<j;
			ct_y[1]=(((tg>> 7)&31)-16)<<j;
			ct_y[2]=(((tg>>12)&31)-16)<<j;
			ct_y[3]=(((tg>>17)&31)-16)<<j;
			ct_c[0]=(((tg>>22)&31)-16)<<j;
			ct_c[1]=(((tg>>27)&31)-16)<<j;
			cs+=4;
#endif
		}else
		{
#if 1
			ct_y[0]=((cs[0]&15)-8)<<3;
			ct_y[1]=((cs[0]>>4)-8)<<3;
			ct_y[2]=((cs[1]&15)-8)<<3;
			ct_y[3]=((cs[1]>>4)-8)<<3;
			ct_c[0]=((cs[2]&15)-8)<<3;
			ct_c[1]=((cs[2]>>4)-8)<<3;
			cs+=3;
#endif
		}
		
		*rcs=cs;
		return(ix2);
	}
	
	tg=btpic_getu64(cs);

	if(!(tg&0x01))
	{
		cs++;

		ix0=((tg>>1)&127);
//		ix1=(ix0*15)>>4;
		ix1=(ix0*7)>>3;
		ix0=(ix0+ctx->strov_2x2)&127;
		ix1=(ix1+ctx->strov_2x2)&127;
		ix2=ctx->stf_2x2[ix0];	ix3=ctx->stf_2x2[ix1];
		ctx->stf_2x2[ix0]=ix3;	ctx->stf_2x2[ix1]=ix2;
		*rcs=cs;
		return(ix2);
	}else
		if(!(tg&0x02))
	{
		if(!(tg&0x0C))
		{
			cs++;
			ctx->acc_zero_l3=(tg>>4)&15;
			*rcs=cs;
			return(0xFFFF);
		}else
			if(tg&0x04)
		{
			cs+=5;

			ix2=ctx->rov_2x2;
			ctx->rov_2x2=(ctx->rov_2x2+1)&0x3FFF;
			j=ix2&(ctx->msk_2x2);
			ct_y=ctx->cb_2x2_y+(j*4);
			ct_c=ctx->cb_2x2_c+(j*2);

			j=(tg&8)?2:1;
			ct_y[0]=(((tg>> 4)&63)-32)<<j;
			ct_y[1]=(((tg>>10)&63)-32)<<j;
			ct_y[2]=(((tg>>16)&63)-32)<<j;
			ct_y[3]=(((tg>>22)&63)-32)<<j;
			ct_c[0]=(((tg>>28)&63)-32)<<j;
			ct_c[1]=(((tg>>34)&63)-32)<<j;

			*rcs=cs;
			return(ix2);
		}else
		{
//			__debugbreak();

			cs+=2;
			ix2=(ctx->rov_2x2-(((tg>>4)&4095)+1))&0x3FFF;

			j=ix2&(ctx->msk_2x2);
			cs_y=ctx->cb_2x2_y+(j*4);
			cs_c=ctx->cb_2x2_c+(j*2);

			ix2=ctx->rov_2x2;
			ctx->rov_2x2=(ctx->rov_2x2+1)&0x3FFF;
			j=ix2&(ctx->msk_2x2);
			ct_y=ctx->cb_2x2_y+(j*4);
			ct_c=ctx->cb_2x2_c+(j*2);

			memcpy(ct_y, cs_y, 4);
			memcpy(ct_c, cs_c, 2);

			ctx->strov_2x2--;
			ctx->stf_2x2[ctx->strov_2x2&127]=ix2;

			*rcs=cs;
			return(ix2);
		}
	}else	
		if(!(tg&0x04))
	{
//		if((tg&0x0F)==0x03)
		if(!(tg&0x08))
		{
			cs+=2;
			ix2=(ctx->rov_2x2-(((tg>>4)&4095)+1))&0x3FFF;
			ctx->strov_2x2--;
			ctx->stf_2x2[ctx->strov_2x2&127]=ix2;

			*rcs=cs;
			return(ix2);
		}else
		{
			cs+=5;

			ix2=ctx->rov_2x2;
			ctx->rov_2x2=(ctx->rov_2x2+1)&0x3FFF;
			j=ix2&(ctx->msk_2x2);
			ct_y=ctx->cb_2x2_y+(j*4);
			ct_c=ctx->cb_2x2_c+(j*2);

			ct_y[0]=((tg>> 4)&63)-32;
			ct_y[1]=((tg>>10)&63)-32;
			ct_y[2]=((tg>>16)&63)-32;
			ct_y[3]=((tg>>22)&63)-32;
			ct_c[0]=((tg>>28)&63)-32;
			ct_c[1]=((tg>>34)&63)-32;

			*rcs=cs;
			return(ix2);
		}
	}else
//	if((tg&0x0F)==0x07)
		if(!(tg&0x08))
	{
		cs++;
		ctx->acc_dcom_l3=((tg>>4)&15)+4;
		ix2=BTIC6A_DecodeMacroblockL3(ctx, &cs);
		*rcs=cs;
		return(ix2);
	}

	__debugbreak();
	return(0);
}

int BTIC6A_DecodeMacroblockL2(BTIC6A_DecodeContext *ctx, byte **rcs)
{
	byte *cs;
	u16 *ct_i, *cs_i;
	int tg, tg1, n, ix0, ix1, ix2, ix3, ix4;
	int i, j, k;
	
	if(ctx->acc_zero_l2)
	{
		ctx->acc_zero_l2--;
		return(0xFFFF);
	}

	if(ctx->acc_skip_l2)
	{
		ctx->acc_skip_l2--;
		return(0xFFFC);
	}

	cs=*rcs;

	if(ctx->acc_dcom_l2)
	{
		ctx->acc_dcom_l2--;
		ix4=ctx->rov_4x4;
		ctx->rov_4x4=(ix4+1)&0x3FFF;
		ct_i=ctx->cb_4x4+((ix4&(ctx->msk_4x4))*4);
		for(i=0; i<4; i++)
		{
			ix2=BTIC6A_DecodeMacroblockL3(ctx, &cs);
			ct_i[i]=ix2;
		}
		*rcs=cs;
		return(ix4);
	}

	tg=btpic_getu32(cs);
	
	if(!(tg&0x01))
	{
		cs++;
		ix0=((tg>>1)&127);
//		ix1=(ix0*15)>>4;
		ix1=(ix0*7)>>3;
		ix0=(ix0+ctx->strov_4x4)&127;
		ix1=(ix1+ctx->strov_4x4)&127;
		ix2=ctx->stf_4x4[ix0];	ix3=ctx->stf_4x4[ix1];
		ctx->stf_4x4[ix0]=ix3;	ctx->stf_4x4[ix1]=ix2;
		*rcs=cs;
		return(ix2);
	}else
		if(!(tg&0x02))
	{
		if(!(tg&0x0C))
		{
			cs++;
			ctx->acc_zero_l2=(tg>>4)&15;
			*rcs=cs;
			return(0xFFFF);
		}else
			if(!(tg&4))
		{
//			__debugbreak();

			cs+=2;
			ix2=(ctx->rov_4x4-(((tg>>4)&4095)+1))&0x3FFF;

			cs_i=ctx->cb_4x4+((ix2&(ctx->msk_4x4))*4);

			ix2=ctx->rov_4x4;
			ctx->rov_4x4=(ix2+1)&0x3FFF;
			ct_i=ctx->cb_4x4+((ix2&(ctx->msk_4x4))*4);

			ct_i[0]=cs_i[0];	ct_i[1]=cs_i[1];
			ct_i[2]=cs_i[2];	ct_i[3]=cs_i[3];

			ctx->strov_4x4--;
			ctx->stf_4x4[ctx->strov_4x4&127]=ix2;
			*rcs=cs;
			return(ix2);
		}else
		{
//			__debugbreak();
			cs++;
			ctx->acc_skip_l2=(tg>>4)&15;
			*rcs=cs;
			return(0xFFFC);
		}
	}else
		if(!(tg&0x04))
	{
		if(!(tg&0x08))
		{
			cs+=2;
			ix2=(ctx->rov_4x4-(((tg>>4)&4095)+1))&0x3FFF;
			ctx->strov_4x4--;
			ctx->stf_4x4[ctx->strov_4x4&127]=ix2;
			*rcs=cs;
			return(ix2);
		}else
		{
			__debugbreak();
		}
	}else
		if(!(tg&0x08))
	{
		cs++;
		if(!(tg&0x10))
		{
			ctx->acc_dcom_l2=((tg>>5)&7)+1;
		}else
		{
			__debugbreak();
		}
		ix4=BTIC6A_DecodeMacroblockL2(ctx, &cs);
		*rcs=cs;
		return(ix4);
	}

	__debugbreak();
	return(0);
}

int BTIC6A_DecodeMacroblockL1(BTIC6A_DecodeContext *ctx, byte **rcs)
{
	byte *cs;
	u16 *ct_i, *cs_i;
	int tg, tg1, n, ix0, ix1, ix2, ix3, ix4;
	int i, j, k;
	
	if(ctx->acc_zero_l1)
	{
		ctx->acc_zero_l1--;
		return(0xFFFF);
	}

	if(ctx->acc_skip_l1)
	{
		ctx->acc_skip_l1--;
		return(0xFFFC);
	}
	
	cs=*rcs;

	if(ctx->acc_dcom_l1)
	{
		ctx->acc_dcom_l1--;
		ix4=ctx->rov_8x8;
		ctx->rov_8x8=(ix4+1)&0x3FFF;
		ct_i=ctx->cb_8x8+((ix4&(ctx->msk_8x8))*4);
		for(i=0; i<4; i++)
			ct_i[i]=BTIC6A_DecodeMacroblockL2(ctx, &cs);
		*rcs=cs;
		return(ix4);
	}
	
	tg=btpic_getu32(cs);

	if(!(tg&0x01))
	{
		cs++;
		ix0=((tg>>1)&127);
//		ix1=(ix0*15)>>4;
		ix1=(ix0*7)>>3;
		ix0=(ix0+ctx->strov_8x8)&127;
		ix1=(ix1+ctx->strov_8x8)&127;
		ix2=ctx->stf_8x8[ix0];	ix3=ctx->stf_8x8[ix1];
		ctx->stf_8x8[ix0]=ix3;	ctx->stf_8x8[ix1]=ix2;
		*rcs=cs;
		return(ix2);
	}else
		if(!(tg&0x02))
	{
		if(!(tg&0x0C))
		{
			cs++;
			ctx->acc_zero_l1=(tg>>4)&15;
			*rcs=cs;
			return(0xFFFF);
		}else
			if(!(tg&4))
		{
			cs+=2;
			ix2=(ctx->rov_8x8-(((tg>>4)&4095)+1))&0x3FFF;

			cs_i=ctx->cb_8x8+((ix2&(ctx->msk_8x8))*4);

			ix2=ctx->rov_8x8;
			ctx->rov_8x8=(ix2+1)&0x3FFF;
			ct_i=ctx->cb_8x8+((ix2&(ctx->msk_8x8))*4);

			ct_i[0]=cs_i[0];	ct_i[1]=cs_i[1];
			ct_i[2]=cs_i[2];	ct_i[3]=cs_i[3];

			ctx->strov_8x8--;
			ctx->stf_8x8[ctx->strov_8x8&127]=ix2;
			*rcs=cs;
			return(ix2);
		}else
		{
//			__debugbreak();
			cs++;
			ctx->acc_skip_l1=(tg>>4)&15;
			*rcs=cs;
			return(0xFFFC);
		}
	}else
		if(!(tg&0x04))
	{
		if(!(tg&0x08))
		{
			cs+=2;
			ix2=(ctx->rov_8x8-(((tg>>4)&4095)+1))&0x3FFF;
			ctx->strov_8x8--;
			ctx->stf_8x8[ctx->strov_8x8&127]=ix2;
			*rcs=cs;
			return(ix2);
		}else
		{
			__debugbreak();
		}
	}else
		if(!(tg&0x08))
	{
		cs++;
		if(!(tg&0x10))
		{
			ctx->acc_dcom_l1=((tg>>5)&7)+1;
		}
		else
		{
			ctx->acc_dcom_l2=((tg>>5)&7)+1;
			ctx->acc_dcom_l1=1+(ctx->acc_dcom_l2>>2);
		}

		ix4=BTIC6A_DecodeMacroblockL1(ctx, &cs);
		*rcs=cs;
		return(ix4);
	}

	__debugbreak();
	return(0);
}

int BTIC6A_DecodeMacroblockL0(BTIC6A_DecodeContext *ctx, byte **rcs)
{
	byte *cs;
	u16 *ct_i;
	int tg, tg1, n, ix0, ix1, ix2, ix3, ix4;
	int i, j, k;
	
	if(ctx->acc_zero_l0)
	{
		ctx->acc_zero_l0--;
		return(0xFFFF);
	}

	if(ctx->acc_skip_l0)
	{
		ctx->acc_skip_l0--;
		return(0xFFFC);
	}

	cs=*rcs;

	if(ctx->acc_dcom_l0)
	{
		ctx->acc_dcom_l0--;
		ix4=ctx->rov_16x16;
		ctx->rov_16x16=(ix4+1)&0x3FFF;
		ct_i=ctx->cb_16x16+((ix4&(ctx->msk_16x16))*4);
		for(i=0; i<4; i++)
			ct_i[i]=BTIC6A_DecodeMacroblockL1(ctx, &cs);
		*rcs=cs;
		return(ix4);
	}

	tg=btpic_getu32(cs);
	
	if(!(tg&0x01))
	{
		cs++;
		ix0=((tg>>1)&127);
//		ix1=(ix0*15)>>4;
		ix1=(ix0*7)>>3;
		ix0=(ix0+ctx->strov_16x16)&127;
		ix1=(ix1+ctx->strov_16x16)&127;
		ix2=ctx->stf_16x16[ix0];	ix3=ctx->stf_16x16[ix1];
		ctx->stf_16x16[ix0]=ix3;	ctx->stf_16x16[ix1]=ix2;
		*rcs=cs;
		return(ix2);
	}else
		if(!(tg&0x02))
	{
		if(!(tg&0x0C))
		{
			cs++;
			ctx->acc_zero_l0=(tg>>4)&15;
			*rcs=cs;
			return(0xFFFF);
		}else
			if(!(tg&0x04))
		{
			__debugbreak();
		}else
		{
//			__debugbreak();
			cs++;
			ctx->acc_skip_l0=(tg>>4)&15;
			*rcs=cs;
			return(0xFFFC);
		}
	}else
		if(!(tg&0x04))
	{
		if(!(tg&0x08))
		{
			cs+=2;
			ix2=(ctx->rov_16x16-(((tg>>4)&4095)+1))&0x3FFF;
			ctx->strov_16x16--;
			ctx->stf_16x16[ctx->strov_16x16&127]=ix2;
			*rcs=cs;
			return(ix2);
		}else
		{
			__debugbreak();
		}
	}else
		if(!(tg&0x08))
	{
		cs++;
		if(!(tg&0x10))
			{ ctx->acc_dcom_l0=((tg>>5)&7)+1; }
		else
		{	ctx->acc_dcom_l1=((tg>>5)&7)+1;
			ctx->acc_dcom_l0=1+(ctx->acc_dcom_l2>>2);	}
		ix4=BTIC6A_DecodeMacroblockL0(ctx, &cs);
		*rcs=cs;
		return(ix4);
	}
	
	__debugbreak();
	return(0);
}

int BTIC6A_DecodeMacroblock(BTIC6A_DecodeContext *ctx)
{
	byte *cs;
	int tg, ix;

	if(ctx->acc_skip_l0)
	{
		ctx->acc_skip_l0--;
		BTIC6A_DecodeCopyMacroblockSkip(ctx);
		return(0);
	}

	if(ctx->acc_zero_l0)
	{
		ctx->acc_zero_l0--;
		BTIC6A_DecodeMacroblockPixelsZero(ctx);
		return(0);
	}
	
	cs=ctx->cs;
	ix=BTIC6A_DecodeMacroblockL0(ctx, &cs);
	BTIC6A_DecodeMacroblockPixelsIndex(ctx, ix);
	ctx->cs=cs;
	return(0);
}

int BTIC6A_DecodeFrameInner(BTIC6A_DecodeContext *ctx, byte *idat)
{
	int x, y, xs, ys, cxs, cys;

	ctx->cs=idat;
	xs=ctx->xs;			ys=ctx->ys;
	cxs=(xs+15)>>4;		cys=(ys+15)>>4;
	
	ctx->clr_y=0x80;
	ctx->clr_u=0x80;
	ctx->clr_v=0x80;
	ctx->mv_x=0;
	ctx->mv_y=0;

	ctx->rov_2x2=0;
	ctx->rov_4x4=0;
	ctx->rov_8x8=0;
	ctx->rov_16x16=0;

	ctx->acc_zero_l3=0;
	ctx->acc_zero_l2=0;
	ctx->acc_zero_l1=0;
	ctx->acc_zero_l0=0;
	ctx->acc_dcom_l3=0;
	ctx->acc_dcom_l2=0;
	ctx->acc_dcom_l1=0;
	ctx->acc_dcom_l0=0;
	ctx->acc_skip_l0=0;
	
	for(y=0; y<cys; y++)
	{
		for(x=0; x<cxs; x++)
		{
			ctx->c_x=x;		ctx->c_y=y;
			BTIC6A_DecodeMacroblock(ctx);
		}
	}
	return(0);
}

void BTIC6A_DecodeCopyFrameClrs(BTIC6A_DecodeContext *ctx,
	byte *oimg, int oystr, int clrs)
{
	byte *cimg;
	byte *ycs, *yct, *cs, *ct;
	int x, y, xs, ys, xsr, ysr, iystr, ystr2;
	int ocn, rgflip;
	
	ocn=4; rgflip=0;
	if(	(clrs==BTIC4B_CLRS_RGB) ||
		(clrs==BTIC4B_CLRS_BGR))
			ocn=3;
	if(	(clrs==BTIC4B_CLRS_RGB) ||
		(clrs==BTIC4B_CLRS_RGBA) ||
		(clrs==BTIC4B_CLRS_RGBX))
			rgflip=2;
	
	xs=ctx->xs;
	ys=ctx->ys;
	xsr=ctx->xsr;
	ysr=ctx->ysr;
	cimg=ctx->cimg;
	iystr=xs*4;
	
	if((ocn==4) && !rgflip)
	{
		if(oystr==iystr)
		{
			memcpy(oimg, cimg, ys*oystr);
			return;
		}
	
		ystr2=oystr;
		if(iystr<ystr2)
			ystr2=iystr;
		for(y=0; y<ysr; y++)
			{ memcpy(oimg+y*oystr, cimg+y*iystr, ystr2); }
		return;
	}
	
	ycs=cimg;
	yct=oimg;
	for(y=0; y<ysr; y++)
	{
		cs=ycs;
		ct=yct;
		for(x=0; x<xsr; x++)
		{
			ct[0]=cs[0^rgflip];
			ct[1]=cs[1];
			ct[2]=cs[2^rgflip];
			if(ocn==4)
				ct[3]=cs[3];
			cs+=4; ct+=ocn;
		}
		ycs+=iystr;
		yct+=oystr;
	}
}

int BTIC6A_DecodeFrame(BTIC6A_DecodeContext *ctx,
	byte *cibuf, int cisz,
	byte *cimg, int cystr, int clrs)
{
	byte *cs, *cs0, *cs1, *cse, *cs0e, *z3dat;
	byte *idat, *pdat;
	int ix0, ix1, ix2, ix3;
	int tg, sz, ipsz, zsz, dsz;
	int i, j, k;
	
	cs=cibuf;
	cse=cibuf+cisz;
	idat=NULL;
	pdat=NULL;
	z3dat=NULL;
	
	while(cs<cse)
	{
		tg=btpic_getu32(cs+0);
		sz=btpic_getu32(cs+4);
		if(tg&0x80808080)
		{
			tg=btpic_getu16(cs+0);
			sz=btpic_getu16(cs+2);
			if((tg&0x8080) || !(sz&0x8000))
				break;
			sz^=0xFFFF;
			if(sz<4)
				break;
			cs0=cs+4;
		}else
		{
			sz=~sz;
			if(sz<8)
				break;
			cs0=cs+8;
		}

		cs0e=cs+sz;
		cs=cs0e;

		if(tg==BTPIC_TCC_IX)
			{ idat=cs0; ipsz=cs0e-cs0; continue; }
		if(tg==BTPIC_TCC_PX)
			{ pdat=cs0; ipsz=cs0e-cs0; continue; }

		if(tg==BTPIC_TCC_Z3)
			{ z3dat=cs0; zsz=cs0e-cs0; continue; }
		
		if(tg==BTPIC_TCC_HX)
		{
			ctx->xsr=btpic_getu16(cs0+0);
			ctx->ysr=btpic_getu16(cs0+2);
			ctx->xsc=(ctx->xsr+15)>>4;
			ctx->ysc=(ctx->ysr+15)>>4;
			ctx->xs=ctx->xsc<<4;
			ctx->ys=ctx->ysc<<4;

			if(!ctx->cb_2x2_y)
			{
				k=btpic_getu16(cs0+6);
				
				ix3=1<<((k>>0)&15);
				ix2=1<<((k>>4)&15);
				ix1=1<<((k>>8)&15);
				ix0=1<<((k>>12)&15);

				ctx->cb_2x2_y=malloc(ix3*6);
				ctx->cb_2x2_c=ctx->cb_2x2_y+(ix3*4);

				j=ix2+ix1+ix0;
				ctx->cb_4x4=malloc(j*4*2);
				ctx->cb_8x8=ctx->cb_4x4+(ix2*4);
				ctx->cb_16x16=ctx->cb_8x8+(ix1*4);

				ctx->msk_2x2=ix3-1;
				ctx->msk_4x4=ix2-1;
				ctx->msk_8x8=ix1-1;
				ctx->msk_16x16=ix0-1;
			}
			
			/* current and backbuffer images, padded out */
			if(!ctx->cimg2x)
			{
				ctx->cimg2x=malloc(ctx->xs*((ctx->ys+24)*2)*4);
//				memset(ctx->cimg2x, 0x80, ctx->xs*((ctx->ys+24)*2)*4);
				memset(ctx->cimg2x, 0x00, ctx->xs*((ctx->ys+24)*2)*4);

				ctx->cimg=ctx->cimg2x+(ctx->xs*16)*4;
				ctx->pimg=ctx->cimg+(ctx->xs*(ctx->ys+16))*4;
			}
		}
	}

#ifdef RP2POST_RP2AUTO_C
	if(z3dat)
	{
		dsz=btpic_getu32(z3dat+0);
		
		if(!ctx->tfzbuf)
		{
			sz=4096;
			while(sz<=(dsz+512))
				sz=sz+(sz>>1);
			ctx->tfzsz=sz;
			ctx->tfzbuf=malloc(sz);
			ctx->tfz2buf=malloc(sz);
		}
		
		if(((z3dat[4]&0x0F)==0x00) && (z3dat[5]==0x00))
		{
			sz=PostRp2_DecodeBufferPostRp2(
				ctx->tfz2buf, z3dat+4, ctx->tfzsz, zsz);
			PostRp2Stf_DecodeRP2(ctx->tfzbuf, ctx->tfz2buf, dsz, sz);
		}else
		{
//			PostRp2_DecodeBufferRp2Full(ctx->tfzbuf, z3dat, dsz, zsz);
			PostRp2Stf_DecodeRP2(ctx->tfzbuf, z3dat, dsz, zsz);
		}
		return(BTIC6A_DecodeFrame(ctx, ctx->tfzbuf, dsz, cimg, cystr, clrs));
	}
#endif

	if(idat)
	{
		BTIC6A_DecodeFrameInner(ctx, idat);
		BTIC6A_DecodeCopyFrameClrs(ctx, cimg, cystr, clrs);
	}

	if(pdat)
	{
		cs0=ctx->cimg;	cs1=ctx->pimg;
		ctx->cimg=cs1;	ctx->pimg=cs0;
		BTIC6A_DecodeFrameInner(ctx, pdat);
		BTIC6A_DecodeCopyFrameClrs(ctx, cimg, cystr, clrs);
	}
	return(0);
}
