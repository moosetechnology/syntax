/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 2005 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */

/* Actions s�mantiques permettant le filtrage de la for�t par l'etude d'une famille donn�e
de non-terminaux (chunks, propositions,...). Ces non-terminaux sont appel�s "special non terminals",
abr�g� en snt */


/* L'analyseur est de type Earley */
/* Le source est sous forme de SDAG */

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

static char	ME [] = "snt_semact";

#include "sxunix.h"
#include "earley.h"
#include "dag_scanner.h"
#include "sxword.h"
#include "sxstack.h"
#include "varstr.h"
#include "XH.h"
#include td_h

#undef SX_GLOBAL_VAR
#undef SX_INIT_VAL
#define SX_GLOBAL_VAR	/*rien*/
#include "snt_semact.h"
#undef SX_GLOBAL_VAR
#undef SX_INIT_VAL
#define SX_GLOBAL_VAR	extern

//char WHAT_SNT_SEMACT[] = "@(#)SYNTAX - $Id: snt_semact.c 3678 2024-02-06 08:38:24Z garavel $" WHAT_DEBUG;

static char	Usage [] = "";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 	   0

static char	*option_tbl [] = {
    "",
};

static int	option_kind [] = {
    UNKNOWN_ARG,
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

/* retourne la chaine des arguments possibles propres a sntfilter */
static char*
sntfilter_args_usage ()
{
  return Usage;
}

/* decode les arguments specifiques a sntfilter */
/* l'option argv [*parg_num] est inconnue du parseur earley */
static bool
sntfilter_args_decode (pargnum, argc, argv)
     int  *pargnum, argc;
     char *argv [];
{
  switch (option_get_kind (argv [*pargnum])) {

  case UNKNOWN_ARG:
    return false;
  }

  return true;
}





static int               maxxt; /* nb des terminaux instancies Tij, y compris ceux couverts par les nt de la rcvr == -spf.outputG.maxt */

static double *Pij2weight,*Xpq2weight;
static SXBA Xpq_is_blessed;

static int *localmaxn, *sentence_offset;
static int minn;
static int maxn;
static int filter_id;

static sxword_header easy_tokrefs,easy_commrefs;
static int           **nm2Cnm, **nm2BlessedCnm;
static int           ***nmt2Cnmt;
static int           *i2n,*j2n,*n2easy_tokref,*n2easy_commref;
static VARSTR        easy_vstr;
static double        *spf_count;
static int           sentence_eid0;

static XxY_header    lbxub;
static XH_header     lbxub_list;
static int           *Aij2lbxub_list;

int     sentence_id;

#if 0
/* nom et poids des types de chunks ; pour les poids c'est un co�t: on va pr�f�rer un (ensemble de) chunk(s) de poids minimal */
char          *chunkid2string [MAX_CHUNK_ID+1] = {"GN","GA","GR","GP","NV","PV","ZZ"};
static double chunkid2weight_1Maj [MAX_CHUNK_ID+1] = {/* GN */ 1.4,/* GA */ 1.5,/* GR */ 1.1, /* GP */ 1.2, /* NV */ 1.3, /* PV */ 1.0,/* ZZ */ 1.6};
//static double chunkid2weight_1Maj [MAX_CHUNK_ID+1] = {/* GN */ 1.4,/* GA */ 1.5,/* GR */ 1.2, /* GP */ 1.1, /* NV */ 1.3, /* PV */ 1.0,/* ZZ */ 1.6};
static double chunkid2weight_1Min [MAX_CHUNK_ID+1] = {/* GN */ 1.5,/* GA */ 1.3,/* GR */ 1.1, /* GP */ 1.4, /* NV */ 1.2, /* PV */ 1.0,/* ZZ */ 1.6};
//static double chunkid2weight_1Min [MAX_CHUNK_ID+1] = {/* GN */ 1.5,/* GA */ 1.3,/* GR */ 1.4, /* GP */ 1.1, /* NV */ 1.2, /* PV */ 1.0,/* ZZ */ 1.6};
static double chunkid2weight_n [MAX_CHUNK_ID+1] = {/* GN */ 1.4,/* GA */ 1.1,/* GR */ 1.2, /* GP */ 1.3, /* NV */ 1.5, /* PV */ 1.0,/* ZZ */ 1.6};

int string2chunkid(nts)
     char* nts;
{
  if (nts[CHUNK_PREFIX_LENGTH]=='0' || nts[CHUNK_PREFIX_LENGTH]=='Z')
    return 6;
  else if (nts[CHUNK_PREFIX_LENGTH+1]=='V') {
    if (nts[CHUNK_PREFIX_LENGTH]=='P')
      return 5;
    else if (nts[CHUNK_PREFIX_LENGTH]=='N')
      return 4;
  }
  else if (nts[CHUNK_PREFIX_LENGTH]=='G') {
    if (nts[CHUNK_PREFIX_LENGTH+1]=='P')
      return 3;
    else if (nts[CHUNK_PREFIX_LENGTH+1]=='R')
      return 2;
    else if (nts[CHUNK_PREFIX_LENGTH+1]=='A')
      return 1;
    else if (nts[CHUNK_PREFIX_LENGTH+1]=='N')
      return 0;
  }
  return -1;
}
#endif /* 0 */

#define sntfilter_c_init
#include ntf_h
#undef sntfilter_c_init

extern bool       SNTFILTER_SEM_PASS_ARG;

static void
allocate_all ()
{
  int i,j,final_dagstate,trans,tok_no,sentence_eid,Tpq,T,p,q,fid,passed_length,comment_lgth;
  char *comment,*str1,*str2;
  struct sxtoken   *tok_ptr;

  sentence_eid0 = -1;
  localmaxn = (int*) sxcalloc (10, sizeof (int));
  sentence_offset = (int*) sxcalloc (10, sizeof (int));

  for (Tpq = 1; Tpq <= maxxt; Tpq++) {
    T = -Tij2T (Tpq);
#if EBUG
    if (T <= 0)
      sxtrap (ME, "allocate_all");
#endif /* EBUG */
    
    p = Tij2i (Tpq);
    q = Tij2j (Tpq);
    p = mlstn2dag_state (p);
    q = mlstn2dag_state (q);

    tok_ptr = parser_Tpq2tok (Tpq);
    comment = tok_ptr->comment;

    /*    XxYxZ_Xforeach (dag_hd, p, trans) {
      if (XxYxZ_Z (dag_hd, trans) == q) {
	tok_no = XxYxZ_Y (dag_hd, trans);
	comment = toks_buf [tok_no].comment;*/
	if (comment) {
	  comment_lgth = strlen (comment)-2;
	  passed_length=0;
	  while (passed_length < comment_lgth) {
	    /*<F id="E1F18">...</F>
	      1      2 3  4    5  6  */
	    str1 = strchr (comment + passed_length, (int)'<');
	    str2 = strchr (str1 + 3, (int)'E');
	    str1 /* str3 */ = strchr (str2, (int)'F');
	    *str1 = SXNUL;
	    sentence_eid = atoi(str2+1);
	    if (sentence_eid0 == -1)
	      sentence_eid0 = sentence_eid;
	    *str1='F';
	    str2 /* str4 */ = strchr(str1,(int)'"');
	    *str2 = SXNUL;
	    fid = atoi(str1 + 1);
	    if (localmaxn [sentence_eid-sentence_eid0] < fid)
	      localmaxn [sentence_eid-sentence_eid0] = fid;
	    if (minn > fid)
	      minn = fid;
	    *str2='"';
	    str1 /* str5 */ = strchr (str2, (int)'<');
	    str2 /* str6 */ = strchr (str1, (int)'>')+1;
	    passed_length = str2-comment;
	  }
	}
	/*      }
		}*/
  }
  i=0;
  maxn=0;
  while (localmaxn[i]>0) {
    if (i==0)
      sentence_offset[i]=0;
    else
      sentence_offset[i]=sentence_offset[i-1]+localmaxn[i-1];
    maxn+=localmaxn[i++];
  }


  Pij2weight = (double*) sxcalloc (spf.outputG.maxxprod+1, sizeof (double));
  Xpq2weight = (double*) sxcalloc (spf.outputG.maxxnt+1, sizeof (double));
  Xpq_is_blessed = sxba_calloc (spf.outputG.maxxnt+1);
  snts = (struct snt*) sxcalloc (spf.outputG.maxxprod+1, sizeof (struct snt));
  Aij2sntid = (int*) sxcalloc (spf.outputG.maxxnt+1, sizeof (struct snt));
  //  X_reuse (&X_amalgam_if_id_hd, "X_amalgam_if_id_hd", NULL, NULL);
  //  XH_unlock (amalgam_list);

  XxY_alloc (&lbxub, "lbxub", Xpq_top+2, 1, 0 /* pas de foreach */, 0, NULL /* lbxub_oflw */, NULL);
  XH_alloc (&lbxub_list, "lbxub_list", n+20+1, 1, 5, NULL, NULL); /* A VOIR */
  Aij2lbxub_list = (int*) sxalloc (spf.outputG.maxxnt+1, sizeof (int));

  easy_vstr = varstr_alloc (128);
  sxword_alloc (&easy_tokrefs, "easy_tokrefs", Xpq_top+2, 1, 8,
		sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL, NULL);
  sxword_alloc (&easy_commrefs, "easy_commrefs", Xpq_top+2, 1, 8,
		sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL, NULL);
  i2n = (int*) sxcalloc (Xpq_top+2, sizeof (int));
  j2n = (int*) sxcalloc (Xpq_top+2, sizeof (int));
  n2easy_tokref = (int*) sxcalloc (maxn+1, sizeof (int));
  n2easy_commref = (int*) sxcalloc (maxn+1, sizeof (int));
  nm2Cnm = (int**) sxcalloc (maxn+1, sizeof (int));
  for (i=0;i<=maxn;i++)
    nm2Cnm[i]= (int*) sxcalloc (maxn+1, sizeof (int));
  nm2BlessedCnm = (int**) sxcalloc (maxn+1, sizeof (int));
  for (i=0;i<=maxn;i++)
    nm2BlessedCnm[i]= (int*) sxcalloc (maxn+1, sizeof (int));
  nmt2Cnmt = (int***) sxcalloc (maxn+1, sizeof (int));
  for (i=0;i<=maxn;i++) {
    nmt2Cnmt[i]= (int**) sxcalloc (maxn+1, sizeof (int));
    for (j=0;j<=maxn;j++)
      nmt2Cnmt[i][j]= (int*) sxcalloc (MAX_SNT_ID+1, sizeof (int));
  }
}

static void
free_all ()
{
  int i,j;
  
  sxfree (localmaxn), localmaxn = NULL;
  sxfree (Pij2weight), Pij2weight = NULL;
  sxfree (Xpq_is_blessed), Xpq_is_blessed = NULL;
  sxfree (Xpq2weight), Xpq2weight = NULL;
  for (i=0;i<=maxsnt;i++) {
    if (snts[i].comm)
      varstr_free(snts[i].comm);
  }
  sxfree (snts), snts = NULL;
  sxfree (Aij2sntid), Aij2sntid = NULL;

  XxY_free (&lbxub);
  XH_free (&lbxub_list);
  sxfree (Aij2lbxub_list), Aij2lbxub_list = NULL;

  for (i=0;i<=maxn;i++) {
    for (j=0;j<=maxn;j++) {
      sxfree(nmt2Cnmt[i][j]);
      nmt2Cnmt[i][j]=NULL;
    }
    sxfree(nmt2Cnmt[i]);
    nmt2Cnmt[i]=NULL;
  }
  sxfree (nmt2Cnmt), nmt2Cnmt = NULL;
  for (i=0;i<=maxn;i++) {
    sxfree(nm2Cnm[i]);
    nm2Cnm[i]=NULL;
    sxfree(nm2BlessedCnm[i]);
    nm2BlessedCnm[i]=NULL;
  }
  sxfree (nm2Cnm), nm2Cnm = NULL;
  sxfree (nm2BlessedCnm), nm2BlessedCnm = NULL;
  sxword_free (&easy_tokrefs);
  sxword_free (&easy_commrefs);
}

static void
sntfilter_sem_final()
{
  free_all();
}


static int
snt_is_blessed (lb,ub,id)
     int lb,ub,id;
{
  return (ub<=maxn && (nm2BlessedCnm[lb][ub] & 1 << id)>0);
}

static int
snt_exists (lb,ub,id)
     int lb,ub,id;
{
  return (ub<=maxn && (nm2Cnm[lb][ub] & 1 << id)>0);
}

static int
snt_exists_any (lb,ub)
     int lb,ub;
{
  return (ub<=maxn && nm2Cnm[lb][ub] > 0);
}


extern  FILE    *xml_file;

static bool
is_an_snt (Aij)
     int Aij;
{
  char *ntstring;

  ntstring=spf.inputG.ntstring[Aij2A (Aij)];
  return (strncmp(ntstring,SNT_PREFIX,SNT_PREFIX_LENGTH)==0 
	  && strncmp(ntstring+strlen(ntstring)-7,"___STAR", 7) != 0
	  && strncmp(ntstring+strlen(ntstring)-7,"___PLUS", 7) != 0
	  && strncmp(ntstring+strlen(ntstring)-4,"___1", 4) != 0
	  && strncmp(ntstring+strlen(ntstring)-4,"___c", 4) != 0
	  );
}

static int
print_snts (Pij)
     int     Pij;
{
  struct snt *c;
  int i,sntnum,lb,ub,id,Aij,sntid;
  char *comm,*ntstring;

#if EBUG
  char *snt_prefix = SNT_PREFIX;
  int snt_prefix_length = SNT_PREFIX_LENGTH;
#endif /* EBUG */  
  
  Aij = spf.outputG.lhs [Pij].lhs;
  ntstring=spf.inputG.ntstring[Aij2A (Aij)];
  if (is_an_snt(Aij)&& string2sntid(ntstring)>=0 && (sntid=Aij2sntid[Aij])) {/* si c'est un nt de snt */
    /* on r�cup�re ses easy-limites */
    c=&snts[sntid];
    
    if (!(snt_exists(c->lb,c->ub,c->id)))
      fprintf(stderr,"Warning: found pruned snt (Aij=%i ; Pij=%i)!!!\n",Aij,Pij);
    
    if (c->bool == 0) {
#if EBUG
      if (c->id != MAX_SNT_ID) /* snt_exists(c->lb,c->ub,c->id) && : Test supprim�: en cas de mode robuste
			  dans sntfilter_semact (on �lague raison par raison), les snts "�limin�s"
			  par les heuristiques (=raisons) qui rendaient la for�t vide ne rendent pas
			  snt_exist, et pourtant on veut les garder */
	fprintf(xml_file,"<!-- Kept snt %i: %s[%i..%i]<%i..%i> -->\n",
		Aij2sntid[Aij],sntid2string[c->id],Aij2i (Aij),Aij2j (Aij),c->lb,c->ub);
      else
	fprintf(xml_file,"<!-- Kept external snt %i -->\n",Aij2sntid[Aij]);
#endif /* EBUG */
      /* if (snt_exists(c->lb,c->ub,c->id)) {*/ /* idem */
	if (c->id != MAX_SNT_ID) {
	  fprintf(xml_file,"<Groupe type=\"%s\" id=\"E%iG%i\">\n%s</Groupe>\n",
		  sntid2string[c->id],sentence_id,maxsnt++,varstr_tostr(c->comm));
	}
	else
	  fprintf(xml_file,"%s",varstr_tostr(c->comm));
	/* } */
    }
    c->bool=1;
  }
  return 1;
  
}

static void
print_snt (snt)
     struct snt snt;
{
  int Tij;

  printf ("%s[%i..%i]<%i..%i>\t%s ... %s\n",
	  sntid2string[snt.id],Aij2i(snt.Aij),Aij2j(snt.Aij), snt.lb, snt.ub, snt.cfirst, snt.clast);
  printf ("%s\n", varstr_tostr(snt.comm));
}

static int
load_snts (Pij)
     int Pij;
{
  int Aij, trans, lb, ub, id, tok_no, Tij, n, i, j;
  char *ntstring, currchar;
  VARSTR vstr;

  /*  spf_print_iprod (stdout, Pij, "","\n");*/
    
  Aij = spf.outputG.lhs [Pij].lhs;
  ntstring = spf.inputG.ntstring[Aij2A (Aij)];
  if (strncmp (ntstring,SNT_PREFIX,SNT_PREFIX_LENGTH) == 0 && (id = string2sntid (ntstring)) >= 0) {
    i = Aij2i (Aij);
    j = Aij2j (Aij);
    lb = i2n [i]; /* �a risque de merder */
    ub = j2n [j];
    if (ub==-1)
      ub=maxn;
    if (j>i && lb>0  /* pas de snt vide au sens des ff (ie des terminaux) */
	&& ub>=lb /* pas de snt vide au sens des commentaires */) {
      if (!snt_exists(lb,ub,id) /* si on n'a pas d�j� stock� un snt de type id entre lb et ub */ ) { 
	maxsnt++;
	snts[maxsnt].bool=0;
	snts[maxsnt].lb=lb;
	snts[maxsnt].ub=ub;
	snts[maxsnt].Aij=Aij;
	Aij2sntid[Aij]=maxsnt;
	snts[maxsnt].id=id;
	snts[maxsnt].cfirst=SXWORD_get (easy_tokrefs, n2easy_tokref [lb]);
	snts[maxsnt].clast=SXWORD_get (easy_tokrefs, n2easy_tokref [ub]);
	/* A DEMANDER: autre moyen pour r�cup�rer n'importe quel Tij commen�ant par un i connu? */
	nm2Cnm[lb][ub] |= 1 << id;
	nmt2Cnmt[lb][ub][id]=maxsnt;
	
	vstr = varstr_alloc(64);
	for (n=lb;n<=ub;n++) {
	  vstr = varstr_catenate(vstr,SXWORD_get (easy_commrefs, n2easy_commref [n]));/*,SXWORD_get (easy_tokrefs, n2easy_tokref [n])*/
	  vstr = varstr_catenate(vstr,"\n");
	}
	snts[maxsnt].comm = vstr;
	//    printf("nm2Cnm[%i][%i] = %i (%i)\n",lb,ub,nm2Cnm[lb][ub],id);
	//      printf("Snt %i: (%i)\n",Aij,i2Tij[i]);
#if EBUG
	printf("%s - ",ntstring);
	print_snt(snts[maxsnt]);
#endif /* EBUG */
      } else {
	Aij2sntid[Aij]=nmt2Cnmt[lb][ub][id];
#if EBUG
	printf("Snt %s[%i..%i]<%i..%i> already stored (snt %i, Aij=%i)\n",sntid2string[id],Aij2i (Aij),Aij2j (Aij), lb, ub, nmt2Cnmt[lb][ub][id],Aij);
#endif /* EBUG */
      }
    }
  }

  return 1;
}



static void
set_terminal_leaves ()
{
  int  Tpq, T, p, q, trans, if_id;
  char *word, *comment;
  int  n,  tokref_id, commref_id, comment_lgth, passed_length;
  char *str0, *str1, *str2, *str3, *str4, *str5, savedchar;
  struct sxtoken   *tok_ptr;


#if LOG
  fputs ("Terminal leaves\n", stdout);
#endif /* LOG */

  for (Tpq = 1; Tpq <= maxxt; Tpq++) {
    tok_ptr = parser_Tpq2tok (Tpq);

    if (SXBA_bit_is_set (spf.outputG.Tpq_rcvr_set, Tpq))
      /* terminal de l'error-recovery */
      if_id = 0;
    else
      if_id = tok_ptr->lahead;
	
    T = -Tij2T (Tpq);

    /* On peut donc instancier les f-structures lexicales associees a (if_id, T) */
    p = Tij2i (Tpq);
    q = Tij2j (Tpq);
    p = mlstn2dag_state (p);
    q = mlstn2dag_state (q);
    comment = tok_ptr->comment;
#if EBUG
    word = sxstrget (tok_ptr->string_table_entry);
    printf ("<%i, %s%s/%s, %i>(if_id=%i, Tpq=%i)\n",
	    p, (comment == NULL) ? "" : comment, word, spf.inputG.tstring [T], q, if_id, Tpq);
#endif /* EBUG */

    if (comment) {
      comment_lgth=strlen (comment)-2;
      passed_length=0;
      while (passed_length < comment_lgth) {
	/* On extrait le "eref", i.e. le num�ro de "F" easy imm�diatement � droite de la position p du dag */
	/* On prend le + a gauche */
	/* Prudence, on y accede par les ">" et "<"  a cause d'un >"< eventuel ... */
	str0 = strchr (comment+passed_length+3, (int)'E')+1;
	if (str0) str1 = strchr (str0, (int)'F')+1;
	if (str1) str2 = strchr (str1, (int)'"');
	if (str2) str3 = strchr (str2+1, (int)'>');
	if (str3) str4 = strchr (str3+1, (int)'<');
	if (str4) str5 = strchr (str4+1, (int)'>');
		
	if (str1 && str2 && str3 && str4 && str5) {
	  str5++;
	  /*          {<F id="E1F1">�</F> <F id="E1F2">moins</F> <F id="E1F3">que</F>} �_moins_que  
		       ^        ^ ^^^ ^   ^
		      comment  str0 1 23 4   5		*/
	  *str2 = SXNUL;
	  *(str1-1) = SXNUL;
	  n = atoi(str1)+sentence_offset[atoi(str0)-sentence_eid0];
	  /* if (maxn<n)
	     maxn=n; d�j� positionn�*/
	  *str2 = '"';
	  *(str1-1)='F';
	  *str4 = SXNUL;
	  tokref_id = sxword_save (&easy_tokrefs, str3+1);
	  *str4 = '<';
	  savedchar=*str5;
	  *str5=SXNUL;
	  commref_id = sxword_save (&easy_commrefs, comment+passed_length+1);
	  *str5=savedchar;
	  n2easy_commref [n] = commref_id;
	  n2easy_tokref [n] = tokref_id;
	  if (passed_length==0)
	    i2n [Tij2i(Tpq)] = n;
	  j2n [Tij2j(Tpq)] = n;
	  passed_length=str5-comment;
	} else {
	  sxtrap(ME, "set_terminal_leaves");
	}
      }
    } else {
      /* JAMAIS TESTE, SUREMENT FAUX */
      if (!SXBA_bit_is_set(spf.outputG.Tpq_rcvr_set,Tpq)) {
	//	  i2n [Tij2i(Tpq)] = Tij2i(Tpq);
	//	  j2n [Tij2j(Tpq)] = Tij2i(Tpq);
      }
    }
  }
}



static int
snt_start_min (lb,ubmin,id)
     int lb,ubmin,id;
{
  int i,retval;
  retval=(ubmin>maxn);
  for (i=0;i<12;i++)
    if (retval) return 1;
    else retval=(ubmin<=maxn && snt_exists(lb,ubmin++,id));
  return retval;
}

static int
snt_start_min_any (lb,ubmin)
     int lb,ubmin;
{
  int i,retval;
  retval=(ubmin>maxn);
  for (i=0;i<12;i++)
    if (retval) return 1;
    else retval=(ubmin<=maxn && nm2Cnm[lb][ubmin++]>0);
  return retval;
}

static int
snt_end_max (lbmax,ub,id)
     int lbmax,ub,id;
{
  int i,retval;
  retval=(lbmax<minn);
  for (i=0;i<12;i++)
    if (retval) return 1;
    else retval=(lbmax>=minn && snt_exists(lbmax--,ub,id));
  return retval;
}

static int
snt_end_max_any (lbmax,ub)
     int lbmax,ub;
{
  int i,retval;
  retval=(lbmax<minn);
  for (i=0;i<12;i++)
    if (retval) return 1;
    else retval=(lbmax>=minn && nm2Cnm[lbmax--][ub]>0);
  return retval;
}

static int
snt_start (lb,id)
     int lb,id;
{
  return (snt_start_min(lb,lb,id));
}

static int
snt_start_any (lb)
     int lb;
{
  return (snt_start_min_any(lb,lb));
}

static int
snt_end (ub,id)
     int ub,id;
{
  return (snt_end_max(ub,ub,id));
}

static int
snt_end_any (ub)
     int ub;
{
  return (snt_end_max_any(ub,ub));
}

static int
strcmp_ci (str1,str2)
  char *str1,*str2;
{
  char *str1copy;
  int i;

  if (strlen(str1) != strlen(str2))
    return 0;
  str1copy=sxcalloc(strlen(str1),sizeof(char));
  for (i=0;i<strlen(str1);i++)
    str1copy[i]=sxtolower(str1[i]);
  return (strncmp(str1copy,str2,strlen(str1))==0); /* sxtolower suppos� inutile sur str2 */
}

static void
delete (lb,ub,id,filter)
  int lb,ub,id,filter;
{
  if (lb<=ub) {
#if EBUG
    printf("Removing snt %s<%i..%i> (filter %i)\n",sntid2string[id],lb,ub,filter);
#endif /* EBUG */
    if (nmt2Cnmt[lb] && nmt2Cnmt[lb][ub] && nmt2Cnmt[lb][ub][id] && !snt_is_blessed(lb,ub,id)) {
      snts[nmt2Cnmt[lb][ub][id]].pruning_filter = filter;
      nm2Cnm[lb][ub] &= ((~(0ul)) - (1 << id)) ;
    }
  }
}

static void
bless (lb,ub,id,filter)
  int lb,ub,id,filter;
{
  if (lb<=ub) {
#if EBUG
    printf("Blessing snt %s<%i..%i> (filter %i)\n",sntid2string[id],lb,ub,filter);
#endif /* EBUG */
    if (nmt2Cnmt[lb] && nmt2Cnmt[lb][ub] && nmt2Cnmt[lb][ub][id]) {
      snts[nmt2Cnmt[lb][ub][id]].blessing_filter = filter;
      nm2BlessedCnm[lb][ub] |= (1 << id) ;
    }
  }
}


#define sntfilter_c_filter
#include ntf_h
#undef sntfilter_c_filter


static void
output_snts ()
{
  struct snt c;
  int i,sntnum;
  
#if EBUG
#else /* EBUG */
  printf("<E id=\"E%i\">\n",sentence_id);
#endif /* EBUG */
  
  sntnum=1;
  for (i=1;i<=maxsnt;i++) {
    c=snts[i];
    if (snt_exists(c.lb,c.ub,c.id) && c.id != MAX_SNT_ID)
#if EBUG
      printf("Kept snt %i: ",i);
    else if (snt_exists(c.lb,c.ub,c.id) && c.id == MAX_SNT_ID)
      printf("Kept external snt %i: ",i);
    else
      printf("Rejected snt %i: ",i);
    print_snt(c);
#else /* EBUG */
    printf("<Groupe type=\"%s\" id=\"E%iG%i\">\n%s</Groupe>\n",sntid2string[c.id],sentence_id,sntnum++,varstr_tostr(c.comm));
#endif /* EBUG */
    
  }
#if EBUG
#else /* EBUG */
  printf("</E>\n",sentence_id);
#endif /* EBUG */
  
}

static int
prune_snts (Pij)
     int Pij;
{
  struct snt *c;
  int Aij,lb,ub,sntid;
  char *ntstring;
  
  Aij = spf.outputG.lhs [Pij].lhs;
  ntstring=spf.inputG.ntstring[Aij2A (Aij)];
  if (is_an_snt(Aij) && string2sntid(ntstring)>=0 && (sntid=Aij2sntid[Aij])) {/* si c'est un snt */
    if (sntid = Aij2sntid[Aij]) { /* prudence */
      c=&snts[sntid];
      
      /* on regarde s'il a �t� filtr�, et dans ce cas on demande son �lagage */
      if (!(snt_exists(c->lb,c->ub,c->id)) && (filter_id == -1 || filter_id == c->pruning_filter)) {
#if EBUG
	printf("Pruning non-terminal %s[%i..%i] (Aij=%i ; Pij=%i ; snt %i, filter %i)\n",ntstring,Aij2i (Aij),Aij2j (Aij),Aij,Pij,sntid,c->pruning_filter);
#endif /* EBUG */
	return -1;
      }
#if EBUG
      printf("Keeping non-terminal %s[%i..%i] (Aij=%i ; Pij=%i)\n",ntstring,Aij2i (Aij),Aij2j (Aij),Aij,Pij);
#endif /* EBUG */
    }
#if EBUG
    else {
      printf("!!! Ignoring non-terminal %s[%i..%i] (Aij=%i ; Pij=%i)\n",ntstring,Aij2i (Aij),Aij2j (Aij),Aij,Pij);
    }
#endif /* EBUG */
  }
  return 1;
}

/* calcule r�cursivement le nombre de sous-arbres de m�me racine */
static double
spf_ptn_count (Aij)
     int Aij;
{
  int              output_prod, output_item, Xpq, hook;
  double           val, Aij_val;

  if ((Aij_val = spf_count [Aij]) == -1.0L) {
    /* 1ere visite de Aij => non calcule */
    Aij_val = spf_count [Aij] = 0;
    
    hook = spf.outputG.lhs [spf.outputG.maxxprod+Aij].prolon;
    
    while ((output_prod = spf.outputG.rhs [hook++].lispro) != 0) {
      /* On parcourt les productions dont la lhs est Aij */
      if (output_prod > 0) { /* et qui ne sont pas filtr�es */
	output_item = spf.outputG.lhs [output_prod].prolon;
	
	val = 1.0L;
	
	while ((Xpq = spf.outputG.rhs [output_item++].lispro) != 0) {
	  /* On parcourt la RHS de la  prod instanciee output_prod */
	  if (Xpq > 0 && Xpq <= spf.outputG.maxxnt) { /* y compris ceux en erreur */
	    /* nt */
	    val *= spf_ptn_count (Xpq);
	  }
	}
	Aij_val += val;
      }
    }
    spf_count [Aij] = Aij_val; /* Aij est entierement calcule' (et non cyclique) */
  }

  return Aij_val;
}

/* calcule le nombre d'arbres de la for�t */
static double
spf_ptn ()
{
  double val;
  int i;

  if (spf.outputG.start_symbol == 0) {
    val = 0.0L;
  }
  else {
    spf_count = (double*) sxalloc (spf.outputG.maxxnt+1, sizeof (double));
    for (i=0;i<=spf.outputG.maxxnt;i++)
      spf_count[i]=-1.0L;

    val = spf_ptn_count (spf.outputG.start_symbol);

    sxfree (spf_count), spf_count = NULL;
  }
  return val;

}

/* imprime le nombre d'arbres de la for�t */
static void
print_ptn ()
{
  double val=spf_ptn();

  /*  fprintf (sxstdout, "|N|=%i, |T|=%i, |P|=%i, S=%i, |outpuG|=%i, TreeCount=", 
      spf.outputG.maxxnt, -spf.outputG.maxt, spf.outputG.maxxprod, spf.outputG.start_symbol, spf.outputG.maxitem);*/
  fprintf(stdout, "TreeCount\t");
  
  if (val < 1.0E9)
    fprintf (stdout, "%.f\n", val);
  else
    fprintf (stdout, "%e\n", val);
}

static double
locally_select_tree_with_longest_snts (Aij)
     int Aij;
{
  int hook, init_hook, cur_Pij;
  double localmin;
  bool found_best_hook, blessed_Aij;


  found_best_hook = false;
  init_hook = hook = spf.outputG.lhs [spf.outputG.maxxprod+Aij].prolon;     
  localmin=1000000000000.0;

  /* pour pouvoir garder ou bien un seul arbre ou bien tous les arbres de poids optimal, on fait 2 passes
     sur la liste des productions de partie gauche Aij:
     1. passe 1 pour d�terminer le poids optimal
     2. passe 2 pour �laguer
  */

  /* passe 1 */
  while ((cur_Pij = spf.outputG.rhs [hook].lispro) != 0) {
    if (cur_Pij > 0) { /* il ne s'agit pas d'une branche elaguee */
      if (localmin > Pij2weight[cur_Pij]) { /* si cette production est meilleure que ce qu'on a d�j� vu... */
	localmin = Pij2weight[cur_Pij]; /*... on stocke la nouvelle valeur optimale... */
      }
    }
    hook++;
  }

  /* passe 2 */
  hook = init_hook;
  found_best_hook = false;
  blessed_Aij = SXBA_bit_is_set(Xpq_is_blessed, Aij);
  while ((cur_Pij = spf.outputG.rhs [hook].lispro) != 0) {
    if (cur_Pij > 0) { /* il ne s'agit pas d'une branche elaguee */
#if EBUG
      spf_print_iprod(stdout,cur_Pij,"","");
      fprintf(stdout," %e - %i\n",Pij2weight[cur_Pij], blessed_Aij);
#endif
      //      if (localmin < Pij2weight[cur_Pij]*0.9999 /* si cette production n'est pas optimale... */
      if ((Pij2weight[cur_Pij] > localmin+0.00001 /* si cette production n'est pas optimale... */
	   || (found_best_hook && (SNTFILTER_SEM_PASS_ARG == 2)))
	  /* ou alors on a d�j� trouv� une prod optimale, celle-ci l'est aussi mais on ne veut qu'un seul arbre*/
	  && !blessed_Aij   /* et la prod n'est pas "b�nite" � cause d'un snt de rhs b�ni */
	  ) { 
#if EBUG
	fprintf(stdout," - eliminated\n");
	//	fprintf(stdout," - eliminated (%i / %e > %e == %i)\n", found_best_hook, Pij2weight[cur_Pij], localmin, (localmin < Pij2weight[cur_Pij]));
#endif
	spf.outputG.rhs [hook].lispro=-cur_Pij; /*...on l'�lague ;... */
	spf.outputG.is_proper = false;
      } else {
#if EBUG
	fprintf(stdout," - kept\n");
	//	fprintf(stdout," - kept (%i / %e > %e == %i)\n", found_best_hook, Pij2weight[cur_Pij], localmin, (localmin < Pij2weight[cur_Pij]));
#endif
	found_best_hook = true; /* on se souvient qu'on a trouv� au moins une prod optimale */
      }
    }
    hook++;
  }

  return localmin;
}

static int
select_tree_with_longest_snts (Pij)
     int Pij;
{
  int Xpq, cur_Aij, prev_Aij, hook, best_hook, sntid, id, Tij, lb, ub;
  char *ntstring, tempChar, *tempString, *word, *tstring, *comment, *cfirst;
  double localweight, tweight;
  struct snt *c;
  struct lhs *plhs;
  struct rhs *prhs;
  
  cur_Aij=spf.walk.cur_Aij;
  prev_Aij=spf.walk.prev_Aij;

  plhs = spf.outputG.lhs+Pij; /* on accede a l'entree de la prod dans lhs */
  prhs = spf.outputG.rhs+plhs->prolon; /* on accede a la partie droite de la prod dans rhs */
  
  if (cur_Aij != prev_Aij && prev_Aij != 0) {/* si on est sur la premi�re prod d'un nouvel Aij, 
					       ajouter au poids de la rhs de sa Pij optimale le poids dudit Aij ...*/

    Xpq2weight[prev_Aij] = locally_select_tree_with_longest_snts(prev_Aij);

    /* la contrib de prev_Aij: */
    ntstring = spf.inputG.ntstring[Aij2A (prev_Aij)];
    if (is_an_snt(prev_Aij) && string2sntid(ntstring)>=0 && (sntid=Aij2sntid[prev_Aij])) {/* si c'est un snt */
      if (Aij2i (prev_Aij) < Aij2j (prev_Aij)) {
	/* on r�cup�re le snt associ� */
	lb = i2n [Aij2i (prev_Aij)]; /* �a risque de merder */
	ub = j2n [Aij2j (prev_Aij)];
	id = snts[sntid].id;
	if (ub==-1)
	  ub=maxn;
	cfirst=SXWORD_get (easy_tokrefs, n2easy_tokref [lb]);
	
	if (lb == ub) {
	  tempChar=(cfirst)[0];
	  if (tempChar == sxtolower(tempChar)) {
	    localweight=sntid2weight_1Min [id];
	  } else
	    localweight=sntid2weight_1Maj [id];
	  
	} else
	  localweight=sntid2weight_n [id];

	Xpq2weight[prev_Aij]+=localweight;

	if (snt_is_blessed(lb,ub,id))
	  SXBA_1_bit (Xpq_is_blessed, prev_Aij);
      }
    }
  }
  if (cur_Aij == spf.outputG.start_symbol) /* tout pareil, mais si on est sur l'axiome on travaille aussi sur cur_Aij */
    Xpq2weight[cur_Aij]=locally_select_tree_with_longest_snts(cur_Aij);
  
  while ((Xpq = (prhs++)->lispro) != 0) { /* on parcourt la partie droite */
    if (Xpq>0) { /* nt */
      if (SXBA_bit_is_set (Xpq_is_blessed, Xpq)) /* blessed snt */
	SXBA_1_bit (Xpq_is_blessed, cur_Aij);
      Pij2weight[Pij]+=Xpq2weight[Xpq];
    } else if (Xpq<0) { /* terminal */
      Tij=-Xpq;
      word = sxstrget (parser_Tpq2tok (Tij) -> string_table_entry);
      tstring = spf.inputG.tstring[-Tij2T(Tij)];
      if (strncmp(word,"_uw",3)==0 || strncmp(word,"_Uw",3)==0) { /* si c'est un _[uU]w */
	tweight=0.1; /* par d�faut on n'aime pas (cas v)*/
	if (strncmp(tstring,"n",1)==0) /* si c'est un nc ou un np */
	  tweight=-0.4; /* on aime bien */
	if (strncmp(word,"_Uw",3)==0) {
	  comment=SXWORD_get (easy_commrefs, n2easy_commref [i2n[Tij2i(Tij)]]);
	  if (strncmp(comment,"Le",2) || strncmp(comment,"Le",2) || strncmp(comment,"Les",3))
	    tweight+=4;
	}
	Pij2weight[Pij] += tweight /* ce qu'il faut pour mettre GN en place 1 */ + 0.4 /* le co�t d'utiliser _[Uu]w */;

      } else {
	if (strncmp(tstring,"np",2)==0) /* si c'est un np */
	  if (strchr(word,(int) '\''))
	    Pij2weight[Pij]+=0.05; /* on aime moins bien */
	  else
	    Pij2weight[Pij]-=0.05; /* on aime bien */
      }
      tempString = word;

    }
  }
  return 1;
}

static void
rec_print_terminals (Pij)
     int Pij;
{
  int Xpq, Tij, lb, ub, hook, cur_Pij;
  char *word, *tstring, *comment;
  struct lhs *plhs;
  struct rhs *prhs;
  
  plhs = spf.outputG.lhs+Pij; /* on accede a l'entree de la prod dans lhs */
  prhs = spf.outputG.rhs+plhs->prolon; /* on accede a la partie droite de la prod dans rhs */
  
  while ((Xpq = (prhs++)->lispro) != 0) { /* on parcourt la partie droite */
    if (Xpq<0) { /* terminal */
      Tij=-Xpq;
      lb=i2n [Tij2i (Tij)]; /* �a risque de merder */
      ub=j2n [Tij2j (Tij)];
      word = sxstrget (parser_Tpq2tok (Tij) -> string_table_entry);
      tstring = spf.inputG.tstring[-Tij2T(Tij)];
      printf("<W id=\"E%iT%i\" cat=\"%s\">\n",sentence_id,Tij2i (Tij),tstring);
      while (lb<=ub) {
	printf("\t%s\n",SXWORD_get (easy_commrefs, n2easy_commref [lb++]));
      }
      printf("</W>\n");
    } else if (Xpq>0) {
      hook = spf.outputG.lhs [spf.outputG.maxxprod+Xpq].prolon;     
      while ((cur_Pij = spf.outputG.rhs [hook++].lispro) != 0) {
	if (cur_Pij > 0) { /* il ne s'agit pas d'une branche elaguee */
	  rec_print_terminals(cur_Pij);
	}
      }
    }
  }
}

static void
print_terminals ()
{
  rec_print_terminals(spf.outputG.rhs [spf.outputG.lhs [spf.outputG.maxxprod+spf.outputG.start_symbol].prolon].lispro);
}


static int
sntfilter_sem_pass ()
{
  double tree_nb,new_tree_nb;
  int maxeid;
  char msg[60], qualifier[10];
  bool is_not_empty, temp_bool;

#if EBUG
  fprintf(stdout,"\n");
  fprintf(stdout,"*************************************************************************************\n");
  fprintf(stdout,"************* S T A R T I N G   S N T   M O D U L E   ( %s ) *************\n",SNTS_NAME);
  fprintf(stdout,"*************************************************************************************\n");
#endif /* EBUG */

  qualifier[0] = SXNUL;
    
  maxxt = -spf.outputG.maxt;
  Xpq_top = spf.outputG.maxxnt + maxxt;
  maxsnt=0;
  minn=INT_MAX;
  maxn=-1;
  
  allocate_all ();
  tree_nb = spf_ptn();
  //  distribute_comment (epsilon_code);

#if EBUG
  fprintf(stdout,"\n************* L O A D I N G *************\n\n");
#endif /* EBUG */
  set_terminal_leaves (); /* me sert � remplir les structures o� chercher les commentaires */
  //spf_topological_walk (spf.outputG.start_symbol, easy_td_walk, NULL);
  spf_topological_walk (spf.outputG.start_symbol, load_snts, NULL);

#if EBUG
  fprintf(stdout,"\n************* F I L T E R I N G *************\n\n");
#endif /* EBUG */
  filter_snts();

#if EBUG
  print_ptn();
#endif /* EBUG */

  spf_push_status();
  filter_id = -1;

#if EBUG
  fprintf(stdout,"\n************* P R U N I N G *************\n\n");
#endif /* EBUG */
  temp_bool = spf_topological_walk (spf.outputG.start_symbol, prune_snts, NULL);
  is_not_empty = (spf_ptn() > 0);
#if EBUG
  if (is_not_empty != temp_bool)
    fprintf(stderr, "Warning: result returned by spf_topological_walk and spf_ptn differ (%i != %i)\n", temp_bool, is_not_empty);
#endif /* EBUG */
  if (!is_not_empty) {
#if EBUG
    fprintf(stdout, "Warning: global snts filtering output an empty forest. Using robust snt filtering\n");
#endif /* EBUG */
    sprintf(qualifier,"robust ");
    spf_pop_status();
    for (filter_id = 0; filter_id <= MAX_FILTER; filter_id++) {
      spf_push_status();
      temp_bool = spf_topological_walk (spf.outputG.start_symbol, prune_snts, NULL);
      is_not_empty = (spf_ptn() > 0);
#if EBUG
  if (is_not_empty != temp_bool)
    fprintf(stderr, "Warning: result returned by robust spf_topological_walk and spf_ptn differ (%i != %i)\n", temp_bool, is_not_empty);
#endif /* EBUG */
      if (!is_not_empty) {
#if EBUG
	fprintf(stdout, "Warning: Snt filtering heuristics number %i skipped (it empties the forest)\n",filter_id);
#endif /* EBUG */
	spf_pop_status();
      }
#if EBUG
      printf("After snt filtering heuristics number %i, ",filter_id);
      print_ptn();
#endif /* EBUG */
    }
  }

#if EBUG
  print_ptn();
#endif /* EBUG */
  //  output_snts() ;
  new_tree_nb = spf_ptn();
  if (SNTFILTER_SEM_PASS_ARG > 0 && new_tree_nb > 1) {
#if EBUG
  fprintf(stdout,"\n************* D E T E R M I N I Z I N G *************\n\n");
#endif /* EBUG */
    spf_topological_walk (spf.outputG.start_symbol, NULL, select_tree_with_longest_snts);
  }
  
  new_tree_nb = spf_ptn();

  //  print_ptn();

  maxsnt = 1;

#ifdef TAGGER
      printf("<E id=\"E%i\">\n",sentence_id);
      print_terminals();
      printf("</E>\n",sentence_id);
#endif /* TAGGER */


  if (new_tree_nb == 0) {
    if (is_print_time) {
      sprintf(msg,"\t%s filtering (FALSE)", SNTS_NAME);
      msg [1] = sxtoupper (msg [1]);
      sxtime (SXACTION, msg);
    }
    return 0;
  }

  if (is_print_time) {
    sprintf(msg,"\t%s %sfiltering (true ", SNTS_NAME, qualifier);
    msg [1] = sxtoupper (msg [1]);
    if (tree_nb < 1.0E6)
      sprintf(msg,"%s%.f -> ",msg,tree_nb);
    else
      sprintf(msg,"%s%.2le -> ",msg,tree_nb);
    
    if (new_tree_nb < 1.0E6)
      sprintf(msg,"%s%.f)",msg,new_tree_nb);
    else
      sprintf(msg,"%s%.2le)",msg,new_tree_nb);

    sxtime (SXACTION, msg);
  }

#if EBUG
  fprintf(stdout,"\n");
  fprintf(stdout,"*************************************************************************************\n");
  fprintf(stdout,"************* E N D   O F   S N T   M O D U L E   ( %s ) *************\n",SNTS_NAME);
  fprintf(stdout,"*************************************************************************************\n");
#endif /* EBUG */

  return 1;
}
 
#if 0
void
sntfilter_semact ()
{
  for_semact.sem_final = sntfilter_sem_final;
  for_semact.sem_pass = sntfilter_sem_pass;
  for_semact.process_args = sntfilter_args_decode;
  for_semact.string_args = sntfilter_args_usage;
  for_semact.ME = output_ME;
  

  for_semact.rcvr = NULL;
  
  rcvr_spec.range_walk_kind = MIXED_FORWARD_RANGE_WALK_KIND;

}
#endif /* 0 */

#define sntfilter_c_semact
#include ntf_h
#undef sntfilter_c_semact

  /* si  on veut sa propre strat�gie:
     for_semact.rcvr_range_walk = mafonction;
     o� mafonction a 5 arguments:
     1=le pt de d�tection de l'erreur
     2,3=le range pr�c�dent (qui a rat�,  sinon on serait pas appel�)
     4,5=le prochain range � essayer: ce sont des pointeurs, � moi leur donner la valeur que je veux
  */
