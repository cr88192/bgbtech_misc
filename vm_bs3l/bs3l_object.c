bs3l_key	bs3lvm_tag_object;
bs3l_key	bs3lvm_tag_array;
bs3l_key	bs3lvm_tag_string;
bs3l_key	bs3lvm_tag_symbol;

char		*bs3lvm_tag_str[4096];
s16			 bs3lvm_tag_chn[4096];
s16			 bs3lvm_tag_tab[256];
s16			 bs3lvm_tag_cnt;

int BS3LVM_HashTagForName(char *name)
{
	uchar *cs;
	int h;
	if(!name)
		return(0);
	cs=(uchar *)name; h=1;
	while(*cs)
		{ h=(h<<1)^(h>>5)^(*cs++); }
	return(h&255);
}

int BS3LVM_GetTagForName(char *name)
{
	int h, i;
	if(!name)
		return(0);
	h=BS3LVM_HashTagForName(name);
	
	i=bs3lvm_tag_tab[h];
	while(i>0)
	{
		if(!strcmp(bs3lvm_tag_str[i], name))
			return(i);
		i=bs3lvm_tag_chn[i];
	}
	
	if(bs3lvm_tag_cnt<1)
		bs3lvm_tag_cnt=1;
	i=bs3lvm_tag_cnt++;

	bs3lvm_tag_str[i]=strdup(name);
	bs3lvm_tag_chn[i]=bs3lvm_tag_tab[h];
	bs3lvm_tag_tab[h]=i;
	return(i);
}

int BS3LVM_InitTags(void)
{
	if(bs3lvm_tag_cnt>0)
		return(0);
	bs3lvm_tag_object=BS3LVM_GetTagForName("Iobject");
	bs3lvm_tag_array=BS3LVM_GetTagForName("Iarray");
	bs3lvm_tag_string=BS3LVM_GetTagForName("Istring");
	bs3lvm_tag_symbol=BS3LVM_GetTagForName("Isymbol");
	return(0);
}

int BS3LVM_ValIsFixnum(bs3l_val a)
	{ return((a>>62)==1); }
int BS3LVM_ValIsFlonum(bs3l_val a)
	{ return((a>>62)==2); }

int BS3LVM_ValIsObjTag(bs3l_val a, int tag)
{
	return((a>>48)==tag);
}

int BS3LVM_ValIsObject(bs3l_val a)
{
	if(!bs3lvm_tag_object)
		BS3LVM_InitTags();
	return(BS3LVM_ValIsObjTag(a, bs3lvm_tag_object));
}

int BS3LVM_ValIsString(bs3l_val a)
{
	if(!bs3lvm_tag_object)
		BS3LVM_InitTags();
	return(BS3LVM_ValIsObjTag(a, bs3lvm_tag_string));
}

void *BS3LVM_ValUnwrapPointer(bs3l_val a)
{
	return((void *)(intptr_t)(((u64)(a<<16))>>16));
}

bs3l_val BS3LVM_ValWrapPointer(void *ptr, int tag)
{
	u64 t;
	t=((intptr_t)ptr);
	t=(t<<16)>>16;
	t|=((u64)tag)<<48;
	return(t);
}

BS3LVM_Object *BS3LVM_ValUnwrapObject(bs3l_val a)
{
	BS3LVM_Object *obj;
	if(!BS3LVM_ValIsObject(a))
		return(NULL);
	obj=BS3LVM_ValUnwrapPointer(a);
	return(obj);
}

char *BS3LVM_ValUnwrapString(bs3l_val a)
{
	char *str;
	if(!BS3LVM_ValIsString(a))
		return(NULL);
	str=BS3LVM_ValUnwrapPointer(a);
	return(str+1);
}

bs3l_val BS3LVM_ValWrapString(char *str)
{
	char *str1;
	int k;
	
	if(!str)
		return(0);
	
	k=BS3LVM_GetTokenForIdString(str);
	str1=BS3LVM_GetStringForToken(k);
	return(BS3LVM_ValWrapPointer(str1, bs3lvm_tag_string));
}

int BS3LVM_ValIsSmallIntTag(bs3l_val a, int tag)
{
	if((a>>60)!=1)
		return(0);
	return(((a>>48)&4095)==tag);
}

bs3l_val BS3LVM_ValWrapSmallInt(s64 val, int tag)
{
	u64 tv;
	tv=(u32)val;
	tv|=((u64)tag)<<48;
	tv|=1ULL<<60;
	return(tv);
}

int BS3LVM_ValIsSymbol(bs3l_val a)
{
	if(!bs3lvm_tag_object)
		BS3LVM_InitTags();
	return(BS3LVM_ValIsSmallIntTag(a, bs3lvm_tag_symbol));
}

bs3l_val BS3LVM_ValWrapSymbol(char *str)
{
	char *str1;
	int k;
	
	if(!str)
		return(0);
	if(	(*str=='_') ||
		((*str>='a') && (*str<='z')) ||
		((*str>='A') && (*str<='Z')))
	{
		k=BS3LVM_GetTokenForIdName(str);
	}else
		if(((*str>='0') && (*str<='9')))
	{
		k=BS3LVM_GetTokenForIdNumber(str);
	}else
	{
		k=BS3LVM_GetTokenForIdOpr(str);
	}
	return(BS3LVM_ValWrapSmallInt(k, bs3lvm_tag_symbol));
}

int BS3LVM_ValUnwrapSymbolKey(bs3l_val a)
{
	return((u16)a);
}


s64 BS3LVM_ValUnwrapFixnum(bs3l_val a)
{
	return(((s64)(a<<2))>>2);
}

bs3l_val BS3LVM_ValWrapFixnum(s64 val)
{
	u64 t;
	t=((u64)(val<<2))>>2;
	t|=1ULL<<62;
	return(t);
}

double BS3LVM_ValUnwrapFlonum(bs3l_val a)
{
	u64 t;
	double f;
	t=a<<2;
	memcpy(&f, &t, 8);
	return(f);
}

bs3l_val BS3LVM_ValWrapFlonum(double val)
{
	u64 t;
	double f;
	memcpy(&t, &f, 8);
	t=(t>>2)|(2ULL<<62);
	return(f);
}

int BS3LVM_ValIsReal(bs3l_val a)
{
	int i;
	i=(a>>62);
	return((i==1)||(i==2));
}

double BS3LVM_ValUnwrapReal(bs3l_val a)
{
	if(BS3LVM_ValIsFixnum(a))
		return(BS3LVM_ValUnwrapFixnum(a));
	if(BS3LVM_ValIsFlonum(a))
		return(BS3LVM_ValUnwrapFlonum(a));
	return(0);
}

bs3l_val BS3LVM_ValAdd(bs3l_val a, bs3l_val b)
{
	double fx, fy, fz;
	s64 lx, ly, lz;
	if(BS3LVM_ValIsFixnum(a) && BS3LVM_ValIsFixnum(b))
	{	lx=BS3LVM_ValUnwrapFixnum(a);	ly=BS3LVM_ValUnwrapFixnum(b);
		lz=lx+ly;						return(BS3LVM_ValWrapFixnum(lz));	}
	if(BS3LVM_ValIsReal(a) && BS3LVM_ValIsReal(b))
	{	fx=BS3LVM_ValUnwrapReal(a);		fy=BS3LVM_ValUnwrapReal(b);
		fz=fx+fy;						return(BS3LVM_ValWrapFlonum(fz));	}
	return(0);
}

bs3l_val BS3LVM_ValSub(bs3l_val a, bs3l_val b)
{
	double fx, fy, fz;
	s64 lx, ly, lz;
	if(BS3LVM_ValIsFixnum(a) && BS3LVM_ValIsFixnum(b))
	{	lx=BS3LVM_ValUnwrapFixnum(a);	ly=BS3LVM_ValUnwrapFixnum(b);
		lz=lx-ly;						return(BS3LVM_ValWrapFixnum(lz));	}
	if(BS3LVM_ValIsReal(a) && BS3LVM_ValIsReal(b))
	{	fx=BS3LVM_ValUnwrapReal(a);		fy=BS3LVM_ValUnwrapReal(b);
		fz=fx-fy;						return(BS3LVM_ValWrapFlonum(fz));	}
	return(0);
}

bs3l_val BS3LVM_ValMul(bs3l_val a, bs3l_val b)
{
	double fx, fy, fz;
	s64 lx, ly, lz;
	if(BS3LVM_ValIsFixnum(a) && BS3LVM_ValIsFixnum(b))
	{	lx=BS3LVM_ValUnwrapFixnum(a);	ly=BS3LVM_ValUnwrapFixnum(b);
		lz=lx*ly;						return(BS3LVM_ValWrapFixnum(lz));	}
	if(BS3LVM_ValIsReal(a) && BS3LVM_ValIsReal(b))
	{	fx=BS3LVM_ValUnwrapReal(a);		fy=BS3LVM_ValUnwrapReal(b);
		fz=fx*fy;						return(BS3LVM_ValWrapFlonum(fz));	}
	return(0);
}


bs3l_val BS3LVM_ValDiv(bs3l_val a, bs3l_val b)
{
	double fx, fy, fz;
	s64 lx, ly, lz;
	if(BS3LVM_ValIsFixnum(a) && BS3LVM_ValIsFixnum(b))
	{
		lx=BS3LVM_ValUnwrapFixnum(a);	ly=BS3LVM_ValUnwrapFixnum(b);
		if(ly==0)		return(0);
		lz=lx/ly;
		return(BS3LVM_ValWrapFixnum(lz));
	}
	if(BS3LVM_ValIsReal(a) && BS3LVM_ValIsReal(b))
	{
		fx=BS3LVM_ValUnwrapReal(a);		fy=BS3LVM_ValUnwrapReal(b);
		if(fy==0)		return(0);
		fz=fx*fy;
		return(BS3LVM_ValWrapFlonum(fz));
	}
	return(0);
}

bs3l_val BS3LVM_ValMod(bs3l_val a, bs3l_val b)
{
	double fx, fy, fz;
	s64 lx, ly, lz;
	if(BS3LVM_ValIsFixnum(a) && BS3LVM_ValIsFixnum(b))
	{
		lx=BS3LVM_ValUnwrapFixnum(a);	ly=BS3LVM_ValUnwrapFixnum(b);
		if(ly==0)		return(0);
		lz=lx%ly;
		return(BS3LVM_ValWrapFixnum(lz));
	}
	if(BS3LVM_ValIsReal(a) && BS3LVM_ValIsReal(b))
	{
		fx=BS3LVM_ValUnwrapReal(a);		fy=BS3LVM_ValUnwrapReal(b);
		if(fy==0)		return(0);
		fz=fx-(((int)(fx/fy))*fy);
		return(BS3LVM_ValWrapFlonum(fz));
	}
	return(0);
}


bs3l_val BS3LVM_ValAnd(bs3l_val a, bs3l_val b)
{
	s64 lx, ly, lz;
	if(BS3LVM_ValIsFixnum(a) && BS3LVM_ValIsFixnum(b))
	{	lx=BS3LVM_ValUnwrapFixnum(a);	ly=BS3LVM_ValUnwrapFixnum(b);
		lz=lx&ly;						return(BS3LVM_ValWrapFixnum(lz));	}
	if(BS3LVM_ValIsReal(a) && BS3LVM_ValIsReal(b))
	{	lx=BS3LVM_ValUnwrapReal(a);		ly=BS3LVM_ValUnwrapReal(b);
		lz=lx&ly;						return(BS3LVM_ValWrapFixnum(lz));	}
	return(0);
}

bs3l_val BS3LVM_ValOr(bs3l_val a, bs3l_val b)
{
	s64 lx, ly, lz;
	if(BS3LVM_ValIsFixnum(a) && BS3LVM_ValIsFixnum(b))
	{	lx=BS3LVM_ValUnwrapFixnum(a);	ly=BS3LVM_ValUnwrapFixnum(b);
		lz=lx|ly;						return(BS3LVM_ValWrapFixnum(lz));	}
	if(BS3LVM_ValIsReal(a) && BS3LVM_ValIsReal(b))
	{	lx=BS3LVM_ValUnwrapReal(a);		ly=BS3LVM_ValUnwrapReal(b);
		lz=lx|ly;						return(BS3LVM_ValWrapFixnum(lz));	}
	return(0);
}

bs3l_val BS3LVM_ValXor(bs3l_val a, bs3l_val b)
{
	s64 lx, ly, lz;
	if(BS3LVM_ValIsFixnum(a) && BS3LVM_ValIsFixnum(b))
	{	lx=BS3LVM_ValUnwrapFixnum(a);	ly=BS3LVM_ValUnwrapFixnum(b);
		lz=lx^ly;						return(BS3LVM_ValWrapFixnum(lz));	}
	if(BS3LVM_ValIsReal(a) && BS3LVM_ValIsReal(b))
	{	lx=BS3LVM_ValUnwrapReal(a);		ly=BS3LVM_ValUnwrapReal(b);
		lz=lx^ly;						return(BS3LVM_ValWrapFixnum(lz));	}
	return(0);
}

bs3l_val BS3LVM_ValShl(bs3l_val a, bs3l_val b)
{
	s64 lx, ly, lz;
	if(BS3LVM_ValIsFixnum(a) && BS3LVM_ValIsFixnum(b))
	{	lx=BS3LVM_ValUnwrapFixnum(a);	ly=BS3LVM_ValUnwrapFixnum(b);
		lz=lx<<((int)ly);				return(BS3LVM_ValWrapFixnum(lz));	}
	if(BS3LVM_ValIsReal(a) && BS3LVM_ValIsReal(b))
	{	lx=BS3LVM_ValUnwrapReal(a);		ly=BS3LVM_ValUnwrapReal(b);
		lz=lx<<((int)ly);				return(BS3LVM_ValWrapFixnum(lz));	}
	return(0);
}

bs3l_val BS3LVM_ValShr(bs3l_val a, bs3l_val b)
{
	s64 lx, ly, lz;
	if(BS3LVM_ValIsFixnum(a) && BS3LVM_ValIsFixnum(b))
	{	lx=BS3LVM_ValUnwrapFixnum(a);	ly=BS3LVM_ValUnwrapFixnum(b);
		lz=lx>>((int)ly);				return(BS3LVM_ValWrapFixnum(lz));	}
	if(BS3LVM_ValIsReal(a) && BS3LVM_ValIsReal(b))
	{	lx=BS3LVM_ValUnwrapReal(a);		ly=BS3LVM_ValUnwrapReal(b);
		lz=lx>>((int)ly);				return(BS3LVM_ValWrapFixnum(lz));	}
	return(0);
}

bs3l_val BS3LVM_ValShrr(bs3l_val a, bs3l_val b)
	{ return(BS3LVM_ValShr(a, b)); }

int BS3LVM_ValCmpTri(bs3l_val a, bs3l_val b)
{
	double fx, fy, fz;
	s64 lx, ly, lz;
	if(BS3LVM_ValIsFixnum(a) && BS3LVM_ValIsFixnum(b))
	{
		lx=BS3LVM_ValUnwrapFixnum(a);	ly=BS3LVM_ValUnwrapFixnum(b);
		if(lx==ly)	return(0);
		if(lx> ly)	return(1);
		if(lx< ly)	return(2);
	}
	if(BS3LVM_ValIsReal(a) && BS3LVM_ValIsReal(b))
	{
		fx=BS3LVM_ValUnwrapReal(a);		fy=BS3LVM_ValUnwrapReal(b);
		if(fx==fy)	return(0);
		if(fx> fy)	return(1);
		if(fx< fy)	return(2);
	}
	return(3);
}

bs3l_val BS3LVM_ValCmpEq(bs3l_val a, bs3l_val b)
{
	int m;
	m=BS3LVM_ValCmpTri(a, b);
	return(BS3LVM_ValWrapFixnum(m==0));
}

bs3l_val BS3LVM_ValCmpNe(bs3l_val a, bs3l_val b)
{
	int m;
	m=BS3LVM_ValCmpTri(a, b);
	return(BS3LVM_ValWrapFixnum(m!=0));
}

bs3l_val BS3LVM_ValCmpLt(bs3l_val a, bs3l_val b)
{
	int m;
	m=BS3LVM_ValCmpTri(a, b);
	return(BS3LVM_ValWrapFixnum(m==2));
}

bs3l_val BS3LVM_ValCmpGt(bs3l_val a, bs3l_val b)
{
	int m;
	m=BS3LVM_ValCmpTri(a, b);
	return(BS3LVM_ValWrapFixnum(m==1));
}

bs3l_val BS3LVM_ValCmpLe(bs3l_val a, bs3l_val b)
{
	int m;
	m=BS3LVM_ValCmpTri(a, b);
	return(BS3LVM_ValWrapFixnum((m==2)||(m==0)));
}

bs3l_val BS3LVM_ValCmpGe(bs3l_val a, bs3l_val b)
{
	int m;
	m=BS3LVM_ValCmpTri(a, b);
	return(BS3LVM_ValWrapFixnum((m==1)||(m==0)));
}

bs3l_val BS3LVM_ValBinary(int opn, bs3l_val a, bs3l_val b)
{
	bs3l_val c;
	switch(opn)
	{
	case BS3LVM_OPR_ADD:	c=BS3LVM_ValAdd(a, b);		break;
	case BS3LVM_OPR_SUB:	c=BS3LVM_ValSub(a, b);		break;
	case BS3LVM_OPR_MUL:	c=BS3LVM_ValMul(a, b);		break;
	case BS3LVM_OPR_DIV:	c=BS3LVM_ValDiv(a, b);		break;
	case BS3LVM_OPR_MOD:	c=BS3LVM_ValMod(a, b);		break;
	case BS3LVM_OPR_AND:	c=BS3LVM_ValAnd(a, b);		break;
	case BS3LVM_OPR_OR :	c=BS3LVM_ValOr(a, b);		break;
	case BS3LVM_OPR_XOR:	c=BS3LVM_ValXor(a, b);		break;
	case BS3LVM_OPR_SHL:	c=BS3LVM_ValShl(a, b);		break;
	case BS3LVM_OPR_SHR:	c=BS3LVM_ValShr(a, b);		break;
	case BS3LVM_OPR_SHRR:	c=BS3LVM_ValShrr(a, b);		break;

	case BS3LVM_OPR_CMPEQ:	c=BS3LVM_ValCmpEq(a, b);		break;
	case BS3LVM_OPR_CMPNE:	c=BS3LVM_ValCmpNe(a, b);		break;
	case BS3LVM_OPR_CMPLT:	c=BS3LVM_ValCmpLt(a, b);		break;
	case BS3LVM_OPR_CMPGT:	c=BS3LVM_ValCmpGt(a, b);		break;
	case BS3LVM_OPR_CMPLE:	c=BS3LVM_ValCmpLe(a, b);		break;
	case BS3LVM_OPR_CMPGE:	c=BS3LVM_ValCmpGe(a, b);		break;
	default:				c=0;	break;
	}
	return(c);
}

bs3l_val BS3LVM_ValUnary(int opn, bs3l_val a)
{
	bs3l_val c, t;
	switch(opn)
	{
	case BS3LVM_OPR_ADD:
		c=a;
		break;
	case BS3LVM_OPR_SUB:
		t=BS3LVM_ValWrapFixnum(0);
		c=BS3LVM_ValSub(t, a);
		break;
	case BS3LVM_OPR_NOT:
		t=BS3LVM_ValWrapFixnum(-1);
		c=BS3LVM_ValXor(t, a);
		break;
	case BS3LVM_OPR_LNOT:
		t=BS3LVM_ValWrapFixnum(0);
		c=BS3LVM_ValCmpEq(t, a);
		break;
	default:
		c=0;
		break;
	}
	return(c);
}


bs3l_val BS3LVM_ValAllocObj()
{
	BS3LVM_Object *obj;

	if(!bs3lvm_tag_object)
		{ BS3LVM_InitTags(); }
	obj=bs3lvm_malloc(sizeof(BS3LVM_Object));
	return(BS3LVM_ValWrapPointer(obj, bs3lvm_tag_object));
}

bs3l_val BS3LVM_ValAllocArray()
{
	BS3LVM_Object *obj;

	if(!bs3lvm_tag_object)
		{ BS3LVM_InitTags(); }
	obj=bs3lvm_malloc(sizeof(BS3LVM_Object));
	obj->n_tag=1;
	return(BS3LVM_ValWrapPointer(obj, bs3lvm_tag_object));
}

bs3l_key BS3LVM_ValObjGetTag(bs3l_val aobj)
{
	BS3LVM_Object *obj;
	obj=BS3LVM_ValUnwrapObject(aobj);
	if(!obj)
		return(0);
	return(obj->n_tag);
}

int BS3LVM_ValObjSetTag(bs3l_val aobj, bs3l_key tag)
{
	BS3LVM_Object *obj;
	obj=BS3LVM_ValUnwrapObject(aobj);
	if(!obj)
		return(0);
	obj->n_tag=tag;
	return(0);
}

bs3l_val BS3LVM_ValGetSlot(bs3l_val aobj, bs3l_key idx)
{
	BS3LVM_Object *obj;
	bs3l_val v, v1;
	int i, j, k, j1;

	if(!idx)
		return(0);

	obj=BS3LVM_ValUnwrapObject(aobj);
	if(!obj)
		return(0);
	
	if(obj->z>0)
	{
		for(i=0; i<16; i++)
		{
			j=obj->keys[i];
			v=obj->vals[i];

			if(i<15)
			{
				j1=obj->keys[i+1];
				if(j1 && (j1<=idx))
					{ i++; continue; }
			}

			if(v)
			{
				v1=BS3LVM_ValGetSlot(v, idx);
				if(v1)
					return(v1);
				continue;
			}
		}
		return(0);
	}
	
	for(i=0; i<16; i++)
	{
		j=obj->keys[i];
		v=obj->vals[i];
		if(j==idx)
			return(v);
		if(!j)
			break;
	}
	return(0);
}

int BS3LVM_ValSetSlotI(bs3l_val aobj, bs3l_key idx, bs3l_val aval)
{
	BS3LVM_Object *obj, *obj1, *obj2;
	bs3l_val v, v1, v2;
	int j1, j2;
	int i, j, k;

	if(!idx)
		return(-1);

	obj=BS3LVM_ValUnwrapObject(aobj);
	if(!obj)
		return(-1);
	
	if(obj->z>0)
	{
		for(i=0; i<16; i++)
		{
			j=obj->keys[i];
			v=obj->vals[i];
			if(i<15)
			{
				j1=obj->keys[i+1];
				if(j1 && (j1<=idx))
					{ i++; continue; }
			}
			if(v)
			{
				k=BS3LVM_ValSetSlotI(v, idx, aval);
				if(k>=0)
					return(k+(i<<(obj->z*4)));
			}
		}
		return(-1);
	}

	for(i=0; i<16; i++)
	{
		j=obj->keys[i];
		v=obj->vals[i];
		if(j==idx)
		{
			obj->vals[i]=aval;
			return(i);
		}
	}
	return(-1);
}

int BS3LVM_ValGetSlotKeyValIdx(bs3l_val aobj, int idx,
	bs3l_key *rkey, bs3l_val *rval)
{
	BS3LVM_Object *obj;
	bs3l_val v, v1;
	int i, j, k;

	if(idx<0)
		return(-1);

	obj=BS3LVM_ValUnwrapObject(aobj);
	if(!obj)
		return(-1);
	
	if(obj->z>0)
	{
		i=(idx>>(obj->z*4))&15;
		j=obj->keys[i];
		v=obj->vals[i];
		if(!v)
			return(0);
		k=BS3LVM_ValGetSlotKeyValIdx(v, idx, rkey, rval);
		return(k+(i<<(obj->z*4)));
	}

	i=(idx>>(obj->z*4))&15;
	*rkey=obj->keys[i];
	*rval=obj->vals[i];
	return(i);
}

int BS3LVM_ValSetSlotKeyValIdxI(bs3l_val aobj, int idx,
	bs3l_key akey, bs3l_val aval)
{
	BS3LVM_Object *obj;
	bs3l_val v, v1;
	int i, j, k;

	if(idx<0)
		return(-1);

	obj=BS3LVM_ValUnwrapObject(aobj);
	if(!obj)
		return(-1);
	
	if(obj->z>0)
	{
		i=(idx>>(obj->z*4))&15;
		j=obj->keys[i];
		v=obj->vals[i];
		if(!v)
			return(0);
		k=BS3LVM_ValSetSlotKeyValIdx(v, idx, akey, aval);
		obj->keys[i]=BS3LVM_ValGetSlotKeyIdx(v, 0);
		return(k+(i<<(obj->z*4)));
	}

	i=(idx>>(obj->z*4))&15;
	obj->keys[i]=akey;
	obj->vals[i]=aval;
	return(i);
}

int BS3LVM_ValSetSlotKeyValIdx(bs3l_val aobj, int idx,
	bs3l_key akey, bs3l_val aval)
{
	BS3LVM_Object *obj, *obj1;
	bs3l_val aobj1;
	int i;

	obj=BS3LVM_ValUnwrapObject(aobj);
	if(!obj)
		return(0);
	
	while(idx>=(1<<((obj->z+1)*4)))
	{
		aobj1=BS3LVM_ValAllocObj();
		obj1=BS3LVM_ValUnwrapObject(aobj1);
		
		for(i=0; i<16; i++)
		{
			obj1->keys[i]=obj->keys[i];
			obj1->vals[i]=obj->vals[i];
			obj->keys[i]=0;
			obj->vals[i]=0;
		}
		obj->keys[0]=obj1->keys[0];
		obj->vals[0]=aobj1;

		obj1->z=obj->z;
		obj->z=obj->z+1;
	}
	return(BS3LVM_ValSetSlotKeyValIdxI(aobj, idx, akey, aval));
}

bs3l_val BS3LVM_ValGetSlotValIdx(bs3l_val aobj, int idx)
{
	bs3l_key akey;
	bs3l_val aval;
	BS3LVM_ValGetSlotKeyValIdx(aobj, idx, &akey, &aval);
	return(aval);
}

bs3l_key BS3LVM_ValGetSlotKeyIdx(bs3l_val aobj, int idx)
{
	bs3l_key akey;
	bs3l_val aval;
	BS3LVM_ValGetSlotKeyValIdx(aobj, idx, &akey, &aval);
	return(akey);
}

//int BS3LVM_ValSetSlotValIdx(bs3l_val aobj, int idx, bs3l_val aval)
//{
//}

int BS3LVM_ValSetSlot(bs3l_val aobj, bs3l_key akey, bs3l_val aval)
{
	BS3LVM_Object *obj, *obj1;
	int ix, ix1;
	bs3l_key k0, k1;
	bs3l_val v0, v1;
	
	if(!akey)
		return(-1);
	
	ix=BS3LVM_ValSetSlotI(aobj, akey, aval);
	if(ix>=0)
		return(ix);
	
	obj=BS3LVM_ValUnwrapObject(aobj);
	if(!obj)
		return(0);
	
	ix=obj->nkey++;
	BS3LVM_ValSetSlotKeyValIdx(aobj, ix, akey, aval);
	
	if(!ix)
		return(0);
	
	ix1=ix-1;
	BS3LVM_ValGetSlotKeyValIdx(aobj, ix, &k0, &v0);
	BS3LVM_ValGetSlotKeyValIdx(aobj, ix1, &k1, &v1);
	while((ix>0) && (k0<k1))
	{
		BS3LVM_ValSetSlotKeyValIdx(aobj, ix, k1, v1);
		BS3LVM_ValSetSlotKeyValIdx(aobj, ix1, k0, v0);		
		ix=ix1;
		if(ix>0)
		{
			ix1=ix-1;
			BS3LVM_ValGetSlotKeyValIdx(aobj, ix1, &k1, &v1);
		}
	}
	
	return(ix);
}


int BS3LVM_ValSetSlotKName(bs3l_val aobj,
	bs3l_key *rkey, char *name, bs3l_val aval)
{
	bs3l_key key;
	key=*rkey;
	if(!key)
	{
		key=BS3LVM_GetTokenForIdName(name);
		*rkey=key;
	}
	return(BS3LVM_ValSetSlot(aobj, key, aval));
}

int BS3LVM_ValSetSlotKNameInt(bs3l_val aobj,
	bs3l_key *rkey, char *name, s64 val)
{
	return(BS3LVM_ValSetSlotKName(aobj, rkey, name,
		BS3LVM_ValWrapFixnum(val)));
}

int BS3LVM_ValSetSlotKNameFloat(bs3l_val aobj,
	bs3l_key *rkey, char *name, double val)
{
	return(BS3LVM_ValSetSlotKName(aobj, rkey, name,
		BS3LVM_ValWrapFlonum(val)));
}

int BS3LVM_ValSetSlotKNameStr(bs3l_val aobj,
	bs3l_key *rkey, char *name, char *str)
{
	return(BS3LVM_ValSetSlotKName(aobj, rkey, name,
		BS3LVM_ValWrapString(str)));
}

int BS3LVM_ValSetSlotKNameSym(bs3l_val aobj,
	bs3l_key *rkey, char *name, char *str)
{
	return(BS3LVM_ValSetSlotKName(aobj, rkey, name,
		BS3LVM_ValWrapSymbol(str)));
}

int BS3LVM_ValSetSlotTag(bs3l_val aobj, char *str)
{
	bs3l_val sym;

	sym=BS3LVM_ValWrapSymbol(str);
	BS3LVM_ValSetSlotKName(aobj, &bs3lvm_nkey_tag, "tag", sym);
	BS3LVM_ValObjSetTag(aobj, BS3LVM_ValUnwrapSymbolKey(sym));
	return(1);
}

int BS3LVM_ValSetIndex(bs3l_val aobj, int idx, bs3l_val aval)
{
	int k0;
	k0=idx|0x8000;
	BS3LVM_ValSetSlotKeyValIdx(aobj, idx, k0, aval);
	return(0);
}

bs3l_val BS3LVM_ValGetIndex(bs3l_val aobj, int idx)
{
	bs3l_val v0;
	int k0;
	k0=idx|0x8000;
	v0=BS3LVM_ValGetSlotValIdx(aobj, idx);
	return(v0);
}

int BS3LVM_ValIsTrueP(bs3l_val aobj)
{
	if(!aobj)
		return(0);
	if(!(aobj<<2))
		return(0);
	return(1);
}
