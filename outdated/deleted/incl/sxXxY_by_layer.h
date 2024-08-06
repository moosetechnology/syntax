
/* this (commented out) code fragment was originally in sxnd.h */

#if 0

/* XxY  by  L A Y E R */

struct XxY_layer {
    XxY_header		local;
    struct XxY_elem	*global;
    SXINT			base, size;
};

struct XxY_layer	*_LAYER_HEADER_PTR;


static void XxY_layer_oflw (old_size, new_size)
    SXINT old_size, new_size;
{
    _LAYER_HEADER_PTR->size += new_size - old_size;
    _LAYER_HEADER_PTR->global =
	(struct XxY_elem *) sxrealloc (_LAYER_HEADER_PTR->global,
				       _LAYER_HEADER_PTR->size + 1,
				       sizeof (struct XxY_elem));
}

static void XxY_layer_alloc (header, size)
    struct XxY_layer	*header;
    SXINT			size;
{
    XxY_alloc (&(header->local), "local", size, 1, 0, 0, XxY_layer_oflw, NULL);
    header->size = XxY_size (header->local);
    header->global = (struct XxY_elem *) sxalloc (header->size + 1, sizeof (struct XxY_elem));
    header->base = 0;
}

static SXINT XxY_layer_put (header, X, Y)
    struct XxY_layer	*header;
    SXINT			X, Y;
{
    struct XxY_elem	*eptr;
    SXINT			index;
    bool		is_new;

    _LAYER_HEADER_PTR = header;
    is_new = !XxY_set (&(header->local), X, Y, &index);
    index += header->base;
    
    if (is_new) {
	eptr = header->global + index;
	eptr->X = X;
	eptr->Y = Y;
    }

    return index;
}

static void XxY_new_layer (header)
    struct XxY_layer	*header;
{
    SXINT top;

    if ((top = XxY_top (header->local)) > 0) {
	header->base += top;
	XxY_clear (&(header->local));
	_LAYER_HEADER_PTR = header;
	XxY_layer_oflw (header->size, header->size + XxY_size (header->local));
    }
}

static void XxY_layer_free (header)
    struct XxY_layer	*header;
{
    XxY_free (&(header->local));
    sxfree (header->global);
}

#define	XxY_layer_get(header,x,i,j)	i=header.global[x].X,j=header.global[x].Y 

/* END XxY  by  L A Y E R */

#endif
