volatile BTIC1H_Context *btic1h_workqueue;
void *btic1h_workqueue_lock;
volatile int btic1h_workqueue_workers;
volatile int btic1h_workqueue_taskcnt[1024];

int btic1h_workqueue_defaultworkers=4;

void btic1h_thLockQueue()
{
	if(!btic1h_workqueue_lock)
		btic1h_workqueue_lock=thFastMutex();
	thLockFastMutex(btic1h_workqueue_lock);
}

void btic1h_thUnlockQueue()
{
	thUnlockFastMutex(btic1h_workqueue_lock);
}

int btic1h_QueueWorker(void *data)
{
	BTIC1H_Context *job, *ljob;
	int idle1, idle2;
	int i;
	
//	btic1h_workqueue_workers++;
	ljob=NULL;
	idle1=1024;
	idle2=64;
	while(idle1>0)
	{
		btic1h_thLockQueue();
		if(ljob)
			{ btic1h_workqueue_taskcnt[ljob->sltid]--; ljob=NULL; }
		job=(BTIC1H_Context *)btic1h_workqueue;
		if(job)
			{ btic1h_workqueue=job->wqnext; }
		btic1h_thUnlockQueue();
		if(!job)
		{
			if(idle2>0)
			{
				idle2--;
				thSleep(0);
				continue;
			}
			idle1--;
			thSleep(1);
			continue;
		}
		
		idle1=1024;
		idle2=64;
		job->DoWork(job);
		ljob=job;

//		btic1h_thLockQueue();
//		btic1h_workqueue_taskcnt[job->sltid]--;
//		btic1h_thUnlockQueue();
	}
	btic1h_workqueue_workers--;
	return(0);
}

int BTIC1H_Work_CheckSpawnWorkers(int n)
{
	int i;
	
	i=n-btic1h_workqueue_workers;
	while(i>0)
	{
		btic1h_workqueue_workers++;
		thThread(btic1h_QueueWorker, NULL);
		i--;
	}
	return(0);
}

int BTIC1H_Work_QueueJob(BTIC1H_Context *ctx, int tid)
{
	btic1h_thLockQueue();
	ctx->sltid=tid;
	btic1h_workqueue_taskcnt[tid]++;
	ctx->wqnext=(BTIC1H_Context *)btic1h_workqueue;
	btic1h_workqueue=ctx;
	btic1h_thUnlockQueue();
	return(0);
}

int BTIC1H_Work_QueueJobNb(BTIC1H_Context *ctx, int tid)
{
	ctx->sltid=tid;
	btic1h_workqueue_taskcnt[tid]++;
	ctx->wqnext=(BTIC1H_Context *)btic1h_workqueue;
	btic1h_workqueue=ctx;
	return(0);
}

int BTIC1H_Work_AllocTidNb(void)
{
	int i, j, k;
	
	for(i=0; i<1024; i++)
	{
		if(!btic1h_workqueue_taskcnt[i])
		{
			btic1h_workqueue_taskcnt[i]=1;
			return(i);
		}
	}
	return(-1);
}

void BTIC1H_Work_FreeTidNb(int tid)
{
	if(btic1h_workqueue_taskcnt[tid]!=1)
	{
		*(int *)-1=-1;
	}
	btic1h_workqueue_taskcnt[tid]=0;
}

int BTIC1H_Work_GetTidCountNb(int tid)
{
	return(btic1h_workqueue_taskcnt[tid]-1);
}
