

static int
check_keyword (init_string, init_length)
    unsigned char *init_string;
    int           init_length;
{
    unsigned char	*string = init_string, *keyword;
    int			length = init_length;
    unsigned int	kw_code, t_code;

#if KW_NB==1
    kw_code = 1;
#else
    unsigned char	*pbs, class;
    unsigned int	is_suffix, base, tooth;

    base = INIT_BASE;
    is_suffix = INIT_SUFFIX;
    pbs = init_string + init_length - 1;

    for (;;) {
	if (length == 0) {
	    tooth = comb_vector [base];

	    if ((tooth & CLASS_MASK) != 0 || (tooth & STOP_MASK) == 0)
		return 0;

	    kw_code = tooth >> BASE_SHIFT;
	    break;
	}

	if ((class = char2class [is_suffix ? *pbs-- : *string++]) == 0)
	    return 0;

	length--;
	tooth = comb_vector [base+class];

	if ((tooth & CLASS_MASK) != class)
	    return 0;

	if (tooth & STOP_MASK) {
	    /* Reconnaissance partielle */
	    kw_code = tooth >> BASE_SHIFT;
	    break;
	}

	is_suffix = tooth & SUFFIX_MASK;
	base = tooth >> BASE_SHIFT;
    }
#endif

    if (kw_code2lgth [kw_code] != init_length)
	return 0;

    t_code = kw_code2t_code [kw_code];
    keyword = (unsigned char*)sxtables.SXS_tables.S_adrp [t_code] + (string-init_string);

    while (length-- > 0) {
	if (*string++ != *keyword++)
	    return 0;
    }

#ifdef SYNO
{
    int	syno;

    if ((syno = (int)t_code-(EOF_CODE+1)) >= 0)
	t_code = syno2t_code [syno];
}
#endif

    return t_code;
}
