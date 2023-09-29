/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2023 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://sourcesup.renater.fr/projects/syntax
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.info
 *****************************************************************************/




static SXBA			rcvr_item_set, rcvr_kernel_item_set, rcvr_t_set;
static SXBOOLEAN			is_recursive_call;
static struct sxsource_coord	error_source_index;
static SXBOOLEAN			*is_an_S_rule, *is_an_X_rule;
static struct recognize_item	empty_RT, save_RT, save_RTj;
#if is_parser
static struct parse_item	empty_PT, save_PT, save_PTj;
#endif

static SXINT	rcvr_until, X_nb, Xs;


static void
sxp_rcvr_filter (iRT, lbot, ltop)
    SXINT iRT, lbot, ltop;
{
    /* Seuls les bons terminaux sont laisses ds les tables correspondant aux X. */
    SXINT				**ptr2;
    SXINT 			a, l, jRT, item, t, k, k_max, d;
    struct recognize_item	*RTj;
    SXINT				*top_ptr, *bot_ptr;
    SXBOOLEAN			must_go_on, checked;

    if (rcvr_t_set == NULL) {
	rcvr_t_set = sxba_calloc (-tmax+1);
    }

    for (a = 0, l = lbot; l <= ltop; a++, l++) {
	if (sxplocals.rcvr.lter [l] == 0) {
	    /* X */
	    jRT = iRT+a;
	    /* On extrait de T[jRT] les terminaux possibles */
	    RTj = &(RT [jRT]);

	    save_RTj = *RTj;
#if is_parser
	    save_PTj = PT [jRT];
#endif

	    top_ptr = save_RTj.shift_NT_hd [0];
	    bot_ptr = &(save_RTj.items_stack [0]);

	    while (--top_ptr >= bot_ptr) {
		item = *top_ptr;
		t = -lispro [item];
		SXBA_1_bit (rcvr_t_set, t);
	    }
    
	    T2_items_stack = &(RTj->items_stack [0]);
	    T2_shift_NT_hd = &(RTj->shift_NT_hd [0]);
	    k_max = iRT+ltop-lbot;
	    t = 0;

	    while ((t = sxba_scan (rcvr_t_set, t)) > 0) {
		for (k = k_max; k >= jRT; k--) {
		    RT [k] = empty_RT;
#if is_parser
		    PT [k] = empty_PT;
#endif
		}

		top_ptr = save_RTj.shift_NT_hd [0];
		ptr2 = T2_shift_NT_hd;

		while (--top_ptr >= bot_ptr) {
		    item = *top_ptr;

		    if (t == -lispro [item]) {
			if (*ptr2 == NULL) {
			    /* c'est la 1ere fois qu'on rencontre une occurrence de t */
			    *ptr2 = T2_items_stack;
			}

			*((*ptr2)++) = item;
		    }
		}

		item_set_i2 = t2item_set [t];

		must_go_on = SXTRUE;
		checked = SXFALSE;
		d = 1;
		k_max = 0;

		while (must_go_on) {
		    if (must_go_on = complete (jRT+d-1)) {
			if (jRT+d > k_max)
			    k_max = jRT+d; /* La table k_max est non vide */

			/* La table T[jRT+d] est non vide et lter[l+d-1] est valide */
			if (l+d-1 == ltop) {
			    must_go_on = SXFALSE;
			    checked = SXTRUE;
			}

			if (must_go_on) {
			    item_set_i2 = t2item_set [sxplocals.rcvr.lter [l+d]];
			    d++;
			}
		    }
		}

		if (checked) {
		    /* t en position l ds lter est correct */
		    sxba_or (rcvr_item_set, item_set_i2);
		}
		else
		    SXBA_0_bit (rcvr_t_set, t);
	    }

	    for (k = k_max; k >= jRT; k--) {
		RT [k] = empty_RT;
#if is_parser
		PT [k] = empty_PT;
#endif
	    }

	    top_ptr = save_RTj.shift_NT_hd [0];
	    ptr2 = T2_shift_NT_hd;

	    while (--top_ptr >= bot_ptr) {
		item = *top_ptr;
		t = -lispro [item];

		if (SXBA_bit_is_set (rcvr_t_set, t)) {
		    if (*ptr2 == NULL) {
			*ptr2 = T2_items_stack;
		    }

		    *((*ptr2)++) = item;
		}
	    }

	    sxba_empty (rcvr_t_set);

	    item_set_i2 = rcvr_item_set;

	    for (d = 1; l+d-1 <= ltop; d++) {
		complete (jRT+d-1);
		item_set_i2 = t2item_set [sxplocals.rcvr.lter [l+d]];
	    }

	    sxba_empty (rcvr_item_set);
	}
    }
}

static SXBOOLEAN
local_correction (k)
    SXINT k;
{
    /* 0 <= k <= n */
    /* Erreur, la table Tk+1 (T2) est vide. (le symbole en erreur est ak+1). */
    /* La table Tk est correcte par rapport a ak. */
    /* Si k == 0 (erreur sur le premier token), seuls les modeles de la forme
       "0 ... " sont utilisables. */
    SXINT		        *regle;
    SXINT			i, j, l, lregle, v, h, d, k_max, x, t, iRT, lbot, ister_eof, ltop;
    SXBOOLEAN		checked = SXFALSE, must_go_on, has_eof, reject_rule;
    struct sxtoken	*p;

    /* On remplit sxplocals.rcvr.ster avec les terminaux source :
       ak ak+1 ak+2 ... ak+P_nmax */

    ister_eof = -1;

    for (j = k, i = 0; j <= k+sxplocals.SXP_tables.P_nmax; j++, i++) {
	sxplocals.rcvr.ster [i] = (j < 1 || j > n) ? -tmax : SXGET_TOKEN (j).lahead;

	if (j == n+1)
	    ister_eof = i;
    }

    /* On commence par sauver les RT, PT qui peuvent etre touches */
    save_RT = RT [k];
#if is_parser
    save_PT = PT [k];
#endif

    /* On parcourt les modeles */
    sxplocals.rcvr.nomod = 0;

    while (!checked && ++sxplocals.rcvr.nomod <=  sxplocals.SXP_tables.P_nbcart) {
	/* mise en place pour le traitement de la regle nomod */
	if (!is_an_S_rule [sxplocals.rcvr.nomod]) {
	    /* On ne considere pas les fautes d'orthographe sur les mots-cles. */
	    /* On n'active pas une regle qui "supprime" eof. */
	    regle = sxplocals.SXP_tables.P_lregle [sxplocals.rcvr.nomod];

	    if (k > 0 || regle [1] == 0) {
		lregle = regle [0];
		l = -1;
		j = 0;
		ltop = -1;
		has_eof = SXFALSE;
		reject_rule = SXFALSE;

		while (++j <= lregle) {
		    if ((v = regle [j]) < 0) {
			/* X */
			/* Si has_eof on laisse quand meme courir (le X sera un eof) !? */
			sxplocals.rcvr.lter [++l] = 0;
		    }
		    else {
			t = sxplocals.rcvr.lter [++l] = sxplocals.rcvr.ster [v];

			/* Il ne faut pas considerer le token eof no 0 de debut de chaine. */
			if (v > 0 && t == -tmax) {
			    if (!has_eof) {
				/* C'est la premiere fois qu'on rencontre eof. */
				if (v != ister_eof) {
				    /* Ca doit etre le 1er eof de la chaine source */
				    reject_rule = SXTRUE;
				    break;
				}

				ltop = l;
				has_eof = SXTRUE;
			    }
			}
		    }
		}

		if (reject_rule)
		    continue;	/* On n'examine pas ce modele */
	    
		if (ltop == -1)
		    /* Les terminaux de lter sont examines jusqu'a ltop */
		    ltop = l;

		/* On calcule iRT et lbot :
		   RT[iRT] est la table sur laquelle on lance le calcul avec le terminal lter [lbot]. */

		if (regle [1] == 0) {
		    /* correction sur le symbole en erreur */
		    /* On va lancer complete (k) avec le nouveau symbole de look-ahead lter [1] */
		    iRT = k;
		    lbot = 1;

		    if (iRT > 0) {
			/* elle a pu etre changee par un modele secondaire (ne commencant pas par 0) */
			RT [iRT] = save_RT;
#if parser
			PT [iRT] = save_PT;

#endif
		    }
		}
		else {	
		    /* correction sur le symbole precedant le symbole en erreur */
		    /* k > 0 */
		    /* On va lancer complete (k-1) avec le nouveau symbole de look-ahead lter [0] */
		    iRT = k-1;
		    lbot = 0;

		    /* Au cas ou ... */
		    RT [k] = empty_RT;
#if is_parser
		    PT [k] = empty_PT;
#endif
		}
	    
		item_set_i2 = t2item_set [sxplocals.rcvr.lter [lbot]];

		T2_shift_NT_hd = &(RT [iRT].shift_NT_hd [0]);
		T2_items_stack = &(RT [iRT].items_stack [0]);

		must_go_on = SXTRUE;

		d = 1;
		k_max = 0;

		while (must_go_on) {
		    if (must_go_on = complete (iRT+d-1)) {
			if (iRT+d > k_max)
			    k_max = iRT+d; /* La table k_max est non vide */

			/* La table T[iRT+d] est non vide et lter[lbot+d-1] est valide */
			if (lbot+d-1 == ltop) {
			    must_go_on = SXFALSE;
			    checked = SXTRUE;
			    sxplocals.atok_no = iRT+d-1; /* on fera complete (iRT+d) */
			}

			if (must_go_on) {
			    item_set_i2 = t2item_set [sxplocals.rcvr.lter [lbot+d]];
			    d++;
			}
		    }
		}

		if (!checked) {
		    /* On remet en etat! */
		    while (k_max > iRT) {
			RT [k_max] = empty_RT;
#if is_parser
			PT [k_max] = empty_PT;
#endif
			k_max--;
		    }
		}
	    }
	}
    }
    
    if (checked) {
	/* Le modele sxplocals.rcvr.nomod est valide */
	struct SXP_local_mess	*local_mess;
	char			*msg_params [5];
	struct recognize_item	*pRT;
	SXINT			im, ll, item, t;
	SXBOOLEAN			is_warning = SXTRUE;

	if (is_an_X_rule [sxplocals.rcvr.nomod])
	    sxp_rcvr_filter (iRT, lbot, ltop);

	local_mess = sxplocals.SXP_tables.SXP_local_mess + sxplocals.rcvr.nomod;

	/* preparation du message d'erreur */
	for (j = local_mess->P_param_no - 1; j >= 0; j--) {
	    struct sxtoken	*tok;
	    SXINT 		ate;

	    i = local_mess->P_param_ref [j];

	    if (i > 0) {
		tok = &SXGET_TOKEN (k+i);
		ate = tok->string_table_entry;
		msg_params [j] = ate  == SXEMPTY_STE || ate == SXERROR_STE
		    ? sxttext (sxplocals.sxtables, tok->lahead)
			: sxstrget (ate);
	    }
	    else {
		/* X */
		/* On reference la table k-i-1 */
		pRT = &(RT [k-i-1]);
		/* On cherche un symbole terminal */
		/* On prend le 1er. Un peu violent! */
		t = -lispro [pRT->items_stack [0]];

		msg_params [j] = sxttext (sxplocals.sxtables, t);

		if (sxgenericp (sxplocals.sxtables, t))
		    is_warning = SXFALSE;
	    }
	}

#if 0
	/* On doit retrouver NULL ds le token, meme si les commentaires
	   ne sont pas gardes. */
	for (y1 = 1; y1 <= lregle; y1++)
	    sxplocals.rcvr.com [y1] = NULL;

	if (!sxsvar.SXS_tables.S_are_comments_erased) {

	    /* deplacement des commentaires */
	    im = 0;

	    for (j = lregle; j > 0; j--) {
		if (regle [j] > im)
		    im = regle [j];
	    }

	    x = im;

	    for (y = lregle; regle [y] == x && x >= 0; y--) {
		sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + x--;
		sxplocals.rcvr.com [y] = SXGET_TOKEN (sxplocals.rcvr.TOK_i).comment;
	    }

	    if (lregle < ++y)
		y = lregle;

	    y1 = 1;

	    for (x1 = 0; x1 <= x && y1 < y; x1++) {
		sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + x1;
		sxplocals.rcvr.com [y1++] = SXGET_TOKEN (sxplocals.rcvr.TOK_i).comment;
	    }

	    for (x1 = x1; x1 <= x; x1++) {
		struct sxtoken	*tok;

		sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + x1;
		tok = &(SXGET_TOKEN (sxplocals.rcvr.TOK_i));

		if (sxplocals.rcvr.com [y] == NULL)
		    sxplocals.rcvr.com [y] = tok->comment;
		else if (tok->comment != NULL) {
		    /* concatenation des commentaires */
		    char	*tcom = tok->comment;
#error sxrealloc_is_not_allowed_on_token_comment
		    sxplocals.rcvr.com [y] = sxrealloc (sxplocals.rcvr.com [y], strlen (sxplocals.rcvr.com [y]) + strlen (tcom) + 1, sizeof (char));
		    strcat (sxplocals.rcvr.com [y], tcom);
		    sxfree (tcom);
		}
	    }
	}
#endif

	/* deplacement des source_index
	   principe:
	   si un token se retrouve dans la chaine corrigee, il conserve son
	   source_index, cela dans tous les cas (echange, duplication, ..)
	   s'il correspond a un remplacement, il prend le source_index du
	   token qu'il remplace
	   s'il correspond a une insertion, il prend le source_index du token
	   qui le suit.
	   */

	x = 0;
	j = sxplocals.SXP_tables.P_right_ctxt_head [sxplocals.rcvr.nomod];

	for (l = 1, p = sxplocals.rcvr.toks; (i = regle [l]) < j; l++, p++) {
	    if (i < 0) {
		p->lahead = 0;	/* On redemarre avec Any !! */
		/* p->lahead = sxplocals.rcvr.lter [l]; */
		p->string_table_entry = SXERROR_STE /* error value */ ;
		sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + x;
		p->source_index = SXGET_TOKEN (sxplocals.rcvr.TOK_i).source_index;
	    }
	    else {
		*p = (sxplocals.rcvr.TOK_i = sxplocals.rcvr.TOK_0 + i, SXGET_TOKEN (sxplocals.rcvr.TOK_i));
		x = i + 1;
	    }

	    p->comment = NULL;	/* Pour l'instant */
#if 0
	    p->comment = sxplocals.rcvr.com [l];
#endif
	    /* et voila */
	}

	sxtknmdf (sxplocals.rcvr.toks, l - 1, k+1, j);

	X_nb += l-j-1;		/* bilan du modele */
	n =  sxplocals.Mtok_no - 1;

	/* "Warning: "machin" is replaced by chose." ou similaire */

	if (!sxplocals.mode.is_silent)
	    sxerror (error_source_index,
			 is_warning ?
			 sxplocals.sxtables->err_titles [1][0] :
			 sxplocals.sxtables->err_titles [2][0],
			 local_mess->P_string,
			 is_warning ?
			 sxplocals.sxtables->err_titles [1]+1 :
			 sxplocals.sxtables->err_titles [2]+1,
			 msg_params [0],
			 msg_params [1],
			 msg_params [2],
			 msg_params [3],
			 msg_params [4]);
    }
    else {
	/* La correction a echouee */
	RT [k] = save_RT;
#if is_parser
	PT [k] = save_PT;
#endif
    }

    return checked;
}



static SXBOOLEAN
sxp_rcvr_try_table (k, l)
    SXINT k, l;
{
    /* 0 <= k < l <= n */
    /* Une erreur a ete detectee au cours du traitement de Tk. C'est-a-dire que
       la table Tk+1 est vide (l'erreur est detectee sur le terminal ak+1).
       On essaie de construire la table Tl (reprise sur le terminal al).
       La table Tl+1 pourra etre vide.
       
       Un item [A->\alpha \beta . \gamma, i] est mis dans ds Tl ssi
       A->\alpha \beta . \gamma \in t2item_set [al+1] et
       \exists j t.q. [A->\alpha . \beta \gamma, i] \in Tj
       */
    SXINT				**ptr2;
    SXBA			index_set;
    SXINT				item, it, j, Y, YY, prdct_no;

    item_set_i2 = t2item_set [SXGET_TOKEN (l).lahead];

    T2_shift_NT_hd = &(RT [l].shift_NT_hd [0]);
    T2_items_stack = &(RT [l].items_stack [0]);
    T2_index_sets = &(RT [l].index_sets [0]);
    T2_non_kernel_item_set = non_kernel_item_sets [l];

    T2_has_non_kernel = SXFALSE;

     /* rcvr_kernel_item_set = {A -> \alpha . \beta | \alpha != \epsilon et \beta != \epsilon} */
    /* Ce calcul peut etre statique */

    if (rcvr_kernel_item_set == NULL) {
	SXINT	prod, item, init_item;

	rcvr_kernel_item_set = sxba_calloc (itemmax+1);

	for (prod = 0; prod <= prodmax; prod++) {
	    item = init_item = prolon [prod];

	    while (lispro [item] != 0) {
		if (item != init_item)
		    SXBA_1_bit (rcvr_kernel_item_set, item);

		item++;
	    }
	}
    }
	   
    AND3 (rcvr_item_set, rcvr_kernel_item_set, item_set_i2);
    /* rcvr_item_set = {A -> \alpha . \beta | \alpha != \epsilon et \beta =>* al ...} */
    
    item = -1;
	   
    while ((item = sxba_scan (rcvr_item_set, item)) >= 0) {
	Y = lispro [item];
	   
	if (Y < 0) {
#if is_prdct 
	    if (is_prdct_fun &&
		(prdct_no = prdct [item]) != -1 &&
		!(*for_semact.prdct) (1 /* No de passe : reconnaisseur */,
				      item, l, 0 /* inutilise */, prdct_no))
	    {
		/*le predicat a echoue */
		continue;
	    }
#endif
	    YY = 0;
	}
	else {
	    YY = Y;
	}

	it = prolon [prolis [item]];
	index_set = T2_index_sets [item];

	for (j = 0; j <= k; j++) {
	    if (SXBA_bit_is_set (non_kernel_item_sets [j], it)) {
		if (index_set == NULL) {
		    if (*(ptr2 = &(T2_shift_NT_hd [YY])) == NULL) {
			/* c'est la 1ere fois qu'on rencontre YY */
			/* rhs_nt2where [0] == 0 (pour les terminaux) */
			*ptr2 = &(T2_items_stack [rhs_nt2where [YY]]);

			if (Y > 0) {
			    T2_has_non_kernel = SXTRUE;
			    sxba_or (T2_non_kernel_item_set, nt2item_set [Y]);
			}
		    }

		    *((*ptr2)++) = item;

		    index_set = T2_index_sets [item] = bag_get (&shift_bag, l+1);
		}

		SXBA_1_bit (index_set, j);
	    }
	}
    }

    if (T2_has_non_kernel)
	process_non_kernel (l);

    return T2_shift_NT_hd [0] != NULL;
}


static void
sxp_error_recovery (k)
    SXINT k;
{
    /* 0 <= k <= n */
    /* Erreur, la table Tk+1 (T2) est vide. (le symbole en erreur est ak+1). */
    /* La table Tk est correcte par rapport a ak. */
    /* Si k == 0 (erreur sur le premier token), seuls les modeles de la forme
       "0 ... " sont utilisables. */
    struct sxtoken		*p;
    char			*msg_params [5];
    SXINT				l, ate;

    /* k == sxplocals.atok_no */
    error_source_index = SXGET_TOKEN (k+1).source_index;
    sxplocals.rcvr.TOK_0 = k; /* symbole precedant le symbole en erreur */

    /* Correction locale */
    if (X_nb /* Nb total d'insertions-suppression des corrections precedentes */
	+ Xs /* Nb max de X pour les modeles */
	<= delta_n /* Marge que l'on s'est donnee */
	&& local_correction (k) /* Il y a correction locale */)
	return;

    /* Pour l'instant, on ne valide pas la recuperation globale avec du look-ahead. */

    /* Si on est ds le cas "is_parser", il faut l'inhiber. */

    /* "Error: Global Recovery." */
    if (sxplocals.mode.is_silent)
	sxerrmngr.nbmess [2]++; /* error nb */
    else
	sxerror (error_source_index,
		     sxplocals.sxtables->err_titles [2][0],
		     sxplocals.SXP_tables.P_global_mess [sxplocals.SXP_tables.P_followers_number],
		     sxplocals.sxtables->err_titles [2]+1);
    /* Recuperation globale */

    if (k >= n) {
	/* Erreur sur eof */
	l = n+1;
    }
    else {
	for (l = k+1; l <= n; l++) {
	    if (sxp_rcvr_try_table (k, l))
		/* recuperation sur al */
		break;
	}
    }

    /* Le prochain complete doit s'effectuer sur Tl, or la premiere instruction
       qui s'execute au retour ds recognize est ++sxplocals.atok_no, on en tient compte. */
    sxplocals.atok_no = l-1;
    p = &(SXGET_TOKEN (l));

    if (l == n+1) {
	/* on a trouve la fin du fichier */
	/* "Error: Parsing stops on End of File." */
	if (!sxplocals.mode.is_silent)
	    sxerror (p->source_index,
		         sxplocals.sxtables->err_titles [2][0],
			 sxplocals.SXP_tables.P_global_mess
			 [sxplocals.SXP_tables.P_followers_number + 3],
			 sxplocals.sxtables->err_titles [2]+1);
    }
    else {

	msg_params [0] = (ate = p->string_table_entry) == SXEMPTY_STE || ate == SXERROR_STE
	    ? sxttext (sxplocals.sxtables, p->lahead)
		: sxstrget (ate);

	/* "Warning: Parsing resumes on "machin"." */
	if (!sxplocals.mode.is_silent)
	    sxerror (p->source_index,
		     sxplocals.sxtables->err_titles [1][0],
		     sxplocals.SXP_tables.P_global_mess
		     [sxplocals.SXP_tables.P_followers_number + 1],
		     sxplocals.sxtables->err_titles [1]+1,
		     msg_params [0]);
    }
}



SXBOOLEAN
sxp_rcvr_earley_lc (what_to_do, i)
    SXINT		what_to_do, i;
{
    /* La table Ti+1 est vide. */
    SXINT	v;

    switch (what_to_do) {
    case SXOPEN:
	break;

    case SXACTION:
	if (is_recursive_call)
	    return SXFALSE;

	is_recursive_call = SXTRUE;

	if (rcvr_item_set == NULL) {
	    SXINT 	nomod, l, i, *regle;

	    rcvr_item_set = sxba_calloc (itemmax+1);
	    is_an_S_rule = (SXBOOLEAN*) sxcalloc (sxplocals.SXP_tables.P_nbcart+1, sizeof (SXBOOLEAN));
	    is_an_X_rule = (SXBOOLEAN*) sxcalloc (sxplocals.SXP_tables.P_nbcart+1, sizeof (SXBOOLEAN));

	    for (nomod = 1; nomod <= sxplocals.SXP_tables.P_nbcart; nomod++) {
		regle = sxplocals.SXP_tables.P_lregle [nomod];
		l = regle [0];

		for (i = 1; i <= l; i++) {
		    if ((v = regle [i]) == -2 /* S */) {
			if (v == -2)
			     /* S */
			    is_an_S_rule [nomod] = SXTRUE;
			else /* v == -1 */
			    /* X */
			    is_an_X_rule [nomod] = SXTRUE;

			break;
		    }
		}

		l -= regle [l];

		/* la plus grande augmentation de longueur pour une regle */
		if (l > Xs)
		    Xs = l;
	    }

	    sxplocals.rcvr.toks = (struct sxtoken*) sxalloc (sxplocals.SXP_tables.P_sizofpts + 1,
							     sizeof (struct sxtoken));
	    sxplocals.rcvr.ster = (SXINT*) sxalloc (sxplocals.SXP_tables.P_nmax + 4, sizeof (SXINT));
	    sxplocals.rcvr.lter = (SXINT*) sxalloc (sxplocals.SXP_tables.P_maxlregle + 2, sizeof (SXINT));
	}

	sxp_error_recovery (i);

	is_recursive_call = SXFALSE;
	return SXTRUE;

    case SXCLOSE:
	if (rcvr_item_set != NULL) {
	    sxfree (rcvr_item_set),  rcvr_item_set = NULL;
	    sxfree (is_an_S_rule);
	    sxfree (is_an_X_rule);

	    if (rcvr_kernel_item_set != NULL)
		sxfree (rcvr_kernel_item_set), rcvr_kernel_item_set = NULL;

	    sxfree (sxplocals.rcvr.toks);
	    sxfree (sxplocals.rcvr.ster);
	    sxfree (sxplocals.rcvr.lter);

	    if (rcvr_t_set != NULL)
		sxfree (rcvr_t_set);
	}
	
	break;

    default:
	fprintf (sxstderr, "The function \"sxp_rcvr_earley_lc\" is out of date with respect to its specification.\n");
	abort ();
    }

    return SXTRUE;
}
