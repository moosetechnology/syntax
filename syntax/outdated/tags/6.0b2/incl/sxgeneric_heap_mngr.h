/* Module generique de gestion d'un tas d'element de type quelconque dont on veut
   gerer (acquerir ou rendre) les elements un a un.
   Les elements ne sont jamais deplaces, l'utilisateur peut donc conserver des
   pointeurs dessus.
*/

/* MODE D'EMPLOI :
   
   Il suffit de creer 2 fichiers type.h et type_mngr.c a partir du modele
   ci-dessous en
   - substituant la chaine "sxheap" par la chaine "type"
   - en completant la structure type_elem par les elements du type que l'on veut
     definir

   On definit de cette facon un typedef de nom type_header et une structure type_elem.
   Chaque variable v de tas de type se declare par :
           type_header v;
   Chaque element e de type se declare par
           struct type_elem e;

   Les trois principales fonctions sont
   - Demander un nouvel element, son identifiant est un entier i obtenu par :
             i = type_seek (v);
     ou v est une variable de tas de type

   - Referencer un element d'identifiant i par :
             type_access (v, i)
     On peut ecrire type_access (v, i).champ1..., &type_access (v, i), ...

   - Supprimer un element d'identifiant i par :
             type_release (v, i)


   On peut de plus creer (type_alloc (&v, taille_initiale)), liberer (type_free (&v)),
   vider (type_clear (&v)) un tas.
*/

/* Exemple gestion d'une liste d'entiers.
   On substitue "sxheap" par "int_list"
   On complete

struct int_list_elem {
};

   en

struct int_list_elem {
   int			val;
   int			current_index;
   struct int_list_elem	*next_ptr;
};

   ou val est la valeur d'un element de la liste
   next_ptr est un pointeur (resultat de &int_list_access ()) vers l'element suivant,
   current_index est l'identifiant de l'element courant resultat de int_list_seek ().

   next_ptr permet de parcourir la liste rapidement alors que current_index permet de
   detruire les elements. Retrouver les index a partir ded ptr semble couteux.

   Declaration d'un tas de listes d'entiers
   int_list	liste_d_entiers;

   Allocation :
   int_list_alloc (&liste_d_entiers, 100);

   Declaration d'une liste l (dans liste_d_entiers):
   struct int_list_elem *l = NULL;

   Ajouter un element en tete de l
   int elem = int_list_seek (liste_d_entiers);
   struct int_list_elem *elem_ptr = &int_list_access (liste_d_entiers, elem);
   elem_ptr->val = ...;
   elem_ptr->current_index = elem;
   elem_ptr->next_ptr = l;
   l = elem_ptr;
   
   Supprimer l'element en tete de l
   int current_index = l->current_index;
   l = l->next_ptr;
   int_list_release (liste_d_entiers, current_index);

   
   Parcourt d'une liste l :
   while (l != NULL) {
      acces a l->val
      l = l->next;
   }

   C'est fini
   int_list_free (&liste_d_entiers);

   etc, etc...

*/


/* INCLUDE	sxheap.h	*/

struct sxheap_elem {
};

typedef struct {
    sxindex_header	indexes;
    int			old_line_nb;
    struct sxheap_elem	**buf;
} sxheap;

sxheap	*sxheap_ptr_;

#define sxheap_clear(h)		sxindex_clear ((h).indexes)
#define sxheap_release(h,x)	sxindex_release((h).indexes,x)
#define sxheap_size(h)		sxindex_size((h).indexes)
#define sxheap_line_nb(h)	sxindex_line_nb((h).indexes)
#define sxheap_column_nb(h)	sxindex_column_nb((h).indexes)
#define sxheap_seek(h)		(sxheap_ptr_=&(h),sxindex_seek(&((h).indexes)))
#define sxheap_access(h,x)	(h).buf [sxindex_i((h).indexes,x)][sxindex_j((h).indexes,x)]

extern VOID sxheap_oflw (/* sxindex_header* */);
extern VOID sxheap_alloc (/* sxheap_header*, int */);
extern VOID sxheap_free (/* sxheap_header* */);

/* fin INCLUDE	sxheap.h	*/

/* Module sxheap_mngr.c		*/

VOID sxheap_oflw (index_header_ptr, old_line_nb, old_size)
    sxindex_header 	*index_header_ptr;
    int			old_line_nb, old_size;
{
    int new_line_nb = sxindex_line_nb (sxheap_ptr_->indexes);
    int new_column_nb = sxindex_column_nb (*index_header_ptr);

    sxheap_ptr_->buf = (struct sxheap_elem**)
	sxrealloc (sxheap_ptr_->buf, new_line_nb, sizeof (struct sxheap_elem*));

    while (--new_line_nb >= old_line_nb) {
	sxheap_ptr_->buf [new_line_nb] = (struct sxheap_elem*)
	    sxalloc (new_column_nb, sizeof (struct sxheap_elem));
    }
}

VOID sxheap_alloc (header, size)
    sxheap_header	*header;
    int			size;
{
    int i, l;

    sxindex_alloc (&header->indexes, 1, size, sxheap_oflw);
    i = sxindex_line_nb (header->indexes);
    header->buf = (struct sxheap_elem**) sxalloc (i, sizeof (struct sxheap_elem*));

    l = sxindex_column_nb (header->indexes);

    while (--i >= 0) {
	header->buf [i] = (struct sxheap_elem*) sxalloc (l, sizeof (struct sxheap_elem));
    }
}

VOID sxheap_free (header)
    sxheap_header	*header;
{
    int i = sxindex_line_nb (header->indexes);

    while (--i >= 0) {
	sxfree (header->buf [i]);
    }

    sxfree (header->buf);
    sxindex_free (header->indexes);
}

/* Fin module sxheap_mngr.c		*/
