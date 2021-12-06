#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "repak_core.h"

#include "snd_adpcm.c"

#include "tk_ramavi.c"
#include "tk_outavi.c"

#define TGVLZ_NOMAIN
#include "tgvlz1.c"

#include "tkulzd.c"
#include "tkulze.c"

#include "btrlz0_dec.c"
#include "btrlz0_enc.c"

#include "bt5b_decode.c"
#include "bt5b_encode.c"

#include "bt1h_targa.c"

int clamp255(int v)
{
	if(v<0)return(0);
	if(v>255)return(255);
	return(v);
}

int cram_vqenc_rgb2rgb555(int cr, int cg, int cb)
{
	int mr, mg, mb, ia;
	
	mr=cr;
	mg=cg;
	mb=cb;
	mr=mr+15;
	mg=mg+15;
	mb=mb+15;
	mr=clamp255(mr);
	mg=clamp255(mg);
	mb=clamp255(mb);
	
//	ia=((mr>>3)<<10)|((mg>>3)<<5)|(mb>>3);
	ia=((mb>>3)<<10)|((mg>>3)<<5)|(mr>>3);
	return(ia);
}

int cram_vqenc(byte *iblk, int ystr, u16 *opx, u16 *oclra, u16 *oclrb)
{
	short pxy[16];
	int min, max;

	int acr, acg, acb, acy, acy_c, acy_m, acy_y;

	int cr, cg, cb, cy;
	int mr, mg, mb, my;
	int nr, ng, nb, ny;

	int mr2, mg2, mb2, my2;
	int nr2, ng2, nb2, ny2;

	int cy_c, cy_m, cy_y;
	int my_c, my_m, my_y;
	int ny_c, ny_m, ny_y;
	int ax, dy;

	int cya, cyb;
	int ia, ib, ix, px;
	int i, j, k;
	
	my=1024; ny=-1024;
	my_c=1024; ny_c=-1024;
	my_m=1024; ny_m=-1024;
	my_y=1024; ny_y=-1024;
	acr=0; acg=0; acb=0; acy=0;
	acy_c=0; acy_m=0; acy_y=0;
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		cr=iblk[i*ystr+j*4+0];
		cg=iblk[i*ystr+j*4+1];
		cb=iblk[i*ystr+j*4+2];
		cy=(cr+2*cg+cb)/4;

		cy_c=(1*cr+4*cg+3*cb)/8;
		cy_m=(4*cr+1*cg+3*cb)/8;
		cy_y=(3*cr+4*cg+1*cb)/8;

		if(cy<my) { my=cy; }
		if(cy>ny) { ny=cy; }
		if(cy_c<my_c) { my_c=cy_c; }
		if(cy_c>ny_c) { ny_c=cy_c; }
		if(cy_m<my_m) { my_m=cy_m; }
		if(cy_m>ny_m) { ny_m=cy_m; }
		if(cy_y<my_y) { my_y=cy_y; }
		if(cy_y>ny_y) { ny_y=cy_y; }
		
		acr+=cr;	acg+=cg;
		acb+=cb;	acy+=cy;
		acy_c+=cy_c;
		acy_m+=cy_m;
		acy_y+=cy_y;
	}
	
	acr=acr/16;	acg=acg/16;
	acb=acb/16;	acy=acy/16;

	acy_c=acy_c/16;
	acy_m=acy_m/16;
	acy_y=acy_y/16;

	ax=0; dy=ny-my;
	i=ny_c-my_c; if(i>dy) { ax=1; dy=i; acy=acy_c; }
	i=ny_m-my_m; if(i>dy) { ax=2; dy=i; acy=acy_m; }
	i=ny_y-my_y; if(i>dy) { ax=3; dy=i; acy=acy_y; }

	mr=0; mg=0; mb=0; my=0; cya=0;
	nr=0; ng=0; nb=0; ny=0; cyb=0;
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		cr=iblk[i*ystr+j*4+0];
		cg=iblk[i*ystr+j*4+1];
		cb=iblk[i*ystr+j*4+2];
//		cy=(cr+2*cg+cb)/4;

		switch(ax)
		{
		case 0: cy=(cr+2*cg+cb)/4; break;
		case 1: cy=(1*cr+4*cg+3*cb)/8; break;
		case 2: cy=(4*cr+1*cg+3*cb)/8; break;
		case 3: cy=(3*cr+4*cg+1*cb)/8; break;
		}

		pxy[i*4+j]=cy;
		
		if(cy<acy)
			{ mr+=cr; mg+=cg; mb+=cb; my+=cy; cya++; }
		else
			{ nr+=cr; ng+=cg; nb+=cb; ny+=cy; cyb++; }
	}
	
	if(!cya)cya++;
	if(!cyb)cyb++;

	mr=mr/cya; mg=mg/cya;
	mb=mb/cya; my=my/cya;

	nr=nr/cyb; ng=ng/cyb;
	nb=nb/cyb; ny=ny/cyb;

	my2=1024; ny2=-1024;
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		cr=iblk[i*ystr+j*4+0];
		cg=iblk[i*ystr+j*4+1];
		cb=iblk[i*ystr+j*4+2];

//		cy=(cr+2*cg+cb)/4;

		switch(ax)
		{
		case 0: cy=(cr+2*cg+cb)/4; break;
		case 1: cy=(1*cr+4*cg+3*cb)/8; break;
		case 2: cy=(4*cr+1*cg+3*cb)/8; break;
		case 3: cy=(3*cr+4*cg+1*cb)/8; break;
		}
		
		pxy[i*4+j]=cy;
		if(cy<my2)
			{ my2=cy; mr2=cr; mg2=cg; mb2=cb; }
		if(cy>ny2)
			{ ny2=cy; nr2=cr; ng2=cg; nb2=cb; }
	}
	
	mr=(mr+mr2)/2;
	mg=(mg+mg2)/2;
	mb=(mb+mb2)/2;
	my=(my+my2)/2;

	nr=(nr+nr2)/2;
	ng=(ng+ng2)/2;
	nb=(nb+nb2)/2;
	ny=(ny+ny2)/2;
	
//	ia=((mr>>3)<<10)|((mg>>3)<<5)|(mb>>3);
//	ib=((nr>>3)<<10)|((ng>>3)<<5)|(nb>>3);
	
	ia=cram_vqenc_rgb2rgb555(mr, mg, mb);
	ib=cram_vqenc_rgb2rgb555(nr, ng, nb);
//	ix=(ia<<4)|ib;
	
//	cya=(3*my+1*ny)/4;
//	cyb=(1*my+3*ny)/4;

//	cya=(5*my+3*ny)/8;
//	cyb=(3*my+5*ny)/8;

	cya=(5*my+3*ny+8*acy)/16;
	cyb=(3*my+5*ny+8*acy)/16;
	
	px=0;
	if(pxy[ 0]>cya)px|=0x0001;
	if(pxy[ 1]>cyb)px|=0x0002;
	if(pxy[ 2]>cya)px|=0x0004;
	if(pxy[ 3]>cyb)px|=0x0008;

	if(pxy[ 4]>cyb)px|=0x0010;
	if(pxy[ 5]>cya)px|=0x0020;
	if(pxy[ 6]>cyb)px|=0x0040;
	if(pxy[ 7]>cya)px|=0x0080;

	if(pxy[ 8]>cya)px|=0x0100;
	if(pxy[ 9]>cyb)px|=0x0200;
	if(pxy[10]>cya)px|=0x0400;
	if(pxy[11]>cyb)px|=0x0800;

	if(pxy[12]>cyb)px|=0x1000;
	if(pxy[13]>cya)px|=0x2000;
	if(pxy[14]>cyb)px|=0x4000;
	if(pxy[15]>cya)px|=0x8000;
	
	*opx=px;
	*oclra=ib;
	*oclrb=ia;
	return(0);
}

int cram_repak_buf(byte *ibuf, byte *obuf,
	int isz, int osz,
	BGBBTJ_Video_Stats *vstat)
{
	u32 aclr[2], bclr[2], cclr[2], dclr[2];
	u32 pxbuf[16];
	u32 *bbuf, *bb0, *bb1, *bb2, *bb3;
	u16 px, px1;
	u16 clra, clrb;
	u32 clrc, clrd;
	byte *cs, *cse, *ct;
	int xs, ys;
	int n, bx, by, by2, bxs, bys;
	int i, j, k;
	
	xs=vstat->width;
	ys=vstat->height;
	bxs=xs>>2;
	bys=ys>>2;
	
	ct=obuf;
	cs=ibuf; cse=ibuf+isz; bx=0; by=0;
	by2=bys-by-1;
	while(cs<cse)
	{
		px=CRAM_GETU16(cs);
		if(!px && (by>=bys))
			break;
		if(px&0x8000)
		{
			cs+=2;

			if((px&0xFC00)==0x8400)
			{
				n=px&1023;
				bx+=n;
				while(bx>=bxs)
				{
					by++; bx-=bxs;
					by2=bys-by-1;
					if(by>=bys)
						break;
				}
				if(by>=bys)
				{
					if(!cs[0] && !cs[1])
					{
						*ct++=px;	*ct++=px>>8;
						break;
					}
					printf("Past End A\n");
					break;
				}
				*ct++=px;	*ct++=px>>8;
				continue;
			}

			*ct++=px;	*ct++=px>>8;
						
			bx++;
			if(bx>=bxs)
			{
				by++; bx-=bxs;
				by2=bys-by-1;
				if(by>=bys)
				{
					if(!cs[0] && !cs[1])
						break;
					if(cs<cse)
						printf("Past End B %d\n", (int)(cse-cs));
					break;
				}
			}
			continue;
		}

		clra=CRAM_GETU16(cs+2);
		clrb=CRAM_GETU16(cs+4);

		bbuf=pxbuf;
		bb0=bbuf;		bb1=bb0+4;
		bb2=bb1+4;		bb3=bb2+4;

		if(clra&0x8000)
		{
			cs+=6;
			aclr[1]=cram_tclr_rgb555(clra);
			aclr[0]=cram_tclr_rgb555(clrb);

			bclr[1]=cram_tclr_rgb555(CRAM_GETU16(cs+ 0));
			bclr[0]=cram_tclr_rgb555(CRAM_GETU16(cs+ 2));
			cclr[1]=cram_tclr_rgb555(CRAM_GETU16(cs+ 4));
			cclr[0]=cram_tclr_rgb555(CRAM_GETU16(cs+ 6));
			dclr[1]=cram_tclr_rgb555(CRAM_GETU16(cs+ 8));
			dclr[0]=cram_tclr_rgb555(CRAM_GETU16(cs+10));
			cs+=12;

			bb0[0]=aclr[(px>> 0)&1];	bb0[1]=aclr[(px>> 1)&1];
			bb0[2]=bclr[(px>> 2)&1];	bb0[3]=bclr[(px>> 3)&1];
			bb1[0]=aclr[(px>> 4)&1];	bb1[1]=aclr[(px>> 5)&1];
			bb1[2]=bclr[(px>> 6)&1];	bb1[3]=bclr[(px>> 7)&1];
			bb2[0]=cclr[(px>> 8)&1];	bb2[1]=cclr[(px>> 9)&1];
			bb2[2]=dclr[(px>>10)&1];	bb2[3]=dclr[(px>>11)&1];
			bb3[0]=cclr[(px>>12)&1];	bb3[1]=cclr[(px>>13)&1];
			bb3[2]=dclr[(px>>14)&1];	bb3[3]=dclr[(px>>15)&1];

			cram_vqenc((byte *)pxbuf, 16, &px, &clra, &clrb);

			clra&=0x7FFF;
			clrb&=0x7FFF;
			
			if(px&0x8000)
			{
				k=clra; clra=clrb; clrb=k;
				px=~px;
			}
			
//			if(!px)
//				px|=0x0100;

			*ct++=px;	*ct++=px>>8;
			*ct++=clra;	*ct++=clra>>8;
			*ct++=clrb;	*ct++=clrb>>8;

		}else if(clrb&0x8000)
		{
			printf("!\n");
		}else
		{
//			if(!px)
//				px|=0x0100;

			*ct++=px;	*ct++=px>>8;
			*ct++=clra;	*ct++=clra>>8;
			*ct++=clrb;	*ct++=clrb>>8;

			cs+=6;
		}

		bx++;
		if(bx>=bxs)
		{
			by++; bx-=bxs;
			by2=bys-by-1;
			if(by>=bys)
			{
				if(!cs[0] && !cs[1])
					break;
				if(cs<cse)
					printf("Past End C %d\n", (int)(cse-cs));
				break;
			}
		}
		continue;
	}
	
	*ct++=0;
	*ct++=0;
	
	k=ct-obuf;
	return(k);
}

int cram_repak_buf_craz(byte *ibuf, byte *obuf,
	int isz, int osz,
	BGBBTJ_Video_Stats *vstat)
{
	TgvLz_Context *zctx;
	byte *tbuf, *t2buf;
	int tsz, t2sz, sz;

	tbuf=malloc(isz*2);
	t2buf=malloc(isz*2);
	tsz=cram_repak_buf(ibuf, tbuf+4, isz, isz*2, vstat);
	set_u32le(tbuf, 0x00000000|tsz);
	
	zctx=TgvLz_CreateContext();
	t2sz=TgvLz_DoEncode(zctx, tbuf+4, t2buf+4, tsz);
	TgvLz_DestroyContext(zctx);
	set_u32le(t2buf, 0x03000000|t2sz);
	
	if((t2sz*1.3)<tsz)
	{
		sz=t2sz+4;
		memcpy(obuf, t2buf, sz);
	}else
	{
		sz=t2sz+4;
		memcpy(obuf, tbuf, sz);
	}
	
	free(tbuf);
	free(t2buf);
	return(sz);
}

int main(int argc, char *argv[])
{
	char tb[256];
	short sndbuf[8192];
	char *ifn, *ofn;

	BGBBTJ_AVI_Context *avi;
	BGBBTJ_Video_Stats *vstat;
	BGBBTJ_AVICtx *oavi;
	BTIC5B_EncodeContext *ectx;
	BTIC5B_DecodeContext *dctx;
	byte *buf;
	byte *tbuf;
	byte *obuf;
	u16 *i2buf;
	int xs, ys, osz;
	int t0, t1, t2, te;
	int sz, sndlen, slen2;

	u32 *vreg;
	u32 *vram;
	u32 px;
	int i, j, k, qfl;

//	ifn="M:\\TestSonicUnleashed_WindmillIsleDA2_CRAM2.avi";

	ifn="M:\\BadApple_CV1.avi";
	ofn="TestOut_CV0.avi";

//	ifn="M:\\HEYYEYAAEYAAAEYAEYAA_CV0.avi";
//	ofn="TestOut_CV1.avi";

	avi=BGBBTJ_AVI_LoadAVI(ifn);
	if(!avi)
	{
		printf("AVI Load Failed\n");
		return;
	}

	vstat=BGBBTJ_AVI_GetStats(avi);
	
	xs=vstat->width;
	ys=vstat->height;
	
//	oavi=BGBBTJ_AVI_OpenOutStream2(ofn, vstat->width, vstat->height,
//		1000000.0/vstat->frametime, RIFF_TAG_CRAM, 0);

//	oavi=BGBBTJ_AVI_OpenOutStream2(ofn, vstat->width, vstat->height,
//		1000000.0/vstat->frametime, RIFF_TAG_CRAM, BGBBTJ_AVI_AUID_16K8BM);
//	oavi=BGBBTJ_AVI_OpenOutStream2(ofn, vstat->width, vstat->height,
//		1000000.0/vstat->frametime, RIFF_TAG_zcra, BGBBTJ_AVI_AUID_16K8BM);

	oavi=BGBBTJ_AVI_OpenOutStream2(ofn, vstat->width, vstat->height,
		1000000.0/vstat->frametime, BGBBTJ_FCC_bt5b, BGBBTJ_AVI_AUID_16K8BM);
	
	tbuf=malloc(1<<20);
	obuf=malloc(1<<20);

	i2buf=malloc(xs*ys*4);

//	sndlen=44100*(vstat->frametime/1000000.0);
	sndlen=11025*(vstat->frametime/1000000.0)+0.5;
	sndlen=8192;

	ectx=BTIC5B_AllocEncodeContext();
	BTIC5B_EncodeInitContext(ectx, xs, ys, 100);

	dctx=BTIC5B_AllocDecodeContext();

//	memset(sndbuf, 0, 2*sndlen);
	
	for(i=0; i<vstat->num_frames; i++)
	{
//		buf=BGBBTJ_AVI_FrameRawClrs(avi, t2*2000, BGBBTJ_JPG_RAWCON);

#if 0
		buf=BGBBTJ_AVI_DecodeFrame2(avi, 0, BGBBTJ_JPG_RAWDAT);
//		osz=cram_repak_buf(buf, obuf, 1<<20, 1<<20, vstat);
		osz=cram_repak_buf_craz(buf, obuf, 1<<20, 1<<20, vstat);
		slen2=BGBBTJ_AVI_GetMonoSamplesRate(avi, sndbuf, sndlen, 16000);
#endif

#if 1
		buf=BGBBTJ_AVI_DecodeFrame2(avi, 0, BGBBTJ_JPG_RGB555);
//		slen2=BGBBTJ_AVI_GetMonoSamplesRate(avi, sndbuf, sndlen, 11025);
		slen2=BGBBTJ_AVI_GetMonoSamplesRate(avi, sndbuf, sndlen, 16000);

		for(j=0; j<(xs*ys); j++)
			((u16 *)buf)[i]&=0x7FFF;

//		if(!(i&255))
		if(!(i&63))
		{
			sprintf(tb, "frdump/frame_%d.tga", i);
			BTIC1H_Img_SaveTGA555(tb, (u16 *)buf, xs, ys);
		}

//		qfl=100;
//		qfl=90;
		qfl=95;
//		if((i&15))qfl|=BTPIC_QFL_PFRAME;
//		if((i&127))qfl|=BTPIC_QFL_PFRAME;
		if((i&31))qfl|=BTPIC_QFL_PFRAME;

		osz=BTIC5B_EncodeFrameImage(ectx,
			obuf, 1<<22, (u16 *)buf, qfl, BTIC4B_CLRS_RGB555);

		BTIC5B_DecodeFrame(dctx, obuf, osz, i2buf, xs);

		if(!(i&63))
		{
			sprintf(tb, "frdump/frame_%dd.tga", i);
			BTIC1H_Img_SaveTGA555(tb, i2buf, xs, ys);
		}

//		slen2=sndlen;
#endif

		BGBBTJ_AVI_WriteContextMonoSamples(oavi, sndbuf, slen2);
//		BGBBTJ_AVI_EmitCodedFrame(oavi, tbuf, sz, 0);
		BGBBTJ_AVI_EmitCodedFrame(oavi, obuf, osz, 0);
		BGBBTJ_AVI_EmitAudioFrame(oavi);
	}
	
	BGBBTJ_AVI_CloseOutStream(oavi);

	printf("\n\nTest Output\n\n");

#if 0
	avi=BGBBTJ_AVI_LoadAVI(ofn);
	if(!avi)
	{
		printf("AVI Open Out Failed\n");
		return;
	}

	for(i=0; i<vstat->num_frames; i++)
	{
//		buf=BGBBTJ_AVI_FrameRawClrs(avi, t2*2000, BGBBTJ_JPG_RAWCON);
		buf=BGBBTJ_AVI_DecodeFrame2(avi, 0, BGBBTJ_JPG_BGRA);
	}
#endif
}
