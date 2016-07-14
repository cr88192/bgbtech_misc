#include "bt4b_multi.c"
#include "bt1h_targa.c"

float calc_rmse(byte *ibuf0, byte *ibuf1, int xs, int ys)
{
	double er, eg, eb;
	int dr, dg, db;
	int i, j, k, n;
	
	n=xs*ys; er=0; eg=0; eb=0;
	for(i=0; i<n; i++)
	{
		dr=ibuf0[i*4+0]-ibuf1[i*4+0];
		dg=ibuf0[i*4+1]-ibuf1[i*4+1];
		db=ibuf0[i*4+2]-ibuf1[i*4+2];
		er=er+dr*dr;
		eg=eg+dg*dg;
		eb=eb+db*db;
	}
	return(sqrt((er+eg+eb)/(3*n)));
}

void cmp_rmse(byte *ibuf0, byte *ibuf1, int xs, int ys)
{
	double er, eg, eb;
	int dr, dg, db;
	int i, j, k, n;
	
	n=xs*ys; er=0; eg=0; eb=0;
	for(i=0; i<n; i++)
	{
		dr=ibuf0[i*4+0]-ibuf1[i*4+0];
		dg=ibuf0[i*4+1]-ibuf1[i*4+1];
		db=ibuf0[i*4+2]-ibuf1[i*4+2];
		er=er+dr*dr;
		eg=eg+dg*dg;
		eb=eb+db*db;
	}
	
	printf("RMSE: r=%.3f g=%.3f b=%.3f avg=%.3f\n",
		sqrt(er/n),
		sqrt(eg/n),
		sqrt(eb/n),
		sqrt((er+eg+eb)/(3*n)));
}

void cmp_blkrmse(byte *ibuf0, byte *ibuf1, byte *blks, int xs, int ys)
{
	static int btcost[32]={
	  1,   4,   8,   8,  16,  32,  32,  64,		//00-07
	 -1,   8,  16,  16,  32,  64,  64, 128,		//08-0F
	192, 256,  -1, 192,  16,  48,  96, 144,		//10-17
	192, 448, 256, 384, 256, 384, 384, 640};	//18-1F
	static char *btname[32]={
	  "Flat", "2x2x1", "4x2x1", "2x4x1",	//00-03
	 "4x4x1", "8x4x1", "4x8x1", "8x8x1",	//04-07
	  "R-08", "2x2x2", "4x2x2", "2x4x2", 	//08-0B
	 "4x4x2", "8x4x2", "4x8x2", "8x8x2",	//0C-0F
	 "8x8x3",			"8x8x4",			//10/11
	 "R-12",			"Y8x8x2+UV4x4x2",	//12/13
	 "Y2x2x2+UV2x2x1", 	"Y4x4x2+UV2x2x2",	//14/15
	 "Y4x4x2+UV4x4x2",	"Y8x8x2+UV2x2x2",	//16/1F
	 "Y8x8x2+UV4x4x2",	"Y8x8x3+UV8x8x2",	//18/1F
	 "Y8x8x2+UV4x8x2",  "Y8x8x2+UV8x8x2",	//1A/1F
	 "Y8x8x3+UV4x4x2",	"Y8x8x4+UV4x4x4",	//1C/1F
	 "Y8x8x3+UV4x8x3",	"Y8x8x4+UV8x8x3"};	//1E/1F

	static int abtcost[16]={
	  1,   4,  16,  64,   8,  32,  48,  64,		//00-07
	  1,  -1,  64,  64, 128, 128, 192, 256};	//18-1F
	static char *abtname[16]={
	  "Flat", "2x2x1", "4x4x1", "8x8x1",	//00-03
	 "2x2x2", "4x4x2", "4x4x3", "4x8x2",	//04-07
	 "Flat2", "RSV09", "4x8x2", "8x4x2", 	//08-0B
	 "8x8x2", "8x8x2", "8x8x3", "8x8x4"};	//0C-0F

	double btte[64];
	double bttea[64];
	int btn[64];

	double abtte[64];
	int abtn[64];

	byte *cs0, *cs1;
	double er, eg, eb, ea, e;
	int dr, dg, db, da;
	int xs1, ys1, ystr;
	int i, j, k, l, n;
	
	ystr=xs*4;
	xs1=xs>>3;
	ys1=ys>>3;
	
	for(i=0; i<64; i++)
	{
		btte[i]=0;
		bttea[i]=0;
		btn[i]=0;

		abtte[i]=0;
		abtn[i]=0;
	}
	
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		cs0=ibuf0+i*8*ystr+j*8*4;
		cs1=ibuf1+i*8*ystr+j*8*4;
		
		er=0; eg=0; eb=0; ea=0;
		for(k=0; k<8; k++)
			for(l=0; l<8; l++)
		{
			dr=cs0[k*ystr+l*4+0]-cs1[k*ystr+l*4+0];
			dg=cs0[k*ystr+l*4+1]-cs1[k*ystr+l*4+1];
			db=cs0[k*ystr+l*4+2]-cs1[k*ystr+l*4+2];
			da=cs0[k*ystr+l*4+3]-cs1[k*ystr+l*4+3];
			er=er+dr*dr;
			eg=eg+dg*dg;
			eb=eb+db*db;
			ea=ea+da*da;
		}
		e=(er+eg+eb)/(3*64);

		k=i*xs1+j;
		l=blks[k*64+0];
		btte[l]+=e;
		bttea[l]+=ea/64;
		btn[l]++;

		l=blks[k*64+1];
		abtte[l]+=ea/64;
		abtn[l]++;
	}

	for(i=0; i<64; i++)
	{
		if(!btn[i])
			continue;

		j=sqrt(btte[i]/btn[i])*100;
		k=sqrt(bttea[i]/btn[i])*10;
		printf("B-RMSE: %02X: n=%6d avge=%3d.%02d ae=%3d.%01d,"
			" Tc=%6d (%-14s)\n",
			i, btn[i], j/100, j%100, k/10, k%10,
			(btn[i]*btcost[i]+7)/8, btname[i]);
	}

	for(i=0; i<64; i++)
	{
		if(!abtn[i])
			continue;

		j=sqrt(abtte[i]/abtn[i])*100;
		printf("AB-RMSE: %02X: n=%6d ae=%3d.%01d,"
			" Tc=%6d (%-14s)\n",
			i, abtn[i], j/100, j%100,
			(abtn[i]*abtcost[i]+7)/8, abtname[i]);
	}

	n=xs*ys; er=0; eg=0; eb=0; ea=0;
	for(i=0; i<n; i++)
	{
		dr=ibuf0[i*4+0]-ibuf1[i*4+0];
		dg=ibuf0[i*4+1]-ibuf1[i*4+1];
		db=ibuf0[i*4+2]-ibuf1[i*4+2];
		da=ibuf0[i*4+3]-ibuf1[i*4+3];
		er=er+dr*dr;
		eg=eg+dg*dg;
		eb=eb+db*db;
		ea=ea+da*da;
	}
	
	printf("RMSE: r=%.2f g=%.2f b=%.2f a=%.2f avg=%.3f\n",
		sqrt(er/n),
		sqrt(eg/n),
		sqrt(eb/n),
		sqrt(ea/n),
		sqrt((er+eg+eb)/(3*n)));
}

int dump_bmp(char *name, int xs, int ys, int fcc, byte *ibuf, int isz)
{
	byte *tbuf;
	FILE *fd;
	int sz;

	fd=fopen(name, "wb");
	if(!fd)
	{
		printf("Fail Open %s\n", name);
		return(-1);
	}

	tbuf=malloc(isz+256);
	sz=flatten_bmp(tbuf, xs, ys, fcc, ibuf, isz);
	fwrite(tbuf, 1, sz, fd);
	free(tbuf);
	fclose(fd);
	return(0);
}

int flatten_bmp(byte *obuf,
	int xs, int ys, int fcc,
	byte *ibuf, int isz)
{
	byte hbuf[256];
	int sz1, sz2, sz3, dpm;
	
	sz1=54+isz;
	sz2=54;
	sz3=40;
	dpm=2880;
	
	hbuf[0x00]='B';		hbuf[0x01]='M';
	hbuf[0x02]=sz1;		hbuf[0x03]=sz1>>8;
	hbuf[0x04]=sz1>>16;	hbuf[0x05]=sz1>>24;
	hbuf[0x06]=0;		hbuf[0x07]=0;
	hbuf[0x08]=0;		hbuf[0x09]=0;
	hbuf[0x0A]=sz2;		hbuf[0x0B]=sz2>>8;
	hbuf[0x0C]=sz2>>16;	hbuf[0x0D]=sz2>>24;

	hbuf[0x0E]=sz3;		hbuf[0x0F]=sz3>>8;
	hbuf[0x10]=sz3>>16;	hbuf[0x11]=sz3>>24;
	hbuf[0x12]=xs;		hbuf[0x13]=xs>>8;
	hbuf[0x14]=xs>>16;	hbuf[0x15]=xs>>24;
	hbuf[0x16]=ys;		hbuf[0x17]=ys>>8;
	hbuf[0x18]=ys>>16;	hbuf[0x19]=ys>>24;

	hbuf[0x1A]=1;		hbuf[0x1B]=0;
	hbuf[0x1C]=32;		hbuf[0x1D]=0;
	hbuf[0x1E]=fcc;		hbuf[0x1F]=fcc>>8;
	hbuf[0x20]=fcc>>16;	hbuf[0x21]=fcc>>24;
	hbuf[0x22]=isz;		hbuf[0x23]=isz>>8;
	hbuf[0x24]=isz>>16;	hbuf[0x25]=isz>>24;
	hbuf[0x26]=dpm;		hbuf[0x27]=dpm>>8;
	hbuf[0x28]=dpm>>16;	hbuf[0x29]=dpm>>24;
	hbuf[0x2A]=dpm;		hbuf[0x2B]=dpm>>8;
	hbuf[0x2C]=dpm>>16;	hbuf[0x2D]=dpm>>24;

	hbuf[0x2E]=0;		hbuf[0x2F]=0;
	hbuf[0x30]=0;		hbuf[0x31]=0;

	hbuf[0x32]=0;		hbuf[0x33]=0;
	hbuf[0x34]=0;		hbuf[0x35]=0;
	
	memcpy(obuf, hbuf, sz2);
	memcpy(obuf+sz2, ibuf, isz);
	return(sz2+isz);
	
//	fwrite(hbuf, 1, sz2, fd);
//	fwrite(ibuf, 1, isz, fd);
//	fclose(fd);
//	return(0);
}

int dump_file(char *name, byte *ibuf, int isz)
{
	byte *tbuf;
	FILE *fd;
	int sz;

	fd=fopen(name, "wb");
	if(!fd)
	{
		printf("Fail Open Write %s\n", name);
		return(-1);
	}

	fwrite(ibuf, 1, isz, fd);
	fclose(fd);
	return(0);
}

int load_file(char *name, byte **rbuf, int *rsz)
{
	byte *tbuf;
	FILE *fd;
	int sz, sz1;

	fd=fopen(name, "rb");
	if(!fd)
	{
		printf("Fail Open Read %s\n", name);
		return(-1);
	}

	fseek(fd, 0, 2);
	sz=ftell(fd);
	fseek(fd, 0, 0);

	tbuf=*rbuf;
	if(!tbuf)
	{
		tbuf=malloc(sz+16);
		*rbuf=tbuf;
	}else
	{
		sz1=*rsz;
		if(sz>sz1)
		{
			*rsz=sz;
			return(-1);
		}
	}

	fread(tbuf, 1, sz, fd);
	fclose(fd);
	
	*rbuf=tbuf;
	*rsz=sz;
	
	return(0);
}

int BufBmpGetSpec(byte *buf,
	int *rxs, int *rys,
	int *rfcc, int *risz)
{
	*rxs=*(u32 *)(buf+0x12);
	*rxs=*(u32 *)(buf+0x16);
	*rfcc=*(u32 *)(buf+0x1E);
	*risz=*(u32 *)(buf+0x22);
	return(0);
}

byte *BufBmpGetImg(byte *buf,
	int *rxs, int *rys,
	int *rfcc, int *risz)
{
	int ofs;
	
	if((buf[0]!='B') || (buf[1]!='M'))
		return(NULL);

	ofs=*(u32 *)(buf+0x0A);
	*rxs=*(u32 *)(buf+0x12);
	*rxs=*(u32 *)(buf+0x16);
	*rfcc=*(u32 *)(buf+0x1E);
	*risz=*(u32 *)(buf+0x22);
	return(buf+ofs);
}

int bt4b_encode(char *infile, char *outfile, int qfl)
{
	byte *cbuf, *ibuf;
	int xs, ys, sz;

	ibuf=BTIC1H_Img_LoadTGA(infile, &xs, &ys);
	if(!ibuf)
	{
		printf("Failed Load TGA %s\n", infile);
		return(-1);
	}
	
	cbuf=malloc(1<<24);
	sz=BTIC4B_EncodeImgBmpBuffer(cbuf, 1<<24,
		ibuf, xs, ys, qfl, BTIC4B_CLRS_RGBA);
	if(sz<0)
	{
		printf("Encode Failed %d\n", sz);

		free(cbuf);
		free(ibuf);
		return(sz);
	}
	
	dump_file(outfile, cbuf, sz);

	free(cbuf);
	free(ibuf);
	return(sz);
}

int bt4b_decode(char *infile, char *outfile, int dfl)
{
	byte *cbuf, *ibuf;
	int csz, xs, ys;
	int i;
	
	cbuf=NULL; csz=0;
	i=load_file(infile, &cbuf, &csz);
	if(i<0)
		return(-1);

	i=BTIC4B_DecodeImgBmpBuffer(cbuf, csz, NULL, &xs, &ys, dfl);
	if(i<0)
	{
		printf("Failed decode resolution %d\n", i);
		free(cbuf);
		return(i);
	}
	
	ibuf=malloc(xs*ys*4);
	i=BTIC4B_DecodeImgBmpBuffer(cbuf, csz,
		ibuf, &xs, &ys, BTIC4B_CLRS_RGBA|dfl);
	if(i<0)
	{
		printf("Failed decode image %d\n", i);
		free(cbuf);
		free(ibuf);
		return(i);
	}

	BTIC1H_Img_SaveTGA(outfile, ibuf, xs, ys);
	free(cbuf);
	free(ibuf);
	return(0);
}

int bt4b_test(char *infile, int qfl)
{
	BTIC4B_Context tctx;
	BTIC4B_Context *ctx;
	byte *tbuf0, *tbuf1, *tbuf2, *cbuf;
	double dt, mpxf;
	int t0, t1, t2, t3, t0e;
	int xs, ys, nf, sz;
	int i, j, k;
	
//	tbuf0=BTIC1H_Img_LoadTGA("DSC00602_1.tga", &xs, &ys);
//	tbuf0=BTIC1H_Img_LoadTGA("yazil0_1.tga", &xs, &ys);
	tbuf0=BTIC1H_Img_LoadTGA(infile, &xs, &ys);
	
//	tbuf0=malloc(xs*ys*4);
	tbuf1=malloc(xs*ys*8);
	tbuf2=malloc(xs*ys*8);
//	cbuf=malloc(xs*ys*8);
	cbuf=malloc(1<<24);
	
//	for(i=0; i<512; i++)
//	{
//		bit_htab[i]=i|(9<<16);
//	}

#if 0
	k=xs*ys;
	for(i=0; i<k; i++)
	{
		tbuf0[i*4+0]=rand();
		tbuf0[i*4+1]=rand();
		tbuf0[i*4+2]=rand();
		tbuf0[i*4+3]=rand();
	}
#endif
	
	mpxf=(xs*ys)/1000000.0;
	
	ctx=&tctx;

#if 0
	printf("Encode Test:\n");
	t0=clock(); t1=t0; t0e=t0+(10*CLOCKS_PER_SEC);
	nf=0; sz=0;
	while(t1<t0e)
	{
		memset(ctx, 0, sizeof(BTIC4B_Context));
		ctx->blks=tbuf2;

#if 1
//		BTIC4B_SetupContextQf(ctx, 75);
		BTIC4B_SetupContextQf(ctx, 60);
		BTIC4B_EncImageBGRA(ctx, tbuf2, tbuf0, xs, ys);
		sz=BTIC4B_EncImgBlocks(ctx, cbuf, tbuf2, NULL, xs, ys, qfl);
#endif

//		sz=BTIC4B_EncImgBufFastBGRA(ctx, tbuf1, tbuf0, xs, ys, 75);

//		sz=BTIC4B_EncImageThreadsBGRA(ctx,
//			tbuf1, xs*ys*8, tbuf0, xs, ys, 75);

//		BTIC4B_EncImageThreadsBGRA(ctx, tbuf1, tbuf0, xs, ys);
	
//		BTIC4B_EncImageBGRA(tbuf1, tbuf0, xs, ys);
//		tst_llq0(tbuf0, tbuf1, xs, ys);
//		sz=tst_llqe0(tbuf1, tbuf2, xs, ys);
		nf++;
		t1=clock();
		t2=t1-t0;
		dt=t2/((double)CLOCKS_PER_SEC);
		printf("%d %.2fs %.2ffps %.2fMpix/s sz=%dKiB (%.2fbpp)\r", nf, dt,
			nf/dt, mpxf*(nf/dt), ((sz+512)>>10), (sz*8.0)/(xs*ys));
	}
	printf("\n");
#endif

#if 1
	memset(ctx, 0, sizeof(BTIC4B_Context));
//	ctx->blks=tbuf2;

//	sz=BTIC4B_EncodeImgBufferCtx(ctx, cbuf, 1<<24, tbuf0, xs, ys, 40, 0);
//	sz=BTIC4B_EncodeImgBufferCtx(ctx, cbuf, 1<<24, tbuf0, xs, ys, 60, 0);
	sz=BTIC4B_EncodeImgBufferCtx(ctx, cbuf, 1<<24,
		tbuf0, xs, ys, qfl, BTIC4B_CLRS_RGBA);
//	sz=BTIC4B_EncodeImgBufferCtx(ctx, cbuf, 1<<24, tbuf0, xs, ys, 85, 0);

//	BTIC4B_SetupContextQf(ctx, 75);
//	BTIC4B_SetupContextQf(ctx, 40);
//	BTIC4B_EncImageBGRA(ctx, tbuf2, tbuf0, xs, ys);
//	sz=BTIC4B_EncImgBlocks(ctx, cbuf, tbuf2, NULL, xs, ys, 40);

	BTIC4B_DumpStatsCtx(ctx);

	dump_bmp("bt4at0_bt4b.bpx", xs, ys,
		BTIC4B_FCC_BT4B, cbuf, sz);

	printf("sz=%dKiB (%.2fbpp)\n", ((sz+512)>>10), (sz*8.0)/(xs*ys));
#endif

#if 1
	memset(ctx, 0, sizeof(BTIC4B_Context));

	printf("Decode Test:\n");
	t0=clock(); t1=t0; t0e=t0+(10*CLOCKS_PER_SEC);
	nf=0;
	while(t1<t0e)
	{
//		memset(ctx, 0, sizeof(BTIC4B_Context));
//		ctx->blks=tbuf2;

//		BTIC4B_DecImgBlocks(ctx, cbuf, sz,
//			tbuf2, NULL, xs, ys);
//		BTIC4B_DecImageBGRA(tbuf2, tbuf1, xs, ys);

		BTIC4B_DecodeImgBufferCtx(ctx, cbuf, sz,
			tbuf1, xs, ys, BTIC4B_CLRS_RGBA);

		nf++;
		t1=clock();
		t2=t1-t0;
		dt=t2/((double)CLOCKS_PER_SEC);
		printf("%d %.2fs %.2ffps %.2fMpix/s\r", nf, dt,
			nf/dt, mpxf*(nf/dt));
	}
	printf("\n");
#endif

#if 1
	memset(ctx, 0, sizeof(BTIC4B_Context));
	memset(tbuf1, 0, xs*ys*8);
	memset(tbuf2, 0, xs*ys*8);

	BTIC4B_DecodeImgBufferCtx(ctx, cbuf, sz,
		tbuf1, xs, ys, BTIC4B_CLRS_RGBA);
	cmp_blkrmse(tbuf0, tbuf1, ctx->blks, xs, ys);

//	BTIC4B_DecImgBlocks(ctx, cbuf, sz,
//		tbuf2, NULL, xs, ys);
//	BTIC4B_DecImageBGRA(tbuf2, tbuf1, xs, ys);
//	cmp_rmse(tbuf0, tbuf1, xs, ys);
//	cmp_blkrmse(tbuf0, tbuf1, tbuf2, xs, ys);
	BTIC1H_Img_SaveTGA("bt4at0.tga", tbuf1, xs, ys);
#endif

//	BTIC4B_DecImageBGRA(tbuf1, tbuf2, xs, ys);
//	cmp_rmse(tbuf0, tbuf2, xs, ys);

//	BTIC1H_Img_SaveTGA("bt4at0.tga", tbuf2, xs, ys);
}


int bt4b_test_bcn(char *infile, int qfl, int clrs)
{
	char tb[256];
	BTIC4B_Context tctx;
	BTIC4B_Context *ctx;
	byte *tbuf0, *tbuf1, *tbuf2, *cbuf;
	byte *cs;
	double dt, mpxf;
	int t0, t1, t2, t3, t0e;
	int xs, ys, xs1, ys1, nf, sz;
	int i, j, k;
	
	tbuf0=BTIC1H_Img_LoadTGA(infile, &xs, &ys);
	tbuf1=malloc(xs*ys*8);
	tbuf2=malloc(xs*ys*8);
	cbuf=malloc(1<<24);

	mpxf=(xs*ys)/1000000.0;
	
	ctx=&tctx;

#if 1
	memset(ctx, 0, sizeof(BTIC4B_Context));

	sz=BTIC4B_EncodeImgBufferCtx(ctx, cbuf, 1<<24,
		tbuf0, xs, ys, qfl, BTIC4B_CLRS_RGBA);
	BTIC4B_DumpStatsCtx(ctx);

	dump_bmp("bt4at0_bt4b.bpx", xs, ys,
		BTIC4B_FCC_BT4B, cbuf, sz);

	printf("sz=%dKiB (%.2fbpp)\n", ((sz+512)>>10), (sz*8.0)/(xs*ys));
#endif

#if 1
	memset(ctx, 0, sizeof(BTIC4B_Context));

	printf("Decode Test(BCn):\n");
	t0=clock(); t1=t0; t0e=t0+(10*CLOCKS_PER_SEC);
	nf=0;
	while(t1<t0e)
	{
//		BTIC4B_DecodeImgBufferCtx(ctx, cbuf, sz,
//			tbuf1, xs, ys, BTIC4B_CLRS_BC7MIP);
		BTIC4B_DecodeImgBufferCtx(ctx, cbuf, sz,
			tbuf1, xs, ys, clrs);

		nf++;
		t1=clock();
		t2=t1-t0;
		dt=t2/((double)CLOCKS_PER_SEC);
		printf("%d %.2fs %.2ffps %.2fMpix/s\r", nf, dt,
			nf/dt, mpxf*(nf/dt));
	}
	printf("\n");
#endif

#if 1
	memset(ctx, 0, sizeof(BTIC4B_Context));
	memset(tbuf1, 0, xs*ys*8);
	memset(tbuf2, 0, xs*ys*8);

//	BTIC4B_DecodeImgBufferCtx(ctx, cbuf, sz,
//		tbuf2, xs, ys, BTIC4B_CLRS_BC7MIP);
	BTIC4B_DecodeImgBufferCtx(ctx, cbuf, sz,
		tbuf2, xs, ys, clrs);

	BTIC4B_BC7_DecodeImage(tbuf2, tbuf1, xs, ys, 4, 0);

	cmp_blkrmse(tbuf0, tbuf1, ctx->blks, xs, ys);

	BTIC1H_Img_SaveTGA("bt4at0.tga", tbuf1, xs, ys);
	
	if((clrs==BTIC4B_CLRS_BC1MIP) ||
		(clrs==BTIC4B_CLRS_BC3MIP) ||
		(clrs==BTIC4B_CLRS_BC6MIP) ||
		(clrs==BTIC4B_CLRS_BC7MIP))
	{
		cs=tbuf2+((xs+3)>>2)*((ys+3)>>2)*16;
		xs1=(xs+1)>>1;	ys1=(ys+1)>>1; nf=1;
		while((xs1>1) || (ys1>1))
		{
			BTIC4B_BC7_DecodeImage(cs, tbuf1, xs1, ys1, 4, 0);
			
			sprintf(tb, "bt4at0_mip%d.tga", nf);
			BTIC1H_Img_SaveTGA(tb, tbuf1, xs1, ys1);

			cs+=((xs1+3)>>2)*((ys1+3)>>2)*16;
			xs1=(xs1+1)>>1;	ys1=(ys1+1)>>1; nf++;
		}
	}
	
#endif
}


typedef struct {
short bfq_qdy[32];		//dy cutoff
short bfq_qduv[32];		//duv cutoff
byte bfq_rqfl[32];		//require flags
byte bfq_exfl[32];		//exclude flags
byte bfq_cost[32];		//block cost
float eb, err, bpp;
}BT4B_TestVector;

typedef struct {
BT4B_TestVector vecs[256];
}BT4B_TestArray;

int bt4b_test2_run(BT4B_TestVector *test,
	byte *ibuf, int xs, int ys, int qfl)
{
	static byte *tbuf0, *tbuf1, *tbuf2, *cbuf;
	BTIC4B_Context tctx;
	BTIC4B_Context *ctx;
	double dt, mpxf, e, eb, bpp, qsc;
	int t0, t1, t2, t3, t0e;
	int nf, sz;
	int i, j, k;
	
	if(!tbuf1)
		tbuf1=malloc(xs*ys*8);
	if(!cbuf)
		cbuf=malloc(1<<24);

	qsc=(100-(qfl&127))/25.0;

	ctx=BTIC4B_AllocContext();
	
	for(i=0; i<32; i++)
	{
		ctx->bfq_qdy[i]=qsc*test->bfq_qdy[i];
		ctx->bfq_qduv[i]=qsc*test->bfq_qduv[i];
		ctx->bfq_rqfl[i]=test->bfq_rqfl[i];
		ctx->bfq_exfl[i]=test->bfq_exfl[i];
		ctx->bfq_cost[i]=test->bfq_cost[i];
	}
	
	qfl|=BTIC4B_QFL_USEBFQ;
	
	sz=BTIC4B_EncodeImgBufferCtx(ctx, cbuf, 1<<24,
		ibuf, xs, ys, qfl, BTIC4B_CLRS_RGBA);
	BTIC4B_FreeContext(ctx);

	ctx=BTIC4B_AllocContext();
	BTIC4B_DecodeImgBufferCtx(ctx, cbuf, sz,
		tbuf1, xs, ys, BTIC4B_CLRS_RGBA);
	BTIC4B_FreeContext(ctx);

	bpp=(sz*8.0)/(xs*ys);
	e=calc_rmse(ibuf, tbuf1, xs, ys);
	eb=pow(e, 1.5)*(bpp+1);

	test->err=e;
	test->bpp=bpp;
	test->eb=eb;

//	cmp_rmse(tbuf0, tbuf1, xs, ys);
}

int bt4b_test2_setupvec(BT4B_TestVector *test)
{
	int i;
	
	for(i=0; i<32; i++)
	{
		test->bfq_qdy[i]=192+(rand()&31)+1;
		test->bfq_qduv[i]=192+(rand()&31)+1;
		test->bfq_rqfl[i]=0;
		test->bfq_exfl[i]=0;
		test->bfq_cost[i]=128;
	}
	
	test->bfq_cost[0x08]=255;
	test->bfq_cost[0x12]=255;
	test->bfq_cost[0x13]=255;
	test->bfq_cost[0x1E]=255;
	test->bfq_cost[0x1F]=255;
	
	test->bfq_exfl[0x13]=4;
	test->bfq_exfl[0x19]=4;
	test->bfq_exfl[0x1B]=4;
}

int bt4b_test2_breedvec(
	BT4B_TestVector *testa, BT4B_TestVector *testb,
	BT4B_TestVector *testc)
{
	float f, g;

	int i, j;
	
	for(i=0; i<32; i++)
	{
		f=(rand()&255)/255.0;
		g=(rand()&255)/255.0; g=2*g-10;
		j=(1.0-f)*testa->bfq_qdy[i]+f*testb->bfq_qdy[i]+g;
		if(j<1)j=1;
		if(j>224)j=224;
		testc->bfq_qdy[i]=j;

		f=(rand()&255)/255.0;
		g=(rand()&255)/255.0; g=2*g-10;
		j=(1.0-f)*testa->bfq_qduv[i]+f*testb->bfq_qduv[i]+g;
		if(j<1)j=1;
		if(j>224)j=224;
		testc->bfq_qduv[i]=j;

		f=(rand()&255)/255.0;
		g=(rand()&255)/255.0; g=2*g-10;
		j=(1.0-f)*testa->bfq_cost[i]+f*testb->bfq_cost[i]+g;
		if(j<1)j=1;
		if(j>199)j=199;
		testc->bfq_cost[i]=j;
	}

	testc->bfq_cost[0x08]=255;
	testc->bfq_cost[0x12]=255;
	testc->bfq_cost[0x13]=255;
	testc->bfq_cost[0x1E]=255;
	testc->bfq_cost[0x1F]=255;
}

int bt4b_test2_dump(BT4B_TestVector *test)
{
	int i, j, k;

	printf("Eb=%.2f (Err=%.2f, bpp=%.2f)\n",
		test->eb, test->err, test->bpp);
	for(i=0; i<8; i++)
		for(j=0; j<4; j++)
	{
		k=i*4+j;
		if(test->bfq_cost[k]>200)
			continue;
		printf("%02X(%3d %3d %3d) ", k,
			test->bfq_qdy[k],
			test->bfq_qduv[k],
			test->bfq_cost[k]);
	}
	printf("\n");
}

int bt4b_test2_sim(byte *ibuf, int xs, int ys, int qfl)
{
	BT4B_TestVector *vecs[256];
	BT4B_TestVector *tvec0, *tvec1;
	int i0, i1;
	int i, j, k, n, ng;
	
	for(i=0; i<64; i++)
	{
		vecs[i]=malloc(sizeof(BT4B_TestVector));
		bt4b_test2_setupvec(vecs[i]);
	}
	n=64;
	
	ng=0;
	while(ng<64)
	{
		printf("Generation %d\n", ng++);
		
		for(i=0; i<n; i++)
		{
			printf("%d/%d\r", i, n);
			bt4b_test2_run(vecs[i], ibuf, xs, ys, qfl);
		}
		
		for(i=0; i<n; i++)
			for(j=i+1; j<n; j++)
		{
			tvec0=vecs[i];	tvec1=vecs[j];
			if(tvec1->eb <= tvec0->eb)
				{ vecs[i]=tvec1; vecs[j]=tvec0; }
		}
		
		for(i=16; i<64; i++)
		{
			i0=rand()&15;
			i1=rand()&15;
			bt4b_test2_breedvec(vecs[i0], vecs[i1], vecs[i]);
		}

		for(i=56; i<64; i++)
			bt4b_test2_setupvec(vecs[i]);

		bt4b_test2_dump(vecs[0]);
		
		for(i=0; i<4; i++)
			printf("%p:%.2f ", vecs[i], vecs[i]->eb);
		printf("\n");
	}
}

int bt4b_test2(char *infile, int qfl)
{
	BTIC4B_Context tctx;
	BTIC4B_Context *ctx;
	byte *ibuf;
	double dt, mpxf;
	int t0, t1, t2, t3, t0e;
	int xs, ys, nf, sz;
	int i, j, k;
	
	ibuf=BTIC1H_Img_LoadTGA(infile, &xs, &ys);
	bt4b_test2_sim(ibuf, xs, ys, qfl);
}

int bt4b_help(char *pgm)
{
	printf("usage: %s [opts] infile outfile\n", pgm);
	printf("    -e    Encode infile.TGA outfile.BPX\n");
	printf("    -d    Encode infile.BPX outfile.TGA\n");
	printf("    -t    Test infile.TGA\n");
	printf("    -q q  Set Quality Level (Default=90)\n");
	printf("    -np   Disable Block Prediction\n");
	printf("    -tc   Test infile.TGA / test decode as BCn\n");
	printf("    -bc1 -bc3 -bc6 -bc7, -bc1m -bc3m -bc6m -bc7m\n");
	printf("          Set BCn format used in BCn test\n");
	return(0);
}

int main(int argc, char *argv[])
{
	char *infile, *outfile;
	int cmd, q, qfl, dfl, tclrs;
	int i, j, k;
	
	infile=NULL; outfile=NULL; cmd=0; q=90; qfl=0; dfl=0;
	qfl|=BTIC4B_QFL_USEPRED;
	tclrs=BTIC4B_CLRS_BC7;
	
	for(i=1; i<argc; i++)
	{
		if(argv[i][0]=='-')
		{
			if(!strcmp(argv[i], "-e"))
				cmd=1;
			if(!strcmp(argv[i], "-d"))
				cmd=2;
			if(!strcmp(argv[i], "-t"))
				cmd=3;
			if(!strcmp(argv[i], "-tg"))
				cmd=4;
			if(!strcmp(argv[i], "-tc"))
				cmd=5;

			if(!strcmp(argv[i], "-q"))
			{
				q=atoi(argv[i+1]);
				i++;
				continue;
			}

			if(!strcmp(argv[i], "-p"))
				qfl|=BTIC4B_QFL_USEPRED;
			if(!strcmp(argv[i], "-np"))
				qfl&=~BTIC4B_QFL_USEPRED;

			if(!strcmp(argv[i], "-bc1"))
				tclrs=BTIC4B_CLRS_BC1;
			if(!strcmp(argv[i], "-bc3"))
				tclrs=BTIC4B_CLRS_BC3;
			if(!strcmp(argv[i], "-bc6"))
				tclrs=BTIC4B_CLRS_BC6;
			if(!strcmp(argv[i], "-bc7"))
				tclrs=BTIC4B_CLRS_BC7;
			if(!strcmp(argv[i], "-bc1m"))
				tclrs=BTIC4B_CLRS_BC1MIP;
			if(!strcmp(argv[i], "-bc3m"))
				tclrs=BTIC4B_CLRS_BC3MIP;
			if(!strcmp(argv[i], "-bc6m"))
				tclrs=BTIC4B_CLRS_BC6MIP;
			if(!strcmp(argv[i], "-bc7m"))
				tclrs=BTIC4B_CLRS_BC7MIP;

			continue;
		}
		
		if(!infile)
		{
			infile=argv[i];
			continue;
		}

		if(!outfile)
		{
			outfile=argv[i];
			continue;
		}
	}
	
	if(!cmd)
	{
		bt4b_help(argv[0]);
		return(0);
	}
	
	if(cmd==3)
	{
		if(!infile)
			infile="yazil0_1.tga";
		bt4b_test(infile, q|qfl);
		return(0);
	}

	if(cmd==4)
	{
		if(!infile)
			infile="yazil0_1.tga";
		bt4b_test2(infile, q|qfl);
		return(0);
	}

	if(cmd==5)
	{
		if(!infile)
			infile="yazil0_1.tga";
		bt4b_test_bcn(infile, q|qfl, tclrs);
		return(0);
	}

	if(!infile || !outfile)
	{
		bt4b_help(argv[0]);
		return(0);
	}

	if(cmd==1)
	{
		bt4b_encode(infile, outfile, q|qfl);
		return(0);
	}

	if(cmd==2)
	{
		bt4b_decode(infile, outfile, dfl);
		return(0);
	}
}
