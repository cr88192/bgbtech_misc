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
#else
int LQTVQ_DecReadCommand(BT4A_Context *ctx)
{
	int i;
	i=LQTVQ_ReadSymbolSmtf(ctx, &(ctx->sm_cmd));
	return(i);
}

int LQTVQ_DecReadMask(BT4A_Context *ctx)
{
	int i;
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
	
	m=LQTVQ_DecReadMask(ctx);
	
	dcy=0;	dcu=0;	dcv=0;
	if(m&0x02)dcy=LQTVQ_DecReadValCy(ctx);
	if(m&0x04)dcu=LQTVQ_DecReadValCuv(ctx);
	if(m&0x08)dcv=LQTVQ_DecReadValCuv(ctx);

	ctx->cy+=dcy*ctx->qy;
	ctx->cu+=dcu*ctx->quv;
	ctx->cv+=dcv*ctx->quv;
}

void LQTVQ_DecColorYUVD(BT4A_Context *ctx)
{
	int dcy, dcu, dcv, ddy, ddu, ddv;
	int i;
	
	i=LQTVQ_DecReadMask(ctx);
	dcy=0;	dcu=0;	dcv=0;
	ddy=0;	ddu=0;	ddv=0;

	if(i&0x02)dcy=LQTVQ_DecReadValCy(ctx);
	if(i&0x04)dcu=LQTVQ_DecReadValCuv(ctx);
	if(i&0x08)dcv=LQTVQ_DecReadValCuv(ctx);
	if(i&0x10)ddy=LQTVQ_DecReadValDy(ctx);

	ctx->cy+=dcy*ctx->qy;
	ctx->cu+=dcu*ctx->quv;
	ctx->cv+=dcv*ctx->quv;
	ctx->dy+=ddy*ctx->qdy;
}

void LQTVQ_DecColorYUVDyuv(BT4A_Context *ctx)
{
	int dcy, dcu, dcv, ddy, ddu, ddv;
	int i;
	
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

	ctx->cy+=dcy*ctx->qy;
	ctx->cu+=dcu*ctx->quv;
	ctx->cv+=dcv*ctx->quv;
	ctx->dy+=ddy*ctx->qdy;
	ctx->du+=ddu*ctx->qduv;
	ctx->dv+=ddv*ctx->qduv;
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

force_inline void LQTVQ_DecRead64B(BT4A_Context *ctx, byte *buf)
{
	((u32 *)buf)[0]=LQTVQ_Read32Bits(ctx);
	((u32 *)buf)[1]=LQTVQ_Read32Bits(ctx);
}

force_inline void LQTVQ_DecRead96B(BT4A_Context *ctx, byte *buf)
{
	((u32 *)buf)[0]=LQTVQ_Read32Bits(ctx);
	((u32 *)buf)[1]=LQTVQ_Read32Bits(ctx);
	((u32 *)buf)[2]=LQTVQ_Read32Bits(ctx);
}

force_inline void LQTVQ_DecRead128B(BT4A_Context *ctx, byte *buf)
{
	((u32 *)buf)[0]=LQTVQ_Read32Bits(ctx);
	((u32 *)buf)[1]=LQTVQ_Read32Bits(ctx);
	((u32 *)buf)[2]=LQTVQ_Read32Bits(ctx);
	((u32 *)buf)[3]=LQTVQ_Read32Bits(ctx);
}

force_inline void LQTVQ_DecRead192B(BT4A_Context *ctx, byte *buf)
{
	((u32 *)buf)[0]=LQTVQ_Read32Bits(ctx);
	((u32 *)buf)[1]=LQTVQ_Read32Bits(ctx);
	((u32 *)buf)[2]=LQTVQ_Read32Bits(ctx);
	((u32 *)buf)[3]=LQTVQ_Read32Bits(ctx);
	((u32 *)buf)[4]=LQTVQ_Read32Bits(ctx);
	((u32 *)buf)[5]=LQTVQ_Read32Bits(ctx);
}

force_inline void LQTVQ_DecRead256B(BT4A_Context *ctx, byte *buf)
{
	((u32 *)buf)[0]=LQTVQ_Read32Bits(ctx);
	((u32 *)buf)[1]=LQTVQ_Read32Bits(ctx);
	((u32 *)buf)[2]=LQTVQ_Read32Bits(ctx);
	((u32 *)buf)[3]=LQTVQ_Read32Bits(ctx);
	((u32 *)buf)[4]=LQTVQ_Read32Bits(ctx);
	((u32 *)buf)[5]=LQTVQ_Read32Bits(ctx);
	((u32 *)buf)[6]=LQTVQ_Read32Bits(ctx);
	((u32 *)buf)[7]=LQTVQ_Read32Bits(ctx);
}

#endif

void LQTVQ_FillBlockHeadTag(BT4A_Context *ctx, byte *blk, int tag)
{
	*(u32 *)(blk+ 0)=tag;

	*(u16 *)(blk+ 4)=lqtvq_clamp65535(ctx->cy);
	*(u16 *)(blk+ 6)=lqtvq_clamp65535(ctx->cu);
	*(u16 *)(blk+ 8)=lqtvq_clamp65535(ctx->cv);
	*(u16 *)(blk+10)=lqtvq_clamp65535(ctx->dy);
	*(u16 *)(blk+12)=lqtvq_clamp65535(ctx->du);
	*(u16 *)(blk+14)=lqtvq_clamp65535(ctx->dv);
}

void LQTVQ_FillBlockHeadL8(BT4A_Context *ctx, byte *blk, int tag)
{
	*(u16 *)(blk+ 0)=tag;
	*(blk+2)=lqtvq_clamp255(ctx->cy);
	*(blk+3)=lqtvq_clamp255(ctx->cu);
	*(blk+4)=lqtvq_clamp255(ctx->cv);
	*(blk+5)=lqtvq_clamp255(ctx->dy);
	*(blk+6)=lqtvq_clamp255(ctx->du);
	*(blk+7)=lqtvq_clamp255(ctx->dv);
}

void LQTVQ_DecodeReadBytes(BT4A_Context *ctx, byte *blk, int len)
{
	memcpy(blk, ctx->cs, len);
	ctx->cs+=len;
}

void LQTVQ_DecodeCopyBlocks(BT4A_Context *ctx,
	byte *dblks, byte *sblks, int cnt)
{
	memcpy(dblks, sblks, cnt*64);
}

void LQTVQ_DecodeSetParm(BT4A_Context *ctx, int var, int val)
{
}

void LQTVQ_DecodeEnableFeature(BT4A_Context *ctx, int var)
{
}

void LQTVQ_DecodeDisableFeature(BT4A_Context *ctx, int var)
{
}

byte *LQTVQ_DecSetupDecBlockInner(BT4A_Context *ctx, byte *ct, int op)
{
	byte *ct;

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
		*(ct+16)=LQTVQ_Read8BitsNM(ctx);
		break;
	case 0x04:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		*(u16 *)(ct+16)=LQTVQ_Read16BitsNM(ctx);
		break;
	case 0x05:
	case 0x06:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		*(u32 *)(ct+16)=LQTVQ_Read32Bits(ctx);
		break;
	case 0x07:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		*(u32 *)(ct+16)=LQTVQ_Read32Bits(ctx);
		*(u32 *)(ct+20)=LQTVQ_Read32Bits(ctx);
		break;

	case 0x09:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecodeRead8B(ctx, ct+16);
		break;
	case 0x0A:	case 0x0B:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecodeRead16B(ctx, ct+16);
		break;
	case 0x0C:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecodeRead32B(ctx, ct+16);
		break;
	case 0x0D:	case 0x0E:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecodeRead64B(ctx, ct+16);
		break;
	case 0x0F:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecodeRead128B(ctx, ct+16);
		break;

	case 0x13:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecodeRead192B(ctx, ct+16);
		break;
	case 0x14:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecodeRead256B(ctx, ct+16);
		break;
	case 0x15:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecodeRead32B(ctx, ct+16);
		LQTVQ_DecodeRead8B(ctx, ct+20);
		LQTVQ_DecodeRead8B(ctx, ct+21);
		break;
	case 0x16:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecodeRead32B(ctx, ct+16);
		LQTVQ_DecodeRead32B(ctx, ct+20);
		LQTVQ_DecodeRead32B(ctx, ct+24);
		break;
	case 0x17:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecodeRead128B(ctx, ct+16);
		LQTVQ_DecodeRead8B(ctx, ct+32);
		LQTVQ_DecodeRead8B(ctx, ct+33);
		break;
	case 0x18:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecodeRead128B(ctx, ct+16);
		LQTVQ_DecodeRead32B(ctx, ct+32);
		LQTVQ_DecodeRead32B(ctx, ct+36);
		break;

	case 0x19:
		LQTVQ_FillBlockHeadTagL8(ctx, ct, op);
		LQTVQ_DecodeRead192B(ctx, ct+ 8);
		LQTVQ_DecodeRead128B(ctx, ct+32);
		LQTVQ_DecodeRead128B(ctx, ct+48);
		break;

	case 0x1A:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecodeRead128B(ctx, ct+16);
		LQTVQ_DecodeRead64B(ctx, ct+32);
		LQTVQ_DecodeRead64B(ctx, ct+40);
		break;
	case 0x1B:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecodeRead128B(ctx, ct+16);
		LQTVQ_DecodeRead128B(ctx, ct+32);
		LQTVQ_DecodeRead128B(ctx, ct+48);
		break;
	case 0x1C:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecodeRead192B(ctx, ct+16);
		LQTVQ_DecodeRead32B(ctx, ct+40);
		LQTVQ_DecodeRead32B(ctx, ct+44);
		break;
	case 0x1D:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecodeRead256B(ctx, ct+16);
		LQTVQ_DecodeRead64B(ctx, ct+48);
		LQTVQ_DecodeRead64B(ctx, ct+56);
		break;
	case 0x1E:
		LQTVQ_FillBlockHeadTag(ctx, ct, op);
		LQTVQ_DecodeRead192B(ctx, ct+16);
		LQTVQ_DecodeRead96B(ctx, ct+40);
		LQTVQ_DecodeRead96B(ctx, ct+52);
		break;
	}
	return(ct+64);
}

int LQTVQ_DecImgBufFastBGRA(BT4A_Context *ctx,
	byte *cbuf, int cbsz,
	byte *blks, byte *lblks,
	int xs, int ys)
{
//	byte blkb[64];
	byte *cs, *cse;
	byte *ct, *lcs;
	int xs1, ys1;
	int op, lop0, lop1, ret;
	int i, j, k, l, n;

	LQTVQ_InitScTables();
	LQTVQ_InitRice();
	
	LQTVQ_SetupContextQf(ctx, 100);
//	LQTVQ_SetupWriteBits(ctx, cbuf, xs*ys*8);

	xs1=(xs+7)>>3;
	ys1=(ys+7)>>3;

	ctx->cs=cbuf; cse=cbuf+cbsz;
	ct=blks; lcs=lblks;
	while(((ctx->cs)<cse) && !ret)
	{
		lop1=lop0; lop0=op;
		op=LQTVQ_DecReadCommand(ctx);
		switch(op)
		{
		case 0x00:
			LQTVQ_DecColorYUV(ctx);
			LQTVQ_FillBlockHeadTag(ctx, ct, 0);
			ct=ct+64;
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

		case 0x15:
		case 0x16:		case 0x17:
		case 0x18:		case 0x19:
		case 0x1A:		case 0x1B:
		case 0x1C:		case 0x1D:
		case 0x1E:
			LQTVQ_DecColorYUVDyuv(ctx);
			ct=LQTVQ_DecSetupDecBlockInner(ctx, ct, op);
			break;

		case 0x20:	case 0x21:	case 0x22:	case 0x23:
		case 0x24:	case 0x25:	case 0x26:	case 0x27:
		case 0x29:
		case 0x2A:	case 0x2B:
		case 0x2C:	case 0x2D:	case 0x2E:	case 0x2F:
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
			LQTVQ_DecodeCopyBlocks(ct, lcs, n);
			break;
		case 0x42:
			n=LQTVQ_DecReadCountVal(ctx);
			i=LQTVQ_DecReadGenericVal(ctx);
			j=LQTVQ_DecReadGenericVal(ctx);
			lcs=lblks+(ct-blks);
			LQTVQ_DecodeCopyBlocks(ct, lcs+((i*xs1+j)*64), n);
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

		default:
			*(int *)-1=-1;
			break;
		}
	}
	return(ret);
}
