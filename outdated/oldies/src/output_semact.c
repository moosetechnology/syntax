/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 2005 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */

/* Actions semantiques permettant la sortie de diverses vues du résultat de l'analyse Earley et/ou LFG et/ou chunks et/ou nbest */

/* ********************************************************
   *                                                      *
   *          Produit de l'equipe ATOLL.                  *
   *                                                      *
   ******************************************************** */

/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 26-10-05 11:30 (pb&an):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

static char	ME [] = "output_semact";

/* Les valeurs atomiques sont qcq et on peut faire de la disjonction sur les "..." */
#define ESSAI

#include "sxunix.h"
#include "earley.h"
#include "XH.h"
#include "varstr.h"
#include "sxstack.h"
#include <math.h>
#include <float.h>

char WHAT_OUTPUT_SEMACT[] = "@(#)SYNTAX - $Id: output_semact.c 3678 2024-02-06 08:38:24Z garavel $" WHAT_DEBUG;

#ifdef ESSAI
/* Pour atom_id2local_atom_id_hd */
#include "X.h"
#endif /* ESSAI */


#ifdef lfg_h

#define def_prod2rule

#define def_ranks
#define def_rule2next_rule

#include lfg_h
#endif /* lfg_h */


#ifdef lex_lfg_h

#define def_amalgam_list
#define def_amalgam_names
#define def_atom_id2string

#define def_compound_field
#define def_compound_field_list

#define def_field_id2atom_field_id
#define def_field_id2kind
#define def_field_id2string

#define def_if_id2t_list
#define def_inflected_form2display

#define def_lexeme_id2string

#define def_sous_cat
#define def_sous_cat_list

#define def_X_amalgam_if_id_hd

#include lex_lfg_h
#endif /* lex_lfg_h*/


#include td_h

#include "lfg_semact.h"
/* #include "snt_semact.h" */
#include "dag_scanner.h"

static char             *output_kind;
static bool          is_print_headers;
static bool          is_print_xml_headers;

static int              *spf_count;
static int              *Pij2out_sentence, *Aij2out_sentence, *Tij2tok_no;
static int              rec_level, last_f;
#define SENTENCE_CONCAT 0X80000000
#define SENTENCE_OR     0X40000000
static XH_header        out_sentence_hd;
static int              maxxt; /* nb des terminaux instancies Tij, y compris ceux couverts par les nt de la rcvr == -spf.outputG.maxt */
static bool          nl_done;

static FILE    *weights_file;
static char    *weights_file_name;
FILE    *xml_file;
static char    *xml_file_name;
static FILE    *odag_file;
static char    *odag_file_name;

static char    compact_infos[32];

#include "sxword.h"

#if 0
/* obsolete */
static SXBA_CST_ALLOC         (local_arg_set, MAX_FIELD_ID+1);
#endif /* 0 */

static int           Ei_lgth;

#define FS_ID_SHIFT     (STRUCTURE_NAME_shift+MAX_FIELD_ID_SHIFT)

#define FIELD_AND       (~((~0)<<MAX_FIELD_ID_SHIFT))
#define KIND_AND        (~((~0)<<STRUCTURE_NAME_shift))
#define FS_ID_AND        (~((~0)<<FS_ID_SHIFT))

#define Tpq2str(Tpq)  (Tpq ? (spf.inputG.tstring [-Tij2T (Tpq-spf.outputG.maxxnt)]) : NULL)

extern int            sentence_id; /* nbest peut en avoir besoin */
/* extern char           *sntid2string[MAX_SNT_ID+1]; */
extern char           output_sem_pass_arg;

/* valeur par defaut */

static char	Usage [] = "\
\t\t-xml pathname,\n\
\t\t-odag pathname,\n\
\t\t-wf pathname, -weights_file pathname,\n\
\t\t-ok kind, -output_kind kind,\n\
";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 	   0
#define XML                1
#define WEIGHTS            2
#define OUTPUT_KIND        3
#define PRINT_HEADERS      4
#define PRINT_XML_HEADER   5
#define ODAG               6

static char	*option_tbl [] = {
    "",
    "xml",
    "wf", "weights_file",
    "ok","output_kind",
    "ph","print_headers",
    "pxh","print_xml_header",
    "odag",
};

static int	option_kind [] = {
    UNKNOWN_ARG,
    XML,
    WEIGHTS, WEIGHTS,
    OUTPUT_KIND,OUTPUT_KIND,
    PRINT_HEADERS, PRINT_HEADERS,
    PRINT_XML_HEADER, PRINT_XML_HEADER,
    ODAG,
};

static int	option_get_kind (arg)
    char	*arg;
{
  char	**opt;

  if (*arg++ != '-')
    return UNKNOWN_ARG;

  if (*arg == SXNUL)
    return UNKNOWN_ARG;

  for (opt = &(option_tbl [OPT_NB]); opt > option_tbl; opt--) {
    if (strcmp (*opt, arg) == 0 /* egalite */ )
      break;
  }

  return option_kind [opt - option_tbl];
}



static char	*option_get_text (kind)
    int	kind;
{
  int	i;

  for (i = OPT_NB; i > 0; i--) {
    if (option_kind [i] == kind)
      break;
  }

  return option_tbl [i];
}


/* retourne le ME */
static char*
output_ME ()
{
  return ME;
}

/* retourne la chaine des arguments possibles propres a output */
static char*
output_args_usage ()
{
  return Usage;
}

/* decode les arguments specifiques a output */
/* l'option argv [*parg_num] est inconnue du parseur earley */
static bool
output_args_decode (pargnum, argc, argv)
     int  *pargnum, argc;
     char *argv [];
{
  switch (option_get_kind (argv [*pargnum])) {

  case XML:
    if (++*pargnum >= argc) {
      fprintf (sxstderr, "%s: a pathname must follow the \"%s\" option;\n", ME, option_get_text (XML));
      return false;
    }
    xml_file_name = argv [*pargnum];
    
    break;

  case ODAG:
    if (++*pargnum >= argc) {
      fprintf (sxstderr, "%s: a pathname must follow the \"%s\" option;\n", ME, option_get_text (ODAG));
      return false;
    }
    odag_file_name = argv [*pargnum];
    
    break;

  case WEIGHTS:
    if (++*pargnum >= argc) {
      fprintf (sxstderr, "%s: a pathname must follow the \"%s\" option;\n", ME, option_get_text (WEIGHTS));
      return false;
    }
	      
    weights_file_name = argv [*pargnum];

    break;

  case OUTPUT_KIND:
    if (++*pargnum >= argc) {
      fprintf (sxstderr, "%s: an output kind specification string must follow the \"%s\" option;\n", ME, option_get_text (OUTPUT_KIND));
      return false;
    }

    output_kind = argv [*pargnum];
    break;

  case PRINT_HEADERS:
    is_print_headers = true;

  case PRINT_XML_HEADER:
    is_print_xml_headers = true;

  case UNKNOWN_ARG:
    return false;
  }

  return true;
}



static char*
ntstring2propname(ntstring)
     char* ntstring;
{
  if (!strcmp(ntstring,"WS")) {
    return("completive");
  } else if (!strcmp(ntstring,"Prop_INTD")
	     || !strcmp(ntstring,"Prop_COMPLETIVE")
	     || !strcmp(ntstring,"Prop_COMPLETIVE_A")
	     || !strcmp(ntstring,"Prop_COMPLETIVE_DE")) {
    return("completive");
  } else if (!strcmp(ntstring,"Prop_CIRC")) {
    return("circonstancielle");
  } else if (!strncmp(ntstring,"Prop_INTI",9)) {
    return("interrogative_indirecte");
  } else if (!strcmp(ntstring,"Prop_RELATIVE")) {
    return("relative");
  }
  return SXNUL;
}

static char*
ntstring2phrasename(ntstring)
     char* ntstring;
{
  if (!strcmp(ntstring,"NP")
      || !strcmp(ntstring,"NumP")
      || !strcmp(ntstring,"AP")
      || !strcmp(ntstring,"PP")
      || !strcmp(ntstring,"VP")
      || !strcmp(ntstring,"AdvP")
      )
    return(ntstring);
  return SXNUL;
}

static int cur_snt_id;

/* parcours top-down y compris les terminaux: boucle récursive */
static void
spf_td_walk_incl_terminals (Aij, ntstring2name, ntkind, ntkindshort)
     int Aij;
     char* (*ntstring2name)();
     char *ntkind, *ntkindshort;
{
  int              output_prod, output_item, Xpq, hook, i, tok_no, k;
  int              Aij_val, local_f;
  char             *ntstring, *comment, *str1, *str2;

  if ((Aij_val = spf_count [Aij]) == -1) {
    /* 1ere visite de Aij */
    Aij_val = spf_count [Aij] = 0;
    ntstring = spf.inputG.ntstring[Aij2A (Aij)];

    ntstring=(*ntstring2name) (ntstring);

    if (ntstring != SXNUL && Aij2i (Aij) < Aij2j (Aij)) {
      if (!nl_done) {
	fprintf(xml_file,"\n");
	nl_done=true;
      }
      for (i = 0; i <= rec_level; i++)
	fprintf(xml_file,"  ");
      fprintf(xml_file,"<%s type=\"%s\" id=\"E%i%s%i\">", ntkind, ntstring, sentence_id, ntkindshort, cur_snt_id++);
      rec_level++;
      nl_done=false;
    }
    
    hook = spf.outputG.lhs [spf.outputG.maxxprod+Aij].prolon;
    
    while ((output_prod = spf.outputG.rhs [hook++].lispro) != 0) {
      /* On parcourt les productions dont la lhs est Aij */
      if (output_prod > 0) { /* et qui ne sont pas filtrées */
	output_item = spf.outputG.lhs [output_prod].prolon;
	Aij_val = 1;
	
	while ((Xpq = spf.outputG.rhs [output_item++].lispro) != 0) {
	  /* On parcourt la RHS de la  prod instanciee output_prod */
	  if (Xpq <= spf.outputG.maxnt) {
	    //	    fprintf(xml_file,"(%i <? %i)\n",Xpq,spf.outputG.maxnt);
	    if (Xpq > 0) {
	      /* nt */
	      spf_td_walk_incl_terminals(Xpq, ntstring2name, ntkind, ntkindshort);
	    }
	    else {
	      /* terminal*/
	      if (!nl_done)
		fputs("\n",xml_file);
	      nl_done=true;
	      tok_no=spf.outputG.Tij2tok_no [-Xpq];

	      comment = toks_buf [tok_no].comment;

	      if (comment) {
		comment++;
		for (comment; *comment != SXNUL; comment++) {
		  str1 = strchr(comment,(int)'F');
		  str2 = strchr(str1+3,(int)'F');
		  str1 /* str3 */ = strchr(str2,(int)'"');
		  *str1=SXNUL;
		  local_f=atoi(str2+1);
		  *str1='"';
		  if (local_f > last_f) {
		    last_f=local_f;
		    if (!nl_done)
		      fputs("\n",xml_file);
		    nl_done=true;
		    for (i = 0; i <= rec_level; i++)
		      fprintf(xml_file,"  ");
		    for (comment; *comment != '}'; comment++){
		      fprintf(xml_file,"%c",*comment);
		      nl_done=false;
		    }
		  } else {
		    comment = strchr(comment,(int)'>');
		    comment = strchr(comment+1,(int)'>');
		    comment+=2;
		  }
		}
	      } else { // pas de commentaires: on sort la ff dans un commentaire XML
		fprintf(xml_file,"<!-- %s -->\n",sxstrget (toks_buf [tok_no].string_table_entry),tok_no,-Xpq);
	      }
	    }
	  }
	}
      }
    }
    if (ntstring != SXNUL && Aij2i (Aij) < Aij2j (Aij)) {
      rec_level--;
      if (!nl_done)
	fprintf(xml_file,"\n");
      nl_done=true;
      for (i = 0; i <= rec_level; i++)
	fprintf(xml_file,"  ");
      fprintf(xml_file,"</%s>\n",ntkind);
    }
    spf_count [Aij] = Aij_val;
  }
}

static void
spf_output_partial_forest (ntstring2name, ntkind, ntkindshort)
     char* (*ntstring2name)();
     char *ntkind, *ntkindshort;
{
  int i;

  if (spf.outputG.start_symbol != 0) {
    spf_count = (int*) sxalloc (spf.outputG.maxxnt+1, sizeof (int));
    for (i=0;i<=spf.outputG.maxxnt;i++)
      spf_count[i]=-1;

    cur_snt_id = 1;
    spf_td_walk_incl_terminals (spf.outputG.start_symbol,ntstring2name, ntkind, ntkindshort);

    sxfree (spf_count), spf_count = NULL;
  }
  if (!nl_done)
    fprintf(xml_file,"\n");

}









#ifndef no_lfg

/* Start of the weights_campaign */
static int *signature_local_stack;

static bool
push_in_signature_local_stack (int elem, int level)
{
#if 0
  /* On sort tous les niveaux, c'est weights2proba qui tronquera eventuellement */
  if (elem == 0 /* appel pour verification */ && MAX_SIGNATURE_DEPTH > 0 /* == 0 => profondeur non bornee */ && level > MAX_SIGNATURE_DEPTH)
    return false;
#endif /* 0 */

  if (elem != 0)	
    DPUSH (signature_local_stack, elem);

  return true;
}

/* Calcule et sort les signatures des f_structures associees a Ppq */
static void
print_fs_signatures (int Ppq)
{  
  int        i, nb, fs_id, head, Aij, rule, x, size, val, sign_nb;
  int        *local_valid_heads_stack;
  SXBA_ELT   mot;
  struct lhs *plhs;
    
  i = Pij2disp [Ppq];

  if (i == 0)
    return;

  local_valid_heads_stack = fs_id_Pij_dstack + i;
    
  plhs = spf.outputG.lhs+Ppq;
  Aij = plhs->lhs;

#if 0
  size = LOCAL_ARG_SET_SIZE;
#if SXBITS_PER_LONG == 64
  size *= 2;
#endif /* SXBITS_PER_LONG == 64 */
#endif /* 0 */
  
  for (rule = prod2rule [plhs->init_prod]; rule != 0; rule = rule2next_rule [rule]) {
    nb = local_valid_heads_stack [0];

    if (nb) {
      i = 1;

      while (i <= nb) {
	head = local_valid_heads_stack [i];
	fs_id = (head > 0) ? XxY_Y (heads, head) : XxY_Y (heads, -head);
    
	/* calcul de la signature de fs_id */
	set_fs_id_signature (fs_id, 1, push_in_signature_local_stack);

	sign_nb = DTOP (signature_local_stack);

	/* non vide */
	fprintf (weights_file, "0 0 0 %i %i %i", spf.outputG.lhs [Ppq].init_prod, spf.outputG.Pij2eval [Ppq], sign_nb);
#if LOG
	printf ("0 0 0 %i %i %i", spf.outputG.lhs [Ppq].init_prod, spf.outputG.Pij2eval [Ppq], sign_nb);
#endif /* LOG */

	for (x = 1; x <= sign_nb; x++) {
	  val = signature_local_stack [x];

	  fprintf (weights_file, " %i", val);
#if LOG
	  printf (" %i", val);
#endif /* LOG */
	}

	DTOP (signature_local_stack) = 0;

	fputs ("\n", weights_file);
#if LOG
	putchar ('\n');
#endif /* LOG */

	/* sauter le doldol */
	i += 2;
      }
    }

    local_valid_heads_stack += nb+1;
  }
}


static int
weights_bu_walk (Pij)
     int Pij;
{
  int                 init_prod, A, item, X, val, Xpq, hook, Ppq, pos;
  int                 Tpq, tok_no, ff_id, cur, bot, top, t;
  char                *ranking, cur_rank;
  struct if_id2t_list *ptr;

  /* spf.outputG.Pij2eval a ete rempli par spf_fill_Pij2eval () */
  val = spf.outputG.Pij2eval [Pij];
 
  init_prod = spf.outputG.lhs [Pij].init_prod;
  A = Aij2A (spf.walk.cur_Aij);

  pos = 0;
  item = spf.outputG.lhs [Pij].prolon;

  while ((Xpq = spf.outputG.rhs [item].lispro) != 0) {
    if (Xpq > 0) {
      /* On parcourt les Xpq-prods ... */
      hook = spf.outputG.lhs [spf.outputG.maxxprod+Xpq].prolon;

      while ((Ppq = spf.outputG.rhs [hook++].lispro) != 0) {
	if (Ppq > 0) {
	  /* ... valides */
	  fprintf (weights_file, "%i %i %i %i %i 0\n", init_prod, val, pos, spf.outputG.lhs [Ppq].init_prod, spf.outputG.Pij2eval [Ppq]);
#if LOG
	  printf ("%i %i %i %i %i 0\n", init_prod, val, pos, spf.outputG.lhs [Ppq].init_prod, spf.outputG.Pij2eval [Ppq]);
#endif /* LOG */
	}
      }
    }
    else {
      Tpq = -Xpq;
      tok_no = spf.outputG.Tij2tok_no [Tpq];
      ff_id = toks_buf [tok_no].lahead;

      bot = inflected_form2display [ff_id];
      top = inflected_form2display [ff_id+1];

      if (top-bot > 1) {
	/* f_structures multiples, il peut donc y avoir plusieurs terminaux */
	t = if_id2t_list [bot].t;

	for (cur = bot+1; cur < top; cur++) {
	  if (if_id2t_list [cur].t != t)
	    /* multiples */
	    break;
	}

	if (cur < top) {
	  /* ff ambigue, on la sort */
	  t = -Tij2T (Tpq);

	  fprintf (weights_file, "%i %i %i %i %i %i\n", init_prod, val, pos, spf.inputG.maxprod+1 /* identifiant des "Tpq" */, ff_id, t);
#if LOG
	  printf ("%i %i %i %i %i %i\n", init_prod, val, pos, spf.inputG.maxprod+1 /* identifiant des "Tpq" */, ff_id, t);
#endif /* LOG */
	}
      }
    }

    pos++;
    item++;
  }

  if (A == 1) {
    /* axiome */
    fprintf (weights_file, "0 0 0 %i %i 0\n", init_prod, val);
#if LOG
    printf ("0 0 0 %i %i 0\n", init_prod, val);
#endif /* LOG */
  }

  if (ranking = ranks [A]) {
    while ((cur_rank = *ranking++) != SXNUL) {
      if (cur_rank == 'p')
	break;
    }
	
    if (cur_rank == 'p') {
      print_fs_signatures (Pij);
    }
  }

  return 1;
}

/* On met a jour le fichier qui contient pour chaque production du squelette son nombre d'occurrences */
static void
weights_campaign ()
{
  /*
    SYNOPSIS

    int fgetc(FILE *stream);
    char *fgets(char *s, int size, FILE *stream); 
    int ungetc(int c, FILE *stream);

    DESCRIPTION
    fgetc() reads the next character from  stream  and  returns  it  as  an
    unsigned char cast to an int, or EOF on end of file or error.

    fgets() reads in at most one less than size characters from stream  and
    stores  them  into  the buffer pointed to by s.  Reading stops after an
    EOF or a newline.  If a newline is read, it is stored into the  buffer.
    A '\0' is stored after the last character in the buffer.

    ungetc()  pushes  c  back to stream, cast to unsigned char, where it is
    available for subsequent read operations.  Pushed-back characters  will
    be returned in reverse order; only one pushback is guaranteed.
  */

#include <stdio.h>

#if LOG
  fputs ("\n************** weights_campaign **************\n", stdout);
#endif /* LOG */

  weights_file = fopen (weights_file_name, "a");

  if (weights_file == NULL)
    sxtrap (ME, "weights_campaign (open weights file)");

  DALLOC_STACK (signature_local_stack, 100);

  /* Pour compter le nb de phrases traitees */
  fputs ("0 0 0 0 0 0\n", weights_file);

#if LOG
  fputs ("pd = weights_bu_walk\n", stdout);
#endif /* LOG */

  spf_fill_Pij2eval (); /* remplit spf.outputG.Pij2eval si ce n'est deja fait */

  spf_topological_walk (spf.outputG.start_symbol, NULL, weights_bu_walk);

  /* et on les ferme */
  fclose (weights_file), weights_file = NULL;

#if LOG
  fputs ("\n\n************** weights_campaign (end) **************\n", stdout);
#endif /* LOG */

  DFREE_STACK (signature_local_stack);
}
/* End of the weights_campaign */

#endif /* !no_lfg */


/* On extrait le no de la phrase source a partir des commentaires */
/* Peut etre appele' depuis nbest */
int
easy_get_sentence_no ()
{
  int  sentence_no = 0;
  char *str1, *str2, *comment;

  comment = toks_buf [1 /* tok_no, on prend le 1er */].comment;

  if (comment ) {
    str1 = strchr (comment+1, (int)'"');
	
    if (str1) {
      if (*++str1 == 'E') {
	str1++;
	str2 = strchr (str1, (int)'F');

	if (str2) {
	  /*
	    {<F id="E1F1">à</F> <F id="E1F2">moins</F> <F id="E1F3">que</F>} à_moins_que  
	    comment   ^
	    str1               ^
	    str2                ^
	  */
	  *str2 = SXNUL;
	  sentence_no = atoi (str1);
	  *str2 = 'F';
	}
      }
    }
  }

  return sentence_no;
}

#ifndef no_lfg

#ifdef EASY
/* On est ds la campagne d'evaluation EASY */

static XxYxZ_header  easy_hd, easy_Tij_hd;
static SXBA          easy_constituent_nt_set, easy_Aij_constituent_set, easy_range_set;
static int           *easy_constituent_stack, *easy_Tij_stack;
static VARSTR        easy_vstr;
static sxword_header easy_hrefs;
static int           *Xpq2easy_href;

static int           *i2lb, *non_repair_i2lb;

static struct attr {
  int i, j, size;
}                    *Aij2attr, *Tpq2attr, *dummy_Aij2attr;

static int           *dummy_easy_stack;

/* Met ds fs_id_stack tous les fs_id des sous-structures accessibles depuis le parametre */
static void
fill_fs_id_stack (fs_id)
     int fs_id;
{
  int bot, top, val, field_id, cur2, top2, x;
  unsigned char   static_field_kind;

  fs_id_stack [0] = 0;
  sxba_empty (fs_id_set);
  SXBA_1_bit (fs_id_set, fs_id);
  PUSH (fs_id_stack, fs_id);

  x = 1;

  do {
    bot = XH_X (XH_fs_hd, fs_id);
    top = XH_X (XH_fs_hd, fs_id+1);
      
    if (bot < top && XH_list_elem (XH_fs_hd, top-1) < 0)
      top--;

    while (bot < top) {
      val = XH_list_elem (XH_fs_hd, bot);

      field_id = val & FIELD_AND;
      static_field_kind = field_id2kind [field_id];
      val >>= FS_ID_SHIFT;

      if ((static_field_kind & STRUCT_KIND) && val) {
	if (static_field_kind == STRUCT_KIND) {
	  if (SXBA_bit_is_reset_set (fs_id_set, val))
	    PUSH (fs_id_stack, val);
	}
	else {
	  for (cur2 = XH_X (XH_ufs_hd, val), top2 = XH_X (XH_ufs_hd, val+1);
	       cur2 < top2;
	       cur2++) {
	    val = XH_list_elem (XH_ufs_hd, cur2);

	    if (val < 0)
	      break;
		
	    if (SXBA_bit_is_reset_set (fs_id_set, val))
	      PUSH (fs_id_stack, val);
	  }
	}
      }

      bot++;
    }

    if (++x > fs_id_stack [0])
      /* Pas de nl pour Priority ... */
      fs_id = 0;
    else
      fs_id = fs_id_stack [x];
  } while (fs_id);
}


/* Earley a execute' xt_complete qui assure la presence de terminaux instancies Tpq (0 < p < q < final_mlstn) dans les ranges
   en erreur, et donc accessibles par Tij_iforeach
   Tpq peut ne pas etre un terminal de la foret (il a pu etre remplace' par un Thk avec h<=0 ou k<=0) */
/* 0 < i <= j < final_mlstn */
static bool
tpath2easy (i, j)
     int i, j;
{
  int     trans, k, triple, Tij;
  bool ret_val;

#if EBUG
  if (i <= 0 || j < i || j >= final_mlstn)
    sxtrap (ME, "tpath2easy");
#endif /* EBUG */

  if (i == j) return true;

  ret_val = false;

  Tij_iforeach (i, Tij) {
    k = Tij2j (Tij); /* k > i */

    if (i < k && k <= j && tpath2easy (k, j)) {
      ret_val = true;
      XxYxZ_set (&easy_Tij_hd, i, Tij, k, &triple);

      /* un seul chemin */
      break;
    }
  }

  return ret_val;
}

static int *Easy_sorted_Aij;

/* Grossier */
static int
Easy_cmp_Aij (ptr1, ptr2)
     int *ptr1, *ptr2;
{
  int Aij, Bpq, i, p, size1, size2;

  Aij = *ptr1;
  Bpq = *ptr2;

  size1 = (Aij < 0) ? dummy_easy_stack [-Aij+1] : Aij2attr [Aij].size;
  size2 = (Bpq < 0) ? dummy_easy_stack [-Bpq+1] : Aij2attr [Bpq].size;

  if (size1 != size2)
    return size2-size1; /* prio au + grand */

  if (spf.outputG.has_repair) {
    /* On peut avoir j < i */
    i = (Aij < 0) ? dummy_easy_stack [-Aij+4] : Aij2attr [Aij].i;
    p = (Bpq < 0) ? dummy_easy_stack [-Bpq+4] : Aij2attr [Bpq].i;
  }
  else {
    i = (Aij < 0) ? dummy_easy_stack [-Aij+2] : Aij2i (Aij);
    p = (Bpq < 0) ? dummy_easy_stack [-Bpq+2] : Aij2i (Bpq);
  }

  return i-p; /* prio au + proche du debut si meme taille */

#if 0
  if (i != p)
    return i-p;

  return q-j;
#endif /* 0 */
}

static void
Easy_maximal_constituent ()
{
  int top, i, j, x, Aij, triple;

  top = Easy_sorted_Aij [0];

  if (top) {
    if (top > 1)
      qsort (Easy_sorted_Aij+1, top, sizeof (int), Easy_cmp_Aij);

    for (x = 1; x <= top; x++) {
      Aij = Easy_sorted_Aij [x];
  
      if (Aij < 0) {
	/* dummy */
	i = dummy_easy_stack [-Aij+2];
	j = dummy_easy_stack [-Aij+3];
      }
      else {
	i = Aij2i (Aij);
	j = Aij2j (Aij);
      }

      /* On met les "vrais" i, j ds easy_hd */

#if 0
      if (spf.outputG.has_repair) {
	/* On peut avoir j < i */
	if (Aij < 0) {
	  /* dummy */
	  i = dummy_easy_stack [-Aij+4];
	  j = dummy_easy_stack [-Aij+5];
	}
	else {
	  i = Aij2attr [Aij].i;
	  j = Aij2attr [Aij].j;
	}
      }
#endif /* 0 */
	
      if (!SXBA_bit_is_set (easy_range_set, i)) {
	XxYxZ_set (&easy_hd, i, Aij, j, &triple);
	sxba_range (easy_range_set, i, j-i);
      }
    }
  }
}

#define Easy_PV_code (-1)
#define Easy_NV_code (-2)
static int     VP3_code, VERB_code, PV_code, VP_code; 
static bool has_VP3;
static int     *easy_upper_bound;
static bool xml_close_tag;

/* Aii <= maxnt */
static void
Aii_update (Aii, bsup)
     int Aii, bsup;
{
  int            hook, prod, item, Xpq;
  struct attr    *attr_ptr;
      
  attr_ptr = Aij2attr+Aii;
  attr_ptr->i = attr_ptr->j = bsup;

  hook = spf.outputG.lhs [spf.outputG.maxxprod+Aii].prolon;

  while ((prod = spf.outputG.rhs [hook++].lispro) != 0) {
    if (prod > 0) {
      item = spf.outputG.lhs [prod].prolon;

      while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
#if EBUG
	if (Xpq < 0 || Xpq > spf.outputG.maxxnt || Aij2i (Xpq) != Aij2j (Xpq))
	  sxtrap (ME, "Aii_update");
#endif /* EBUG */

	Aii_update (Xpq, bsup);
      }
    }
  }
}


/* On synthetise l'index sup max des Easy_NV--- ds les regles "VERB -> Easy_NV--- ... " */
/* Construit Aij2attr */
static int
dummy_easy_bu_walk (prod)
     int prod;
{
  int            Aij, A, item, Xpq, Tpq, X, q, trans, i, j, bot_item, size1, size2, new_q;
  int            binf, bsup, pos, size, p;
  struct lhs     *plhs;
  struct attr    *attr_ptr;
  bool        first_symb;
  
  plhs = spf.outputG.lhs+prod;
  Aij = plhs->lhs; /* $$ */
  A = Aij2A (Aij);
  bot_item = spf.outputG.lhs [prod].prolon;

  i = Aij2i (Aij);
  j = Aij2j (Aij);

  /* PB :  ... B[i..i] C[i::i] D[i..i] ...
     C[i::i] - > C'[i::k] C''[k::i]
     ...
     Les vraies bornes associees aux [i..i] dependent de l'environnement
  */

  /* Si repair, on peut avoir i == j et des fils !! */
  /* On remplit Aij2attr */
  item = bot_item;

  size = 0;
  binf = bsup = 0;
  first_symb = true;

  Xpq = spf.outputG.rhs [item++].lispro;

  if (Xpq == 0) {
    /* Vraie production vide */
    attr_ptr = Aij2attr+Aij;
    attr_ptr->size = 0;

    if (spf.outputG.has_repair) {
      /* ces valeurs ne sont peut etre pas definitives !! */
      attr_ptr->i = attr_ptr->j = non_repair_i2lb [i];
    }
  }
  else {
    if (spf.outputG.has_repair) {
      binf = bsup = non_repair_i2lb [i];
    }

    do {
      if (spf.outputG.has_repair) {
	if (Xpq > 0) {
	  attr_ptr = Aij2attr+Xpq;
	}
	else {
	  /* Attention, si on est sur un Tpq d'erreur, il peut ne pas figurer ds une xprod
	     (p<=0 ou q<=0) ... et donc son Tpq2attr n'est pas positionne' */
	  Tpq = -Xpq;
	  p = Tij2i (Tpq);
	  q = Tij2j (Tpq);

	  if (p <= 0 || q <= 0) {
	    /* on suppose que c'est un terminal qui a ete supprime' => size == 0 */
	    continue;
	  }

	  attr_ptr = Tpq2attr+Tpq;
	}

	if (first_symb) {
	  first_symb = false;
	  binf = attr_ptr->i;
	  bsup = attr_ptr->j; /* truc */
	}

	if (Xpq > 0 && Xpq <= spf.outputG.maxnt && attr_ptr->size == 0) {
	  p = Aij2i (Xpq);
	  q = Aij2j (Xpq);

	  if (p == q) {
	    /* On change les valeurs stockees */
	    /* A faire recursivement */
	    /* pas de repair */
	    Aii_update (Xpq, bsup);
	  }
	}

	bsup = attr_ptr->j;
      }

      if (Xpq > 0) {
	size += Aij2attr [Xpq].size;
      }
      else {
	Tpq = -Xpq;
	p = Tij2i (Tpq);
	q = Tij2j (Tpq);
	  
	if (p > 0 && q > 0)
	  size++;
      }
    } while ((Xpq = spf.outputG.rhs [item++].lispro) != 0);

    attr_ptr = Aij2attr+Aij;
    attr_ptr->size = size;
    
    if (spf.outputG.has_repair) {
      attr_ptr->i = binf;
      attr_ptr->j = bsup;
    }
  }

  item = bot_item;

  if (A == VERB_code) {
    /* VERB -> Easy_NV* ... ; (tous les VERB commencent par Easy_NV*) */
    Xpq = spf.outputG.rhs [item].lispro; /* Easy_NV--- */
    dummy_Aij2attr [Aij].i = Aij2j (Xpq);
    dummy_Aij2attr [Aij].size = Aij2attr [Xpq].size;

    if (spf.outputG.has_repair)
      dummy_Aij2attr [Aij].j = Aij2attr [Xpq].j; 
  }
  else {
    if (A == VP3_code) {
      /* On est sur "VP3 -> (Clneg_advneg) VERB (POSTVERB) ;" */
      Xpq = spf.outputG.rhs [item+1].lispro; /* VERB */
      q = dummy_Aij2attr [Aij].i = dummy_Aij2attr [Xpq].i; /* celle de Easy_NV--- */
      size2 = dummy_Aij2attr [Xpq].size; /* celle de Easy_NV--- */
    
      if (spf.outputG.has_repair)
	new_q = dummy_Aij2attr [Aij].j = dummy_Aij2attr [Xpq].j; 

      Xpq = spf.outputG.rhs [item].lispro; /* (Clneg_advneg) */
      size1 = dummy_Aij2attr [Xpq].size;  /* celle de (Clneg_advneg) */

      if (size1) {
	/* non vide */
	/* On fabrique un Easy_NVij */
	DCHECK (dummy_easy_stack, 6);
	DSPUSH (dummy_easy_stack, Easy_NV_code);
	/* C'est un dummy constituant easy ...*/
	/* ... on le pushe en neg.  Normalement ca tient ds Easy_sorted_Aij */
	PUSH (Easy_sorted_Aij, -DTOP (dummy_easy_stack)); /* indice du bas de pile */
	DSPUSH (dummy_easy_stack, size1+size2); /* ... puis la size */
	DSPUSH (dummy_easy_stack, Aij2i (Aij)); /* ... puis la binf */
	DSPUSH (dummy_easy_stack, q); /* ... et la bsup (ancienne) */

	if (spf.outputG.has_repair) {
	  DSPUSH (dummy_easy_stack, Aij2attr [Aij].j); /* ... puis la binf */
	  DSPUSH (dummy_easy_stack, new_q); /* ... et la bsup (nouvelle) */
	}
      }
    }
    else {
      if (A == VP_code) {
	/* On est sur "VP -> VP3"
	   ou sur une des recursives "VP -> ... VP ... VP3" */
	/* Si une des recursives, on prend le VP sinon VP3 */
	size1 = 0;

	while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
	  if (Xpq > 0) {
	    X = Aij2A (Xpq);

	    if (X == VP_code || X == VP3_code) {
	      dummy_Aij2attr [Aij] = dummy_Aij2attr [Xpq];
	      dummy_Aij2attr [Aij].size += size1;
	      break;
	    }

	    size1 += Aij2attr [Xpq].size;
	  }
	  else {
	    Tpq = -Xpq;
	    p = Tij2i (Tpq);
	    q = Tij2j (Tpq);
	  
	    if (p > 0 && q > 0)
	      size1++;
	  }
	}
      }
      else {
	if (A == PV_code) {
	  /* On est sur "PV -> prep VP" */
	  /* Soyons un peu + general */
	  size1 = 0;

	  while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
	    if (Xpq > 0) {
	      X = Aij2A (Xpq);

	      if (X == VP_code) {
		/* PV n'a pas l'air d'etre utilise' !! */
		/* oN NE PROLONGE DONC PAS LES VALEURS */
		/* On fabrique un Easy_PVij */
		DCHECK (dummy_easy_stack, 6);
		DSPUSH (dummy_easy_stack, Easy_PV_code);
		/* C'est un dummy constituant easy ...*/
		/* ... on le pushe en neg.  Normalement ca tient ds Easy_sorted_Aij */
		PUSH (Easy_sorted_Aij, -DTOP (dummy_easy_stack)); /* indice du bas de pile */
		DSPUSH (dummy_easy_stack, size1+Aij2attr [Xpq].size); /* ... puis la size */
		DSPUSH (dummy_easy_stack, Aij2i (Aij)); /* ... puis la binf */
		DSPUSH (dummy_easy_stack, dummy_Aij2attr [Xpq].i); /* ... et la bsup (ancienne) */

		if (spf.outputG.has_repair) {
		  DSPUSH (dummy_easy_stack, Aij2attr [Aij].j); /* ... puis la binf */
		  DSPUSH (dummy_easy_stack, dummy_Aij2attr [Xpq].j); /* ... et la bsup (nouvelle) */
		}
		break;
	      }

	      size1 += Aij2attr [Xpq].size;
	    }
	    else {
	      Tpq = -Xpq;
	      p = Tij2i (Tpq);
	      q = Tij2j (Tpq);
	  
	      if (p > 0 && q > 0)
		size1++;
	    }
	  }
	}
      }
    }
  }

  return 1;
}




/* Forme de l'automate :
   0 ->"VP3" 1
   0 ->"PV"  4
   1 ->"VP3 -> (Clneg_advneg) VERB (POSTVERB)" 2  :  si (Clneg_advneg) est non vide
   2 ->"VERB" 3
   2 ->"else" 2
   4 -> "VP3" 5
   4 -> "else" 4
   5 ->"VP3 -> (Clneg_advneg) VERB (POSTVERB)" 2  : (Clneg_advneg) peut etre vide
*/

static int
easy_td_walk (prod)
     int prod;
{
  int            Aij, Xpq, Tpq, A, i, j, p, q, item, trans;

#if 0
  int            Aij, Xpq, A, triple, trans, item, Tpq, t, i, j, i_rcvr, j_rcvr, p, q, prev_ub, size;
  static int     prev_state, prev2_state, prev_i;
#endif /* 0 */

  Aij = spf.walk.cur_Aij;
  A = Aij2A (Aij); 
  i = Aij2i (Aij);
  j = Aij2j (Aij);

  if (SXBA_bit_is_set (easy_constituent_nt_set, A)) {
    /* C'est un constituant easy ...*/
    PUSH (Easy_sorted_Aij, Aij);
  }

  /* On capture les terminaux de la RHS */
  /* qui ne sont pas couverts par des productions (normales ou de la reparation) */
  /* Attention, meme ds le cas repair, on peut avoir des [i::j] ds les RHS des prod instanciees */
  /* ... mais pas a l'interieur des xprod */
  item = spf.outputG.lhs [prod].prolon;

  while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
    /* On parcourt la RHS de la prod instanciee */
    /* On est obligatoirement hors constituant easy, les terminaux trouves permettent donc
       de "faire la liaison" entre les constituants easy */
    if (Xpq > 0) {
      p = Aij2i (Xpq);
      q = Aij2j (Xpq);
    }
    else {
      /* t ... */
      Tpq = -Xpq;
      p = Tij2i (Tpq);
      q = Tij2j (Tpq);
    }

    /*
          | p<0 | p==0 | p>0
      ------------------------
      q<0 |rien |rien  | i->p
          |     |      | i=p
      ------------------------
      q==0|rien |rien  | impos
          |     |      | sible
      ------------------------
      q>0 |impos| i->q | i->p
          |sible| i==q | i=p
    */

    if (p <= 0 && q <= 0) {
      /* rien, traitement diffe're' */
    }
    else {
      if (p > 0) {
#if EBUG
	if (q == 0)
	  sxtrap (ME, "easy_td_walk");
#endif /* EBUG */

	if (i < p)
	  tpath2easy (i, p);

	i = (q < 0) ? p : q;
      }
      else {
#if EBUG
	if (p < 0)
	  sxtrap (ME, "easy_td_walk");
#endif /* EBUG */

	/* p == 0 */
	if (i < q) {
	  tpath2easy (i, q);
	  i = q;
	}
      }
    }

    if (Xpq < 0 && p > 0 && q > 0) {
      /* t ou xt de la reparation */
      XxYxZ_set (&easy_Tij_hd, p, Tpq, q, &trans);
    }
  }

  if (i < j)
    tpath2easy (i, j);

#if 0
  i_rcvr = 0;
  prev_ub = i;

  /* Attention, meme ds le cas repair, on peut avoir des [i::j] ds les RHS des prod instanciees */
  /* ... mais pas a l'interieur des xprod */

  while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
    /* On parcourt la RHS de la prod instanciee */
    /* On est obligatoirement hors constituant easy, les terminaux trouves permettent donc
       de "faire la liaison" entre les constituants easy */
    if (Xpq > 0) {
      p = Aij2i (Xpq);
      q = Aij2j (Xpq);

      if (Xpq > spf.outputG.maxnt && SXBA_bit_is_set (spf.outputG.non_productive_Aij_rcvr_set, Xpq)) {
	/* nt de la rcvr ... et pas de repair dessus */
	if (i_rcvr == 0) {
	  i_rcvr = (p < 0) ? -p : p;
#if EBUG
	  if (i_rcvr == 0)
	    sxtrap (ME, "easy_td_walk");
#endif /* 0 */
	}

	if (q != 0) {
	  /* On force un chemin terminal entre i_rcvr et j_rcvr */
	  /* Les symboles de l'error recovery couvrent [i_rcvr..j_rcvr] */
	  /* On stocke tous les chemins terminaux entre i_rcvr et j_rcvr */
	  j_rcvr = (q < 0) ? -q : q;

	  if (j_rcvr > i_rcvr)
	    tpath2easy (i_rcvr, j_rcvr);

	  i_rcvr = 0;
	}
      }
    }
    else {
      /* t ... */
      Tpq = -Xpq;
      p = Tij2i (Tpq);
      q = Tij2j (Tpq);

      /* ... et on met tous les terminaux */
      if (spf.outputG.Tpq_rcvr_set && SXBA_bit_is_set (spf.outputG.Tpq_rcvr_set, Tpq)) {
	/* terminal de la rcvr */
	if (spf.outputG.Tpq_repair_nb == 0 /* ... et pas de la reparation */) {
	  /* t de la rcvr */
	  if (i_rcvr == 0) {
#if EBUG
	    if (p == 0)
	      sxtrap (ME, "easy_td_walk");
#endif /* 0 */

	    i_rcvr = (p < 0) ? -p : p;
	  }

	  if (q != 0) {
	    /* On force un chemin terminal entre i_rcvr et j_rcvr */
	    /* Les symboles de l'error recovery couvrent [i_rcvr..j_rcvr] */
	    /* On stocke tous les chemins terminaux entre i_rcvr et j_rcvr */
	    j_rcvr = (q < 0) ? -q : q;

	    if (j_rcvr > i_rcvr)
	      tpath2easy (i_rcvr, j_rcvr);

	    i_rcvr = 0;
	  }
	}
	else {
	  /* repair */
	  XxYxZ_set (&easy_Tij_hd, p, Tpq, q, &trans); /* p et q qcq */
	}
      }
      else {
	XxYxZ_set (&easy_Tij_hd, p, Tpq, q, &trans); /* q > p */
      }
    }

    if (prev_ub < p) {
      /* Erreur Cas [prev_ub]::[p] */
      tpath2easy (prev_ub, p);
    }

    if (q > 0)
      prev_ub = q;
  }

  if (j > prev_ub) {
    /* Erreur : cas A -> \alpha [prev_ub]::[j] */
    tpath2easy (prev_ub, j);
  }
#endif /* 0 */

  return 1;
}



/* retourne le "dpv_ptr" du champ pred de la f_structure fs_id */
static int*
seek_pred (fs_id)
     int fs_id;
{
  int  val;
  int  *val_ptr;
      
  if (fs_is_set (fs_id, pred_id, &val_ptr) && (val = get_fs_id_atom_val (*val_ptr))) {
    return &(XH_list_elem (dynam_pred_vals, XH_X (dynam_pred_vals, val)));
  }

  return NULL;
}


/* Retourne le code interne de la tete terminale d'un champ pred d'un f_structure */
static int
fs_id2tete (fs_id)
     int fs_id;
{
  int *dpv_ptr, Tpq, t_code;

  if ((dpv_ptr = seek_pred (fs_id)) && (Tpq = dpv_ptr [4])) {
    Tpq -=  spf.outputG.maxxnt;
    t_code = -Tij2T (Tpq);
  }
  else
    t_code = 0;

  return t_code;
}

/* Recherche ds la sous-foret de racine Aij le premier (?) terminal dont le (prefixe du) nom est tstr */
static int
seek_t (Aij, tstr, is_prefix)
     int     Aij;
     char    *tstr;
     bool is_prefix;
{
  int  hook, prod, item, Xpq, ret_val, lstr, ltstr;
  char *str;

  if (Aij > spf.outputG.maxnt /* nt en erreur */) return 0;

  hook = spf.outputG.lhs [spf.outputG.maxxprod+Aij].prolon;

  while ((prod = spf.outputG.rhs [hook++].lispro) != 0) {
    if (prod > 0) {
      item = spf.outputG.lhs [prod].prolon;

      while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
	/* On parcourt la RHS de la prod instanciee */
	if (Xpq > 0) {
	  if (ret_val = seek_t (Xpq, tstr, is_prefix))
	    return ret_val;
	}
	else {
	  str = spf.inputG.tstring [-Tij2T (-Xpq)];
	  lstr = strlen (str);
	  ltstr = strlen (tstr);

	  if ((is_prefix ? (lstr >= ltstr) : (lstr == ltstr)) && strncmp (str, tstr, ltstr) == 0)
	    return Xpq2easy_href [spf.outputG.maxxnt - Xpq];
	}
      }
    }
  }

  return 0;
}

static int
seek_Easy_Infl_v (Pij)
     int Pij;
{
  int item, Xpq, href_verbe;
  char *str;


  if (seek_prefix_Aij ("Easy_NVS", Pij) 
      || seek_prefix_Aij ("Easy_NVA1", Pij) 
      || seek_prefix_Aij ("Easy_NVAU", Pij)) {
    /* ... C'est la bonne */
    item = spf.outputG.lhs [Pij].prolon;
    
    while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
      if ((Xpq < 0) && (href_verbe = Xpq2easy_href [spf.outputG.maxxnt - Xpq])) {
	/* On suppose que c'est le bon "verbe" */
	return href_verbe;
      }
    }
  }
  
  return 0;
}

/* Si T est le terminal de Tpq, on retourne son href */
static int
Tpq2href (T, Tpq, is_prefix)
     char    *T;
     int     Tpq;
     bool is_prefix;
{
  int  lgth, str_lgth;
  char *str;

  if (Tpq) {
    lgth = strlen (T);
    str = Tpq2str(Tpq);
    str_lgth = strlen (str);

    if ((is_prefix && str_lgth >= lgth || !is_prefix && str_lgth == lgth) && strncmp (str, T, lgth) == 0) {
      return Xpq2easy_href [Tpq];
    }
  }

  return 0;
}


static int
seek_Easy_GN (Pij)
     int Pij;
{
  int  Aij, href_gn;

  if (Aij = seek_prefix_Aij ("Easy_GN", Pij)) {
    if (href_gn = Xpq2easy_href [Aij])
      return href_gn;
  }
  
  return 0;
}


static int
seek_Easy_GN_np (Pij)
     int Pij;
{
  int  Aij, href_gn;

  if (Aij = seek_prefix_Aij ("Easy_GN", Pij)) {
    if (href_gn = seek_t (Aij, "np", false /* Le terminal "np" */))
      return href_gn;
  }
  
  return 0;
}


static int
seek_Easy_GR (Pij)
     int Pij;
{
  int  Aij, href_gn;

  if (Aij = seek_prefix_Aij ("Easy_GR", Pij)) {
    if (href_gn = Xpq2easy_href [Aij])
      return href_gn;
  }
  
  return 0;
}


#ifdef no_lfg
/* On regarde si le nom du nt en lhs de Pij commence par Easy_A */
int
seek_prefix_Aij (Easy_A, Pij)
     char *Easy_A;
     int  Pij;
{
  int  lgth, Aij;
  char *str;

  lgth = strlen (Easy_A);
  Aij = spf.outputG.lhs [Pij].lhs;
  str = spf.inputG.ntstring [Aij2A (Aij)];

  if (strlen (str) >= lgth && strncmp (str, Easy_A, lgth) == 0) {
    return Aij;
  }
  
  return 0;
}
/* On regarde si le nom du nt en lhs de Pij est A */
int
seek_Aij (A, Pij)
     char *A;
     int  Pij;
{
  int  lgth, Aij;
  char *str;

  lgth = strlen (A);
  Aij = spf.outputG.lhs [Pij].lhs;
  str = spf.inputG.ntstring [Aij2A (Aij)];

  if (strlen (str) == lgth && strncmp (str, A, lgth) == 0) {
    return Aij;
  }
  
  return 0;
}
#endif /* no_lfg */

/* Attention retourne Pij si la lhs de cette Pij est "PP_tempij" */
static int
seek_PP_tempij (Pij)
     int Pij;
{
  return seek_Aij ("PP_temp", Pij) ? Pij : 0;
}

/* Attention retourne  Pij si la lhs de cette Pij est "NP_temp...ij" */
static int
seek_Easy_GNij (Pij)
     int Pij;
{
  return seek_prefix_Aij ("Easy_GN", Pij) ? Pij : 0;
}


/* Attention retourne  Pij si la lhs de cette Pij est "NP_temp...ij" */
static int
seek_NP_tempij (Pij)
     int Pij;
{
  return seek_prefix_Aij ("NP_temp", Pij) ? Pij : 0;
}


/* Attention retourne  Pij si la lhs de cette Pij est "Easy_JUXT2ij" */
static int
seek_Easy_JUXT2ij (Pij)
     int Pij;
{
  return seek_prefix_Aij ("Easy_JUXT2", Pij) ? Pij : 0;
}


/* Attention retourne  Pij si la lhs de cette Pij est "Easy_JUXTij" */
static int
seek_Easy_JUXTij (Pij)
     int Pij;
{
  return seek_Aij ("Easy_JUXT", Pij) ? Pij : 0;
}


static int
seek_VERB_v (Pij)
     int Pij;
{
  int  Aij, href_verbe;
  
  if ((Aij = seek_prefix_Aij ("Easy_NVS", Pij)) || (Aij = seek_prefix_Aij ("Easy_NVP", Pij))) {
    if (href_verbe = seek_t (Aij, "v", 
			     true /* Un terminal dont le nom commence par "v" */
			     ))
      return href_verbe;
  }

  return 0;
}


static int
seek_N_nc (Pij)
     int Pij;
{
  int  Aij, href_verbe;

  if (Aij = seek_Aij ("N", Pij)) {
    if (href_verbe = seek_t (Aij, "nc", false /* Le terminal "nc" */))
      return href_verbe;
  }
  
  return 0;
}


static int
seek_WS_csu (Pij)
     int Pij;
{
  int  Aij, href;

  if (Aij = seek_Aij ("WS", Pij)) {
    if (href = seek_t (Aij, "csu", false /* Le terminal "csu" */))
      return href;
  }
  
  return 0;
}


static int
seek_Easy_PV_prep (Pij)
     int Pij;
{
  int  Aij, href;

  if (Aij = seek_prefix_Aij ("Easy_PV", Pij)) {
    if (href = seek_t (Aij, "prep", false /* Le terminal "prep" */))
      return href;
  }
  
  return 0;
}


static int
seek_Easy_GA_adj (Pij)
     int Pij;
{
  int  Aij, href;

  if (Aij = seek_prefix_Aij ("Easy_GA", Pij)) {
    if (href = seek_t (Aij, "adj", false /* Le terminal "adj" */))
      return href;
  }
  
  return 0;
}


static int
seek_Easy_GR_adv (Pij)
     int Pij;
{
  int  Aij, href;

  if (Aij = seek_prefix_Aij ("Easy_GR", Pij)) {
    if (href = seek_t (Aij, "adv", false /* Le terminal "adv" */))
      return href;
  }
  
  return 0;
}


static int
seek_Easy_NV (Pij)
     int Pij;
{
  int  Aij, href_gn;

  if (Aij = seek_prefix_Aij ("Easy_NV", Pij)) {
    if (href_gn = Xpq2easy_href [Aij])
      return href_gn;
  }
  
  return 0;
}

static int
seek_Easy_NVij (Pij)
     int Pij;
{
  int  Aij, href_gn;

  if (Aij = seek_prefix_Aij ("Easy_NV", Pij)) {
    return Aij;
  }
  
  return 0;
}

static int
seek_Easy_GA_or_adj (Pij)
     int Pij;
{
  int  Aij, href_gn;

  if (Aij = seek_prefix_Aij ("Easy_GA", Pij)) {
    if (href_gn = Xpq2easy_href [Aij])
      return href_gn;

    return seek_t (Aij, "adj", false /* Le terminal "adj" */);
  }
  
  return 0;
}

static int
seek_Easy_GA (Pij)
     int Pij;
{
  int  Aij, href_gn;

  if (Aij = seek_prefix_Aij ("Easy_GA", Pij)) {
    if (href_gn = Xpq2easy_href [Aij])
      return href_gn;
  }
  
  return 0;
}

static int
seek_Easy_GP (Pij)
     int Pij;
{
  int  Aij, href_gn;

  if (Aij = seek_prefix_Aij ("Easy_GP", Pij)) {
    if (href_gn = Xpq2easy_href [Aij])
      return href_gn;
  }
  
  return 0;
}


static int
seek_Easy_PV (Pij)
     int Pij;
{
  int  Aij, href_gn;

  if (Aij = seek_prefix_Aij ("Easy_PV", Pij)) {
    if (href_gn = Xpq2easy_href [Aij])
      return href_gn;
  }
  
  return 0;
}


static int *NV_stack;

#ifndef no_print_relations

static int R; /* Numero de la relation */

/*
 C.15 Table r?capitulative sur l'annotation des relations
La table suivante r?capitule les informations mentionn?es ci-dessus. Elle indique la nature des constituants qui peuvent ?tre arguments pour chaque relation.

A noter :

   1. un constituant (GN, NV, GP, GA, GR, PV) peut ?tre remplac? par la forme lui correspondant;
   2. il est ?vident que pour annoter les relations dont la source et la cible sont dans le m?me constituant, il est imp?ratif d'utiliser les formes;
   3. dans le cas particulier de proposition sans verbe, le NV peut ?tre remplac? par un syntagme d'une autre nature : ceci n'est pas pris en compte dans la grille suivante (voir D.1.VIII Phrases et propositions sans verbe)



	                  Argument 1	                  Argument 2	   Argument 3
1. Sujet-Verbe            GN, NV, un pronom clitique 	  NV 	           _
2. Auxiliaire-Verbe       NV                        	  NV       	   _
3. COD-Verbe              GN, NV, PV, un pronom clitique  NV, PV    	   _
4. Compl?ment-Verbe       GP, PV, un pronom clitique  	  NV, PV   	   _
5. Modifieur-Verbe        GN, NV, GR                	  NV, PV   	   _
6. Compl?menteur          une forme                 	  NV, GN, GA	   _
7. Attribut-Sujet/Objet	  GA, GN, GP, PV           	  NV, PV   	   s ou o (pour sujet ou objet)
8. Modifieur-Nom	  GA, GP, GN, NV, PV, GR   	  GN, GP   	   x (pour indiquer la propagation), rien sinon
9. Modifieur-Adjectif	  GA, GR, GP, PV          	  GA, adjectif     -
                                                          d un GN sans nom
10. Modifieur-Adverbe	  GR                      	  GR               _
11. Modifieur-Pr?position GR                     	  une pr?position  _
12. Coordination          une conjonction de coordination GA, GP, GN,      GA, GP, GN,
                          une virgule                     NV, PV, GR       NV, PV, GR
                          un point virgule
13. Apposition            GN                              GN               _
14. Juxtaposition         GA, GP, GN,                     GA, GP, GN,      -       
                          NV, PV, GR                      NV, PV, GR
*/

/*
<relation xlink:type="extended" type="SUJ-V" id="E1R1"> 
<sujet xlink:type="locator" xlink:href="E1G2"/> 
<verbe xlink:type="locator" xlink:href="E1F7"/> 
</relation> 
*/
/* Le 18/11/04 capture aussi des relations MOD-N */
static void
easy_Sujet_Verbe ()
{
  int     fs_id, sujet_val, atom_id, href_verbe, aij_val, val, cur, top, Pij, Aij, A, item, Xpq, href_sujet, x, v_form_val, atom_val; 
  int     *dpv_ptr, *val_ptr;
  char    *str;
  bool has_mode_field, has_v_form_field, is_MOD_N;

  /* Pour chaque f_structure ... */
  for (x = 1; x <= fs_id_stack [0]; x++) {
    fs_id = fs_id_stack [x];

    /* ... on regarde si elle a un champ subj */
    if (fs_is_set (fs_id, subj_id, &val_ptr) && (sujet_val = (*val_ptr) >> FS_ID_SHIFT)) {
      /* Il y a un champ subj qui reference la [sous-]structure sujet_val */
      if (fs_is_set (fs_id, cat_id, &val_ptr) && (atom_id = get_fs_id_atom_val (*val_ptr))) {
	/* ... et un champ cat ... */
#ifndef ESSAI
	val = field_id2atom_field_id [cat_id] [atom_id2local_atom_id (atom_id)];
#else /* ESSAI */
	val = atom_id;
#endif /* ESSAI */

	if (val == atom_v
	    || val == atom_vSe 
	    || val == atom_vppart || val == atom_vSeppart 
	    || val == atom_vinf || val == atom_vSeinf
	    || val == atom_vprespart || val == atom_vSeprespart 
	    ) {
	  /* ... qui reference un verbe */
	  /* On cherche ce verbe */
	  if (fs_is_set (fs_id, aij_id, &val_ptr))
	    aij_val = (*val_ptr) >> FS_ID_SHIFT;
	  else
	    aij_val = 0;

	  if (aij_val) {
	    /* Y'a un champ aij */
	    /* Si ce verbe n'a pas de champ tense ou s'il a un champ v_form de valeur participle,
	       on suppose qu'on est ds le cas MOD-N */
	    has_mode_field = fs_is_set (fs_id, mode_id, &val_ptr);
	    has_v_form_field = fs_is_set (fs_id, v_form_id, &val_ptr);

	    if (has_v_form_field)
	      v_form_val = get_fs_id_atom_val (*val_ptr);
	    else
	      v_form_val = 0;

#ifndef ESSAI
	    atom_val = field_id2atom_field_id [v_form_id] [atom_id2local_atom_id (v_form_val)];
#else /* ESSAI */
	    atom_val = v_form_val;
#endif /* ESSAI */

	    is_MOD_N = !(v_form_val && atom_val == atom_infinitive)
	      && ((!has_mode_field) || (v_form_val && atom_val == atom_participle));

	    /* Y'a un champ aij */
	    if (is_MOD_N) {
	      /* On cherche un Easy_NV */
	      href_verbe = walk_aij (aij_val, seek_Easy_NV);
	      
	      if (href_verbe) {
		/* Le MOD */
		/* ... maintenant, on cherche le nom ... */
		/* ... qui est un nc ou un np */
		if (fs_is_set (sujet_val, aij_id, &val_ptr) && (aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
		  href_sujet = walk_aij (aij_val, seek_N_nc);

		  if (href_sujet == 0)
		     href_sujet = walk_aij (aij_val, seek_Easy_GN_np);
		}
		else
		  href_sujet = 0;

		if (href_sujet) {
		  /* Le N */
		  /* On a une relation Modifieur_Nom */
		  fprintf (xml_file,
			   "<relation xlink:type=\"extended\" type=\"MOD-N\" id=\"E%iR%i\">\n",
			   sentence_id,
			   ++R
			   );
		  fprintf (xml_file,"\
<modifieur xlink:type=\"locator\" xlink:href=\"%s\"/>\n\
<nom xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
			   SXWORD_get (easy_hrefs, href_verbe),
			   SXWORD_get (easy_hrefs, href_sujet)
			   );
 
		  fputs ("<à-propager xlink:type=\"locator\" xlink:href=\"faux\"/>\n", xml_file);
  
		  fputs ("</relation>\n", xml_file);

		  if (is_print_f_structure) {
		    printf ("MOD-N (%s, %s, false)\n",
			    SXWORD_get (easy_hrefs, href_verbe)+Ei_lgth,
			    SXWORD_get (easy_hrefs, href_sujet)+Ei_lgth
			    );
		  }
		}
	      }
	    }
	    else {
	      href_verbe = walk_aij (aij_val, seek_Easy_Infl_v);

	      if (href_verbe) {
		/* ... maintenant, on cherche le sujet ... */
		/* ... qui est un pronom clitique */
		/* ... si la f_structure sujet_val a un predicat 'pro' et un aij qui reference une prod dont un fils est
		   le terminal 'cln' */
		/* ... ou un GN */
		/* ... si  la f_structure sujet_val a un aij qui reference un Easy_GN */
		str = (dpv_ptr = seek_pred (sujet_val)) ? lexeme_id2string [dpv_ptr [0] /* lexeme_id */] : NULL;

		if (str && strlen (str) == 3 && strncmp (str, "pro", 3) == 0)
		  href_sujet = Tpq2href ("cl", dpv_ptr [4], true /* is_prefix */);
		else {
		  if (fs_is_set (sujet_val, aij_id, &val_ptr) && (aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
		    href_sujet = walk_aij (aij_val, seek_Easy_GN);
		  }
		  else
		    href_sujet = 0;
		}

		if (href_sujet) {
		  /* On a une relation Sujet-Verbe */
		  fprintf (xml_file,
			   "<relation xlink:type=\"extended\" type=\"SUJ-V\" id=\"E%iR%i\">\n",
			   sentence_id,
			   ++R
			   ); 
		  fprintf (xml_file,"\
<sujet xlink:type=\"locator\" xlink:href=\"%s\"/>\n\
<verbe xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
			   SXWORD_get (easy_hrefs, href_sujet),
			   SXWORD_get (easy_hrefs, href_verbe)
			   );
		  fputs ("</relation>\n", xml_file);

		  if (is_print_f_structure) {
		    printf ("SUJ-V (%s, %s)\n",
			    SXWORD_get (easy_hrefs, href_sujet)+Ei_lgth,
			    SXWORD_get (easy_hrefs, href_verbe)+Ei_lgth
			    );
		  }
		}
	      }

	    }
	  }
	}
      }
    }
  }
}


static int
get_aux_href (Aij)
     int Aij;
{
  /* Aij est un noeud "Easy_.*", on cherche son constituant associe a un terminal de nom "aux.*" */
  /* On cherche toutes les Aij-prods */
  int hook, prod, item, Xpq, ret_val;
  char *str;

  if (Aij > spf.outputG.maxnt)
    /* nt d'erreur */
    return 0;

  hook = spf.outputG.lhs [spf.outputG.maxxprod+Aij].prolon;

  while ((prod = spf.outputG.rhs [hook++].lispro) != 0) {
    if (prod > 0) {
      item = spf.outputG.lhs [prod].prolon;

      while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
	/* On parcourt la RHS de la prod instanciee */
	if (Xpq > 0) {
	  str = spf.inputG.ntstring [Aij2A (Xpq)];

	  if (strlen (str) > 5 && strncmp (str, "Easy_", 5) == 0 && (ret_val = get_aux_href (Xpq)))
	    return ret_val;
	}
	else {
	  str = spf.inputG.tstring [-Tij2T (-Xpq)];

	  if (strlen (str) > 3 && strncmp (str, "aux", 3) == 0)
	    return Xpq2easy_href [spf.outputG.maxxnt - Xpq];
	}
      }
    }
  }

  return 0;
}

static int
get_v_href (Aij)
     int Aij;
{
  /* Aij est un noeud EASY_NVi, on cherche son constituant associe au terminal de nom "v" */
  int hook, prod, item, Xpq, ret_val;
  char *str;

  if (Aij > spf.outputG.maxnt)
    /* nt d'erreur */
    return 0;

  hook = spf.outputG.lhs [spf.outputG.maxxprod+Aij].prolon;

  while ((prod = spf.outputG.rhs [hook++].lispro) != 0) {
    if (prod > 0) {
      item = spf.outputG.lhs [prod].prolon;

      while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
	/* On parcourt la RHS de la prod instanciee */
	if (Xpq > 0) {
	  str = spf.inputG.ntstring [Aij2A (Xpq)];

	  if (strlen (str) > 5 && strncmp (str, "Easy_", 5) == 0 && (ret_val = get_v_href (Xpq)))
	    return ret_val;
	}
	else {
	  str = spf.inputG.tstring [-Tij2T (-Xpq)];

	  if (strlen (str) == 1 && strncmp (str, "v", 1) == 0)
	    return Xpq2easy_href [spf.outputG.maxxnt - Xpq];
	}
      }
    }
  }

  return 0;
}

/*
<relation xlink:type="extended" type="AUX-V" id="E1R3"> 
<auxiliaire xlink:type="locator" xlink:href="E1F7"/> 
<verbe xlink:type="locator" xlink:href="E1F9"/> 
</relation> 
*/
static int
easy_pre_bu_Auxiliaire_Verbe (Aij)
     int Aij;
{
  char *str;

  str = spf.inputG.ntstring [Aij2A (Aij)];

  if (strncmp (str, "NV___1", 6) == 0) {
    return 1;
  }

  /* On saute les Aij-prods */
  return 0;
}

/* On est sur que prod definit un VERB (ou un NV___1 si beasy) */
static int
easy_bu_Auxiliaire_Verbe (prod)
     int prod;
{
  int  Xpq, item, href_aux, href2_aux, href3_aux, href_v;
  char *str;

  NV_stack [0] = 0;
  item = spf.outputG.lhs [prod].prolon;

  while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
    if (Xpq > 0) {
      str = spf.inputG.ntstring [Aij2A (Xpq)];
    
      if (strncmp(str+7,"Easy_NVAU",9) == 0 || strncmp(str+7,"Easy_NVAUSe",11) == 0) {
	PUSH (NV_stack, Xpq);
	PUSH (NV_stack, '2');	  
      } else if (strncmp(str+7,"PPART",5) == 0 || strncmp(str+7,"PPARTSe",7) == 0) {
	PUSH (NV_stack, Xpq);
	PUSH (NV_stack, '3');	  
      } else if (strncmp(str+7,"Easy_NVA1",9)) {
	PUSH (NV_stack, Xpq);
	PUSH (NV_stack, '6');	  
      } else if (strncmp(str+7,"AUX",3) == 0) {
	PUSH (NV_stack, Xpq);
	PUSH (NV_stack, '8');	  
      }
    }
  }

  switch (NV_stack[0] / 2) {
  case 2: /* (2, 3) ou (4, 5) */
    if (NV_stack [2] == 2 && NV_stack [4] == 3
	|| NV_stack [2] == 4 && NV_stack [4] == 5) {
      href_aux = get_aux_href (NV_stack [1]);
      href_v = get_v_href (NV_stack [3]);

      if (href_aux && href_v) {
	fprintf (xml_file,
		 "<relation xlink:type=\"extended\" type=\"AUX-V\" id=\"E%iR%i\">\n",
		 sentence_id,
		 ++R
		 ); 
	/* On a une relation Auxiliaire-Verbe */
	fprintf (xml_file,"\
<auxiliaire xlink:type=\"locator\" xlink:href=\"%s\"/>\n\
<verbe xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
		 SXWORD_get (easy_hrefs, href_aux),
		 SXWORD_get (easy_hrefs, href_v)
		 );
	fputs ("</relation>\n", xml_file);

	if (is_print_f_structure) {
	  printf ("AUX-V (%s, %s)\n",
		  SXWORD_get (easy_hrefs, href_aux)+Ei_lgth,
		  SXWORD_get (easy_hrefs, href_v)+Ei_lgth
		  );
	}
      }
    }
    break;

  case 3: /* (6, 7, 5) ou (6, 8, 3) */
    if (NV_stack [2] == 6 && (NV_stack [4] == 7 && NV_stack [6] == 5
			      || NV_stack [4] == 8 && NV_stack [6] == 3)) {
      href_aux = get_aux_href (NV_stack [1]);
      href2_aux = get_aux_href (NV_stack [3]);
      href_v = get_v_href (NV_stack [5]);

      if (href_aux && href2_aux && href_v) {
	fprintf (xml_file,
		 "<relation xlink:type=\"extended\" type=\"AUX-V\" id=\"E%iR%i\">\n",
		 sentence_id,
		 ++R
		 ); 
	/* On a une relation Auxiliaire-Verbe */
	fprintf (xml_file,"\
<auxiliaire xlink:type=\"locator\" xlink:href=\"%s\"/>\n\
<verbe xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
		 SXWORD_get (easy_hrefs, href_aux),
		 SXWORD_get (easy_hrefs, href2_aux)
		 );

	fputs ("</relation>\n", xml_file);
	fprintf (xml_file,
		 "<relation xlink:type=\"extended\" type=\"AUX-V\" id=\"E%iR%i\">\n",
		 sentence_id,
		 ++R
		 ); 
	/* On a une relation Auxiliaire-Verbe */
	fprintf (xml_file,"\
<auxiliaire xlink:type=\"locator\" xlink:href=\"%s\"/>\n\
<verbe xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
		 SXWORD_get (easy_hrefs, href2_aux),
		 SXWORD_get (easy_hrefs, href_v)
		 );
	fputs ("</relation>\n", xml_file);

	if (is_print_f_structure) {
	  printf ("AUX-V (%s, %s)\n",
		  SXWORD_get (easy_hrefs, href_aux)+Ei_lgth,
		  SXWORD_get (easy_hrefs, href2_aux)+Ei_lgth
		  );
	  printf ("AUX-V (%s, %s)\n",
		  SXWORD_get (easy_hrefs, href2_aux)+Ei_lgth,
		  SXWORD_get (easy_hrefs, href_v)+Ei_lgth
		  );
	}
      }
    }
    break;

  case 4: /* (6, 8, 7, 5) */
    if (NV_stack [2] == 6 && NV_stack [4] == 8 && NV_stack [6] == 7 && NV_stack [8] == 5) {
      href_aux = get_aux_href (NV_stack [1]);
      href2_aux = get_aux_href (NV_stack [3]);
      href3_aux = get_aux_href (NV_stack [5]);
      href_v = get_v_href (NV_stack [7]);

      if (href_aux && href2_aux && href3_aux && href_v) {
	fprintf (xml_file,
		 "<relation xlink:type=\"extended\" type=\"AUX-V\" id=\"E%iR%i\">\n",
		 sentence_id,
		 ++R
		 ); 
	/* On a une relation Auxiliaire-Verbe */
	fprintf (xml_file,"\
<auxiliaire xlink:type=\"locator\" xlink:href=\"%s\"/>\n\
<verbe xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
		 SXWORD_get (easy_hrefs, href_aux),
		 SXWORD_get (easy_hrefs, href2_aux)
		 );
	fprintf (xml_file,
		 "<relation xlink:type=\"extended\" type=\"AUX-V\" id=\"E%iR%i\">\n",
		 sentence_id,
		 ++R
		 ); 
	/* On a une relation Auxiliaire-Verbe */
	fprintf (xml_file,"\
<auxiliaire xlink:type=\"locator\" xlink:href=\"%s\"/>\n\
<verbe xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
		 SXWORD_get (easy_hrefs, href2_aux),
		 SXWORD_get (easy_hrefs, href3_aux)
		 );
	fprintf (xml_file,
		 "<relation xlink:type=\"extended\" type=\"AUX-V\" id=\"E%iR%i\">\n",
		 sentence_id,
		 ++R
		 ); 
	/* On a une relation Auxiliaire-Verbe */
	fprintf (xml_file,"\
<auxiliaire xlink:type=\"locator\" xlink:href=\"%s\"/>\n\
<verbe xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
		 SXWORD_get (easy_hrefs, href3_aux),
		 SXWORD_get (easy_hrefs, href_v)
		 );

	if (is_print_f_structure) {
	  printf ("AUX-V (%s, %s)\n",
		  SXWORD_get (easy_hrefs, href_aux)+Ei_lgth,
		  SXWORD_get (easy_hrefs, href2_aux)+Ei_lgth
		  );
	  printf ("AUX-V (%s, %s)\n",
		  SXWORD_get (easy_hrefs, href2_aux)+Ei_lgth,
		  SXWORD_get (easy_hrefs, href3_aux)+Ei_lgth
		  );
	  printf ("AUX-V (%s, %s)\n",
		  SXWORD_get (easy_hrefs, href3_aux)+Ei_lgth,
		  SXWORD_get (easy_hrefs, href_v)+Ei_lgth
		  );
	}
      }
    }

    break;

  default:
    break;
  }


  return 1;
}




static void
easy_Auxiliaire_Verbe (root)
     int root;
{
  /* On travaille uniquement ds la c_structure */
#if LOG
  fputs ("\npd = easy_bu_Auxiliaire_Verbe (", stdout);
  print_symb (sxstdout, Aij2A (root), Aij2i (root), Aij2j (root));
  fputs (")\n", stdout);
#endif /* LOG */

  spf_topological_bottom_up_walk (root,
				  easy_bu_Auxiliaire_Verbe,
				  easy_pre_bu_Auxiliaire_Verbe,
				  NULL);
  
}


static int
call_arg1_arg2 (fs_id, href_verbe)
     int fs_id, href_verbe;
{
  int  href_objet1, href_objet2, arg1_val, arg2_val, object_aij_val, de_obj_val;
  int  *dpv_ptr, *val_ptr;
  char *str;

  href_objet1 = 0;
  dpv_ptr = seek_pred (fs_id);

  if (dpv_ptr) {
    /* lexeme */
    str = lexeme_id2string [dpv_ptr [0]];

    if (str && strlen (str) == 3 && strncmp (str, "pro", 3) == 0) {
      if ((href_objet1 = Tpq2href ("cla", dpv_ptr [4], false)) == 0) {
	href_objet1 = Tpq2href ("pri_acc", dpv_ptr [4], false);
      }
    }
    else {
      /* terminal string */
      str = Tpq2str (dpv_ptr [4]);

      if (str && (strlen (str) == 3 && strncmp (str, "coo", 3) == 0 ||
		  strlen (str) == 6 && strncmp (str, "ponctw", 6) == 0)) {
	/* On regarde si fs_id a un arg1 ... */
	if (fs_is_set (fs_id, arg1_id, &val_ptr) && (arg1_val = (*val_ptr) >> FS_ID_SHIFT)) {
	  href_objet1 = call_arg1_arg2 (arg1_val, href_verbe);
	}
		    
	/* On regarde si fs_id a un arg2 ... */
	if (fs_is_set (fs_id, arg2_id, &val_ptr) && (arg2_val = (*val_ptr) >> FS_ID_SHIFT)) {
	  href_objet2 = call_arg1_arg2 (arg2_val, href_verbe);
	}
	else
	  href_objet2 = 0;

	return href_objet2 ? href_objet2 : href_objet1;
      }

      if (fs_is_set (fs_id, aij_id, &val_ptr) && (object_aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
	if ((href_objet1 = walk_aij (object_aij_val, seek_Easy_GN)) == 0)
	  href_objet1 = walk_aij (object_aij_val, seek_Easy_NV);
      }
    }
  }

  if (href_objet1) {
    /* On a une relation Objet direct-Verbe */
    fprintf (xml_file,
	     "<relation xlink:type=\"extended\" type=\"COD-V\" id=\"E%iR%i\">\n",
	     sentence_id,
	     ++R
	     );
    fprintf (xml_file,"\
<cod xlink:type=\"locator\" xlink:href=\"%s\"/>\n\
<verbe xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
	     SXWORD_get (easy_hrefs, href_objet1),
	     SXWORD_get (easy_hrefs, href_verbe)
	     );
    fputs ("</relation>\n", xml_file);

    if (is_print_f_structure) {
      printf ("COD-V (%s, %s)\n",
	      SXWORD_get (easy_hrefs, href_objet1)+Ei_lgth,
	      SXWORD_get (easy_hrefs, href_verbe)+Ei_lgth
	      );
    }
  }

  /* Ajout le 29/11/04 */
  /* On cherche un de-obj ... */
  href_objet2 = 0;

  if (fs_is_set (fs_id, de_obj_id, &val_ptr) && (de_obj_val = (*val_ptr) >> FS_ID_SHIFT)) {
    str = (dpv_ptr = seek_pred (de_obj_val)) ? lexeme_id2string [dpv_ptr [0] /* lexeme_id */] : NULL;

    if (str && strlen (str) == 3 && strncmp (str, "pro", 3) == 0) {
      if (href_objet2 = Tpq2href ("clg", dpv_ptr [4], false)) {
	/* On a une relation Objet direct-Verbe  correspondant a un genitif extrait de l'objet */
	fprintf (xml_file,
		 "<relation xlink:type=\"extended\" type=\"COD-V\" id=\"E%iR%i\">\n",
		 sentence_id,
		 ++R
		 );
	fprintf (xml_file,"\
<cod xlink:type=\"locator\" xlink:href=\"%s\"/>\n\
<verbe xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
		 SXWORD_get (easy_hrefs, href_objet2),
		 SXWORD_get (easy_hrefs, href_verbe)
		 );
	fputs ("</relation>\n", xml_file);

	if (is_print_f_structure) {
	  printf ("COD-V (%s, %s)\n",
		  SXWORD_get (easy_hrefs, href_objet2)+Ei_lgth,
		  SXWORD_get (easy_hrefs, href_verbe)+Ei_lgth
		  );
	}
      }
    }
  }

  return href_objet2 ? href_objet2 : href_objet1;
}
	
static int
call2_arg1_arg2 (fs_id, href_verbe)
     int fs_id, href_verbe;
{
  int  href_objet1, href_objet2, arg1_val, arg2_val, object_aij_val;
  int  *dpv_ptr, *val_ptr;
  char *str;

  href_objet1 = 0;
  dpv_ptr = seek_pred (fs_id);

  if (dpv_ptr) {
    /* terminal string */
    str = Tpq2str (dpv_ptr [4]);

    if (str && (strlen (str) == 3 && strncmp (str, "coo", 3) == 0 ||
		strlen (str) == 6 && strncmp (str, "ponctw", 6) == 0)) {
      /* On regarde si fs_id a un arg1 ... */
      if (fs_is_set (fs_id, arg1_id, &val_ptr) && (arg1_val = (*val_ptr) >> FS_ID_SHIFT)) {
	href_objet1 = call2_arg1_arg2 (arg1_val, href_verbe);
      }
		    
      /* On regarde si fs_id a un arg2 ... */
      if (fs_is_set (fs_id, arg2_id, &val_ptr) && (arg2_val = (*val_ptr) >> FS_ID_SHIFT)) {
	href_objet2 = call2_arg1_arg2 (arg2_val, href_verbe);
      }
      else
	href_objet2 = 0;

      return href_objet2 ? href_objet2 : href_objet1;
    }

    if (fs_is_set (fs_id, aij_id, &val_ptr) && (object_aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
      href_objet1 = walk_aij (object_aij_val, seek_Easy_NV);
    }
  }

  if (href_objet1) {
    /* On a une relation Objet direct-Verbe */
    fprintf (xml_file,
	     "<relation xlink:type=\"extended\" type=\"COD-V\" id=\"E%iR%i\">\n",
	     sentence_id,
	     ++R
	     );
    fprintf (xml_file,"\
<cod xlink:type=\"locator\" xlink:href=\"%s\"/>\n\
<verbe xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
	     SXWORD_get (easy_hrefs, href_objet1),
	     SXWORD_get (easy_hrefs, href_verbe)
	     );
    fputs ("</relation>\n", xml_file);

    if (is_print_f_structure) {
      printf ("COD-V (%s, %s)\n",
	      SXWORD_get (easy_hrefs, href_objet1)+Ei_lgth,
	      SXWORD_get (easy_hrefs, href_verbe)+Ei_lgth
	      );
    }
  }

  return href_objet1;
}
	
static int
call3_arg1_arg2 (fs_id, href_verbe)
     int fs_id, href_verbe;
{
  int  href_objet1, href_objet2, arg1_val, arg2_val, object_aij_val;
  int  *dpv_ptr, *val_ptr;
  char *str;

  href_objet1 = 0;
  dpv_ptr = seek_pred (fs_id);

  if (dpv_ptr) {
    /* terminal string */
    str = Tpq2str (dpv_ptr [4]);

    if (str && (strlen (str) == 3 && strncmp (str, "coo", 3) == 0 ||
		strlen (str) == 6 && strncmp (str, "ponctw", 6) == 0)) {
      /* On regarde si fs_id a un arg1 ... */
      if (fs_is_set (fs_id, arg1_id, &val_ptr) && (arg1_val = (*val_ptr) >> FS_ID_SHIFT)) {
	href_objet1 = call3_arg1_arg2 (arg1_val, href_verbe);
      }
		    
      /* On regarde si fs_id a un arg2 ... */
      if (fs_is_set (fs_id, arg2_id, &val_ptr) && (arg2_val = (*val_ptr) >> FS_ID_SHIFT)) {
	href_objet2 = call3_arg1_arg2 (arg2_val, href_verbe);
      }
      else
	href_objet2 = 0;

      return href_objet2 ? href_objet2 : href_objet1;
    }


    if (fs_is_set (fs_id, aij_id, &val_ptr) && (object_aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
      if ((href_objet1 = walk_aij (object_aij_val, seek_Easy_NV)) == 0)
	href_objet1 = walk_aij (object_aij_val, seek_Easy_GR);
    }
  }

  if (href_objet1) {
    /* On a une relation Objet direct-Verbe */
    fprintf (xml_file,
	     "<relation xlink:type=\"extended\" type=\"COD-V\" id=\"E%iR%i\">\n",
	     sentence_id,
	     ++R
	     );
    fprintf (xml_file,"\
<cod xlink:type=\"locator\" xlink:href=\"%s\"/>\n\
<verbe xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
	     SXWORD_get (easy_hrefs, href_objet1),
	     SXWORD_get (easy_hrefs, href_verbe)
	     );
    fputs ("</relation>\n", xml_file);

    if (is_print_f_structure) {
      printf ("COD-V (%s, %s)\n",
	      SXWORD_get (easy_hrefs, href_objet1)+Ei_lgth,
	      SXWORD_get (easy_hrefs, href_verbe)+Ei_lgth
	      );
    }
  }

  return href_objet1;
}
	

/*
<relation xlink:type="extended" type="COD-V" id="E2R3"> 
<cod xlink:type="locator" xlink:href="E2G5"/> 
<verbe xlink:type="locator" xlink:href="E2F7"/> 
</relation> 
*/
static void
easy_Objet_direct_Verbe ()
{  
  int  val, fs_id, objet_val, atom_id, x, aij_val, href_verbe, href_objet, object_aij_val, de_obj_val; 
  int  *val_ptr;
  char *str;

  /* Pour chaque f_structure ... */
  for (x = 1; x <= fs_id_stack [0]; x++) {
    fs_id = fs_id_stack [x];

    /* ... on regarde si elle a un trait local "cat = v" ... */
    if (fs_is_set (fs_id, cat_id, &val_ptr) && (atom_id = get_fs_id_atom_val (*val_ptr))) {
#ifndef ESSAI
      val = field_id2atom_field_id [cat_id] [atom_id2local_atom_id (atom_id)];
#else /* ESSAI */
      val = atom_id;
#endif /* ESSAI */

      if (val == atom_v
	    || val == atom_vSe 
	    || val == atom_vppart || val == atom_vSeppart 
	    || val == atom_vinf || val == atom_vSeinf
	    || val == atom_vprespart || val == atom_vSeprespart 
	  ) {
	/* On cherche ce verbe */
	if (fs_is_set (fs_id, aij_id, &val_ptr) && (aij_val = (*val_ptr) >> FS_ID_SHIFT))
	  /* Y'a un champ aij */
	  href_verbe = walk_aij (aij_val, seek_VERB_v);
	else
	  aij_val = href_verbe = 0;

	if (href_verbe) {
	  /* href_verbe designe Le terminal "v" domine' par "VERB" */
	  /* ... et un champ obj */
	  href_objet = 0;

	  if (fs_is_set (fs_id, obj_id, &val_ptr) && (objet_val = (*val_ptr) >> FS_ID_SHIFT)) {
	    /* ... maintenant, on cherche l'objet ... */
	    /* ... qui est un pronom (clitique accusatif ou genitif) */
	    /* ... si la f_structure objet_val a un predicat 'pro' et un aij qui reference une prod dont un fils est
	       le terminal 'cla' ou 'clg' */
	    /* ... ou un GN */
	    /* ... si  la f_structure objet_val a un aij qui reference un Easy_GN */
	    href_objet = call_arg1_arg2 (objet_val, href_verbe); /* href_objet => ca a marche' ... */
  	  }

	  /* echec ... un champ vcomp */
	  if (href_objet == 0 && fs_is_set (fs_id, vcomp_id, &val_ptr) && (objet_val = (*val_ptr) >> FS_ID_SHIFT)) {
	    /* ... maintenant, on cherche l'objet ... */
	    /* ... qui est un NV */
	    /* ... si  la f_structure objet_val a un aij qui reference un Easy_NV */
	    href_objet = call2_arg1_arg2 (objet_val, href_verbe); /* href_objet => ca a marche' ... */
	  }

	  /* Si echec ... un champ scomp */
	  if (href_objet == 0 && fs_is_set (fs_id, scomp_id, &val_ptr) && (objet_val = (*val_ptr) >> FS_ID_SHIFT)) {
	    /* On cherche ce verbe */
	    if (aij_val)
	      /* Y'a un champ aij */
	      /* ... qui reference un Easy_NV */
	      href_verbe = walk_aij (aij_val, seek_Easy_NV);
	    else
	      href_verbe = 0;

	    if (href_verbe) {
	      /* href_verbe designe Le terminal "v" domine' par "VERB" */
	      /* ... maintenant, on cherche l'objet ... */
	      /* ... qui est un NV */
	      /* ... si  la f_structure objet_val a un aij qui reference un Easy_NV ou GR */
	      href_objet = call3_arg1_arg2 (objet_val, href_verbe); /* href_objet => ca a marche' ... */
	    }
	  }
	}
      }
    }
  }
}

static int
call4_arg1_arg2 (fs_id, href_verbe)
     int fs_id, href_verbe;
{
  int  href_objet1, href_objet2, arg1_val, arg2_val, object_aij_val, de_obj_val, vcomp_val;
  int  *dpv_ptr, *val_ptr;
  char *str;

  href_objet1 = 0;
  dpv_ptr = seek_pred (fs_id);

  if (dpv_ptr) {
    /* lexeme */
    str = lexeme_id2string [dpv_ptr [0]];

    if (str && strlen (str) == 3 && strncmp (str, "pro", 3) == 0) {
      if ((href_objet1 = Tpq2href ("cld", dpv_ptr [4], false)) == 0)
	if ((href_objet1 = Tpq2href ("clg", dpv_ptr [4], false)) == 0)
	  href_objet1 = Tpq2href ("cll", dpv_ptr [4], false);
    }
    else {
      /* terminal string */
      str = Tpq2str (dpv_ptr [4]);

      if (str && (strlen (str) == 4 && strncmp (str, "prep", 4) == 0)) {
	/* On descend sur son vcomp */
	if (fs_is_set (fs_id, vcomp_id, &val_ptr) && (vcomp_val = (*val_ptr) >> FS_ID_SHIFT)) {
	  href_objet1 = call4_arg1_arg2 (vcomp_val, href_verbe);
	}

	return href_objet1;
      }

      if (str && (strlen (str) == 3 && strncmp (str, "coo", 3) == 0 ||
		  strlen (str) == 6 && strncmp (str, "ponctw", 6) == 0)) {
	/* On regarde si fs_id a un arg1 ... */
	if (fs_is_set (fs_id, arg1_id, &val_ptr) && (arg1_val = (*val_ptr) >> FS_ID_SHIFT)) {
	  href_objet1 = call4_arg1_arg2 (arg1_val, href_verbe);
	}
		    
	/* On regarde si fs_id a un arg2 ... */
	if (fs_is_set (fs_id, arg2_id, &val_ptr) && (arg2_val = (*val_ptr) >> FS_ID_SHIFT)) {
	  href_objet2 = call4_arg1_arg2 (arg2_val, href_verbe);
	}
	else
	  href_objet2 = 0;

	return href_objet2 ? href_objet2 : href_objet1;
      }

      if (fs_is_set (fs_id, aij_id, &val_ptr) && (object_aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
	href_objet1 = walk_aij (object_aij_val, seek_Easy_GP);

	if (href_objet1 == 0)
	  href_objet1 = walk_aij (object_aij_val, seek_Easy_PV);

	if (href_objet1 == 0)
	  href_objet1 = walk_aij (object_aij_val, seek_Easy_NV);
      }
    }
  }


  if (href_objet1) {
    /* On a une relation Complement_Verbe */
    fprintf (xml_file,
	     "<relation xlink:type=\"extended\" type=\"CPL-V\" id=\"E%iR%i\">\n",
	     sentence_id,
	     ++R
	     );
    fprintf (xml_file,"\
<complement xlink:type=\"locator\" xlink:href=\"%s\"/>\n\
<verbe xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
	     SXWORD_get (easy_hrefs, href_objet1),
	     SXWORD_get (easy_hrefs, href_verbe)
	     );
    fputs ("</relation>\n", xml_file);

    if (is_print_f_structure) {
      printf ("CPL-V (%s, %s)\n",
	      SXWORD_get (easy_hrefs, href_objet1)+Ei_lgth,
	      SXWORD_get (easy_hrefs, href_verbe)+Ei_lgth
	      );
    }
  }

  return href_objet1;
}
	

/*
<relation xlink:type="extended" type="CPL-V" id="E1R5"> 
<complement xlink:type="locator" xlink:href="E1G1"/> 
<verbe xlink:type="locator" xlink:href="E1F9"/> 
</relation> 
*/
static void
easy_Complement_Verbe ()
{  
  int  val, fs_id, objet_val, atom_id, x, aij_val, href_verbe, href_objet, i, cur, top, adjunct_val, object_aij_val, adjunct_aij_val; 
  int  field_id;
  int  *dpv_ptr, *val_ptr;
  char *str;

  /* Pour chaque f_structure ... */
  for (x = 1; x <= fs_id_stack [0]; x++) {
    fs_id = fs_id_stack [x];

    /* ... on regarde si elle a un trait local "cat = v" ... */
    if (fs_is_set (fs_id, cat_id, &val_ptr) && (atom_id = get_fs_id_atom_val (*val_ptr))) {
#ifndef ESSAI
      val = field_id2atom_field_id [cat_id] [atom_id2local_atom_id (atom_id)];
#else /* ESSAI */
      val = atom_id;
#endif /* ESSAI */

      if (val == atom_v
	    || val == atom_vSe 
	    || val == atom_vppart || val == atom_vSeppart 
	    || val == atom_vinf || val == atom_vSeinf
	    || val == atom_vprespart || val == atom_vSeprespart 
	  ) {
	if (fs_is_set (fs_id, aij_id, &val_ptr) && (aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
	  /* Y'a un champ aij */
	  /* On cherche le mot "v" domine' par "VERB" */
	  if (href_verbe = walk_aij (aij_val, seek_VERB_v)) {
	    /* ... trouve' */
	    /* ... et un champ ".*-obj" */
	    /* ... et un champ "*vcomp" */
	    /* ... et un champ "*scomp" */
	    field_id = 0;

	    while ((field_id = sxba_scan (obj_vcomp_scomp_ids_set, field_id)) > 0) {
	      if (fs_is_set (fs_id, field_id, &val_ptr) && (objet_val = (*val_ptr) >> FS_ID_SHIFT)) {
		/* ... trouve' */
		if (field_id == vcomp_id || field_id == scomp_id) {
		  /* Ds le cas vcomp ou scomp ... */
		  /* ... condition supplementaire il doit y avoir un obj ds la structure */
		  if (fs_is_set (fs_id, obj_id, &val_ptr) && ((*val_ptr) >> FS_ID_SHIFT)) {
		    /* ... oui */
		  }
		  else
		    continue;
		}

		/* ... maintenant, on cherche le complement ... */
		/* ... qui est un pronom (clitique datif ou genitif ou l0catif) */
		/* ... si la f_structure objet_val a un predicat 'pro' et un aij qui reference une prod dont un fils est
		   le terminal 'cld' 'clg' ou 'cll' */
		/* ... ou un GP */
		/* ... si  la f_structure objet_val a un aij qui reference un Easy_GP */
		/* ... ou un PV */
		/* ... si  la f_structure objet_val a un aij qui reference un Easy_PV */
		href_objet = call4_arg1_arg2 (objet_val, href_verbe);
	      }
	    }

	    if (fs_is_set (fs_id, adjunct_id, &val_ptr) && (objet_val = (*val_ptr) >> FS_ID_SHIFT)) {
	      /* ... et un champ "adjunct" */
	      for (cur = XH_X (XH_ufs_hd, objet_val), top = XH_X (XH_ufs_hd, objet_val+1);
		   cur < top;
		   cur++) {
		adjunct_val = XH_list_elem (XH_ufs_hd, cur);

		if (adjunct_val < 0)
		  break;

		/* On cherche si adjunct_val contient le trait "pcas = y" */
		if (fs_is_set (adjunct_val, pcas_id, &val_ptr) && (atom_id = get_fs_id_atom_val (*val_ptr))) {
		  /* Y'a un champ "pcas", peu importe sa valeur */
		  /* ... maintenant, on cherche le complement ... */
		  /* ... qui est un pronom (clitique datif ou genitif) */
		  /* ... si la f_structure adjunct_val a un predicat 'pro' et un aij qui reference une prod dont un fils est
		     le terminal 'cld' ou 'clg' */
		  /* ... ou un GP */
		  /* ... si  la f_structure adjunct_val a un aij qui reference un Easy_GP */
		  str = (dpv_ptr = seek_pred (objet_val)) ? lexeme_id2string [dpv_ptr [0] /* lexeme_id */] : NULL;

		  if (str && strlen (str) == 3 && strncmp (str, "pro", 3) == 0) {
		    if ((href_objet = Tpq2href ("cld", dpv_ptr [4], false)) == 0)
		      href_objet = Tpq2href ("clg", dpv_ptr [4], false);
		  }
		  else {
		    if (fs_is_set (adjunct_val, aij_id, &val_ptr) && (object_aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
		      href_objet = walk_aij (object_aij_val, seek_Easy_GP);
		    }
		    else
		      href_objet = 0;
		  }

		  if (href_objet) {
		    /* On a une relation Complement_Verbe */
		    fprintf (xml_file,
			     "<relation xlink:type=\"extended\" type=\"CPL-V\" id=\"E%iR%i\">\n",
			     sentence_id,
			     ++R
			     );
		    fprintf (xml_file,"\
<complement xlink:type=\"locator\" xlink:href=\"%s\"/>\n\
<verbe xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
			     SXWORD_get (easy_hrefs, href_objet),
			     SXWORD_get (easy_hrefs, href_verbe)
			     );
		    fputs ("</relation>\n", xml_file);

		    if (is_print_f_structure) {
		      printf ("CPL-V (%s, %s)\n",
			      SXWORD_get (easy_hrefs, href_objet)+Ei_lgth,
			      SXWORD_get (easy_hrefs, href_verbe)+Ei_lgth
			      );
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
}

/*
<relation xlink:type="extended" type="MOD-V" id="E2R5"> 
<modifieur xlink:type="locator" xlink:href="E2F6"/> 
<verbe xlink:type="locator" xlink:href="E2F7"/> 
</relation> 
*/
static void
easy_Modifieur_Verbe ()
{
  int  val, fs_id, objet_val, atom_id, x, aij_val, href_verbe, href_objet, i, cur, top, adjunct_val, object_aij_val, atom_val; 
  int  local_atom_id;
  int  *val_ptr;
  char *str;

  /* Pour chaque f_structure ... */
  for (x = 1; x <= fs_id_stack [0]; x++) {
    fs_id = fs_id_stack [x];

    /* ... on regarde si elle a un trait local "cat = v" ... */
    if (fs_is_set (fs_id, cat_id, &val_ptr) && (atom_id = get_fs_id_atom_val (*val_ptr))) {
#ifndef ESSAI
      val = field_id2atom_field_id [cat_id] [atom_id2local_atom_id (atom_id)];
#else /* ESSAI */
      val = atom_id;
#endif /* ESSAI */

      if (val == atom_v
	    || val == atom_vSe 
	    || val == atom_vppart || val == atom_vSeppart 
	    || val == atom_vinf || val == atom_vSeinf
	    || val == atom_vprespart || val == atom_vSeprespart 
	  ) {
	/* ... oui */
	if (fs_is_set (fs_id, aij_id, &val_ptr) && (aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
	  /* Y'a un champ aij */
	  /* On cherche le mot "v" domine' par "VERB" */
	  if (href_verbe = walk_aij (aij_val, seek_VERB_v)) {
	    /* ... trouve' */
	    /* ... et un champ "adjunct" */
	    if (fs_is_set (fs_id, adjunct_id, &val_ptr) && (objet_val = (*val_ptr) >> FS_ID_SHIFT)) {
	      /* ... trouve' */
	      for (cur = XH_X (XH_ufs_hd, objet_val), top = XH_X (XH_ufs_hd, objet_val+1);
		   cur < top;
		   cur++) {
		adjunct_val = XH_list_elem (XH_ufs_hd, cur);

		if (adjunct_val < 0)
		  break;

		if (fs_is_set (adjunct_val, aij_id, &val_ptr) && (object_aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
		  /* Y'a un champ aij */
		  /* On cherche si adjunct_val NE CONTIENT PAS le trait "pcas = y" */
		  if (!fs_is_set (adjunct_val, pcas_id, &val_ptr) /* pas de champ pcas */
		      || ((*val_ptr) >> FS_ID_SHIFT) == 0 /* ou vide !! */
		      ) {
		    /* ... oui */
		    /* ... maintenant, on cherche le modifieur ... */
		    /* ... qui est un GN */
		    /* ... si  la f_structure adjunct_val a un aij qui reference un Easy_GN */
		    if (href_objet = walk_aij (object_aij_val, seek_Easy_GN)) {
		      /* On a une relation Modifieur_Verbe */
		      fprintf (xml_file,
			       "<relation xlink:type=\"extended\" type=\"MOD-V\" id=\"E%iR%i\">\n",
			       sentence_id,
			       ++R
			       );
		      fprintf (xml_file,"\
<modifieur xlink:type=\"locator\" xlink:href=\"%s\"/>\n\
<verbe xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
			       SXWORD_get (easy_hrefs, href_objet),
			       SXWORD_get (easy_hrefs, href_verbe)
			       );
		      fputs ("</relation>\n", xml_file);

		      if (is_print_f_structure) {
			printf ("MOD-V (%s, %s)\n",
				SXWORD_get (easy_hrefs, href_objet)+Ei_lgth,
				SXWORD_get (easy_hrefs, href_verbe)+Ei_lgth
				);
		      }
		    }
		  }

		  /* On cherche si adjunct_val contient le trait "cat = adv|etr" */
		  if (fs_is_set (adjunct_val, cat_id, &val_ptr) && (atom_id = get_fs_id_atom_val (*val_ptr))) {
#ifndef ESSAI
		    local_atom_id = field_id2atom_field_id [cat_id] [atom_id2local_atom_id (atom_id)];
#else /* ESSAI */
		    local_atom_id = atom_id;
#endif /* ESSAI */

		    if (local_atom_id == atom_adv || local_atom_id == atom_etr) {
		      /* ... oui */
		      /* ... maintenant, on cherche le modifieur ... */
		      /* ... qui est un GR */
		      /* ... si  la f_structure adjunct_val a un aij qui reference un Easy_GR */
		      if (href_objet = walk_aij (object_aij_val, seek_Easy_GR)) {
			/* On a une relation Modifieur_Verbe */
			fprintf (xml_file,
				 "<relation xlink:type=\"extended\" type=\"MOD-V\" id=\"E%iR%i\">\n",
				 sentence_id,
				 ++R
				 );
			fprintf (xml_file,"\
<modifieur xlink:type=\"locator\" xlink:href=\"%s\"/>\n\
<verbe xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
				 SXWORD_get (easy_hrefs, href_objet),
				 SXWORD_get (easy_hrefs, href_verbe)
				 );
			fputs ("</relation>\n", xml_file);

			if (is_print_f_structure) {
			  printf ("MOD-V (%s, %s)\n",
				  SXWORD_get (easy_hrefs, href_objet)+Ei_lgth,
				  SXWORD_get (easy_hrefs, href_verbe)+Ei_lgth
				  );
			}
		      }
		    }
		  }
		}
	      }
	    }

	    if (atom_val = get_atomic_field_val (fs_id, neg_id)) {
	      /* Faut-il verifier que atom_val == + ?? */
	      /* On cherche un "GR[2]" */
	      if (href_objet = walk_aij (aij_val, seek_Easy_GR)) {
		/* ... trouve' */
		/* On a une relation Modifieur_Verbe */
		fprintf (xml_file,
			 "<relation xlink:type=\"extended\" type=\"MOD-V\" id=\"E%iR%i\">\n",
			 sentence_id,
			 ++R
			 );
		fprintf (xml_file,"\
<modifieur xlink:type=\"locator\" xlink:href=\"%s\"/>\n\
<verbe xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
			 SXWORD_get (easy_hrefs, href_objet),
			 SXWORD_get (easy_hrefs, href_verbe)
			 );
		fputs ("</relation>\n", xml_file);

		if (is_print_f_structure) {
		  printf ("MOD-V (%s, %s)\n",
			  SXWORD_get (easy_hrefs, href_objet)+Ei_lgth,
			  SXWORD_get (easy_hrefs, href_verbe)+Ei_lgth
			  );
		}
	      }
	    }
	  }
	}
      }
    }
  }
}

/*
<relation xlink:type="extended" type="COMP" id="E3R4"> 
<complementeur xlink:type="locator" xlink:href="E3F5"/> 
<verbe xlink:type="locator" xlink:href="E3G3"/> 
</relation> 
*/
static void
easy_Complementeur ()
{
  int x, fs_id, i, objet_val, aij_val, object_aij_val, href1, href2, field_id;
  int *val_ptr;

  /* Pour chaque f_structure ... */
  for (x = 1; x <= fs_id_stack [0]; x++) {
    fs_id = fs_id_stack [x];

    /* On cherche un champ ".*scomp" */
    field_id = 0;

    while ((field_id = sxba_scan (scomp_ids_set, field_id)) > 0) {
      if (fs_is_set (fs_id, field_id, &val_ptr) && (objet_val = (*val_ptr) >> FS_ID_SHIFT)) {
	/* ... trouve' */
	if (fs_is_set (objet_val, aij_id, &val_ptr) && (object_aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
	  /* Y'a un champ aij ds objet_val */
	  /* On cherche le mot "csu" domine' par "WS" */
	  if (href1 = walk_aij (object_aij_val, seek_WS_csu)) {
	    /* ... trouve' */
	    /* ... Easy_NV.* ou GR */
	    if ((href2 = walk_aij (object_aij_val, seek_Easy_NV)) || (href2 = walk_aij (object_aij_val, seek_Easy_GR))) {
	      /* On a une relation Complementeur */
	      fprintf (xml_file,
		       "<relation xlink:type=\"extended\" type=\"COMP\" id=\"E%iR%i\">\n",
		       sentence_id,
		       ++R
		       );
	      fprintf (xml_file,"\
<complementeur xlink:type=\"locator\" xlink:href=\"%s\"/>\n\
<verbe xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
		       SXWORD_get (easy_hrefs, href1),
		       SXWORD_get (easy_hrefs, href2)
		       );
	      fputs ("</relation>\n", xml_file);

	      if (is_print_f_structure) {
		printf ("COMP (%s, %s)\n",
			SXWORD_get (easy_hrefs, href1)+Ei_lgth,
			SXWORD_get (easy_hrefs, href2)+Ei_lgth
			);
	      }
	    }
	  }
	}
      }
    }

    /* On cherche un champ ".*vcomp" */
    field_id = 0;

    while ((field_id = sxba_scan (vcomp_ids_set, field_id)) > 0) {
      if (fs_is_set (fs_id, field_id, &val_ptr) && (objet_val = (*val_ptr) >> FS_ID_SHIFT)) {
	/* ... trouve' */
	if (fs_is_set (fs_id, aij_id, &val_ptr) && (aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
	  /* Y'a un champ aij ds la structure principale */
	  /* On cherche le mot "prep" domine' par "Easy_PV" */
	  if (href1 = walk_aij (aij_val, seek_Easy_PV_prep)) {
	    /* ... trouve' */
	    if (fs_is_set (objet_val, aij_id, &val_ptr) && (object_aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
	      /* Y'a un champ aij ds objet_val */
	      /* On y cherche Easy_NV.* */
	      if (href2 = walk_aij (object_aij_val, seek_Easy_NV)) {
		/* On a une relation Complementeur */
		fprintf (xml_file,
			 "<relation xlink:type=\"extended\" type=\"COMP\" id=\"E%iR%i\">\n",
			 sentence_id,
			 ++R
			 );
		fprintf (xml_file,"\
<complementeur xlink:type=\"locator\" xlink:href=\"%s\"/>\n\
<verbe xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
			 SXWORD_get (easy_hrefs, href1),
			 SXWORD_get (easy_hrefs, href2)
			 );
		fputs ("</relation>\n", xml_file);

		if (is_print_f_structure) {
		  printf ("COMP (%s, %s)\n",
			  SXWORD_get (easy_hrefs, href1)+Ei_lgth,
			  SXWORD_get (easy_hrefs, href2)+Ei_lgth
			  );
		}
	      }
	    }
	  }
	}
      }
    }
  }
}

/*
<relation xlink:type="extended" type="ATB-SO" id="E6R30"> 
<attribut xlink:type="locator" xlink:href="E6G31"/> 
<verbe xlink:type="locator" xlink:href="E6G29"/> 
<s-o xlink:type="locator" xlink:href="sujet"/> 
</relation> 
*/
static void
easy_Attribut_Sujet_objet ()
{
  int  fs_id, objet_val, atom_id, x, aij_val, href1, href2, href3, i, cur, top, subj_val, val, object_aij_val, field_id; 
  int  adjunct_val, adjunct_item, pcas_val;
  int  *val_ptr;

  /* Pour chaque f_structure ... */
  for (x = 1; x <= fs_id_stack [0]; x++) {
    fs_id = fs_id_stack [x];

    /* ... on regarde si elle a un trait local "cat = v" ... */
    if (fs_is_set (fs_id, cat_id, &val_ptr) && (atom_id = get_fs_id_atom_val (*val_ptr))) {
#ifndef ESSAI
      val = field_id2atom_field_id [cat_id] [atom_id2local_atom_id (atom_id)];
#else /* ESSAI */
      val = atom_id;
#endif /* ESSAI */

      if (val == atom_v
	    || val == atom_vSe 
	    || val == atom_vppart || val == atom_vSeppart 
	    || val == atom_vinf || val == atom_vSeinf
	    || val == atom_vprespart || val == atom_vSeprespart 
	  ) {
	/* ... oui */
	/* ... et un champ ".*acomp" */
	field_id = 0;

	while ((field_id = sxba_scan (acomp_ids_set, field_id)) > 0) {
	  if (fs_is_set (fs_id, field_id, &val_ptr) && (objet_val = (*val_ptr) >> FS_ID_SHIFT)) {
	    /* ... oui */
	    if (fs_is_set (fs_id, aij_id, &val_ptr) && (aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
	      /* Y'a un champ aij ds la structure principale */
	      if (fs_is_set (objet_val, aij_id, &val_ptr) && (object_aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
		/* Y'a un champ aij ds objet_val */
		if (field_id == acomp_id && (href1 = walk_aij (object_aij_val, seek_Easy_GA)) ||
		    field_id == acomp_id && (href1 = walk_aij (object_aij_val, seek_Easy_GN)) ||
		    field_id != acomp_id /* de_acomp ou pour_acomp */&& (href1 = walk_aij (object_aij_val, seek_Easy_GP))) {
		  /* Easy_GA.* pointe' par acomp ou Easy_GP pointes par de_acomp ou pour_acomp */
		  if (href2 = walk_aij (aij_val, seek_Easy_NV)) {
		    /* Easy_NV.* pointe' par la principale */
		    /* On regarde si acomp contient un champ "subj" indice' */
		    href3 = 0;

		    if (fs_is_set (objet_val, subj_id, &val_ptr) && (subj_val = (*val_ptr) >> FS_ID_SHIFT)) {
		      /* Y'a un champ subj */
		      if (XH_list_elem (XH_fs_hd, XH_X (XH_fs_hd, subj_val+1)-1) < 0) {
			/* subj est partagee */
			/* On regarde s'il y a un subj ou un obj local partage' avec subj_val */
			if (fs_is_set (fs_id, subj_id, &val_ptr) && (val = (*val_ptr) >> FS_ID_SHIFT)) {
			  /* Y'a un subj local */
			  if (val == subj_val)
			    /* partage' */
			    href3 = 1; /* sujet */
			  else {
			    if (fs_is_set (fs_id, obj_id, &val_ptr) && (val = (*val_ptr) >> FS_ID_SHIFT)) {
			      /* Y'a un obj local */
			      if (val == subj_val)
				/* partage' */
				href3 = -1; /* objet */
			    }
			  }
			}
		      }
		    }
		
		    /* On a une relation Attribut-Sujet/Objet */
		    fprintf (xml_file,
			     "<relation xlink:type=\"extended\" type=\"ATB-SO\" id=\"E%iR%i\">\n",
			     sentence_id,
			     ++R
			     );
		    fprintf (xml_file,"\
<attribut xlink:type=\"locator\" xlink:href=\"%s\"/>\n\
<verbe xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
			     SXWORD_get (easy_hrefs, href1),
			     SXWORD_get (easy_hrefs, href2)
			     );

		    if (href3)
		      fprintf (xml_file,"\
<s-o xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
			       (href3 == 1) ? "sujet" : "objet"
			       );
		    fputs ("</relation>\n", xml_file);

		    if (is_print_f_structure) {
		      printf ("ATB-SO (%s, %s%s)\n",
			      SXWORD_get (easy_hrefs, href1)+Ei_lgth,
			      SXWORD_get (easy_hrefs, href2)+Ei_lgth,
			      href3 ? ((href3 == 1) ? ", sujet" : ", objet") : ""
			      );
		    }
		  }
		}
	      }
	    }
	  }
	}

	/* ... et un champ "adjunct" */
	if (fs_is_set (fs_id, adjunct_id, &val_ptr) && (adjunct_val = (*val_ptr) >> FS_ID_SHIFT)) {
	  /* ... trouve' */
	  /* On parcourt les adjunct ... */
	  for (cur = XH_X (XH_ufs_hd, adjunct_val), top = XH_X (XH_ufs_hd, adjunct_val+1);
	       cur < top;
	       cur++) {
	    adjunct_item = XH_list_elem (XH_ufs_hd, cur);

	    if (adjunct_item < 0)
	      break;

	    /* On cherche un pcas ... qui vaut comme */
	    if ((pcas_val = get_atomic_field_val (adjunct_item, pcas_id)) && pcas_val == atom_comme) {
	      if (fs_is_set (adjunct_item, aij_id, &val_ptr) && (aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
		/* On cherche un Easy_GP dans ses aij */
		if (href1 = walk_aij (aij_val, seek_Easy_GP)) {
		  if (fs_is_set (fs_id, aij_id, &val_ptr) && (aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
		    /* Y'a un champ aij ds la structure principale */
		    /* On y cherche un Easy_NV */
		    if (href2 = walk_aij (aij_val, seek_Easy_NV)) {
		      if (fs_is_set (fs_id, obj_id, &val_ptr) && ((*val_ptr) >> FS_ID_SHIFT))
			href3 = -1; /* objet */
		      else
			href3 = 1; /* sujet */

		      /* On a une relation Attribut-Sujet/Objet */
		      fprintf (xml_file,
			       "<relation xlink:type=\"extended\" type=\"ATB-SO\" id=\"E%iR%i\">\n",
			       sentence_id,
			       ++R
			       );
		      fprintf (xml_file,"\
<attribut xlink:type=\"locator\" xlink:href=\"%s\"/>\n\
<verbe xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
			       SXWORD_get (easy_hrefs, href1),
			       SXWORD_get (easy_hrefs, href2)
			       );

		      if (href3)
			fprintf (xml_file,"\
<s-o xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
				 (href3 == 1) ? "sujet" : "objet"
				 );
		      fputs ("</relation>\n", xml_file);

		      if (is_print_f_structure) {
			printf ("ATB-SO (%s, %s%s)\n",
				SXWORD_get (easy_hrefs, href1)+Ei_lgth,
				SXWORD_get (easy_hrefs, href2)+Ei_lgth,
				href3 ? ((href3 == 1) ? ", sujet" : ", objet") : ""
				);
		      }
		    }
		  }
		}
	      }
	    }
	  }
	}
	

	if (fs_is_set (fs_id, field_id, &val_ptr) && (objet_val = (*val_ptr) >> FS_ID_SHIFT)) {
	}



      }
    }
  }
}

/*<relation xlink:type="extended" type="MOD-N" id="E1R6"> 
<modifieur xlink:type="locator" xlink:href="E1G7"/> 
<nom xlink:type="locator" xlink:href="E1F12"/> 
<à-propager xlink:type="locator" xlink:href="faux"/> 
</relation> 
*/
static void
easy_Modifieur_Nom ()
{
  int x, fs_id, adjunct_val, atom_id, aij_val, adjunct_aij_val, cur, top, href1, href2, adjunct_item, local_atom_id; 
  int *val_ptr;

  /* Pour chaque f_structure ... */
  for (x = 1; x <= fs_id_stack [0]; x++) {
    fs_id = fs_id_stack [x];

    /* ... on regarde si elle a un trait local "cat = nc" ... */
    if (fs_is_set (fs_id, cat_id, &val_ptr) && (atom_id = get_fs_id_atom_val (*val_ptr))) {
#ifndef ESSAI
      local_atom_id = field_id2atom_field_id [cat_id] [atom_id2local_atom_id (atom_id)];
#else /* ESSAI */
      local_atom_id = atom_id;
#endif /* ESSAI */

      if (local_atom_id == atom_nc || local_atom_id == atom_etr) {
	/* ... trouve' */
	/* ... et un champ "adjunct" */
	if (fs_is_set (fs_id, adjunct_id, &val_ptr) && (adjunct_val = (*val_ptr) >> FS_ID_SHIFT)) {
	  /* ... trouve' */
	  if (fs_is_set (fs_id, aij_id, &val_ptr) && (aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
	    /* Y'a un champ aij */
	    /* On cherche le mot "nc" domine' par "N" */
	    if (href2 = walk_aij (aij_val, seek_N_nc)) {
	      /* On parcourt les adjunct ... */
	      for (cur = XH_X (XH_ufs_hd, adjunct_val), top = XH_X (XH_ufs_hd, adjunct_val+1);
		   cur < top;
		   cur++) {
		adjunct_item = XH_list_elem (XH_ufs_hd, cur);

		if (adjunct_item < 0)
		  break;
		
		if (fs_is_set (adjunct_item, aij_id, &val_ptr) && (adjunct_aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
		  /* Y'a un champ aij */
		  if ((href1 = walk_aij (adjunct_aij_val, seek_Easy_GA_or_adj))
		      || (href1 = walk_aij (adjunct_aij_val, seek_Easy_GP))
		      || (href1 = walk_aij (adjunct_aij_val, seek_Easy_GN))
		      ) {
		    /* On a une relation Modifieur_Nom */
		    fprintf (xml_file,
			     "<relation xlink:type=\"extended\" type=\"MOD-N\" id=\"E%iR%i\">\n",
			     sentence_id,
			     ++R
			     );
		    fprintf (xml_file,"\
<modifieur xlink:type=\"locator\" xlink:href=\"%s\"/>\n\
<nom xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
			     SXWORD_get (easy_hrefs, href1),
			     SXWORD_get (easy_hrefs, href2)
			     );
 
		    fputs ("<à-propager xlink:type=\"locator\" xlink:href=\"faux\"/>\n", xml_file);
  
		    fputs ("</relation>\n", xml_file);

		    if (is_print_f_structure) {
		      printf ("MOD-N (%s, %s, false)\n",
			      SXWORD_get (easy_hrefs, href1)+Ei_lgth,
			      SXWORD_get (easy_hrefs, href2)+Ei_lgth
			      );
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
}

/*
<relation xlink:type="extended" type="MOD-A" id="E2R3"> 
<modifieur xlink:type="locator" xlink:href="E2G3"/> 
<adjectif xlink:type="locator" xlink:href="E2G2"/> 
</relation> 
*/
static void
easy_Modifieur_Adjectif ()
{
  int x, fs_id, adjunct_val, atom_id, aij_val, adjunct_aij_val, cur, top, href1, href2, adjunct_item, local_atom_id; 
  int *val_ptr;

  /* Pour chaque f_structure ... */
  for (x = 1; x <= fs_id_stack [0]; x++) {
    fs_id = fs_id_stack [x];

    /* ... on regarde si elle a un trait local "cat = adj" ... */
    if (fs_is_set (fs_id, cat_id, &val_ptr) && (atom_id = get_fs_id_atom_val (*val_ptr))) {
#ifndef ESSAI
      local_atom_id = field_id2atom_field_id [cat_id] [atom_id2local_atom_id (atom_id)];
#else /* ESSAI */
      local_atom_id = atom_id;
#endif /* ESSAI */

      if (local_atom_id == atom_adj || local_atom_id == atom_etr) {
	/* ... trouve' */
	/* ... et un champ "adjunct" */
	if (fs_is_set (fs_id, adjunct_id, &val_ptr) && (adjunct_val = (*val_ptr) >> FS_ID_SHIFT)) {
	  /* ... trouve' */
	  if (fs_is_set (fs_id, aij_id, &val_ptr) && (aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
	    /* Y'a un champ aij */
	    /* On cherche le mot "adj" domine' par "GA" */
	    if (href2 = walk_aij (aij_val, seek_Easy_GA_adj)) {
	      /* On parcourt les adjunct ... */
	      for (cur = XH_X (XH_ufs_hd, adjunct_val), top = XH_X (XH_ufs_hd, adjunct_val+1);
		   cur < top;
		   cur++) {
		adjunct_item = XH_list_elem (XH_ufs_hd, cur);

		if (adjunct_item < 0)
		  break;
		
		if (fs_is_set (adjunct_item, aij_id, &val_ptr) && (adjunct_aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
		  /* Y'a un champ aij */
		  if ((href1 = walk_aij (adjunct_aij_val, seek_Easy_GR))) {
		    /* On a une relation Modifieur_Nom */
		    fprintf (xml_file,
			     "<relation xlink:type=\"extended\" type=\"MOD-A\" id=\"E%iR%i\">\n",
			     sentence_id,
			     ++R
			     );
		    fprintf (xml_file,"\
<modifieur xlink:type=\"locator\" xlink:href=\"%s\"/>\n\
<adjectif xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
			     SXWORD_get (easy_hrefs, href1),
			     SXWORD_get (easy_hrefs, href2)
			     );
		    fputs ("</relation>\n", xml_file);

		    if (is_print_f_structure) {
		      printf ("MOD-A (%s, %s)\n",
			      SXWORD_get (easy_hrefs, href1)+Ei_lgth,
			      SXWORD_get (easy_hrefs, href2)+Ei_lgth
			      );
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
}

/*
<relation xlink:type="extended" type="MOD-R" id="E6R43"> 
<modifieur xlink:type="locator" xlink:href="E6G25"/> 
<adverbe xlink:type="locator" xlink:href="E6G26"/> 
</relation> 
*/
static void
easy_Modifieur_Adverbe ()
{
  int x, fs_id, adjunct_val, atom_id, aij_val, adjunct_aij_val, cur, top, href1, href2, adjunct_item, local_atom_id; 
  int *val_ptr;

  /* Pour chaque f_structure ... */
  for (x = 1; x <= fs_id_stack [0]; x++) {
    fs_id = fs_id_stack [x];

    /* ... on regarde si elle a un trait local "cat = nc" ... */
    if (fs_is_set (fs_id, cat_id, &val_ptr) && (atom_id = get_fs_id_atom_val (*val_ptr))) {
#ifndef ESSAI
      local_atom_id = field_id2atom_field_id [cat_id] [atom_id2local_atom_id (atom_id)];
#else /* ESSAI */
      local_atom_id = atom_id;
#endif /* ESSAI */

      if (local_atom_id == atom_adv || local_atom_id == atom_etr) {
	/* ... trouve' */
	/* ... et un champ "adjunct" */
	if (fs_is_set (fs_id, adjunct_id, &val_ptr) && (adjunct_val = (*val_ptr) >> FS_ID_SHIFT)) {
	  /* ... trouve' */
	  if (fs_is_set (fs_id, aij_id, &val_ptr) && (aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
	    /* Y'a un champ aij */
	    /* On cherche le mot "nc" domine' par "N" */
	    if (href2 = walk_aij (aij_val, seek_Easy_GR_adv)) {
	      /* On parcourt les adjunct ... */
	      for (cur = XH_X (XH_ufs_hd, adjunct_val), top = XH_X (XH_ufs_hd, adjunct_val+1);
		   cur < top;
		   cur++) {
		adjunct_item = XH_list_elem (XH_ufs_hd, cur);

		if (adjunct_item < 0)
		  break;
		
		if (fs_is_set (adjunct_item, aij_id, &val_ptr) && (adjunct_aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
		  /* Y'a un champ aij */
		  if ((href1 = walk_aij (adjunct_aij_val, seek_Easy_GR))) {
		    /* On a une relation Modifieur_Nom */
		    fprintf (xml_file,
			     "<relation xlink:type=\"extended\" type=\"MOD-R\" id=\"E%iR%i\">\n",
			     sentence_id,
			     ++R
			     );
		    fprintf (xml_file,"\
<modifieur xlink:type=\"locator\" xlink:href=\"%s\"/>\n\
<adverbe xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
			     SXWORD_get (easy_hrefs, href1),
			     SXWORD_get (easy_hrefs, href2)
			     );
		    fputs ("</relation>\n", xml_file);

		    if (is_print_f_structure) {
		      printf ("MOD-R (%s, %s)\n",
			      SXWORD_get (easy_hrefs, href1)+Ei_lgth,
			      SXWORD_get (easy_hrefs, href2)+Ei_lgth
			      );
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
}

/*
<relation xlink:type="extended" type="MOD-P" id="E3R3"> 
<modifieur xlink:type="locator" xlink:href="E3F3"/> 
<preposition xlink:type="locator" xlink:href="E3F4"/> 
</relation> 
*/
static void
easy_Modifieur_Preposition ()
{
  int  x, fs_id, adjunct_val, atom_id, aij_val, adjunct_aij_val, cur, top, href1, href2, adjunct_item; 
  int  *val_ptr, *dpv_ptr;
  char *str;

  /* Pour chaque f_structure ... */
  for (x = 1; x <= fs_id_stack [0]; x++) {
    fs_id = fs_id_stack [x];

    /* ... on regarde si sa tete est "prep" ... */
    if ((dpv_ptr = seek_pred (fs_id)) && (str = Tpq2str (dpv_ptr [4])) && (strlen (str) == 4) && (strncmp (str, "prep", 4))) {
      /* ... trouve' */
      /* ... et un champ "adjunct" */
      if (fs_is_set (fs_id, adjunct_id, &val_ptr) && (adjunct_val = (*val_ptr) >> FS_ID_SHIFT)) {
	/* ... trouve' */
	/* Le href2 est la tete du fs_id */
	if (href2 = Xpq2easy_href [dpv_ptr [4]]) {
	  /* On parcourt les adjunct ... */
	  for (cur = XH_X (XH_ufs_hd, adjunct_val), top = XH_X (XH_ufs_hd, adjunct_val+1);
	       cur < top;
	       cur++) {
	    adjunct_item = XH_list_elem (XH_ufs_hd, cur);

	    if (adjunct_item < 0)
	      break;
		
	    if (fs_is_set (adjunct_item, aij_id, &val_ptr) && (adjunct_aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
	      /* Y'a un champ aij */
	      if ((href1 = walk_aij (adjunct_aij_val, seek_Easy_GR))) {
		/* On a une relation Modifieur_Nom */
		fprintf (xml_file,
			 "<relation xlink:type=\"extended\" type=\"MOD-P\" id=\"E%iR%i\">\n",
			 sentence_id,
			 ++R
			 );
		fprintf (xml_file,"\
<modifieur xlink:type=\"locator\" xlink:href=\"%s\"/>\n\
<preposition xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
			 SXWORD_get (easy_hrefs, href1),
			 SXWORD_get (easy_hrefs, href2)
			 );
		fputs ("</relation>\n", xml_file);

		if (is_print_f_structure) {
		  printf ("MOD-P (%s, %s)\n",
			  SXWORD_get (easy_hrefs, href1)+Ei_lgth,
			  SXWORD_get (easy_hrefs, href2)+Ei_lgth
			  );
		}
	      }
	    }
	  }
	}
      }
    }
  }
}

/*
<relation xlink:type="extended" type="COORD" id="E6R44"> 
<coordonnant xlink:type="locator" xlink:href="E6F12"/> 
<coord-g xlink:type="locator" xlink:href="E6G1"/> 
<coord-d xlink:type="locator" xlink:href="E6G6"/> 
</relation> 
*/
static void
easy_Coordination ()
{
  int x, fs_id, atom_id, local_atom_id, href, href1, href2, arg1_val, arg2_val, arg2b_val, t_code; 
  int *val_ptr, *dpv_ptr;

  /* Pour chaque f_structure ... */
  for (x = 1; x <= fs_id_stack [0]; x++) {
    fs_id = fs_id_stack [x];

    /* ... on regarde si elle a un trait local "cat = coo" ... */
    if (fs_is_set (fs_id, cat_id, &val_ptr) && (atom_id = get_fs_id_atom_val (*val_ptr))) {
      if (
#ifndef ESSAI
	  field_id2atom_field_id [cat_id] [local_atom_id = atom_id2local_atom_id (atom_id)]
#else /* ESSAI */
	  atom_id
#endif /* ESSAI */
	  == atom_coo) {
	/* ... trouve' */
	local_atom_id = 1;
	dpv_ptr = seek_pred (fs_id);

	if ((dpv_ptr != NULL && (href = Xpq2easy_href [dpv_ptr [4]]))
	    && (fs_is_set (fs_id, arg1_id, &val_ptr) && (arg1_val = (*val_ptr) >> FS_ID_SHIFT))
	    && (fs_is_set (fs_id, arg2_id, &val_ptr) && (arg2_val = (*val_ptr) >> FS_ID_SHIFT))) {
	  /* Y'a un pred dont on prend le href de la tete (le Tpq), un arg1 et un arg2 */
	  href1 = 0;

	  t_code = fs_id2tete (arg1_val);

	  /* Si la tete du arg1 est "coo" ou "ponctw" ... */
	  if (t_code == coo_code || t_code == ponctw_code) {
	    /* ... qui vaut "coo" ou "ponctw" ... */
	    /* ... il doit y avoir un champ arg2 */
	    if (fs_is_set (arg1_val, arg2_id, &val_ptr) && (arg2b_val = (*val_ptr) >> FS_ID_SHIFT)) {
	      /* On cherche la tete du arg2 ... */
	      dpv_ptr = seek_pred (arg2b_val);

	      if (dpv_ptr)
		href1 = Xpq2easy_href [dpv_ptr [4]];
	    }
#if 0
	    /* on sortira le vide */
	    else
	      sxtrap (ME, "easy_Coordination () : manque champ \"arg2\"");
#endif /* 0 */
	  }
	  else
	    local_atom_id = 0;

	  if (local_atom_id == 0) {
	    /* ... bon ben on prend le href du arg1 d'origine */
	    dpv_ptr = seek_pred (arg1_val);

	    if (dpv_ptr)
	      href1 = Xpq2easy_href [dpv_ptr [4]];
	  }

	  /* idem sur arg2 */
	  /* On regarde ds arg2 s'il y a un champ "cat = coo" ou "cat = ponctw" */
	  href2 = 0;

	  t_code = fs_id2tete (arg2_val);

	  /* Si la tete du arg1 est "coo" ou "ponctw" ... */
	  if (t_code == coo_code || t_code == ponctw_code) {
	    /* ... qui vaut "coo" ou "ponctw" ... */
	    /* ... il doit y avoir un champ arg2 */
	    if (fs_is_set (arg2_val, arg2_id, &val_ptr) && (arg2b_val = (*val_ptr) >> FS_ID_SHIFT)) {
	      /* On cherche la tete du arg2 ... */
	      dpv_ptr = seek_pred (arg2b_val);

	      if (dpv_ptr)
		href2 = Xpq2easy_href [dpv_ptr [4]];
	    }
#if 0
	    /* on sortira le vide */
	    else
	      sxtrap (ME, "easy_Coordination () : manque champ \"arg2\"");
#endif /* 0 */
	  }
	  else
	    local_atom_id = 0;

	  if (local_atom_id == 0) {
	    /* ... bon ben on prend le href du arg2 d'origine */
	    dpv_ptr = seek_pred (arg2_val);

	    if (dpv_ptr)
	      href2 = Xpq2easy_href [dpv_ptr [4]];
	  }

	  /* On a une relation Modifieur_Nom */
	  fprintf (xml_file,
		   "<relation xlink:type=\"extended\" type=\"COORD\" id=\"E%iR%i\">\n",
		   sentence_id,
		   ++R
		   );
	  fprintf (xml_file,"\
<coordonnant xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
		   SXWORD_get (easy_hrefs, href)
		   );

	  if (href1)
	    fprintf (xml_file,"\
<coord-g xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
		     SXWORD_get (easy_hrefs, href1)
		     );

	  if (href2)
	    fprintf (xml_file,"\
<coord-d xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
		     SXWORD_get (easy_hrefs, href2)
		     );

	  fputs ("</relation>\n", xml_file);


	  if (is_print_f_structure) {
	    printf ("COORD (%s, %s, %s)\n",
		    SXWORD_get (easy_hrefs, href)+Ei_lgth,
		    href1 ? SXWORD_get (easy_hrefs, href1)+Ei_lgth : "",
		    href2 ? SXWORD_get (easy_hrefs, href2)+Ei_lgth : ""
		    );
	  }
	}
      }
    }
  }
}

/* On cherche le (premier) fs_id associe' a la prod Pij */
static int
seek_fs_id (Pij)
     int Pij;
{
  int        Aij, x, rule, head;
  int        *local_heads_stack;
  struct lhs *plhs;

  if (x = Pij2disp [Pij]) {
    plhs = spf.outputG.lhs+Pij;
    Aij = plhs->lhs /* static */;
    local_heads_stack = fs_id_Pij_dstack+x ;

    for (rule = prod2rule [plhs->init_prod]; rule != 0; rule = rule2next_rule [rule]) {
      if (x = *local_heads_stack) {
	/* non vide */
	/* On prend la 1ere tete */
	head = local_heads_stack [1];

	if (head < 0)
	  head = -head;

#if EBUG
	if (XxY_X (heads, head) != plhs->lhs)
	  sxtrap (ME, "seek_fs_id");
#endif /* EBUG */

	return XxY_Y (heads, head);
      }

      local_heads_stack++;
    }
  }

  return 0;
}

/*
<relation xlink:type="extended" type="APPOS" id="E6R51"> 
<premier xlink:type="locator" xlink:href="E6G9"/> 
<appose xlink:type="locator" xlink:href="E6G6"/> 
</relation> 
*/
static void
easy_Apposition ()
{
  int     x, fs_id, atom_id, local_atom_id, aij_val, Pij, fs_id2, /* temp4_val, */href1, href2; 
  int     *val_ptr;
  bool is_NP_temp;

  /* Pour chaque f_structure ... */
  for (x = 1; x <= fs_id_stack [0]; x++) {
    fs_id = fs_id_stack [x];

    /* ... on regarde si elle a un trait local "cat = nc" ou "cat = np" ... */
    if (fs_is_set (fs_id, cat_id, &val_ptr) && (atom_id = get_fs_id_atom_val (*val_ptr))) {
#ifndef ESSAI
      local_atom_id = field_id2atom_field_id [cat_id] [atom_id2local_atom_id (atom_id)];
#else /* ESSAI */
      local_atom_id = atom_id;
#endif /* ESSAI */

      if (local_atom_id == atom_nc || local_atom_id == atom_np || local_atom_id == atom_etr) {
	/* ... trouve' */
	if (fs_is_set (fs_id, aij_id, &val_ptr) && (aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
	  /* Y'a un champ aij */
	  /* Ds les sous-arbres referencess par les aij ... */
	  /* ... On cherche un sous-arbre de racine "PP_temp" */
	  is_NP_temp = false;

	  if (Pij = walk_aij (aij_val, seek_PP_tempij)) {
	    /* On cherche la f_structure associee */
	    fs_id2 = seek_fs_id (Pij);
	  }
	  else
	    fs_id2 = 0;

	  /* ... ou On cherche un sous-arbre de racine "NP_temp..." */
	  if (fs_id2 == 0 && (Pij = walk_aij (aij_val, seek_NP_tempij))) {
	    fs_id2 = seek_fs_id (Pij);
	    is_NP_temp = true; /* Si fs_id2 */
	  }

	  if (fs_id2) {
	    /* trouve */
	    /* On cherche un champ "temp4" non vide */
	    if (/* fs_is_set (fs_id2, temp4_id, &val_ptr) && (temp4_val = (*val_ptr) >> FS_ID_SHIFT)
		   && fs_is_set (temp4_val, aij_id, &val_ptr) && (aij_val = (*val_ptr) >> FS_ID_SHIFT) */
		/* Suppression de "temp4" */
		fs_is_set (fs_id2, aij_id, &val_ptr) && (aij_val = (*val_ptr) >> FS_ID_SHIFT)
               ) {
	      /* ... trouve */
	      /* Dans les aij de ce temp4 */
	      /* on cherche un Easy_GN */
	      if (href2 = walk_aij (aij_val, seek_Easy_GN)) {
		/* trouve */
		/* Ds les aij de fs_id2 ... */
		if (fs_is_set (fs_id2, aij_id, &val_ptr) && (aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
		  /* ... on cherche */
		  /* un Easy_GN si on a trouve un NP_temp ou un Easy_GP pour un PP_temp ... */
		  if (href1 = walk_aij (aij_val, is_NP_temp ? seek_Easy_GN : seek_Easy_GP)) {
		    /* On a une relation d'apposition */
		    fprintf (xml_file,
			     "<relation xlink:type=\"extended\" type=\"APPOS\" id=\"E%iR%i\">\n",
			     sentence_id,
			     ++R
			     );
		    fprintf (xml_file,"\
<premier xlink:type=\"locator\" xlink:href=\"%s\"/>\n\
<appose xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
			     SXWORD_get (easy_hrefs, href1),
			     SXWORD_get (easy_hrefs, href2)
			     );
		    fputs ("</relation>\n", xml_file);

		    if (is_print_f_structure) {
		      printf ("APPOS (%s, %s)\n",
			      SXWORD_get (easy_hrefs, href1)+Ei_lgth,
			      SXWORD_get (easy_hrefs, href2)+Ei_lgth
			      );
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
}

/*
<relation xlink:type="extended" type="JUXT" id="E4R2"> 
<premier xlink:type="locator" xlink:href="E4G1"/> 
<suivant xlink:type="locator" xlink:href="E4G2"/> 
</relation> 
*/
static void
easy_Juxtaposition ()
{
  int x, fs_id, fs_id2, atom_id, local_atom_id, aij_val, Pij, arg1_val, arg2_val, href1, href2, t_code;
  int hrefA, hrefB;
  int *val_ptr, *dpv_ptr; 

  /* Pour chaque f_structure ... */
  for (x = 1; x <= fs_id_stack [0]; x++) {
    fs_id = fs_id_stack [x];

    t_code = fs_id2tete (fs_id);

    /* Si la tete du arg1 est "sbound" ou "ponctw" ou "poncts" ... */
    if (t_code == sbound_code || t_code == ponctw_code || t_code == poncts_code) {
      /* ... trouve' */
      if (fs_is_set (fs_id, aij_id, &val_ptr) && (aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
	/* Y'a un champ aij */
	/* Ds les sous-arbres references par les aij ... */
	/* ... On cherche un sous-arbre de racine "Easy_JUXT2*" */
	if (Pij = walk_aij (aij_val, seek_Easy_JUXT2ij)) {
	  /* On cherche la f_structure associee */
	  fs_id2 = seek_fs_id (Pij);
	}
	else
	  fs_id2 = 0;

	if (fs_id2) {
	  /* On verifie qu'il y a un arg1 et un arg2 ... */
	  if (fs_is_set (fs_id2, arg1_id, &val_ptr) && (arg1_val = (*val_ptr) >> FS_ID_SHIFT) &&
	      fs_is_set (fs_id2, arg2_id, &val_ptr) && (arg2_val = (*val_ptr) >> FS_ID_SHIFT)) {
	    /* ... trouves ... */
	    /* On va chercher la tete du arg2 */
	    dpv_ptr = seek_pred (arg2_val);
	    href2 = dpv_ptr ? Xpq2easy_href [dpv_ptr [4]] : 0;

	    if (href2) {
	      t_code = fs_id2tete (arg1_val);

	      /* Si la tete du arg1 est "coo" ou "sbound" ou "ponctw" ou "poncts" ... */
	      if (t_code == coo_code || t_code == sbound_code || t_code == ponctw_code || t_code == poncts_code) {
		/* ... alors le arg1 doit avoir lui-meme un arg2 ... */
		if (fs_is_set (arg1_val, arg2_id, &val_ptr) && (arg2_val = (*val_ptr) >> FS_ID_SHIFT)) {
		  /* ... dont la tete est le href1 */
		  dpv_ptr = seek_pred (arg2_val);
		  href1 = dpv_ptr ? Xpq2easy_href [dpv_ptr [4]] : 0;
		}
		else
		  href1 = 0;
	      }
	      else {
		/* ... sinon href1 est le arg1 lui_meme */
		dpv_ptr = seek_pred (arg1_val);
		href1 = dpv_ptr ? Xpq2easy_href [dpv_ptr [4]] : 0;
	      }

	      if (href1) {
		/* On a une relation de juxtaposition */
		fprintf (xml_file,
			 "<relation xlink:type=\"extended\" type=\"JUXT\" id=\"E%iR%i\">\n",
			 sentence_id,
			 ++R
			 );
		fprintf (xml_file,"\
<premier xlink:type=\"locator\" xlink:href=\"%s\"/>\n\
<suivant xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
			 SXWORD_get (easy_hrefs, href1),
			 SXWORD_get (easy_hrefs, href2)
			 );
		fputs ("</relation>\n", xml_file);

		if (is_print_f_structure) {
		  printf ("JUXT (%s, %s)\n",
			  SXWORD_get (easy_hrefs, href1)+Ei_lgth,
			  SXWORD_get (easy_hrefs, href2)+Ei_lgth
			  );
		}
	      }
	    }
	  }
	}
      }
    }

    /* ... on regarde si elle a un trait local "cat = v" ... */
    if (fs_is_set (fs_id, cat_id, &val_ptr) && (atom_id = get_fs_id_atom_val (*val_ptr))) {
#ifndef ESSAI
      local_atom_id = field_id2atom_field_id [cat_id] [atom_id2local_atom_id (atom_id)];
#else /* ESSAI */
      local_atom_id = atom_id;
#endif /* ESSAI */

      if (local_atom_id == atom_v
	    || local_atom_id == atom_vSe 
	    || local_atom_id == atom_vppart || local_atom_id == atom_vSeppart 
	    || local_atom_id == atom_vinf || local_atom_id == atom_vSeinf
	    || local_atom_id == atom_vprespart || local_atom_id == atom_vSeprespart 
	  ) {
	if (fs_is_set (fs_id, aij_id, &val_ptr) && (aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
	  /* Y'a un champ aij */
	  /* Ds les sous-arbres references par les aij ... */
	  /* ... On cherche un sous-arbre de racine "Easy_JUXT" ... */
	  if (Pij = walk_aij (aij_val, seek_Easy_JUXTij)) {
	    /* ... trouve' */
	    /* On cherche aussi un Easy_NV* */
	    if (hrefA = walk_aij (aij_val, seek_Easy_NV)) {
	      /* ... trouve' */
	      /* En partie droite de la regle Easy_JUXT, on cherche un SCOMP */
	      struct lhs *plhs;
	      struct rhs *prhs;
	      int        Xpq, Tpq, lgth, hook, Easy_JUXTij, href_temp, Easy_NVij;
	      char       *str;

	      fs_id2 = 0;
	      plhs = spf.outputG.lhs+Pij;
	      Easy_JUXTij = plhs->lhs;
	      prhs = spf.outputG.rhs+plhs->prolon-1;
  
	      while ((Xpq = (++prhs)->lispro) != 0) {
		if (Xpq > 0) {
		  str = spf.inputG.ntstring [Aij2A (Xpq)];
		  lgth = strlen (str);

		  if (strlen (str) == 5 && strncmp (str, "SCOMP", 5) == 0) {
		    /* On cherche la f_structure associee a cet Xpq */
		    /* On parcourt les Xpq-prods ... */
		    hook = spf.outputG.lhs [spf.outputG.maxxprod+Xpq].prolon;

		    while ((Pij = spf.outputG.rhs [hook++].lispro) != 0) {
		      if (Pij > 0) {
			/* ... valides */
			if (fs_id2 = seek_fs_id (Pij))
			  break;
		      }
		    }
		    
		    break;
		  }
		}
	      }

	      if (fs_id2) {
		/* La f_structure fs_id2 est associee au SCOMP */
		/* on prend sa tete */
		dpv_ptr = seek_pred (fs_id2);
		hrefB = dpv_ptr ? Xpq2easy_href [Tpq = dpv_ptr [4]] : 0;

		if (hrefB) {
		  /* On a une relation de juxtaposition */
		  /* On regarde les positions relatives ds le source de Easy_JUXTij et de la tete Tpq du SCOMP */
		  /* On sort la relation JUXT (hrefA, hrefB) avec hrefA "avant" hrefB */
		  Tpq -= spf.outputG.maxxnt;
		  Easy_NVij = walk_aij (aij_val, seek_Easy_NVij);

		  if (Aij2i (Easy_NVij) >= Tij2j (Tpq)) {
		    /* On echange */
		    href_temp = hrefA;
		    hrefA = hrefB;
		    hrefB = href_temp;
		  }

		  fprintf (xml_file,
			   "<relation xlink:type=\"extended\" type=\"JUXT\" id=\"E%iR%i\">\n",
			   sentence_id,
			   ++R
			   );
		  fprintf (xml_file,"\
<premier xlink:type=\"locator\" xlink:href=\"%s\"/>\n\
<suivant xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
			   SXWORD_get (easy_hrefs, hrefA),
			   SXWORD_get (easy_hrefs, hrefB)
			   );
		  fputs ("</relation>\n", xml_file);

		  if (is_print_f_structure) {
		    printf ("JUXT (%s, %s)\n",
			    SXWORD_get (easy_hrefs, hrefA)+Ei_lgth,
			    SXWORD_get (easy_hrefs, hrefB)+Ei_lgth
			    );
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
}

/*<relation xlink:type="extended" type="MOD-N" id="E1R6"> 
<modifieur xlink:type="locator" xlink:href="E1G7"/> 
<nom xlink:type="locator" xlink:href="E1G7"/> 
<à-propager xlink:type="locator" xlink:href="true"/> 
</relation> 
*/
/* On propage ... */
static void
easy_Modifieur_Nom_a_propager ()
{
  int        x, fs_id, object_aij_val, href, Pij, Easy_GNij, i, j;
  int        *val_ptr, *dpv_ptr;
  char       *str;

  /* Pour chaque f_structure ... */
  for (x = 1; x <= fs_id_stack [0]; x++) {
    fs_id = fs_id_stack [x];

    if ((dpv_ptr = seek_pred (fs_id)) && (str = Tpq2str (dpv_ptr [4])) && ((strlen (str) == 2) && (strncmp (str, "np", 2) == 0))) {
      /* La tete de la fs_id est un nom-propre */
      /* On cherche ses aij */
      if (fs_is_set (fs_id, aij_id, &val_ptr) && (object_aij_val = (*val_ptr) >> FS_ID_SHIFT)){
	/* ... a la recherche d'un Easy_GN */
	if (href = walk_aij (object_aij_val, seek_Easy_GN)) {
	  /* En fait, on ne rend la relation que si le Easy_GNij est tel que j-i > 1 ou
	     si le lemme (du fs_id) est "_NP*" (aggregats de noms-propres) */
	  Pij = walk_aij (object_aij_val, seek_Easy_GNij);
	  Easy_GNij = spf.outputG.lhs [Pij].lhs;
	  i = Aij2i (Easy_GNij);
	  j = Aij2j (Easy_GNij);

	  if (j - i <= 1) {
	    str = lexeme_id2string [dpv_ptr [0]];

	    if (strlen (str) <= 3 || strncmp (str, "_NP", 3) != 0)
	      continue;
	  }

	  /* On a une relation Modifieur_Nom a` propager */
	  fprintf (xml_file,
		   "<relation xlink:type=\"extended\" type=\"MOD-N\" id=\"E%iR%i\">\n",
		   sentence_id,
		   ++R
		   );
	  fprintf (xml_file,"\
<modifieur xlink:type=\"locator\" xlink:href=\"%s\"/>\n\
<nom xlink:type=\"locator\" xlink:href=\"%s\"/>\n",
		   SXWORD_get (easy_hrefs, href),
		   SXWORD_get (easy_hrefs, href)
		   );
 
	  fputs ("<à-propager xlink:type=\"locator\" xlink:href=\"vrai\"/>\n", xml_file);
  
	  fputs ("</relation>\n", xml_file);

	  if (is_print_f_structure) {
	    printf ("MOD-N (%s, %s, true)\n",
		    SXWORD_get (easy_hrefs, href)+Ei_lgth,
		    SXWORD_get (easy_hrefs, href)+Ei_lgth
		    );
	  }
	}
      }
    }
  }
}



static void
easy_print_relations ()
{
  int cur_nb, i, head, Xpq, fs_id, pos;

  fputs ("<relations>\n", xml_file);

  cur_nb = heads_stack [0]; /* Nombre de structures "valides" */

  for (i = 1; i <= cur_nb; i++) {
    head = heads_stack [i];

    if (head < 0)
      head = -head;

    Xpq = XxY_X (heads, head);
    fs_id = XxY_Y (heads, head);

    fill_fs_id_stack (fs_id);

    easy_Sujet_Verbe ();
    easy_Auxiliaire_Verbe (Xpq);
    easy_Objet_direct_Verbe ();
    easy_Complement_Verbe ();
    easy_Modifieur_Verbe ();
    easy_Complementeur ();
    easy_Attribut_Sujet_objet ();
    easy_Modifieur_Nom ();
    easy_Modifieur_Adjectif ();
    easy_Modifieur_Adverbe ();
    easy_Modifieur_Preposition ();
    easy_Coordination ();
    easy_Apposition ();
    easy_Juxtaposition ();
    easy_Modifieur_Nom_a_propager ();
  }

  fputs ("</relations>\n", xml_file);
}
#endif /* no_print_relations*/


#if 1
/* Il faut reconstituer les AMALGAM et les COMPOUND */
static int
easy_print_Tij_stack (F)
     int F;
{
  int  top, x, y, z, tok_no, if_id, am_id, ste, href_id, Tij, Tij2;
  char *str1, *str2, easy_int [8], easy_string [16];
  char *comment;

  top = easy_Tij_stack [0];

  for (x = 1; x <= top; x++) {
    Tij = easy_Tij_stack [x];
    tok_no = spf.outputG.Tij2tok_no [Tij];
    if_id = toks_buf [tok_no].lahead; /* L'if_id a ete stocke en lahead par store_dag() ds dag_scanner.c */

    if (X_is_set (&X_amalgam_if_id_hd, if_id))
      /* C'est un element d'un amalgam */
      easy_Tij_stack [x] = -Tij;
  }

  for (x = 1; x <= top; x++) {
    Tij = easy_Tij_stack [x];
    am_id = 0;

    if (Tij < 0) {
      /* On cherche l'amalgam le + long!! */
      Tij = -Tij;
      easy_Tij_stack [x] = Tij; /* Prudence */
      tok_no = spf.outputG.Tij2tok_no [Tij];
      comment = toks_buf [tok_no].comment;
      z = top;

      while (x <= z) {
	XH_push (amalgam_list, toks_buf [tok_no].lahead);

	for (y = x+1; y <= z; y++) {
	  if ((Tij2 = easy_Tij_stack [y]) > 0)
	    break;
	
	  Tij2 = -Tij2;
	  easy_Tij_stack [y] = Tij2; /* Prudence */
	  tok_no = spf.outputG.Tij2tok_no [Tij2];
	  XH_push (amalgam_list, toks_buf [tok_no].lahead);
	}

	if (am_id = XH_is_set (&amalgam_list)) {
	  /* Ici, l'expansion d'un amalgame se trouve dans le meme composant */

	  /* il existe */
	  if (is_print_f_structure) printf ("F%i = \"%s\"", F, amalgam_names [am_id]);

	  if (xml_file) {
	    /* Pour chaque Xpq, on sauve le "href" correspondant */
	    sprintf (easy_string, "E%iF%i", sentence_id, F);

	    if (comment == NULL) {
	      /* Pour "simuler" un source a la easy, sans commentaires */
	      href_id = sxword_save (&easy_hrefs, easy_string);
	      /* On force tous les constituants de l'amalgame Tij a referencer href_id */
	      Xpq2easy_href [spf.outputG.maxxnt + Tij] = href_id;

	      while (++x < y) {
		Tij2 = easy_Tij_stack [x];
		Xpq2easy_href [spf.outputG.maxxnt + Tij2] = href_id;
		tok_no = spf.outputG.Tij2tok_no [Tij2];
	      }

#if EBUG
	    }
#endif /* EBUG */
	    fprintf (xml_file,
		     "  <F debug_id=\"%s\">%s</F>\n",
		     easy_string,
		     amalgam_names [am_id]
		     );

#if EBUG
	    if (comment == NULL) {
#endif /* EBUG */
	      sprintf (easy_int, "%i", F);
	      easy_vstr = varstr_catenate (varstr_catenate (varstr_catenate (easy_vstr, "F"), easy_int), " ");
	    }
	  }

	  x = y-1;
	  break;
	}

	z = y-2;
      }
    }
    else {
      tok_no = spf.outputG.Tij2tok_no [Tij];
      comment = toks_buf [tok_no].comment;
    }
    
    if (am_id == 0) {
      str1 = sxstrget (toks_buf [tok_no].string_table_entry);

      /* Les composants d'un COMPOUND sont separes par des '_' */
      /* On ne tient pas compte de ceux commencant par = '_'
	 ni de ceux qui contiennent  '__' */
      if (*str1 != '_') {

	while (str2 = strchr (str1, '_')) {
	  if (str2 [1] == '_')
	    break;

	  *str2 = SXNUL;
	  if (is_print_f_structure) printf ("F%i = \"%s\", ", F, str1);

	  if (xml_file) {
#if !EBUG
	    if (comment == NULL) {
#endif /* EBUG */
	    fprintf (xml_file,
		     "  <F debug_id=\"E%iF%i\">%s</F>\n",
		     sentence_id,
		     F,
		     str1
		     );

#if EBUG
	    if (comment == NULL) {
#endif /* EBUG */
	      sprintf (easy_int, "%i", F);
	      easy_vstr = varstr_catenate (varstr_catenate (varstr_catenate (easy_vstr, "F"), easy_int), " ");
	    }
	  }

	  *str2 = '_';
	  str1 = str2+1;
	  F++;
	}
      }

      if (str2 = strchr (str1, '/'))
	/* On supprime le suffixe introduit par un '/' Ex le/det/ */
	*str2 = SXNUL;

      if (is_print_f_structure) printf ("F%i = \"%s\"", F, str1);
	  
      if (xml_file) {
	/* on sauve le "href" correspondant au dernier composant d'un mot compose' */
	sprintf (easy_string, "E%iF%i", sentence_id, F);

	if (comment == NULL) {
	  Xpq2easy_href [spf.outputG.maxxnt + Tij] = sxword_save (&easy_hrefs, easy_string);
#if EBUG
	}
#endif /* EBUG */

	fprintf (xml_file,
		 "  <F debug_id=\"%s\">%s</F>\n",
		 easy_string,
		 str1
		 );

#if EBUG
	if (comment == NULL) {
#endif /* EBUG */
	  sprintf (easy_int, "%i", F);
	  easy_vstr = varstr_catenate (varstr_catenate (varstr_catenate (easy_vstr, "F"), easy_int), " ");
	}
      }

      if (str2)
	*str2 = '/';
    }

    F++;

    if (x < top)
      if (is_print_f_structure) fputs (", ", stdout);
  }

  return F;
}
#endif /* 1 */

/* On est ds le cas if (xml_file) */
/* Version de easy_print_Tij_stack ds laquelle ce sont les commentaires associes aux tokens qui contiennent
   les TAG  xml pour easy */
/* 3 cas principaux :
   1 : un mot donne un mot ; il => {<F id="E1F1">il</F>} il
   2 : un amalgamme ; au => {<F id="E1F1">au</F>} à {<F id="E1F1">au</F>} le__det
   3 : un mot_compose ; à moins que => {<F id="E1F1">à</F> <F id="E1F2">moins</F> <F id="E1F3">que</F>} à_moins_que     
*/
static void
easy_print_Tij_stack_from_comments ()
{
  int  top, x, Tij, href_id, tok_no, comment_lgth;
  char *str1, *str2, *comment;

  top = easy_Tij_stack [0];

  for (x = 1; x <= top; x++) {
    Tij = easy_Tij_stack [x];

    if (Tij <= -spf.outputG.maxt) {
      /* On ne s'occupe pas des "terminaux" de la rcvr */
      tok_no = spf.outputG.Tij2tok_no [Tij];
      comment = toks_buf [tok_no].comment;

      if (comment) {
	/* Ca permet de tester du texte en mode easy meme sans TAG xml ... */
	comment_lgth = strlen (comment)-1;
	str1 = comment+comment_lgth;
	*str1 = SXNUL;
      
	fprintf (xml_file, "%s\n", comment+1);

	*str1 = '}';
  
	/* On extrait le "href" */
	/* On prend le + a gauche */
	/* Prudence, on y accede par les ">" et "<"  a cause d'un >"< eventuel ... */
	str1 = strchr (comment+1, (int)'"');
	if (str1) str2 = strchr (str1+1, (int)'"');

	if (str1 && str2) {
	  /*
	              {<F id="E1F1">à</F> <F id="E1F2">moins</F> <F id="E1F3">que</F>} à_moins_que  
	    comment   ^
	    str1             ^
	    str2                  ^
	  */
	  *str2 = SXNUL;
	  href_id = sxword_save (&easy_hrefs, str1+1);
	  *str2 = '"';
	  Xpq2easy_href [spf.outputG.maxxnt + Tij] = href_id;
	}
      }
    }
  }
}


static int
easy_print_ste_path (F, i, j, repair_i)
     int F, i, j, repair_i;
{
  int     triple, k, Tik, new_F;
  bool is_crooked;

  if (i == j) {
#if 1
    /* Pour l'instant pour verif ... */
    F = easy_print_Tij_stack (F);
#endif /* 1 */

    if (xml_file)
      easy_print_Tij_stack_from_comments ();
  }
  else {
    if (spf.outputG.has_repair) {
      /* easy_Tij_hd peut etre cyclique ... */
      /* ... mais il y a forcement un chemin de i a j */ 
      is_crooked = false;

      XxYxZ_Xforeach (easy_Tij_hd, i, triple) {
	Tik = XxYxZ_Y (easy_Tij_hd, triple);
	k = XxYxZ_Z (easy_Tij_hd, triple);

	/* Car easy_Tij_hd peut etre cyclique (pas seulement avec des boucles) */
	if (i == k && Tpq2attr [Tik].i >= repair_i) {
	  /* ... oui */
	  /* On ne considere que le 1er */
	  is_crooked = true;
	  PUSH (easy_Tij_stack, Tik);
	  break;
	}
      }

      XxYxZ_Xforeach (easy_Tij_hd, i, triple) {
	Tik = XxYxZ_Y (easy_Tij_hd, triple);
	k = XxYxZ_Z (easy_Tij_hd, triple);

	if (i != k && Tpq2attr [Tik].i >= repair_i) {
	  PUSH (easy_Tij_stack, Tik);
	  new_F = easy_print_ste_path (F, k, j, Tpq2attr [Tik].j);
	  POP (easy_Tij_stack);

	  if (new_F > F) {
	    /* un seul chemin !! */
	    F = new_F;
	    break;
	  }
	}
      }

      if (is_crooked)
	POP (easy_Tij_stack);
    }
    else {
      XxYxZ_Xforeach (easy_Tij_hd, i, triple) {
	Tik = XxYxZ_Y (easy_Tij_hd, triple);

	k = XxYxZ_Z (easy_Tij_hd, triple);
	
	if (k <= j) {
	  /* transition possible */
	  PUSH (easy_Tij_stack, Tik);
	  new_F = easy_print_ste_path (F, k, j, 0);
	  POP (easy_Tij_stack);

	  if (new_F > F) {
	    /* un seul chemin !! */
	    F = new_F;
	    break;
	  }
	}
      }
    }
  }
    
  return F;
}


static int easy_path_nb;

/* easy_constituent_stack contient un chemin complet constituants+terminaux de liaison */
static void
easy_print_constituent_path ()
{
  int  Aij, top, x, A, i, j, F, G;
  char easy_name [4], *str, easy_int [8], easy_string [16];

  if (is_print_f_structure) fputs ("\n", stdout);

  easy_name [2] = SXNUL;

  top = easy_constituent_stack [0];
  F = G = 1;

  for (x = 1; x <= top; x++) {
    Aij = easy_constituent_stack [x];

    if (Aij <= spf.outputG.maxxnt) {
      if (Aij < 0) {
	/* dummy */
	A = dummy_easy_stack [-Aij];

	if (A == Easy_PV_code)
	  strncpy (easy_name, "PV", 2);
	else
	  strncpy (easy_name, "NV", 2);

	i = dummy_easy_stack [-Aij+2];
	j = dummy_easy_stack [-Aij+3];
      }
      else {
	A = Aij2A (Aij);
	strncpy (easy_name, spf.inputG.ntstring [A]+5, 2);
	i = Aij2i (Aij);
	j = Aij2j (Aij);
      }

      if (i >= 0 && j >= 0) {
	/* Pas des rcvr ou repair */
	/* On cherche un chemin terminal allant de i a j */
	/* dont les formes flechies sont reperees a partir de F */
	if (is_print_f_structure) printf ("G%i : %s%i = [", G, easy_name, G);

	if (xml_file) {
	  sprintf (easy_string, "E%iG%i", sentence_id, G);

	  /* Pour chaque Xpq, on sauve le "href" correspondant */
	  if (Aij > 0)
	    /* A FAIRE ds le cas des dummy !! */
	    Xpq2easy_href [Aij] = sxword_save (&easy_hrefs, easy_string);

	  fprintf (xml_file,
		   "<Groupe type=\"%s\" id=\"%s\">\n",
		   easy_name,
		   easy_string
		   );
	}

	G++; 
	
	F = easy_print_ste_path (F, i, j, 0);
      
	if (is_print_f_structure) fputs ("]\n", stdout);

	if (xml_file)
	  fputs ("</Groupe>\n", xml_file);
      } 
    }
    else {
      PUSH (easy_Tij_stack, Aij - spf.outputG.maxxnt);
#if 1
      /* Il faut traiter les amalgam et compound */
      F = easy_print_Tij_stack (F);
#endif /* 1 */

      if (is_print_f_structure) fputs ("\n", stdout);

      if (xml_file)
	easy_print_Tij_stack_from_comments ();

      POP (easy_Tij_stack);
    }
  }
}

/* On parcourt les constituants consecutifs */
static bool
easy_walk (i, repair_i)
     int i, repair_i;
{
  int     triple, Aij, j, Tij;
  bool ret_val, is_crooked;

  if (i == final_mlstn) {
    /* easy_constituent_stack contient un chemin complet (constituants et terminaux de liaison) */
    if (++easy_path_nb == 1) {
      /* Je n'imprime que le 1er chemin ... */
      if (xml_file) {
	fputs ("<constituants>\n", xml_file);
      }

      easy_print_constituent_path ();

      if (xml_file) {
	fputs ("</constituants>\n", xml_file);

#if 0
#ifndef no_print_relations
	easy_print_relations ();
#endif /* no_print_relations*/
#endif /* 0 */

	xml_close_tag = true;
      }
    }

    return true;
  } 

  ret_val = false;

  XxYxZ_Xforeach (easy_hd, i, triple) {
    Aij =  XxYxZ_Y (easy_hd, triple);
    /* Attention Aij peut etre negatif ds le cas de dummy constituents */

    /* Attention, easy_hd peut etre cyclique */
    if (Aij < 0 || SXBA_bit_is_reset_set (easy_Aij_constituent_set, Aij)) {
      PUSH (easy_constituent_stack, Aij);
      j = XxYxZ_Z (easy_hd, triple);

      if (easy_walk (j, 0))
	ret_val = true;

      POP (easy_constituent_stack);

      if (Aij > 0)
	SXBA_0_bit (easy_Aij_constituent_set, Aij);

      if (ret_val)
	break;
    }

#if 0
    /* Le nombre de chemins peut etre gigantesque ... */
    /* ... le 22/12/05 abandon, on ne les compte pas */
#if !LOG
    if (ret_val)
      break;
#endif /* !LOG */
#endif /* 0 */
  }

  if (!ret_val) {
    /* On fait une transition terminale ... */
    if (!spf.outputG.has_repair) {
      XxYxZ_Xforeach (easy_Tij_hd, i, triple) {
	Tij =  XxYxZ_Y (easy_Tij_hd, triple);
	PUSH (easy_constituent_stack, spf.outputG.maxxnt+Tij);
	j = XxYxZ_Z (easy_Tij_hd, triple);

	if (easy_walk (j, 0))
	  ret_val = true;

	POP (easy_constituent_stack);

	if (ret_val)
	  /* Le nombre de chemins peut etre gigantesque ... */
	  /* ... le 22/12/05 abandon, on ne les compte pas */
	  break;
      }
    }
    else {
      /* repair ... on regarde s'il y a des Tii ... */
      is_crooked = false;

      XxYxZ_Xforeach (easy_Tij_hd, i, triple) {
	Tij =  XxYxZ_Y (easy_Tij_hd, triple);

	/* Car easy_Tij_hd peut etre cyclique (pas seulement avec des boucles) */
	if (i == XxYxZ_Z (easy_Tij_hd, triple) && Tpq2attr [Tij].i >= repair_i) {
	  /* ... oui */
	  /* On ne considere que le 1er */
	  is_crooked = true;
	  PUSH (easy_constituent_stack, spf.outputG.maxxnt+Tij);
	  break;
	}
      }

      XxYxZ_Xforeach (easy_Tij_hd, i, triple) {
	j = XxYxZ_Z (easy_Tij_hd, triple);
	Tij =  XxYxZ_Y (easy_Tij_hd, triple);

	if (i != j && Tpq2attr [Tij].i >= repair_i) {
	  PUSH (easy_constituent_stack, spf.outputG.maxxnt+Tij);

	  if (easy_walk (j, Tpq2attr [Tij].j))
	    ret_val = true;

	  POP (easy_constituent_stack);

	  if (ret_val)
	    /* Le nombre de chemins peut etre gigantesque ... */
	    /* ... le 22/12/05 abandon, on ne les compte pas */
	    break;
	}
      }

      if (is_crooked)
	POP (easy_constituent_stack);
    }

    /* Faut-il faire Tij_iforeach aussi ? */
    /* Oui le 17/02/06 car on peut avoir du rcvr sur la super_regle non accedee par les tris topologiques. Ex :
       <0> -> $ S [n-1] [n] $ (suppression du dernier symbole), donc pas ds easy_Tij_hd
       or il faut atteindre l'etat final */
    /* Essai le 17/11/04 */
    if (!ret_val) {
      Tij_iforeach (i, Tij) {
	j = Tij2j (Tij); /* j > i */
	PUSH (easy_constituent_stack, spf.outputG.maxxnt+Tij);

	if (easy_walk (j, 0))
	  ret_val = true;

	POP (easy_constituent_stack);

	if (ret_val)
	  break;

#if 0
	/* Le nombre de chemins peut etre gigantesque ... */
	/* ... le 22/12/05 abandon, on ne les compte pas */
#if !LOG
	if (ret_val)
	  break;
#endif /* !LOG */
#endif /* 0 */
      }
    }
  }

  return ret_val;
}


/* On construit Tpq2attr */
static int
build_Tpq2attr ()
{
  int              i, cur_bound, Tpq, Tqr, j, map, ib, p, pb, q, trans2, r, qb, trans, max_bound;
  bool          has_cyclic_repair, has_repair;
  struct attr      *attr_ptr;

  max_bound = i2lb [1] = 1;

  for (i = 1; i <= final_mlstn; i++) {
    non_repair_i2lb [i] = cur_bound = i2lb [i];

    if (cur_bound) {
      /* Il se peut que des Tpq soient sautes */
      has_cyclic_repair = has_repair = false;

      /* On regarde s'il y a des Tik qui proviennent de la reparation */
      XxYxZ_Xforeach (easy_Tij_hd, i, trans) {
	Tpq = XxYxZ_Y (easy_Tij_hd, trans);

	if (SXBA_bit_is_set (spf.outputG.Tpq_rcvr_set, Tpq)) {
	  /* oui */
	  j = XxYxZ_Z (easy_Tij_hd, trans);

	  if (i == j) {
	    if (!has_cyclic_repair) {
	      /* On ne prend que le 1er */
	      has_cyclic_repair = true;

	      /* (i, i) => (cur_bound, cur_bound+1) */
	      attr_ptr = Tpq2attr+Tpq;
	      attr_ptr->i = cur_bound;
	      attr_ptr->j = ++cur_bound;
	      attr_ptr->size = 1;

	      max_bound = cur_bound;
	      break; /* 1 seul chemin */
	    }
	  }
	  else {
	    has_repair = true;
	  }
	}
      }

      if (!has_cyclic_repair && has_repair) {
	/* On traite ces Tpq */
	ib = cur_bound;

	XxYxZ_Xforeach (easy_Tij_hd, i, trans) {
	  Tpq = XxYxZ_Y (easy_Tij_hd, trans);

	  if (SXBA_bit_is_set (spf.outputG.Tpq_rcvr_set, Tpq)) {
	    /* oui */
	    p = i;
	    pb = ib;
	    q = XxYxZ_Z (easy_Tij_hd, trans);

	    if (p != q) {
	      while (q > final_mlstn) {
		/* (p, q) => (pb, cur_bound+1) */
		attr_ptr = Tpq2attr+Tpq;
		attr_ptr->i = pb;
		attr_ptr->j = ++cur_bound;
		attr_ptr->size = 1;

		XxYxZ_Xforeach (easy_Tij_hd, q, trans2) {
		  /* trans unique */
		  break;
		}
	
		p = q;
		pb = cur_bound;

		Tpq = XxYxZ_Y (easy_Tij_hd, trans2);
		q = XxYxZ_Z (easy_Tij_hd, trans2);
	      }

	      /* On ne conserve qu'un chemin */
	      if ((qb = i2lb [q]) == 0 /* 1ere fois */ || qb < cur_bound+1 /* on choisit le + long */)
		max_bound = qb = i2lb [q] = cur_bound+1;

	      attr_ptr = Tpq2attr+Tpq;
	      attr_ptr->i = pb;
	      attr_ptr->j = qb;
	      attr_ptr->size = 1;
	    
	      break;
	    }
	  }
	}
      }

      /* on traite les normaux */
      ib = cur_bound;
    
      XxYxZ_Xforeach (easy_Tij_hd, i, trans) {
	Tpq = XxYxZ_Y (easy_Tij_hd, trans);

	if (!SXBA_bit_is_set (spf.outputG.Tpq_rcvr_set, Tpq)) {
	  /* normal */
	  q = XxYxZ_Z (easy_Tij_hd, trans);
	  qb = ib+1;

	  if (i2lb [q] < qb)
	    max_bound = i2lb [q] = qb;
	  else
	    qb = i2lb [q];

	  /* (i, q) => (ib, qb) */
	  attr_ptr = Tpq2attr+Tpq;
	  attr_ptr->i = ib;
	  attr_ptr->j = qb;
	  attr_ptr->size = 1;
	}
      }
    }
  }

  return max_bound;
}


static void
easy_header ()
{
#if 0
  /* Cette entete sera produite par un wrappeur */
  fputs ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n", xml_file);
#endif /* 0 */
    
  if (is_print_headers) {
    fprintf (xml_file,
	     "\n\
<!-- ********************************************************************\n\
\tThe file \"%s\" contains the XML format of the input sentence #%i\n",
	     xml_file_name,
	     sentence_id
	     );
    
    if (input_sentence_string)
      fprintf (xml_file,
	       "\t\"%s\"\n",
	       input_sentence_string
	       );
    
    fprintf (xml_file,
	     "\twhich has been generated\
\tby the SYNTAX(*) LFG processor SXLFG(*)\n\
     ********************************************************************\n\
\t(*) SYNTAX and SXLFG are trademarks of INRIA.\n\
     ******************************************************************** -->\n\n")
	     );
  }
#if 0
  /* Cette commande sera produite par un wrappeur */
  fprintf (xml_file,
	   "<DOCUMENT fichier=\"%s\">\n",
	   xml_file_name);
#endif /* 0 */
  
  fprintf (xml_file,
	   "<E id=\"E%i\">\n",
	   sentence_id);
}

/* On repere les constituants */
static void
easy_constituent ()
{
  static int      easy_hd_foreach [6] = {1, 0, 0, 0, 0, 0};
  static int      easy_Tij_hd_foreach [6] = {1, 0, 0, 0, 0, 0};
  static int      dummy_easy_hd_foreach [6] = {0, 0, 0, 0, 0, 0};
  int             nt, t, lgth, max_bound;
  char            *nt_string, *t_string;

  easy_constituent_nt_set = sxba_calloc (spf.inputG.maxnt+1);
  easy_Aij_constituent_set = sxba_calloc (spf.outputG.maxxnt+1);
  XxYxZ_alloc (&easy_hd, "easy_hd", final_mlstn+1, 1, easy_hd_foreach, NULL, NULL);
  XxYxZ_alloc (&easy_Tij_hd, "easy_Tij_hd", final_mlstn+1, 1, easy_Tij_hd_foreach, NULL, NULL);
  easy_constituent_stack = (int*) sxalloc (spf.outputG.maxxnt-spf.outputG.maxt+1, sizeof (int)), easy_constituent_stack [0] = 0;
  easy_Tij_stack = (int*) sxalloc (-spf.outputG.maxt+1, sizeof (int)), easy_Tij_stack [0] = 0;

  Aij2attr = (struct attr*) sxalloc (spf.outputG.maxxnt+1, sizeof (struct attr));
  dummy_Aij2attr = (struct attr*) sxalloc (spf.outputG.maxxnt+1, sizeof (struct attr));
  DALLOC_STACK (dummy_easy_stack, 10*6);

  for (nt = 1; nt <= spf.inputG.maxnt; nt++) {
    nt_string = spf.inputG.ntstring [nt];

    if (strlen (nt_string) > 5 && strncmp (nt_string, "Easy_", 5) == 0
      /* C'est un easy ... */
      /* ... mais est-ce un constituant ? */
	&& strlen (nt_string+5) >= 2 && strncmp (nt_string+5, "00", 2) != 0 && strncmp (nt_string+5, "ZZ", 2) != 0)
      SXBA_1_bit (easy_constituent_nt_set, nt);
  }
  
  if (spf.outputG.start_symbol) {
    if (xml_file)
      easy_header ();

    Easy_sorted_Aij = (int*) sxalloc (spf.outputG.maxxnt+1, sizeof (int)), Easy_sorted_Aij [0] = 0;

#if LOG
    fputs ("\npi = easy_td_walk\n", stdout);
#endif /* LOG */

    spf_topological_walk (spf.outputG.start_symbol, easy_td_walk, NULL);

    if (spf.outputG.has_repair) {
      Tpq2attr = (struct attr*) sxcalloc (-spf.outputG.maxt+1, sizeof (struct attr));
      i2lb = (int*) sxcalloc (spf.outputG.repair_mlstn_top+1, sizeof (int));
      non_repair_i2lb = (int*) sxcalloc (spf.outputG.repair_mlstn_top+1, sizeof (int));

      build_Tpq2attr ();

      max_bound = spf.outputG.repair_mlstn_top;

      sxfree (i2lb), i2lb = NULL;
    }
    else
      max_bound = final_mlstn;

    easy_range_set = sxba_calloc (max_bound+1);

    /* Nelle version du 10/02/06, on propage les tailles ds tous les cas */
    easy_upper_bound = (int *) sxcalloc (spf.outputG.maxxnt+1, sizeof (int));

#if LOG
    fputs ("\npd = dummy_easy_bu_walk\n", stdout);
#endif /* LOG */

    spf_topological_walk (spf.outputG.start_symbol, NULL, dummy_easy_bu_walk);

    sxfree (easy_upper_bound), easy_upper_bound = NULL;

    Easy_maximal_constituent ();

    sxfree (Easy_sorted_Aij), Easy_sorted_Aij = NULL;

    /* Il faut chercher les compositions sequentielles */
    easy_walk (1, 0);

#if EBUG
    if (xml_file && !xml_close_tag)
      sxtrap (ME, "easy_constituent");
#endif /* EBUG */
    fputs ("</E>\n", xml_file);
  }

  sxfree (easy_constituent_nt_set), easy_constituent_nt_set = NULL;
  sxfree (easy_range_set), easy_range_set = NULL;
  sxfree (easy_Aij_constituent_set), easy_Aij_constituent_set = NULL;
  XxYxZ_free (&easy_hd);
  XxYxZ_free (&easy_Tij_hd);
  sxfree (easy_constituent_stack), easy_constituent_stack = NULL;
  sxfree (easy_Tij_stack), easy_Tij_stack = NULL;

  sxfree (Aij2attr), Aij2attr = NULL;
  sxfree (dummy_Aij2attr), dummy_Aij2attr = NULL;
  DFREE_STACK (dummy_easy_stack);

  if (spf.outputG.has_repair) {
    sxfree (Tpq2attr), Tpq2attr = NULL;
    sxfree (non_repair_i2lb), non_repair_i2lb = NULL;
  }
}






static void
easy_campaign ()
{
  int  sentence_no;
  char string [16];

  X_reuse (&X_amalgam_if_id_hd, "X_amalgam_if_id_hd", NULL, NULL);
  XH_unlock (amalgam_list);

  easy_vstr = varstr_alloc (128);
  sxword_alloc (&easy_hrefs, "easy_hrefs", Xpq_top+2, 1, 8,
		sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL, NULL);
  Xpq2easy_href = (int*) sxcalloc (Xpq_top+2, sizeof (int));
  NV_stack = (int*) sxalloc (2*spf.inputG.maxrhs+1, sizeof (int)); 

  if (is_print_f_structure) {
    /*    sentence_no = easy_get_sentence_no ();

    if (sentence_no != 0 && sentence_no != sentence_id)
      sentence_id = sentence_no;
      */

    sprintf (string, "E%i", sentence_id);
    Ei_lgth = strlen (&(string [0]));
  }

  easy_constituent ();

  varstr_free (easy_vstr), easy_vstr = NULL;
  sxword_free (&easy_hrefs);
  sxfree (Xpq2easy_href), Xpq2easy_href = NULL;
  sxfree (NV_stack), NV_stack = NULL;
}
#endif /* EASY */

static void
xml_gen_header ()
{
    if (is_print_headers) {
      fprintf (xml_file,
	       "\n\
<!-- ********************************************************************\n\
\tThe file \"%s\" contains the LFG parse forest in XML format\n\
\twhich has been generated\
\tby the SYNTAX(*) LFG processor SXLFG(*)\n\
     ********************************************************************\n\
\t(*) SYNTAX and SXLFG are trademarks of INRIA.\n\
     ******************************************************************** -->\n\n",
	       xml_file_name);
    }
}

static int
xml_print_terminal_leaves ()
{
  int  Tpq, T, p, q, trans, tok_no, t_nb, i;
  char *terminal_string, *short_tstring, *c;

  fputs ("  <terminals>\n", xml_file); 

  t_nb = 0;

  for (Tpq = 1; Tpq <= maxxt; Tpq++) {
    /* T est un terminal */
    T = -Tij2T (Tpq);

    p = Tij2i (Tpq);
    q = Tij2j (Tpq);

    short_tstring = strdup (spf.inputG.tstring [T]);
    c = strchr(short_tstring,'_');
    if (c)
      *c = SXNUL;

    if (p >= 0 && q >= 0) {
      /* p et q sont des mlstn */
      p = mlstn2dag_state (p);
      q = mlstn2dag_state (q);
      /* En "standard" on dispose de X et Zforeach sur dag_hd (voir sxearley_main.c) */

      XxYxZ_Xforeach (dag_hd, p, trans) {
	if (XxYxZ_Z (dag_hd, trans) == q) {
	  tok_no = XxYxZ_Y (dag_hd, trans);

	  if (SXBA_bit_is_set (glbl_source [tok_no], T)) {
	    /* La transition tok_no entre p et q contient bien le terminal T */
	    terminal_string = sxstrget (toks_buf [tok_no].string_table_entry);
	    t_nb++;
	    fprintf (xml_file,
		     "    <terminal id=\"T%i\" name=\"%s\" realName=\"%s\" form=\"%s\" lb=\"%i\" ub=\"%i\"/>\n",
		     Tpq,
		     short_tstring,
		     spf.inputG.tstring [T],
		     terminal_string,
		     p,
		     q
		     );
	  }
	}
      }
    }
    else {
      /* Cas d'un xt */
      t_nb++;
      fprintf (xml_file,
	       "    <terminal id=\"T%i\" rcvr=\"+\" name=\"%s\" realName=\"%s\" lb=\"%i\" ub=\"%i\"/>\n",
	       Tpq,
	       short_tstring,
	       spf.inputG.tstring [T],
	       p,
	       q
	       );
      
    }
  } 

  fputs ("  </terminals>\n", xml_file); 

  return t_nb;
}


static void
xml_print_nt_set (Aij_set)
     SXBA Aij_set;
{
  int Aij, hook, prod;
  
  fputs ("  <non_terminals>\n", xml_file); 

  Aij = 0;

  while ((Aij = sxba_scan (Aij_set, Aij)) > 0) {
    if (Aij <= spf.outputG.maxnt) {
      if (Aij2i(Aij) < Aij2j(Aij)) { /* on n'affiche pas les Aij vides */
	/* nt */
	fprintf (xml_file,
		 "    <non_terminal id=\"N%i\" name=\"%s\" lb=\"%i\" ub=\"%i\">\n",
		 Aij,
		 spf.inputG.ntstring [Aij2A (Aij)],
		 Aij2i (Aij),
		 Aij2j (Aij)
		 );
	
	hook = spf.outputG.lhs [spf.outputG.maxxprod+Aij].prolon;
	while ((prod = spf.outputG.rhs [hook++].lispro) != 0) {
	  /* On parcourt les productions dont la lhs est Aij */
	  if (prod > 0)
	    /* Non vide */
	    fprintf (xml_file, "      <rule idref=\"R_%i_%i\"/>\n", prod, spf.outputG.lhs [prod].init_prod);
	}
	fputs("    </non_terminal>\n",xml_file);
      }
    }
    else {
      /* xnt */
      fprintf (xml_file,
	       "    <non_terminal id=\"N%i\" rcvr=\"+\" name=\"%s\" lb=\"%i\" ub=\"%i\"/>\n",
	       Aij,
	       spf.inputG.ntstring [Aij2A (Aij)],
	       Aij2i (Aij) /* -1 => undef */,
	       Aij2j (Aij) /* -1 => undef */
	       );
      /* On enleve les xnt pour la suite */
      SXBA_0_bit (Aij_set, Aij);
    }
  }

  fputs ("  </non_terminals>\n", xml_file); 
}

static int offset;

static void xml_print_terminal (int Tij)
{
  int tok_no, i;
  char *comment, *str1, *str2, *str3;
  int local_f=0, last_f=0;

  for (i=0; i<offset; i++)
    fputs("  ",xml_file);
  fprintf(xml_file,"<T id=\"T%i\" name=\"%s\">\n",Tij,spf.inputG.tstring[-Tij2T (Tij)]);

  for (i=0; i<=offset; i++)
    fputs("  ",xml_file);

  if (!SXBA_bit_is_set (spf.outputG.Tpq_rcvr_set, Tij)) {
    tok_no=spf.outputG.Tij2tok_no [Tij];
    
    comment = toks_buf [tok_no].comment;
    
    if (comment) {
      comment++;
      for (comment; *comment != SXNUL; comment++) {
	str1 = strchr(comment,(int)'F');
	str2 = strchr(str1+3,(int)'F');
	str1 /* str3 */ = strchr(str2,(int)'"');
	*str1=SXNUL;
	local_f=atoi(str2+1);
	*str1='"';
	if (local_f > last_f) {
	  last_f=local_f;
	  for (comment; *comment != '}'; comment++){
	    fprintf(xml_file,"%c",*comment);
	  }
	} else {
	  comment = strchr(comment,(int)'>');
	  comment = strchr(comment+1,(int)'>');
	  comment+=2;
	}
      }
      fputs("\n",xml_file);
    } else { // pas de commentaires: on sort la ff dans un commentaire XML
      fprintf(xml_file,"<!-- %s -->\n",sxstrget (toks_buf [tok_no].string_table_entry),tok_no,Tij);
    }
  } else {
      fprintf(xml_file,"<!-- (error) -->\n");
  }
  for (i=0; i<offset; i++)
    fputs("  ",xml_file);
  fputs("</T>\n",xml_file);
}

static void
xml_print_unique_tree (int Aij)
{
  int Pij, Xpq, i, hook;
  struct lhs *plhs;
  struct rhs *prhs;
  char* ntstring;
  bool hide = false;

  if (Aij2i (Aij) == Aij2j (Aij) && Aij2i (Aij) > 0)
    return;

  ntstring = spf.inputG.ntstring[Aij2A (Aij)];
  if (ntstring[0] == '[')
    hide = true;

  if (strlen(ntstring) > 6 && strncmp(ntstring, "DISJ__", 6) == 0)
    hide = true;

  if (strlen(ntstring) > 7 && strncmp(ntstring + strlen(ntstring) - 7, "___PLUS", 7) == 0)
    hide = true;
  if (strlen(ntstring) > 7 && strncmp(ntstring + strlen(ntstring) - 7, "___STAR", 7) == 0)
    hide = true;

  hook = spf.outputG.lhs [spf.outputG.maxxprod+Aij].prolon; 
  while ((Pij = spf.outputG.rhs [hook++].lispro) < 0);
  if (Pij == 0) {
    return;
#if EBUG
    sxtrap(ME,"xml_print_unique_tree: emtpy node found");
#endif /* EBUG */
  }

  if (!hide) {
    for (i=0; i<offset; i++)
      fputs("  ",xml_file);
    fprintf(xml_file,"<node name=\"%s\" lb=\"%i\" ub=\"%i\" id=\"R_%i_%i\">\n", ntstring, Aij2i (Aij), Aij2j (Aij), Pij, spf.outputG.lhs [Pij].init_prod);
    offset++;
  }

  plhs = spf.outputG.lhs+Pij; /* on accede a l'entree de la prod dans lhs */
  prhs = spf.outputG.rhs+plhs->prolon; /* on accede a la partie droite de la prod dans rhs */
  while ((Xpq = (prhs++)->lispro) != 0) /* on parcourt la partie droite */
    if (Xpq < 0) /* terminal */
      xml_print_terminal (-Xpq);
    else
      xml_print_unique_tree (Xpq);

  if (!hide) {
    offset--;
    for (i=0; i<offset; i++)
      fputs("  ",xml_file);
    fprintf(xml_file,"</node>\n");
  }

}

static void
xml_print_forest ()
{
  int  Aij, hook, prod, item, Xpq, t_nb;
  SXBA prod_set, Aij_set;

  prod_set = sxba_calloc (spf.outputG.maxxprod+1);
  Aij_set = sxba_calloc (spf.outputG.maxxnt+1);

  fprintf (xml_file,
	   "<parse_forest t_nb=\"%i\" nt_nb=\"%i\" prod_nb=\"%i\">\n",
	   t_nb,
	   sxba_cardinal (Aij_set),
	   sxba_cardinal (prod_set)
	   );

  fprintf (xml_file, "  <axiom idref=\"N%i\"/>\n",spf.outputG.start_symbol);

  for (Aij = 1; Aij <= spf.outputG.maxnt; Aij++) {
    /* nt ... */
    if (Aij2i (Aij) < Aij2j (Aij)) {
      /* ... non vide */
      /* On ne sort ni les nt vides ni leurs nt_prod */
      hook = spf.outputG.lhs [spf.outputG.maxxprod+Aij].prolon;

      while ((prod = spf.outputG.rhs [hook].lispro) != 0) {
	/* On parcourt les productions dont la lhs est Aij */
	if (prod > 0) {
	  SXBA_1_bit (prod_set, prod);
	  SXBA_1_bit (Aij_set, Aij);

	  /* On capture aussi les xnt */
	  item = spf.outputG.lhs [prod].prolon;

	  while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
	    if (Xpq > 0)
	      /* nt ou xnt */
	      SXBA_1_bit (Aij_set, Xpq);
	  }
	}

	hook++;
      }
    }
  }

  t_nb = xml_print_terminal_leaves ();

  /* Sortir les non-terminaux */
  xml_print_nt_set (Aij_set);
  /* Les xnt ont ete enleves de Aij_set */
  
  fputs ("  <rules>\n", xml_file); 

  prod = 0;

  while ((prod = sxba_scan (prod_set, prod)) > 0) {
    item = spf.outputG.lhs [prod].prolon;
    Aij = spf.outputG.lhs [prod].lhs;

    fprintf (xml_file,
	     "    <rule id=\"R_%i_%i\">\n",
	     prod,
	     spf.outputG.lhs [prod].init_prod);

    spf_print_iprod (xml_file, prod, "      <!-- ", " -->\n");

    fprintf (xml_file, "      <lhs_node idref=\"N%i\"/>\n", Aij);

    while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
      /* On parcourt la RHS de prod */
      if (Xpq > 0) {
	/* nt ... */
	if (Xpq > spf.outputG.maxnt || Aij2i (Xpq) < Aij2j (Xpq)) {
	  /* ... non vide ou xnt */
	  fprintf (xml_file, "      <rhs_node idref=\"N%i\"/>\n", Xpq);
	}
      }
      else {
	/* T */
	fprintf (xml_file, "      <rhs_node idref=\"T%i\"/>\n", -Xpq);
      }
    }

    fputs ("    </rule>\n", xml_file);
  }

  fputs ("  </rules>\n", xml_file); 

  fputs ("</parse_forest>\n", xml_file);

  sxfree (prod_set);
  sxfree (Aij_set);
}


static void
xml_print_f_structure ()
{
  int             fs_id, bot, top, bot2, cur2, top2, bot3, cur3, top3, orig_field_val;
  int             val, field_id, field_kind, sous_cat_id, atom_id, local_atom_id, x, i, Tpq, id;
  int             *local2atom_id;
  bool         is_optional, is_first2;
  struct pred_val *ptr2;
  unsigned char   static_field_kind;
  bool         is_shared;

  /* traite' comme une queue */
  x = 0; /* indice ds fs_id_stack */

  while (++x <= fs_id_stack [0]) {
    fs_id = fs_id_stack [x];

    bot = XH_X (XH_fs_hd, fs_id);
    top = XH_X (XH_fs_hd, fs_id+1);

    if (bot < top && XH_list_elem (XH_fs_hd, top-1) < 0) {
      top--;
      fprintf (xml_file, "  <f_structure id=\"F%i\" shared=\"true\">\n", fs_id);
    }
    else
      fprintf (xml_file, "  <f_structure id=\"F%i\">\n", fs_id);

    while (bot < top) {
      orig_field_val = val = XH_list_elem (XH_fs_hd, bot);

      field_id = val & FIELD_AND;
      static_field_kind = field_id2kind [field_id];
      val >>= MAX_FIELD_ID_SHIFT;
      field_kind = val & KIND_AND;
      val >>= STRUCTURE_NAME_shift;

      if (field_id == pred_id) {
	int *dpv_ptr;

#ifndef ESSAI
	val = get_ufs_atom_val (val, &is_shared);
#else /* ESSAI */
	val = get_ufs_atom_val (val, &is_shared, NULL);
#endif /* ESSAI */
	dpv_ptr = &(XH_list_elem (dynam_pred_vals, XH_X (dynam_pred_vals, val)));

	fputs ("    <pred>\n", xml_file);

	fprintf (xml_file, "      <lexeme value=\"%s\"/>\n", lexeme_id2string [dpv_ptr [0] /* lexeme_id */]);

	sous_cat_id = dpv_ptr [1] /* sous_cat1 */;

	if (dpv_ptr [1] || dpv_ptr [2]) {
	  fprintf (xml_file, "      <scat>\n");

	  for (i = 1; i <= 2; i++) {
	    if (dpv_ptr [i]) {	   
	      for (bot2 = cur2 = sous_cat [sous_cat_id], top2 = sous_cat [sous_cat_id+1];
		   cur2 < top2;
		   cur2++) {
		if (i == 1)
		  fputs ("        <synt_function name=\"", xml_file);
		else
		  fputs ("        <virt_function name=\"", xml_file);
		
		field_id = sous_cat_list [cur2];
		
		if (field_id < 0) {
		  field_id = -field_id;
		  is_optional = true;
		}
		else
		  is_optional = false;
		
		if (field_id > MAX_FIELD_ID) {
		  field_id -= MAX_FIELD_ID;
		  
		  for (bot3 = cur3 = compound_field [field_id], top3 = compound_field [field_id+1];
		       cur3 < top3;
		       cur3++) {
		    field_id = compound_field_list [cur3];
		    fprintf (xml_file, "%s%s", (cur3 != bot3) ? "|" : "", field_id2string [field_id]);
		  }
		}
		else {
		  fprintf (xml_file, "%s", field_id2string [field_id]);
		}
		
		if (is_optional)
		  fputs ("\" optional=\"yes\"/>\n", xml_file);
		else
		  fputs ("\"/>\n", xml_file);
	      }
	    }
	  }

	  if (i==1)
	    sous_cat_id = dpv_ptr [2] /* sous_cat2 */;

	  fprintf (xml_file, "      </scat>\n", i);
	}

	if (Tpq = dpv_ptr [4]) {
	  Tpq -=  spf.outputG.maxxnt;
	  fprintf (xml_file, "      <anchor idref=\"T%i\"/>\n", Tpq);
	} 
      
	fputs ("    </pred>\n", xml_file);
      }
      else {
	fprintf (xml_file, "    <attribute name=\"%s\"", field_id2string [field_id]);

	if (static_field_kind & STRUCT_KIND) {
	  if (static_field_kind & UNBOUNDED_KIND)
	    fputs (" type=\"set\"", xml_file);
	  else
	    fputs (" type=\"struct\"", xml_file);
	}
	else {
	  /* atomique ... */
	  if (static_field_kind & UNBOUNDED_KIND) {
#if ATOM_Aij || ATOM_Pij
	    if (static_field_kind & Aij_KIND)
	      fputs (" type=\"c_struct\"", xml_file);
	    else
#endif /* ATOM_Aij || ATOM_Pij */
	      fputs (" type=\"atom\"", xml_file);
	  }
	  else
	    fputs (" type=\"atom\"", xml_file);
	}
	    
	fputs (">\n", xml_file);

	if (static_field_kind & ATOM_KIND) {
#if ATOM_Aij || ATOM_Pij
	  if ((static_field_kind & UNBOUNDED_KIND) && (static_field_kind & Aij_KIND)) {
	    if (val) {
	      for (cur2 = bot2 = XH_X (XH_ufs_hd, val), top2 = XH_X (XH_ufs_hd, val+1);
		   cur2 < top2;
		   cur2++) {
		id = XH_list_elem (XH_ufs_hd, cur2);

		if (id < 0)
		  break;

		if (id > spf.outputG.maxxnt) { 
		  id -= spf.outputG.maxxnt; /* Pij */
		  fprintf (xml_file, "      <value idref=\"R_%i_%i\"/>\n", id, spf.outputG.lhs [id].init_prod);
		}
		else
		  fprintf (xml_file, "      <value idref=\"N%i\"/>\n", id /* Aij */);
	      }
	    }
	    else
	      fputs ("      <value error_val=\"\"/>\n", xml_file);
	  }
	  else
#endif /* ATOM_Aij || ATOM_Pij */
	    {
#ifndef ESSAI
	      val = get_ufs_atom_val (val, &is_shared);		
	      if (val == 0)
		fprintf (xml_file, "      <value atomic_val=\"(unknown value)\"/>\n");
	      else {
		if (static_field_kind & UNBOUNDED_KIND)
		  fprintf (xml_file, "      <value atomic_val=\"%s\"/>\n", ATOM_ID2STRING (val));
		else {
		  local2atom_id = field_id2atom_field_id [field_id];
		  local_atom_id = 0;
		  is_first2 = true;
		  
		  while ((val >>= 1) != 0) {
		    local_atom_id++;
		    
		    if (val & 1) {
		      atom_id = local2atom_id [local_atom_id];
		      fprintf (xml_file, "      <value atomic_val=\"%s\"/>\n", ATOM_ID2STRING (atom_id));
		    }
		  }
		}
	      }
#else /* ESSAI */
	      if (local_atom_id = get_ufs_atom_val (orig_field_val >> FS_ID_SHIFT, &is_shared, left_local_atom_id_set)) {
#if 0
		if (is_shared)
		  fputs ("(", stdout);
#endif /* 0 */

		SXBA_0_bit (left_local_atom_id_set, local_atom_id);
		atom_id = X_X (atom_id2local_atom_id_hd, local_atom_id);
		fprintf (xml_file,"      <value atomic_val=\"%s\"", atom_id2string [atom_id]);
		if (is_shared)
		  fputs (" shared=\"+\"", xml_file);
		fprintf (xml_file, "/>\n");

		while ((local_atom_id = sxba_scan_reset (left_local_atom_id_set, local_atom_id)) > 0) {
		  atom_id = X_X (atom_id2local_atom_id_hd, local_atom_id);
		  fprintf (xml_file, "      <value atomic_val=\"%s\"", atom_id2string [atom_id]);
		  if (is_shared)
		    fputs (" shared=\"+\"", xml_file);
		  fprintf (xml_file, "/>\n");
		}

	      }
#if 0 // BS: à retravailler
	      else {
		if (is_shared)
		  /* shared */
		  fputs ("()shared", stdout);
		else
		  fputs ("()", stdout);
	      }
#endif /* 0 */
#endif /* ESSAI */
	    }
	}
	else {
	  if (static_field_kind == STRUCT_KIND) {
	    if (val == 0) {
	      fputs ("<value emtpy_val=\"\"/>\n", xml_file);
	    }
	    else {
	      if (SXBA_bit_is_reset_set (fs_id_set, val))
		PUSH (fs_id_stack, val);
		
	      fprintf (xml_file, "      <value idref=\"F%i\"/>\n", val);
	    }
	  }
	  else {
	    if (static_field_kind == STRUCT_KIND+UNBOUNDED_KIND) {
	      if (val == 0)
		fputs ("      <value emtpy_val=\"\"/>\n", xml_file);
	      else {
		for (cur2 = bot2 = XH_X (XH_ufs_hd, val), top2 = XH_X (XH_ufs_hd, val+1);
		     cur2 < top2;
		     cur2++) {
		  val = XH_list_elem (XH_ufs_hd, cur2);

		  if (val < 0)
		    break;

		  if (SXBA_bit_is_reset_set (fs_id_set, val))
		    PUSH (fs_id_stack, val);

		  fprintf (xml_file, "      <value idref=\"F%i\"/>\n", val);
		}
	      }
	    }
	  }
	}

	fputs ("    </attribute>\n", xml_file);
      }

      bot++;
    }

    fputs ("  </f_structure>\n", xml_file);
  }

  sxba_empty (fs_id_set), fs_id_stack [0] = 0;
}

static void
xml_print_f_structures ()
{
  int        head, x, y, fs_id, cur_nb, Xpq, hook, prod, rule, i;
  int        *local_heads_stack;
  struct lhs *plhs;
#if EBUG
  bool    found;
#endif /* EBUG */

  fputs ("<f_structures>\n", xml_file); 

  cur_nb = 0;

  if (heads_stack) {
    for (x = 1; x <= heads_stack [0]; x++) {
      head = heads_stack [x];
      if (head < 0) /* structure inconsistente ? */
	head = -head;
      fs_id = XxY_Y (heads, head);

      if (SXBA_bit_is_reset_set (fs_id_set, fs_id)) {
	PUSH (fs_id_stack, fs_id);
	cur_nb++;
      }
    }

    fprintf (xml_file, "  <main_f_structures kind=\"%s\" relaxed=\"%s\" consistent=\"%s\" ranking=\"%s\" nb=\"%i\">\n",
	     is_partial ? "partial" : "complete",
	     is_relaxed_run ? "yes" : "no",
	     is_relaxed_run ? "no" : (is_consistent ? "yes" : "no"),
	     with_ranking ? "yes" : "no",
	     cur_nb);

    /* Le 16/06/04 On associe la production (ou le terminal) qui a produit la f_structure */
#if EBUG
    found = false;
#endif /* EBUG */

    for (y = 1; y <= heads_stack [0]; y++) {
      head = heads_stack [y];
      Xpq = XxY_X (heads, (head >= 0) ? head : -head);
      fs_id = XxY_Y (heads, (head >= 0) ? head : -head);

      if (Xpq > spf.outputG.maxxnt) {
	/* C'est une feuille terminale */
	Xpq -= spf.outputG.maxxnt;
	fprintf (xml_file, "    <main_f_structure idref=\"F%i\"/>\n", fs_id); // leaf_id = Xpq
      }
      else {
	/* C'est un non terminal */
	/* On cherche toutes les Xpq-prods */
	hook = spf.outputG.lhs [spf.outputG.maxxprod+Xpq].prolon;

	while ((prod = spf.outputG.rhs [hook++].lispro) != 0) {
	  /* head peut etre trouve pour des prod differentes */
	  if (prod < 0)
	    /* On doit considerer les prod "effacees" car on a efface' les prods de tous les noeuds
	       ayant des f_structures de + haut niveau */
	    prod = -prod;

	  if (x = Pij2disp [prod]) {
	    /* ... valide */
	    local_heads_stack = fs_id_Pij_dstack+x;
	    plhs = spf.outputG.lhs+prod;

	    for (rule = prod2rule [plhs->init_prod]; rule != 0; rule = rule2next_rule [rule]) {
	      if (x = *local_heads_stack) {
		for (i = 1; i <= x; i++) {
		  if (head == local_heads_stack [i++]) {
		    /* C'est une bonne */
		    fprintf (xml_file, "    <main_f_structure idref=\"F%i\"/>\n", fs_id); // rule_id= R_%i_%i, prod, plhs->init_prod
#if EBUG
		    found = true;
#endif /* EBUG */
		    break;
		  }	      
		}

		if (i <= x)
		  /* une seule rule par prod */
		  break;
	      }
	
	      local_heads_stack += x+1;
	    }
	  }
	}

#if EBUG
	if (!found)
	  /* head doit etre trouve' */
	  sxtrap (ME, "xml_print_f_structures");
	else
	  found = false;
#endif /* EBUG */
      }
    }

    fputs ("  </main_f_structures>\n", xml_file);

    xml_print_f_structure ();
  }

  fputs ("</f_structures>\n", xml_file); 
}

static void
xml_output (bool print_unique_tree)
{
  int tok_no;

  if (is_print_xml_headers) {
    fputs ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"yes\"?>\n", xml_file); 
    fputs ("<DOCUMENT parserGenerator=\"sxlfg\">\n", xml_file); 
  }

  xml_gen_header ();

  fprintf (xml_file, "<E id=\"E%i\"", sentence_id); 
  /* if (on veut l'input comme attribut de l'élément E) { */
  fprintf (xml_file, " input=\"");
  for (tok_no = 1; tok_no <= last_tok_no; tok_no++) {
    fprintf (xml_file, "%s%s", sxstrget (toks_buf [tok_no].string_table_entry), tok_no == last_tok_no ? "" : " ");
  }
  fprintf (xml_file, "\""); 
  /* } */
  fprintf (xml_file, ">\n"); 

  if (print_unique_tree) {
    fputs("<c_structure>\n",xml_file);
    offset = 1;
    xml_print_unique_tree (spf.outputG.start_symbol);
    fputs("</c_structure>\n",xml_file);
  }
  else
    xml_print_forest ();

  xml_print_f_structures ();

  fputs("</E>\n",xml_file);
  
  if (is_print_xml_headers)
    fputs ("</DOCUMENT>\n", xml_file); 
}
#endif /* !no_lfg */

extern int print_chunks(int Pij) ;

extern int print_props(int Pij) ;


static void
dag_output ()
{
#ifndef no_lfg
  if (is_consistent)
    compact_infos[0]='C';
  else if (is_partial)
    compact_infos[0]='P';
  else
    compact_infos[0]='U';
#endif /* !no_lfg */
  if (spf.outputG.is_error_detected)
    compact_infos[1]='R';

  fprintf (odag_file,"%s // %s\n", get_dag_sentence_str (
#ifndef no_lfg
							 is_unique_parse
#else /* !no_lfg */
							 true
#endif /* !no_lfg */
							 ), compact_infos);
}

#ifndef no_chunker
extern char* ntstring2chunksname(char* ntstring);

static void
chunks_output ()
{
  fprintf(xml_file,"<E id=\"E%i\">\n",sentence_id);
  fprintf(xml_file,"<constituants>\n");
  nl_done=true;
  spf_output_partial_forest(ntstring2chunksname, "Groupe", "G", 1);
  //  spf_topological_walk (spf.outputG.start_symbol,NULL,print_chunks);
  fprintf(xml_file,"</constituants>\n");
  fprintf(xml_file,"</E>\n");
}
#endif /* !no_chunker */

#ifndef no_proper
extern char* ntstring2propsname(char* ntstring);

static void
propositions_output()
{
  int i;

  rec_level=0;

  fprintf(xml_file,"<E id=\"E%i\">\n",sentence_id);
  fprintf(xml_file,"<propositions>\n");
  nl_done=true;
  spf_output_partial_forest(ntstring2propsname, "Proposition", "P", 1);
  /*  spf_output_partial_forest(ntstring2propname, "Proposition", "P", 1);*/
  fprintf(xml_file,"</propositions>\n");
  fprintf(xml_file,"</E>\n");
}
#endif /* !no_proper */

static void
props_output()
{
  fprintf(xml_file,"<E id=\"E%i\">\n",sentence_id);
  nl_done=true;
  spf_topological_walk (spf.outputG.start_symbol,NULL,print_props);
  fprintf(xml_file,"</E>\n");
}

static void
phrases_output()
{
  int i;

  rec_level=0;

  fprintf(xml_file,"<E id=\"E%i\">\n",sentence_id);
  nl_done=true;
  spf_output_partial_forest(ntstring2phrasename, "Syntagme", "S", 1);
  fprintf(xml_file,"</E>\n");
}

static int
output_sem_pass ()
{
  char msg[30];
  int sentence_no, sentence_id;

  /*  extern void print_c_structures();
      spf_topological_walk (spf.outputG.start_symbol, print_c_structures, NULL);*/

  sentence_no = easy_get_sentence_no ();
  if (sentence_no != 0)
    /* On force sentence_id */
    sentence_id = sentence_no;
  else
    sentence_id = 0;

  maxxt = -spf.outputG.maxt;
  spf_fill_Tij2tok_no();

  switch (output_sem_pass_arg) {
  case 'd':
    if (odag_file_name) {
      if ((odag_file = sxfopen (odag_file_name, "w")) == NULL) {
	fprintf (sxstderr, "%s: Cannot open (create) ", ME);
	sxperror (odag_file_name);
	return false;
      }
    } else
    odag_file=stdout;
    break;
  default:
    if (xml_file_name) {
      if ((xml_file = sxfopen (xml_file_name, "w")) == NULL) {
	fprintf (sxstderr, "%s: Cannot open (create) ", ME);
	sxperror (xml_file_name);
	return false;
      }
    } else
      xml_file=stdout;
    break;
  }

  switch (output_sem_pass_arg) {
  case 'f':
    spf_print_td_forest(stdout, spf.outputG.start_symbol);
    break;
  case 'd':
    dag_output();
    break;
#ifndef no_proper
  case 'p':
       propositions_output();
/*     props_output(); */
    break;
#else /* !no_proper */
    sxtrap(ME,"output_sem_pass (this executable has been compiled with the -Dno_proper option ; you can't call the proper output)");
#endif /* !no_proper */
  case 'S':
    phrases_output();
    break;
  case 'E':
#ifndef no_chunker
    chunks_output();
#else /* !no_chunker */
    sxtrap(ME,"output_sem_pass (this executable has been compiled with the -Dno_chunker option ; you can't call the chunker output)");
#endif /* !no_chunker */
    break;
#ifndef no_lfg
#ifdef EASY
  case 'e':
    if (qq_valeurs_positionnees)
      easy_campaign();
    else
      sxtrap(ME,"output_sem_pass (lfg semantic pass needed before any easy output)");
    break;
#endif /* EASY */
  case 'x':
    xml_output(false);
    break;
  case 'X':
    xml_output(true);
    break;
  case 'w':
    /* On ne sort les stats que s'il n'y a eu ni erreur de syntaxe ni incoherence */
    if (weights_file_name) {
      if (is_consistent && sxerrmngr.nbmess [2] == 0)
	weights_campaign();
    } else
      sxtrap(ME,"output_sem_pass (weights output requested, but no weight file name given (use option -wf filename)");
    break;
#endif /* !no_lfg */
  }

#if LOG
  printf ("\n ******* Tagged input sentence with f_structure filtering:\n%s\n", get_dag_sentence_str (
#ifndef no_lfg
							 is_unique_parse
#else /* !no_lfg */
							 true
#endif /* !no_lfg */
							 ));
#endif /* LOG */

  if (is_print_time) {
    sprintf(msg, "\tOutput (%c)", output_sem_pass_arg);
    sxtime (SXACTION,msg);
  }

  return 1;
}

void
output_sem_final() {
  if (xml_file && xml_file != stdout)
    fclose(xml_file);
  if (odag_file && odag_file != stdout)
    fclose(odag_file);
}

void
output_semact ()
{

  for_semact.sem_pass = output_sem_pass;
  for_semact.process_args = output_args_decode; /* decodage des options de output */
  for_semact.string_args = output_args_usage; /* decodage des options de output */
  for_semact.ME = output_ME; /* decodage des options de output */
  for_semact.sem_final = output_sem_final;
}

