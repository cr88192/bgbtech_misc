/*
AST walking interpreter.
Will not aim for speed.
 */

BS3LVM_Context *BS3LVM_AllocContext()
{
	BS3LVM_Context *ctx;
	ctx=bs3lvm_malloc(sizeof(BS3LVM_Context));
	return(ctx);
}

bs3l_val BS3LVM_Eval_LookupKey(BS3LVM_Context *ctx, bs3l_key tok)
{
	int i;
	
	if(tok<=bs3lvm_nkey_this)
	{
		if(tok==bs3lvm_nkey_null)
			return(0);
		if(tok==bs3lvm_nkey_true)
			return(BS3LVM_ValWrapFixnum(1));
		if(tok==bs3lvm_nkey_false)
			return(BS3LVM_ValWrapFixnum(0));
		if(tok==bs3lvm_nkey_this)
			return(ctx->cur_this);
	}
	
	for(i=ctx->dyn_pos-1; i>=0; i--)
	{
		if(ctx->dyn_keys[i]==tok)
			return(ctx->dyn_vals[i]);
	}
	
	return(0);
}

bs3l_val BS3LVM_Eval_AssignKey(BS3LVM_Context *ctx,
	bs3l_key tok, bs3l_val val)
{
	int i;
	
	for(i=ctx->dyn_pos-1; i>=0; i--)
	{
		if(ctx->dyn_keys[i]==tok)
		{
			ctx->dyn_vals[i]=val;
			return(1);
		}
	}
	
//	i=ctx->dyn_pos++;
//	ctx->dyn_keys[i]=tok;
//	ctx->dyn_vals[i]=val;
	return(0);
}

bs3l_val BS3LVM_Eval_BindKey(BS3LVM_Context *ctx,
	bs3l_key tok, bs3l_val val)
{
	int i, low;
	
	if(ctx->frm_pos>0)
		{ low=ctx->frm_dynidx[ctx->frm_pos-1]; }
	else
		{ low=0; }

	for(i=ctx->dyn_pos-1; i>=low; i--)
	{
		if(ctx->dyn_keys[i]==tok)
		{
			ctx->dyn_vals[i]=val;
			return(1);
		}
	}
	
	i=ctx->dyn_pos++;
	ctx->dyn_keys[i]=tok;
	ctx->dyn_vals[i]=val;
	return(1);
}

bs3l_val BS3LVM_Eval_BindNativeFunc(BS3LVM_Context *ctx,
	char *name, void *func)
{
	BS3LVM_Eval_BindKey(ctx,
		BS3LVM_ValWrapSymbol(name),
		BS3LVM_ValWrapPointer(func, 0));
	return(0);
}

bs3l_val BS3LVM_Eval_LookupSymbol(BS3LVM_Context *ctx, bs3l_val sym)
	{ return(BS3LVM_Eval_LookupKey(ctx, BS3LVM_ValUnwrapSymbolKey(sym))); }

bs3l_val BS3LVM_Eval_AssignSymbol(BS3LVM_Context *ctx,
		bs3l_val sym, bs3l_val val)
{
	bs3l_val nv0, nv1, v0, v1;
	int tgn;
	if(BS3LVM_ValIsSymbol(sym))
	{
		tgn=BS3LVM_ValUnwrapSymbolKey(sym);
		return(BS3LVM_Eval_AssignKey(ctx, tgn, val));
	}
	if(BS3LVM_Eval_NodeIsOprLoadIndex(sym))
	{
		nv0=BS3LVM_ValGetSlot(sym, bs3lvm_nkey_lnode);
		nv1=BS3LVM_ValGetSlot(sym, bs3lvm_nkey_rnode);
		v0=BS3LVM_Eval_Expr(ctx, nv0);
		v1=BS3LVM_Eval_Expr(ctx, nv1);
		tgn=BS3LVM_ValUnwrapFixnum(v1);
		BS3LVM_ValSetIndex(v0, v1, val);
		return(0);
	}
	if(BS3LVM_Eval_NodeIsOprLoadSlot(sym))
	{
		nv0=BS3LVM_ValGetSlot(sym, bs3lvm_nkey_lnode);
		nv1=BS3LVM_ValGetSlot(sym, bs3lvm_nkey_rnode);
		v0=BS3LVM_Eval_Expr(ctx, nv0);
		BS3LVM_ValSetSlot(v0, nv1, val);
		return(0);
	}

	return(0);
}

bs3l_val BS3LVM_Eval_BindSymbol(BS3LVM_Context *ctx,
		bs3l_val sym, bs3l_val val)
	{ return(BS3LVM_Eval_BindKey(ctx, BS3LVM_ValUnwrapSymbolKey(sym), val)); }

bs3l_val BS3LVM_Eval_ValBinary(bs3l_key opr, bs3l_val a, bs3l_val b)
{
	bs3l_val c;
	int opn;
	opn=bs3lvm_tok_tok2oprnum[opr];
	c=BS3LVM_ValBinary(opn, a, b);
	return(c);
}

bs3l_val BS3LVM_Eval_ValUnary(bs3l_key opr, bs3l_val a)
{
	bs3l_val c;
	int opn;
	opn=bs3lvm_tok_tok2oprnum[opr];
	c=BS3LVM_ValUnary(opn, a);
	return(c);
}

bs3l_key BS3LVM_Eval_NodeGetOpr(bs3l_val obj)
{
	bs3l_val nv2;
	bs3l_key tgo;
	nv2=BS3LVM_ValGetSlot(obj, bs3lvm_nkey_opr);
	tgo=BS3LVM_ValUnwrapSymbolKey(nv2);
	return(tgo);
}

int BS3LVM_Eval_NodeIsBinaryOpr(bs3l_val obj, bs3l_key isopr)
{
	bs3l_key tag, tgo;
	tag=BS3LVM_ValObjGetTag(obj);
	if(tag!=bs3lvm_nkey_binary)
		return(0);
	tgo=BS3LVM_Eval_NodeGetOpr(obj);
	if(tgo!=isopr)
		return(0);
	return(1);
}

int BS3LVM_Eval_NodeIsOprComma(bs3l_val obj)
	{ return(BS3LVM_Eval_NodeIsBinaryOpr(obj, bs3lvm_token_comma)); }
int BS3LVM_Eval_NodeIsOprLoadIndex(bs3l_val obj)
	{ return(BS3LVM_Eval_NodeIsBinaryOpr(obj, bs3lvm_token_lbrack)); }
int BS3LVM_Eval_NodeIsOprLoadSlot(bs3l_val obj)
	{ return(BS3LVM_Eval_NodeIsBinaryOpr(obj, bs3lvm_token_dot)); }

int BS3LVM_Eval_NodeIsAssign(bs3l_val obj)
{
	bs3l_key tag, tgo;
	tag=BS3LVM_ValObjGetTag(obj);
	if(tag!=bs3lvm_nkey_assign)
		return(0);
	return(1);
}


bs3l_val BS3LVM_Eval_MakeArray(BS3LVM_Context *ctx, bs3l_val alst)
{
	bs3l_val tmparray[512];
	bs3l_val acur, aval, anxt, tv;
	int apos, i, n;
	
	acur=alst; apos=0;
	while(acur)
	{
		if(BS3LVM_Eval_NodeIsOprComma(acur))
		{
			anxt=BS3LVM_ValGetSlot(acur, bs3lvm_nkey_lnode);
			aval=BS3LVM_ValGetSlot(acur, bs3lvm_nkey_rnode);
		}else
		{
			aval=acur;
			anxt=0;
		}
		
		tv=BS3LVM_Eval_Expr(ctx, aval);
		tmparray[apos++]=tv;

		acur=anxt;
	}
	
	aval=BS3LVM_ValAllocArray();
	n=0; i=apos-1;
	while(i>=0)
	{
		BS3LVM_ValSetIndex(aval, n++, tmparray[i--]);
	}
	return(aval);
}

bs3l_val BS3LVM_Eval_MakeObject(BS3LVM_Context *ctx, bs3l_val alst)
{
	bs3l_val acur, aval, anxt, aotmp, akey, aval1, tv;
	bs3l_key tgn;
	
	aotmp=BS3LVM_ValAllocObj();
	acur=alst;
	while(acur)
	{
		if(BS3LVM_Eval_NodeIsOprComma(acur))
		{
			anxt=BS3LVM_ValGetSlot(acur, bs3lvm_nkey_lnode);
			aval=BS3LVM_ValGetSlot(acur, bs3lvm_nkey_rnode);
		}else
		{
			aval=acur;
			anxt=0;
		}
		if(!BS3LVM_Eval_NodeIsAssign(aval))
			{ acur=anxt; continue; }
		
		akey=BS3LVM_ValGetSlot(aval, bs3lvm_nkey_lnode);
		aval1=BS3LVM_ValGetSlot(aval, bs3lvm_nkey_rnode);
		tgn=BS3LVM_ValUnwrapSymbolKey(akey);

		tv=BS3LVM_Eval_Expr(ctx, aval1);
		BS3LVM_ValSetSlot(aotmp, tgn, tv);

		acur=anxt;
	}
	return(aotmp);
}

bs3l_val BS3LVM_Eval_Call(BS3LVM_Context *ctx, bs3l_val afcn, bs3l_val alst)
{
	bs3l_val tmpaval[32];
	bs3l_val tmpakey[32];
	bs3l_val (*DoRun)(BS3LVM_Context *ctx, int n_args, bs3l_val *args);
	bs3l_val acur, aval, anxt, tv, fcn, falst, fstmt;
	int avpos, akpos, fpos, i, n;

	fcn=BS3LVM_Eval_Expr(ctx, afcn);

	falst=0;
	fstmt=0;
	DoRun=NULL;
	
	if(BS3LVM_ValIsObject(fcn))
	{
		falst=BS3LVM_ValGetSlot(fcn, bs3lvm_nkey_lnode);
		fstmt=BS3LVM_ValGetSlot(fcn, bs3lvm_nkey_rnode);
	}else
		if(BS3LVM_ValIsObjTag(fcn, 0))
	{
		DoRun=BS3LVM_ValUnwrapPointer(fcn);
	}

	acur=alst; avpos=0;
	while(acur)
	{
		if(BS3LVM_Eval_NodeIsOprComma(acur))
		{
			anxt=BS3LVM_ValGetSlot(acur, bs3lvm_nkey_lnode);
			aval=BS3LVM_ValGetSlot(acur, bs3lvm_nkey_rnode);
		}else
		{
			aval=acur;
			anxt=0;
		}
		
		tv=BS3LVM_Eval_Expr(ctx, aval);
		tmpaval[avpos++]=tv;

		acur=anxt;
	}
	
	if(DoRun)
	{
		for(i=0; i<avpos; i++)
			tmpakey[i]=tmpaval[avpos-(i+1)];
		aval=DoRun(ctx, avpos, tmpakey);
		return(aval);
	}

	acur=falst; akpos=0;
	while(acur)
	{
		if(BS3LVM_Eval_NodeIsOprComma(acur))
		{
			anxt=BS3LVM_ValGetSlot(acur, bs3lvm_nkey_lnode);
			aval=BS3LVM_ValGetSlot(acur, bs3lvm_nkey_rnode);
		}else
		{
			aval=acur;
			anxt=0;
		}
		
//		tv=BS3LVM_Eval_Expr(ctx, aval);
		tmpakey[akpos++]=aval;

		acur=anxt;
	}
	
	fpos=ctx->frm_pos++;
	ctx->frm_dynidx[fpos]=ctx->dyn_pos;
	ctx->frm_this[fpos]=ctx->cur_this;
	ctx->cur_this=0;
	ctx->cur_return=0;
	ctx->tag_break=0;

	n=akpos;
	if(avpos<n)
		n=avpos;
	for(i=0; i<n; i++)
		{ BS3LVM_Eval_BindSymbol(ctx, tmpakey[i], tmpaval[i]); }

	BS3LVM_Eval_Expr(ctx, fstmt);

	aval=ctx->cur_return;
	ctx->cur_return=0;
	ctx->tag_break=0;
	ctx->dyn_pos=ctx->frm_dynidx[fpos];
	ctx->cur_this=ctx->frm_this[fpos];
	ctx->frm_pos=fpos;

	return(aval);
}

bs3l_val BS3LVM_Eval_LoadIndex(BS3LVM_Context *ctx,
	bs3l_val aobj, bs3l_val aidx)
{
	bs3l_val obj, idx, tv;
	obj=BS3LVM_Eval_Expr(ctx, aobj);
	idx=BS3LVM_Eval_Expr(ctx, aidx);
	tv=BS3LVM_ValGetIndex(obj, BS3LVM_ValUnwrapFixnum(idx));
	return(tv);
}

bs3l_val BS3LVM_Eval_LoadSlot(BS3LVM_Context *ctx,
	bs3l_val aobj, bs3l_val aidx)
{
	bs3l_val obj, idx, tv;
	obj=BS3LVM_Eval_Expr(ctx, aobj);
//	idx=BS3LVM_Eval_Expr(ctx, aidx);
	tv=BS3LVM_ValGetSlot(obj, BS3LVM_ValUnwrapSymbolKey(aidx));
	return(tv);
}

bs3l_val BS3LVM_Eval_Node(BS3LVM_Context *ctx, bs3l_val obj)
{
	bs3l_key tag, tgo;
	bs3l_val nv0, nv1, nv2, nv3;
	bs3l_val v0, v1, v2;
	int i, j, k;

	tag=BS3LVM_ValObjGetTag(obj);
	if(tag<4)
	{
		/* JSON or something... */
		return(obj);
	}

	if(tag==bs3lvm_nkey_unary)
	{
		nv0=BS3LVM_ValGetSlot(obj, bs3lvm_nkey_value);
		tgo=BS3LVM_Eval_NodeGetOpr(obj);
		if(tgo==bs3lvm_token_lbrack)
			{ return(BS3LVM_Eval_MakeArray(ctx, nv0)); }
		if(tgo==bs3lvm_token_lbrace)
			{ return(BS3LVM_Eval_MakeObject(ctx, nv0)); }
		if((tgo==bs3lvm_token_dbladd) || (tgo==bs3lvm_token_dblsub))
		{
			k=1;
			if(tgo==bs3lvm_token_dblsub)
				k=-1;
			v0=BS3LVM_Eval_Expr(ctx, nv0);
			v1=BS3LVM_ValAdd(v0, BS3LVM_ValWrapFixnum(k));
			BS3LVM_Eval_AssignSymbol(ctx, nv0, v1);
			return(v0);
		}
		v0=BS3LVM_Eval_Expr(ctx, nv0);
		v0=BS3LVM_Eval_ValUnary(tgo, v0);
		return(v0);
	}

	if(tag==bs3lvm_nkey_binary)
	{
		nv0=BS3LVM_ValGetSlot(obj, bs3lvm_nkey_lnode);
		nv1=BS3LVM_ValGetSlot(obj, bs3lvm_nkey_rnode);
		tgo=BS3LVM_Eval_NodeGetOpr(obj);
		if(tgo==bs3lvm_token_lbrack)
			{ return(BS3LVM_Eval_LoadIndex(ctx, nv0, nv1)); }
		if(tgo==bs3lvm_token_lparen)
			{ return(BS3LVM_Eval_Call(ctx, nv0, nv1)); }
		if(tgo==bs3lvm_token_dot)
			{ return(BS3LVM_Eval_LoadSlot(ctx, nv0, nv1)); }
		v0=BS3LVM_Eval_Expr(ctx, nv0);
		v1=BS3LVM_Eval_Expr(ctx, nv1);
		v0=BS3LVM_Eval_ValBinary(tgo, v0, v1);
		return(v0);
	}

	if(tag==bs3lvm_token_kw_var)
	{
		nv0=BS3LVM_ValGetSlot(obj, bs3lvm_nkey_value);
		if(BS3LVM_Eval_NodeIsAssign(nv0))
		{
			nv1=BS3LVM_ValGetSlot(nv0, bs3lvm_nkey_lnode);
			nv2=BS3LVM_ValGetSlot(nv0, bs3lvm_nkey_rnode);
		}else
		{
			nv1=nv0;
			nv2=0;
		}
		
		v2=BS3LVM_Eval_Expr(ctx, nv2);
		BS3LVM_Eval_BindSymbol(ctx, nv1, v2);
		return(0);
	}

	if(tag==bs3lvm_nkey_assign)
	{
		nv1=BS3LVM_ValGetSlot(obj, bs3lvm_nkey_lnode);
		nv2=BS3LVM_ValGetSlot(obj, bs3lvm_nkey_rnode);
		
		v2=BS3LVM_Eval_Expr(ctx, nv2);
		BS3LVM_Eval_AssignSymbol(ctx, nv1, v2);
		return(0);
	}
	
	if(tag==bs3lvm_token_scolon)
	{
		nv0=obj; tgo=tag;
		while(tgo==bs3lvm_token_scolon)
		{
			nv1=BS3LVM_ValGetSlot(nv0, bs3lvm_nkey_lnode);
			nv2=BS3LVM_ValGetSlot(nv0, bs3lvm_nkey_rnode);
			BS3LVM_Eval_Expr(ctx, nv1);
			if(ctx->tag_break)
				return(0);
			tgo=BS3LVM_ValObjGetTag(nv2);
			nv0=nv2;
		}
		BS3LVM_Eval_Expr(ctx, nv2);
		return(0);
	}
	
	if(tag==bs3lvm_token_kw_function)
	{
		nv0=BS3LVM_ValGetSlot(obj, bs3lvm_nkey_cond);		
		BS3LVM_Eval_BindSymbol(ctx, nv0, obj);
		return(0);
	}

	if(tag==bs3lvm_token_kw_if)
	{
		nv0=BS3LVM_ValGetSlot(obj, bs3lvm_nkey_cond);
		nv1=BS3LVM_ValGetSlot(obj, bs3lvm_nkey_lnode);
		nv2=BS3LVM_ValGetSlot(obj, bs3lvm_nkey_rnode);
		v0=BS3LVM_Eval_Expr(ctx, nv0);
		if(BS3LVM_ValIsTrueP(v0))
		{
			v1=BS3LVM_Eval_Expr(ctx, nv1);
		}else
		{
			v1=0;
			if(nv2)
				v1=BS3LVM_Eval_Expr(ctx, nv2);
		}
		return(v1);
	}

	if(tag==bs3lvm_token_kw_while)
	{
		nv0=BS3LVM_ValGetSlot(obj, bs3lvm_nkey_cond);
		nv1=BS3LVM_ValGetSlot(obj, bs3lvm_nkey_lnode);
		nv2=BS3LVM_ValGetSlot(obj, bs3lvm_nkey_rnode);
		v0=BS3LVM_Eval_Expr(ctx, nv0);
		while(BS3LVM_ValIsTrueP(v0))
		{
			v1=BS3LVM_Eval_Expr(ctx, nv1);
			k=ctx->tag_break;
			if(k)
			{
				if(k==1)	break;
				if(k==2)	{ ctx->tag_break=0; break; }
				if(k==3)	{ ctx->tag_break=0; }
			}
			v0=BS3LVM_Eval_Expr(ctx, nv0);
		}
		return(v1);
	}

	if(tag==bs3lvm_token_kw_for)
	{
		nv0=BS3LVM_ValGetSlot(obj, bs3lvm_nkey_init);
		nv1=BS3LVM_ValGetSlot(obj, bs3lvm_nkey_cond);
		nv2=BS3LVM_ValGetSlot(obj, bs3lvm_nkey_step);
		nv3=BS3LVM_ValGetSlot(obj, bs3lvm_nkey_value);

		BS3LVM_Eval_Expr(ctx, nv0);
		v0=BS3LVM_Eval_Expr(ctx, nv1);
		while(BS3LVM_ValIsTrueP(v0))
		{
			v1=BS3LVM_Eval_Expr(ctx, nv3);
			k=ctx->tag_break;
			if(k)
			{
				if(k==1)	break;
				if(k==2)	{ ctx->tag_break=0; break; }
				if(k==3)	{ ctx->tag_break=0; }
			}
			BS3LVM_Eval_Expr(ctx, nv2);
			v0=BS3LVM_Eval_Expr(ctx, nv1);
		}
		return(v1);
	}

	if(tag==bs3lvm_token_kw_return)
	{
		nv0=BS3LVM_ValGetSlot(obj, bs3lvm_nkey_value);
		v0=BS3LVM_Eval_Expr(ctx, nv0);
		ctx->cur_return=v0;
		ctx->tag_break=1;
		return(0);
	}

	if(tag==bs3lvm_token_kw_break)
	{
		ctx->tag_break=2;
		return(0);
	}

	if(tag==bs3lvm_token_kw_continue)
	{
		ctx->tag_break=3;
		return(0);
	}


	return(0);
}

bs3l_val BS3LVM_Eval_Expr(BS3LVM_Context *ctx, bs3l_val val)
{
	int tok;

	if(BS3LVM_ValIsObject(val))
		{ return(BS3LVM_Eval_Node(ctx, val)); }
	if(BS3LVM_ValIsSymbol(val))
		return(BS3LVM_Eval_LookupSymbol(ctx, val));

	if(BS3LVM_ValIsFixnum(val))
		return(val);
	if(BS3LVM_ValIsFlonum(val))
		return(val);
	if(BS3LVM_ValIsString(val))
		return(val);

	/* assume some other literal value */
	return(val);
}

