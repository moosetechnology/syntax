/* ********************************************************************
   *  This program has been generated from cx.lecl		      *
   *  on Thu Nov 23 13:24:42 2006				      *
   *  by the SYNTAX (*) lexical constructor LECL                      *
   ********************************************************************
   *  (*) SYNTAX is a trademark of INRIA.                             *
   ******************************************************************** */

#include "sxunix.h"
#include <ctype.h>
char WHAT_CXSCNR[] = "@(#)SYNTAX - $Id: cx_scnr.c 1174 2008-03-10 12:00:12Z rlacroix $" WHAT_DEBUG;

/*     S T A T I C S     */


static unsigned char	S_char_to_simple_class [] = {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3,
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 5, 3, 6, 3, 3, 7, 3, 3, 8, 9, 3, 9, 10, 11, 12, 13,
     13, 13, 13, 13, 13, 13, 14, 14, 3, 3, 3, 3, 3, 3, 3, 15, 15, 15, 15, 16, 15, 17, 17, 17, 17, 17, 18, 17, 17, 17, 17,
     17, 17, 17, 17, 17, 17, 17, 19, 17, 17, 3, 20, 3, 3, 21, 3, 22, 22, 22, 22, 23, 22, 24, 24, 24, 24, 24, 25, 24, 24,
     24, 24, 24, 24, 24, 24, 24, 24, 24, 26, 24, 24, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,};



/*     D E F I N E     */

#ifdef LDBG
#define DBG
#endif

/* a new character is catenated to token_string */
#define put_a_char(c)							\
(    (sxsvar.sxlv.ts_lgth+2 == sxsvar.sxlv_s.ts_lgth_use)			\
	? sxsvar.sxlv_s.token_string = (char *) sxrealloc (sxsvar.sxlv_s.	\
	    token_string, sxsvar.sxlv_s.ts_lgth_use *= 2, sizeof (char))	\
	: NULL,								\
     sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth ++] = c		\
)

/* token_string becomes a c string */

/* computes the class of the character 'c' */
#define char_to_class(c)    ((S_char_to_simple_class + 128) [c])
/*     M A J O R    E N T R Y     */

SXVOID	cx_scan_it (void)
{
    unsigned char	current_class;

    /* let's go for a new lexical token */
#ifdef LDBG
    printf ("\n******************* SCAN_IT ********************\n");

#endif

    sxsvar.sxlv.ts_lgth = 0;
    current_class = char_to_class (sxsrcmngr.current_char);
    sxsvar.sxlv.terminal_token.string_table_entry = EMPTY_STE;
    goto STATE_1;


/**************************************************************************/

STATE_1_scan:
    current_class = char_to_class (sxnextchar ());
STATE_1:
    sxsvar.sxlv.terminal_token.source_index = sxsrcmngr.source_coord;

#ifdef LDBG

    printf ("state_no = %d, current_char = '%s', current_class = %d\n", 1, SXCHAR_TO_STRING (sxsrcmngr.current_char),
	 current_class);


#endif

    switch (current_class) {
    case 2:
	/* Eof */
	sxsvar.sxlv.terminal_token.lahead = 3 /* End Of File */ ;
	goto done;

    case 3:
    case 4:
    case 6:
    case 8:
    case 9:
    case 20:
    case 21:
	/* "\000".."!" + "#$%&()*+,-:;<=>?@[\\]^_`" + "{".."ÿ" */
	goto STATE_2_scan;

    case 5:
	/* "\"" */
	goto STATE_3_scan;

    case 7:
	/* "'" */

/**************************************************************************/

	current_class = char_to_class (sxnextchar ());
STATE_4:

#ifdef LDBG

	printf ("state_no = %d, current_char = '%s', current_class = %d\n", 4, SXCHAR_TO_STRING (sxsrcmngr.current_char),
	     current_class);


#endif

	switch (current_class) {
	case 3:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
	case 21:
	case 22:
	case 23:
	case 24:
	case 25:
	case 26:
	    /* "\000".."\t" + "\013".."[" + "]".."ÿ" */
	    goto STATE_12_scan;

	case 20:
	    /* "\\" */

/**************************************************************************/

	    current_class = char_to_class (sxnextchar ());
STATE_13:

#ifdef LDBG

	    printf ("state_no = %d, current_char = '%s', current_class = %d\n", 13, SXCHAR_TO_STRING (sxsrcmngr.
		 current_char), current_class);


#endif

	    switch (current_class) {
	    case 3:
	    case 5:
	    case 6:
	    case 7:
	    case 8:
	    case 9:
	    case 10:
	    case 11:
	    case 14:
	    case 15:
	    case 16:
	    case 17:
	    case 18:
	    case 19:
	    case 20:
	    case 21:
	    case 22:
	    case 23:
	    case 24:
	    case 25:
	    case 26:
		/* "\000".."\t" + "\013".."/" + "8".."ÿ" */
		goto STATE_12_scan;

	    case 12:
	    case 13:
		/* "0".."7" */

/**************************************************************************/

		current_class = char_to_class (sxnextchar ());
STATE_19:

#ifdef LDBG

		printf ("state_no = %d, current_char = '%s', current_class = %d\n", 19, SXCHAR_TO_STRING (sxsrcmngr.
		     current_char), current_class);


#endif

		switch (current_class) {
		case 7:
		    /* "'" */
		    goto STATE_1_scan;

		case 12:
		case 13:
		    /* "0".."7" */

/**************************************************************************/

		    current_class = char_to_class (sxnextchar ());
STATE_24:

#ifdef LDBG

		    printf ("state_no = %d, current_char = '%s', current_class = %d\n", 24, SXCHAR_TO_STRING (sxsrcmngr.
			 current_char), current_class);


#endif

		    switch (current_class) {
		    case 7:
			/* "'" */
			goto STATE_1_scan;

		    case 12:
		    case 13:
			/* "0".."7" */
			goto STATE_12_scan;

		    default:
			/* Eof + "\000".."&" + "(".."/" + "8".."ÿ" */
			if (sxsvar.sxlv.mode.errors_nb++, (*sxsvar.SXS_tables.recovery) (ACTION, 24, &current_class)) {
			    goto STATE_24;
			}

			return;
		    }


/**************************************************************************/


		default:
		    /* Eof + "\000".."&" + "(".."/" + "8".."ÿ" */
		    if (sxsvar.sxlv.mode.errors_nb++, (*sxsvar.SXS_tables.recovery) (ACTION, 19, &current_class)) {
			goto STATE_19;
		    }

		    return;
		}


/**************************************************************************/


	    default:
		/* Eof + "\n" */
		if (sxsvar.sxlv.mode.errors_nb++, (*sxsvar.SXS_tables.recovery) (ACTION, 13, &current_class)) {
		    goto STATE_13;
		}

		return;
	    }


/**************************************************************************/


	default:
	    /* Eof + "\n" */
	    if (sxsvar.sxlv.mode.errors_nb++, (*sxsvar.SXS_tables.recovery) (ACTION, 4, &current_class)) {
		goto STATE_4;
	    }

	    return;
	}


/**************************************************************************/


    case 10:
	/* "." */

/**************************************************************************/

	current_class = char_to_class (sxnextchar ());
STATE_5:

#ifdef LDBG

	printf ("state_no = %d, current_char = '%s', current_class = %d\n", 5, SXCHAR_TO_STRING (sxsrcmngr.current_char),
	     current_class);


#endif

	switch (current_class) {
	case 2:
	case 5:
	case 7:
	case 11:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
	case 22:
	case 23:
	case 24:
	case 25:
	case 26:
	    /* Eof + "\"'/" + "A".."Z" + "a".."z" */
	    goto STATE_1;

	case 3:
	case 4:
	case 6:
	case 8:
	case 9:
	case 10:
	case 20:
	case 21:
	    /* "\000".."!" + "#$%&()*+,-.:;<=>?@[\\]^_`" + "{".."ÿ" */
	    goto STATE_2_scan;

	case 12:
	case 13:
	case 14:
	    /* "0".."9" */
	    goto STATE_14_scan;

	default:
	    /*  */
	    if (sxsvar.sxlv.mode.errors_nb++, (*sxsvar.SXS_tables.recovery) (ACTION, 5, &current_class)) {
		goto STATE_5;
	    }

	    return;
	}


/**************************************************************************/


    case 11:
	/* "/" */

/**************************************************************************/

	current_class = char_to_class (sxnextchar ());
STATE_6:

#ifdef LDBG

	printf ("state_no = %d, current_char = '%s', current_class = %d\n", 6, SXCHAR_TO_STRING (sxsrcmngr.current_char),
	     current_class);


#endif

	switch (current_class) {
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
	case 24:
	case 25:
	case 26:
	    /* Eof + "\000"..")" + "+".."ÿ" */
	    goto STATE_1;

	case 8:
	    /* "*" */
	    goto STATE_15_scan;

	default:
	    /*  */
	    if (sxsvar.sxlv.mode.errors_nb++, (*sxsvar.SXS_tables.recovery) (ACTION, 6, &current_class)) {
		goto STATE_6;
	    }

	    return;
	}


/**************************************************************************/


    case 12:
	/* "0" */

/**************************************************************************/

	current_class = char_to_class (sxnextchar ());
STATE_7:

#ifdef LDBG

	printf ("state_no = %d, current_char = '%s', current_class = %d\n", 7, SXCHAR_TO_STRING (sxsrcmngr.current_char),
	     current_class);


#endif

	switch (current_class) {
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 11:
	case 15:
	case 17:
	case 20:
	case 21:
	case 22:
	case 24:
	    /* Eof + "\000".."-" + "/" + ":".."D" + "FGHIJK" + "M".."W" + "Y".."d" + "fghijk" + "m".."w" + "y".."ÿ" */
	    goto STATE_1;

	case 10:
	    /* "." */
	    goto STATE_14_scan;

	case 12:
	case 13:
	case 14:
	    /* "0".."9" */
	    goto STATE_8_scan;

	case 16:
	case 23:
	    /* "Ee" */
	    goto STATE_16_scan;

	case 18:
	case 25:
	    /* "Ll" */
	    goto STATE_1_scan;

	case 19:
	case 26:
	    /* "Xx" */

/**************************************************************************/

	    current_class = char_to_class (sxnextchar ());
STATE_17:

#ifdef LDBG

	    printf ("state_no = %d, current_char = '%s', current_class = %d\n", 17, SXCHAR_TO_STRING (sxsrcmngr.
		 current_char), current_class);


#endif

	    switch (current_class) {
	    case 12:
	    case 13:
	    case 14:
	    case 15:
	    case 16:
	    case 22:
	    case 23:
		/* "0".."9" + "ABCDEFabcdef" */
		goto STATE_23_scan;

	    default:
		/* Eof + "\000".."/" + ":;<=>?@" + "G".."`" + "g".."ÿ" */
		if (sxsvar.sxlv.mode.errors_nb++, (*sxsvar.SXS_tables.recovery) (ACTION, 17, &current_class)) {
		    goto STATE_17;
		}

		return;
	    }


/**************************************************************************/


	default:
	    /*  */
	    if (sxsvar.sxlv.mode.errors_nb++, (*sxsvar.SXS_tables.recovery) (ACTION, 7, &current_class)) {
		goto STATE_7;
	    }

	    return;
	}


/**************************************************************************/


    case 13:
    case 14:
	/* "1".."9" */
	goto STATE_8_scan;

    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
	/* "A".."Z" */
	goto STATE_9_keep_and_scan;

    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
	/* "a".."z" */
	goto STATE_10_keep_and_scan;

    default:
	/*  */
	if (sxsvar.sxlv.mode.errors_nb++, (*sxsvar.SXS_tables.recovery) (ACTION, 1, &current_class)) {
	    goto STATE_1;
	}

	return;
    }


/**************************************************************************/


/**************************************************************************/

STATE_2_scan:
    current_class = char_to_class (sxnextchar ());
STATE_2:

#ifdef LDBG

    printf ("state_no = %d, current_char = '%s', current_class = %d\n", 2, SXCHAR_TO_STRING (sxsrcmngr.current_char),
	 current_class);


#endif

    switch (current_class) {
    case 2:
    case 5:
    case 7:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
	/* Eof + "\"'" + "/".."9" + "A".."Z" + "a".."z" */
	goto STATE_1;

    case 3:
    case 4:
    case 6:
    case 8:
    case 9:
    case 10:
    case 20:
    case 21:
	/* "\000".."!" + "#$%&()*+,-.:;<=>?@[\\]^_`" + "{".."ÿ" */
	goto STATE_2_scan;

    default:
	/*  */
	if (sxsvar.sxlv.mode.errors_nb++, (*sxsvar.SXS_tables.recovery) (ACTION, 2, &current_class)) {
	    goto STATE_2;
	}

	return;
    }


/**************************************************************************/


/**************************************************************************/

STATE_3_scan:
    current_class = char_to_class (sxnextchar ());
STATE_3:

#ifdef LDBG

    printf ("state_no = %d, current_char = '%s', current_class = %d\n", 3, SXCHAR_TO_STRING (sxsrcmngr.current_char),
	 current_class);


#endif

    switch (current_class) {
    case 3:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
	/* "\000".."\t" + "\013".."!" + "#".."[" + "]".."ÿ" */
	goto STATE_3_scan;

    case 5:
	/* "\"" */
	goto STATE_1_scan;

    case 20:
	/* "\\" */

/**************************************************************************/

	current_class = char_to_class (sxnextchar ());
STATE_11:

#ifdef LDBG

	printf ("state_no = %d, current_char = '%s', current_class = %d\n", 11, SXCHAR_TO_STRING (sxsrcmngr.current_char),
	     current_class);


#endif

	switch (current_class) {
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
	case 24:
	case 25:
	case 26:
	    /* "\000".."ÿ" */
	    goto STATE_3_scan;

	default:
	    /* Eof */
	    if (sxsvar.sxlv.mode.errors_nb++, (*sxsvar.SXS_tables.recovery) (ACTION, 11, &current_class)) {
		goto STATE_11;
	    }

	    return;
	}


/**************************************************************************/


    default:
	/* Eof + "\n" */
	if (sxsvar.sxlv.mode.errors_nb++, (*sxsvar.SXS_tables.recovery) (ACTION, 3, &current_class)) {
	    goto STATE_3;
	}

	return;
    }


/**************************************************************************/


/**************************************************************************/

STATE_8_scan:
    current_class = char_to_class (sxnextchar ());
STATE_8:

#ifdef LDBG

    printf ("state_no = %d, current_char = '%s', current_class = %d\n", 8, SXCHAR_TO_STRING (sxsrcmngr.current_char),
	 current_class);


#endif

    switch (current_class) {
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 11:
    case 15:
    case 17:
    case 19:
    case 20:
    case 21:
    case 22:
    case 24:
    case 26:
	/* Eof + "\000".."-" + "/" + ":".."D" + "FGHIJK" + "M".."d" + "fghijk" + "m".."ÿ" */
	goto STATE_1;

    case 10:
	/* "." */
	goto STATE_14_scan;

    case 12:
    case 13:
    case 14:
	/* "0".."9" */
	goto STATE_8_scan;

    case 16:
    case 23:
	/* "Ee" */
	goto STATE_16_scan;

    case 18:
    case 25:
	/* "Ll" */
	goto STATE_1_scan;

    default:
	/*  */
	if (sxsvar.sxlv.mode.errors_nb++, (*sxsvar.SXS_tables.recovery) (ACTION, 8, &current_class)) {
	    goto STATE_8;
	}

	return;
    }


/**************************************************************************/


/**************************************************************************/

STATE_9_keep_and_scan:
    put_a_char (sxsrcmngr.current_char);
/* STATE_9_scan: */
    current_class = char_to_class (sxnextchar ());
STATE_9:

#ifdef LDBG

    printf ("state_no = %d, current_char = '%s', current_class = %d\n", 9, SXCHAR_TO_STRING (sxsrcmngr.current_char),
	 current_class);


#endif

    switch (current_class) {
    case 2:
    case 3:
    case 4:
    case 5:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 20:
	/* Eof + "\000".."#" + "%".."/" + ":;<=>?@[\\]^`" + "{".."ÿ" */
	sxsvar.sxlv.terminal_token.lahead = 1 /* %ID */ ;
	goto reduce;

    case 6:
	/* "$" */
	goto STATE_18_keep_and_scan;

    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
	/* "0".."9" + "A".."Z" + "_" + "a".."z" */
	goto STATE_9_keep_and_scan;

    default:
	/*  */
	if (sxsvar.sxlv.mode.errors_nb++, (*sxsvar.SXS_tables.recovery) (ACTION, 9, &current_class)) {
	    goto STATE_9;
	}

	return;
    }


/**************************************************************************/


/**************************************************************************/

STATE_10_keep_and_scan:
    put_a_char (sxsrcmngr.current_char);
/* STATE_10_scan: */
    current_class = char_to_class (sxnextchar ());
STATE_10:

#ifdef LDBG

    printf ("state_no = %d, current_char = '%s', current_class = %d\n", 10, SXCHAR_TO_STRING (sxsrcmngr.current_char),
	 current_class);


#endif

    switch (current_class) {
    case 2:
    case 3:
    case 4:
    case 5:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 20:
	/* Eof + "\000".."#" + "%".."/" + ":;<=>?@[\\]^`" + "{".."ÿ" */
	sxsvar.sxlv.terminal_token.lahead = 1 /* HASH + %ID */ ;
	goto hash_reduce;

    case 6:
	/* "$" */
	goto STATE_18_keep_and_scan;

    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 21:
	/* "0".."9" + "A".."Z" + "_" */
	goto STATE_9_keep_and_scan;

    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
	/* "a".."z" */
	goto STATE_10_keep_and_scan;

    default:
	/*  */
	if (sxsvar.sxlv.mode.errors_nb++, (*sxsvar.SXS_tables.recovery) (ACTION, 10, &current_class)) {
	    goto STATE_10;
	}

	return;
    }


/**************************************************************************/


/**************************************************************************/

STATE_12_scan:
    current_class = char_to_class (sxnextchar ());
STATE_12:

#ifdef LDBG

    printf ("state_no = %d, current_char = '%s', current_class = %d\n", 12, SXCHAR_TO_STRING (sxsrcmngr.current_char),
	 current_class);


#endif

    switch (current_class) {
    case 7:
	/* "'" */
	goto STATE_1_scan;

    default:
	/* Eof + "\000".."&" + "(".."ÿ" */
	if (sxsvar.sxlv.mode.errors_nb++, (*sxsvar.SXS_tables.recovery) (ACTION, 12, &current_class)) {
	    goto STATE_12;
	}

	return;
    }


/**************************************************************************/


/**************************************************************************/

STATE_14_scan:
    current_class = char_to_class (sxnextchar ());
STATE_14:

#ifdef LDBG

    printf ("state_no = %d, current_char = '%s', current_class = %d\n", 14, SXCHAR_TO_STRING (sxsrcmngr.current_char),
	 current_class);


#endif

    switch (current_class) {
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 15:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 24:
    case 25:
    case 26:
	/* Eof + "\000".."/" + ":".."D" + "F".."d" + "f".."ÿ" */
	goto STATE_1;

    case 12:
    case 13:
    case 14:
	/* "0".."9" */
	goto STATE_14_scan;

    case 16:
    case 23:
	/* "Ee" */
	goto STATE_16_scan;

    default:
	/*  */
	if (sxsvar.sxlv.mode.errors_nb++, (*sxsvar.SXS_tables.recovery) (ACTION, 14, &current_class)) {
	    goto STATE_14;
	}

	return;
    }


/**************************************************************************/


/**************************************************************************/

STATE_15_scan:
    current_class = char_to_class (sxnextchar ());
STATE_15:

#ifdef LDBG

    printf ("state_no = %d, current_char = '%s', current_class = %d\n", 15, SXCHAR_TO_STRING (sxsrcmngr.current_char),
	 current_class);


#endif

    switch (current_class) {
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
	/* "\000"..")" + "+".."ÿ" */
	goto STATE_15_scan;

    case 8:
	/* "*" */
	goto STATE_20_scan;

    default:
	/* Eof */
	if (sxsvar.sxlv.mode.errors_nb++, (*sxsvar.SXS_tables.recovery) (ACTION, 15, &current_class)) {
	    goto STATE_15;
	}

	return;
    }


/**************************************************************************/


/**************************************************************************/

STATE_16_scan:
    current_class = char_to_class (sxnextchar ());
STATE_16:

#ifdef LDBG

    printf ("state_no = %d, current_char = '%s', current_class = %d\n", 16, SXCHAR_TO_STRING (sxsrcmngr.current_char),
	 current_class);


#endif

    switch (current_class) {
    case 9:
	/* "+-" */

/**************************************************************************/

	current_class = char_to_class (sxnextchar ());
STATE_21:

#ifdef LDBG

	printf ("state_no = %d, current_char = '%s', current_class = %d\n", 21, SXCHAR_TO_STRING (sxsrcmngr.current_char),
	     current_class);


#endif

	switch (current_class) {
	case 12:
	case 13:
	case 14:
	    /* "0".."9" */
	    goto STATE_22_scan;

	default:
	    /* Eof + "\000".."/" + ":".."ÿ" */
	    if (sxsvar.sxlv.mode.errors_nb++, (*sxsvar.SXS_tables.recovery) (ACTION, 21, &current_class)) {
		goto STATE_21;
	    }

	    return;
	}


/**************************************************************************/


    case 12:
    case 13:
    case 14:
	/* "0".."9" */
	goto STATE_22_scan;

    default:
	/* Eof + "\000".."*" + ",./" + ":".."ÿ" */
	if (sxsvar.sxlv.mode.errors_nb++, (*sxsvar.SXS_tables.recovery) (ACTION, 16, &current_class)) {
	    goto STATE_16;
	}

	return;
    }


/**************************************************************************/


/**************************************************************************/

STATE_18_keep_and_scan:
    put_a_char (sxsrcmngr.current_char);
/* STATE_18_scan: */
    current_class = char_to_class (sxnextchar ());
STATE_18:

#ifdef LDBG

    printf ("state_no = %d, current_char = '%s', current_class = %d\n", 18, SXCHAR_TO_STRING (sxsrcmngr.current_char),
	 current_class);


#endif

    switch (current_class) {
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 20:
	/* Eof + "\000".."/" + ":;<=>?@[\\]^`" + "{".."ÿ" */
	sxsvar.sxlv.terminal_token.lahead = 1 /* %ID */ ;
	goto reduce;

    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
	/* "0".."9" + "A".."Z" + "_" + "a".."z" */
	goto STATE_18_keep_and_scan;

    default:
	/*  */
	if (sxsvar.sxlv.mode.errors_nb++, (*sxsvar.SXS_tables.recovery) (ACTION, 18, &current_class)) {
	    goto STATE_18;
	}

	return;
    }


/**************************************************************************/


/**************************************************************************/

STATE_20_scan:
    current_class = char_to_class (sxnextchar ());
STATE_20:

#ifdef LDBG

    printf ("state_no = %d, current_char = '%s', current_class = %d\n", 20, SXCHAR_TO_STRING (sxsrcmngr.current_char),
	 current_class);


#endif

    switch (current_class) {
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 9:
    case 10:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
	/* "\000"..")" + "+,-." + "0".."ÿ" */
	goto STATE_15_scan;

    case 8:
	/* "*" */
	goto STATE_20_scan;

    case 11:
	/* "/" */
	goto STATE_1_scan;

    default:
	/* Eof */
	if (sxsvar.sxlv.mode.errors_nb++, (*sxsvar.SXS_tables.recovery) (ACTION, 20, &current_class)) {
	    goto STATE_20;
	}

	return;
    }


/**************************************************************************/


/**************************************************************************/

STATE_22_scan:
    current_class = char_to_class (sxnextchar ());
STATE_22:

#ifdef LDBG

    printf ("state_no = %d, current_char = '%s', current_class = %d\n", 22, SXCHAR_TO_STRING (sxsrcmngr.current_char),
	 current_class);


#endif

    switch (current_class) {
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
	/* Eof + "\000".."/" + ":".."ÿ" */
	goto STATE_1;

    case 12:
    case 13:
    case 14:
	/* "0".."9" */
	goto STATE_22_scan;

    default:
	/*  */
	if (sxsvar.sxlv.mode.errors_nb++, (*sxsvar.SXS_tables.recovery) (ACTION, 22, &current_class)) {
	    goto STATE_22;
	}

	return;
    }


/**************************************************************************/


/**************************************************************************/

STATE_23_scan:
    current_class = char_to_class (sxnextchar ());
STATE_23:

#ifdef LDBG

    printf ("state_no = %d, current_char = '%s', current_class = %d\n", 23, SXCHAR_TO_STRING (sxsrcmngr.current_char),
	 current_class);


#endif

    switch (current_class) {
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 17:
    case 19:
    case 20:
    case 21:
    case 24:
    case 26:
	/* Eof + "\000".."/" + ":;<=>?@GHIJK" + "M".."`" + "ghijk" + "m".."ÿ" */
	goto STATE_1;

    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 22:
    case 23:
	/* "0".."9" + "ABCDEFabcdef" */
	goto STATE_23_scan;

    case 18:
    case 25:
	/* "Ll" */
	goto STATE_1_scan;

    default:
	/*  */
	if (sxsvar.sxlv.mode.errors_nb++, (*sxsvar.SXS_tables.recovery) (ACTION, 23, &current_class)) {
	    goto STATE_23;
	}

	return;
    }


/**************************************************************************/

/*     H A S H   A N D   R E D U C E     */

hash_reduce:
    {
	SXINT	look_ahead;

	if ((look_ahead = (*sxsvar.SXS_tables.check_keyword) (sxsvar.sxlv_s.token_string, sxsvar.sxlv.ts_lgth)) != 0) {
	    sxsvar.sxlv.terminal_token.lahead = look_ahead;

#ifdef DBG

	    printf ("\t\t\t\ttoken = \"%s\"\n", sxttext (sxsvar.sxtables, sxsvar.sxlv.terminal_token.lahead));

#endif

	    goto done;
	}
	else {
	}
    }


/*   R E D U C E   */

reduce:
    if (sxsvar.sxlv.ts_lgth != 0) {
	sxsvar.sxlv.terminal_token.string_table_entry = sxstr2save (sxsvar.sxlv_s.token_string, sxsvar.sxlv.ts_lgth);

#ifdef DBG

	printf ("\t\t\t\tstring = \"%s\"\n", sxstrget (sxsvar.sxlv.terminal_token.string_table_entry));

#endif

    }

done:
    sxput_token (sxsvar.sxlv.terminal_token);

#ifdef DBG

    printf ("\t\t\t\ttoken = \"%s\"\n", sxttext (sxsvar.sxtables, sxsvar.sxlv.terminal_token.lahead));

#endif
/* end cx_scan_it */

}




SXVOID	cx_scanner (SXINT what_to_do, struct sxtables *arg)
{
    switch (what_to_do) {
    case OPEN:
	/* new language, new tables: prepare new local variables */
	sxsvar.sxtables = arg;
	sxsvar.SXS_tables = arg->SXS_tables;
	sxsvar.sxlv_s.token_string = (char*) sxalloc (sxsvar.sxlv_s.ts_lgth_use = 120, sizeof (char));
	sxsvar.sxlv_s.counters = NULL;
	sxsvar.sxlv.mode.errors_nb = 0;
	sxsvar.sxlv.mode.is_silent = FALSE;
	sxsvar.sxlv.mode.with_system_act = TRUE;
	sxsvar.sxlv.mode.with_user_act = TRUE;
	sxsvar.sxlv.mode.with_system_prdct = TRUE;
	sxsvar.sxlv.mode.with_user_prdct = TRUE;
	break;

    case INIT:
	/* new source text */
	sxsvar.sxlv.terminal_token.comment = NULL;
	/*  the first char of the source text is read */
	sxnextchar ();
	break;

    case ACTION:
	cx_scan_it ();
	break;

    case CLOSE:
	/* end of language, local variables are freed */
	sxfree (sxsvar.sxlv_s.token_string);
	sxsvar.sxlv_s.token_string = NULL;
	(*sxsvar.SXS_tables.recovery) (CLOSE);
	break;

    default:
	break;
    }
    /* end cx_scanner */
}
