#!/usr/bin/perl

use strict;

my (%disambs,%disamb_comment,%sets,%set_comment,@atoms,$code);

my $section;
my $temp_comment;
my $l;
while (<>) {
  $l++;
  chomp;
  next if (/^\s*$/);
  if (/^(FIELD_SETS|USED_ATOMS|DISAMBIGUATORS|CODE)$/) {
    $section=$1;
    $temp_comment="";
  } elsif ($section eq "FIELD_SETS") {
    if (/^\t([^ ]+)\s*=\s*\{(.*)\}\s*$/) {
      $sets{$1}=$2;
      if ($temp_comment ne "") {$set_comment{$1}=$temp_comment; $temp_comment=""}
    } elsif (/^\s*(\/\*|\/\/)/ || /^\#/) {
      $temp_comment.=$_."\n";
    } else {
      die "Error in section FIELD_SETS, line $l\n";
    }
  } elsif ($section eq "USED_ATOMS") {
    if (s/^\t// || /^\#/) {
      push(@atoms,$_)
    } else {
      die "Error in section USED_ATOMS, line $l\n";
    }
  } elsif ($section eq "DISAMBIGUATORS") {
    if (/^\t([a-zA-Z0-9])\s*=\s*(.*)/) {
      $disambs{$1}=$2;
      if ($temp_comment ne "") {$disamb_comment{$1}=$temp_comment; $temp_comment=""}
    } elsif (/^\s*(\/\*|\/\/)/ || /^\#/) {
      $temp_comment.=$_."\n";
    } else {
      die "Error in section DISAMBIGUATORS, line $l\n";
    }
  } elsif ($section eq "CODE") {
    $code.=$_."\n";
  }
}


print <<STOP;
#if include_qq_valeurs_declarations

SX_GLOBAL_VAR BOOLEAN           qq_valeurs_positionnees;
STOP

my ($atom_name,$C_atom_name);
my ($set_name,$att_name);
for $set_name (sort keys %sets) {
  print "SX_GLOBAL_VAR SXBA   $set_name\_ids_set;\n";
}

for $atom_name (@atoms) {
  if ($atom_name =~ /^\#/) {print "$atom_name\n"}
  else {
    $C_atom_name = $atom_name;
    $C_atom_name=~s/^\+$/plus/;
    $C_atom_name=~s/^\-$/minus/;
    $C_atom_name=~s/à/a/g;
    $C_atom_name=~s/é/e/g;
    $C_atom_name=~s/-/_/g;
    if ($C_atom_name !~ /^[a-zA-Z_][a-zA-Z_0-9]*$/) {
      die "Atom name $C_atom_name is invalid\n"
    }
    print "SX_GLOBAL_VAR int    atom_$C_atom_name;\n";
  }
}


print <<STOP;

#endif /* include_qq_valeurs_declarations */

#if include_qq_valeurs_free

STOP

for $set_name (sort keys %sets) {
  print "if ($set_name\_ids_set) sxfree ($set_name\_ids_set), $set_name\_ids_set = NULL;\n";
}

print <<STOP;

#endif /* include_qq_valeurs_free */

#if include_qq_valeurs

static void
qq_valeurs ()
{
  int           id;
  char          *str;
  sxword_header field_names, atom_names;

  if (qq_valeurs_positionnees)
    return;

  qq_valeurs_positionnees = TRUE;

STOP

for $set_name (sort keys %sets) {
  print "  $set_name\_ids_set = sxba_calloc (MAX_FIELD_ID+1);\n";
  for $att_name(split(/\s*,\s*/,$sets{$set_name})) {
    $att_name=~s/^\+$/plus/;
    $att_name=~s/^\-$/minus/;
    $att_name=~s/à/a/g;
    $att_name=~s/é/e/g;
    $att_name=~s/-/_/g;
    if ($att_name !~ /^[a-zA-Z_][a-zA-Z_0-9]*$/) {
      die "Attribute name $att_name is invalid (used while defining set $set_name)\n"
    }
    print "  SXBA_1_bit ($set_name\_ids_set, $att_name\_id);\n";
  }
}

print <<STOP;
  sxword_alloc (&atom_names, "atom_names", MAX_STATIC_ATOM_ID+1, 1, 16,
		sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL, NULL);

  for (id = 2; id < MAX_STATIC_ATOM_ID; id++) {
    sxword_save (&atom_names, atom_id2string [id]);
  }
STOP

for $atom_name (@atoms) {
  if ($atom_name =~ /^\#/) {print "$atom_name\n"}
  else {
    $C_atom_name = $atom_name;
    $C_atom_name=~s/^\+$/plus/;
    $C_atom_name=~s/^\-$/minus/;
    $C_atom_name=~s/à/a/g;
    $C_atom_name=~s/é/e/g;
    $C_atom_name=~s/-/_/g;
    if ($C_atom_name !~ /^[a-zA-Z_][a-zA-Z_0-9]*$/) {
      die "Atom name $C_atom_name is invalid\n"
    }
    print "  if ((atom_$C_atom_name = sxword_2retrieve (&atom_names, \"$atom_name\", ".length($atom_name).")) == 0) sxtrap (ME, \"qq_valeurs (atom $atom_name not found in the grammar)\");\n";
  }
}

print <<STOP;
  sxword_free (&atom_names);
}
#endif /* include_qq_valeurs */


#if include_ranking_functions
STOP

print $code;

print <<STOP;
static BOOLEAN
call_inside_ranking (int Aij, char *ranking)
{
  int           cur_nb, old_cur_nb, ranking_kind, x;
  unsigned char cur_rank;
  BOOLEAN       success;
  int           new_size = XxY_size (heads)+1;

  old_cur_nb = cur_nb = working_heads_stack [0]; /* Nombre de structures "valides" */

  /* f_structures multiples et ranking demande' */
#if LOG
  varstr_raz (vstr);
#endif /* LOG */ 

  ranking_kind = 0;

  while (cur_nb > 1 && (cur_rank = *ranking++)) {
    if (isascii (cur_rank) && isalnum (cur_rank)) {
#if EBUG
      printf("Ranking \%c\\n",cur_rank);
#endif /* EBUG */
      switch (cur_rank) {
      case '0':
	if (cur_pass_number == MAX_PASS_NB) {
	  /* Le 27/10/05 on ne choisit au hasard que lors de la derniere passe.
	     Ca permet au ranking 7 qui utilise les aij et qui ne fonctionera donc qu'en seconde passe de travailler
	     sur des f_structures non choisies au hasard */
	  working_heads_stack [0] = 1; /* le 1er !! */
	  /* Ca a marche' */
	  success = TRUE;
	}
	else
	  success = FALSE;

	break;

#if FS_PROBAS
      case 'p':
	/* preferer les f-structures de proba max */
	/* On ne le lance pas si is_proba_ranking n'a pas ete demande' ... */
	success = is_proba_ranking ? ranking_by_probabilities () : FALSE;
	break;

      case 'n':
	/* preferer les f-structures qui sont associees aux c_structures de poids max */
	/* On ne le lance pas si is_nbest_ranking n'a pas ete demande' ... */
	success = is_nbest_ranking ? ranking_by_nbest (Aij) : FALSE;
	break;
#endif /* FS_PROBAS */

STOP

my ($disamb,$function);
for $disamb (sort keys %disambs) {
  $function = $disambs{$disamb};
  if (defined ($disamb_comment{$disamb})) {print "$disamb_comment{$disamb}"}
  print "      case '$disamb':\n";
  $function =~ s/((?:min|max)_ranking\s*\(\s*([^ ]+))\s*\)/\1\n\#if LOG\n			       , \"\2\"\n\#endif \/* LOG *\/\n			       )/g;
  $function =~ s/(ranking_by_special_atomic_field\s*\(\s*[^\s\)]+)/\1\_id/;
  $function =~ s/dummy_disambiguator(?:\(\))?/FALSE/;
  $function = "success = $function;";
  print "        $function\n";
  print "        break;\n\n";
}

print <<STOP;
      default:
	/* unused */
	success = FALSE;
#if EBUG
	sxtrap (ME, \"call_inside_ranking (unknown rank)\");
#endif /* EBUG */
	break;
      }
#if LOG
      if (success)  
	varstr_catchar (vstr, cur_rank);
#endif /* LOG */
    }

    cur_nb = working_heads_stack [0]; /* Nombre de structures "valides" */
  }
 

#if LOG
  if (cur_nb < old_cur_nb)
    varstr_complete (vstr);
#endif /* LOG */ 

  if (fs_nb_threshold > 0 && cur_nb > fs_nb_threshold)
    working_heads_stack [0] = fs_nb_threshold; /* les fs_nb_threshold 1ers !! */

  return cur_nb < old_cur_nb;
}

#endif /* include_ranking_functions */
STOP
