#ifndef BGBBTJ_API
#define BGBBTJ_API
#endif

#ifndef FOURCC
#define FOURCC(a, b, c, d) ((a)|((b)<<8)|((c)<<16)|((d)<<24))
#endif

#define FCC_FORM	FOURCC('F', 'O', 'R', 'M')
#define FCC_RIFF	FOURCC('R', 'I', 'F', 'F')
#define FCC_RIFX	FOURCC('R', 'I', 'F', 'X')

#define FCC_LIST	FOURCC('L', 'I', 'S', 'T')
#define FCC_AVI		FOURCC('A', 'V', 'I', ' ')

#define FCC_hdrl	FOURCC('h', 'd', 'r', 'l')
#define FCC_strl	FOURCC('s', 't', 'r', 'l')
#define FCC_strh	FOURCC('s', 't', 'r', 'h')
#define FCC_strf	FOURCC('s', 't', 'r', 'f')
#define FCC_strd	FOURCC('s', 't', 'r', 'd')

#define FCC_avih	FOURCC('a', 'v', 'i', 'h')
#define FCC_movi	FOURCC('m', 'o', 'v', 'i')
#define FCC_idx1	FOURCC('i', 'd', 'x', '1')

#define FCC_vids	FOURCC('v', 'i', 'd', 's')
#define FCC_auds	FOURCC('a', 'u', 'd', 's')

#define FCC_MJPG	FOURCC('M', 'J', 'P', 'G')
#define FCC_JPEG	FOURCC('J', 'P', 'E', 'G')

#define FCC_MBTJ	FOURCC('M', 'B', 'T', 'J')
#define FCC_BTJP	FOURCC('B', 'T', 'J', 'P')

#define FCC_MPNG	FOURCC('M', 'P', 'N', 'G')
#define FCC_PNG		FOURCC('P', 'N', 'G', ' ')
#define FCC_PNG1	FOURCC('P', 'N', 'G', '1')
#define FCC_png1	FOURCC('p', 'n', 'g', '1')

#define FCC_MBTC	FOURCC('M', 'B', 'T', 'C')
#define FCC_BTIC	FOURCC('B', 'T', 'I', 'C')

#define FCC_RGBA	FOURCC('R', 'G', 'B', 'A')
#define FCC_RGBT	FOURCC('R', 'G', 'B', 'T')

#define FCC_BtV0	FOURCC('B', 't', 'V', '0')

#define FCC_00dc	FOURCC('0', '0', 'd', 'c')
#define FCC_01wb	FOURCC('0', '1', 'w', 'b')

#define FCC_rpza	RIFF_MAKETAG('r','p','z','a')
#define FCC_azpr	RIFF_MAKETAG('a','z','p','r')

#define FCC_bt1c	RIFF_MAKETAG('b','t','1','c')
#define FCC_c1tb	RIFF_MAKETAG('c','1','t','b')
#define FCC_bt1d	RIFF_MAKETAG('b','t','1','d')

// volatile BGBBTJ_AVICtx *pdgl_avi_cap;

int pdgl_avi_nbase=0;
int pdgl_avi_nseq=0;

volatile int pdgl_avi_encloop=0;
volatile int pdgl_avi_enctime=0;

void *pdgl_avi_malloc(int sz)
{
	void *p;
	
	p=malloc(sz);
	memset(p, 0, sz);
	return(p);
}

void BGBBTJ_PNG_WriteInt32LE(TK_FILE *fd, u32 val)
{
	tk_fputc((val    )&255, fd);
	tk_fputc((val>> 8)&255, fd);
	tk_fputc((val>>16)&255, fd);
	tk_fputc((val>>24)&255, fd);
}

void BGBBTJ_PNG_WriteFourcc(TK_FILE *fd, u32 val)
{
	tk_fputc((val    )&255, fd);
	tk_fputc((val>> 8)&255, fd);
	tk_fputc((val>>16)&255, fd);
	tk_fputc((val>>24)&255, fd);
}

void BGBBTJ_AVI_UpdateSize(TK_FILE *fd, int o)
{
	int i;

	i=tk_ftell(fd);
	tk_fseek(fd, o, 0);
	BGBBTJ_PNG_WriteInt32LE(fd, i-o-4);
	tk_fseek(fd, i, 0);
}

BGBBTJ_AVICtx *BGBBTJ_AVI_OpenOutStream(char *name,
	int w, int h, float fps, int fcc)
{
	return(BGBBTJ_AVI_OpenOutStream2(
		name, w, h, fps, fcc, BGBBTJ_AVI_AUID_22K8BM));
}

BGBBTJ_AVICtx *BGBBTJ_AVI_OpenOutStream2(char *name,
	int w, int h, float fps, int fcc, int auid)
{
	byte buf[512];

	TK_FILE *fd;
	BGBBTJ_AVICtx *ctx;
	BGBBTJ_VidCodecCTX *vcctx;
	int nbsz;
	int i, j, k;

	memset(buf, 0, 512);

	fd=tk_fopen(name, "w+b");
	if(!fd)return(NULL);

	ctx=pdgl_avi_malloc(sizeof(BGBBTJ_AVICtx));
	ctx->fd=fd;

	ctx->index=pdgl_avi_malloc(4096*sizeof(u32));
	ctx->n_index=0;
	ctx->m_index=4096/4;
	ctx->fps=fps;
	ctx->xs=w;
	ctx->ys=h;

	ctx->avih=pdgl_avi_malloc(sizeof(BGBBTJ_MainAVIHeader));
	ctx->vidh=pdgl_avi_malloc(sizeof(BGBBTJ_AVIStreamHeader));
	ctx->vidf=pdgl_avi_malloc(sizeof(BGBBTJ_BITMAPINFOHEADER));


	ctx->vidh->fccType=RIFF_TAG_vids;

	printf("BGBBTJ_AVI_OpenOutStream: fcc=%08X\n", fcc);

	ctx->vidh->fccHandler=RIFF_TAG_msvc;

	ctx->vidh->dwFlags=0;
	ctx->vidh->dwRate=fps*1024+0.5;
	ctx->vidh->dwScale=1024;
	ctx->vidh->dwSuggestedBufferSize=0;
	ctx->vidh->dwQuality=0;

	ctx->vidf->biSize=sizeof(BGBBTJ_BITMAPINFOHEADER);
	ctx->vidf->biWidth=w;
	ctx->vidf->biHeight=h;
	ctx->vidf->biPlanes=1;
	ctx->vidf->biXPelsPerMeter=0;
	ctx->vidf->biYPelsPerMeter=0;

	ctx->vidf->biBitCount=24;
	ctx->vidf->biCompression=RIFF_TAG_CRAM;
	ctx->vidf->biSizeImage=w*h*3;

//	if(fcc==BGBBTJ_FCC_bt5a)
	if(((fcc&0xFFFF)==('b'|('t'<<8))) ||
		(fcc==BGBBTJ_FCC_zcra))
	{
		ctx->vidh->fccHandler=RIFF_TAG_btic;
		ctx->vidf->biCompression=fcc;
	}

#if 1
	if(auid)
	{
		ctx->audh=pdgl_avi_malloc(sizeof(BGBBTJ_AVIStreamHeader));
		ctx->audf=pdgl_avi_malloc(sizeof(BGBBTJ_WAVEFORMATEX));

		ctx->audh->fccType=FCC_auds;
		ctx->audh->fccHandler=1;
		ctx->audh->dwFlags=0;

		switch(auid&BGBBTJ_AVI_AUID_MASK_RATE)
		{
		case BGBBTJ_AVI_AUID_8K:
			ctx->audh->dwRate=8000; break;
		case BGBBTJ_AVI_AUID_11K:
			ctx->audh->dwRate=11025; break;
		case BGBBTJ_AVI_AUID_16K:
			ctx->audh->dwRate=16000; break;
		case BGBBTJ_AVI_AUID_22K:
			ctx->audh->dwRate=22050; break;
		case BGBBTJ_AVI_AUID_32K:
			ctx->audh->dwRate=32000; break;
		case BGBBTJ_AVI_AUID_44K:
			ctx->audh->dwRate=44100; break;
		default:
			break;
		}

		ctx->audh->dwScale=1;
		ctx->audh->dwSuggestedBufferSize=0;
		ctx->audh->dwQuality=0;

		ctx->audf->wFormatTag=1;
//		ctx->audf->nChannels=2;
//		ctx->audf->nChannels=1;
		ctx->audf->nChannels=(auid&BGBBTJ_AVI_AUID_STEREO)?2:1;

//		ctx->audf->nSamplesPerSec=22050;
//		ctx->audf->nAvgBytesPerSec=22050;
		ctx->audf->wBitsPerSample=8;

		nbsz=(ctx->audf->wBitsPerSample*ctx->audf->nChannels)/8;

		ctx->audf->nSamplesPerSec=ctx->audh->dwRate;
		ctx->audf->nAvgBytesPerSec=ctx->audh->dwRate*nbsz;

		ctx->audf->nBlockAlign=nbsz;
//		ctx->audf->wBitsPerSample=8;
		ctx->audf->cbSize=0;
	}
#endif

	ctx->avih->dwMicroSecPerFrame=1000000/fps;
	ctx->avih->dwMaxBytesPerSec=0;
	ctx->avih->dwFlags=16;
	ctx->avih->dwStreams=ctx->audh?2:1;
	ctx->avih->dwSuggestedBufferSize=0;
	ctx->avih->dwWidth=w;
	ctx->avih->dwHeight=h;
	ctx->avih->dwRate=fps*1024+0.5;
	ctx->avih->dwScale=1024;


	BGBBTJ_PNG_WriteFourcc(ctx->fd, RIFF_TAG_RIFF);
	ctx->osz_avi=tk_ftell(ctx->fd);
	BGBBTJ_PNG_WriteInt32LE(ctx->fd, 0);
	BGBBTJ_PNG_WriteFourcc(ctx->fd, RIFF_TAG_AVI);

	BGBBTJ_PNG_WriteFourcc(ctx->fd, RIFF_TAG_LIST);
	ctx->osz_hdrl=tk_ftell(ctx->fd);
	BGBBTJ_PNG_WriteInt32LE(ctx->fd, 0);
	BGBBTJ_PNG_WriteFourcc(ctx->fd, RIFF_TAG_hdrl);

	i=sizeof(BGBBTJ_MainAVIHeader);
	BGBBTJ_PNG_WriteFourcc(ctx->fd, RIFF_TAG_avih);
	BGBBTJ_PNG_WriteInt32LE(ctx->fd, i);
	ctx->offs_avih=tk_ftell(ctx->fd);
	tk_fwrite(ctx->avih, 1, (i&1)?(i+1):i, ctx->fd);


	BGBBTJ_PNG_WriteFourcc(ctx->fd, RIFF_TAG_LIST);
	ctx->osz_strl=tk_ftell(ctx->fd);
	BGBBTJ_PNG_WriteInt32LE(ctx->fd, 0);
	BGBBTJ_PNG_WriteFourcc(ctx->fd, RIFF_TAG_strl);

	i=sizeof(BGBBTJ_AVIStreamHeader);
	BGBBTJ_PNG_WriteFourcc(ctx->fd, RIFF_TAG_strh);
	BGBBTJ_PNG_WriteInt32LE(ctx->fd, i);
	ctx->offs_vidh=tk_ftell(ctx->fd);
	tk_fwrite(ctx->vidh, 1, (i&1)?(i+1):i, ctx->fd);

#if 1
	i=sizeof(BGBBTJ_BITMAPINFOHEADER);
	BGBBTJ_PNG_WriteFourcc(ctx->fd, RIFF_TAG_strf);
	BGBBTJ_PNG_WriteInt32LE(ctx->fd, i);
	ctx->offs_vidf=tk_ftell(ctx->fd);
	tk_fwrite(ctx->vidf, 1, (i&1)?(i+1):i, ctx->fd);

	if(ctx->vidStrd)
	{
		i=ctx->sz_vidStrd;
		BGBBTJ_PNG_WriteFourcc(ctx->fd, RIFF_TAG_strd);
		BGBBTJ_PNG_WriteInt32LE(ctx->fd, i);
		ctx->offs_vidStrd=tk_ftell(ctx->fd);
		tk_fwrite(ctx->vidStrd, 1, (i&1)?(i+1):i, ctx->fd);
	}
#endif

	BGBBTJ_AVI_UpdateSize(ctx->fd, ctx->osz_strl);

#if 1
	if(ctx->audh)
	{
		BGBBTJ_PNG_WriteFourcc(ctx->fd, RIFF_TAG_LIST);
		ctx->osz_strl=tk_ftell(ctx->fd);
		BGBBTJ_PNG_WriteInt32LE(ctx->fd, 0);
		BGBBTJ_PNG_WriteFourcc(ctx->fd, RIFF_TAG_strl);

		i=sizeof(BGBBTJ_AVIStreamHeader);
		BGBBTJ_PNG_WriteFourcc(ctx->fd, RIFF_TAG_strh);
		BGBBTJ_PNG_WriteInt32LE(ctx->fd, i);
		ctx->offs_audh=tk_ftell(ctx->fd);
		tk_fwrite(ctx->audh, 1, (i&1)?(i+1):i, ctx->fd);

		i=sizeof(BGBBTJ_WAVEFORMATEX);
		BGBBTJ_PNG_WriteFourcc(ctx->fd, RIFF_TAG_strf);
		BGBBTJ_PNG_WriteInt32LE(ctx->fd, i);
		ctx->offs_audf=tk_ftell(ctx->fd);
		tk_fwrite(ctx->audf, 1, (i&1)?(i+1):i, ctx->fd);

		if(ctx->audStrd)
		{
			i=ctx->sz_audStrd;
			BGBBTJ_PNG_WriteFourcc(ctx->fd, RIFF_TAG_strd);
			BGBBTJ_PNG_WriteInt32LE(ctx->fd, i);
			ctx->offs_audStrd=tk_ftell(ctx->fd);
			tk_fwrite(ctx->audStrd, 1, (i&1)?(i+1):i, ctx->fd);
		}

		BGBBTJ_AVI_UpdateSize(ctx->fd, ctx->osz_strl);
	}
#endif

	BGBBTJ_AVI_UpdateSize(ctx->fd, ctx->osz_hdrl);

	BGBBTJ_PNG_WriteFourcc(ctx->fd, RIFF_TAG_LIST);
	ctx->osz_movi=tk_ftell(ctx->fd);
	BGBBTJ_PNG_WriteInt32LE(ctx->fd, 0);
	BGBBTJ_PNG_WriteFourcc(ctx->fd, RIFF_TAG_movi);

	return(ctx);
}

void BGBBTJ_AVI_CloseOutStream(BGBBTJ_AVICtx *ctx)
{
	int i;

//	BGBBTJ_JPG_FlushEncodeFast(ctx->vidCtx);

	BGBBTJ_AVI_UpdateSize(ctx->fd, ctx->osz_movi);

#if 1
	BGBBTJ_PNG_WriteFourcc(ctx->fd, RIFF_TAG_idx1);
	BGBBTJ_PNG_WriteInt32LE(ctx->fd, ctx->n_index*4*4);
	for(i=0; i<ctx->n_index; i++)
	{
		BGBBTJ_PNG_WriteFourcc(ctx->fd, ctx->index[i*4+0]);
		BGBBTJ_PNG_WriteInt32LE(ctx->fd, ctx->index[i*4+1]);
		BGBBTJ_PNG_WriteInt32LE(ctx->fd, ctx->index[i*4+2]);
		BGBBTJ_PNG_WriteInt32LE(ctx->fd, ctx->index[i*4+3]);
	}
#endif


	BGBBTJ_AVI_UpdateSize(ctx->fd, ctx->osz_avi);

	if(ctx->audh)
	{
		tk_fseek(ctx->fd, ctx->offs_audh, 0);
		tk_fwrite(ctx->audh, 1, sizeof(BGBBTJ_AVIStreamHeader), ctx->fd);

		tk_fseek(ctx->fd, ctx->offs_audf, 0);
		tk_fwrite(ctx->audf, 1, sizeof(BGBBTJ_WAVEFORMATEX), ctx->fd);
	}

	tk_fseek(ctx->fd, ctx->offs_vidh, 0);
	tk_fwrite(ctx->vidh, 1, sizeof(BGBBTJ_AVIStreamHeader), ctx->fd);

	tk_fseek(ctx->fd, ctx->offs_vidf, 0);
	tk_fwrite(ctx->vidf, 1, sizeof(BGBBTJ_BITMAPINFOHEADER), ctx->fd);

	tk_fseek(ctx->fd, ctx->offs_avih, 0);
	tk_fwrite(ctx->avih, 1, sizeof(BGBBTJ_MainAVIHeader), ctx->fd);

	free(ctx->vidh);
	free(ctx->vidf);
	free(ctx->avih);

	if(ctx->audh)free(ctx->audh);
	if(ctx->audf)free(ctx->audf);

	tk_fclose(ctx->fd);
	free(ctx);
}

int BGBBTJ_AVI_AddIndex(BGBBTJ_AVICtx *ctx, u32 tag, u32 len, u32 fl)
{
	int i;

	i=ctx->n_index++;

	if(i>=ctx->m_index)
	{
		ctx->m_index+=ctx->m_index>>1;
		ctx->index=realloc(ctx->index, ctx->m_index*4*sizeof(u32));
	}

	ctx->index[i*4+0]=tag;
	ctx->index[i*4+1]=fl;
	ctx->index[i*4+2]=tk_ftell(ctx->fd)-(ctx->osz_movi+4);
	ctx->index[i*4+3]=len;

	return(i);
}

void BGBBTJ_AVI_EmitCodedFrame(BGBBTJ_AVICtx *ctx, byte *buf,
	int sz, int qf)
{
	int fl;

	fl=0;
	if(!(qf&BGBBTJ_QFL_PFRAME))
		{ fl|=16; }

	BGBBTJ_AVI_AddIndex(ctx, RIFF_TAG_00dc, sz, fl);

	BGBBTJ_PNG_WriteFourcc(ctx->fd, RIFF_TAG_00dc);
	BGBBTJ_PNG_WriteInt32LE(ctx->fd, sz);
	tk_fwrite(buf, 1, (sz&1)?(sz+1):sz, ctx->fd);

	ctx->avih->dwTotalFrames++;
	ctx->avih->dwLength++;
	ctx->vidh->dwLength++;
	ctx->frnum++;
}

void BGBBTJ_AVI_EmitAudioFrame(BGBBTJ_AVICtx *ctx)
{
	static byte *tbuf=NULL;
	static s16 *tsbuf=NULL;
	int i0, i1, i2, i3;
	int j0, j1, j2, j3;
	int i, j, k, l, n, sz, bl, bsz;

	tbuf=ctx->capEncodeAuBuf;
	if(!tbuf)
	{
		tbuf=pdgl_avi_malloc(1<<18);
		ctx->capEncodeAuBuf=tbuf;
		tsbuf=(s16 *)tbuf;
	}

#if 1
	if(ctx->audh)
	{
		l=ctx->audf->nSamplesPerSec/ctx->fps;
		sz=(l*ctx->audf->nAvgBytesPerSec)/ctx->audf->nSamplesPerSec;

		if(ctx->audf->wBitsPerSample==8)
			{ memset(tbuf, 128, sz); }
		else
			{ memset(tbuf, 0, sz); }

		if(ctx->audf->wFormatTag==BGBBTJ_WAVE_FORMAT_IMAADPCM)
		{
//#if !defined(BGBBTJ_DRV) && !defined(BGBBTJ_NODY)
#if 1
			bsz=ctx->audf->nBlockAlign;
			bl=BGBDT_MsImaAdpcm_StereoSamplesFromBlockSize(bsz);
			n=(l+bl-1)/bl;
			l=n*bl;
			sz=n*bsz;
			memset(tbuf, 0, sz);

			if(!ctx->audSampleBuf)
			{
				BGBBTJ_AVI_AddIndex(ctx, RIFF_TAG_01wb, sz, 16);
				BGBBTJ_PNG_WriteFourcc(ctx->fd, RIFF_TAG_01wb);
				BGBBTJ_PNG_WriteInt32LE(ctx->fd, sz);
				tk_fwrite(tbuf, 1, (sz&1)?(sz+1):sz, ctx->fd);
				ctx->audh->dwLength+=l;
				return;
			}
			
			for(i=0; i<n; i++)
			{
				j=(ctx->audSampleBufStartPos+i*bl)%
					ctx->audSampleBufSz;
				BGBDT_MsImaAdpcm_EncodeBlockStereo(
					ctx->audSampleBuf+j*2, tbuf+i*bsz, bl);
			}

			j=(ctx->audSampleBufStartPos+i*bl)%ctx->audSampleBufSz;
			ctx->audSampleBufStartPos=j;
#endif
		}
		else if(ctx->audf->wFormatTag==BGBBTJ_WAVE_FORMAT_PCM)
		{
			if(!ctx->audSampleBuf)
			{
				BGBBTJ_AVI_AddIndex(ctx, RIFF_TAG_01wb, sz, 16);
				BGBBTJ_PNG_WriteFourcc(ctx->fd, RIFF_TAG_01wb);
				BGBBTJ_PNG_WriteInt32LE(ctx->fd, sz);
				tk_fwrite(tbuf, 1, (sz&1)?(sz+1):sz, ctx->fd);
				ctx->audh->dwLength+=l;
				return;
			}

			for(i=0; i<l; i++)
			{
#if 0
				if(ctx->audf->nSamplesPerSec==22050)
				{
					j0=(ctx->audSampleBufStartPos+(2*i+0))%
						ctx->audSampleBufSz;
					j1=(ctx->audSampleBufStartPos+(2*i+1))%
						ctx->audSampleBufSz;
					if(j0==ctx->audSampleBufEndPos)break;
					if(j1==ctx->audSampleBufEndPos)break;

					i0=(ctx->audSampleBuf[j0*2+0]+
						ctx->audSampleBuf[j1*2+0])/2;
					i1=(ctx->audSampleBuf[j0*2+1]+
						ctx->audSampleBuf[j1*2+1])/2;
				}else
				{
					j=(ctx->audSampleBufStartPos+i)%
						ctx->audSampleBufSz;
					i0=ctx->audSampleBuf[j*2+0];
					i1=ctx->audSampleBuf[j*2+1];
				}
#endif

				j=(ctx->audSampleBufStartPos+i)%
					ctx->audSampleBufSz;
				i0=ctx->audSampleBuf[j*2+0];
				i1=ctx->audSampleBuf[j*2+1];
	
				if(ctx->audf->nChannels==1)
				{
					if(ctx->audf->wBitsPerSample==8)
					{
						k=(i0+i1)/2;
						k=(k>>8)+128;
						k=(k<0)?0:(k<=255)?k:255;

//						k=(k>>8);
//						k=(k<(-128))?(-128):(k>127)?127:k;
						tbuf[i]=k;
					}else
					{
						k=(i0+i1)/2;
						k=(k<(-32768))?(-32768):(k>32767)?32767:k;
						tsbuf[i]=k;
					}
				}else
				{
					if(ctx->audf->wBitsPerSample==8)
					{
						j0=(i0>>8)+128;
						j1=(i1>>8)+128;
						j0=(j0<0)?0:(j0<=255)?j0:255;
						j1=(j1<0)?0:(j1<=255)?j1:255;
						tbuf[i*2+0]=j0;
						tbuf[i*2+1]=j1;
					}else
					{
						tsbuf[i*2+0]=i0;
						tsbuf[i*2+1]=i1;
					}
				}
		
//				i0=((j/64)&1)?1024:-1024; i1=-i0;

//				tbuf[i*2+0]=i0;
//				tbuf[i*2+1]=i1;
			}

//			j=(ctx->audSampleBufStartPos+l)%ctx->audSampleBufSz;
			j=(ctx->audSampleBufStartPos+i)%ctx->audSampleBufSz;

//			if(ctx->audf->nSamplesPerSec==22050)
//			{
//				j=(ctx->audSampleBufStartPos+2*i)%ctx->audSampleBufSz;
//			}else
//			{
//				j=(ctx->audSampleBufStartPos+i)%ctx->audSampleBufSz;
//			}

			ctx->audSampleBufStartPos=j;
		}

//		for(k=0; k<8; k++)
//		{
//			tbuf[k]=(tbuf[k]-128)*(k/8.0)+128;
//			tbuf[i-k]=(tbuf[i-k]-128)*(k/8.0)+128;
//		}

		BGBBTJ_AVI_AddIndex(ctx, RIFF_TAG_01wb, sz, 16);
		BGBBTJ_PNG_WriteFourcc(ctx->fd, RIFF_TAG_01wb);
		BGBBTJ_PNG_WriteInt32LE(ctx->fd, sz);
		tk_fwrite(tbuf, 1, (sz&1)?(sz+1):sz, ctx->fd);

		ctx->audh->dwLength+=l;
	}
#endif
}

void BGBBTJ_AVI_EncodeFrame(BGBBTJ_AVICtx *ctx,
	byte *ibuf, int w, int h)
{
	BGBBTJ_AVI_EncodeFrame2(ctx, ibuf, w, h, 90, 0);
}

void BGBBTJ_AVI_EncodeFrame2(BGBBTJ_AVICtx *ctx,
	byte *ibuf, int w, int h, int qf, int clrs)
{
	byte *tbuf;
	int i, j, k, l, sz, fl;

	tbuf=ctx->capEncodeBuf;
	if(!tbuf)
	{
		tbuf=pdgl_avi_malloc(1<<22);
		ctx->capEncodeBuf=tbuf;
	}

	if(ctx->vid_codec_ctx)
	{
		if(!(qf&(BGBBTJ_QFL_IFRAME|BGBBTJ_QFL_PFRAME)))
		{
//			if(!(ctx->frnum&15))
			if(!(ctx->frnum&63))
				{ qf|=BGBBTJ_QFL_IFRAME; }
			else
				{ qf|=BGBBTJ_QFL_PFRAME; }
		}

		sz=BGBBTJ_Codec_CompressFrame(ctx->vid_codec_ctx,
			ibuf, tbuf, 1<<22, qf, 0, &fl);
		qf&=(~(BGBBTJ_QFL_IFRAME|BGBBTJ_QFL_PFRAME));
		qf|=fl&(BGBBTJ_QFL_IFRAME|BGBBTJ_QFL_PFRAME);
	}

	if(sz==-999)
	{
		ctx->vid_codec_ctx=BGBBTJ_Codec_BeginCompress(
			ctx->vidh->fccHandler, ctx->vidf, ctx->vidf);

		if(ctx->vid_codec_ctx)
		{
			sz=BGBBTJ_Codec_CompressFrame(ctx->vid_codec_ctx,
				ibuf, tbuf, 1<<22, qf, 0, &fl);
			qf&=(~(BGBBTJ_QFL_IFRAME|BGBBTJ_QFL_PFRAME));
			qf|=fl&(BGBBTJ_QFL_IFRAME|BGBBTJ_QFL_PFRAME);
		}
	}

	if(ctx->vidh->fccHandler==0)
	{
		k=w*h;
		for(i=0; i<k; i++)
		{
			tbuf[i*3+0]=ibuf[i*4+2];
			tbuf[i*3+1]=ibuf[i*4+1];
			tbuf[i*3+2]=ibuf[i*4+0];
		}
		
		sz=w*h*3;
	}

	if(qf&BGBBTJ_QFL_BATCHENCODE)
	{
		BGBBTJ_AVI_EmitCodedFrame(ctx, tbuf, sz, qf);
		BGBBTJ_AVI_EmitAudioFrame(ctx);
		ctx->accDt2=0;
	}else
	{
		while(ctx->accDt2>=(1.0/ctx->fps))
		{
			BGBBTJ_AVI_EmitCodedFrame(ctx, tbuf, sz, qf);
			BGBBTJ_AVI_EmitAudioFrame(ctx);
			ctx->accDt2-=1.0/ctx->fps;
		}
	}
}

int BGBBTJ_AVI_WriteContextStereoSamples(
	BGBBTJ_AVICtx *ctx, short *buf, int cnt)
{
	int i, j, k;

	if(!ctx)
		return(-1);
		
	if(!ctx->audSampleBuf)
	{
//		i=2*44100;
		i=2*65536;

		ctx->audSampleBufSz=i;
		ctx->audSampleBufStartPos=0;
		ctx->audSampleBufEndPos=0;

		ctx->audSampleBuf=pdgl_avi_malloc(i*2*sizeof(short));
	}
	
	for(i=0; i<cnt; i++)
	{
		j=(ctx->audSampleBufEndPos+i)%
			ctx->audSampleBufSz;
		ctx->audSampleBuf[j*2+0]=buf[i*2+0];
		ctx->audSampleBuf[j*2+1]=buf[i*2+1];
	}
	j=(ctx->audSampleBufEndPos+cnt)%
		ctx->audSampleBufSz;
	ctx->audSampleBufEndPos=j;
	
	return(0);
}

int BGBBTJ_AVI_WriteContextMonoSamples(
	BGBBTJ_AVICtx *ctx, short *buf, int cnt)
{
	int i, j, k;

	if(!ctx)
		return(-1);
		
	if(!ctx->audSampleBuf)
	{
//		i=2*44100;
		i=2*65536;

		ctx->audSampleBufSz=i;
		ctx->audSampleBufStartPos=0;
		ctx->audSampleBufEndPos=0;

		ctx->audSampleBuf=pdgl_avi_malloc(i*2*sizeof(short));
	}
	
	for(i=0; i<cnt; i++)
	{
		j=(ctx->audSampleBufEndPos+i)%
			ctx->audSampleBufSz;
		ctx->audSampleBuf[j*2+0]=buf[i];
		ctx->audSampleBuf[j*2+1]=buf[i];
	}
	j=(ctx->audSampleBufEndPos+cnt)%
		ctx->audSampleBufSz;
	ctx->audSampleBufEndPos=j;
	
	return(0);
}

#if 0
int BGBBTJ_AVI_GetEncodeTime()
	{ return(pdgl_avi_enctime); }

int BGBBTJ_AVI_WriteStereoSamples(short *buf, int cnt)
{
	int i, j, k;

	if(!pdgl_avi_cap)
		return(-1);
		
	if(!pdgl_avi_cap->audSampleBuf)
	{
		i=44100;

		pdgl_avi_cap->audSampleBufSz=i;
		pdgl_avi_cap->audSampleBufStartPos=0;
		pdgl_avi_cap->audSampleBufEndPos=0;

		pdgl_avi_cap->audSampleBuf=pdgl_avi_malloc(i*2*sizeof(short));
	}
	
	for(i=0; i<cnt; i++)
	{
		j=(pdgl_avi_cap->audSampleBufEndPos+i)%
			pdgl_avi_cap->audSampleBufSz;
		pdgl_avi_cap->audSampleBuf[j*2+0]=buf[i*2+0];
		pdgl_avi_cap->audSampleBuf[j*2+1]=buf[i*2+1];
	}
	j=(pdgl_avi_cap->audSampleBufEndPos+cnt)%
		pdgl_avi_cap->audSampleBufSz;
	pdgl_avi_cap->audSampleBufEndPos=j;
	
	return(0);
}
#endif

int BGBBTJ_Tex_Resample(
	byte *src, int iw, int ih,
	byte *dst, int ow, int oh)
{
	float xs, ys, xc, yc;
	float ix, iy, fx, fy, fxn, fyn;
	float fxya, fxyb, fxyc, fxyd;
	int i, j, k, l, ik, il;
	int la, lb, lc, ld, iw4;

	xs=((float)iw/(float)ow);
	ys=((float)ih/(float)oh);
	iw4=iw*4;

	for(i=0; i<oh; i++)
	{
		iy=i*ys;
		ik=iy; fy=iy-ik; fyn=1-fy;

		for(j=0; j<ow; j++)
		{
			ix=j*xs;
			il=ix; fx=ix-il;
			l=(ik*iw+il)*4;
			fxn=1-fx;

			fxya=fxn*fyn;	fxyb=fx*fyn;
			fxyc=fxn*fy;	fxyd=fx*fy;
			la=l;			lb=l+4;
			lc=l+iw4;		ld=l+iw4+4;

			for(k=0; k<4; k++)
				dst[k]=
					src[la+k]*fxya+
					src[lb+k]*fxyb+
					src[lc+k]*fxyc+
					src[ld+k]*fxyd;
			dst+=4;
		}
	}
	return(0);
}


int BGBBTJ_Tex_SplinePolateRGBA8(byte *src, int w, int h,
	float x, float y, float *rgba)
{
	float tv[16], tva[4], tvb[4], tvc[4], tvd[4];
	double f, g, xf, yf, xg, yg;
	int i, j, k, l, xi, yi;
	int xi0, xi1, xi2, xi3;
	int yi0, yi1, yi2, yi3;

	xi=floor(x); yi=floor(y);
	xf=x-xi; yf=y-yi; xg=1-xf; yg=1-yf;

	xi0=xi-1; xi1=xi; xi2=xi+1; xi3=xi+2;
	yi0=yi-1; yi1=yi; yi2=yi+1; yi3=yi+2;

	if((xi<1) || (xi>=(w-2)) || (yi<1) || (yi>=(h-2)))
	{
		while(xi0<0)xi0+=w; while(xi0>=w)xi0-=w;
		while(xi1<0)xi1+=w; while(xi1>=w)xi1-=w;
		while(xi2<0)xi2+=w; while(xi2>=w)xi2-=w;
		while(xi3<0)xi3+=w; while(xi3>=w)xi3-=w;

		while(yi0<0)yi0+=h; while(yi0>=h)yi0-=h;
		while(yi1<0)yi1+=h; while(yi1>=h)yi1-=h;
		while(yi2<0)yi2+=h; while(yi2>=h)yi2-=h;
		while(yi3<0)yi3+=h; while(yi3>=h)yi3-=h;
	}

	for(k=0; k<4; k++)
	{
		tv[ 0]=src[(yi0*w+xi0)*4+k];
		tv[ 1]=src[(yi0*w+xi1)*4+k];
		tv[ 2]=src[(yi0*w+xi2)*4+k];
		tv[ 3]=src[(yi0*w+xi3)*4+k];
		tv[ 4]=src[(yi1*w+xi0)*4+k];
		tv[ 5]=src[(yi1*w+xi1)*4+k];
		tv[ 6]=src[(yi1*w+xi2)*4+k];
		tv[ 7]=src[(yi1*w+xi3)*4+k];
		tv[ 8]=src[(yi2*w+xi0)*4+k];
		tv[ 9]=src[(yi2*w+xi1)*4+k];
		tv[10]=src[(yi2*w+xi2)*4+k];
		tv[11]=src[(yi2*w+xi3)*4+k];
		tv[12]=src[(yi3*w+xi0)*4+k];
		tv[13]=src[(yi3*w+xi1)*4+k];
		tv[14]=src[(yi3*w+xi2)*4+k];
		tv[15]=src[(yi3*w+xi3)*4+k];

		tva[0]=tv[ 1]+xf*(tv[ 1]-tv[ 0]);
		tva[1]=tv[ 5]+xf*(tv[ 5]-tv[ 4]);
		tva[2]=tv[ 9]+xf*(tv[ 9]-tv[ 8]);
		tva[3]=tv[13]+xf*(tv[13]-tv[12]);

		tvb[0]=tv[ 2]+xg*(tv[ 2]-tv[ 3]);
		tvb[1]=tv[ 6]+xg*(tv[ 6]-tv[ 7]);
		tvb[2]=tv[10]+xg*(tv[10]-tv[11]);
		tvb[3]=tv[14]+xg*(tv[14]-tv[15]);

		tvc[0]=tv[ 4]+yf*(tv[ 4]-tv[ 0]);
		tvc[1]=tv[ 5]+yf*(tv[ 5]-tv[ 1]);
		tvc[2]=tv[ 6]+yf*(tv[ 6]-tv[ 2]);
		tvc[3]=tv[ 7]+yf*(tv[ 7]-tv[ 3]);

		tvd[0]=tv[ 8]+yg*(tv[ 8]-tv[12]);
		tvd[1]=tv[ 9]+yg*(tv[ 9]-tv[13]);
		tvd[2]=tv[10]+yg*(tv[10]-tv[14]);
		tvd[3]=tv[11]+yg*(tv[11]-tv[15]);


		tva[0]=tva[0]*xg+tvb[0]*xf;
		tva[1]=tva[1]*xg+tvb[1]*xf;
		tva[2]=tva[2]*xg+tvb[2]*xf;
		tva[3]=tva[3]*xg+tvb[3]*xf;

		tvb[0]=tvc[0]*yg+tvd[0]*yf;
		tvb[1]=tvc[1]*yg+tvd[1]*yf;
		tvb[2]=tvc[2]*yg+tvd[2]*yf;
		tvb[3]=tvc[3]*yg+tvd[3]*yf;

		tvc[0]=tva[1]+yf*(tva[1]-tva[0]);
		tvc[1]=tva[2]+yg*(tva[2]-tva[3]);
		tvc[2]=tvb[1]+xf*(tvb[1]-tvb[0]);
		tvc[3]=tvb[2]+xg*(tvb[2]-tvb[3]);

		f=(tvc[0]*yg+tvc[1]*yf+
		   tvc[2]*xg+tvc[3]*xf)*0.5;

		rgba[k]=f;
	}
	return(0);
}

int BGBBTJ_Tex_ResampleSpline(byte *src, int iw, int ih, byte *dst, int ow, int oh)
{
	float tv[4];
	float xs, ys, xc, yc;
	float ix, iy, fx, fy;
	int i, j, k, l, ik, il;

	xs=((float)iw/(float)ow);
	ys=((float)ih/(float)oh);

	for(i=0; i<oh; i++)
		for(j=0; j<ow; j++)
	{
		BGBBTJ_Tex_SplinePolateRGBA8(src, iw, ih, j*xs, i*ys, tv);
		k=tv[0]; if(k<0)k=0; if(k>255)k=255; dst[0]=k;
		k=tv[1]; if(k<0)k=0; if(k>255)k=255; dst[1]=k;
		k=tv[2]; if(k<0)k=0; if(k>255)k=255; dst[2]=k;
		k=tv[3]; if(k<0)k=0; if(k>255)k=255; dst[3]=k;

		dst+=4;
	}
	return(0);
}
