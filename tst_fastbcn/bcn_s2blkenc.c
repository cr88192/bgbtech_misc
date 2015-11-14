static u16 bcn_dtmap[16][16];
static int dtminit=0;


void BGBBTJ_BCn_InitDither_S2TCB(void)
{
	int i, j, k, l, i2, j1, j2;
	int rn;

	if(dtminit)
		return;
	dtminit=1;

	k=0; rn=1;
	for(i=0; i<16; i++)
		for(i2=0; i2<16; i2++)
	{
		l=0;
//		k=0;
//		k=rand()&255;
//		for(j=0; j<16; j++)
		for(j1=0; j1<4; j1++)
		{
			for(j2=0; j2<4; j2++)
			{
				j=j1*4+j2;
				k+=((i<<4)|i);
				if(k&256)
					l|=1<<j;
				k=k&255;
			}
//			if(i2&1)
//			if(((i2+1)*(j1+1))&4)
//				k=(k+((i<<4)|i))&255;

			if(rn&256)
				k=(k+((i<<4)|i))&255;
			rn=rn*251+1;
		}
//		if(((i2+1)*(i+1))&16)
//			k=(k+((i<<4)|i))&255;
		if(rn&256)
			k=(k+((i<<4)|i))&255;
		rn=rn*251+1;
		bcn_dtmap[i2][i]=l;
	}
	
	for(i=0; i<16; i++)
	{
		for(j=0; j<16; j++)
		{
			printf("%04X ", bcn_dtmap[i][j]);
		}
//		printf("\n");
	}
}


void BGBBTJ_BCn_EncodeBlockI_S2TC(byte *block,
	byte *pxy,
	int *min, int *max,
	int mcy, int ncy)
{
	int min2[4], max2[4];
	int avg[4];
	int p0, p1, p2, p3;
	int acy, acya, acyb;
	int pxa, pxb;
	
	avg[0]=(min[0]+max[0])>>1;
	avg[1]=(min[1]+max[1])>>1;
	avg[2]=(min[2]+max[2])>>1;
	min2[0]=(min[0]+avg[0])>>1;
	min2[1]=(min[1]+avg[1])>>1;
	min2[2]=(min[2]+avg[2])>>1;
	max2[0]=(max[0]+avg[0])>>1;
	max2[1]=(max[1]+avg[1])>>1;
	max2[2]=(max[2]+avg[2])>>1;
	
	acy=(mcy+ncy)>>1;
//	acya=(acy+mcy)>>1;
//	acyb=(acy+ncy)>>1;
	acya=(3*acy+mcy)>>2;
	acyb=(3*acy+ncy)>>2;

	pxa=((min2[0]<<8)&0xF800)|
		((min2[1]<<3)&0x07E0)|
		((min2[2]>>3)&0x001F);
	pxb=((max2[0]<<8)&0xF800)|
		((max2[1]<<3)&0x07E0)|
		((max2[2]>>3)&0x001F);

	block[0]=pxa;	block[1]=pxa>>8;
	block[2]=pxb;	block[3]=pxb>>8;

	p0=((pxy[ 0]-acya)>>31)&1;	p1=((pxy[ 1]-acyb)>>31)&1;
	p2=((pxy[ 2]-acya)>>31)&1;	p3=((pxy[ 3]-acyb)>>31)&1;
	block[4]=(p0|(p1<<2)|(p2<<4)|(p3<<6))^0x55;
	p0=((pxy[ 4]-acyb)>>31)&1;	p1=((pxy[ 5]-acya)>>31)&1;
	p2=((pxy[ 6]-acyb)>>31)&1;	p3=((pxy[ 7]-acya)>>31)&1;
	block[5]=(p0|(p1<<2)|(p2<<4)|(p3<<6))^0x55;
	p0=((pxy[ 8]-acya)>>31)&1;	p1=((pxy[ 9]-acyb)>>31)&1;
	p2=((pxy[10]-acya)>>31)&1;	p3=((pxy[11]-acyb)>>31)&1;
	block[6]=(p0|(p1<<2)|(p2<<4)|(p3<<6))^0x55;
	p0=((pxy[12]-acyb)>>31)&1;	p1=((pxy[13]-acya)>>31)&1;
	p2=((pxy[14]-acyb)>>31)&1;	p3=((pxy[15]-acya)>>31)&1;
	block[7]=(p0|(p1<<2)|(p2<<4)|(p3<<6))^0x55;

	if(pxa>pxb)
	{
		block[0]=pxb;		block[1]=pxb>>8;
		block[2]=pxa;		block[3]=pxa>>8;
		block[4]^=0x55;		block[5]^=0x55;
		block[6]^=0x55;		block[7]^=0x55;
	}
}

void BGBBTJ_BCn_EncodeBlockI_S2TCB(byte *block,
	byte *pxy, byte *pxa,
	int *min, int *max,
	int mcy, int ncy,
	int mca, int nca)
{
	u16 *dtmap;
	int min2[4], max2[4];
	int avg[4];
	int p0, p1, p2, p3;
	int acy, acya, acyb;
	int aca, acaa, acab;
	int cxa, cxb, acm;
	int i, j, k;
	
	if(mca>=240)
//	if(1)
	{
		BGBBTJ_BCn_EncodeBlockI_S2TC(block, pxy, min, max, mcy, ncy);
		return;
	}

	BGBBTJ_BCn_InitDither_S2TCB();

	avg[0]=(min[0]+max[0])>>1;
	avg[1]=(min[1]+max[1])>>1;
	avg[2]=(min[2]+max[2])>>1;
	min2[0]=(min[0]+avg[0])>>1;
	min2[1]=(min[1]+avg[1])>>1;
	min2[2]=(min[2]+avg[2])>>1;
	max2[0]=(max[0]+avg[0])>>1;
	max2[1]=(max[1]+avg[1])>>1;
	max2[2]=(max[2]+avg[2])>>1;
	
	acy=(mcy+ncy)>>1;
	acya=(acy+mcy)>>1;
	acyb=(acy+ncy)>>1;

//	aca=(mca+nca)>>1;
//	acaa=(aca+mca)>>1;
//	acab=(aca+nca)>>1;

	cxa=((min2[0]<<8)&0xF800)|
		((min2[1]<<3)&0x07E0)|
		((min2[3]>>3)&0x001F);
	cxb=((max2[0]<<8)&0xF800)|
		((max2[1]<<3)&0x07E0)|
		((max2[3]>>3)&0x001F);
		
	block[0]=cxa;	block[1]=cxa>>8;
	block[2]=cxb;	block[3]=cxb>>8;

	i=((cxa*251^cxb*251)>>8)&3;
	dtmap=bcn_dtmap[i];
	
//	p0=((pxy[ 0]-acya)>>31)&1;	p1=((pxy[ 1]-acyb)>>31)&1;
//	p2=((pxy[ 2]-acya)>>31)&1;	p3=((pxy[ 3]-acyb)>>31)&1;
	p0=!((pxy[ 0]-acya)>>31);	p1=!((pxy[ 1]-acyb)>>31);
	p2=!((pxy[ 2]-acya)>>31);	p3=!((pxy[ 3]-acyb)>>31);
	p0|=((dtmap[pxa[ 0]>>4]<<31)>>31)&3;
	p1|=((dtmap[pxa[ 1]>>4]<<30)>>31)&3;
	p2|=((dtmap[pxa[ 2]>>4]<<29)>>31)&3;
	p3|=((dtmap[pxa[ 3]>>4]<<28)>>31)&3;
	block[4]=p0|(p1<<2)|(p1<<4)|(p1<<6);

//	p0=((pxy[ 4]-acyb)>>31)&1;	p1=((pxy[ 5]-acya)>>31)&1;
//	p2=((pxy[ 6]-acyb)>>31)&1;	p3=((pxy[ 7]-acya)>>31)&1;
	p0=!((pxy[ 4]-acyb)>>31);	p1=!((pxy[ 5]-acya)>>31);
	p2=!((pxy[ 6]-acyb)>>31);	p3=!((pxy[ 7]-acya)>>31);
	p0|=((dtmap[pxa[ 4]>>4]<<27)>>31)&3;
	p1|=((dtmap[pxa[ 5]>>4]<<26)>>31)&3;
	p2|=((dtmap[pxa[ 6]>>4]<<25)>>31)&3;
	p3|=((dtmap[pxa[ 7]>>4]<<24)>>31)&3;
	block[5]=p0|(p1<<2)|(p1<<4)|(p1<<6);

//	p0=((pxy[ 8]-acya)>>31)&1;	p1=((pxy[ 9]-acyb)>>31)&1;
//	p2=((pxy[10]-acya)>>31)&1;	p3=((pxy[11]-acyb)>>31)&1;
	p0=!((pxy[ 8]-acya)>>31);	p1=!((pxy[ 9]-acyb)>>31);
	p2=!((pxy[10]-acya)>>31);	p3=!((pxy[11]-acyb)>>31);
	p0|=((dtmap[pxa[ 8]>>4]<<23)>>31)&3;
	p1|=((dtmap[pxa[ 9]>>4]<<22)>>31)&3;
	p2|=((dtmap[pxa[10]>>4]<<21)>>31)&3;
	p3|=((dtmap[pxa[11]>>4]<<20)>>31)&3;
	block[6]=p0|(p1<<2)|(p1<<4)|(p1<<6);

//	p0=((pxy[12]-acyb)>>31)&1;	p1=((pxy[13]-acya)>>31)&1;
//	p2=((pxy[14]-acyb)>>31)&1;	p3=((pxy[15]-acya)>>31)&1;
	p0=!((pxy[12]-acyb)>>31);	p1=!((pxy[13]-acya)>>31);
	p2=!((pxy[14]-acyb)>>31);	p3=!((pxy[15]-acya)>>31);
	p0|=((dtmap[pxa[12]>>4]<<19)>>31)&3;
	p1|=((dtmap[pxa[13]>>4]<<18)>>31)&3;
	p2|=((dtmap[pxa[14]>>4]<<17)>>31)&3;
	p3|=((dtmap[pxa[15]>>4]<<16)>>31)&3;
	block[7]=p0|(p1<<2)|(p1<<4)|(p1<<6);

	if(cxa>cxb)
	{
		block[0]=cxb;		block[1]=cxb>>8;
		block[2]=cxa;		block[3]=cxa>>8;
		block[4]^=0x55&(~(block[4]>>1));
		block[5]^=0x55&(~(block[5]>>1));
		block[6]^=0x55&(~(block[6]>>1));
		block[7]^=0x55&(~(block[7]>>1));
	}
}

void BGBBTJ_BCn_EncodeAlphaBlockI_S2TC(byte *block,
	byte *pxa, int mca, int nca)
{
	int min2[4], max2[4];
	int avg[4];
	int p0, p1, p2, p3;
	int aca, acaa, acab;
	int cxa, cxb;
	
	aca=(mca+nca)>>1;
	acaa=(aca+mca)>>1;
	acab=(aca+nca)>>1;

	block[0]=mca;	block[1]=nca;
	
	p0=((pxa[ 0]-acaa)>>31)&1;	p1=((pxa[ 1]-acab)>>31)&1;
	p2=((pxa[ 2]-acaa)>>31)&1;	p3=((pxa[ 3]-acab)>>31)&1;
	cxa=p0|(p1<<3)|(p1<<6)|(p1<<9);
	p0=((pxa[ 4]-acab)>>31)&1;	p1=((pxa[ 5]-acaa)>>31)&1;
	p2=((pxa[ 6]-acab)>>31)&1;	p3=((pxa[ 7]-acaa)>>31)&1;
	cxb=p0|(p1<<3)|(p1<<6)|(p1<<9);

	cxa=cxa|(cxb<<12);
	block[2]=cxa;
	block[3]=cxa>>8;
	block[4]=cxa>>16;

	p0=((pxa[ 8]-acaa)>>31)&1;	p1=((pxa[ 9]-acab)>>31)&1;
	p2=((pxa[10]-acaa)>>31)&1;	p3=((pxa[11]-acab)>>31)&1;
	cxa=p0|(p1<<3)|(p1<<6)|(p1<<9);
	p0=((pxa[12]-acab)>>31)&1;	p1=((pxa[13]-acaa)>>31)&1;
	p2=((pxa[14]-acab)>>31)&1;	p3=((pxa[15]-acaa)>>31)&1;
	cxb=p0|(p1<<3)|(p1<<6)|(p1<<9);

	cxa=cxa|(cxb<<12);
	block[2]=cxa;
	block[3]=cxa>>8;
	block[4]=cxa>>16;
}

void BGBBTJ_BCn_EncodeAlphaBlock2I_S2TC(byte *block,
	byte *pxa, int mca, int nca)
{
	block[0]=(pxa[ 0]>>4)|(pxa[ 1]&0xF0);
	block[1]=(pxa[ 2]>>4)|(pxa[ 3]&0xF0);
	block[2]=(pxa[ 4]>>4)|(pxa[ 5]&0xF0);
	block[3]=(pxa[ 6]>>4)|(pxa[ 7]&0xF0);
	block[4]=(pxa[ 8]>>4)|(pxa[ 9]&0xF0);
	block[5]=(pxa[10]>>4)|(pxa[11]&0xF0);
	block[6]=(pxa[12]>>4)|(pxa[13]&0xF0);
	block[7]=(pxa[14]>>4)|(pxa[15]&0xF0);
}

void BGBBTJ_BC7_EncodeBlockBCnI(byte *block, int bcf,
	byte *pxy, byte *pxy2, byte *pxa,
	int *min, int *max,
	int *mcy, int *ncy,
	int mca, int nca, int pn)
{
	if(bcf==1)
	{
		BGBBTJ_BCn_EncodeBlockI_S2TCB(block, pxy, pxa,
			min, max, mcy[0], ncy[0], mca, nca);
		return;
	}

	if(bcf==2)
	{
		BGBBTJ_BCn_EncodeBlockI_S2TC(block+8, pxy,
			min, max, mcy[0], ncy[0]);
		BGBBTJ_BCn_EncodeAlphaBlock2I_S2TC(block,
			pxa, mca, nca);
		return;
	}

	if(bcf==3)
	{
		BGBBTJ_BCn_EncodeBlockI_S2TC(block+8, pxy,
			min, max, mcy[0], ncy[0]);
		BGBBTJ_BCn_EncodeAlphaBlockI_S2TC(block,
			pxa, mca, nca);
		return;
	}

	if(bcf==4)
	{
		BGBBTJ_BCn_EncodeAlphaBlockI_S2TC(block,
			pxy, mcy[0], ncy[0]);
		return;
	}

	if(bcf==5)
	{
		BGBBTJ_BCn_EncodeAlphaBlockI_S2TC(block,
			pxy, mcy[0], ncy[0]);
		BGBBTJ_BCn_EncodeAlphaBlockI_S2TC(block,
			pxa, mca, nca);
		return;
	}
	
	if(bcf==7)
	{
		BGBBTJ_BC7_EncodeBlockI(block, pxy, pxy2, pxa, min, max,
			mcy, ncy, mca, nca, pn);
		return;
	}
}

void BGBBTJ_BCn_EncodeBlockBCn(byte *block, int bcf,
	byte *rgba, int xstride, int ystride, int pfb)
{
	byte pxy[16], pxy2[16], pxa[16];
	int min[16], max[16];
	int mcy[8], ncy[8];
	int dy, du, dv, da, duv, pn;
	int mca, nca;
	
	BGBBTJ_BC7_EncodeBlock_SplitMinMaxClrYA2(
		pxy, pxy2, pxa, min, max, mcy, ncy, &mca, &nca, &pn,
		rgba, xstride, ystride, pfb);
	
	BGBBTJ_BC7_EncodeBlockBCnI(block, bcf,
		pxy, pxy2, pxa, min, max,
		mcy, ncy, mca, nca, pn);
}

void BGBBTJ_BCn_EncodeBlockBCnYuva4204(byte *block, int bcf,
	byte *ybuf, byte *ubuf, byte *vbuf, byte *abuf,
	int xystride, int yystride,
	int xustride, int yustride, int pfb)
{
	byte pxy[16], pxy2[16], pxa[16];
	int min[16], max[16];
	int mcy[8], ncy[8];
	int dy, du, dv, da, duv, pn;
	int mca, nca;
	
	BGBBTJ_BC7_EncodeBlock_SplitMinMaxClrYA2_Yuva4204(
		pxy, pxy2, pxa, min, max, mcy, ncy, &mca, &nca, &pn,
		ybuf, ubuf, vbuf, abuf,
		xystride, yystride,
		xustride, yustride, pfb);
	
	BGBBTJ_BC7_EncodeBlockI(block, pxy, pxy2, pxa, min, max,
		mcy, ncy, mca, nca, pn);
}

int BGBBTJ_BCn_GetBlockStride(int bcf)
{
	int sz;
	switch(bcf)
	{
	case 1:		sz=8;  break;
	case 2:		sz=16; break;
	case 3:		sz=16; break;
	case 4:		sz=8;  break;
	case 5:		sz=16; break;
	case 6:		sz=16; break;
	case 7:		sz=16; break;
	default:	sz=16; break;
	}
	return(sz);
}

void BGBBTJ_BCn_EncodeBlockEdgeBCn(byte *block, int bcf,
	byte *rgba, int xstride, int ystride,
	int xfrac, int yfrac, int pfb)
{
	byte tblk[16*4];
	byte cr, cg, cb, ca;
	byte *cs, *ct;
	int i, j, xn;

	xn=xfrac*xstride;
	for(i=0; i<yfrac; i++)
	{
		cs=rgba+i*ystride;
		ct=tblk+i*4*4;
		if(xstride==4)
		{
			for(j=0; j<xfrac; j++)
			{
				cr=*cs++; cg=*cs++;
				cb=*cs++; ca=*cs++;
				*ct++=cr; *ct++=cg;
				*ct++=cb; *ct++=ca;
			}
			for(; j<4; j++)
			{
				*ct++=cr; *ct++=cg;
				*ct++=cb; *ct++=ca;
			}
			continue;
		}
		if(xstride==3)
		{
			ca=255;
			for(j=0; j<xfrac; j++)
			{
				cr=*cs++; cg=*cs++; cb=*cs++; 
				*ct++=cr; *ct++=cg;
				*ct++=cb; *ct++=ca;
			}
			for(; j<4; j++)
			{
				*ct++=cr; *ct++=cg;
				*ct++=cb; *ct++=ca;
			}
			continue;
		}
		for(j=0; j<xn; j++)
			*ct++=*cs++;
	}

	for(; i<4; i++)
	{
		ct=tblk+i*4*4;
		for(j=0; j<4; j++)
		{
			*ct++=cr; *ct++=cg;
			*ct++=cb; *ct++=ca;
		}
	}
	
	BGBBTJ_BCn_EncodeBlockBCn(block, bcf, tblk, 4, 4*4, pfb);
}

void BGBBTJ_BCn_EncodeImageBCnI(byte *block, int bcf,
	byte *rgba, int xs, int ys, int stride, int pfb)
{
	byte *rgba2, *rgba3;
	int xstr, ystr, bstr;
	int xs1, ys1, xs2, ys2, xf, yf;
	int i, j, k;
	
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
			BGBBTJ_BCn_EncodeBlockBCn(
				block+(i*xs2+j)*bstr, bcf,
				rgba3+(j*4*xstr),
				stride, ystr, pfb);
		}

		if(xf)
		{
			BGBBTJ_BCn_EncodeBlockEdgeBCn(
				block+(i*xs2+j)*bstr, bcf,
				rgba3+(j*4*xstr),
				xstr, ystr, xf, 4, pfb);
		}
	}

	if(yf)
	{
		rgba3=rgba2+(i*4*ystr);
		for(j=0; j<xs1; j++)
		{
			BGBBTJ_BCn_EncodeBlockEdgeBCn(
				block+(i*xs2+j)*bstr, bcf,
				rgba3+(j*4*xstr),
				xstr, ystr, 4, yf, pfb);
		}
		if(xf)
		{
			BGBBTJ_BCn_EncodeBlockEdgeBCn(
				block+(i*xs2+j)*bstr, bcf,
				rgba3+(j*4*xstr),
				xstr, ystr, xf, yf, pfb);
		}
	}
}

void BGBBTJ_BCn_EncodeImageBCn(byte *block, int bcf,
	byte *rgba, int xs, int ys, int pfb)
{
	BGBBTJ_BCn_EncodeImageBCnI(block, bcf,
		rgba, xs, ys, 4-((pfb>>4)&3), pfb);
}
