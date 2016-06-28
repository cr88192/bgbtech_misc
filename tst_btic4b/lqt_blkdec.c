
void LQTVQ_DecBlock0BGRA(byte *blkbuf, byte *ibuf, int ystr)
{
	int ytab[8], utab[4], vtab[4];
	byte *ct0, *ct1;
	int acy, acu, acv, dcy, dcu, dcv;
	int mcy, mcu, mcv, ncy, ncu, ncv;

	int cr0, cr1, cr2, cr3;
	int cg0, cg1, cg2, cg3;
	int cb0, cb1, cb2, cb3;
	int cy0, cy1, cy2, cy3;
	int cy, cu, cv, cu1, cv1;

	u32 pxy0, pxy1, pxu, pxv;
	int i, j, k;
	
	acy=*(s16 *)(blkbuf+ 4);
	acu=*(s16 *)(blkbuf+ 6);
	acv=*(s16 *)(blkbuf+ 8);
	dcy=*(s16 *)(blkbuf+10);
	dcu=*(s16 *)(blkbuf+12);
	dcv=*(s16 *)(blkbuf+14);
	
	mcy=acy-(dcy>>1); ncy=mcy+dcy;
	mcu=acu-(dcu>>1); ncu=mcu+dcu;
	mcv=acv-(dcv>>1); ncv=mcv+dcv;

//	mcu=128;	ncu=128;
//	mcv=128;	ncv=128;

	ytab[0]=mcy;
	ytab[1]=(14*mcy+ 2*ncy)>>4;
	ytab[2]=(11*mcy+ 5*ncy)>>4;
	ytab[3]=( 9*mcy+ 7*ncy)>>4;
	ytab[4]=( 7*mcy+ 9*ncy)>>4;
	ytab[5]=( 5*mcy+11*ncy)>>4;
	ytab[6]=( 2*mcy+14*ncy)>>4;
	ytab[7]=ncy;

	utab[0]=mcu;
	utab[1]=(11*mcu+ 5*ncu)>>4;
	utab[2]=( 5*mcu+11*ncu)>>4;
	utab[3]=ncu;

	vtab[0]=mcv;
	vtab[1]=(11*mcv+ 5*ncv)>>4;
	vtab[2]=( 5*mcv+11*ncv)>>4;
	vtab[3]=ncv;
	
	pxu=*(u32 *)(blkbuf+40);
	pxv=*(u32 *)(blkbuf+44);

	for(i=0; i<4; i++)
	{
		ct0=ibuf+(i*2+0)*ystr;
		ct1=ibuf+(i*2+1)*ystr;
		pxy0=*(u32 *)(blkbuf+16+i*6+0);
		pxy1=*(u32 *)(blkbuf+16+i*6+3);

//		pxy0=*(u32 *)(blkbuf+16+(3-i)*6+3);
//		pxy1=*(u32 *)(blkbuf+16+(3-i)*6+0);
		
		for(j=0; j<4; j++)
		{
			k=j*6;
			cy0=ytab[(pxy0>>(k+0))&7];
			cy1=ytab[(pxy0>>(k+3))&7];
			cy2=ytab[(pxy1>>(k+0))&7];
			cy3=ytab[(pxy1>>(k+3))&7];

			k=(i*4+j)*2;
			cu=utab[(pxu>>k)&3];
			cv=vtab[(pxv>>k)&3];

//			k=(3-j)*6;
//			cy0=ytab[(pxy0>>(k+3))&7];
//			cy1=ytab[(pxy0>>(k+0))&7];
//			cy2=ytab[(pxy1>>(k+3))&7];
//			cy3=ytab[(pxy1>>(k+0))&7];
//			k=30-((i*4+j)*2);
//			cu=utab[(pxu>>k)&3];
//			cv=vtab[(pxv>>k)&3];
			
//			cu1=(cu-128)<<1;
//			cv1=(cv-128)<<1;
			cu1=cu;		cv1=cv;

			cg0=cy0; cb0=cg0+cu1; cr0=cg0+cv1;
			cg1=cy1; cb1=cg1+cu1; cr1=cg1+cv1;
			cg2=cy2; cb2=cg2+cu1; cr2=cg2+cv1;
			cg3=cy3; cb3=cg3+cu1; cr3=cg3+cv1;
			
			if((cr0|cr1|cr2|cr3)>>8)
			{
				cr0=lqtvq_clamp255(cr0);
				cr1=lqtvq_clamp255(cr1);
				cr2=lqtvq_clamp255(cr2);
				cr3=lqtvq_clamp255(cr3);
			}

			if((cg0|cg1|cg2|cg3)>>8)
			{
				cg0=lqtvq_clamp255(cg0);
				cg1=lqtvq_clamp255(cg1);
				cg2=lqtvq_clamp255(cg2);
				cg3=lqtvq_clamp255(cg3);
			}

			if((cb0|cb1|cb2|cb3)>>8)
			{
				cb0=lqtvq_clamp255(cb0);
				cb1=lqtvq_clamp255(cb1);
				cb2=lqtvq_clamp255(cb2);
				cb3=lqtvq_clamp255(cb3);
			}
			
			ct0[0]=cb0;	ct0[1]=cg0;	ct0[2]=cr0;	ct0[3]=255;
			ct0[4]=cb1;	ct0[5]=cg1;	ct0[6]=cr1;	ct0[7]=255;
			ct1[0]=cb2;	ct1[1]=cg2;	ct1[2]=cr2;	ct1[3]=255;
			ct1[4]=cb3;	ct1[5]=cg3;	ct1[6]=cr3;	ct1[7]=255;
			
			ct0+=8;
			ct1+=8;
		}
	}
}

force_inline void lqtvq_fillu32_8x1(u32 *px, u32 v)
{
	px[0]=v; px[1]=v; px[2]=v; px[3]=v;
	px[4]=v; px[5]=v; px[6]=v; px[7]=v;
}

force_inline void lqtvq_fillu32_8x1_2x1(u32 *px, u32 u, u32 v)
{
	px[0]=u; px[1]=u; px[2]=u; px[3]=u;
	px[4]=v; px[5]=v; px[6]=v; px[7]=v;
}

force_inline void lqtvq_fillu32_8x1_4x1(u32 *px,
	u32 v0, u32 v1, u32 v2, u32 v3)
{
	px[0]=v0; px[1]=v0; px[2]=v1; px[3]=v1;
	px[4]=v2; px[5]=v2; px[6]=v3; px[7]=v3;
}

force_inline void lqtvq_fillu32_8x1_8x1(u32 *px,
	u32 v0, u32 v1, u32 v2, u32 v3,
	u32 v4, u32 v5, u32 v6, u32 v7)
{
	px[0]=v0; px[1]=v1; px[2]=v2; px[3]=v3;
	px[4]=v4; px[5]=v5; px[6]=v6; px[7]=v7;
}

void LQTVQ_DecBlockFlatBGRA(byte *blkbuf, byte *ibuf, int ystr)
{
	byte *ct0, *ct1;
	byte *ct2, *ct3;
	int acy, acu, acv, dcy, dcu, dcv;
	int mcy, mcu, mcv, ncy, ncu, ncv;

	int cr0, cr1, cr2, cr3;
	int cg0, cg1, cg2, cg3;
	int cb0, cb1, cb2, cb3;
	int cy0, cy1, cy2, cy3;
	int cy, cu, cv, cu1, cv1;

	u32 pxy0, pxy1, pxu, pxv;
	int i, j, k;
	
	acy=*(s16 *)(blkbuf+ 4);
	acu=*(s16 *)(blkbuf+ 6);
	acv=*(s16 *)(blkbuf+ 8);

//	cu1=(acu-128)<<1;	cv1=(acv-128)<<1;
	cu1=acu;	cv1=acv;
	cg0=acy; cb0=cg0+cu1; cr0=cg0+cv1;

	if((cr0|cg0|cb0)>>8)
	{
		cr0=lqtvq_clamp255(cr0);
		cg0=lqtvq_clamp255(cg0);
		cb0=lqtvq_clamp255(cb0);
	}
		
	pxu=0xFF000000|(cr0<<16)|(cg0<<8)|cb0;

	lqtvq_fillu32_8x1((u32 *)(ibuf+0*ystr), pxu);
	lqtvq_fillu32_8x1((u32 *)(ibuf+1*ystr), pxu);
	lqtvq_fillu32_8x1((u32 *)(ibuf+2*ystr), pxu);
	lqtvq_fillu32_8x1((u32 *)(ibuf+3*ystr), pxu);
	lqtvq_fillu32_8x1((u32 *)(ibuf+4*ystr), pxu);
	lqtvq_fillu32_8x1((u32 *)(ibuf+5*ystr), pxu);
	lqtvq_fillu32_8x1((u32 *)(ibuf+6*ystr), pxu);
	lqtvq_fillu32_8x1((u32 *)(ibuf+7*ystr), pxu);
}

void LQTVQ_DecBlockFlatBGRA_FA(byte *blkbuf,
	byte *ibuf, int ystr, int ca)
{
	byte *ct0, *ct1;
	byte *ct2, *ct3;
	int acy, acu, acv, dcy, dcu, dcv;
	int mcy, mcu, mcv, ncy, ncu, ncv;

	int cr0, cr1, cr2, cr3;
	int cg0, cg1, cg2, cg3;
	int cb0, cb1, cb2, cb3;
	int cy0, cy1, cy2, cy3;
	int cy, cu, cv, cu1, cv1;

	u32 pxy0, pxy1, pxu, pxv;
	int i, j, k;
	
	acy=*(s16 *)(blkbuf+ 4);
	acu=*(s16 *)(blkbuf+ 6);
	acv=*(s16 *)(blkbuf+ 8);

//	cu1=(acu-128)<<1;	cv1=(acv-128)<<1;
	cu1=acu;	cv1=acv;
	cg0=acy; cb0=cg0+cu1; cr0=cg0+cv1;

	if((cr0|cg0|cb0)>>8)
	{
		cr0=lqtvq_clamp255(cr0);
		cg0=lqtvq_clamp255(cg0);
		cb0=lqtvq_clamp255(cb0);
	}
		
	pxu=(ca<<24)|(cr0<<16)|(cg0<<8)|cb0;

	lqtvq_fillu32_8x1((u32 *)(ibuf+0*ystr), pxu);
	lqtvq_fillu32_8x1((u32 *)(ibuf+1*ystr), pxu);
	lqtvq_fillu32_8x1((u32 *)(ibuf+2*ystr), pxu);
	lqtvq_fillu32_8x1((u32 *)(ibuf+3*ystr), pxu);
	lqtvq_fillu32_8x1((u32 *)(ibuf+4*ystr), pxu);
	lqtvq_fillu32_8x1((u32 *)(ibuf+5*ystr), pxu);
	lqtvq_fillu32_8x1((u32 *)(ibuf+6*ystr), pxu);
	lqtvq_fillu32_8x1((u32 *)(ibuf+7*ystr), pxu);
}

/* XxXx2 */
void LQTVQ_DecBlock1BGRA(byte *blkbuf, byte *ibuf, int ystr)
{
//	int ytab[8], utab[4], vtab[4];
	u32 rgbtab[4];
	byte *ct0, *ct1;
	byte *ct2, *ct3;
	int acy, acu, acv, dcy, dcu, dcv;
	int mcy, mcu, mcv, ncy, ncu, ncv;

	int cr0, cr1, cr2, cr3;
	int cg0, cg1, cg2, cg3;
	int cb0, cb1, cb2, cb3;
	int cy0, cy1, cy2, cy3;
	int cy, cu, cv, cu1, cv1;

	u32 px0, px1, px2, px3;
	u32 px4, px5, px6, px7;
	u32 px8, px9, pxA, pxB;
	u32 pxC, pxD, pxE, pxF;

	u32 pxy0, pxy1, pxu, pxv;
	int i, j, k;
	
	acy=*(s16 *)(blkbuf+ 4);
	acu=*(s16 *)(blkbuf+ 6);
	acv=*(s16 *)(blkbuf+ 8);
	dcy=*(s16 *)(blkbuf+10);
	dcu=*(s16 *)(blkbuf+12);
	dcv=*(s16 *)(blkbuf+14);
	
	mcy=acy-(dcy>>1); ncy=mcy+dcy;
	mcu=acu-(dcu>>1); ncu=mcu+dcu;
	mcv=acv-(dcv>>1); ncv=mcv+dcv;

//	mcu=128;	ncu=128;
//	mcv=128;	ncv=128;

//	cu1=(acu-128)<<1;	cv1=(acv-128)<<1;
	cu1=acu;	cv1=acv;
	cg0=mcy; cb0=cg0+cu1; cr0=cg0+cv1;
	cg3=ncy; cb3=cg3+cu1; cr3=cg3+cv1;

	if((cr0|cg0|cb0)>>8)
	{
		cr0=lqtvq_clamp255(cr0);
		cg0=lqtvq_clamp255(cg0);
		cb0=lqtvq_clamp255(cb0);
	}

	if((cr3|cg3|cb3)>>8)
	{
		cr3=lqtvq_clamp255(cr3);
		cg3=lqtvq_clamp255(cg3);
		cb3=lqtvq_clamp255(cb3);
	}

	cr1=(11*cr0+ 5*cr3)>>4;
	cg1=(11*cg0+ 5*cg3)>>4;
	cb1=(11*cb0+ 5*cb3)>>4;
	cr2=( 5*cr0+11*cr3)>>4;
	cg2=( 5*cg0+11*cg3)>>4;
	cb2=( 5*cb0+11*cb3)>>4;

	rgbtab[0]=0xFF000000|(cr0<<16)|(cg0<<8)|cb0;
	rgbtab[1]=0xFF000000|(cr1<<16)|(cg1<<8)|cb1;
	rgbtab[2]=0xFF000000|(cr2<<16)|(cg2<<8)|cb2;
	rgbtab[3]=0xFF000000|(cr3<<16)|(cg3<<8)|cb3;

	switch(blkbuf[0])
	{
	case 0x09:
		pxy0=blkbuf[16];
		for(i=0; i<2; i++)
		{
			pxu=rgbtab[(pxy0>>(i*4+0))&3];
			pxv=rgbtab[(pxy0>>(i*4+2))&3];
//			j=1-i;
//			pxu=rgbtab[(pxy0>>(j*4+2))&3];
//			pxv=rgbtab[(pxy0>>(j*4+0))&3];
			ct0=ibuf+(i*4+0)*ystr;	ct1=ibuf+(i*4+1)*ystr;
			ct2=ibuf+(i*4+2)*ystr;	ct3=ibuf+(i*4+3)*ystr;
			lqtvq_fillu32_8x1_2x1((u32 *)ct0, pxu, pxv);
			lqtvq_fillu32_8x1_2x1((u32 *)ct1, pxu, pxv);
			lqtvq_fillu32_8x1_2x1((u32 *)ct2, pxu, pxv);
			lqtvq_fillu32_8x1_2x1((u32 *)ct3, pxu, pxv);
		}
		break;
	case 0x0A:
		pxy0=*(u16 *)(blkbuf+16);
		for(i=0; i<2; i++)
		{
			px0=rgbtab[(pxy0>>(i*8+0))&3];
			px1=rgbtab[(pxy0>>(i*8+2))&3];
			px2=rgbtab[(pxy0>>(i*8+4))&3];
			px3=rgbtab[(pxy0>>(i*8+6))&3];

//			j=1-i;
//			px0=rgbtab[(pxy0>>(j*8+6))&3];
//			px1=rgbtab[(pxy0>>(j*8+4))&3];
//			px2=rgbtab[(pxy0>>(j*8+2))&3];
//			px3=rgbtab[(pxy0>>(j*8+0))&3];

			ct0=ibuf+(i*4+0)*ystr;	ct1=ibuf+(i*4+1)*ystr;
			ct2=ibuf+(i*4+2)*ystr;	ct3=ibuf+(i*4+3)*ystr;
			lqtvq_fillu32_8x1_4x1((u32 *)ct0, px0, px1, px2, px3);
			lqtvq_fillu32_8x1_4x1((u32 *)ct1, px0, px1, px2, px3);
			lqtvq_fillu32_8x1_4x1((u32 *)ct2, px0, px1, px2, px3);
			lqtvq_fillu32_8x1_4x1((u32 *)ct3, px0, px1, px2, px3);
		}
		break;
	case 0x0B:
		pxy0=*(u16 *)(blkbuf+16);
		for(i=0; i<4; i++)
		{
			pxu=rgbtab[(pxy0>>(i*4+0))&3];
			pxv=rgbtab[(pxy0>>(i*4+2))&3];

//			j=3-i;
//			pxu=rgbtab[(pxy0>>(j*4+2))&3];
//			pxv=rgbtab[(pxy0>>(j*4+0))&3];
			ct0=ibuf+(i*2+0)*ystr;	ct1=ibuf+(i*2+1)*ystr;
			lqtvq_fillu32_8x1_2x1((u32 *)ct0, pxu, pxv);
			lqtvq_fillu32_8x1_2x1((u32 *)ct1, pxu, pxv);
		}
		break;
	case 0x0C:
		pxy0=*(u32 *)(blkbuf+16);
		for(i=0; i<4; i++)
		{
			px0=rgbtab[(pxy0>>(i*8+0))&3];
			px1=rgbtab[(pxy0>>(i*8+2))&3];
			px2=rgbtab[(pxy0>>(i*8+4))&3];
			px3=rgbtab[(pxy0>>(i*8+6))&3];

//			j=3-i;
//			px0=rgbtab[(pxy0>>(j*8+6))&3];
//			px1=rgbtab[(pxy0>>(j*8+4))&3];
//			px2=rgbtab[(pxy0>>(j*8+2))&3];
//			px3=rgbtab[(pxy0>>(j*8+0))&3];
			ct0=ibuf+(i*2+0)*ystr;	ct1=ibuf+(i*2+1)*ystr;
			lqtvq_fillu32_8x1_4x1((u32 *)ct0, px0, px1, px2, px3);
			lqtvq_fillu32_8x1_4x1((u32 *)ct1, px0, px1, px2, px3);
		}
		break;
	case 0x0D:
		for(i=0; i<2; i++)
		{
			pxy0=*(u32 *)(blkbuf+16+i*4);
			pxF=rgbtab[(pxy0>>30)&3];	pxE=rgbtab[(pxy0>>28)&3];
			pxD=rgbtab[(pxy0>>26)&3];	pxC=rgbtab[(pxy0>>24)&3];
			pxB=rgbtab[(pxy0>>22)&3];	pxA=rgbtab[(pxy0>>20)&3];
			px9=rgbtab[(pxy0>>18)&3];	px8=rgbtab[(pxy0>>16)&3];
			px7=rgbtab[(pxy0>>14)&3];	px6=rgbtab[(pxy0>>12)&3];
			px5=rgbtab[(pxy0>>10)&3];	px4=rgbtab[(pxy0>> 8)&3];
			px3=rgbtab[(pxy0>> 6)&3];	px2=rgbtab[(pxy0>> 4)&3];
			px1=rgbtab[(pxy0>> 2)&3];	px0=rgbtab[(pxy0>> 0)&3];

//			pxy0=*(u32 *)(blkbuf+16+(1-i)*4);
//			px0=rgbtab[(pxy0>>30)&3];	px1=rgbtab[(pxy0>>28)&3];
//			px2=rgbtab[(pxy0>>26)&3];	px3=rgbtab[(pxy0>>24)&3];
//			px4=rgbtab[(pxy0>>22)&3];	px5=rgbtab[(pxy0>>20)&3];
//			px6=rgbtab[(pxy0>>18)&3];	px7=rgbtab[(pxy0>>16)&3];
//			px8=rgbtab[(pxy0>>14)&3];	px9=rgbtab[(pxy0>>12)&3];
//			pxA=rgbtab[(pxy0>>10)&3];	pxB=rgbtab[(pxy0>> 8)&3];
//			pxC=rgbtab[(pxy0>> 6)&3];	pxD=rgbtab[(pxy0>> 4)&3];
//			pxE=rgbtab[(pxy0>> 2)&3];	pxF=rgbtab[(pxy0>> 0)&3];

			ct0=ibuf+(i*4+0)*ystr;	ct1=ibuf+(i*4+1)*ystr;
			ct2=ibuf+(i*4+2)*ystr;	ct3=ibuf+(i*4+3)*ystr;

			lqtvq_fillu32_8x1_8x1((u32 *)ct0,
				px0, px1, px2, px3, px4, px5, px6, px7);
			lqtvq_fillu32_8x1_8x1((u32 *)ct1,
				px0, px1, px2, px3, px4, px5, px6, px7);
			lqtvq_fillu32_8x1_8x1((u32 *)ct2,
				px8, px9, pxA, pxB, pxC, pxD, pxE, pxF);
			lqtvq_fillu32_8x1_8x1((u32 *)ct3,
				px8, px9, pxA, pxB, pxC, pxD, pxE, pxF);
		}
		break;
	case 0x0E:
		for(i=0; i<2; i++)
		{
			pxy0=*(u32 *)(blkbuf+16+i*4);
			pxF=rgbtab[(pxy0>>30)&3];	pxE=rgbtab[(pxy0>>28)&3];
			pxD=rgbtab[(pxy0>>26)&3];	pxC=rgbtab[(pxy0>>24)&3];
			pxB=rgbtab[(pxy0>>22)&3];	pxA=rgbtab[(pxy0>>20)&3];
			px9=rgbtab[(pxy0>>18)&3];	px8=rgbtab[(pxy0>>16)&3];
			px7=rgbtab[(pxy0>>14)&3];	px6=rgbtab[(pxy0>>12)&3];
			px5=rgbtab[(pxy0>>10)&3];	px4=rgbtab[(pxy0>> 8)&3];
			px3=rgbtab[(pxy0>> 6)&3];	px2=rgbtab[(pxy0>> 4)&3];
			px1=rgbtab[(pxy0>> 2)&3];	px0=rgbtab[(pxy0>> 0)&3];

//			pxy0=*(u32 *)(blkbuf+16+(1-i)*4);
//			px0=rgbtab[(pxy0>>30)&3];	px1=rgbtab[(pxy0>>28)&3];
//			px2=rgbtab[(pxy0>>26)&3];	px3=rgbtab[(pxy0>>24)&3];
//			px4=rgbtab[(pxy0>>22)&3];	px5=rgbtab[(pxy0>>20)&3];
//			px6=rgbtab[(pxy0>>18)&3];	px7=rgbtab[(pxy0>>16)&3];
//			px8=rgbtab[(pxy0>>14)&3];	px9=rgbtab[(pxy0>>12)&3];
//			pxA=rgbtab[(pxy0>>10)&3];	pxB=rgbtab[(pxy0>> 8)&3];
//			pxC=rgbtab[(pxy0>> 6)&3];	pxD=rgbtab[(pxy0>> 4)&3];
//			pxE=rgbtab[(pxy0>> 2)&3];	pxF=rgbtab[(pxy0>> 0)&3];

			ct0=ibuf+(i*4+0)*ystr;	ct1=ibuf+(i*4+1)*ystr;
			ct2=ibuf+(i*4+2)*ystr;	ct3=ibuf+(i*4+3)*ystr;

			lqtvq_fillu32_8x1_4x1((u32 *)ct0, px0, px1, px2, px3);
			lqtvq_fillu32_8x1_4x1((u32 *)ct1, px4, px5, px6, px7);
			lqtvq_fillu32_8x1_4x1((u32 *)ct2, px8, px9, pxA, pxB);
			lqtvq_fillu32_8x1_4x1((u32 *)ct3, pxC, pxD, pxE, pxF);
		}
		break;
	case 0x0F:
		for(i=0; i<4; i++)
		{
			pxy0=*(u32 *)(blkbuf+16+i*4);
			pxF=rgbtab[(pxy0>>30)&3];	pxE=rgbtab[(pxy0>>28)&3];
			pxD=rgbtab[(pxy0>>26)&3];	pxC=rgbtab[(pxy0>>24)&3];
			pxB=rgbtab[(pxy0>>22)&3];	pxA=rgbtab[(pxy0>>20)&3];
			px9=rgbtab[(pxy0>>18)&3];	px8=rgbtab[(pxy0>>16)&3];
			px7=rgbtab[(pxy0>>14)&3];	px6=rgbtab[(pxy0>>12)&3];
			px5=rgbtab[(pxy0>>10)&3];	px4=rgbtab[(pxy0>> 8)&3];
			px3=rgbtab[(pxy0>> 6)&3];	px2=rgbtab[(pxy0>> 4)&3];
			px1=rgbtab[(pxy0>> 2)&3];	px0=rgbtab[(pxy0>> 0)&3];

//			pxy0=*(u32 *)(blkbuf+16+(3-i)*4);
//			px0=rgbtab[(pxy0>>30)&3];	px1=rgbtab[(pxy0>>28)&3];
//			px2=rgbtab[(pxy0>>26)&3];	px3=rgbtab[(pxy0>>24)&3];
//			px4=rgbtab[(pxy0>>22)&3];	px5=rgbtab[(pxy0>>20)&3];
//			px6=rgbtab[(pxy0>>18)&3];	px7=rgbtab[(pxy0>>16)&3];
//			px8=rgbtab[(pxy0>>14)&3];	px9=rgbtab[(pxy0>>12)&3];
//			pxA=rgbtab[(pxy0>>10)&3];	pxB=rgbtab[(pxy0>> 8)&3];
//			pxC=rgbtab[(pxy0>> 6)&3];	pxD=rgbtab[(pxy0>> 4)&3];
//			pxE=rgbtab[(pxy0>> 2)&3];	pxF=rgbtab[(pxy0>> 0)&3];
			ct0=ibuf+(i*2+0)*ystr;	ct1=ibuf+(i*2+1)*ystr;

			lqtvq_fillu32_8x1_8x1((u32 *)ct0,
				px0, px1, px2, px3, px4, px5, px6, px7);
			lqtvq_fillu32_8x1_8x1((u32 *)ct1,
				px8, px9, pxA, pxB, pxC, pxD, pxE, pxF);
		}
		break;
	}
}

/* 8x8x3 */
void LQTVQ_DecBlock2BGRA(byte *blkbuf, byte *ibuf, int ystr)
{
	u32 rgbtab[8];
	byte *ct0, *ct1;
	byte *ct2, *ct3;
	int acy, acu, acv, dcy, dcu, dcv;
	int mcy, mcu, mcv, ncy, ncu, ncv;

	int cr0, cr1, cr2, cr3, cr4, cr5, cr6, cr7;
	int cg0, cg1, cg2, cg3, cg4, cg5, cg6, cg7;
	int cb0, cb1, cb2, cb3, cb4, cb5, cb6, cb7;
	int cy0, cy1, cy2, cy3;
	int cy, cu, cv, cu1, cv1;

	u32 px0, px1, px2, px3;
	u32 px4, px5, px6, px7;
	u32 px8, px9, pxA, pxB;
	u32 pxC, pxD, pxE, pxF;

	u32 pxy0, pxy1, pxu, pxv;
	int i, j, k;
	
	acy=*(s16 *)(blkbuf+ 4);
	acu=*(s16 *)(blkbuf+ 6);
	acv=*(s16 *)(blkbuf+ 8);
	dcy=*(s16 *)(blkbuf+10);
	dcu=*(s16 *)(blkbuf+12);
	dcv=*(s16 *)(blkbuf+14);
	
	mcy=acy-(dcy>>1); ncy=mcy+dcy;
	mcu=acu-(dcu>>1); ncu=mcu+dcu;
	mcv=acv-(dcv>>1); ncv=mcv+dcv;

//	cu1=(acu-128)<<1;	cv1=(acv-128)<<1;
	cu1=acu;	cv1=acv;
	cg0=mcy; cb0=cg0+cu1; cr0=cg0+cv1;
	cg7=ncy; cb7=cg7+cu1; cr7=cg7+cv1;

	if((cr0|cg0|cb0)>>8)
	{
		cr0=lqtvq_clamp255(cr0);
		cg0=lqtvq_clamp255(cg0);
		cb0=lqtvq_clamp255(cb0);
	}

	if((cr7|cg7|cb7)>>8)
	{
		cr7=lqtvq_clamp255(cr7);
		cg7=lqtvq_clamp255(cg7);
		cb7=lqtvq_clamp255(cb7);
	}

	cr1=(14*cr0+ 2*cr7)>>4;	cg1=(14*cg0+ 2*cg7)>>4;	cb1=(14*cb0+ 2*cb7)>>4;
	cr2=(11*cr0+ 5*cr7)>>4;	cg2=(11*cg0+ 5*cg7)>>4;	cb2=(11*cb0+ 5*cb7)>>4;
	cr3=( 9*cr0+ 7*cr7)>>4;	cg3=( 9*cg0+ 7*cg7)>>4;	cb3=( 9*cb0+ 7*cb7)>>4;
	cr4=( 7*cr0+ 9*cr7)>>4;	cg4=( 7*cg0+ 9*cg7)>>4;	cb4=( 7*cb0+ 9*cb7)>>4;
	cr5=( 5*cr0+11*cr7)>>4;	cg5=( 5*cg0+11*cg7)>>4;	cb5=( 5*cb0+11*cb7)>>4;
	cr6=( 2*cr0+14*cr7)>>4;	cg6=( 2*cg0+14*cg7)>>4;	cb6=( 2*cb0+14*cb7)>>4;

	rgbtab[0]=0xFF000000|(cr0<<16)|(cg0<<8)|cb0;
	rgbtab[1]=0xFF000000|(cr1<<16)|(cg1<<8)|cb1;
	rgbtab[2]=0xFF000000|(cr2<<16)|(cg2<<8)|cb2;
	rgbtab[3]=0xFF000000|(cr3<<16)|(cg3<<8)|cb3;
	rgbtab[4]=0xFF000000|(cr4<<16)|(cg4<<8)|cb4;
	rgbtab[5]=0xFF000000|(cr5<<16)|(cg5<<8)|cb5;
	rgbtab[6]=0xFF000000|(cr6<<16)|(cg6<<8)|cb6;
	rgbtab[7]=0xFF000000|(cr7<<16)|(cg7<<8)|cb7;

	for(i=0; i<4; i++)
	{
		pxy0=*(u32 *)(blkbuf+16+i*6+3);
		pxy1=*(u32 *)(blkbuf+16+i*6+0);
		pxF=rgbtab[(pxy0>>21)&7];	pxE=rgbtab[(pxy0>>18)&7];
		pxD=rgbtab[(pxy0>>15)&7];	pxC=rgbtab[(pxy0>>12)&7];
		pxB=rgbtab[(pxy0>> 9)&7];	pxA=rgbtab[(pxy0>> 6)&7];
		px9=rgbtab[(pxy0>> 3)&7];	px8=rgbtab[(pxy0>> 0)&7];
		px7=rgbtab[(pxy1>>21)&7];	px6=rgbtab[(pxy1>>18)&7];
		px5=rgbtab[(pxy1>>15)&7];	px4=rgbtab[(pxy1>>12)&7];
		px3=rgbtab[(pxy1>> 9)&7];	px2=rgbtab[(pxy1>> 6)&7];
		px1=rgbtab[(pxy1>> 3)&7];	px0=rgbtab[(pxy1>> 0)&7];

//		pxy0=*(u32 *)(blkbuf+16+(3-i)*6+3);
//		pxy1=*(u32 *)(blkbuf+16+(3-i)*6+0);
//		px0=rgbtab[(pxy0>>21)&7];	px1=rgbtab[(pxy0>>18)&7];
//		px2=rgbtab[(pxy0>>15)&7];	px3=rgbtab[(pxy0>>12)&7];
//		px4=rgbtab[(pxy0>> 9)&7];	px5=rgbtab[(pxy0>> 6)&7];
//		px6=rgbtab[(pxy0>> 3)&7];	px7=rgbtab[(pxy0>> 0)&7];
//		px8=rgbtab[(pxy1>>21)&7];	px9=rgbtab[(pxy1>>18)&7];
//		pxA=rgbtab[(pxy1>>15)&7];	pxB=rgbtab[(pxy1>>12)&7];
//		pxC=rgbtab[(pxy1>> 9)&7];	pxD=rgbtab[(pxy1>> 6)&7];
//		pxE=rgbtab[(pxy1>> 3)&7];	pxF=rgbtab[(pxy1>> 0)&7];
		ct0=ibuf+(i*2+0)*ystr;	ct1=ibuf+(i*2+1)*ystr;

		lqtvq_fillu32_8x1_8x1((u32 *)ct0,
			px0, px1, px2, px3, px4, px5, px6, px7);
		lqtvq_fillu32_8x1_8x1((u32 *)ct1,
			px8, px9, pxA, pxB, pxC, pxD, pxE, pxF);
	}
}

/* 8x8x4 */
void LQTVQ_DecBlock3BGRA(byte *blkbuf, byte *ibuf, int ystr)
{
	u32 rgbtab[16];
	byte *ct0, *ct1;
	byte *ct2, *ct3;
	int acy, acu, acv, dcy, dcu, dcv;
	int mcy, mcu, mcv, ncy, ncu, ncv;

	int cr0, cr1, cr2, cr3, cr4, cr5, cr6, cr7;
	int cg0, cg1, cg2, cg3, cg4, cg5, cg6, cg7;
	int cb0, cb1, cb2, cb3, cb4, cb5, cb6, cb7;
	int cr8, cr9, crA, crB, crC, crD, crE, crF;
	int cg8, cg9, cgA, cgB, cgC, cgD, cgE, cgF;
	int cb8, cb9, cbA, cbB, cbC, cbD, cbE, cbF;

	int cy0, cy1, cy2, cy3;
	int cy, cu, cv, cu1, cv1;

	u32 px0, px1, px2, px3;
	u32 px4, px5, px6, px7;
	u32 px8, px9, pxA, pxB;
	u32 pxC, pxD, pxE, pxF;

	u32 pxy0, pxy1, pxu, pxv;
	int i, j, k;
	
	acy=*(s16 *)(blkbuf+ 4);	acu=*(s16 *)(blkbuf+ 6);
	acv=*(s16 *)(blkbuf+ 8);	dcy=*(s16 *)(blkbuf+10);
	dcu=*(s16 *)(blkbuf+12);	dcv=*(s16 *)(blkbuf+14);
	
	mcy=acy-(dcy>>1); ncy=mcy+dcy;
	mcu=acu-(dcu>>1); ncu=mcu+dcu;
	mcv=acv-(dcv>>1); ncv=mcv+dcv;

//	cu1=(acu-128)<<1;	cv1=(acv-128)<<1;
	cu1=acu;	cv1=acv;
	cg0=mcy; cb0=cg0+cu1; cr0=cg0+cv1;
	cgF=ncy; cbF=cgF+cu1; crF=cgF+cv1;

	if((cr0|cg0|cb0)>>8)
	{
		cr0=lqtvq_clamp255(cr0);
		cg0=lqtvq_clamp255(cg0);
		cb0=lqtvq_clamp255(cb0);
	}

	if((crF|cgF|cbF)>>8)
	{
		crF=lqtvq_clamp255(crF);
		cgF=lqtvq_clamp255(cgF);
		cbF=lqtvq_clamp255(cbF);
	}

	cr1=(14*cr0+ 1*crF)>>4;	cg1=(14*cg0+ 1*cgF)>>4;	cb1=(14*cb0+ 1*cbF)>>4;
	cr2=(13*cr0+ 2*crF)>>4;	cg2=(13*cg0+ 2*cgF)>>4;	cb2=(13*cb0+ 2*cbF)>>4;
	cr3=(12*cr0+ 3*crF)>>4;	cg3=(12*cg0+ 3*cgF)>>4;	cb3=(12*cb0+ 3*cbF)>>4;
	cr4=(11*cr0+ 4*crF)>>4;	cg4=(11*cg0+ 4*cgF)>>4;	cb4=(11*cb0+ 4*cbF)>>4;
	cr5=(10*cr0+ 5*crF)>>4;	cg5=(10*cg0+ 5*cgF)>>4;	cb5=(10*cb0+ 5*cbF)>>4;
	cr6=( 9*cr0+ 6*crF)>>4;	cg6=( 9*cg0+ 6*cgF)>>4;	cb6=( 9*cb0+ 6*cbF)>>4;
	cr7=( 8*cr0+ 7*crF)>>4;	cg7=( 8*cg0+ 7*cgF)>>4;	cb7=( 8*cb0+ 7*cbF)>>4;
	cr8=( 7*cr0+ 8*crF)>>4;	cg8=( 7*cg0+ 8*cgF)>>4;	cb8=( 7*cb0+ 8*cbF)>>4;
	cr9=( 6*cr0+ 9*crF)>>4;	cg9=( 6*cg0+ 9*cgF)>>4;	cb9=( 6*cb0+ 9*cbF)>>4;
	crA=( 5*cr0+10*crF)>>4;	cgA=( 5*cg0+10*cgF)>>4;	cbA=( 5*cb0+10*cbF)>>4;
	crB=( 4*cr0+11*crF)>>4;	cgB=( 4*cg0+11*cgF)>>4;	cbB=( 4*cb0+11*cbF)>>4;
	crC=( 3*cr0+12*crF)>>4;	cgC=( 3*cg0+12*cgF)>>4;	cbC=( 3*cb0+12*cbF)>>4;
	crD=( 2*cr0+13*crF)>>4;	cgD=( 2*cg0+13*cgF)>>4;	cbD=( 2*cb0+13*cbF)>>4;
	crE=( 1*cr0+14*crF)>>4;	cgE=( 1*cg0+14*cgF)>>4;	cbE=( 1*cb0+14*cbF)>>4;

	rgbtab[ 0]=0xFF000000|(cr0<<16)|(cg0<<8)|cb0;
	rgbtab[ 1]=0xFF000000|(cr1<<16)|(cg1<<8)|cb1;
	rgbtab[ 2]=0xFF000000|(cr2<<16)|(cg2<<8)|cb2;
	rgbtab[ 3]=0xFF000000|(cr3<<16)|(cg3<<8)|cb3;
	rgbtab[ 4]=0xFF000000|(cr4<<16)|(cg4<<8)|cb4;
	rgbtab[ 5]=0xFF000000|(cr5<<16)|(cg5<<8)|cb5;
	rgbtab[ 6]=0xFF000000|(cr6<<16)|(cg6<<8)|cb6;
	rgbtab[ 7]=0xFF000000|(cr7<<16)|(cg7<<8)|cb7;
	rgbtab[ 8]=0xFF000000|(cr8<<16)|(cg8<<8)|cb8;
	rgbtab[ 9]=0xFF000000|(cr9<<16)|(cg9<<8)|cb9;
	rgbtab[10]=0xFF000000|(crA<<16)|(cgA<<8)|cbA;
	rgbtab[11]=0xFF000000|(crB<<16)|(cgB<<8)|cbB;
	rgbtab[12]=0xFF000000|(crC<<16)|(cgC<<8)|cbC;
	rgbtab[13]=0xFF000000|(crD<<16)|(cgD<<8)|cbD;
	rgbtab[14]=0xFF000000|(crE<<16)|(cgE<<8)|cbE;
	rgbtab[15]=0xFF000000|(crF<<16)|(cgF<<8)|cbF;

	for(i=0; i<4; i++)
	{
		pxy0=*(u32 *)(blkbuf+16+i*8+4);
		pxy1=*(u32 *)(blkbuf+16+i*8+0);
		pxF=rgbtab[(pxy0>>28)&15];	pxE=rgbtab[(pxy0>>24)&15];
		pxD=rgbtab[(pxy0>>20)&15];	pxC=rgbtab[(pxy0>>16)&15];
		pxB=rgbtab[(pxy0>>12)&15];	pxA=rgbtab[(pxy0>> 8)&15];
		px9=rgbtab[(pxy0>> 4)&15];	px8=rgbtab[(pxy0>> 0)&15];
		px7=rgbtab[(pxy1>>28)&15];	px6=rgbtab[(pxy1>>24)&15];
		px5=rgbtab[(pxy1>>20)&15];	px4=rgbtab[(pxy1>>16)&15];
		px3=rgbtab[(pxy1>>12)&15];	px2=rgbtab[(pxy1>> 8)&15];
		px1=rgbtab[(pxy1>> 4)&15];	px0=rgbtab[(pxy1>> 0)&15];

//		pxy0=*(u32 *)(blkbuf+16+(3-i)*8+4);
//		pxy1=*(u32 *)(blkbuf+16+(3-i)*8+0);
//		px0=rgbtab[(pxy0>>28)&15];	px1=rgbtab[(pxy0>>24)&15];
//		px2=rgbtab[(pxy0>>20)&15];	px3=rgbtab[(pxy0>>16)&15];
//		px4=rgbtab[(pxy0>>12)&15];	px5=rgbtab[(pxy0>> 8)&15];
//		px6=rgbtab[(pxy0>> 4)&15];	px7=rgbtab[(pxy0>> 0)&15];
//		px8=rgbtab[(pxy1>>28)&15];	px9=rgbtab[(pxy1>>24)&15];
//		pxA=rgbtab[(pxy1>>20)&15];	pxB=rgbtab[(pxy1>>16)&15];
//		pxC=rgbtab[(pxy1>>12)&15];	pxD=rgbtab[(pxy1>> 8)&15];
//		pxE=rgbtab[(pxy1>> 4)&15];	pxF=rgbtab[(pxy1>> 0)&15];
		ct0=ibuf+(i*2+0)*ystr;	ct1=ibuf+(i*2+1)*ystr;

		lqtvq_fillu32_8x1_8x1((u32 *)ct0,
			px0, px1, px2, px3, px4, px5, px6, px7);
		lqtvq_fillu32_8x1_8x1((u32 *)ct1,
			px8, px9, pxA, pxB, pxC, pxD, pxE, pxF);
	}
}

/* 8x8x2 */
void LQTVQ_DecBlock4BGRA(byte *blkbuf, byte *ibuf, int ystr)
{
	int ytab[4], utab[4], vtab[4];
	byte *ct0, *ct1;
	int acy, acu, acv, dcy, dcu, dcv;
	int mcy, mcu, mcv, ncy, ncu, ncv;

	int cr0, cr1, cr2, cr3;
	int cg0, cg1, cg2, cg3;
	int cb0, cb1, cb2, cb3;
	int cy0, cy1, cy2, cy3;
	int cy, cu, cv, cu0, cv0, cu1, cv1, cu2, cv2, cu3, cv3;

	u32 pxy0, pxy1, pxu, pxv;
	int i, j, k;
	
	acy=*(s16 *)(blkbuf+ 4);
	acu=*(s16 *)(blkbuf+ 6);
	acv=*(s16 *)(blkbuf+ 8);
	dcy=*(s16 *)(blkbuf+10);
	dcu=*(s16 *)(blkbuf+12);
	dcv=*(s16 *)(blkbuf+14);
	
	mcy=acy-(dcy>>1); ncy=mcy+dcy;
	mcu=acu-(dcu>>1); ncu=mcu+dcu;
	mcv=acv-(dcv>>1); ncv=mcv+dcv;

	ytab[0]=mcy;
	ytab[1]=(11*mcy+ 5*ncy)>>4;
	ytab[2]=( 5*mcy+11*ncy)>>4;
	ytab[3]=ncy;

	utab[0]=mcu;
	utab[1]=(11*mcu+ 5*ncu)>>4;
	utab[2]=( 5*mcu+11*ncu)>>4;
	utab[3]=ncu;

	vtab[0]=mcv;
	vtab[1]=(11*mcv+ 5*ncv)>>4;
	vtab[2]=( 5*mcv+11*ncv)>>4;
	vtab[3]=ncv;
	
	if(blkbuf[0]==0x18)
	{
		pxu=*(u32 *)(blkbuf+32);
		pxv=*(u32 *)(blkbuf+36);

		for(i=0; i<4; i++)
		{
			ct0=ibuf+(i*2+0)*ystr;
			ct1=ibuf+(i*2+1)*ystr;
//			pxy0=*(u32 *)(blkbuf+16+(3-i)*4+0);
			pxy0=*(u32 *)(blkbuf+16+i*4+0);
		
			for(j=0; j<4; j++)
			{
//				k=(3-j)*4;
				k=j*4;
				cy0=ytab[(pxy0>>(k+18))&3];
				cy1=ytab[(pxy0>>(k+16))&3];
				cy2=ytab[(pxy0>>(k+ 2))&3];
				cy3=ytab[(pxy0>>(k+ 0))&3];
//				k=30-((i*4+j)*2);
				k=(i*4+j)*2;
				cu=utab[(pxu>>k)&3];
				cv=vtab[(pxv>>k)&3];
				
//				cu1=(cu-128)<<1;
//				cv1=(cv-128)<<1;
				cu1=cu;	cv1=cv;

				cg0=cy0; cb0=cg0+cu1; cr0=cg0+cv1;
				cg1=cy1; cb1=cg1+cu1; cr1=cg1+cv1;
				cg2=cy2; cb2=cg2+cu1; cr2=cg2+cv1;
				cg3=cy3; cb3=cg3+cu1; cr3=cg3+cv1;
				
				if((cr0|cr1|cr2|cr3)>>8)
				{
					cr0=lqtvq_clamp255(cr0);
					cr1=lqtvq_clamp255(cr1);
					cr2=lqtvq_clamp255(cr2);
					cr3=lqtvq_clamp255(cr3);
				}

				if((cg0|cg1|cg2|cg3)>>8)
				{
					cg0=lqtvq_clamp255(cg0);
					cg1=lqtvq_clamp255(cg1);
					cg2=lqtvq_clamp255(cg2);
					cg3=lqtvq_clamp255(cg3);
				}

				if((cb0|cb1|cb2|cb3)>>8)
				{
					cb0=lqtvq_clamp255(cb0);
					cb1=lqtvq_clamp255(cb1);
					cb2=lqtvq_clamp255(cb2);
					cb3=lqtvq_clamp255(cb3);
				}
				
				ct0[0]=cb0;	ct0[1]=cg0;	ct0[2]=cr0;	ct0[3]=255;
				ct0[4]=cb1;	ct0[5]=cg1;	ct0[6]=cr1;	ct0[7]=255;
				ct1[0]=cb2;	ct1[1]=cg2;	ct1[2]=cr2;	ct1[3]=255;
				ct1[4]=cb3;	ct1[5]=cg3;	ct1[6]=cr3;	ct1[7]=255;
				
				ct0+=8;
				ct1+=8;
			}
		}
		return;
	}

	if(blkbuf[0]==0x1A)
	{
		for(i=0; i<4; i++)
		{
//			j=(1-(i>>1))*4;
			j=(i>>1)*4;
			pxu=*(u32 *)(blkbuf+32+j);
			pxv=*(u32 *)(blkbuf+40+j);

			ct0=ibuf+(i*2+0)*ystr;
			ct1=ibuf+(i*2+1)*ystr;
//			pxy0=*(u32 *)(blkbuf+16+(3-i)*4+0);
			pxy0=*(u32 *)(blkbuf+16+i*4+0);
		
			for(j=0; j<4; j++)
			{
//				k=(3-j)*4;
				k=j*4;
				cy0=ytab[(pxy0>>(k+18))&3];
				cy1=ytab[(pxy0>>(k+16))&3];
				cy2=ytab[(pxy0>>(k+ 2))&3];
				cy3=ytab[(pxy0>>(k+ 0))&3];
//				k=30-((i*4+j)*2);
//				cu=utab[(pxu>>k)&3];
//				cv=vtab[(pxv>>k)&3];

//				k=30-(((i&1)*8+j)*2);
				k=((i&1)*8+j)*2;
				cu0=utab[(pxu>>(k+8))&3];
				cu1=utab[(pxu>>(k+0))&3];
				cv0=vtab[(pxv>>(k+8))&3];
				cv1=vtab[(pxv>>(k+0))&3];
				
//				cu2=(cu0-128)<<1;	cv2=(cv0-128)<<1;
//				cu3=(cu1-128)<<1;	cv3=(cv1-128)<<1;
				cu2=cu0;	cv2=cv0;
				cu3=cu1;	cv3=cv1;

				cg0=cy0; cb0=cg0+cu2; cr0=cg0+cv2;
				cg1=cy1; cb1=cg1+cu2; cr1=cg1+cv2;
				cg2=cy2; cb2=cg2+cu3; cr2=cg2+cv3;
				cg3=cy3; cb3=cg3+cu3; cr3=cg3+cv3;
				
				if((cr0|cr1|cr2|cr3)>>8)
				{
					cr0=lqtvq_clamp255(cr0);
					cr1=lqtvq_clamp255(cr1);
					cr2=lqtvq_clamp255(cr2);
					cr3=lqtvq_clamp255(cr3);
				}

				if((cg0|cg1|cg2|cg3)>>8)
				{
					cg0=lqtvq_clamp255(cg0);
					cg1=lqtvq_clamp255(cg1);
					cg2=lqtvq_clamp255(cg2);
					cg3=lqtvq_clamp255(cg3);
				}

				if((cb0|cb1|cb2|cb3)>>8)
				{
					cb0=lqtvq_clamp255(cb0);
					cb1=lqtvq_clamp255(cb1);
					cb2=lqtvq_clamp255(cb2);
					cb3=lqtvq_clamp255(cb3);
				}
				
				ct0[0]=cb0;	ct0[1]=cg0;	ct0[2]=cr0;	ct0[3]=255;
				ct0[4]=cb1;	ct0[5]=cg1;	ct0[6]=cr1;	ct0[7]=255;
				ct1[0]=cb2;	ct1[1]=cg2;	ct1[2]=cr2;	ct1[3]=255;
				ct1[4]=cb3;	ct1[5]=cg3;	ct1[6]=cr3;	ct1[7]=255;
				ct0+=8;		ct1+=8;
			}
		}
		return;
	}
}


/* 8x8x2+UV4x4x2 LDR8 */
void LQTVQ_DecBlock5BGRA(byte *blkbuf, byte *ibuf, int ystr)
{
	int ytab[4], utab[4], vtab[4];
	byte *ct0, *ct1;
	int acy, acu, acv, dcy, dcu, dcv;
	int mcy, mcu, mcv, ncy, ncu, ncv;

	int cr0, cr1, cr2, cr3;
	int cg0, cg1, cg2, cg3;
	int cb0, cb1, cb2, cb3;
	int cy0, cy1, cy2, cy3;
	int cy, cu, cv, cu0, cv0, cu1, cv1, cu2, cv2, cu3, cv3;

	u32 pxy0, pxy1, pxu, pxv;
	int i, j, k;
	
	acy=blkbuf[2];
	acu=(blkbuf[3]-128)<<1;
	acv=(blkbuf[4]-128)<<1;
	dcy=blkbuf[5];
	dcu=blkbuf[6]<<1;
	dcv=blkbuf[7]<<1;
	
	mcy=acy-(dcy>>1); ncy=mcy+dcy;
	mcu=acu-(dcu>>1); ncu=mcu+dcu;
	mcv=acv-(dcv>>1); ncv=mcv+dcv;

	ytab[0]=mcy;
	ytab[1]=(11*mcy+ 5*ncy)>>4;
	ytab[2]=( 5*mcy+11*ncy)>>4;
	ytab[3]=ncy;

	utab[0]=mcu;
	utab[1]=(11*mcu+ 5*ncu)>>4;
	utab[2]=( 5*mcu+11*ncu)>>4;
	utab[3]=ncu;

	vtab[0]=mcv;
	vtab[1]=(11*mcv+ 5*ncv)>>4;
	vtab[2]=( 5*mcv+11*ncv)>>4;
	vtab[3]=ncv;
	
	if(blkbuf[0]==0x10)
	{
		pxu=*(u32 *)(blkbuf+24);
		pxv=*(u32 *)(blkbuf+28);

		for(i=0; i<4; i++)
		{
			ct0=ibuf+(i*2+0)*ystr;
			ct1=ibuf+(i*2+1)*ystr;
			pxy0=*(u32 *)(blkbuf+8+i*4+0);
		
			for(j=0; j<4; j++)
			{
				k=j*4;
				cy0=ytab[(pxy0>>(k+18))&3];
				cy1=ytab[(pxy0>>(k+16))&3];
				cy2=ytab[(pxy0>>(k+ 2))&3];
				cy3=ytab[(pxy0>>(k+ 0))&3];
				k=(i*4+j)*2;
				cu=utab[(pxu>>k)&3];
				cv=vtab[(pxv>>k)&3];
				
				cu1=cu;	cv1=cv;
				cg0=cy0; cb0=cg0+cu1; cr0=cg0+cv1;
				cg1=cy1; cb1=cg1+cu1; cr1=cg1+cv1;
				cg2=cy2; cb2=cg2+cu1; cr2=cg2+cv1;
				cg3=cy3; cb3=cg3+cu1; cr3=cg3+cv1;
				
				if((cr0|cr1|cr2|cr3)>>8)
				{	cr0=lqtvq_clamp255(cr0); cr1=lqtvq_clamp255(cr1);
					cr2=lqtvq_clamp255(cr2); cr3=lqtvq_clamp255(cr3); }
				if((cg0|cg1|cg2|cg3)>>8)
				{	cg0=lqtvq_clamp255(cg0); cg1=lqtvq_clamp255(cg1);
					cg2=lqtvq_clamp255(cg2); cg3=lqtvq_clamp255(cg3); }
				if((cb0|cb1|cb2|cb3)>>8)
				{	cb0=lqtvq_clamp255(cb0); cb1=lqtvq_clamp255(cb1);
					cb2=lqtvq_clamp255(cb2); cb3=lqtvq_clamp255(cb3); }
				
				ct0[0]=cb0;	ct0[1]=cg0;	ct0[2]=cr0;	ct0[3]=255;
				ct0[4]=cb1;	ct0[5]=cg1;	ct0[6]=cr1;	ct0[7]=255;
				ct1[0]=cb2;	ct1[1]=cg2;	ct1[2]=cr2;	ct1[3]=255;
				ct1[4]=cb3;	ct1[5]=cg3;	ct1[6]=cr3;	ct1[7]=255;
				
				ct0+=8;
				ct1+=8;
			}
		}
		return;
	}
}

/* XxXx1 */
void LQTVQ_DecBlock6BGRA(byte *blkbuf, byte *ibuf, int ystr)
{
	u32 rgbtab[4];
	byte *ct0, *ct1;
	byte *ct2, *ct3;
	int acy, acu, acv, dcy, dcu, dcv;
	int mcy, mcu, mcv, ncy, ncu, ncv;

	int cr0, cr1, cr2, cr3;
	int cg0, cg1, cg2, cg3;
	int cb0, cb1, cb2, cb3;
	int cy0, cy1, cy2, cy3;
	int cy, cu, cv, cu1, cv1;

	u32 px0, px1, px2, px3;
	u32 px4, px5, px6, px7;
	u32 px8, px9, pxA, pxB;
	u32 pxC, pxD, pxE, pxF;

	u32 pxy0, pxy1, pxu, pxv;
	int i, j, k;
	
	acy=*(s16 *)(blkbuf+ 4);
	acu=*(s16 *)(blkbuf+ 6);
	acv=*(s16 *)(blkbuf+ 8);
	dcy=*(s16 *)(blkbuf+10);
	dcu=*(s16 *)(blkbuf+12);
	dcv=*(s16 *)(blkbuf+14);
	
	mcy=acy-(dcy>>1); ncy=mcy+dcy;
	mcu=acu-(dcu>>1); ncu=mcu+dcu;
	mcv=acv-(dcv>>1); ncv=mcv+dcv;

//	mcy=acy-(dcy>>2); ncy=mcy+(dcy>>1);
//	mcu=acu-(dcu>>2); ncu=mcu+(dcu>>1);
//	mcv=acv-(dcv>>2); ncv=mcv+(dcv>>1);

//	mcy=acy; ncy=acy;
//	acu=0; acv=0;

	cu1=acu;	cv1=acv;
	cg0=mcy; cb0=cg0+cu1; cr0=cg0+cv1;
	cg3=ncy; cb3=cg3+cu1; cr3=cg3+cv1;
	
	

	if((cr0|cg0|cb0)>>8)
	{
		cr0=lqtvq_clamp255(cr0);
		cg0=lqtvq_clamp255(cg0);
		cb0=lqtvq_clamp255(cb0);
	}

	if((cr3|cg3|cb3)>>8)
	{
		cr3=lqtvq_clamp255(cr3);
		cg3=lqtvq_clamp255(cg3);
		cb3=lqtvq_clamp255(cb3);
	}

	rgbtab[0]=0xFF000000|(cr0<<16)|(cg0<<8)|cb0;
	rgbtab[1]=0xFF000000|(cr3<<16)|(cg3<<8)|cb3;

	switch(blkbuf[0])
	{
	case 0x01:
		pxy0=blkbuf[16];
		for(i=0; i<2; i++)
		{
			pxu=rgbtab[(pxy0>>(i*2+0))&1];
			pxv=rgbtab[(pxy0>>(i*2+1))&1];
			ct0=ibuf+(i*4+0)*ystr;	ct1=ibuf+(i*4+1)*ystr;
			ct2=ibuf+(i*4+2)*ystr;	ct3=ibuf+(i*4+3)*ystr;
			lqtvq_fillu32_8x1_2x1((u32 *)ct0, pxu, pxv);
			lqtvq_fillu32_8x1_2x1((u32 *)ct1, pxu, pxv);
			lqtvq_fillu32_8x1_2x1((u32 *)ct2, pxu, pxv);
			lqtvq_fillu32_8x1_2x1((u32 *)ct3, pxu, pxv);
		}
		break;
	case 0x02:
		pxy0=blkbuf[16];
		for(i=0; i<2; i++)
		{
			px0=rgbtab[(pxy0>>(i*4+0))&1];
			px1=rgbtab[(pxy0>>(i*4+1))&1];
			px2=rgbtab[(pxy0>>(i*4+2))&1];
			px3=rgbtab[(pxy0>>(i*4+3))&1];

			ct0=ibuf+(i*4+0)*ystr;	ct1=ibuf+(i*4+1)*ystr;
			ct2=ibuf+(i*4+2)*ystr;	ct3=ibuf+(i*4+3)*ystr;
			lqtvq_fillu32_8x1_4x1((u32 *)ct0, px0, px1, px2, px3);
			lqtvq_fillu32_8x1_4x1((u32 *)ct1, px0, px1, px2, px3);
			lqtvq_fillu32_8x1_4x1((u32 *)ct2, px0, px1, px2, px3);
			lqtvq_fillu32_8x1_4x1((u32 *)ct3, px0, px1, px2, px3);
		}
		break;
	case 0x03:
		pxy0=blkbuf[16];
		for(i=0; i<4; i++)
		{
			pxu=rgbtab[(pxy0>>(i*2+0))&1];
			pxv=rgbtab[(pxy0>>(i*2+1))&1];
			ct0=ibuf+(i*2+0)*ystr;	ct1=ibuf+(i*2+1)*ystr;
			lqtvq_fillu32_8x1_2x1((u32 *)ct0, pxu, pxv);
			lqtvq_fillu32_8x1_2x1((u32 *)ct1, pxu, pxv);
		}
		break;
	case 0x04:
		pxy0=*(u16 *)(blkbuf+16);
		for(i=0; i<4; i++)
		{
			px0=rgbtab[(pxy0>>(i*4+0))&1];
			px1=rgbtab[(pxy0>>(i*4+1))&1];
			px2=rgbtab[(pxy0>>(i*4+2))&1];
			px3=rgbtab[(pxy0>>(i*4+3))&1];
			ct0=ibuf+(i*2+0)*ystr;	ct1=ibuf+(i*2+1)*ystr;
			lqtvq_fillu32_8x1_4x1((u32 *)ct0, px0, px1, px2, px3);
			lqtvq_fillu32_8x1_4x1((u32 *)ct1, px0, px1, px2, px3);
		}
		break;
	case 0x05:
		for(i=0; i<2; i++)
		{
			pxy0=*(u16 *)(blkbuf+16+i*2);
			pxF=rgbtab[(pxy0>>15)&1];	pxE=rgbtab[(pxy0>>14)&1];
			pxD=rgbtab[(pxy0>>13)&1];	pxC=rgbtab[(pxy0>>12)&1];
			pxB=rgbtab[(pxy0>>11)&1];	pxA=rgbtab[(pxy0>>10)&1];
			px9=rgbtab[(pxy0>> 9)&1];	px8=rgbtab[(pxy0>> 8)&1];
			px7=rgbtab[(pxy0>> 7)&1];	px6=rgbtab[(pxy0>> 6)&1];
			px5=rgbtab[(pxy0>> 5)&1];	px4=rgbtab[(pxy0>> 4)&1];
			px3=rgbtab[(pxy0>> 3)&1];	px2=rgbtab[(pxy0>> 2)&1];
			px1=rgbtab[(pxy0>> 1)&1];	px0=rgbtab[(pxy0>> 0)&1];

			ct0=ibuf+(i*4+0)*ystr;	ct1=ibuf+(i*4+1)*ystr;
			ct2=ibuf+(i*4+2)*ystr;	ct3=ibuf+(i*4+3)*ystr;

			lqtvq_fillu32_8x1_8x1((u32 *)ct0,
				px0, px1, px2, px3, px4, px5, px6, px7);
			lqtvq_fillu32_8x1_8x1((u32 *)ct1,
				px0, px1, px2, px3, px4, px5, px6, px7);
			lqtvq_fillu32_8x1_8x1((u32 *)ct2,
				px8, px9, pxA, pxB, pxC, pxD, pxE, pxF);
			lqtvq_fillu32_8x1_8x1((u32 *)ct3,
				px8, px9, pxA, pxB, pxC, pxD, pxE, pxF);
		}
		break;
	case 0x06:
		for(i=0; i<2; i++)
		{
			pxy0=*(u16 *)(blkbuf+16+i*2);
			pxF=rgbtab[(pxy0>>15)&1];	pxE=rgbtab[(pxy0>>14)&1];
			pxD=rgbtab[(pxy0>>13)&1];	pxC=rgbtab[(pxy0>>12)&1];
			pxB=rgbtab[(pxy0>>11)&1];	pxA=rgbtab[(pxy0>>10)&1];
			px9=rgbtab[(pxy0>> 9)&1];	px8=rgbtab[(pxy0>> 8)&1];
			px7=rgbtab[(pxy0>> 7)&1];	px6=rgbtab[(pxy0>> 6)&1];
			px5=rgbtab[(pxy0>> 5)&1];	px4=rgbtab[(pxy0>> 4)&1];
			px3=rgbtab[(pxy0>> 3)&1];	px2=rgbtab[(pxy0>> 2)&1];
			px1=rgbtab[(pxy0>> 1)&1];	px0=rgbtab[(pxy0>> 0)&1];

			ct0=ibuf+(i*4+0)*ystr;	ct1=ibuf+(i*4+1)*ystr;
			ct2=ibuf+(i*4+2)*ystr;	ct3=ibuf+(i*4+3)*ystr;

			lqtvq_fillu32_8x1_4x1((u32 *)ct0, px0, px1, px2, px3);
			lqtvq_fillu32_8x1_4x1((u32 *)ct1, px4, px5, px6, px7);
			lqtvq_fillu32_8x1_4x1((u32 *)ct2, px8, px9, pxA, pxB);
			lqtvq_fillu32_8x1_4x1((u32 *)ct3, pxC, pxD, pxE, pxF);
		}
		break;
	case 0x07:
		for(i=0; i<4; i++)
		{
			pxy0=*(u16 *)(blkbuf+16+i*2);
			pxF=rgbtab[(pxy0>>15)&1];	pxE=rgbtab[(pxy0>>14)&1];
			pxD=rgbtab[(pxy0>>13)&1];	pxC=rgbtab[(pxy0>>12)&1];
			pxB=rgbtab[(pxy0>>11)&1];	pxA=rgbtab[(pxy0>>10)&1];
			px9=rgbtab[(pxy0>> 9)&1];	px8=rgbtab[(pxy0>> 8)&1];
			px7=rgbtab[(pxy0>> 7)&1];	px6=rgbtab[(pxy0>> 6)&1];
			px5=rgbtab[(pxy0>> 5)&1];	px4=rgbtab[(pxy0>> 4)&1];
			px3=rgbtab[(pxy0>> 3)&1];	px2=rgbtab[(pxy0>> 2)&1];
			px1=rgbtab[(pxy0>> 1)&1];	px0=rgbtab[(pxy0>> 0)&1];

			ct0=ibuf+(i*2+0)*ystr;	ct1=ibuf+(i*2+1)*ystr;

			lqtvq_fillu32_8x1_8x1((u32 *)ct0,
				px0, px1, px2, px3, px4, px5, px6, px7);
			lqtvq_fillu32_8x1_8x1((u32 *)ct1,
				px8, px9, pxA, pxB, pxC, pxD, pxE, pxF);
		}
		break;
	}
}

force_inline void lqtvq_fillau32_8x1(byte *px, byte v)
{
	px[ 3]=v; px[ 7]=v; px[11]=v; px[15]=v;
	px[19]=v; px[23]=v; px[27]=v; px[31]=v;
}

force_inline void lqtvq_fillau32_8x1_2x1(byte *px, byte u, byte v)
{
	px[ 3]=u; px[ 7]=u; px[11]=u; px[15]=u;
	px[19]=v; px[23]=v; px[27]=v; px[31]=v;
}

force_inline void lqtvq_fillau32_8x1_4x1(byte *px,
	byte v0, byte v1, byte v2, byte v3)
{
	px[ 3]=v0; px[ 7]=v0; px[11]=v1; px[15]=v1;
	px[19]=v2; px[23]=v2; px[27]=v3; px[31]=v3;
}

force_inline void lqtvq_fillau32_8x1_8x1(byte *px,
	byte v0, byte v1, byte v2, byte v3,
	byte v4, byte v5, byte v6, byte v7)
{
	px[ 3]=v0; px[ 7]=v1; px[11]=v2; px[15]=v3;
	px[19]=v4; px[23]=v5; px[27]=v6; px[31]=v7;
}

void LQTVQ_DecBlockAlphaBGRA(byte *blkbuf, byte *ibuf, int ystr)
{
	byte tab[4];
	u32 pxy0;
	byte px0, px1, px2, px3, px4, px5, px6, px7;
	byte px8, px9, pxA, pxB, pxC, pxD, pxE, pxF;
	int ma, na, ca, da;
	int i, j, k;
	
//	if(*(u16 *)(blkbuf+2)==0x00FF)
//		return;

	ca=blkbuf[2];
	da=blkbuf[3];

	switch(blkbuf[1])
	{
	case 0:
		lqtvq_fillau32_8x1(ibuf+0*ystr, ca);
		lqtvq_fillau32_8x1(ibuf+1*ystr, ca);
		lqtvq_fillau32_8x1(ibuf+2*ystr, ca);
		lqtvq_fillau32_8x1(ibuf+3*ystr, ca);
		lqtvq_fillau32_8x1(ibuf+4*ystr, ca);
		lqtvq_fillau32_8x1(ibuf+5*ystr, ca);
		lqtvq_fillau32_8x1(ibuf+6*ystr, ca);
		lqtvq_fillau32_8x1(ibuf+7*ystr, ca);
		break;
	case 4:
		break;
	case 12:
		ma=(ca<<1)-da;
		na=ma+da;
		
		tab[0]=ma;
		tab[1]=(11*ma+ 5*na)>>4;
		tab[2]=( 5*ma+11*na)>>4;
		tab[3]=na;

		for(i=0; i<4; i++)
		{
			pxy0=*(u32 *)(blkbuf+48+i*4);
			pxF=tab[(pxy0>>30)&3];	pxE=tab[(pxy0>>28)&3];
			pxD=tab[(pxy0>>26)&3];	pxC=tab[(pxy0>>24)&3];
			pxB=tab[(pxy0>>22)&3];	pxA=tab[(pxy0>>20)&3];
			px9=tab[(pxy0>>18)&3];	px8=tab[(pxy0>>16)&3];
			px7=tab[(pxy0>>14)&3];	px6=tab[(pxy0>>12)&3];
			px5=tab[(pxy0>>10)&3];	px4=tab[(pxy0>> 8)&3];
			px3=tab[(pxy0>> 6)&3];	px2=tab[(pxy0>> 4)&3];
			px1=tab[(pxy0>> 2)&3];	px0=tab[(pxy0>> 0)&3];

//			pxy0=*(u32 *)(blkbuf+48+(3-i)*4);
//			px0=tab[(pxy0>>30)&3];	px1=tab[(pxy0>>28)&3];
//			px2=tab[(pxy0>>26)&3];	px3=tab[(pxy0>>24)&3];
//			px4=tab[(pxy0>>22)&3];	px5=tab[(pxy0>>20)&3];
//			px6=tab[(pxy0>>18)&3];	px7=tab[(pxy0>>16)&3];
//			px8=tab[(pxy0>>14)&3];	px9=tab[(pxy0>>12)&3];
//			pxA=tab[(pxy0>>10)&3];	pxB=tab[(pxy0>> 8)&3];
//			pxC=tab[(pxy0>> 6)&3];	pxD=tab[(pxy0>> 4)&3];
//			pxE=tab[(pxy0>> 2)&3];	pxF=tab[(pxy0>> 0)&3];
			lqtvq_fillau32_8x1_8x1(ibuf+(i*2+0)*ystr,
				px0, px1, px2, px3, px4, px5, px6, px7);
			lqtvq_fillau32_8x1_8x1(ibuf+(i*2+1)*ystr,
				px8, px9, pxA, pxB, pxC, pxD, pxE, pxF);
		}
		break;
	default:
		break;
	}
}

void LQTVQ_DecBlockBGRA(byte *blkbuf, byte *ibuf, int ystr)
{
	switch(blkbuf[0])
	{
	case 0x00:
		if(*(u16 *)(blkbuf+2)!=0x00FF)
//		if(((*(u32 *)blkbuf)&0xFFFFFF00)!=0x00FF0000)
		{
			if(!blkbuf[1] && !blkbuf[3])
			{
				LQTVQ_DecBlockFlatBGRA_FA(
					blkbuf, ibuf, ystr, blkbuf[2]);
				break;
			}

			LQTVQ_DecBlockFlatBGRA(blkbuf, ibuf, ystr);
			LQTVQ_DecBlockAlphaBGRA(blkbuf, ibuf, ystr);
			break;
		}
		LQTVQ_DecBlockFlatBGRA(blkbuf, ibuf, ystr);
		break;

	case 0x01: case 0x02:
	case 0x03: case 0x04:
	case 0x05: case 0x06:
	case 0x07:
		LQTVQ_DecBlock6BGRA(blkbuf, ibuf, ystr);
		if(*(u16 *)(blkbuf+2)!=0x00FF)
			LQTVQ_DecBlockAlphaBGRA(blkbuf, ibuf, ystr);
		break;

	case 0x09: case 0x0A:
	case 0x0B: case 0x0C:
	case 0x0D: case 0x0E:
	case 0x0F:
		LQTVQ_DecBlock1BGRA(blkbuf, ibuf, ystr);
		if(*(u16 *)(blkbuf+2)!=0x00FF)
//		if(((*(u32 *)blkbuf)&0xFFFFFF00)!=0x00FF0000)
			LQTVQ_DecBlockAlphaBGRA(blkbuf, ibuf, ystr);
		break;
	case 0x10:
		LQTVQ_DecBlock5BGRA(blkbuf, ibuf, ystr);
		break;

	case 0x13:
		LQTVQ_DecBlock2BGRA(blkbuf, ibuf, ystr);
		if(*(u16 *)(blkbuf+2)!=0x00FF)
//		if(((*(u32 *)blkbuf)&0xFFFFFF00)!=0x00FF0000)
			LQTVQ_DecBlockAlphaBGRA(blkbuf, ibuf, ystr);
		break;
	case 0x14:
		LQTVQ_DecBlock3BGRA(blkbuf, ibuf, ystr);
		if(*(u16 *)(blkbuf+2)!=0x00FF)
//		if(((*(u32 *)blkbuf)&0xFFFFFF00)!=0x00FF0000)
			LQTVQ_DecBlockAlphaBGRA(blkbuf, ibuf, ystr);
		break;
	case 0x18:
	case 0x1A:
	case 0x1B:
		LQTVQ_DecBlock4BGRA(blkbuf, ibuf, ystr);
		if(*(u16 *)(blkbuf+2)!=0x00FF)
//		if(((*(u32 *)blkbuf)&0xFFFFFF00)!=0x00FF0000)
			LQTVQ_DecBlockAlphaBGRA(blkbuf, ibuf, ystr);
		break;
	case 0x1C:
		LQTVQ_DecBlock0BGRA(blkbuf, ibuf, ystr);
		if(*(u16 *)(blkbuf+2)!=0x00FF)
//		if(((*(u32 *)blkbuf)&0xFFFFFF00)!=0x00FF0000)
			LQTVQ_DecBlockAlphaBGRA(blkbuf, ibuf, ystr);
		break;
	default:
		break;
	}
}

void LQTVQ_DecBlockBGRX(byte *blkbuf, byte *ibuf, int ystr)
{
	switch(blkbuf[0])
	{
	case 0x00:
		LQTVQ_DecBlockFlatBGRA(blkbuf, ibuf, ystr);
		break;

	case 0x01: case 0x02:
	case 0x03: case 0x04:
	case 0x05: case 0x06:
	case 0x07:
		LQTVQ_DecBlock6BGRA(blkbuf, ibuf, ystr);
		break;

	case 0x09: case 0x0A:
	case 0x0B: case 0x0C:
	case 0x0D: case 0x0E:
	case 0x0F:
		LQTVQ_DecBlock1BGRA(blkbuf, ibuf, ystr);
		break;
	case 0x13:
		LQTVQ_DecBlock2BGRA(blkbuf, ibuf, ystr);
		break;
	case 0x14:
		LQTVQ_DecBlock3BGRA(blkbuf, ibuf, ystr);
		break;
	case 0x18:
	case 0x1A:
	case 0x1B:
		LQTVQ_DecBlock4BGRA(blkbuf, ibuf, ystr);
		break;
	case 0x1C:
		LQTVQ_DecBlock0BGRA(blkbuf, ibuf, ystr);
		break;
	default:
		break;
	}
}

void LQTVQ_DecImageBGRA(byte *blks, byte *ibuf, int xs, int ys)
{
	byte *cs, *ct;
	int xs1, ys1, ystr;
	int bi;
	int i, j, k;
	
	LQTVQ_InitScTables();
	
	ystr=xs*4;
	xs1=xs>>3;
	ys1=ys>>3;
	
	for(i=0; i<ys1; i++)
	{
		cs=ibuf+(i*8)*ystr;
		ct=blks+((i*xs1)<<6);
		for(j=0; j<xs1; j++)
		{
			LQTVQ_DecBlockBGRA(ct, cs, ystr);
			ct+=64; cs+=32;
		}
	}
}

void LQTVQ_DecImageBGRX(byte *blks, byte *ibuf, int xs, int ys)
{
	byte *cs, *ct;
	int xs1, ys1, ystr;
	int bi;
	int i, j, k;
	
	LQTVQ_InitScTables();
	
	ystr=xs*4;
	xs1=xs>>3;
	ys1=ys>>3;
	
	for(i=0; i<ys1; i++)
	{
		cs=ibuf+(i*8)*ystr;
		ct=blks+((i*xs1)<<6);
		for(j=0; j<xs1; j++)
		{
			LQTVQ_DecBlockBGRX(ct, cs, ystr);
			ct+=64; cs+=32;
		}
	}
}


void LQTVQ_DecImageClrs(byte *blks, byte *ibuf,
	int xs, int ys, int clrs)
{
	LQTVQ_DecImageBGRA(blks, ibuf, xs, ys);
}
