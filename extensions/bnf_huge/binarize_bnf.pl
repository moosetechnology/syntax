#!/usr/bin/perl

my $n = 0;
my $nb_rules = 0;


while(<>){
#    print "\n-----------------\n";
#    print;
#    print "-----------------\n";

    /([^ ]*) += +([^;]*); +(.*)/;
    $lhs = $1;
    $rhs = $2;
    $proba = $3;

    @rhs = split / /, $rhs;
    $l = $#rhs;

#    on recherche le plus long facteur droit dans la regle courante
    if($l > 1){
	$facteur_droit = "NULL";
	$s2 = $rhs[$l];
	for($j = $l-1; $j > 0; $j--){
	    $s1 = $rhs[$j];
	    $r = recherche_regle($s1, $s2);
	    if($r != -1){
		$s2 = $r0[$r];
		$facteur_droit = $s2;
		$debut_facteur_droit = $j;
	    }
	    else{
		last;
	    }
	}
	
#	on binarise la regle courante jusqu'au facteur trouve ci-dessus
	for($k=0; $k<=$l-1; $k++){
	    if(($k == $debut_facteur_droit - 1) && ($facteur_droit ne "NULL")){
		print "$lhs = $rhs[$k] $facteur_droit; $proba\n";
		if($k != 0){
		    $r0[$nb_rules] = $lhs;
		    $r1[$nb_rules] = $rhs[$k];
		    $r2[$nb_rules] = $facteur_droit;
		    $nb_rules++;
		}
		last;
	    }
	    else{
		if($k == $l - 1){
		    print "$lhs = $rhs[$l-1] $rhs[$l]; 1\n";
		    if($k != 0){
			$r0[$nb_rules] = $lhs;
			$r1[$nb_rules] = $rhs[$l-1];
			$r2[$nb_rules] = $rhs[$l];
			$nb_rules++;
		    }
		}
		else{
		    $new_sym = "<X$n>";
		    $n++;
		    print "$lhs = $rhs[$k] $new_sym; $proba\n";
		    if($k != 0){
			$r0[$nb_rules] = $lhs;
			$r1[$nb_rules] = $rhs[$k];
			$r2[$nb_rules] = $new_sym;
			$nb_rules++;
		    }
		    $proba = 1;
		    $lhs = $new_sym;
		}
	    }
	}
    }
    else{
	print;
    }
}


sub recherche_regle{
    local($s1, $s2, $i);		
    ($s1, $s2) = ($_[0], $_[1]);

    for($i=0; $i < $nb_rules; $i++){
	if(($r1[$i] eq $s1) && ($r2[$i] eq $s2)){
	    return $i;
	}
    }
    return -1;
}
