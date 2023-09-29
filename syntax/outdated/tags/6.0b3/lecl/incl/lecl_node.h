/* LECL_NODE.H */

struct lecl_node {
    NODE_HEADER_S VOID_NAME;
    SXINT i_item_no,
	d_item_no,
	item_code,
	node_no /* init 0 */ ;

    BOOLEAN is_empty, 
	    not_is_first_visit,
	    is_erased;
   };


/* end LECL_NODE.H */

