#include "tfau_multi.c"

// #include "adl2q_dec.c"
// #include "adl2q_enc.c"
// #include "adlq_bitrc.c"

#include "utts_dict.c"
#include "utts_diphlib.c"

typedef struct {
float x, y, z, w;
}vec4f;

#define DO_BANDS	4

#if (DO_BANDS==2)
#define DO_2BANDS
#endif

#if (DO_BANDS==4)
#define DO_4BANDS
#endif

#if (DO_BANDS==6)
#define DO_6BANDS
#endif

// #define DO_BAND_DELTA

#define BLKSIZE		128
// #define BLKSIZE		64

int encswg_1x(short *src, int *rbi, int *rbd, int shr, int wsc)
{
//	static signed char sint[16]={
//	 0,  3,  5,  8,	 8,  5,  3,  0,
//	 0, -3, -5, -8,	-8, -5, -3,  0
//	};

	static signed char pul1t[16]={
	  8,  8,  8,  8,   0,  0,  0,  0,  
	 -8, -8, -8, -8,   0,  0,  0,  0
	};
	static signed char pul2t[16]={
	  8,  8,  8,  8,   5,  3, -3, -8,
	 -8, -8, -8, -8,  -5, -3,  3,  8
	};

	int i0, i1, i2, i3;
	int i, j, k, bi, bd, vi, vb, vs, po;
	
	bd=0; bi=0;
//	for(i=0; i<256; i++)
//	for(i=0; i<128; i++)
//	for(i=0; i<96; i++)
//	for(i=0; i<64; i++)
//	for(i=0; i<48; i++)
//	for(i=0; i<32; i++)
//	for(i=0; i<24; i++)
//	for(i=0; i<12; i++)
	for(i=0; i<wsc; i++)
//	for(i=0; i<8; i++)
		for(po=0; po<4; po++)
	{
//		vi=(shr<<7)|i;
//		vi=shr*192+i;
//		vi=shr*96+i;
//		vi=shr*48+i;
//		vi=shr*24+i;
//		vi=shr*12+i;
		vi=shr*wsc+i;
//		vi=shr*8+i;
//		vi=shr*32+i;

		if(vi>=256)
			continue;

//		vb=(vi&63)*4;
//		vs=vi>>6;

		vb=(vi&31)*8;
		vs=vi>>5;

		k=0;
		for(j=0; j<BLKSIZE; j++)
		{
			i0=src[j];
//			i2=1024-(((shr&1)+1)<<8)+i;
//			i1=(j*i2)>>(10+(shr>>1));
//			k+=i0*((i1&1)*2-1);

//			i1=(j*i2)>>(7+(shr>>1));
//			i3=sint[i1&15];

//			i2=256+vb;
			i2=512-vb;
			i1=(j*i2)>>(6+vs);
//			i1=(j*i2)>>(5+vs);
//			i1=(j*i2)>>(4+vs);
//			i3=sint[i1&15];
//			i3=pul1t[i1&15];
			i1+=po*2;
			i3=pul2t[i1&15];

			k+=(i0*i3)>>3;

		}
		k/=BLKSIZE;
		
		if((k^(k>>31))>bd)
//		if(k>bd)
			{ bi=vi; bd=k; }
	}
	*rbi=bi+0x20;
	*rbd=bd;
	return(0);
}

int encswg_1xr(short *src, int *rbi, int *rbd, int rmin, int rmax)
{
	static signed char sint[16]={
	 0,  3,  5,  8,	 8,  5,  3,  0,
	 0, -3, -5, -8,	-8, -5, -3,  0
	};

	static signed char pul1t[16]={
	  8,  8,  8,  8,   0,  0,  0,  0,  
	 -8, -8, -8, -8,   0,  0,  0,  0
	};
	static signed char pul2t[16]={
	  8,  8,  8,  8,   5,  3, -3, -8,
	 -8, -8, -8, -8,  -5, -3,  3,  8
	};

	int i0, i1, i2, i3;
	int i, j, k, bi, bd, vi, vb, vs, po;
	
	bd=0; bi=0;
	for(i=rmin; i<=rmax; i++)
		for(po=0; po<4; po++)
	{
		vi=i;

		vb=(vi&31)*8;
		vs=vi>>5;

		k=0;
		for(j=0; j<BLKSIZE; j++)
		{
			i0=src[j];

//			i2=256+vb;
			i2=512-vb;
			i1=(j*i2)>>(6+vs);
			i1+=po*2;
			i3=sint[i1&15];
//			i3=pul2t[i1&15];

			k+=(i0*i3)>>3;

		}
		k/=BLKSIZE;
		
		if((k^(k>>31))>bd)
			{ bi=vi; bd=k; }
	}
	*rbi=bi+0x20;
	*rbd=bd;
	return(0);
}

int decswg_1x(int *dst, int *rbi, int *rbd)
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

	int tdst[BLKSIZE];
	int i0, i1, i2, i3;
	int i, j, k, bi, bd, shr, wf;

	bi=*rbi;
	bd=*rbd;

//	bi-=0x20;
//	if(bi<0)
//		return(0);

//	shr=bi>>8;
//	bi&=255;

//	shr=bi>>6;
//	bi=(bi&63)*4;

	shr=bi>>5;
	bi=(bi&31)*8;
	
	for(j=0; j<2*BLKSIZE; j++)
	{
//		i2=1024-(((shr&1)+1)<<8)+bi;
//		i1=(j*i2)>>(10+(shr>>1));
//		k=i1;
//		dst[j]+=bd*((k&1)*2-1);

//		i1=(j*i2)>>(7+(shr>>1));

		wf=j-BLKSIZE;
		wf=wf^(wf>>31);
		wf*=(256/BLKSIZE);
		wf=512-wf;
		if(wf>256)
			wf=256;

//		i2=256+bi;
		i2=512-bi;
//		i1=(j*i2)>>(6+shr);
//		i3=sint[i1&15];
//		i1=(j*i2)>>(5+shr);
		i1=(j*i2)>>(4+shr);
//		i3=sint2[i1&31];
//		i3=pul1t[(i1&31)>>1];
		i3=pul2t[(i1&31)>>1];
		k=(bd*i3)>>3;
//		k=(k*wf)>>8;
		dst[j]+=k;
//		dst[j]=clamp16s(dst[j]+k);
	}
		
	return(0);
}

int Mul_Fp8toFp16(int vala, int valb)
{
	int fra, frb, frc, exa, exb, exc, sga, sgb, sgc;
	int valc;
	sga=(vala>>7)&1;	sgb=(valb>>7)&1;
	exa=(vala>>3)&15;	exb=(valb>>3)&15;
	fra=8|(vala&7);		frb=8|(valb&7);
	sgc=sga^sgb;
	frc=(fra*frb)<<4;
	exc=exa+exb+1;
	if(frc&2048)
		{ exc++; frc>>=1; }
	valc=(sgc<<15)|(exc<<10)|(frc&1023);
	return(valc);
}

int Mul_Fp16(int vala, int valb)
{
	int fra, frb, frc, exa, exb, exc, sga, sgb, sgc;
	int valc;
	sga=(vala>>15)&1;			sgb=(valb>>15)&1;
	exa=(vala>>10)&31;			exb=(valb>>10)&31;
	fra=1024|(vala&1023);		frb=1024|(valb&1023);
	sgc=sga^sgb;
	frc=(fra*frb)>>10;
	exc=exa+exb-15;
	if(exc<=0)
		return(0);
	if(frc&2048)
		{ exc++; frc>>=1; }
	valc=(sgc<<15)|(exc<<10)|(frc&1023);
	return(valc);
}


int Add_Fp16(int vala, int valb)
{
	int fra, frb, frc, exa, exb, exc, sga, sgb, sgc;
	int shra, shrb;
	int valc;
	sga=(vala>>15)&1;			sgb=(valb>>15)&1;
	exa=(vala>>10)&31;			exb=(valb>>10)&31;
	fra=1024|(vala&1023);		frb=1024|(valb&1023);

	shra=(exb>=exa)?(exb-exa):0;
	shrb=(exa>=exb)?(exa-exb):0;

	fra=fra>>shra;
	frb=frb>>shrb;
	
	if(sga!=sgb)
	{
		if(fra>frb)
		{
			sgc=sga;
			exc=fra;
			frc=fra-frb;
		}else
		{
			sgc=sgb;
			exc=frb;
			frc=frb-fra;
		}
	}else
	{
		sgc=sga;
		exc=(exa>exb)?exa:exb;
		frc=fra+frb;
	}

	if(exc<=0)
		return(0);
	if(!frc)
		return(0);
	if(frc&2048)
		{ exc++; frc>>=1; }
	while(!(frc&1024))
		{ exc--; frc<<=1; }
	if(exc<=0)
		return(0);

	valc=(sgc<<15)|(exc<<10)|(frc&1023);
	return(valc);
}

int FixToFp8(int val)
{
	int ex, fr, sg, valc;
	
	if(!val)
		return(0);
	
	sg=0; ex=0; fr=val;
	if(val<0)
		{ sg=1; fr=-val; }
	
	while(fr>>4)
		{ fr=fr>>1; ex++; }
	
	if(ex>15)
		return((sg<<7)|0x7F);
	
	valc=(sg<<7)|(ex<<3)|(fr&7);
	return(valc);
}

int Fp8ToFix(int vala)
{
	int sga, exa, fra;
	int valb;
	
	sga=(vala>>7)&1;
	exa=(vala>>3)&15;
	fra=8|(vala&7);
	
	valb=fra<<exa;
	if(sga)
		valb=-valb;
	return(valb);
}

int FixToFp8_E3M3(int val)
{
	int ex, fr, sg, valc;
	
	if(!val)
		return(0);
	
	sg=0; ex=0; fr=val;
	if(val<0)
		{ sg=1; fr=-val; }
	
	while(fr>>4)
		{ fr=fr>>1; ex++; }
	
	if(ex>7)
		return(0x3F);
	
	valc=(ex<<3)|(fr&7);
	return(valc);
}

int Conv_Fp16ToFp8(int val)
{
	int fra, frb, frc, exa, exb, exc, sga, sgb, sgc;
	int shra, shrb;
	int valc;

	sga=(val>>15)&1;
	exa=(val>>10)&31;
	fra=1024|(val&1023);

	sgc=sga;
	exc=exa-(15-7);
	frc=fra>>7;

	if(exc<0)
		return(0);
	if(exc>15)
		return((sgc<<7)|0x7F);
	
	valc=(sgc<<7)|(exc<<3)|(frc&7);
	return(valc);
}

int Conv_Fp8ToFp16(int val)
{
	int fra, frb, frc, exa, exb, exc, sga, sgb, sgc;
	int shra, shrb;
	int valc;

	sga=(val>>7)&1;
	exa=(val>>3)&15;
	fra=8|(val&7);

	sgc=sga;
	exc=exa+(15-7);
	frc=fra<<7;
	
	valc=(sgc<<15)|(exc<<10)|(frc&1023);
	return(valc);
}

u32 Conv_Fp16ToFp32(int val)
{
	u32 v;
	
	if(!((val>>10)&31))
		return(0);
		
	v=	((val&0x03FF)<<13)|
		((((val>>10)&31)+(127-31))<<23)|
		((val&0x8000U)<<16);
	return(v);
}

u16 Conv_Fp32ToFp16(u32 val)
{
	u32 v;
	
	if(!((val>>23)&255))
		return(0);
		
	v=	((val>>13)&0x03FF)|
		((((val>>23)&255)-(127-31))<<10);
	
	if(v>>31)
		v=0;
	if(v>>15)
		v=0x7C00;
	v|=((val>>16)&0x8000U);
	return(v);
}

float Conv_Fp16ToFloat(int val)
{
	float f;
	u32 v;
	v=Conv_Fp16ToFp32(val);
	f=0;
	memcpy(&f, &v, 4);
	return(f);
}

u16 Conv_FloatToFp16(float fval)
{
	u32 v;
	v=0;
	memcpy(&v, &fval, 4);
	return(Conv_Fp32ToFp16(v));
}

float Conv_Fp8ToFloat(int val)
{
	float f;
	u32 v;
	v=Conv_Fp8ToFp16(val);
	v=Conv_Fp16ToFp32(v);
	f=0;
	memcpy(&f, &v, 4);
	return(f);
}

static float *fp8tof_mtab;

void Init_Fp8ToFloat()
{
	int i, j;

	if(!fp8tof_mtab)
	{
		fp8tof_mtab=malloc(256*sizeof(float));
		for(i=0; i<256; i++)
			{ fp8tof_mtab[i]=Conv_Fp8ToFloat(i); }
	}
}

float Mul_Fp8ToFloat(byte vala, byte valb)
{
	return(fp8tof_mtab[vala]*fp8tof_mtab[valb]);
}

vec4f Mul_4xFp8To4xFloat(u32 vala, u32 valb)
{
	vec4f t;
	int i0, i1, i2, i3;
	float f0, f1, f2, f3;
	
	i0=(vala>> 0)&255;	i1=(vala>> 8)&255;
	i2=(valb>> 0)&255;	i3=(valb>> 8)&255;
	f0=fp8tof_mtab[i0];	f1=fp8tof_mtab[i1];
	f2=fp8tof_mtab[i2];	f3=fp8tof_mtab[i3];
	t.x=f0*f2;
	t.y=f1*f3;

	i0=(vala>>16)&255;	i1=(vala>>24)&255;
	i2=(valb>>16)&255;	i3=(valb>>24)&255;
	f0=fp8tof_mtab[i0];	f1=fp8tof_mtab[i1];
	f2=fp8tof_mtab[i2];	f3=fp8tof_mtab[i3];
	t.z=f0*f2;
	t.w=f1*f3;
	
	return(t);
}

int clamp16s(int val)
{
	int v;
	v=val;
	if(v<-32767)
		v=-32767;
	if(v> 32767)
		v=32767;
	return(v);
}

int simlz(byte *ibuf, int ibsz)
{
	static byte *hash[4096];	
	byte *cs, *cse, *cs1;
	int ml, md, mr, hi, tot;
	int i, j, k;
	
	for(i=0; i<4096; i++)
		hash[i]=NULL;
	
	tot=0; mr=0;
	cs=ibuf; cse=ibuf+ibsz;
	while(cs<cse)
	{
		hi=cs[0]^(cs[1]<<2)^(cs[2]<<4);
		cs1=hash[hi];
		if(!cs1)
		{
			hash[hi]=cs;
			mr++; cs++; continue;
		}
		for(j=0; j<256; j++)
		{
			if((cs+j)>=cse)
				break;
			if(cs[j]!=cs1[j])
				break;
		}
		ml=0; md=0;
		if(j>3)
			{ ml=j; md=cs-cs1; }
		
		if(ml && md)
		{
			tot+=2;
			if((ml>16) || (md>512))
				tot++;
			if(mr>=8)
				tot+=(mr/64);
			tot+=mr;

			for(j=0; j<ml; j++)
			{
				hi=cs[0]^(cs[1]<<2)^(cs[2]<<4);
				hash[hi]=cs;
				cs++;
			}

//			cs+=ml;
			mr=0;
			continue;
		}
		
		hash[hi]=cs;
		mr++;
		cs++;
	}

	if(mr>=8)
		tot+=(mr/64);
	tot+=mr;
	tot++;
	
	return(tot);
}

byte *GANN_LoadFile(char *name, int *rsz)
{
	byte *buf;
	FILE *fd;
	int sz, i;
	
	fd=fopen(name, "rb");
	if(!fd)
	{
		return(NULL);
	}
	fseek(fd, 0, 2);
	sz=ftell(fd);
	fseek(fd, 0, 0);
	buf=malloc(sz+24);
	i=fread(buf, 1, sz, fd);
	fclose(fd);
	
	if(i!=sz)
	{
		if(i>0)
		{
			sz=i;
		}else
		{
			free(buf);
			return(NULL);
		}
	}

	memset(buf+sz, 0, 16);
	
	*rsz=sz;
	return(buf);
}

byte *TkuTts_LoadFile(char *name, int *rsz)
{
	return(GANN_LoadFile(name, rsz));
}

int GANN_StoreFile(char *name, void *buf, int sz)
{
	FILE *fd;
	
	fd=fopen(name, "wb");
	if(!fd)
		return(-1);
	
	fwrite(buf, 1, sz, fd);
	fclose(fd);
	return(0);
}

char *strGetLine(char *cs, char *buf)
{
	char *ct;
	
	ct=buf;
	while(*cs)
	{
		if(*cs=='\r')	break;
		if(*cs=='\n')	break;
		*ct++=*cs++;
	}
	*ct++=0;

	if(*cs=='\r')	cs++;
	if(*cs=='\n')	cs++;
	return(cs);
}

typedef struct diphone_s diphone_t;
struct diphone_s {
diphone_t *next;
int pn0, pn1;

s64 stat_am[192*16];
s64 stat_fr[192*16];
int stat_n[192];

byte dat_am[192*8];
byte dat_fr[192*8];
byte dat_len;
};

diphone_t *diphones;
diphone_t *diphones_free;

diphone_t *GetDiphone(int pn0, int pn1)
{
	diphone_t *dcur;
	int i;
	
	dcur=diphones;
	while(dcur)
	{
		if((dcur->pn0==pn0) && (dcur->pn1==pn1))
			return(dcur);
		dcur=dcur->next;
	}
	
	if(diphones_free)
	{
		dcur=diphones_free;
		diphones_free=dcur->next;

		memset(dcur, 0, sizeof(diphone_t));
		dcur->pn0=pn0;
		dcur->pn1=pn1;
		dcur->next=diphones;
		diphones=dcur;
		return(dcur);
	}

	dcur=malloc(256*sizeof(diphone_t));
	memset(dcur, 0, 256*sizeof(diphone_t));
	for(i=0; i<254; i++)
	{
		dcur->next=diphones_free;
		diphones_free=dcur;
		dcur++;
	}
	memset(dcur, 0, sizeof(diphone_t));
	dcur->pn0=pn0;
	dcur->pn1=pn1;
	dcur->next=diphones;
	diphones=dcur;
	return(dcur);

#if 0
	dcur=malloc(sizeof(diphone_t));
	memset(dcur, 0, sizeof(diphone_t));
	dcur->pn0=pn0;
	dcur->pn1=pn1;
	dcur->next=diphones;
	diphones=dcur;
	return(dcur);
#endif
}

diphone_t *LookupDiphone(int pn0, int pn1)
{
	diphone_t *dcur;
	
	dcur=diphones;
	while(dcur)
	{
		if((dcur->pn0==pn0) && (dcur->pn1==pn1))
			return(dcur);
		dcur=dcur->next;
	}
	return(NULL);
}

int EncSwgBlock_4x(s16 *ibuf, int *tbf, int *tbv)
{
	int tmpbands[32];
	int s0, s1, s2, s3;
	int i0, i1, i2, i3;
	int i, j, k;

	encswg_1x(ibuf, tmpbands+ 0, tmpbands+ 1, 0,  8);
	encswg_1x(ibuf, tmpbands+ 2, tmpbands+ 3, 1,  8);
	encswg_1x(ibuf, tmpbands+ 4, tmpbands+ 5, 1, 12);
	encswg_1x(ibuf, tmpbands+ 6, tmpbands+ 7, 2, 12);
	encswg_1x(ibuf, tmpbands+ 8, tmpbands+ 9, 2, 16);
	encswg_1x(ibuf, tmpbands+10, tmpbands+11, 3, 16);
	encswg_1x(ibuf, tmpbands+12, tmpbands+13, 4, 16);
	encswg_1x(ibuf, tmpbands+14, tmpbands+15, 5, 16);

#if 0
	for(j=0; j<8; j++)
	{
		s1=tmpbands[j*2+0];
		if(s1>=0x40)
			tmpbands[j*2+1]=0;
	}
#endif

	for(j=0; j<8; j++)
		for(k=j+1; k<8; k++)
	{
		s1=tmpbands[j*2+1];
		s2=tmpbands[k*2+1];
		if((s2^(s2>>31))>(s1^(s1>>31)))
		{
			i0=tmpbands[j*2+0]; i1=tmpbands[j*2+1];
			i2=tmpbands[k*2+0]; i3=tmpbands[k*2+1];
			tmpbands[j*2+0]=i2; tmpbands[j*2+1]=i3;
			tmpbands[k*2+0]=i0; tmpbands[k*2+1]=i1;
		}
	}

#if 0
	for(j=0; j<4; j++)
		for(k=j+1; k<4; k++)
	{
		s1=tmpbands[j*2+0];
		s2=tmpbands[k*2+0];
		if(s2<s1)
		{
			i0=tmpbands[j*2+0]; i1=tmpbands[j*2+1];
			i2=tmpbands[k*2+0]; i3=tmpbands[k*2+1];
			tmpbands[j*2+0]=i2; tmpbands[j*2+1]=i3;
			tmpbands[k*2+0]=i0; tmpbands[k*2+1]=i1;
		}
	}
#endif

	for(j=0; j<4; j++)
	{
		k=FixToFp8(tmpbands[j*2+1]);
		tmpbands[j*2+1]=Fp8ToFix(k);
		
		tbf[j]=tmpbands[j*2+0];
		tbv[j]=k;
	}
	return(0);
}

int SwgUnpackHz(int fv)
{
	int m, e;
	
	e=(fv>>5)&7;
//	m=4096-((fv&31)<<6);
	m=8192-((fv&31)<<7);
	m>>=e;
	return(m);
}

int SwgPackHz(int hz)
{
	int e, m, v;
	
	if(!hz)
		return(255);
	m=hz; e=0;
//	while(m<=2048)
	while(m<=4096)
		{ m=m<<1; e++; }
	if(e>=8)
		return(255);
//	v=((4096-m)>>6)|(e<<5);
	v=((8192-m)>>7)|(e<<5);
	return(v);
}

int EncSwgBlock_Raw8x(s16 *ibuf, int *tbf, int *tbv)
{
	int tmpbands[32];
	int s0, s1, s2, s3;
	int i0, i1, i2, i3;
	int i, j, k;

//	encswg_1x(ibuf, tmpbands+ 0, tmpbands+ 1, 0, 12);
//	encswg_1x(ibuf, tmpbands+ 2, tmpbands+ 3, 1, 12);
//	encswg_1x(ibuf, tmpbands+ 4, tmpbands+ 5, 2, 12);
//	encswg_1x(ibuf, tmpbands+ 6, tmpbands+ 7, 3, 12);
//	encswg_1x(ibuf, tmpbands+ 8, tmpbands+ 9, 2, 24);
//	encswg_1x(ibuf, tmpbands+10, tmpbands+11, 3, 24);
//	encswg_1x(ibuf, tmpbands+12, tmpbands+13, 2, 48);
//	encswg_1x(ibuf, tmpbands+14, tmpbands+15, 3, 48);

	encswg_1xr(ibuf, tmpbands+ 0, tmpbands+ 1, 0x00, 0x0B);
	encswg_1xr(ibuf, tmpbands+ 2, tmpbands+ 3, 0x0C, 0x17);
	encswg_1xr(ibuf, tmpbands+ 4, tmpbands+ 5, 0x18, 0x23);
	encswg_1xr(ibuf, tmpbands+ 6, tmpbands+ 7, 0x23, 0x2F);
	encswg_1xr(ibuf, tmpbands+ 8, tmpbands+ 9, 0x30, 0x3B);
	encswg_1xr(ibuf, tmpbands+10, tmpbands+11, 0x3C, 0x47);
	encswg_1xr(ibuf, tmpbands+12, tmpbands+13, 0x48, 0x6B);
	encswg_1xr(ibuf, tmpbands+14, tmpbands+15, 0x6C, 0x8F);

	for(j=0; j<8; j++)
	{
		k=tmpbands[j*2+1];
		k=k^(k>>31);
		tbf[j]=SwgUnpackHz(tmpbands[j*2+0]);
		tbv[j]=k;
	}
	return(0);
}

int EncSwgBlock_CheckZoneExclude(int *tband, int zmin, int zmax)
{
	int f0, f1, v0, v1, fd, f2;
	
	f0=tband[0];	f1=tband[2];
	v0=tband[1];	v1=tband[3];
	if((f0<zmin) || (f0>zmax))	return(0);
	if((f1<zmin) || (f1>zmax))	return(0);
	
	fd=f0-f1;
	fd^=fd>>31;
	if(fd>4)
	{
		return(0);
	}
	
	f2=(f0+f1)>>2;
	
	if(v0>v1)
	{
		if((v0*0.75)<v1)
			tband[0]=f2;
		tband[2]=0; tband[3]=0;
	}
	else
	{
		if((v1*0.75)<v0)
			tband[2]=f2;
		tband[0]=0; tband[1]=0;
	}
	return(1);
}

int EncSwgBlock_Raw16x(s16 *ibuf, int *tbf, int *tbv)
{
	int tmpbands[32];
	int s0, s1, s2, s3;
	int i0, i1, i2, i3;
	int i, j, k;

	encswg_1xr(ibuf, tmpbands+ 0, tmpbands+ 1, 0x00, 0x07);
	encswg_1xr(ibuf, tmpbands+ 2, tmpbands+ 3, 0x08, 0x0F);
	encswg_1xr(ibuf, tmpbands+ 4, tmpbands+ 5, 0x10, 0x17);
	encswg_1xr(ibuf, tmpbands+ 6, tmpbands+ 7, 0x18, 0x1F);
	encswg_1xr(ibuf, tmpbands+ 8, tmpbands+ 9, 0x20, 0x27);
	encswg_1xr(ibuf, tmpbands+10, tmpbands+11, 0x28, 0x2F);
	encswg_1xr(ibuf, tmpbands+12, tmpbands+13, 0x30, 0x37);
	encswg_1xr(ibuf, tmpbands+14, tmpbands+15, 0x38, 0x3F);

	encswg_1xr(ibuf, tmpbands+16, tmpbands+17, 0x40, 0x47);
	encswg_1xr(ibuf, tmpbands+18, tmpbands+19, 0x48, 0x4F);
	encswg_1xr(ibuf, tmpbands+20, tmpbands+21, 0x50, 0x57);
	encswg_1xr(ibuf, tmpbands+22, tmpbands+23, 0x58, 0x5F);
	encswg_1xr(ibuf, tmpbands+24, tmpbands+25, 0x60, 0x67);
	encswg_1xr(ibuf, tmpbands+26, tmpbands+27, 0x68, 0x6F);
	encswg_1xr(ibuf, tmpbands+28, tmpbands+29, 0x70, 0x77);
//	encswg_1xr(ibuf, tmpbands+30, tmpbands+31, 0x78, 0x7F);
	encswg_1xr(ibuf, tmpbands+30, tmpbands+31, 0x78, 0x8F);

#if 1
	EncSwgBlock_CheckZoneExclude(tmpbands+ 0, 0x04, 0x0B);
	EncSwgBlock_CheckZoneExclude(tmpbands+ 2, 0x0C, 0x13);
	EncSwgBlock_CheckZoneExclude(tmpbands+ 4, 0x14, 0x1B);
	EncSwgBlock_CheckZoneExclude(tmpbands+ 6, 0x1C, 0x23);
	EncSwgBlock_CheckZoneExclude(tmpbands+ 8, 0x24, 0x2B);
	EncSwgBlock_CheckZoneExclude(tmpbands+10, 0x2C, 0x33);
	EncSwgBlock_CheckZoneExclude(tmpbands+12, 0x34, 0x3B);
	EncSwgBlock_CheckZoneExclude(tmpbands+14, 0x3C, 0x43);
	EncSwgBlock_CheckZoneExclude(tmpbands+16, 0x44, 0x4B);
	EncSwgBlock_CheckZoneExclude(tmpbands+18, 0x4C, 0x53);
	EncSwgBlock_CheckZoneExclude(tmpbands+20, 0x54, 0x5B);
	EncSwgBlock_CheckZoneExclude(tmpbands+22, 0x5C, 0x63);
	EncSwgBlock_CheckZoneExclude(tmpbands+24, 0x64, 0x6B);
	EncSwgBlock_CheckZoneExclude(tmpbands+26, 0x6C, 0x73);
	EncSwgBlock_CheckZoneExclude(tmpbands+28, 0x74, 0x7B);
#endif

	for(j=0; j<16; j++)
	{
		k=tmpbands[j*2+1];
		k=k^(k>>31);
		tbf[j]=SwgUnpackHz(tmpbands[j*2+0]);
		tbv[j]=k;
	}
	return(0);
}

int DecSwgBlock_2x(s16 *obuf, byte *tbf, byte *tbv)
{
	int tmpdbuf[2*BLKSIZE];
	int tmpbands[8];
	int j, k;

	tmpbands[0]=tbf[0];
	tmpbands[1]=tbv[0];
	tmpbands[2]=tbf[1];
	tmpbands[3]=tbv[1];

	for(j=0; j<(2*BLKSIZE); j++)
		tmpdbuf[j]=0;
	decswg_1x(tmpdbuf, tmpbands+0, tmpbands+1);
	decswg_1x(tmpdbuf, tmpbands+2, tmpbands+3);

	for(j=0; j<(BLKSIZE/2); j++)
	{
		k=(2*BLKSIZE)-(j+1);
		tmpdbuf[j]=(tmpdbuf[j]*j)/(BLKSIZE/2);
		tmpdbuf[k]=(tmpdbuf[k]*j)/(BLKSIZE/2);
	}

	for(j=0; j<2*BLKSIZE; j++)
	{
		obuf[j]=clamp16s(obuf[j]+tmpdbuf[j]);
//		obuf[j]=clamp16s(tmpdbuf[j]);
	}
	return(0);
}

int DecSwgBlock_4x(s16 *obuf, byte *tbf, byte *tbv)
{
	int tmpdbuf[2*BLKSIZE];
	int tmpbands[8];
	int j, k;

	tmpbands[0]=tbf[0];
	tmpbands[1]=tbv[0];
	tmpbands[2]=tbf[1];
	tmpbands[3]=tbv[1];
	tmpbands[4]=tbf[2];
	tmpbands[5]=tbv[2];
	tmpbands[6]=tbf[3];
	tmpbands[7]=tbv[3];

	for(j=0; j<(2*BLKSIZE); j++)
		tmpdbuf[j]=0;
	decswg_1x(tmpdbuf, tmpbands+0, tmpbands+1);
	decswg_1x(tmpdbuf, tmpbands+2, tmpbands+3);
	decswg_1x(tmpdbuf, tmpbands+4, tmpbands+5);
	decswg_1x(tmpdbuf, tmpbands+6, tmpbands+7);

	for(j=0; j<(BLKSIZE/2); j++)
	{
		k=(2*BLKSIZE)-(j+1);
		tmpdbuf[j]=(tmpdbuf[j]*j)/(BLKSIZE/2);
		tmpdbuf[k]=(tmpdbuf[k]*j)/(BLKSIZE/2);
	}

	for(j=0; j<2*BLKSIZE; j++)
	{
		obuf[j]=clamp16s(obuf[j]+tmpdbuf[j]);
//		obuf[j]=clamp16s(tmpdbuf[j]);
	}
	return(0);
}

int DecSwgBlock_6x(s16 *obuf, byte *tbf, byte *tbv)
{
	int tmpdbuf[2*BLKSIZE+64];
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

	for(j=0; j<(2*BLKSIZE); j++)
		tmpdbuf[j]=0;
	decswg_1x(tmpdbuf, tmpbands+ 0, tmpbands+ 1);
	decswg_1x(tmpdbuf, tmpbands+ 2, tmpbands+ 3);
	decswg_1x(tmpdbuf, tmpbands+ 4, tmpbands+ 5);
	decswg_1x(tmpdbuf, tmpbands+ 6, tmpbands+ 7);
	decswg_1x(tmpdbuf, tmpbands+ 8, tmpbands+ 9);
	decswg_1x(tmpdbuf, tmpbands+10, tmpbands+11);

	for(j=0; j<(BLKSIZE/2); j++)
	{
		k=(2*BLKSIZE)-(j+1);
		tmpdbuf[j]=(tmpdbuf[j]*j)/(BLKSIZE/2);
		tmpdbuf[k]=(tmpdbuf[k]*j)/(BLKSIZE/2);
	}

	for(j=0; j<2*BLKSIZE; j++)
	{
		obuf[j]=clamp16s(obuf[j]+tmpdbuf[j]);
//		obuf[j]=clamp16s(tmpdbuf[j]);
	}
	return(0);
}

s16 *vox_pcmbuf[2048];
int vox_pcmbuf_vsi[2048];
int vox_pcmbuf_vsn[2048];
int vox_pcmbuf_len[2048];
int n_vox;

int vox_samp_i0[2048*64];
int vox_samp_i1[2048*64];
int vox_samp_pn[2048*64];
int n_vox_samp;

int ProcessVoices()
{
	char tbuf[256], t1buf[256];
	int samp_i0[64];
	int samp_i1[64];
	int samp_pn[64];
	diphone_t *dcur;
	char *txbuf, *tybuf;
	char *cs, *cse, *cs1;
	s16 *ibuf;
	int sz, sz1, rov, ilen, ns, ni, nb, ns1, nb1;
	int ix0, ix1, ix2, ix3;
	s64 d0, d1, d2, d3;
	int i, j, k, i1, j1, k1, po;

	txbuf=GANN_LoadFile(
		"fsew0_v1.1/fsew0_MFCC_E_D_A_phoneme_labels.mlf", &sz);

	cs=txbuf;
	cse=cs+sz;
	rov=0;
	ibuf=NULL;
	
	while(cs<cse)
	{
		cs=strGetLine(cs, tbuf);
		if(!tbuf[0])
			continue;
		if(tbuf[0]=='#')
			continue;
		if(tbuf[0]=='\"')
		{
			if(ibuf)
			{
				free(ibuf);
				ibuf=NULL;
			}
		
			rov++;
			sprintf(t1buf, "%s/%s_%03u.wav", "fsew0_v1.1", "fsew0", rov);
//			ibuf=BGBMID_LoadWAV_16Mono16(t1buf, &ilen);

			printf("%s\n", t1buf);

			tybuf=GANN_LoadFile(t1buf, &sz1);
			if(tybuf)
			{
				ilen=(sz1-1024)/2;
				ibuf=malloc((ilen+512)*2);
				memcpy(ibuf, tybuf+1024, ilen*2);
				free(tybuf);
				
				for(i=0; i<16; i++)
					BGBMID_HighPassSample(ibuf, ibuf, ilen);

//				BGBMID_HighPassSample(ibuf, ibuf, ilen);
//				BGBMID_HighPassSample(ibuf, ibuf, ilen);
//				BGBMID_HighPassSample(ibuf, ibuf, ilen);
				BGBMID_NormalizeSample(ibuf, ibuf, ilen);

#if 0
				sprintf(t1buf, "%s/%s_%03u_filt.wav",
					"diphone0", "fsew0", rov-1);
				BGBMID_StoreWAV(t1buf, (byte *)ibuf, 1, 16000, 16, 
					ilen);
#endif
			}
			
			ns=0;
			continue;
		}

		if(tbuf[0]=='.')
		{
			j=n_vox_samp;
			k=n_vox++;
			vox_pcmbuf[k]=ibuf;
			vox_pcmbuf_len[k]=ilen;
			vox_pcmbuf_vsi[k]=j;
			vox_pcmbuf_vsn[k]=ns;
			
			for(i=0; i<ns; i++)
			{
				vox_samp_i0[j+i]=samp_i0[i];
				vox_samp_i1[j+i]=samp_i1[i];
				vox_samp_pn[j+i]=samp_pn[i];
			}
			
			n_vox_samp+=ns;
			
			ibuf=NULL;
		
			continue;
		}
		
		sscanf(tbuf, "%d %d %s", &ix0, &ix1, t1buf);
		
		ix2=-1;
		if(!strcmp(t1buf, "breath"))	ix2=0;
		if(!strcmp(t1buf, "sil"))		ix2=0;

		if(!strcmp(t1buf, "b"))			ix2='b';
		if(!strcmp(t1buf, "d"))			ix2='d';
		if(!strcmp(t1buf, "f"))			ix2='f';
		if(!strcmp(t1buf, "g"))			ix2='g';

		if(!strcmp(t1buf, "h"))			ix2='h';
		if(!strcmp(t1buf, "k"))			ix2='k';
		if(!strcmp(t1buf, "l"))			ix2='l';

		if(!strcmp(t1buf, "r"))			ix2='r';
		if(!strcmp(t1buf, "m"))			ix2='m';
		if(!strcmp(t1buf, "n"))			ix2='n';
		if(!strcmp(t1buf, "p"))			ix2='p';
		if(!strcmp(t1buf, "r"))			ix2='r';
		if(!strcmp(t1buf, "s"))			ix2='s';
		if(!strcmp(t1buf, "t"))			ix2='t';
		if(!strcmp(t1buf, "v"))			ix2='v';
		if(!strcmp(t1buf, "w"))			ix2='w';
		if(!strcmp(t1buf, "y"))			ix2='y';
		if(!strcmp(t1buf, "z"))			ix2='z';

		if(!strcmp(t1buf, "a"))			ix2='a';
		if(!strcmp(t1buf, "e"))			ix2='e';
		if(!strcmp(t1buf, "i"))			ix2='i';
		if(!strcmp(t1buf, "o"))			ix2='o';
		if(!strcmp(t1buf, "u"))			ix2='u';
		if(!strcmp(t1buf, "@"))			ix2='@';

		if(!strcmp(t1buf, "ch"))		ix2='C';
		if(!strcmp(t1buf, "dh"))		ix2='D';
		if(!strcmp(t1buf, "jh"))		ix2='J';
		if(!strcmp(t1buf, "ng"))		ix2='G';
		if(!strcmp(t1buf, "sh"))		ix2='S';
		if(!strcmp(t1buf, "th"))		ix2='T';
		if(!strcmp(t1buf, "wh"))		ix2='H';
		if(!strcmp(t1buf, "zh"))		ix2='Z';

		if(!strcmp(t1buf, "aa"))		ix2='A';
		if(!strcmp(t1buf, "ii"))		ix2='I';
		if(!strcmp(t1buf, "uu"))		ix2='U';
		if(!strcmp(t1buf, "oo"))		ix2='O';
		if(!strcmp(t1buf, "@@"))		ix2='@';


		if(!strcmp(t1buf, "ai"))		ix2='A'|('I'<<8);
		if(!strcmp(t1buf, "ei"))		ix2='e'|('I'<<8);
		if(!strcmp(t1buf, "oi"))		ix2='O'|('I'<<8);

		if(!strcmp(t1buf, "au"))		ix2='A'|('@'<<8);
		if(!strcmp(t1buf, "ou"))		ix2='O'|('@'<<8);
		if(!strcmp(t1buf, "e@"))		ix2='e'|('@'<<8);
		if(!strcmp(t1buf, "i@"))		ix2='I'|('@'<<8);
		if(!strcmp(t1buf, "u@"))		ix2='U'|('@'<<8);

		if(!strcmp(t1buf, "eir"))		ix2='R';

		if(!strcmp(t1buf, "iy"))		ix2='I'|('h'<<8);
		if(!strcmp(t1buf, "uh"))		ix2='U'|('h'<<8);
		if(!strcmp(t1buf, "ow"))		ix2='W';

//		if(!strcmp(t1buf, "ae"))		ix2='@';
//		if(!strcmp(t1buf, "ux"))		ix2='u';

		if(ix2<0)
		{
			printf("Miss %s\n", t1buf);
		}

		if(ix2>0)
		{
			samp_i0[ns]=ix0*(16000/10000000.0);
			samp_i1[ns]=ix1*(16000/10000000.0);
			samp_pn[ns]=ix2;
			ns++;
		}
	}

	free(txbuf);
	txbuf=NULL;
	
	po=0;
	
	for(i=0; i<n_vox; i++)
	{
		ni=vox_pcmbuf_vsi[i];
		ns=vox_pcmbuf_vsn[i];

		for(j=0; j<(ns-1); j++)
		{
			printf("%d/%d %d/%d  po=%d  \r", i, n_vox, j, ns, po);
			fflush(stdout);
		
			ix0=vox_samp_pn[ni+j+0];
			ix1=vox_samp_pn[ni+j+1];
			dcur=GetDiphone(ix0, ix1);
			ix2=vox_samp_i0[ni+j+0];
			ix3=vox_samp_i1[ni+j+1];
			
			nb=(ix3-ix2)/BLKSIZE;
			if((nb<=0) || (nb>=128))
				continue;

#if 1
//			if(dcur->stat_n[0])
//				continue;
			
			if(dcur->dat_len)
			{
				k=nb-dcur->dat_len;
				if(k<0)		k=-k;
//				if(k>12)
				if(k>6)
					continue;
			}
#endif

			po=0;

			if(dcur->dat_len)
			{
				k=nb-dcur->dat_len;
				po=k/2;
			}

#if 0
			j=dcur->stat_n[0];
			if(j>0)
			{
				for(j1=0; j1<8; j1++)
				{
					samp_i1[16+j1]=dcur->stat_am[0*8+j1]/j;
					d3=dcur->stat_fr[0*8+j1]/j;
					if(samp_i1[16+j1])
						d3=d3/samp_i1[16+j1];
					samp_i0[16+j1]=d3;
				}

//				for(po=0; po<8; po++)
				while(1)
				{
					if((po>=8) || (po<(-8)))
						break;

					EncSwgBlock_Raw8x(
						vox_pcmbuf[i]+ix2+(po+0)*BLKSIZE,
						samp_i0+0, samp_i1+0);
					EncSwgBlock_Raw8x(
						vox_pcmbuf[i]+ix2+(po+1)*BLKSIZE,
						samp_i0+8, samp_i1+8);
					EncSwgBlock_Raw8x(
						vox_pcmbuf[i]+ix2+(po-1)*BLKSIZE,
						samp_i0+24, samp_i1+24);

					d0=0; d1=0; d2=0;
					for(j1=0; j1<8; j1++)
					{
						d3=samp_i0[ 0+j1]-samp_i0[16+j1]; d3^=d3>>63; d0+=d3;
						d3=samp_i1[ 0+j1]-samp_i1[16+j1]; d3^=d3>>63; d0+=d3;
						d3=samp_i0[ 8+j1]-samp_i0[16+j1]; d3^=d3>>63; d1+=d3;
						d3=samp_i1[ 8+j1]-samp_i1[16+j1]; d3^=d3>>63; d1+=d3;
						d3=samp_i0[24+j1]-samp_i0[16+j1]; d3^=d3>>63; d2+=d3;
						d3=samp_i1[24+j1]-samp_i1[16+j1]; d3^=d3>>63; d2+=d3;
					}
					if(d1<d0)
						{ po++; continue; }
					if(d2<d0)
						{ po--; continue; }
					break;
				}
				if((po>=8) || (po<(-8)))
					continue;
			}
#endif			
				
			for(k=0; k<nb; k++)
			{
#if 1
				EncSwgBlock_Raw16x(
					vox_pcmbuf[i]+ix2+(k+po)*BLKSIZE,
					samp_i0, samp_i1);

				for(j1=0; j1<16; j1++)
				{
					dcur->stat_fr[k*16+j1]+=((s64)samp_i0[j1])*samp_i1[j1];
//					dcur->stat_fr[k*16+j1]+=samp_i0[j1];
					dcur->stat_am[k*16+j1]+=samp_i1[j1];
				}
				dcur->stat_n[k]++;
#endif
			}
			dcur->dat_len=nb;
		}
	}
	printf("\n");


#if 1
	for(i=0; i<n_vox; i++)
	{
		ni=vox_pcmbuf_vsi[i];
		ns=vox_pcmbuf_vsn[i];

		for(j=0; j<ns; j++)
		{
			printf("%d/%d %d/%d  po=%d  \r", i, n_vox, j, ns, po);
			fflush(stdout);
		
			ix0=vox_samp_pn[ni+j+0];
			ix1=vox_samp_pn[ni+j+0];
			dcur=GetDiphone(ix0, 0);
			ix2=vox_samp_i0[ni+j+0];
			ix3=vox_samp_i1[ni+j+0];
			
			nb=(ix3-ix2)/BLKSIZE;
			if((nb<=0) || (nb>=128))
				continue;

#if 1
//			if(dcur->stat_n[0])
//				continue;
			
			if(dcur->dat_len)
			{
				k=nb-dcur->dat_len;
				if(k<0)		k=-k;
//				if(k>12)
				if(k>6)
					continue;
			}
#endif

			po=0;				

			if(dcur->dat_len)
			{
				k=nb-dcur->dat_len;
				po=k/2;
			}

			for(k=0; k<nb; k++)
			{
				EncSwgBlock_Raw16x(
					vox_pcmbuf[i]+ix2+(k+po)*BLKSIZE,
					samp_i0, samp_i1);

				for(j1=0; j1<16; j1++)
				{
					dcur->stat_fr[k*16+j1]+=((s64)samp_i0[j1])*samp_i1[j1];
					dcur->stat_am[k*16+j1]+=samp_i1[j1];
				}
				dcur->stat_n[k]++;
			}
			dcur->dat_len=nb;
		}
	}
	printf("\n");
#endif

	nb=0; ns=0;
	dcur=diphones;
	while(dcur)
	{
		for(k=0; k<128; k++)
		{
			j=dcur->stat_n[k];
			if(!j)
				break;

#if 1
			for(j1=0; j1<16; j1++)
			{
				samp_i1[j1]=dcur->stat_am[k*16+j1]/j;
				d3=dcur->stat_fr[k*16+j1]/j;
				if(samp_i1[j1]>0)
					d3=d3/samp_i1[j1];
				samp_i0[j1]=d3;
			}
#endif

#if 0
			ix0=0;
			ix1=0;
			for(j1=0; j1<4; j1++)
			{
				if(samp_i1[ 8+j1]>samp_i1[ 8+ix0])	ix0=j1;
				if(samp_i1[12+j1]>samp_i1[12+ix1])	ix1=j1;
			}
			for(j1=0; j1<4; j1++)
			{
				if(j1!=ix0)		samp_i1[ 8+j1]=0;
				if(j1!=ix1)		samp_i1[12+j1]=0;
			}
#endif

#if 1
			ix0=samp_i1[ 8+0]>samp_i1[ 8+1];	samp_i1[ 8+0+ix0]=0;
			ix0=samp_i1[ 8+2]>samp_i1[ 8+3];	samp_i1[ 8+2+ix0]=0;
			ix0=samp_i1[12+0]>samp_i1[12+1];	samp_i1[12+0+ix0]=0;
			ix0=samp_i1[12+2]>samp_i1[12+3];	samp_i1[12+2+ix0]=0;
#endif

			for(j1=0; j1<16; j1++)
				for(k1=j1+1; k1<16; k1++)
			{
				if(samp_i1[k1]>samp_i1[j1])
				{
					i1=samp_i0[k1]; samp_i0[k1]=samp_i0[j1]; samp_i0[j1]=i1;
					i1=samp_i1[k1]; samp_i1[k1]=samp_i1[j1]; samp_i1[j1]=i1;
				}
			}

			for(j1=0; j1<16; j1++)
			{
				samp_i1[j1]>>=3;
				
//				if(FixToFp8_E3M3(samp_i1[j1])>=0x3F)
//					printf("OOR %d\n", samp_i1[j1]);
			}

			for(j1=0; j1<8; j1++)
			{
				dcur->dat_fr[k*8+j1]=SwgPackHz(samp_i0[j1]);
//				dcur->dat_am[k*8+j1]=FixToFp8(samp_i1[j1]);
				dcur->dat_am[k*8+j1]=FixToFp8_E3M3(samp_i1[j1]);
				if(dcur->dat_am[k*8+j1]==0)
					dcur->dat_fr[k*8+j1]=0;

				if(dcur->dat_am[k*8+j1]<(dcur->dat_am[k*8+0]-(3<<3)))
				{
					dcur->dat_am[k*8+j1]=0;
					dcur->dat_fr[k*8+j1]=0;
				}
			}

			nb++;
		}
		dcur->dat_len=k;
		ns++;
		dcur=dcur->next;
	}
	
#ifdef DO_6BANDS
	sz=((ns+16)*16)+(nb*12);
#else
	sz=((ns+16)*16)+(nb*8);
#endif

	txbuf=malloc(sz+4096+65536);
	memset(txbuf, 0, sz);

	dcur=diphones;
	ns1=0; nb1=0;
	ni=(ns+4)*16;

#ifdef DO_2BANDS
	ni=((ni+31)/16)*4;
#endif

#ifdef DO_4BANDS
	ni=((ni+31)/16)*2;
#endif

#ifdef DO_6BANDS
	ni=(ni+47)/12;
#endif

	ns1++;
	txbuf[0]='D';
	txbuf[1]='I';
	txbuf[2]='P';
	txbuf[3]='H';
	txbuf[4]='L';
	txbuf[5]='I';
	txbuf[6]='B';
	txbuf[7]='1';

	txbuf[ 8]=ns>> 0;
	txbuf[ 9]=ns>> 8;
	txbuf[10]=ns>>16;
	txbuf[11]=ns>>24;

	while(dcur)
	{
//		*(int *)(txbuf+(ns1*16)+ 0)=dcur->pn0;
//		*(int *)(txbuf+(ns1*16)+ 4)=dcur->pn1;
//		*(int *)(txbuf+(ns1*16)+ 8)=(ni+nb1)*3;
//		*(int *)(txbuf+(ns1*16)+12)=dcur->dat_len;

		*(u16 *)(txbuf+(ns1*16)+ 0)=dcur->pn0;
		*(u16 *)(txbuf+(ns1*16)+ 2)=dcur->pn1;
		*(u16 *)(txbuf+(ns1*16)+ 4)=dcur->dat_len;

#ifdef DO_2BANDS
		*(byte *)(txbuf+(ns1*16)+ 6)=2;
		*(byte *)(txbuf+(ns1*16)+ 7)=0;
		*(u32 *)(txbuf+(ns1*16)+ 8)=(ni+nb1)*4;
#endif

#ifdef DO_4BANDS
		*(byte *)(txbuf+(ns1*16)+ 6)=4;
		*(byte *)(txbuf+(ns1*16)+ 7)=0;
		*(u32 *)(txbuf+(ns1*16)+ 8)=(ni+nb1)*8;
#endif

#ifdef DO_6BANDS
		*(byte *)(txbuf+(ns1*16)+ 6)=6;
		*(byte *)(txbuf+(ns1*16)+ 7)=0;
		*(u32 *)(txbuf+(ns1*16)+ 8)=(ni+nb1)*12;
#endif

#if (BLKSIZE==128)
		*(byte *)(txbuf+(ns1*16)+ 6)|=0x40;
#endif

#if (BLKSIZE==256)
		*(byte *)(txbuf+(ns1*16)+ 6)|=0x80;
#endif

#if (BLKSIZE==512)
		*(byte *)(txbuf+(ns1*16)+ 6)|=0xC0;
#endif

#ifdef DO_BAND_DELTA
		*(byte *)(txbuf+(ns1*16)+ 6)|=32;
#endif

		for(k=0; k<dcur->dat_len; k++)
		{			
#ifdef DO_2BANDS
			cs=txbuf+((ni+nb1)*4);
			cs[0]=dcur->dat_am[k*8+0];
			cs[1]=dcur->dat_am[k*8+1];
			cs[2]=dcur->dat_fr[k*8+0];
			cs[3]=dcur->dat_fr[k*8+1];
#endif

#ifdef DO_4BANDS
			cs=txbuf+((ni+nb1)*8);
			cs[0]=dcur->dat_am[k*8+0];
			cs[1]=dcur->dat_am[k*8+1];
			cs[2]=dcur->dat_am[k*8+2];
			cs[3]=dcur->dat_am[k*8+3];
			cs[4]=dcur->dat_fr[k*8+0];
			cs[5]=dcur->dat_fr[k*8+1];
			cs[6]=dcur->dat_fr[k*8+2];
			cs[7]=dcur->dat_fr[k*8+3];

#ifdef DO_BAND_DELTA
			if(k>0)
			{
				cs1=cs-8;
				cs[0]-=cs1[0];	cs[1]-=cs1[1];
				cs[2]-=cs1[2];	cs[3]-=cs1[3];
				cs[4]-=cs1[4];	cs[5]-=cs1[5];
				cs[6]-=cs1[6];	cs[7]-=cs1[7];
			}
#endif

#endif

#ifdef DO_6BANDS
			cs=txbuf+((ni+nb1)*12);
			cs[ 0]=dcur->dat_am[k*8+0];
			cs[ 1]=dcur->dat_am[k*8+1];
			cs[ 2]=dcur->dat_am[k*8+2];
			cs[ 3]=dcur->dat_am[k*8+3];
			cs[ 4]=dcur->dat_am[k*8+4];
			cs[ 5]=dcur->dat_am[k*8+5];
			cs[ 6]=dcur->dat_fr[k*8+0];
			cs[ 7]=dcur->dat_fr[k*8+1];
			cs[ 8]=dcur->dat_fr[k*8+2];
			cs[ 9]=dcur->dat_fr[k*8+3];
			cs[10]=dcur->dat_fr[k*8+4];
			cs[11]=dcur->dat_fr[k*8+5];

#ifdef DO_BAND_DELTA
			if(k>0)
			{
				cs1=cs-12;
				cs[ 0]-=cs1[ 0];	cs[ 1]-=cs1[ 1];
				cs[ 2]-=cs1[ 2];	cs[ 3]-=cs1[ 3];
				cs[ 4]-=cs1[ 4];	cs[ 5]-=cs1[ 5];
				cs[ 6]-=cs1[ 6];	cs[ 7]-=cs1[ 7];
				cs[ 8]-=cs1[ 8];	cs[ 9]-=cs1[ 9];
				cs[10]-=cs1[10];	cs[11]-=cs1[11];
			}
#endif
#endif

			nb1++;
		}
		ns1++;
		dcur=dcur->next;
	}

#ifdef DO_6BANDS
	sz=(ni+nb1)*12;
#endif

#ifdef DO_4BANDS
	sz=(ni+nb1)*8;
#endif

#ifdef DO_2BANDS
	sz=(ni+nb1)*4;
#endif


	txbuf[12]=sz>> 0;
	txbuf[13]=sz>> 8;
	txbuf[14]=sz>>16;
	txbuf[15]=sz>>24;

	GANN_StoreFile("diphones0.dpl", txbuf, sz);

#if 1
	ilen=(nb+2)*BLKSIZE;
	ibuf=malloc((ilen+16384)*2);
	memset(ibuf, 0, ilen*2);
	
	for(i=0; i<nb; i++)
	{
#ifdef DO_2BANDS
		cs=txbuf+((ni+i)*4);
		DecSwgBlock_2x(ibuf+(i+1)*BLKSIZE, (byte *)(cs+2), (byte *)(cs+0));
#endif

#ifdef DO_4BANDS
		cs=txbuf+((ni+i)*8);
		DecSwgBlock_4x(ibuf+(i+1)*BLKSIZE, (byte *)(cs+4), (byte *)(cs+0));
//		DecSwgBlock_6x(ibuf+(i+1)*BLKSIZE, (byte *)(cs+6), (byte *)(cs+0));
#endif

#ifdef DO_6BANDS
		cs=txbuf+((ni+i)*12);
		DecSwgBlock_6x(ibuf+(i+1)*BLKSIZE, (byte *)(cs+6), (byte *)(cs+0));
#endif
	}

	BGBMID_NormalizeSample(ibuf, ibuf, ilen);
	BGBMID_StoreWAV("tst0_diphone0.wav", (byte *)ibuf, 1, 11025, 16, ilen);

	free(ibuf);
#endif

#if 0
	for(i=0; i<n_vox; i++)
	{
		ni=vox_pcmbuf_vsi[i];
		ns=vox_pcmbuf_vsn[i];

		memset(vox_pcmbuf[i], 0, vox_pcmbuf_len[i]*2);

		printf("regen %d/%d\r", i, n_vox);
		fflush(stdout);

		for(j=0; j<(ns-1); j++)
		{
//			printf("%d/%d %d/%d\r", i, n_vox, j, ns);
//			fflush(stdout);
		
			ix0=vox_samp_pn[ni+j+0];
			ix1=vox_samp_pn[ni+j+1];
			dcur=GetDiphone(ix0, ix1);
			ix2=vox_samp_i0[ni+j+0];
			ix3=vox_samp_i1[ni+j+1];
			
			nb=(ix3-ix2)/BLKSIZE;
			if((nb<=0) || (nb>=128))
				continue;
			for(k=0; k<nb; k++)
			{
//				DecSwgBlock_4x(
//					vox_pcmbuf[i]+ix2+k*BLKSIZE,
//					dcur->dat_fr+k*8,
//					dcur->dat_am+k*8);

				DecSwgBlock_6x(
					vox_pcmbuf[i]+ix2+k*BLKSIZE,
					dcur->dat_fr+k*8,
					dcur->dat_am+k*8);
			}

			sprintf(t1buf, "%s/%s_%03u_rgn.wav", "diphone0", "fsew0", i);
			BGBMID_StoreWAV(t1buf, (byte *)vox_pcmbuf[i], 1, 11025, 16, 
				vox_pcmbuf_len[i]);
		}
	}
#endif
}

int TestGenerate(s16 *obuf, char *str)
{
	diphone_t *dcur;
	s16 *ct;
	char *cs;
	int i0, i1, i2, i3;
	int k, nb;
	
	cs=str; ct=obuf;
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
		i1=cs[1];
		dcur=LookupDiphone(i0, i1);
		if(!dcur)
		{
			i1=0;
			dcur=LookupDiphone(i0, 0);
			if(!dcur)
			{
				cs++;
				continue;
			}
		}

		nb=dcur->dat_len;

		if(i0 && i1)
		{
			i2=(nb/4);
			i3=nb-(nb/4);
		}else
		{
			i2=(nb/2);
			i3=nb;
		}

//		for(k=0; k<nb; k++)
		for(k=i2; k<i3; k++)
		{
#ifdef DO_2BANDS
			DecSwgBlock_2x(
				ct+(k-i2)*BLKSIZE,
				dcur->dat_fr+k*8,
				dcur->dat_am+k*8);
#endif

#ifdef DO_4BANDS
			DecSwgBlock_4x(
				ct+(k-i2)*BLKSIZE,
				dcur->dat_fr+k*8,
				dcur->dat_am+k*8);
#endif

#ifdef DO_6BANDS
			DecSwgBlock_6x(
				ct+(k-i2)*BLKSIZE,
				dcur->dat_fr+k*8,
				dcur->dat_am+k*8);
#endif
		}
//		ct+=nb*BLKSIZE;
		ct+=(i3-i2)*BLKSIZE;
		cs++;
	}
	return(ct-obuf);
}

int main(int argc, char *argv[])
{
	int tmpbands[128];
	int tmpdbuf[512];
	char t1buf[256];
	char t2buf[256];
	static byte prbuf[8192];

	TkuTts_WordDict *dict;
	TkuTts_DiphLib *dlib;
	TkuTts_PcmState *dpcm;

	s16 *bands, *tbct;
	byte *tbuf, *tbuct, *tbucs;
	s16 *ibuf, *obuf, *o2buf;
	char *ifn, *ofn;
	char *cs, *ct;
	u64 tblk, tblk1;
	int ilen, acc1, acc2, dsc, dsc1, tts_rate, tts_freq;
	int s0, s1, s2, s3, t0, t1, t2, t3;
	int i0, i1, i2, i3;
	int j0, j1, j2, j3;
	int i4, i5, i6, i7;
	int s4, s5, s6, s7;
	int t4, t5, t6, t7;
	float f, g, h;
	s64 err;
	int i, j, k, n;
	
	ifn=NULL;
	ofn=NULL;
	
//	buildbandtab();

	tts_rate=64;
	tts_freq=0;

	for(i=1; i<argc; i++)
	{
		if(argv[i][0]=='-')
		{
			if(!strcmp(argv[i], "--rate"))
			{
//				tts_rate=atoi(argv[i+1]);
//				tts_rate=64-(atoi(argv[i+1])-64);
				tts_rate=64*(100.0/atoi(argv[i+1]));
				i++;
				continue;
			}
			if(!strcmp(argv[i], "--freq"))
			{
				tts_freq=-atoi(argv[i+1]);
				i++;
				continue;
			}
			continue;
		}
		
		if(!ifn)
		{
			ifn=argv[i];
			continue;
		}

		if(!ofn)
		{
			ofn=argv[i];
			continue;
		}
	}


	dict=TkuTts_LoadDict("worddict0.txt");

	cs="this is a test; 1; 2; 3. I got that skibidi rizz."
		" skibidi; dom; yes, yes; skibidi skibidi neep neep.";
	ct=t1buf;
	TkuTts_TranscribePhrase(dict, cs, &ct);

	obuf=malloc(30*16000*2);
	memset(obuf, 0, 10*16000*2);

	dlib=TkuTts_LoadDiphLib("diphones0.dpl");
	dpcm=TkuTts_PcmState_Create();
	if(dlib && dpcm)
	{
		dpcm->ratescale=tts_rate;
		dpcm->freqadj=tts_freq;
	
		n=TkuTts_PcmRunPhonetic(dpcm, dlib, obuf, t1buf);
		BGBMID_NormalizeSample(obuf, obuf, n);
//		BGBMID_StoreWAV("tts_test1.wav", (byte *)obuf, 1, 11025, 16,  n);
		BGBMID_StoreWAV("tts_test1.wav", (byte *)obuf, 1, 16000, 16,  n);
	}
	
#if 0
	ProcessVoices();

//	obuf=malloc(10*16000*2);
	memset(obuf, 0, 10*16000*2);
	n=TestGenerate(obuf, "Tiziz@test won tU TrI ");

	BGBMID_NormalizeSample(obuf, obuf, n);
	BGBMID_StoreWAV("tts_test0.wav", (byte *)obuf, 1, 11025, 16,  n);
#endif

#if 0
	if(!ifn)
		return(0);

	ibuf=BGBMID_LoadWAV_16Mono16(ifn, &ilen);
//	ibuf=BGBMID_LoadWAV_8Mono16(ifn, &ilen);
//	ibuf=BGBMID_LoadWAV_32Mono16(ifn, &ilen);
	obuf=malloc((ilen+32)*2*4*sizeof(s16));
	o2buf=malloc((ilen+32)*2*4*sizeof(s16));
	memset(obuf, 0, (ilen+32)*2*4*sizeof(s16));
	memset(o2buf, 0, (ilen+32)*2*4*sizeof(s16));
	
	err=0;
	
//	tbuf=malloc(ilen);

//	bands=malloc((ilen/BLKSIZE)*(4*NBANDS)*sizeof(s16));
//	tbuf=malloc((ilen/BLKSIZE)*(4*NBANDS)*sizeof(byte));
	tbuf=malloc(ilen);
//	tbct=bands;
	tbuct=tbuf;

	s0=0;
	for(i=0; i<ilen; i++)
	{
		s1=ibuf[i];
		s2=clamp16s(s1-s0);
//		s0=((s0*15)+s1)>>4;
		s3=(s0+s1)>>1;
		s3=(s0+s3)>>1;
		s3=(s0+s3)>>1;
		s3=(s0+s3)>>1;
		s0=s3;
		ibuf[i]=s2;
	}

#if 1
	n=ilen/BLKSIZE;
	for(i=0; i<n; i++)
	{
		printf("%d/%d\r", i, n);
		fflush(stdout);
		encswg_1x(ibuf+i*BLKSIZE, tmpbands+ 0, tmpbands+ 1, 0);
		encswg_1x(ibuf+i*BLKSIZE, tmpbands+ 2, tmpbands+ 3, 1);
		encswg_1x(ibuf+i*BLKSIZE, tmpbands+ 4, tmpbands+ 5, 2);
		encswg_1x(ibuf+i*BLKSIZE, tmpbands+ 6, tmpbands+ 7, 3);
		encswg_1x(ibuf+i*BLKSIZE, tmpbands+ 8, tmpbands+ 9, 4);
		encswg_1x(ibuf+i*BLKSIZE, tmpbands+10, tmpbands+11, 5);
		encswg_1x(ibuf+i*BLKSIZE, tmpbands+12, tmpbands+13, 6);
		encswg_1x(ibuf+i*BLKSIZE, tmpbands+14, tmpbands+15, 7);

		for(j=0; j<8; j++)
			for(k=j+1; k<8; k++)
		{
			s1=tmpbands[j*2+1];
			s2=tmpbands[k*2+1];
			if((s2^(s2>>31))>(s1^(s1>>31)))
			{
				i0=tmpbands[j*2+0]; i1=tmpbands[j*2+1];
				i2=tmpbands[k*2+0]; i3=tmpbands[k*2+1];
				tmpbands[j*2+0]=i2; tmpbands[j*2+1]=i3;
				tmpbands[k*2+0]=i0; tmpbands[k*2+1]=i1;
			}
		}
		
		for(j=0; j<4; j++)
		{
			k=FixToFp8(tmpbands[j*2+1]);
			tmpbands[j*2+1]=Fp8ToFix(k);
			
			tbuf[i*8+(j*2)+0]=tmpbands[j*2+0];
			tbuf[i*8+(j*2)+1]=k;
		}

		for(j=0; j<(2*BLKSIZE); j++)
			tmpdbuf[j]=0;
		decswg_1x(tmpdbuf, tmpbands+0, tmpbands+1);
		decswg_1x(tmpdbuf, tmpbands+2, tmpbands+3);
		decswg_1x(tmpdbuf, tmpbands+4, tmpbands+5);
		decswg_1x(tmpdbuf, tmpbands+6, tmpbands+7);

		for(j=0; j<(BLKSIZE/2); j++)
		{
			k=(2*BLKSIZE)-(j+1);
			tmpdbuf[j]=(tmpdbuf[j]*j)/(BLKSIZE/2);
			tmpdbuf[k]=(tmpdbuf[k]*j)/(BLKSIZE/2);
		}

		for(j=0; j<2*BLKSIZE; j++)
		{
			obuf[i*BLKSIZE+j]=clamp16s(tmpdbuf[j]);
		}

#if 0
		if(i>0)
		{
			for(j=0; j<8; j++)
			{
				obuf[i*BLKSIZE+j]=
					((obuf[i*BLKSIZE+j]*j)+
					((2*obuf[i*BLKSIZE-1]-obuf[i*BLKSIZE-(j+1)])*(8-j)))/8;
			}
		}
#endif

//		j=ibuf[i];
	}
	printf("\n");
	BGBMID_StoreWAV("tst0_sgb0.wav", (byte *)obuf, 1, 16000, 16, ilen);
	
	printf("%f kbps\n", ((n*4*16)/(ilen/16000.0))/1000.0);
	
	GANN_StoreFile("tst0_sgb0.dat", tbuf, n*4*2);
#endif

#if 0
	err=0;
	for(i=0; i<ilen; i++)
	{
		k=o2buf[i]-ibuf[i];
		k=k^(k>>31);
		err+=k;
	}
	printf("err=%lld\n", err/ilen);
#endif

#endif

	return(0);
}
