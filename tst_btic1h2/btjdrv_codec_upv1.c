static int upv1_decompress_frame_clrs(BTIC1H_VidCodecCTX *ctx,
	void *src, void *dst, int ssz, int dsz, int clrs)
{
	btic5a_ctxinfo *info;
	int err, i, j, k, ib, ob, clrfl, ystr;
	int x, y, xs, ys, psz;
	byte *ibuf1, *ibuf2;
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

	xs=info->ohead->biWidth;
	ys=info->ohead->biHeight;
	psz=info->ohead->biBitCount>>3;
	if(!psz)
		psz=4;
	
	if(xs<0)	xs=-xs;
	if(ys<0)	ys=-ys;

//	BTIC5C_DecodeFrame(info->vctx, src, ssz, dst,
//		info->ihead->biWidth, clrs);

	ibuf1=info->buffer1;
	ibuf2=info->buffer2;
	info->buffer1=ibuf2;
	info->buffer2=ibuf1;

	if(info->ohead->biCompression==BTIC1H_FCC_upic)
	{
	//	memcpy(ibuf, info->buffer, xs*ys*psz);
		TKuPI_DecodeImageBufferI(info->vctx,
			src, ssz, ibuf1, NULL,
			xs, ys, clrs);
	}else
	{
	//	memcpy(ibuf, info->buffer, xs*ys*psz);
		TKuPI_DecodeImageBufferI(info->vctx,
			src, ssz, ibuf1, ibuf2,
			xs, ys, clrs);
	}

	if(info->flip)
	{
		ibuf=dst;
		for(y=0; y<ys; y++)
		{
			memcpy(ibuf+(y*xs*psz), ibuf1+(ys-y-1)*xs*psz, xs*psz);
		}
	}else
	{
//		memcpy(dst, ibuf1, xs*ys*psz);
		ibuf=dst;
		for(y=0; y<ys; y++)
		{
			memcpy(ibuf+(y*xs*psz), ibuf1+y*xs*psz, xs*psz);
		}
	}

//	memcpy(info->buffer, ibuf, xs*ys*psz);

	return(0);
}

static int upv1_decompress_frame(BTIC1H_VidCodecCTX *ctx,
	void *src, void *dst, int ssz, int dsz)
{
	btic5a_ctxinfo *info;

	info=ctx->data;
	return(upv1_decompress_frame_clrs(ctx,
		src, dst, ssz, dsz, info->clrs));
}

int upv1_end_decompress(BTIC1H_VidCodecCTX *ctx)
{
	btic5a_ctxinfo *info;
	
	info=ctx->data;

	BTIC1H_Work_KillWorkers();
//	BTIC5C_FreeDecodeContext(ctx->data);
//	BTIC5C_FreeEncodeContext(info->vctx);
	free(info);
	return(0);
}

static BTIC1H_VidCodecCTX *upv1_begin_decompress(int fcc,
	BTIC1H_BMPInfoHeader *in, BTIC1H_BMPInfoHeader *out)
{
	BTIC1H_VidCodecCTX *ctx;
	TKuPI_DecState *btctx;
	btic5a_ctxinfo *info;
	int err, xs, ys, psz;

	if(	(in->biCompression!=BTIC1H_FCC_upv1) &&
		(in->biCompression!=BTIC1H_FCC_upic))
			return(NULL);

	ctx=BTIC1H_VidCodecCTX_New();
//	info=bgbrpi_malloc(sizeof(upv1_ctxinfo));
	info=malloc(sizeof(btic5a_ctxinfo));
	memset(info, 0, sizeof(btic5a_ctxinfo));
	ctx->data=info;

	btctx=TKuPI_AllocDecodeContext();
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

	xs=out->biWidth;
	ys=out->biHeight;
	psz=out->biBitCount/8;
	if(xs<0)	xs=-xs;
	if(ys<0)	ys=-ys;
	if(psz<4)	psz=4;

	info->ohead=malloc(sizeof(BTIC1H_BMPInfoHeader));
	memset(info->ohead, 0, sizeof(BTIC1H_BMPInfoHeader));
	info->ohead->biSize		= sizeof(BTIC1H_BMPInfoHeader);
	info->ohead->biWidth		= out->biWidth;
//	info->ohead->biHeight		= -out->biHeight;
	info->ohead->biHeight		= out->biHeight;
	info->ohead->biPlanes		= out->biPlanes;
	info->ohead->biBitCount		= out->biBitCount;
	info->ohead->biCompression	= out->biCompression;
	info->ihead->biSizeImage	= xs*ys*psz;

	info->buffer1=malloc((xs+15)*(ys+15)*psz);
	info->buffer2=malloc((xs+15)*(ys+15)*psz);

	info->pxsz=out->biBitCount/8;

	if(out)
	{
		info->flip=((short)(out->biHeight))<0;

		info->clrs=(out->biBitCount==24)?
			TKUPI_PIXFMT_RGB24:TKUPI_PIXFMT_RGBA32;

//		if(out->biCompression==BTIC1H_FCC_BGRA)
//			info->clrs=BTIC1H_PXF_BGRA;

		if(out->biCompression==BTIC1H_FCC_RGBX)
		{
			info->clrs=(out->biBitCount==24)?
				TKUPI_PIXFMT_RGB24:TKUPI_PIXFMT_RGBA32;
		}

		if(out->biCompression==BTIC1H_FCC_RGBA)
		{
			info->clrs=(out->biBitCount==24)?
				TKUPI_PIXFMT_RGB24:TKUPI_PIXFMT_RGBA32;
		}

		if(out->biCompression==BTIC1H_FCC_BGRX)
		{
			info->clrs=(out->biBitCount==24)?
				TKUPI_PIXFMT_BGR24:TKUPI_PIXFMT_BGRA32;
		}

		if(out->biCompression==BTIC1H_FCC_BGRA)
		{
			info->clrs=(out->biBitCount==24)?
				TKUPI_PIXFMT_BGR24:TKUPI_PIXFMT_BGRA32;
		}

#if 1
		if(out->biCompression==BTIC1H_FCC_DXT1)
			{ info->clrs=TKUPI_PIXFMT_BC1; }
		if(out->biCompression==BTIC1H_FCC_DXT5)
			{ info->clrs=TKUPI_PIXFMT_BC3; }
		if(out->biCompression==BTIC1H_FCC_BC1)
			{ info->clrs=TKUPI_PIXFMT_BC1; }
		if(out->biCompression==BTIC1H_FCC_BC3)
			{ info->clrs=TKUPI_PIXFMT_BC3; }
//		if(out->biCompression==BTIC1H_FCC_BC6H)
//			{ info->clrs=BTIC4B_CLRS_BC6H; }
//		if(out->biCompression==BTIC1H_FCC_BC7)
//			{ info->clrs=BTIC4B_CLRS_BC7; }
#endif
	}
	else
		{ info->clrs=TKUPI_PIXFMT_RGBA32; }

	ctx->decompress_frame=&upv1_decompress_frame;
	ctx->decompress_frame_clrs=&upv1_decompress_frame_clrs;
	ctx->end_decompress=upv1_end_decompress;

	return(ctx);
}

int upv1_decompress_query(int fcc,
	BTIC1H_BMPInfoHeader *in, BTIC1H_BMPInfoHeader *out)
{
	int err;

	if(	(fcc!=BTIC1H_FCC_upv1) && (in->biCompression!=BTIC1H_FCC_upv1) &&
		(fcc!=BTIC1H_FCC_upic) && (in->biCompression!=BTIC1H_FCC_upic))
			return(0);
	return(1);
}


int upv1_compress_frame(BTIC1H_VidCodecCTX *ctx,
	void *src, void *dst, int dsz, int qfl, int clrs, int *rfl)
{
	static int leqfl, leinh;
//	BTIC4B_Context *info;
	btic5a_ctxinfo *info;
	byte *ibuf1, *ibuf2, *ibuf;
	int sz, qfl1, clrs1, xs, ys, psz, y;
	
	if(!src || !dst)
		return(0);
	
	info=ctx->data;

	if(clrs==-1)
		{ clrs1=info->clrs; }
	else
	{
		switch(clrs)
		{
		case BTIC1H_PXF_RGBA: clrs1=TKUPI_PIXFMT_RGBA32; break;
		case BTIC1H_PXF_BGRA: clrs1=TKUPI_PIXFMT_BGRA32; break;
		case BTIC1H_PXF_RGBX: clrs1=TKUPI_PIXFMT_RGBA32; break;
		case BTIC1H_PXF_BGRX: clrs1=TKUPI_PIXFMT_BGRA32; break;
		case BTIC1H_PXF_RGB: clrs1=TKUPI_PIXFMT_RGB24; break;
		case BTIC1H_PXF_BGR: clrs1=TKUPI_PIXFMT_BGR24; break;

		case BTIC1H_PXF_BC1: clrs1=TKUPI_PIXFMT_BC1; break;
		case BTIC1H_PXF_BC3: clrs1=TKUPI_PIXFMT_BC3; break;
//		case BTIC1H_PXF_BC6: clrs1=BTIC4B_CLRS_BC6; break;
//		case BTIC1H_PXF_BC7: clrs1=BTIC4B_CLRS_BC7; break;
		default: clrs1=clrs;
		}
	}

	qfl1=qfl&127;
	if(!qfl1)
		qfl1=75;

	if(qfl&BTIC1H_QFL_PFRAME)
	{
		if((qfl1<101) && (qfl1>82))
			qfl1=82;
	
		if(leinh)
		{
			qfl1|=leqfl&
				(	TKUPI_ENCFL_USETNS_MSK |
					TKUPI_ENCFL_USECLR_MSK |
					TKUPI_ENCFL_USERK_MSK  );
			leinh--;
		}else
		{
			qfl1|=TKUPI_ENCFL_ENCAUTO;
			leinh=15;
		}
		qfl1|=TKUPI_ENCFL_PFRAME;
	}else
	{
		qfl1|=TKUPI_ENCFL_ENCAUTO;
		leinh=0;
	}

	if(info->flip)qfl1|=BTIC4B_QFL_FLIP;

//	if(qfl&BTIC1H_QFL_USEGDBDR)qfl1|=BTIC4B_QFL_OPTBCN;

//	sz=BTIC4B_EncodeImgBufferCtx(info, dst, dsz,
//		src, info->xs, info->ys, qfl1, clrs1);

	ibuf1=info->buffer1;
	ibuf2=info->buffer2;
	info->buffer1=ibuf2;
	info->buffer2=ibuf1;

	if(!ibuf1 || !ibuf2)
		return(-1);

	xs=info->ihead->biWidth;
	ys=info->ihead->biHeight;
	psz=info->ihead->biBitCount/8;

	if(!xs || !ys)
		return(-1);

	if(xs<0)	xs=-xs;
	if(ys<0)	ys=-ys;

	if(info->flip)
	{
		ibuf=src;
		for(y=0; y<ys; y++)
		{
			memcpy(ibuf1+(ys-y-1)*xs*psz, ibuf+(y*xs*psz), xs*psz);
		}
	}else
	{
//		memcpy(dst, ibuf1, xs*ys*psz);
		ibuf=src;
		for(y=0; y<ys; y++)
		{
			memcpy(ibuf1+y*xs*psz, ibuf+(y*xs*psz), xs*psz);
		}
	}

	if(info->ohead->biCompression==BTIC1H_FCC_upic)
	{
		sz=TKuPI_EncodeImageBufferI(info->vctx, dst, dsz,
			ibuf1, NULL,
			xs, ys, qfl1, clrs1);
	}else
	{
		sz=TKuPI_EncodeImageBufferI(info->vctx, dst, dsz,
			ibuf1, ibuf2, xs, ys, qfl1, clrs1);
		TKuPI_DecodeImageBufferI(info->v2ctx,
			dst, sz, ibuf1, ibuf2,
			xs, ys, clrs1);
	}

	leqfl=((TKuPI_EncState *)info->vctx)->enc_qfl;

//	if(info->bits_total>=(1<<27))
//	{
//		BTIC4B_DumpStatsCtx(info);
//		BTIC4B_ClearEncodeStats(info);
//	}
	
	*rfl=qfl;
	return(sz);
}

int upv1_end_compress(BTIC1H_VidCodecCTX *ctx)
{
	btic5a_ctxinfo *info;
	
	info=ctx->data;

	BTIC1H_Work_KillWorkers();
//	BTIC5C_FreeEncodeContext(info->vctx);
	free(info);
	return(0);
}

BTIC1H_VidCodecCTX *upv1_begin_compress(int fcc,
	BTIC1H_BMPInfoHeader *in, BTIC1H_BMPInfoHeader *out)
{
	BTIC1H_VidCodecCTX *ctx;
	TKuPI_EncState *btctx;
	btic5a_ctxinfo *info;
	int err, xs, ys, psz;

	if(	(fcc!=BTIC1H_FCC_upv1) && (out->biCompression!=BTIC1H_FCC_upv1) &&
		(fcc!=BTIC1H_FCC_upic) && (out->biCompression!=BTIC1H_FCC_upic))
			return(NULL);

	ctx=BTIC1H_VidCodecCTX_New();
//	info=BTIC4B_AllocContext();
//	ctx->data=info;

	info=malloc(sizeof(btic5a_ctxinfo));
	memset(info, 0, sizeof(btic5a_ctxinfo));
	ctx->data=info;

	info->ihead=malloc(sizeof(BTIC1H_BMPInfoHeader));
	memset(info->ihead, 0, sizeof(BTIC1H_BMPInfoHeader));
	info->ihead->biSize		= sizeof(BTIC1H_BMPInfoHeader);
	if(in)
	{
		xs=in->biWidth;
		ys=in->biHeight;
		info->pxsz=in->biBitCount/8;
		psz=info->pxsz;
		if(psz<4)
			psz=4;
		
		if(xs<0)	xs=-xs;
		if(ys<0)	ys=-ys;

		info->ihead->biWidth		= in->biWidth;
		info->ihead->biHeight		= in->biHeight;
		info->ihead->biPlanes		= in->biPlanes;
		info->ihead->biBitCount		= in->biBitCount;
		info->ihead->biCompression	= in->biCompression;
		info->ihead->biSizeImage	= xs*ys*(info->pxsz);

		info->buffer1=malloc((xs+15)*(ys+15)*psz);
		info->buffer2=malloc((xs+15)*(ys+15)*psz);
	}

	info->ohead=malloc(sizeof(BTIC1H_BMPInfoHeader));
	memset(info->ohead, 0, sizeof(BTIC1H_BMPInfoHeader));
	if(out)
	{
		info->ohead->biSize		= sizeof(BTIC1H_BMPInfoHeader);
		info->ohead->biWidth		= out->biWidth;
		info->ohead->biHeight		= out->biHeight;
		info->ohead->biPlanes		= out->biPlanes;
		info->ohead->biBitCount		= out->biBitCount;
		info->ohead->biCompression	= out->biCompression;
		info->ihead->biSizeImage	=
			out->biWidth*out->biHeight*out->biBitCount/8;
	}

	btctx=TKuPI_AllocEncodeContext();
	info->vctx=btctx;

	info->v2ctx=TKuPI_AllocDecodeContext();

//	info->xs=abs(in->biWidth);
//	info->ys=abs(in->biHeight);
	info->clrs=TKUPI_PIXFMT_RGBA32;
	info->flip=((short)(in->biHeight))<0;

//	BTIC5C_EncodeInitContext(btctx,
//		abs(in->biWidth), abs(in->biHeight), 100);

	if(in)
	{
		info->clrs=(in->biBitCount==24)?
			TKUPI_PIXFMT_RGB24:TKUPI_PIXFMT_RGBA32;

		if(in->biCompression==BTIC1H_FCC_RGBX)
		{
			info->clrs=(in->biBitCount==24)?
				TKUPI_PIXFMT_RGB24:TKUPI_PIXFMT_RGBA32;
		}

		if(in->biCompression==BTIC1H_FCC_RGBA)
		{
			info->clrs=(in->biBitCount==24)?
				TKUPI_PIXFMT_RGB24:TKUPI_PIXFMT_RGBA32;
		}

		if(in->biCompression==BTIC1H_FCC_BGRX)
		{
			info->clrs=(in->biBitCount==24)?
				TKUPI_PIXFMT_BGR24:TKUPI_PIXFMT_BGRA32;
		}

		if(in->biCompression==BTIC1H_FCC_BGRA)
		{
			info->clrs=(in->biBitCount==24)?
				TKUPI_PIXFMT_BGR24:TKUPI_PIXFMT_BGRA32;
		}
	}
	else
		{ info->clrs=TKUPI_PIXFMT_RGBA32; }

	ctx->compress_frame=&upv1_compress_frame;
	ctx->end_compress=upv1_end_compress;

	return(ctx);
}

int upv1_compress_query(int fcc,
	BTIC1H_BMPInfoHeader *in, BTIC1H_BMPInfoHeader *out)
{
	int err;

//	if(fcc!=RIFF_TAG_BTIC)
//		return(0);

	if(	(fcc!=BTIC1H_FCC_upv1) && (out->biCompression!=BTIC1H_FCC_upv1) &&
		(fcc!=BTIC1H_FCC_upic) && (out->biCompression!=BTIC1H_FCC_upic) )
		return(0);
	return(1);
}


static void upv1_shutdown()
{
	int i;
}

int BTIC1H_CodecUPV1_Init()
{
	BTIC1H_VidCodec *codec;
	int i, j;
	short *sw;
	char buf[256];
	char *t;

	printf("UPV1: Init\n");

	codec=BTIC1H_VidCodec_New();
	codec->begin_decompress=&upv1_begin_decompress;
	codec->begin_compress=&upv1_begin_compress;

	codec->decompress_query=&upv1_decompress_query;
	codec->compress_query=&upv1_compress_query;

	return(0);
}
