// #define LQTVQ_BYTES

#ifdef LQTVQ_BYTES
int LQTVQ_DecodeUVLI(BT4A_Context *ctx)
{
	int i;

	i=*ctx->cs++;
	if(i<0x80)
	{
		return(i);
	}else if(i<0xC0)
	{
		i=(i&0x7F<<8)|(*ctx->cs++);
		return(i);
	}else if(i<0xE0)
	{
		i=(i&0x3F<<8)|(*ctx->cs++);
		i=(i<<8)|(*ctx->cs++);
		return(i);
	}else if(i<0xF0)
	{
		i=(i&0x1F<<8)|(*ctx->cs++);
		i=(i<<8)|(*ctx->cs++);
		i=(i<<8)|(*ctx->cs++);
		return(i);
	}else if(i<0xF0)
	{
		i=(i&0x0F<<8)|(*ctx->cs++);
		i=(i<<8)|(*ctx->cs++);
		i=(i<<8)|(*ctx->cs++);
		i=(i<<8)|(*ctx->cs++);
		return(i);
	}
}

int LQTVQ_DecReadCountVal(BT4A_Context *ctx)
{
	return(LQTVQ_DecodeUVLI(ctx));
}

int LQTVQ_DecReadGenericUVal(BT4A_Context *ctx)
{
	return(LQTVQ_DecodeUVLI(ctx));
}

int LQTVQ_DecReadGenericVal(BT4A_Context *ctx)
{
	int v;
	v=LQTVQ_DecodeUVLI(ctx);
	v=(v>>1)^((v<<31)>>31);
	return(v);
}

int LQTVQ_DecReadCommand(BT4A_Context *ctx)
{
	return(*ctx->cs++);
}

void LQTVQ_DecColorYUV(BT4A_Context *ctx)
{
	int dcy, dcu, dcv;
	int i;
	
	if((*ctx->cs)&1)
	{
		i=*ctx->cs++;
		i|=(*ctx->cs++)<<8;
		dcy=(i>>1)&31;
		dcu=(i>>6)&31;
		dcv=(i>>11)&31;
	}else
	{
		i=LQTVQ_DecodeUVLI(ctx);
		dcy=0;	dcu=0;	dcv=0;
		if(i&0x02)dcy=LQTVQ_DecodeUVLI(ctx);
		if(i&0x04)dcu=LQTVQ_DecodeUVLI(ctx);
		if(i&0x08)dcv=LQTVQ_DecodeUVLI(ctx);
	}

	dcy=(dcy>>1)^((dcy<<31)>>31);
	dcu=(dcu>>1)^((dcu<<31)>>31);
	dcv=(dcv>>1)^((dcv<<31)>>31);

	ctx->cy+=dcy*ctx->qy;
	ctx->cu+=dcu*ctx->quv;
	ctx->cv+=dcv*ctx->quv;
//	ctx->dy+=ddy*ctx->qdy;
}

void LQTVQ_DecColorYUVD(BT4A_Context *ctx)
{
	int dcy, dcu, dcv, ddy, ddu, ddv;
	int i;
	
	if((*ctx->cs)&1)
	{
		i=*ctx->cs++;
		i|=(*ctx->cs++)<<8;
		i|=(*ctx->cs++)<<16;
		dcy=(i>>1)&31;
		dcu=(i>>6)&31;
		dcv=(i>>11)&31;
		ddy=(i>>16)&31;
		if((i>>21)&1)
		{
			i|=(*ctx->cs++)<<24;
			ddu=(i>>22)&31;
			ddv=(i>>27)&31;
		}else
		{
			ddu=0;
			ddv=0;
		}
	}else
	{
		i=LQTVQ_DecodeUVLI(ctx);
		dcy=0;	dcu=0;	dcv=0;
		ddy=0;	ddu=0;	ddv=0;
		if(i&0x02)dcy=LQTVQ_DecodeUVLI(ctx);
		if(i&0x04)dcu=LQTVQ_DecodeUVLI(ctx);
		if(i&0x08)dcv=LQTVQ_DecodeUVLI(ctx);
		if(i&0x10)ddy=LQTVQ_DecodeUVLI(ctx);
		if(i&0x20)ddu=LQTVQ_DecodeUVLI(ctx);
		if(i&0x40)ddv=LQTVQ_DecodeUVLI(ctx);
	}

	dcy=(dcy>>1)^((dcy<<31)>>31);
	dcu=(dcu>>1)^((dcu<<31)>>31);
	dcv=(dcv>>1)^((dcv<<31)>>31);
	ddy=(ddy>>1)^((ddy<<31)>>31);
	ddu=(ddu>>1)^((ddu<<31)>>31);
	ddv=(ddv>>1)^((ddv<<31)>>31);

	ctx->cy+=dcy*ctx->qy;
	ctx->cu+=dcu*ctx->quv;
	ctx->cv+=dcv*ctx->quv;
	ctx->dy+=ddy*ctx->qdy;
	ctx->du+=ddu*ctx->qduv;
	ctx->dv+=ddv*ctx->qduv;
}

void LQTVQ_DecColorYUVDyuv(BT4A_Context *ctx)
{
	int dcy, dcu, dcv, ddy, ddu, ddv;
	int i;
	
	if((*ctx->cs)&1)
	{
		i=*ctx->cs++;
		i|=(*ctx->cs++)<<8;
		i|=(*ctx->cs++)<<16;
		dcy=(i>>1)&31;
		dcu=(i>>6)&31;
		dcv=(i>>11)&31;
		ddy=(i>>16)&31;
		if((i>>21)&1)
		{
			i|=(*ctx->cs++)<<24;
			ddu=(i>>22)&31;
			ddv=(i>>27)&31;
		}else
		{
			ddu=0;
			ddv=0;
		}
	}else
	{
		i=LQTVQ_DecodeUVLI(ctx);
		dcy=0;	dcu=0;
		dcv=0;	ddy=0;
		ddu=0;	ddv=0;
		if(i&0x02)dcy=LQTVQ_DecodeUVLI(ctx);
		if(i&0x04)dcu=LQTVQ_DecodeUVLI(ctx);
		if(i&0x08)dcv=LQTVQ_DecodeUVLI(ctx);
		if(i&0x10)ddy=LQTVQ_DecodeUVLI(ctx);
		if(i&0x20)ddu=LQTVQ_DecodeUVLI(ctx);
		if(i&0x40)ddv=LQTVQ_DecodeUVLI(ctx);
	}

	dcy=(dcy>>1)^((dcy<<31)>>31);
	dcu=(dcu>>1)^((dcu<<31)>>31);
	dcv=(dcv>>1)^((dcv<<31)>>31);
	ddy=(ddy>>1)^((ddy<<31)>>31);
	ddu=(ddu>>1)^((ddu<<31)>>31);
	ddv=(ddv>>1)^((ddv<<31)>>31);

	ctx->cy+=dcy*ctx->qy;
	ctx->cu+=dcu*ctx->quv;
	ctx->cv+=dcv*ctx->quv;
	ctx->dy+=ddy*ctx->qdy;
	ctx->du+=ddu*ctx->qduv;
	ctx->dv+=ddv*ctx->qduv;
}

force_inline void LQTVQ_DecRead8B(BT4A_Context *ctx, byte *buf)
{
	*buf=*ctx->cs++;
}

force_inline void LQTVQ_DecRead16B(BT4A_Context *ctx, byte *buf)
{
	*(u16 *)buf=*(u16 *)ctx->cs;
	ctx->cs+=2;
}

force_inline void LQTVQ_DecRead32B(BT4A_Context *ctx, byte *buf)
{
	*(u32 *)buf=*(u32 *)ctx->cs;
	ctx->cs+=4;
}

force_inline void LQTVQ_DecRead64B(BT4A_Context *ctx, byte *buf)
{
	*(u64 *)buf=*(u64 *)ctx->cs;
	ctx->cs+=8;
}

force_inline void LQTVQ_DecRead96B(BT4A_Context *ctx, byte *buf)
{
	*(u64 *)(buf+0)=*(u64 *)(ctx->cs+0);
	*(u32 *)(buf+8)=*(u32 *)(ctx->cs+8);
	ctx->cs+=12;
}

force_inline void LQTVQ_DecRead128B(BT4A_Context *ctx, byte *buf)
{
	*(u64 *)(buf+0)=*(u64 *)(ctx->cs+0);
	*(u64 *)(buf+8)=*(u64 *)(ctx->cs+8);
	ctx->cs+=16;
}

force_inline void LQTVQ_DecRead192B(BT4A_Context *ctx, byte *buf)
{
	*(u64 *)(buf+ 0)=*(u64 *)(ctx->cs+ 0);
	*(u64 *)(buf+ 8)=*(u64 *)(ctx->cs+ 8);
	*(u64 *)(buf+16)=*(u64 *)(ctx->cs+16);
	ctx->cs+=24;
}

force_inline void LQTVQ_DecRead256B(BT4A_Context *ctx, byte *buf)
{
	*(u64 *)(buf+ 0)=*(u64 *)(ctx->cs+ 0);
	*(u64 *)(buf+ 8)=*(u64 *)(ctx->cs+ 8);
	*(u64 *)(buf+16)=*(u64 *)(ctx->cs+16);
	*(u64 *)(buf+24)=*(u64 *)(ctx->cs+24);
	ctx->cs+=32;
}

#else
int LQTVQ_DecReadCommand(BT4A_Context *ctx)
{
	int i;
	i=LQTVQ_ReadSymbolSmtf(ctx, &(ctx->sm_cmd));
	return(i);
}

int LQTVQ_DecReadMask(BT4A_Context *ctx)
{
	int i, j;
	i=LQTVQ_ReadAdRiceLL(ctx, &(ctx->sm_mask.rk));
	if(!i)
		return(ctx->cmask);
	j=LQTVQ_DecodeSymbolIndexSmtf(ctx, &(ctx->sm_mask), i-1);
	if(j&1)ctx->cmask=j;
	return(j);
}

int LQTVQ_DecReadGenericUVal(BT4A_Context *ctx)
{
	int i;
	i=LQTVQ_ReadAdRiceLL(ctx, &(ctx->rk_misc));
	return(i);
}

int LQTVQ_DecReadGenericVal(BT4A_Context *ctx)
{
	int i;
	i=LQTVQ_ReadAdSRiceLL(ctx, &(ctx->rk_misc));
	return(i);
}

int LQTVQ_DecReadCountVal(BT4A_Context *ctx)
	{ return(LQTVQ_ReadAdRiceLL(ctx, &(ctx->rk_cnt))); }

int LQTVQ_DecReadValCy(BT4A_Context *ctx)
	{ return(LQTVQ_ReadAdSRiceLL(ctx, &(ctx->rk_cy))); }
int LQTVQ_DecReadValCuv(BT4A_Context *ctx)
	{ return(LQTVQ_ReadAdSRiceLL(ctx, &(ctx->rk_cuv))); }
int LQTVQ_DecReadValDy(BT4A_Context *ctx)
	{ return(LQTVQ_ReadAdSRiceLL(ctx, &(ctx->rk_dy))); }
int LQTVQ_DecReadValDuv(BT4A_Context *ctx)
	{ return(LQTVQ_ReadAdSRiceLL(ctx, &(ctx->rk_duv))); }

void LQTVQ_DecColorYUV(BT4A_Context *ctx)
{
	int dcy, dcu, dcv;
	int m;
	int i;
	
	if(ctx->cmask)
	{
		m=LQTVQ_DecReadMask(ctx);	
		dcy=0;	dcu=0;	dcv=0;
		if(m&0x02)dcy=LQTVQ_DecReadValCy(ctx);
		if(m&0x04)dcu=LQTVQ_DecReadValCuv(ctx);
		if(m&0x08)dcv=LQTVQ_DecReadValCuv(ctx);
	}else
	{
		dcy=LQTVQ_DecReadValCy(ctx);
		dcu=LQTVQ_DecReadValCuv(ctx);
		dcv=LQTVQ_DecReadValCuv(ctx);
	}

	ctx->cy+=dcy*ctx->qy;
	ctx->cu+=dcu*ctx->quv;
	ctx->cv+=dcv*ctx->quv;
}

void LQTVQ_DecColorYUVD(BT4A_Context *ctx)
{
	int dcy, dcu, dcv, ddy, ddu, ddv;
	int i;
	
	if(ctx->cmask)
	{
		i=LQTVQ_DecReadMask(ctx);
		dcy=0;	dcu=0;	dcv=0;
		ddy=0;	ddu=0;	ddv=0;

		if(i&0x02)dcy=LQTVQ_DecReadValCy(ctx);
		if(i&0x04)dcu=LQTVQ_DecReadValCuv(ctx);
		if(i&0x08)dcv=LQTVQ_DecReadValCuv(ctx);
		if(i&0x10)ddy=LQTVQ_DecReadValDy(ctx);
	}else
	{
		dcy=LQTVQ_DecReadValCy(ctx);
		dcu=LQTVQ_DecReadValCuv(ctx);
		dcv=LQTVQ_DecReadValCuv(ctx);
		ddy=LQTVQ_DecReadValDy(ctx);
	}

	ctx->cy+=dcy*ctx->qy;
	ctx->cu+=dcu*ctx->quv;
	ctx->cv+=dcv*ctx->quv;
	ctx->dy+=ddy*ctx->qdy;
}

void LQTVQ_DecColorYUVDyuv(BT4A_Context *ctx)
{
	int dcy, dcu, dcv, ddy, ddu, ddv;
	int i;
	
	if(ctx->cmask)
	{
		i=LQTVQ_DecReadMask(ctx);
		dcy=0;	dcu=0;
		dcv=0;	ddy=0;
		ddu=0;	ddv=0;
		if(i&0x02)dcy=LQTVQ_DecReadValCy(ctx);
		if(i&0x04)dcu=LQTVQ_DecReadValCuv(ctx);
		if(i&0x08)dcv=LQTVQ_DecReadValCuv(ctx);
		if(i&0x10)ddy=LQTVQ_DecReadValDy(ctx);
		if(i&0x20)ddu=LQTVQ_DecReadValDuv(ctx);
		if(i&0x40)ddv=LQTVQ_DecReadValDuv(ctx);
	}else
	{
		dcy=LQTVQ_DecReadValCy(ctx);
		dcu=LQTVQ_DecReadValCuv(ctx);
		dcv=LQTVQ_DecReadValCuv(ctx);
		ddy=LQTVQ_DecReadValDy(ctx);
		ddu=LQTVQ_DecReadValDuv(ctx);
		ddv=LQTVQ_DecReadValDuv(ctx);
	}

	ctx->cy+=dcy*ctx->qy;
	ctx->cu+=dcu*ctx->quv;
	ctx->cv+=dcv*ctx->quv;
	ctx->dy+=ddy*ctx->qdy;
	ctx->du+=ddu*ctx->qduv;
	ctx->dv+=ddv*ctx->qduv;
}

void LQTVQ_DecColorY(BT4A_Context *ctx)
{
	int dcy;
	int i;
	
	if(ctx->cmask)
	{
		i=LQTVQ_DecReadMask(ctx);
		dcy=0;
		if(i&0x02)dcy=LQTVQ_DecReadValCy(ctx);
	}else
	{
		dcy=LQTVQ_DecReadValCy(ctx);
	}

	ctx->cy+=dcy*ctx->qy;
}

void LQTVQ_DecColorYD(BT4A_Context *ctx)
{
	int dcy, ddy;
	int i;
	
	if(ctx->cmask)
	{
		i=LQTVQ_DecReadMask(ctx);
		dcy=0;	ddy=0;

		if(i&0x02)dcy=LQTVQ_DecReadValCy(ctx);
		if(i&0x10)ddy=LQTVQ_DecReadValDy(ctx);
	}else
	{
		dcy=LQTVQ_DecReadValCy(ctx);
		ddy=LQTVQ_DecReadValDy(ctx);
	}

	ctx->cy+=dcy*ctx->qy;
	ctx->dy+=ddy*ctx->qdy;
}


force_inline void LQTVQ_DecRead8B(BT4A_Context *ctx, byte *buf)
{
	*buf=LQTVQ_Read8BitsNM(ctx);
}

force_inline void LQTVQ_DecRead16B(BT4A_Context *ctx, byte *buf)
{
	*(u16 *)buf=LQTVQ_Read16BitsNM(ctx);
}

force_inline void LQTVQ_DecRead32B(BT4A_Context *ctx, byte *buf)
{
	*(u32 *)buf=LQTVQ_Read32Bits(ctx);
}

force_inline void LQTVQ_DecRead48B(BT4A_Context *ctx, byte *buf)
{
	((u32 *)buf)[0]=LQTVQ_Read32Bits(ctx);
	((u16 *)buf)[2]=LQTVQ_Read16BitsNM(ctx);
}

force_inline void LQTVQ_DecRead64B(BT4A_Context *ctx, byte *buf)
{
	((u64 *)buf)[0]=LQTVQ_Read64Bits(ctx);
}

force_inline void LQTVQ_DecRead96B(BT4A_Context *ctx, byte *buf)
{
	((u64 *)buf)[0]=LQTVQ_Read64Bits(ctx);
	((u32 *)buf)[2]=LQTVQ_Read32Bits(ctx);
}

force_inline void LQTVQ_DecRead128B(BT4A_Context *ctx, byte *buf)
{
	((u64 *)buf)[0]=LQTVQ_Read64Bits(ctx);
	((u64 *)buf)[1]=LQTVQ_Read64Bits(ctx);
}

force_inline void LQTVQ_DecRead192B(BT4A_Context *ctx, byte *buf)
{
	((u64 *)buf)[0]=LQTVQ_Read64Bits(ctx);
	((u64 *)buf)[1]=LQTVQ_Read64Bits(ctx);
	((u64 *)buf)[2]=LQTVQ_Read64Bits(ctx);
}

force_inline void LQTVQ_DecRead256B(BT4A_Context *ctx, byte *buf)
{
	((u64 *)buf)[0]=LQTVQ_Read64Bits(ctx);
	((u64 *)buf)[1]=LQTVQ_Read64Bits(ctx);
	((u64 *)buf)[2]=LQTVQ_Read64Bits(ctx);
	((u64 *)buf)[3]=LQTVQ_Read64Bits(ctx);
}

#endif

void LQTVQ_FillBlockHeadTag(BT4A_Context *ctx, byte *blk, int tag)
{
//	*(u16 *)(blk+ 0)=tag;
//	*(u16 *)(blk+ 2)=0x00FF;

	*(u32 *)(blk+ 0)=tag|0x00FF0000;

//	*(u16 *)(blk+ 4)=lqtvq_clamp65535(ctx->cy);
//	*(u16 *)(blk+ 6)=lqtvq_clamp65535(ctx->cu);
//	*(u16 *)(blk+ 8)=lqtvq_clamp65535(ctx->cv);
//	*(u16 *)(blk+10)=lqtvq_clamp65535(ctx->dy);
//	*(u16 *)(blk+12)=lqtvq_clamp65535(ctx->du);
//	*(u16 *)(blk+14)=lqtvq_clamp65535(ctx->dv);

	*(s16 *)(blk+ 4)=lqtvq_clamp32767S(ctx->cy);
	*(s16 *)(blk+ 6)=lqtvq_clamp32767S(ctx->cu);
	*(s16 *)(blk+ 8)=lqtvq_clamp32767S(ctx->cv);
	*(s16 *)(blk+10)=lqtvq_clamp32767S(ctx->dy);
	*(s16 *)(blk+12)=lqtvq_clamp32767S(ctx->du);
	*(s16 *)(blk+14)=lqtvq_clamp32767S(ctx->dv);
}

void LQTVQ_FillBlockHeadL8(BT4A_Context *ctx, byte *blk, int tag)
{
	*(u16 *)(blk+ 0)=tag;
	*(blk+2)=lqtvq_clamp255(ctx->cy);
//	*(blk+3)=lqtvq_clamp255(ctx->cu);
//	*(blk+4)=lqtvq_clamp255(ctx->cv);
	*(blk+5)=lqtvq_clamp255(ctx->dy);
//	*(blk+6)=lqtvq_clamp255(ctx->du);
//	*(blk+7)=lqtvq_clamp255(ctx->dv);

	*(blk+3)=lqtvq_clamp255((ctx->cu>>1)+128);
	*(blk+4)=lqtvq_clamp255((ctx->cv>>1)+128);
	*(blk+6)=lqtvq_clamp255(ctx->du>>1);
	*(blk+7)=lqtvq_clamp255(ctx->dv>>1);
}


void LQTVQ_FillBlockHeadAlphaTag(BT4A_Context *ctx, byte *blk, int tag)
{
	blk[1]=tag;
	blk[2]=lqtvq_clamp255(ctx->cy);
	blk[3]=lqtvq_clamp255(ctx->dy);
	*(s16 *)(blk+48+0)=lqtvq_clamp32767S(ctx->cy);
	*(s16 *)(blk+48+2)=lqtvq_clamp32767S(ctx->dy);
}

void LQTVQ_FillBlockHeadAlphaTagL8(BT4A_Context *ctx, byte *blk, int tag)
{
	blk[1]=tag;
	blk[2]=lqtvq_clamp255(ctx->cy);
	blk[3]=lqtvq_clamp255(ctx->dy);
}

void LQTVQ_DecodeReadBytes(BT4A_Context *ctx, byte *blk, int len)
{
	memcpy(blk, ctx->cs, len);
	ctx->cs+=len;
}

void LQTVQ_DecodeCopyBlocks(BT4A_Context *ctx,
	byte *dblks, byte *sblks, int cnt)
{
	memcpy(dblks, sblks, cnt*ctx->blksz);
}

void LQTVQ_DecodeCopyAlphaBlocks(BT4A_Context *ctx,
	byte *dblks, byte *sblks, int cnt)
{
	byte *cs, *ct;
	int i;
	
	cs=sblks; ct=dblks;
	for(i=0; i<cnt; i++)
	{
		ct[1]=cs[1];
		ct[2]=cs[2];
		ct[3]=cs[3];
		memcpy(ct+48, cs+48, 16);
		ct+=ctx->blksz; cs+=ctx->blksz;
	}
}

void LQTVQ_DecodeSetParm(BT4A_Context *ctx, int var, int val)
{
}

void LQTVQ_DecodeEnableFeature(BT4A_Context *ctx, int var)
{
	if(var==-1)
	{
		ctx->cmask=0xFF;
		return;
	}
}

void LQTVQ_DecodeDisableFeature(BT4A_Context *ctx, int var)
{
	if(var==-1)
	{
		ctx->cmask=0x00;
		return;
	}
}

byte *LQTVQ_DecSetupDecBlockInner(BT4A_Context *ctx, byte *ct, int op)
{
//	byte *ct;

	switch(op)
	{
	case 0x00:
		LQTVQ_FillBlockHeadTag(ctx, ct, 0);
		break;

	case 0x01:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		*(ct+16)=LQTVQ_ReadNBits(ctx, 4);
		break;
	case 0x02:
	case 0x03:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecRead8B(ctx, ct+16);
		break;
	case 0x04:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecRead16B(ctx, ct+16);
		break;
	case 0x05:
	case 0x06:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecRead32B(ctx, ct+16);
		break;
	case 0x07:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecRead64B(ctx, ct+16);
		break;

	case 0x09:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecRead8B(ctx, ct+16);
		break;
	case 0x0A:	case 0x0B:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecRead16B(ctx, ct+16);
		break;
	case 0x0C:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecRead32B(ctx, ct+16);
		break;
	case 0x0D:	case 0x0E:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecRead64B(ctx, ct+16);
		break;
	case 0x0F:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecRead128B(ctx, ct+16);
		break;
	case 0x10:
		LQTVQ_FillBlockHeadL8(ctx, ct, op);
		LQTVQ_DecRead128B(ctx, ct+ 8);
		LQTVQ_DecRead32B(ctx, ct+24);
		LQTVQ_DecRead32B(ctx, ct+28);
		break;

	case 0x13:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecRead192B(ctx, ct+16);
		break;
	case 0x14:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecRead256B(ctx, ct+16);
		break;
	case 0x15:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecRead32B(ctx, ct+16);
		LQTVQ_DecRead8B(ctx, ct+20);
		LQTVQ_DecRead8B(ctx, ct+21);
		break;
	case 0x16:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecRead32B(ctx, ct+16);
		LQTVQ_DecRead32B(ctx, ct+20);
		LQTVQ_DecRead32B(ctx, ct+24);
		break;
	case 0x17:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecRead128B(ctx, ct+16);
		LQTVQ_DecRead8B(ctx, ct+32);
		LQTVQ_DecRead8B(ctx, ct+33);
		break;
	case 0x18:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecRead128B(ctx, ct+16);
		LQTVQ_DecRead32B(ctx, ct+32);
		LQTVQ_DecRead32B(ctx, ct+36);
		break;

	case 0x19:
		LQTVQ_FillBlockHeadL8(ctx, ct, op);
		LQTVQ_DecRead192B(ctx, ct+ 8);
		LQTVQ_DecRead128B(ctx, ct+32);
		LQTVQ_DecRead128B(ctx, ct+48);
		break;

	case 0x1A:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecRead128B(ctx, ct+16);
		LQTVQ_DecRead64B(ctx, ct+32);
		LQTVQ_DecRead64B(ctx, ct+40);
		break;
	case 0x1B:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecRead128B(ctx, ct+16);
		LQTVQ_DecRead128B(ctx, ct+32);
		LQTVQ_DecRead128B(ctx, ct+48);
		break;
	case 0x1C:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecRead192B(ctx, ct+16);
		LQTVQ_DecRead32B(ctx, ct+40);
		LQTVQ_DecRead32B(ctx, ct+44);
		break;
	case 0x1D:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecRead256B(ctx, ct+16);
		LQTVQ_DecRead64B(ctx, ct+48);
		LQTVQ_DecRead64B(ctx, ct+56);
		break;
	case 0x1E:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecRead192B(ctx, ct+16);
		LQTVQ_DecRead96B(ctx, ct+40);
		LQTVQ_DecRead96B(ctx, ct+52);
		break;
	}
	return(ct+ctx->blksz);
}

int LQTVQ_DecImgBlocks(BT4A_Context *ctx,
	byte *cbuf, int cbsz,
	byte *blks, byte *lblks,
	int xs, int ys)
{
	byte opb[256];
	byte oprov;

//	byte blkb[64];
	byte *cs, *cse;
	byte *ct, *lcs, *cte;
	int xs1, ys1, nblk;
	int op, lop0, lop1, ret;
	int i, j, k, l, n;

	LQTVQ_SetupContextQf(ctx, 100);

#ifdef LQTVQ_BYTES
	ctx->cs=cbuf; cse=cbuf+cbsz;
#else
	LQTVQ_InitScTables();
	LQTVQ_InitRice();
	LQTVQ_SetupReadBits(ctx, cbuf, cbsz);
#endif

	xs1=(xs+7)>>3;
	ys1=(ys+7)>>3;
	nblk=xs1*ys1;

//	ctx->cs=cbuf; cse=cbuf+cbsz;

	memset(opb, 255, 256);
	oprov=0;

	ct=blks; lcs=lblks; ret=0; lop0=-1; op=-1;
	cte=blks+nblk*ctx->blksz;
//	while(((ctx->cs)<cse) && !ret)
	while(!ret && (ct<cte))
	{
#if 0
		k=LQTVQ_DecReadCommand(ctx);
		if(k!=0x4C)
		{
			__asm { int 3 }
			break;
		}
#endif

		lop1=lop0; lop0=op;
		op=LQTVQ_DecReadCommand(ctx);
		opb[oprov++]=op;
		
		switch(op)
		{
		case 0x00:
			LQTVQ_DecColorYUV(ctx);
			LQTVQ_FillBlockHeadTag(ctx, ct, 0);
			ct=ct+ctx->blksz;
//			ct=LQTVQ_DecSetupDecBlockInner(ctx, ct, op);
			break;
		
		case 0x01:
		case 0x02:	case 0x03:
		case 0x04:	case 0x05:
		case 0x06:	case 0x07:
		case 0x09:
		case 0x0A:	case 0x0B:
		case 0x0C:	case 0x0D:
		case 0x0E:	case 0x0F:
		case 0x13:
		case 0x14:
			LQTVQ_DecColorYUVD(ctx);
			ct=LQTVQ_DecSetupDecBlockInner(ctx, ct, op);
			break;

		case 0x10:
		case 0x15:
		case 0x16:		case 0x17:
		case 0x18:		case 0x19:
		case 0x1A:		case 0x1B:
		case 0x1C:		case 0x1D:
		case 0x1E:
			LQTVQ_DecColorYUVDyuv(ctx);
			ct=LQTVQ_DecSetupDecBlockInner(ctx, ct, op);
			break;

		case 0x20:
			n=LQTVQ_DecReadCountVal(ctx);
			for(i=0; i<n; i++)
			{
				ct=LQTVQ_DecSetupDecBlockInner(ctx, ct, op&0x1F);
			}
			break;

		case 0x21:	case 0x22:	case 0x23:
		case 0x24:	case 0x25:	case 0x26:	case 0x27:
		case 0x29:
		case 0x2A:	case 0x2B:
		case 0x2C:	case 0x2D:	case 0x2E:	case 0x2F:
		case 0x30:
		case 0x33:
		case 0x34:	case 0x35:
		case 0x36:	case 0x37:
		case 0x38:	case 0x39:	case 0x3A:	case 0x3B:
		case 0x3C:	case 0x3D:
		case 0x3E:
			n=LQTVQ_DecReadCountVal(ctx);
			for(i=0; i<n; i++)
			{
				ct=LQTVQ_DecSetupDecBlockInner(ctx, ct, op&0x1F);
			}
			break;
		
		case 0x40:
			ret=1; break;

		case 0x41:
			n=LQTVQ_DecReadCountVal(ctx);
			lcs=lblks+(ct-blks);
			LQTVQ_DecodeCopyBlocks(ctx, ct, lcs, n);
			ct+=n*ctx->blksz;
			break;
		case 0x42:
			n=LQTVQ_DecReadCountVal(ctx);
			i=LQTVQ_DecReadGenericVal(ctx);
			j=LQTVQ_DecReadGenericVal(ctx);
			lcs=lblks+(ct-blks);
			LQTVQ_DecodeCopyBlocks(ctx, ct, lcs+((i*xs1+j)*ctx->blksz), n);
			ct+=n*ctx->blksz;
			break;
		case 0x43:
			i=LQTVQ_DecReadGenericVal(ctx);
			j=LQTVQ_DecReadGenericVal(ctx);
			LQTVQ_DecodeSetParm(ctx, i, j);
			break;
		
		case 0x45:
			ctx->qy=LQTVQ_DecReadGenericUVal(ctx);
			ctx->quv=LQTVQ_DecReadGenericUVal(ctx);
			ctx->qdy=LQTVQ_DecReadGenericUVal(ctx);
			ctx->qduv=LQTVQ_DecReadGenericUVal(ctx);
			break;
		case 0x46:
			i=LQTVQ_DecReadGenericVal(ctx);
			LQTVQ_DecodeEnableFeature(ctx, i);
			break;
		case 0x47:
			i=LQTVQ_DecReadGenericVal(ctx);
			LQTVQ_DecodeDisableFeature(ctx, i);
			break;

		case 0x48:
			n=LQTVQ_DecReadCountVal(ctx);
			for(i=0; i<n; i++)
			{
				LQTVQ_DecColorYUV(ctx);
				LQTVQ_FillBlockHeadTag(ctx, ct, 0);
				ct=ct+ctx->blksz;
			}
			break;
		
		case 0x4C:
			break;

		default:
//			*(int *)-1=-1;
			__asm { int 3 }
			ret=-1;
			break;
		}
	}
	return(ret);
}

byte *LQTVQ_DecSetupDecAlphaBlockInner(BT4A_Context *ctx, byte *ct, int op)
{
//	byte *ct;

	switch(op)
	{
	case 0x00:
		LQTVQ_FillBlockHeadAlphaTagL8(ctx, ct, 0);
		break;
	case 0x01:
		LQTVQ_FillBlockHeadAlphaTag(ctx, ct, op);
		*(ct+48+8)=LQTVQ_ReadNBits(ctx, 4);
		break;
	case 0x02:
		LQTVQ_FillBlockHeadAlphaTag(ctx, ct, op);
		*(u16 *)(ct+48+8)=LQTVQ_Read16Bits(ctx);
		break;
	case 0x03:
		LQTVQ_FillBlockHeadAlphaTag(ctx, ct, op);
		LQTVQ_DecRead64B(ctx, ct+48+8);
		break;
	case 0x04:
		LQTVQ_FillBlockHeadAlphaTag(ctx, ct, op);
		*(ct+48+8)=LQTVQ_Read8Bits(ctx);
		break;
	case 0x05:
		LQTVQ_FillBlockHeadAlphaTag(ctx, ct, op);
		*(u32 *)(ct+48+8)=LQTVQ_Read32Bits(ctx);
		break;
	case 0x06:
		LQTVQ_FillBlockHeadAlphaTag(ctx, ct, op);
		LQTVQ_DecRead48B(ctx, ct+48+8);
		break;
	case 0x07:
		LQTVQ_FillBlockHeadAlphaTag(ctx, ct, op);
		LQTVQ_DecRead64B(ctx, ct+48+8);
		break;
	case 0x08:
		LQTVQ_FillBlockHeadAlphaTag(ctx, ct, op);
		break;

	case 0x0A:
	case 0x0B:
		LQTVQ_FillBlockHeadAlphaTag(ctx, ct, op);
		LQTVQ_DecRead64B(ctx, ct+48+8);
		break;
	case 0x0C:
		LQTVQ_FillBlockHeadAlphaTagL8(ctx, ct, op);
		LQTVQ_DecRead128B(ctx, ct+48);
		break;
	case 0x0D:
		LQTVQ_FillBlockHeadAlphaTag(ctx, ct, op);
		LQTVQ_DecRead128B(ctx, ct+64+8);
		break;
	case 0x0E:
		LQTVQ_FillBlockHeadAlphaTag(ctx, ct, op);
		LQTVQ_DecRead192B(ctx, ct+64+8);
		break;
	case 0x0F:
		LQTVQ_FillBlockHeadAlphaTagL8(ctx, ct, op);
		LQTVQ_DecRead256B(ctx, ct+64);
		break;
	default:
		break;
	}
	return(ct+ctx->blksz);
}

int LQTVQ_DecImgAlphaBlocks(BT4A_Context *ctx,
	byte *cbuf, int cbsz,
	byte *blks, byte *lblks,
	int xs, int ys)
{
	byte opb[256];
	byte oprov;

//	byte blkb[64];
	byte *cs, *cse;
	byte *ct, *lcs;
	int xs1, ys1;
	int op, lop0, lop1, ret;
	int i, j, k, l, n;

	LQTVQ_SetupContextQf(ctx, 100);

#ifdef LQTVQ_BYTES
	ctx->cs=cbuf; cse=cbuf+cbsz;
#else
	LQTVQ_InitScTables();
	LQTVQ_InitRice();
	LQTVQ_SetupReadBits(ctx, cbuf, cbsz);
#endif

	xs1=(xs+7)>>3;
	ys1=(ys+7)>>3;

//	ctx->cs=cbuf; cse=cbuf+cbsz;

	memset(opb, 255, 256);
	oprov=0;

	ct=blks; lcs=lblks; ret=0; lop0=-1; op=-1;
	while(!ret)
	{
		lop1=lop0; lop0=op;
		op=LQTVQ_DecReadCommand(ctx);
		opb[oprov++]=op;
		
		switch(op)
		{
		case 0x00:
			LQTVQ_DecColorY(ctx);
			LQTVQ_FillBlockHeadAlphaTag(ctx, ct, 0);
			ct=ct+ctx->blksz;
			break;
		
		case 0x01:
		case 0x02:	case 0x03:
		case 0x04:	case 0x05:
		case 0x06:	case 0x07:
		case 0x09:
		case 0x0A:	case 0x0B:
		case 0x0C:	case 0x0D:
		case 0x0E:	case 0x0F:
			LQTVQ_DecColorYD(ctx);
			ct=LQTVQ_DecSetupDecAlphaBlockInner(ctx, ct, op);
			break;

		case 0x20:	case 0x21:	case 0x22:	case 0x23:
		case 0x24:	case 0x25:	case 0x26:	case 0x27:
		case 0x28:	case 0x29:	case 0x2A:	case 0x2B:
		case 0x2C:	case 0x2D:	case 0x2E:	case 0x2F:
			n=LQTVQ_DecReadCountVal(ctx);
			for(i=0; i<n; i++)
				{ ct=LQTVQ_DecSetupDecAlphaBlockInner(ctx, ct, op&0x1F); }
			break;
		
		case 0x40:
			ret=1; break;

		case 0x41:
			n=LQTVQ_DecReadCountVal(ctx);
			lcs=lblks+(ct-blks);
//			LQTVQ_DecodeCopyBlocks(ctx, ct, lcs, n);
			ct+=n*ctx->blksz;
			break;
		case 0x42:
			n=LQTVQ_DecReadCountVal(ctx);
			i=LQTVQ_DecReadGenericVal(ctx);
			j=LQTVQ_DecReadGenericVal(ctx);
			lcs=lblks+(ct-blks);
			LQTVQ_DecodeCopyAlphaBlocks(ctx, ct, lcs+((i*xs1+j)*64), n);
			ct+=n*ctx->blksz;
			break;
		case 0x43:
			i=LQTVQ_DecReadGenericVal(ctx);
			j=LQTVQ_DecReadGenericVal(ctx);
			LQTVQ_DecodeSetParm(ctx, i, j);
			break;
		
		case 0x45:
			ctx->qy=LQTVQ_DecReadGenericUVal(ctx);
			ctx->qdy=LQTVQ_DecReadGenericUVal(ctx);
			break;
		case 0x46:
			i=LQTVQ_DecReadGenericVal(ctx);
			LQTVQ_DecodeEnableFeature(ctx, i);
			break;
		case 0x47:
			i=LQTVQ_DecReadGenericVal(ctx);
			LQTVQ_DecodeDisableFeature(ctx, i);
			break;

		case 0x48:
			n=LQTVQ_DecReadCountVal(ctx);
			for(i=0; i<n; i++)
			{
				LQTVQ_DecColorY(ctx);
				LQTVQ_FillBlockHeadTag(ctx, ct, 0);
				ct=ct+ctx->blksz;
			}
			break;

		case 0x4C:
			break;

		default:
//			*(int *)-1=-1;
			__asm { int 3 }
			ret=-1;
			break;
		}
	}
	return(ret);
}

int LQTVQ_DecodeImgBufferCtx(BT4A_Context *ctx,
	byte *cbuf, int cbsz, byte *ibuf, int xs, int ys, int clrfl)
{
	byte *csib, *csibe, *csax, *csaxe;
	byte *cs, *cs1, *cse;
	int i, j, k;
	
	if(!ctx->blks)
	{
		ctx->xs=xs;
		ctx->ys=ys;
		ctx->xsb=(xs+7)>>3;
		ctx->ysb=(ys+7)>>3;
		ctx->nblk=ctx->xsb*ctx->ysb;
//		ctx->nblk=(ctx->xsb+1)*(ctx->ysb+1);
		ctx->blksz=64;
		
		ctx->blks=malloc(ctx->nblk*ctx->blksz);
		ctx->lblks=malloc(ctx->nblk*ctx->blksz);
	}
	
	if(!cbsz)
		return(0);

	csib=NULL; csax=NULL;
	cs=cbuf; cse=cbuf+cbsz;
	while(cs<cse)
	{
		if((*cs>=0x00) && (*cs<=0x1F))
		{
			i=(cs[0]<<8)|(cs[1]);
			j=cs[2]|(cs[3]<<8);
			cs1=cs+i;
			
			if(j==LQTVQ_TCC_I0)
				{ csib=cs+4; csibe=cs1; }
			if(j==LQTVQ_TCC_AX)
				{ csax=cs+4; csaxe=cs1; }
			cs=cs1;
			continue;
		}
		if((*cs>=0x20) && (*cs<=0x3F))
		{
			i=(cs[0]<<8)|(cs[1]);
			j=cs[2]|(cs[3]<<8)|(cs[4]<<16)|(cs[5]<<24);
			cs1=cs+i;
			cs=cs1;
			continue;
		}

		if((*cs>=0x40) && (*cs<=0x5F))
		{
			i=((cs[0]&0x1F)<<24)|(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
			j=cs[4]|(cs[5]<<8);
			cs1=cs+i;
			
			if(j==LQTVQ_TCC_I0)
				{ csib=cs+6; csibe=cs1; }
			if(j==LQTVQ_TCC_AX)
				{ csax=cs+6; csaxe=cs1; }
			cs=cs1;
			continue;
		}

		if((*cs>=0x60) && (*cs<=0x7F))
		{
			i=((cs[0]&0x1F)<<24)|(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
			j=cs[4]|(cs[5]<<8)|(cs[6]<<16)|(cs[7]<<24);
			cs1=cs+i;
			cs=cs1;
			continue;
		}
		
		break;
	}

	if(!csib)
		return(-1);

	LQTVQ_DecImgBlocks(ctx, csib, csibe-csib,
		ctx->blks, ctx->lblks, xs, ys);
	if(csax)
	{
		LQTVQ_DecImgAlphaBlocks(ctx, csax, csaxe-csax,
			ctx->blks, ctx->lblks, xs, ys);
	}
	LQTVQ_DecImageClrs(ctx->blks, ibuf, xs, ys, clrfl);

	return(0);
}
