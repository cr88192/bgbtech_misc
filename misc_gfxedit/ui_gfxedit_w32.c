#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

typedef unsigned char			byte;
typedef signed char			sbyte;
typedef unsigned short		u16;
typedef signed short			s16;
typedef unsigned int			u32;
typedef signed int			s32;
typedef unsigned long long	u64;
typedef signed long long		s64;

#ifdef _WIN32
#include "gfxdrv_w32.c"
#include "gfxdrv_input.c"
#endif

char *gfxedit_getcwd();
void *gfxedit_opendir(char *path);
char *gfxedit_readdir(void *idir);
void gfxedit_closedir(void *idir);
char *gfxedit_chdir(char *cwd, char *newdir);
char *gfxedit_combinepath(char *cwd, char *newdir);


#include "gfxedit/gfxedit_multi.c"

char *gfxedit_emitutf(char *ct, int val)
{
	if(val<=0)
	{
		*ct++=0xC0;
		*ct++=0x80;
		return(ct);
	}

	if(val<0x80)
	{
		*ct++=val;
		return(ct);
	}

	if(val<0x0800)
	{
		*ct++=0xC0|((val>>6)&31);
		*ct++=0x80|((val>>0)&63);
		return(ct);
	}

	if(val<0x10000)
	{
		*ct++=0xE0|((val>>12)&15);
		*ct++=0x80|((val>> 6)&63);
		*ct++=0x80|((val>> 0)&63);
		return(ct);
	}

	*ct++=0xF0|((val>>18)& 7);
	*ct++=0x80|((val>>12)&63);
	*ct++=0x80|((val>> 6)&63);
	*ct++=0x80|((val>> 0)&63);
	return(ct);
}

int gfxedit_readutf(char **rcs)
{
	byte *cs;
	int c;
	
	cs=(byte *)(*rcs);
	c=*cs;
	
	if(c<0x80)
	{
		*rcs=(char *)(cs+1);
		return(c);
	}
	
	if(c<0xC0)
	{
		*rcs=(char *)(cs+1);
		return(c);
	}
	
	if(c<0xE0)
	{
		c=((cs[0]&0x1F)<<6)|((cs[1]&63)<<0);
		*rcs=(char *)(cs+2);
		return(c);
	}

	if(c<0xF0)
	{
		c=((cs[0]&0x0F)<<12)|((cs[1]&63)<<6)|((cs[2]&63)<<0);
		*rcs=(char *)(cs+3);
		return(c);
	}

	if(c<0xF8)
	{
		c=((cs[0]&0x07)<<18)|((cs[1]&63)<<12)|((cs[2]&63)<<6)|((cs[3]&63)<<0);
		*rcs=(char *)(cs+4);
		return(c);
	}
	
	return(0);
}

char *gfxedit_strdup_wcs2utf(wchar_t *wstr)
{
	char tbuf[512];
	char *ct;
	wchar_t *wcs;
	
	wcs=wstr; ct=tbuf;
	while(*wcs)
		{ ct=gfxedit_emitutf(ct, *wcs++); }
	*ct=0;
	return(gfxedit_strdup(tbuf));
}

char *gfxedit_getcwd()
{
	wchar_t tbuf[512];
	GetCurrentDirectoryW(512, tbuf);
	return(gfxedit_strdup_wcs2utf(tbuf));
}

typedef struct {
	HANDLE hFind;
	WIN32_FIND_DATAW find_data;
	char *str;
}GfxEdit_DirInfo;

wchar_t *gfxedit_fillwcspath(wchar_t *obuf, char *str)
{
	char *cs;
	wchar_t *wct;
	int val;
	
	cs=str; wct=obuf;
	while(*cs)
	{
		val=gfxedit_readutf(&cs);
		
		if(val=='/')
			val='\\';
		
		if(val<0x10000)
		{
			*wct++=val;
			continue;
		}
	}
	*wct=0;
	return(wct);
}

void *gfxedit_opendir(char *path)
{
	static GfxEdit_DirInfo t_dir;
	wchar_t pathbuf[MAX_PATH];
	GfxEdit_DirInfo *dir;
	wchar_t *wct;
	
	dir=&t_dir;
	memset(dir, 0, sizeof(t_dir));
	
	wct=gfxedit_fillwcspath(pathbuf, path);
	*wct++='\\';
	*wct++='*';
	*wct=0;
	
	dir->hFind=FindFirstFileW(pathbuf, &(dir->find_data));
	
	if (dir->hFind == INVALID_HANDLE_VALUE)
		return(NULL);
	dir->str=gfxedit_strdup_wcs2utf(dir->find_data.cFileName);

	return(dir);
}

char *gfxedit_readdir(void *idir)
{
	GfxEdit_DirInfo *dir;
	char *str;
	int rt;
	
	dir=idir;
	if(!dir)
		return(NULL);
	str=dir->str;
	if(!str)
		return(str);

	rt=FindNextFileW(dir->hFind, &(dir->find_data));
	dir->str=gfxedit_strdup_wcs2utf(dir->find_data.cFileName);
	if(!rt)
		dir->str=NULL;
	return(str);
}

void gfxedit_closedir(void *idir)
{
	GfxEdit_DirInfo *dir;
	char *str;
	dir=idir;
	if(!dir)
		return;
	FindClose(dir->hFind);
}

char *gfxedit_combinepath(char *cwd, char *newdir)
{
	char tbuf[512];
	char *cs, *ct, *ct1;
	
	strcpy(tbuf, cwd);
	ct=tbuf+strlen(tbuf);
	
	cs=newdir;
	
	if(cs[1]==':' && ((cs[2]=='/') || (cs[2]=='\\')))
	{
		tbuf[0]=cs[0];
		tbuf[1]=cs[1];
		ct=tbuf+2;
		*ct=0;
		cs+=3;
		while((*cs=='/') || (*cs=='\\'))
			cs++;
	}
	
	if((*cs=='/') || (*cs=='\\'))
	{
		if(tbuf[1]==':')
		{
			ct=tbuf+2;
			*ct=0;	
		}else
		{
			ct=tbuf;
			*ct=0;	
		}
		while((*cs=='/') || (*cs=='\\'))
			cs++;
	}
	
	while(*cs)
	{
		if(!strcmp(cs, "."))
			{ cs++; continue; }
		if(!strncmp(cs, "./", 2) || !strncmp(cs, ".\\", 2))
			{ cs+=2; continue; }
		if(!strncmp(cs, "../", 3) || !strncmp(cs, "..\\", 3) ||
			!strcmp(cs, ".."))
		{
			ct1=ct;
			while(ct1>tbuf)
			{
				if((*ct1=='/') || (*ct1=='\\'))
					break;
				ct1--;
			}
			
			if(ct1>tbuf)
			{
				ct=ct1;
				*ct=0;
			}
			cs+=2;
			while((*cs=='/') || (*cs=='\\'))
				cs++;
			continue;
		}
		*ct++='\\';
		while(*cs && (*cs!='/') && (*cs!='\\'))
			{ *ct++=*cs++; }
		while((*cs=='/') || (*cs=='\\'))
			cs++;
	}
	*ct=0;
	
	return(gfxedit_strdup(tbuf));
}

char *gfxedit_chdir(char *cwd, char *newdir)
{
	char *cwd1;
	void *dir;

	cwd1=gfxedit_combinepath(cwd, newdir);
	
	dir=gfxedit_opendir(cwd1);
	if(dir)
	{
		gfxedit_closedir(dir);
		return(cwd1);
	}
	return(cwd);
}

u32 repack_rgb555to32(u16 px)
{
	int cr, cg, cb;
	u32 c;
	cr=(px>>10)&31;
	cg=(px>> 5)&31;
	cb=(px>> 0)&31;
	cr=(cr<<3)|(cr>>2);
	cg=(cg<<3)|(cg>>2);
	cb=(cb<<3)|(cb>>2);
//	c=(255U<<24)|(cr<<16)|(cg<<8)|cb;
	c=(255U<<24)|(cb<<16)|(cg<<8)|cr;
	return(c);
}

int main()
{
	GfxEdit_Context *ctx;
	u16 *kcs;
	int t0, t1, dt;
	int mx, my, mz, mb;
	int x, y, z, c;
	int i, j, k;

	btesh2_gfxcon_fbxs=320;
	btesh2_gfxcon_fbys=200;
	window_def_label="TK_GfxEdit";

	ctx=GfxEdit_CreateContext();

	ctx->canvas_width=64;
	ctx->canvas_height=64;
	ctx->canvas_pixels=malloc(64*64);
	ctx->canvas_pal4=malloc(256*4);
	ctx->canvas_pal2=malloc(256*2);
	memset(ctx->canvas_pixels, 15, 64*64);
	ctx->canvas_bpp=8;
	ctx->zoom=1;
	
	ctx->conbuf=malloc(40*25);
	memset(ctx->conbuf, 0, 40*25);

	GfxEdit_SetupPalRGBIx2(ctx);

#if 0
	for(i=0; i<256; i++)
		{ ctx->canvas_pal2[i]=gfxedit_uipal[0xD]; }
	ctx->canvas_pal2[255]|=0x8000;
	
	for(i=0; i<16; i++)
	{
		ctx->canvas_pal2[i]=gfxedit_uipal[i];
		ctx->canvas_pal4[i]=repack_rgb555to32(gfxedit_uipal[i]);

		mx=i*2; my=mx; mz=mx;
		k=(mz<<10)|(my<<5)|(mz<<0);
		ctx->canvas_pal2[16+i]=k;
	}

	for(i=0; i<216; i++)
	{
		mx=(i/ 1)%6;
		my=(i/ 6)%6;
		mz=(i/36)%6;
		mx*=6; my*=6; mz*=6;
		k=(mz<<10)|(my<<5)|(mx<<0);
		ctx->canvas_pal2[32+i]=k;
	}

	for(i=0; i<256; i++)
		{ ctx->canvas_pal4[i]=repack_rgb555to32(ctx->canvas_pal2[i]); }
#endif

	ctx->sel_tool=GFXEDIT_TOOL_PENCIL;
	ctx->sel_color1=0;
	ctx->sel_color2=15;

	GfxEdit_InitConsole();
//	GfxEdit_ConPuts(ctx, "Console Test\n");

#if 1
	GfxDrv_Start();
	GfxDrv_PrepareFramebuf();
	GfxDrv_HideCursor(1);
#endif

	t0=FRGL_TimeMS();
	while(!gfxdrv_kill)
	{
		t1=FRGL_TimeMS();
		dt=t1-t0;
		if((dt>=0) && (dt<10))
		{
			Sleep(10);
			continue;
		}
		t0=t1;
		
		ctx->time_ms=t1;
		
		kcs=FRGL_GetKeybuf();
		while(*kcs)
		{
			GfxEdit_PumpKey(ctx, ((*kcs)&0x7FFF), ((*kcs)&0x8000)==0);
			kcs++;
		}
		
		GfxDrv_MouseGetPos(&mx, &my, &mb);
		
		mx=(mx*(320.0/window_width))+160;
		my=(my*(200.0/window_height))+100;
		
		if(mb&7)
		{
//			printf("click %d %d\n", mx, my);
			GfxEdit_MouseClickDown(ctx, mx, my, mb);
		}
		
		GfxEdit_RedrawView(ctx);

#if 1
		GfxDrv_BeginDrawing();

		for(y=0; y<200; y++)
		{
			for(x=0; x<320; x++)
			{
				z=y*320+x;
				c=ctx->screen[z];
				((u32 *)btesh2_gfxcon_framebuf)[z]=repack_rgb555to32(c);
			}
		}
		
		if((mx>0) && (mx<319) && (my>0) && (my<199))
		{
			z=my*320+mx;
			((u32 *)btesh2_gfxcon_framebuf)[z-1]^=0x00FFFFFF;
			((u32 *)btesh2_gfxcon_framebuf)[z+1]^=0x00FFFFFF;
			((u32 *)btesh2_gfxcon_framebuf)[z-320]^=0x00FFFFFF;
			((u32 *)btesh2_gfxcon_framebuf)[z+320]^=0x00FFFFFF;
			GfxDrv_HideCursor(1);
		}

		btesh2_gfxcon_fb_dirty=1;

		GfxDrv_EndDrawing();
#endif
	}
	
	return(0);
}
