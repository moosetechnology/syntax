#!/usr/bin/perl

my $counts = 0;
my $input_file = "";
my $zero = 0;
while (1) {
    $_=shift;
    if (/^$/) {last;}
    elsif (/^[^-]/) {$input_file=$_;}
    elsif (/^-c$/ || /^--?counts?$/i) {$counts=1;}
    elsif (/^-p$/ || /^--?probas?$/i) {$counts=0;} # default
    elsif (/^-z$/ || /^--?zeros?$/i) {$zero = shift;} # default
    else {
      die <<END;
Usage:  bnf2proba [options] bnf_file
        bnf2proba [options]                input bnf is stdin
options=              -c, -counts          bnf contains occurrence counts
                      -p, -probas          bnf contains probabilities (default)
                      -h, -help            print this
END
    }
}

if ($input_file ne "") {
  open (FILE, "<$input_file") || die "Could not open $input_file: $!";
} else {
  *FILE = *STDIN;
}
while (<FILE>) {
  chomp;
  next if (/^\*/ || /^\s*$/);
  if (/^</) {
    /^(.*;) *([^; ]*)/;
    $pid++;
    $prod=$1;
    $proba=lc($2);
    if ($proba == 0) {$proba = $zero}
    $pid2prod[$pid] = $prod;
    $pid2prob[$pid] = $proba;
    if ($proba == 0) {die "Null probability for the following rule: $_"}
    $prod =~ /^<(.*?)>\s*=/;
    $nt_hash{$1}++;
    if ($counts) {
      $nt2prob{$1} += $proba;
      $pid2nt[$pid] = $1;
    }
  }
}
$nt_nb = scalar keys %nt_hash;

if ($pid == 0) {
  print STDERR "This text is not a grammar. Check your input file.\n";
  exit (1);
}

if ($counts) {
  for $pid (1..$#pid2prob) {
    $pid2prob[$pid] /= $nt2prob{$pid2nt[$pid]};
  }
}
for $pid (1..$#pid2prob) {
  $pid2logprob[$pid] = log ($pid2prob[$pid]) / log (10);
}

print <<END;
/************************************************************************************************************
        This file contains, for each production, its logprob
        it has been generated on Mon Jun 23 10:59:13 2008
        by the SYNTAX(*) processor bnf2proba
     **************************************************************************************************
        (*) SYNTAX is a trademark of INRIA.
     **************************************************************************************************/

#include "XxY.h"
#include "XH.h"

#define PROD_NB $pid
#define NT_NB $nt_nb

SXINT logprobs_prod_nb = PROD_NB;
SXINT logprobs_nt_nb = NT_NB;

/* Dummy data structures that are of no use in the pure PCFG case */
double lex_id2logprobs [1] /* empty */;
int  max_signature_depth = 0;
static struct XxY_elem ff_idXt2lex_id_hd_display [1] /* empty */;
static SXINT ff_idXt2lex_id_hd_hash_display [1] /* empty */;
static SXINT ff_idXt2lex_id_hd_hash_lnk [1] /* empty */;
XxY_header prodXval2eprod_hd;
XxY_header full_ctxt_hd;
XxY_header full_ctxtXeprod_hd;
double full_ctxtXeprod2logprobs [1] /* empty */;
XxY_header eprod_ctxtXeprod_hd;
double eprod_ctxtXeprod2logprobs [1] /* empty */;
XxY_header prod_ctxtXeprod_hd;
double prod_ctxtXeprod2logprobs [1] /* empty */;
XxY_header A_ctxtXeprod_hd;
double A_ctxtXeprod2logprobs [1] /* empty */;
double eprod2logprobs [1] /* empty */;
XxY_header full_ctxtXlex_id_hd;
double full_ctxtXlex_id2logprobs [1] /* empty */;
XxY_header eprod_ctxtXlex_id_hd;
double eprod_ctxtXlex_id2logprobs [1] /* empty */;
XxY_header prod_ctxtXlex_id_hd;
double prod_ctxtXlex_id2logprobs [1] /* empty */;
XxY_header A_ctxtXlex_id_hd;
double A_ctxtXlex_id2logprobs [1] /* empty */;
XxY_header ff_idXt2lex_id_hd;

/* PCFG (log)probas */
double prod2logprobs [PROD_NB+1] = {0,
END

for $pid (1..$#pid2logprob) {
  print "/* $pid: $pid2prod[$pid] log10 ($pid2prob[$pid]) = */ $pid2logprob[$pid],\n";
}
print "};\n";


