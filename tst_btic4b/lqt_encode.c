
// #define LQTVQ_BYTES

#ifdef LQTVQ_BYTES
void LQTVQ_EncEmitGenericUVal(BT4A_Context *ctx, int cv)
{
	if(cv<0x80)
	{
		*ctx->ct++=cv;
	}else if(cv<0x4000)
	{
		*ctx->ct++=0x80|(cv>>8);
		*ctx->ct++=cv;
	}else if(cv<0x200000)
	{
		*ctx->ct++=0xC0|(cv>>16);
		*ctx->ct++=cv>>8;
		*ctx->ct++=cv;
	}else if(cv<0x10000000)
	{
		*ctx->ct++=0xE0|(cv>>16);
		*ctx->ct++=cv>>16;
		*ctx->ct++=cv>>8;
		*ctx->ct++=cv;
	}else if(cv<0x10000000)
	{
		*ctx->ct++=0xF0;
		*ctx->ct++=cv>>24;
		*ctx->ct++=cv>>16;
		*ctx->ct++=cv>>8;
		*ctx->ct++=cv;
	}
}

void LQTVQ_EncEmitGenericVal(BT4A_Context *ctx, int cv)
{
	LQTVQ_EncEmitGenericUVal(ctx, (cv<<1)^(cv>>31));
}

void LQTVQ_EncEmitCount(BT4A_Context *ctx, int cv)
{
	LQTVQ_EncEmitGenericUVal(ctx, (cv<<1)^(cv>>31));
}

#else

void LQTVQ_EncEmitGenericUVal(BT4A_Context *ctx, int cv)
{
	LQTVQ_WriteAdRiceLL(ctx, cv, &(ctx->rk_misc));
}

void LQTVQ_EncEmitGenericVal(BT4A_Context *ctx, int cv)
{
	LQTVQ_WriteAdSRiceLL(ctx, cv, &(ctx->rk_misc));
}

void LQTVQ_EncEmitCount(BT4A_Context *ctx, int cv)
{
	LQTVQ_WriteAdRiceLL(ctx, cv, &(ctx->rk_cnt));
}

#endif

force_inline void LQTVQ_EncEmitCommand(BT4A_Context *ctx, int cmd)
{
#ifdef LQTVQ_BYTES
	*ctx->ct++=cmd;
#else
	LQTVQ_WriteSymbolSmtf(ctx, &(ctx->sm_cmd), cmd);
#endif
}

#ifndef LQTVQ_BYTES
force_inline void LQTVQ_EncEmitMask(BT4A_Context *ctx, int msk, int mmsk)
{
	int i, j, k;
//	*ctx->ct++=cmd;
//	LQTVQ_WriteSymbolSmtf(ctx, &(ctx->sm_mask), cmd);

	if((ctx->cmask&mmsk)==msk)
	{
		LQTVQ_WriteAdRiceLL(ctx, 0, &(ctx->sm_mask.rk));
		return;
	}

	msk=(ctx->cmask&(~mmsk))|msk|1;

	i=LQTVQ_EmitIndexSymbolSmtf(ctx, &(ctx->sm_mask), msk);
	LQTVQ_WriteAdRiceLL(ctx, i+1, &(ctx->sm_mask.rk));
	ctx->cmask=msk;
}
#endif

void LQTVQ_FastEncYUV(BT4A_Context *ctx, int cy, int cu, int cv)
{
	int dcy, dcu, dcv, mv;
	int adcy, adcu, adcv;
	int i, j, k;
	
	dcy=((cy-ctx->cy)*ctx->qfy +128)>>8;
	dcu=((cu-ctx->cu)*ctx->qfuv+128)>>8;
	dcv=((cv-ctx->cv)*ctx->qfuv+128)>>8;

#ifdef LQTVQ_BYTES

	adcy=(dcy<<1)^(dcy>>31);
	adcu=(dcu<<1)^(dcu>>31);
	adcv=(dcv<<1)^(dcv>>31);
	k=adcy|adcu|adcv;

#if 1
	if(!k)
	{
		*ctx->ct++=0;
	}else if(!(k>>5))
	{
		mv=(adcv<<11)|(adcu<<6)|(adcy<<1)|1;
		*ctx->ct++=mv;
		*ctx->ct++=mv>>8;
	}else
#endif
//	if(1)
	{
		mv=0;
		if(dcy)mv|=0x02;
		if(dcu)mv|=0x04;
		if(dcv)mv|=0x08;

		*ctx->ct++=mv;
		if(mv&0x02)LQTVQ_EncEmitGenericVal(ctx, dcy);
		if(mv&0x04)LQTVQ_EncEmitGenericVal(ctx, dcu);
		if(mv&0x08)LQTVQ_EncEmitGenericVal(ctx, dcv);
	}

#else
#if 1
	mv=0;
	if(dcy)mv|=0x02;
	if(dcu)mv|=0x04;
	if(dcv)mv|=0x08;
	
	LQTVQ_EncEmitMask(ctx, mv, 0x0E);
	if(mv&0x02)LQTVQ_WriteAdSRiceLL(ctx, dcy, &ctx->rk_cy );
	if(mv&0x04)LQTVQ_WriteAdSRiceLL(ctx, dcu, &ctx->rk_cuv);
	if(mv&0x08)LQTVQ_WriteAdSRiceLL(ctx, dcv, &ctx->rk_cuv);
#endif

//	LQTVQ_WriteAdSRiceLL(ctx, dcy, &ctx->rk_cy );
//	LQTVQ_WriteAdSRiceLL(ctx, dcu, &ctx->rk_cuv);
//	LQTVQ_WriteAdSRiceLL(ctx, dcv, &ctx->rk_cuv);
#endif

	ctx->cy+=dcy*ctx->qy;
	ctx->cu+=dcu*ctx->quv;
	ctx->cv+=dcv*ctx->quv;
}

void LQTVQ_FastEncYUVD(BT4A_Context *ctx,
	int cy, int cu, int cv, int dy)
{
	int dcy, dcu, dcv, ddy;
	int adcy, adcu, adcv, addy;
	int k, mv;
	
	dcy=((cy-ctx->cy)*ctx->qfy +128)>>8;
	dcu=((cu-ctx->cu)*ctx->qfuv+128)>>8;
	dcv=((cv-ctx->cv)*ctx->qfuv+128)>>8;
	ddy=((dy-ctx->dy)*ctx->qfdy+128)>>8;

#ifndef LQTVQ_BYTES
#if 1
	mv=0;
	if(dcy)mv|=0x02;
	if(dcu)mv|=0x04;
	if(dcv)mv|=0x08;
	if(ddy)mv|=0x10;

	LQTVQ_EncEmitMask(ctx, mv, 0x1E);
	if(mv&0x02)LQTVQ_WriteAdSRiceLL(ctx, dcy, &ctx->rk_cy );
	if(mv&0x04)LQTVQ_WriteAdSRiceLL(ctx, dcu, &ctx->rk_cuv);
	if(mv&0x08)LQTVQ_WriteAdSRiceLL(ctx, dcv, &ctx->rk_cuv);
	if(mv&0x10)LQTVQ_WriteAdSRiceLL(ctx, ddy, &ctx->rk_dy );
#endif

//	LQTVQ_WriteAdSRiceLL(ctx, dcy, &ctx->rk_cy );
//	LQTVQ_WriteAdSRiceLL(ctx, dcu, &ctx->rk_cuv);
//	LQTVQ_WriteAdSRiceLL(ctx, dcv, &ctx->rk_cuv);
//	LQTVQ_WriteAdSRiceLL(ctx, ddy, &ctx->rk_dy );

#else
	adcy=(dcy<<1)^(dcy>>31);
	adcu=(dcu<<1)^(dcu>>31);
	adcv=(dcv<<1)^(dcv>>31);
	addy=(ddy<<1)^(ddy>>31);
	k=adcy|adcu|adcv|addy;

	if(!k)
	{
		*ctx->ct++=0;
	}else if(!(k>>5))
	{
		mv=(addy<<16)|(adcv<<11)|(adcu<<6)|(adcy<<1)|1;
		*ctx->ct++=mv;
		*ctx->ct++=mv>>8;
		*ctx->ct++=mv>>16;
	}else
	{
		mv=0;
		if(dcy)mv|=0x02;
		if(dcu)mv|=0x04;
		if(dcv)mv|=0x08;
		if(ddy)mv|=0x10;

		*ctx->ct++=mv;
		if(mv&0x02)LQTVQ_EncEmitGenericVal(ctx, dcy);
		if(mv&0x04)LQTVQ_EncEmitGenericVal(ctx, dcu);
		if(mv&0x08)LQTVQ_EncEmitGenericVal(ctx, dcv);
		if(mv&0x10)LQTVQ_EncEmitGenericVal(ctx, ddy);
	}
#endif

	ctx->cy+=dcy*ctx->qy;
	ctx->cu+=dcu*ctx->quv;
	ctx->cv+=dcv*ctx->quv;
	ctx->dy+=ddy*ctx->qdy;
}

void LQTVQ_FastEncYUVDyuv(BT4A_Context *ctx,
	int cy, int cu, int cv,
	int dy, int du, int dv)
{
	int dcy, dcu, dcv, ddy, ddu, ddv;
	int adcy, adcu, adcv, addy, addu, addv;
	int k, mv;
	
	dcy=((cy-ctx->cy)*ctx->qfy  +128)>>8;
	dcu=((cu-ctx->cu)*ctx->qfuv +128)>>8;
	dcv=((cv-ctx->cv)*ctx->qfuv +128)>>8;
	ddy=((dy-ctx->dy)*ctx->qfdy +128)>>8;
	ddu=((du-ctx->du)*ctx->qfduv+128)>>8;
	ddv=((dv-ctx->dv)*ctx->qfduv+128)>>8;

#ifndef LQTVQ_BYTES

#if 1
	mv=0;
	if(dcy)mv|=0x02;
	if(dcu)mv|=0x04;
	if(dcv)mv|=0x08;
	if(ddy)mv|=0x10;
	if(ddu)mv|=0x20;
	if(ddv)mv|=0x40;

	LQTVQ_EncEmitMask(ctx, mv, 0x7E);
	if(mv&0x02)LQTVQ_WriteAdSRiceLL(ctx, dcy, &ctx->rk_cy );
	if(mv&0x04)LQTVQ_WriteAdSRiceLL(ctx, dcu, &ctx->rk_cuv);
	if(mv&0x08)LQTVQ_WriteAdSRiceLL(ctx, dcv, &ctx->rk_cuv);
	if(mv&0x10)LQTVQ_WriteAdSRiceLL(ctx, ddy, &ctx->rk_dy );
	if(mv&0x20)LQTVQ_WriteAdSRiceLL(ctx, ddu, &ctx->rk_duv);
	if(mv&0x40)LQTVQ_WriteAdSRiceLL(ctx, ddv, &ctx->rk_duv);
#endif

//	LQTVQ_WriteAdSRiceLL(ctx, dcy, &ctx->rk_cy );
//	LQTVQ_WriteAdSRiceLL(ctx, dcu, &ctx->rk_cuv);
//	LQTVQ_WriteAdSRiceLL(ctx, dcv, &ctx->rk_cuv);
//	LQTVQ_WriteAdSRiceLL(ctx, ddy, &ctx->rk_dy );
//	LQTVQ_WriteAdSRiceLL(ctx, ddu, &ctx->rk_duv);
//	LQTVQ_WriteAdSRiceLL(ctx, ddv, &ctx->rk_duv);

#else

	adcy=(dcy<<1)^(dcy>>31);
	adcu=(dcu<<1)^(dcu>>31);
	adcv=(dcv<<1)^(dcv>>31);
	addy=(ddy<<1)^(ddy>>31);
	addu=(ddu<<1)^(ddu>>31);
	addv=(ddv<<1)^(ddv>>31);
	k=adcy|adcu|adcv|addy|addu|addv;

	if(!k)
	{
		*ctx->ct++=0;
	}else if(!(k>>5))
	{
		mv=(addv<<27)|(addu<<22)|(1<<21)|(addy<<16)|
			(adcv<<11)|(adcu<<6)|(adcy<<1)|1;
//		*ctx->ct++=mv;
//		*ctx->ct++=mv>>8;
//		*ctx->ct++=mv>>16;
//		*ctx->ct++=mv>>24;
		*(u32 *)ctx->ct=mv;
		ctx->ct+=4;
	}else
	{
		mv=0;
		if(dcy)mv|=0x02;
		if(dcu)mv|=0x04;
		if(dcv)mv|=0x08;
		if(ddy)mv|=0x10;
		if(ddu)mv|=0x20;
		if(ddv)mv|=0x40;

		*ctx->ct++=mv;
		if(mv&0x02)LQTVQ_EncEmitGenericVal(ctx, dcy);
		if(mv&0x04)LQTVQ_EncEmitGenericVal(ctx, dcu);
		if(mv&0x08)LQTVQ_EncEmitGenericVal(ctx, dcv);
		if(mv&0x10)LQTVQ_EncEmitGenericVal(ctx, ddy);
		if(mv&0x20)LQTVQ_EncEmitGenericVal(ctx, ddu);
		if(mv&0x40)LQTVQ_EncEmitGenericVal(ctx, ddv);
	}

#endif

	ctx->cy+=dcy*ctx->qy;
	ctx->cu+=dcu*ctx->quv;
	ctx->cv+=dcv*ctx->quv;
	ctx->dy+=ddy*ctx->qdy;
	ctx->du+=ddu*ctx->qduv;
	ctx->dv+=ddv*ctx->qduv;
}

force_inline void LQTVQ_EncEmit8B(BT4A_Context *ctx, byte *buf)
{
#ifndef LQTVQ_BYTES
	LQTVQ_Write8BitsNM(ctx, *buf);
#else
	*ctx->ct++=*buf;
#endif
}

force_inline void LQTVQ_EncEmit16B(BT4A_Context *ctx, byte *buf)
{
#ifndef LQTVQ_BYTES
	LQTVQ_Write16BitsNM(ctx, *(u16 *)buf);
#else
	*(u16 *)ctx->ct=*(u16 *)buf;
	ctx->ct+=2;
#endif
}

force_inline void LQTVQ_EncEmit32B(BT4A_Context *ctx, byte *buf)
{
#ifndef LQTVQ_BYTES
	LQTVQ_Write32Bits(ctx, *(u32 *)buf);
#else
	*(u32 *)ctx->ct=*(u32 *)buf;
	ctx->ct+=4;
#endif
}

force_inline void LQTVQ_EncEmit64B(BT4A_Context *ctx, byte *buf)
{
#ifndef LQTVQ_BYTES
	LQTVQ_Write32Bits(ctx, ((u32 *)buf)[0]);
	LQTVQ_Write32Bits(ctx, ((u32 *)buf)[1]);
#else
	*(u64 *)ctx->ct=*(u64 *)buf;
	ctx->ct+=8;
#endif
}

force_inline void LQTVQ_EncEmit96B(BT4A_Context *ctx, byte *buf)
{
#ifndef LQTVQ_BYTES
	LQTVQ_Write32Bits(ctx, ((u32 *)buf)[0]);
	LQTVQ_Write32Bits(ctx, ((u32 *)buf)[1]);
	LQTVQ_Write32Bits(ctx, ((u32 *)buf)[2]);
#else
	*(u64 *)ctx->ct=*(u64 *)buf;
	*(u32 *)ctx->ct=*(u32 *)buf;
	ctx->ct+=12;
#endif
}

force_inline void LQTVQ_EncEmit128B(BT4A_Context *ctx, byte *buf)
{
#ifndef LQTVQ_BYTES
	((u64 *)ctx->ct)[0]=((u64 *)buf)[0];
	((u64 *)ctx->ct)[1]=((u64 *)buf)[1];
	ctx->ct+=16;
#else
	LQTVQ_Write32Bits(ctx, ((u32 *)buf)[0]);
	LQTVQ_Write32Bits(ctx, ((u32 *)buf)[1]);
	LQTVQ_Write32Bits(ctx, ((u32 *)buf)[2]);
	LQTVQ_Write32Bits(ctx, ((u32 *)buf)[3]);
#endif
}

force_inline void LQTVQ_EncEmit192B(BT4A_Context *ctx, byte *buf)
{
#ifdef LQTVQ_BYTES
	((u64 *)ctx->ct)[0]=((u64 *)buf)[0];
	((u64 *)ctx->ct)[1]=((u64 *)buf)[1];
	((u64 *)ctx->ct)[2]=((u64 *)buf)[2];
	ctx->ct+=24;
#else
	LQTVQ_Write32Bits(ctx, ((u32 *)buf)[0]);
	LQTVQ_Write32Bits(ctx, ((u32 *)buf)[1]);
	LQTVQ_Write32Bits(ctx, ((u32 *)buf)[2]);
	LQTVQ_Write32Bits(ctx, ((u32 *)buf)[3]);
	LQTVQ_Write32Bits(ctx, ((u32 *)buf)[4]);
	LQTVQ_Write32Bits(ctx, ((u32 *)buf)[5]);
#endif
}

force_inline void LQTVQ_EncEmit256B(BT4A_Context *ctx, byte *buf)
{
#ifdef LQTVQ_BYTES
	((u64 *)ctx->ct)[0]=((u64 *)buf)[0];
	((u64 *)ctx->ct)[1]=((u64 *)buf)[1];
	((u64 *)ctx->ct)[2]=((u64 *)buf)[2];
	((u64 *)ctx->ct)[3]=((u64 *)buf)[3];
	ctx->ct+=32;
#else
	LQTVQ_Write32Bits(ctx, ((u32 *)buf)[0]);
	LQTVQ_Write32Bits(ctx, ((u32 *)buf)[1]);
	LQTVQ_Write32Bits(ctx, ((u32 *)buf)[2]);
	LQTVQ_Write32Bits(ctx, ((u32 *)buf)[3]);
	LQTVQ_Write32Bits(ctx, ((u32 *)buf)[4]);
	LQTVQ_Write32Bits(ctx, ((u32 *)buf)[5]);
	LQTVQ_Write32Bits(ctx, ((u32 *)buf)[6]);
	LQTVQ_Write32Bits(ctx, ((u32 *)buf)[7]);
#endif
}

force_inline byte *LQTVQ_EncFlushBits(BT4A_Context *ctx)
{
#ifndef LQTVQ_BYTES
	return(LQTVQ_EndWriteBits(ctx));
#else
	return(ctx->ct);
#endif
}

int LQTVQ_SetupContextQf(BT4A_Context *ctx, int qf)
{
	float qsc;
	
	qsc=(100-(qf&127))/25.0;
	
//	ctx->qdy_flat=16*qsc;
	ctx->qdy_flat=20*qsc;
	ctx->qdy_2x2x2=32*qsc;
//	ctx->qdy_4x4x2=48*qsc;
	ctx->qdy_4x4x2=64*qsc;
//	ctx->qdy_4x4x2=56*qsc;
	ctx->qdy_8x8x2=80*qsc;
//	ctx->qdy_8x8x2=96*qsc;
//	ctx->qdy_8x8x2=112*qsc;
//	ctx->qdy_8x8x3=192*qsc;
//	ctx->qdy_8x8x2=128*qsc;
	ctx->qdy_8x8x3=224*qsc;

	ctx->qduv_flat=16*qsc;

	ctx->rk_cy=2;
	ctx->rk_cuv=2;
	ctx->rk_dy=2;
	ctx->rk_duv=2;
	ctx->rk_cnt=2;
	ctx->rk_misc=2;

	ctx->qy=6*qsc;
	ctx->quv=8*qsc;
	ctx->qdy=6*qsc;
	ctx->qduv=8*qsc;
	if(ctx->qy<1)ctx->qy=1;
	if(ctx->quv<1)ctx->quv=1;
	if(ctx->qdy<1)ctx->qdy=1;
	if(ctx->qduv<1)ctx->qduv=1;

	ctx->qfy=256/ctx->qy;
	ctx->qfuv=256/ctx->quv;
	ctx->qfdy=256/ctx->qdy;
	ctx->qfduv=256/ctx->qduv;
	
	ctx->cy=0;	ctx->cu=0;	ctx->cv=0;
	ctx->dy=0;	ctx->du=0;	ctx->dv=0;
	
	LQTVQ_ResetSmtfDefault(ctx, &(ctx->sm_cmd));
	LQTVQ_ResetSmtfReverse(ctx, &(ctx->sm_mask));

	return(0);
}

int LQTVQ_EncImgBufFastBGRA(BT4A_Context *ctx,
	byte *cbuf, byte *ibuf, int xs, int ys, int qf)
{
	byte blkb[64];
	byte *cs, *ct;
	int xs1, ys1, ystr;
	int bi;
	int i, j, k;
	
	LQTVQ_InitScTables();
	LQTVQ_InitRice();
	
	LQTVQ_SetupContextQf(ctx, qf);
	LQTVQ_SetupWriteBits(ctx, cbuf, xs*ys*8);
	
	ystr=xs*4;
	xs1=xs>>3;
	ys1=ys>>3;
	
//	ctx->ct=cbuf;

	LQTVQ_EncEmitCommand(ctx, 0x45);
	LQTVQ_EncEmitGenericUVal(ctx, ctx->qy);
	LQTVQ_EncEmitGenericUVal(ctx, ctx->quv);
	LQTVQ_EncEmitGenericUVal(ctx, ctx->qdy);
	LQTVQ_EncEmitGenericUVal(ctx, ctx->qduv);
	
	for(i=0; i<ys1; i++)
	{
		cs=ibuf+(i*8)*ystr;
//		ct=blks+((i*xs1)<<6);
		for(j=0; j<xs1; j++)
		{
			LQTVQ_EncBlockBGRA(ctx, blkb, cs, ystr);
			cs+=32;

//			LQTVQ_EncBlockBGRA(ct, cs, ystr);
//			ct+=64;

			switch(blkb[0])
			{
			case 0x00:
				LQTVQ_EncEmitCommand(ctx, 0x00);
				LQTVQ_FastEncYUV(ctx,
					*(u16 *)(blkb+ 4), *(u16 *)(blkb+ 6), *(u16 *)(blkb+ 8));
				break;
			case 0x09:
				LQTVQ_EncEmitCommand(ctx, 0x09);
				LQTVQ_FastEncYUVD(ctx,
					*(u16 *)(blkb+ 4), *(u16 *)(blkb+ 6),
					*(u16 *)(blkb+ 8), *(u16 *)(blkb+10));
//				*ctx->ct++=blkb[16];
				LQTVQ_EncEmit8B(ctx, blkb+16);
				break;
			case 0x0A:	case 0x0B:
				LQTVQ_EncEmitCommand(ctx, blkb[0]);
				LQTVQ_FastEncYUVD(ctx,
					*(u16 *)(blkb+ 4), *(u16 *)(blkb+ 6),
					*(u16 *)(blkb+ 8), *(u16 *)(blkb+10));
				LQTVQ_EncEmit16B(ctx, blkb+16);
				break;
			case 0x0C:
				LQTVQ_EncEmitCommand(ctx, 0x0C);
				LQTVQ_FastEncYUVD(ctx,
					*(u16 *)(blkb+ 4), *(u16 *)(blkb+ 6),
					*(u16 *)(blkb+ 8), *(u16 *)(blkb+10));
				LQTVQ_EncEmit32B(ctx, blkb+16);
				break;

			case 0x0D:	case 0x0E:
				LQTVQ_EncEmitCommand(ctx, blkb[0]);
				LQTVQ_FastEncYUVD(ctx,
					*(u16 *)(blkb+ 4), *(u16 *)(blkb+ 6),
					*(u16 *)(blkb+ 8), *(u16 *)(blkb+10));
				LQTVQ_EncEmit64B(ctx, blkb+16);
				break;
			case 0x0F:
				LQTVQ_EncEmitCommand(ctx, 0x0F);
				LQTVQ_FastEncYUVD(ctx,
					*(u16 *)(blkb+ 4), *(u16 *)(blkb+ 6),
					*(u16 *)(blkb+ 8), *(u16 *)(blkb+10));
				LQTVQ_EncEmit128B(ctx, blkb+16);
				break;

			case 0x13:
				LQTVQ_EncEmitCommand(ctx, blkb[0]);
				LQTVQ_FastEncYUVD(ctx,
					*(u16 *)(blkb+ 4), *(u16 *)(blkb+ 6),
					*(u16 *)(blkb+ 8), *(u16 *)(blkb+10));
				LQTVQ_EncEmit192B(ctx, blkb+16);
				break;

			case 0x17:
				LQTVQ_EncEmitCommand(ctx, blkb[0]);
				LQTVQ_FastEncYUVD(ctx,
					*(u16 *)(blkb+ 4), *(u16 *)(blkb+ 6),
					*(u16 *)(blkb+ 8), *(u16 *)(blkb+10));
				LQTVQ_EncEmit256B(ctx, blkb+16);
				break;
			case 0x18:
				LQTVQ_EncEmitCommand(ctx, 0x18);
				LQTVQ_FastEncYUVDyuv(ctx,
					*(u16 *)(blkb+ 4), *(u16 *)(blkb+ 6), *(u16 *)(blkb+ 8),
					*(u16 *)(blkb+10), *(u16 *)(blkb+12), *(u16 *)(blkb+14));
				LQTVQ_EncEmit128B(ctx, blkb+16);
				LQTVQ_EncEmit32B(ctx, blkb+32);
				LQTVQ_EncEmit32B(ctx, blkb+36);
				break;
			case 0x19:
				break;
			case 0x1A:
				LQTVQ_EncEmitCommand(ctx, 0x1A);
				LQTVQ_FastEncYUVDyuv(ctx,
					*(u16 *)(blkb+ 4), *(u16 *)(blkb+ 6), *(u16 *)(blkb+ 8),
					*(u16 *)(blkb+10), *(u16 *)(blkb+12), *(u16 *)(blkb+14));
				LQTVQ_EncEmit128B(ctx, blkb+16);
				LQTVQ_EncEmit64B(ctx, blkb+32);
				LQTVQ_EncEmit64B(ctx, blkb+40);
				break;
			case 0x1B:
				LQTVQ_EncEmitCommand(ctx, 0x1B);
				LQTVQ_FastEncYUVDyuv(ctx,
					*(u16 *)(blkb+ 4), *(u16 *)(blkb+ 6), *(u16 *)(blkb+ 8),
					*(u16 *)(blkb+10), *(u16 *)(blkb+12), *(u16 *)(blkb+14));
				LQTVQ_EncEmit128B(ctx, blkb+16);
				LQTVQ_EncEmit128B(ctx, blkb+32);
				LQTVQ_EncEmit128B(ctx, blkb+48);
				break;
			case 0x1C:
				LQTVQ_EncEmitCommand(ctx, 0x1C);
				LQTVQ_FastEncYUVDyuv(ctx,
					*(u16 *)(blkb+ 4), *(u16 *)(blkb+ 6), *(u16 *)(blkb+ 8),
					*(u16 *)(blkb+10), *(u16 *)(blkb+12), *(u16 *)(blkb+14));
				LQTVQ_EncEmit192B(ctx, blkb+16);
				LQTVQ_EncEmit32B(ctx, blkb+40);
				LQTVQ_EncEmit32B(ctx, blkb+44);
				break;
			default:
				*(int *)-1=-1;
			}
		}
	}
	return(ctx->ct-cbuf);
}


int LQTVQ_EncImgBlocks(BT4A_Context *ctx,
	byte *cbuf, byte *blks, byte *lblks, int xs, int ys, int qf)
{
	byte *cs, *cse, *csl;
	byte *blkb;
	int xs1, ys1;
	int bi;
	int i, j, k, n;
	
	LQTVQ_InitScTables();
	LQTVQ_InitRice();
	
	LQTVQ_SetupContextQf(ctx, qf);
	LQTVQ_SetupWriteBits(ctx, cbuf, xs*ys*8);
	
	xs1=(xs+7)>>3;
	ys1=(ys+7)>>3;
	n=xs1*ys1;
	
//	ctx->ct=cbuf;

	LQTVQ_EncEmitCommand(ctx, 0x45);
	LQTVQ_EncEmitGenericUVal(ctx, ctx->qy);
	LQTVQ_EncEmitGenericUVal(ctx, ctx->quv);
	LQTVQ_EncEmitGenericUVal(ctx, ctx->qdy);
	LQTVQ_EncEmitGenericUVal(ctx, ctx->qduv);
	
	cs=blks;
	csl=lblks;
	
	cse=cs+n*64;
	
	while(cs<cse)
	{
		blkb=cs;
		cs+=64;

		switch(*cs)
		{
		case 0x00:
			LQTVQ_EncEmitCommand(ctx, 0x00);
			LQTVQ_FastEncYUV(ctx,
				*(u16 *)(blkb+ 4), *(u16 *)(blkb+ 6), *(u16 *)(blkb+ 8));
			break;
		case 0x09:
			LQTVQ_EncEmitCommand(ctx, 0x09);
			LQTVQ_FastEncYUVD(ctx,
				*(u16 *)(blkb+ 4), *(u16 *)(blkb+ 6),
				*(u16 *)(blkb+ 8), *(u16 *)(blkb+10));
			LQTVQ_EncEmit8B(ctx, blkb+16);
			break;
		case 0x0A:	case 0x0B:
			LQTVQ_EncEmitCommand(ctx, blkb[0]);
			LQTVQ_FastEncYUVD(ctx,
				*(u16 *)(blkb+ 4), *(u16 *)(blkb+ 6),
				*(u16 *)(blkb+ 8), *(u16 *)(blkb+10));
			LQTVQ_EncEmit16B(ctx, blkb+16);
			break;
		case 0x0C:
			LQTVQ_EncEmitCommand(ctx, 0x0C);
			LQTVQ_FastEncYUVD(ctx,
				*(u16 *)(blkb+ 4), *(u16 *)(blkb+ 6),
				*(u16 *)(blkb+ 8), *(u16 *)(blkb+10));
			LQTVQ_EncEmit32B(ctx, blkb+16);
			break;

		case 0x0D:	case 0x0E:
			LQTVQ_EncEmitCommand(ctx, blkb[0]);
			LQTVQ_FastEncYUVD(ctx,
				*(u16 *)(blkb+ 4), *(u16 *)(blkb+ 6),
				*(u16 *)(blkb+ 8), *(u16 *)(blkb+10));
			LQTVQ_EncEmit64B(ctx, blkb+16);
			break;
		case 0x0F:
			LQTVQ_EncEmitCommand(ctx, 0x0F);
			LQTVQ_FastEncYUVD(ctx,
				*(u16 *)(blkb+ 4), *(u16 *)(blkb+ 6),
				*(u16 *)(blkb+ 8), *(u16 *)(blkb+10));
			LQTVQ_EncEmit128B(ctx, blkb+16);
			break;

		case 0x13:
			LQTVQ_EncEmitCommand(ctx, blkb[0]);
			LQTVQ_FastEncYUVD(ctx,
				*(u16 *)(blkb+ 4), *(u16 *)(blkb+ 6),
				*(u16 *)(blkb+ 8), *(u16 *)(blkb+10));
			LQTVQ_EncEmit192B(ctx, blkb+16);
			break;

		case 0x17:
			LQTVQ_EncEmitCommand(ctx, blkb[0]);
			LQTVQ_FastEncYUVD(ctx,
				*(u16 *)(blkb+ 4), *(u16 *)(blkb+ 6),
				*(u16 *)(blkb+ 8), *(u16 *)(blkb+10));
			LQTVQ_EncEmit256B(ctx, blkb+16);
			break;
		case 0x18:
			LQTVQ_EncEmitCommand(ctx, 0x18);
			LQTVQ_FastEncYUVDyuv(ctx,
				*(u16 *)(blkb+ 4), *(u16 *)(blkb+ 6), *(u16 *)(blkb+ 8),
				*(u16 *)(blkb+10), *(u16 *)(blkb+12), *(u16 *)(blkb+14));
			LQTVQ_EncEmit128B(ctx, blkb+16);
			LQTVQ_EncEmit32B(ctx, blkb+32);
			LQTVQ_EncEmit32B(ctx, blkb+36);
			break;
		case 0x19:
			break;
		case 0x1A:
			LQTVQ_EncEmitCommand(ctx, 0x1A);
			LQTVQ_FastEncYUVDyuv(ctx,
				*(u16 *)(blkb+ 4), *(u16 *)(blkb+ 6), *(u16 *)(blkb+ 8),
				*(u16 *)(blkb+10), *(u16 *)(blkb+12), *(u16 *)(blkb+14));
			LQTVQ_EncEmit128B(ctx, blkb+16);
			LQTVQ_EncEmit64B(ctx, blkb+32);
			LQTVQ_EncEmit64B(ctx, blkb+40);
			break;
		case 0x1B:
			LQTVQ_EncEmitCommand(ctx, 0x1B);
			LQTVQ_FastEncYUVDyuv(ctx,
				*(u16 *)(blkb+ 4), *(u16 *)(blkb+ 6), *(u16 *)(blkb+ 8),
				*(u16 *)(blkb+10), *(u16 *)(blkb+12), *(u16 *)(blkb+14));
			LQTVQ_EncEmit128B(ctx, blkb+16);
			LQTVQ_EncEmit128B(ctx, blkb+32);
			LQTVQ_EncEmit128B(ctx, blkb+48);
			break;
		case 0x1C:
			LQTVQ_EncEmitCommand(ctx, 0x1C);
			LQTVQ_FastEncYUVDyuv(ctx,
				*(u16 *)(blkb+ 4), *(u16 *)(blkb+ 6), *(u16 *)(blkb+ 8),
				*(u16 *)(blkb+10), *(u16 *)(blkb+12), *(u16 *)(blkb+14));
			LQTVQ_EncEmit192B(ctx, blkb+16);
			LQTVQ_EncEmit32B(ctx, blkb+40);
			LQTVQ_EncEmit32B(ctx, blkb+44);
			break;
		default:
			*(int *)-1=-1;
			break;
		}
	}
	
	blkb=LQTVQ_EncFlushBits(ctx);
	return(blkb-cbuf);
}
