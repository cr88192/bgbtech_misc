#define GANN_MODE_MAJ7		0		//majority of 7
#define GANN_MODE_MAJ3		1		//majority of 3
#define GANN_MODE_HAMM		2		//hamming
#define GANN_MODE_RAWBYTE	3		//raw bytes

typedef unsigned char			byte;
typedef unsigned short		u16;
typedef unsigned int			u32;
typedef unsigned long long	u64;

typedef signed char			sbyte;
typedef signed short			s16;
typedef signed int			s32;
typedef signed long long		s64;

typedef struct GANN_Member_s	GANN_Member;
typedef struct GANN_Context_s	GANN_Context;

struct GANN_Member_s {
u64 *vec;
u64 mrm;		//mutation rate control
int vsz;
u16 brseq;
short curix;
byte mrmb;
byte mrmc;
byte flag;

u64 wv0;
u64 wv1;
int pen;
int uflag;

};

struct GANN_Context_s {
GANN_Member **marr;
int *earr;
int nmemb;
int szmemb;
int curgen;
int flag;

u16 brseq;

s64 trend1;
s64 trend2;
s64 trend3;
s64 trend4;

//u64 wv0;
//u64 wv1;
//int pen;

int mstat_sk[8];

int skrank[16];
byte skatab[64];

int (*TestMember)(GANN_Context *ctx, GANN_Member *mm);
};


byte GANN_Maj64to8(u64 maj);
u16 GANN_Rand16();
u64 GANN_Rand64();

u16 GANN_Rand16B();
u64 GANN_Rand64B();
