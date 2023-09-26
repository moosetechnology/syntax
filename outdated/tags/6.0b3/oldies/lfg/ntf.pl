#!/usr/bin/perl

use strict;

my (%types,@types,$maxtype,$filter,$max_filter,$prefix,%weights,$typelength,$ntfname,$sntsname);
$max_filter=-1;

my $section;
my $temp_comment;
my $l;
while (<>) {
  $l++;
  chomp;
  s/\/\/.*//;
  next if (/^\s*$/);
  if (/^(TYPES|PREFIX|WEIGHTS|FILTER|MODULE_NAME|SNTS_NAME|MAX_FILTER)$/) {
    $section=$1;
    $temp_comment="";
  } elsif ($section eq "TYPES") {
    if (/^\t([^ ]+)\s*$/) {
      if ($typelength > 0 && length($1) != $typelength) {
	die "Error in section TYPES, line $l (all types must have the same length)\n";
      } else {
	$typelength = length($1);
      }
      if (!defined($types{$1})) {
	$types{$1}=$maxtype;
	$types[$maxtype]=$1;
	$maxtype++;
      } else {
	die "Error in section TYPES, line $l (duplicate type $1)\n";	
      }
    } else {
      die "Error in section TYPES, line $l\n";
    }
  } elsif ($section eq "PREFIX") {
    if (/^\t([^ ]+)\s*$/) {
      if ($prefix eq "") {
	$prefix=$1;
      } else {
	die "Error in section PREFIX, line $l (duplicate prefix)\n";	
      }
    } else {
      die "Error in section PREFIX, line $l\n";
    }
  } elsif ($section eq "WEIGHTS") {
    if (/^\t(1Maj|1Min|n)\t(.*)/) {
      $weights{$1}=$2;
    } else {
      die "Error in section WEIGHTS, line $l\n";
    }
  } elsif ($section eq "MODULE_NAME") {
    if (/^\t([^ ]+)\s*$/) {
      if ($ntfname eq "") {
	$ntfname=$1;
      } else {
	die "Error in section MODULE_NAME, line $l (duplicate ntfname)\n";	
      }
    } else {
      die "Error in section MODULE_NAME, line $l\n";
    }
  } elsif ($section eq "SNTS_NAME") {
    if (/^\t([^ ]+)\s*$/) {
      if ($sntsname eq "") {
	$sntsname = $1;
      } else {
	die "Error in section SNTS_NAME, line $l (duplicate sntsname)\n";	
      }
    } else {
      die "Error in section SNTS_NAME, line $l\n";
    }
  } elsif ($section eq "MAX_FILTER") {
    if (/^\t([0-9]+)\s*$/) {
      $max_filter=$1;
    }
  } elsif ($section eq "FILTER") {
    $filter.=$_."\n";
  }
}

if ($prefix eq "") {die "No prefix given\n";}
if ($ntfname eq "") {die "No module name given\n";}
if ($sntsname eq "") {die "No name given for special non-terminals\n";}
if ($max_filter < 0) {die "No maximum filter number given\n";}
if (!(defined($weights{"1Maj"}) && defined($weights{"1Maj"}) && defined($weights{"1Maj"}))) {die "Weights part not complete\n";}

### partie incluse dans ntfilter_semact.h
print "#ifdef sntfilter_h\n";
print "#define SNTFILTER_SEM_PASS_ARG ".$ntfname."_sem_pass_arg\n";
print "#define SNTS_NAME \"$sntsname\"\n";
print "\n";
print "#define SNT_PREFIX \"$prefix\"\n";
print "#define SNT_PREFIX_LENGTH ".length($prefix)."\n";
print "\n";
for my $id (0..$#types) {
  print "#define    $types[$id]  $id\n";
}
print "#define    ".("Z"x$typelength)."  $maxtype\n";
print "\n";
print "#define    MAX_SNT_ID  $maxtype\n";
print "#endif /* sntfilter_h */\n";

### partie incluse vers le début de ntfilter_semact.c
print "#ifdef sntfilter_c_init\n";
print "static char    *sntid2string [".($maxtype+1)."] = {\"".(join("\",\"",@types))."\",\"".("Z"x$typelength)."\"};\n";
print "\n";
for (keys %weights) {
  print "static double sntid2weight_".$_." [".($maxtype+1)."] = ".$weights{$_}."\n";
}
print "\n";
print "static int string2sntid(nts)\n";
print "     char* nts;\n";
print "{\n";
for my $id (0..$#types) {
  print "  if (strncmp (nts+".length($prefix).", \"$types[$id]\", $typelength) == 0)\n";
  print "    return $id;\n";
}
print "  if (strncmp (nts+".length($prefix).", \"".("Z"x$typelength)."\", $typelength) == 0)\n";
print "    return $maxtype;\n";
print "  if (strncmp (nts+".length($prefix).", \"".("0"x$typelength)."\", $typelength) == 0)\n";
print "    return $maxtype;\n";
print "  return -1;\n";
print "}\n";
print "\n";
print "char* ntstring2".$sntsname."name(ntstring)\n";
print "     char* ntstring;\n";
print "{\n";
print "  if (ntstring == NULL || strlen(ntstring) < ".($typelength+length($prefix)).")\n";
print "    return NULL;\n";
for my $id (0..$#types) {
  print "  if (strncmp (ntstring, \"$prefix$types[$id]\", ".($typelength+length($prefix)).") == 0)\n";
  print "    return \"$types[$id]\";\n";
}
print "  return NULL;\n";
print "}\n";
print "\n";
print "int ".$sntsname."name2id(name)\n";
print "     char* name;\n";
print "{\n";
print "  if (name == NULL || strlen(name) < $typelength)\n";
print "    return -1;\n";
for my $id (0..$#types) {
  print "  if (strncmp (name, \"$types[$id]\", ".($typelength).") == 0)\n";
  print "    return $id;\n";
}
print "  return -1;\n";
print "}\n";

print "#endif /* sntfilter_c_init */\n";

### partie incluse vers le début de ntfilter_semact.c
print "#ifdef sntfilter_c_filter\n";
print "#define MAX_FILTER $max_filter\n\n";
print <<END;
static void
filter_snts ()
{
  struct snt c;
  int i,lb,ub,id;
  char *cfirst,*clast,*ntstring;
  
  /* on supprime les groupes vides */
  for (i=1;i<=maxsnt;i++) {
    c=snts[i];
    lb=c.lb; ub=c.ub; id=c.id; cfirst=c.cfirst; clast=c.clast;
    if (lb > ub || lb == 0 || ub == 0)
      delete(lb,ub,id,-1);
  }
  for (i=1;i<=maxsnt;i++) {
    c=snts[i];
    lb=c.lb; ub=c.ub; id=c.id; cfirst=c.cfirst; clast=c.clast;
#if EBUG
    printf("Studying SNT %i: ",i);
    print_snt(c);
#endif /* EBUG */
END
print $filter;
print <<END;
  }
#if EBUG
  for (i=1;i<=maxsnt;i++) {
    c=snts[i];
    if(snt_exists(c.lb,c.ub,c.id))
      printf("Possible SNT:  %s[%i..%i]<%i..%i>\\n",sntid2string[c.id],Aij2i(c.Aij),Aij2j(c.Aij),c.lb,c.ub);
  }
#endif /* EBUG */
}
END
print "#endif /* sntfilter_c_filter */\n";

### partie incluse à la fin de ntfilter_semact.c
print "#ifdef sntfilter_c_semact\n";
print "int\n";
print "print_$sntsname (Pij)\n";
print "     int Pij;\n";
print "{\n";
print "  return print_snts (Pij);\n";
print "}\n";
print "\n";
print "void\n";
print $ntfname."_semact ()\n";
print <<END;
{
  for_semact.sem_final = sntfilter_sem_final;
  for_semact.sem_pass = sntfilter_sem_pass;
  for_semact.process_args = sntfilter_args_decode;
  for_semact.string_args = sntfilter_args_usage;
  for_semact.ME = output_ME;
  

  for_semact.rcvr = NULL;
  
  rcvr_spec.range_walk_kind = MIXED_FORWARD_RANGE_WALK_KIND;

}
END
print "#endif /* sntfilter_c_semact */\n";
