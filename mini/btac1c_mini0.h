/*
Copyright (C) 2016 by Brendan G Bohannon

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

/*
 * BTAC1C Mini 0
 *
 * BTAC1C is an extended variant of MS-IMA-ADPCM.
 * It is partially backwards compatible with normal ADPCM.
 *   Mono is backwards compatible for decode.
 *   Mono encoding can be forced to produce MS-IMA-ADPCM 
 *     By setting ridx->bcfcn to 0x40.
 *   Stereo is incompatible mostly due to different bitrate/block-size.
 *
 * Total block size is 4 bits per logical-sample in both mono and stereo.
 *   To make this true, the lg2 encode/decode functions are used.
 *   These will insert/remove 8 (mono) or 16 (stereo) samples to get a 2^n size.
 *
 * So, for example, a block size of 512 bytes will encode 1024 logical samples.
 *
 * Nominally, this is stored in WAV files with a wFormatTag of 0x7B1C.
 * For the raw ADPCM case, the tag will be 0x0011.
 */

/* Header stuff */

typedef unsigned char btac1c_byte;
typedef unsigned short btac1c_u16;
typedef signed short btac1c_s16;
typedef unsigned int btac1c_u32;
typedef signed int btac1c_s32;
typedef unsigned long long btac1c_u64;
typedef signed long long btac1c_s64;

typedef struct btac1c_idxstate_s btac1c_idxstate;

struct btac1c_idxstate_s {
btac1c_u16 idx;					/* step index values */
btac1c_s16 lpred;					/* left/center predictor value */
btac1c_s16 rpred;					/* right/side predictor value */
btac1c_byte tag;					/* last (or forced) block type */
btac1c_byte bcfcn;					/* center(left/right) function */
btac1c_byte bsfcn;					/* side function */
btac1c_byte usefx;					/* number of FIR filters to use */
btac1c_s16 firfx[4][8];			/* FIR filter coeffs (8.8) */
};

/*
 * btac1c_initstate
 *   Initialize state to default values.
 *   Used for encode or decode prior to beginning.
 *
 * BTAC1C2_DecodeBlockMono(ibuf, obuf, len, ridx)
 * BTAC1C2_DecodeBlockStereo(ibuf, obuf, len, ridx)
 *   Decode a block (in terms of raw ADPCM samples).
 *   ibuf is the input block (bytes).
 *   obuf is the output samples.
 *   len is the length (in samples of a block).
 *   ridx is the index state.
 * 
 * BTAC1C2_DecodeBlockMonoLg2(ibuf, obuf, lg2, ridx)
 * BTAC1C2_DecodeBlockStereoLg2(ibuf, obuf, lg2, ridx)
 *   Decode a block a block padded to a power-of-2 size.
 *   This is used so that both the block size and samples are power-of-2.
 *   This will insert the missing samples to make up the difference.
 *
 * Note that decoding for blocks may occur in any order.
 * Likewise, any given block may be decoded any number of times.
 *
 *
 * BTAC1C2_EncodeBlockMono(ibuf, obuf, len, ridx)
 * BTAC1C2_EncodeBlockStereo(ibuf, obuf, len, ridx)
 *   Encode a block (in terms of raw ADPCM samples).
 *   ibuf is the input samples.
 *   obuf is the output block (bytes).
 *   len is the length (in samples of a block).
 *   ridx is the index state.
 * 
 * BTAC1C2_EncodeBlockMonoLg2(ibuf, obuf, lg2, ridx)
 * BTAC1C2_EncodeBlockStereoLg2(ibuf, obuf, lg2, ridx)
 *   Encode a block in terms of a block padded to a power-of-2 size.
 *
 * Note that unlike the decoder, sample blocks will need to be encoded
 * sequentially. This is because encoder state will carry over between
 * blocks (needed to minimize introduction of crackling and pops).
 */


/* Prototypes */

void btac1c_initstate(btac1c_idxstate *ridx);

int BTAC1C2_PredictSample(
	int *psamp, int idx, int pfcn, btac1c_idxstate *ridx);
void *BTAC1C2_GetPredictFunc(int pfcn);

void BTAC1C2_DecodeBlockMono(
	btac1c_byte *ibuf, btac1c_s16 *obuf, int len, btac1c_idxstate *ridx);
void BTAC1C2_DecodeMonoBlockStereo(
	btac1c_byte *ibuf, btac1c_s16 *obuf, int len, btac1c_idxstate *ridx);
void BTAC1C2_DecodeJointBlockStereo(
	btac1c_byte *ibuf, btac1c_s16 *obuf, int len, btac1c_idxstate *ridx);
void BTAC1C2_DecodeStereoBlockStereoI(
	btac1c_byte *ibuf, btac1c_s16 *obuf, int len, btac1c_idxstate *ridx);
void BTAC1C2_DecodeStereoBlockStereo(
	btac1c_byte *ibuf, btac1c_s16 *obuf, int len, btac1c_idxstate *ridx);
void BTAC1C2_DecodeBlockStereo(
	btac1c_byte *ibuf, btac1c_s16 *obuf, int len, btac1c_idxstate *ridx);
void BTAC1C2_DecodeBlockStereoLg2(
	btac1c_byte *ibuf, btac1c_s16 *obuf, int lg2, btac1c_idxstate *ridx);
void BTAC1C2_DecodeBlockMonoLg2(
	btac1c_byte *ibuf, btac1c_s16 *obuf, int lg2, btac1c_idxstate *ridx);

int BTAC1C2_ErrorBlockMonoSamples(
	btac1c_s16 *ibuf0, btac1c_s16 *ibuf1, int len);
int BTAC1C2_ErrorBlockStereoSamples(
	btac1c_s16 *ibuf0, btac1c_s16 *ibuf1, int len);
int BTAC1C2_EncodeQuantUni(
	int tgt, int pred, int step, int lsbit);
int BTAC1C2_EncodeQuantUni2(
	int tgt, int tgt2, int pred, int step, int lsbit);
int BTAC1C2_EncodeQuantUni3(
	int uni, int step, int pred, int tgt, int tgt2, int lsbit);
void BTAC1C2_PredictSampleMulti(
	int *psamp, int idx, btac1c_idxstate *ridx, int *prvec);
int BTAC1C2_SelectFilterBlockMono(
	btac1c_s16 *ibuf, int len, btac1c_idxstate *ridx);
int BTAC1C2_SelectFilterBlockStereo(
	btac1c_s16 *ibuf, int len, btac1c_idxstate *ridx,
	int *rpcfcn, int *rpsfcn);
void BTAC1C2_EncodeBlockMonoPfcn(
	btac1c_s16 *ibuf, btac1c_byte *obuf, int len,
	btac1c_idxstate *ridx, int pfcn);
void BTAC1C2_EncodeBlockMono(
	btac1c_s16 *ibuf, btac1c_byte *obuf, int len, btac1c_idxstate *ridx);
void BTAC1C2_EncodeBlockStereoJS_Pfcn(
	btac1c_s16 *ibuf, btac1c_byte *obuf, int len,
	btac1c_idxstate *ridx, int pcfcn, int psfcn);
int BTAC1C2_StereoBlockSizeFromSamples(int len);
void BTAC1C2_EncodeBlockStereoJS(
	btac1c_s16 *ibuf, btac1c_byte *obuf, int len, btac1c_idxstate *ridx);
void BTAC1C2_EncodeStereoBlockStereoI(
	btac1c_s16 *ibuf, btac1c_byte *obuf, int len,
	btac1c_idxstate *ridx, int pfcn);
void BTAC1C2_EncodeStereoBlockStereo2Pfcn(
	btac1c_s16 *ibuf, btac1c_byte *obuf, int len,
	btac1c_idxstate *ridx, int pfcn);
void BTAC1C2_EncodeMonoBlockStereoPfcn(
	btac1c_s16 *ibuf, btac1c_byte *obuf, int len,
	btac1c_idxstate *ridx, int pfcn);
void BTAC1C2_EncodeBlockStereo(
	btac1c_s16 *ibuf, btac1c_byte *obuf, int len,
	btac1c_idxstate *ridx);
void BTAC1C2_EncodeBlockStereoLg2(
	btac1c_s16 *ibuf, btac1c_byte *obuf, int lg2,
	btac1c_idxstate *ridx);
void BTAC1C2_EncodeBlockMonoLg2(
	btac1c_s16 *ibuf, btac1c_byte *obuf, int lg2,
	btac1c_idxstate *ridx);


/* Common */

static const int btac1c_index_table[16] = {
  -1, -1, -1, -1, 2, 4, 6, 8,
  -1, -1, -1, -1, 2, 4, 6, 8
}; 

static const int btac1c_step_table[128] = { 
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

int BTAC1C2_PredictSample(
	int *psamp, int idx, int pfcn,
	btac1c_idxstate *ridx)
{
	int pred, p0, p1;
	int i;
	
	i=idx;

	switch(pfcn)
	{
	case 0:
		pred=psamp[(i-1)&7];
		break;
	case 1:
		pred=2*psamp[(i-1)&7]-psamp[(i-2)&7];
		break;
	case 2:
		pred=(3*psamp[(i-1)&7]-psamp[(i-2)&7])>>1;
		break;
	case 3:
		pred=(5*psamp[(i-1)&7]-psamp[(i-2)&7])>>2;
		break;
	case 4:
		p0=(psamp[(i-1)&7]+psamp[(i-2)&7]);
		p1=(psamp[(i-2)&7]+psamp[(i-3)&7]);
		pred=p0-(p1>>1);
		break;
	case 5:
		p0=(psamp[(i-1)&7]+psamp[(i-2)&7]);
		p1=(psamp[(i-2)&7]+psamp[(i-3)&7]);
		pred=(3*p0-p1)>>2;
		break;
	case 6:
		p0=(psamp[(i-1)&7]+psamp[(i-2)&7]);
		p1=(psamp[(i-2)&7]+psamp[(i-3)&7]);
		pred=(5*p0-p1)>>3;
		break;
	case 7:
		pred=(	18*psamp[(i-1)&7]-4*psamp[(i-2)&7]+
				 3*psamp[(i-3)&7]-2*psamp[(i-4)&7]+
				 1*psamp[(i-5)&7])/16;
		break;
	case 8:
		pred=(	72*psamp[(i-1)&7]-16*psamp[(i-2)&7]+
				12*psamp[(i-3)&7]- 8*psamp[(i-4)&7]+
				 5*psamp[(i-5)&7]- 3*psamp[(i-6)&7]+
				 3*psamp[(i-7)&7]- 1*psamp[(i-8)&7])/64;
		break;
	case 9:
		pred=(	76*psamp[(i-1)&7]-17*psamp[(i-2)&7]+
				10*psamp[(i-3)&7]- 7*psamp[(i-4)&7]+
				 5*psamp[(i-5)&7]- 4*psamp[(i-6)&7]+
				 4*psamp[(i-7)&7]- 3*psamp[(i-8)&7])/64;
		break;
	case 10:
		p0=(psamp[(i-1)&7]+psamp[(i-2)&7]+psamp[(i-3)&7]+psamp[(i-4)&7]);
		p1=(psamp[(i-5)&7]+psamp[(i-6)&7]+psamp[(i-7)&7]+psamp[(i-8)&7]);
		pred=(5*p0-p1)>>4;
		break;
	case 11:
		p0=(psamp[(i-1)&7]+psamp[(i-2)&7]+psamp[(i-3)&7]+psamp[(i-4)&7]);
		p1=(psamp[(i-5)&7]+psamp[(i-6)&7]+psamp[(i-7)&7]+psamp[(i-8)&7]);
		pred=(p0+p1)>>3;
		break;

	case 12:	case 13:
	case 14:	case 15:
		pred=(
			ridx->firfx[pfcn-12][0]*psamp[(i-1)&7]+
			ridx->firfx[pfcn-12][1]*psamp[(i-2)&7]+
			ridx->firfx[pfcn-12][2]*psamp[(i-3)&7]+
			ridx->firfx[pfcn-12][3]*psamp[(i-4)&7]+
			ridx->firfx[pfcn-12][4]*psamp[(i-5)&7]+
			ridx->firfx[pfcn-12][5]*psamp[(i-6)&7]+
			ridx->firfx[pfcn-12][6]*psamp[(i-7)&7]+
			ridx->firfx[pfcn-12][7]*psamp[(i-8)&7])/256;
		break;

	default:
		pred=0;
		break;
	}
	return(pred);
}

int BTAC1C2_PredictSample_Pfn0(int *psamp, int idx,
	int pfcn, btac1c_idxstate *ridx)
	{ return(psamp[(idx-1)&7]); }
int BTAC1C2_PredictSample_Pfn1(int *psamp, int idx,
	int pfcn, btac1c_idxstate *ridx)
	{ return(2*psamp[(idx-1)&7]-psamp[(idx-2)&7]); }
int BTAC1C2_PredictSample_Pfn2(int *psamp, int idx,
	int pfcn, btac1c_idxstate *ridx)
	{ return((3*psamp[(idx-1)&7]-psamp[(idx-2)&7])>>1); }
int BTAC1C2_PredictSample_Pfn3(int *psamp, int idx,
	int pfcn, btac1c_idxstate *ridx)
	{ return((5*psamp[(idx-1)&7]-psamp[(idx-2)&7])>>2); }

int BTAC1C2_PredictSample_Pfn4(int *psamp, int idx,
	int pfcn, btac1c_idxstate *ridx)
{
	int p0, p1;
	p0=(psamp[(idx-1)&7]+psamp[(idx-2)&7]);
	p1=(psamp[(idx-2)&7]+psamp[(idx-3)&7]);
	return(p0-(p1>>1));
}

int BTAC1C2_PredictSample_Pfn5(int *psamp, int idx,
	int pfcn, btac1c_idxstate *ridx)
{
	int p0, p1;
	p0=(psamp[(idx-1)&7]+psamp[(idx-2)&7]);
	p1=(psamp[(idx-2)&7]+psamp[(idx-3)&7]);
	return((3*p0-p1)>>2);
}

int BTAC1C2_PredictSample_Pfn6(int *psamp, int idx,
	int pfcn, btac1c_idxstate *ridx)
{
	int p0, p1;
	p0=(psamp[(idx-1)&7]+psamp[(idx-2)&7]);
	p1=(psamp[(idx-2)&7]+psamp[(idx-3)&7]);
	return((5*p0-p1)>>3);
}

int BTAC1C2_PredictSample_Pfn7(int *psamp, int idx,
	int pfcn, btac1c_idxstate *ridx)
{
	return((18*psamp[(idx-1)&7]-4*psamp[(idx-2)&7]+
			 3*psamp[(idx-3)&7]-2*psamp[(idx-4)&7]+
			 1*psamp[(idx-5)&7])/16);
}


int BTAC1C2_PredictSample_Pfn8(int *psamp, int idx,
	int pfcn, btac1c_idxstate *ridx)
{
	return((	72*psamp[(idx-1)&7]-16*psamp[(idx-2)&7]+
				12*psamp[(idx-3)&7]- 8*psamp[(idx-4)&7]+
				 5*psamp[(idx-5)&7]- 3*psamp[(idx-6)&7]+
				 3*psamp[(idx-7)&7]- 1*psamp[(idx-8)&7])/64);
}

int BTAC1C2_PredictSample_Pfn9(int *psamp, int idx,
	int pfcn, btac1c_idxstate *ridx)
{
	return((	76*psamp[(idx-1)&7]-17*psamp[(idx-2)&7]+
				10*psamp[(idx-3)&7]- 7*psamp[(idx-4)&7]+
				 5*psamp[(idx-5)&7]- 4*psamp[(idx-6)&7]+
				 4*psamp[(idx-7)&7]- 3*psamp[(idx-8)&7])/64);
}

int BTAC1C2_PredictSample_Pfn10(int *psamp, int idx,
	int pfcn, btac1c_idxstate *ridx)
{
	int p0, p1;
	p0=(psamp[(idx-1)&7]+psamp[(idx-2)&7]+psamp[(idx-3)&7]+psamp[(idx-4)&7]);
	p1=(psamp[(idx-5)&7]+psamp[(idx-6)&7]+psamp[(idx-7)&7]+psamp[(idx-8)&7]);
	return((5*p0-p1)>>3);
}

int BTAC1C2_PredictSample_Pfn11(int *psamp, int idx,
	int pfcn, btac1c_idxstate *ridx)
{
	int p0, p1;
	p0=(psamp[(idx-1)&7]+psamp[(idx-2)&7]+psamp[(idx-3)&7]+psamp[(idx-4)&7]);
	p1=(psamp[(idx-5)&7]+psamp[(idx-6)&7]+psamp[(idx-7)&7]+psamp[(idx-8)&7]);
	return((p0+p1)>>1);
}

void *BTAC1C2_GetPredictFunc(int pfcn)
{
	void *fcn;
	switch(pfcn)
	{
	case 0: fcn=(void *)BTAC1C2_PredictSample_Pfn0; break;
	case 1: fcn=(void *)BTAC1C2_PredictSample_Pfn1; break;
	case 2: fcn=(void *)BTAC1C2_PredictSample_Pfn2; break;
	case 3: fcn=(void *)BTAC1C2_PredictSample_Pfn3; break;
	case 4: fcn=(void *)BTAC1C2_PredictSample_Pfn4; break;
	case 5: fcn=(void *)BTAC1C2_PredictSample_Pfn5; break;
	case 6: fcn=(void *)BTAC1C2_PredictSample_Pfn6; break;
	case 7: fcn=(void *)BTAC1C2_PredictSample_Pfn7; break;
	case 8: fcn=(void *)BTAC1C2_PredictSample_Pfn8; break;
	case 9: fcn=(void *)BTAC1C2_PredictSample_Pfn9; break;
	case 10: fcn=(void *)BTAC1C2_PredictSample_Pfn10; break;
	case 11: fcn=(void *)BTAC1C2_PredictSample_Pfn11; break;
	default: fcn=(void *)BTAC1C2_PredictSample; break;
	}
	return(fcn);
}

void btac1c_memset(void *dst, int val, int sz)
{
	int i;
	for(i=0; i<sz; i++)
		((btac1c_byte *)dst)[i]=0x00;
}

void btac1c_memcpy(void *dst, void *src, int sz)
{
	int i;
	for(i=0; i<sz; i++)
		((btac1c_byte *)dst)[i]=((btac1c_byte *)src)[i];
}

void btac1c_initstate(btac1c_idxstate *ridx)
{
	btac1c_memset(ridx, 0, sizeof(btac1c_idxstate));
}


/* Decoder */

void BTAC1C2_DecodeBlockMono(
	btac1c_byte *ibuf, btac1c_s16 *obuf, int len, btac1c_idxstate *ridx)
{
	int psamp[8];
	int (*pfptr)(int *psamp, int idx,
		int pfcn, btac1c_idxstate *ridx);
	int pred, pfcn, index, step, diff, uni, sni;
	int i, j;
	
	pred=(btac1c_s16)(ibuf[0]+(ibuf[1]<<8));
	index=ibuf[2]&127;
	pfcn=ibuf[3]&15;
	
	psamp[0]=pred;	psamp[1]=pred;	psamp[2]=pred;	psamp[3]=pred;
	psamp[4]=pred;	psamp[5]=pred;	psamp[6]=pred;	psamp[7]=pred;
	
	step=btac1c_step_table[index&127];

	pfptr=(int (*)(int *psamp, int idx,
		int pfcn, btac1c_idxstate *ridx))
			BTAC1C2_GetPredictFunc(pfcn);

	for(i=0; i<len; i++)
	{
		j=(ibuf[4+(i>>1)]>>((i&1)*4))&15;
		uni=j;
		sni=(j&8)?(-(j&7)):(j&7);

		pred=pfptr(psamp, i, pfcn, ridx);
		index=index+btac1c_index_table[uni];
		index=(index<0)?0:((index>88)?88:index);
		diff=((2*(uni&7)+1)*step)/8;
		if(uni&8)diff=-diff;
		pred=pred+diff;
		step=btac1c_step_table[index];

		pred=(pred<(-32768))?(-32768):((pred>32767)?32767:pred);
		obuf[i]=pred;
		psamp[i&7]=pred;
	}
}

void BTAC1C2_DecodeMonoBlockStereo(
	btac1c_byte *ibuf, btac1c_s16 *obuf, int len,
	btac1c_idxstate *ridx)
{
	int psamp[8];
	int (*pfptr)(int *psamp, int idx,
		int pfcn, btac1c_idxstate *ridx);
	int pred, index, step, diff, uni, sni, ofs;
	int lpred, rpred;
	int pfcn;
	int lp, rp, itr;
	int i, j, k, l;
	
	pred=(btac1c_s16)(ibuf[0]+(ibuf[1]<<8));
	index=ibuf[2]&127;
	ofs=(btac1c_s16)(ibuf[4]+(ibuf[5]<<8));
	pfcn=ibuf[3]&15;
	itr=ibuf[7]&3;
	
	psamp[0]=pred;	psamp[1]=pred;
	psamp[2]=pred;	psamp[3]=pred;
	psamp[4]=pred;	psamp[5]=pred;
	psamp[6]=pred;	psamp[7]=pred;
	
	step=btac1c_step_table[index&127];

	pfptr=(int (*)(int *psamp, int idx,
		int pfcn, btac1c_idxstate *ridx))
			BTAC1C2_GetPredictFunc(pfcn);

	for(i=0; i<len; i++)
	{
		uni=(ibuf[8+(i>>1)]>>((i&1)*4))&15;

		pred=pfptr(psamp, i, pfcn, ridx);

		index=index+btac1c_index_table[uni];
		index=(index<0)?0:((index>88)?88:index);
		diff=((2*(uni&7)+1)*step)/8;
		if(uni&8)diff=-diff;
		pred=pred+diff;
		step=btac1c_step_table[index];

		pred=(pred<(-32768))?(-32768):((pred>32767)?32767:pred);
		psamp[i&7]=pred;

		lp=pred+ofs; rp=lp-(ofs<<1);

		lp=(lp<(-32768))?(-32768):((lp>32767)?32767:lp);
		rp=(rp<(-32768))?(-32768):((rp>32767)?32767:rp);
		obuf[i*2+0]=lp;
		obuf[i*2+1]=rp;
	}

	if(itr)
	{
		l=len-2;
		for(i=0; i<l; i+=2)
		{
			lp=(obuf[(i+0)*2+0]+obuf[(i+2)*2+0])>>1;
			obuf[(i+1)*2+0]=lp;
		}
		for(i=1; i<l; i+=2)
		{
			rp=(obuf[(i+0)*2+1]+obuf[(i+2)*2+1])>>1;
			obuf[(i+1)*2+1]=rp;
		}
	}
}

void BTAC1C2_DecodeJointBlockStereo(
	btac1c_byte *ibuf, btac1c_s16 *obuf, int len,
	btac1c_idxstate *ridx)
{
	int psamp[8];
	int (*pfptr)(int *psamp, int idx,
		int pfcn, btac1c_idxstate *ridx);
	int pred, index, step, diff, uni, sni, ofs;
	int lp, rp, pfcn, ispf;
	int p0, p1, p2, p3;
	int i, j, k, l;
	
	/* decode center channel */
	pred=(btac1c_s16)(ibuf[0]+(ibuf[1]<<8));
	index=ibuf[2]&127;
	pfcn=ibuf[3]&15;
	step=btac1c_step_table[index&127];	
	psamp[0]=pred;	psamp[1]=pred;
	psamp[2]=pred;	psamp[3]=pred;
	psamp[4]=pred;	psamp[5]=pred;
	psamp[6]=pred;	psamp[7]=pred;
	
	ispf=(ibuf[3]!=0)|(ibuf[7]!=0);

	pfptr=(int (*)(int *psamp, int idx,
		int pfcn, btac1c_idxstate *ridx))
			BTAC1C2_GetPredictFunc(pfcn);
	
	for(i=0; i<len; i++)
	{
		k=ibuf[8+(i>>2)*2+0]|((ibuf[8+(i>>2)*2+1])<<8);
		j=(k>>((i&3)*3))&7;
		uni=j<<1;
		uni|=(uni>>1)&(uni>>2)&ispf;

		pred=pfptr(psamp, i, pfcn, ridx);

		index=index+btac1c_index_table[uni];
		index=(index<0)?0:((index>88)?88:index);
		diff=((2*(uni&7)+1)*step)/8;
		if(uni&8)diff=-diff;
		pred=pred+diff;
		step=btac1c_step_table[index];

		if(pred!=((btac1c_s16)pred))
			pred=(pred<(-32768))?(-32768):((pred>32767)?32767:pred);
		obuf[i*2+0]=pred;
		psamp[i&7]=pred;
	}
	
	l=len>>2;

	/* decode side channel */
	pred=(btac1c_s16)(ibuf[4]+(ibuf[5]<<8));
	index=ibuf[6]&127;
	pfcn=ibuf[7]&15;
	step=btac1c_step_table[index&127];
	psamp[0]=pred;	psamp[1]=pred;
	psamp[2]=pred;	psamp[3]=pred;
	psamp[4]=pred;	psamp[5]=pred;
	psamp[6]=pred;	psamp[7]=pred;

	pfptr=(int (*)(int *psamp, int idx,
		int pfcn, btac1c_idxstate *ridx))
			BTAC1C2_GetPredictFunc(pfcn);
	
	for(i=0; i<l; i++)
	{
		k=ibuf[8+i*2+0]|((ibuf[8+i*2+1])<<8);
		uni=(k>>12)&15;

		pred=pfptr(psamp, i, pfcn, ridx);

		index=index+btac1c_index_table[uni];
		index=(index<0)?0:((index>88)?88:index);
		diff=((2*(uni&7)+1)*step)/8;
		if(uni&8)diff=-diff;
		pred=pred+diff;
		step=btac1c_step_table[index];

		if(pred!=((btac1c_s16)pred))
			pred=(pred<(-32768))?(-32768):((pred>32767)?32767:pred);
		obuf[i*8+1]=pred;
		psamp[i&7]=pred;
	}

	/* center/side -> stereo */
	for(i=0; i<l; i++)
	{
		ofs=obuf[i*8+1];
		for(j=0; j<4; j++)
		{
			k=(i*4+j)*2;
			pred=obuf[k+0];
			lp=pred+ofs; rp=lp-(ofs<<1);
			if(((lp+32768)|(rp+32768))>>16)
			{
				lp=(lp<(-32768))?(-32768):((lp>32767)?32767:lp);
				rp=(rp<(-32768))?(-32768):((rp>32767)?32767:rp);
			}
			obuf[k+0]=lp; obuf[k+1]=rp;
		}
	}
}

void BTAC1C2_DecodeStereoBlockStereoI(
	btac1c_byte *ibuf, btac1c_s16 *obuf, int len, btac1c_idxstate *ridx)
{
	int plsamp[8], prsamp[8];
	int (*pfptr)(int *psamp, int idx,
		int pfcn, btac1c_idxstate *ridx);
	int lpred, lindex, lstep, ldiff, luni, lsni;
	int rpred, rindex, rstep, rdiff, runi, rsni;
	int pfcn;
	int i, j, k;
	
	lpred=(btac1c_s16)(ibuf[0]+(ibuf[1]<<8));
	rpred=(btac1c_s16)(ibuf[4]+(ibuf[5]<<8));
	lindex=ibuf[2];
	rindex=ibuf[6];
	pfcn=ibuf[3]&15;

	lstep=btac1c_step_table[lindex&127];
	rstep=btac1c_step_table[rindex&127];

	plsamp[0]=lpred;	plsamp[1]=lpred;
	plsamp[2]=lpred;	plsamp[3]=lpred;
	plsamp[4]=lpred;	plsamp[5]=lpred;
	plsamp[6]=lpred;	plsamp[7]=lpred;
	prsamp[0]=rpred;	prsamp[1]=rpred;
	prsamp[2]=rpred;	prsamp[3]=rpred;
	prsamp[4]=rpred;	prsamp[5]=rpred;
	prsamp[6]=rpred;	prsamp[7]=rpred;

	pfptr=(int (*)(int *psamp, int idx,
		int pfcn, btac1c_idxstate *ridx))
			BTAC1C2_GetPredictFunc(pfcn);

	for(i=0; i<len; i++)
	{
		k=((i>>3)*8)+((i&7)>>1);
		luni=(ibuf[ 8+k]>>((i&1)*4))&15;
		runi=(ibuf[12+k]>>((i&1)*4))&15;

		lpred=pfptr(plsamp, i, pfcn, ridx);
		rpred=pfptr(prsamp, i, pfcn, ridx);

		lindex=lindex+btac1c_index_table[luni];
		lindex=(lindex<0)?0:((lindex>88)?88:lindex);
		ldiff=((2*(luni&7)+1)*lstep)/8;
		if(luni&8)ldiff=-ldiff;
		lpred=lpred+ldiff;
		lstep=btac1c_step_table[lindex];

		rindex=rindex+btac1c_index_table[runi];
		rindex=(rindex<0)?0:((rindex>88)?88:rindex);
		rdiff=((2*(runi&7)+1)*rstep)/8;
		if(runi&8)rdiff=-rdiff;
		rpred=rpred+rdiff;
		rstep=btac1c_step_table[rindex];

		lpred=(lpred<(-32768))?(-32768):((lpred>32767)?32767:lpred);
		rpred=(rpred<(-32768))?(-32768):((rpred>32767)?32767:rpred);
		
		obuf[i*2+0]=lpred;
		obuf[i*2+1]=rpred;
		plsamp[i&7]=lpred;
		prsamp[i&7]=rpred;
	}
	obuf[i*2+0]=lpred;
	obuf[i*2+1]=rpred;
}

void BTAC1C2_DecodeStereoBlockStereo(
	btac1c_byte *ibuf, btac1c_s16 *obuf, int len, btac1c_idxstate *ridx)
{
	int p0, p1, p2, p3;
	int p4, p5, p6, p7;
	int i0, i1, i2, i3;
	int m0, m1;
	int itr;
	int i, j, k, l;

	BTAC1C2_DecodeStereoBlockStereoI(ibuf, obuf, len>>1, ridx);

	itr=ibuf[7]&3;

#if 1
	if(itr==3)
	{
		l=len>>1;
		for(i=len-1; i>0; i--)
		{
			i0=(i+0)>>1;	i1=(i+1)>>1;
			i2=(i+0)>>1;	i3=(i-1)>>1;
			obuf[i*2+0]=(obuf[i0*2+0]+obuf[i1*2+0])>>1;
			obuf[i*2+1]=(obuf[i2*2+1]+obuf[i3*2+1])>>1;
		}
	}else if((itr==1) || (itr==2))
	{
		l=len>>1;
		for(i=len-1; i>0; i--)
		{
			i0=(i+0)>>1; i1=(i+((itr==1)*2-1))>>1;
			obuf[i*2+0]=(obuf[i0*2+0]+obuf[i1*2+0])>>1;
			obuf[i*2+1]=(obuf[i0*2+1]+obuf[i1*2+1])>>1;
		}
	}else
	{
		l=len>>1;
		for(i=len-1; i>0; i--)
		{
			j=i>>1;
			obuf[i*2+0]=obuf[j*2+0];
			obuf[i*2+1]=obuf[j*2+1];
		}
	}
#endif
}

void BTAC1C2_DecodeBlockStereo(
	btac1c_byte *ibuf, btac1c_s16 *obuf, int len, btac1c_idxstate *ridx)
{
	int i;
	
	i=ibuf[6];

	if(i<89)
	{
		BTAC1C2_DecodeStereoBlockStereo(ibuf, obuf, len, ridx);
		return;
	}

	if(i==89)
	{
		BTAC1C2_DecodeMonoBlockStereo(ibuf, obuf, len, ridx);
		return;
	}

	if((i>=128) && (i<217))
	{
		BTAC1C2_DecodeJointBlockStereo(ibuf, obuf, len, ridx);
		return;
	}

	for(i=0; i<len; i++)
	{
		obuf[i*2+0]=0;
		obuf[i*2+1]=0;
	}
}

void BTAC1C2_DecodeBlockStereoLg2(
	btac1c_byte *ibuf, btac1c_s16 *obuf, int lg2, btac1c_idxstate *ridx)
{
	int i, j, k, l;

	BTAC1C2_DecodeBlockStereo(ibuf, obuf, (1<<lg2)-16, ridx);
	
	l=1<<lg2;

	i=l-1;
	j=(i+1)>>(lg2-4);
	obuf[i*2+0]=obuf[(i-j)*2+0];
	obuf[i*2+1]=obuf[(i-j)*2+1];

	for(i=l-2; i>0; i--)
	{
		j=(i+1)>>(lg2-4);
		k=(i+0)>>(lg2-4);
		obuf[i*2+0]=(obuf[(i-j)*2+0]+obuf[(i-k)*2+0])>>1;
		obuf[i*2+1]=(obuf[(i-j)*2+1]+obuf[(i-k)*2+1])>>1;
	}
}

void BTAC1C2_DecodeBlockMonoLg2(
	btac1c_byte *ibuf, btac1c_s16 *obuf, int lg2, btac1c_idxstate *ridx)
{
	int i, j, k, l;

	BTAC1C2_DecodeBlockMono(ibuf, obuf, (1<<lg2)-8, ridx);

	l=1<<lg2;

	i=l-1;
	j=(i+1)>>(lg2-3);
	obuf[i]=obuf[i-j];

	for(i=l-2; i>0; i--)
	{
		j=(i+1)>>(lg2-3);	k=(i+0)>>(lg2-3);
		obuf[i]=(obuf[i-j]+obuf[i-k])>>1;
	}
}

/* Encoder */

btac1c_s64 btac1c2_fakesqrt(btac1c_s64 val)
{
	btac1c_s64 v, v1;
	int i, j;
	if(val<0)
		return(-btac1c2_fakesqrt(-val));
	
	v=val;
	while(v>(1LL<<30))
		{ v=v>>1; }
	while((v*v)>val)
		{ v=v>>1; }

	for(i=1; i<17; i++)
	{
		v1=v+(v>>i); j=16;
		if(v1==v)break;
		while(((v1*v1)<=val) && (j--))
			{ v=v1; v1=v+(v>>i); }
	}
	return(v);
}

int BTAC1C2_ErrorBlockMonoSamples(
	btac1c_s16 *ibuf0, btac1c_s16 *ibuf1, int len)
{
	int p0, p1;
	btac1c_s64 e, d;
	int i, j, k;
	
	e=0; p0=0; p1=0;
	for(i=0; i<len; i++)
	{
		d=ibuf0[i]-ibuf1[i];
		e+=d*d;
	}
	e=btac1c2_fakesqrt(e);
	if((e<0) || (e>(1<<30)))
		e=(1<<30);
	return(e);
}

int BTAC1C2_ErrorBlockStereoSamples(
	btac1c_s16 *ibuf0, btac1c_s16 *ibuf1, int len)
{
	int p0, p1, p2, p3, pc0, ps0, pc1, ps1;
	btac1c_s64 e, d, dc, ds;
	int i, j, k;
	
	e=0; p0=0; p1=0;
	for(i=0; i<len; i++)
	{
		p0=ibuf0[i*2+0];
		p1=ibuf0[i*2+1];
		p2=ibuf1[i*2+0];
		p3=ibuf1[i*2+1];

		pc0=(p0+p1)>>1;
		ps0=p0-p1;

		pc1=(p2+p3)>>1;
		ps1=p2-p3;

		dc=pc0-pc1;
		ds=ps0-ps1;
		ds=ds>>2;
		d=dc*dc+ds*ds;
		e+=d;
	}
	e=btac1c2_fakesqrt(e);
	if((e<0) || (e>(1<<30)))
		e=(1<<30);
	return(e);
}


int BTAC1C2_EncodeQuantUni(
	int tgt, int pred, int step, int lsbit)
{
	return(BTAC1C2_EncodeQuantUni2(
		tgt, tgt, pred, step, lsbit));
}

int BTAC1C2_EncodeQuantUni2(
	int tgt, int tgt2, int pred, int step, int lsbit)
{
	int uni, sni, diff;

	diff=tgt-pred;

	if(diff>=0)
	{
		sni=(diff*8-1)/(step*2);
		uni=(sni>7)?7:sni;
	}else
	{
		sni=(-diff*8-1)/(step*2);
		uni=(sni>7)?15:(8|sni);
	}

	uni=BTAC1C2_EncodeQuantUni3(uni, step, pred, tgt, tgt2, lsbit);
	return(uni);
}

int BTAC1C2_EncodeQuantUni3(
	int uni, int step, int pred, int tgt, int tgt2, int lsbit)
{
	int uni1, uni2;
	int diff, p0, p1, p2, p3, d0, d1, d2, d3;

	uni1=uni+1;
	uni2=uni-1;

	if((uni^uni1)&(~7))uni1=uni;
	if((uni^uni2)&(~7))uni2=uni;

	if(lsbit)
	{
		if(lsbit==4)
		{
			uni&=~1;
			uni1&=~1;
			uni2&=~1;

			uni|=(uni>>1)&(uni>>2)&1;
			uni1|=(uni1>>1)&(uni1>>2)&1;
			uni2|=(uni2>>1)&(uni2>>2)&1;
		}else if(lsbit&1)
			{ uni|=1; uni1|=1; uni2|=1; }
		else
			{ uni&=~1; uni1&=~1; uni2&=~1; }
	}

	diff=((2*(uni&7)+1)*step)/8;
	if(uni&8)diff=-diff;
	p0=pred+diff; d0=tgt-p0;
	d0=d0^(d0>>31);

	diff=((2*(uni1&7)+1)*step)/8;
	if(uni1&8)diff=-diff;
	p1=pred+diff; d1=tgt-p1;
	d1=d1^(d1>>31);

	diff=((2*(uni2&7)+1)*step)/8;
	if(uni2&8)diff=-diff;
	p2=pred+diff; d2=tgt-p2;
	d2=d2^(d2>>31);

	d3=tgt2-p0;	d3=d3^(d3>>31); d0+=d3>>5;
	d3=tgt2-p1;	d3=d3^(d3>>31); d1+=d3>>5;
	d3=tgt2-p2;	d3=d3^(d3>>31); d2+=d3>>5;
	
	if(d1<d0)uni=uni1;
	if(d2<d0)uni=uni2;

	return(uni);
}

void BTAC1C2_PredictSampleMulti(
	int *psamp, int idx, btac1c_idxstate *ridx,
	int *prvec)
{
	int fcn;
	
	prvec[0]=BTAC1C2_PredictSample_Pfn0(psamp, idx, 0, ridx);
	prvec[1]=BTAC1C2_PredictSample_Pfn1(psamp, idx, 1, ridx);
	prvec[2]=BTAC1C2_PredictSample_Pfn2(psamp, idx, 2, ridx);
	prvec[3]=BTAC1C2_PredictSample_Pfn3(psamp, idx, 3, ridx);
	prvec[4]=BTAC1C2_PredictSample_Pfn4(psamp, idx, 4, ridx);
	prvec[5]=BTAC1C2_PredictSample_Pfn5(psamp, idx, 5, ridx);
	prvec[6]=BTAC1C2_PredictSample_Pfn6(psamp, idx, 6, ridx);
	prvec[7]=BTAC1C2_PredictSample_Pfn7(psamp, idx, 7, ridx);
	prvec[8]=BTAC1C2_PredictSample_Pfn8(psamp, idx, 8, ridx);
	prvec[9]=BTAC1C2_PredictSample_Pfn9(psamp, idx, 9, ridx);
	prvec[10]=BTAC1C2_PredictSample_Pfn10(psamp, idx, 10, ridx);
	prvec[11]=BTAC1C2_PredictSample_Pfn11(psamp, idx, 11, ridx);

	for(fcn=0; fcn<ridx->usefx; fcn++)
	{
		prvec[fcn]=BTAC1C2_PredictSample(
			psamp, idx, 12+fcn, ridx);
	}
}

int BTAC1C2_SelectFilterBlockMono(
	btac1c_s16 *ibuf, int len, btac1c_idxstate *ridx)
{
	int psamp[8];
	int prvec[16];
	btac1c_s64 evec[16];
	int p0, p1, p2, d0, d1, d2, uni1, uni2;
	int lsbt, lsbit, npf, be;
	int pred, index, step, diff, uni, sni;
	int i, j, k;
	
	pred=(ibuf[0]+ibuf[1]+ibuf[2]+ibuf[3])>>2;
	
	psamp[0]=pred;	psamp[1]=pred;
	psamp[2]=pred;	psamp[3]=pred;
	psamp[4]=pred;	psamp[5]=pred;
	psamp[6]=pred;	psamp[7]=pred;

	npf=12+ridx->usefx;
	for(i=0; i<npf; i++)
		{ evec[i]=0; }

	for(i=0; i<len; i++)
	{
		BTAC1C2_PredictSampleMulti(psamp, i, ridx, prvec);

		p0=ibuf[i];

		for(j=0; j<npf; j++)
		{
			d0=p0-prvec[j];
			d1=d0^(d0>>31);
			d0=d0>>1; d2=(d0*d0+d1)>>1;
			evec[j]+=d2;
		}

		pred=p0;
		psamp[i&7]=pred;
	}
	
	uni=8;
	be=(evec[uni]*31)/32;
	for(i=0; i<npf; i++)
	{
		if(evec[i]<be)
			{ uni=i; be=evec[i]; }
	}
	return(uni);
}

int BTAC1C2_SelectFilterBlockStereo(
	btac1c_s16 *ibuf, int len, btac1c_idxstate *ridx,
	int *rpcfcn, int *rpsfcn)
{
	int pcsamp[8], pssamp[8];
	int prcvec[16];
	int prsvec[16];
	btac1c_s64 ecvec[16], esvec[16];
	int p0, p1, p2, p3, d0, d1, d2, uni1, uni2;
	int lsbt, lsbit, npf, bue, bse;
	int cpred, spred, index, step, diff, uni, sni;
	int i, j, k;
	
	cpred=(ibuf[0]+ibuf[1]+ibuf[2]+ibuf[3])>>2;
	spred=((ibuf[0]+ibuf[2])/2)-((ibuf[1]+ibuf[3])/2);
	
	pcsamp[0]=cpred; pcsamp[1]=cpred; pcsamp[2]=cpred; pcsamp[3]=cpred;
	pcsamp[4]=cpred; pcsamp[5]=cpred; pcsamp[6]=cpred; pcsamp[7]=cpred;
	pssamp[0]=spred; pssamp[1]=spred; pssamp[2]=spred; pssamp[3]=spred;
	pssamp[4]=spred; pssamp[5]=spred; pssamp[6]=spred; pssamp[7]=spred;

	npf=12+ridx->usefx;
	for(i=0; i<npf; i++)
		{ ecvec[i]=0; esvec[i]=0; }

	for(i=0; i<len; i++)
	{
		BTAC1C2_PredictSampleMulti(pcsamp, i, ridx, prcvec);
		BTAC1C2_PredictSampleMulti(pssamp, i, ridx, prsvec);

		p0=ibuf[i*2+0];
		p1=ibuf[i*2+1];

		p2=(p0+p1)/2;
		p3=p0-p2;

		for(j=0; j<npf; j++)
		{
			d0=p2-prcvec[j];
			d1=d0^(d0>>31);
			d0=d0>>1; d2=(d0*d0+d1)>>1;
			ecvec[j]+=d2;

			d0=p3-prsvec[j];
			d1=d0^(d0>>31);
			d0=d0>>1; d2=(d0*d0+d1)>>1;
			esvec[j]+=d2;
		}

		cpred=p2;
		spred=p3;

		pcsamp[i&7]=cpred;
		pssamp[i&7]=spred;
	}
	
	uni=8; sni=8;
	bue=ecvec[uni];
	bse=esvec[sni];
	bue=(bue*31)/32;
	bse=(bse*31)/32;
	for(i=0; i<npf; i++)
	{
		if(ecvec[i]<bue)
			{ uni=i; bue=ecvec[i]; }
		if(esvec[i]<bse)
			{ sni=i; bse=esvec[i]; }
	}
	
	*rpcfcn=uni;
	*rpsfcn=sni;
	return(uni);
}

void BTAC1C2_EncodeBlockMonoPfcn(
	btac1c_s16 *ibuf, btac1c_byte *obuf, int len,
	btac1c_idxstate *ridx, int pfcn)
{
	int psamp[8];
	int p0, p1, p2, d0, d1, d2, uni1, uni2;
	int lsbt, lsbit;
	int pred, index, step, diff, uni, sni;
	int i, j, k;
	
	if(ridx && ridx->tag)
	{
		pred=ridx->lpred;
		index=ridx->idx&127;
		if(index>88)
			index=0;
	}
	else
	{
		pred=(ibuf[0]+ibuf[1])>>1;
		index=0;
	}
	
	obuf[0]=pred&255;
	obuf[1]=(pred>>8)&255;
	obuf[2]=index;
	obuf[3]=pfcn;
	
	btac1c_memset(obuf+4, 0, (len+1)/2);
	
	step=btac1c_step_table[index&127];
	lsbt=0;

	psamp[0]=pred;	psamp[1]=pred;	psamp[2]=pred;	psamp[3]=pred;
	psamp[4]=pred;	psamp[5]=pred;	psamp[6]=pred;	psamp[7]=pred;

	for(i=0; i<len; i++)
	{
		if(pfcn)
			{ pred=BTAC1C2_PredictSample(psamp, i, pfcn, ridx); }

		p0=ibuf[i];
		p1=((i+1)<len)?ibuf[i+1]:p0;
		uni=BTAC1C2_EncodeQuantUni2(p0, p1, pred, step, 0);
	
		index=index+btac1c_index_table[uni];
		index=(index<0)?0:((index>88)?88:index);
		diff=((2*(uni&7)+1)*step)/8;
		if(uni&8)diff=-diff;
		pred=pred+diff;
		step=btac1c_step_table[index];

		pred=(pred<(-32768))?(-32768):((pred>32767)?32767:pred);
		
		obuf[4+(i>>1)]|=uni<<((i&1)*4);
		psamp[i&7]=pred;
	}

	if(ridx)
	{
		ridx->idx=index;
		ridx->lpred=pred;
		ridx->tag=3;
	}
}

void BTAC1C2_EncodeBlockMono(
	btac1c_s16 *ibuf, btac1c_byte *obuf, int len, btac1c_idxstate *ridx)
{
	int fcn;
	
	if(ridx->bcfcn&0x40)
	{
		BTAC1C2_EncodeBlockMonoPfcn(
			ibuf, obuf, len, ridx, (ridx->bcfcn)&15);
		return;
	}
	
	fcn=BTAC1C2_SelectFilterBlockMono(ibuf, len, ridx);
	BTAC1C2_EncodeBlockMonoPfcn(ibuf, obuf, len, ridx, fcn);
}

void BTAC1C2_EncodeBlockStereoJS_Pfcn(
	btac1c_s16 *ibuf, btac1c_byte *obuf, int len,
	btac1c_idxstate *ridx, int pcfcn, int psfcn)
{
	int pcsamp[8], pssamp[8];
	int p0, p1, p2, p3, d0, d1, d2, uni1, uni2;
	int lsbt, lsbit, ispf;
	int cv, sv, cv2, sv2, len2;
	int cpred, spred, cindex, sindex, cstep, sstep, usblk;
	int tpred, tindex, tstep;
	int diff, uni, sni;
	int i, j, k;

	if(ridx && (ridx->tag==1))
	{
		cpred=ridx->lpred;
		spred=ridx->rpred;

		cindex=(ridx->idx)&127;
		sindex=((ridx->idx)>>8)&127;
	}else if(ridx && (ridx->tag==2))
	{
		p0=ridx->lpred;
		p1=ridx->rpred;
		cpred=(p0+p1)>>1;
		spred=p0-cpred;

		p2=(ridx->idx)&127;
		p3=((ridx->idx)>>8)&127;
		cindex=(p2+p3)>>1;
		sindex=cindex;
	}else
	{
		p0=(ibuf[0*2+0]+ibuf[1*2+0])>>1;
		p1=(ibuf[0*2+1]+ibuf[1*2+1])>>1;

		p2=(ibuf[(0*4+0)*2+0]+ibuf[(0*4+1)*2+0]+
			ibuf[(0*4+2)*2+0]+ibuf[(0*4+3)*2+0]+
			ibuf[(0*4+4)*2+0]+ibuf[(0*4+5)*2+0]+
			ibuf[(0*4+6)*2+0]+ibuf[(0*4+7)*2+0])>>2;
		p3=(ibuf[(0*4+0)*2+1]+ibuf[(0*4+1)*2+1]+
			ibuf[(0*4+2)*2+1]+ibuf[(0*4+3)*2+1]+
			ibuf[(0*4+4)*2+1]+ibuf[(0*4+5)*2+1]+
			ibuf[(0*4+6)*2+1]+ibuf[(0*4+7)*2+1])>>2;

		cpred=(p0+p1)>>1;

		cv=(p2+p3)>>1;
		spred=p2-cv;
		
		cindex=0;
		sindex=0;
	}

	obuf[0]=cpred&255;
	obuf[1]=(cpred>>8)&255;
	obuf[2]=cindex;
	obuf[3]=pcfcn;
	obuf[4]=spred&255;
	obuf[5]=(spred>>8)&255;
	obuf[6]=0x80|sindex;
	obuf[7]=psfcn;
	
	btac1c_memset(obuf+8, 0, 2*((len+3)/4));
	
	pcsamp[0]=cpred;	pcsamp[1]=cpred;
	pcsamp[2]=cpred;	pcsamp[3]=cpred;
	pcsamp[4]=cpred;	pcsamp[5]=cpred;
	pcsamp[6]=cpred;	pcsamp[7]=cpred;

	pssamp[0]=spred;	pssamp[1]=spred;
	pssamp[2]=spred;	pssamp[3]=spred;
	pssamp[4]=spred;	pssamp[5]=spred;
	pssamp[6]=spred;	pssamp[7]=spred;

	ispf=(obuf[3]!=0)|(obuf[7]!=0);

	cstep=btac1c_step_table[cindex&127];
	sstep=btac1c_step_table[sindex&127];

	len2=len/4;
	for(i=0; i<len2; i++)
	{
		usblk=0;
		for(j=0; j<4; j++)
		{
			if(pcfcn)
			{
				cpred=BTAC1C2_PredictSample(
					pcsamp, i*4+j, pcfcn, ridx);
			}

			p0=ibuf[(i*4+j)*2+0];
			p1=ibuf[(i*4+j)*2+1];
			cv=(p0+p1)>>1;
			cv2=cv;

			if((i*4+j+1)<len)
			{
				p2=ibuf[(i*4+j+1)*2+0];
				p3=ibuf[(i*4+j+1)*2+1];
				cv2=(p2+p3)>>1;
			}

			uni=BTAC1C2_EncodeQuantUni2(
				cv, cv2, cpred, cstep, ispf?4:2);

			cindex=cindex+btac1c_index_table[uni];
			cindex=(cindex<0)?0:((cindex>88)?88:cindex);
			diff=((2*(uni&7)+1)*cstep)/8;
			if(uni&8)diff=-diff;
			cpred=cpred+diff;
			cstep=btac1c_step_table[cindex];
			
			usblk|=(uni>>1)<<(j*3);
			pcsamp[(i*4+j)&7]=cpred;
		}

		if(1)
		{
			if(psfcn)
			{
				spred=BTAC1C2_PredictSample(
					pssamp, i, psfcn, ridx);
			}

			p0=(ibuf[(i*4+0)*2+0]+ibuf[(i*4+1)*2+0]+
				ibuf[(i*4+2)*2+0]+ibuf[(i*4+3)*2+0])>>2;
			p1=(ibuf[(i*4+0)*2+1]+ibuf[(i*4+1)*2+1]+
				ibuf[(i*4+2)*2+1]+ibuf[(i*4+3)*2+1])>>2;

			cv=(p0+p1)>>1;
			sv=p0-cv;
			sv2=sv;

			if((i+1)<len2)
			{
				p2=(ibuf[(i*4+4)*2+0]+ibuf[(i*4+5)*2+0]+
					ibuf[(i*4+6)*2+0]+ibuf[(i*4+7)*2+0])>>2;
				p3=(ibuf[(i*4+4)*2+1]+ibuf[(i*4+5)*2+1]+
					ibuf[(i*4+6)*2+1]+ibuf[(i*4+7)*2+1])>>2;
				cv2=(p2+p3)>>1;
				sv2=p2-cv2;
			}

			uni=BTAC1C2_EncodeQuantUni2(sv, sv2, spred, sstep, 0);

			sindex=sindex+btac1c_index_table[uni];
			sindex=(sindex<0)?0:((sindex>88)?88:sindex);
			diff=((2*(uni&7)+1)*sstep)/8;
			if(uni&8)diff=-diff;
			spred=spred+diff;
			sstep=btac1c_step_table[sindex];
			
			usblk|=uni<<12;
			pssamp[i&7]=spred;
		}

		obuf[8+i*2+0]=usblk;
		obuf[8+i*2+1]=usblk>>8;
	}

	if(ridx)
	{
		ridx->idx=cindex|(sindex<<8);
		ridx->lpred=cpred;
		ridx->rpred=spred;
		ridx->tag=1;
	}
}

int BTAC1C2_StereoBlockSizeFromSamples(int len)
{
	return(8+((len+1)/2));
}

void BTAC1C2_EncodeBlockStereoJS(
	btac1c_s16 *ibuf, btac1c_byte *obuf, int len, btac1c_idxstate *ridx)
{
	btac1c_byte tbuf[1024];
	btac1c_s16 stbuf[4096];
	int e0, e1, be, bsz, fcn, utag, bcfcn, bsfcn;
	btac1c_idxstate idx, iidx;

	bsz=BTAC1C2_StereoBlockSizeFromSamples(len);
	iidx=*ridx;

	bcfcn=ridx->bcfcn&15;
	bsfcn=ridx->bsfcn&15;
	utag=ridx->tag;

	idx=iidx;
	if(ridx->bcfcn&0x40)
	{
		switch(utag)
		{
		case 1:
			BTAC1C2_EncodeBlockStereoJS_Pfcn(
				ibuf, tbuf, len, &idx, bcfcn, bsfcn);
			break;
		case 2:
			BTAC1C2_EncodeStereoBlockStereo2Pfcn(
				ibuf, tbuf, len, &idx, bcfcn);
			break;
		case 3:
			BTAC1C2_EncodeMonoBlockStereoPfcn(
				ibuf, tbuf, len, &idx, bcfcn);
			break;
		default:
			break;
		}
		btac1c_memcpy(obuf, tbuf, bsz);
		*ridx=idx;
		return;
	}else
	{
		BTAC1C2_SelectFilterBlockStereo(
			ibuf, len, &idx, &bcfcn, &bsfcn);
	}

	be=1999999999;

	idx=iidx;
	BTAC1C2_EncodeBlockStereoJS_Pfcn(
		ibuf, tbuf, len, &idx, bcfcn, bsfcn);
	BTAC1C2_DecodeJointBlockStereo(tbuf, stbuf, len, &idx);
	e0=BTAC1C2_ErrorBlockStereoSamples(ibuf, stbuf, len);
	if(e0<be)
	{
		btac1c_memcpy(obuf, tbuf, bsz);
		*ridx=idx;
		be=e0;
	}

	idx=iidx;
	BTAC1C2_EncodeStereoBlockStereo2Pfcn(
		ibuf, tbuf, len, &idx, bcfcn);
	BTAC1C2_DecodeBlockStereo(tbuf, stbuf, len, &idx);
	e0=BTAC1C2_ErrorBlockStereoSamples(ibuf, stbuf, len);
	if(e0<be)
	{
		btac1c_memcpy(obuf, tbuf, bsz);
		*ridx=idx;
		be=e0;
	}

	idx=iidx;
	BTAC1C2_EncodeMonoBlockStereoPfcn(
		ibuf, tbuf, len, &idx, bcfcn);
	BTAC1C2_DecodeBlockStereo(tbuf, stbuf, len, &idx);
	e0=BTAC1C2_ErrorBlockStereoSamples(ibuf, stbuf, len);
	if(e0<be)
	{
		btac1c_memcpy(obuf, tbuf, bsz);
		*ridx=idx;
		be=e0;
	}

	ridx->bcfcn=bcfcn;
	ridx->bsfcn=bsfcn;
}


void BTAC1C2_EncodeStereoBlockStereoI(
	btac1c_s16 *ibuf, btac1c_byte *obuf, int len,
	btac1c_idxstate *ridx, int pfcn)
{
	int plsamp[8], prsamp[8];
	int lpred, lindex, lstep, ldiff, luni, lsni;
	int rpred, rindex, rstep, rdiff, runi, rsni;
	int p0, p1, p2, p3;
	int i, j, k;
	
	if(ridx && (ridx->tag==2))
	{
		lpred=ridx->lpred;
		rpred=ridx->rpred;
		lindex=(ridx->idx   )&127;
		rindex=(ridx->idx>>8)&127;
	}else if(ridx && (ridx->tag==1))
	{
		i=ridx->lpred;
		j=ridx->rpred;
		lpred=i+j;
		rpred=i-j;
		lindex=(ridx->idx   )&127;
		rindex=lindex;
	}else
	{
		lpred=(ibuf[0]+ibuf[2])>>1;
		rpred=(ibuf[1]+ibuf[3])>>1;
		lindex=0;
		rindex=0;
	}

	btac1c_memset(obuf, 0, 8+((len+7)/8)*8);
	
	obuf[0]=lpred&255;
	obuf[1]=(lpred>>8)&255;
	obuf[2]=lindex;
	obuf[3]=pfcn;

	obuf[4]=rpred&255;
	obuf[5]=(rpred>>8)&255;
	obuf[6]=rindex;
	obuf[7]=pfcn>>8;
	
	pfcn=pfcn&15;
	
	lstep=btac1c_step_table[lindex&127];
	rstep=btac1c_step_table[rindex&127];
	plsamp[0]=lpred; plsamp[1]=lpred; plsamp[2]=lpred; plsamp[3]=lpred;
	plsamp[4]=lpred; plsamp[5]=lpred; plsamp[6]=lpred; plsamp[7]=lpred;
	prsamp[0]=rpred; prsamp[1]=rpred; prsamp[2]=rpred; prsamp[3]=rpred;
	prsamp[4]=rpred; prsamp[5]=rpred; prsamp[6]=rpred; prsamp[7]=rpred;

	for(i=0; i<len; i++)
	{
		if(pfcn)
		{
			lpred=BTAC1C2_PredictSample(plsamp, i, pfcn, ridx);
			rpred=BTAC1C2_PredictSample(prsamp, i, pfcn, ridx);
		}

		p0=ibuf[(i+0)*2+0]; p1=p0;
		p2=ibuf[(i+0)*2+1]; p3=p2;

		if((i+1)<len)
			{ p1=ibuf[(i+1)*2+0]; p3=ibuf[(i+1)*2+1]; }

		luni=BTAC1C2_EncodeQuantUni2(p0, p1, lpred, lstep, 0);
		runi=BTAC1C2_EncodeQuantUni2(p2, p3, rpred, rstep, 0);

		lindex=lindex+btac1c_index_table[luni];
		lindex=(lindex<0)?0:((lindex>88)?88:lindex);
		ldiff=((2*(luni&7)+1)*lstep)/8;
		if(luni&8)ldiff=-ldiff;
		lpred=lpred+ldiff;
		lstep=btac1c_step_table[lindex];

		rindex=rindex+btac1c_index_table[runi];
		rindex=(rindex<0)?0:((rindex>88)?88:rindex);
		rdiff=((2*(runi&7)+1)*rstep)/8;
		if(runi&8)rdiff=-rdiff;
		rpred=rpred+rdiff;
		rstep=btac1c_step_table[rindex];

		lpred=(lpred<(-32768))?(-32768):((lpred>32767)?32767:lpred);
		rpred=(rpred<(-32768))?(-32768):((rpred>32767)?32767:rpred);

		k=((i>>3)*8)+((i&7)>>1);
		obuf[ 8+k]|=luni<<((i&1)*4);
		obuf[12+k]|=runi<<((i&1)*4);
		plsamp[i&7]=lpred;
		prsamp[i&7]=rpred;
	}
	
	ridx->idx=lindex|(rindex<<8);
	ridx->lpred=lpred;
	ridx->rpred=rpred;
	ridx->tag=2;
}

void BTAC1C2_EncodeStereoBlockStereo2Pfcn(
	btac1c_s16 *ibuf, btac1c_byte *obuf, int len,
	btac1c_idxstate *ridx, int pfcn)
{
	btac1c_s16 stbuf[4096];
	int l, e, be, tag, iidx, idx, bsz;
	int i, j, k;

	l=len>>1;
	for(i=0; i<l; i++)
	{
		stbuf[i*2+0]=ibuf[i*4+0];
		stbuf[i*2+1]=ibuf[i*4+3];
	}
	
	pfcn|=0x0300;
	BTAC1C2_EncodeStereoBlockStereoI(stbuf, obuf, l, ridx, pfcn);
}

void BTAC1C2_EncodeMonoBlockStereoPfcn(
	btac1c_s16 *ibuf, btac1c_byte *obuf, int len,
	btac1c_idxstate *ridx, int pfcn)
{
	btac1c_s16 stbuf[4096];
	int l, e, be, tag, bsz;
	int cl, cr, tl, tr, ofs, intr;
	int i, j, k;

	if(1)
	{
		intr=0;
		ofs=0;
		for(i=0; i<len; i++)
		{
			cl=ibuf[i*2+0];		cr=ibuf[i*2+1];
			stbuf[i]=(cl+cr)>>1;
			ofs+=(cl-cr);
		}
		ofs=ofs/len;
		ofs=ofs/2;
	}
	
	BTAC1C2_EncodeBlockMonoPfcn(stbuf, obuf, len, ridx, pfcn);
	
	l=(len+7)/8;
	for(i=l-1; i>=0; i--)
		{ *(btac1c_u32 *)(obuf+8+i*4)=*(btac1c_u32 *)(obuf+4+i*4); }
	obuf[4]=ofs;
	obuf[5]=ofs>>8;
	obuf[6]=89;
	obuf[7]=intr;
}

void BTAC1C2_EncodeBlockStereo(
	btac1c_s16 *ibuf, btac1c_byte *obuf, int len,
	btac1c_idxstate *ridx)
{
	BTAC1C2_EncodeBlockStereoJS(ibuf, obuf, len, ridx);
}

void BTAC1C2_EncodeBlockStereoLg2(
	btac1c_s16 *ibuf, btac1c_byte *obuf, int lg2,
	btac1c_idxstate *ridx)
{
	btac1c_s16 stbuf[2048];
	int len, len2;
	int i, j, k;
	
	len2=(1<<lg2);
	len=len2-16;
	
	for(i=0; i<len2; i++)
	{
		j=i>>(lg2-4);
		stbuf[(i-j)*2+0]=ibuf[i*2+0];
		stbuf[(i-j)*2+1]=ibuf[i*2+1];
	}
	
	BTAC1C2_EncodeBlockStereo(stbuf, obuf, len, ridx);
}

void BTAC1C2_EncodeBlockMonoLg2(
	btac1c_s16 *ibuf, btac1c_byte *obuf, int lg2,
	btac1c_idxstate *ridx)
{
	btac1c_s16 stbuf[2048];
	int len, len2;
	int i, j, k;
	
	len2=(1<<lg2);
	len=len2-8;
	
	for(i=0; i<len2; i++)
	{
		j=(i+0)>>(lg2-3);
		k=(i+1)>>(lg2-3);
		stbuf[i-j]=(ibuf[i]+ibuf[i+(k-j)])/2;
	}
	
	BTAC1C2_EncodeBlockMono(stbuf, obuf, len, ridx);
}
