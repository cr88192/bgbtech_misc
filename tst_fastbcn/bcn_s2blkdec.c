void BGBBTJ_S2TC_DecodeBlockRGBA(byte *block,
	byte *rgba, int xstride, int ystride, int pfb)
{
	int clri[4];
	byte *clr, *ct;
	int *cti;
	int i0, i1, i2, i3;
	int cxa, cxb, px;
	int cra, cga, cba, crb, cgb, cbb;
	
	clr=(byte *)clri;
	cxa=block[0]|(block[1]<<8);
	cxb=block[2]|(block[3]<<8);
	
//	cxa=0x5555;	
//	cxb=0x5555;	

	cra=(cxa>>8)&0xF8; cra=cra|(cra>>5);
	cga=(cxa>>3)&0xFC; cga=cga|(cga>>6);
	cba=(cxa<<3)&0xF8; cba=cba|(cba>>5);
	crb=(cxb>>8)&0xF8; crb=crb|(crb>>5);
	cgb=(cxb>>3)&0xFC; cgb=cgb|(cgb>>6);
	cbb=(cxb<<3)&0xF8; cbb=cbb|(cbb>>5);
	
	clr[0]=cra;	clr[1]=cga;
	clr[2]=cba;	clr[3]=255;
	clr[4]=crb;	clr[5]=cgb;
	clr[6]=cbb;	clr[7]=255;

//	clr[ 8]=cra;
//	clr[ 9]=cga;
//	clr[10]=cba;
	clr[11]=255;

//	clr[12]=crb;
//	clr[13]=cgb;
//	clr[14]=cbb;
	clr[15]=0;

	clri[2]=(clri[0]+clri[1])>>1;
	clri[3]=(clri[0]+clri[1])>>1;
	clr[15]=0;
	
	if(xstride==4)
	{
		cti=(int *)(rgba+0); px=block[4];
		cti[0]=clri[(px   )&3];	cti[1]=clri[(px>>2)&3];
		cti[2]=clri[(px>>4)&3];	cti[3]=clri[(px>>6)&3];
		cti=(int *)(rgba+ystride); px=block[5];
		cti[0]=clri[(px   )&3];	cti[1]=clri[(px>>2)&3];
		cti[2]=clri[(px>>4)&3];	cti[3]=clri[(px>>6)&3];
		cti=(int *)(rgba+2*ystride); px=block[6];
		cti[0]=clri[(px   )&3];	cti[1]=clri[(px>>2)&3];
		cti[2]=clri[(px>>4)&3];	cti[3]=clri[(px>>6)&3];
		cti=(int *)(rgba+3*ystride); px=block[7];
		cti[0]=clri[(px   )&3];	cti[1]=clri[(px>>2)&3];
		cti[2]=clri[(px>>4)&3];	cti[3]=clri[(px>>6)&3];
		return;
	}else if(xstride==3)
	{
		ct=rgba+0; px=block[4];
		i0=clri[(px   )&3];		i1=clri[(px>>2)&3];
		i2=clri[(px>>4)&3];		i3=clri[(px>>6)&3];
		ct[0]=i0; ct[ 1]=i0>>8; ct[ 2]=i0>>16;
		ct[3]=i1; ct[ 4]=i1>>8; ct[ 5]=i1>>16;
		ct[6]=i2; ct[ 7]=i2>>8; ct[ 8]=i2>>16;
		ct[9]=i3; ct[10]=i3>>8; ct[11]=i3>>16;
		ct=rgba+ystride; px=block[5];
		i0=clri[(px   )&3];		i1=clri[(px>>2)&3];
		i2=clri[(px>>4)&3];		i3=clri[(px>>6)&3];
		ct[0]=i0; ct[ 1]=i0>>8; ct[ 2]=i0>>16;
		ct[3]=i1; ct[ 4]=i1>>8; ct[ 5]=i1>>16;
		ct[6]=i2; ct[ 7]=i2>>8; ct[ 8]=i2>>16;
		ct[9]=i3; ct[10]=i3>>8; ct[11]=i3>>16;
		ct=rgba+2*ystride; px=block[6];
		i0=clri[(px   )&3];		i1=clri[(px>>2)&3];
		i2=clri[(px>>4)&3];		i3=clri[(px>>6)&3];
		ct[0]=i0; ct[ 1]=i0>>8; ct[ 2]=i0>>16;
		ct[3]=i1; ct[ 4]=i1>>8; ct[ 5]=i1>>16;
		ct[6]=i2; ct[ 7]=i2>>8; ct[ 8]=i2>>16;
		ct[9]=i3; ct[10]=i3>>8; ct[11]=i3>>16;
		ct=rgba+3*ystride; px=block[7];
		i0=clri[(px   )&3];		i1=clri[(px>>2)&3];
		i2=clri[(px>>4)&3];		i3=clri[(px>>6)&3];
		ct[0]=i0; ct[ 1]=i0>>8; ct[ 2]=i0>>16;
		ct[3]=i1; ct[ 4]=i1>>8; ct[ 5]=i1>>16;
		ct[6]=i2; ct[ 7]=i2>>8; ct[ 8]=i2>>16;
		ct[9]=i3; ct[10]=i3>>8; ct[11]=i3>>16;
		return;
	}
}

void BGBBTJ_S2TC_DecodeBlockBC2A(byte *block,
	byte *rgba, int xstride, int ystride, int pfb)
{
	byte *ct;
	int i0, i1, i2, i3;
	
	i0=(block[0]<<4)&0xF0; i0=i0|(i0>>4);
	i1=(block[0]   )&0xF0; i1=i1|(i1>>4);
	i2=(block[1]<<4)&0xF0; i2=i2|(i2>>4);
	i3=(block[1]   )&0xF0; i3=i3|(i3>>4);
	ct=rgba;
	*ct=i0; ct+=xstride; *ct=i1; ct+=xstride;
	*ct=i2; ct+=xstride; *ct=i3; ct+=xstride;

	i0=(block[2]<<4)&0xF0; i0=i0|(i0>>4);
	i1=(block[2]   )&0xF0; i1=i1|(i1>>4);
	i2=(block[3]<<4)&0xF0; i2=i2|(i2>>4);
	i3=(block[3]   )&0xF0; i3=i3|(i3>>4);
	ct=rgba+ystride;
	*ct=i0; ct+=xstride; *ct=i1; ct+=xstride;
	*ct=i2; ct+=xstride; *ct=i3; ct+=xstride;

	i0=(block[4]<<4)&0xF0; i0=i0|(i0>>4);
	i1=(block[4]   )&0xF0; i1=i1|(i1>>4);
	i2=(block[5]<<4)&0xF0; i2=i2|(i2>>4);
	i3=(block[5]   )&0xF0; i3=i3|(i3>>4);
	ct=rgba+2*ystride;
	*ct=i0; ct+=xstride; *ct=i1; ct+=xstride;
	*ct=i2; ct+=xstride; *ct=i3; ct+=xstride;

	i0=(block[6]<<4)&0xF0; i0=i0|(i0>>4);
	i1=(block[6]   )&0xF0; i1=i1|(i1>>4);
	i2=(block[7]<<4)&0xF0; i2=i2|(i2>>4);
	i3=(block[7]   )&0xF0; i3=i3|(i3>>4);
	ct=rgba+3*ystride;
	*ct=i0; ct+=xstride; *ct=i1; ct+=xstride;
	*ct=i2; ct+=xstride; *ct=i3; ct+=xstride;
}

void BGBBTJ_S2TC_DecodeBlockBC4(byte *block,
	byte *rgba, int xstride, int ystride, int pfb)
{
	byte clr[8];
	byte *ct;
	int pxa, pxb;
	
	clr[0]=block[0];
	clr[1]=block[1];
	clr[2]=(clr[0]+clr[1])>>1;
	clr[3]=clr[2];
	clr[4]=clr[2];
	clr[5]=clr[2];
	clr[6]=0;
	clr[7]=255;
	
	pxa=block[2]|(block[3]<<8)|(block[4]<<16);
	pxb=block[5]|(block[6]<<8)|(block[7]<<16);
	
	ct=rgba;
	*ct=clr[(pxa    )&7]; ct+=xstride; *ct=clr[(pxa>> 3)&7]; ct+=xstride;
	*ct=clr[(pxa>> 6)&7]; ct+=xstride; *ct=clr[(pxa>> 9)&7]; ct+=xstride;
	ct=rgba+ystride;
	*ct=clr[(pxa>>12)&7]; ct+=xstride; *ct=clr[(pxa>>15)&7]; ct+=xstride;
	*ct=clr[(pxa>>18)&7]; ct+=xstride; *ct=clr[(pxa>>21)&7]; ct+=xstride;
	ct=rgba+2*ystride;
	*ct=clr[(pxb    )&7]; ct+=xstride; *ct=clr[(pxb>> 3)&7]; ct+=xstride;
	*ct=clr[(pxb>> 6)&7]; ct+=xstride; *ct=clr[(pxb>> 9)&7]; ct+=xstride;
	ct=rgba+3*ystride;
	*ct=clr[(pxb>>12)&7]; ct+=xstride; *ct=clr[(pxb>>15)&7]; ct+=xstride;
	*ct=clr[(pxb>>18)&7]; ct+=xstride; *ct=clr[(pxb>>21)&7]; ct+=xstride;
}

void BGBBTJ_BCn_DecodeBlock(byte *block, int bcf,
	byte *rgba, int xstride, int ystride, int pfb)
{
	if(bcf==BCN_BCF_BC1)
	{
		BGBBTJ_S2TC_DecodeBlockRGBA(block, rgba, xstride, ystride, pfb);
		return;
	}

	if(bcf==BCN_BCF_BC2)
	{
		BGBBTJ_S2TC_DecodeBlockRGBA(block+8, rgba, xstride, ystride, pfb);
		if(xstride==4)
		{
			BGBBTJ_S2TC_DecodeBlockBC2A(block, rgba+3,
				xstride, ystride, pfb);
		}
		return;
	}

	if(bcf==BCN_BCF_BC3)
	{
		BGBBTJ_S2TC_DecodeBlockRGBA(block+8, rgba, xstride, ystride, pfb);
		if(xstride==4)
		{
			BGBBTJ_S2TC_DecodeBlockBC4(block, rgba+3,
				xstride, ystride, pfb);
		}
		return;
	}

	if(bcf==BCN_BCF_BC4)
	{
		BGBBTJ_S2TC_DecodeBlockBC4(block, rgba,
			xstride, ystride, pfb);
		return;
	}

	if(bcf==BCN_BCF_BC5)
	{
		BGBBTJ_S2TC_DecodeBlockBC4(block, rgba,
			xstride, ystride, pfb);
		BGBBTJ_S2TC_DecodeBlockBC4(block+8, rgba+1,
			xstride, ystride, pfb);
		return;
	}
	
	if(bcf==BCN_BCF_BC7)
	{
		BGBBTJ_BC7_DecodeBlock(block, rgba,
			xstride, ystride, pfb);
		return;
	}
}

void BGBBTJ_BCn_DecodeBlockEdge(byte *block, int bcf,
	byte *rgba, int xstride, int ystride,
	int xfrac, int yfrac, int pfb)
{
	byte tblk[16*4];
	byte *cs, *ct;
	int i, j, xn;

	BGBBTJ_BCn_DecodeBlock(block, bcf,
		tblk, xstride, 4*xstride, pfb);

	xn=xfrac*xstride;
	for(i=0; i<yfrac; i++)
	{
		cs=tblk+i*4*xstride;
		ct=rgba+i*ystride;
		for(j=0; j<xn; j++)
			*ct++=*cs++;
	}
}

void BGBBTJ_BCn_DecodeImageBCnI(byte *block, int bcf,
	byte *rgba, int xs, int ys, int stride, int pfb)
{
	byte *rgba2, *rgba3;
	int xstr, ystr, bstr;
	int xs1, ys1, xs2, ys2, xf, yf;
	int i, j;

	rgba2=rgba; xstr=stride; ystr=xs*stride;
	if(ys<0)
	{
		ys=-ys; ystr=-xs*stride;
		rgba2=rgba+((ys-1)*xs*stride);
	}

	bstr=BGBBTJ_BCn_GetBlockStride(bcf);
	
	xs1=xs>>2; ys1=ys>>2;
	xs2=(xs+3)>>2; ys2=(ys+3)>>2;
	xf=xs&3; yf=ys&3;

	for(i=0; i<ys1; i++)
	{
		rgba3=rgba2+(i*4*ystr);

		for(j=0; j<xs1; j++)
		{
			BGBBTJ_BCn_DecodeBlock(
				block+(i*xs2+j)*bstr, bcf,
				rgba3+(j*4*xstr),
				stride, xs*stride, 0);
		}

		if(xf)
		{
			BGBBTJ_BCn_DecodeBlockEdge(
				block+(i*xs2+j)*bstr, bcf,
				rgba3+(j*4*xstr),
				xstr, ystr, xf, 4, 0);
		}
	}

	if(yf)
	{
		rgba3=rgba2+(i*4*ystr);
		for(j=0; j<xs1; j++)
		{
			BGBBTJ_BCn_DecodeBlockEdge(
				block+(i*xs2+j)*bstr, bcf,
				rgba3+(j*4*xstr),
				xstr, ystr, 4, yf, 0);
		}
		if(xf)
		{
			BGBBTJ_BCn_DecodeBlockEdge(
				block+(i*xs2+j)*bstr, bcf,
				rgba3+(j*4*xstr),
				xstr, ystr, xf, yf, 0);
		}
	}
}

void BGBBTJ_BCn_DecodeImageBCn(byte *block, int bcf,
	byte *rgba, int xs, int ys, int pfb)
{
	BGBBTJ_BCn_DecodeImageBCnI(block, bcf,
		rgba, xs, ys, 4-((pfb>>4)&3), pfb);
}
