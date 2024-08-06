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

static char	ME [] = "output_semact";

#include "sxversion.h"
#include "sxunix.h"
#include "earley.h"
#include "XH.h"
#include "varstr.h"
#include "sxstack.h"
#include "udag_scanner.h"
#include <math.h>
#include <float.h>
#include "X.h"

char WHAT_OUTPUT_SEMACT[] = "@(#)SYNTAX - $Id: output_semact.c 3911 2024-04-22 10:39:16Z garavel $" WHAT_DEBUG;

static bool          is_print_headers;
static bool          is_print_xml_headers;

#define SENTENCE_CONCAT 0X80000000
#define SENTENCE_OR     0X40000000
static SXINT              maxxt; /* nb des terminaux instancies Tij, y compris ceux couverts par les nt de la rcvr == -spf.outputG.maxt */

FILE    *xml_file = NULL;
static char    *xml_file_name;
static FILE    *odag_file;
static char    *odag_file_name;

static char    compact_infos[32];

#include "sxword.h"

#define Tpq2str(Tpq)  (Tpq ? (spf.inputG.tstring [-Tij2T (Tpq-spf.outputG.maxxnt)]) : NULL)

static SXINT            sentence_id; /* nbest peut en avoir besoin */
/* extern char           *sntid2string[MAX_SNT_ID+1]; */
extern char             output_sem_pass_arg;

/* valeur par defaut */

static char	Usage [] = "\
\t\t-xml pathname,\n\
\t\t-odag pathname,\n\
\t\t-ph, -print_headers,\n\
\t\t-pxh, -print_xml_header\n\
\t\t-odag,\n\
";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 	   0
#define XML                1
#define PRINT_HEADERS      2
#define PRINT_XML_HEADER   3
#define ODAG               4

static char	*option_tbl [] = {
    "",
    "xml",
    "ph","print_headers",
    "pxh","print_xml_header",
    "odag",
};

static SXINT	option_kind [] = {
    UNKNOWN_ARG,
    XML,
    PRINT_HEADERS, PRINT_HEADERS,
    PRINT_XML_HEADER, PRINT_XML_HEADER,
    ODAG,
};

static SXINT
option_get_kind (char *arg)
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



static char *
option_get_text (SXINT kind)
{
  register SXINT	i;

  for (i = OPT_NB; i > 0; i--) {
    if (option_kind [i] == kind)
      break;
  }

  return option_tbl [i];
}


/* retourne le ME */
static char*
output_ME (void)
{
  return ME;
}

/* retourne la chaine des arguments possibles propres a output */
static char*
output_args_usage (void)
{
  return Usage;
}

/* decode les arguments specifiques a output */
/* l'option argv [*parg_num] est inconnue du parseur earley */
static bool
output_args_decode (int *pargnum, int argc, char *argv [])
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

  case PRINT_HEADERS:
    is_print_headers = true;
    break;

  case PRINT_XML_HEADER:
    is_print_xml_headers = true;
    break;

  case UNKNOWN_ARG:
  default:
    return false;
  }

  return true;
}


#ifdef proper
static char*
ntstring2propname (char *ntstring)
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
#endif /* proper */

#ifdef phrase
static char*
ntstring2phrasename (char *ntstring)
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
#endif /* phrase */

#if defined(phrase) || defined(proper) || defined(chunker)

static SXINT              *spf_count;
static SXINT              rec_level;
static bool          nl_done;
static SXINT cur_snt_id;

/* parcours top-down y compris les terminaux: boucle récursive */
static void
spf_td_walk_incl_terminals (SXINT Aij, char* (*ntstring2name) (char *), char *ntkind, char *ntkindshort)
{
  SXINT            output_prod, output_item, Xpq, hook, i, tok_no;
  SXINT            Aij_val, local_f;
  char             *ntstring, *comment, *str1, *str2;
  struct sxtoken   *tok_ptr = NULL;
  static SXINT     last_f;

  if (Aij == spf.outputG.start_symbol)
    last_f = 0;

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
      fprintf(xml_file,"<%s type=\"%s\" id=\"E%ld%s%ld\">", ntkind, ntstring, sentence_id, ntkindshort, cur_snt_id++);
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
	    //	    fprintf(xml_file,"(%ld <? %ld)\n",Xpq,spf.outputG.maxnt);
	    if (Xpq > 0) {
	      /* nt */
	      spf_td_walk_incl_terminals (Xpq, ntstring2name, ntkind, ntkindshort);
	    }
	    else {
	      /* terminal*/
	      if (!nl_done)
		fputs ("\n",xml_file);
	      nl_done = true;

	      tok_no = spf_get_Tij2tok_no_stack (-Xpq)[1]; /* le premier token de Tpq  (ils ont tous le meme commentaire) */
	      tok_ptr = &(SXGET_TOKEN (tok_no));
	      comment = tok_ptr->comment;

	      if (comment) {
		comment = strchr (comment, (int)'{') + 1;
		do {
		  str1 = strchr (comment, (int)'F');
		  str2 = strchr (str1 + 3, (int)'F');
		  str1 /* str3 */ = strchr (str2, (int)'"');
		  *str1 = SXNUL;
		  local_f = atoi(str2+1);
		  *str1 = '"';
		  if (local_f > last_f) {
		    last_f = local_f;
		    if (!nl_done)
		      fputs("\n",xml_file);
		    nl_done = true;
		    for (i = 0; i <= rec_level; i++)
		      fprintf(xml_file, "  ");
		    do {
		      fprintf(xml_file, "%c", *comment);
		      nl_done = false;
		    } while (*++comment != '}');
		  } else {
		    comment = strchr (comment, (int)'>');
		    comment = strchr (comment + 1, (int)'>');
		    comment ++;
		  }
		} while (*++comment != SXNUL);
	      } else { // pas de commentaires: on sort la ff dans un commentaire XML
		fprintf(xml_file, "<!-- %s -->\n", sxstrget (tok_ptr->string_table_entry));
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
#endif /* defined(phrase) || defined(proper) || defined(chunker) */

#if defined(phrase) || defined(proper) || defined(chunker)
static void
spf_output_partial_forest (char* (*ntstring2name)(char *), char *ntkind, char *ntkindshort)
{
  SXINT i;

  if (spf.outputG.start_symbol != 0) {
    spf_count = (SXINT*) sxalloc (spf.outputG.maxxnt+1, sizeof (SXINT));
    for (i=0;i<=spf.outputG.maxxnt;i++)
      spf_count[i]=-1;

    cur_snt_id = 1;
    spf_td_walk_incl_terminals (spf.outputG.start_symbol, ntstring2name, ntkind, ntkindshort);

    sxfree (spf_count), spf_count = NULL;
  }
  if (!nl_done)
    fprintf(xml_file,"\n");

}
#endif /* phrase || proper || chunker */







SXINT
easy_get_sentence_no (void)
{
  SXINT  sentence_no = 0;
  char *str1, *str2, *comment;

  comment = SXGET_TOKEN (1).comment;

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

/* On regarde si le nom du nt en lhs de Pij commence par Easy_A */
int
seek_prefix_Aij (char *Easy_A, SXINT Pij)
{
  size_t lgth;
  SXINT  Aij;
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
seek_Aij (char *A, SXINT Pij)
{
  size_t lgth;
  SXINT Aij;
  char *str;

  lgth = strlen (A);
  Aij = spf.outputG.lhs [Pij].lhs;
  str = spf.inputG.ntstring [Aij2A (Aij)];

  if (strlen (str) == lgth && strncmp (str, A, lgth) == 0) {
    return Aij;
  }
  
  return 0;
}



static SXINT
xml_count_leaves (void)
{
  SXINT  Tpq, p, q, t_nb;
  SXINT  *tok_no_stack, *top_tok_no_stack;

  t_nb = 0;

  for (Tpq = 1; Tpq <= maxxt; Tpq++) {
    /* T est un terminal */
    p = Tij2i (Tpq);
    q = Tij2j (Tpq);

    if (p >= 0 && q >= 0) {
      tok_no_stack = spf_get_Tij2tok_no_stack (Tpq);
      top_tok_no_stack = tok_no_stack + *tok_no_stack;

      while (++tok_no_stack <= top_tok_no_stack) {
	/* La transition tok_no entre p et q contient bien le terminal Tpq2T (Tpq) */
	t_nb++;
      }
    }
    else {
      /* Cas d'un xt */
      t_nb++;
    }
  } 

  return t_nb;
}


static void
xml_print_terminal_leaves (void)
{
  SXINT  Tpq, T, p, q, tok_no;
  SXINT  *tok_no_stack, *top_tok_no_stack;
  char   *terminal_string, *short_tstring, *c;

  fputs ("  <terminals>\n", xml_file); 

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
      tok_no_stack = spf_get_Tij2tok_no_stack (Tpq);
      top_tok_no_stack = tok_no_stack + *tok_no_stack;

      while (++tok_no_stack <= top_tok_no_stack) {
	tok_no = *tok_no_stack;
	/* La transition tok_no entre p et q contient bien le terminal T */
	terminal_string = sxstrget (SXGET_TOKEN (tok_no).string_table_entry);
	fprintf (xml_file,
		 "    <terminal id=\"T%ld\" name=\"%s\" realName=\"%s\" form=\"%s\" lb=\"%ld\" ub=\"%ld\"/>\n",
		 Tpq,
		 short_tstring,
		 spf.inputG.tstring [T],
		 terminal_string,
		 p,
		 q
		 );

      }
    }
    else {
      /* Cas d'un xt */
      fprintf (xml_file,
	       "    <terminal id=\"T%ld\" rcvr=\"+\" name=\"%s\" realName=\"%s\" lb=\"%ld\" ub=\"%ld\"/>\n",
	       Tpq,
	       short_tstring,
	       spf.inputG.tstring [T],
	       p,
	       q
	       );
      
    }
  } 

  fputs ("  </terminals>\n", xml_file); 
}


static void
xml_print_nt_set (SXBA Aij_set)
{
  SXINT Aij, hook, prod;
  
  fputs ("  <non_terminals>\n", xml_file); 

  Aij = 0;

  while ((Aij = sxba_scan (Aij_set, Aij)) > 0) {
    if (Aij <= spf.outputG.maxnt) {
      if (Aij2i(Aij) < Aij2j(Aij)) { /* on n'affiche pas les Aij vides */
	/* nt */
	fprintf (xml_file,
		 "    <non_terminal id=\"N%ld\" name=\"%s\" lb=\"%ld\" ub=\"%ld\">\n",
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
	    fprintf (xml_file, "      <rule idref=\"E%ldR_%ld_%ld\"/>\n", sentence_id, prod, spf.outputG.lhs [prod].init_prod);
	}
	fputs("    </non_terminal>\n",xml_file);
      }
    }
    else {
      /* xnt */
      fprintf (xml_file,
	       "    <non_terminal id=\"N%ld\" rcvr=\"+\" name=\"%s\" lb=\"%ld\" ub=\"%ld\"/>\n",
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

static SXINT offset;

static void
xml_print_terminal (SXINT Tij)
{
  SXINT i;
  char *comment, *str1, *str2;
  SXINT local_f = 0, last_f = 0, tok_no;
  struct sxtoken *tok_ptr = NULL;

  for (i=0; i<offset; i++)
    fputs("  ",xml_file);
  fprintf(xml_file,"<T id=\"E%ldT%ld\" name=\"%s\">\n",sentence_id, Tij,spf.inputG.tstring[-Tij2T (Tij)]);

  for (i=0; i<=offset; i++)
    fputs("  ",xml_file);

  if (!SXBA_bit_is_set (spf.outputG.Tpq_rcvr_set, Tij)) {
    tok_no = spf_get_Tij2tok_no_stack (Tij)[1]; /* on ne prend que le premier token :( */
    comment = SXGET_TOKEN (tok_no).comment;
    
    if (comment) {
      comment++;
      do {
	str1 = strchr (comment, (int)'F');
	str2 = strchr (str1+3, (int)'F');
	str1 /* str3 */ = strchr (str2, (int)'"');
	*str1 = SXNUL;
	local_f = atoi(str2+1);
	*str1 = '"';
	if (local_f > last_f) {
	  last_f = local_f;
	  do {
	    if (*comment == '&')
	      fprintf(xml_file, "&amp;");
	    else
	      fprintf(xml_file, "%c", *comment);
	  } while (*++comment != '}');
	} else {
	  comment = strchr (comment, (int)'>');
	  comment = strchr (comment+1, (int)'>');
	  comment += 2;
	}
      } while (*++comment != SXNUL);
      fputs("\n", xml_file);
    } else { // pas de commentaires: on sort la ff dans un commentaire XML
      fprintf(xml_file, "<!-- %s -->\n", sxstrget (tok_ptr->string_table_entry));
    }
  } else {
      fprintf(xml_file, "<!-- (error) -->\n");
  }
  for (i = 0; i < offset; i++)
    fputs ("  ", xml_file);
  fputs ("</T>\n", xml_file);
}

static void
xml_print_unique_tree (SXINT Aij)
{
  SXINT Pij, Xpq, i, hook;
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
    fprintf(xml_file,"<node name=\"%s\" lb=\"%ld\" ub=\"%ld\" id=\"E%ldR_%ld_%ld\">\n", ntstring, Aij2i (Aij), Aij2j (Aij), sentence_id, Pij, spf.outputG.lhs [Pij].init_prod);
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
xml_print_forest (void)
{
  SXINT  Aij, hook, prod, item, Xpq;
  SXBA prod_set, Aij_set;

  prod_set = sxba_calloc (spf.outputG.maxxprod+1);
  Aij_set = sxba_calloc (spf.outputG.maxxnt+1);

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

  fprintf (xml_file,
	   "<parse_forest t_nb=\"%ld\" nt_nb=\"%ld\" prod_nb=\"%ld\">\n",
	   xml_count_leaves(),
	   (SXINT) sxba_cardinal (Aij_set),
	   (SXINT) sxba_cardinal (prod_set)
	   );

  fprintf (xml_file, "  <axiom idref=\"E%ldN%ld\"/>\n",sentence_id, spf.outputG.start_symbol);

  xml_print_terminal_leaves ();

  /* Sortir les non-terminaux */
  xml_print_nt_set (Aij_set);
  /* Les xnt ont ete enleves de Aij_set */
  
  fputs ("  <rules>\n", xml_file); 

  prod = 0;

  while ((prod = sxba_scan (prod_set, prod)) > 0) {
    item = spf.outputG.lhs [prod].prolon;
    Aij = spf.outputG.lhs [prod].lhs;

    fprintf (xml_file,
	     "    <rule id=\"E%ldR_%ld_%ld\">\n",
	     sentence_id,
	     prod,
	     spf.outputG.lhs [prod].init_prod);

    spf_print_iprod (xml_file, prod, "      <!-- ", " -->\n");

    fprintf (xml_file, "      <lhs_node idref=\"E%ldN%ld\"/>\n", sentence_id, Aij);

    while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
      /* On parcourt la RHS de prod */
      if (Xpq > 0) {
	/* nt ... */
	if (Xpq > spf.outputG.maxnt || Aij2i (Xpq) < Aij2j (Xpq)) {
	  /* ... non vide ou xnt */
	  fprintf (xml_file, "      <rhs_node idref=\"E%ldN%ld\"/>\n", sentence_id, Xpq);
	}
      }
      else {
	/* T */
	fprintf (xml_file, "      <rhs_node idref=\"E%ldT%ld\"/>\n", sentence_id, -Xpq);
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
xml_escape_fprintf (FILE *file, char *string)
{
  size_t length = strlen (string);

  while (length) {
    if (*string == '&') {
      fprintf (file, "&amp;");
    } else if (*string == '<') {
      fprintf (file, "&lt;");
    } else if (*string == '>') {
      fprintf (file, "&gt;");
    } else {
      fprintf (file, "%c", *string);
    }
    length--;
    string++;
  }
}

static void
xml_gen_header (void)
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

static void
xml_output (bool print_unique_tree)
{
  SXINT tok_no;

  if (is_print_xml_headers) {
    fputs ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"yes\"?>\n", xml_file); 
    fputs ("<DOCUMENT parserGenerator=\"sxlfg\">\n", xml_file); 
  }

  xml_gen_header ();

  fprintf (xml_file, "<E sid=\"E%ld\"", sentence_id); 
  /* if (on veut l'input comme attribut de l'élément E) { */
  fprintf (xml_file, " input=\"");

  for (tok_no = 1; tok_no <= idag.toks_buf_top; tok_no++) {
    fprintf (xml_file, "%s", tok_no == 1 ? "" : " ");
    xml_escape_fprintf (xml_file, sxstrget (SXGET_TOKEN (tok_no).string_table_entry));
  }

  fprintf (xml_file, "\" rcvr=\"%i\"", spf.outputG.is_error_detected); 
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

  fputs("</E>\n",xml_file);
  
  if (is_print_xml_headers)
    fputs ("</DOCUMENT>\n", xml_file); 
}




static void
dag_output (void)
{
  if (spf.outputG.is_error_detected)
    compact_infos[1]='R';

  spf_yield2dfa (false /*= DAG ; true = UDAG */);
}

#ifdef chunker
extern SXINT print_chunks (SXINT Pij) ;
extern char* ntstring2chunksname(char* ntstring);

static void
chunks_output (void)
{
  fprintf(xml_file,"<E sid=\"E%ld\">\n",sentence_id);
  fprintf(xml_file,"<constituants>\n");
  nl_done=true;
  spf_output_partial_forest (ntstring2chunksname, "Groupe", "G");
  //  spf_topological_walk (spf.outputG.start_symbol,NULL,print_chunks);
  fprintf(xml_file,"</constituants>\n");
  fprintf(xml_file,"</E>\n");
}
#endif /* chunker */

#ifdef proper
extern SXINT print_props (SXINT Pij) ;
extern char* ntstring2propsname (char* ntstring);

static void
propositions_output (void)
{
  SXINT i;

  rec_level=0;

  fprintf(xml_file,"<E sid=\"E%ld\">\n",sentence_id);
  fprintf(xml_file,"<propositions>\n");
  nl_done=true;
  spf_output_partial_forest (ntstring2propsname, "Proposition", "P");
  /*  spf_output_partial_forest (ntstring2propname, "Proposition", "P", 1);*/
  fprintf(xml_file,"</propositions>\n");
  fprintf(xml_file,"</E>\n");
}

static void
props_output (void)
{
  fprintf(xml_file,"<E sid=\"E%ld\">\n",sentence_id);
  nl_done=true;
  spf_topological_walk (spf.outputG.start_symbol,NULL,print_props);
  fprintf(xml_file,"</E>\n");
}
#endif /* proper */

#ifdef phrases
static void
phrases_output (void)
{
  SXINT i;

  rec_level=0;

  fprintf(xml_file,"<E sid=\"E%ld\">\n",sentence_id);
  nl_done=true;
  spf_output_partial_forest (ntstring2phrasename, "Syntagme", "S");
  fprintf(xml_file,"</E>\n");
}
#endif /* phrases */

static SXINT
output_sem_pass (void)
{
  char msg[30];
  SXINT sentence_no;

  /*  extern void print_c_structures();
      spf_topological_walk (spf.outputG.start_symbol, print_c_structures, NULL);*/

  sentence_no = easy_get_sentence_no ();
  if (sentence_no != 0)
    /* On force sentence_id */
    sentence_id = sentence_no;
  else
    sentence_id = 0;

  maxxt = -spf.outputG.maxt;

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
    spf_print_bu_forest (stdout, spf.outputG.start_symbol);
    //    spf_print_td_forest (stdout, spf.outputG.start_symbol); /* il semble qu'il y ait un pb: en huge, on a parfois des résultats différents avec ces deux fonctions, et apparemment la bonne réponse est donnée par spf_print_bu_forest */
    break;
  case 'F':
    spf_print_bu_forest (stdout, spf.outputG.start_symbol);
    fputs ("\n", sxstdout);
    //    spf_print_td_forest (stdout, spf.outputG.start_symbol); /* il semble qu'il y ait un pb: en huge, on a parfois des résultats différents avec ces deux fonctions, et apparemment la bonne réponse est donnée par spf_print_bu_forest */
    break;
  case 't':
    spf_unfold ();
    break;
  case 'i':
    spf_print_signature (stdout, "Forest signature: ");
    break;
  case 'd':
    dag_output();
    break;
  case 'p':
#ifdef proper
       propositions_output ();
/*     props_output(); */
#else /* proper */
    sxtrap(ME,"output_sem_pass (this executable has been compiled without the -Dproper option ; you can't call the proper output)");
#endif /* proper */
    break;
  case 'S':
#ifdef phrases
    phrases_output();
#else
    sxtrap(ME,"output_sem_pass (this executable has been compiled without the -Dphrases option ; you can't call the phrases output)");
#endif /* phrases */
    break;
  case 'E':
#ifdef chunker
    chunks_output();
#else /* chunker */
    sxtrap(ME,"output_sem_pass (this executable has been compiled without the -Dchunker option ; you can't call the chunker output)");
#endif /* chunker */
    break;
  case 'x':
    xml_output(false);
    break;
  case 'X':
    xml_output(true);
    break;
  default:
    sxtrap(ME,"output_sem_pass (unknown action for output_semact module)");
    break;
  }

#if LOG
  printf ("\n ******* Tagged input sentence with f_structure filtering:\n");
  spf_yield2dfa (false /*= DAG ; true = UDAG */);
#endif /* LOG */

  if (is_print_time) {
    sprintf(msg, "\tOutput (%c)", output_sem_pass_arg);
    sxtime (SXACTION,msg);
  }

  return 1;
}

void
output_sem_final (void) {
  if (xml_file && xml_file != stdout)
    fclose(xml_file);
  if (odag_file && odag_file != stdout)
    fclose(odag_file);
}

void
output_semact (void)
{
  for_semact.sem_init = NULL;
  for_semact.sem_pass = output_sem_pass;
  for_semact.process_args = output_args_decode; /* decodage des options de output */
  for_semact.string_args = output_args_usage; /* decodage des options de output */
  for_semact.ME = output_ME; /* decodage des options de output */
  for_semact.sem_final = output_sem_final;
  for_semact.sem_close = NULL;
}

