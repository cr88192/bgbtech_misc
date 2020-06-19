#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TGVLZ_NOMAIN
#include "tgvlz1.c"

#if 0
typedef unsigned char byte;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
#endif

typedef unsigned short word;

typedef struct
{
	byte	identification[4];		// should be IWAD
	u32		numlumps;
	u32		infotableofs;
} wadinfo_t;


typedef struct
{
	u32		filepos;
	u32		size;
	byte	name[8];
} filelump_t;

typedef struct
{
	u32		filepos;
	u32		csize;
	u32		dsize;
	byte	ety;		//Entry Type
	byte	cmp;		//Compression
	u16		chn;		//Chain (ExWAD)
	byte	name[16];
} wad2lump_t;


FILE		*wad_fd;
// filelump_t	wad_dir[2048];
wad2lump_t	wad_dir[16384];
wadinfo_t	wad_head;
int 		wad_n_lumps;

byte		*wad_data;
int			wad_rover;

byte		*iwad_data;
filelump_t	*iwad_dir;
wadinfo_t	*iwad_head;
int			iwad_size;

int		wad_mincsz;
int		wad_maxcsz;
int 	wad_cmp[16];

void w_strupr_n (char *t, char *s, int n)
{
	int i;
	for(i=0; *s && (i<n); i++)
		{ *t++ = toupper(*s++); }
	for(; i<n; i++)
		*t++=0;
}

void w_strlwr_n (char *t, char *s, int n)
{
	int i;
	for(i=0; *s && (i<n); i++)
		{ *t++ = tolower(*s++); }
	for(; i<n; i++)
		*t++=0;
}

int AddWadLump2(char *name, void *buf, int csz, int dsz, int cmp)
{
	byte *obuf;
	char tn[17];
	int osz;
	int n;

	if(csz!=dsz)
	{
		if(csz<wad_mincsz)
			wad_mincsz=csz;
		if(csz>wad_maxcsz)
			wad_maxcsz=csz;
	}

//	w_strupr_n(tn, name, 16);
	w_strlwr_n(tn, name, 16);
	
	n=wad_n_lumps++;
	wad_dir[n].filepos=wad_rover;
	wad_dir[n].csize=csz;
	wad_dir[n].dsize=dsz;
	wad_dir[n].ety=0;
	wad_dir[n].cmp=cmp;
	wad_dir[n].chn=0;
	memcpy(wad_dir[n].name, tn, 16);
	
	memcpy(wad_data+wad_rover, buf, csz);
	wad_rover+=csz;
//	wad_rover=(wad_rover+15)&(~15);
	wad_rover=(wad_rover+7)&(~7);
	
	return(n);
}

int AddWadLump(char *name, byte *buf, int isz)
{
	TgvLz_Context *ctx1, *ctx2;
	byte *ibuf;
	byte *obuf;
	byte *obuf1;
	byte *obuf2;
	int osz, osz1, osz2, n, cmp;

	ctx1=TgvLz_CreateContext();
	ctx2=TgvLz_CreateContextLZ4();

	ibuf=malloc(isz+24);
	memset(ibuf, 0, isz+24);
	memcpy(ibuf, buf, isz);
	
	obuf1=malloc(isz*2+1024);
	obuf2=malloc(isz*2+1024);
//	osz=TgvLz_DoEncode(ctx, ibuf, obuf, isz);
	osz1=TgvLz_DoEncodeSafe(ctx1, ibuf, obuf1, isz);
	osz2=TgvLz_DoEncodeSafe(ctx2, ibuf, obuf2, isz);
	TgvLz_DestroyContext(ctx1);
	TgvLz_DestroyContext(ctx2);
	
	if((osz1<=osz2) && (osz1>0))
	{
		obuf=obuf1;
		osz=osz1;
		cmp=3;
	}else if(osz2>0)
	{
		obuf=obuf2;
		osz=osz2;		
		cmp=4;
	}else
	{
		obuf=NULL;
		osz=-1;
		cmp=-1;
	}
	
//	if((1.5*osz)<isz)
//	if((1.3*osz)<isz)
//	if((1.2*osz)<isz)
//	if(((1.2*osz)<isz) && (osz>0))
//	if(((1.2*osz)<isz) && (osz>32))
	if(((1.2*osz)<isz) && (osz>16))
	{
		wad_cmp[cmp]++;
		n=AddWadLump2(name, obuf, osz, isz, cmp);
	}else
	{
		wad_cmp[0]++;
		n=AddWadLump2(name, ibuf, isz, isz, 0);
	}
	
	free(ibuf);
	free(obuf1);
	free(obuf2);
	
	return(n);
}

void *LoadFile(char *name, int *rsz)
{
	FILE *fd;
	void *buf;
	int sz;
	
	fd=fopen(name, "rb");
	if(!fd)return(NULL);
	fseek(fd, 0, 2);
	sz=ftell(fd);
	fseek(fd, 0, 0);
	buf=malloc(sz+16);
	fread(buf, 1, sz, fd);
	fclose(fd);
	*rsz=sz;
	return(buf);
}

void StoreFile(char *name, void *buf, int sz)
{
	FILE *fd;
	
	fd=fopen(name, "wb");
	if(!fd)return;
	fwrite(buf, 1, sz, fd);
	fclose(fd);
}

int main(int argc, char *argv[])
{
	char tn[9];
	char *ifn, *ofn;
	int i, j, k;
	
//	ifn=argv[1];
//	ofn=argv[2];

	ifn=NULL;
	ofn=NULL;
	for(i=1; i<argc; i++)
	{
		if(argv[i][0]=='-')
		{
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

	if(!ifn)
	{
		printf("no input file\n");
	}

	if(!ofn)
	{
		printf("no output file\n");
	}

	wad_data=malloc(1<<26);
	wad_rover=16;
	wad_n_lumps=0;

	wad_mincsz=99999999;
	wad_maxcsz=0;

	memset(wad_data, 0, 1<<26);

	iwad_data=LoadFile(ifn, &iwad_size);
	iwad_head=(wadinfo_t *)iwad_data;
	iwad_dir=(filelump_t *)(iwad_data+(iwad_head->infotableofs));
	
	for(i=0; i<iwad_head->numlumps; i++)
	{
		printf("%d/%d\r", i, iwad_head->numlumps);
		w_strupr_n(tn, iwad_dir[i].name, 8);
		tn[8]=0;
		AddWadLump(tn,
			iwad_data+iwad_dir[i].filepos,
			iwad_dir[i].size);
	}
	printf("\n");

	wad_rover=(wad_rover+15)&(~15);
	
	memcpy(wad_head.identification, "WAD2", 4);
	wad_head.numlumps=wad_n_lumps;
	wad_head.infotableofs=wad_rover;

	memcpy(wad_data+wad_rover, wad_dir, wad_n_lumps*32);
	wad_rover+=wad_n_lumps*32;
	
	memcpy(wad_data, &wad_head, sizeof(wad_head));

	printf("%d -> %d bytes %d%%\n", iwad_size, wad_rover,
		(100*wad_rover)/iwad_size);
	
	printf("csz: min=%d max=%d\n", wad_mincsz, wad_maxcsz);
	
	for(i=0; i<16; i++)
	{
		if(wad_cmp[i])
			printf("%d: %d\n", i, wad_cmp[i]);
	}
	
	StoreFile(ofn, wad_data, wad_rover);

	free(wad_data);
}
