
/* Action par defaut appelee si aucune @function n'est specifiee sur une clause */
#define sem_void			NULL

#define _binary_semact_init		sxvoid
#define _binary_semact_final		sxvoid
#define _binary_semact_last_pass	sxbvoid

static BOOLEAN
mess (rho, son_nb, sons, is_first)
    int 	*rho, son_nb, sons[];
    BOOLEAN	is_first;
{
    /* axiome(X Y) --> &StrLen(2, X) !d(X) . @mess */
    call_put_error (1,
		 "%sinput = \"['1'|'2']' 'sentence\"\n\
'1' selects { bx | bx \in {0,1}^* et b est une repre'sentation binaire de |x|}\n\
'2' selects L_0 = \set{\varepsilon}, i>0, L_i = \set{(w)[w^c] \mid w \in L_{i-1}} \cup \set{[w](w^c) \mid w \in L_{i-1}}, L = \cup_{i \geq 0} L_i\n\
");

    return TRUE;
}  
