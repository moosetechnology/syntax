
/* Action par defaut appelee si aucune @function n'est specifiee sur une clause */
#define sem_void			NULL

#define _cfg_semact_init		sxvoid
#define _cfg_semact_final		sxvoid
#define _cfg_semact_last_pass		sxbvoid

static bool
mess (rho, son_nb, sons, is_first)
    SXINT 	*rho, son_nb, sons[];
    bool	is_first;
{
    /* axiome(X Y) --> &StrLen(2, X) !d(X) . @mess */
    call_put_error (1,
		 "%sTous ces languages sont definis par des 1-RCG\n\
input = \"['1'|'2'|'3'|'4'|'5'|'6'|'7'|'8'|'9'|'10'|'11'|'12'|'13'|'14'|'15'|'16'|'17'|'18'|'19'|'20'|'21']' 'sentence\"\n\
'1' selects {a^n b^n c^n | n > 0}\n\
'2' selects {a^n b^m c^n d^m| n, m > 0}\n\
'3' selects {w w | w \in {a,b}^*}\n\
'4' selects {a^n b^n c^n d^n e^n| n > 0}\n\
'5' selects {ab^ka b^{k-1}...abbab | k >= 1}\n\
'6' selects {ab^ka b^{k-1}...abbab | k >= 1} as 5 with a combinatorial RCG\n\
'7' selects {a1^n a2^n ... ak^n | ai \in {a, b, c}, n>1, k>1}\n\
'8' selects {a1^n1 a2^n2 ... al^nl a'1^n1 a'2^n2 ... a'l^nl | ni>0, 1<=l<=k, (ai,a'i) \in {(a,b),(c,d),(e,f)}}\n\
'9' selects {w{cw}^+ | w \in {a, b}^*}\n\
'10' as '6' with a non-combinatorial conversion\n\
'11' as '1' but always linear\n\
'12' as '2' but always linear\n\
'13' as '3' but linear\n\
'14' selects {w w^+ | w \in {a,b}^*} quadratic\n\
'15' as '14' but sub-quadratic\n\
'16' as '5' with a linear RCG\n\
'17' as '9' with a linear RCG\n\
'18' selects {a^n b^m c^l d^n e^m f^l| n, m, l > 0}\n\
'19' selects {a b^{k_1} a b^{k_2} ... a b^{k_p} | k_1>k_2>...>k_p>0}\n\
'20' selects {a^{p^2} | p >= 0} in n log n\n\
'21' as '20' in \sqrt{n}\n\
");

    return true;
}  
