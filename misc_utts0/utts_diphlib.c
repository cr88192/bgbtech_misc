typedef struct TkuTts_DiphLib_s TkuTts_DiphLib;
typedef struct TkuTts_PcmState_s TkuTts_PcmState;

struct TkuTts_DiphLib_s {
byte *diph_img;
int diph_sz;
int diph_nrec;
u16 *rec_ix0;
u16 *rec_ix1;
u16 *rec_chn;
int *rec_ofs;
byte *rec_len;
byte *rec_chw;
u16 rec_hash[256];

byte upk_buf[32*128*8*2];
u16 upk_idx[32];
byte upk_len[32];
};

struct TkuTts_PcmState_s {
u32 chan_phase[8];
u32 chan_step[8];
s16 chan_vol[8];
byte *chan_fun[8];
byte ratescale;
sbyte freqadj;
};

TkuTts_DiphLib *TkuTts_LoadDiphLib(char *name)
{
	TkuTts_DiphLib *tmp;
	byte *ibuf, *cs;
	int isz, nrec, ix0, ix1;
	int i, j, k, h;
	
	ibuf=TkuTts_LoadFile(name, &isz);
	if(!ibuf)
		return(NULL);
	
	tmp=malloc(sizeof(TkuTts_DiphLib));
	memset(tmp, 0, sizeof(TkuTts_DiphLib));

	tmp->diph_img=ibuf;
	tmp->diph_sz=isz;

	for(i=1; i<16384; i++)
	{
		if(!(*(int *)(ibuf+(i*16))))
			break;
	}
	nrec=i;
	tmp->diph_nrec=nrec;

	tmp->rec_ix0=malloc(nrec*sizeof(u16));
	tmp->rec_ix1=malloc(nrec*sizeof(u16));
	tmp->rec_chn=malloc(nrec*sizeof(u16));
	tmp->rec_ofs=malloc(nrec*sizeof(int));
	tmp->rec_len=malloc(nrec*sizeof(byte));
	tmp->rec_chw=malloc(nrec*sizeof(byte));

	for(i=1; i<nrec; i++)
	{
		cs=ibuf+i*16;
		ix0=*(u16 *)(cs+ 0);
		ix1=*(u16 *)(cs+ 2);
		tmp->rec_ix0[i]=ix0;
		tmp->rec_ix1[i]=ix1;
		tmp->rec_len[i]=*(byte *)(cs+4);
		tmp->rec_chw[i]=*(byte *)(cs+6);
		tmp->rec_ofs[i]=*(u32 *)(cs+8);
		
		h=(((((ix0*251+ix1)*251)+13)*251)>>8)&255;
		tmp->rec_chn[i]=tmp->rec_hash[h];
		tmp->rec_hash[h]=i;
	}

	return(tmp);
}

int TkuTts_DiphLib_LookupDiphIndex(TkuTts_DiphLib *lib, int ix0, int ix1)
{
	int i, h;
	
	h=(((((ix0*251+ix1)*251)+13)*251)>>8)&255;
	i=lib->rec_hash[h];
	while(i>0)
	{
		if(	(lib->rec_ix0[i]==ix0) &&
			(lib->rec_ix1[i]==ix1) )
				return(i);
		i=lib->rec_chn[i];
	}
	return(0);
}

byte *TkuTts_DiphLib_GetDiphUnpackIndex(TkuTts_DiphLib *lib,
	int idx, int *rlen)
{
	byte *bcs, *bct, *bcb;
	int nb, nw, nw1, chw, nbd;
	int i, j, k;

	if(idx<=0)
		return(NULL);
	if(idx>=lib->diph_nrec)
		return(NULL);
	
	bct=lib->upk_buf+(idx&31)*(128*8*2);
	bcb=bct;
	if(lib->upk_idx[idx&31]==idx)
	{
		*rlen=lib->upk_len[idx&31];
		return(bct);
	}
	
	chw=lib->rec_chw[idx];
	nb=lib->rec_len[idx];
	nw=chw&15;
	nbd=1<<((chw>>6)&3);
	bcs=lib->diph_img+lib->rec_ofs[idx];
	lib->upk_idx[idx&31]=idx;
	lib->upk_len[idx&31]=nb*nbd;
	
	nw1=nw;
	if(nw1>8)
		nw1=8;
	
	for(i=0; i<nb; i++)
	{
		for(j=0; j<16; j++)
			{ bct[j]=0; }
		for(j=0; j<nw1; j++)
		{
			bct[0+j]=bcs[0+j];
			bct[8+j]=bcs[nw+j];
		}

		if((chw&0x20) && (i>0))
		{
			for(j=0; j<16; j++)
				{ bct[j]+=bct[j-16]; }
		}
		
		bct+=8*2;
		bcs+=nw*2;
		
		if(nbd>1)
		{
			for(k=0; k<(nbd-1); k++)
			{
				for(j=0; j<16; j++)
					{ bct[j]=bct[j-16]; }
				bct+=8*2;
			}
		}
	}
	
	*rlen=nb*nbd;
	return(bcb);
}


byte tkutts_pcmtab_sn[256];
byte tkutts_pcmtab_tz[256];
byte tkutts_pcmtab_sq[256];
byte tkutts_pcmtab_pl[256];
byte tkutts_pcmtab_ns[256];
byte tkutts_pcmtab_tz2[256];

u16 tkutts_pcmtab_vol[256];
u32 tkutts_pcmtab_hzstep[256];

int TkuTts_PcmState_InitTables()
{
	static signed char pul2t[16]={
	  8,  8,  8,  8,   5,  3, -3, -8,
	 -8, -8, -8, -8,  -5, -3,  3,  8
	};
	static signed char pul3t[16]={
	  8,  3,  8,  8,   5,  3, -3, -8,
	 -8, -3, -8, -8,  -5, -3,  3,  8
	};

	u32 step;
	int ma, ex;
	int j1, j2, j3;
	int i, j, k;
	
	if(tkutts_pcmtab_sq[0]==0xC0)
		return(0);
		
	
	for(i=0; i<256; i++)
	{
		tkutts_pcmtab_sq[i]=(i&0x80)?0x40:0xC0;
		tkutts_pcmtab_tz[i]=(i&0x80)?0x40:0xC0;
		tkutts_pcmtab_sn[i]=cos(i*(2*3.14159265359/256.0))*64+128;
		tkutts_pcmtab_pl[i]=(i>51)?0x40:0xC0;
		tkutts_pcmtab_ns[i]=0x40+(rand()&127);
	}
	for(i=0; i<64; i++)
	{
		tkutts_pcmtab_tz[(i+  0-64)&255]=0x40+(i*2);
		tkutts_pcmtab_tz[(i+128-64)&255]=0xC0-(i*2);

//		tkutts_pcmtab_tz[(i+  0-64)&255]=0x80;
//		tkutts_pcmtab_tz[(i+128-64)&255]=0x80;
	}

	for(i=0; i<256; i++)
	{
//		tkutts_pcmtab_tz[i]=0x80+(pul2t[i>>4]*8);
//		tkutts_pcmtab_tz[i]=0x80+(pul3t[i>>4]*8);
	}

	for(i=0; i<256; i++)
	{
		j1=i*2;
		j2=i*3;
		tkutts_pcmtab_tz2[i]=
			tkutts_pcmtab_tz[i]+
			((tkutts_pcmtab_tz[(i*2)&255]-128)/3)+
			((tkutts_pcmtab_tz[(i*3)&255]-128)/5)+
			((tkutts_pcmtab_tz[(i*4)&255]-128)/7)+
			((tkutts_pcmtab_tz[(i*5)&255]-128)/9);
	}

	for(i=0; i<256; i++)
	{
		ex=i>>5;
//		if(ex>=1)
//			ex--;
		
		ma=i&31;
		step=0x80000000U-(ma<<25);
//		step=0xC0000000U-(ma<<25);
//		step=0xA0000000U-(ma<<25);
//		step=0x40000000U+(ma<<25);
		step>>=ex;
//		step+=step>>2;

		if(step>0x80000000U)
			step=0x80000000U;

		tkutts_pcmtab_hzstep[i]=step;
	}

	for(i=0; i<256; i++)
	{
		ex=(i>>3)&7;
		ma=8+(i&7);
		tkutts_pcmtab_vol[i]=ma<<(ex+2);
		
//		if((i&63)==0)
//			tkutts_pcmtab_vol[i]=0;

//		tkutts_pcmtab_vol[i]=(i&63)*64;

//		k=i&63;
//		tkutts_pcmtab_vol[i]=k*k;

		if((i&63)==0)
			tkutts_pcmtab_vol[i]=0;
	}

	return(1);
}

TkuTts_PcmState *TkuTts_PcmState_Create()
{
	TkuTts_PcmState *tmp;
	
	TkuTts_PcmState_InitTables();
	
	tmp=malloc(sizeof(TkuTts_PcmState));
	memset(tmp, 0, sizeof(TkuTts_PcmState));
	return(tmp);
}

s16 TkuTts_PcmState_StepSample(TkuTts_PcmState *pcm)
{
	byte *fn;
	int ph, vl, va, st;
	int i, j, k;
	
	va=0;
	for(i=0; i<8; i++)
	{
		fn=pcm->chan_fun[i];
		ph=pcm->chan_phase[i];
		st=pcm->chan_step[i];
		vl=pcm->chan_vol[i];
		k=fn[(ph>>24)&255];
		va+=(vl*(k-128))>>6;
		pcm->chan_phase[i]=ph+st;
	}
	
	if(va<(-32767))		va=-32767;
	if(va>( 32767))		va= 32767;
	return(va);
}

int TkuTts_PcmState_SetupBlock(TkuTts_PcmState *pcm,
	byte *frq, byte *amp)
{
	int fri;
	int f1, a1;
	int i, j, k;

#if 1
	f1=frq[0]+0x20;
	a1=amp[0]-(3<<3);
	if(f1<0)f1=0;
	if(a1<0)a1=0;
	a1|=2<<6;
	frq[6]=f1;	amp[6]=a1;

	f1=frq[1]+0x20;
	a1=amp[1]-(3<<3);
	if(f1<0)f1=0;
	if(a1<0)a1=0;
	a1|=2<<6;
	frq[7]=f1;	amp[7]=a1;

#if 0
	f1=frq[2]+0x30;
	a1=amp[2]-(3<<3);
	if(f1<0)f1=0;
	if(a1<0)a1=0;
	a1|=2<<6;
	frq[7]=f1;	amp[7]=a1;
#endif
#endif

#if 0
	f1=0xE0;
	a1=amp[0]-(1<<3);
	if(a1<0)	a1=0;
	if(a1>63)	a1=63;
//	a1|=2<<6;
	frq[7]=f1;	amp[7]=a1;
#endif

	for(i=0; i<8; i++)
	{
		fri=frq[i];

		fri-=0x20;
//		fri-=0x40;

//		fri+=0x20;

//		fri+=0x10;
//		fri-=0x10;

		fri+=pcm->freqadj;

		if(fri<0)
			fri=0;
		pcm->chan_step[i]=tkutts_pcmtab_hzstep[fri];
		pcm->chan_vol[i]=tkutts_pcmtab_vol[amp[i]];

//		pcm->chan_phase[i]=0;

		switch((amp[i]>>6)&3)
//		switch(0)
		{
		case 0:
			pcm->chan_fun[i]=tkutts_pcmtab_tz;
//			pcm->chan_fun[i]=tkutts_pcmtab_tz2;
//			pcm->chan_fun[i]=tkutts_pcmtab_sq;
//			pcm->chan_fun[i]=tkutts_pcmtab_sn;
//			pcm->chan_fun[i]=tkutts_pcmtab_pl;
			break;
		case 1:
			pcm->chan_fun[i]=tkutts_pcmtab_sn;
			break;
		case 2:
			pcm->chan_fun[i]=tkutts_pcmtab_sq;
			break;
		case 3:
			pcm->chan_fun[i]=tkutts_pcmtab_pl;
			if(fri<0x20)
			{
				pcm->chan_fun[i]=tkutts_pcmtab_ns;
				pcm->chan_step[i]=tkutts_pcmtab_hzstep[fri<<2]>>8;
			}
			break;
		}
	}
	return(0);
}


int TkuTts_clamp16s(int val)
{
	int v;
	v=val;
	if(v<-32767)
		v=-32767;
	if(v> 32767)
		v=32767;
	return(v);
}


int TkuTts_decswg_1x(int *dst, int *rbi, int *rbd)
{
	static signed char sint[16]={
	 0,  3,  5,  8,	 8,  5,  3,  0,
	 0, -3, -5, -8,	-8, -5, -3,  0
	};
	static signed char sint2[32]={
	  0,  1,  2,  3,  5,  6,  7,  8,
	  8,  7,  6,  5,  3,  2,  1,  0,
	  0, -1, -2, -3, -5, -6, -7, -8,
	 -8, -7, -6, -5, -3, -2, -1,  0,
	};
	static signed char pul1t[16]={
	  8,  8,  8,  8,   0,  0,  0,  0,  
	 -8, -8, -8, -8,   0,  0,  0,  0
	};
	static signed char pul2t[16]={
	  8,  8,  8,  8,   5,  3, -3, -8,
	 -8, -8, -8, -8,  -5, -3,  3,  8
	};

	int tdst[64];
	int i0, i1, i2, i3;
	int i, j, k, bi, bd, shr, wf;

	bi=*rbi;
	bd=*rbd;

//	bi-=0x20;
//	if(bi<0)
//		return(0);

	shr=bi>>5;
	bi=(bi&31)*8;
	
	for(j=0; j<2*64; j++)
	{
		wf=j-64;
		wf=wf^(wf>>31);
		wf*=(256/64);
		wf=512-wf;
		if(wf>256)
			wf=256;

//		i2=256+bi;
		i2=512-bi;
//		i1=(j*i2)>>(5+shr);
		i1=(j*i2)>>(4+shr);
		i3=pul2t[(i1&31)>>1];
		k=(bd*i3)>>3;
//		k=(k*wf)>>8;
		dst[j]+=k;
	}
		
	return(0);
}


int TkuTts_DecSwgBlock_6x(s16 *obuf, byte *tbf, byte *tbv)
{
	int tmpdbuf[2*64+64];
	int tmpbands[16];
	int j, k;

	tmpbands[ 0]=tbf[0];
	tmpbands[ 1]=tbv[0];
	tmpbands[ 2]=tbf[1];
	tmpbands[ 3]=tbv[1];
	tmpbands[ 4]=tbf[2];
	tmpbands[ 5]=tbv[2];
	tmpbands[ 6]=tbf[3];
	tmpbands[ 7]=tbv[3];
	tmpbands[ 8]=tbf[4];
	tmpbands[ 9]=tbv[4];
	tmpbands[10]=tbf[5];
	tmpbands[11]=tbv[5];

	for(j=0; j<(2*64); j++)
		tmpdbuf[j]=0;
	TkuTts_decswg_1x(tmpdbuf, tmpbands+ 0, tmpbands+ 1);
	TkuTts_decswg_1x(tmpdbuf, tmpbands+ 2, tmpbands+ 3);
	TkuTts_decswg_1x(tmpdbuf, tmpbands+ 4, tmpbands+ 5);
	TkuTts_decswg_1x(tmpdbuf, tmpbands+ 6, tmpbands+ 7);
	TkuTts_decswg_1x(tmpdbuf, tmpbands+ 8, tmpbands+ 9);
	TkuTts_decswg_1x(tmpdbuf, tmpbands+10, tmpbands+11);

	for(j=0; j<(64/2); j++)
	{
		k=(2*64)-(j+1);
		tmpdbuf[j]=(tmpdbuf[j]*j)/(64/2);
		tmpdbuf[k]=(tmpdbuf[k]*j)/(64/2);
	}

	for(j=0; j<2*64; j++)
	{
		obuf[j]=TkuTts_clamp16s(obuf[j]+tmpdbuf[j]);
	}
	return(0);
}

int TkuTts_PcmRunPhonetic(
	TkuTts_PcmState *pcm, TkuTts_DiphLib *lib,
	s16 *obuf, char *str)
{
	s16 *ct;
	char *cs;
	byte *bcs;
	int i0, i1, i2, i3, dix, nb, nb1, cstp, waslv, bsz;
	int i, j, k;
	
	bsz=64;
	if(pcm->ratescale)
		bsz=pcm->ratescale;
		
	
	cs=str; ct=obuf; waslv=0;
	while(*cs)
	{
		if(*cs==' ')
		{
			cs++;
			memset(ct, 0, 2048);
			ct+=2048;
			continue;
		}

		i0=cs[0];
		if(i0=='.')
		{
			waslv=0;
			cs++;
			continue;
		}

		cstp=1;
		i1=cs[1];
		if(i1=='x')
		{
			switch(cs[2])
			{
			case 'a': i1='A'|('@'<<8);	break;
			case 'e': i1='e'|('@'<<8);	break;
			case 'i': i1='I'|('@'<<8);	break;
			case 'o': i1='O'|('@'<<8);	break;
			case 'u': i1='U'|('@'<<8);	break;

			case 'A': i1='e'|('I'<<8);	break;
			case 'I': i1='A'|('I'<<8);	break;
			case 'O': i1='O'|('I'<<8);	break;
			}
		}

		if(i0=='x')
		{
			cstp=2;
			switch(cs[1])
			{
			case 'a': i0='A'|('@'<<8);	break;
			case 'e': i0='e'|('@'<<8);	break;
			case 'i': i0='I'|('@'<<8);	break;
			case 'o': i0='O'|('@'<<8);	break;
			case 'u': i0='U'|('@'<<8);	break;

			case 'A': i0='e'|('I'<<8);	break;
			case 'I': i0='A'|('I'<<8);	break;
			case 'O': i0='O'|('I'<<8);	break;
			}
			i1=cs[2];
		}

		if(i0=='X')
		{
			cstp=3;
			i0=cs[1]|((cs[2])<<8);
			i1=cs[3];
		}

		if(i1=='.')
		{
			i1=0;
		}

		if((i1=='x') || (i1=='X'))
			{ i1=0; }
		
		dix=TkuTts_DiphLib_LookupDiphIndex(lib, i0, i1);
		if((dix<0) && (i1>>8))
		{
			dix=TkuTts_DiphLib_LookupDiphIndex(lib, i0, (i1&255));
		}
		if(dix<=0)
		{
			i1=0;
			dix=TkuTts_DiphLib_LookupDiphIndex(lib, i0, i1);
			if((dix<0) && (i0>>8))
			{
				dix=TkuTts_DiphLib_LookupDiphIndex(lib, (i0&255), (i0>>8));
			}

			if(dix<0)
			{
				waslv=0;
//				cs++;
				cs+=cstp;
				continue;
			}
		}
		
//		cs++;
		cs+=cstp;

		bcs=TkuTts_DiphLib_GetDiphUnpackIndex(lib, dix, &nb);

		if(i0 && i1)
		{
			if(waslv)
				{ i2=(nb/4); }
			else
				{ i2=0; }
			i3=nb-(nb/4);
		}else
		{
			if(waslv)
			{
				i2=(nb/2);
				i3=nb;
			}else
			{
				i2=0;
				i3=nb;
			}
		}
		
		bcs+=i2*16;
		nb1=i3-i2;
		for(i=0; i<nb1; i++)
		{
#if 1
			TkuTts_PcmState_SetupBlock(pcm, bcs+8, bcs+0);
//			for(j=0; j<64; j++)
//			for(j=0; j<80; j++)
//			for(j=0; j<96; j++)
			for(j=0; j<bsz; j++)
				{ *ct++=TkuTts_PcmState_StepSample(pcm); }
#else
			TkuTts_DecSwgBlock_6x(ct, bcs+8, bcs+0);
			ct+=64;
#endif
			bcs+=16;
			waslv=1;
		}
	}
	return(ct-obuf);
}
