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

/*

Mode 4
0000 1ttm  rrrr rRRR ( 0/ 1)
RRgg gggG  GGGG bbbb ( 2/ 3)
bBBB BBaa  aaaa AAAA ( 4/ 5)
AAii iiii  iiii iiii ( 6/ 7)
iiii iiii  iiii iiii ( 8/ 9)
ijjj jjjj  jjjj jjjj (10/11)
jjjj jjjj  jjjj jjjj (12/13)
jjjj jjjj  jjjj jjjj (14/15)

Mode 5
0000 01tt  rrrr rrrR ( 0/ 1)
RRRR RRgg  gggg gGGG ( 2/ 3)
GGGG bbbb  bbbB BBBB ( 4/ 5)
BBaa aaaa  aaAA AAAA ( 6/ 7)
AAii iiii  iiii iiii ( 8/ 9)
iiii iiii  iiii iiii (10/11)
ijjj jjjj  jjjj jjjj (12/13)
jjjj jjjj  jjjj jjjj (14/15)

Mode 6
0000 001r  rrrr rrRR ( 0/ 1)
RRRR Rggg  gggg GGGG ( 2/ 3)
GGGb bbbb  bbBB BBBB ( 4/ 5)
Baaa aaaa  AAAA AAAp ( 6/ 7)
Piii iiii  iiii iiii ( 8/ 9)
iiii iiii  iiii iiii (10/11)
iiii iiii  iiii iiii (12/13)
iiii iiii  iiii iiii (14/15)

*/


int bgbbtj_bc7_weights2[4]=
	{ 0, 21, 43, 64};
int bgbbtj_bc7_weights3[8]=
	{ 0,  9, 18, 27, 37, 46, 55, 64};
int bgbbtj_bc7_weights4[16]= 
	{ 0,  4,  9, 13, 17, 21, 26, 30,
	 34, 38, 43, 47, 51, 55, 60, 64};

byte bgbbtj_bc7_partition2[64*16]=
{   0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,    0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,
    0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,    0,0,0,1,0,0,1,1,0,0,1,1,0,1,1,1,
    0,0,0,0,0,0,0,1,0,0,0,1,0,0,1,1,    0,0,1,1,0,1,1,1,0,1,1,1,1,1,1,1,
    0,0,0,1,0,0,1,1,0,1,1,1,1,1,1,1,    0,0,0,0,0,0,0,1,0,0,1,1,0,1,1,1,
    0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,    0,0,1,1,0,1,1,1,1,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,1,0,1,1,1,1,1,1,1,    0,0,0,0,0,0,0,0,0,0,0,1,0,1,1,1,
    0,0,0,1,0,1,1,1,1,1,1,1,1,1,1,1,    0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,
    0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,    0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,
    0,0,0,0,1,0,0,0,1,1,1,0,1,1,1,1,    0,1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,1,0,0,0,1,1,1,0,    0,1,1,1,0,0,1,1,0,0,0,1,0,0,0,0,
    0,0,1,1,0,0,0,1,0,0,0,0,0,0,0,0,    0,0,0,0,1,0,0,0,1,1,0,0,1,1,1,0,
    0,0,0,0,0,0,0,0,1,0,0,0,1,1,0,0,    0,1,1,1,0,0,1,1,0,0,1,1,0,0,0,1,
    0,0,1,1,0,0,0,1,0,0,0,1,0,0,0,0,    0,0,0,0,1,0,0,0,1,0,0,0,1,1,0,0,
    0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,    0,0,1,1,0,1,1,0,0,1,1,0,1,1,0,0,
    0,0,0,1,0,1,1,1,1,1,1,0,1,0,0,0,    0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,
    0,1,1,1,0,0,0,1,1,0,0,0,1,1,1,0,    0,0,1,1,1,0,0,1,1,0,0,1,1,1,0,0,
    0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,    0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,
    0,1,0,1,1,0,1,0,0,1,0,1,1,0,1,0,    0,0,1,1,0,0,1,1,1,1,0,0,1,1,0,0,
    0,0,1,1,1,1,0,0,0,0,1,1,1,1,0,0,    0,1,0,1,0,1,0,1,1,0,1,0,1,0,1,0,
    0,1,1,0,1,0,0,1,0,1,1,0,1,0,0,1,    0,1,0,1,1,0,1,0,1,0,1,0,0,1,0,1,
    0,1,1,1,0,0,1,1,1,1,0,0,1,1,1,0,    0,0,0,1,0,0,1,1,1,1,0,0,1,0,0,0,
    0,0,1,1,0,0,1,0,0,1,0,0,1,1,0,0,    0,0,1,1,1,0,1,1,1,1,0,1,1,1,0,0,
    0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,    0,0,1,1,1,1,0,0,1,1,0,0,0,0,1,1,
    0,1,1,0,0,1,1,0,1,0,0,1,1,0,0,1,    0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0,
    0,1,0,0,1,1,1,0,0,1,0,0,0,0,0,0,    0,0,1,0,0,1,1,1,0,0,1,0,0,0,0,0,
    0,0,0,0,0,0,1,0,0,1,1,1,0,0,1,0,    0,0,0,0,0,1,0,0,1,1,1,0,0,1,0,0,
    0,1,1,0,1,1,0,0,1,0,0,1,0,0,1,1,    0,0,1,1,0,1,1,0,1,1,0,0,1,0,0,1,
    0,1,1,0,0,0,1,1,1,0,0,1,1,1,0,0,    0,0,1,1,1,0,0,1,1,1,0,0,0,1,1,0,
    0,1,1,0,1,1,0,0,1,1,0,0,1,0,0,1,    0,1,1,0,0,0,1,1,0,0,1,1,1,0,0,1,
    0,1,1,1,1,1,1,0,1,0,0,0,0,0,0,1,    0,0,0,1,1,0,0,0,1,1,1,0,0,1,1,1,
    0,0,0,0,1,1,1,1,0,0,1,1,0,0,1,1,    0,0,1,1,0,0,1,1,1,1,1,1,0,0,0,0,
    0,0,1,0,0,0,1,0,1,1,1,0,1,1,1,0,    0,1,0,0,0,1,0,0,0,1,1,1,0,1,1,1};

byte bgbbtj_bc7_partition3[64*16]=
{   0,0,1,1,0,0,1,1,0,2,2,1,2,2,2,2,    0,0,0,1,0,0,1,1,2,2,1,1,2,2,2,1,
    0,0,0,0,2,0,0,1,2,2,1,1,2,2,1,1,    0,2,2,2,0,0,2,2,0,0,1,1,0,1,1,1,
    0,0,0,0,0,0,0,0,1,1,2,2,1,1,2,2,    0,0,1,1,0,0,1,1,0,0,2,2,0,0,2,2,
    0,0,2,2,0,0,2,2,1,1,1,1,1,1,1,1,    0,0,1,1,0,0,1,1,2,2,1,1,2,2,1,1,
    0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,    0,0,0,0,1,1,1,1,1,1,1,1,2,2,2,2,
    0,0,0,0,1,1,1,1,2,2,2,2,2,2,2,2,    0,0,1,2,0,0,1,2,0,0,1,2,0,0,1,2,
    0,1,1,2,0,1,1,2,0,1,1,2,0,1,1,2,    0,1,2,2,0,1,2,2,0,1,2,2,0,1,2,2,
    0,0,1,1,0,1,1,2,1,1,2,2,1,2,2,2,    0,0,1,1,2,0,0,1,2,2,0,0,2,2,2,0,
    0,0,0,1,0,0,1,1,0,1,1,2,1,1,2,2,    0,1,1,1,0,0,1,1,2,0,0,1,2,2,0,0,
    0,0,0,0,1,1,2,2,1,1,2,2,1,1,2,2,    0,0,2,2,0,0,2,2,0,0,2,2,1,1,1,1,
    0,1,1,1,0,1,1,1,0,2,2,2,0,2,2,2,    0,0,0,1,0,0,0,1,2,2,2,1,2,2,2,1,
    0,0,0,0,0,0,1,1,0,1,2,2,0,1,2,2,    0,0,0,0,1,1,0,0,2,2,1,0,2,2,1,0,
    0,1,2,2,0,1,2,2,0,0,1,1,0,0,0,0,    0,0,1,2,0,0,1,2,1,1,2,2,2,2,2,2,
    0,1,1,0,1,2,2,1,1,2,2,1,0,1,1,0,    0,0,0,0,0,1,1,0,1,2,2,1,1,2,2,1,
    0,0,2,2,1,1,0,2,1,1,0,2,0,0,2,2,    0,1,1,0,0,1,1,0,2,0,0,2,2,2,2,2,
    0,0,1,1,0,1,2,2,0,1,2,2,0,0,1,1,    0,0,0,0,2,0,0,0,2,2,1,1,2,2,2,1,
    0,0,0,0,0,0,0,2,1,1,2,2,1,2,2,2,    0,2,2,2,0,0,2,2,0,0,1,2,0,0,1,1,
    0,0,1,1,0,0,1,2,0,0,2,2,0,2,2,2,    0,1,2,0,0,1,2,0,0,1,2,0,0,1,2,0,
    0,0,0,0,1,1,1,1,2,2,2,2,0,0,0,0,    0,1,2,0,1,2,0,1,2,0,1,2,0,1,2,0,
    0,1,2,0,2,0,1,2,1,2,0,1,0,1,2,0,    0,0,1,1,2,2,0,0,1,1,2,2,0,0,1,1,
    0,0,1,1,1,1,2,2,2,2,0,0,0,0,1,1,    0,1,0,1,0,1,0,1,2,2,2,2,2,2,2,2,
    0,0,0,0,0,0,0,0,2,1,2,1,2,1,2,1,    0,0,2,2,1,1,2,2,0,0,2,2,1,1,2,2,
    0,0,2,2,0,0,1,1,0,0,2,2,0,0,1,1,    0,2,2,0,1,2,2,1,0,2,2,0,1,2,2,1,
    0,1,0,1,2,2,2,2,2,2,2,2,0,1,0,1,    0,0,0,0,2,1,2,1,2,1,2,1,2,1,2,1,
    0,1,0,1,0,1,0,1,0,1,0,1,2,2,2,2,    0,2,2,2,0,1,1,1,0,2,2,2,0,1,1,1,
    0,0,0,2,1,1,1,2,0,0,0,2,1,1,1,2,    0,0,0,0,2,1,1,2,2,1,1,2,2,1,1,2,
    0,2,2,2,0,1,1,1,0,1,1,1,0,2,2,2,    0,0,0,2,1,1,1,2,1,1,1,2,0,0,0,2,
    0,1,1,0,0,1,1,0,0,1,1,0,2,2,2,2,    0,0,0,0,0,0,0,0,2,1,1,2,2,1,1,2,
    0,1,1,0,0,1,1,0,2,2,2,2,2,2,2,2,    0,0,2,2,0,0,1,1,0,0,1,1,0,0,2,2,
    0,0,2,2,1,1,2,2,1,1,2,2,0,0,2,2,    0,0,0,0,0,0,0,0,0,0,0,0,2,1,1,2,
    0,0,0,2,0,0,0,1,0,0,0,2,0,0,0,1,    0,2,2,2,1,2,2,2,0,2,2,2,1,2,2,2,
    0,1,0,1,2,2,2,2,2,2,2,2,2,2,2,2,    0,1,1,1,2,0,1,1,2,2,0,1,2,2,2,0};

byte bgbbtj_bc7_anchor2[64]=
{   15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,
    15, 2, 8, 2, 2, 8, 8,15,
     2, 8, 2, 2, 8, 8, 2, 2,
    15,15, 6, 8, 2, 8,15,15,
     2, 8, 2, 2, 2,15,15, 6,
     6, 2, 6, 8,15,15, 2, 2,
    15,15,15,15,15, 2, 2,15};

byte bgbbtj_bc7_anchor3a[64]=
{    3, 3,15,15, 8, 3,15,15,
     8, 8, 6, 6, 6, 5, 3, 3,
     3, 3, 8,15, 3, 3, 6,10,
     5, 8, 8, 6, 8, 5,15,15,
     8,15, 3, 5, 6,10, 8,15,
    15, 3,15, 5,15,15,15,15,
     3,15, 5, 5, 5, 8, 5,10,
     5,10, 8,13,15,12, 3, 3};

byte bgbbtj_bc7_anchor3b[64]=
{   15, 8, 8, 3,15,15, 3, 8,
    15,15,15,15,15,15,15, 8,
    15, 8,15, 3,15, 8,15, 8,
     3,15, 6,10,15,15,10, 8,
    15, 3,15,10,10, 8, 9,10,
     6,15, 8,15, 3, 6, 6, 8,
    15, 3,15,15,15,15,15,15,
    15,15,15,15, 3,15,15, 8};

byte bgbbtj_bc7_part2_idx[1<<16];
byte bgbbtj_bc7_part3_idx1[1<<16];
byte bgbbtj_bc7_part3_idx2[1<<16];
byte bgbbtj_bc7_part3_idxj[64*64];

int bgbbtj_bc7_part_init=0;

int bgbbtj_bc7_bitcnt8(int i)
{
	static byte bcnttab[256];
	static byte init=0;
	int j, k, l;
	
	if(!init)
	{
		for(j=0; j<256; j++)
		{
			l=0;
			for(k=0; k<8; k++)
				if(j&(1<<k))l++;
			bcnttab[j]=l;
		}
		init=1;
	}
	
	return(bcnttab[i&255]);
}

int bgbbtj_bc7_bitcnt16(int i)
{
	return(bgbbtj_bc7_bitcnt8(i)+
		bgbbtj_bc7_bitcnt8(i>>8));
}

int BGBBTJ_BC7_PartitionInit(void)
{
	int tpbits[64];
	byte tpart[16];
	byte *cp, *tp, *cp1, *cp2;
	int te, te2, be, bi;
	int i, j, k, l;
	
	if(bgbbtj_bc7_part_init)
		return(0);
	bgbbtj_bc7_part_init=1;
	
	for(i=0; i<64; i++)
	{
		cp=bgbbtj_bc7_partition2+(i*16);
		k=0;
		for(j=0; j<16; j++)
			if(cp[j])k|=1<<j;
		tpbits[i]=k;
	}
	
	for(i=0; i<65536; i++)
	{
		be=256; bi=0;
		for(j=0; j<64; j++)
		{
			te=bgbbtj_bc7_bitcnt16(i^tpbits[j]);
			te2=bgbbtj_bc7_bitcnt16(~(i^tpbits[j]));

			if(te<be)
				{ bi=j; be=te; }
			if(te2<be)
				{ bi=j; be=te2; }
		}
		bgbbtj_bc7_part2_idx[i]=bi;
	}

#if 0
	for(i=0; i<65536; i++)
	{
		for(j=0; j<16; j++)
			tpart[j]=(i>>j)&1;
		
		be=256; bi=0;
		for(j=0; j<64; j++)
		{
			tp=tpart;
//			cp=bgbbtj_bc7_partition3+(j*64);
			cp=bgbbtj_bc7_partition3+(j*16);
			te=0; te2=0;
			for(k=0; k<16; k++)
			{
				te+=((!cp[k])^(!tp[k]));
				te2+=!((!cp[k])^(!tp[k]));

//				if(bgbbtj_bc7_partition2[j*64+k]!=tpart[k])
//					te++;
			}
			if(te<be)
				{ bi=j; be=te; }
			if(te2<be)
				{ bi=j; be=te2; }
		}

		bgbbtj_bc7_part3_idx1[i]=bi;
		bgbbtj_bc7_part3_idx2[i]=bi;
	}

	for(i=0; i<64; i++)
		for(j=0; j<64; j++)
	{
		if(i==j)
		{
			bgbbtj_bc7_part3_idxj[i*64+j]=i;
			continue;
		}

		bgbbtj_bc7_part3_idxj[i*64+j]=i;

		cp1=bgbbtj_bc7_partition3+(i*16);
		cp2=bgbbtj_bc7_partition3+(j*16);

		for(k=0; k<16; k++)
		{
			if(cp1[k])
			{
				if(cp2[k])
					{ tpart[k]=2; }
				else
					{ tpart[k]=1; }
			}else
				{ tpart[k]=0; }
		}

		be=256; bi=0;
		for(k=0; k<64; k++)
		{
			tp=tpart;
//			cp=bgbbtj_bc7_partition3+(k*64);
			cp=bgbbtj_bc7_partition3+(k*16);
			te=0; te2=0;
			for(l=0; l<16; l++)
			{
				te+=(cp[l]!=tp[l]);
//				te2+=(tp[k]&&(!cp[k]))||(!tp[k]);
			}
			if(te<be)
				{ bi=k; be=te; }
//			if(te2<be)
//				{ bi=k; be=te2; }
		}

		bgbbtj_bc7_part3_idxj[i*64+j]=bi;
	}
#endif

	return(1);
}

int BGBBTJ_BC7_CalcBlockError(
	byte *rgba0, int xstride0, int ystride0,
	byte *rgba1, int xstride1, int ystride1)
{
	int e, dr, dg, db, da;
	int p0, p1;
	int i, j, k;
	
	e=0;
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		p0=i*ystride0+j*xstride0;
		p1=i*ystride1+j*xstride1;
		dr=rgba0[p0+0]-rgba1[p1+0];
		dg=rgba0[p0+1]-rgba1[p1+1];
		db=rgba0[p0+2]-rgba1[p1+2];
		da=rgba0[p0+3]-rgba1[p1+3];
		e+=dr*dr+dg*dg+db*db+da*da;
	}
	return(e);
}
