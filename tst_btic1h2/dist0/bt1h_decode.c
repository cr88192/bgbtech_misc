/*
Copyright (C) 2015 by Brendan G Bohannon

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#if 1
int BTIC1H_ReadCommandCode(BTIC1H_Context *ctx)
{
	int i, j, k;
	
//	i=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_cmdidx));
	i=BTIC1H_Rice_ReadAdRice(ctx, &(ctx->rk_cmdidx));

	if((i>0) && (i<=16))
//	if((i>=0) && (i<16))
	{
		i--;

		if(!i)
		{
			j=ctx->cmdwin[ctx->cmdwpos&15];
			return(j);
		}

#if 1
		if(i>0)
		{
			j=ctx->cmdwin[(ctx->cmdwpos+i+0)&15];
			k=ctx->cmdwin[(ctx->cmdwpos+i-1)&15];
			ctx->cmdwin[(ctx->cmdwpos+i-1)&15]=j;
			ctx->cmdwin[(ctx->cmdwpos+i+0)&15]=k;
			return(j);
		}
#endif

#if 0
		if(i==1)
		{
			j=ctx->cmdwin[ctx->cmdwpos&15];
			k=ctx->cmdwin[(ctx->cmdwpos+1)&15];
			ctx->cmdwin[ctx->cmdwpos&15]=k;
			ctx->cmdwin[(ctx->cmdwpos+1)&15]=j;
			return(k);
		}

		if(i>0)
		{
			k=ctx->cmdwin[(ctx->cmdwpos+i)&15];
			j=(byte)(--ctx->cmdwpos);
			ctx->cmdwin[j&15]=k;
			return(k);
		}
#endif

		return(-1);
	}

//	if(i==(-1))
	if(i==0)
	{
		k=BTIC1H_Rice_ReadAdRice(ctx, &(ctx->rk_cmdabs));
//		k=BTIC1H_Rice_Read8Bits(ctx);

//		k=(-i)-1;
//		k=BTIC1H_Rice_ReadNBits(ctx, -i);

		j=(byte)(--ctx->cmdwpos);
		ctx->cmdwin[j&15]=k;
		return(k);	
	}
	
//	*(int *)-1=-1;
	return(-1);
}
#endif

void BTIC1H_DecodeDeltaYUV(BTIC1H_Context *ctx)
{
	int dy, du, dv;
	int qdy, qdu, qdv;

	qdy=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_dy));
	qdu=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_duv));
	qdv=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_duv));

	ctx->cy+=qdy*ctx->qfy;
	ctx->cu+=qdu*ctx->qfuv;
	ctx->cv+=qdv*ctx->qfuv;
	
//	if((ctx->cy<0) || (ctx->cy>255) ||
//		(ctx->cu<0) || (ctx->cu>255) ||
//		(ctx->cv<0) || (ctx->cv>255))
//	{
//		*(int *)-1=-1;
//	}
}

void BTIC1H_DecodeDeltaYUVD(BTIC1H_Context *ctx)
{
	int dd, qdd;

	BTIC1H_DecodeDeltaYUV(ctx);
	qdd=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_ddy));
	ctx->cd+=qdd*ctx->qfd;

//	if((ctx->cd<0) || (ctx->cd>255))
//	{
//		*(int *)-1=-1;
//	}
}


void BTIC1H_DecodeDeltaY(BTIC1H_Context *ctx)
{
	int dy, du, dv;
	int qdy, qdu, qdv;

	qdy=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_dy));
	ctx->cy+=qdy*ctx->qfy;
}

void BTIC1H_DecodeDeltaUV(BTIC1H_Context *ctx)
{
	int dy, du, dv;
	int qdy, qdu, qdv;

	qdu=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_duv));
	qdv=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_duv));
	ctx->cu+=qdu*ctx->qfuv;
	ctx->cv+=qdv*ctx->qfuv;
}


void BTIC1H_DecodeDeltaYUVDyuv(BTIC1H_Context *ctx)
{
	int dy, du, dv;
	int qdy, qdu, qdv;

	BTIC1H_DecodeDeltaYUV(ctx);

	qdy=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_ddy));
	qdu=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_dduv));
	qdv=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_dduv));

	ctx->cdy+=qdy*ctx->qfdy;
	ctx->cdu+=qdu*ctx->qfduv;
	ctx->cdv+=qdv*ctx->qfduv;
}

void BTIC1H_DecodeAbsYUV(BTIC1H_Context *ctx)
{
	int dy, du, dv;
	int qdy, qdu, qdv;

	qdy=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_ay));
	qdu=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_auv));
	qdv=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_auv));

	ctx->cy=ctx->absyuvbias+qdy*ctx->qfay;
	ctx->cu=ctx->absyuvbias+qdu*ctx->qfauv;
	ctx->cv=ctx->absyuvbias+qdv*ctx->qfauv;
}

void BTIC1H_DecodeAbsY(BTIC1H_Context *ctx)
{
	int dy, qdy;
	qdy=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_ay));
	ctx->cy=ctx->absyuvbias+qdy*ctx->qfay;
}

void BTIC1H_DecodeAbsUV(BTIC1H_Context *ctx)
{
	int du, dv;
	int qdu, qdv;
	qdu=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_auv));
	qdv=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_auv));
	ctx->cu=ctx->absyuvbias+qdu*ctx->qfauv;
	ctx->cv=ctx->absyuvbias+qdv*ctx->qfauv;
}


void BTIC1H_DecodeAbsYUVD(BTIC1H_Context *ctx)
{
	int dd, qdd;

	BTIC1H_DecodeAbsYUV(ctx);
	qdd=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_ady));
	ctx->cd=ctx->absyuvbias+qdd*ctx->qfad;
}

void BTIC1H_DecodeAbsYUVDyuv(BTIC1H_Context *ctx)
{
	int dy, du, dv;
	int qdy, qdu, qdv;

	BTIC1H_DecodeAbsYUV(ctx);

	qdy=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_ady));
	qdu=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_aduv));
	qdv=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_aduv));

	ctx->cdy=ctx->absyuvbias+qdy*ctx->qfady;
	ctx->cdu=ctx->absyuvbias+qdu*ctx->qfaduv;
	ctx->cdv=ctx->absyuvbias+qdv*ctx->qfaduv;
}

void BTIC1H_DecodeQfDeltaYUVD(BTIC1H_Context *ctx)
{
	int qdy, qduv, qdd;

	qdy=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_qfy));
	qduv=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_qfuv));
	qdd=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_qfdy));

	ctx->qfy+=qdy;
	ctx->qfuv+=qduv;
	ctx->qfd+=qdd;
}

void BTIC1H_DecodeQfDeltaYUVDyuv(BTIC1H_Context *ctx)
{
	int qdy, qduv, qddy, qdduv;

	qdy=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_qfy));
	qduv=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_qfuv));
	qddy=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_qfdy));
	qdduv=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_qfuv));

	ctx->qfy+=qdy;
	ctx->qfuv+=qduv;
	ctx->qfdy+=qddy;
	ctx->qfduv+=qdduv;
}

void BTIC1H_DecodeQfAbsYUVD(BTIC1H_Context *ctx)
{
	int qdy, qduv, qdd;

	qdy=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_qfy));
	qduv=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_qfuv));
	qdd=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_qfdy));

	ctx->qfay+=qdy;
	ctx->qfauv+=qduv;
	ctx->qfad+=qdd;
}

void BTIC1H_DecodeQfAbsYUVDyuv(BTIC1H_Context *ctx)
{
	int qdy, qduv, qddy, qdduv;

	qdy=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_qfy));
	qduv=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_qfuv));
	qddy=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_qfdy));
	qdduv=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_qfuv));

	ctx->qfay+=qdy;
	ctx->qfauv+=qduv;
	ctx->qfady+=qddy;
	ctx->qfaduv+=qdduv;
}

int BTIC1H_ReadPix8_2x2(BTIC1H_Context *ctx)
{
	int i;
	i=BTIC1H_Rice_Read8Bits(ctx);
	return(i);
}

int BTIC1H_ReadPix8_2x1(BTIC1H_Context *ctx)
{
	int i;
	i=BTIC1H_Rice_Read4Bits(ctx);
	i=i|(i<<4);
	return(i);
}

int BTIC1H_ReadPix8_1x2(BTIC1H_Context *ctx)
{
	int i;
	i=BTIC1H_Rice_Read4Bits(ctx);
//	i=i|(i<<4);
	i=(i&3)|((i&3)<<2)|((i&12)<<2)|((i&12)<<4);
	return(i);
}

int BTIC1H_ReadPix16_4x4x1(BTIC1H_Context *ctx)
{
	int i;
	i=BTIC1H_Rice_Read16Bits(ctx);
	return(i);
}

int BTIC1H_ReadPix16_2x2x1(BTIC1H_Context *ctx)
{
	static const int tab[16]={
		0x0000, 0x0033, 0x00CC, 0x00FF,
		0x3300, 0x3333, 0x33CC, 0x33FF,
		0xCC00, 0xCC33, 0xCCCC, 0xCCFF,
		0xFF00, 0xFF33, 0xFFCC, 0xFFFF};
	int i, j;
	i=BTIC1H_Rice_Read4Bits(ctx);
	j=tab[i];
	return(j);
}

u32 BTIC1H_ReadPix32_4x4(BTIC1H_Context *ctx)
{
	u32 i;
	i=BTIC1H_Rice_Read32Bits(ctx);
	return(i);
}

u32 BTIC1H_ReadPix32_4x2(BTIC1H_Context *ctx)
{
	int i, j, k;
	i=BTIC1H_Rice_Read8Bits(ctx);
	j=BTIC1H_Rice_Read8Bits(ctx);
	k=(i<<24)|(i<<16)|(j<<8)|j;
	return(k);
}

u32 BTIC1H_ReadPix32_2x4(BTIC1H_Context *ctx)
{
	static const byte remap[16]={
		0x00, 0x05, 0x0A, 0x0F,
		0x50, 0x55, 0x5A, 0x5F,
		0xA0, 0xA5, 0xAA, 0xAF,
		0xF0, 0xF5, 0xFA, 0xFF};
	int i0, i1, i2, i3;
	int i, j, k;

	i=BTIC1H_Rice_Read16Bits(ctx);
	i0=remap[(i>>12)&15];
	i1=remap[(i>> 8)&15];
	i2=remap[(i>> 4)&15];
	i3=remap[(i    )&15];
	
	k=(i0<<24)|(i1<<16)|(i2<<8)|i3;
	return(k);
}

u64 BTIC1H_ReadPix48_4x4x3(BTIC1H_Context *ctx)
{
	u64 i;
	i=BTIC1H_Rice_Read48Bits(ctx);
	return(i);
}

static const int btic1h_dec_hilbert4x4[16]=
	{ 0, 1, 5, 4, 8, 12, 13, 9, 10, 14, 15, 11, 7, 6, 2, 3 };
static const int btic1h_dec_hilbert2x2[4]=
	{ 0, 2, 3, 1 };

void BTIC1H_ReadPixDeltaY4x4(BTIC1H_Context *ctx, byte *yblk)
{
	int i;

	for(i=0; i<16; i++)
	{
		BTIC1H_DecodeDeltaY(ctx);
		yblk[btic1h_dec_hilbert4x4[i]]=ctx->cy;
	}
}

void BTIC1H_ReadPixDeltaUV2x2(BTIC1H_Context *ctx, byte *ublk, byte *vblk)
{
	int i;

	for(i=0; i<4; i++)
	{
		BTIC1H_DecodeDeltaUV(ctx);
		ublk[btic1h_dec_hilbert2x2[i]]=ctx->cu;
		vblk[btic1h_dec_hilbert2x2[i]]=ctx->cv;
	}
}

void BTIC1H_ReadPixDeltaYUV4x4(BTIC1H_Context *ctx,
	byte *yblk, byte *ublk, byte *vblk)
{
	int i;

	for(i=0; i<16; i++)
	{
		BTIC1H_DecodeDeltaYUV(ctx);
		yblk[btic1h_dec_hilbert4x4[i]]=ctx->cy;
		ublk[btic1h_dec_hilbert4x4[i]]=ctx->cu;
		vblk[btic1h_dec_hilbert4x4[i]]=ctx->cv;
	}
}

void BTIC1H_ReadPixDeltaYUV2x2(BTIC1H_Context *ctx,
	byte *yblk, byte *ublk, byte *vblk)
{
	int i;

	for(i=0; i<4; i++)
	{
		BTIC1H_DecodeDeltaYUV(ctx);
		yblk[btic1h_dec_hilbert2x2[i]]=ctx->cy;
		ublk[btic1h_dec_hilbert2x2[i]]=ctx->cu;
		vblk[btic1h_dec_hilbert2x2[i]]=ctx->cv;
	}
}

void BTIC1H_ReadPixDeltaYUVD2x2(BTIC1H_Context *ctx,
	byte *yblk, byte *ublk, byte *vblk, byte *dblk)
{
	int i;

	for(i=0; i<4; i++)
	{
		BTIC1H_DecodeDeltaYUVD(ctx);
		yblk[btic1h_dec_hilbert2x2[i]]=ctx->cy;
		ublk[btic1h_dec_hilbert2x2[i]]=ctx->cu;
		vblk[btic1h_dec_hilbert2x2[i]]=ctx->cv;
		dblk[btic1h_dec_hilbert2x2[i]]=ctx->cd;
	}
}


void BTIC1H_ReadPixAbsY4x4(BTIC1H_Context *ctx, byte *yblk)
{
	int i;

	for(i=0; i<16; i++)
	{
		BTIC1H_DecodeAbsY(ctx);
		yblk[btic1h_dec_hilbert4x4[i]]=ctx->cy;
	}
}

void BTIC1H_ReadPixAbsUV2x2(BTIC1H_Context *ctx, byte *ublk, byte *vblk)
{
	int i;

	for(i=0; i<4; i++)
	{
		BTIC1H_DecodeAbsUV(ctx);
		ublk[btic1h_dec_hilbert2x2[i]]=ctx->cu;
		vblk[btic1h_dec_hilbert2x2[i]]=ctx->cv;
	}
}

void BTIC1H_ReadPixAbsYUV4x4(BTIC1H_Context *ctx,
	byte *yblk, byte *ublk, byte *vblk)
{
	int i;

	for(i=0; i<16; i++)
	{
		BTIC1H_DecodeAbsYUV(ctx);
		yblk[btic1h_dec_hilbert4x4[i]]=ctx->cy;
		ublk[btic1h_dec_hilbert4x4[i]]=ctx->cu;
		vblk[btic1h_dec_hilbert4x4[i]]=ctx->cv;
	}
}


byte *BTIC1H_FillBlockFlat(BTIC1H_Context *ctx, byte *ct, int stride)
{
	ct[0]=ctx->cy;		ct[1]=ctx->cu;
	ct[2]=ctx->cv;		ct[3]=0;
	ct[4]=0;			ct[5]=0;
	ct[6]=0;			ct[7]=0;
	ct+=stride;
	return(ct);
}

byte *BTIC1H_FillBlock2x2(BTIC1H_Context *ctx,
	int px, byte *ct, int stride)
{
	ct[0]=ctx->cy;		ct[1]=ctx->cu;
	ct[2]=ctx->cv;		ct[3]=0;
	ct[4]=1;			ct[5]=ctx->cd;
	ct[6]=px;			ct[7]=0;
	ct+=stride;
	return(ct);
}

byte *BTIC1H_FillBlock4x4(BTIC1H_Context *ctx,
	int px, byte *ct, int stride)
{
	ct[0]=ctx->cy;		ct[1]=ctx->cu;
	ct[2]=ctx->cv;		ct[3]=ctx->cd;
	ct[4]=(px>>24);		ct[5]=(px>>16);
	ct[6]=(px>> 8);		ct[7]=(px    );
	ct+=stride;
	return(ct);
}

byte *BTIC1H_FillBlock4x4x1(BTIC1H_Context *ctx,
	int px, byte *ct, int stride)
{
	ct[0]=ctx->cy;		ct[1]=ctx->cu;
	ct[2]=ctx->cv;		ct[3]=0;
	ct[4]=2;			ct[5]=ctx->cd;
	ct[6]=(px>>8);		ct[7]=px;
	ct+=stride;
	return(ct);
}

byte *BTIC1H_FillBlock4x4x3(BTIC1H_Context *ctx,
	u64 px, byte *ct, int stride)
{
	ct[ 0]=ctx->cy;		ct[ 1]=ctx->cu;
	ct[ 2]=ctx->cv;		ct[ 3]=0;
	ct[ 4]=6;			ct[ 5]=ctx->cd;
	ct[ 6]=0;			ct[ 7]=0;
	ct[ 8]=0;			ct[ 9]=0;
	ct[10]=(px>>40);	ct[11]=(px>>32);
	ct[12]=(px>>24);	ct[13]=(px>>16);
	ct[14]=(px>> 8);	ct[15]=(px    );
	ct+=stride;
	return(ct);
}

byte *BTIC1H_FillBlock2x2B(BTIC1H_Context *ctx,
	int px, byte *ct, int stride)
{
	ct[0]=ctx->cy;		ct[1]=ctx->cu;
	ct[2]=ctx->cv;		ct[3]=0;
	ct[4]=1;			ct[5]=ctx->cdy;
	ct[6]=px;			ct[7]=0;
	ct+=stride;
	return(ct);
}

byte *BTIC1H_FillBlock4x4B(BTIC1H_Context *ctx,
	int px, byte *ct, int stride)
{
	ct[0]=ctx->cy;		ct[1]=ctx->cu;
	ct[2]=ctx->cv;		ct[3]=ctx->cdy;
	ct[4]=(px>>24);		ct[5]=(px>>16);
	ct[6]=(px>> 8);		ct[7]=(px    );
	ct+=stride;
	return(ct);
}

byte *BTIC1H_FillBlock4x4x3B(BTIC1H_Context *ctx,
	u64 px, byte *ct, int stride)
{
	ct[ 0]=ctx->cy;		ct[ 1]=ctx->cu;
	ct[ 2]=ctx->cv;		ct[ 3]=0;
	ct[ 4]=2;			ct[ 5]=ctx->cdy;
	ct[ 6]=0;			ct[ 7]=0;
	ct[ 8]=0;			ct[ 9]=0;
	ct[10]=(px>>40);	ct[11]=(px>>32);
	ct[12]=(px>>24);	ct[13]=(px>>16);
	ct[14]=(px>> 8);	ct[15]=(px    );
	ct+=stride;
	return(ct);
}

byte *BTIC1H_FillBlock4x4B420(BTIC1H_Context *ctx,
	byte *ypix, byte *upix, byte *vpix, byte *dpix,
	int px, byte *ct, int stride)
{
	ct[ 0]=ypix[0];		ct[ 1]=upix[0];
	ct[ 2]=vpix[0];		ct[ 3]=0;
	ct[ 4]=17;			ct[ 5]=dpix[0];
	ct[ 6]=0;			ct[ 7]=0;
	ct[ 8]=ypix[1];		ct[ 9]=upix[1];
	ct[10]=vpix[1];		ct[11]=dpix[1];
	ct[12]=(px>>24);	ct[13]=(px>>16);
	ct[14]=(px>> 8);	ct[15]=(px    );
	ct[16]=ypix[2];		ct[17]=upix[2];
	ct[18]=vpix[2];		ct[19]=dpix[2];
	ct[20]=ypix[3];		ct[21]=upix[3];
	ct[22]=vpix[3];		ct[23]=dpix[3];

	ct+=stride;
	return(ct);
}

byte *BTIC1H_FillBlock4x4YUV420(BTIC1H_Context *ctx,
	byte *ypix, byte *upix, byte *vpix,
	byte *ct, int stride)
{
	if(stride>=32)
	{
		ct[ 0]=ctx->cy;		ct[ 1]=ctx->cu;
		ct[ 2]=ctx->cv;		ct[ 3]=0;
		ct[ 4]=7;			ct[ 5]=ctx->cd;
		ct[ 6]=0;			ct[ 7]=0;
		ct[ 8]=upix[ 0];	ct[ 9]=upix[ 1];
		ct[10]=upix[ 2];	ct[11]=upix[ 3];
		ct[12]=vpix[ 0];	ct[13]=vpix[ 1];
		ct[14]=vpix[ 2];	ct[15]=vpix[ 3];
		ct[16]=ypix[ 0];	ct[17]=ypix[ 1];
		ct[18]=ypix[ 2];	ct[19]=ypix[ 3];
		ct[20]=ypix[ 4];	ct[21]=ypix[ 5];
		ct[22]=ypix[ 6];	ct[23]=ypix[ 7];
		ct[24]=ypix[ 8];	ct[25]=ypix[ 9];
		ct[26]=ypix[10];	ct[27]=ypix[11];
		ct[28]=ypix[12];	ct[29]=ypix[13];
		ct[30]=ypix[14];	ct[31]=ypix[15];

		ct+=stride;
		return(ct);
	}

	ct[ 0]=ctx->cy;		ct[ 1]=ctx->cu;
	ct[ 2]=ctx->cv;		ct[ 3]=0;
	ct[ 4]=2;			ct[ 5]=ctx->cdy;
	ct[ 6]=0;			ct[ 7]=0;
	ct[ 8]=0;			ct[ 9]=0;
//	ct[10]=(px>>40);	ct[11]=(px>>32);
//	ct[12]=(px>>24);	ct[13]=(px>>16);
//	ct[14]=(px>> 8);	ct[15]=(px    );
	ct+=stride;
	return(ct);
}

byte *BTIC1H_FillBlock4x4YUV444(BTIC1H_Context *ctx,
	byte *ypix, byte *upix, byte *vpix,
	byte *ct, int stride)
{
	byte upix2[4], vpix2[4];
	
	upix2[0]=(upix[ 0]+upix[ 1]+upix[ 4]+upix[ 5])>>2;
	upix2[1]=(upix[ 2]+upix[ 3]+upix[ 6]+upix[ 7])>>2;
	upix2[2]=(upix[ 8]+upix[ 9]+upix[12]+upix[13])>>2;
	upix2[3]=(upix[10]+upix[11]+upix[14]+upix[15])>>2;
	vpix2[0]=(vpix[ 0]+vpix[ 1]+vpix[ 4]+vpix[ 5])>>2;
	vpix2[1]=(vpix[ 2]+vpix[ 3]+vpix[ 6]+vpix[ 7])>>2;
	vpix2[2]=(vpix[ 8]+vpix[ 9]+vpix[12]+vpix[13])>>2;
	vpix2[3]=(vpix[10]+vpix[11]+vpix[14]+vpix[15])>>2;
	
	ct=BTIC1H_FillBlock4x4YUV420(ctx, ypix, upix2, vpix2, ct, stride);
	return(ct);
}

u64 BTIC1H_FillBlockGr2x2_Repack(int px)
{
	int p0, p1, p2, p3;
	int qa0, qa1, qa2, qa3;
	int qb0, qb1, qb2, qb3;
	int qc0, qc1, qc2, qc3;
	int qd0, qd1, qd2, qd3;
	int pxr0, pxr1, pxr2, pxr3;
	u64 px1;

	p0=(px>>6)&3;			p1=(px>>4)&3;
	p2=(px>>2)&3;			p3=(px   )&3;
	p0=(p0<<1)|(p0>>1);		p1=(p1<<1)|(p1>>1);
	p2=(p2<<1)|(p2>>1);		p3=(p3<<1)|(p3>>1);

	qa0=p0; qa3=p1; qa1=(11*qa0+5*qa3+8)>>4; qa2=(5*qa0+11*qa3+8)>>4;
	qd0=p0; qd3=p1; qd1=(11*qd0+5*qd3+8)>>4; qd2=(5*qd0+11*qd3+8)>>4;
	qb0=(11*qa0+ 5*qd0+8)>>4;	qb1=(11*qa1+ 5*qd1+8)>>4;
	qb2=(11*qa2+ 5*qd2+8)>>4;	qb3=(11*qa3+ 5*qd3+8)>>4;
	qc0=( 5*qa0+11*qd0+8)>>4;	qc1=( 5*qa1+11*qd1+8)>>4;
	qc2=( 5*qa2+11*qd2+8)>>4;	qc3=( 5*qa3+11*qd3+8)>>4;

	pxr0=(qa0<<9)|(qa1<<6)|(qa2<<3)|qa3;
	pxr1=(qb0<<9)|(qb1<<6)|(qb2<<3)|qb3;
	pxr2=(qc0<<9)|(qc1<<6)|(qc2<<3)|qc3;
	pxr3=(qd0<<9)|(qd1<<6)|(qd2<<3)|qd3;

	px1=(((u64)pxr0)<<36)|(((u64)pxr1)<<24)|(pxr2<<12)|pxr3;
	
	return(px1);
}

byte *BTIC1H_FillBlockGr2x2(BTIC1H_Context *ctx,
	int px, byte *ct, int stride)
{
	u64 px1;
	px1=BTIC1H_FillBlockGr2x2_Repack(px);
	ct=BTIC1H_FillBlock4x4x3(ctx, px1, ct, stride);
	return(ct);
}

byte *BTIC1H_FillBlockGr2x2B(BTIC1H_Context *ctx,
	int px, byte *ct, int stride)
{
	u64 px1;
	px1=BTIC1H_FillBlockGr2x2_Repack(px);
	ct=BTIC1H_FillBlock4x4x3B(ctx, px1, ct, stride);
	return(ct);
}

int BTIC1H_DecodeBlocksCtx(BTIC1H_Context *ctx,
	byte *blks, byte *lblks, int nblks, int stride,
	int *rnblks)
{
	byte ypix[16], upix[16], vpix[16], dpix[16];
	byte *ct, *cte, *csl;
	u64 lpx;
	int cmd, ret;
	int n, xo, yo;
	int i, j, k;

	ct=blks; cte=ct+nblks*stride; csl=lblks;
	
	ret=0;
	while((ct<cte) && !ret)
	{
		cmd=BTIC1H_ReadCommandCode(ctx);
		switch(cmd)
		{
		case 0x00:
			BTIC1H_DecodeDeltaYUV(ctx);
			ct=BTIC1H_FillBlockFlat(ctx, ct, stride);
			break;
		case 0x01:
			BTIC1H_DecodeDeltaYUVD(ctx);
			i=BTIC1H_ReadPix8_2x2(ctx);
			ct=BTIC1H_FillBlock2x2(ctx, i, ct, stride);
			break;
		case 0x02:
			BTIC1H_DecodeDeltaYUVD(ctx);
			i=BTIC1H_ReadPix8_2x1(ctx);
			ct=BTIC1H_FillBlock2x2(ctx, i, ct, stride);
			break;
		case 0x03:
			BTIC1H_DecodeDeltaYUVD(ctx);
			i=BTIC1H_ReadPix8_1x2(ctx);
			ct=BTIC1H_FillBlock2x2(ctx, i, ct, stride);
			break;
		case 0x04:
			BTIC1H_DecodeQfDeltaYUVD(ctx);
			break;
		case 0x05:
			BTIC1H_DecodeDeltaYUVD(ctx);
			i=BTIC1H_ReadPix32_4x4(ctx);
			ct=BTIC1H_FillBlock4x4(ctx, i, ct, stride);
			break;
		case 0x06:
			BTIC1H_DecodeDeltaYUVD(ctx);
			i=BTIC1H_ReadPix32_4x2(ctx);
			ct=BTIC1H_FillBlock4x4(ctx, i, ct, stride);
			break;
		case 0x07:
			BTIC1H_DecodeDeltaYUVD(ctx);
			i=BTIC1H_ReadPix32_2x4(ctx);
			ct=BTIC1H_FillBlock4x4(ctx, i, ct, stride);
			break;
		case 0x08:
			BTIC1H_DecodeQfDeltaYUVDyuv(ctx);
			break;
		case 0x09:
			BTIC1H_DecodeDeltaYUVDyuv(ctx);
			i=BTIC1H_ReadPix32_4x4(ctx);
			ct=BTIC1H_FillBlock4x4B(ctx, i, ct, stride);
			break;
		case 0x0A:
			BTIC1H_DecodeDeltaYUVDyuv(ctx);
			i=BTIC1H_ReadPix32_4x2(ctx);
			ct=BTIC1H_FillBlock4x4B(ctx, i, ct, stride);
			break;
		case 0x0B:
			BTIC1H_DecodeDeltaYUVDyuv(ctx);
			i=BTIC1H_ReadPix32_2x4(ctx);
			ct=BTIC1H_FillBlock4x4B(ctx, i, ct, stride);
			break;
		case 0x0C:
			BTIC1H_DecodeAbsYUVD(ctx);
			i=BTIC1H_ReadPix8_2x2(ctx);
			ct=BTIC1H_FillBlock2x2(ctx, i, ct, stride);
			break;
		case 0x0D:
			BTIC1H_DecodeAbsYUVD(ctx);
			i=BTIC1H_ReadPix32_4x4(ctx);
			ct=BTIC1H_FillBlock4x4(ctx, i, ct, stride);
			break;
		case 0x0E:
			BTIC1H_DecodeAbsYUVDyuv(ctx);
			i=BTIC1H_ReadPix8_2x2(ctx);
			ct=BTIC1H_FillBlock2x2B(ctx, i, ct, stride);
			break;
		case 0x0F:
			BTIC1H_DecodeAbsYUVDyuv(ctx);
			i=BTIC1H_ReadPix32_4x4(ctx);
			ct=BTIC1H_FillBlock4x4B(ctx, i, ct, stride);
			ct+=stride;
			break;
		case 0x10:
			n=BTIC1H_Rice_ReadAdRice(ctx, &(ctx->rk_cmdcnt));
			for(i=0; i<n; i++)
				{ ct=BTIC1H_FillBlockFlat(ctx, ct, stride); }
			break;
		case 0x11:
			n=BTIC1H_Rice_ReadAdRice(ctx, &(ctx->rk_cmdcnt));
			for(i=0; i<n; i++)
			{
				j=BTIC1H_ReadPix8_2x2(ctx);
				ct=BTIC1H_FillBlock2x2(ctx, j, ct, stride);
			}
			break;
		case 0x12:
			n=BTIC1H_Rice_ReadAdRice(ctx, &(ctx->rk_cmdcnt));
			for(i=0; i<n; i++)
			{
				j=BTIC1H_ReadPix8_2x1(ctx);
				ct=BTIC1H_FillBlock2x2(ctx, j, ct, stride);
			}
			break;
		case 0x13:
			n=BTIC1H_Rice_ReadAdRice(ctx, &(ctx->rk_cmdcnt));
			for(i=0; i<n; i++)
			{
				j=BTIC1H_ReadPix8_1x2(ctx);
				ct=BTIC1H_FillBlock2x2(ctx, j, ct, stride);
			}
			break;
		case 0x14:
			BTIC1H_DecodeQfAbsYUVD(ctx);
			break;
		case 0x15:
			n=BTIC1H_Rice_ReadAdRice(ctx, &(ctx->rk_cmdcnt));
			for(i=0; i<n; i++)
			{
				j=BTIC1H_ReadPix32_4x4(ctx);
				ct=BTIC1H_FillBlock4x4(ctx, j, ct, stride);
			}
			break;
		case 0x16:
			n=BTIC1H_Rice_ReadAdRice(ctx, &(ctx->rk_cmdcnt));
			for(i=0; i<n; i++)
			{
				j=BTIC1H_ReadPix32_4x2(ctx);
				ct=BTIC1H_FillBlock4x4(ctx, j, ct, stride);
			}
			break;
		case 0x17:
			n=BTIC1H_Rice_ReadAdRice(ctx, &(ctx->rk_cmdcnt));
			for(i=0; i<n; i++)
			{
				j=BTIC1H_ReadPix32_2x4(ctx);
				ct=BTIC1H_FillBlock4x4(ctx, j, ct, stride);
			}
			break;
		case 0x18:
			BTIC1H_DecodeQfAbsYUVDyuv(ctx);
			break;
		case 0x19:
			n=BTIC1H_Rice_ReadAdRice(ctx, &(ctx->rk_cmdcnt));
			for(i=0; i<n; i++)
			{
				j=BTIC1H_ReadPix16_2x2x1(ctx);
				ct=BTIC1H_FillBlock4x4x1(ctx, j, ct, stride);
			}
			break;
		case 0x1A:
			n=BTIC1H_Rice_ReadAdRice(ctx, &(ctx->rk_cmdcnt));
			for(i=0; i<n; i++)
			{
				j=BTIC1H_ReadPix16_4x4x1(ctx);
				ct=BTIC1H_FillBlock4x4x1(ctx, j, ct, stride);
			}
			break;
		case 0x1B:
			n=BTIC1H_Rice_ReadAdRice(ctx, &(ctx->rk_cmdcnt));
			for(i=0; i<n; i++)
			{
				BTIC1H_DecodeDeltaYUV(ctx);
				ct=BTIC1H_FillBlockFlat(ctx, ct, stride);
			}
			break;
		case 0x1C:
			BTIC1H_DecodeDeltaYUV(ctx);
			ypix[0]=ctx->cy;	upix[0]=ctx->cu;
			vpix[0]=ctx->cv;	dpix[0]=0;
			BTIC1H_DecodeDeltaYUV(ctx);
			ypix[2]=ctx->cy;	upix[2]=ctx->cu;
			vpix[2]=ctx->cv;	dpix[2]=0;
			BTIC1H_DecodeDeltaYUV(ctx);
			ypix[3]=ctx->cy;	upix[3]=ctx->cu;
			vpix[3]=ctx->cv;	dpix[3]=0;
			BTIC1H_DecodeDeltaYUV(ctx);
			ypix[1]=ctx->cy;	upix[1]=ctx->cu;
			vpix[1]=ctx->cv;	dpix[1]=0;
			ct=BTIC1H_FillBlock4x4B420(ctx,
				ypix, upix, vpix, dpix,
				0, ct, stride);
			break;
		case 0x1D:
			BTIC1H_DecodeDeltaYUVD(ctx);
			ypix[0]=ctx->cy;	upix[0]=ctx->cu;
			vpix[0]=ctx->cv;	dpix[0]=ctx->cd;
			BTIC1H_DecodeDeltaYUVD(ctx);
			ypix[2]=ctx->cy;	upix[2]=ctx->cu;
			vpix[2]=ctx->cv;	dpix[2]=ctx->cd;
			BTIC1H_DecodeDeltaYUVD(ctx);
			ypix[3]=ctx->cy;	upix[3]=ctx->cu;
			vpix[3]=ctx->cv;	dpix[3]=ctx->cd;
			BTIC1H_DecodeDeltaYUVD(ctx);
			ypix[1]=ctx->cy;	upix[1]=ctx->cu;
			vpix[1]=ctx->cv;	dpix[1]=ctx->cd;
			j=BTIC1H_ReadPix32_4x4(ctx);
			ct=BTIC1H_FillBlock4x4B420(ctx,
				ypix, upix, vpix, dpix,
				j, ct, stride);
			break;
		case 0x1E:
			BTIC1H_ReadPixDeltaYUV4x4(ctx, ypix, upix, vpix);
			ct=BTIC1H_FillBlock4x4YUV444(ctx,
				ypix, upix, vpix, ct, stride);
			break;
		case 0x1F:
			BTIC1H_ReadPixAbsYUV4x4(ctx, ypix, upix, vpix);
			ct=BTIC1H_FillBlock4x4YUV444(ctx,
				ypix, upix, vpix, ct, stride);
			break;
		case 0x20:
			ret=1;
			break;
		case 0x21:
			n=BTIC1H_Rice_ReadAdRice(ctx, &(ctx->rk_cmdcnt));
			if(!lblks)break;
			csl=lblks+(ct-blks);
			memcpy(ct, csl, n*stride);
			ct=ct+(n*stride);
			break;
		case 0x22:
			n=BTIC1H_Rice_ReadAdRice(ctx, &(ctx->rk_cmdcnt));
			xo=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_parmxy));
			yo=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_parmxy));
			if(!lblks)break;
			csl=lblks+(ct-blks)+((yo*ctx->xbsz+xo)*stride);
			memcpy(ct, csl, n*stride);
			ct=ct+(n*stride);
			break;
		case 0x23:
			xo=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_parmvar));
			yo=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_parmval));
			if(xo<0)
			{
				if(xo==-1)
					{ if(yo==0)break; }
				ret=-1; break;
			}
			break;
		case 0x24:
			xo=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_parmvar));
			k=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_parmix));
			n=BTIC1H_Rice_ReadAdRice(ctx, &(ctx->rk_parmix));
			for(i=0; i<n; i++)
			{
				yo=BTIC1H_Rice_ReadAdSRice(ctx, &(ctx->rk_parmval));
			}
			if(xo<0) { ret=-1; break; }
			break;

		case 0x28:
			n=BTIC1H_Rice_ReadAdRice(ctx, &(ctx->rk_cmdcnt));
			for(i=0; i<n; i++)
			{
				BTIC1H_DecodeDeltaYUVD(ctx);
				j=BTIC1H_ReadPix8_2x2(ctx);
				ct=BTIC1H_FillBlockGr2x2(ctx, j, ct, stride);
			}
			break;
		case 0x29:
			n=BTIC1H_Rice_ReadAdRice(ctx, &(ctx->rk_cmdcnt));
			for(i=0; i<n; i++)
			{
				BTIC1H_DecodeDeltaYUVDyuv(ctx);
				j=BTIC1H_ReadPix8_2x2(ctx);
				ct=BTIC1H_FillBlockGr2x2B(ctx, j, ct, stride);
			}
			break;

		case 0x2A:
			BTIC1H_DecodeDeltaYUVD(ctx);
			i=BTIC1H_ReadPix16_4x4x1(ctx);
			ct=BTIC1H_FillBlock4x4x1(ctx, i, ct, stride);
			break;
		case 0x2B:
			BTIC1H_DecodeDeltaYUVD(ctx);
			i=BTIC1H_ReadPix16_2x2x1(ctx);
			ct=BTIC1H_FillBlock4x4x1(ctx, i, ct, stride);
			break;
		case 0x2C:
			BTIC1H_DecodeDeltaYUVD(ctx);
			lpx=BTIC1H_ReadPix48_4x4x3(ctx);
			ct=BTIC1H_FillBlock4x4x3(ctx, lpx, ct, stride);
			break;
		case 0x2D:
			BTIC1H_DecodeDeltaYUVDyuv(ctx);
			lpx=BTIC1H_ReadPix48_4x4x3(ctx);
			ct=BTIC1H_FillBlock4x4x3B(ctx, lpx, ct, stride);
			break;
		case 0x2E:
			BTIC1H_ReadPixDeltaY4x4(ctx, ypix);
			BTIC1H_ReadPixDeltaUV2x2(ctx, upix, vpix);
			ct=BTIC1H_FillBlock4x4YUV420(ctx,
				ypix, upix, vpix, ct, stride);
			break;
		case 0x2F:
			BTIC1H_ReadPixAbsY4x4(ctx, ypix);
			BTIC1H_ReadPixAbsUV2x2(ctx, upix, vpix);
			ct=BTIC1H_FillBlock4x4YUV420(ctx,
				ypix, upix, vpix, ct, stride);
			break;

		default:
			ret=-1;
			break;
		}
	}
	return(ret);
}

int BTIC1H_DecodeBlocksBuffer(
	byte *ibuf, byte *blks, byte *lblks, int nblks, int stride,
	int xbsz, int ybsz, int ibsz)
{
	BTIC1H_Context tctx;
	BTIC1H_Context *ctx;
	int i;
	
	ctx=&tctx;
	memset(ctx, 0, sizeof(BTIC1H_Context));

	BTIC1H_SetupContextInitial(ctx);

	ctx->xbsz=xbsz;
	ctx->ybsz=ybsz;

	BTIC1H_Rice_SetupRead(ctx, ibuf, ibsz);
	BTIC1H_DecodeBlocksCtx(ctx, blks, lblks, nblks, stride, &i);
	return(i);
}

int BTIC1H_DecodeCtx(BTIC1H_Context *ctx,
	byte *src, byte *dst, int ssz, int dsz,
	int *rxy, int *rys, int clrs)
{
	byte *cs, *cse, *csi;
	int i;

	if(!ctx->blks)
	{
		ctx->xbsz=(ctx->xs+3)>>2;
		ctx->ybsz=(ctx->ys+3)>>2;
		ctx->nblks=ctx->xbsz*ctx->ybsz;
		
		ctx->blks=malloc(ctx->nblks*32);
		ctx->lblks=malloc(ctx->nblks*32);
	}

	cs=src; cse=src+ssz; csi=NULL;
	while(cs<cse)
	{
		if(*cs==0xE0)
			break;
		if(*cs==0xE1)
		{
			csi=cs;
			i=(cs[1]<<16)|(cs[2]<<8)|cs[3];
			cs=cs+i;
			continue;
		}else if(*cs==0xE2)
		{
			csi=cs;
			cs=cs+cs[1];
			continue;
		}else if((*cs==0xE3) || (*cs==0xE4))
		{
			i=(cs[1]<<16)|(cs[2]<<8)|cs[3];
			cs=cs+i;
			continue;
		}else if(*cs==0xE5)
		{
			cs=cs+cs[1];
			continue;
		}
		
		return(-1);
	}

	BTIC1H_SetupContextInitial(ctx);

	if(!csi)
		return(-1);

	if(csi[0]==0xE1)
	{
		i=(csi[1]<<16)|(csi[2]<<8)|csi[3];
		cs=csi+4; cse=csi+i;
	}else if(csi[0]==0xE2)
	{
		i=csi[1];
		cs=csi+2; cse=csi+i;
	}else
	{
		return(-1);
	}

	BTIC1H_Rice_SetupRead(ctx, cs, cse-cs);
//	BTIC1H_Rice_SetupRead(ctx, src+4, ssz-4);
	BTIC1H_DecodeBlocksCtx(ctx,
		ctx->blks, ctx->lblks,
		ctx->nblks, 32, &i);
	memcpy(ctx->lblks, ctx->blks, ctx->nblks*32);
	
	BTIC1H_DecodeImageMB2B_Clrs(ctx->blks, 32, dst,
		ctx->xs, ctx->flip?ctx->ys:(-ctx->ys), clrs);
	return(0);
}
