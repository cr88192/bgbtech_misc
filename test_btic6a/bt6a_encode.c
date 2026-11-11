// #define BTIC6A_CBSZ_2X		512
// #define BTIC6A_CBSZ_4X		256
// #define BTIC6A_CBSZ_2X		1024
// #define BTIC6A_CBSZ_4X		512

#define BTIC6A_CBSZ_2X		4096
// #define BTIC6A_CBSZ_2X		2048
// #define BTIC6A_CBSZ_2X		1024
// #define BTIC6A_CBSZ_4X		1024
#define BTIC6A_CBSZ_4X		2048

#define BTIC6A_CBSZ_8X		512
// #define BTIC6A_CBSZ_8X		256
// #define BTIC6A_CBSZ_16X		256
#define BTIC6A_CBSZ_16X		128

// #define BTIC6A_CBSZ_8X		256
//#define BTIC6A_CBSZ_8X		128
// #define BTIC6A_CBSZ_16X		64
#define BTIC6A_CBMSK_2X		(BTIC6A_CBSZ_2X-1)
#define BTIC6A_CBMSK_4X		(BTIC6A_CBSZ_4X-1)
#define BTIC6A_CBMSK_8X		(BTIC6A_CBSZ_8X-1)
#define BTIC6A_CBMSK_16X	(BTIC6A_CBSZ_16X-1)


typedef struct BTIC6A_EncodeContext_s BTIC6A_EncodeContext;

struct BTIC6A_EncodeContext_s {
	byte *ct;			//current position
	byte *lct;			//last position
	byte *lct2;			//last position (2x2)
	byte *lct4;			//last position (4x4)
	byte *lct8;			//last position (8x8)
	byte *lct16;		//last position (16x16)
	byte *tfbuf;
	byte *tfzbuf;
	int tfbsz;

	byte cb_2x2_y[BTIC6A_CBSZ_2X*4];
	byte cb_2x2_c[BTIC6A_CBSZ_2X*2];
	u16 cb_4x4   [BTIC6A_CBSZ_4X*4];
	u16 cb_8x8   [BTIC6A_CBSZ_8X*4];
	u16 cb_16x16 [BTIC6A_CBSZ_16X*4];
	u16 rov_2x2;
	u16 rov_4x4;
	u16 rov_8x8;
	u16 rov_16x16;

	u16 rfr_2x2   [BTIC6A_CBSZ_2X];
	u16 rfr_4x4   [BTIC6A_CBSZ_4X];
	u16 rfr_8x8   [BTIC6A_CBSZ_8X];
	u16 rfr_16x16 [BTIC6A_CBSZ_16X];
	byte flg_2x2  [BTIC6A_CBSZ_2X];
	byte flg_4x4  [BTIC6A_CBSZ_4X];
	byte flg_8x8  [BTIC6A_CBSZ_8X];
	byte flg_16x16[BTIC6A_CBSZ_8X];
	
	u16 stf_2x2  [128];
	u16 stf_4x4  [128];
	u16 stf_8x8  [128];
	u16 stf_16x16[128];
	byte strov_2x2;
	byte strov_4x4;
	byte strov_8x8;
	byte strov_16x16;

//	byte acc_zero_l3;
//	byte acc_zero_l2;
//	byte acc_zero_l1;
//	byte acc_zero_l0;

//	byte acc_dcom_l3;
//	byte acc_dcom_l2;
//	byte acc_dcom_l1;
//	byte acc_dcom_l0;

//	byte acc_skip_l0;

	byte ld_flat;
	byte ld_2x2;
	byte ld_skip;
	byte ld_joint;

	int ld_flat_sq;
	int ld_skip_sq;

	int bytes_cb_2x2_6b;
	int bytes_cb_2x2_4b;
	int bytes_cb_2x2_5b;
	int bytes_cb_2x2_3b;

	int bytes_cb_2x2_zero;
	int bytes_cb_2x2_idx;
	int bytes_cb_2x2_idx2;
	int bytes_cb_2x2_tag;

	int bytes_cb_4x4_zero;
	int bytes_cb_4x4_idx;
	int bytes_cb_4x4_idx2;
	int bytes_cb_4x4_tag;

	int bytes_cb_8x8_zero;
	int bytes_cb_8x8_idx;
	int bytes_cb_8x8_idx2;
	int bytes_cb_8x8_tag;

	int bytes_cb_16x16_zero;
	int bytes_cb_16x16_idx;
	int bytes_cb_16x16_idx2;
	int bytes_cb_16x16_tag;

	byte *cimg2x;
	byte *cimg;
	byte *pimg;
	int xs, ys;
	int cxs, cys;
	int bxs, bys;
	int mv_x, mv_y;
	int c_x, c_y;
	
	int clr_y;
	int clr_u;
	int clr_v;
};


BTIC6A_EncodeContext *BTIC6A_AllocEncodeContext()
{
	BTIC6A_EncodeContext *ctx;
	ctx=malloc(sizeof(BTIC6A_EncodeContext));
	memset(ctx, 0, sizeof(BTIC6A_EncodeContext));
	return(ctx);
}

byte *BTIC5C_EncodeEmitFourCC(byte *ict, u32 fcc, void *buf, int sz)
{
	byte *ct;
	int sz1, nsz1;

	ct=ict;
	sz1=sz+8;
	nsz1=sz1^0xFFFFFFFF;
	*ct++=(fcc >> 0)&0xFF;
	*ct++=(fcc >> 8)&0xFF;
	*ct++=(fcc >>16)&0xFF;
	*ct++=(fcc >>24)&0xFF;
	*ct++=(nsz1>> 0)&0xFF;
	*ct++=(nsz1>> 8)&0xFF;
	*ct++=(nsz1>>16)&0xFF;
	*ct++=(nsz1>>24)&0xFF;
	memcpy(ct, buf, sz);
	ct+=sz;
	return(ct);	
}

byte *BTIC5C_EncodeEmitTwoCC(byte *ict, u16 tcc, void *buf, int sz)
{
	byte *ct;
	int sz1, nsz1;
	
	ct=ict;
	if((sz+8)<32768)
	{
		sz1=sz+4;
		nsz1=sz1^0xFFFF;
		*ct++=(tcc >>0)&0xFF;
		*ct++=(tcc >>8)&0xFF;
		*ct++=(nsz1>>0)&0xFF;
		*ct++=(nsz1>>8)&0xFF;
		memcpy(ct, buf, sz);
		ct+=sz;
		return(ct);
	}
	ct=BTIC5C_EncodeEmitFourCC(ict, tcc, buf, sz);
	return(ct);	
}

void BTIC6A_EncodeGetFrameMacroblockPixelsYUV(
	byte *cimg, int xs, int ys,
	byte *blk_y, byte *blk_c, int ox, int oy)
{
	byte *cs0, *cs1;
	byte *ct_y, *ct_c;
	int cr0, cr1, cr2, cr3;
	int cg0, cg1, cg2, cg3;
	int cb0, cb1, cb2, cb3;
	int cy0, cy1, cy2, cy3;
	int cu0, cu1, cu2, cu3;
	int cv0, cv1, cv2, cv3;
	int cu, cv;
	int x, y, z;
	
	ct_y=blk_y;
	ct_c=blk_c;
	for(y=0; y<8; y++)
	{
		z=(oy+y*2)*xs+(ox);
		cs0=cimg+z*4;
		cs1=cs0+xs*4;

		for(x=0; x<8; x++)
		{
			cb0=cs0[0];		cg0=cs0[1];		cr0=cs0[2];
			cb1=cs0[4];		cg1=cs0[6];		cr1=cs0[6];
			cb2=cs1[0];		cg2=cs1[1];		cr2=cs1[2];
			cb3=cs1[4];		cg3=cs1[6];		cr3=cs1[6];
			cy0=(cg0*4+cr0*3+cb0)>>3;	cy1=(cg1*4+cr1*3+cb1)>>3;
			cy2=(cg2*4+cr2*3+cb2)>>3;	cy3=(cg3*4+cr3*3+cb3)>>3;
			cu0=((cb0-cy0)/2)+128;		cu1=((cb1-cy1)/2)+128;
			cu2=((cb2-cy2)/2)+128;		cu3=((cb3-cy3)/2)+128;
			cv0=((cr0-cy0)/2)+128;		cv1=((cr1-cy1)/2)+128;
			cv2=((cr2-cy2)/2)+128;		cv3=((cr3-cy3)/2)+128;
			cu=(cu0+cu1+cu2+cu3)/4;		cv=(cv0+cv1+cv2+cv3)/4;
			ct_y[0]=cy0;	ct_y[1]=cy1;
			ct_y[2]=cy2;	ct_y[3]=cy3;
			ct_c[0]=cu;		ct_c[1]=cv;
			ct_y+=4;		ct_c+=2;
			cs0+=8;			cs1+=8;
		}
	}
}

static const byte bt6a_hilbtab_4[16]={
0x00, 0x01, 0x11, 0x10, 0x20, 0x30, 0x31, 0x21,
0x22, 0x32, 0x33, 0x23, 0x13, 0x12, 0x02, 0x03
};

static const byte bt6a_hilbtab_8[64]={
0x00, 0x10, 0x11, 0x01, 0x02, 0x03, 0x13, 0x12,
0x22, 0x23, 0x33, 0x32, 0x31, 0x21, 0x20, 0x30,
0x40, 0x41, 0x51, 0x50, 0x60, 0x70, 0x71, 0x61,
0x62, 0x72, 0x73, 0x63, 0x53, 0x52, 0x42, 0x43,
0x44, 0x45, 0x55, 0x54, 0x64, 0x74, 0x75, 0x65,
0x66, 0x76, 0x77, 0x67, 0x57, 0x56, 0x46, 0x47,
0x37, 0x27, 0x26, 0x36, 0x35, 0x34, 0x24, 0x25,
0x15, 0x14, 0x04, 0x05, 0x06, 0x16, 0x17, 0x07
};

static const byte bt6a_hilbtab_16[256]={
0x00, 0x01, 0x11, 0x10, 0x20, 0x30, 0x31, 0x21,
0x22, 0x32, 0x33, 0x23, 0x13, 0x12, 0x02, 0x03,
0x04, 0x14, 0x15, 0x05, 0x06, 0x07, 0x17, 0x16,
0x26, 0x27, 0x37, 0x36, 0x35, 0x25, 0x24, 0x34,
0x44, 0x54, 0x55, 0x45, 0x46, 0x47, 0x57, 0x56,
0x66, 0x67, 0x77, 0x76, 0x75, 0x65, 0x64, 0x74,
0x73, 0x72, 0x62, 0x63, 0x53, 0x43, 0x42, 0x52,
0x51, 0x41, 0x40, 0x50, 0x60, 0x61, 0x71, 0x70,
0x80, 0x90, 0x91, 0x81, 0x82, 0x83, 0x93, 0x92,
0xA2, 0xA3, 0xB3, 0xB2, 0xB1, 0xA1, 0xA0, 0xB0,
0xC0, 0xC1, 0xD1, 0xD0, 0xE0, 0xF0, 0xF1, 0xE1,
0xE2, 0xF2, 0xF3, 0xE3, 0xD3, 0xD2, 0xC2, 0xC3,
0xC4, 0xC5, 0xD5, 0xD4, 0xE4, 0xF4, 0xF5, 0xE5,
0xE6, 0xF6, 0xF7, 0xE7, 0xD7, 0xD6, 0xC6, 0xC7,
0xB7, 0xA7, 0xA6, 0xB6, 0xB5, 0xB4, 0xA4, 0xA5,
0x95, 0x94, 0x84, 0x85, 0x86, 0x96, 0x97, 0x87,
0x88, 0x98, 0x99, 0x89, 0x8A, 0x8B, 0x9B, 0x9A,
0xAA, 0xAB, 0xBB, 0xBA, 0xB9, 0xA9, 0xA8, 0xB8,
0xC8, 0xC9, 0xD9, 0xD8, 0xE8, 0xF8, 0xF9, 0xE9,
0xEA, 0xFA, 0xFB, 0xEB, 0xDB, 0xDA, 0xCA, 0xCB,
0xCC, 0xCD, 0xDD, 0xDC, 0xEC, 0xFC, 0xFD, 0xED,
0xEE, 0xFE, 0xFF, 0xEF, 0xDF, 0xDE, 0xCE, 0xCF,
0xBF, 0xAF, 0xAE, 0xBE, 0xBD, 0xBC, 0xAC, 0xAD,
0x9D, 0x9C, 0x8C, 0x8D, 0x8E, 0x9E, 0x9F, 0x8F,
0x7F, 0x7E, 0x6E, 0x6F, 0x5F, 0x4F, 0x4E, 0x5E,
0x5D, 0x4D, 0x4C, 0x5C, 0x6C, 0x6D, 0x7D, 0x7C,
0x7B, 0x6B, 0x6A, 0x7A, 0x79, 0x78, 0x68, 0x69,
0x59, 0x58, 0x48, 0x49, 0x4A, 0x5A, 0x5B, 0x4B,
0x3B, 0x2B, 0x2A, 0x3A, 0x39, 0x38, 0x28, 0x29,
0x19, 0x18, 0x08, 0x09, 0x0A, 0x1A, 0x1B, 0x0B,
0x0C, 0x0D, 0x1D, 0x1C, 0x2C, 0x3C, 0x3D, 0x2D,
0x2E, 0x3E, 0x3F, 0x2F, 0x1F, 0x1E, 0x0E, 0x0F
};


void BTIC6A_EncodeGetFrameMacroblockPixelsHilbertYUV(
	byte *cimg, int xs, int ys,
	byte *blk_y, byte *blk_c, int ox, int oy)
{
	byte *cs0, *cs1, *cs2, *cs3;
	byte *ct_y, *ct_c;
	int cr0, cr1, cr2, cr3;
	int cg0, cg1, cg2, cg3;
	int cb0, cb1, cb2, cb3;
	int cy0, cy1, cy2, cy3;
	int cu0, cu1, cu2, cu3;
	int cv0, cv1, cv2, cv3;
	int cu, cv;
	int x, y, z, i;
	
	ct_y=blk_y;
	ct_c=blk_c;
	for(i=0; i<64; i++)
	{
		z=bt6a_hilbtab_16[i*4+0];
		x=(z>>0)&15;	y=(z>>4)&15;
		cs0=cimg+((oy+y)*xs+(ox+x))*4;
		z=bt6a_hilbtab_16[i*4+1];
		x=(z>>0)&15;	y=(z>>4)&15;
		cs1=cimg+((oy+y)*xs+(ox+x))*4;
		z=bt6a_hilbtab_16[i*4+2];
		x=(z>>0)&15;	y=(z>>4)&15;
		cs2=cimg+((oy+y)*xs+(ox+x))*4;
		z=bt6a_hilbtab_16[i*4+3];
		x=(z>>0)&15;	y=(z>>4)&15;
		cs3=cimg+((oy+y)*xs+(ox+x))*4;

		cb0=cs0[0];		cg0=cs0[1];		cr0=cs0[2];
		cb1=cs1[0];		cg1=cs1[1];		cr1=cs1[2];
		cb2=cs2[0];		cg2=cs2[1];		cr2=cs2[2];
		cb3=cs3[0];		cg3=cs3[1];		cr3=cs3[2];
		cy0=(cg0*4+cr0*3+cb0)>>3;	cy1=(cg1*4+cr1*3+cb1)>>3;
		cy2=(cg2*4+cr2*3+cb2)>>3;	cy3=(cg3*4+cr3*3+cb3)>>3;
		cu0=((cb0-cy0)/2)+128;		cu1=((cb1-cy1)/2)+128;
		cu2=((cb2-cy2)/2)+128;		cu3=((cb3-cy3)/2)+128;
		cv0=((cr0-cy0)/2)+128;		cv1=((cr1-cy1)/2)+128;
		cv2=((cr2-cy2)/2)+128;		cv3=((cr3-cy3)/2)+128;
		cu=(cu0+cu1+cu2+cu3)/4;		cv=(cv0+cv1+cv2+cv3)/4;
		ct_y[0]=cy0;	ct_y[1]=cy1;
		ct_y[2]=cy2;	ct_y[3]=cy3;
		ct_c[0]=cu;		ct_c[1]=cv;
		ct_y+=4;		ct_c+=2;
	}
}

void BTIC6A_EncodeBlockNDeltaYUV(
	byte *iblk_y, byte *iblk_c,
	byte *oblk_y, byte *oblk_c,
	int icy, int icu, int icv, int bn)
{
	int pcy, pcu, pcv, cy, cu, cv;
	int i, j, k, l;
	
	pcy=icy;
	l=1<<(bn+2);
	for(i=0; i<l; i++)
	{
		cy=iblk_y[i];
		j=(cy-pcy)&255;
		oblk_y[i]=j;
		pcy=cy;
	}

	pcu=icu;
	pcv=icv;
	l=1<<(bn+0);
	for(i=0; i<l; i++)
	{
		cu=iblk_c[i*2+0];
		cv=iblk_c[i*2+1];
		j=(cu-pcu)&255;
		k=(cv-pcv)&255;
		oblk_c[i*2+0]=j;
		oblk_c[i*2+1]=k;
		pcu=cu;
		pcv=cv;
	}
}

void BTIC6A_EncodeBlock16DeltaYUV(
	byte *iblk_y, byte *iblk_c,
	byte *oblk_y, byte *oblk_c,
	int icy, int icu, int icv)
{	BTIC6A_EncodeBlockNDeltaYUV(
		iblk_y, iblk_c, oblk_y, oblk_c, icy, icu, icv, 6);	}
void BTIC6A_EncodeBlock8DeltaYUV(
	byte *iblk_y, byte *iblk_c,
	byte *oblk_y, byte *oblk_c,
	int icy, int icu, int icv)
{	BTIC6A_EncodeBlockNDeltaYUV(
		iblk_y, iblk_c, oblk_y, oblk_c, icy, icu, icv, 4);	}
void BTIC6A_EncodeBlock4DeltaYUV(
	byte *iblk_y, byte *iblk_c,
	byte *oblk_y, byte *oblk_c,
	int icy, int icu, int icv)
{	BTIC6A_EncodeBlockNDeltaYUV(
		iblk_y, iblk_c, oblk_y, oblk_c, icy, icu, icv, 2);	}
void BTIC6A_EncodeBlock2DeltaYUV(
	byte *iblk_y, byte *iblk_c,
	byte *oblk_y, byte *oblk_c,
	int icy, int icu, int icv)
{	BTIC6A_EncodeBlockNDeltaYUV(
		iblk_y, iblk_c, oblk_y, oblk_c, icy, icu, icv, 0);	}

int bt6a_abserr(int x)
{
//	return(x*x);
//	return(abs(x));

//	return((x*x+abs(x))/2);
//	return(abs(x)+(x*x*7/8));
	return(abs(x)+(x*x));
}

int BTIC6A_EncodeCompareBlockNErrorYUV(
	byte *blk0_y, byte *blk0_c,
	byte *blk1_y, byte *blk1_c,
	int icy, int icu, int icv,
	int bn)
{
	int pcy0, pcu0, pcv0, pcy1, pcu1, pcv1;
	int cy0, cu0, cv0, cy1, cu1, cv1;
	int dcy, dcu, dcv;
	int ecy, ecu, ecv;
	int i, j, k, l;
	
	ecy=0;
	ecu=0;
	ecv=0;
	
	pcy0=icy;
	pcy1=icy;
	l=1<<(bn+2);
	for(i=0; i<l; i++)
	{
		cy0=blk0_y[i];
		cy1=blk1_y[i];
		pcy0=(pcy0+cy0)&255;
		pcy1=(pcy1+cy1)&255;
		dcy=(pcy0-pcy1);
//		ecy+=dcy*dcy;
		ecy+=bt6a_abserr(dcy);
	}

	pcu0=icu;	pcv0=icv;
	pcu1=icu;	pcv1=icv;
	l=1<<(bn+0);
	for(i=0; i<l; i++)
	{
		cu0=blk0_c[i*2+0];		cv0=blk0_c[i*2+1];
		cu1=blk1_c[i*2+0];		cv1=blk1_c[i*2+1];
		pcu0=(pcu0+cu0)&255;	pcu1=(pcu1+cu1)&255;
		pcv0=(pcv0+cv0)&255;	pcv1=(pcv1+cv1)&255;
		dcu=(pcu0-pcu1);		dcv=(pcv0-pcv1);
//		ecu+=dcu*dcu;			ecv+=dcv*dcv;
		ecu+=bt6a_abserr(dcu);	ecv+=bt6a_abserr(dcv);
	}
	ecu*=4;
	ecv*=4;
	return(ecy+ecu+ecv);
}

void BTIC6A_EncodeCalcBlockNStepYUV(
	byte *blk_y, byte *blk_c,
	int icy, int icu, int icv,
	int *rocy, int *rocu, int *rocv, int bn)
{
	int pcy, pcu, pcv;
	int cy, cu, cv;
	int i, j, k, l;
	
	pcy=icy;	pcu=icu;	pcv=icv;
	l=1<<(bn+2);
	for(i=0; i<l; i++)
	{	cy=blk_y[i]; pcy=(pcy+cy)&255;	}
	l=1<<(bn+0);
	for(i=0; i<l; i++)
	{	cu=blk_c[i*2+0];	cv=blk_c[i*2+1];
		pcu=(pcu+cu)&255;	pcv=(pcv+cv)&255;	}
	*rocy=pcy;	*rocu=pcu;	*rocv=pcv;	
}

int BTIC6A_CheckValidVqBlock16Index(BTIC6A_EncodeContext *ctx, int idx)
{
	if(idx&(~0xFFFF))	return(0);		//Not 16b
	if(idx>=0xC000)		return(1);		//Special Always-Valid
	if(idx>=0x4000)		return(0);		//Special, Absolute
	if(((ctx->rov_16x16-idx)&0x3FFF)>=(BTIC6A_CBSZ_16X-2))
		return(0);
	return(1);
}

int BTIC6A_CheckValidVqBlock8Index(BTIC6A_EncodeContext *ctx, int idx)
{
	if(idx&(~0xFFFF))	return(0);		//Not 16b
	if(idx>=0xC000)		return(1);		//Special Always-Valid
	if(idx>=0x4000)		return(0);		//Special, Absolute
	if(((ctx->rov_8x8-idx)&0x3FFF)>=(BTIC6A_CBSZ_8X-6))
		return(0);
	return(1);
}

int BTIC6A_CheckValidVqBlock4Index(BTIC6A_EncodeContext *ctx, int idx)
{
	if(idx&(~0xFFFF))	return(0);		//Not 16b
	if(idx>=0xC000)		return(1);		//Special Always-Valid
	if(idx>=0x4000)		return(0);		//Special, Absolute
	if(((ctx->rov_4x4-idx)&0x3FFF)>=(BTIC6A_CBSZ_4X-20))
		return(0);
	return(1);
}

int BTIC6A_CheckValidVqBlock2Index(BTIC6A_EncodeContext *ctx, int idx)
{
	if(idx&(~0xFFFF))	return(0);		//Not 16b
	if(idx>=0xC000)		return(1);		//Special Always-Valid
	if(idx>=0x4000)		return(0);		//Special, Absolute
	if(((ctx->rov_2x2-idx)&0x3FFF)>=(BTIC6A_CBSZ_2X-72))
		return(0);
	return(1);
}

int BTIC6A_CheckStaleBlock2Index(BTIC6A_EncodeContext *ctx, int idx)
{
	if(idx&(~0xFFFF))	return(0);		//Not 16b
	if(idx>=0xC000)		return(0);		//Special Always-Valid
	if(idx>=0x4000)		return(0);		//Special, Absolute
	if(((ctx->rov_2x2-idx)&0x3FFF)>=((BTIC6A_CBSZ_2X*3)/4))
		return(1);
	return(0);
}

int BTIC6A_CheckFreshBlock2Index(BTIC6A_EncodeContext *ctx, int idx)
{
	if(idx&(~0xFFFF))	return(0);		//Not 16b
	if(idx>=0xC000)		return(1);		//Special Always-Valid
	if(idx>=0x4000)		return(0);		//Special, Absolute
	if(((ctx->rov_2x2-idx)&0x3FFF)<((BTIC6A_CBSZ_2X*3)/8))
		return(1);
	return(0);
}

int BTIC6A_CheckStaleBlock4Index(BTIC6A_EncodeContext *ctx, int idx)
{
	u16 *ici;
	if(idx&(~0xFFFF))	return(0);		//Not 16b
	if(idx>=0xC000)		return(0);		//Special Always-Valid
	if(idx>=0x4000)		return(0);		//Special, Absolute
	if(((ctx->rov_4x4-idx)&0x3FFF)>=((BTIC6A_CBSZ_4X*3)/4))
//	if(((ctx->rov_4x4-idx)&0x3FFF)>=((BTIC6A_CBSZ_4X*7)/16))
//	if(((ctx->rov_4x4-idx)&0x3FFF)>=((BTIC6A_CBSZ_4X*7)/8))
		return(1);
	ici=ctx->cb_4x4+((idx&BTIC6A_CBMSK_4X)*4);
	if(	BTIC6A_CheckStaleBlock2Index(ctx, ici[0]) ||
		BTIC6A_CheckStaleBlock2Index(ctx, ici[1]) ||
		BTIC6A_CheckStaleBlock2Index(ctx, ici[2]) ||
		BTIC6A_CheckStaleBlock2Index(ctx, ici[3]) )
			return(1);
	return(0);
}

int BTIC6A_CheckFreshBlock4Index(BTIC6A_EncodeContext *ctx, int idx)
{
	u16 *ici;
	if(idx&(~0xFFFF))	return(0);		//Not 16b
	if(idx>=0xC000)		return(1);		//Special Always-Valid
	if(idx>=0x4000)		return(0);		//Special, Absolute
	if(((ctx->rov_4x4-idx)&0x3FFF)>=((BTIC6A_CBSZ_4X*3)/8))
		return(0);
	ici=ctx->cb_4x4+((idx&BTIC6A_CBMSK_4X)*4);
	if(	BTIC6A_CheckFreshBlock2Index(ctx, ici[0]) &&
		BTIC6A_CheckFreshBlock2Index(ctx, ici[1]) &&
		BTIC6A_CheckFreshBlock2Index(ctx, ici[2]) &&
		BTIC6A_CheckFreshBlock2Index(ctx, ici[3]) )
			return(1);
	return(0);
}

int BTIC6A_CheckStaleBlock8Index(BTIC6A_EncodeContext *ctx, int idx)
{
	u16 *ici;
	if(idx&(~0xFFFF))	return(0);		//Not 16b
	if(idx>=0xC000)		return(0);		//Special Always-Valid
	if(idx>=0x4000)		return(0);		//Special, Absolute
	if(((ctx->rov_8x8-idx)&0x3FFF)>=((BTIC6A_CBSZ_8X*3)/4))
		return(1);
	ici=ctx->cb_8x8+((idx&BTIC6A_CBMSK_8X)*4);
	if(	BTIC6A_CheckStaleBlock4Index(ctx, ici[0]) ||
		BTIC6A_CheckStaleBlock4Index(ctx, ici[1]) ||
		BTIC6A_CheckStaleBlock4Index(ctx, ici[2]) ||
		BTIC6A_CheckStaleBlock4Index(ctx, ici[3]) )
			return(1);
	return(0);
}

int BTIC6A_CheckFreshBlock8Index(BTIC6A_EncodeContext *ctx, int idx)
{
	u16 *ici;
	if(idx&(~0xFFFF))	return(0);		//Not 16b
	if(idx>=0xC000)		return(1);		//Special Always-Valid
	if(idx>=0x4000)		return(0);		//Special, Absolute
	if(((ctx->rov_8x8-idx)&0x3FFF)>=((BTIC6A_CBSZ_8X*3)/8))
		return(0);
	ici=ctx->cb_8x8+((idx&BTIC6A_CBMSK_8X)*4);
	if(	BTIC6A_CheckFreshBlock4Index(ctx, ici[0]) &&
		BTIC6A_CheckFreshBlock4Index(ctx, ici[1]) &&
		BTIC6A_CheckFreshBlock4Index(ctx, ici[2]) &&
		BTIC6A_CheckFreshBlock4Index(ctx, ici[3]) )
			return(1);
	return(0);
}

int BTIC6A_CheckStaleBlock16Index(BTIC6A_EncodeContext *ctx, int idx)
{
	u16 *ici;
	if(idx&(~0xFFFF))	return(0);		//Not 16b
	if(idx>=0xC000)		return(0);		//Special Always-Valid
	if(idx>=0x4000)		return(0);		//Special, Absolute
	if(((ctx->rov_16x16-idx)&0x3FFF)>=((BTIC6A_CBSZ_16X*3)/4))
		return(1);
	ici=ctx->cb_16x16+((idx&BTIC6A_CBMSK_16X)*4);
	if(	BTIC6A_CheckStaleBlock8Index(ctx, ici[0]) ||
		BTIC6A_CheckStaleBlock8Index(ctx, ici[1]) ||
		BTIC6A_CheckStaleBlock8Index(ctx, ici[2]) ||
		BTIC6A_CheckStaleBlock8Index(ctx, ici[3]) )
			return(1);
	return(0);
}

int BTIC6A_CheckFreshBlock16Index(BTIC6A_EncodeContext *ctx, int idx)
{
	u16 *ici;
	if(idx&(~0xFFFF))	return(0);		//Not 16b
	if(idx>=0xC000)		return(1);		//Special Always-Valid
	if(idx>=0x4000)		return(0);		//Special, Absolute
	if(((ctx->rov_16x16-idx)&0x3FFF)>=((BTIC6A_CBSZ_16X*3)/8))
		return(0);
	ici=ctx->cb_16x16+((idx&BTIC6A_CBMSK_16X)*4);
	if(	BTIC6A_CheckFreshBlock8Index(ctx, ici[0]) &&
		BTIC6A_CheckFreshBlock8Index(ctx, ici[1]) &&
		BTIC6A_CheckFreshBlock8Index(ctx, ici[2]) &&
		BTIC6A_CheckFreshBlock8Index(ctx, ici[3]) )
			return(1);
	return(0);
}

int BTIC6A_EncodeGetVqBlock2(BTIC6A_EncodeContext *ctx,
	int idx, byte *blk_y, byte *blk_c)
{
	byte *ici_y, *ici_c;
	int rt0, rt1, rt2, rt3;
	memset(blk_y, 0, 4);
	memset(blk_c, 0, 2);
	if(idx>=0xC000)
		{ return(0); }
	if(!BTIC6A_CheckValidVqBlock2Index(ctx, idx))
		return(-1);
	ici_y=ctx->cb_2x2_y+((idx&BTIC6A_CBMSK_2X)*4);
	ici_c=ctx->cb_2x2_c+((idx&BTIC6A_CBMSK_2X)*2);
	memcpy(blk_y, ici_y, 4);
	memcpy(blk_c, ici_c, 2);
	return(0);
}

int BTIC6A_EncodeGetVqBlock4(BTIC6A_EncodeContext *ctx,
	int idx, byte *blk_y, byte *blk_c)
{
	u16 *ici;
	int rt0, rt1, rt2, rt3;
	memset(blk_y, 0, 16);
	memset(blk_c, 0, 8);
	if(idx>=0xC000)
		{ return(0); }
	if(!BTIC6A_CheckValidVqBlock4Index(ctx, idx))
		return(-1);
	ici=ctx->cb_4x4+((idx&BTIC6A_CBMSK_4X)*4);
	rt0=BTIC6A_EncodeGetVqBlock2(ctx, ici[0], blk_y+ 0, blk_c+0);
	rt1=BTIC6A_EncodeGetVqBlock2(ctx, ici[1], blk_y+ 4, blk_c+2);
	rt2=BTIC6A_EncodeGetVqBlock2(ctx, ici[2], blk_y+ 8, blk_c+4);
	rt3=BTIC6A_EncodeGetVqBlock2(ctx, ici[3], blk_y+12, blk_c+6);
	if((rt0<0) || (rt1<0) || (rt2<0) || (rt3<0))
		return(-1);
	return(0);
}

int BTIC6A_EncodeGetVqBlock8(BTIC6A_EncodeContext *ctx,
	int idx, byte *blk_y, byte *blk_c)
{
	u16 *ici;
	int rt0, rt1, rt2, rt3;
	memset(blk_y, 0, 64);
	memset(blk_c, 0, 32);
	if(idx>=0xC000)
		{ return(0); }
	if(!BTIC6A_CheckValidVqBlock8Index(ctx, idx))
		return(-1);
	ici=ctx->cb_8x8+((idx&BTIC6A_CBMSK_8X)*4);
	rt0=BTIC6A_EncodeGetVqBlock4(ctx, ici[0], blk_y+ 0, blk_c+ 0);
	rt1=BTIC6A_EncodeGetVqBlock4(ctx, ici[1], blk_y+16, blk_c+ 8);
	rt2=BTIC6A_EncodeGetVqBlock4(ctx, ici[2], blk_y+32, blk_c+16);
	rt3=BTIC6A_EncodeGetVqBlock4(ctx, ici[3], blk_y+48, blk_c+24);
	if((rt0<0) || (rt1<0) || (rt2<0) || (rt3<0))
		return(-1);
	return(0);
}

int BTIC6A_EncodeGetVqBlock16(BTIC6A_EncodeContext *ctx,
	int idx, byte *blk_y, byte *blk_c)
{
	u16 *ici;
	int rt0, rt1, rt2, rt3;
	memset(blk_y, 0, 256);
	memset(blk_c, 0, 128);
	if(idx>=0xC000)
		{ return(0); }
	if(!BTIC6A_CheckValidVqBlock16Index(ctx, idx))
		return(-1);
	ici=ctx->cb_16x16+((idx&BTIC6A_CBMSK_16X)*4);
	rt0=BTIC6A_EncodeGetVqBlock8(ctx, ici[0], blk_y+  0, blk_c+ 0);
	rt1=BTIC6A_EncodeGetVqBlock8(ctx, ici[1], blk_y+ 64, blk_c+32);
	rt2=BTIC6A_EncodeGetVqBlock8(ctx, ici[2], blk_y+128, blk_c+64);
	rt3=BTIC6A_EncodeGetVqBlock8(ctx, ici[3], blk_y+192, blk_c+96);
	if((rt0<0) || (rt1<0) || (rt2<0) || (rt3<0))
		return(-1);
	return(0);
}

int BTIC6A_EncAddBlock2Index(BTIC6A_EncodeContext *ctx,
	byte *blk_y, byte *blk_c)
{
	byte *ici_y, *ici_c;
	int ix2;

	ix2=ctx->rov_2x2;
	ctx->rov_2x2=(ix2+1)&0x3FFF;
	ici_y=ctx->cb_2x2_y+((ix2&BTIC6A_CBMSK_2X)*4);
	ici_c=ctx->cb_2x2_c+((ix2&BTIC6A_CBMSK_2X)*2);
	ici_y[0]=blk_y[0];		ici_y[1]=blk_y[1];
	ici_y[2]=blk_y[2];		ici_y[3]=blk_y[3];
	ici_c[0]=blk_c[0];		ici_c[1]=blk_c[1];
	ctx->rfr_2x2[ix2&BTIC6A_CBMSK_2X]=0xBFFF;
	ctx->flg_2x2[ix2&BTIC6A_CBMSK_2X]=0;
	return(ix2);
}

int BTIC6A_EncRefreshBlock2Index(BTIC6A_EncodeContext *ctx, int ix2)
{
	byte *ici_y, *ici_c;
	int ix3;
	ici_y=ctx->cb_2x2_y+((ix2&BTIC6A_CBMSK_2X)*4);
	ici_c=ctx->cb_2x2_c+((ix2&BTIC6A_CBMSK_2X)*2);
	ix3=BTIC6A_EncAddBlock2Index(ctx, ici_y, ici_c);
	ctx->rfr_2x2[ix2&BTIC6A_CBMSK_2X]=ix3;
	return(ix3);
}

int BTIC6A_EncWalkFreshenBlock2Index(BTIC6A_EncodeContext *ctx, int ix2)
{
	int ix4, ix5;
	ix4=ix2;
	ix5=ctx->rfr_2x2[ix4&BTIC6A_CBMSK_2X];
	while(BTIC6A_CheckValidVqBlock2Index(ctx, ix5))
		{ ix4=ix5; ix5=ctx->rfr_2x2[ix4&BTIC6A_CBMSK_2X]; }
	return(ix4);
}

int BTIC6A_EncAddBlock4Index(BTIC6A_EncodeContext *ctx, u16 *blk_i)
{
	u16 *ici;
	int ix2;

	ix2=ctx->rov_4x4;
	ctx->rov_4x4=(ix2+1)&0x3FFF;
	ici=ctx->cb_4x4+((ix2&BTIC6A_CBMSK_4X)*4);
	ici[0]=blk_i[0];		ici[1]=blk_i[1];
	ici[2]=blk_i[2];		ici[3]=blk_i[3];
	ctx->rfr_4x4[ix2&BTIC6A_CBMSK_4X]=0xBFFF;
	ctx->flg_4x4[ix2&BTIC6A_CBMSK_4X]=0;
	return(ix2);
}

int BTIC6A_EncRefreshBlock4Index(BTIC6A_EncodeContext *ctx, int ix2)
{
	u16 *ici;
	int ix3;
	ici=ctx->cb_4x4+((ix2&BTIC6A_CBMSK_4X)*4);
	ix3=BTIC6A_EncAddBlock4Index(ctx, ici);
	ctx->rfr_4x4[ix2&BTIC6A_CBMSK_4X]=ix3;
	return(ix3);
}

int BTIC6A_EncWalkFreshenBlock4Index(BTIC6A_EncodeContext *ctx, int ix2)
{
	int ix4, ix5;
	ix4=ix2;
	ix5=ctx->rfr_4x4[ix4&BTIC6A_CBMSK_4X];
	while(BTIC6A_CheckValidVqBlock4Index(ctx, ix5))
		{ ix4=ix5; ix5=ctx->rfr_4x4[ix4&BTIC6A_CBMSK_4X]; }
	return(ix4);
}

int BTIC6A_EncAddBlock8Index(BTIC6A_EncodeContext *ctx, u16 *blk_i)
{
	u16 *ici;
	int ix2;

	ix2=ctx->rov_8x8;
	ctx->rov_8x8=(ix2+1)&0x3FFF;
	ici=ctx->cb_8x8+((ix2&BTIC6A_CBMSK_8X)*4);
	ici[0]=blk_i[0];		ici[1]=blk_i[1];
	ici[2]=blk_i[2];		ici[3]=blk_i[3];
	ctx->rfr_8x8[ix2&BTIC6A_CBMSK_8X]=0xBFFF;
	ctx->flg_8x8[ix2&BTIC6A_CBMSK_8X]=0;
	return(ix2);
}

int BTIC6A_EncRefreshBlock8Index(BTIC6A_EncodeContext *ctx, int ix2)
{
	u16 *ici;
	int ix3;
	ici=ctx->cb_8x8+((ix2&BTIC6A_CBMSK_8X)*4);
	ix3=BTIC6A_EncAddBlock8Index(ctx, ici);
	ctx->rfr_8x8[ix2&BTIC6A_CBMSK_8X]=ix3;
	return(ix3);
}

int BTIC6A_EncWalkFreshenBlock8Index(BTIC6A_EncodeContext *ctx, int ix2)
{
	int ix4, ix5;
	ix4=ix2;
	ix5=ctx->rfr_8x8[ix4&BTIC6A_CBMSK_8X];
	while(BTIC6A_CheckValidVqBlock8Index(ctx, ix5))
		{ ix4=ix5; ix5=ctx->rfr_8x8[ix4&BTIC6A_CBMSK_8X]; }
	return(ix4);
}

int BTIC6A_EncAddBlock16Index(BTIC6A_EncodeContext *ctx, u16 *blk_i)
{
	u16 *ici;
	int ix2;

	ix2=ctx->rov_16x16;
	ctx->rov_16x16=(ix2+1)&0x3FFF;
	ici=ctx->cb_16x16+((ix2&BTIC6A_CBMSK_16X)*4);
	ici[0]=blk_i[0];		ici[1]=blk_i[1];
	ici[2]=blk_i[2];		ici[3]=blk_i[3];
	ctx->rfr_16x16[ix2&BTIC6A_CBMSK_16X]=0xBFFF;
	ctx->flg_16x16[ix2&BTIC6A_CBMSK_16X]=0;
	return(ix2);
}

int BTIC6A_EncRefreshBlock16Index(BTIC6A_EncodeContext *ctx, int ix2)
{
	u16 *ici;
	int ix3;
	ici=ctx->cb_16x16+((ix2&BTIC6A_CBMSK_16X)*4);
	ix3=BTIC6A_EncAddBlock16Index(ctx, ici);
	ctx->rfr_16x16[ix2&BTIC6A_CBMSK_16X]=ix3;
	return(ix3);
}

int BTIC6A_EncWalkFreshenBlock16Index(BTIC6A_EncodeContext *ctx, int ix2)
{
	int ix4, ix5;
	ix4=ix2;
	ix5=ctx->rfr_16x16[ix4&BTIC6A_CBMSK_8X];
	while(BTIC6A_CheckValidVqBlock16Index(ctx, ix5))
		{ ix4=ix5; ix5=ctx->rfr_16x16[ix4&BTIC6A_CBMSK_16X]; }
	return(ix4);
}

int btic6a_clamp63(int v)
{
	if(v< 0)	return( 0);
	if(v>63)	return(63);
	return(v);
}

int BTIC6A_EncodeBlock2(BTIC6A_EncodeContext *ctx,
	byte *blk_y, byte *blk_c)
{
	byte blk0_y[4];
	byte blk0_c[2];
	byte blk1_y[4];
	byte blk1_c[2];
	byte *ici_y, *ici_c;
	u64 tv;
	int bi, bd, d;
	int ix0, ix1, ix2, ix3, ix4, ix5, ix6, ix7;
	int iy0, iy1, iy2, iy3, iy4, iy5, iy6, iy7;
	int cy, cu, cv, icy, icu, icv;
	int i, j, k;

	icy=ctx->clr_y;		icu=ctx->clr_u;		icv=ctx->clr_v;
	BTIC6A_EncodeBlock2DeltaYUV(blk_y, blk_c, blk0_y, blk0_c, icy, icu, icv);

	BTIC6A_EncodeGetVqBlock2(ctx, 0xFFFF, blk1_y, blk1_c);
	d=BTIC6A_EncodeCompareBlockNErrorYUV(
		blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv, 0);
	if((d/4)<ctx->ld_flat_sq)
	{
		if(	ctx->lct2 &&
//			0 &&
			(((*ctx->lct2)&0x0F)==0x01) &&
			(((*ctx->lct2)&0xF0)< 0xF0))
		{
			*ctx->lct2=(*ctx->lct2)+0x10;
		}else
		{
			ctx->lct=ctx->ct;
			ctx->lct2=ctx->ct;
			*ctx->ct++=0x01;
			ctx->bytes_cb_2x2_zero++;
		}
		BTIC6A_EncodeCalcBlockNStepYUV(
			blk1_y, blk1_c, icy, icu, icv, &cy, &cu, &cv, 0);
		ctx->clr_y=cy;		ctx->clr_u=cu;		ctx->clr_v=cv;
		return(0xFFFF);
	}

#if 1
	bi=-1; bd=3*256*256*256;
	for(i=0; i<128; i++)
	{
		ix2=ctx->stf_2x2[(ctx->strov_2x2+i)&127];
		if(ix2&0x8000)
			continue;
//		ix2=BTIC6A_EncWalkFreshenBlock2Index(ctx, ix2);
		if(BTIC6A_CheckStaleBlock2Index(ctx, ix2))
			continue;
		k=BTIC6A_EncodeGetVqBlock2(ctx, ix2, blk1_y, blk1_c);
		if(k<0)
		{
			ctx->stf_2x2[(ctx->strov_2x2+i)&127]=0xBFFC;
			continue;
		}
		d=BTIC6A_EncodeCompareBlockNErrorYUV(
			blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv, 0);
		if(d<0)
			continue;
		if(d<bd)
		{
			bi=i; bd=d;
//			if(((bd/2)<ctx->ld_skip_sq))
//				break;
		}
	}

	if((bi>=0) && ((bd/4)<ctx->ld_skip_sq))
	{
		ix0=bi;
//		ix1=(ix0*15)>>4;
		ix1=(ix0*7)>>3;
		ix0=(ix0+ctx->strov_2x2)&127;
		ix1=(ix1+ctx->strov_2x2)&127;
		ix2=ctx->stf_2x2[ix0];	ix3=ctx->stf_2x2[ix1];
		ctx->stf_2x2[ix0]=ix3;	ctx->stf_2x2[ix1]=ix2;

		BTIC6A_EncodeGetVqBlock2(ctx,
			ix2, blk1_y, blk1_c);

		ctx->bytes_cb_2x2_idx++;

		ctx->lct=ctx->ct;
		ctx->lct2=ctx->ct;

		*ctx->ct++=0x00|(bi<<1);

		BTIC6A_EncodeCalcBlockNStepYUV(
			blk1_y, blk1_c, icy, icu, icv, &cy, &cu, &cv, 0);
		ctx->clr_y=cy;		ctx->clr_u=cu;		ctx->clr_v=cv;
		return(ix2);
	}
#endif


	bi=-1; bd=3*256*256*256;
#if 1
	for(i=0; i<BTIC6A_CBSZ_2X; i++)
	{
		k=BTIC6A_EncodeGetVqBlock2(ctx,
			ctx->rov_2x2-(i+1), blk1_y, blk1_c);
		if(k<0)		continue;
		d=BTIC6A_EncodeCompareBlockNErrorYUV(
			blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv, 0);
		if(d<0)
			continue;
//		if(i>64)
//			d*=2;
		if(d<bd)
		{
			bi=i; bd=d;
			if(((bd/2)<ctx->ld_skip_sq))
				break;
		}
	}
#endif

//	if(bi>=64)
//		bi=-1;

	if((bi>=0) && ((bd/4)<ctx->ld_skip_sq))
	{
		ix2=(ctx->rov_2x2-(bi+1))&0x3FFF;
		BTIC6A_EncodeGetVqBlock2(ctx,
			ix2, blk1_y, blk1_c);

		ctx->bytes_cb_2x2_idx2+=2;

		ctx->lct=ctx->ct;
		ctx->lct2=ctx->ct;

		if(BTIC6A_CheckStaleBlock2Index(ctx, ix2))
//		if(0)
		{
			*ctx->ct++=0x09|(bi<<4);
			*ctx->ct++=(bi>>4)&255;
//			ix2=BTIC6A_EncAddBlock2Index(ctx, blk1_y, blk1_c);
			ix2=BTIC6A_EncRefreshBlock2Index(ctx, ix2);
		}else
		{
			*ctx->ct++=0x03|(bi<<4);
			*ctx->ct++=(bi>>4)&255;
		}

		ctx->strov_2x2--;
		ctx->stf_2x2[ctx->strov_2x2&127]=ix2;

		BTIC6A_EncodeCalcBlockNStepYUV(
			blk1_y, blk1_c, icy, icu, icv, &cy, &cu, &cv, 0);
		ctx->clr_y=cy;		ctx->clr_u=cu;		ctx->clr_v=cv;
		return(ix2);
	}

	ix0=blk0_y[0];	ix1=blk0_y[1];
	ix2=blk0_y[2];	ix3=blk0_y[3];
	ix6=blk0_c[0];	ix7=blk0_c[1];

#if 1
	for(j=2; j<4; j++)
	{
#if 1
		iy0=((((sbyte)ix0)>>j)+8)&255;
		iy1=((((sbyte)ix1)>>j)+8)&255;
		iy2=((((sbyte)ix2)>>j)+8)&255;
		iy3=((((sbyte)ix3)>>j)+8)&255;
		iy6=((((sbyte)ix6)>>j)+8)&255;
		iy7=((((sbyte)ix7)>>j)+8)&255;
#endif

//		if(	(ctx->lct==ctx->lct2) &&
//			(((*ctx->lct2)&0x3F)==0x37) &&
//			(((*ctx->lct2)&0xC0)<0xC0))
//				break;

		if(	(iy0<16) &&
			(iy1<16) &&
			(iy2<16) &&
			(iy3<16) &&
			(iy6<16) &&
			(iy7<16) )
		{
			blk1_y[0]=(iy0-8)<<j;		blk1_y[1]=(iy1-8)<<j;
			blk1_y[2]=(iy2-8)<<j;		blk1_y[3]=(iy3-8)<<j;
			blk1_c[0]=(iy6-8)<<j;		blk1_c[1]=(iy7-8)<<j;

			d=BTIC6A_EncodeCompareBlockNErrorYUV(
				blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv, 0);
			if((d/4)>ctx->ld_skip_sq)
				continue;

			tv=(iy0<<0)|(iy1<<4)|(iy2<<8)|(iy3<<12)|(iy6<<16)|(iy7<<20);
			tv=(tv<<8)|0x17;
			if(j==3)
				tv|=0x20;
			
			if(	(ctx->lct==ctx->lct2) &&
				(((*ctx->lct2)&0x3F)==(tv&0x3F)) &&
				(((*ctx->lct2)&0xC0)<0xC0))
			{
				*ctx->lct2=(*ctx->lct2)+0x40;
				*ctx->ct++=(tv>> 8)&255;
				*ctx->ct++=(tv>>16)&255;
				*ctx->ct++=(tv>>24)&255;
				ctx->bytes_cb_2x2_3b+=3;
			}else
			{
				ctx->lct=ctx->ct;
				ctx->lct2=ctx->ct;
				*ctx->ct++=(tv>> 0)&255;
				*ctx->ct++=(tv>> 8)&255;
				*ctx->ct++=(tv>>16)&255;
				*ctx->ct++=(tv>>24)&255;
				ctx->bytes_cb_2x2_3b+=4;
			}

			BTIC6A_EncodeCalcBlockNStepYUV(
				blk1_y, blk1_c, icy, icu, icv, &cy, &cu, &cv, 0);
			ctx->clr_y=cy;		ctx->clr_u=cu;		ctx->clr_v=cv;

			ix4=BTIC6A_EncAddBlock2Index(ctx, blk1_y, blk1_c);
			return(ix4);
		}
	}
#endif

#if 1
	for(j=0; j<4; j++)
	{
#if 1
		iy0=((((sbyte)ix0)>>j)+16)&255;
		iy1=((((sbyte)ix1)>>j)+16)&255;
		iy2=((((sbyte)ix2)>>j)+16)&255;
		iy3=((((sbyte)ix3)>>j)+16)&255;
		iy6=((((sbyte)ix6)>>j)+16)&255;
		iy7=((((sbyte)ix7)>>j)+16)&255;
#endif

		if(	(iy0<32) &&
			(iy1<32) &&
			(iy2<32) &&
			(iy3<32) &&
			(iy6<32) &&
			(iy7<32) )
		{
			blk1_y[0]=(iy0-16)<<j;		blk1_y[1]=(iy1-16)<<j;
			blk1_y[2]=(iy2-16)<<j;		blk1_y[3]=(iy3-16)<<j;
			blk1_c[0]=(iy6-16)<<j;		blk1_c[1]=(iy7-16)<<j;

			d=BTIC6A_EncodeCompareBlockNErrorYUV(
				blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv, 0);
			if((d/4)>ctx->ld_skip_sq)
				continue;

			tv=(iy0<<0)|(iy1<<5)|(iy2<<10)|(iy3<<15)|(iy6<<20)|(iy7<<25);
			tv=(tv<<2)|j;
			tv=(tv<<8)|0x27;
			
			if(	(ctx->lct==ctx->lct2) &&
				(((*ctx->lct2)&0x3F)==(tv&0x3F)) &&
				(((*ctx->lct2)&0xC0)<0xC0))
			{
				*ctx->lct2=(*ctx->lct2)+0x40;
				*ctx->ct++=(tv>> 8)&255;
				*ctx->ct++=(tv>>16)&255;
				*ctx->ct++=(tv>>24)&255;
				*ctx->ct++=(tv>>32)&255;
				ctx->bytes_cb_2x2_4b+=4;
			}else
			{
				ctx->lct=ctx->ct;
				ctx->lct2=ctx->ct;
				*ctx->ct++=(tv>> 0)&255;
				*ctx->ct++=(tv>> 8)&255;
				*ctx->ct++=(tv>>16)&255;
				*ctx->ct++=(tv>>24)&255;
				*ctx->ct++=(tv>>32)&255;
				ctx->bytes_cb_2x2_4b+=5;
			}

			BTIC6A_EncodeCalcBlockNStepYUV(
				blk1_y, blk1_c, icy, icu, icv, &cy, &cu, &cv, 0);
			ctx->clr_y=cy;		ctx->clr_u=cu;		ctx->clr_v=cv;

			ix4=BTIC6A_EncAddBlock2Index(ctx, blk1_y, blk1_c);
			return(ix4);
		}
	}

#endif

	for(j=0; j<3; j++)
	{
#if 1
		iy0=((((sbyte)ix0)>>j)+32)&255;
		iy1=((((sbyte)ix1)>>j)+32)&255;
		iy2=((((sbyte)ix2)>>j)+32)&255;
		iy3=((((sbyte)ix3)>>j)+32)&255;
		iy6=((((sbyte)ix6)>>j)+32)&255;
		iy7=((((sbyte)ix7)>>j)+32)&255;
#endif

#if 0
		iy0=btic6a_clamp63((((sbyte)ix0)>>j)+32);
		iy1=btic6a_clamp63((((sbyte)ix1)>>j)+32);
		iy2=btic6a_clamp63((((sbyte)ix2)>>j)+32);
		iy3=btic6a_clamp63((((sbyte)ix3)>>j)+32);
		iy6=btic6a_clamp63((((sbyte)ix6)>>j)+32);
		iy7=btic6a_clamp63((((sbyte)ix7)>>j)+32);
#endif

		if(	(iy0<64) &&
			(iy1<64) &&
			(iy2<64) &&
			(iy3<64) &&
			(iy6<64) &&
			(iy7<64) )
		{
			blk1_y[0]=(iy0-32)<<j;		blk1_y[1]=(iy1-32)<<j;
			blk1_y[2]=(iy2-32)<<j;		blk1_y[3]=(iy3-32)<<j;
			blk1_c[0]=(iy6-32)<<j;		blk1_c[1]=(iy7-32)<<j;

			if(j)
			{
				d=BTIC6A_EncodeCompareBlockNErrorYUV(
					blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv, 0);
				if((d/4)>ctx->ld_skip_sq)
					continue;

				tv=(iy7<<6)|(iy6);
				tv=(iy0<<0)|(iy1<<6)|(iy2<<12)|(iy3<<18)|(tv<<24);
				tv=(tv<<4)|0x05;
				if(j==2)
					tv|=0x08;
			}else
			{
				tv=(iy7<<6)|(iy6);
				tv=(iy0<<0)|(iy1<<6)|(iy2<<12)|(iy3<<18)|(tv<<24);
				tv=(tv<<4)|0x0B;
			}
			
			ctx->lct=ctx->ct;
			ctx->lct2=ctx->ct;
			*ctx->ct++=(tv>> 0)&255;
			*ctx->ct++=(tv>> 8)&255;
			*ctx->ct++=(tv>>16)&255;
			*ctx->ct++=(tv>>24)&255;
			*ctx->ct++=(tv>>32)&255;

			ctx->bytes_cb_2x2_5b+=5;

			BTIC6A_EncodeCalcBlockNStepYUV(
				blk1_y, blk1_c, icy, icu, icv, &cy, &cu, &cv, 0);
			ctx->clr_y=cy;		ctx->clr_u=cu;		ctx->clr_v=cv;

			ix4=BTIC6A_EncAddBlock2Index(ctx, blk1_y, blk1_c);
			return(ix4);
		}
	}

	if(	(ctx->lct2==ctx->lct) &&
//			0 &&
		(((*ctx->lct)&0x3F)==0x07) &&
		(((*ctx->lct)&0xC0)< 0xC0))
	{
		*ctx->lct=(*ctx->lct)+0x40;
	}else
	{
		ctx->lct=ctx->ct;
		ctx->lct2=ctx->ct;
		*ctx->ct++=0x07;
		ctx->bytes_cb_2x2_tag++;
	}
	*ctx->ct++=ix0;
	*ctx->ct++=ix1;
	*ctx->ct++=ix2;
	*ctx->ct++=ix3;
	*ctx->ct++=ix6;
	*ctx->ct++=ix7;
	ctx->bytes_cb_2x2_6b+=6;

	BTIC6A_EncodeCalcBlockNStepYUV(
		blk0_y, blk0_c, icy, icu, icv, &cy, &cu, &cv, 0);
	ctx->clr_y=cy;		ctx->clr_u=cu;		ctx->clr_v=cv;

	ix4=BTIC6A_EncAddBlock2Index(ctx, blk0_y, blk0_c);
	return(ix4);
}

int BTIC6A_EncodeBlock4(BTIC6A_EncodeContext *ctx,
	byte *blk_y, byte *blk_c)
{
	byte blk0_y[16];
	byte blk0_c[ 8];
	byte blk1_y[16];
	byte blk1_c[ 8];
	u16 ictab[4];
	u16 *ici, *icj;
	int bi, bd, d;
	int ix0, ix1, ix2, ix3, ix4;
	int cy, cu, cv, icy, icu, icv;
	int i, j, k;

	icy=ctx->clr_y;		icu=ctx->clr_u;		icv=ctx->clr_v;
	BTIC6A_EncodeBlock4DeltaYUV(blk_y, blk_c, blk0_y, blk0_c, icy, icu, icv);

	BTIC6A_EncodeGetVqBlock4(ctx, 0xFFFF, blk1_y, blk1_c);
	d=BTIC6A_EncodeCompareBlockNErrorYUV(
		blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv, 2);
	if((d/16)<ctx->ld_flat_sq)
	{
		if(	ctx->lct4 &&
//			0 &&
			(((*ctx->lct4)&0x0F)==0x01) &&
			(((*ctx->lct4)&0xF0)< 0xF0))
		{
			*ctx->lct4=(*ctx->lct4)+0x10;
		}else
		{
			ctx->lct=ctx->ct;
			ctx->lct4=ctx->ct;
			*ctx->ct++=0x01;
			ctx->bytes_cb_4x4_zero++;
		}
		BTIC6A_EncodeCalcBlockNStepYUV(
			blk1_y, blk1_c, icy, icu, icv, &cy, &cu, &cv, 2);
		ctx->clr_y=cy;		ctx->clr_u=cu;		ctx->clr_v=cv;
		return(0xFFFF);
	}

#if 1
	bi=-1; bd=3*256*256*256;
	for(i=0; i<128; i++)
	{
		ix2=ctx->stf_4x4[(ctx->strov_4x4+i)&127];
		if(ix2&0x8000)
			continue;
//		ix2=BTIC6A_EncWalkFreshenBlock4Index(ctx, ix2);
		if(BTIC6A_CheckStaleBlock4Index(ctx, ix2))
			continue;
		k=BTIC6A_EncodeGetVqBlock4(ctx, ix2, blk1_y, blk1_c);
		if(k<0)
		{
			ctx->stf_4x4[(ctx->strov_4x4+i)&127]=0xBFFC;
			continue;
		}
		d=BTIC6A_EncodeCompareBlockNErrorYUV(
			blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv, 2);
		if(d<0)
			continue;
		if(d<bd)
		{
			bi=i; bd=d;
//			if(((bd/8)<ctx->ld_skip_sq))
//				break;
		}
	}

	if((bi>=0) && ((bd/16)<ctx->ld_skip_sq))
	{
		ix0=bi;
//		ix1=(ix0*15)>>4;
		ix1=(ix0*7)>>3;
		ix0=(ix0+ctx->strov_4x4)&127;
		ix1=(ix1+ctx->strov_4x4)&127;
		ix2=ctx->stf_4x4[ix0];	ix3=ctx->stf_4x4[ix1];
		ctx->stf_4x4[ix0]=ix3;	ctx->stf_4x4[ix1]=ix2;

		BTIC6A_EncodeGetVqBlock4(ctx, ix2, blk1_y, blk1_c);

		ctx->bytes_cb_4x4_idx++;

		ctx->lct=ctx->ct;
		ctx->lct4=ctx->ct;

		*ctx->ct++=0x00|(bi<<1);

		BTIC6A_EncodeCalcBlockNStepYUV(
			blk1_y, blk1_c, icy, icu, icv, &cy, &cu, &cv, 2);
		ctx->clr_y=cy;		ctx->clr_u=cu;		ctx->clr_v=cv;
		return(ix2);
	}
#endif

	bi=-1; bd=3*256*256*256;

#if 1
	for(i=0; i<BTIC6A_CBSZ_4X; i++)
	{
		ix2=ctx->rov_4x4-(i+1);
		if(ctx->flg_4x4[ix2&BTIC6A_CBMSK_4X]&1)
			continue;
		k=BTIC6A_EncodeGetVqBlock4(ctx, ix2, blk1_y, blk1_c);
		if(k<0)
			{ ctx->flg_4x4[ix2&BTIC6A_CBMSK_4X]|=1; continue; }
		d=BTIC6A_EncodeCompareBlockNErrorYUV(
			blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv, 2);
//		if(i>64)
//			d*=1.1;
//		if(i<64)
//			d*=0.875;
		if(d<bd)
		{
			bi=i; bd=d;
			if(((bd/8)<ctx->ld_skip_sq))
				break;
		}
	}
#endif

	if((bi>=0) && ((bd/16)<ctx->ld_skip_sq))
	{
		ix2=ctx->rov_4x4-(bi+1);
		if(BTIC6A_CheckStaleBlock4Index(ctx, ix2))
		{
			icj=ctx->cb_4x4+((ix2&BTIC6A_CBMSK_4X)*4);
			if(		BTIC6A_CheckStaleBlock2Index(ctx, icj[0]) ||
					BTIC6A_CheckStaleBlock2Index(ctx, icj[1]) ||
					BTIC6A_CheckStaleBlock2Index(ctx, icj[2]) ||
					BTIC6A_CheckStaleBlock2Index(ctx, icj[3]) )
			{
				/* Force decompose to un-stale components */
				bi=-1;
			}
		}
	}

	if((bi>=0) && ((bd/16)<ctx->ld_skip_sq))
	{
		ix2=ctx->rov_4x4-(bi+1);
	
		BTIC6A_EncodeGetVqBlock4(ctx, ix2, blk1_y, blk1_c);

//		ctx->bytes_cb_4x4_idx++;
//		if(bi>=64)
//		if(bi>=128)
//			ctx->bytes_cb_4x4_idx2++;
		ctx->bytes_cb_4x4_idx2+=2;

		ctx->lct=ctx->ct;
		ctx->lct4=ctx->ct;

		if(BTIC6A_CheckStaleBlock4Index(ctx, ix2))
//		if(0)
		{
			*ctx->ct++=0x09|(bi<<4);
			*ctx->ct++=(bi>>4)&255;
			ix2=BTIC6A_EncRefreshBlock4Index(ctx, ix2);
		}else
		{
			*ctx->ct++=0x03|(bi<<4);
			*ctx->ct++=(bi>>4)&255;
		}

		ctx->strov_4x4--;
		ctx->stf_4x4[ctx->strov_4x4&127]=ix2;

		BTIC6A_EncodeCalcBlockNStepYUV(
			blk1_y, blk1_c, icy, icu, icv, &cy, &cu, &cv, 2);
		ctx->clr_y=cy;		ctx->clr_u=cu;		ctx->clr_v=cv;
		return(ctx->rov_4x4-(bi+1));
	}

	if(	ctx->lct4 &&
		(((*ctx->lct4)&0x1F)==0x07) &&
		(((*ctx->lct4)&0xE0)< 0xE0) )
	{
		*ctx->lct4=(*ctx->lct4)+0x20;
	}else
#if 0
		if(	(ctx->lct==ctx->lct8) &&
			((*ctx->lct8)==0x07))
	{
		*ctx->lct8=(*ctx->lct8)|0x10;
	}else
		if(	(ctx->lct==ctx->lct8) &&
			(((*ctx->lct8)&0x1F)==0x17) &&
			(((*ctx->lct8)&0xE0)< 0x60))
	{
		*ctx->lct8=(*ctx->lct8)+0x20;
	}else
#endif
	{
		ctx->lct=ctx->ct;
		ctx->lct4=ctx->ct;
		*ctx->ct++=0x07;
		ctx->bytes_cb_4x4_tag++;
	}

	ix0=BTIC6A_EncodeBlock2(ctx, blk_y+ 0, blk_c+0);
	ix1=BTIC6A_EncodeBlock2(ctx, blk_y+ 4, blk_c+2);
	ix2=BTIC6A_EncodeBlock2(ctx, blk_y+ 8, blk_c+4);
	ix3=BTIC6A_EncodeBlock2(ctx, blk_y+12, blk_c+6);
	ictab[0]=ix0;	ictab[1]=ix1;
	ictab[2]=ix2;	ictab[3]=ix3;
	ix4=BTIC6A_EncAddBlock4Index(ctx, ictab);
	return(ix4);
}

int BTIC6A_EncodeBlock8(BTIC6A_EncodeContext *ctx,
	byte *blk_y, byte *blk_c)
{
	byte blk0_y[64];
	byte blk0_c[32];
	byte blk1_y[64];
	byte blk1_c[32];
	u16 tidxi[4], tidxj[4];
	u16 ictab[4];
	u16 *ici, *icj;
	int bi, bd, d, bj, bi_dorfr, ix_dorfr;
	int ix0, ix1, ix2, ix3, ix4;
	int cy, cu, cv, icy, icu, icv;
	int i, j, k;

	icy=ctx->clr_y;		icu=ctx->clr_u;		icv=ctx->clr_v;
	BTIC6A_EncodeBlock8DeltaYUV(blk_y, blk_c, blk0_y, blk0_c, icy, icu, icv);

	BTIC6A_EncodeGetVqBlock8(ctx, 0xFFFF, blk1_y, blk1_c);
	d=BTIC6A_EncodeCompareBlockNErrorYUV(
		blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv, 4);
//	if((d/64)<ctx->ld_flat_sq)
//	if((d/64)<(ctx->ld_flat_sq*2))
	if((d/64)<(ctx->ld_flat_sq*1.414))
	{
		if(	ctx->lct8 &&
//			0 &&
			(((*ctx->lct8)&0x0F)==0x01) &&
			(((*ctx->lct8)&0xF0)< 0xF0))
		{
			*ctx->lct8=(*ctx->lct8)+0x10;
		}else
		{
			ctx->lct=ctx->ct;
			ctx->lct8=ctx->ct;
			*ctx->ct++=0x01;
			ctx->bytes_cb_8x8_zero++;
		}
		BTIC6A_EncodeCalcBlockNStepYUV(
			blk1_y, blk1_c, icy, icu, icv, &cy, &cu, &cv, 4);
		ctx->clr_y=cy;		ctx->clr_u=cu;		ctx->clr_v=cv;
		return(0xFFFF);
	}

#if 1
	bi=-1; bd=3*256*256*256;
	for(i=0; i<128; i++)
	{
		ix2=ctx->stf_8x8[(ctx->strov_8x8+i)&127];
		if(ix2&0x8000)
			continue;
		if(BTIC6A_CheckStaleBlock8Index(ctx, ix2))
			continue;
		k=BTIC6A_EncodeGetVqBlock8(ctx, ix2, blk1_y, blk1_c);
		if(k<0)
		{
			ctx->stf_8x8[(ctx->strov_8x8+i)&127]=0xBFFC;
			continue;
		}
		d=BTIC6A_EncodeCompareBlockNErrorYUV(
			blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv, 4);
		if(d<0)
			continue;
		if(d<bd)
		{
			bi=i; bd=d;
			if(((bd/32)<ctx->ld_skip_sq))
				break;
		}
	}

	if((bi>=0) && ((bd/64)<(ctx->ld_skip_sq*1.414)))
	{
		ix0=bi;
//		ix1=(ix0*15)>>4;
		ix1=(ix0*7)>>3;
		ix0=(ix0+ctx->strov_8x8)&127;
		ix1=(ix1+ctx->strov_8x8)&127;
		ix2=ctx->stf_8x8[ix0];	ix3=ctx->stf_8x8[ix1];
		ctx->stf_8x8[ix0]=ix3;	ctx->stf_8x8[ix1]=ix2;

		BTIC6A_EncodeGetVqBlock8(ctx, ix2, blk1_y, blk1_c);

		ctx->bytes_cb_8x8_idx++;

		ctx->lct=ctx->ct;
		ctx->lct8=ctx->ct;

		*ctx->ct++=0x00|(bi<<1);

		BTIC6A_EncodeCalcBlockNStepYUV(
			blk1_y, blk1_c, icy, icu, icv, &cy, &cu, &cv, 4);
		ctx->clr_y=cy;		ctx->clr_u=cu;		ctx->clr_v=cv;
		return(ix2);
	}
#endif

	bi=-1; bd=3*256*256*256;
#if 1
	for(i=0; i<BTIC6A_CBSZ_8X; i++)
	{
		ix2=ctx->rov_8x8-(i+1);
		if(ctx->flg_8x8[ix2&BTIC6A_CBMSK_8X]&1)
			continue;
		k=BTIC6A_EncodeGetVqBlock8(ctx, ix2, blk1_y, blk1_c);
		if(k<0)
			{ ctx->flg_8x8[ix2&BTIC6A_CBMSK_8X]|=1; continue; }
		d=BTIC6A_EncodeCompareBlockNErrorYUV(
			blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv, 4);
		if(i>64)
			d*=2;
		if(d<bd)
		{
			bi=i; bd=d;
			if(((bd/32)<ctx->ld_skip_sq))
				break;
		}
	}
#endif

	ix_dorfr=-1;
	bi_dorfr=0;
	if((bi>=0) && ((bd/64)<(ctx->ld_skip_sq*1.414)))
	{
		ix2=ctx->rov_8x8-(bi+1);
		if(BTIC6A_CheckStaleBlock8Index(ctx, ix2))
		{
			ix_dorfr=ix2;
			ici=ctx->cb_8x8+((ix2&BTIC6A_CBMSK_8X)*4);
			if(		BTIC6A_CheckStaleBlock4Index(ctx, ici[0]) ||
					BTIC6A_CheckStaleBlock4Index(ctx, ici[1]) ||
					BTIC6A_CheckStaleBlock4Index(ctx, ici[2]) ||
					BTIC6A_CheckStaleBlock4Index(ctx, ici[3]) )
			{
				/* Force decompose to un-stale components */
				bi=-1;
			}
			if(		BTIC6A_CheckFreshBlock4Index(ctx, ici[0]) &&
					BTIC6A_CheckFreshBlock4Index(ctx, ici[1]) &&
					BTIC6A_CheckFreshBlock4Index(ctx, ici[2]) &&
					BTIC6A_CheckFreshBlock4Index(ctx, ici[3]) )
			{
				bi_dorfr=1;
			}
		}
	}

//	if((bi>=0) && ((bd/64)<ctx->ld_skip_sq))
//	if((bi>=0) && ((bd/64)<(ctx->ld_skip_sq*2)))
	if((bi>=0) && ((bd/64)<(ctx->ld_skip_sq*1.414)))
	{
		ix2=ctx->rov_8x8-(bi+1);
		BTIC6A_EncodeGetVqBlock8(ctx, ix2, blk1_y, blk1_c);

//		ctx->bytes_cb_8x8_idx++;
//		if(bi>=128)
//			ctx->bytes_cb_8x8_idx2++;

		ctx->bytes_cb_8x8_idx2+=2;

		ctx->lct=ctx->ct;
		ctx->lct8=ctx->ct;

		if(bi_dorfr)
//		if(0)
		{
			*ctx->ct++=0x09|(bi<<4);
			*ctx->ct++=(bi>>4)&255;
			ix2=BTIC6A_EncRefreshBlock8Index(ctx, ix2);
		}else
		{
			*ctx->ct++=0x03|(bi<<4);
			*ctx->ct++=(bi>>4)&255;
		}

		ctx->strov_8x8--;
		ctx->stf_8x8[ctx->strov_8x8&127]=ix2;

		BTIC6A_EncodeCalcBlockNStepYUV(
			blk1_y, blk1_c, icy, icu, icv, &cy, &cu, &cv, 4);
		ctx->clr_y=cy;		ctx->clr_u=cu;		ctx->clr_v=cv;
		return(ix2);
	}
	
#if 0
	if(ix_dorfr>=0)
	{
		ix2=ix_dorfr;
		ici=ctx->cb_8x8+((ix2&BTIC6A_CBMSK_8X)*4);
		k=0;
		for(i=0; i<4; i++)
		{
			ix3=ici[i];
			ix3=BTIC6A_EncWalkFreshenBlock2Index(ctx, ix3);
			if(BTIC6A_CheckStaleBlock4Index(ctx, ix3))
				k|=1<<i;
		}
		
		if(k==15)
		{
			*ctx->ct++=0x77;
			ctx->bytes_cb_8x8_tag++;
			for(i=0; i<4; i++)
			{
				ix3=ici[i];
				icj=ctx->cb_8x8+((ix3&BTIC6A_CBMSK_8X)*4);
				for(j=0; j<4; j++)
				{
					ix4=icj[j];
					ix4=BTIC6A_EncWalkFreshenBlock2Index(ctx, ix4);
					bj=(ctx->rov_2x2-ix4)&0x3FFF;
					if(!BTIC6A_CheckStaleBlock2Index(ctx, ix4))
					{
						*ctx->ct++=0x09|(bj<<4);
						*ctx->ct++=(bj>>4)&255;
						ix4=BTIC6A_EncRefreshBlock2Index(ctx, ix4);
						tidxj[j]=ix4;
					}else
					{
						*ctx->ct++=0x03|(bj<<4);
						*ctx->ct++=(bj>>4)&255;
						tidxj[j]=ix4;
					}
				}
				tidxi[i]=BTIC6A_EncAddBlock4Index(ctx, tidxj);
			}
			ix4=BTIC6A_EncAddBlock8Index(ctx, tidxi);
			return(ix4);
		}
	}
#endif

	if(	ctx->lct8 &&
		(((*ctx->lct8)&0x1F)==0x07) &&
		(((*ctx->lct8)&0xE0)< 0xE0) )
	{
		*ctx->lct8=(*ctx->lct8)+0x20;
	}else
	{
		ctx->lct=ctx->ct;
		ctx->lct8=ctx->ct;
		*ctx->ct++=0x07;
		ctx->bytes_cb_8x8_tag++;
	}

	ix0=BTIC6A_EncodeBlock4(ctx, blk_y+ 0, blk_c+ 0);
	ix1=BTIC6A_EncodeBlock4(ctx, blk_y+16, blk_c+ 8);
	ix2=BTIC6A_EncodeBlock4(ctx, blk_y+32, blk_c+16);
	ix3=BTIC6A_EncodeBlock4(ctx, blk_y+48, blk_c+24);
	ictab[0]=ix0;	ictab[1]=ix1;
	ictab[2]=ix2;	ictab[3]=ix3;
	ix4=BTIC6A_EncAddBlock8Index(ctx, ictab);
	return(ix4);
}

int BTIC6A_EncodeBlock16(BTIC6A_EncodeContext *ctx,
	byte *blk_y, byte *blk_c)
{
	byte blk0_y[256];
	byte blk0_c[128];
	byte blk1_y[256];
	byte blk1_c[128];
	u16 ictab[4];
	u16 *ici;
	int bi, bd, d;
	int ix0, ix1, ix2, ix3, ix4;
	int cy, cu, cv, icy, icu, icv;
	int i, j, k;

	icy=ctx->clr_y;		icu=ctx->clr_u;		icv=ctx->clr_v;
	BTIC6A_EncodeBlock16DeltaYUV(blk_y, blk_c, blk0_y, blk0_c, icy, icu, icv);

	BTIC6A_EncodeGetVqBlock16(ctx, 0xFFFF, blk1_y, blk1_c);
	d=BTIC6A_EncodeCompareBlockNErrorYUV(
		blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv, 6);
//	if((d/256)<ctx->ld_flat_sq)
	if((d/256)<(ctx->ld_flat_sq*2))
	{
		if(	ctx->lct16 &&
//			0 &&
			(((*ctx->lct16)&0x0F)==0x01) &&
			(((*ctx->lct16)&0xF0)< 0xF0))
		{
			*ctx->lct16=(*ctx->lct16)+0x10;
		}else
		{
			ctx->lct=ctx->ct;
			ctx->lct16=ctx->ct;
			*ctx->ct++=0x01;
			ctx->bytes_cb_16x16_zero++;
		}
		BTIC6A_EncodeCalcBlockNStepYUV(
			blk1_y, blk1_c, icy, icu, icv, &cy, &cu, &cv, 6);
		ctx->clr_y=cy;		ctx->clr_u=cu;		ctx->clr_v=cv;
		return(0xFFFF);
	}

#if 1
	bi=-1; bd=3*256*256*256;
	for(i=0; i<128; i++)
	{
		ix2=ctx->stf_16x16[(ctx->strov_16x16+i)&127];
		if(ix2&0x8000)		continue;
//		if(BTIC6A_CheckStaleBlock16Index(ctx, ix2))
//			continue;
		k=BTIC6A_EncodeGetVqBlock16(ctx, ix2, blk1_y, blk1_c);
		if(k<0)
		{	ctx->stf_16x16[(ctx->strov_16x16+i)&127]=0xBFFC;
			continue;		}
		d=BTIC6A_EncodeCompareBlockNErrorYUV(
			blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv, 6);
		if(d<0)
			continue;
		if(d<bd)
		{
			bi=i; bd=d;
			if(((bd/128)<ctx->ld_skip_sq))
				break;
		}
	}

	if((bi>=0) && ((bd/256)<(ctx->ld_skip_sq*2)))
	{
		ix0=bi;
//		ix1=(ix0*15)>>4;
		ix1=(ix0*7)>>3;
		ix0=(ix0+ctx->strov_16x16)&127;
		ix1=(ix1+ctx->strov_16x16)&127;
		ix2=ctx->stf_16x16[ix0];	ix3=ctx->stf_16x16[ix1];
		ctx->stf_16x16[ix0]=ix3;	ctx->stf_16x16[ix1]=ix2;

		BTIC6A_EncodeGetVqBlock16(ctx, ix2, blk1_y, blk1_c);

		ctx->bytes_cb_16x16_idx++;

		ctx->lct=ctx->ct;
		ctx->lct16=ctx->ct;

		*ctx->ct++=0x00|(bi<<1);

		BTIC6A_EncodeCalcBlockNStepYUV(
			blk1_y, blk1_c, icy, icu, icv, &cy, &cu, &cv, 6);
		ctx->clr_y=cy;		ctx->clr_u=cu;		ctx->clr_v=cv;
		return(ix2);
	}
#endif

	bi=-1; bd=3*256*256*256;
#if 1
	for(i=0; i<BTIC6A_CBSZ_16X; i++)
	{
		ix2=ctx->rov_16x16-(i+1);
		if(ctx->flg_16x16[ix2&BTIC6A_CBMSK_16X]&1)
			continue;
		k=BTIC6A_EncodeGetVqBlock16(ctx, ix2, blk1_y, blk1_c);
		if(k<0)
			{ ctx->flg_16x16[ix2&BTIC6A_CBMSK_16X]|=1; continue; }
		d=BTIC6A_EncodeCompareBlockNErrorYUV(
			blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv, 6);
		if(i>64)
			d*=2;
		if(d<bd)
		{
			bi=i; bd=d;
			if(((bd/128)<ctx->ld_skip_sq))
				break;
		}
	}
#endif

#if 1
	if((bi>=0) && ((bd/256)<(ctx->ld_skip_sq*2)))
	{
		ix2=ctx->rov_16x16-(bi+1);
		if(BTIC6A_CheckStaleBlock16Index(ctx, ix2))
		{
			ici=ctx->cb_16x16+((ix2&BTIC6A_CBMSK_16X)*4);
			if(		BTIC6A_CheckStaleBlock8Index(ctx, ici[0]) ||
					BTIC6A_CheckStaleBlock8Index(ctx, ici[1]) ||
					BTIC6A_CheckStaleBlock8Index(ctx, ici[2]) ||
					BTIC6A_CheckStaleBlock8Index(ctx, ici[3]) )
			{
				/* Force decompose to un-stale components */
				bi=-1;
			}
		}
	}
#endif

//	if((bi>=0) && ((bd/256)<ctx->ld_skip_sq))
	if((bi>=0) && ((bd/256)<(ctx->ld_skip_sq*2)))
	{
		ix2=ctx->rov_16x16-(bi+1);

		BTIC6A_EncodeGetVqBlock16(ctx, ix2, blk1_y, blk1_c);

		ctx->bytes_cb_16x16_idx2+=2;

		ctx->lct=ctx->ct;
		ctx->lct16=ctx->ct;

		*ctx->ct++=0x03|(bi<<4);
		*ctx->ct++=(bi>>4)&255;

		ctx->strov_16x16--;
		ctx->stf_16x16[ctx->strov_16x16&127]=ix2;

		BTIC6A_EncodeCalcBlockNStepYUV(
			blk1_y, blk1_c, icy, icu, icv, &cy, &cu, &cv, 6);
		ctx->clr_y=cy;		ctx->clr_u=cu;		ctx->clr_v=cv;
		return(ix2);
	}

	if(	ctx->lct16 &&
		(((*ctx->lct16)&0x1F)==0x07) &&
		(((*ctx->lct16)&0xE0)< 0xE0) )
	{
		*ctx->lct16=(*ctx->lct16)+0x20;
	}else
	{
		ctx->lct=ctx->ct;
		ctx->lct16=ctx->ct;
		*ctx->ct++=0x07;
		ctx->bytes_cb_16x16_tag++;
	}
	ix0=BTIC6A_EncodeBlock8(ctx, blk_y+  0, blk_c+ 0);
	ix1=BTIC6A_EncodeBlock8(ctx, blk_y+ 64, blk_c+32);
	ix2=BTIC6A_EncodeBlock8(ctx, blk_y+128, blk_c+64);
	ix3=BTIC6A_EncodeBlock8(ctx, blk_y+192, blk_c+96);
	ictab[0]=ix0;	ictab[1]=ix1;
	ictab[2]=ix2;	ictab[3]=ix3;
	ix4=BTIC6A_EncAddBlock16Index(ctx, ictab);
	return(ix4);
}

int BTIC6A_EncodeFrameMacroblock(BTIC6A_EncodeContext *ctx)
{
	byte blk0_y[256];
	byte blk0_c[128];
//	byte blk1_y[256];
//	byte blk1_c[128];

	BTIC6A_EncodeGetFrameMacroblockPixelsHilbertYUV(
		ctx->cimg, ctx->bxs, ctx->bys,
		blk0_y, blk0_c,
		ctx->c_x*16, ctx->c_y*16);
	BTIC6A_EncodeBlock16(ctx, blk0_y, blk0_c);
	return(0);
}


void BTIC6A_EncodeCopyFrameClrs(BTIC6A_EncodeContext *ctx,
	byte *oimg, int clrs)
{
	byte *cimg;
	byte *ycs, *yct, *cs, *ct;
	int x, y, xs, ys, xsr, ysr, iystr, oystr, ystr2;
	int ocn, rgflip;
	
	ocn=4; rgflip=0;
	if(	(clrs==BTIC4B_CLRS_RGB) ||
		(clrs==BTIC4B_CLRS_BGR))
			ocn=3;
	if(	(clrs==BTIC4B_CLRS_RGB) ||
		(clrs==BTIC4B_CLRS_RGBA) ||
		(clrs==BTIC4B_CLRS_RGBX))
			rgflip=2;
	
	xs=ctx->bxs;
	ys=ctx->bys;
	xsr=ctx->xs;
	ysr=ctx->ys;
	cimg=ctx->cimg;
	iystr=xs*4;
	oystr=xsr*ocn;
	
	if((ocn==4) && !rgflip)
	{
		if(oystr==iystr)
		{
			memcpy(cimg, oimg, ys*oystr);
			return;
		}
	
		ystr2=oystr;
		if(iystr<ystr2)
			ystr2=iystr;
		for(y=0; y<ysr; y++)
			{ memcpy(cimg+y*iystr, oimg+y*oystr, ystr2); }
		return;
	}
	
	ycs=cimg;
	yct=oimg;
	for(y=0; y<ysr; y++)
	{
		cs=ycs;
		ct=yct;
		for(x=0; x<xsr; x++)
		{
			cs[0]=ct[0^rgflip];
			cs[1]=ct[1];
			cs[2]=ct[2^rgflip];
			if(ocn==4)
				cs[3]=ct[3];
			cs+=4; ct+=ocn;
		}
		ycs+=iystr;
		yct+=oystr;
	}
}

int btic6a_log2(int val)
{
	int ex, v;
	ex=0; v=val;
	while(v>1)
		{ ex++; v=v>>1; }
	return(ex);
}

int BTIC6A_EncodeFrameImage(BTIC6A_EncodeContext *ctx,
	byte *obuf, int obsz, void *ibuf, int qfl, int clrs)
{
	byte thbuf[64];
	byte *ct;
	int x, y, sz, osz, osz2;
	int i, j, k;

	BTIC6A_EncodeCopyFrameClrs(ctx, ibuf, clrs);
	
	ctx->clr_y=128;
	ctx->clr_u=128;
	ctx->clr_v=128;
	ctx->mv_x=0;
	ctx->mv_y=0;
	
	for(i=0; i<128; i++)
	{
		ctx->stf_2x2  [i]=0xBFFF;
		ctx->stf_4x4  [i]=0xBFFF;
		ctx->stf_8x8  [i]=0xBFFF;
		ctx->stf_16x16[i]=0xBFFF;
	}
	ctx->strov_2x2  =0;
	ctx->strov_4x4  =0;
	ctx->strov_8x8  =0;
	ctx->strov_16x16=0;
	
	ctx->ct=ctx->tfbuf;
	for(y=0; y<ctx->cys; y++)
	{
		for(x=0; x<ctx->cxs; x++)
		{
			ctx->c_x=x;		ctx->c_y=y;
			BTIC6A_EncodeFrameMacroblock(ctx);
		}
	}
	
	sz=ctx->ct-ctx->tfbuf;
	ct=obuf;
	
	k=0;
	k|=btic6a_log2(BTIC6A_CBSZ_2X )<< 0;
	k|=btic6a_log2(BTIC6A_CBSZ_4X )<< 4;
	k|=btic6a_log2(BTIC6A_CBSZ_8X )<< 8;
	k|=btic6a_log2(BTIC6A_CBSZ_16X)<<12;
	
	memset(thbuf, 0, 16);
	btpic_setu16(thbuf+0, ctx->xs);
	btpic_setu16(thbuf+2, ctx->ys);
	btpic_setu16(thbuf+4, 0x0000);
	btpic_setu16(thbuf+6, k);
	ct=BTIC5C_EncodeEmitTwoCC(ct, BTPIC_TCC_HX, thbuf, 16);

	ct=BTIC5C_EncodeEmitTwoCC(ct, BTPIC_TCC_IX, ctx->tfbuf, sz);
	osz=ct-obuf;

#ifdef RP2POST_RP2AUTO_C
	sz=PostRp2_EncodeRP2TestAuto(
		ctx->tfzbuf+12, obuf, ctx->tfbsz, osz, 
		RP2POST_FLAGS_FAVOR_SPEED);
	if(sz>0)
	{
		osz2=sz+12;
		btpic_setu32(ctx->tfzbuf+0, BTPIC_TCC_HX);
		btpic_setu32(ctx->tfzbuf+4, ~osz2);
		btpic_setu32(ctx->tfzbuf+8, osz);
		
		if((osz2*1.2)<osz)
		{
//			printf("BT6A: RP2=Yes, %d->%d\n", osz, osz2);
			memcpy(obuf, ctx->tfzbuf, osz2);
			osz=osz2;
		}else
		{
//			printf("BT6A: RP2=No, %d->%d\n", osz, osz2);
		}
	}else
	{
//		printf("BT6A: RP2=Failed\n");
	}
#endif
	
	return(osz);
}

int BTIC6A_EncodeContextSetupQuality(BTIC6A_EncodeContext *ctx, int qfl)
{
	int qf;

	qf=qfl&127;
	if(qf>100)qf=100;
	ctx->ld_flat=50-(qf/2);
	ctx->ld_2x2=100-qf;
	ctx->ld_skip=100-qf;
	ctx->ld_joint=100-qf;
	
	ctx->ld_flat/=3;
	ctx->ld_2x2/=3;
	ctx->ld_skip/=3;
	ctx->ld_joint/=3;
	
	ctx->ld_flat_sq=bt6a_abserr(ctx->ld_flat);
	ctx->ld_skip_sq=bt6a_abserr(ctx->ld_skip);
	
	return(0);
}

int BTIC6A_EncodeInitContext(BTIC6A_EncodeContext *ctx,
	int xs, int ys, int qfl)
{
	int qf;

	ctx->xs=xs;
	ctx->ys=ys;
	ctx->cxs=(xs+15)>>4;
	ctx->cys=(ys+15)>>4;
	ctx->bxs=ctx->cxs*16;
	ctx->bys=ctx->cys*16;

	ctx->cimg2x=malloc(ctx->bxs*((ctx->bys+24)*2)*4);
	ctx->cimg=ctx->cimg2x+(ctx->bxs*16)*4;
	ctx->pimg=ctx->cimg+(ctx->bxs*(ctx->bys+16))*4;

//	ctx->fl=0;

//	ctx->blka=malloc(ctx->bxs*ctx->bys*sizeof(u64));
//	ctx->lblka=malloc(ctx->bxs*ctx->bys*sizeof(u64));

	ctx->tfbsz=xs*ys*4;
	ctx->tfbuf=malloc(xs*ys*4);
	ctx->tfzbuf=malloc(xs*ys*4);
	
	BTIC6A_EncodeContextSetupQuality(ctx, qfl);

	return(0);
}
