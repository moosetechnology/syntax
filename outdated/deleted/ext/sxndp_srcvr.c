bool		sxndpsrecovery (SXINT what_to_do)
{
    SXINT		kind = sxplocals.mode.kind;
    bool	ret_val;

    sxplocals.mode.kind = SXWITH_RECOVERY;
    ret_val = sxndprecovery (what_to_do);
    sxplocals.mode.kind = kind;

    return ret_val;
}

/* Pour la version ou toutes les corrections valides sont retournees. */


