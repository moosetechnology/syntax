
/* Action par defaut appelee si aucune @function n'est specifiee sur une clause */
#define sem_void			NULL

#define _SCR_semact_init		sxvoid
#define _SCR_semact_final		sxvoid
#define _SCR_semact_last_pass	sxbvoid

static SXBOOLEAN
mess (rho, son_nb, sons, is_first)
    SXINT 	*rho, son_nb, sons[];
    SXBOOLEAN	is_first;
{
    /* axiome(X Y) --> &StrLen(2, X) !d(X) . @mess */
    call_put_error (1,
		 "%sinput = \"['a'|'b']' 'sentence\"\n\
'a' selects simple scrambling\n\
'b' selects extended scrambling\n\
");

    return SXTRUE;
}  
