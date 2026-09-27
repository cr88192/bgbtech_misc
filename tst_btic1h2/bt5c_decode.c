/*

Block Format:
 (63:32): Pixel Selectors
 (31:16): ColorB
 (15: 0): ColorA

 */

#define		BTPIC_TWOCC(a, b)			((a)|((b)<<8))
#define		BTPIC_FOURCC(a, b, c, d)	((a)|((b)<<8)|((c)<<16)|((d)<<24))

#define		BTPIC_TCC_IX	BTPIC_TWOCC('I', 'X')
#define		BTPIC_TCC_PX	BTPIC_TWOCC('P', 'X')
#define		BTPIC_TCC_IZ	BTPIC_TWOCC('I', 'Z')
#define		BTPIC_TCC_PZ	BTPIC_TWOCC('P', 'Z')
#define		BTPIC_TCC_HX	BTPIC_TWOCC('H', 'X')
#define		BTPIC_TCC_PT	BTPIC_TWOCC('P', 'T')

#define		BTPIC_TCC_Z3	BTPIC_TWOCC('Z', '3')
#define		BTPIC_TCC_Z4	BTPIC_TWOCC('Z', '4')
#define		BTPIC_TCC_Z5	BTPIC_TWOCC('Z', '5')
#define		BTPIC_TCC_Z6	BTPIC_TWOCC('Z', '6')
#define		BTPIC_TCC_Z7	BTPIC_TWOCC('Z', '7')

#ifdef BTIC4B_QFL_PFRAME
#ifndef BTPIC_QFL_PFRAME
#define BTPIC_QFL_PFRAME	BTIC4B_QFL_PFRAME
#endif
#endif

#ifndef BTPIC_QFL_PFRAME
#define BTPIC_QFL_PFRAME	0x100
// #define BTPIC_QFL_PFRAME	0x200
#endif

typedef	uint8_t		byte;
typedef	uint16_t	u16;
typedef	uint32_t	u32;
typedef	uint64_t	u64;

typedef	int8_t		sbyte;
typedef	int16_t		s16;
typedef	int32_t		s32;
typedef	int64_t		s64;

#ifdef _M_IX86
#define		unaligned
#endif

#ifndef unaligned
#define		unaligned	__unaligned
#endif

#ifndef btpic_getu16
#define		btpic_getu16(p)		(*(unaligned u16 *)(p))
#define		btpic_getu32(p)		(*(unaligned u32 *)(p))
#define		btpic_getu64(p)		(*(unaligned u64 *)(p))

#define		btpic_setu16(p, v)	((*(unaligned u16 *)(p))=(v))
#define		btpic_setu32(p, v)	((*(unaligned u32 *)(p))=(v))
#define		btpic_setu64(p, v)	((*(unaligned u64 *)(p))=(v))
#endif

typedef struct BTIC5C_DecodeContext_s BTIC5C_DecodeContext;

struct BTIC5C_DecodeContext_s {
	u64 *blka;
	u64 *lblka;
	
	byte *cs;			//current position

	int xs;				//image X size (pixels)
	int ys;				//image Y size (pixels)
	int bxs;			//image X size (4x4 blocks)
	int bys;			//image Y size (4x4 blocks)
	int cxs;			//image X size (16x16 blocks)
	int cys;			//image Y size (16x16 blocks)
	int fl;
	
	u32 clrab;			//Color A/B endpoints

	short n_dummy;
	byte dummy_mode;
	sbyte skip_ox;
	sbyte skip_oy;
	byte dummy_ldy;


	byte clrhist_pos;	//color history position
	byte blkhist_pos;	//block history position

	u32 clrhist[16];	//color history
	u32 blkhist[16];	//block history
	
	byte *zfbuf;		//LZ Frame temporary buffer
	int zfbsz;			//LZ Frame buffer size
};

const u16 btic5c_clr_dyptab[8]=
	{ 0x0000, 0x0842, 0x1084, 0x18C6, 0x2108, 0x294A, 0x318C, 0x39CE };

const u32 btic5c_clr_deltatab[32]={
	0x00020002, 0x001E001E, 0x00400040, 0x00420042, 
	0x005E005E, 0x001E001E, 0x00020002, 0x001E001E, 
	0x08000800, 0x08020802, 0x081E081E, 0x08400840, 
	0x08420842, 0x085E085E, 0x081E081E, 0x08020802, 
	0x081E081E, 0x78007800, 0x78027802, 0x781E781E, 
	0x78407840, 0x78427842, 0x785E785E, 0x781E781E, 
	0x78027802, 0x781E781E, 0x08420842, 0x7BDE7BDE, 
	0x10841084, 0x739C739C, 0x18C618C6, 0x6B5A6B5A, 
};

const u32 btic5c_blk_pat6tab[64]={
	0xFFFFFFFF, 0x2F2F2F2F, 0x3F3F3F3F, 0x33333333, 
	0xAAAAAAAA, 0xF8F8F8F8, 0xFCFCFCFC, 0xCCCCCCCC, 
	0x00AAFFFF, 0x030B2FFF, 0x03033FFF, 0x333333FF, 
	0x0000AAFF, 0xC0E0F8FF, 0xC0C0FCFF, 0xCCCCCCFF, 
	0x00FFFFFF, 0x030F0FFF, 0x033F33FF, 0x333333FF, 
	0x000000FF, 0xC0F0F0FF, 0xC0FCCCFF, 0xCCCCCCFF, 
	0x00FF00FF, 0x03FF03FF, 0x03FF03FF, 0x33FF33FF, 
	0x00FF00FF, 0xC0FFC0FF, 0xC0FFC0FF, 0xCCFFCCFF, 
	0xAAAAAAAA, 0x0B0B0B0B, 0x03030303, 0x33333333, 
	0x00000000, 0xE0E0E0E0, 0xC0C0C0C0, 0xCCCCCCCC, 
	0xFFFFAA00, 0xFF2F0B03, 0xFF3F0303, 0xFF333333, 
	0xFFAA0000, 0xFFF8E0C0, 0xFFFCC0C0, 0xFFCCCCCC, 
	0xFFFFFF00, 0xFF0F0F03, 0xFF333F03, 0xFF333333, 
	0xFF000000, 0xFFF0F0C0, 0xFFCCFCC0, 0xFFCCCCCC, 
	0xFF00FF00, 0xFF03FF03, 0xFF03FF03, 0xFF33FF33, 
	0xFF00FF00, 0xFFC0FFC0, 0xFFC0FFC0, 0xFFCCFFCC,
};

const sbyte btic5c_skip_xoffs[32]={
	 0, -1,  1, -2,  2,
	 0, -1,  1, -2,  2,
	 0, -1,  1, -2,  2,
	 0, -1,  1, -2,  2,
	 0, -1,  1, -2,  2,
	-3,  3, -4,  4,
	 0,  0,  0
	};
const sbyte btic5c_skip_yoffs[32]={
	 0,  0,  0,  0,  0,
	-1, -1, -1, -1, -1,
	 1,  1,  1,  1,  1,
	-2, -2, -2, -2, -2,
	 2,  2,  2,  2,  2,
	 0,  0,  0,  0,
	-3,  3,  4
	};

const u32 btic5c_blk_pat221[16]={
	0x00000000, 0x00000F0F, 0x0000F0F0, 0x0000FFFF,
	0x0F0F0000, 0x0F0F0F0F, 0x0F0FF0F0, 0x0F0FFFFF,
	0xF0F00000, 0xF0F00F0F, 0xF0F0F0F0, 0xF0F0FFFF,
	0xFFFF0000, 0xFFFF0F0F, 0xFFFFF0F0, 0xFFFFFFFF
	};

const u16 btic5c_blk_pat421[256] = {
	0x0000, 0x0003, 0x000C, 0x000F, 0x0030, 0x0033, 0x003C, 0x003F, 
	0x00C0, 0x00C3, 0x00CC, 0x00CF, 0x00F0, 0x00F3, 0x00FC, 0x00FF, 
	0x0300, 0x0303, 0x030C, 0x030F, 0x0330, 0x0333, 0x033C, 0x033F, 
	0x03C0, 0x03C3, 0x03CC, 0x03CF, 0x03F0, 0x03F3, 0x03FC, 0x03FF, 
	0x0C00, 0x0C03, 0x0C0C, 0x0C0F, 0x0C30, 0x0C33, 0x0C3C, 0x0C3F, 
	0x0CC0, 0x0CC3, 0x0CCC, 0x0CCF, 0x0CF0, 0x0CF3, 0x0CFC, 0x0CFF, 
	0x0F00, 0x0F03, 0x0F0C, 0x0F0F, 0x0F30, 0x0F33, 0x0F3C, 0x0F3F, 
	0x0FC0, 0x0FC3, 0x0FCC, 0x0FCF, 0x0FF0, 0x0FF3, 0x0FFC, 0x0FFF, 
	0x3000, 0x3003, 0x300C, 0x300F, 0x3030, 0x3033, 0x303C, 0x303F, 
	0x30C0, 0x30C3, 0x30CC, 0x30CF, 0x30F0, 0x30F3, 0x30FC, 0x30FF, 
	0x3300, 0x3303, 0x330C, 0x330F, 0x3330, 0x3333, 0x333C, 0x333F,
	0x33C0, 0x33C3, 0x33CC, 0x33CF, 0x33F0, 0x33F3, 0x33FC, 0x33FF,
	0x3C00, 0x3C03, 0x3C0C, 0x3C0F, 0x3C30, 0x3C33, 0x3C3C, 0x3C3F, 
	0x3CC0, 0x3CC3, 0x3CCC, 0x3CCF, 0x3CF0, 0x3CF3, 0x3CFC, 0x3CFF, 
	0x3F00, 0x3F03, 0x3F0C, 0x3F0F, 0x3F30, 0x3F33, 0x3F3C, 0x3F3F, 
	0x3FC0, 0x3FC3, 0x3FCC, 0x3FCF, 0x3FF0, 0x3FF3, 0x3FFC, 0x3FFF, 
	0xC000, 0xC003, 0xC00C, 0xC00F, 0xC030, 0xC033, 0xC03C, 0xC03F, 
	0xC0C0, 0xC0C3, 0xC0CC, 0xC0CF, 0xC0F0, 0xC0F3, 0xC0FC, 0xC0FF, 
	0xC300, 0xC303, 0xC30C, 0xC30F, 0xC330, 0xC333, 0xC33C, 0xC33F, 
	0xC3C0, 0xC3C3, 0xC3CC, 0xC3CF, 0xC3F0, 0xC3F3, 0xC3FC, 0xC3FF, 
	0xCC00, 0xCC03, 0xCC0C, 0xCC0F, 0xCC30, 0xCC33, 0xCC3C, 0xCC3F, 
	0xCCC0, 0xCCC3, 0xCCCC, 0xCCCF, 0xCCF0, 0xCCF3, 0xCCFC, 0xCCFF, 
	0xCF00, 0xCF03, 0xCF0C, 0xCF0F, 0xCF30, 0xCF33, 0xCF3C, 0xCF3F, 
	0xCFC0, 0xCFC3, 0xCFCC, 0xCFCF, 0xCFF0, 0xCFF3, 0xCFFC, 0xCFFF, 
	0xF000, 0xF003, 0xF00C, 0xF00F, 0xF030, 0xF033, 0xF03C, 0xF03F, 
	0xF0C0, 0xF0C3, 0xF0CC, 0xF0CF, 0xF0F0, 0xF0F3, 0xF0FC, 0xF0FF, 
	0xF300, 0xF303, 0xF30C, 0xF30F, 0xF330, 0xF333, 0xF33C, 0xF33F, 
	0xF3C0, 0xF3C3, 0xF3CC, 0xF3CF, 0xF3F0, 0xF3F3, 0xF3FC, 0xF3FF, 
	0xFC00, 0xFC03, 0xFC0C, 0xFC0F, 0xFC30, 0xFC33, 0xFC3C, 0xFC3F, 
	0xFCC0, 0xFCC3, 0xFCCC, 0xFCCF, 0xFCF0, 0xFCF3, 0xFCFC, 0xFCFF, 
	0xFF00, 0xFF03, 0xFF0C, 0xFF0F, 0xFF30, 0xFF33, 0xFF3C, 0xFF3F, 
	0xFFC0, 0xFFC3, 0xFFCC, 0xFFCF, 0xFFF0, 0xFFF3, 0xFFFC, 0xFFFF, 
};

const u32 btic5c_blk_pat222[256] = {
	0x00000000, 0x00000505, 0x00000A0A, 0x00000F0F,
	0x00005050, 0x00005555, 0x00005A5A, 0x00005F5F,
	0x0000A0A0, 0x0000A5A5, 0x0000AAAA, 0x0000AFAF,
	0x0000F0F0, 0x0000F5F5, 0x0000FAFA, 0x0000FFFF,
	0x05050000, 0x05050505, 0x05050A0A, 0x05050F0F,
	0x05055050, 0x05055555, 0x05055A5A, 0x05055F5F,
	0x0505A0A0, 0x0505A5A5, 0x0505AAAA, 0x0505AFAF,
	0x0505F0F0, 0x0505F5F5, 0x0505FAFA, 0x0505FFFF,
	0x0A0A0000, 0x0A0A0505, 0x0A0A0A0A, 0x0A0A0F0F,
	0x0A0A5050, 0x0A0A5555, 0x0A0A5A5A, 0x0A0A5F5F,
	0x0A0AA0A0, 0x0A0AA5A5, 0x0A0AAAAA, 0x0A0AAFAF,
	0x0A0AF0F0, 0x0A0AF5F5, 0x0A0AFAFA, 0x0A0AFFFF,
	0x0F0F0000, 0x0F0F0505, 0x0F0F0A0A, 0x0F0F0F0F,
	0x0F0F5050, 0x0F0F5555, 0x0F0F5A5A, 0x0F0F5F5F,
	0x0F0FA0A0, 0x0F0FA5A5, 0x0F0FAAAA, 0x0F0FAFAF,
	0x0F0FF0F0, 0x0F0FF5F5, 0x0F0FFAFA, 0x0F0FFFFF,
	0x50500000, 0x50500505, 0x50500A0A, 0x50500F0F,
	0x50505050, 0x50505555, 0x50505A5A, 0x50505F5F,
	0x5050A0A0, 0x5050A5A5, 0x5050AAAA, 0x5050AFAF,
	0x5050F0F0, 0x5050F5F5, 0x5050FAFA, 0x5050FFFF,
	0x55550000, 0x55550505, 0x55550A0A, 0x55550F0F,
	0x55555050, 0x55555555, 0x55555A5A, 0x55555F5F,
	0x5555A0A0, 0x5555A5A5, 0x5555AAAA, 0x5555AFAF,
	0x5555F0F0, 0x5555F5F5, 0x5555FAFA, 0x5555FFFF,
	0x5A5A0000, 0x5A5A0505, 0x5A5A0A0A, 0x5A5A0F0F,
	0x5A5A5050, 0x5A5A5555, 0x5A5A5A5A, 0x5A5A5F5F,
	0x5A5AA0A0, 0x5A5AA5A5, 0x5A5AAAAA, 0x5A5AAFAF,
	0x5A5AF0F0, 0x5A5AF5F5, 0x5A5AFAFA, 0x5A5AFFFF,
	0x5F5F0000, 0x5F5F0505, 0x5F5F0A0A, 0x5F5F0F0F,
	0x5F5F5050, 0x5F5F5555, 0x5F5F5A5A, 0x5F5F5F5F,
	0x5F5FA0A0, 0x5F5FA5A5, 0x5F5FAAAA, 0x5F5FAFAF,
	0x5F5FF0F0, 0x5F5FF5F5, 0x5F5FFAFA, 0x5F5FFFFF,
	0xA0A00000, 0xA0A00505, 0xA0A00A0A, 0xA0A00F0F,
	0xA0A05050, 0xA0A05555, 0xA0A05A5A, 0xA0A05F5F,
	0xA0A0A0A0, 0xA0A0A5A5, 0xA0A0AAAA, 0xA0A0AFAF,
	0xA0A0F0F0, 0xA0A0F5F5, 0xA0A0FAFA, 0xA0A0FFFF,
	0xA5A50000, 0xA5A50505, 0xA5A50A0A, 0xA5A50F0F,
	0xA5A55050, 0xA5A55555, 0xA5A55A5A, 0xA5A55F5F,
	0xA5A5A0A0, 0xA5A5A5A5, 0xA5A5AAAA, 0xA5A5AFAF,
	0xA5A5F0F0, 0xA5A5F5F5, 0xA5A5FAFA, 0xA5A5FFFF,
	0xAAAA0000, 0xAAAA0505, 0xAAAA0A0A, 0xAAAA0F0F,
	0xAAAA5050, 0xAAAA5555, 0xAAAA5A5A, 0xAAAA5F5F,
	0xAAAAA0A0, 0xAAAAA5A5, 0xAAAAAAAA, 0xAAAAAFAF,
	0xAAAAF0F0, 0xAAAAF5F5, 0xAAAAFAFA, 0xAAAAFFFF,
	0xAFAF0000, 0xAFAF0505, 0xAFAF0A0A, 0xAFAF0F0F,
	0xAFAF5050, 0xAFAF5555, 0xAFAF5A5A, 0xAFAF5F5F,
	0xAFAFA0A0, 0xAFAFA5A5, 0xAFAFAAAA, 0xAFAFAFAF,
	0xAFAFF0F0, 0xAFAFF5F5, 0xAFAFFAFA, 0xAFAFFFFF,
	0xF0F00000, 0xF0F00505, 0xF0F00A0A, 0xF0F00F0F,
	0xF0F05050, 0xF0F05555, 0xF0F05A5A, 0xF0F05F5F,
	0xF0F0A0A0, 0xF0F0A5A5, 0xF0F0AAAA, 0xF0F0AFAF,
	0xF0F0F0F0, 0xF0F0F5F5, 0xF0F0FAFA, 0xF0F0FFFF,
	0xF5F50000, 0xF5F50505, 0xF5F50A0A, 0xF5F50F0F,
	0xF5F55050, 0xF5F55555, 0xF5F55A5A, 0xF5F55F5F,
	0xF5F5A0A0, 0xF5F5A5A5, 0xF5F5AAAA, 0xF5F5AFAF,
	0xF5F5F0F0, 0xF5F5F5F5, 0xF5F5FAFA, 0xF5F5FFFF,
	0xFAFA0000, 0xFAFA0505, 0xFAFA0A0A, 0xFAFA0F0F,
	0xFAFA5050, 0xFAFA5555, 0xFAFA5A5A, 0xFAFA5F5F,
	0xFAFAA0A0, 0xFAFAA5A5, 0xFAFAAAAA, 0xFAFAAFAF,
	0xFAFAF0F0, 0xFAFAF5F5, 0xFAFAFAFA, 0xFAFAFFFF,
	0xFFFF0000, 0xFFFF0505, 0xFFFF0A0A, 0xFFFF0F0F,
	0xFFFF5050, 0xFFFF5555, 0xFFFF5A5A, 0xFFFF5F5F,
	0xFFFFA0A0, 0xFFFFA5A5, 0xFFFFAAAA, 0xFFFFAFAF,
	0xFFFFF0F0, 0xFFFFF5F5, 0xFFFFFAFA, 0xFFFFFFFF,
};


BTIC5C_DecodeContext *BTIC5C_AllocDecodeContext()
{
	BTIC5C_DecodeContext *ctx;
	ctx=malloc(sizeof(BTIC5C_DecodeContext));
	memset(ctx, 0, sizeof(BTIC5C_DecodeContext));
	return(ctx);
}

int BTIC5C_DecodeBufferRP2(
	byte *ibuf, byte *obuf, int ibsz, int obsz)
{
	u32 tag;
	byte *cs, *ct, *cse, *cs1, *cs1e, *ct1e;
	int pl, pd;
	int rl, l, d;
	u64 t0, v0, v1;
	int t1, t2;
	
	cs=ibuf; cse=ibuf+ibsz;
	ct=obuf;
	pl=0; pd=0;
	
	while(1)
	{
		t0=*(u64 *)cs;
		if(!(t0&0x01))
		{
			cs+=2;
			rl=(t0>>1)&7;
			l=((t0>>4)&7)+3;
			d=(t0>>7)&511;
		}else
			if(!(t0&0x02))
		{
			cs+=3;
			rl=(t0>>2)&7;
			l=((t0>>5)&63)+4;
			d=(t0>>11)&8191;
		}else
			if(!(t0&0x04))
		{
			cs+=4;
			rl=(t0>>3)&7;
			l=((t0>>6)&511)+4;
			d=(t0>>15)&131071;
		}else
			if(!(t0&0x08))
		{
			cs++;
			t1=(t0>>4)&15;
			rl=(t1+1)*8;
//			W_RawCopyB(ct, cs, rl);
//			cs+=rl;
//			ct+=rl;

			cs1e=cs+rl;
			ct1e=ct+rl;
			while(ct<ct1e)
			{
				v0=((u64 *)cs)[0];
				v1=((u64 *)cs)[1];
				cs+=16;
				((u64 *)ct)[0]=v0;
				((u64 *)ct)[1]=v1;
				ct+=16;
			}			
			cs=cs1e;
			ct=ct1e;

			continue;
		}else
			if(!(t0&0x10))
		{
#if 1
			if(t0&0x100)
			{
				rl=(t0>>5)&7;
				l=((t0>>9)&255)+68;
				d=((t0>>17)&127)+1;
				cs+=3;
			}else
			{
				rl=(t0>>5)&7;
				l=((t0>> 9)&15)+11;
				d=((t0>>13)& 7)+1;
				cs+=2;
			}
#endif

#if 0
			/* Long Match, RP2-Org */
			cs++;
			rl=(t0>>5)&7;
			t1=t0>>8;
			if(!(t1&1))
				{ l=((t1>>1)&0x007F)+4; cs+=1; t2=t0>>16; }
			else
				{ l=((t1>>2)&0x3FFF)+4; cs+=2; t2=t0>>24; }
			if(!(t2&1))
				{ d=((t2>>1)&0x007FFF); cs+=2; }
			else
				{ d=((t2>>2)&0x3FFFFF); cs+=3; }
#endif
		}else
			if(!(t0&0x20))
		{
			cs++;
			rl=(t0>>6)&3;
			if(!rl)break;
			*(u32 *)ct=*(u32 *)cs;
			cs+=rl;
			ct+=rl;
			continue;
		}else
			if(!(t0&0x40))
		{
			/* Long Raw */
			cs+=2;
			t1=(t0>>7)&511;
			rl=(t1+1)*8;
//			W_RawCopyB(ct, cs, rl);
//			cs+=rl;
//			ct+=rl;

			cs1e=cs+rl;
			ct1e=ct+rl;
			while(ct<ct1e)
			{
				v0=((u64 *)cs)[0];
				v1=((u64 *)cs)[1];
				cs+=16;
				((u64 *)ct)[0]=v0;
				((u64 *)ct)[1]=v1;
				ct+=16;
			}			
			cs=cs1e;
			ct=ct1e;
			continue;
		}else if(!(t0&0x80))
		{
			if(!(t0&0x100))
			{
				rl=0;
				l=((t0>>9)&127)+4;
				d=1;
				cs+=2;
			}else
			{
				rl=(t0>>9)&7;
				l=((t0>>12)&16383)+4;
				d=(*((u32 *)(cs+3))>>2)&0x3FFFFF;
				cs+=6;
			}
		}else
		{
			__debugbreak();
		}

		*(u64 *)ct=*(u64 *)cs;
		cs+=rl;
		ct+=rl;
		
		cs1=ct-d;
		ct1e=ct+l;
		
		if(d<16)
		{
			v0=((u64 *)cs1)[0];
			v1=((u64 *)cs1)[1];
			while(ct<ct1e)
			{
				((u64 *)ct)[0]=v0;
				((u64 *)ct)[1]=v1;
				ct+=d;
			}
		}else
		{
			while(ct<ct1e)
			{
				v0=((u64 *)cs1)[0];
				v1=((u64 *)cs1)[1];
				cs1+=16;
				((u64 *)ct)[0]=v0;
				((u64 *)ct)[1]=v1;
				ct+=16;
			}
		}
		
		ct=ct1e;
	}
	
	return(ct-obuf);
}

u32 BTIC5C_DecodeEndpointPred(BTIC5C_DecodeContext *ctx,
	byte **rcs, int pred)
{
	byte *cs;
	int pxa, pxb, pxc, dy, dx, dz, pos;
	u32 tg, px, px1;
	
	if(!pred)
		{ return(ctx->clrab); }
	
	cs=*rcs;
	tg=btpic_getu32(cs);

#if 0
	dy=tg&0x0F;
	if(!(tg&1))		dy=0;
	else
		if((tg&3)==3)	dy=3;
	else
		if(!(tg&4))		dy=1;

	printf("P%01X ", dy);
#endif

	if(!(tg&1))
	{
		pxc=(tg>>1)&0x7BDE;
		dy=((tg>>1)&1)|(((tg>>6)&1)<<1)|(((tg>>11)&1)<<2);
		pxa=(pxc+btic5c_clr_dyptab[dy])&0x7BDE;
		pxb=(pxc-btic5c_clr_dyptab[dy])&0x7BDE;
		px=(pxa<<16)|pxb;
		pos=ctx->clrhist_pos;
		pos=(pos-1)&15;
		ctx->clrab=px;
		ctx->clrhist[pos]=px;
		ctx->clrhist_pos=pos;
		*rcs=cs+2;
		return(px);
	}

	if(!(tg&2))
	{
		if(!(tg&4))
		{
			px=ctx->clrab;
			px=(px+btic5c_clr_deltatab[(tg>>3)&31])&0x7BDE7BDEU;
//			px=px+btic5c_clr_deltatab[(tg>>3)&31];
			pos=ctx->clrhist_pos;
			pos=(pos-1)&15;
			ctx->clrhist[pos]=px;
			ctx->clrhist_pos=pos;
			ctx->clrab=px;
			*rcs=cs+1;
			return(px);
		}else if(!(tg&8))
		{
			dy=(tg>>4)&15;
			pos=ctx->clrhist_pos;
			px=ctx->clrhist[(pos+dy)&15];
			if(dy==15)
			{
				pos=(pos-1)&15;
				ctx->clrhist_pos=pos;
			}
			ctx->clrab=px;
			*rcs=cs+1;
			return(px);
		}
		
		pos=ctx->clrhist_pos;
		dx=(tg>>11)&31;
		dy=(tg>> 6)&31;

		if(!(tg&0x10))
		{
			px=ctx->clrab;
			if(!(tg&0x20))
			{
				px=px+
					(btic5c_clr_deltatab[dx]&0xFFFF0000U) +
					(btic5c_clr_deltatab[dy]&0x0000FFFFU) ;
				px=px&0x7BDE7BDEU;
			}else
			{
				px=(px+btic5c_clr_deltatab[dy])&0x7BDE7BDEU;
				px=(px+btic5c_clr_deltatab[dx])&0x7BDE7BDEU;
//				__debugbreak();
			}

//			printf("D:%08X ", px);
		}else
		{
			pxa=ctx->clrhist[(pos+dx)&15];
			pxb=ctx->clrhist[(pos+dy)&15];
			if(dx&16)	pxa>>=16;
			if(dy&16)	pxb>>=16;
			pxa&=0xFFFF;
			pxb&=0xFFFF;
			px=(pxa<<16)|pxb;
		}

		pos=(pos-1)&15;
		ctx->clrab=px;
		ctx->clrhist[pos]=px;
		ctx->clrhist_pos=pos;

		*rcs=cs+2;
		return(px);
	}
	
	pxa=(tg>>17)&0x7FFF;
	pxb=(tg>> 2)&0x7FFF;

//	pxa&=0x7BDE;
//	pxb&=0x7BDE;

	px=(pxa<<16)|pxb;
	pos=ctx->clrhist_pos;
	pos=(pos-1)&15;
	ctx->clrab=px;
	ctx->clrhist[pos]=px;
	ctx->clrhist_pos=pos;
	*rcs=cs+4;
	return(px);
}

u64 BTIC5C_DecodeBlock_Dummy(BTIC5C_DecodeContext *ctx, int c_bx, int c_by)
{
	byte *cs;
	u64 blk;
	u32 px, clr;
	int bx, by, md, dy;

	ctx->n_dummy--;
	md=ctx->dummy_mode;

	if(md==1)
	{
		bx=c_bx+ctx->skip_ox;
		by=c_by+ctx->skip_oy;
		return(ctx->lblka[by*ctx->bxs+bx]);
	}

	if(md==2)
		{ return(ctx->clrab); }
	if(md==3)
		{ return(ctx->clrab|0xFFFFFFFF00000000ULL); }

	if(md==4)
	{
		cs=ctx->cs;
		clr=ctx->clrab;
		dy=btpic_getu16(cs);
		px=         btic5c_blk_pat421[(dy>>8)&0xFF];
		px=(px<<16)|btic5c_blk_pat421[(dy>>0)&0xFF];
		blk=(((u64)px)<<32)|clr;
		ctx->cs=cs+2;
		return(blk);
	}

	if(md==5)
	{
		dy=*ctx->cs++;
		clr=ctx->clrab;
		px=btic5c_blk_pat222[dy];
		blk=(((u64)px)<<32)|clr;
		return(blk);
	}

	if(md==6)
	{
		dy=*ctx->cs++;
		ctx->dummy_mode=7;
		ctx->dummy_ldy=dy;
		clr=ctx->clrab;
		px=btic5c_blk_pat221[dy&15];
		blk=(((u64)px)<<32)|clr;
		return(blk);
	}

	if(md==7)
	{
		dy=ctx->dummy_ldy;
		ctx->dummy_mode=6;
		clr=ctx->clrab;
		px=btic5c_blk_pat221[dy>>4];
		blk=(((u64)px)<<32)|clr;
		return(blk);
	}

	if(md==8)
	{
		cs=ctx->cs;
		clr=BTIC5C_DecodeEndpointPred(ctx, &cs, 1);
		blk=clr;
		ctx->cs=cs;
		return(blk);
	}

	return(0);
}

u64 BTIC5C_DecodeBlockBase(BTIC5C_DecodeContext *ctx, int c_bx, int c_by)
{
	byte *cs;
	int pxa, pxb, pxc, dy, pos;
	u32 tg, px, px1, clr;
	u64 blk;
	
	if(ctx->n_dummy)
		{ return(BTIC5C_DecodeBlock_Dummy(ctx, c_bx, c_by)); }
	
	cs=ctx->cs;
	tg=btpic_getu32(cs);
	
//	printf("T%02X ", tg&0xFF);
	
	if(!(tg&0x01))
	{
		px=btic5c_blk_pat6tab[(tg>>2)&63];
		cs++;
		clr=BTIC5C_DecodeEndpointPred(ctx, &cs, tg&2);
		blk=(((u64)px)<<32)|clr;
		ctx->cs=cs;
		return(blk);
	}

	if(!(tg&0x02))
	{
		if(!(tg&4))
		{
			/* skip */
			dy=(tg>>3)&31;
			if(dy<31)
			{
				ctx->skip_ox=btic5c_skip_xoffs[dy];
				ctx->skip_oy=btic5c_skip_yoffs[dy];
			}
			ctx->n_dummy=((tg>>8)&255)+1;
			ctx->dummy_mode=1;
			ctx->cs=cs+2;
			return(BTIC5C_DecodeBlock_Dummy(ctx, c_bx, c_by));
		}
		dy=(tg>>4)&15;
		pos=ctx->blkhist_pos;
		px=ctx->blkhist[(pos+dy)&15];
		if(dy==15)
			{ ctx->blkhist_pos=(pos-1)&15; }
		cs++;
		clr=BTIC5C_DecodeEndpointPred(ctx, &cs, tg&8);
		blk=(((u64)px)<<32)|clr;
		ctx->cs=cs;
		return(blk);
	}

	if(!(tg&0x04))
	{
		dy=(tg>>4)&15;
		cs++;
		px=btic5c_blk_pat221[dy];
		clr=BTIC5C_DecodeEndpointPred(ctx, &cs, tg&8);
		blk=(((u64)px)<<32)|clr;
		ctx->cs=cs;
		return(blk);
	}

	if(!(tg&0x08))
	{
		if(!(tg&0x10))
		{
			dy=((tg>>6)&3)+2;
			cs++;
//			clr=BTIC5C_DecodeEndpointPred(ctx, &cs, tg&0x20);
			ctx->n_dummy=dy;
			ctx->dummy_mode=4;
			if(tg&0x20)
				ctx->dummy_mode=8;
			ctx->cs=cs;
			return(BTIC5C_DecodeBlock_Dummy(ctx, c_bx, c_by));
		}

		dy=((tg>>6)&3)+1;
		cs++;
		ctx->cs=cs;
		if(!(tg&0x20))
		{
			ctx->n_dummy=dy*2;
			ctx->dummy_mode=6;
		}else
		{
			ctx->n_dummy=dy+1;
			ctx->dummy_mode=5;
		}
		return(BTIC5C_DecodeBlock_Dummy(ctx, c_bx, c_by));
	}

	if(!(tg&0x10))
	{
		dy=((tg>>6)&3)+2;
		ctx->cs=cs+1;
		ctx->n_dummy=dy;
		ctx->dummy_mode=2+((tg>>5)&1);
		return(BTIC5C_DecodeBlock_Dummy(ctx, c_bx, c_by));
	}

	if(!(tg&0x20))
	{
		if(tg&0x40)
		{
			cs+=2;
			dy=(tg>>8)&255;
			px=btic5c_blk_pat222[dy];
		}else
		{
			cs+=3;
			dy=(tg>>8)&0xFFFF;
			px=         btic5c_blk_pat421[(dy>>8)&0xFF];
			px=(px<<16)|btic5c_blk_pat421[(dy>>0)&0xFF];
		}

		pos=ctx->blkhist_pos;
		pos=(pos-1)&15;
		ctx->blkhist[pos]=px;
		ctx->blkhist_pos=pos;

		clr=BTIC5C_DecodeEndpointPred(ctx, &cs, tg&0x80);
		blk=(((u64)px)<<32)|clr;
		ctx->cs=cs;
		return(blk);
	}

	if(!(tg&0x40))
	{
		cs++;
		px=btpic_getu32(cs);
		cs+=4;

		pos=ctx->blkhist_pos;
		pos=(pos-1)&15;
		ctx->blkhist[pos]=px;
		ctx->blkhist_pos=pos;

		clr=BTIC5C_DecodeEndpointPred(ctx, &cs, tg&0x80);
		blk=(((u64)px)<<32)|clr;
		ctx->cs=cs;
		return(blk);
	}

	if(!(tg&0x80))
	{
		if(((tg>>8)&3)==0)
		{
			dy=((tg>>10)&63)+4;
			ctx->cs=cs+2;
			ctx->n_dummy=dy;
			ctx->dummy_mode=2;
			return(BTIC5C_DecodeBlock_Dummy(ctx, c_bx, c_by));
		}
	}

	return(0);
}

int BTIC5C_DecodeBlockSuper(
	BTIC5C_DecodeContext *ctx, int c_cx, int c_cy)
{
	static const byte xotab[16]=
		{ 0, 1, 1, 0,  0, 0, 1, 1,  2, 2, 3, 3,  3, 2, 2, 3 };
	static const byte yotab[16]=
		{ 0, 0, 1, 1,  2, 3, 3, 2,  2, 3, 3, 2,  1, 1, 0, 0 };
	byte *cs;
	u64 *blka, *lblka;
	u64 blk;
	u32 tg;
	int dy;
	int bbx, bby, bbxl, bbyl, bx, by, bxl, byl, bxs;
	int i, j, k;

	if(ctx->n_dummy<=0)
	{
		cs=ctx->cs;
		tg=btpic_getu32(cs);
		
		if((tg&7)==0x01)
		{
			/* special case skips at superblock level */
			dy=(tg>>3)&31;
			if(dy<31)
			{
				ctx->skip_ox=btic5c_skip_xoffs[dy];
				ctx->skip_oy=btic5c_skip_yoffs[dy];
			}
			ctx->n_dummy=((tg>>8)&255)+1;
			ctx->dummy_mode=1;
			ctx->cs=cs+2;
		}
	}

	if(ctx->n_dummy>=16)
	{
		if(ctx->dummy_mode==1)
		{
			blka=ctx->blka;
			lblka=ctx->lblka;
			bxs=ctx->bxs;
			bbx=c_cx*4;	bby=c_cy*4;
			bbxl=bbx+ctx->skip_ox;
			bbyl=bby+ctx->skip_oy;

			for(i=0; i<4; i++)
				for(j=0; j<4; j++)
			{
				by=bby+i; bx=bbx+j;
				byl=bbyl+i; bxl=bbxl+j;
				blk=lblka[byl*bxs+bxl];
				blka[by*bxs+bx]=blk;
			}
			ctx->n_dummy-=16;
			return(0);
		}
	}

	blka=ctx->blka;
	bxs=ctx->bxs;
	bbx=c_cx*4;	bby=c_cy*4;
	for(i=0; i<16; i++)
	{
		bx=bbx+xotab[i];	by=bby+yotab[i];
		blk=BTIC5C_DecodeBlockBase(ctx, bx, by);
		blka[by*bxs+bx]=blk;
	}
	return(0);
}

int BTIC5C_DecodeBlockPlane(
	BTIC5C_DecodeContext *ctx, byte *idat)
{
	int cx, cy, cxs, cys;
	
	ctx->cs=idat;
	cxs=ctx->cxs;
	cys=ctx->cys;
	for(cy=0; cy<cys; cy++)
		for(cx=0; cx<cxs; cx++)
	{
		BTIC5C_DecodeBlockSuper(ctx, cx, cy);
	}
	return(0);
}

u16 BTIC5C_UnpackCell_Blend555Interp(u16 clra, u16 clrb)
{
	int cr0, cg0, cb0;
	int cr1, cg1, cb1;
	int cr2, cg2, cb2;
	int px;
	
	cr0=(clra>>10)&31;	cg0=(clra>> 5)&31;	cb0=(clra>> 0)&31;
	cr1=(clrb>>10)&31;	cg1=(clrb>> 5)&31;	cb1=(clrb>> 0)&31;
	cr2=(5*cr0+3*cr1)>>3;
	cg2=(5*cg0+3*cg1)>>3;
	cb2=(5*cb0+3*cb1)>>3;
	px=(cr2<<10)+(cg2<<5)+cb2;
	return(px);
}

int BTIC5C_UnpackCell_RGB555(
	u64 cblk, u16 *ibuf, int ystr)
{
	u16 *ct;
	u16 ctab[4];
	u16 clra, clrb, clrc, clrd, cma;
	u16 px0, px1, px2, px3, px4, px5;
	u64 pv;
	u32 px;
	
	clra=(cblk>> 0)&0x7FFF;
	clrb=(cblk>>16)&0x7FFF;
	
	if(clra==clrb)
	{
		ct=ibuf;
		pv=clra|(clra<<16);
		pv|=pv<<32;
		*(u64 *)ct=pv; ct+=ystr;
		*(u64 *)ct=pv; ct+=ystr;
		*(u64 *)ct=pv; ct+=ystr;
		*(u64 *)ct=pv;
//		ct[0]=clra; ct[1]=clra; ct[2]=clra; ct[3]=clra; ct+=ystr;
//		ct[0]=clra; ct[1]=clra; ct[2]=clra; ct[3]=clra; ct+=ystr;
//		ct[0]=clra; ct[1]=clra; ct[2]=clra; ct[3]=clra; ct+=ystr;
//		ct[0]=clra; ct[1]=clra; ct[2]=clra; ct[3]=clra;
		return(0);
	}

	px=cblk>>32;
	if(!((px^(px>>1))&0x55555555U))
	{
		ctab[0]=clra;	ctab[1]=clrb;

		ct=ibuf;
		px0=(px>>0)&1;	px1=(px>>2)&1;	px2=(px>>4)&1;	px3=(px>>6)&1;
		px0=ctab[px0];	px1=ctab[px1];	px2=ctab[px2];	px3=ctab[px3];
		ct[0]=px0;		ct[1]=px1;		ct[2]=px2;		ct[3]=px3;
		ct+=ystr;
		px0=(px>>8)&1;	px1=(px>>10)&1;	px2=(px>>12)&1;	px3=(px>>14)&1;
		px0=ctab[px0];	px1=ctab[px1];	px2=ctab[px2];	px3=ctab[px3];
		ct[0]=px0;		ct[1]=px1;		ct[2]=px2;		ct[3]=px3;
		ct+=ystr;
		px0=(px>>16)&1;	px1=(px>>18)&1;	px2=(px>>20)&1;	px3=(px>>22)&1;
		px0=ctab[px0];	px1=ctab[px1];	px2=ctab[px2];	px3=ctab[px3];
		ct[0]=px0;		ct[1]=px1;		ct[2]=px2;		ct[3]=px3;
		ct+=ystr;
		px0=(px>>24)&1;	px1=(px>>26)&1;	px2=(px>>28)&1;	px3=(px>>30)&1;
		px0=ctab[px0];	px1=ctab[px1];	px2=ctab[px2];	px3=ctab[px3];
		ct[0]=px0;		ct[1]=px1;		ct[2]=px2;		ct[3]=px3;

		return(0);
	}
	
//	cma=0x7BDE;
	cma=0x3DEF;

//	px0=(clra>>1)&cma;	px1=(clrb>>1)&cma;
//	px2=(px0 >>1)&cma;	px3=(px1 >>1)&cma;
//	px4=(px2 >>1)&cma;	px5=(px3 >>1)&cma;
//	clrc=px0+px3+px4;	clrd=px1+px2+px5;

	clrc=BTIC5C_UnpackCell_Blend555Interp(clra, clrb);
	clrd=BTIC5C_UnpackCell_Blend555Interp(clrb, clra);

	ctab[0]=clra;	ctab[1]=clrc;
	ctab[2]=clrd;	ctab[3]=clrb;

//	ctab[0]=clra;	ctab[1]=clra;
//	ctab[2]=clrb;	ctab[3]=clrb;

	ct=ibuf;
	px0=(px>>0)&3;	px1=(px>>2)&3;	px2=(px>>4)&3;	px3=(px>>6)&3;
	px0=ctab[px0];	px1=ctab[px1];	px2=ctab[px2];	px3=ctab[px3];
	ct[0]=px0;		ct[1]=px1;		ct[2]=px2;		ct[3]=px3;
	ct+=ystr;
	px0=(px>>8)&3;	px1=(px>>10)&3;	px2=(px>>12)&3;	px3=(px>>14)&3;
	px0=ctab[px0];	px1=ctab[px1];	px2=ctab[px2];	px3=ctab[px3];
	ct[0]=px0;		ct[1]=px1;		ct[2]=px2;		ct[3]=px3;
	ct+=ystr;
	px0=(px>>16)&3;	px1=(px>>18)&3;	px2=(px>>20)&3;	px3=(px>>22)&3;
	px0=ctab[px0];	px1=ctab[px1];	px2=ctab[px2];	px3=ctab[px3];
	ct[0]=px0;		ct[1]=px1;		ct[2]=px2;		ct[3]=px3;
	ct+=ystr;
	px0=(px>>24)&3;	px1=(px>>26)&3;	px2=(px>>28)&3;	px3=(px>>30)&3;
	px0=ctab[px0];	px1=ctab[px1];	px2=ctab[px2];	px3=ctab[px3];
	ct[0]=px0;		ct[1]=px1;		ct[2]=px2;		ct[3]=px3;
//	ct+=ystr;
	return(0);
}

int BTIC5C_UnpackCellB_RGB555(
	u64 cblk, u16 *ibuf, int ystr)
{
	u64 c2blk;
	int dy;
	if(!(cblk&0x8000))
		{ return(BTIC5C_UnpackCell_RGB555(cblk, ibuf, ystr)); }

	if(((cblk>>48)&15)==1)
	{
		c2blk=cblk&0x7FFF7FFFULL;
		dy=(cblk>>32)&15;
		c2blk|=((u64)btic5c_blk_pat221[dy])<<32;
	}else
		if(((cblk>>48)&15)==2)
	{
		c2blk=cblk&0x7FFF7FFFULL;
		dy=(cblk>>32)&255;
		c2blk|=((u64)btic5c_blk_pat222[dy])<<32;
	}
	else
		if(((cblk>>48)&15)==3)
	{
		c2blk=cblk&0x7FFF7FFFULL;
		dy=(cblk>>32)&63;
		c2blk|=((u64)btic5c_blk_pat6tab[dy])<<32;
	}
	else
		if(((cblk>>48)&15)==4)
	{
		c2blk=cblk&0x7FFF7FFFULL;
		dy=(cblk>>32)&65535;
		c2blk|=((u64)btic5c_blk_pat421[(dy>>0)&255])<<32;
		c2blk|=((u64)btic5c_blk_pat421[(dy>>8)&255])<<48;
	}
	return(BTIC5C_UnpackCell_RGB555(c2blk, ibuf, ystr));
}

void BTIC5C_RGB555_BlockPartCopyH1(u16 *dst, u16 *src, int ystr)
{	dst[0]=src[0];	dst+=ystr; src+=4;
	dst[0]=src[0];	dst+=ystr; src+=4;
	dst[0]=src[0];	dst+=ystr; src+=4;
	dst[0]=src[0];	}
void BTIC5C_RGB555_BlockPartCopyH2(u16 *dst, u16 *src, int ystr)
{	dst[0]=src[0]; dst[1]=src[1];	dst+=ystr; src+=4;
	dst[0]=src[0]; dst[1]=src[1];	dst+=ystr; src+=4;
	dst[0]=src[0]; dst[1]=src[1];	dst+=ystr; src+=4;
	dst[0]=src[0]; dst[1]=src[1];	}
void BTIC5C_RGB555_BlockPartCopyH3(u16 *dst, u16 *src, int ystr)
{	dst[0]=src[0]; dst[1]=src[1]; dst[2]=src[2]; dst+=ystr; src+=4;
	dst[0]=src[0]; dst[1]=src[1]; dst[2]=src[2]; dst+=ystr; src+=4;
	dst[0]=src[0]; dst[1]=src[1]; dst[2]=src[2]; dst+=ystr; src+=4;
	dst[0]=src[0]; dst[1]=src[1]; dst[2]=src[2];	}

void BTIC5C_RGB555_BlockPartCopyV1(u16 *dst, u16 *src, int ystr)
{	dst[0]=src[0]; dst[1]=src[1]; dst[2]=src[2]; dst[3]=src[3];
	dst+=ystr; src+=4;	}
void BTIC5C_RGB555_BlockPartCopyV2(u16 *dst, u16 *src, int ystr)
{	dst[0]=src[0]; dst[1]=src[1]; dst[2]=src[2]; dst[3]=src[3];
	dst+=ystr; src+=4;
	dst[0]=src[0]; dst[1]=src[1]; dst[2]=src[2]; dst[3]=src[3];	}
void BTIC5C_RGB555_BlockPartCopyV3(u16 *dst, u16 *src, int ystr)
{	dst[0]=src[0]; dst[1]=src[1]; dst[2]=src[2]; dst[3]=src[3];
	dst+=ystr; src+=4;
	dst[0]=src[0]; dst[1]=src[1]; dst[2]=src[2]; dst[3]=src[3];
	dst+=ystr; src+=4;
	dst[0]=src[0]; dst[1]=src[1]; dst[2]=src[2]; dst[3]=src[3];	}

int BTIC5C_UnpackCellImage_RGB555(
	u64 *blka, int bxs, int bys,
	u16 *ibuf, int ibxs, int ibys, int ystr)
{
	void (*CopyHN)(u16 *dst, u16 *src, int ystr);
	void (*CopyVN)(u16 *dst, u16 *src, int ystr);
	u16 tpix[16];
	u16 *ct, *ct1;
	int bx, by, bz, bxs0, bys0;
	
	bz=ibxs&3;
	if(bz)
	{
		if(bz==1)	CopyHN=BTIC5C_RGB555_BlockPartCopyH1;
		if(bz==2)	CopyHN=BTIC5C_RGB555_BlockPartCopyH2;
		if(bz==3)	CopyHN=BTIC5C_RGB555_BlockPartCopyH3;
	}
	bz=ibys&3;
	if(bz)
	{
		if(bz==1)	CopyVN=BTIC5C_RGB555_BlockPartCopyV1;
		if(bz==2)	CopyVN=BTIC5C_RGB555_BlockPartCopyV2;
		if(bz==3)	CopyVN=BTIC5C_RGB555_BlockPartCopyV3;
	}
	
	bxs0=ibxs>>2;
	bys0=ibys>>2;
	ct=ibuf;
	for(by=0; by<bys0; by++)
	{
		bz=by*bxs; ct1=ct;
		for(bx=0; bx<bxs0; bx++)
		{
			BTIC5C_UnpackCell_RGB555(blka[bz], ct1, ystr);
			ct1+=4; bz++;
		}
		if(ibxs&3)
		{
			BTIC5C_UnpackCell_RGB555(blka[bz], tpix, 4);
			CopyHN(ct1, tpix, ystr);
		}
		ct+=ystr<<2;
	}
	if(ibys&3)
	{
		bz=by*bxs; ct1=ct;
		for(bx=0; bx<bxs0; bx++)
		{
			BTIC5C_UnpackCell_RGB555(blka[bz], tpix, 4);
			CopyVN(ct1, tpix, ystr);
			ct1+=4; bz++;
		}
		if(ibxs&3)
		{
			BTIC5C_UnpackCell_RGB555(blka[bz], tpix, 4);
			memcpy(ct1, tpix, (ibxs&3)*2);
			if((ibys&3)>1)
				memcpy(ct1+ystr, tpix+4, (ibxs&3)*2);
			if((ibys&3)>2)
				memcpy(ct1+2*ystr, tpix+8, (ibxs&3)*2);
		}
	}
	return(0);
}

int BTIC5C_UnpackCellImageP_RGB555(
	u64 *blka, u64 *lblka, int bxs, int bys,
	u16 *ibuf, int ibxs, int ibys, int ystr)
{
	void (*CopyHN)(u16 *dst, u16 *src, int ystr);
	void (*CopyVN)(u16 *dst, u16 *src, int ystr);
	u16 tpix[16];
	u64 blk0, blk1;
	u16 *ct, *ct1;
	int bx, by, bz, bxs0, bys0;
	
	bz=ibxs&3;
	if(bz)
	{
		if(bz==1)	CopyHN=BTIC5C_RGB555_BlockPartCopyH1;
		if(bz==2)	CopyHN=BTIC5C_RGB555_BlockPartCopyH2;
		if(bz==3)	CopyHN=BTIC5C_RGB555_BlockPartCopyH3;
	}
	bz=ibys&3;
	if(bz)
	{
		if(bz==1)	CopyVN=BTIC5C_RGB555_BlockPartCopyV1;
		if(bz==2)	CopyVN=BTIC5C_RGB555_BlockPartCopyV2;
		if(bz==3)	CopyVN=BTIC5C_RGB555_BlockPartCopyV3;
	}
	
	bxs0=ibxs>>2;
	bys0=ibys>>2;
	ct=ibuf;
	for(by=0; by<bys0; by++)
	{
		bz=by*bxs; ct1=ct;
		for(bx=0; bx<bxs0; bx++)
		{
			blk0=blka[bz];
			blk1=lblka[bz];
			if(blk0!=blk1)
				BTIC5C_UnpackCell_RGB555(blk0, ct1, ystr);
			ct1+=4; bz++;
		}
		if(ibxs&3)
		{
			blk0=blka[bz];
			blk1=lblka[bz];
			if(blk0!=blk1)
			{
				BTIC5C_UnpackCell_RGB555(blk0, tpix, 4);
				CopyHN(ct1, tpix, ystr);
			}
		}
		ct+=ystr<<2;
	}
	if(ibys&3)
	{
		bz=by*bxs; ct1=ct;
		for(bx=0; bx<bxs0; bx++)
		{
			blk0=blka[bz];
			blk1=lblka[bz];
			if(blk0!=blk1)
			{
				BTIC5C_UnpackCell_RGB555(blk0, tpix, 4);
				CopyVN(ct, tpix, ystr);
			}
			ct1+=4; bz++;
		}
		if(ibxs&3)
		{
			blk0=blka[bz];
			blk1=lblka[bz];
			if(blk0!=blk1)
				BTIC5C_UnpackCell_RGB555(blk0, tpix, 4);
//			CopyHVN(ct, tpix, ystr);
		}
	}
	return(0);
}

int BTIC5C_UnpackCell_RGBA32(
	u64 cblk, u32 *ibuf, int ystr)
{
	u16 tpix[16];
	u32 *ct;
	
	u64 c2blk;
	int x, y, dy, px1, px2;
//	BTIC5C_UnpackCell_RGB555(cblk, ibuf, ystr);
	BTIC5C_UnpackCell_RGB555(cblk, tpix, 4);
	
	ct=ibuf;
	for(y=0; y<4; y++)
	{
		for(x=0; x<4; x++)
		{
			px1=tpix[(y<<2)|x];
			px2=	0xFF000000|
					((px1<<9)&0x00F80000)|
					((px1<<4)&0x00070000)|
					((px1<<6)&0x0000F800)|
					((px1<<1)&0x00000700)|
					((px1<<3)&0x000000F8)|
					((px1>>2)&0x00000007);
			ct[x]=px2;
		}
		ct+=ystr;
	}
	return(0);
}

void BTIC5C_RGB24_BlockRowCopyH1(byte *dst, u32 *src, int flip)
{
	byte *srcb;
	if(flip&1)
	{
		srcb=(byte *)src;
		dst[0]=srcb[2]; dst[1]=srcb[1];
		dst[2]=srcb[0];
		return;
	}
	memcpy(dst+0, src+0, 3);
}
void BTIC5C_RGB24_BlockRowCopyH2(byte *dst, u32 *src, int flip)
{
	byte *srcb;
	if(flip&1)
	{
		srcb=(byte *)src;
		dst[0]=srcb[2]; dst[1]=srcb[1];
		dst[2]=srcb[0]; dst+=3; srcb+=4;
		dst[0]=srcb[2]; dst[1]=srcb[1];
		dst[2]=srcb[0];
		return;
	}
	memcpy(dst+0, src+0, 3);
	memcpy(dst+3, src+1, 3);
}
void BTIC5C_RGB24_BlockRowCopyH3(byte *dst, u32 *src, int flip)
{
	byte *srcb;
	if(flip&1)
	{
		srcb=(byte *)src;
		dst[0]=srcb[2]; dst[1]=srcb[1];
		dst[2]=srcb[0]; dst+=3; srcb+=4;
		dst[0]=srcb[2]; dst[1]=srcb[1];
		dst[2]=srcb[0]; dst+=3; srcb+=4;
		dst[0]=srcb[2]; dst[1]=srcb[1];
		dst[2]=srcb[0];
		return;
	}
	memcpy(dst+0, src+0, 3);	memcpy(dst+3, src+1, 3);
	memcpy(dst+6, src+2, 3);
}
void BTIC5C_RGB24_BlockRowCopyH4(byte *dst, u32 *src, int flip)
{
	byte *srcb;
	if(flip&1)
	{
		srcb=(byte *)src;
		dst[0]=srcb[2]; dst[1]=srcb[1];
		dst[2]=srcb[0]; dst+=3; srcb+=4;
		dst[0]=srcb[2]; dst[1]=srcb[1];
		dst[2]=srcb[0]; dst+=3; srcb+=4;
		dst[0]=srcb[2]; dst[1]=srcb[1];
		dst[2]=srcb[0]; dst+=3; srcb+=4;
		dst[0]=srcb[2]; dst[1]=srcb[1];
		dst[2]=srcb[0];
		return;
	}
	memcpy(dst+0, src+0, 3);	memcpy(dst+3, src+1, 3);
	memcpy(dst+6, src+2, 3);	memcpy(dst+9, src+3, 3);
}

void BTIC5C_RGB32_BlockRowCopyH1(byte *dst, u32 *src, int flip)
{
	byte *srcb;
	if(flip&1)
	{
		srcb=(byte *)src;
		dst[0]=srcb[2]; dst[1]=srcb[1];
		dst[2]=srcb[0]; dst[3]=srcb[3];
		return;
	}
	memcpy(dst+0, src+0,  4);
}
void BTIC5C_RGB32_BlockRowCopyH2(byte *dst, u32 *src, int flip)
{
	byte *srcb;
	if(flip&1)
	{
		srcb=(byte *)src;
		dst[0]=srcb[2]; dst[1]=srcb[1];
		dst[2]=srcb[0]; dst[3]=srcb[3];
		dst+=4; srcb+=4;
		dst[0]=srcb[2]; dst[1]=srcb[1];
		dst[2]=srcb[0]; dst[3]=srcb[3];
		return;
	}
	memcpy(dst+0, src+0,  8);
}
void BTIC5C_RGB32_BlockRowCopyH3(byte *dst, u32 *src, int flip)
{
	byte *srcb;
	if(flip&1)
	{
		srcb=(byte *)src;
		dst[0]=srcb[2]; dst[1]=srcb[1];
		dst[2]=srcb[0]; dst[3]=srcb[3];
		dst+=4; srcb+=4;
		dst[0]=srcb[2]; dst[1]=srcb[1];
		dst[2]=srcb[0]; dst[3]=srcb[3];
		dst+=4; srcb+=4;
		dst[0]=srcb[2]; dst[1]=srcb[1];
		dst[2]=srcb[0]; dst[3]=srcb[3];
		return;
	}
	memcpy(dst+0, src+0, 12);
}
void BTIC5C_RGB32_BlockRowCopyH4(byte *dst, u32 *src, int flip)
{
	byte *srcb;
	if(flip&1)
	{
		srcb=(byte *)src;
		dst[0]=srcb[2]; dst[1]=srcb[1];
		dst[2]=srcb[0]; dst[3]=srcb[3];
		dst+=4; srcb+=4;
		dst[0]=srcb[2]; dst[1]=srcb[1];
		dst[2]=srcb[0]; dst[3]=srcb[3];
		dst+=4; srcb+=4;
		dst[0]=srcb[2]; dst[1]=srcb[1];
		dst[2]=srcb[0]; dst[3]=srcb[3];
		dst+=4; srcb+=4;
		dst[0]=srcb[2]; dst[1]=srcb[1];
		dst[2]=srcb[0]; dst[3]=srcb[3];
		return;
	}
	memcpy(dst+0, src+0, 16);
}

void BTIC5C_RGB24_BlockPartCopyH1(byte *dst, u32 *src, int ystr, int flip)
{	BTIC5C_RGB24_BlockRowCopyH1(dst, src, flip);	dst+=ystr; src+=4;
	BTIC5C_RGB24_BlockRowCopyH1(dst, src, flip);	dst+=ystr; src+=4;
	BTIC5C_RGB24_BlockRowCopyH1(dst, src, flip);	dst+=ystr; src+=4;
	BTIC5C_RGB24_BlockRowCopyH1(dst, src, flip);	}
void BTIC5C_RGB24_BlockPartCopyH2(byte *dst, u32 *src, int ystr, int flip)
{	BTIC5C_RGB24_BlockRowCopyH2(dst, src, flip);	dst+=ystr; src+=4;
	BTIC5C_RGB24_BlockRowCopyH2(dst, src, flip);	dst+=ystr; src+=4;
	BTIC5C_RGB24_BlockRowCopyH2(dst, src, flip);	dst+=ystr; src+=4;
	BTIC5C_RGB24_BlockRowCopyH2(dst, src, flip);	}
void BTIC5C_RGB24_BlockPartCopyH3(byte *dst, u32 *src, int ystr, int flip)
{	BTIC5C_RGB24_BlockRowCopyH3(dst, src, flip);	dst+=ystr; src+=4;
	BTIC5C_RGB24_BlockRowCopyH3(dst, src, flip);	dst+=ystr; src+=4;
	BTIC5C_RGB24_BlockRowCopyH3(dst, src, flip);	dst+=ystr; src+=4;
	BTIC5C_RGB24_BlockRowCopyH3(dst, src, flip);	}
void BTIC5C_RGB24_BlockCopyH4(byte *dst, u32 *src, int ystr, int flip)
{	BTIC5C_RGB24_BlockRowCopyH4(dst, src, flip);	dst+=ystr; src+=4;
	BTIC5C_RGB24_BlockRowCopyH4(dst, src, flip);	dst+=ystr; src+=4;
	BTIC5C_RGB24_BlockRowCopyH4(dst, src, flip);	dst+=ystr; src+=4;
	BTIC5C_RGB24_BlockRowCopyH4(dst, src, flip);	}

void BTIC5C_RGB24_BlockPartCopyV1(byte *dst, u32 *src, int ystr, int flip)
{	BTIC5C_RGB24_BlockRowCopyH4(dst, src, flip);	}
void BTIC5C_RGB24_BlockPartCopyV2(byte *dst, u32 *src, int ystr, int flip)
{	BTIC5C_RGB24_BlockRowCopyH4(dst, src, flip);	dst+=ystr; src+=4;
	BTIC5C_RGB24_BlockRowCopyH4(dst, src, flip);	}
void BTIC5C_RGB24_BlockPartCopyV3(byte *dst, u32 *src, int ystr, int flip)
{	BTIC5C_RGB24_BlockRowCopyH4(dst, src, flip);	dst+=ystr; src+=4;
	BTIC5C_RGB24_BlockRowCopyH4(dst, src, flip);	dst+=ystr; src+=4;
	BTIC5C_RGB24_BlockRowCopyH4(dst, src, flip);	}


void BTIC5C_RGB32_BlockPartCopyH1(byte *dst, u32 *src, int ystr, int flip)
{	BTIC5C_RGB32_BlockRowCopyH1(dst, src, flip);	dst+=ystr; src+=4;
	BTIC5C_RGB32_BlockRowCopyH1(dst, src, flip);	dst+=ystr; src+=4;
	BTIC5C_RGB32_BlockRowCopyH1(dst, src, flip);	dst+=ystr; src+=4;
	BTIC5C_RGB32_BlockRowCopyH1(dst, src, flip);	}
void BTIC5C_RGB32_BlockPartCopyH2(byte *dst, u32 *src, int ystr, int flip)
{	BTIC5C_RGB32_BlockRowCopyH2(dst, src, flip);	dst+=ystr; src+=4;
	BTIC5C_RGB32_BlockRowCopyH2(dst, src, flip);	dst+=ystr; src+=4;
	BTIC5C_RGB32_BlockRowCopyH2(dst, src, flip);	dst+=ystr; src+=4;
	BTIC5C_RGB32_BlockRowCopyH2(dst, src, flip);	}
void BTIC5C_RGB32_BlockPartCopyH3(byte *dst, u32 *src, int ystr, int flip)
{	BTIC5C_RGB32_BlockRowCopyH3(dst, src, flip);	dst+=ystr; src+=4;
	BTIC5C_RGB32_BlockRowCopyH3(dst, src, flip);	dst+=ystr; src+=4;
	BTIC5C_RGB32_BlockRowCopyH3(dst, src, flip);	dst+=ystr; src+=4;
	BTIC5C_RGB32_BlockRowCopyH3(dst, src, flip);	}
void BTIC5C_RGB32_BlockCopyH4(byte *dst, u32 *src, int ystr, int flip)
{	BTIC5C_RGB32_BlockRowCopyH4(dst, src, flip);	dst+=ystr; src+=4;
	BTIC5C_RGB32_BlockRowCopyH4(dst, src, flip);	dst+=ystr; src+=4;
	BTIC5C_RGB32_BlockRowCopyH4(dst, src, flip);	dst+=ystr; src+=4;
	BTIC5C_RGB32_BlockRowCopyH4(dst, src, flip);	}

void BTIC5C_RGB32_BlockPartCopyV1(byte *dst, u32 *src, int ystr, int flip)
{	BTIC5C_RGB32_BlockRowCopyH4(dst, src, flip);	}
void BTIC5C_RGB32_BlockPartCopyV2(byte *dst, u32 *src, int ystr, int flip)
{	BTIC5C_RGB32_BlockRowCopyH4(dst, src, flip);	dst+=ystr; src+=4;
	BTIC5C_RGB32_BlockRowCopyH4(dst, src, flip);	}
void BTIC5C_RGB32_BlockPartCopyV3(byte *dst, u32 *src, int ystr, int flip)
{	BTIC5C_RGB32_BlockRowCopyH4(dst, src, flip);	dst+=ystr; src+=4;
	BTIC5C_RGB32_BlockRowCopyH4(dst, src, flip);	dst+=ystr; src+=4;
	BTIC5C_RGB32_BlockRowCopyH4(dst, src, flip);	}

int BTIC5C_UnpackCellImageP_RGB24(
	u64 *blka, u64 *lblka, int bxs, int bys,
	byte *ibuf, int ibxs, int ibys, int ystr, int clrs)
{
	static byte *libuf;
	void (*CopyFull)(byte *dst, u32 *src, int ystr, int flip);
	void (*CopyHN)(byte *dst, u32 *src, int ystr, int flip);
	void (*CopyVN)(byte *dst, u32 *src, int ystr, int flip);
	u32 tpix[32];
	u64 blk0, blk1;
	byte *ct, *ct1;
	int xstr, bxstr, ystrb, flip, fulldec;
	int bx, by, bz, bxs0, bys0;
	
	flip=0;
	CopyFull=BTIC5C_RGB32_BlockCopyH4;

	if(ibuf!=libuf)
	{
		libuf=ibuf;
		fulldec=1;
	}

	bz=ibxs&3;
	if(bz)
	{
		if(bz==1)	CopyHN=BTIC5C_RGB32_BlockPartCopyH1;
		if(bz==2)	CopyHN=BTIC5C_RGB32_BlockPartCopyH2;
		if(bz==3)	CopyHN=BTIC5C_RGB32_BlockPartCopyH3;
	}
	bz=ibys&3;
	if(bz)
	{
		if(bz==1)	CopyVN=BTIC5C_RGB32_BlockPartCopyV1;
		if(bz==2)	CopyVN=BTIC5C_RGB32_BlockPartCopyV2;
		if(bz==3)	CopyVN=BTIC5C_RGB32_BlockPartCopyV3;
	}
	
	xstr=4;
	bxstr=4*4;

	if(clrs==BTIC4B_CLRS_RGBA)		flip=1;
	if(clrs==BTIC4B_CLRS_RGBX)		flip=1;
	
	if(	(clrs==BTIC4B_CLRS_RGB) ||
		(clrs==BTIC4B_CLRS_BGR)	)
	{
		xstr=3;
		bxstr=3*4;

		if(clrs==BTIC4B_CLRS_RGB)
			flip=1;

		CopyFull=BTIC5C_RGB24_BlockCopyH4;

		bz=ibxs&3;
		if(bz)
		{
			if(bz==1)	CopyHN=BTIC5C_RGB24_BlockPartCopyH1;
			if(bz==2)	CopyHN=BTIC5C_RGB24_BlockPartCopyH2;
			if(bz==3)	CopyHN=BTIC5C_RGB24_BlockPartCopyH3;
		}
		bz=ibys&3;
		if(bz)
		{
			if(bz==1)	CopyVN=BTIC5C_RGB24_BlockPartCopyV1;
			if(bz==2)	CopyVN=BTIC5C_RGB24_BlockPartCopyV2;
			if(bz==3)	CopyVN=BTIC5C_RGB24_BlockPartCopyV3;
		}
	}
	
	ystrb=ystr*xstr;
	
	bxs0=ibxs>>2;
	bys0=ibys>>2;
	ct=ibuf;
	for(by=0; by<bys0; by++)
	{
		bz=by*bxs; ct1=ct;
		for(bx=0; bx<bxs0; bx++)
		{
			blk0=blka[bz];
			if(lblka)
				blk1=lblka[bz];
			else
				blk1=~blk0;
			if((blk0!=blk1) || fulldec)
			{
				BTIC5C_UnpackCell_RGBA32(blk0, tpix, 4);
				CopyFull(ct1, tpix, ystrb, flip);
			}
			ct1+=bxstr; bz++;
		}
		if(ibxs&3)
		{
			blk0=blka[bz];
			if(lblka)
				blk1=lblka[bz];
			else
				blk1=~blk0;
			if((blk0!=blk1) || fulldec)
			{
				BTIC5C_UnpackCell_RGBA32(blk0, tpix, 4);
				CopyHN(ct1, tpix, ystrb, flip);
			}
		}
		ct+=ystr*bxstr;
	}
	if(ibys&3)
	{
		bz=by*bxs; ct1=ct;
		for(bx=0; bx<bxs0; bx++)
		{
			blk0=blka[bz];
			if(lblka)
				blk1=lblka[bz];
			else
				blk1=~blk0;
			if((blk0!=blk1) || fulldec)
			{
				BTIC5C_UnpackCell_RGBA32(blk0, tpix, 4);
				CopyVN(ct, tpix, ystrb, flip);
			}
			ct1+=bxstr; bz++;
		}
		if(ibxs&3)
		{
			blk0=blka[bz];
			if(lblka)
				blk1=lblka[bz];
			else
				blk1=~blk0;
			if((blk0!=blk1) || fulldec)
			{
				BTIC5C_UnpackCell_RGBA32(blk0, tpix, 4);
//				CopyHVN(ct, tpix, ystr);
			}
		}
	}
	return(0);
}

int BTIC5C_UnpackCellImageP_RGBA32(
	u64 *blka, u64 *lblka, int bxs, int bys,
	u32 *ibuf, int ibxs, int ibys, int ystr, int clrs)
{
	BTIC5C_UnpackCellImageP_RGB24(blka, lblka, bxs, bys,
		(byte *)ibuf, ibxs, ibys, ystr, clrs);
	return(0);
}



int BTIC5C_DecodeFrame(BTIC5C_DecodeContext *ctx,
	byte *cdat, int csz, u16 *img, int ystr, int clrs)
{
	byte *cs, *cse, *cs0, *cs1, *idat, *pdat;
	u64 *blka;
	int tag, nln, len, xs, ys, dsz, sz;

	cs=cdat;

#if 1
	tag=btpic_getu32(cs+0);
	nln=btpic_getu32(cs+4);
	if(!(tag&0x80808080U))
	{
		if(!(nln&0x80000000U))
			return(-1);
		len=nln^0xFFFFFFFFU;
		if(len<8)
			return(-1);
		cs0=cs+8;
		cs1=cs+len;
	}else
	{
		tag=btpic_getu16(cs+0);
		nln=btpic_getu16(cs+2);
		if(tag&0x8080)
			return(-1);
		if(!(nln&0x8000))
			return(-1);
		len=nln^0xFFFF;
		if(len<4)
			return(-1);
		cs0=cs+4;
		cs1=cs+len;
	}

	if(tag==BTPIC_TCC_Z3)
	{
		dsz=btpic_getu16(cs0);
	
		if(!ctx->zfbuf)
		{
			sz=4096;
			while(sz<=(dsz+64))
				sz=sz+(sz>>1);
			ctx->zfbsz=sz;
			ctx->zfbuf=malloc(sz);
		}

		if((dsz+64)>ctx->zfbsz)
		{
			sz=ctx->zfbsz;
			while(sz<=(dsz+64))
				sz=sz+(sz>>1);
			ctx->zfbsz=sz;
			ctx->zfbuf=realloc(ctx->zfbuf, sz);
		}
	
		BTIC5C_DecodeBufferRP2(cs0+4, ctx->zfbuf, len, ctx->zfbsz);
		
		cdat=ctx->zfbuf;
		csz=dsz;
	}
#endif
	
	idat=NULL;
	pdat=NULL;
	cs=cdat; cse=cs+csz;
	while(cs<cse)
	{
		tag=btpic_getu32(cs+0);
		nln=btpic_getu32(cs+4);
		if(!(tag&0x80808080U))
		{
			if(!(nln&0x80000000U))
				break;
			len=nln^0xFFFFFFFFU;
			if(len<8)
				break;
			cs0=cs+8;
			cs1=cs+len;
		}else
		{
			tag=btpic_getu16(cs+0);
			nln=btpic_getu16(cs+2);
			if(tag&0x8080)
				break;
			if(!(nln&0x8000))
				break;
			len=nln^0xFFFF;
			if(len<4)
				break;
			cs0=cs+4;
			cs1=cs+len;
		}
		
		if(tag==BTPIC_TCC_HX)
		{
			xs=btpic_getu16(cs0+0);
			ys=btpic_getu16(cs0+2);
			
			ctx->xs=xs;
			ctx->ys=ys;

			ctx->cxs=(xs+15)>>4;
			ctx->cys=(ys+15)>>4;
			ctx->bxs=ctx->cxs*4;
			ctx->bys=ctx->cys*4;
			
			cs=cs1;
			continue;
		}

		if(tag==BTPIC_TCC_IX)
		{
			idat=cs0;
			cs=cs1;
			continue;
		}

		if(tag==BTPIC_TCC_PX)
		{
			pdat=cs0;
			cs=cs1;
			continue;
		}

		cs=cs1;
		continue;
	}
	
	if(idat)
	{
		if(!ctx->blka)
			{ ctx->blka=malloc(ctx->bxs*ctx->bys*8); }
		BTIC5C_DecodeBlockPlane(ctx, idat);

		if(img)
		{
			if(	(clrs==BTIC4B_CLRS_RGBA) ||
				(clrs==BTIC4B_CLRS_BGRA) ||
				(clrs==BTIC4B_CLRS_RGBX) ||
				(clrs==BTIC4B_CLRS_BGRX) ||
				(clrs==BTIC4B_CLRS_RGB) ||
				(clrs==BTIC4B_CLRS_BGR))
			{
				BTIC5C_UnpackCellImageP_RGB24(
					ctx->blka, NULL, ctx->bxs, ctx->bys,
					(byte *)img, ctx->xs, ctx->ys, ystr, clrs);
			}else
			{
				BTIC5C_UnpackCellImage_RGB555(
					ctx->blka, ctx->bxs, ctx->bys,
					img, ctx->xs, ctx->ys, ystr);
			}
		}
		return(0);
	}
	
	if(pdat)
	{
		blka=ctx->lblka;
		ctx->lblka=ctx->blka;
		ctx->blka=blka;
		if(!ctx->blka)
			{ ctx->blka=malloc(ctx->bxs*ctx->bys*8); }
		if(!ctx->lblka)
			{ ctx->lblka=malloc(ctx->bxs*ctx->bys*8); }
		BTIC5C_DecodeBlockPlane(ctx, pdat);

		if(img)
		{
			if(	(clrs==BTIC4B_CLRS_RGBA) ||
				(clrs==BTIC4B_CLRS_BGRA) ||
				(clrs==BTIC4B_CLRS_RGBX) ||
				(clrs==BTIC4B_CLRS_BGRX) ||
				(clrs==BTIC4B_CLRS_RGB) ||
				(clrs==BTIC4B_CLRS_BGR))
			{
				BTIC5C_UnpackCellImageP_RGB24(
					ctx->blka, ctx->lblka, ctx->bxs, ctx->bys,
					(byte *)img, ctx->xs, ctx->ys, ystr, clrs);
			}else
			{
				BTIC5C_UnpackCellImageP_RGB555(
					ctx->blka, ctx->lblka, ctx->bxs, ctx->bys,
					img, ctx->xs, ctx->ys, ystr);
			}
		}
		return(0);
	}

	return(0);
}
