#include "sxunix.h"
#include "dcg_td.h"

VOID	dcg_scan_act (what, act_no)
    int		what, act_no;
{
    int cur_lahead, c0, c1, c2;

    switch (what) {
    case OPEN:
    case CLOSE:
    case INIT:
    case FINAL:
	return;

    case ACTION:
	switch (act_no) {
	case 1:
	    /* %atom = .... ; @1 ; */
	    /* On verifie si les mot-cles en sont vraiment! */
	    c0 = sxsvar.sxlv_s.counters [0]; /* [ ... ] */
	    c1 = sxsvar.sxlv_s.counters [1]; /* ( ... ) */
	    c2 = sxsvar.sxlv_s.counters [2]; /* { ... } */

	    switch (cur_lahead = sxsvar.sxlv.terminal_token.lahead) {
	    case atom_t:
		return;

	    case is_t: /* is */
		if (c1 != 0 || c2 == 0) {
		    /* Ce n'est pas le mot-cle "is" */
		    sxsvar.sxlv.terminal_token.lahead = atom_t;
		    sxsvar.sxlv.terminal_token.string_table_entry = sxstr2save ("is", 2);
		}

		return;

	    case less_t:
		if (c1 == 0 && c2 == 0) {
		    /* Ce n'est pas le mot-cle "<" */
		    sxsvar.sxlv.terminal_token.lahead = atom_t;
		    sxsvar.sxlv.terminal_token.string_table_entry = sxstr2save ("<", 1);
		}

		return;
	    case less_equal_t:
		if (c1 == 0 && c2 == 0) {
		    /* Ce n'est pas le mot-cle "=<" */
		    sxsvar.sxlv.terminal_token.lahead = atom_t;
		    sxsvar.sxlv.terminal_token.string_table_entry = sxstr2save ("=<", 2);
		}

		return;
	    case sup_t:
		if (c1 == 0 && c2 == 0) {
		    /* Ce n'est pas le mot-cle ">" */
		    sxsvar.sxlv.terminal_token.lahead = atom_t;
		    sxsvar.sxlv.terminal_token.string_table_entry = sxstr2save (">", 1);
		}

		return;
	    case sup_equal_t:
		if (c1 == 0 && c2 == 0) {
		    /* Ce n'est pas le mot-cle ">=" */
		    sxsvar.sxlv.terminal_token.lahead = atom_t;
		    sxsvar.sxlv.terminal_token.string_table_entry = sxstr2save (">=", 1);
		}

		return;
	    case plus_t:
		if (c1 == 0 && c2 == 0) {
		    /* Ce n'est pas le mot-cle "+" */
		    sxsvar.sxlv.terminal_token.lahead = atom_t;
		    sxsvar.sxlv.terminal_token.string_table_entry = sxstr2save ("+", 1);
		}

		return;
	    case minus_t:
		if (c1 == 0 && c2 == 0) {
		    /* Ce n'est pas le mot-cle "-" */
		    sxsvar.sxlv.terminal_token.lahead = atom_t;
		    sxsvar.sxlv.terminal_token.string_table_entry = sxstr2save ("-", 1);
		}

		return;
	    case multiply_t:
		if (c1 == 0 && c2 == 0) {
		    /* Ce n'est pas le mot-cle "*" */
		    sxsvar.sxlv.terminal_token.lahead = atom_t;
		    sxsvar.sxlv.terminal_token.string_table_entry = sxstr2save ("*", 1);
		}

		return;
	    case divide_t:
		if (c1 == 0 && c2 == 0) {
		    /* Ce n'est pas le mot-cle "/" */
		    sxsvar.sxlv.terminal_token.lahead = atom_t;
		    sxsvar.sxlv.terminal_token.string_table_entry = sxstr2save ("/", 1);
		}

		return;

	    case modulo_t: /* mod */
		if (c1 == 0 && c2 == 0) {
		    /* Ce n'est pas le mot-cle "mod" */
		    sxsvar.sxlv.terminal_token.lahead = atom_t;
		    sxsvar.sxlv.terminal_token.string_table_entry = sxstr2save ("mod", 3);
		}

		return;

	    case lhs_rhs_sep_t:	/* "-->" */
		if (c0 != 0 || c1 != 0 || c2 != 0) {
		    /* Ce n'est pas le mot-cle "-->" */
		    sxsvar.sxlv.terminal_token.lahead = atom_t;
		    sxsvar.sxlv.terminal_token.string_table_entry = sxstr2save ("-->", 3);
		}

		return;


	    default:
		break;
	    }

	default:
	    break;
	}

    default:
	fputs ("The function \"dcg_scan_act\" is out of date with respect to its specification.\n", sxstderr);
	abort ();
    }
}
