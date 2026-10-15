static int bgbmid_ima_index_table[16] = {
  -1, -1, -1, -1, 2, 4, 6, 8,
  -1, -1, -1, -1, 2, 4, 6, 8
}; 

//static int bgbmid_ima_step_table[89] = { 
static int bgbmid_ima_step_table[128] = { 
     7,     8,     9,    10,    11,    12,    13,    14,    16,    17, 
    19,    21,    23,    25,    28,    31,    34,    37,    41,    45, 
    50,    55,    60,    66,    73,    80,    88,    97,   107,   118, 
   130,   143,   157,   173,   190,   209,   230,   253,   279,   307,
   337,   371,   408,   449,   494,   544,   598,   658,   724,   796,
   876,   963,  1060,  1166,  1282,  1411,  1552,  1707,  1878,  2066, 
  2272,  2499,  2749,  3024,  3327,  3660,  4026,  4428,  4871,  5358,
  5894,  6484,  7132,  7845,  8630,  9493, 10442, 11487, 12635, 13899, 
 15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767 
};

void BGBCC_MsImaAdpcm_DecodeBlockMono(
	byte *ibuf, s16 *obuf, int len)
{
	int pred, index, step, diff, uni, sni;
	int i, j;
	
	pred=(s16)(ibuf[0]+(ibuf[1]<<8));
	index=ibuf[2];
	
	step=bgbmid_ima_step_table[index&127];

	for(i=0; i<len; i++)
	{
		j=(ibuf[4+(i>>1)]>>((i&1)*4))&15;
		uni=j;
		sni=(j&8)?(-(j&7)):(j&7);
	
		index=index+bgbmid_ima_index_table[uni];
		index=(index<0)?0:((index>88)?88:index);
//		diff=(sni+0.5)*step/4;
		diff=((2*(uni&7)+1)*step)/8;
		if(uni&8)diff=-diff;
		pred=pred+diff;
		step=bgbmid_ima_step_table[index];

		pred=(pred<(-32768))?(-32768):((pred>32767)?32767:pred);
		obuf[i]=pred;
	}
}


void BGBCC_MsImaAdpcm_EncodeBlockMono(
	s16 *ibuf, byte *obuf, int len, int *ridx)
{
	int s0, s1, p0, p1, p2, d0, d1, d2, uni1, uni2;
	int uni0b, uni1b, uni2b, p0b, p1b, p2b, d0b, d1b, d2b;
	int stp0b, stp1b, stp2b, ix0b, ix1b, ix2b;
	int lsbt, lsbit;
	int pred, index, step, diff, uni, sni;
	int err;
	int i, j, k;
	
	pred=(ibuf[0]+ibuf[1])>>1;
	index=ridx?(*ridx):0;
	
	if(index)
	{
		pred=index>>16;
		index&=255;
	}
	
	obuf[0]=pred&255;
	obuf[1]=(pred>>8)&255;
	obuf[2]=index;
	obuf[3]=0;
	
	memset(obuf+4, 0, (len+1)/2);
	
	step=bgbmid_ima_step_table[index&127];
	lsbt=0;
	err=0;

	for(i=0; i<len; i++)
	{
		j=ibuf[i];
		k=j-pred;
		s0=j;
		s1=ibuf[i+1];

		if(k>=0)
		{
			sni=(k*4)/step;
			uni=(sni>7)?7:sni;
		}else
		{
			sni=(-k*4)/step;
			uni=(sni>7)?15:(8|sni);
		}

#if 1
		uni1=uni+1;
		uni2=uni-1;

		if((uni^uni1)&(~7))uni1=uni;
		if((uni^uni2)&(~7))uni2=uni;

		diff=((2*(uni&7)+1)*step)/8;
		if(uni&8)diff=-diff;
		p0=pred+diff; d0=j-p0;
		d0=(d0<<1)^(d0>>31);

		diff=((2*(uni1&7)+1)*step)/8;
		if(uni1&8)diff=-diff;
		p1=pred+diff; d1=j-p1;
		d1=(d1<<1)^(d1>>31);

		diff=((2*(uni2&7)+1)*step)/8;
		if(uni2&8)diff=-diff;
		p2=pred+diff; d2=j-p2;
		d2=(d2<<1)^(d2>>31);

#if 1
		ix0b=index+bgbmid_ima_index_table[uni];
		ix1b=index+bgbmid_ima_index_table[uni1];
		ix2b=index+bgbmid_ima_index_table[uni2];
		ix0b=(ix0b<0)?0:((ix0b>88)?88:ix0b);
		ix1b=(ix1b<0)?0:((ix1b>88)?88:ix1b);
		ix2b=(ix2b<0)?0:((ix2b>88)?88:ix2b);
		stp0b=bgbmid_ima_step_table[ix0b];
		stp1b=bgbmid_ima_step_table[ix1b];
		stp2b=bgbmid_ima_step_table[ix2b];

		k=s1-p0;
		if(k>=0)	{ sni=(k*4)/stp0b; uni0b=(sni>7)?7:sni; }
		else		{ sni=(-k*4)/stp0b; uni0b=(sni>7)?15:(8|sni); }
		k=s1-p1;
		if(k>=0)	{ sni=(k*4)/stp1b; uni1b=(sni>7)?7:sni; }
		else		{ sni=(-k*4)/stp1b; uni1b=(sni>7)?15:(8|sni); }
		k=s1-p2;
		if(k>=0)	{ sni=(k*4)/stp2b; uni2b=(sni>7)?7:sni; }
		else		{ sni=(-k*4)/stp2b; uni2b=(sni>7)?15:(8|sni); }

		diff=((2*(uni0b&7)+1)*stp0b)/8;
		if(uni0b&8)diff=-diff;
		p0b=p0+diff; d0b=s1-p0b;
		d0b=(d0b<<1)^(d0b>>31);

		diff=((2*(uni1b&7)+1)*stp1b)/8;
		if(uni1b&8)diff=-diff;
		p1b=p1+diff; d1b=s1-p1b;
		d1b=(d1b<<1)^(d1b>>31);

		diff=((2*(uni2b&7)+1)*stp2b)/8;
		if(uni2b&8)diff=-diff;
		p2b=p2+diff; d2b=s1-p2b;
		d2b=(d2b<<1)^(d2b>>31);
		
		d0+=d0b/2;
		d1+=d1b/2;
		d2+=d2b/2;
#endif
		
		if(d1<d0)uni=uni1;
		if(d2<d0)uni=uni2;
#endif
	
		index=index+bgbmid_ima_index_table[uni];
		index=(index<0)?0:((index>88)?88:index);
		diff=((2*(uni&7)+1)*step)/8;
		if(uni&8)diff=-diff;
		pred=pred+diff;
		step=bgbmid_ima_step_table[index];
		
		obuf[4+(i>>1)]|=uni<<((i&1)*4);
		
		d0=s0-pred;		d0^=d0>>31;
		err+=d0;
	}
	
	err/=len;
//	bgbcc_wave_blockerr+=err;

	if(ridx)
		{ *ridx=index|(pred<<16); }
}

void BGBCC_MsImaAdpcm_EncodeBlockChan2b(
	s16 *ibuf, byte *obuf, int chan, int len, int *ridx)
{
	int s0, s1, s2, p0, p1, p2, d0, d1, d2, uni1, uni2;
	int ix0, ix1, ix2, stp0, stp1, stp2;
	int lsbt, lsbit, err, bd, bk, d;
	int pred, index, step, diff, uni, sni;
	int excl, noovf, chanx4;
	int i, j, k;
	
	pred=(ibuf[0*chan]+ibuf[1*chan])>>1;
	index=ridx?(*ridx):0;
	
	if(index)
	{
		pred=index>>16;
		index&=255;
	}
	
	obuf[0]=pred&255;
	obuf[1]=(pred>>8)&255;
	obuf[2]=index;
	obuf[3]=0;

//	noovf=1;
	noovf=0;
	chanx4=chan*4;
	
//	memset(obuf+4, 0, (len+3)/4);
	
	step=bgbmid_ima_step_table[index&127];
	lsbt=0;
	err=0;

	for(i=0; i<len; i++)
	{
		s0=ibuf[(i+0)*chan];
		s1=ibuf[(i+1)*chan];
		s2=ibuf[(i+2)*chan];

		excl=0;
		bd=999999999; bk=-1;
//		for(k=0; k<16; k++)
		for(k=0; k<64; k++)
		{
			if((excl>>(k&15))&1)
				continue;
		
			uni=(k>>0)&3;
			ix0=index+((uni&1)*3-1);
			if(noovf && ((ix0<0) || (ix0>88)))
				continue;
			ix0=(ix0<0)?0:((ix0>88)?88:ix0);
			diff=(step>>1)+((uni&1)*step);
			if(uni&2)diff=-diff;
			p0=pred+diff;
			stp0=bgbmid_ima_step_table[ix0];
			if(noovf && ((p0<-32767) || (p0>32767)))
				continue;
			p0=(p0<(-32768))?(-32768):((p0>32767)?32767:p0);

			d0=p0-s0; d0^=d0>>31;
			if(d0>=bd)
			{
				excl|=0x1111<<(k&3);
				continue;
			}

			uni=(k>>2)&3;
			ix1=ix0+((uni&1)*3-1);
			if(noovf && ((ix1<0) || (ix1>88)))
				continue;
			ix1=(ix1<0)?0:((ix1>88)?88:ix1);
			diff=(stp0>>1)+((uni&1)*stp0);
			if(uni&2)diff=-diff;
			p1=p0+diff;
			stp1=bgbmid_ima_step_table[ix1];
			if(noovf && ((p1<-32767) || (p1>32767)))
				continue;
			p1=(p1<(-32768))?(-32768):((p1>32767)?32767:p1);

			d1=p1-s1; d1^=d1>>31;
			if((d0+d1)>=bd)
			{
				excl|=0x0001<<(k&15);
				continue;
			}

#if 1			
			uni=(k>>4)&3;
			ix2=ix1+((uni&1)*3-1);
			if(noovf && ((ix2<0) || (ix2>88)))
				continue;
			ix2=(ix2<0)?0:((ix2>88)?88:ix2);
			diff=(stp1>>1)+((uni&1)*stp1);
			if(uni&2)diff=-diff;
			p2=p1+diff;
			stp2=bgbmid_ima_step_table[ix2];
			if(noovf && ((p2<-32767) || (p2>32767)))
				continue;
			p2=(p2<(-32768))?(-32768):((p2>32767)?32767:p2);
#endif
			
			d2=p2-s2; d2^=d2>>31;

			d=d0+d1+d2;
			if(d<bd)
				{ bd=d; bk=k; }
		}

		uni=bk&3;

#if 0
		j=ibuf[i];
		k=j-pred;
		if(k>=0)
		{
			sni=k/step;
			uni=(sni>1)?1:0;
		}else
		{
			sni=-k/step;
			uni=(sni>1)?3:2;
		}
#endif
	
		index=index+((uni&1)*3-1);
		index=(index<0)?0:((index>88)?88:index);
		diff=(step>>1)+((uni&1)*step);
		if(uni&2)diff=-diff;
		pred=pred+diff;
		step=bgbmid_ima_step_table[index];

		pred=(pred<(-32768))?(-32768):((pred>32767)?32767:pred);
		
		k=((i>>4)*chanx4)+((i&15)>>2);
		obuf[chanx4+k]|=uni<<((i&3)*2);

//		obuf[4+(i>>2)]|=uni<<((i&3)*2);
		
		d0=s0-pred;		d0^=d0>>31;
		err+=d0;
	}

	err/=len;
//	bgbcc_wave_blockerr+=err;

	if(ridx)
//		{ *ridx=index; }
		{ *ridx=index|(pred<<16); }
}

void BGBCC_MsImaAdpcm_EncodeBlockMono2b(
	s16 *ibuf, byte *obuf, int len, int *ridx)
{
	memset(obuf, 0, 4+((len+3)/4));
	BGBCC_MsImaAdpcm_EncodeBlockChan2b(ibuf, obuf, 1, len, ridx);
}

void BGBCC_MsImaAdpcm_EncodeBlock8Mono2b(
	byte *ibuf, byte *obuf, int len, int *ridx)
{
	s16 tbuf[1024];
	int i;
	
	for(i=0; i<len; i++)
	{
//		tbuf[i]=(ibuf[i]-128)<<8;
		tbuf[i]=ibuf[i]<<8;
	}
	
	memset(obuf, 0, 4+((len+3)/4));
	BGBCC_MsImaAdpcm_EncodeBlockChan2b(tbuf, obuf, 1, len, ridx);
}

void BGBCC_MsImaAdpcm_EncodeBlock8Mono(
	byte *ibuf, byte *obuf, int len, int *ridx)
{
	s16 tbuf[1024];
	int i;
	
	for(i=0; i<len; i++)
	{
//		tbuf[i]=(ibuf[i]-128)<<8;
		tbuf[i]=ibuf[i]<<8;
	}
	
	memset(obuf, 0, 4+((len+3)/4));
	BGBCC_MsImaAdpcm_EncodeBlockMono(tbuf, obuf, len, ridx);
}

void BGBCC_MsImaAdpcm_DecodeBlockMono2b(
	byte *ibuf, s16 *obuf, int len)
{
	int pred, index, step, diff, uni, sni;
	int i, j;
	
	pred=(s16)(ibuf[0]+(ibuf[1]<<8));
	index=ibuf[2];
	
	step=bgbmid_ima_step_table[index&127];

	for(i=0; i<len; i++)
	{
		j=(ibuf[4+(i>>2)]>>((i&3)*2))&3;
		uni=j;
	
		index=index+((uni&1)*3-1);
		index=(index<0)?0:((index>88)?88:index);
//		diff=((2*(uni&7)+1)*step)/8;
		diff=(step>>1)+((uni&1)*step);
		if(uni&2)diff=-diff;
		pred=pred+diff;
		step=bgbmid_ima_step_table[index];

		pred=(pred<(-32768))?(-32768):((pred>32767)?32767:pred);
		obuf[i]=pred;
	}
}

void BGBCC_MsImaAdpcm_DecodeBlock8Mono2b(
	byte *ibuf, byte *obuf, int len)
{
	s16 tbuf[1024];
	int i, j, k;
	BGBCC_MsImaAdpcm_DecodeBlockMono2b(ibuf, tbuf, len);
	for(i=0; i<len; i++)
	{
//		obuf[i]=(tbuf[i]>>8)+128;
		obuf[i]=tbuf[i]>>8;
	}
}

void BGBCC_MsImaAdpcm_DecodeBlock8Mono(
	byte *ibuf, byte *obuf, int len)
{
	s16 tbuf[1024];
	int i, j, k;
	BGBCC_MsImaAdpcm_DecodeBlockMono(ibuf, tbuf, len);
	for(i=0; i<len; i++)
	{
//		obuf[i]=(tbuf[i]>>8)+128;
		obuf[i]=tbuf[i]>>8;
	}
}
