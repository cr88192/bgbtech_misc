
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

force_inline void LQTVQ_EncEmitCommand(BT4A_Context *ctx, int cmd)
{
	*ctx->ct++=cmd;
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

force_inline void LQTVQ_EncEmitCommand(BT4A_Context *ctx, int cmd)
{
	LQTVQ_WriteSymbolSmtf(ctx, &(ctx->sm_cmd), cmd);
}

#endif

#ifndef LQTVQ_BYTES
force_inline int LQTVQ_EncEmitMask(BT4A_Context *ctx, int msk, int mmsk)
{
	int i0, i1;
	int i, j, k;
//	*ctx->ct++=cmd;
//	LQTVQ_WriteSymbolSmtf(ctx, &(ctx->sm_mask), cmd);

//	if(((ctx->cmask&mmsk)==msk) ||
//		(((ctx->cmask|msk)&mmsk)==(ctx->cmask&mmsk)))
	if((ctx->cmask==(ctx->cmask|msk)) &&
		((ctx->cmask&mmsk)==((ctx->imask|msk)&mmsk)))
//	if((ctx->cmask&mmsk)==msk)
	{
		i0=ctx->stat_bits;
		LQTVQ_WriteAdRiceLL(ctx, 0, &(ctx->sm_mask.rk));

		i1=ctx->stat_bits;
		ctx->sm_mask.bits+=i1-i0;
		ctx->sm_mask.cnt++;

		return(ctx->cmask);
	}

	msk=(ctx->imask&(~mmsk))|msk|0;
//	msk=(ctx->imask&(~mmsk))|msk|1;
	
	if(((ctx->imask|msk)&mmsk)==(ctx->imask&mmsk))
	{
		msk=ctx->imask|1;
	}

//	msk|=0x80;

	i0=ctx->stat_bits;

	i=LQTVQ_EmitIndexSymbolSmtf(ctx, &(ctx->sm_mask), msk);
	LQTVQ_WriteAdRiceLL(ctx, i+1, &(ctx->sm_mask.rk));
	if(msk&1)
		ctx->cmask=msk;

	i1=ctx->stat_bits;
	ctx->sm_mask.bits+=i1-i0;
	ctx->sm_mask.cnt++;

	return(msk);
}
#endif

void LQTVQ_EmitEnableMask(BT4A_Context *ctx)
{
	LQTVQ_EncEmitCommand(ctx, 0x46);
	LQTVQ_WriteAdSRiceLL(ctx, -1, &ctx->rk_misc);
	ctx->cmask=0xFF;
}

void LQTVQ_EmitDisableMask(BT4A_Context *ctx)
{
	LQTVQ_EncEmitCommand(ctx, 0x47);
	LQTVQ_WriteAdSRiceLL(ctx, -1, &ctx->rk_misc);
	ctx->cmask=0x00;
}

void LQTVQ_EmitSetParm(BT4A_Context *ctx, int var, int val)
{
	LQTVQ_EncEmitCommand(ctx, 0x43);
	LQTVQ_WriteAdSRiceLL(ctx, var, &ctx->rk_misc);
	LQTVQ_WriteAdSRiceLL(ctx, val, &ctx->rk_misc);
	
	if(var==-2)
	{
		ctx->pred=val;
	}
}

int LQTVQ_EncCheckEnableMask(BT4A_Context *ctx)
{
	int i, j, k, j0, j1;

//	return(0);

	if(ctx->stat_yuvcnt<256)
		return(0);

	if(ctx->cmask)
	{
//		j=ctx->stat_yuvcnt>>1;
		j=(ctx->stat_yuvcnt*5)>>3;
		k=(ctx->stat_yuvcnt*3)>>3;
		j0=-j;
		j1=-k;
	
		i=ctx->imask;
		if(ctx->yuv_cz[0]<j0)i&=(~0x02);
		if(ctx->yuv_cz[1]<j0)i&=(~0x04);
		if(ctx->yuv_cz[2]<j0)i&=(~0x08);
		if(ctx->yuv_cz[3]<j0)i&=(~0x10);
		if(ctx->yuv_cz[4]<j0)i&=(~0x20);
		if(ctx->yuv_cz[5]<j0)i&=(~0x40);

//		if(ctx->yuv_cz[0]>j)i|=0x02;
//		if(ctx->yuv_cz[1]>j)i|=0x04;
//		if(ctx->yuv_cz[2]>j)i|=0x08;
//		if(ctx->yuv_cz[3]>j)i|=0x10;
//		if(ctx->yuv_cz[4]>j)i|=0x20;
//		if(ctx->yuv_cz[5]>j)i|=0x40;

		if(ctx->yuv_cz[0]>j1)i|=0x02;
		if(ctx->yuv_cz[1]>j1)i|=0x04;
		if(ctx->yuv_cz[2]>j1)i|=0x08;
		if(ctx->yuv_cz[3]>j1)i|=0x10;
		if(ctx->yuv_cz[4]>j1)i|=0x20;
		if(ctx->yuv_cz[5]>j1)i|=0x40;

		if(i==0xFF)
		{
			LQTVQ_EmitDisableMask(ctx);
		}

		ctx->imask=i;
		return(0);
	}

//	j=ctx->stat_yuvcnt>>1;
	j=(ctx->stat_yuvcnt*5)>>3;
	j0=-j;

	i=0xFF;
	if(ctx->yuv_cz[0]<j0)i&=(~0x02);
	if(ctx->yuv_cz[1]<j0)i&=(~0x04);
	if(ctx->yuv_cz[2]<j0)i&=(~0x08);
	if(ctx->yuv_cz[3]<j0)i&=(~0x10);
	if(ctx->yuv_cz[4]<j0)i&=(~0x20);
	if(ctx->yuv_cz[5]<j0)i&=(~0x40);

	if(i==0xFF)
		return(0);

	LQTVQ_EmitEnableMask(ctx);
	ctx->imask=i;
	return(0);
}

void LQTVQ_FastEncYUV(BT4A_Context *ctx, int cy, int cu, int cv)
{
	int dcy, dcu, dcv, mv;
	int adcy, adcu, adcv;
	int i0, i1;
	int i, j, k;
	
//	dcy=((cy-ctx->cy)*ctx->qfy +128)>>8;
//	dcu=((cu-ctx->cu)*ctx->qfuv+128)>>8;
//	dcv=((cv-ctx->cv)*ctx->qfuv+128)>>8;

	dcy=((cy-ctx->cy)*ctx->qfy +2048)>>12;
	dcu=((cu-ctx->cu)*ctx->qfuv+2048)>>12;
	dcv=((cv-ctx->cv)*ctx->qfuv+2048)>>12;

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
	i0=ctx->stat_bits;

	if(ctx->cmask)
	{
		mv=0;
		if(dcy)mv|=0x02;
		if(dcu)mv|=0x04;
		if(dcv)mv|=0x08;

		ctx->yuv_cz[0]+=(dcy!=0)*2-1;
		ctx->yuv_cz[1]+=(dcu!=0)*2-1;
		ctx->yuv_cz[2]+=(dcv!=0)*2-1;
	
		mv=LQTVQ_EncEmitMask(ctx, mv, 0x0E);
//		if(mv&0x02)LQTVQ_WriteAdSRiceLL(ctx, dcy, &ctx->rk_cy );
//		if(mv&0x04)LQTVQ_WriteAdSRiceLL(ctx, dcu, &ctx->rk_cuv);
//		if(mv&0x08)LQTVQ_WriteAdSRiceLL(ctx, dcv, &ctx->rk_cuv);

		if(mv&0x02)LQTVQ_WriteAdSRiceLLb(ctx,
			dcy, &ctx->rk_cy , ctx->stat_yuvcbits+0);
		if(mv&0x04)LQTVQ_WriteAdSRiceLLb(ctx,
			dcu, &ctx->rk_cuv, ctx->stat_yuvcbits+1);
		if(mv&0x08)LQTVQ_WriteAdSRiceLLb(ctx,
			dcv, &ctx->rk_cuv, ctx->stat_yuvcbits+2);
	}else
	{
		ctx->yuv_cz[0]+=(dcy!=0)*2-1;
		ctx->yuv_cz[1]+=(dcu!=0)*2-1;
		ctx->yuv_cz[2]+=(dcv!=0)*2-1;
	
//		LQTVQ_WriteAdSRiceLL(ctx, dcy, &ctx->rk_cy );
//		LQTVQ_WriteAdSRiceLL(ctx, dcu, &ctx->rk_cuv);
//		LQTVQ_WriteAdSRiceLL(ctx, dcv, &ctx->rk_cuv);

		LQTVQ_WriteAdSRiceLLb(ctx, dcy, &ctx->rk_cy , ctx->stat_yuvcbits+0);
		LQTVQ_WriteAdSRiceLLb(ctx, dcu, &ctx->rk_cuv, ctx->stat_yuvcbits+1);
		LQTVQ_WriteAdSRiceLLb(ctx, dcv, &ctx->rk_cuv, ctx->stat_yuvcbits+2);
	}

	i1=ctx->stat_bits;
	ctx->stat_yuvbits+=i1-i0;
	ctx->stat_yuvcnt++;

#endif

	ctx->cy+=dcy*ctx->qy;
	ctx->cu+=dcu*ctx->quv;
	ctx->cv+=dcv*ctx->quv;
	
#if 0
	if(	(ctx->cy<   0) || (ctx->cy>255) ||
		(ctx->cu<-255) || (ctx->cu>255) ||
		(ctx->cv<-255) || (ctx->cv>255))
	{
		__asm { int 3 }
	}
#endif
}

void LQTVQ_FastEncYUVD(BT4A_Context *ctx,
	int cy, int cu, int cv, int dy)
{
	int dcy, dcu, dcv, ddy;
	int adcy, adcu, adcv, addy;
	int i0, i1;
	int k, mv;
	
//	dcy=((cy-ctx->cy)*ctx->qfy +128)>>8;
//	dcu=((cu-ctx->cu)*ctx->qfuv+128)>>8;
//	dcv=((cv-ctx->cv)*ctx->qfuv+128)>>8;
//	ddy=((dy-ctx->dy)*ctx->qfdy+128)>>8;

	dcy=((cy-ctx->cy)*ctx->qfy +2048)>>12;
	dcu=((cu-ctx->cu)*ctx->qfuv+2048)>>12;
	dcv=((cv-ctx->cv)*ctx->qfuv+2048)>>12;
	ddy=((dy-ctx->dy)*ctx->qfdy+2048)>>12;

#ifndef LQTVQ_BYTES
	i0=ctx->stat_bits;

	if(ctx->cmask)
	{
		mv=0;
		if(dcy)mv|=0x02;
		if(dcu)mv|=0x04;
		if(dcv)mv|=0x08;
		if(ddy)mv|=0x10;

		ctx->yuv_cz[0]+=(dcy!=0)*2-1;
		ctx->yuv_cz[1]+=(dcu!=0)*2-1;
		ctx->yuv_cz[2]+=(dcv!=0)*2-1;
		ctx->yuv_cz[3]+=(ddy!=0)*2-1;

		mv=LQTVQ_EncEmitMask(ctx, mv, 0x1E);
//		if(mv&0x02)LQTVQ_WriteAdSRiceLL(ctx, dcy, &ctx->rk_cy );
//		if(mv&0x04)LQTVQ_WriteAdSRiceLL(ctx, dcu, &ctx->rk_cuv);
//		if(mv&0x08)LQTVQ_WriteAdSRiceLL(ctx, dcv, &ctx->rk_cuv);
//		if(mv&0x10)LQTVQ_WriteAdSRiceLL(ctx, ddy, &ctx->rk_dy );

		if(mv&0x02)LQTVQ_WriteAdSRiceLLb(ctx,
			dcy, &ctx->rk_cy , ctx->stat_yuvcbits+0);
		if(mv&0x04)LQTVQ_WriteAdSRiceLLb(ctx,
			dcu, &ctx->rk_cuv, ctx->stat_yuvcbits+1);
		if(mv&0x08)LQTVQ_WriteAdSRiceLLb(ctx,
			dcv, &ctx->rk_cuv, ctx->stat_yuvcbits+2);
		if(mv&0x10)LQTVQ_WriteAdSRiceLLb(ctx,
			ddy, &ctx->rk_dy , ctx->stat_yuvcbits+3);
	}else
	{
		ctx->yuv_cz[0]+=(dcy!=0)*2-1;
		ctx->yuv_cz[1]+=(dcu!=0)*2-1;
		ctx->yuv_cz[2]+=(dcv!=0)*2-1;
		ctx->yuv_cz[3]+=(ddy!=0)*2-1;

//		LQTVQ_WriteAdSRiceLL(ctx, dcy, &ctx->rk_cy );
//		LQTVQ_WriteAdSRiceLL(ctx, dcu, &ctx->rk_cuv);
//		LQTVQ_WriteAdSRiceLL(ctx, dcv, &ctx->rk_cuv);
//		LQTVQ_WriteAdSRiceLL(ctx, ddy, &ctx->rk_dy );

		LQTVQ_WriteAdSRiceLLb(ctx, dcy, &ctx->rk_cy , ctx->stat_yuvcbits+0);
		LQTVQ_WriteAdSRiceLLb(ctx, dcu, &ctx->rk_cuv, ctx->stat_yuvcbits+1);
		LQTVQ_WriteAdSRiceLLb(ctx, dcv, &ctx->rk_cuv, ctx->stat_yuvcbits+2);
		LQTVQ_WriteAdSRiceLLb(ctx, ddy, &ctx->rk_dy , ctx->stat_yuvcbits+3);
	}

	i1=ctx->stat_bits;
	ctx->stat_yuvbits+=i1-i0;
	ctx->stat_yuvcnt++;
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
	
#if 0
	if(	(ctx->cy<   0) || (ctx->cy>255) ||
		(ctx->cu<-255) || (ctx->cu>255) ||
		(ctx->cv<-255) || (ctx->cv>255) ||
		(ctx->dy<-255) || (ctx->dy>255))
	{
		__asm { int 3 }
	}
#endif
}

void LQTVQ_FastEncYUVDyuv(BT4A_Context *ctx,
	int cy, int cu, int cv,
	int dy, int du, int dv)
{
	int dcy, dcu, dcv, ddy, ddu, ddv;
	int adcy, adcu, adcv, addy, addu, addv;
	int i0, i1;
	int k, mv;
	
//	dcy=((cy-ctx->cy)*ctx->qfy  +128)>>8;
//	dcu=((cu-ctx->cu)*ctx->qfuv +128)>>8;
//	dcv=((cv-ctx->cv)*ctx->qfuv +128)>>8;
//	ddy=((dy-ctx->dy)*ctx->qfdy +128)>>8;
//	ddu=((du-ctx->du)*ctx->qfduv+128)>>8;
//	ddv=((dv-ctx->dv)*ctx->qfduv+128)>>8;

	dcy=((cy-ctx->cy)*ctx->qfy  +2048)>>12;
	dcu=((cu-ctx->cu)*ctx->qfuv +2048)>>12;
	dcv=((cv-ctx->cv)*ctx->qfuv +2048)>>12;
	ddy=((dy-ctx->dy)*ctx->qfdy +2048)>>12;
	ddu=((du-ctx->du)*ctx->qfduv+2048)>>12;
	ddv=((dv-ctx->dv)*ctx->qfduv+2048)>>12;

#ifndef LQTVQ_BYTES

	i0=ctx->stat_bits;

	if(ctx->cmask)
	{
		mv=0;
		if(dcy)mv|=0x02;
		if(dcu)mv|=0x04;
		if(dcv)mv|=0x08;
		if(ddy)mv|=0x10;
		if(ddu)mv|=0x20;
		if(ddv)mv|=0x40;

		ctx->yuv_cz[0]+=(dcy!=0)*2-1;
		ctx->yuv_cz[1]+=(dcu!=0)*2-1;
		ctx->yuv_cz[2]+=(dcv!=0)*2-1;
		ctx->yuv_cz[3]+=(ddy!=0)*2-1;
		ctx->yuv_cz[4]+=(ddu!=0)*2-1;
		ctx->yuv_cz[5]+=(ddv!=0)*2-1;

		mv=LQTVQ_EncEmitMask(ctx, mv, 0x7E);
//		if(mv&0x02)LQTVQ_WriteAdSRiceLL(ctx, dcy, &ctx->rk_cy );
//		if(mv&0x04)LQTVQ_WriteAdSRiceLL(ctx, dcu, &ctx->rk_cuv);
//		if(mv&0x08)LQTVQ_WriteAdSRiceLL(ctx, dcv, &ctx->rk_cuv);
//		if(mv&0x10)LQTVQ_WriteAdSRiceLL(ctx, ddy, &ctx->rk_dy );
//		if(mv&0x20)LQTVQ_WriteAdSRiceLL(ctx, ddu, &ctx->rk_duv);
//		if(mv&0x40)LQTVQ_WriteAdSRiceLL(ctx, ddv, &ctx->rk_duv);

		if(mv&0x02)LQTVQ_WriteAdSRiceLLb(ctx,
			dcy, &ctx->rk_cy , ctx->stat_yuvcbits+0);
		if(mv&0x04)LQTVQ_WriteAdSRiceLLb(ctx,
			dcu, &ctx->rk_cuv, ctx->stat_yuvcbits+1);
		if(mv&0x08)LQTVQ_WriteAdSRiceLLb(ctx,
			dcv, &ctx->rk_cuv, ctx->stat_yuvcbits+2);
		if(mv&0x10)LQTVQ_WriteAdSRiceLLb(ctx,
			ddy, &ctx->rk_dy , ctx->stat_yuvcbits+3);
		if(mv&0x20)LQTVQ_WriteAdSRiceLLb(ctx,
			ddu, &ctx->rk_duv, ctx->stat_yuvcbits+4);
		if(mv&0x40)LQTVQ_WriteAdSRiceLLb(ctx,
			ddv, &ctx->rk_duv, ctx->stat_yuvcbits+5);
	}else
	{
		ctx->yuv_cz[0]+=(dcy!=0)*2-1;
		ctx->yuv_cz[1]+=(dcu!=0)*2-1;
		ctx->yuv_cz[2]+=(dcv!=0)*2-1;
		ctx->yuv_cz[3]+=(ddy!=0)*2-1;
		ctx->yuv_cz[4]+=(ddu!=0)*2-1;
		ctx->yuv_cz[5]+=(ddv!=0)*2-1;

//		LQTVQ_WriteAdSRiceLL(ctx, dcy, &ctx->rk_cy );
//		LQTVQ_WriteAdSRiceLL(ctx, dcu, &ctx->rk_cuv);
//		LQTVQ_WriteAdSRiceLL(ctx, dcv, &ctx->rk_cuv);
//		LQTVQ_WriteAdSRiceLL(ctx, ddy, &ctx->rk_dy );
//		LQTVQ_WriteAdSRiceLL(ctx, ddu, &ctx->rk_duv);
//		LQTVQ_WriteAdSRiceLL(ctx, ddv, &ctx->rk_duv);

		LQTVQ_WriteAdSRiceLLb(ctx, dcy, &ctx->rk_cy , ctx->stat_yuvcbits+0);
		LQTVQ_WriteAdSRiceLLb(ctx, dcu, &ctx->rk_cuv, ctx->stat_yuvcbits+1);
		LQTVQ_WriteAdSRiceLLb(ctx, dcv, &ctx->rk_cuv, ctx->stat_yuvcbits+2);
		LQTVQ_WriteAdSRiceLLb(ctx, ddy, &ctx->rk_dy , ctx->stat_yuvcbits+3);
		LQTVQ_WriteAdSRiceLLb(ctx, ddu, &ctx->rk_duv, ctx->stat_yuvcbits+4);
		LQTVQ_WriteAdSRiceLLb(ctx, ddv, &ctx->rk_duv, ctx->stat_yuvcbits+5);
	}

	i1=ctx->stat_bits;
	ctx->stat_yuvbits+=i1-i0;
	ctx->stat_yuvcnt++;
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

#if 0
	if(	(ctx->cy<   0) || (ctx->cy>255) ||
		(ctx->cu<-255) || (ctx->cu>255) ||
		(ctx->cv<-255) || (ctx->cv>255) ||
		(ctx->dy<-255) || (ctx->dy>255) ||
		(ctx->du<-255) || (ctx->du>255) ||
		(ctx->dv<-255) || (ctx->dv>255))
	{
		__asm { int 3 }
	}
#endif
}


#if 1
void LQTVQ_FastEncY(BT4A_Context *ctx, int cy)
{
	int dcy, mv;
	int adcy;
	int i, j, k;
	
	dcy=((cy-ctx->cy)*ctx->qfy +2048)>>12;

#ifdef LQTVQ_BYTES
	adcy=(dcy<<1)^(dcy>>31);
	if(1)
	{
		mv=0;
		if(dcy)mv|=0x02;
		*ctx->ct++=mv;
		if(mv&0x02)LQTVQ_EncEmitGenericVal(ctx, dcy);
	}
#else
	if(ctx->cmask)
	{
		mv=0;
		if(dcy)mv|=0x02;
		mv=LQTVQ_EncEmitMask(ctx, mv, 0x02);
		if(mv&0x02)LQTVQ_WriteAdSRiceLL(ctx, dcy, &ctx->rk_cy );
	}else
	{
		LQTVQ_WriteAdSRiceLL(ctx, dcy, &ctx->rk_cy );
	}
#endif

	ctx->cy+=dcy*ctx->qy;
}

void LQTVQ_FastEncYD(BT4A_Context *ctx, int cy, int dy)
{
	int dcy, ddy, mv;
	int adcy, addy;
	int i, j, k;
	
	dcy=((cy-ctx->cy)*ctx->qfy +2048)>>12;
	ddy=((dy-ctx->dy)*ctx->qfdy+2048)>>12;

#ifdef LQTVQ_BYTES

	adcy=(dcy<<1)^(dcy>>31);
	addy=(ddy<<1)^(ddy>>31);
	k=adcy|addy;

#if 1
	if(!k)
	{
		*ctx->ct++=0;
	}else if(!(k>>7))
	{
		mv=(addy<<8)|(adcy<<1)|1;
		*ctx->ct++=mv;
		*ctx->ct++=mv>>8;
	}else
#endif
//	if(1)
	{
		mv=0;
		if(dcy)mv|=0x02;
		if(ddy)mv|=0x10;

		*ctx->ct++=mv;
		if(mv&0x02)LQTVQ_EncEmitGenericVal(ctx, dcy);
		if(mv&0x10)LQTVQ_EncEmitGenericVal(ctx, ddy);
	}

#else
	if(ctx->cmask)
	{
		mv=0;
		if(dcy)mv|=0x02;
		if(ddy)mv|=0x10;
	
		mv=LQTVQ_EncEmitMask(ctx, mv, 0x12);
		if(mv&0x02)LQTVQ_WriteAdSRiceLL(ctx, dcy, &ctx->rk_cy);
		if(mv&0x10)LQTVQ_WriteAdSRiceLL(ctx, ddy, &ctx->rk_dy);
	}else
	{
		LQTVQ_WriteAdSRiceLL(ctx, dcy, &ctx->rk_cy);
		LQTVQ_WriteAdSRiceLL(ctx, ddy, &ctx->rk_dy);
	}
#endif

	ctx->cy+=dcy*ctx->qy;
	ctx->dy+=ddy*ctx->qdy;
}

void LQTVQ_FastEncA(BT4A_Context *ctx, int cy)
{
	LQTVQ_FastEncY(ctx, cy);
}

void LQTVQ_FastEncAD(BT4A_Context *ctx, int cy, int dy)
{
	LQTVQ_FastEncYD(ctx, cy, dy);
}

#endif


#ifndef LQTVQ_BYTES

force_inline void LQTVQ_EncEmit4B(BT4A_Context *ctx, byte *buf)
{
	LQTVQ_WriteNBits(ctx, *buf, 4);
	ctx->stat_pixbits+=4;
}

force_inline void LQTVQ_EncEmit8B(BT4A_Context *ctx, byte *buf)
{
	LQTVQ_Write8BitsNM(ctx, *buf);
	ctx->stat_pixbits+=8;
}

force_inline void LQTVQ_EncEmit16B(BT4A_Context *ctx, byte *buf)
{
	LQTVQ_Write16BitsNM(ctx, *(u16 *)buf);
	ctx->stat_pixbits+=16;
}

force_inline void LQTVQ_EncEmit32B(BT4A_Context *ctx, byte *buf)
{
	LQTVQ_Write32Bits(ctx, *(u32 *)buf);
	ctx->stat_pixbits+=32;
}

force_inline void LQTVQ_EncEmit48B(BT4A_Context *ctx, byte *buf)
{
	LQTVQ_Write32Bits(ctx, *(u32 *)buf);
	LQTVQ_Write16BitsNM(ctx, *(u16 *)(buf+4));
	ctx->stat_pixbits+=48;
}

force_inline void LQTVQ_EncEmit64B(BT4A_Context *ctx, byte *buf)
{
	LQTVQ_Write64Bits(ctx, ((u64 *)buf)[0]);
	ctx->stat_pixbits+=64;
}

force_inline void LQTVQ_EncEmit96B(BT4A_Context *ctx, byte *buf)
{
	LQTVQ_Write64Bits(ctx, ((u64 *)buf)[0]);
	LQTVQ_Write32Bits(ctx, ((u32 *)buf)[2]);
	ctx->stat_pixbits+=96;
}

force_inline void LQTVQ_EncEmit128B(BT4A_Context *ctx, byte *buf)
{
	LQTVQ_Write64Bits(ctx, ((u64 *)buf)[0]);
	LQTVQ_Write64Bits(ctx, ((u64 *)buf)[1]);
	ctx->stat_pixbits+=128;
}

force_inline void LQTVQ_EncEmit192B(BT4A_Context *ctx, byte *buf)
{
	LQTVQ_Write64Bits(ctx, ((u64 *)buf)[0]);
	LQTVQ_Write64Bits(ctx, ((u64 *)buf)[1]);
	LQTVQ_Write64Bits(ctx, ((u64 *)buf)[2]);
	ctx->stat_pixbits+=191;
}

force_inline void LQTVQ_EncEmit256B(BT4A_Context *ctx, byte *buf)
{
	LQTVQ_Write64Bits(ctx, ((u64 *)buf)[0]);
	LQTVQ_Write64Bits(ctx, ((u64 *)buf)[1]);
	LQTVQ_Write64Bits(ctx, ((u64 *)buf)[2]);
	LQTVQ_Write64Bits(ctx, ((u64 *)buf)[3]);
	ctx->stat_pixbits+=256;
}

force_inline byte *LQTVQ_EncFlushBits(BT4A_Context *ctx)
{
	return(LQTVQ_EndWriteBits(ctx));
}

#else

force_inline void LQTVQ_EncEmit8B(BT4A_Context *ctx, byte *buf)
{
	*ctx->ct++=*buf;
}

force_inline void LQTVQ_EncEmit16B(BT4A_Context *ctx, byte *buf)
{
	*(u16 *)ctx->ct=*(u16 *)buf;
	ctx->ct+=2;
}

force_inline void LQTVQ_EncEmit32B(BT4A_Context *ctx, byte *buf)
{
	*(u32 *)ctx->ct=*(u32 *)buf;
	ctx->ct+=4;
}

force_inline void LQTVQ_EncEmit64B(BT4A_Context *ctx, byte *buf)
{
	*(u64 *)ctx->ct=*(u64 *)buf;
	ctx->ct+=8;
}

force_inline void LQTVQ_EncEmit96B(BT4A_Context *ctx, byte *buf)
{
	*(u64 *)ctx->ct=*(u64 *)buf;
	*(u32 *)ctx->ct=*(u32 *)buf;
	ctx->ct+=12;
}

force_inline void LQTVQ_EncEmit128B(BT4A_Context *ctx, byte *buf)
{
	((u64 *)ctx->ct)[0]=((u64 *)buf)[0];
	((u64 *)ctx->ct)[1]=((u64 *)buf)[1];
	ctx->ct+=16;
}

force_inline void LQTVQ_EncEmit192B(BT4A_Context *ctx, byte *buf)
{
	((u64 *)ctx->ct)[0]=((u64 *)buf)[0];
	((u64 *)ctx->ct)[1]=((u64 *)buf)[1];
	((u64 *)ctx->ct)[2]=((u64 *)buf)[2];
	ctx->ct+=24;
}

force_inline void LQTVQ_EncEmit256B(BT4A_Context *ctx, byte *buf)
{
	((u64 *)ctx->ct)[0]=((u64 *)buf)[0];
	((u64 *)ctx->ct)[1]=((u64 *)buf)[1];
	((u64 *)ctx->ct)[2]=((u64 *)buf)[2];
	((u64 *)ctx->ct)[3]=((u64 *)buf)[3];
	ctx->ct+=32;
}

force_inline byte *LQTVQ_EncFlushBits(BT4A_Context *ctx)
{
	return(ctx->ct);
}

#endif

int LQTVQ_SetupContextQf(BT4A_Context *ctx, int qf)
{
	float qsc, qsc15;
	
	qsc=(100-(qf&127))/25.0;
	qsc15=pow(qsc, 1.5);
	
	qsc*=0.5;
	qsc15*=0.5;
	
	ctx->qfl=qf;
	
	if(!ctx->blksz)
		ctx->blksz=64;
	
//	ctx->qdy_flat=16*qsc;
//	ctx->qdy_flat=20*qsc;
//	ctx->qdy_2x2x2=32*qsc;

	ctx->qdy_flat=32*qsc;
	ctx->qdy_2x2x2=44*qsc;

//	ctx->qdy_4x4x2=48*qsc;
//	ctx->qdy_4x4x2=64*qsc;
//	ctx->qdy_4x4x2=64*qsc15;

	ctx->qdy_4x4x2=80*qsc15;

//	ctx->qdy_4x4x2=56*qsc;
//	ctx->qdy_8x8x2=80*qsc;
//	ctx->qdy_8x8x2=96*qsc;
//	ctx->qdy_8x8x2=112*qsc;
	ctx->qdy_8x8x2=112*qsc15;

//	ctx->qdy_8x8x3=192*qsc;
//	ctx->qdy_8x8x2=128*qsc;
	ctx->qdy_8x8x3=224*qsc15;

//	ctx->qduv_flat=16*qsc;
//	ctx->qduv_flat=32*qsc;
//	ctx->qduv_flat=40*qsc;
//	ctx->qduv_flat=48*qsc;
	ctx->qduv_flat=64*qsc;
	ctx->qduv_2x2=96*qsc;

	ctx->qdce_sc=16*(1.6/qsc15);
//	ctx->qdce_sc=20*(1.6/qsc15);

	ctx->rk_cy=2;
	ctx->rk_cuv=2;
	ctx->rk_dy=2;
	ctx->rk_duv=2;
	ctx->rk_cnt=2;
	ctx->rk_misc=2;

//	ctx->qy=10*qsc;
	ctx->qy=8*qsc;
//	ctx->qy=6*qsc;

//	ctx->quv=8*qsc;
	ctx->quv=10*qsc;
//	ctx->quv=16*qsc;

//	ctx->qdy=6*qsc;
//	ctx->qdy=10*qsc;
//	ctx->qduv=12*qsc;

	ctx->qdy=12*qsc;
	ctx->qduv=14*qsc;

//	ctx->qduv=8*qsc;
//	ctx->qduv=16*qsc;
	if(ctx->qy<1)ctx->qy=1;
	if(ctx->quv<1)ctx->quv=1;
	if(ctx->qdy<1)ctx->qdy=1;
	if(ctx->qduv<1)ctx->qduv=1;

//	ctx->qfy=256/ctx->qy;
//	ctx->qfuv=256/ctx->quv;
//	ctx->qfdy=256/ctx->qdy;
//	ctx->qfduv=256/ctx->qduv;

	ctx->qfy=4096/ctx->qy;
	ctx->qfuv=4096/ctx->quv;
	ctx->qfdy=4096/ctx->qdy;
	ctx->qfduv=4096/ctx->qduv;
	
	ctx->cy=0;	ctx->cu=0;	ctx->cv=0;
	ctx->dy=0;	ctx->du=0;	ctx->dv=0;
	ctx->cmask=0;
	ctx->imask=0;
	
	LQTVQ_ResetSmtfDefault(ctx, &(ctx->sm_cmd));
	LQTVQ_ResetSmtfReverse(ctx, &(ctx->sm_mask));

	return(0);
}

void LQTVQ_EncUpdatePredV(BT4A_Context *ctx,
	byte *ct, byte *blks, int *rpyc);

int LQTVQ_EncBlockPriorColorP(BT4A_Context *ctx,
	byte *blk, byte *blks)
{
	int pyc[6];
	int dcy, dcu, dcv, ddy, ddu, ddv;
	int adcy, adcu, adcv, addy, addu, addv;
	int cy, cu, cv, dy, du, dv;
	int k, mv, bt;

//	if(sblk[0]!=dblk[0])
//		return(0);

	LQTVQ_EncUpdatePredV(ctx, blk, blks, pyc);

	bt=blk[0];
	cy=*(s16 *)(blk+ 4);
	cu=*(s16 *)(blk+ 6);
	cv=*(s16 *)(blk+ 8);
	dy=*(s16 *)(blk+10);
	du=*(s16 *)(blk+12);
	dv=*(s16 *)(blk+14);

//	dcy=((cy-ctx->cy)*ctx->qfy  +2048)>>12;
//	dcu=((cu-ctx->cu)*ctx->qfuv +2048)>>12;
//	dcv=((cv-ctx->cv)*ctx->qfuv +2048)>>12;
//	ddy=((dy-ctx->dy)*ctx->qfdy +2048)>>12;
//	ddu=((du-ctx->du)*ctx->qfduv+2048)>>12;
//	ddv=((dv-ctx->dv)*ctx->qfduv+2048)>>12;

	dcy=((cy-pyc[0])*ctx->qfy  +2048)>>12;
	dcu=((cu-pyc[1])*ctx->qfuv +2048)>>12;
	dcv=((cv-pyc[2])*ctx->qfuv +2048)>>12;
	ddy=((dy-pyc[3])*ctx->qfdy +2048)>>12;
	ddu=((du-pyc[4])*ctx->qfduv+2048)>>12;
	ddv=((dv-pyc[5])*ctx->qfduv+2048)>>12;

	if(dcy|dcu|dcv)
		return(0);
	
	if(bt>0)
	{
		if(ddy)
			return(0);
		if((bt>=0x15) && (ddu|ddv))
			return(0);
	}
	return(1);

}

int LQTVQ_EncEmitDbgCmdPfx(BT4A_Context *ctx)
{
//	LQTVQ_EncEmitCommand(ctx, 0x4C);	//Debug
}

void LQTVQ_DecUpdateCtxPred(BT4A_Context *ctx,
	byte *pba, byte *pbb, byte *pbc, byte pred);
void LQTVQ_DecUpdateCtxPredV(BT4A_Context *ctx,
	byte *pba, byte *pbb, byte *pbc, byte pred,
	int *rpyc);

force_inline void LQTVQ_EncUpdatePred(BT4A_Context *ctx,
	byte *ct, byte *blks)
{
	byte *ctpa, *ctpb, *ctpc;
	int xs1;

	if(!ctx->pred)
		return;

	xs1=ctx->xsb;
	ctpa=ct-(xs1+1)*ctx->blksz;
	if(ctpa>=blks)
	{
		ctpb=ct-xs1*ctx->blksz;
		ctpc=ct-ctx->blksz;
		LQTVQ_DecUpdateCtxPred(ctx,
			ctpa, ctpb, ctpc, ctx->pred);
	}
}

void LQTVQ_EncUpdatePredV(BT4A_Context *ctx,
	byte *ct, byte *blks, int *rpyc)
{
	byte *ctpa, *ctpb, *ctpc;
	int xs1;

	if(!ctx->pred)
	{
		rpyc[0]=ctx->cy;
		rpyc[1]=ctx->cu;
		rpyc[2]=ctx->cv;
		rpyc[3]=ctx->dy;
		rpyc[4]=ctx->du;
		rpyc[5]=ctx->dv;
		return;
	}

	xs1=ctx->xsb;
	ctpa=ct-(xs1+1)*ctx->blksz;
	if(ctpa>=blks)
	{
		ctpb=ct-xs1*ctx->blksz;
		ctpc=ct-ctx->blksz;
		LQTVQ_DecUpdateCtxPredV(ctx,
			ctpa, ctpb, ctpc, ctx->pred, rpyc);
	}
}

force_inline void LQTVQ_EncUpdatePredPost(BT4A_Context *ctx,
	byte *blk, byte *blks)
{
	if(!ctx->pred)
		return;
		
//	return;

	if(((blk[0]&0x3F)==0x13) ||
		((blk[0]&0x3F)==0x19))
	{
		*(blk+2)=lqtvq_clamp255(ctx->cy);
		*(blk+5)=lqtvq_clamp255(ctx->dy);
		*(blk+3)=lqtvq_clamp255((ctx->cu>>1)+128);
		*(blk+4)=lqtvq_clamp255((ctx->cv>>1)+128);
		*(blk+6)=lqtvq_clamp255((ctx->du>>1)+128);
		*(blk+7)=lqtvq_clamp255((ctx->dv>>1)+128);
		return;
	}

	*(s16 *)(blk+ 4)=lqtvq_clamp32767S(ctx->cy);
	*(s16 *)(blk+ 6)=lqtvq_clamp32767S(ctx->cu);
	*(s16 *)(blk+ 8)=lqtvq_clamp32767S(ctx->cv);
	*(s16 *)(blk+10)=lqtvq_clamp32767S(ctx->dy);
	*(s16 *)(blk+12)=lqtvq_clamp32767S(ctx->du);
	*(s16 *)(blk+14)=lqtvq_clamp32767S(ctx->dv);
}

int LQTVQ_EncImgBlocks(BT4A_Context *ctx,
	byte *cbuf, byte *blks, byte *lblks, int xs, int ys, int qf)
{
	byte *cs, *cs1, *cse, *csl;
	byte *blkb;
	int xs1, ys1;
	int bi, bc, bc2;
	int i, j, k, n;
	
	LQTVQ_SetupContextQf(ctx, qf);

#ifdef LQTVQ_BYTES
	ctx->ct=cbuf;
#else
	LQTVQ_InitScTables();
	LQTVQ_InitRice();
	LQTVQ_SetupWriteBits(ctx, cbuf, xs*ys*8);
#endif

	xs1=(xs+7)>>3;
	ys1=(ys+7)>>3;
	n=xs1*ys1;
	
	LQTVQ_EncEmitDbgCmdPfx(ctx);	//Debug
	LQTVQ_EncEmitCommand(ctx, 0x45);
	LQTVQ_EncEmitGenericUVal(ctx, ctx->qy);
	LQTVQ_EncEmitGenericUVal(ctx, ctx->quv);
	LQTVQ_EncEmitGenericUVal(ctx, ctx->qdy);
	LQTVQ_EncEmitGenericUVal(ctx, ctx->qduv);

	if((qf&127)<50)
		LQTVQ_EmitSetParm(ctx, -2, 1);
	
	cs=blks;
	csl=lblks;
	
	cse=cs+n*64;
	
	while(cs<cse)
	{
		blkb=cs;
		cs+=64;

		cs1=cs; bc=1;
		while((cs1<cse) && (cs1[0]==blkb[0]))
			{ cs1+=64; bc2++; }

		cs1=blkb; bc2=0;
		while((cs1<cse) && (cs1[0]==blkb[0]))
		{
			if(!LQTVQ_EncBlockPriorColorP(ctx, cs1, blks))
				break;
			cs1+=64; bc2++;
		}
		
//		bc2=0;

		LQTVQ_EncCheckEnableMask(ctx);
		LQTVQ_EncEmitDbgCmdPfx(ctx);	//Debug

//		switch(*cs)
		switch(blkb[0])
		{
		case 0x00:
#if 1
			if(bc2>2)
			{
				LQTVQ_EncEmitCommand(ctx, 0x20);
				LQTVQ_EncEmitCount(ctx, bc2);
//				cs=blkb+(bc2*64);
				cs=blkb;
				for(i=0; i<bc2; i++)
				{
					LQTVQ_EncUpdatePred(ctx, cs, blks);
					LQTVQ_EncUpdatePredPost(ctx, cs, blks);
					cs+=64;
				}
				break;
			}
			if(bc>1)
			{
				LQTVQ_EncEmitCommand(ctx, 0x48);
				LQTVQ_EncEmitCount(ctx, bc);
				
				cs=blkb;
				for(i=0; i<bc; i++)
				{
					LQTVQ_EncUpdatePred(ctx, cs, blks);
					LQTVQ_FastEncYUV(ctx,
						*(s16 *)(cs+ 4), *(s16 *)(cs+ 6),
						*(s16 *)(cs+ 8));
					LQTVQ_EncUpdatePredPost(ctx, cs, blks);
					cs+=64;
				}
				break;
			}
#endif
			
			LQTVQ_EncEmitCommand(ctx, 0x00);
			LQTVQ_EncUpdatePred(ctx, blkb, blks);
			LQTVQ_FastEncYUV(ctx,
				*(s16 *)(blkb+ 4), *(s16 *)(blkb+ 6), *(s16 *)(blkb+ 8));
			LQTVQ_EncUpdatePredPost(ctx, blkb, blks);
			break;

		case 0x01:
#if 1
			if(bc2>4)
			{
				LQTVQ_EncEmitCommand(ctx, 0x21);
				LQTVQ_EncEmitCount(ctx, bc2);
				cs=blkb;
				for(i=0; i<bc2; i++)
				{
					LQTVQ_EncUpdatePred(ctx, cs, blks);
					LQTVQ_EncEmit4B(ctx, cs+16);
					LQTVQ_EncUpdatePredPost(ctx, cs, blks);
					cs+=64;
				}
				break;
			}
#endif
			LQTVQ_EncEmitCommand(ctx, 0x01);
			LQTVQ_EncUpdatePred(ctx, blkb, blks);
			LQTVQ_FastEncYUVD(ctx,
				*(s16 *)(blkb+ 4), *(s16 *)(blkb+ 6),
				*(s16 *)(blkb+ 8), *(s16 *)(blkb+10));
			LQTVQ_EncEmit4B(ctx, blkb+16);
			LQTVQ_EncUpdatePredPost(ctx, blkb, blks);
			break;
		case 0x02:	case 0x03:
#if 1
			if(bc2>4)
			{
				LQTVQ_EncEmitCommand(ctx, 0x20|blkb[0]);
				LQTVQ_EncEmitCount(ctx, bc2);
				cs=blkb;
				for(i=0; i<bc2; i++)
				{
					LQTVQ_EncUpdatePred(ctx, cs, blks);
					LQTVQ_EncEmit8B(ctx, cs+16);
					LQTVQ_EncUpdatePredPost(ctx, cs, blks);
					cs+=64;
				}
				break;
			}
#endif
			LQTVQ_EncEmitCommand(ctx, blkb[0]);
			LQTVQ_EncUpdatePred(ctx, blkb, blks);
			LQTVQ_FastEncYUVD(ctx,
				*(s16 *)(blkb+ 4), *(s16 *)(blkb+ 6),
				*(s16 *)(blkb+ 8), *(s16 *)(blkb+10));
			LQTVQ_EncEmit8B(ctx, blkb+16);
			LQTVQ_EncUpdatePredPost(ctx, blkb, blks);
			break;
		case 0x04:
#if 1
			if(bc2>4)
			{
				LQTVQ_EncEmitCommand(ctx, 0x24);
				LQTVQ_EncEmitCount(ctx, bc2);
				cs=blkb;
				for(i=0; i<bc2; i++)
				{
					LQTVQ_EncUpdatePred(ctx, cs, blks);
					LQTVQ_EncEmit16B(ctx, cs+16);
					LQTVQ_EncUpdatePredPost(ctx, cs, blks);
					cs+=64;
				}
				break;
			}
#endif
			LQTVQ_EncEmitCommand(ctx, 0x04);
			LQTVQ_EncUpdatePred(ctx, blkb, blks);
			LQTVQ_FastEncYUVD(ctx,
				*(s16 *)(blkb+ 4), *(s16 *)(blkb+ 6),
				*(s16 *)(blkb+ 8), *(s16 *)(blkb+10));
			LQTVQ_EncEmit16B(ctx, blkb+16);
			LQTVQ_EncUpdatePredPost(ctx, blkb, blks);
			break;
		case 0x05:	case 0x06:
#if 1
			if(bc2>4)
			{
				LQTVQ_EncEmitCommand(ctx, 0x20|blkb[0]);
				LQTVQ_EncEmitCount(ctx, bc2);
				cs=blkb;
				for(i=0; i<bc2; i++)
				{
					LQTVQ_EncUpdatePred(ctx, cs, blks);
					LQTVQ_EncEmit32B(ctx, cs+16);
					LQTVQ_EncUpdatePredPost(ctx, cs, blks);
					cs+=64;
				}
				break;
			}
#endif
			LQTVQ_EncEmitCommand(ctx, blkb[0]);
			LQTVQ_EncUpdatePred(ctx, blkb, blks);
			LQTVQ_FastEncYUVD(ctx,
				*(s16 *)(blkb+ 4), *(s16 *)(blkb+ 6),
				*(s16 *)(blkb+ 8), *(s16 *)(blkb+10));
			LQTVQ_EncEmit32B(ctx, blkb+16);
			LQTVQ_EncUpdatePredPost(ctx, blkb, blks);
			break;
		case 0x07:
#if 1
			if(bc2>4)
			{
				LQTVQ_EncEmitCommand(ctx, 0x27);
				LQTVQ_EncEmitCount(ctx, bc2);
				cs=blkb;
				for(i=0; i<bc2; i++)
				{
					LQTVQ_EncUpdatePred(ctx, cs, blks);
					LQTVQ_EncEmit64B(ctx, cs+16);
					LQTVQ_EncUpdatePredPost(ctx, cs, blks);
					cs+=64;
				}
				break;
			}
#endif
			LQTVQ_EncEmitCommand(ctx, 0x07);
			LQTVQ_EncUpdatePred(ctx, blkb, blks);
			LQTVQ_FastEncYUVD(ctx,
				*(s16 *)(blkb+ 4), *(s16 *)(blkb+ 6),
				*(s16 *)(blkb+ 8), *(s16 *)(blkb+10));
			LQTVQ_EncEmit64B(ctx, blkb+16);
			LQTVQ_EncUpdatePredPost(ctx, blkb, blks);
			break;

		case 0x09:
#if 1
			if(bc2>3)
			{
				LQTVQ_EncEmitCommand(ctx, 0x29);
				LQTVQ_EncEmitCount(ctx, bc2);
				
				cs=blkb;
				for(i=0; i<bc2; i++)
				{
					LQTVQ_EncUpdatePred(ctx, cs, blks);
					LQTVQ_EncEmit8B(ctx, cs+16);
					LQTVQ_EncUpdatePredPost(ctx, cs, blks);
					cs+=64;
				}
				break;
			}
#endif
			LQTVQ_EncEmitCommand(ctx, 0x09);
			LQTVQ_EncUpdatePred(ctx, blkb, blks);
			LQTVQ_FastEncYUVD(ctx,
				*(s16 *)(blkb+ 4), *(s16 *)(blkb+ 6),
				*(s16 *)(blkb+ 8), *(s16 *)(blkb+10));
			LQTVQ_EncEmit8B(ctx, blkb+16);
			LQTVQ_EncUpdatePredPost(ctx, blkb, blks);
			break;
		case 0x0A:	case 0x0B:
#if 1
			if(bc2>4)
			{
				LQTVQ_EncEmitCommand(ctx, 0x20|blkb[0]);
				LQTVQ_EncEmitCount(ctx, bc2);
				cs=blkb;
				for(i=0; i<bc2; i++)
				{
					LQTVQ_EncUpdatePred(ctx, cs, blks);
					LQTVQ_EncEmit16B(ctx, cs+16);
					LQTVQ_EncUpdatePredPost(ctx, cs, blks);
					cs+=64;
				}
				break;
			}
#endif
			LQTVQ_EncEmitCommand(ctx, blkb[0]);
			LQTVQ_EncUpdatePred(ctx, blkb, blks);
			LQTVQ_FastEncYUVD(ctx,
				*(s16 *)(blkb+ 4), *(s16 *)(blkb+ 6),
				*(s16 *)(blkb+ 8), *(s16 *)(blkb+10));
			LQTVQ_EncEmit16B(ctx, blkb+16);
			LQTVQ_EncUpdatePredPost(ctx, blkb, blks);
			break;
		case 0x0C:
#if 1
			if(bc2>4)
			{
				LQTVQ_EncEmitCommand(ctx, 0x20|blkb[0]);
				LQTVQ_EncEmitCount(ctx, bc2);
				cs=blkb;
				for(i=0; i<bc2; i++)
				{
					LQTVQ_EncUpdatePred(ctx, cs, blks);
					LQTVQ_EncEmit32B(ctx, cs+16);
					LQTVQ_EncUpdatePredPost(ctx, cs, blks);
					cs+=64;
				}
				break;
			}
#endif
			LQTVQ_EncEmitCommand(ctx, 0x0C);
			LQTVQ_EncUpdatePred(ctx, blkb, blks);
			LQTVQ_FastEncYUVD(ctx,
				*(s16 *)(blkb+ 4), *(s16 *)(blkb+ 6),
				*(s16 *)(blkb+ 8), *(s16 *)(blkb+10));
			LQTVQ_EncEmit32B(ctx, blkb+16);
			LQTVQ_EncUpdatePredPost(ctx, blkb, blks);
			break;

		case 0x0D:	case 0x0E:
#if 1
			if(bc2>4)
			{
				LQTVQ_EncEmitCommand(ctx, 0x20|blkb[0]);
				LQTVQ_EncEmitCount(ctx, bc2);
				cs=blkb;
				for(i=0; i<bc2; i++)
				{
					LQTVQ_EncUpdatePred(ctx, cs, blks);
					LQTVQ_EncEmit64B(ctx, cs+16);
					LQTVQ_EncUpdatePredPost(ctx, cs, blks);
					cs+=64;
				}
				break;
			}
#endif
			LQTVQ_EncEmitCommand(ctx, blkb[0]);
			LQTVQ_EncUpdatePred(ctx, blkb, blks);
			LQTVQ_FastEncYUVD(ctx,
				*(s16 *)(blkb+ 4), *(s16 *)(blkb+ 6),
				*(s16 *)(blkb+ 8), *(s16 *)(blkb+10));
			LQTVQ_EncEmit64B(ctx, blkb+16);
			LQTVQ_EncUpdatePredPost(ctx, blkb, blks);
			break;
		case 0x0F:
			LQTVQ_EncEmitCommand(ctx, 0x0F);
			LQTVQ_EncUpdatePred(ctx, blkb, blks);
			LQTVQ_FastEncYUVD(ctx,
				*(s16 *)(blkb+ 4), *(s16 *)(blkb+ 6),
				*(s16 *)(blkb+ 8), *(s16 *)(blkb+10));
			LQTVQ_EncEmit128B(ctx, blkb+16);
			LQTVQ_EncUpdatePredPost(ctx, blkb, blks);
			break;
		case 0x10:
			LQTVQ_EncEmitCommand(ctx, blkb[0]);
			LQTVQ_EncUpdatePred(ctx, blkb, blks);
			LQTVQ_FastEncYUVD(ctx,
				*(s16 *)(blkb+ 4), *(s16 *)(blkb+ 6),
				*(s16 *)(blkb+ 8), *(s16 *)(blkb+10));
			LQTVQ_EncEmit192B(ctx, blkb+16);
			LQTVQ_EncUpdatePredPost(ctx, blkb, blks);
			break;
		case 0x11:
			LQTVQ_EncEmitCommand(ctx, blkb[0]);
			LQTVQ_EncUpdatePred(ctx, blkb, blks);
			LQTVQ_FastEncYUVD(ctx,
				*(s16 *)(blkb+ 4), *(s16 *)(blkb+ 6),
				*(s16 *)(blkb+ 8), *(s16 *)(blkb+10));
			LQTVQ_EncEmit256B(ctx, blkb+16);
			LQTVQ_EncUpdatePredPost(ctx, blkb, blks);
			break;

		case 0x14:
			LQTVQ_EncEmitCommand(ctx, 0x14);
			LQTVQ_EncUpdatePred(ctx, blkb, blks);
			LQTVQ_FastEncYUVDyuv(ctx,
				*(s16 *)(blkb+ 4), *(s16 *)(blkb+ 6), *(s16 *)(blkb+ 8),
				*(s16 *)(blkb+10), *(s16 *)(blkb+12), *(s16 *)(blkb+14));
			LQTVQ_EncEmit16B(ctx, blkb+16);
			LQTVQ_EncUpdatePredPost(ctx, blkb, blks);
			break;
		case 0x15:
			LQTVQ_EncEmitCommand(ctx, 0x15);
			LQTVQ_EncUpdatePred(ctx, blkb, blks);
			LQTVQ_FastEncYUVDyuv(ctx,
				*(s16 *)(blkb+ 4), *(s16 *)(blkb+ 6), *(s16 *)(blkb+ 8),
				*(s16 *)(blkb+10), *(s16 *)(blkb+12), *(s16 *)(blkb+14));
			LQTVQ_EncEmit32B(ctx, blkb+16);
			LQTVQ_EncEmit8B(ctx, blkb+20);
			LQTVQ_EncEmit8B(ctx, blkb+21);
			LQTVQ_EncUpdatePredPost(ctx, blkb, blks);
			break;
		case 0x16:
			LQTVQ_EncEmitCommand(ctx, 0x16);
			LQTVQ_EncUpdatePred(ctx, blkb, blks);
			LQTVQ_FastEncYUVDyuv(ctx,
				*(s16 *)(blkb+ 4), *(s16 *)(blkb+ 6), *(s16 *)(blkb+ 8),
				*(s16 *)(blkb+10), *(s16 *)(blkb+12), *(s16 *)(blkb+14));
			LQTVQ_EncEmit32B(ctx, blkb+16);
			LQTVQ_EncEmit32B(ctx, blkb+20);
			LQTVQ_EncEmit32B(ctx, blkb+24);
			LQTVQ_EncUpdatePredPost(ctx, blkb, blks);
			break;
		case 0x17:
			LQTVQ_EncEmitCommand(ctx, 0x17);
			LQTVQ_EncUpdatePred(ctx, blkb, blks);
			LQTVQ_FastEncYUVDyuv(ctx,
				*(s16 *)(blkb+ 4), *(s16 *)(blkb+ 6), *(s16 *)(blkb+ 8),
				*(s16 *)(blkb+10), *(s16 *)(blkb+12), *(s16 *)(blkb+14));
			LQTVQ_EncEmit128B(ctx, blkb+16);
			LQTVQ_EncEmit8B(ctx, blkb+32);
			LQTVQ_EncEmit8B(ctx, blkb+33);
			LQTVQ_EncUpdatePredPost(ctx, blkb, blks);
			break;
		case 0x18:
			LQTVQ_EncEmitCommand(ctx, 0x18);
			LQTVQ_EncUpdatePred(ctx, blkb, blks);
			LQTVQ_FastEncYUVDyuv(ctx,
				*(s16 *)(blkb+ 4), *(s16 *)(blkb+ 6), *(s16 *)(blkb+ 8),
				*(s16 *)(blkb+10), *(s16 *)(blkb+12), *(s16 *)(blkb+14));
			LQTVQ_EncEmit128B(ctx, blkb+16);
			LQTVQ_EncEmit32B(ctx, blkb+32);
			LQTVQ_EncEmit32B(ctx, blkb+36);
			LQTVQ_EncUpdatePredPost(ctx, blkb, blks);
			break;
		case 0x19:
			__asm { int 3 }
			break;
		case 0x1A:
			LQTVQ_EncEmitCommand(ctx, 0x1A);
			LQTVQ_EncUpdatePred(ctx, blkb, blks);
			LQTVQ_FastEncYUVDyuv(ctx,
				*(s16 *)(blkb+ 4), *(s16 *)(blkb+ 6), *(s16 *)(blkb+ 8),
				*(s16 *)(blkb+10), *(s16 *)(blkb+12), *(s16 *)(blkb+14));
			LQTVQ_EncEmit128B(ctx, blkb+16);
			LQTVQ_EncEmit64B(ctx, blkb+32);
			LQTVQ_EncEmit64B(ctx, blkb+40);
			LQTVQ_EncUpdatePredPost(ctx, blkb, blks);
			break;
		case 0x1B:
			LQTVQ_EncEmitCommand(ctx, 0x1B);
			LQTVQ_EncUpdatePred(ctx, blkb, blks);
			LQTVQ_FastEncYUVDyuv(ctx,
				*(s16 *)(blkb+ 4), *(s16 *)(blkb+ 6), *(s16 *)(blkb+ 8),
				*(s16 *)(blkb+10), *(s16 *)(blkb+12), *(s16 *)(blkb+14));
			LQTVQ_EncEmit128B(ctx, blkb+16);
			LQTVQ_EncEmit128B(ctx, blkb+32);
			LQTVQ_EncEmit128B(ctx, blkb+48);
			LQTVQ_EncUpdatePredPost(ctx, blkb, blks);
			break;
		case 0x1C:
			LQTVQ_EncEmitCommand(ctx, 0x1C);
			LQTVQ_EncUpdatePred(ctx, blkb, blks);
			LQTVQ_FastEncYUVDyuv(ctx,
				*(s16 *)(blkb+ 4), *(s16 *)(blkb+ 6), *(s16 *)(blkb+ 8),
				*(s16 *)(blkb+10), *(s16 *)(blkb+12), *(s16 *)(blkb+14));
			LQTVQ_EncEmit192B(ctx, blkb+16);
			LQTVQ_EncEmit32B(ctx, blkb+40);
			LQTVQ_EncEmit32B(ctx, blkb+44);
			LQTVQ_EncUpdatePredPost(ctx, blkb, blks);
			break;
		case 0x1D:
			LQTVQ_EncEmitCommand(ctx, 0x1D);
			LQTVQ_EncUpdatePred(ctx, blkb, blks);
			LQTVQ_FastEncYUVDyuv(ctx,
				*(s16 *)(blkb+ 4), *(s16 *)(blkb+ 6), *(s16 *)(blkb+ 8),
				*(s16 *)(blkb+10), *(s16 *)(blkb+12), *(s16 *)(blkb+14));
			LQTVQ_EncEmit256B(ctx, blkb+16);
			LQTVQ_EncEmit64B(ctx, blkb+48);
			LQTVQ_EncEmit64B(ctx, blkb+56);
			LQTVQ_EncUpdatePredPost(ctx, blkb, blks);
			break;
		default:
			__asm { int 3 }
//			*(int *)-1=-1;
			break;
		}
	}

	LQTVQ_EncEmitDbgCmdPfx(ctx);
	LQTVQ_EncEmitCommand(ctx, 0x40);
	
	blkb=LQTVQ_EncFlushBits(ctx);
	return(blkb-cbuf);
}


#if 1
int LQTVQ_EncImgAlphaBlocks(BT4A_Context *ctx,
	byte *cbuf, byte *blks, byte *lblks, int xs, int ys, int qf)
{
	byte *cs, *cs1, *cse, *csl;
	byte *blkb;
	int xs1, ys1;
	int bi, bc, bc2;
	int i, j, k, n;

	xs1=(xs+7)>>3;
	ys1=(ys+7)>>3;
	n=xs1*ys1;

#if 1
	cs=blks;
	cse=cs+n*64;

	cs1=cs; bc=0;
	while(cs1<cse)
	{
		if((*(u16 *)(cs1+2))!=0x00FF)
			break;
		cs1+=64;
	}
	if(cs1>=cse)
		return(0);
#endif

	LQTVQ_SetupContextQf(ctx, qf);

#ifdef LQTVQ_BYTES
	ctx->ct=cbuf;
#else
	LQTVQ_SetupWriteBits(ctx, cbuf, xs*ys*8);
#endif

	
	LQTVQ_EncEmitDbgCmdPfx(ctx);	//Debug

	LQTVQ_EncEmitCommand(ctx, 0x45);
	LQTVQ_EncEmitGenericUVal(ctx, ctx->qy);
	LQTVQ_EncEmitGenericUVal(ctx, ctx->qdy);
	
	cs=blks;
	csl=lblks;
	
	cse=cs+n*64;
	
	while(cs<cse)
	{
		blkb=cs;
		cs+=64;

		LQTVQ_EncEmitDbgCmdPfx(ctx);	//Debug

		cs1=blkb; bc=0;
		while(cs1<cse)
		{
			if((*(u16 *)(cs1+2))!=0x00FF)
				break;
			cs1+=64; bc++;
		}
		
		if(cs1>=cse)
			break;

		if(bc)
		{
			LQTVQ_EncEmitCommand(ctx, 0x41);
			LQTVQ_EncEmitCount(ctx, bc);
			cs=blkb+(bc*64);
			continue;
		}

//		switch(*cs)
		switch(blkb[1])
		{
		case 0x00:
			LQTVQ_EncEmitCommand(ctx, 0x00);
			LQTVQ_FastEncA(ctx, blkb[2]);
			break;
		case 0x04:
			LQTVQ_EncEmitCommand(ctx, 0x04);
			LQTVQ_FastEncAD(ctx,
				*(s16 *)(blkb+ 48+0),
				*(s16 *)(blkb+ 48+2));
			LQTVQ_EncEmit8B(ctx, blkb+48+8);
			break;
		case 0x05:
			LQTVQ_EncEmitCommand(ctx, 0x05);
			LQTVQ_FastEncAD(ctx,
				*(s16 *)(blkb+ 48+0),
				*(s16 *)(blkb+ 48+2));
			LQTVQ_EncEmit32B(ctx, blkb+48+8);
			break;
		case 0x06:
			LQTVQ_EncEmitCommand(ctx, 0x06);
			LQTVQ_FastEncAD(ctx,
				*(s16 *)(blkb+ 48+0),
				*(s16 *)(blkb+ 48+2));
			LQTVQ_EncEmit48B(ctx, blkb+48+8);
			break;
		case 0x07:
			LQTVQ_EncEmitCommand(ctx, 0x07);
			LQTVQ_FastEncAD(ctx,
				*(s16 *)(blkb+ 48+0),
				*(s16 *)(blkb+ 48+2));
			LQTVQ_EncEmit64B(ctx, blkb+48+8);
			break;
		case 0x08:
			LQTVQ_EncEmitCommand(ctx, 0x08);
			LQTVQ_FastEncA(ctx, *(s16 *)(blkb+48+0));
			break;


		case 0x0A:	case 0x0B:
			LQTVQ_EncEmitCommand(ctx, blkb[0]);
			LQTVQ_FastEncAD(ctx,
				*(s16 *)(blkb+48+0), *(s16 *)(blkb+48+2));
			LQTVQ_EncEmit64B(ctx, blkb+48+8);
			break;

		case 0x0C:
			LQTVQ_EncEmitCommand(ctx, 0x0C);
			LQTVQ_FastEncAD(ctx, blkb[2], blkb[3]);
			LQTVQ_EncEmit128B(ctx, blkb+48);
			break;

		case 0x0D:
			LQTVQ_EncEmitCommand(ctx, 0x0D);
			LQTVQ_FastEncAD(ctx,
				*(s16 *)(blkb+64+0), *(s16 *)(blkb+64+2));
			LQTVQ_EncEmit128B(ctx, blkb+64+8);
			break;
		case 0x0E:
			LQTVQ_EncEmitCommand(ctx, 0x0E);
			LQTVQ_FastEncAD(ctx,
				*(s16 *)(blkb+64+0), *(s16 *)(blkb+64+2));
			LQTVQ_EncEmit192B(ctx, blkb+64+8);
			break;
		case 0x0F:
			LQTVQ_EncEmitCommand(ctx, 0x0F);
			LQTVQ_FastEncAD(ctx, blkb[2], blkb[3]);
			LQTVQ_EncEmit256B(ctx, blkb+64);
			break;

		default:
			__asm { int 3 }
			break;
		}
	}

	if(blkb==blks)
		return(0);

	LQTVQ_EncEmitDbgCmdPfx(ctx);
	LQTVQ_EncEmitCommand(ctx, 0x40);
	
	blkb=LQTVQ_EncFlushBits(ctx);
	return(blkb-cbuf);
}
#endif

int LQTVQ_EncodeImgBufferCtx(BT4A_Context *ctx,
	byte *obuf, int cbsz, byte *ibuf, int xs, int ys, int qfl, int clrs)
{
	byte *ct, *ct0;
	int sz, sz1;
	int i, j, k, l;

	if(!ctx->blks)
	{
		ctx->xs=xs;
		ctx->ys=ys;
		ctx->xsb=(xs+7)>>3;
		ctx->ysb=(ys+7)>>3;
		ctx->nblk=ctx->xsb*ctx->ysb;
		ctx->blks=malloc(ctx->nblk*64);
		ctx->lblks=malloc(ctx->nblk*64);
	}

	ct=obuf;
	
	LQTVQ_SetupContextQf(ctx, qfl);
	LQTVQ_EncImageBGRA(ctx, ctx->blks, ibuf, xs, ys);
	
	ct0=ct+16;
	sz1=LQTVQ_EncImgBlocks(ctx,
		ct0, ctx->blks,
		(qfl&BT4A_QFL_PFRAME)?ctx->lblks:NULL,
		xs, ys, qfl);
	
	if(sz1>=(8192-4))
	{
		i=sz1+6;
		*ct++=0x40+(i>>24);	*ct++=(i>>16);
		*ct++=(i>> 8);		*ct++=(i    );
		*ct++='I';			*ct++='0';
	}else
	{
		i=sz1+4;
		*ct++=0x00+(i>>8);	*ct++=(i    );
		*ct++='I';			*ct++='0';
	}
	memmove(ct, ct0, sz1);
	ct+=sz1;

#if 1
	ct0=ct+16;
	sz1=LQTVQ_EncImgAlphaBlocks(ctx,
		ct0, ctx->blks,
		(qfl&BT4A_QFL_PFRAME)?ctx->lblks:NULL,
		xs, ys, qfl);
	if(sz1>0)
	{
		if(sz1>=(8192-4))
		{
			i=sz1+6;
			*ct++=0x40+(i>>24);	*ct++=(i>>16);
			*ct++=(i>> 8);		*ct++=(i    );
			*ct++='A';			*ct++='X';
		}else
		{
			i=sz1+4;
			*ct++=0x00+(i>>8);	*ct++=(i    );
			*ct++='A';			*ct++='X';
		}
		memmove(ct, ct0, sz1);
		ct+=sz1;
	}
#endif
	
	sz=ct-obuf;
	return(sz);
}


int LQTVQ_DumpStatsCtx(BT4A_Context *ctx)
{
	int i, j, k;
	printf("rk_cy=%d rk_cuv=%d rk_dy=%d rk_duv=%d\n",
		ctx->rk_cy, ctx->rk_cuv, ctx->rk_dy, ctx->rk_duv);
	printf("rk_cnt=%d rk_misc=%d\n",
		ctx->rk_cnt, ctx->rk_misc);

	printf("stat_yuvbits=%d/%d (Ab=%.2f, Tb=%.2f%%)\n",
		ctx->stat_yuvbits, ctx->stat_yuvcnt, 
		(ctx->stat_yuvbits/(ctx->stat_yuvcnt+1.0)),
		(100.0*ctx->stat_yuvbits)/(ctx->stat_bits+1.0));

	printf("  Cy=%d/%d (Ab=%.2f, Tb=%.2f%%)\n",
		ctx->stat_yuvcbits[0], ctx->stat_yuvcnt, 
		(ctx->stat_yuvcbits[0]/(ctx->stat_yuvcnt+1.0)),
		(100.0*ctx->stat_yuvcbits[0])/(ctx->stat_bits+1.0));
	printf("  Cu=%d/%d (Ab=%.2f, Tb=%.2f%%)\n",
		ctx->stat_yuvcbits[1], ctx->stat_yuvcnt, 
		(ctx->stat_yuvcbits[1]/(ctx->stat_yuvcnt+1.0)),
		(100.0*ctx->stat_yuvcbits[1])/(ctx->stat_bits+1.0));
	printf("  Cv=%d/%d (Ab=%.2f, Tb=%.2f%%)\n",
		ctx->stat_yuvcbits[2], ctx->stat_yuvcnt, 
		(ctx->stat_yuvcbits[2]/(ctx->stat_yuvcnt+1.0)),
		(100.0*ctx->stat_yuvcbits[2])/(ctx->stat_bits+1.0));
	printf("  Dy=%d/%d (Ab=%.2f, Tb=%.2f%%)\n",
		ctx->stat_yuvcbits[3], ctx->stat_yuvcnt, 
		(ctx->stat_yuvcbits[3]/(ctx->stat_yuvcnt+1.0)),
		(100.0*ctx->stat_yuvcbits[3])/(ctx->stat_bits+1.0));
	printf("  Du=%d/%d (Ab=%.2f, Tb=%.2f%%)\n",
		ctx->stat_yuvcbits[4], ctx->stat_yuvcnt, 
		(ctx->stat_yuvcbits[4]/(ctx->stat_yuvcnt+1.0)),
		(100.0*ctx->stat_yuvcbits[4])/(ctx->stat_bits+1.0));
	printf("  Dv=%d/%d (Ab=%.2f, Tb=%.2f%%)\n",
		ctx->stat_yuvcbits[5], ctx->stat_yuvcnt, 
		(ctx->stat_yuvcbits[5]/(ctx->stat_yuvcnt+1.0)),
		(100.0*ctx->stat_yuvcbits[5])/(ctx->stat_bits+1.0));
	printf("sm_mask.rk=%d sm_mask.rov=%d sm_mask.bits=%d/%d "
			"(Ab=%.2f, Tb=%.2f%%)\n",
		ctx->sm_mask.rk, ctx->sm_mask.rov,
		ctx->sm_mask.bits, ctx->sm_mask.cnt,
		ctx->sm_mask.bits/(ctx->sm_mask.cnt+1.0),
		(100.0*ctx->sm_mask.bits)/(ctx->stat_bits+1.0));

	printf("yuv>Z: Cy=%d(%.2f%%) Cu=%d(%.2f%%) Cv=%d(%.2f%%)\n"
			"    Dy=%d(%.2f%%) Du=%d(%.2f%%) Dv=%d(%.2f%%)\n",
		ctx->yuv_cz[0], (100.0*ctx->yuv_cz[0])/(ctx->stat_yuvcnt+1.0),
		ctx->yuv_cz[1], (100.0*ctx->yuv_cz[1])/(ctx->stat_yuvcnt+1.0),
		ctx->yuv_cz[2], (100.0*ctx->yuv_cz[2])/(ctx->stat_yuvcnt+1.0),
		ctx->yuv_cz[3], (100.0*ctx->yuv_cz[3])/(ctx->stat_yuvcnt+1.0),
		ctx->yuv_cz[4], (100.0*ctx->yuv_cz[4])/(ctx->stat_yuvcnt+1.0),
		ctx->yuv_cz[5], (100.0*ctx->yuv_cz[5])/(ctx->stat_yuvcnt+1.0));

	printf("sm_cmd.rk=%d sm_cmd.rov=%d sm_cmd.bits=%d/%d "
			"(Ab=%.2f, Tb=%.2f%%)\n",
		ctx->sm_cmd.rk, ctx->sm_cmd.rov,
		ctx->sm_cmd.bits, ctx->sm_cmd.cnt,
		ctx->sm_cmd.bits/(ctx->sm_cmd.cnt+1.0),
		(100.0*ctx->sm_cmd.bits)/(ctx->stat_bits+1.0));
	for(i=0; i<4; i++)
	{
		printf("    ");
		for(j=0; j<16; j++)
		{
			k=(byte)(ctx->sm_cmd.rov+(i*16+j));
			printf("%02X ", ctx->sm_cmd.tab[k]);
		}
		printf("\n");
	}
	
	printf("pixel bits=%d (%.2f%%)\n",
		ctx->stat_pixbits,
		(100.0*ctx->stat_pixbits)/(ctx->stat_bits+1.0));

	return(0);
}

