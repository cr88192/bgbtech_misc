#include "sblk_multi.c"

#include <time.h>

#include "tklcuac_dec.c"
#include "tklcuac_enc.c"

byte *TgvLz_LoadFile(char *name, int *rsz)
{
	byte *buf;
	FILE *fd;
	int sz, i;
	
	fd=fopen(name, "rb");
	if(!fd)
	{
		return(NULL);
	}
	fseek(fd, 0, 2);
	sz=ftell(fd);
	fseek(fd, 0, 0);
	buf=malloc(sz+24);
	i=fread(buf, 1, sz, fd);
	fclose(fd);
	
	if(i!=sz)
	{
		if(i>0)
		{
			sz=i;
		}else
		{
			free(buf);
			return(NULL);
		}
	}

	memset(buf+sz, 0, 16);
	
	*rsz=sz;
	return(buf);
}

int TgvLz_StoreFile(char *name, void *buf, int sz)
{
	FILE *fd;
	
	fd=fopen(name, "wb");
	if(!fd)
		return(-1);
	
	fwrite(buf, 1, sz, fd);
	fclose(fd);
	return(0);
}

int print_usage(char *arg0)
{
	printf("Usage: %s infile outfile [opts*]\n", arg0);
	printf("  -q kbps     Quality in target bitrate (kbps)\n");
	printf("  -e          Encode Image\n");
	printf("  -d          Decode Image\n");
}

int main(int argc, char *argv[])
{
	TKuAC_EncState *ectx;
	TKuAC_DecState *dctx;
	byte *tbuf, *tct;
	short *sibuf;
	short *sobuf;
	short *sebuf;
	short *tcs, *tcs0;
	char *ifn, *ofn;
	s64 err;
	int ilen, nblk, kbps, ihz, ihzm, md;
	int i, j, k, szbuf, szobuf, olen;
	int t0, t0e, t1, t2;

//	ifn=argv[1];
//	ofn=argv[2];

	ifn=NULL;
	ofn=NULL;
	md=0;
	
	kbps=96;
	ihzm=32000;
		
	for(i=1; i<argc; i++)
	{
		if(argv[i][0]=='-')
		{
			if(!strcmp(argv[i], "-t"))
			{
				md=1;
				continue;
			}
		
			if(!strcmp(argv[i], "-e"))
			{
				md=2;
				continue;
			}
		
			if(!strcmp(argv[i], "-d"))
			{
				md=3;
				continue;
			}
		
			if(!strcmp(argv[i], "-q"))
			{
				kbps=atoi(argv[i+1]);
				
				ihzm=44100;
#if 0
				if(kbps<=100)
					ihzm=32000;
				if(kbps<=80)
					ihzm=22050;
				if(kbps<=64)
					ihzm=16000;
				if(kbps<=48)
					ihzm=11025;
				if(kbps<=32)
					ihzm=8000;
#endif

#if 1
				if(kbps<=144)
					ihzm=32000;
				if(kbps<=100)
					ihzm=22050;
				if(kbps<=80)
					ihzm=16000;
				if(kbps<=40)
					ihzm=11025;
				if(kbps<=24)
					ihzm=8000;
#endif

				i++;
				continue;
			}

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

	if(md==1)
	{
	//	sibuf=BGBMID_LoadWAV_16Stereo16(ifn, &ilen);
	//	sibuf=BGBMID_LoadWAV_16Mono16(ifn, &ilen);
	//	sibuf=BGBMID_LoadWAV_32Stereo16(ifn, &ilen);
	//	ihz=16000;

		sibuf=BGBMID_LoadWAV_MaxRateStereo16(ifn, ihzm, &ihz, &ilen);
		
		if(!sibuf)
		{
			printf("Fail open %s\n", ifn);
			return(0);
		}
		
		nblk=ilen/1024;
	//	sobuf=malloc(((nblk+4)*1024)*2*sizeof(short));

		tbuf=malloc((nblk*1024)*2*sizeof(short));

		tcs=sibuf;
		tct=tbuf;
		
		ectx=malloc(sizeof(TKuAC_EncState));
		memset(ectx, 0, sizeof(TKuAC_EncState));

		ectx->tgthz=ihz;

		TKuAC_EncodeStreamParmHead(ectx, &tct, kbps);
		ectx->sbufs=sibuf;
		ectx->sbufe=sibuf+ilen*2;

		nblk=ilen/(256*ectx->tgtbnum);
		
		for(i=0; i<nblk; i++)
		{
			if(i && !(i&127))
			{
				TKuAC_EncodeStreamParmHead(ectx, &tct, kbps);
			}
		
			TKuAC_EncodeStreamBlock(ectx, &tct, &tcs);
		}
		
		szbuf=(int)(tct-tbuf);
		k=(szbuf*8*0.001)/(ilen/ihz)+0.5;
		printf("%d bytes %d kbps\n", szbuf, k);



		dctx=malloc(sizeof(TKuAC_DecState));
		memset(dctx, 0, sizeof(TKuAC_DecState));
		
		szobuf=(1<<18);
		sobuf=malloc(szobuf*2*sizeof(short));
		
		tct=tbuf;
		tcs=sobuf;

	//	for(i=0; i<nblk; i++)
		while(1)
		{
			j=(tcs-sobuf)>>1;
			if((j+16384)>szobuf)
			{
				szobuf=szobuf+(szobuf>>1);
				sobuf=realloc(sobuf, szobuf*2*sizeof(short));
				tcs=sobuf+(j<<1);
			}
			
			j=TKuAC_DecodeStreamBlock(dctx, &tct, &tcs, tbuf+szbuf);
			if(j<=0)
				break;
		}

		olen=(tcs-sobuf)>>1;

	//	BGBMID_StoreWAV(ofn, (byte *)sobuf, 2, 16000, 16, ilen);
		BGBMID_StoreWAV(ofn, (byte *)sobuf, 2, dctx->tgthz, 16, olen);

		err=0;
		sebuf=malloc(szobuf*2*sizeof(short));
		for(i=0; i<olen; i++)
		{
			j=sobuf[i*2+0]-sibuf[i*2+0];
			k=sobuf[i*2+1]-sibuf[i*2+1];
			j=tkuac_clamp16s(j);
			k=tkuac_clamp16s(k);
			err+=j*j;
			err+=k*k;
			sebuf[i*2+0]=j;
			sebuf[i*2+1]=k;
		}
		
		k=sqrt(err/olen);
		printf("RMSE %d\n", k);

		BGBMID_StoreWAV("tst0_err.wav", (byte *)sebuf, 2, dctx->tgthz, 16, olen);

#if 0
		printf("Decode Bench\n");
		t0=clock();
		t0e=t0+10*CLOCKS_PER_SEC;

		t1=t0;
		err=0;
		
		while(t1<t0e)
		{
			tct=tbuf;
			tcs=sobuf;
		//	for(i=0; i<nblk; i++)
			while(1)
			{
				j=(tcs-sobuf)>>1;
				if((j+16384)>szobuf)
				{
					szobuf=szobuf+(szobuf>>1);
					sobuf=realloc(sobuf, szobuf*2*sizeof(short));
					tcs=sobuf+(j<<1);
				}
				
				tcs0=tcs;
				j=TKuAC_DecodeStreamBlock(dctx, &tct, &tcs, tbuf+szbuf);
				if(j<=0)
					break;
				
				k=tcs-tcs0;
				err+=k>>1;
			}
			t1=clock();
		}
		
		printf("%.3f Msamples/sec\n", (err/10)/1000000.0);
#endif
	}

	if(md==2)
	{
//		sibuf=BGBMID_LoadWAV_16Stereo16(ifn, &ilen);
	//	sibuf=BGBMID_LoadWAV_16Mono16(ifn, &ilen);
	//	sibuf=BGBMID_LoadWAV_32Stereo16(ifn, &ilen);
//		ihz=16000;

		sibuf=BGBMID_LoadWAV_MaxRateStereo16(ifn, ihzm, &ihz, &ilen);
		
		if(!sibuf)
		{
			printf("Fail open %s\n", ifn);
			return(0);
		}
		
		nblk=ilen/1024;
	//	sobuf=malloc(((nblk+4)*1024)*2*sizeof(short));

		tbuf=malloc((nblk*1024)*2*sizeof(short));

		tcs=sibuf;
		tct=tbuf;
		
		ectx=malloc(sizeof(TKuAC_EncState));
		memset(ectx, 0, sizeof(TKuAC_EncState));

//		kbps=96;
		
		ectx->tgthz=ihz;
		ectx->curspos=0;

		TKuAC_EncodeStreamParmHead(ectx, &tct, kbps);
		ectx->sbufs=sibuf;
		ectx->sbufe=sibuf+ilen*2;

		nblk=ilen/(256*ectx->tgtbnum);
		
		for(i=0; i<nblk; i++)
		{
			if(i && !(i&127))
			{
				ectx->curspos=(tcs-sibuf)/2;
				TKuAC_EncodeStreamParmHead(ectx, &tct, kbps);
			}
		
			TKuAC_EncodeStreamBlock(ectx, &tct, &tcs);
		}

		ectx->curspos=(tcs-sibuf)/2;
		TKuAC_EncodeStreamParmHead(ectx, &tct, kbps);
		
		szbuf=(int)(tct-tbuf);
		k=(szbuf*8*0.001)/(ilen/ihz)+0.5;
		printf("%d bytes %d kbps\n", szbuf, k);
		
		TgvLz_StoreFile(ofn, tbuf, szbuf);
	}

	if(md==3)
	{
		tbuf=TgvLz_LoadFile(ifn, &szbuf);

		if(!tbuf)
		{
			printf("Fail open %s\n", ifn);
			return(0);
		}

		dctx=malloc(sizeof(TKuAC_DecState));
		memset(dctx, 0, sizeof(TKuAC_DecState));
		
		szobuf=(1<<18);
		sobuf=malloc(szobuf*2*sizeof(short));
		
		tct=tbuf;
		tcs=sobuf;

	//	for(i=0; i<nblk; i++)
		while(1)
		{
			j=(tcs-sobuf)>>1;
			if((j+16384)>szobuf)
			{
				szobuf=szobuf+(szobuf>>1);
				sobuf=realloc(sobuf, szobuf*2*sizeof(short));
				tcs=sobuf+(j<<1);
			}
			
			j=TKuAC_DecodeStreamBlock(dctx, &tct, &tcs, tbuf+szbuf);
			if(j<=0)
				break;
		}

		olen=(tcs-sobuf)>>1;

	//	BGBMID_StoreWAV(ofn, (byte *)sobuf, 2, 16000, 16, ilen);
		BGBMID_StoreWAV(ofn, (byte *)sobuf, 2, dctx->tgthz, 16, olen);
	}
}
