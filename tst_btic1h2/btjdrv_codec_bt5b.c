static int bt5b_decompress_frame_clrs(BTIC1H_VidCodecCTX *ctx,
	void *src, void *dst, int ssz, int dsz, int clrs)
{
	btic5a_ctxinfo *info;
	int err, i, j, k, ib, ob, clrfl, ystr;
	byte *ibuf;
//	byte *sbuf, *tbuf;

	info=ctx->data;

	info->ihead->biSizeImage=ssz;
	info->ohead->biSizeImage=dsz;

	if(info->flip)
	{
		ibuf=(byte *)dst;
		ystr=-info->ihead->biWidth;
		ibuf-=(info->ihead->biHeight-1)*(ystr*info->pxsz);
	}else
	{
		ibuf=dst;
		ystr=info->ihead->biWidth;
	}

//	BTIC5B_DecodeFrame(info->vctx, src, ssz, dst,
//		info->ihead->biWidth, clrs);

	BTIC5B_DecodeFrame(info->vctx, src, ssz, (u16 *)ibuf,
		ystr, clrs);

	return(0);
}

static int bt5b_decompress_frame(BTIC1H_VidCodecCTX *ctx,
	void *src, void *dst, int ssz, int dsz)
{
	btic5a_ctxinfo *info;

	info=ctx->data;
	return(bt5b_decompress_frame_clrs(ctx,
		src, dst, ssz, dsz, info->clrs));
}

int bt5b_end_decompress(BTIC1H_VidCodecCTX *ctx)
{
	btic5a_ctxinfo *info;
	
	info=ctx->data;

	BTIC1H_Work_KillWorkers();
//	BTIC5B_FreeDecodeContext(ctx->data);
	BTIC5B_FreeEncodeContext(info->vctx);
	free(info);
	return(0);
}

static BTIC1H_VidCodecCTX *bt5b_begin_decompress(int fcc,
	BTIC1H_BMPInfoHeader *in, BTIC1H_BMPInfoHeader *out)
{
	BTIC1H_VidCodecCTX *ctx;
	BTIC5B_DecodeContext *btctx;
	btic5a_ctxinfo *info;
	int err;

	if(in->biCompression!=BTIC1H_FCC_bt5b)
		return(NULL);

	ctx=BTIC1H_VidCodecCTX_New();
//	info=bgbrpi_malloc(sizeof(bt5b_ctxinfo));
	info=malloc(sizeof(btic5a_ctxinfo));
	memset(info, 0, sizeof(btic5a_ctxinfo));
	ctx->data=info;

	btctx=BTIC5B_AllocDecodeContext();
	info->vctx=btctx;

	info->ihead=malloc(sizeof(BTIC1H_BMPInfoHeader));
	memset(info->ihead, 0, sizeof(BTIC1H_BMPInfoHeader));
	info->ihead->biSize		= sizeof(BTIC1H_BMPInfoHeader);
	info->ihead->biWidth		= in->biWidth;
	info->ihead->biHeight		= in->biHeight;
	info->ihead->biPlanes		= in->biPlanes;
	info->ihead->biBitCount		= in->biBitCount;
	info->ihead->biCompression	= in->biCompression;
	info->ihead->biSizeImage	=
		in->biWidth*in->biHeight*in->biBitCount/8;

	info->ohead=malloc(sizeof(BTIC1H_BMPInfoHeader));
	memset(info->ohead, 0, sizeof(BTIC1H_BMPInfoHeader));
	info->ohead->biSize		= sizeof(BTIC1H_BMPInfoHeader);
	info->ohead->biWidth		= out->biWidth;
//	info->ohead->biHeight		= -out->biHeight;
	info->ohead->biHeight		= out->biHeight;
	info->ohead->biPlanes		= out->biPlanes;
	info->ohead->biBitCount		= out->biBitCount;
	info->ohead->biCompression	= out->biCompression;
	info->ihead->biSizeImage	=
		out->biWidth*out->biHeight*out->biBitCount/8;

	info->buffer=malloc(out->biWidth*out->biHeight*out->biBitCount/8);

	info->pxsz=out->biBitCount/8;

	if(out)
	{
		info->flip=((short)(out->biHeight))<0;

		info->clrs=(out->biBitCount==24)?
			BTIC4B_CLRS_RGB:BTIC4B_CLRS_RGBA;

//		if(out->biCompression==BTIC1H_FCC_BGRA)
//			info->clrs=BTIC1H_PXF_BGRA;

		if(out->biCompression==BTIC1H_FCC_RGBX)
		{
			info->clrs=(out->biBitCount==24)?
				BTIC4B_CLRS_RGB:BTIC4B_CLRS_RGBX;
		}

		if(out->biCompression==BTIC1H_FCC_RGBA)
		{
			info->clrs=(out->biBitCount==24)?
				BTIC4B_CLRS_RGB:BTIC4B_CLRS_RGBA;
		}

		if(out->biCompression==BTIC1H_FCC_BGRX)
		{
			info->clrs=(out->biBitCount==24)?
				BTIC4B_CLRS_BGR:BTIC4B_CLRS_BGRX;
		}

		if(out->biCompression==BTIC1H_FCC_BGRA)
		{
			info->clrs=(out->biBitCount==24)?
				BTIC4B_CLRS_BGR:BTIC4B_CLRS_BGRA;
		}

#if 0
		if(out->biCompression==BTIC1H_FCC_YUY2)
		{
			info->clrs=BTIC1H_PXF_YUYV;
//			info->flip=!info->flip;
		}

		if(out->biCompression==BTIC1H_FCC_UYVY)
		{
			info->clrs=BTIC1H_PXF_UYVY;
//			info->flip=!info->flip;
		}
#endif

		if(out->biCompression==BTIC1H_FCC_DXT1)
			{ info->clrs=BTIC4B_CLRS_BC1; }
		if(out->biCompression==BTIC1H_FCC_DXT5)
			{ info->clrs=BTIC4B_CLRS_BC3; }
		if(out->biCompression==BTIC1H_FCC_BC1)
			{ info->clrs=BTIC4B_CLRS_BC1; }
		if(out->biCompression==BTIC1H_FCC_BC3)
			{ info->clrs=BTIC4B_CLRS_BC3; }
//		if(out->biCompression==BTIC1H_FCC_BC6H)
//			{ info->clrs=BTIC4B_CLRS_BC6H; }
		if(out->biCompression==BTIC1H_FCC_BC7)
			{ info->clrs=BTIC4B_CLRS_BC7; }
	}
	else
		{ info->clrs=BTIC4B_CLRS_RGBA; }

	ctx->decompress_frame=&bt5b_decompress_frame;
	ctx->decompress_frame_clrs=&bt5b_decompress_frame_clrs;
	ctx->end_decompress=bt5b_end_decompress;

	return(ctx);
}

int bt5b_decompress_query(int fcc,
	BTIC1H_BMPInfoHeader *in, BTIC1H_BMPInfoHeader *out)
{
	int err;

	if((fcc!=BTIC1H_FCC_bt5b) && (in->biCompression!=BTIC1H_FCC_bt5b))
		return(0);
	return(1);
}


int bt5b_compress_frame(BTIC1H_VidCodecCTX *ctx,
	void *src, void *dst, int dsz, int qfl, int clrs, int *rfl)
{
//	BTIC4B_Context *info;
	btic5a_ctxinfo *info;
	int sz, qfl1, clrs1;
	
	info=ctx->data;

	if(clrs==-1)
		{ clrs1=info->clrs; }
	else
	{
		switch(clrs)
		{
		case BTIC1H_PXF_RGBA: clrs1=BTIC4B_CLRS_RGBA; break;
		case BTIC1H_PXF_BGRA: clrs1=BTIC4B_CLRS_BGRA; break;
		case BTIC1H_PXF_RGBX: clrs1=BTIC4B_CLRS_RGBX; break;
		case BTIC1H_PXF_BGRX: clrs1=BTIC4B_CLRS_BGRX; break;
		case BTIC1H_PXF_RGB: clrs1=BTIC4B_CLRS_RGB; break;
		case BTIC1H_PXF_BGR: clrs1=BTIC4B_CLRS_BGR; break;

		case BTIC1H_PXF_BC1: clrs1=BTIC4B_CLRS_BC1; break;
		case BTIC1H_PXF_BC3: clrs1=BTIC4B_CLRS_BC3; break;
		case BTIC1H_PXF_BC6: clrs1=BTIC4B_CLRS_BC6; break;
		case BTIC1H_PXF_BC7: clrs1=BTIC4B_CLRS_BC7; break;
		default: clrs1=clrs;
		}
	}

	qfl1=qfl&127;
	if(qfl&BTIC1H_QFL_IFRAME)qfl1|=BTIC4B_QFL_IFRAME;
	if(qfl&BTIC1H_QFL_PFRAME)qfl1|=BTIC4B_QFL_PFRAME;

	if(info->flip)qfl1|=BTIC4B_QFL_FLIP;

//	if(qfl&BTIC1H_QFL_USEGDBDR)qfl1|=BTIC4B_QFL_OPTBCN;

//	sz=BTIC4B_EncodeImgBufferCtx(info, dst, dsz,
//		src, info->xs, info->ys, qfl1, clrs1);

	sz=BTIC5B_EncodeFrameImage(info->vctx, dst, dsz,
		src, qfl1, clrs1);

//	if(info->bits_total>=(1<<27))
//	{
//		BTIC4B_DumpStatsCtx(info);
//		BTIC4B_ClearEncodeStats(info);
//	}
	
	*rfl=qfl;
	return(sz);
}

int bt5b_end_compress(BTIC1H_VidCodecCTX *ctx)
{
	btic5a_ctxinfo *info;
	
	info=ctx->data;

	BTIC1H_Work_KillWorkers();
	BTIC5B_FreeEncodeContext(info->vctx);
	free(info);
	return(0);
}

BTIC1H_VidCodecCTX *bt5b_begin_compress(int fcc,
	BTIC1H_BMPInfoHeader *in, BTIC1H_BMPInfoHeader *out)
{
	BTIC1H_VidCodecCTX *ctx;
	BTIC5B_EncodeContext *btctx;
	btic5a_ctxinfo *info;
	int err;

	if((fcc!=BTIC1H_FCC_bt5b) && (out->biCompression!=BTIC1H_FCC_bt5b))
		return(NULL);

	ctx=BTIC1H_VidCodecCTX_New();
//	info=BTIC4B_AllocContext();
//	ctx->data=info;

	info=malloc(sizeof(btic5a_ctxinfo));
	memset(info, 0, sizeof(btic5a_ctxinfo));
	ctx->data=info;

	btctx=BTIC5B_AllocEncodeContext();
	info->vctx=btctx;

//	info->xs=abs(in->biWidth);
//	info->ys=abs(in->biHeight);
	info->clrs=BTIC4B_CLRS_RGBA;
	info->flip=((short)(in->biHeight))<0;

	BTIC5B_EncodeInitContext(btctx,
		abs(in->biWidth), abs(in->biHeight), 100);

	if(in)
	{
		info->clrs=(in->biBitCount==24)?
			BTIC4B_CLRS_RGB:BTIC4B_CLRS_RGBA;

		if(in->biCompression==BTIC1H_FCC_RGBX)
		{
			info->clrs=(in->biBitCount==24)?
				BTIC4B_CLRS_RGB:BTIC4B_CLRS_RGBX;
		}

		if(in->biCompression==BTIC1H_FCC_RGBA)
		{
			info->clrs=(in->biBitCount==24)?
				BTIC4B_CLRS_RGB:BTIC4B_CLRS_RGBA;
		}

		if(in->biCompression==BTIC1H_FCC_BGRX)
		{
			info->clrs=(in->biBitCount==24)?
				BTIC4B_CLRS_BGR:BTIC4B_CLRS_BGRX;
		}

		if(in->biCompression==BTIC1H_FCC_BGRA)
		{
			info->clrs=(in->biBitCount==24)?
				BTIC4B_CLRS_BGR:BTIC4B_CLRS_BGRA;
		}
	}
	else
		{ info->clrs=BTIC4B_CLRS_RGBA; }

	ctx->compress_frame=&bt5b_compress_frame;
	ctx->end_compress=bt5b_end_compress;

	return(ctx);
}

int bt5b_compress_query(int fcc,
	BTIC1H_BMPInfoHeader *in, BTIC1H_BMPInfoHeader *out)
{
	int err;

//	if(fcc!=RIFF_TAG_BTIC)
//		return(0);

	if((fcc!=BTIC1H_FCC_bt5b) && (out->biCompression!=BTIC1H_FCC_bt5b))
		return(0);
	return(1);
}


static void bt5b_shutdown()
{
	int i;
}

int BTIC1H_CodecBT5B_Init()
{
	BTIC1H_VidCodec *codec;
	int i, j;
	short *sw;
	char buf[256];
	char *t;

	printf("BT5B: Init\n");

	codec=BTIC1H_VidCodec_New();
	codec->begin_decompress=&bt5b_begin_decompress;
	codec->begin_compress=&bt5b_begin_compress;

	codec->decompress_query=&bt5b_decompress_query;
	codec->compress_query=&bt5b_compress_query;

	return(0);
}
