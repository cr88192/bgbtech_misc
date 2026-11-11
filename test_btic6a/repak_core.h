
typedef unsigned char byte;
typedef signed char sbyte;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned long long u64;
typedef signed long long s64;

#ifndef __cplusplus

#ifndef _BOOL_T
#define _BOOL_T
typedef unsigned char bool;
#endif

#ifndef true
#define true 1
#define false 0
#endif

#endif

#define TK_FILE FILE

#define tk_fopen	fopen
#define tk_fclose	fclose

#define tk_fread	fread
#define tk_fwrite	fwrite
#define tk_fseek	fseek
#define tk_ftell	ftell
#define tk_fgetc	fgetc
#define tk_fputc	fputc

#include "tk_dummyavi.h"
// #include "tk_outavi.h"

BGBBTJ_AVICtx *BGBBTJ_AVI_OpenOutStream2(char *name,
	int w, int h, float fps, int fcc, int auid);

void BGBBTJ_AVI_EncodeFrame2(BGBBTJ_AVICtx *ctx,
	byte *ibuf, int w, int h, int qf, int clrs);
void *pdgl_avi_malloc(int sz);
