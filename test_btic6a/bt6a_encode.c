#define BTIC6A_CBSZ_2X		4096
#define BTIC6A_CBSZ_4X		2048
#define BTIC6A_CBSZ_8X		512
#define BTIC6A_CBSZ_16X		128

#define BTIC6A_CBMSK_2X		(BTIC6A_CBSZ_2X-1)
#define BTIC6A_CBMSK_4X		(BTIC6A_CBSZ_4X-1)
#define BTIC6A_CBMSK_8X		(BTIC6A_CBSZ_8X-1)
#define BTIC6A_CBMSK_16X	(BTIC6A_CBSZ_16X-1)

/* How deep to hash-search for zoom */
#define BTIC6A_CBSZ_2X_H4	512
#define BTIC6A_CBSZ_2X_H8	256
#define BTIC6A_CBSZ_2X_H16	128

/* How deep to linear-search for zoom */
#define BTIC6A_CBSZ_2X_L4	2048
#define BTIC6A_CBSZ_2X_L8	1024
#define BTIC6A_CBSZ_2X_L16	1024

//#define BTIC6A_ERRSC_2X		1.0
//#define BTIC6A_ERRSC_4X		1.1
//#define BTIC6A_ERRSC_8X		1.2
//#define BTIC6A_ERRSC_16X	1.400

#define BTIC6A_ERRSC_2X		(1.0/1.000)
#define BTIC6A_ERRSC_4X		(1.0/1.125)
#define BTIC6A_ERRSC_8X		(1.0/1.250)
#define BTIC6A_ERRSC_16X	(1.0/1.500)

#define BTIC6A_SCBSZ_2X		16384
#define BTIC6A_SCBSZ_4X		16384

#define BTIC6A_SCBMSK_2X	(BTIC6A_SCBSZ_2X-1)
#define BTIC6A_SCBMSK_4X	(BTIC6A_SCBSZ_4X-1)


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

	u16 hcn_2x2   [BTIC6A_CBSZ_2X];
	u16 hcn_4x4   [BTIC6A_CBSZ_4X];
	u16 hcn_8x8   [BTIC6A_CBSZ_8X];
	u16 hcn_16x16 [BTIC6A_CBSZ_16X];

	u16 key_2x2   [BTIC6A_CBSZ_2X];
	u16 key_4x4   [BTIC6A_CBSZ_4X];
//	u16 key_8x8   [BTIC6A_CBSZ_8X];
//	u16 key_16x16 [BTIC6A_CBSZ_16X];

	u16 hash_2x2  [256];
	u16 hash_4x4  [256];
	u16 hash_8x8  [256];
	u16 hash_16x16[256];
	
	u16 stf_2x2  [128];
	u16 stf_4x4  [128];
	u16 stf_8x8  [128];
	u16 stf_16x16[128];

	byte scb_2x2_y[BTIC6A_SCBSZ_2X*4];
	byte scb_2x2_c[BTIC6A_SCBSZ_2X*2];
	u16 scb_4x4   [BTIC6A_SCBSZ_4X*4];
	u16 scnt_2x2  [BTIC6A_SCBSZ_2X];
	u16 scnt_4x4  [BTIC6A_SCBSZ_4X];
//	u16 scnt_8x8  [BTIC6A_SCBSZ_8X];
//	u16 scnt_16x16[BTIC6A_SCBSZ_16X];

	byte strov_2x2;
	byte strov_4x4;
	byte strov_8x8;
	byte strov_16x16;

	byte ld_flat;
	byte ld_2x2;
	byte ld_skip;
	byte ld_joint;

	u16 ld_flat_sq;
	u16 ld_skip_sq;

	u16 ld_flat_sql0;
	u16 ld_skip_sql0;
	u16 ld_flat_sql1;
	u16 ld_skip_sql1;
	u16 ld_flat_sql2;
	u16 ld_skip_sql2;
	u16 ld_flat_sql3;
	u16 ld_skip_sql3;

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
	byte *ppbuf;
	int xs, ys;
	int cxs, cys;
	int bxs, bys;
	int mv_x, mv_y;
	int c_x, c_y;
	int ppbsz, ppsz;
	
	byte clr_y;
	byte clr_u;
	byte clr_v;

	byte is_pframe;

	byte acc_dcom_l3;
	byte acc_dcom_l2;
	byte acc_dcom_l1;
	byte acc_dcom_l0;
};


BTIC6A_EncodeContext *BTIC6A_AllocEncodeContext(void)
{
	BTIC6A_EncodeContext *ctx;
	ctx=malloc(sizeof(BTIC6A_EncodeContext)+256);
	memset(ctx, 0, sizeof(BTIC6A_EncodeContext));
	return(ctx);
}

byte *BTIC6A_EncodeEmitFourCC(byte *ict, u32 fcc, void *buf, int sz)
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

byte *BTIC6A_EncodeEmitTwoCC(byte *ict, u16 tcc, void *buf, int sz)
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
	ct=BTIC6A_EncodeEmitFourCC(ict, tcc, buf, sz);
	return(ct);	
}

int btic6a_log2(int val)
{
	int ex, v;
	ex=0; v=val;
	while(v>1)
		{ ex++; v=v>>1; }
	return(ex);
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

#if 0
int bt6a_abserr(int x)
{
//	return(x*x);
//	return(abs(x));

//	return((x*x+abs(x))/2);
//	return(abs(x)+(x*x*7/8));
//	return(abs(x)+(x*x));
	return(abs(x)+((x*x)/4));
}
#endif

static short btic6a_abserr_tab[1024];
static short *btic6a_abserr_tp;

// #define bt6a_abserr(x)	(((x)^((x)>>31))+(((x)*(x))>>2))
#define bt6a_abserr(x)	(btic6a_abserr_tp[x])

int BTIC6A_EncodeCompareBlockAbsNErrorYUV(
	byte *blk0_y, byte *blk0_c,
	byte *blk1_y, byte *blk1_c,
	int bn)
{
	int cy0, cu0, cv0, cy1, cu1, cv1;
	int dcy, dcu, dcv;
	int ecy, ecu, ecv;
	int i, j, k, l;
	
	ecy=0;
	ecu=0;
	ecv=0;
	
	l=1<<(bn+2);
	for(i=0; i<l; i++)
	{
		cy0=blk0_y[i];	cy1=blk1_y[i];
		dcy=(cy0-cy1);
		ecy+=bt6a_abserr(dcy);
	}

	l=1<<(bn+0);
	for(i=0; i<l; i++)
	{
		cu0=blk0_c[i*2+0];		cv0=blk0_c[i*2+1];
		cu1=blk1_c[i*2+0];		cv1=blk1_c[i*2+1];
		dcu=(cu0-cu1);			dcv=(cv0-cv1);
		ecu+=bt6a_abserr(dcu);	ecv+=bt6a_abserr(dcv);
	}
	ecu*=4;
	ecv*=4;
	return(ecy+ecu+ecv);
}

int BTIC6A_EncodeCompareBlock2ErrorYUV(
	byte *blk0_y, byte *blk0_c,
	byte *blk1_y, byte *blk1_c,
	int icy, int icu, int icv)
{
	int pcy0, pcu0, pcv0, pcy1, pcu1, pcv1;
	int cy0, cu0, cv0, cy1, cy2, cy3, cu1, cv1;
	int dcy, dcu, dcv;
	int ecy, ecu, ecv;
	int i, j, k, l;
	
	ecy=0;
	ecu=0;
	ecv=0;
	
	pcy0=icy;
	pcy1=icy;

	pcu0=icu;	pcv0=icv;
	pcu1=icu;	pcv1=icv;

	cy0=blk0_y[0];			cy1=blk1_y[0];
	cy2=blk0_y[1];			cy3=blk1_y[1];
	cu0=blk0_c[0];			cv0=blk0_c[1];
	cu1=blk1_c[0];			cv1=blk1_c[1];

	pcy0=(pcy0+cy0)&255;	pcy1=(pcy1+cy1)&255;
	dcy=(pcy0-pcy1);		ecy+=bt6a_abserr(dcy);
	pcy0=(pcy0+cy2)&255;	pcy1=(pcy1+cy3)&255;
	dcy=(pcy0-pcy1);		ecy+=bt6a_abserr(dcy);

	cy0=blk0_y[2];			cy1=blk1_y[2];
	cy2=blk0_y[3];			cy3=blk1_y[3];

	pcu0=(pcu0+cu0)&255;	pcu1=(pcu1+cu1)&255;
	pcv0=(pcv0+cv0)&255;	pcv1=(pcv1+cv1)&255;

	pcy0=(pcy0+cy0)&255;	pcy1=(pcy1+cy1)&255;
	dcy=(pcy0-pcy1);		ecy+=bt6a_abserr(dcy);
	pcy0=(pcy0+cy2)&255;	pcy1=(pcy1+cy3)&255;
	dcy=(pcy0-pcy1);		ecy+=bt6a_abserr(dcy);

	dcu=(pcu0-pcu1);		dcv=(pcv0-pcv1);
	ecu=bt6a_abserr(dcu);	ecv=bt6a_abserr(dcv);

	ecu<<=2;	ecv<<=2;
	return(ecy+ecu+ecv);
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

int BTIC6A_EncodeCalcBlockNHashYUV(
	BTIC6A_EncodeContext *ctx,
	byte *blk_y, byte *blk_c, int bn)
{
	byte blk1_y[256], blk1_c[128];
	int cy0, cy1, cy2, cy3;
	int cy, cu, cv, h0, h, ex, bi;
	int i, j, k, ly, lc;

	ex=btic6a_log2(ctx->ld_flat)+1;
	bi=1<<(ex-1);

	if(bn==0)
	{
//		bi=0;

		cy0=blk_y[0]+0x80;		cy1=blk_y[1]+0x80;
		cy2=blk_y[2]+0x80;		cy3=blk_y[3]+0x80;
		cu =blk_c[0]+0x80;		cv =blk_c[1]+0x80;
		cy0=(cy0+bi)>>ex;		cy1=(cy1+bi)>>ex;
		cy2=(cy2+bi)>>ex;		cy3=(cy3+bi)>>ex;
		cu =(cu +bi)>>ex;		cv =(cv +bi)>>ex;

		h0=0x67;
		h0=(h0*31)+cy0;		h0=(h0*31)+cy1;
		h0=(h0*31)+cy2;		h0=(h0*31)+cy3;
		h0=(h0*31)+cu;		h0=(h0*31)+cv;
		h0=h0*251+cy0;		h0=h0*251+cy1;
		h0=h0*251+cy2;		h0=h0*251+cy3;
		h0=h0*251+cu;		h0=h0*251+cv;
		h0=h0*251+13;		h0=h0*251+17;
		
		h=(h0>>8)&255;
		return(h);
	}

//	if(bn==2)
	if(1)
	{
		ly=1<<(bn+2);
		lc=1<<(bn+1);

		for(i=0; i<ly; i++)
		{
			cy=blk_y[i]+0x80;
			cy=(cy+bi)>>ex;
			blk1_y[i]=cy;
		}
		for(i=0; i<lc; i++)
		{
			cy=blk_c[i]+0x80;
			cy=(cy+bi)>>ex;
			blk1_c[i]=cy;
		}
	
		h0=0x67;

		for(i=0; i<ly; i++)
			h0=(h0*251)+blk1_y[i];
		for(i=0; i<lc; i++)
			h0=(h0*251)+blk1_c[i];
		h0=h0*251+13;		h0=h0*251+17;
		
		h=(h0>>8)&255;
		return(h);
	}
	
	BTIC6A_EncodeCalcBlockNStepYUV(blk_y, blk_c,
		0x80, 0x80, 0x80, &cy, &cu, &cv, bn);
	
//	cy=(cy+8)>>4; cu=(cu+8)>>4; cv=(cv+8)>>4;
	cy=(cy+bi)>>ex; cu=(cu+bi)>>ex; cv=(cv+bi)>>ex;

	h0=0x67;
	h0=(h0*31)+cy;
	h0=(h0*31)+cu;
	h0=(h0*31)+cv;
	h0=h0*251+cy;
	h0=h0*251+cu;
	h0=h0*251+cv;
	h=(h0>>8)&255;
	return(h);
}

void BTIC6A_EncodeBlockSubsample4to2YUV(
	BTIC6A_EncodeContext *ctx,
	byte *iblk_y, byte *iblk_c,
	byte *oblk_y, byte *oblk_c)
{
	oblk_y[0]=(iblk_y[ 0]+iblk_y[ 1]+iblk_y[ 2]+iblk_y[ 3])>>2;
	oblk_y[1]=(iblk_y[ 4]+iblk_y[ 5]+iblk_y[ 6]+iblk_y[ 7])>>2;
	oblk_y[2]=(iblk_y[ 8]+iblk_y[ 9]+iblk_y[10]+iblk_y[11])>>2;
	oblk_y[3]=(iblk_y[12]+iblk_y[13]+iblk_y[14]+iblk_y[15])>>2;
	oblk_c[0]=(iblk_c[0]+iblk_c[2]+iblk_c[4]+iblk_c[6])>>2;
	oblk_c[1]=(iblk_c[1]+iblk_c[3]+iblk_c[5]+iblk_c[7])>>2;
}

void BTIC6A_EncodeBlockSubsample8to4YUV(
	BTIC6A_EncodeContext *ctx,
	byte *iblk_y, byte *iblk_c,
	byte *oblk_y, byte *oblk_c)
{
	int i;
	for(i=0; i<4; i++)
	{
		oblk_y[i*4+0]=(	iblk_y[i*16+ 0]+iblk_y[i*16+ 1]+
						iblk_y[i*16+ 2]+iblk_y[i*16+ 3])>>2;
		oblk_y[i*4+1]=(	iblk_y[i*16+ 4]+iblk_y[i*16+ 5]+
						iblk_y[i*16+ 6]+iblk_y[i*16+ 7])>>2;
		oblk_y[i*4+2]=(	iblk_y[i*16+ 8]+iblk_y[i*16+ 9]+
						iblk_y[i*16+10]+iblk_y[i*16+11])>>2;
		oblk_y[i*4+3]=(	iblk_y[i*16+12]+iblk_y[i*16+13]+
						iblk_y[i*16+14]+iblk_y[i*16+15])>>2;
		oblk_c[i*2+0]=(	iblk_c[i*8+0]+iblk_c[i*8+2]+
						iblk_c[i*8+4]+iblk_c[i*8+6])>>2;
		oblk_c[i*2+1]=(	iblk_c[i*8+1]+iblk_c[i*8+3]+
						iblk_c[i*8+5]+iblk_c[i*8+7])>>2;
	}
}

void BTIC6A_EncodeBlockSubsample16to8YUV(
	BTIC6A_EncodeContext *ctx,
	byte *iblk_y, byte *iblk_c,
	byte *oblk_y, byte *oblk_c)
{
	int i;
	for(i=0; i<16; i++)
	{
		oblk_y[i*4+0]=(	iblk_y[i*16+ 0]+iblk_y[i*16+ 1]+
						iblk_y[i*16+ 2]+iblk_y[i*16+ 3])>>2;
		oblk_y[i*4+1]=(	iblk_y[i*16+ 4]+iblk_y[i*16+ 5]+
						iblk_y[i*16+ 6]+iblk_y[i*16+ 7])>>2;
		oblk_y[i*4+2]=(	iblk_y[i*16+ 8]+iblk_y[i*16+ 9]+
						iblk_y[i*16+10]+iblk_y[i*16+11])>>2;
		oblk_y[i*4+3]=(	iblk_y[i*16+12]+iblk_y[i*16+13]+
						iblk_y[i*16+14]+iblk_y[i*16+15])>>2;
		oblk_c[i*2+0]=(	iblk_c[i*8+0]+iblk_c[i*8+2]+
						iblk_c[i*8+4]+iblk_c[i*8+6])>>2;
		oblk_c[i*2+1]=(	iblk_c[i*8+1]+iblk_c[i*8+3]+
						iblk_c[i*8+5]+iblk_c[i*8+7])>>2;
	}
}

void BTIC6A_EncodeBlockSubsample8to2YUV(
	BTIC6A_EncodeContext *ctx,
	byte *iblk_y, byte *iblk_c,
	byte *oblk_y, byte *oblk_c)
{
	byte tblk_y[16], tblk_c[8];
	BTIC6A_EncodeBlockSubsample8to4YUV(ctx, iblk_y, iblk_c, tblk_y, tblk_c);
	BTIC6A_EncodeBlockSubsample4to2YUV(ctx, tblk_y, tblk_c, oblk_y, oblk_c);
}

void BTIC6A_EncodeBlockSubsample16to2YUV(
	BTIC6A_EncodeContext *ctx,
	byte *iblk_y, byte *iblk_c,
	byte *oblk_y, byte *oblk_c)
{
	byte tblk_y[64], tblk_c[32];
	BTIC6A_EncodeBlockSubsample16to8YUV(ctx, iblk_y, iblk_c, tblk_y, tblk_c);
	BTIC6A_EncodeBlockSubsample8to4YUV(ctx, tblk_y, tblk_c, tblk_y, tblk_c);
	BTIC6A_EncodeBlockSubsample4to2YUV(ctx, tblk_y, tblk_c, oblk_y, oblk_c);
}

int BTIC6A_CheckValidVqBlock2Index(BTIC6A_EncodeContext *ctx, int idx)
{
	if(idx&(~0xFFFF))	return(0);		//Not 16b
	if(idx>=0xC000)		return(1);		//Special Always-Valid
	if(idx>=0x8000)		return(0);		//Special, Absolute
	if(((ctx->rov_2x2-idx)&0x3FFF)>=(BTIC6A_CBSZ_2X-72))
		return(0);
	return(1);
}

int BTIC6A_CheckValidVqBlock4Index(BTIC6A_EncodeContext *ctx, int idx)
{
	if(idx&(~0xFFFF))	return(0);		//Not 16b
	if(idx>=0xC000)		return(1);		//Special Always-Valid
	if(idx>=0x8000)		return(0);		//Special, Absolute
	if(idx>=0x4000)
		return(BTIC6A_CheckValidVqBlock2Index(ctx, idx));
	if(((ctx->rov_4x4-idx)&0x3FFF)>=(BTIC6A_CBSZ_4X-20))
		return(0);
	return(1);
}

int BTIC6A_CheckValidVqBlock8Index(BTIC6A_EncodeContext *ctx, int idx)
{
	if(idx&(~0xFFFF))	return(0);		//Not 16b
	if(idx>=0xC000)		return(1);		//Special Always-Valid
	if(idx>=0x8000)		return(0);		//Special, Absolute
	if(idx>=0x4000)
		return(BTIC6A_CheckValidVqBlock2Index(ctx, idx));
	if(((ctx->rov_8x8-idx)&0x3FFF)>=(BTIC6A_CBSZ_8X-6))
		return(0);
	return(1);
}

int BTIC6A_CheckValidVqBlock16Index(BTIC6A_EncodeContext *ctx, int idx)
{
	if(idx&(~0xFFFF))	return(0);		//Not 16b
	if(idx>=0xC000)		return(1);		//Special Always-Valid
	if(idx>=0x8000)		return(0);		//Special, Absolute
	if(idx>=0x4000)
		return(BTIC6A_CheckValidVqBlock2Index(ctx, idx));

	if(((ctx->rov_16x16-idx)&0x3FFF)>=(BTIC6A_CBSZ_16X-2))
		return(0);
	return(1);
}

int BTIC6A_CheckStaleBlock2Index(BTIC6A_EncodeContext *ctx, int idx)
{
	if(idx&(~0xFFFF))	return(0);		//Not 16b
	if(idx>=0xC000)		return(0);		//Special Always-Valid
	if(idx>=0x8000)		return(0);		//Special, Absolute
//	if(idx>=0x4000)		return(0);		//Special, Absolute
	if(((ctx->rov_2x2-idx)&0x3FFF)>=((BTIC6A_CBSZ_2X*3)/4))
		return(1);
	return(0);
}

int BTIC6A_CheckFreshBlock2Index(BTIC6A_EncodeContext *ctx, int idx)
{
	if(idx&(~0xFFFF))	return(0);		//Not 16b
	if(idx>=0xC000)		return(1);		//Special Always-Valid
	if(idx>=0x8000)		return(0);		//Special, Absolute
//	if(idx>=0x4000)		return(0);		//Special, Absolute
	if(((ctx->rov_2x2-idx)&0x3FFF)<((BTIC6A_CBSZ_2X*3)/8))
		return(1);
	return(0);
}

int BTIC6A_CheckStaleBlock4Index(BTIC6A_EncodeContext *ctx, int idx)
{
	u16 *ici;
	if(idx&(~0xFFFF))	return(0);		//Not 16b
	if(idx>=0xC000)		return(0);		//Special Always-Valid
	if(idx>=0x8000)		return(0);		//Special, Absolute
//	if(idx>=0x4000)
	if(idx&0x4000)
		return(BTIC6A_CheckStaleBlock2Index(ctx, idx));
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
	if(idx>=0x8000)		return(0);		//Special, Absolute
//	if(idx>=0x4000)
	if(idx&0x4000)
		return(BTIC6A_CheckFreshBlock2Index(ctx, idx));
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
	if(idx>=0x8000)		return(0);		//Special, Absolute
//	if(idx>=0x4000)
	if(idx&0x4000)
		return(BTIC6A_CheckStaleBlock2Index(ctx, idx));
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
	if(idx>=0x8000)		return(0);		//Special, Absolute
//	if(idx>=0x4000)
	if(idx&0x4000)
		return(BTIC6A_CheckFreshBlock2Index(ctx, idx));
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
	if(idx>=0x8000)		return(0);		//Special, Absolute
//	if(idx>=0x4000)
	if(idx&0x4000)
		return(BTIC6A_CheckStaleBlock2Index(ctx, idx));
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
	if(idx>=0x8000)		return(0);		//Special, Absolute
//	if(idx>=0x4000)
	if(idx&0x4000)
		return(BTIC6A_CheckFreshBlock2Index(ctx, idx));
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

//	if(idx>=0xC000)
	if(idx&0x8000)
	{
		if((idx==0xFFFF) || (idx==0xFFFC))
		{
			btpic_setu32(blk_y, 0);
			btpic_setu16(blk_c, 0);
			return(0);
		}
		__debugbreak();
		return(-1);
	}

	if(!BTIC6A_CheckValidVqBlock2Index(ctx, idx))
	{
		btpic_setu32(blk_y, 0);
		btpic_setu16(blk_c, 0);
		return(-1);
	}

	if(ctx->flg_2x2[idx&BTIC6A_CBMSK_2X]&1)
	{
		btpic_setu32(blk_y, 0);
		btpic_setu16(blk_c, 0);
		return(-1);
	}

	ici_y=ctx->cb_2x2_y+((idx&BTIC6A_CBMSK_2X)*4);
	ici_c=ctx->cb_2x2_c+((idx&BTIC6A_CBMSK_2X)*2);

	rt2=btpic_getu32(ici_y);
	rt3=btpic_getu16(ici_c);
	btpic_setu32(blk_y, rt2);
	btpic_setu16(blk_c, rt3);

//	memcpy(blk_y, ici_y, 4);
//	memcpy(blk_c, ici_c, 2);
	return(0);
}

int BTIC6A_EncodeGetVqBlock4(BTIC6A_EncodeContext *ctx,
	int idx, byte *blk_y, byte *blk_c)
{
	u16 *ici;
	int rt0, rt1, rt2, rt3;
	memset(blk_y, 0, 16);
	memset(blk_c, 0, 8);
//	if(idx>=0xC000)
//		{ return(0); }

	if(idx&0x8000)
	{
		if((idx==0xFFFF) || (idx==0xFFFC))
		{
			memset(blk_y, 0, 16);
			memset(blk_c, 0, 8);
			return(0);
		}
		__debugbreak();
		return(-1);
	}

	if(idx&0x4000)
	{
		memset(blk_y, 0, 16);
		memset(blk_c, 0, 8);
		rt0=BTIC6A_EncodeGetVqBlock2(ctx, idx, blk_y, blk_c);
		blk_y[ 4]=blk_y[1];		blk_y[1]=0;
		blk_y[ 8]=blk_y[2];		blk_y[2]=0;
		blk_y[12]=blk_y[3];		blk_y[3]=0;
		return(rt0);
	}

	if(!BTIC6A_CheckValidVqBlock4Index(ctx, idx))
		return(-1);

	if(ctx->flg_4x4[idx&BTIC6A_CBMSK_4X]&1)
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

	if(idx&0x8000)
	{
		if((idx==0xFFFF) || (idx==0xFFFC))
		{
			memset(blk_y, 0, 64);
			memset(blk_c, 0, 32);
			return(0);
		}
		__debugbreak();
		return(-1);
	}

	if(idx&0x4000)
	{
		memset(blk_y, 0, 64);
		memset(blk_c, 0, 32);

		rt0=BTIC6A_EncodeGetVqBlock2(ctx, idx, blk_y, blk_c);
		blk_y[16]=blk_y[1];		blk_y[1]=0;
		blk_y[32]=blk_y[2];		blk_y[2]=0;
		blk_y[48]=blk_y[3];		blk_y[3]=0;
		return(rt0);
	}

	if(!BTIC6A_CheckValidVqBlock8Index(ctx, idx))
		return(-1);

	if(ctx->flg_8x8[idx&BTIC6A_CBMSK_8X]&1)
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
//	if(idx>=0xC000)

	if(idx&0x8000)
	{
		if((idx==0xFFFF) || (idx==0xFFFC))
		{
			memset(blk_y, 0, 256);
			memset(blk_c, 0, 128);
			return(0);
		}
		__debugbreak();
		return(0);
	}

	if(idx&0x4000)
	{
		memset(blk_y, 0, 256);
		memset(blk_c, 0, 128);

		rt0=BTIC6A_EncodeGetVqBlock2(ctx, idx, blk_y, blk_c);
		blk_y[ 64]=blk_y[1];	blk_y[1]=0;
		blk_y[128]=blk_y[2];	blk_y[2]=0;
		blk_y[192]=blk_y[3];	blk_y[3]=0;
		return(rt0);
	}

	if(!BTIC6A_CheckValidVqBlock16Index(ctx, idx))
		return(-1);

	if(ctx->flg_16x16[idx&BTIC6A_CBMSK_16X]&1)
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


int BTIC6A_EncStaticRefreshBlock2Key(BTIC6A_EncodeContext *ctx, int key)
{
	int ix;
	int i;
	
	ix=key&BTIC6A_SCBMSK_2X;
	if(ctx->scnt_2x2[ix]<0xFFF0)
	{
		ctx->scnt_2x2[ix]++;
	}else
	{
		for(i=0; i<BTIC6A_SCBSZ_2X; i++)
		{
			if(ctx->scnt_2x2[i]>0)
				ctx->scnt_2x2[i]--;
		}
		ctx->scnt_2x2[ix]++;
	}
	return(0);
}

int BTIC6A_EncStaticAddBlock2Key(BTIC6A_EncodeContext *ctx,
	byte *blk_y, byte *blk_c)
{
	byte *ici_y, *ici_c;
	int icy, icu, icv;
	int ix2, ixk, h;
	int bd, bi, d;
	int i;

	icy=ctx->clr_y;
	icu=ctx->clr_u;
	icv=ctx->clr_v;

	bi=-1; bd=3*256*256*256;
	for(i=0; i<BTIC6A_SCBSZ_2X; i++)
	{
		ici_y=ctx->scb_2x2_y+(i*4);
		ici_c=ctx->scb_2x2_c+(i*2);

//		k=BTIC6A_EncodeGetVqBlock2(ctx,
//			ix2, blk1_y, blk1_c);
		d=BTIC6A_EncodeCompareBlock2ErrorYUV(
			blk_y, blk_c, ici_y, ici_c, icy, icu, icv);
		if(d<0)		break;
		if(d<bd)
		{
			bi=i; bd=d;
//			if((bd/2)<=ctx->ld_skip_sql3)
//				break;
		}
	}

	if(bd<=ctx->ld_skip_sql3)
	{
		BTIC6A_EncStaticRefreshBlock2Key(ctx, bi);
		return(bi);
	}

	bi=0; bd=0xFFFF;
	for(i=0; i<BTIC6A_SCBSZ_2X; i++)
	{
		d=ctx->scnt_2x2[i];
		if(d<bd)
			{ bi=i; bd=d; }
	}
	
	if(bd>0)
	{
		for(i=0; i<BTIC6A_SCBSZ_2X; i++)
			ctx->scnt_2x2[i]-=bd;
	}

	ctx->scnt_2x2[bi]=1;
	ici_y=ctx->scb_2x2_y+(bi*4);
	ici_c=ctx->scb_2x2_c+(bi*2);
	memcpy(ici_y, blk_y, 4);
	memcpy(ici_c, blk_c, 2);
	return(bi);
}

int BTIC6A_EncStaticGetTopBlock2Keys(BTIC6A_EncodeContext *ctx,
	u16 *keys, int max)
{
	int nkey, cn1, cn2, k1;
	int i, j, k;

	nkey=0;
	for(i=0; i<BTIC6A_SCBSZ_2X; i++)
	{
		cn1=ctx->scnt_2x2[i];
		if(!cn1)
			continue;
		if(nkey<max)
		{
			k=nkey++;
			for(j=k; j>0; j--)
			{
				k1=keys[j-1];
				cn2=ctx->scnt_2x2[k1];
				if(cn2>=cn1)
					break;
				keys[j]=k1;
			}
			keys[j]=i;
		}else
		{
			k1=keys[nkey-1];
			cn2=ctx->scnt_2x2[k1];
			if(cn2>=cn1)
				continue;
			for(j=nkey-1; j>0; j--)
			{
				k1=keys[j-1];
				cn2=ctx->scnt_2x2[k1];
				if(cn2>=cn1)
					break;
				keys[j]=k1;
			}
			keys[j]=i;			
		}
	}
	return(nkey);
}

int BTIC6A_EncAddBlock2Index(BTIC6A_EncodeContext *ctx,
	byte *blk_y, byte *blk_c)
{
	byte *ici_y, *ici_c;
	int ix2, ixk, h;

	ix2=ctx->rov_2x2;
	ctx->rov_2x2=(ix2+1)&0x3FFF;
	ici_y=ctx->cb_2x2_y+((ix2&BTIC6A_CBMSK_2X)*4);
	ici_c=ctx->cb_2x2_c+((ix2&BTIC6A_CBMSK_2X)*2);
	ici_y[0]=blk_y[0];		ici_y[1]=blk_y[1];
	ici_y[2]=blk_y[2];		ici_y[3]=blk_y[3];
	ici_c[0]=blk_c[0];		ici_c[1]=blk_c[1];
	ctx->rfr_2x2[ix2&BTIC6A_CBMSK_2X]=0xBFFF;
	ctx->flg_2x2[ix2&BTIC6A_CBMSK_2X]=0;

	ixk=BTIC6A_EncStaticAddBlock2Key(ctx, blk_y, blk_c);
	ctx->key_2x2[ix2&BTIC6A_CBMSK_2X]=ixk;
	
	h=BTIC6A_EncodeCalcBlockNHashYUV(ctx, blk_y, blk_c, 0);
	ctx->hcn_2x2[ix2&BTIC6A_CBMSK_2X]=ctx->hash_2x2[h];
	ctx->hash_2x2[h]=ix2;
		
	return(ix2);
}

int BTIC6A_EncRefreshBlock2Index(BTIC6A_EncodeContext *ctx, int ix2)
{
	byte *ici_y, *ici_c;
	int ix3, ixk;
	if(ix2&0x8000)
		{ __debugbreak(); }
	if(ctx->flg_2x2[ix2&BTIC6A_CBMSK_2X]&1)
	{
		__debugbreak();
		return(-1);
	}
	ici_y=ctx->cb_2x2_y+((ix2&BTIC6A_CBMSK_2X)*4);
	ici_c=ctx->cb_2x2_c+((ix2&BTIC6A_CBMSK_2X)*2);
	ixk=ctx->key_2x2[ix2&BTIC6A_CBMSK_2X];
	ix3=BTIC6A_EncAddBlock2Index(ctx, ici_y, ici_c);
	ctx->rfr_2x2[ix2&BTIC6A_CBMSK_2X]=ix3;
	ctx->key_2x2[ix3&BTIC6A_CBMSK_2X]=ixk;
	BTIC6A_EncStaticRefreshBlock2Key(ctx, ixk);
	return(ix3);
}

int BTIC6A_EncBoostBlock2Index(BTIC6A_EncodeContext *ctx, int ix2)
{
	int ix3, ixk;
	ixk=ctx->key_2x2[ix2&BTIC6A_CBMSK_2X];
	BTIC6A_EncStaticRefreshBlock2Key(ctx, ixk);
	return(0);
}

int BTIC6A_EncWalkFreshenBlock2Index(BTIC6A_EncodeContext *ctx, int ix2)
{
	int ix4, ix5;
	ix4=ix2;
	if(ix2&0x8000)
		{ __debugbreak(); }
	if(ctx->flg_2x2[ix2&BTIC6A_CBMSK_2X]&1)
	{
		__debugbreak();
		return(-1);
	}
	ix5=ctx->rfr_2x2[ix4&BTIC6A_CBMSK_2X];
	while(BTIC6A_CheckValidVqBlock2Index(ctx, ix5) && !(ix5&0x8000))
		{ ix4=ix5; ix5=ctx->rfr_2x2[ix4&BTIC6A_CBMSK_2X]; }
	return(ix4);
}

int BTIC6A_EncAddBlock4Index(BTIC6A_EncodeContext *ctx, u16 *blk_i)
{
	byte blk1_y[16], blk1_c[8];
	u16 *ici;
	int ix2, h;

	ix2=ctx->rov_4x4;
	ctx->rov_4x4=(ix2+1)&0x3FFF;
	ici=ctx->cb_4x4+((ix2&BTIC6A_CBMSK_4X)*4);
	ici[0]=blk_i[0];		ici[1]=blk_i[1];
	ici[2]=blk_i[2];		ici[3]=blk_i[3];
	ctx->rfr_4x4[ix2&BTIC6A_CBMSK_4X]=0xBFFF;
	ctx->flg_4x4[ix2&BTIC6A_CBMSK_4X]=0;

	BTIC6A_EncodeGetVqBlock4(ctx, ix2, blk1_y, blk1_c);
	h=BTIC6A_EncodeCalcBlockNHashYUV(ctx, blk1_y, blk1_c, 2);
	ctx->hcn_4x4[ix2&BTIC6A_CBMSK_4X]=ctx->hash_4x4[h];
	ctx->hash_4x4[h]=ix2;

	return(ix2);
}

int BTIC6A_EncRefreshBlock4Index(BTIC6A_EncodeContext *ctx, int ix2)
{
	u16 *ici;
	int ix3;
	if(ix2&0x8000)
		{ __debugbreak(); }
	if(ctx->flg_4x4[ix2&BTIC6A_CBMSK_4X]&1)
	{
		__debugbreak();
		return(-1);
	}
	ici=ctx->cb_4x4+((ix2&BTIC6A_CBMSK_4X)*4);
	ix3=BTIC6A_EncAddBlock4Index(ctx, ici);
	ctx->rfr_4x4[ix2&BTIC6A_CBMSK_4X]=ix3;
	return(ix3);
}

int BTIC6A_EncWalkFreshenBlock4Index(BTIC6A_EncodeContext *ctx, int ix2)
{
	int ix4, ix5;
	if(ix2&0x8000)
		{ __debugbreak(); }
	if(ctx->flg_4x4[ix2&BTIC6A_CBMSK_4X]&1)
	{
		__debugbreak();
		return(-1);
	}
	ix4=ix2;
	ix5=ctx->rfr_4x4[ix4&BTIC6A_CBMSK_4X];
	while(BTIC6A_CheckValidVqBlock4Index(ctx, ix5) && !(ix5&0x8000))
		{ ix4=ix5; ix5=ctx->rfr_4x4[ix4&BTIC6A_CBMSK_4X]; }
	return(ix4);
}

int BTIC6A_EncAddBlock8Index(BTIC6A_EncodeContext *ctx, u16 *blk_i)
{
	byte blk1_y[64], blk1_c[32];
	u16 *ici;
	int ix2, h;

	ix2=ctx->rov_8x8;
	ctx->rov_8x8=(ix2+1)&0x3FFF;
	ici=ctx->cb_8x8+((ix2&BTIC6A_CBMSK_8X)*4);
	ici[0]=blk_i[0];		ici[1]=blk_i[1];
	ici[2]=blk_i[2];		ici[3]=blk_i[3];
	ctx->rfr_8x8[ix2&BTIC6A_CBMSK_8X]=0xBFFF;
	ctx->flg_8x8[ix2&BTIC6A_CBMSK_8X]=0;

	BTIC6A_EncodeGetVqBlock8(ctx, ix2, blk1_y, blk1_c);
	h=BTIC6A_EncodeCalcBlockNHashYUV(ctx, blk1_y, blk1_c, 4);
	ctx->hcn_8x8[ix2&BTIC6A_CBMSK_8X]=ctx->hash_8x8[h];
	ctx->hash_8x8[h]=ix2;

	return(ix2);
}

int BTIC6A_EncRefreshBlock8Index(BTIC6A_EncodeContext *ctx, int ix2)
{
	u16 *ici;
	int ix3;
	if(ix2&0x8000)
		{ __debugbreak(); }
	if(ctx->flg_8x8[ix2&BTIC6A_CBMSK_8X]&1)
	{
		__debugbreak();
		return(-1);
	}
	ici=ctx->cb_8x8+((ix2&BTIC6A_CBMSK_8X)*4);
	ix3=BTIC6A_EncAddBlock8Index(ctx, ici);
	ctx->rfr_8x8[ix2&BTIC6A_CBMSK_8X]=ix3;
	return(ix3);
}

int BTIC6A_EncWalkFreshenBlock8Index(BTIC6A_EncodeContext *ctx, int ix2)
{
	int ix4, ix5;
	if(ix2&0x8000)
		{ __debugbreak(); }
	if(ctx->flg_8x8[ix2&BTIC6A_CBMSK_8X]&1)
	{
		__debugbreak();
		return(-1);
	}
	ix4=ix2;
	ix5=ctx->rfr_8x8[ix4&BTIC6A_CBMSK_8X];
	while(BTIC6A_CheckValidVqBlock8Index(ctx, ix5) && !(ix5&0x8000))
		{ ix4=ix5; ix5=ctx->rfr_8x8[ix4&BTIC6A_CBMSK_8X]; }
	return(ix4);
}

int BTIC6A_EncAddBlock16Index(BTIC6A_EncodeContext *ctx, u16 *blk_i)
{
	byte blk1_y[256], blk1_c[128];
	u16 *ici;
	int ix2, h;

	ix2=ctx->rov_16x16;
	ctx->rov_16x16=(ix2+1)&0x3FFF;
	ici=ctx->cb_16x16+((ix2&BTIC6A_CBMSK_16X)*4);
	ici[0]=blk_i[0];		ici[1]=blk_i[1];
	ici[2]=blk_i[2];		ici[3]=blk_i[3];
	ctx->rfr_16x16[ix2&BTIC6A_CBMSK_16X]=0xBFFF;
	ctx->flg_16x16[ix2&BTIC6A_CBMSK_16X]=0;

	BTIC6A_EncodeGetVqBlock16(ctx, ix2, blk1_y, blk1_c);
	h=BTIC6A_EncodeCalcBlockNHashYUV(ctx, blk1_y, blk1_c, 6);
	ctx->hcn_16x16[ix2&BTIC6A_CBMSK_16X]=ctx->hash_16x16[h];
	ctx->hash_16x16[h]=ix2;

	return(ix2);
}

int BTIC6A_EncRefreshBlock16Index(BTIC6A_EncodeContext *ctx, int ix2)
{
	u16 *ici;
	int ix3;
	if(ix2&0x8000)
		{ __debugbreak(); }
	if(ctx->flg_16x16[ix2&BTIC6A_CBMSK_16X]&1)
	{
		__debugbreak();
		return(-1);
	}
	ici=ctx->cb_16x16+((ix2&BTIC6A_CBMSK_16X)*4);
	ix3=BTIC6A_EncAddBlock16Index(ctx, ici);
	ctx->rfr_16x16[ix2&BTIC6A_CBMSK_16X]=ix3;
	return(ix3);
}

int BTIC6A_EncWalkFreshenBlock16Index(BTIC6A_EncodeContext *ctx, int ix2)
{
	int ix4, ix5;
	if(ix2&0x8000)
		{ __debugbreak(); }
	if(ctx->flg_16x16[ix2&BTIC6A_CBMSK_16X]&1)
	{
		__debugbreak();
		return(-1);
	}
	ix4=ix2;
	ix5=ctx->rfr_16x16[ix4&BTIC6A_CBMSK_8X];
	while(BTIC6A_CheckValidVqBlock16Index(ctx, ix5) && !(ix5&0x8000))
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
	int cy, cu, cv, icy, icu, icv, hash2;
	int i, j, k;

	icy=ctx->clr_y;		icu=ctx->clr_u;		icv=ctx->clr_v;
	BTIC6A_EncodeBlock2DeltaYUV(blk_y, blk_c, blk0_y, blk0_c, icy, icu, icv);

	hash2=BTIC6A_EncodeCalcBlockNHashYUV(ctx, blk0_y, blk0_c, 0);

	BTIC6A_EncodeGetVqBlock2(ctx, 0xFFFF, blk1_y, blk1_c);
	d=BTIC6A_EncodeCompareBlock2ErrorYUV(
		blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv);
	if((d/4)<ctx->ld_flat_sql3)
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
		d=BTIC6A_EncodeCompareBlock2ErrorYUV(
			blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv);
		if(d<0)
			continue;
		if(d<bd)
		{
			bi=i; bd=d;
//			if(((bd/2)<=ctx->ld_skip_sql3))
//				break;
		}
	}

	if((bi>=0) && ((bd/4)<=ctx->ld_skip_sql3))
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

		BTIC6A_EncBoostBlock2Index(ctx, ix2);

		BTIC6A_EncodeCalcBlockNStepYUV(
			blk1_y, blk1_c, icy, icu, icv, &cy, &cu, &cv, 0);
		ctx->clr_y=cy;		ctx->clr_u=cu;		ctx->clr_v=cv;
		return(ix2);
	}
#endif


	bi=-1; bd=3*256*256*256;
#if 1
	ix2=ctx->hash_2x2[hash2];
	for(i=0; i<BTIC6A_CBSZ_2X; i++)
	{
		if(ix2&0x8000)
			break;
		if(ctx->flg_2x2[ix2&BTIC6A_CBMSK_2X]&1)
			break;
		k=BTIC6A_EncodeGetVqBlock2(ctx,
			ix2, blk1_y, blk1_c);
		if(k<0)		break;

		d=BTIC6A_EncodeCompareBlock2ErrorYUV(
			blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv);
		if(d<0)		break;
		j=(ctx->rov_2x2-ix2-1)&0x3FFF;
		if(d<bd && (j<4096))
		{
			bi=j; bd=d;
			if(((bd/2)<=ctx->ld_skip_sql3))
				break;
		}
		ix2=ctx->hcn_2x2[ix2&BTIC6A_CBMSK_2X];
		k=(ctx->rov_2x2-ix2-1)&0x3FFF;
		if(k<=j)
			break;
	}

	if((bd/4)>ctx->ld_skip_sql3)
	{
		for(i=0; i<BTIC6A_CBSZ_2X; i++)
		{
			ix2=(ctx->rov_2x2-(i+1))&0x3FFF;
			if(ctx->flg_2x2[ix2&BTIC6A_CBMSK_2X]&1)
				continue;
			k=BTIC6A_EncodeGetVqBlock2(ctx,
				ix2, blk1_y, blk1_c);
			if(k<0)		continue;
			d=BTIC6A_EncodeCompareBlock2ErrorYUV(
				blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv);
			if(d<0)
				continue;
			if(d<bd)
			{
				bi=i; bd=d;
				if(((bd/2)<=ctx->ld_skip_sql3))
					break;
			}
		}
	}
#endif

	if((bi>=0) && ((bd/4)<=ctx->ld_skip_sql3))
	{
		ix2=(ctx->rov_2x2-(bi+1))&0x3FFF;
		BTIC6A_EncodeGetVqBlock2(ctx,
			ix2, blk1_y, blk1_c);

		ctx->bytes_cb_2x2_idx2+=2;

		ctx->lct=ctx->ct;
		ctx->lct2=ctx->ct;

		if(BTIC6A_CheckStaleBlock2Index(ctx, ix2))
		{
			*ctx->ct++=0x09|(bi<<4);
			*ctx->ct++=(bi>>4)&255;
			ix2=BTIC6A_EncRefreshBlock2Index(ctx, ix2);
		}else
		{
			*ctx->ct++=0x03|(bi<<4);
			*ctx->ct++=(bi>>4)&255;
			BTIC6A_EncBoostBlock2Index(ctx, ix2);
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

		if(	(iy0<16) && (iy1<16) &&
			(iy2<16) && (iy3<16) &&
			(iy6<16) && (iy7<16) )
		{
			blk1_y[0]=(iy0-8)<<j;		blk1_y[1]=(iy1-8)<<j;
			blk1_y[2]=(iy2-8)<<j;		blk1_y[3]=(iy3-8)<<j;
			blk1_c[0]=(iy6-8)<<j;		blk1_c[1]=(iy7-8)<<j;

			d=BTIC6A_EncodeCompareBlock2ErrorYUV(
				blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv);
			if((d/4)>ctx->ld_skip_sql3)
				continue;

			tv=(iy0<<0)|(iy1<<4)|(iy2<<8)|(iy3<<12)|(iy6<<16)|(iy7<<20);
			tv=(tv<<8)|0x17;
			if(j==3)
				tv|=0x20;
			
			if(	(ctx->lct==ctx->lct2) &&
				(((*ctx->lct2)&0x3FU)==(tv&0x3FU)) &&
				(((*ctx->lct2)&0xC0U)<0xC0U))
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

			d=BTIC6A_EncodeCompareBlock2ErrorYUV(
				blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv);
			if((d/4)>ctx->ld_skip_sql3)
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

#if 1
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
				d=BTIC6A_EncodeCompareBlock2ErrorYUV(
					blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv);
				if((d/4)>ctx->ld_skip_sql3)
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
#endif

	ix0=blk0_y[0];	ix1=blk0_y[1];
	ix2=blk0_y[2];	ix3=blk0_y[3];
	ix6=blk0_c[0];	ix7=blk0_c[1];

	if(	(ctx->lct2==ctx->lct) &&
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
	*ctx->ct++=ix0;		*ctx->ct++=ix1;
	*ctx->ct++=ix2;		*ctx->ct++=ix3;
	*ctx->ct++=ix6;		*ctx->ct++=ix7;
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
	byte blk0a_y[4], blk0a_c[2];
	byte blk0b_y[4], blk0b_c[2];
	u16 ictab[4];
	u16 *ici, *icj;
	int bi, bd, d, hash2, hash4, ena_2x2, ena_4x4, ena_2x2b;
	int ix0, ix1, ix2, ix3, ix4;
	int cy, cu, cv, icy, icu, icv;
	int i, j, k;

	icy=ctx->clr_y;		icu=ctx->clr_u;		icv=ctx->clr_v;
	BTIC6A_EncodeBlock4DeltaYUV(blk_y, blk_c, blk0_y, blk0_c, icy, icu, icv);

	hash4=BTIC6A_EncodeCalcBlockNHashYUV(ctx, blk0_y, blk0_c, 2);

	BTIC6A_EncodeGetVqBlock4(ctx, 0xFFFF, blk1_y, blk1_c);
	d=BTIC6A_EncodeCompareBlockNErrorYUV(
		blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv, 2);
	if((d/16)<(ctx->ld_flat_sq*BTIC6A_ERRSC_4X))
	{
		if(	ctx->lct4 &&
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

	BTIC6A_EncodeBlockSubsample4to2YUV(ctx, blk_y, blk_c, blk0a_y, blk0a_c);
	blk0b_y[0]=blk0a_y[0]-icy;
	blk0b_y[1]=blk0a_y[1]-blk0a_y[0];
	blk0b_y[2]=blk0a_y[2]-blk0a_y[1];
	blk0b_y[3]=blk0a_y[3]-blk0a_y[2];
	blk0b_c[0]=blk0a_c[0]-icu;
	blk0b_c[1]=blk0a_c[1]-icv;

	hash2=BTIC6A_EncodeCalcBlockNHashYUV(ctx, blk0b_y, blk0b_c, 0);

	memset(blk1_y, 0, 16);	memset(blk1_c, 0, 8);
	blk1_y[ 0]=blk0b_y[0];	blk1_y[ 4]=blk0b_y[1];
	blk1_y[ 8]=blk0b_y[2];	blk1_y[12]=blk0b_y[3];
	blk1_c[0]=blk0b_c[0];	blk1_c[1]=blk0b_c[1];

	ena_4x4=1;
	ena_2x2=0;
	ena_2x2b=0;
	d=BTIC6A_EncodeCompareBlockNErrorYUV(
		blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv, 2);
	if((d/16)<=(ctx->ld_skip_sq*BTIC6A_ERRSC_4X*1.125))
	{
		ena_2x2=1;
		if((d/16)<=(ctx->ld_skip_sq*BTIC6A_ERRSC_4X*0.875))
			{ ena_4x4=0; }
	}

	if((d/16)<=(ctx->ld_skip_sql2))
	{
		ena_2x2b=1;
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
		
//		if(!(ix2&0x4000) && ena_2x2)
//			{ d*=2; }
//		if((ix2&0x4000) && !ena_2x2)
//			{ d*=2; }
		
		if(d<bd)
		{
			bi=i; bd=d;
//			if(((bd/8)<=ctx->ld_skip_sql3))
//				break;
		}
	}

	if((bi>=0) && ((bd/16)<=(ctx->ld_skip_sql2)))
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

		if((ix2&0xC000)==0x4000)
			BTIC6A_EncBoostBlock2Index(ctx, ix2);

		BTIC6A_EncodeCalcBlockNStepYUV(
			blk1_y, blk1_c, icy, icu, icv, &cy, &cu, &cv, 2);
		ctx->clr_y=cy;		ctx->clr_u=cu;		ctx->clr_v=cv;
		return(ix2);
	}
#endif

#if 1
	if(ena_2x2)
	{
		bi=-1; bd=3*256*256*256;

		ix2=ctx->hash_2x2[hash2];
		ix2|=0x4000;
		for(i=0; i<BTIC6A_CBSZ_2X_H4; i++)
		{
			if(ix2&0x8000)
				break;
			k=BTIC6A_EncodeGetVqBlock2(ctx,
				ix2, blk1_y, blk1_c);
			if(k<0)		break;

			d=BTIC6A_EncodeCompareBlock2ErrorYUV(
				blk0b_y, blk0b_c, blk1_y, blk1_c, icy, icu, icv);
			if(d<0)		break;
			j=(ctx->rov_2x2-ix2-1)&0x3FFF;
			if(d<bd && (j<4096))
			{
				bi=j; bd=d;
				if(((bd/2)<=ctx->ld_skip_sql2))
					break;
			}
			ix2=ctx->hcn_2x2[ix2&BTIC6A_CBMSK_2X];
			ix2|=0x4000;
			k=(ctx->rov_2x2-ix2-1)&0x3FFF;
			if(k<=j)
				break;
		}

//		if((bd/16)>ctx->ld_skip_sql2)
		if((bd/2)>ctx->ld_skip_sql2)
		{
			for(i=0; i<BTIC6A_CBSZ_2X_L4; i++)
			{
				ix2=(ctx->rov_2x2-(i+1))&0x3FFF;
				ix2|=0x4000;
				if(ctx->flg_2x2[ix2&BTIC6A_CBMSK_2X]&1)
					continue;
				k=BTIC6A_EncodeGetVqBlock2(ctx, ix2, blk1_y, blk1_c);
				if(k<0)
					{ ctx->flg_2x2[ix2&BTIC6A_CBMSK_2X]|=1; continue; }
				d=BTIC6A_EncodeCompareBlock2ErrorYUV(
					blk0b_y, blk0b_c, blk1_y, blk1_c, icy, icu, icv);
				if(d<bd)
				{
					bi=i; bd=d;
					if(((bd/2)<=ctx->ld_skip_sql2))
						break;
				}
			}
		}

//		if((bi>=0) && ((bd/16)<=(ctx->ld_skip_sql2)))
		if((bi>=0) && ((bd/4)<=(ctx->ld_skip_sql2)))
		{
			ix2=(ctx->rov_2x2-(bi+1))&0x3FFF;
			ix2|=0x4000;
		
			BTIC6A_EncodeGetVqBlock4(ctx, ix2, blk1_y, blk1_c);

			ctx->bytes_cb_4x4_idx2+=2;

			ctx->lct=ctx->ct;
			ctx->lct4=ctx->ct;

			*ctx->ct++=0x0D|(bi<<4);
			*ctx->ct++=(bi>>4)&255;

			ctx->strov_4x4--;
			ctx->stf_4x4[ctx->strov_4x4&127]=ix2;

			BTIC6A_EncBoostBlock2Index(ctx, ix2);

			BTIC6A_EncodeCalcBlockNStepYUV(
				blk1_y, blk1_c, icy, icu, icv, &cy, &cu, &cv, 2);
			ctx->clr_y=cy;		ctx->clr_u=cu;		ctx->clr_v=cv;
			return(ix2);
		}
	}
#endif

#if 1
//	if(!ena_2x2)
	if(ena_4x4)
	{
		bi=-1; bd=3*256*256*256;

		ix2=ctx->hash_4x4[hash4];
		for(i=0; i<BTIC6A_CBSZ_4X; i++)
		{
			if(ix2&0x8000)
				break;
			k=BTIC6A_EncodeGetVqBlock4(ctx,
				ix2, blk1_y, blk1_c);
			if(k<0)		break;

			d=BTIC6A_EncodeCompareBlockNErrorYUV(
				blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv, 2);
			if(d<0)		break;
			j=(ctx->rov_4x4-ix2-1)&0x3FFF;
			if((d<bd) && (j<4096))
			{
				bi=j; bd=d;
				if(((bd/8)<=ctx->ld_skip_sql2))
					break;
			}
			ix2=ctx->hcn_4x4[ix2&BTIC6A_CBMSK_4X];
			k=(ctx->rov_4x4-ix2-1)&0x3FFF;
			if(k<=j)
				break;
		}

		if((bd/16)>ctx->ld_skip_sql2)
		{
			for(i=0; i<BTIC6A_CBSZ_4X; i++)
			{
				ix2=(ctx->rov_4x4-(i+1))&0x3FFF;
				if(ctx->flg_4x4[ix2&BTIC6A_CBMSK_4X]&1)
					continue;
				k=BTIC6A_EncodeGetVqBlock4(ctx, ix2, blk1_y, blk1_c);
				if(k<0)
					{ ctx->flg_4x4[ix2&BTIC6A_CBMSK_4X]|=1; continue; }
				d=BTIC6A_EncodeCompareBlockNErrorYUV(
					blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv, 2);
				if(d<bd)
				{
					bi=i; bd=d;
					if(((bd/8)<=ctx->ld_skip_sql2))
						break;
				}
			}
		}

		if((bi>=0) && ((bd/16)<=(ctx->ld_skip_sql2)))
		{
			ix2=(ctx->rov_4x4-(bi+1))&0x3FFF;
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

		if((bi>=0) && ((bd/16)<=(ctx->ld_skip_sql2)))
		{
			ix2=(ctx->rov_4x4-(bi+1))&0x3FFF;
		
			BTIC6A_EncodeGetVqBlock4(ctx, ix2, blk1_y, blk1_c);

			ctx->bytes_cb_4x4_idx2+=2;

			ctx->lct=ctx->ct;
			ctx->lct4=ctx->ct;

			if(BTIC6A_CheckStaleBlock4Index(ctx, ix2))
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
			return(ix2);
		}
	}
#endif

#if 1
//	if(ena_2x2)
	if(ena_2x2b)
	{
		if(	ctx->lct4 &&
			(((*ctx->lct4)&0x3F)==0x0F) &&
			(((*ctx->lct4)&0xC0)< 0xC0) )
		{
			*ctx->lct4=(*ctx->lct4)+0x40;
		}else
		{
			ctx->lct=ctx->ct;
			ctx->lct4=ctx->ct;
			*ctx->ct++=0x0F;
			ctx->bytes_cb_4x4_tag++;
		}
		ix4=BTIC6A_EncodeBlock2(ctx, blk0a_y, blk0a_c);
		ix4|=0x4000;
		return(ix4);
	}
#endif

	if(	ctx->lct4 &&
		(((*ctx->lct4)&0x1F)==0x07) &&
		(((*ctx->lct4)&0xE0)< 0xE0) )
	{
		*ctx->lct4=(*ctx->lct4)+0x20;
	}else
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
	byte blk0a_y[4], blk0a_c[2];
	byte blk0b_y[4], blk0b_c[2];
	byte blk1a_y[16], blk1a_c[8];
	u16 tidxi[4], tidxj[4];
	u16 ictab[4];
	u16 *ici, *icj;
	int bi, bd, d, bj, bi_dorfr, ix_dorfr, hash8, hash2, ena_2x2;
	int ix0, ix1, ix2, ix3, ix4, ix5, ix6, ix7;
	int cy, cu, cv, icy, icu, icv;
	int i, j, k;

	icy=ctx->clr_y;		icu=ctx->clr_u;		icv=ctx->clr_v;
	BTIC6A_EncodeBlock8DeltaYUV(blk_y, blk_c, blk0_y, blk0_c, icy, icu, icv);

	hash8=BTIC6A_EncodeCalcBlockNHashYUV(ctx, blk0_y, blk0_c, 4);

	BTIC6A_EncodeGetVqBlock8(ctx, 0xFFFF, blk1_y, blk1_c);
	d=BTIC6A_EncodeCompareBlockNErrorYUV(
		blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv, 4);
//	if((d/64)<ctx->ld_flat_sq)
//	if((d/64)<(ctx->ld_flat_sq*2))
	if((d/64)<(ctx->ld_flat_sq*BTIC6A_ERRSC_8X))
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

	ena_2x2=0;

	BTIC6A_EncodeBlockSubsample8to2YUV(ctx, blk_y, blk_c, blk0a_y, blk0a_c);
	blk0b_y[0]=blk0a_y[0]-icy;
	blk0b_y[1]=blk0a_y[1]-blk0a_y[0];
	blk0b_y[2]=blk0a_y[2]-blk0a_y[1];
	blk0b_y[3]=blk0a_y[3]-blk0a_y[2];
	blk0b_c[0]=blk0a_c[0]-icu;
	blk0b_c[1]=blk0a_c[1]-icv;

	hash2=BTIC6A_EncodeCalcBlockNHashYUV(ctx, blk0b_y, blk0b_c, 0);

	memset(blk1_y, 0, 64);	memset(blk1_c, 0, 32);
	blk1_y[ 0]=blk0b_y[0];	blk1_y[16]=blk0b_y[1];
	blk1_y[32]=blk0b_y[2];	blk1_y[48]=blk0b_y[3];
	blk1_c[0]=blk0b_c[0];	blk1_c[1]=blk0b_c[1];

	d=BTIC6A_EncodeCompareBlockNErrorYUV(
		blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv, 4);
	if((d/64)<=(ctx->ld_skip_sql1))
	{
		ena_2x2=1;
	}

//	ena_2x2=0;

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
			if(((bd/32)<=ctx->ld_skip_sql1))
				break;
		}
	}

	if((bi>=0) && ((bd/64)<=(ctx->ld_skip_sql1)))
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

		if((ix2&0xC000)==0x4000)
			BTIC6A_EncBoostBlock2Index(ctx, ix2);

		BTIC6A_EncodeCalcBlockNStepYUV(
			blk1_y, blk1_c, icy, icu, icv, &cy, &cu, &cv, 4);
		ctx->clr_y=cy;		ctx->clr_u=cu;		ctx->clr_v=cv;
		return(ix2);
	}
#endif

#if 1
	if(ena_2x2)
	{
		bi=-1; bd=3*256*256*256;

		ix2=ctx->hash_2x2[hash2];
		ix2|=0x4000;
		for(i=0; i<BTIC6A_CBSZ_2X_H8; i++)
		{
			if(ix2&0x8000)
				break;
//			k=BTIC6A_EncodeGetVqBlock8(ctx,
//				ix2, blk1_y, blk1_c);
			k=BTIC6A_EncodeGetVqBlock2(ctx,
				ix2, blk1_y, blk1_c);
			if(k<0)		break;

//			d=BTIC6A_EncodeCompareBlockNErrorYUV(
//				blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv, 4);
			d=BTIC6A_EncodeCompareBlock2ErrorYUV(
				blk0b_y, blk0b_c, blk1_y, blk1_c, icy, icu, icv);
			if(d<0)		break;
			j=(ctx->rov_2x2-ix2-1)&0x3FFF;
			if(d<bd && (j<4096))
			{
				bi=j; bd=d;
//				if(((bd/32)<=ctx->ld_skip_sql1))
				if(((bd/2)<=ctx->ld_skip_sql1))
					break;
			}
			ix2=ctx->hcn_2x2[ix2&BTIC6A_CBMSK_2X];
			ix2|=0x4000;
			k=(ctx->rov_2x2-ix2-1)&0x3FFF;
			if(k<=j)
				break;
		}

//		if((bd/32)>ctx->ld_skip_sql1)
		if((bd/2)>ctx->ld_skip_sql1)
		{
			for(i=0; i<BTIC6A_CBSZ_2X_L8; i++)
			{
				ix2=(ctx->rov_2x2-(i+1))&0x3FFF;
				ix2|=0x4000;
				if(ctx->flg_2x2[ix2&BTIC6A_CBMSK_2X]&1)
					continue;
//				k=BTIC6A_EncodeGetVqBlock8(ctx, ix2, blk1_y, blk1_c);
				k=BTIC6A_EncodeGetVqBlock2(ctx, ix2, blk1_y, blk1_c);
				if(k<0)
					{ ctx->flg_2x2[ix2&BTIC6A_CBMSK_2X]|=1; continue; }
//				d=BTIC6A_EncodeCompareBlockNErrorYUV(
//					blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv, 4);
				d=BTIC6A_EncodeCompareBlock2ErrorYUV(
					blk0b_y, blk0b_c, blk1_y, blk1_c, icy, icu, icv);
				if(d<bd)
				{
					bi=i; bd=d;
//					if(((bd/32)<=ctx->ld_skip_sql1))
					if(((bd/2)<=ctx->ld_skip_sql1))
						break;
				}
			}
		}

	//	if((bi>=0) && ((bd/64)<=(ctx->ld_skip_sql1)))
		if((bi>=0) && ((bd/4)<=(ctx->ld_skip_sql1)))
//		if(0)
		{
			ix2=(ctx->rov_2x2-(bi+1))&0x3FFF;
			ix2|=0x4000;
		
			BTIC6A_EncodeGetVqBlock8(ctx, ix2, blk1_y, blk1_c);
//			BTIC6A_EncodeGetVqBlock2(ctx, ix2, blk1a_y, blk1a_c);

			ctx->bytes_cb_8x8_idx2+=2;

			ctx->lct=ctx->ct;
			ctx->lct8=ctx->ct;

			*ctx->ct++=0x0D|(bi<<4);
			*ctx->ct++=(bi>>4)&255;

			ctx->strov_8x8--;
			ctx->stf_8x8[ctx->strov_8x8&127]=(u16)ix2;

			BTIC6A_EncBoostBlock2Index(ctx, ix2);

			BTIC6A_EncodeCalcBlockNStepYUV(
				blk1_y, blk1_c, icy, icu, icv, &cy, &cu, &cv, 4);
//			BTIC6A_EncodeCalcBlockNStepYUV(
//				blk1a_y, blk1a_c, icy, icu, icv, &cy, &cu, &cv, 0);
			ctx->clr_y=cy;		ctx->clr_u=cu;		ctx->clr_v=cv;
			return(ix2);
		}
	}
#endif

	bi=-1; bd=3*256*256*256;
#if 1
	if(!ena_2x2)
	{
		ix2=ctx->hash_8x8[hash8];
		for(i=0; i<BTIC6A_CBSZ_8X; i++)
		{
			if(ix2&0x8000)
				break;
			k=BTIC6A_EncodeGetVqBlock8(ctx,
				ix2, blk1_y, blk1_c);
			if(k<0)		break;

			d=BTIC6A_EncodeCompareBlockNErrorYUV(
				blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv, 4);
			if(d<0)		break;
			j=(ctx->rov_8x8-ix2-1)&0x3FFF;
			if(d<bd && (j<4096))
			{
				bi=j; bd=d;
				if(((bd/32)<=ctx->ld_skip_sql1))
					break;
			}
			ix2=ctx->hcn_8x8[ix2&BTIC6A_CBMSK_8X];
			k=(ctx->rov_8x8-ix2-1)&0x3FFF;
			if(k<=j)
				break;
		}

		if((bd/32)>ctx->ld_skip_sql1)
		{
			for(i=0; i<BTIC6A_CBSZ_8X; i++)
			{
				ix2=(ctx->rov_8x8-(i+1))&0x3FFF;
				if(ctx->flg_8x8[ix2&BTIC6A_CBMSK_8X]&1)
					continue;
				k=BTIC6A_EncodeGetVqBlock8(ctx, ix2, blk1_y, blk1_c);
				if(k<0)
					{ ctx->flg_8x8[ix2&BTIC6A_CBMSK_8X]|=1; continue; }
				d=BTIC6A_EncodeCompareBlockNErrorYUV(
					blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv, 4);
				if(i>64)
//				if(i>128)
					d*=2;
				if(d<bd)
				{
					bi=i; bd=d;
					if(((bd/32)<=ctx->ld_skip_sql1))
						break;
				}
			}
		}
	}
#endif

	ix_dorfr=-1;
	bi_dorfr=0;
	if((bi>=0) && ((bd/64)<=(ctx->ld_skip_sql1)))
	{
		ix2=(ctx->rov_8x8-(bi+1))&0x3FFF;
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

	if((bi>=0) && ((bd/64)<=(ctx->ld_skip_sql1)))
	{
		ix2=(ctx->rov_8x8-(bi+1))&0x3FFF;
		BTIC6A_EncodeGetVqBlock8(ctx, ix2, blk1_y, blk1_c);

		ctx->bytes_cb_8x8_idx2+=2;

		ctx->lct=ctx->ct;
		ctx->lct8=ctx->ct;

		if(bi_dorfr)
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

#if 1
	if(ena_2x2)
	{
		if(	ctx->lct8 &&
			(((*ctx->lct8)&0x3F)==0x0F) &&
			(((*ctx->lct8)&0xC0)< 0xC0) )
		{
			*ctx->lct8=(*ctx->lct8)+0x40;
		}else
		{
			ctx->lct=ctx->ct;
			ctx->lct8=ctx->ct;
			*ctx->ct++=0x0F;
			ctx->bytes_cb_8x8_tag++;
		}
		ix4=BTIC6A_EncodeBlock2(ctx, blk0a_y, blk0a_c);
		ix4|=0x4000;
		return(ix4);
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
	byte blk0_y[256], blk0_c[128];
	byte blk1_y[256], blk1_c[128];
	byte blk1a_y[256], blk1a_c[128];
	byte blk0a_y[4], blk0a_c[2];
	byte blk0b_y[4], blk0b_c[2];
	u16 ictab[4];
	u16 *ici;
	int bi, bd, d, hash16, hash2, ena_2x2;
	int ix0, ix1, ix2, ix3, ix4, ix5, ix6, ix7;
	int cy, cu, cv, icy, icu, icv;
	int i, j, k;

	icy=ctx->clr_y;		icu=ctx->clr_u;		icv=ctx->clr_v;
	BTIC6A_EncodeBlock16DeltaYUV(blk_y, blk_c, blk0_y, blk0_c, icy, icu, icv);

	hash16=BTIC6A_EncodeCalcBlockNHashYUV(ctx, blk0_y, blk0_c, 6);

	BTIC6A_EncodeGetVqBlock16(ctx, 0xFFFF, blk1_y, blk1_c);
	d=BTIC6A_EncodeCompareBlockNErrorYUV(
		blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv, 6);
//	if((d/256)<ctx->ld_flat_sq)
	if((d/256)<(ctx->ld_flat_sq*BTIC6A_ERRSC_16X))
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

	ena_2x2=0;

	BTIC6A_EncodeBlockSubsample16to2YUV(ctx, blk_y, blk_c, blk0a_y, blk0a_c);
	blk0b_y[0]=blk0a_y[0]-icy;
	blk0b_y[1]=blk0a_y[1]-blk0a_y[0];
	blk0b_y[2]=blk0a_y[2]-blk0a_y[1];
	blk0b_y[3]=blk0a_y[3]-blk0a_y[2];
	blk0b_c[0]=blk0a_c[0]-icu;
	blk0b_c[1]=blk0a_c[1]-icv;

	hash2=BTIC6A_EncodeCalcBlockNHashYUV(ctx, blk0b_y, blk0b_c, 0);

	memset(blk1_y, 0, 256);
	memset(blk1_c, 0, 128);
	blk1_y[  0]=blk0b_y[0];	blk1_y[ 64]=blk0b_y[1];
	blk1_y[128]=blk0b_y[2];	blk1_y[192]=blk0b_y[3];
	blk1_c[0]=blk0b_c[0];	blk1_c[1]=blk0b_c[1];

	d=BTIC6A_EncodeCompareBlockNErrorYUV(
		blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv, 6);
	if((d/256)<=(ctx->ld_skip_sql0))
	{
		ena_2x2=1;
	}

//	ena_2x2=0;

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
			if(((bd/128)<=ctx->ld_skip_sql0))
				break;
		}
	}

	if((bi>=0) && ((bd/256)<=(ctx->ld_skip_sql0)))
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

		if((ix2&0xC000)==0x4000)
			BTIC6A_EncBoostBlock2Index(ctx, ix2);

		BTIC6A_EncodeCalcBlockNStepYUV(
			blk1_y, blk1_c, icy, icu, icv, &cy, &cu, &cv, 6);
		ctx->clr_y=cy;		ctx->clr_u=cu;		ctx->clr_v=cv;
		return(ix2);
	}
#endif

#if 1
	bi=-1; bd=3*256*256*256;

	if(ena_2x2)
	{
		ix2=ctx->hash_2x2[hash2];
		ix2|=0x4000;
		for(i=0; i<BTIC6A_CBSZ_2X_H16; i++)
		{
			if(k&0x8000)
				break;
			if(ctx->flg_2x2[ix2&BTIC6A_CBMSK_2X]&1)
				break;
//			k=BTIC6A_EncodeGetVqBlock16(ctx,
//				ix2, blk1_y, blk1_c);
			k=BTIC6A_EncodeGetVqBlock2(ctx,
				ix2, blk1_y, blk1_c);
			if(k<0)		break;

//			d=BTIC6A_EncodeCompareBlockNErrorYUV(
//				blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv, 6);
			d=BTIC6A_EncodeCompareBlock2ErrorYUV(
				blk0b_y, blk0b_c, blk1_y, blk1_c, icy, icu, icv);
			if(d<0)		break;
			j=(ctx->rov_2x2-ix2-1)&0x3FFF;
			if(d<bd && (j<4096))
			{
				bi=j; bd=d;
//				if(((bd/128)<=ctx->ld_skip_sql0))
				if(((bd/2)<=ctx->ld_skip_sql0))
					break;
			}
			ix2=ctx->hcn_2x2[ix2&BTIC6A_CBMSK_2X];
			ix2|=0x4000;
			k=(ctx->rov_2x2-ix2-1)&0x3FFF;
			if(k<=j)
				break;
		}

//		if((bd/128)>ctx->ld_skip_sql0)
		if((bd/2)>ctx->ld_skip_sql0)
		{
			for(i=0; i<BTIC6A_CBSZ_2X_L16; i++)
			{
				ix2=(ctx->rov_2x2-(i+1))&0x3FFF;
				ix2|=0x4000;
				if(ctx->flg_2x2[ix2&BTIC6A_CBMSK_2X]&1)
					continue;
//				k=BTIC6A_EncodeGetVqBlock16(ctx, ix2, blk1_y, blk1_c);
				k=BTIC6A_EncodeGetVqBlock2(ctx, ix2, blk1_y, blk1_c);
				if(k<0)
					{ ctx->flg_2x2[ix2&BTIC6A_CBMSK_2X]|=1; continue; }
//				d=BTIC6A_EncodeCompareBlockNErrorYUV(
//					blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv, 6);
				d=BTIC6A_EncodeCompareBlock2ErrorYUV(
					blk0b_y, blk0b_c, blk1_y, blk1_c, icy, icu, icv);
				if(d<bd)
				{
					bi=i; bd=d;
//					if(((bd/128)<=ctx->ld_skip_sql0))
					if(((bd/2)<=ctx->ld_skip_sql0))
						break;
				}
			}
		}
	}

//	if((bi>=0) && ((bd/256)<=(ctx->ld_skip_sql0)))
	if((bi>=0) && ((bd/4)<=(ctx->ld_skip_sql0)))
	{
		ix2=(ctx->rov_2x2-(bi+1))&0x3FFF;
		ix2|=0x4000;
	
		BTIC6A_EncodeGetVqBlock16(ctx, ix2, blk1_y, blk1_c);

		ctx->bytes_cb_16x16_idx2+=2;

		ctx->lct=ctx->ct;
		ctx->lct16=ctx->ct;

		*ctx->ct++=0x0D|(bi<<4);
		*ctx->ct++=(bi>>4)&255;

		ctx->strov_16x16--;
		ctx->stf_16x16[ctx->strov_16x16&127]=ix2;

		BTIC6A_EncBoostBlock2Index(ctx, ix2);

		BTIC6A_EncodeCalcBlockNStepYUV(
			blk1_y, blk1_c, icy, icu, icv, &cy, &cu, &cv, 6);
		ctx->clr_y=cy;		ctx->clr_u=cu;		ctx->clr_v=cv;
		return(ix2);
	}
#endif

	bi=-1; bd=3*256*256*256;
#if 1
	if(!ena_2x2)
	{
		ix2=ctx->hash_16x16[hash16];
		for(i=0; i<BTIC6A_CBSZ_16X; i++)
		{
			if(ix2&0x8000)
				break;
			k=BTIC6A_EncodeGetVqBlock16(ctx,
				ix2, blk1_y, blk1_c);
			if(k<0)		break;
			d=BTIC6A_EncodeCompareBlockNErrorYUV(
				blk0_y, blk0_c, blk1_y, blk1_c, icy, icu, icv, 6);
			if(d<0)		break;
			j=(ctx->rov_16x16-ix2-1)&0x3FFF;
			if(d<bd && (j<4096))
			{
				bi=j; bd=d;
				if(((bd/128)<=ctx->ld_skip_sql0))
					break;
			}
			ix2=ctx->hcn_16x16[ix2&BTIC6A_CBMSK_16X];
			k=(ctx->rov_16x16-ix2-1)&0x3FFF;
			if(k<=j)
				break;
		}

		if((bd/128)>ctx->ld_skip_sql0)
		{
			for(i=0; i<BTIC6A_CBSZ_16X; i++)
			{
				ix2=(ctx->rov_16x16-(i+1))&0x3FFF;
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
					if(((bd/128)<=ctx->ld_skip_sql0))
						break;
				}
			}
		}
	}
#endif

#if 1
	if((bi>=0) && ((bd/256)<=(ctx->ld_skip_sql0)))
	{
		ix2=(ctx->rov_16x16-(bi+1))&0x3FFF;
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

//	if((bi>=0) && ((bd/256)<=ctx->ld_skip_sql0))
	if((bi>=0) && ((bd/256)<=(ctx->ld_skip_sql0)))
	{
		ix2=(ctx->rov_16x16-(bi+1))&0x3FFF;

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


#if 1
	if(ena_2x2)
	{
		if(	ctx->lct16 &&
			(((*ctx->lct16)&0x3F)==0x0F) &&
			(((*ctx->lct16)&0xC0)< 0xC0) )
		{
			*ctx->lct16=(*ctx->lct16)+0x40;
		}else
		{
			ctx->lct=ctx->ct;
			ctx->lct16=ctx->ct;
			*ctx->ct++=0x0F;
			ctx->bytes_cb_16x16_tag++;
		}
		ix4=BTIC6A_EncodeBlock2(ctx, blk0a_y, blk0a_c);
		ix4|=0x4000;
		return(ix4);
	}
#endif

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
	static const int ofst[32]={
		1,0,   0,1,  -1, 0,  0,-1,
		1,1,  -1,1,  -1,-1,  1,-1,
		2,0,   0,2,  -2, 0,  0,-2,
		3,0,   0,3,  -3, 0,  0,-3 };
	u64 chkmask[64];
	byte blk0_y[256];
	byte blk0_c[128];
	byte blk1_y[256];
	byte blk1_c[128];
	int tg;
	int d0, d1, d2, dz, imx, imy, mx, my, tmx, tmy, t2mx, t2my, t2d, n;
	int bx, by, bxs, bys;
	int i, j, k;

	bx=ctx->c_x*16;
	by=ctx->c_y*16;
	bxs=ctx->bxs;
	bys=ctx->bys;

	BTIC6A_EncodeGetFrameMacroblockPixelsHilbertYUV(
		ctx->cimg, ctx->bxs, ctx->bys,
		blk0_y, blk0_c,
		ctx->c_x*16, ctx->c_y*16);

	if(ctx->is_pframe)
	{
		imx=ctx->mv_x;	imy=ctx->mv_y;
		BTIC6A_EncodeGetFrameMacroblockPixelsHilbertYUV(
			ctx->pimg, ctx->bxs, ctx->bys,
			blk1_y, blk1_c, ctx->c_x*16+imx, ctx->c_y*16+imy);
		d0=BTIC6A_EncodeCompareBlockAbsNErrorYUV(
			blk0_y, blk0_c, blk1_y, blk1_c, 6);
		d0*=sqrt(sqrt(sqrt(imx*imx+imy*imy+1)));

		d1=d0;

		for(i=0; i<64; i++)
			chkmask[i]=0;

		if((imx!=0) || (imy!=0))
		{
			BTIC6A_EncodeGetFrameMacroblockPixelsHilbertYUV(
				ctx->pimg, ctx->bxs, ctx->bys,
				blk1_y, blk1_c, ctx->c_x*16+0, ctx->c_y*16+0);
			d1=BTIC6A_EncodeCompareBlockAbsNErrorYUV(
				blk0_y, blk0_c, blk1_y, blk1_c, 6);
		}
		
		dz=d1;

		mx=imx; my=imy; d2=d0;
		if(d1<d0)
			{ mx=0; my=0; d2=d1; }

		if(!ctx->c_x && (mx<0))		mx=0;
		if(!ctx->c_y && (my<0))		my=0;
		if((ctx->c_x==(ctx->cxs-1)) && (mx>0))		mx=0;
		if((ctx->c_y==(ctx->cys-1)) && (my>0))		my=0;

		chkmask[my+32]|=1ULL<<(mx+32);

#if 1
		n=64;
		while((n--)>0)
		{
			t2d=d2; t2mx=mx; t2my=my;
//			for(i=0; i<8; i++)
			for(i=0; i<16; i++)
			{
				tmx=mx+ofst[i*2+0];
				tmy=my+ofst[i*2+1];
				if(tmx<(-31))		continue;
				if(tmy<(-31))		continue;
				if(tmx>( 31))		continue;
				if(tmy>( 31))		continue;

				if((bx+tmx)<0)			continue;
				if((by+tmy)<0)			continue;
				if(((bx+tmx+16))>bxs)	continue;
				if(((by+tmy+16))>bys)	continue;

				/* avoid looping around */
				if(chkmask[tmy+32]&(1ULL<<(mx+32)))
					continue;
				chkmask[tmy+32]|=1ULL<<(tmx+32);

				BTIC6A_EncodeGetFrameMacroblockPixelsHilbertYUV(
					ctx->pimg, ctx->bxs, ctx->bys,
					blk1_y, blk1_c, ctx->c_x*16+tmx, ctx->c_y*16+tmy);
				d1=BTIC6A_EncodeCompareBlockAbsNErrorYUV(
					blk0_y, blk0_c, blk1_y, blk1_c, 6);
				d1*=sqrt(sqrt(sqrt(tmx*tmx+tmy*tmy+1)));
				if(d1<0)
					continue;
				if(d1<t2d)
					{ t2mx=tmx; t2my=tmy; t2d=d1; }
			}
			if(t2d<d2)
				{ mx=t2mx; my=t2my; d2=t2d; continue; }
			break;
		}
#endif

		if((d0/256)<=(ctx->ld_skip_sq>>2))
		{
			if((d0>>1)<d2)
				{ mx=imx; my=imy; }
		}

		if((dz/256)<=(ctx->ld_skip_sq>>2))
		{
			if((dz>>1)<d2)
				{ mx=0; my=0; }
		}
		
		if(!ctx->c_x && (mx<0))		mx=0;
		if(!ctx->c_y && (my<0))		my=0;
		if((ctx->c_x==(ctx->cxs-1)) && (mx>0))		mx=0;
		if((ctx->c_y==(ctx->cys-1)) && (my>0))		my=0;

//		if((d2/256)<=(ctx->ld_skip_sql0))
//		if((d2/256)<=(ctx->ld_skip_sq>>2))
//		if((d2/256)<=(ctx->ld_skip_sq>>1))
		if((d2/256)<=ctx->ld_skip_sql0)
//		if(0)
		{
			if((mx!=imx) || (my!=imy))
			{
				ctx->mv_x=mx;	ctx->mv_y=my;
				tg=0x0B|((my+32)<<10)|((mx+32)<<4);
				ctx->lct=ctx->ct;
				ctx->lct16=ctx->ct;
				*ctx->ct++=(tg>>0)&255;
				*ctx->ct++=(tg>>8)&255;
				ctx->bytes_cb_16x16_tag+=2;
			}
			
			if(	ctx->lct16 &&
				(((*ctx->lct16)&0x0F)==0x05) &&
				(((*ctx->lct16)&0xF0)< 0xF0) )
			{
				*ctx->lct16=(*ctx->lct16)+0x10;
			}else
			{
				ctx->lct=ctx->ct;
				ctx->lct16=ctx->ct;
				*ctx->ct++=0x05;
				ctx->bytes_cb_16x16_tag++;
			}
			return(0);
		}
	}

	BTIC6A_EncodeBlock16(ctx, blk0_y, blk0_c);
	return(0);
}


void BTIC6A_EncodeCopyFrameClrs(BTIC6A_EncodeContext *ctx,
	byte *oimg, int clrs)
{
	byte *cimg;
	byte *ycs, *yct, *cs, *ct;
	int cr, cg, cb;
	int x, y, xs, ys, xsr, ysr, iystr, oystr, ystr2;
	int ocn, rgflip, px;
	
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

	if(clrs==BTIC4B_CLRS_RGB555)
	{
		oystr=xsr*2;

		ycs=cimg;	yct=oimg;
		for(y=0; y<ysr; y++)
		{
			cs=ycs;		ct=yct;
			for(x=0; x<xsr; x++)
			{
				px=btpic_getu16(ct);
				cr=(px>>10)&31;		cg=(px>> 5)&31;		cb=(px>> 0)&31;
				cr=(cr<<3)|(cr>>2);	cg=(cg<<3)|(cg>>2);	cb=(cb<<3)|(cb>>2);
				cs[0]=cb;	cs[1]=cg;
				cs[2]=cr;	cs[3]=255;
				cs+=4; ct+=2;
			}
			if(xsr<xs)
			{
				cs=cimg+y*iystr+(xsr-1)*4;
				ct=cimg+y*iystr+xsr*4;
				for(x=xsr; x<xs; x++)
					{ memcpy(ct, cs, 4); ct+=4; cs+=4; }
			}
			ycs+=iystr;
			yct+=oystr;
		}
		for(y=ysr; y<ys; y++)
			{ memcpy(cimg+y*iystr, cimg+(ysr-1)*iystr, iystr); }
		return;
	}

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
		{
			memcpy(cimg+y*iystr, oimg+y*oystr, ystr2);
			if(xsr<xs)
			{
				cs=oimg+y*oystr+(xsr-1)*4;
				ct=cimg+y*iystr+xsr*4;
				for(x=xsr; x<xs; x++)
					{ memcpy(ct, cs, 4); ct+=4; cs+=4; }
			}
		}
		for(y=ysr; y<ys; y++)
			{ memcpy(cimg+y*iystr, cimg+(ysr-1)*iystr, iystr); }
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
			else
				cs[3]=255;
			cs+=4; ct+=ocn;
		}
		if(xsr<xs)
		{
			ct-=ocn;
			for(x=xsr; x<xs; x++)
			{
				cs[0]=ct[0^rgflip];
				cs[1]=ct[1];
				cs[2]=ct[2^rgflip];
				if(ocn==4)
					cs[3]=ct[3];
				else
					cs[3]=255;
				cs+=4;
			}
		}
		ycs+=iystr;
		yct+=oystr;
	}
	for(y=ysr; y<ys; y++)
		{ memcpy(cimg+y*iystr, cimg+(ysr-1)*iystr, iystr); }
}

int BTIC6A_EncodeFrameProlog(BTIC6A_EncodeContext *ctx)
{
	u16 keybuf[1536];
	byte *ici_y, *ici_c;
	byte blk_y[4], blk_c[2];
	int nkbuf;
	int cy, cu, cv;
	int i, j, k, l, n, z, ki;

	nkbuf=BTIC6A_EncStaticGetTopBlock2Keys(ctx, keybuf, 1024);
	
	if(nkbuf<256)
	{
		return(0);
	}
	
	n=nkbuf>>6;
	for(l=0; l<n; l++)
	{
		ki=keybuf[l*64];
		if(ctx->scnt_2x2[ki]<16)
			break;

		*ctx->ct++=0x07;	//16x16
		*ctx->ct++=0x67;	//4x 8x8
		for(i=0; i<4; i++)
		{
			*ctx->ct++=0x67;	//4x 4x4
			for(j=0; j<4; j++)
			{
				*ctx->ct++=0xC7;	//4x 2x2 6x8b
				for(k=0; k<4; k++)
				{
					//2x2
					z=l*64+i*16+j*4+k;
					ki=keybuf[z];
					ici_y=ctx->scb_2x2_y+(ki*4);
					ici_c=ctx->scb_2x2_c+(ki*2);
					*ctx->ct++=ici_y[0];	*ctx->ct++=ici_y[1];
					*ctx->ct++=ici_y[2];	*ctx->ct++=ici_y[3];
					*ctx->ct++=ici_c[0];	*ctx->ct++=ici_c[1];
				}
			}
		}
	}
	return(0);
}

int BTIC6A_EncodeApplyFrameProlog_L3(BTIC6A_EncodeContext *ctx, byte **rcs)
{
	byte *ici_y, *ici_c;
	byte *cs;
	int ix4, tg;
	
	cs=*rcs;

	if(ctx->acc_dcom_l3)
	{
		ctx->acc_dcom_l3--;
		ici_y=cs+0;
		ici_c=cs+4;
		cs+=6;
		ix4=BTIC6A_EncAddBlock2Index(ctx, ici_y, ici_c);
		*rcs=cs;
		return(ix4);
	}

	tg=*cs;
	if((tg&0x3F)==0x07)
	{
		ctx->acc_dcom_l3=((tg>>6)&3)+1;
		cs++;
		ix4=BTIC6A_EncodeApplyFrameProlog_L3(ctx, &cs);
		*rcs=cs;
		return(ix4);
	}

	__debugbreak();
	return(-1);
}

int BTIC6A_EncodeApplyFrameProlog_L2(BTIC6A_EncodeContext *ctx, byte **rcs)
{
	u16 icta[4];
	byte *cs;
	int tg, ix2;
	
	cs=*rcs;
	if(ctx->acc_dcom_l2)
	{
		ctx->acc_dcom_l2--;
		icta[0]=BTIC6A_EncodeApplyFrameProlog_L3(ctx, &cs);
		icta[1]=BTIC6A_EncodeApplyFrameProlog_L3(ctx, &cs);
		icta[2]=BTIC6A_EncodeApplyFrameProlog_L3(ctx, &cs);
		icta[3]=BTIC6A_EncodeApplyFrameProlog_L3(ctx, &cs);
		ix2=BTIC6A_EncAddBlock4Index(ctx, icta);
		*rcs=cs;
		return(ix2);
	}

	tg=*cs;
	if((tg&0x1F)==0x07)
	{
		ctx->acc_dcom_l2=((tg>>5)&7)+1;
		cs++;
		ix2=BTIC6A_EncodeApplyFrameProlog_L2(ctx, &cs);
		*rcs=cs;
		return(ix2);
	}
	__debugbreak();
	return(-1);
}

int BTIC6A_EncodeApplyFrameProlog_L1(BTIC6A_EncodeContext *ctx, byte **rcs)
{
	u16 icta[4];
	byte *cs;
	int tg, ix2;
	
	cs=*rcs;
	if(ctx->acc_dcom_l1)
	{
		ctx->acc_dcom_l1--;
		icta[0]=BTIC6A_EncodeApplyFrameProlog_L2(ctx, &cs);
		icta[1]=BTIC6A_EncodeApplyFrameProlog_L2(ctx, &cs);
		icta[2]=BTIC6A_EncodeApplyFrameProlog_L2(ctx, &cs);
		icta[3]=BTIC6A_EncodeApplyFrameProlog_L2(ctx, &cs);
		ix2=BTIC6A_EncAddBlock8Index(ctx, icta);
		*rcs=cs;
		return(ix2);
	}

	tg=*cs;
	if((tg&0x1F)==0x07)
	{
		ctx->acc_dcom_l1=((tg>>5)&7)+1;
		cs++;
		ix2=BTIC6A_EncodeApplyFrameProlog_L1(ctx, &cs);
		*rcs=cs;
		return(ix2);
	}
	__debugbreak();
	return(-1);
}

int BTIC6A_EncodeApplyFrameProlog_L0(BTIC6A_EncodeContext *ctx, byte **rcs)
{
	u16 icta[4];
	byte *cs;
	int tg, ix2;
	
	cs=*rcs;
	if(ctx->acc_dcom_l0)
	{
		ctx->acc_dcom_l0--;
		icta[0]=BTIC6A_EncodeApplyFrameProlog_L1(ctx, &cs);
		icta[1]=BTIC6A_EncodeApplyFrameProlog_L1(ctx, &cs);
		icta[2]=BTIC6A_EncodeApplyFrameProlog_L1(ctx, &cs);
		icta[3]=BTIC6A_EncodeApplyFrameProlog_L1(ctx, &cs);
		ix2=BTIC6A_EncAddBlock16Index(ctx, icta);
		*rcs=cs;
		return(ix2);
	}

	tg=*cs;
	if((tg&0x1F)==0x07)
	{
		ctx->acc_dcom_l0=((tg>>5)&7)+1;
		cs++;
		ix2=BTIC6A_EncodeApplyFrameProlog_L0(ctx, &cs);
		*rcs=cs;
		return(ix2);
	}
	__debugbreak();
	return(-1);
}

int BTIC6A_EncodeApplyFrameProlog(BTIC6A_EncodeContext *ctx)
{
	byte *cs, *cse;

	if(!ctx->ppbuf)
		return(0);
	if(ctx->ppsz<=0)
		return(0);
	
	cs=ctx->ppbuf;
	cse=cs+ctx->ppsz;
	while(cs<cse)
		{ BTIC6A_EncodeApplyFrameProlog_L0(ctx, &cs); }
	return(0);
}

int BTIC6A_EncodeFrameImage(BTIC6A_EncodeContext *ctx,
	byte *obuf, int obsz, void *ibuf, int qfl, int clrs)
{
	byte thbuf[64];
	byte *ct;
	int x, y, sz, osz, osz2;
	int i, j, k;

	BTIC6A_EncodeContextSetupQuality(ctx, qfl);
	BTIC6A_EncodeCopyFrameClrs(ctx, ibuf, clrs);
	
	ctx->clr_y=128;
	ctx->clr_u=128;
	ctx->clr_v=128;
	ctx->mv_x=0;
	ctx->mv_y=0;
	
	ctx->lct=NULL;
	ctx->lct2=NULL;
	ctx->lct4=NULL;
	ctx->lct8=NULL;
	ctx->lct16=NULL;

	ctx->rov_2x2=0;
	ctx->rov_4x4=0;
	ctx->rov_8x8=0;
	ctx->rov_16x16=0;

	for(i=0; i<256; i++)
	{
		ctx->hash_2x2  [i]=0xBFFF;
		ctx->hash_4x4  [i]=0xBFFF;
		ctx->hash_8x8  [i]=0xBFFF;
		ctx->hash_16x16[i]=0xBFFF;
	}
	
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
	
	memset(ctx->flg_2x2, 0xFF, BTIC6A_CBSZ_2X);
	memset(ctx->flg_4x4, 0xFF, BTIC6A_CBSZ_4X);
	memset(ctx->flg_8x8, 0xFF, BTIC6A_CBSZ_8X);
	memset(ctx->flg_16x16, 0xFF, BTIC6A_CBSZ_16X);
	
	ctx->is_pframe=0;
	if(qfl&BTIC4B_QFL_PFRAME)
		ctx->is_pframe=1;
	
	if(!ctx->is_pframe)
	{
		ctx->ct=ctx->tfbuf;
		BTIC6A_EncodeFrameProlog(ctx);
		sz=ctx->ct-ctx->tfbuf;

		ctx->ppsz=0;
		if(sz>0)
		{
			if(ctx->ppbuf && sz>ctx->ppbsz)
			{
				free(ctx->ppbuf);
				ctx->ppbuf=NULL;
			}
			
			if(!ctx->ppbuf)
			{
				k=4096;
				while(sz>=k)
					k=k+(k>>1);
				ctx->ppbuf=malloc(k);
				ctx->ppbsz=k;
			}
			memcpy(ctx->ppbuf, ctx->tfbuf, sz);
			ctx->ppsz=sz;
		}

		ctx->clr_y=128;
		ctx->clr_u=128;
		ctx->clr_v=128;
		ctx->mv_x=0;
		ctx->mv_y=0;
		
		ctx->lct=NULL;
		ctx->lct2=NULL;
		ctx->lct4=NULL;
		ctx->lct8=NULL;
		ctx->lct16=NULL;

		ctx->rov_2x2=0;
		ctx->rov_4x4=0;
		ctx->rov_8x8=0;
		ctx->rov_16x16=0;

	}
	
	BTIC6A_EncodeApplyFrameProlog(ctx);
	
	ctx->ct=ctx->tfbuf;
	for(y=0; y<ctx->cys; y++)
	{
		for(x=0; x<ctx->cxs; x++)
		{
			ctx->c_x=x;		ctx->c_y=y;
			BTIC6A_EncodeFrameMacroblock(ctx);
		}
	}
	
//	if(ctx->is_pframe)
	if(1)
	{
		memcpy(ctx->pimg, ctx->cimg, (ctx->bxs*ctx->bys)*4);
	}
	
	sz=ctx->ct-ctx->tfbuf;
	ct=obuf;
	
	if(ctx->is_pframe)
	{
		ct=BTIC6A_EncodeEmitTwoCC(ct, BTPIC_TCC_PX, ctx->tfbuf, sz);
		osz=ct-obuf;
	}else
	{
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
		ct=BTIC6A_EncodeEmitTwoCC(ct, BTPIC_TCC_HX, thbuf, 16);

		if(ctx->ppbuf && (ctx->ppsz>0))
		{
			ct=BTIC6A_EncodeEmitTwoCC(ct, BTPIC_TCC_PP,
				ctx->ppbuf, ctx->ppsz);
		}

		ct=BTIC6A_EncodeEmitTwoCC(ct, BTPIC_TCC_IX, ctx->tfbuf, sz);
		osz=ct-obuf;
	}

// #ifdef RP2POST_RP2AUTO_C
#if 0
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
	int qf, v;
	int i, j, k;

	if(!btic6a_abserr_tp)
	{
		btic6a_abserr_tp = btic6a_abserr_tab + 512;
		for(i=0; i<512; i++)
		{
	//		j=i+pow(i, 1.5)*0.25;
//			j=i+((i*i)>>2);
//			j=i+((i*i)>>3);
//			j=i+((i*i)>>4);
			j=i+((i*i)>>5);
//			j=i+((i*i)>>6);
			if(j>32767)
				j=32767;
			btic6a_abserr_tp[ i]=j;
			btic6a_abserr_tp[-i]=j;
		}
	}

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
	
	v=ctx->ld_flat;
	ctx->ld_flat_sq=bt6a_abserr(v);
	v=ctx->ld_skip;
	ctx->ld_skip_sq=bt6a_abserr(v);

	ctx->ld_flat_sql0=ctx->ld_flat_sq*BTIC6A_ERRSC_16X;
	ctx->ld_flat_sql1=ctx->ld_flat_sq*BTIC6A_ERRSC_8X;
	ctx->ld_flat_sql2=ctx->ld_flat_sq*BTIC6A_ERRSC_4X;
	ctx->ld_flat_sql3=ctx->ld_flat_sq*BTIC6A_ERRSC_2X;

	ctx->ld_skip_sql0=ctx->ld_skip_sq*BTIC6A_ERRSC_16X;
	ctx->ld_skip_sql1=ctx->ld_skip_sq*BTIC6A_ERRSC_8X;
	ctx->ld_skip_sql2=ctx->ld_skip_sq*BTIC6A_ERRSC_4X;
	ctx->ld_skip_sql3=ctx->ld_skip_sq*BTIC6A_ERRSC_2X;
	
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

//	ctx->cimg2x=malloc(ctx->bxs*((ctx->bys+24)*2)*4);
	ctx->cimg2x=malloc(ctx->bxs*((ctx->bys+64)*2)*4);
	ctx->cimg=ctx->cimg2x+(ctx->bxs*24)*4;
	ctx->pimg=ctx->cimg+(ctx->bxs*(ctx->bys+24))*4;

//	ctx->fl=0;

//	ctx->blka=malloc(ctx->bxs*ctx->bys*sizeof(u64));
//	ctx->lblka=malloc(ctx->bxs*ctx->bys*sizeof(u64));

	ctx->tfbsz=xs*ys*4;
	ctx->tfbuf=malloc(xs*ys*4);
	ctx->tfzbuf=malloc(xs*ys*4);
	
	BTIC6A_EncodeContextSetupQuality(ctx, qfl);

	return(0);
}
