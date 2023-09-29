/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1999 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *  Produit de l'equipe ATOLL.			  *
   *                                                      *
   ******************************************************** */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030505 11:05 (phd):	Modifs pour calmer le compilo SGI	*/
/************************************************************************/
/* Mer 29 Sep 1999 16:33 (pb):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/



#include "sxunix.h"
#include "sxdico.h"

char WHAT_SXDICO_PROCESS[] = "@(#)SYNTAX - $Id: sxdico_process.c 1206 2008-03-12 13:55:40Z sagot $" WHAT_DEBUG;

static struct mot2 mot2;

static struct comb {
    SXINT			delta;
    SXINT                 size;
    SXINT			*vector;
    SXBA_INDEX          *check, *disp;
    SXBA		set, disp_set;
} comb;

static struct dico dico, empty_dico;

static SXBA_INDEX	max_x;

static SXINT 	state_top, teeth_nb, pw_nb, ps_nb; 

static SXINT	*sorted, *sorted_by_char;
static unsigned char	char2class [256], empty_char2class [256], last_class;
static  SXBA    char_set1, char_set2;

static SXBA	*automaton_signature;
static SXINT 	as_size;
static SXBA_INDEX class_nb;
static SXINT      max_lgth;

static SXINT	**automaton, *vector;

static SXBA_INDEX *state2trans_nb;
static SXINT      *sorted_by_state, start_state;


static BOOLEAN	state_by_lgth (SXINT i, SXINT j)
{
    return state2trans_nb [i] > state2trans_nb [j];
}



static BOOLEAN	prefix_is_chosen;

static BOOLEAN	less (SXINT i, SXINT j)
{
    SXINT x, y, x_min, x_max, y_min, y_max, x_char, y_char;

    x = sorted [i];
    x_min = mot2.min [x];
    x_max = mot2.max [x];

    if (x_min > x_max)
	/* mot vide */
	return TRUE;

    y = sorted [j];
    y_min = mot2.min [y];
    y_max = mot2.max [y];

    if (y_min > y_max)
	/* mot vide */
	return FALSE;

    if (prefix_is_chosen) {
	x_char = x_min;
	y_char = y_min;
    }
    else {
	x_char = x_max;
	y_char = y_max;
	
    }

    return ((unsigned char) mot2.string [x] [x_char]) <= ((unsigned char) mot2.string [y] [y_char]);
}



static VOID	sort (SXINT bi, SXINT bf)
{
    SXINT	i;

    for (i = bi; i <= bf; i++) {
	sorted_by_char [i] = i;
    }

    sort_by_tree (sorted_by_char, bi, bf, less);

    for (i = bi; i <= bf; i++) {
	sorted_by_char [i] = sorted [sorted_by_char [i]];
    }

    for (i = bi; i <= bf; i++) {
	sorted [i] = sorted_by_char [i];
    }
}

static SXBA_INDEX
put_in_comb (SXINT state)
{
    /* On cherche ds comb un emplacement vide pour inserer les transitions sortantes de state */
    /* Chaque state doit commencer ds comb a une adresse differente */
    SXBA_INDEX  xi, yi, x;
    SXUINT start; /* start >= 0 */
    SXBA	set;
    SXINT		*line;

    set = automaton_signature [state];
    xi = sxba_scan (set, -1);

    if (xi == -1) return 0;

    /* On cherche ds comb le 1er trou a` partir de xi */
    yi = xi-1;

    while ((yi = sxba_0_lrscan (comb.set, yi)) >= 0) {
	start = yi-xi;

	if (!sxba_bit_is_set (comb.disp_set, start)) {
	    /* adresse initiale non deja utilisee */
	    x = xi;

	    while ((x = sxba_scan (set, x)) >= 0 && !sxba_bit_is_set (comb.set, start+x));

	    if (x < 0) {
		/* C,a a marche' */
		line = automaton [state];
		x = start+xi;
		sxba_1_bit (comb.set, x);
		comb.vector [x] = line [xi];
		comb.check [x] = xi;

		while ((xi = sxba_scan (set, xi)) >= 0){
		    x = start+xi;
		    sxba_1_bit (comb.set, x);
		    comb.vector [x] = line [xi];
		    comb.check [x] = xi;
		}

		sxba_1_bit (comb.disp_set, start);

		if (start+last_class > dico.max)
		    dico.max = start+last_class;
    
		if (x > max_x) {
		  max_x = x;

		  if (max_x + last_class + 1 >= (SXBA_INDEX)comb.size) {
		    /* prudence pour le coup d'apres */
		    comb.size += comb.delta;
		    comb.set = sxba_resize (comb.set, comb.size+1);
		    comb.vector = (SXINT*) sxrealloc (comb.vector, comb.size+1, sizeof (SXINT));
		    comb.check = (SXBA_INDEX*) sxrealloc (comb.check, comb.size+1, sizeof (SXBA_INDEX));
		    comb.disp_set = sxba_resize (comb.disp_set, comb.size+1);
		  }
		}

		if (mot2.print_on_sxtty) fprintf (sxtty, "\010\010\010\010\010\010%-6ld", (long)++ps_nb);
		return start;
	    }
	}
    }

    return 0;
}


#if EBUG
static SXINT
unpack_kw (unsigned char *kw, SXINT kwl)
{
    /* Recherche kw ds le dico et retourne le code de kw ou 0 si kw n'existe pas */
    SXINT			base;
    unsigned char	*pbi, *pbs, class;
    BOOLEAN		is_prefix;
    SXBA		line_set;
    SXINT			*line;

    base = start_state;

    if (base == 0 || kwl < 0)
	return 0;

    if (base > 0 && base <= mot2.nb) {
	return (mot2.nb == 1 && kwl == 0) ? base : 0;
    }

    pbi = kw;
    pbs = kw+kwl-1;

    for (;;) {
	if (base > 0) {
	    base -= mot2.nb;
	    is_prefix = TRUE;
	}
	else {
	    base = -base;
	    is_prefix = FALSE;
	}

	line_set = automaton_signature [base];
	line = automaton [base];

	if (kwl == 0) {
	    if (!sxba_bit_is_set (line_set, 0))
		break;

	    base = line [0];

	    if (base > mot2.nb)
		break;

	    return base;
	}

	if ((class = char2class [(is_prefix) ? *pbi++ : *pbs--]) == 0)
	    break;

	if (!sxba_bit_is_set (line_set, class))
		break;

	base = line [class];

	if (base > 0 && base <= mot2.nb) {
	    if (kwl == 1 || mot2.process_kind == PARTIEL)
		return base;

	    break;
	}

	kwl--;
    }

    return 0;
}
#endif

static BOOLEAN
get_best (SXINT bi, SXINT bf)
{
    /* entre bi et bf, on a deja reconnu *pref_nb caracteres du prefixe et
       *suf_nb caracteres du suffixe */
    SXINT		x, kw, min, max;
    SXBA_INDEX  card1, card2;
    char	*pkw;

    sxba_empty (char_set1);
    sxba_empty (char_set2);

    for (x = bi; x <= bf; x++) {
	kw = sorted [x];
	pkw = mot2.string [kw];
	min = mot2.min [kw];
	max = mot2.max [kw];

	if (min <= max) {
	    sxba_1_bit (char_set1, (unsigned char)pkw [min]);
	    sxba_1_bit (char_set2, (unsigned char)pkw [max]);
	}
    }

    card1 = sxba_cardinal (char_set1);
    card2 = sxba_cardinal (char_set2);

    switch (mot2.optim_kind) {
    case TIME:
    case SPACE:
      /* Si SPACE, on privilegie le partage, donc le cardinal minimal */
      prefix_is_chosen = mot2.optim_kind == SPACE ? (card1 <= card2) : (card1 >= card2);
      break;

    case PREFIX_BOUND:
      prefix_is_chosen = TRUE;
      break;

    case SUFFIX_BOUND:
      prefix_is_chosen = FALSE;
      break;
    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
      sxtrap("sxdico_process","unknown switch case #1");
#endif
      break;
    }

    if (bi < bf)
	sort (bi, bf);

    return prefix_is_chosen;
}


static SXINT
get_next_state (void)
{
    SXINT	*ptr, i;

    if (++state_top >= as_size) {
	automaton_signature = sxbm_resize (automaton_signature, as_size, as_size * 2, class_nb+1);
	as_size *= 2;

	vector = (SXINT*) sxrealloc (vector, as_size*(class_nb+1), sizeof (SXINT));
	automaton = (SXINT**) sxrealloc (automaton, as_size, sizeof (SXINT*));
	automaton [0] = NULL;

	ptr = vector;

	for (i = 1; i < as_size; i++) {
	    automaton [i] = ptr;
	    ptr += class_nb+1;
	}
    }

    ptr = automaton [state_top];
	
#if 0
    for (i = 0; i <= class_nb; i++)
	*ptr++ = 0;
#endif

    return state_top;
}

static	SXINT
gen (SXINT bi, SXINT bf)
{
    SXINT			cur_state, next_state, x, y, kw, kw2, min, max, min2, max2;
    unsigned char	car, class;
    BOOLEAN		is_prefix;

    if (bi > bf)
	return 0;

    if (bi == bf) {
	kw = sorted [bi];
	min = mot2.min [kw];
	max = mot2.max [kw];

	if ((mot2.process_kind == PARTIEL) || (min > max)) {
	    ++pw_nb;
#if !EBUG
	    if (mot2.print_on_sxtty) fprintf (sxtty, "\010\010\010\010\010\010%-6ld", (long)pw_nb);
#endif
	    return kw;
	}
    }

    cur_state = get_next_state ();

    is_prefix = get_best (bi, bf);

#if EBUG
    if (!sxba_is_empty (automaton_signature [cur_state]))
	sxtrap ("dico_process", "gen");

    kw = sorted [bi];
    min = mot2.min [kw];
    max = mot2.lgth [kw] - mot2.max [kw];

    for (x = bi+1; x <= bf; x++) {
	kw = sorted [x];

	if (min != mot2.min [kw] || max != (mot2.lgth [kw] - mot2.max [kw]))
	    sxtrap ("dico_process", "gen");
    }
#endif

    for (x = bi; x <= bf; x = y) {
	kw = sorted [x];
	min = mot2.min [kw];
	max = mot2.max [kw];

	if (x == bi && min > max) {
	    ++pw_nb;
#if !EBUG
	    if (mot2.print_on_sxtty) fprintf (sxtty, "\010\010\010\010\010\010%-6ld", (long)pw_nb);
#endif
	    next_state = kw;
	    class = 0;
	    y = x+1;
	}
	else {
#if EBUG
	    if (x > bi && min > max)
		sxtrap ("dico_process", "gen");
#endif

	    if (is_prefix) {
		mot2.min [kw]++;
		car = (unsigned char)mot2.string [kw] [min];
	    }
	    else {
		mot2.max [kw]--;
		car = (unsigned char)mot2.string [kw] [max];
	    }

	    if ((class = char2class [car]) == 0) {
		/* nouvelle classe */
		char2class [car] = class = ++last_class;
	    }

	    for (y = x + 1; y <= bf; y++) {
		kw2 = sorted [y];
		min2 = mot2.min [kw2];
		max2 = mot2.max [kw2];

#if EBUG
		if (min2 > max2)
		    sxtrap ("dico_process", "gen");
#endif

		if (is_prefix) {
		    if (car != (unsigned char)mot2.string [kw2] [min2])
			break;

		    mot2.min [kw2]++;
		}
		else {
		    if (car != (unsigned char)mot2.string [kw2] [max2])
			break;

		    mot2.max [kw2]--;
		}
#if EBUG
		if (min2 < 0 || min2 >= mot2.lgth [kw2] || min2 > max2)
		    sxtrap ("dico_process", "gen");
#endif
	    }

	    next_state = gen (x, y-1);
	}
		
	teeth_nb++;
#if EBUG
	if (sxba_bit_is_set (automaton_signature [cur_state], class))
	    sxtrap ("dico_process", "gen");
#endif
	sxba_1_bit (automaton_signature [cur_state], class);
	automaton [cur_state] [class] = next_state;
    }

    return (is_prefix) ? cur_state + mot2.nb : -cur_state;
}



static void
make_dico (void)
{
    SXUINT      i;
    SXBA_INDEX          j;
    SXINT 		x, y, action;
    SXBA_INDEX          tooth;

    if (mot2.nb) {
	if (dico.max) {
	    dico.comb_vector = (SXBA_INDEX*) sxalloc (dico.max+1, sizeof (SXBA_INDEX));
	    dico.char2class = &(char2class [0]);
	}
	
	dico.partiel = mot2.process_kind == PARTIEL ? 1 : 0;

	x = 255;
	i = 256;
	j = 512;
	y = 10;

	while (x >= last_class) {
	    dico.class_mask = x;
	    dico.stop_mask = i;
	    dico.is_suffix_mask = j;
	    dico.base_shift = y;
	    x >>= 1;
	    i >>= 1;
	    j >>= 1;
	    y--;

	    if (x == 0)
		break; /* cas de la chaine vide */
	}

	if (start_state > 0) {
	    if (start_state > mot2.nb)
		dico.init_base = comb.disp [start_state-mot2.nb]<<dico.base_shift;
	    else
		dico.init_base = (mot2.code [start_state]<<dico.base_shift) + dico.stop_mask;
	}
	else
	    dico.init_base = (comb.disp [-start_state]<<dico.base_shift) + dico.is_suffix_mask;

	if (dico.max) {
	    for (i = 0; i <= dico.max; i++) {
		if (sxba_bit_is_set (comb.set, i)) {
		    action = comb.vector [i];

#if EBUG
		    if (action == 0)
			sxtrap ("dico_process", "make_dico");
#endif

		    if (action > 0) {
			if (action > mot2.nb) {
			    tooth = comb.disp [action-mot2.nb]<<dico.base_shift;
			}
			else {
			    tooth = (mot2.code [action]<<dico.base_shift) + dico.stop_mask;
			}
		    }
		    else {
			tooth = (comb.disp [-action]<<dico.base_shift) + dico.is_suffix_mask;
		    }

		    tooth += comb.check [i];
		}
		else
		    tooth = 0;

		dico.comb_vector [i] = tooth;

	    }
	}
    }
    else {
	dico.max = 0;
    }
}

void
sxdico_process (struct mot2 input, struct dico *output)
{
    SXINT 	x, i, lgth;
#if EBUG
    SXINT kwl;
    char *kw;
#endif
    char	*pkw;
    SXINT		*ptr;

    /* RAZ des scories laissees par un appel precedent eventuel */
    dico = empty_dico;
    strncpy ((char*)char2class, (char*)empty_char2class, 256);
    max_x = state_top = teeth_nb = pw_nb = ps_nb = max_lgth = 0;
    last_class = (unsigned char) 0;

    mot2 = input;

    sorted = (SXINT*) sxalloc (mot2.nb + 1, sizeof (SXINT));
    char_set1 = sxba_calloc (256);
    mot2.min [0] = mot2.max [0] = 0;

    for (x = 1; x <= mot2.nb; x++) {
	pkw = mot2.string [x];
	lgth = mot2.lgth [x];
	mot2.min [x] = 0;
	mot2.max [x] = lgth-1;

	if (lgth > max_lgth)
	    max_lgth = lgth;

	while (lgth--)
	    sxba_1_bit (char_set1, (unsigned char)*pkw++);
    }

    class_nb = sxba_cardinal (char_set1); /* grossier */

    if (mot2.print_on_sxtty)
      fprintf (sxtty, "\nProcessing of %ld words (%ld characters, %ld classes)...       ",
	       (long)mot2.nb, (long)mot2.total_lgth, (long)class_nb);

    sxba_empty (char_set1);
    
    sorted_by_char = (SXINT*) sxalloc (mot2.nb + 1, sizeof (SXINT));
    char_set2 = sxba_calloc (256);

    as_size = (2*mot2.nb)+1;
    automaton_signature = sxbm_calloc (as_size, class_nb+1);
    automaton = (SXINT**) sxalloc (as_size, sizeof (SXINT*));
    automaton [0] = NULL;
    vector = (SXINT*) sxalloc (as_size*(class_nb+1), sizeof (SXINT));

    ptr = vector;

    for (i = 1; i < as_size; i++) {
	automaton [i] = ptr;
	ptr += class_nb+1;
    }

    /* Aucun tri initial */
    for (i = 1; i <= mot2.nb; i++)
	sorted [i] = i;

    start_state = gen ((SXINT)1, mot2.nb);

    if (mot2.print_on_sxtty) fprintf (sxtty, " done\n(state_nb = %ld, last_class = %i, teeth_nb = %ld)\n",
				      (long)state_top, last_class, (long)teeth_nb);

#if EBUG
    if (mot2.print_on_sxtty) fputs ("Checking unpacked dictionary... ", sxtty);

    /* On verifie que c'est bien rentre' */
    for (i = 1; i <= mot2.nb; i++) {
	kw = mot2.string [i];
	kwl = mot2.lgth [i];

	if (unpack_kw ((unsigned char*)kw, kwl) != i) {
	    unsigned char uchar = kw [kwl];

	    kw [kwl] = NUL;
	    fprintf (sxtty, "\nERROR on keyword %ld = %s", i, kw);
	    kw [kwl] = uchar;
	}
    }

    if (mot2.print_on_sxtty) fputs ("done\n", sxtty);
#endif

    if (mot2.print_on_sxtty) fprintf (sxtty, "Automaton packing of %ld states (%ld cells)...       ",
				      (long)state_top, (long)state_top*last_class);

    state2trans_nb = (SXBA_INDEX*) sxalloc (state_top+1, sizeof (SXBA_INDEX));
    state2trans_nb [0] = 0;

    for (i = 1; i <= state_top; i++)
	state2trans_nb [i] = sxba_cardinal (automaton_signature [i]);

    sorted_by_state = (SXINT*) sxalloc (state_top+1, sizeof (SXINT));

    for (i = 1; i <= state_top; i++)
	sorted_by_state [i] = i;

    sort_by_tree (sorted_by_state, 1, state_top, state_by_lgth);

    sxfree (state2trans_nb);
    
    comb.disp = (SXBA_INDEX*) sxalloc (state_top+1, sizeof (SXBA_INDEX));

    comb.delta = (teeth_nb*10)/100+10;
    comb.size = teeth_nb + comb.delta;

    comb.set = sxba_calloc (comb.size+1);
    comb.vector = (SXINT*) sxalloc (comb.size+1, sizeof (SXINT));
    comb.check = (SXBA_INDEX*) sxalloc (comb.size+1, sizeof (SXBA_INDEX));
    comb.disp_set = sxba_calloc (comb.size+1);

    sxba_1_bit (comb.disp_set, 0); /* On interdit l'utilisation de l'index 0 de comb.vector */

    for (i = 1; i <= last_class; i++)
	comb.vector [i] = i+1;

    for (i = 1; i <= state_top; i++) {
	x = sorted_by_state [i];
	comb.disp [x] = put_in_comb (x);
    }

    sxfree (sorted_by_state);

    if (mot2.print_on_sxtty) {
	fprintf (sxtty, " in %ld cells\n", (long)dico.max);
	fprintf (sxtty, "Dictionary size = %lu bytes for %ld words of %ld characters\n",
		 (SXUINT)(256+(dico.max+1)*(sizeof (SXINT))), (long)mot2.nb, (long)mot2.total_lgth);
    }

    make_dico ();

    sxfree (sorted), sorted = NULL;
    sxfree (sorted_by_char), sorted_by_char = NULL;
    sxfree (char_set1), char_set1 = NULL;
    sxfree (char_set2), char_set2 = NULL;
    sxfree (comb.vector), comb.vector = NULL;
    sxfree (comb.check), comb.check = NULL;
    sxfree (comb.set), comb.set = NULL;

#if EBUG
    /* On verifie que c'est bien rentre' */
    if (mot2.print_on_sxtty) fputs ("Checking dictionary... ", sxtty);

    for (i = 1; i <= mot2.nb; i++) {
	kw = mot2.string [i];
	kwl = mot2.lgth [i];

	if (sxdico_get (&dico, (unsigned char**)(&(kw)), &kwl) != mot2.code [i]) {
	    unsigned char uchar = kw [kwl];

	    kw [kwl] = NUL;
	    fprintf (sxtty, "\nERROR on keyword %ld = %s", i, kw);
	    kw [kwl] = uchar;
	}
    }

    if (mot2.print_on_sxtty) fputs ("done\n", sxtty);
#endif

    *output = dico;
}

void
sxdico2c (struct dico *sxdico2c_dico, FILE *file, char *dico_name, BOOLEAN is_static)
{
  unsigned char	class;
  SXUINT i;

  fputs ("\n#include	\"sxdico.h\"\n", file);

  fprintf (file, "\nstatic SXUINT %scomb_vector [%ld] = {", dico_name == NULL ? "" : dico_name, (long)sxdico2c_dico->max+1);

  for (i = 0; i <= sxdico2c_dico->max; i++) {
    if ((i % 10) == 9)
      fputs ("\n", file);

    fprintf (file, "%ld, ", (long)sxdico2c_dico->comb_vector [i]);
  }

  fprintf (file, "\n};\nstatic unsigned char %schar2class [256] = {", dico_name == NULL ? "" : dico_name);

  for (i = 0; i <= 255; i++) {
    if ((i % 8) == 7)
      fputs ("\n", file);

    class = sxdico2c_dico->char2class [i];
    fputs ("\'", file);

    if (class == '\'')
      fputs ("\\'", file);
    else
      fprintf (file, "%s", SXCHAR_TO_STRING (class));

    fputs ("\', ", file);
  }

  fprintf (file, "\n};\n%sstruct dico %s = {\n", is_static ? "static " : "", dico_name == NULL ? "dico_hd" : dico_name);
  fprintf (file, "%ld /* max */, %ld /* init_base */, %ld /* class_mask */, %ld /* stop_mask */, %ld /* is_suffix_mask */, %ld /* base_shift */, %ld /* partiel */,\n",
	  (long)sxdico2c_dico->max, (long)sxdico2c_dico->init_base, (long)sxdico2c_dico->class_mask, (long)sxdico2c_dico->stop_mask, (long)sxdico2c_dico->is_suffix_mask, (long)sxdico2c_dico->base_shift, (long)sxdico2c_dico->partiel);
	    
  fprintf (file, "%schar2class, %scomb_vector\n};\n", dico_name == NULL ? "" : dico_name, dico_name == NULL ? "" : dico_name);
}

