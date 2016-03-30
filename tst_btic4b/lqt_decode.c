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
	i=LQTVQ_ReadAdRiceLL(ctx, &(ctx->sm_cmd.rk));
	if(!i)
		return(ctx->cmask);
	j=LQTVQ_DecodeSymbolIndexSmtf(ctx, &(ctx->sm_cmd), i-1);
	if(j&1)ctx->cmask=j;
	return(j);
}

int LQTVQ_DecReadGenericVal(BT4A_Context *ctx)
{
	int i;
	i=LQTVQ_ReadAdSRiceLL(ctx, &(ctx->rk_misc));
	return(i);
}

void LQTVQ_DecColorYUV(BT4A_Context *ctx)
{
	int dcy, dcu, dcv;
	int m;
	int i;
	
	m=LQTVQ_DecReadMask(ctx);
	
	dcy=0;	dcu=0;	dcv=0;
	if(m&0x02)dcy=LQTVQ_DecReadGenericVal(ctx);
	if(m&0x04)dcu=LQTVQ_DecReadGenericVal(ctx);
	if(m&0x08)dcv=LQTVQ_DecReadGenericVal(ctx);

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

	if(i&0x02)dcy=LQTVQ_DecReadGenericVal(ctx);
	if(i&0x04)dcu=LQTVQ_DecReadGenericVal(ctx);
	if(i&0x08)dcv=LQTVQ_DecReadGenericVal(ctx);
	if(i&0x10)ddy=LQTVQ_DecReadGenericVal(ctx);

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
	if(i&0x02)dcy=LQTVQ_DecReadGenericVal(ctx);
	if(i&0x04)dcu=LQTVQ_DecReadGenericVal(ctx);
	if(i&0x08)dcv=LQTVQ_DecReadGenericVal(ctx);
	if(i&0x10)ddy=LQTVQ_DecReadGenericVal(ctx);
	if(i&0x20)ddu=LQTVQ_DecReadGenericVal(ctx);
	if(i&0x40)ddv=LQTVQ_DecReadGenericVal(ctx);

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

void LQTVQ_DecodeReadBytes(BT4A_Context *ctx, byte *blk, int len)
{
	memcpy(blk, ctx->cs, len);
	ctx->cs+=len;
}

int LQTVQ_DecImgBufFastBGRA(BT4A_Context *ctx,
	byte *cbuf, int cbsz,
	byte *blks, byte *lblks,
	int xs, int ys)
{
//	byte blkb[64];
	byte *cs, *cse;
	byte *ct, *lcs;
	int op, lop0, lop1;

	LQTVQ_InitScTables();
	LQTVQ_InitRice();
	
	LQTVQ_SetupContextQf(ctx, 100);
//	LQTVQ_SetupWriteBits(ctx, cbuf, xs*ys*8);

	ctx->cs=cbuf; cse=cbuf+cbsz;
	ct=blks; lcs=lblks;
	while((ctx->cs)<cse)
	{
		lop1=lop0; lop0=op;
		op=LQTVQ_DecReadCommand(ctx);
		switch(op)
		{
		case 0x00:
			LQTVQ_DecColorYUV(ctx);
			LQTVQ_FillBlockHeadTag(ctx, ct, 0);
			ct+=64;
			break;

		case 0x09:
			LQTVQ_DecColorYUVD(ctx);
			LQTVQ_FillBlockHeadTag(ctx, ct, 0x09);
			LQTVQ_DecodeReadBytes(ctx, ct+16, 1);
			ct+=64;
			break;
		case 0x0A:	case 0x0B:
			LQTVQ_DecColorYUVD(ctx);
			LQTVQ_FillBlockHeadTag(ctx, ct, op);
			LQTVQ_DecodeReadBytes(ctx, ct+16, 2);
			ct+=64;
			break;
		case 0x0C:
			LQTVQ_DecColorYUVD(ctx);
			LQTVQ_FillBlockHeadTag(ctx, ct, op);
			LQTVQ_DecodeReadBytes(ctx, ct+16, 4);
			ct+=64;
			break;
		case 0x0D:	case 0x0E:
			LQTVQ_DecColorYUVD(ctx);
			LQTVQ_FillBlockHeadTag(ctx, ct, op);
			LQTVQ_DecodeReadBytes(ctx, ct+16, 8);
			ct+=64;
			break;
		case 0x0F:
			LQTVQ_DecColorYUVD(ctx);
			LQTVQ_FillBlockHeadTag(ctx, ct, op);
			LQTVQ_DecodeReadBytes(ctx, ct+16, 16);
			ct+=64;
			break;

		case 0x13:
			LQTVQ_DecColorYUVD(ctx);
			LQTVQ_FillBlockHeadTag(ctx, ct, op);
			LQTVQ_DecodeReadBytes(ctx, ct+16, 24);
			ct+=64;
			break;

		case 0x17:
			LQTVQ_DecColorYUVD(ctx);
			LQTVQ_FillBlockHeadTag(ctx, ct, op);
			LQTVQ_DecodeReadBytes(ctx, ct+16, 32);
			ct+=64;
			break;

		case 0x18:
			LQTVQ_DecColorYUVDyuv(ctx);
			LQTVQ_FillBlockHeadTag(ctx, ct, op);
			LQTVQ_DecodeReadBytes(ctx, ct+16, 24);
			ct+=64;
			break;

		case 0x1A:
			LQTVQ_DecColorYUVDyuv(ctx);
			LQTVQ_FillBlockHeadTag(ctx, ct, op);
			LQTVQ_DecodeReadBytes(ctx, ct+16, 32);
			ct+=64;
			break;
		case 0x1B:
			LQTVQ_DecColorYUVDyuv(ctx);
			LQTVQ_FillBlockHeadTag(ctx, ct, op);
			LQTVQ_DecodeReadBytes(ctx, ct+16, 48);
			ct+=64;
			break;
		case 0x1C:
			LQTVQ_DecColorYUVDyuv(ctx);
			LQTVQ_FillBlockHeadTag(ctx, ct, op);
			LQTVQ_DecodeReadBytes(ctx, ct+16, 32);
			ct+=64;
			break;
		case 0x1D:		case 0x1E:
			LQTVQ_DecColorYUVDyuv(ctx);
			LQTVQ_FillBlockHeadTag(ctx, ct, op);
			LQTVQ_DecodeReadBytes(ctx, ct+16, 48);
			ct+=64;
			break;
		
		case 0x45:
			ctx->qy=LQTVQ_DecodeUVLI(ctx);
			ctx->quv=LQTVQ_DecodeUVLI(ctx);
			ctx->qdy=LQTVQ_DecodeUVLI(ctx);
			ctx->qduv=LQTVQ_DecodeUVLI(ctx);
			break;

		default:
			*(int *)-1=-1;
			break;
		}
	}
}
