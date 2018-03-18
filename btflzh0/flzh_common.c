/*
Huff Tab (Lookup):
   7: 0, Symbol A
  15: 8, Symbol B
  19:16, Length A (0=Escape)
  23:20, Length A+B (Escape)

Huff Tab (Search):
  15: 0, Bitmask
  19:16, Length
  31:24, Next Symbol.
*/

typedef unsigned char byte;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef signed char sbyte;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;

#ifdef _MSC_VER
#define force_inline	__forceinline
#else
#define force_inline	inline
#endif

#if defined(__i386__) || defined(__x86_64__)
// #if 1
/* Looks like GCC or Clang or similar, use memcpy. */
#define BTFLZH_USES_MEMCPY
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define BTFLZH_NATIVE_QWORD
#endif

#ifdef _M_X64
#define USE_MOV_QWORD
#endif

#if defined(_M_X64)
typedef unsigned int fastint;
#else
typedef int fastint;
#endif

// #define BTFLZH_HTAB_LUNB	8
// #define BTFLZH_HTAB_LUSZ	256
// #define BTFLZH_HTAB_LUMASK	255

#define BTFLZH_HTAB_LUNB	10
// #define BTFLZH_HTAB_LUNB	11
#define BTFLZH_HTAB_LUSZ	(1<<BTFLZH_HTAB_LUNB)
#define BTFLZH_HTAB_LUMASK	(BTFLZH_HTAB_LUSZ-1)

#define BTFLZH_GENMASK(n)		(btflzh_pow2masktab[n])
#define BTFLZH_BITMASK(v, n)	((v)&btflzh_pow2masktab[n])

// #define BTFLZH_GENMASK(n)		((1<<(n))-1)
// #define BTFLZH_BITMASK(v, n)	((v)&((1<<(n))-1))

#ifdef BTFLZH_USES_MEMCPY

/*
Memcpy path, because GCC and Clang are smart about memcpy, but may
stupidly screw up with misaligned load/store in some recent incarnations.
*/

static inline u16 btlzh_getu16i(byte *ptr)
	{ u16 v; memcpy(&v, ptr, 2); return(v); }
static inline void btlzh_setu16i(byte *ptr, u16 val)
	{ memcpy(ptr, &val, 2); }

static inline u32 btlzh_getu32i(byte *ptr)
	{ u32 v; memcpy(&v, ptr, 4); return(v); }
static inline void btlzh_setu32i(byte *ptr, u32 val)
	{ memcpy(ptr, &val, 4); }

static inline u64 btlzh_getu64i(byte *ptr)
	{ u64 v; memcpy(&v, ptr, 8); return(v); }
static inline void btlzh_setu64i(byte *ptr, u64 val)
	{ memcpy(ptr, &val, 8); }

#define BTFLZH_GETU32(ptr)				(btlzh_getu32i(ptr))
#define BTFLZH_SETU32(ptr, val)			(btlzh_setu32i(ptr, val))
#define BTFLZH_GETU32IX(ptr, ix)		(btlzh_getu32i((ptr)+((ix)*4)))
#define BTFLZH_SETU32IX(ptr, ix, val)	(btlzh_setu32i((ptr)+((ix)*4), val))

#define BTFLZH_GETU64(ptr)				(btlzh_getu64i(ptr))
#define BTFLZH_SETU64(ptr, val)			(btlzh_setu64i(ptr, val))
#define BTFLZH_GETU64IX(ptr, ix)		(btlzh_getu64i((ptr)+((ix)*8)))
#define BTFLZH_SETU64IX(ptr, ix, val)	(btlzh_setu64i((ptr)+((ix)*8), val))

#define BTFLZH_GETU16(ptr)				(btlzh_getu16i(ptr))
#define BTFLZH_SETU16(ptr, val)			(btlzh_setu16i(ptr, val))

#define BTFLZH_DEBUGBREAK		

#else

#define BTFLZH_GETU32(ptr)			(*(u32 *)(ptr))
#define BTFLZH_SETU32(ptr, val)		(*(u32 *)(ptr)=(val))
#define BTFLZH_GETU32IX(ptr, ix)		(((u32 *)(ptr))[ix])
#define BTFLZH_SETU32IX(ptr, ix, val)	(((u32 *)(ptr))[ix]=(val))

#define BTFLZH_GETU64(ptr)			(*(u64 *)(ptr))
#define BTFLZH_SETU64(ptr, val)		(*(u64 *)(ptr)=(val))
#define BTFLZH_GETU64IX(ptr, ix)		(((u64 *)(ptr))[ix])
#define BTFLZH_SETU64IX(ptr, ix, val)	(((u64 *)(ptr))[ix]=(val))

#define BTFLZH_GETU16(ptr)			(*(u16 *)(ptr))
#define BTFLZH_SETU16(ptr, val)		(*(u16 *)(ptr)=(val))

#define BTFLZH_DEBUGBREAK		__debugbreak();

#endif

#define BTFLZH_STF_RK		2
// #define BTFLZH_STF_RK		3
// #define BTFLZH_STFSWAP(j)	(((j)*13)/16)
// #define BTFLZH_STFSWAP(j)	(((j)*3)/4)
#define BTFLZH_STFSWAP(j)	(((j)*11)/16)
// #define BTFLZH_STFSWAP(j)	(((j)*(13-((j)/8)))/16)
// #define BTFLZH_STFSWAP(j)	(((j)*1)/2)

static const u32 btflzh_pow2masktab[32]={
	0x00000000, 0x00000001, 0x00000003, 0x00000007,
	0x0000000F, 0x0000001F, 0x0000003F, 0x0000007F,
	0x000000FF, 0x000001FF, 0x000003FF, 0x000007FF,
	0x00000FFF, 0x00001FFF, 0x00003FFF, 0x00007FFF,
	0x0000FFFF, 0x0001FFFF, 0x0003FFFF, 0x0007FFFF,
	0x000FFFFF, 0x001FFFFF, 0x003FFFFF, 0x007FFFFF,
	0x00FFFFFF, 0x01FFFFFF, 0x03FFFFFF, 0x07FFFFFF,
	0x0FFFFFFF, 0x1FFFFFFF, 0x3FFFFFFF, 0x7FFFFFFF};

static const byte btflzh_rqtab[256]={
//0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,	//0z
  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,	//1z
  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,	//2z
  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6,	//3z
  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,	//4z
  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,	//5z
  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,	//6z
  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 7,	//7z
  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,	//8z
  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,	//9z
  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,	//Az
  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6,	//Bz
  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,	//Cz
  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,	//Dz
  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,	//Ez
  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 8	//Fz
};

static const byte btflzh_trans8tab[256]={
0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0,
0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8,
0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4,
0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC,
0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2,
0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA,
0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6,
0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE,
0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1,
0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9,
0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5,
0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED,
0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3,
0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB,
0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7,
0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF,
0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
};

static const byte btflzh_distxtra[128]={
	 0, 0, 0, 0,  0, 0, 0, 0,
	 1, 1, 1, 1,  2, 2, 2, 2,
	 3, 3, 3, 3,  4, 4, 4, 4,
	 5, 5, 5, 5,  6, 6, 6, 6,	
	 7, 7, 7, 7,  8, 8, 8, 8,
	 9, 9, 9, 9, 10,10,10,10,
	11,11,11,11, 12,12,12,12,
	13,13,13,13, 14,14,14,14,
	
	15,15,15,15, 16,16,16,16,
	17,17,17,17, 18,18,18,18,
	19,19,19,19, 20,20,20,20,
	21,21,21,21, 22,22,22,22,

	23,23,23,23, 24,24,24,24,
	25,25,25,25, 26,26,26,26,
	27,27,27,27, 28,28,28,28,
//	29,29,29,29,  0, 0, 1, 2
	 0, 0, 0, 0,  1, 1, 2, 2
	};

static const u32 btflzh_distbase[128]={
       0,        1,        2,        3,        4,        5,        6,        7,
       8,       10,       12,       14,       16,       20,       24,       28,
      32,       40,       48,       56,       64,       80,       96,      112,
     128,      160,      192,      224,      256,      320,      384,      448,
     512,      640,      768,      896,     1024,     1280,     1536,     1792,
    2048,     2560,     3072,     3584,     4096,     5120,     6144,     7168,
    8192,    10240,    12288,    14336,    16384,    20480,    24576,    28672,
   32768,    40960,    49152,    57344,    65536,    81920,    98304,   114688,
  131072,   163840,   196608,   229376,   262144,   327680,   393216,   458752,
  524288,   655360,   786432,   917504,  1048576,  1310720,  1572864,  1835008,
 2097152,  2621440,  3145728,  3670016,  4194304,  5242880,  6291456,  7340032,
 8388608, 10485760, 12582912, 14680064, 16777216, 20971520, 25165824, 29360128,
   33554432,   41943040,   50331648,   58720256,
   67108864,   83886080,  100663296,  117440512,
  134217728,  167772160,  201326592,  234881024,
  268435456,  335544320,  402653184,  469762048,
  536870912,  671088640,  805306378,  939524096,
 1073741824, 1342177280, 1610612736, 1879048192,
 4294967295, 4294967294, 4294967293, 4294967292,
 4294967290, 4294967288, 4294967284, 4294967280
 };

#if 1
#define BTFLZH_RAWLIM	128
static const byte btflzh_rawxtra[9]={0,0,1,2,3,4,5,6,7};
static const byte btflzh_rawbase[9]={0,1,2,4,8,16,32,64,128};
#endif

#if 0
#define BTFLZH_RAWLIM	16
static const byte btflzh_rawxtra[9]={0,0,0,0,1,1,2,2,3};
static const byte btflzh_rawbase[9]={0,1,2,3,4,6,8,12,16};
#endif

#if 0
#define BTFLZH_RAWLIM	64
static const byte btflzh_rawxtra[9]={0,0,0,0,1,1,2,3,4};
static const byte btflzh_rawbase[9]={0,1,2,3,4,6,8,12,20};
#endif

// #define BTFLZH_LEN44

static const byte btflzh_lenxtra[32]={
	0,0,0,0, 0,0,0,0,
	1,1,1,1, 2,2,2,2,
	3,3,3,3, 4,4,4,4,
	5,5,5,5, 6,6,6,6};
static const u16 btflzh_lenbase[32]={
   0,   1,   2,   3,   4,   5,   6,   7,
   8,  10,  12,  14,  16,  20,  24,  28,
  32,  40,  48,  56,  64,  80,  96, 112,
 128, 160, 192, 224, 256, 320, 384, 448};

#if 0
static const byte btflzh_len2xtra[17]={
	0,0,0,0, 1,1,2,2,
	3,3,4,4, 5,5,6,6,
	7};
static const u16 btflzh_len2base[17]={
   0,   1,   2,   3,   4,   6,   8,  12,
  16,  24,  32,  48,  64,  96, 128, 192,
 256};
#endif

#if 1
static const byte btflzh_initstf[32]={
	 8,  7,  9,  6,  10,  5, 11,  4, 
	12,  3, 13,  2,  14,  1, 15,  0, 
	16, 17, 18, 19,  20, 21, 22, 23,
	24, 25, 26, 27,  28, 29, 30, 31};
#endif

#if 0
static const byte btflzh_initstf[32]={
	 9,  8, 16, 10,   7,  6, 11,  5, 
	12,  0, 13, 15,  14, 18, 17, 19,  
	 4,  3,  2,  1,  20, 21, 22, 23,
	24, 25, 26, 27,  28, 29, 30, 31};
#endif
