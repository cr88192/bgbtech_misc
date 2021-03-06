/*
Copyright (C) 2015 by Brendan G Bohannon

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

// #include <bgbbtj.h>


// #if defined(BGBBTJ_DRV) || defined(BGBBTJ_NODY)
#if 1

#ifndef BGBBTJ_DRVLOGFILE
#ifdef _MSC_VER
#define BGBBTJ_DRVLOGFILE "F:\\bt1hdrv_log.txt"
#define BGBBTJ_DRVCFGFILE "F:\\bt1hdrv_conf.txt"
#endif
#ifdef __linux__
#define BGBBTJ_DRVLOGFILE "~/.bt1hdrv_log.txt"
#define BGBBTJ_DRVCFGFILE "~/.bt1hdrv_conf.txt"
#endif
#endif

FILE *btjpg_log=NULL;
char *btjpg_home;

extern u32 btjpg_drv_defaultCodecFcc;

int BTJPG_DriverTryLoadConfig(char *name)
{
	char tb[256];
	FILE *fd;
	char **a;

	fd=fopen(name, "rt");
	if(!fd)return(0);
	
	while(!feof(fd))
	{
		fgets(tb, 256, fd);
		
		a=bgbv86_rsplit(tb);
		if(!a[0])continue;
		if(a[0][0]=='#')continue;
		if(a[0][0]==';')continue;
		if(a[0][0]=='/')continue;
		
		if(!strcmp(a[0], "defaultCodec"))
		{
			btjpg_drv_defaultCodecFcc=RIFF_MAKETAG(
				a[1][0], a[1][1], a[1][2], a[1][3]);
			continue;
		}
	}
	
	fclose(fd);
	
	return(1);
}

void BTJPG_DriverInit(void)
{
	static int init=0;

	if(init)return;
	init=1;
	
	btjpg_log=fopen(BGBBTJ_DRVLOGFILE, "at");
	if(btjpg_log)
		{ btjpg_puts("--------\n"); }

	BTJPG_DriverTryLoadConfig(BGBBTJ_DRVCFGFILE);
}

void BTJPG_DriverDeinit(void)
{
	if(btjpg_log)
	{
		fclose(btjpg_log);
		btjpg_log=NULL;
	}
}

void btjpg_puts(char *str)
{
	if(btjpg_log)
	{
		fputs(str, btjpg_log);
		fflush(btjpg_log);
	}
}

void btjpg_printf(char *str, ...)
{
	char buf[65536];
	va_list lst;

	if(!btjpg_log)
		return;
		
	va_start(lst, str);
//	vfprintf(btjpg_log, str, lst);
	vsprintf(buf, str, lst);
	va_end(lst);
	
	btjpg_puts(buf);
}

void *btjpg_malloc(size_t sz)
{
	void *ptr;
	
	ptr=malloc(sz);
	memset(ptr, 0, sz);
	return(ptr);
}

void *btjpg_gcalloc(size_t sz)
{
	void *ptr;
	
	ptr=malloc(sz);
	memset(ptr, 0, sz);
	return(ptr);
}

void *btjpg_gctalloc(char *str, size_t sz)
{
	void *ptr;
	
	ptr=malloc(sz);
	memset(ptr, 0, sz);
	return(ptr);
}

void btjpg_gcfree(void *ptr)
{
	free(ptr);
}

char *btjpg_strdup(char *str)
{
	char *s1;
	
	s1=strdup(str);
	return(s1);
}

#endif
