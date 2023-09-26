#!/usr/bin/perl

my $rule = "";
my $comment = "";
my $in_rule = 0;
my $l = 0;
my $axiom = "";

while (<>) {
  chomp;
  $l++;
  if (/^\*/) {
    print $_."\n";
    next;
  }
  next if /^\s*$/;
  s/\s*~[^~]*~//g;
  unless (/^(<.*?>)\s*=\s*(.*?) ; ([0-9Ee\.\+\-]+)$/) {
    die "Syntax error line $l (\"$_\")";
  }
  $lhs = $1;
  $rhs = $2;
  $proba = $3;
  if ($axiom eq "") {$axiom = $lhs}
  next if ($lhs eq $rhs);
  $lhs2rhs{$lhs}{$rhs} = $proba;
  $rhs_copy = $rhs;
  while ($rhs_copy =~ s/^(<.*?[^\\]>|\".*?[^\\]\")\s*//) {
    $rhs2symbols{$rhs}{$1} = 1;
    $symbols2occ{$1} ++;
  }
}

for $s (keys %symbols2occ) {
  if ($s =~ /^\"/) {$productive{$s} = 1}
}
$productive{""} = 1;
$did_something = 1;
while ($did_something) {
  $did_something = 0;
  $did_something_2 = 1;
  while ($did_something_2) {
    $did_something_2 = 0;
    for $rhs (keys %rhs2symbols) {
      next if (defined($productive{$rhs}) && $productive{$rhs} >= 0);
      $rhs_is_productive = 1;
      for $s (keys %{$rhs2symbols{$rhs}}) {
	if (defined($productive{$s}) && $productive{$s} == 0) {
	  $rhs_is_productive = 0;
	  last;
	} elsif (!defined($productive{$s})) {
	  if (!defined($lhs2rhs{$s})) {
	    $productive{$s} = 0;
	    $rhs_is_productive = 0;
	    last;
	  }
	  $rhs_is_productive = -1;
	}
      }
      if ($rhs_is_productive >= 0) {
	$productive{$rhs} = $rhs_is_productive;
	$did_something_2 = 1;
      }
    }
    for $lhs (keys %lhs2rhs) {
      next if (defined($productive{$lhs}) && $productive{$lhs} >= 0);
      $productive{$lhs} = 0;
      for $rhs (keys %{$lhs2rhs{$lhs}}) {
	if (defined($productive{$rhs})) {
	  if ($productive{$rhs} == 1) {
	    $productive{$lhs} = 1;
	    last;
	  } elsif ($productive{$rhs} == 1) {
	    $productive{$lhs} = -1;
	  }
	} elsif (!defined($productive{$rhs})) {
	  $productive{$lhs} = -1;
	}
      }
      if ($productive{$lhs} >= 0) {
	$did_something_2 = 1;
      }
    }
    if ($productive{$axiom} == 0) {die "The axiom $axiom is not productive"}
  }
  %reachable = (); %visited = ();
  $reachable{$axiom} = 1;
  $did_something_2 = 1;
  while ($did_something_2) {
    $did_something_2 = 0;
    for $lhs (keys %reachable) {
      next if (defined($visited{$lhs}) || !defined($lhs2rhs{$lhs}));
      for $rhs (keys %{$lhs2rhs{$lhs}}) {
	die "Error: we should know whether $rhs is productive or not" if ($productive{$rhs} == -1);
	next if ($productive{$rhs} == 0);
	for $s (keys %{$rhs2symbols{$rhs}}) {
	  if (!defined($reachable{$s})) {
	    $reachable{$s} = 1;
	    $did_something_2 = 1;
	  }
	}
      }
      $visited{$lhs} = 1;
    }
  }
}

for $rhs (keys %{$lhs2rhs{$axiom}}) {
  next unless ($productive{$rhs} == 1);
  print "$axiom = $rhs ; ".$lhs2rhs{$axiom}{$rhs}."\n";
}
for $lhs (keys %lhs2rhs) {
  next if ($lhs eq $axiom || !defined($reachable{$lhs}));
  for $rhs (keys %{$lhs2rhs{$lhs}}) {
    next unless ($productive{$rhs} == 1);
    print "$lhs = $rhs ; ".$lhs2rhs{$lhs}{$rhs}."\n";
  }
}
