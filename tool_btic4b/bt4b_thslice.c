typedef struct BTIC4B_ImageSlice_s BTIC4B_ImageSlice;

struct BTIC4B_ImageSlice_s {
BTIC4B_ImageSlice *next;
BTIC4B_Context tctx;
byte *ibuf;
byte *blks;
byte *lblks;
byte *obuf;
int xs, ys, qf;
int xo, yo;
int obsz;
volatile byte done;
};

volatile BTIC4B_ImageSlice *lqtvq_slicelive=NULL;
void *lqtvq_slicemutex=NULL;
volatile int lqtvq_sliceworker;


int lqtvq_encImageSlice(void *ptr)
{
	struct BTIC4B_ImageSlice_s *slc;
	
	slc=ptr;
	BTIC4B_EncImageBGRA(&slc->tctx, slc->blks, slc->ibuf, slc->xs, slc->ys);
	slc->done=1;
	return(0);
}

int lqtvq_sliceWorker(void *ptr)
{
	BTIC4B_ImageSlice *slc;
	BTIC4B_Context *ctx;
	int idle1, idle2;

	lqtvq_sliceworker++;
	idle1=1<<16; idle2=256;
	while(1)
	{
		thLockFastMutex(lqtvq_slicemutex);
		slc=(BTIC4B_ImageSlice *)lqtvq_slicelive;
		if(slc)
			{ lqtvq_slicelive=slc->next; }
		thUnlockFastMutex(lqtvq_slicemutex);

		if(!slc)
		{
			if(idle2>0)
			{
				idle2--;
				thSleep(0);
				continue;
			}
			if(idle1<=0)
				break;
			idle1--;
			thSleep(1);
			continue;
		}

		idle1=1<<16;
		idle2=256;

		ctx=&slc->tctx;
		BTIC4B_SetupContextQf(ctx, slc->qf);
		BTIC4B_EncImageBGRA(ctx,
			slc->blks, slc->ibuf, slc->xs, slc->ys);
		BTIC4B_EncImgBlocks(ctx, slc->obuf,
			slc->blks, slc->lblks,
			slc->xs, slc->ys, slc->qf);
		
		slc->obsz=ctx->ct-slc->obuf;
		slc->done=1;
	}
	lqtvq_sliceworker--;
}

int BTIC4B_EncImageThreadsBGRA(BTIC4B_Context *ctx,
	byte *obuf, int obmsz,
	byte *ibuf, int xs, int ys, int qf)
{
	static BTIC4B_ImageSlice slice[8];
	BTIC4B_ImageSlice *slc;
	byte *cs, *ct;
	int xs1, ys1, ystr, slcys, ossz;
	int bi;
	int i, j, k;
	
	BTIC4B_InitScTables();

	if(!lqtvq_slicemutex)
	{
		lqtvq_slicemutex=thFastMutex();
		for(i=0; i<4; i++)
			{ thThread(lqtvq_sliceWorker, NULL); }
	}else
	{
		if(lqtvq_sliceworker<3)
		{
			for(i=0; i<2; i++)
				{ thThread(lqtvq_sliceWorker, NULL); }
		}
	}

	ystr=xs*4;
	xs1=xs>>3;
	ys1=ys>>3;
	slcys=(ys1+7)>>3;
	ossz=obmsz>>3;

	thLockFastMutex(lqtvq_slicemutex);
	for(i=0; i<8; i++)
	{
		cs=ibuf+i*8*slcys*ystr;
		
		memcpy(&slice[i].tctx, ctx, sizeof(BTIC4B_Context));

		slc=&slice[i];
		slc->ibuf=ibuf+i*8*slcys*ystr;
		slc->obuf=obuf+i*ossz+256;
		slc->blks=ctx->blks+(i*slcys*xs1)*64;
		slc->lblks=ctx->lblks?(ctx->lblks+(i*slcys*xs1)*64):NULL;
		slc->xs=xs;
		slc->ys=8*slcys;
		slc->xo=0;
		slc->yo=i*slcys;
		slc->qf=qf;
		slc->done=0;
		
		slc->next=(BTIC4B_ImageSlice *)lqtvq_slicelive;
		lqtvq_slicelive=slc;
	}
	thUnlockFastMutex(lqtvq_slicemutex);

	while(1)
	{
		for(i=0; i<8; i++)
		{
			if(!slice[i].done)
				break;
		}
		if(i<8)
		{
			thSleep(1);
			continue;
		}
		break;
	}

	ct=obuf;
	for(i=0; i<8; i++)
	{
		slc=&slice[i];
		
		j=8+slc->obsz;
		
		*ct++='0xE3';
		*ct++=(j>>16);
		*ct++=(j>> 8);
		*ct++=(j    );
		*ct++='S';
		*ct++='L';
		*ct++=(slc->yo>>8);
		*ct++=(slc->yo   );
		memcpy(ct, slc->obuf, slc->obsz);
		ct+=slc->obsz;
	}
	
	return(ct-obuf);
}
