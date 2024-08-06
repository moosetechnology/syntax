/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2023 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://sourcesup.renater.fr/projects/syntax
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.info
 *****************************************************************************/

#include "sxversion.h"
#include "sxunix.h"
#include "X.h"

char WHAT_SXMEM_MNGR[] = "@(#)SYNTAX - $Id: sxmem_mngr.c 3926 2024-04-27 12:37:23Z garavel $" WHAT_DEBUG;

#if EBUG_ALLOC
static bool debug_initiated = {false};
#if defined ( linux ) || defined ( __sun  )
#define MALLOC_DEBUG()	(debug_initiated || (setenv ("MALLOC_CHECK_", "2", 1), debug_initiated = true))
#else
#ifdef __alpha
SXUINT __sbrk_override;
SXUINT __taso_mode;
#define MALLOC_DEBUG()	(debug_initiated || (__sbrk_override = 1, __taso_mode = 0, debug_initiated = true))
#else
#ifdef __sgi
#include <ulocks.h>
#include <malloc.h>
#define MALLOC_DEBUG()	(debug_initiated || (usconfig (CONF_ARENATYPE, US_SHAREDONLY), usmallopt (M_DEBUG, 1, manche = usinit ("/tmp/sxphdmalX")), debug_initiated = true))
static usptr_t *manche;
#define malloc(size)			usmalloc (size, manche)
#define free(ptr)				usfree (ptr, manche)
#define realloc(ptr, size)			usrealloc (ptr, size, manche)
#define calloc(nelem, elsize)		uscalloc (nelem, elsize, manche)
#define recalloc(ptr, nelem, elsize)	usrecalloc (ptr, nelem, elsize, manche)
#else
#ifdef WIN32
#define  MALLOC_DEBUG()  ( sxuse(debug_initiated) )
#else
extern SXINT 	malloc_debug (size_t size);
#define MALLOC_DEBUG()	(debug_initiated || (malloc_debug (2), debug_initiated = true))
#endif
#endif
#endif
#endif
#else
#define MALLOC_DEBUG()
#endif


static void	sxgc (void)
/* Must return to its caller only if memory map has changed */
{
  fprintf (sxstderr, "\tNO MORE ALLOCATION POSSIBLE...%c\n", sxbell);

    if (sxgc_recovery && mem_signature_mode)
      sxgc_recovery ();
    else
      sxexit (SXSEVERITIES);
}

static void freeing_null (bool bypass) {
  fprintf (sxstderr, "\tInternal error%s: freeing NULL...%c\n", bypass ? " (bypassed)" : "", sxbell);
}


static void mem_signature_oflw (struct mem_signature *ms_ptr)
{
  mem_signature_mode = false;

  ms_ptr->ptr_stack = (void **) sxrealloc (ms_ptr->ptr_stack, (ms_ptr->size *= 2) + 1, sizeof (void*));

  mem_signature_mode = true;
}

static void mem_sizes_oflw (size_t **s_ptr, SXINT old_size)
{
  mem_signature_mode = false;

  *s_ptr = (size_t *) sxrealloc (*s_ptr, old_size * 2 + 1, sizeof (size_t));

  mem_signature_mode = true;
}

void	*sxcont_malloc (size_t size)

/* Allocate a block of size size or exit */

{
    void	*result;

    MALLOC_DEBUG ();

#if BUG
    fprintf (stdout, "mallocating %lu:", (SXUINT) size);
#endif

    SYNTAX_is_in_critical_zone = true;

    while ((result = malloc (size)) == NULL)
	sxgc ();

    if (mem_signature_mode) {
      if (++mem_signatures->alloc_signature.top >= mem_signatures->alloc_signature.size)
	mem_signature_oflw (&(mem_signatures->alloc_signature));
      mem_signatures->alloc_signature.ptr_stack [mem_signatures->alloc_signature.top] = result;
    }
      
    SYNTAX_is_in_critical_zone = false;

#if BUG
    fprintf (stdout, "\t0x%lX allocated\n", (SXUINT) result);
#endif

    return result;
}



void	*sxcont_alloc (size_t n, size_t size)

/* Allocate n blocks of size size and initialize them to 0s, or exit */

{
    void	*result;

    MALLOC_DEBUG ();

#if BUG
    fprintf (stdout, "allocating (%lu == %lu*%lu):", (SXUINT) n*size, (SXUINT) n, (SXUINT) size);
#endif

    SYNTAX_is_in_critical_zone = true;

    while ((result = calloc (n, size)) == NULL)
	sxgc ();

    if (mem_signature_mode) {
      if (++mem_signatures->alloc_signature.top >= mem_signatures->alloc_signature.size)
	mem_signature_oflw (&(mem_signatures->alloc_signature));
      mem_signatures->alloc_signature.ptr_stack [mem_signatures->alloc_signature.top] = result;
    }

    SYNTAX_is_in_critical_zone = false;

#if BUG
    fprintf (stdout, "\t0x%lX allocated\n", (SXUINT) result);
#endif

    return result;
}



void	*sxcont_realloc (void *table, size_t size)

/* Reallocate table with new size or exit */

{
    void	*result;

    MALLOC_DEBUG ();

#if BUG
    fprintf (stdout, "\nreallocating 0x%lX (%lu):", (SXUINT) table, (SXUINT) size);
#endif

    SYNTAX_is_in_critical_zone = true;

    while ((result = realloc (table, size)) == NULL)
	sxgc ();

    if (mem_signature_mode && table != result) {
      if (++mem_signatures->alloc_signature.top >= mem_signatures->alloc_signature.size)
	mem_signature_oflw (&(mem_signatures->alloc_signature));
      mem_signatures->alloc_signature.ptr_stack [mem_signatures->alloc_signature.top] = result;
      if (++mem_signatures->free_signature.top >= mem_signatures->free_signature.size)
	mem_signature_oflw (&(mem_signatures->free_signature));
      mem_signatures->free_signature.ptr_stack [mem_signatures->free_signature.top] = table;
    }
      
    SYNTAX_is_in_critical_zone = false;

#if BUG
    fprintf (stdout, "\t0x%lX reallocated\n", (SXUINT) result);
#endif

    return result;
}



void	*sxcont_recalloc (void *table, size_t old_size, size_t new_size)

/* Reallocate table with new_size and RAZ new elements or exit */

{
    void	*result;

    MALLOC_DEBUG ();

#if BUG
    fprintf (stdout, "\nrecallocating %lX (%lu):", (SXUINT) table, (SXUINT) new_size);
#endif

    SYNTAX_is_in_critical_zone = true;

#ifdef recalloc
    while ((result = recalloc (table, 1, new_size)) == NULL)
	sxgc ();
#else
    while ((result = realloc (table, new_size)) == NULL)
	sxgc ();

    memset (((char*) result) + old_size, 0, new_size-old_size);
#endif

    if (mem_signature_mode && table != result) {
      if (++mem_signatures->alloc_signature.top >= mem_signatures->alloc_signature.size)
	mem_signature_oflw (&(mem_signatures->alloc_signature));
      mem_signatures->alloc_signature.ptr_stack [mem_signatures->alloc_signature.top] = result;
      if (++mem_signatures->free_signature.top >= mem_signatures->free_signature.size)
	mem_signature_oflw (&(mem_signatures->free_signature));
      mem_signatures->free_signature.ptr_stack [mem_signatures->free_signature.top] = table;
    }
      
    SYNTAX_is_in_critical_zone = false;

#if BUG
    fprintf (stdout, "\t%lX recallocated\n", (SXUINT) result);
#endif

    return result;
}



void	sxcont_free (void *zone)

/* Frees the area "zone" */

{
    MALLOC_DEBUG ();

    SYNTAX_is_in_critical_zone = true;

#if BUG
    if (zone == NULL) {
        freeing_null (false);
	sxexit (SXSEVERITIES);
    }
    else {
	fprintf (stdout, "freeing 0x%lX:", (SXUINT) zone);
	free (zone);
	fprintf (stdout, "\tfreed\n");
    }
#else
    if (zone == NULL)
        freeing_null (true);
    else
	free (zone);
#endif

    if (mem_signature_mode) {
      if (++mem_signatures->free_signature.top >= mem_signatures->free_signature.size)
	mem_signature_oflw (&(mem_signatures->free_signature));
      mem_signatures->free_signature.ptr_stack [mem_signatures->free_signature.top] = zone;
    }
      
    SYNTAX_is_in_critical_zone = false;
}

bool sxpush_in_tobereset_signature (void *zone, size_t size)
{
  SYNTAX_is_in_critical_zone = true;

  if (++mem_signatures->tobereset_signature.top >= mem_signatures->tobereset_signature.size) {
    mem_sizes_oflw (&(mem_signatures->tobereset_sizes), mem_signatures->tobereset_signature.size);
    mem_signature_oflw (&(mem_signatures->tobereset_signature));
  }
  mem_signatures->tobereset_signature.ptr_stack [mem_signatures->tobereset_signature.top] = zone;
  mem_signatures->tobereset_sizes [mem_signatures->tobereset_signature.top] = size;

  SYNTAX_is_in_critical_zone = false;

  return true;
}

void sxmem_signatures_allocate (SXINT size)
{
  bool is_in_mem_signature_mode;

  SYNTAX_is_in_critical_zone = true;

  is_in_mem_signature_mode = mem_signature_mode;

  mem_signatures->alloc_signature.top = mem_signatures->free_signature.top = 0;
  mem_signatures->tobereset_signature.top = 0;

  mem_signatures->alloc_signature.size = mem_signatures->free_signature.size = size;
  mem_signatures->tobereset_signature.size = size;

  mem_signature_mode = false;

  mem_signatures->alloc_signature.ptr_stack = (void**) sxalloc (size + 1, sizeof (void *));
  mem_signatures->free_signature.ptr_stack = (void**) sxalloc (size + 1, sizeof (void *));
  mem_signatures->tobereset_signature.ptr_stack = (void**) sxalloc (size + 1, sizeof (void *));
  mem_signatures->tobereset_sizes = (size_t*) sxalloc (size + 1, sizeof (size_t));

  mem_signature_mode = is_in_mem_signature_mode;

  SYNTAX_is_in_critical_zone = false;
}

void sxmem_signatures_raz (void)
{
  SYNTAX_is_in_critical_zone = true;

  mem_signatures->alloc_signature.top = mem_signatures->free_signature.top = 0;
  mem_signatures->tobereset_signature.top = 0;

  SYNTAX_is_in_critical_zone = false;
}

void sxmem_signatures_free (void)
{
  bool is_in_mem_signature_mode;

  SYNTAX_is_in_critical_zone = true;

  is_in_mem_signature_mode = mem_signature_mode;

  mem_signatures->alloc_signature.top = mem_signatures->free_signature.top = 0;
  mem_signatures->tobereset_signature.top = 0;

  mem_signatures->alloc_signature.size = mem_signatures->free_signature.size = 0;
  mem_signatures->tobereset_signature.size = 0;

  mem_signature_mode = false;

  sxfree (mem_signatures->alloc_signature.ptr_stack);
  sxfree (mem_signatures->free_signature.ptr_stack);
  sxfree (mem_signatures->tobereset_signature.ptr_stack);
  sxfree (mem_signatures->tobereset_sizes);

  mem_signatures->alloc_signature.ptr_stack = mem_signatures->free_signature.ptr_stack = NULL;
  mem_signatures->tobereset_signature.ptr_stack = NULL;
  mem_signatures->tobereset_sizes = NULL;

  mem_signature_mode = is_in_mem_signature_mode;

  SYNTAX_is_in_critical_zone = false;
}

static SXINT *ptr_id_in_X_hd2free_nb;

static void X_oflw (SXINT old_size, SXINT new_size) {
  sxuse (old_size);
  ptr_id_in_X_hd2free_nb = (SXINT*) sxrealloc (ptr_id_in_X_hd2free_nb, new_size + 1, sizeof (SXINT));
}

bool  sxfree_mem_signatures_content (void)
{
  void* ptr;
  X_header X_hd;
  SXINT i, addr;
  bool ret_val = true;

  SYNTAX_is_in_critical_zone = true;

#if BUG
    fprintf (stdout, "*** Entering sxfree_mem_signatures_content\n");
#endif

  if (mem_signature_mode) {
    mem_signature_mode = false;

    if (mem_signatures->alloc_signature.top != mem_signatures->free_signature.top) {
    
      X_alloc (&X_hd, "X_hd", mem_signatures->free_signature.top + 1, 1, X_oflw, NULL);
      ptr_id_in_X_hd2free_nb = (SXINT*) sxalloc (X_size (X_hd) + 1, sizeof (SXINT));
      
      for (i = 1; i <= mem_signatures->free_signature.top; i++) {
	if (X_set (&X_hd, (SXINT) (intptr_t) (mem_signatures->free_signature.ptr_stack [i]), &addr))
	  ptr_id_in_X_hd2free_nb [addr] ++;
	else
	  ptr_id_in_X_hd2free_nb [addr] = 1;
      }
      
      for (i = 1; i <= mem_signatures->alloc_signature.top; i++) {
	ptr = mem_signatures->alloc_signature.ptr_stack [i];
	addr = X_is_set (&X_hd, (SXINT) (intptr_t) (ptr));
	if (addr == 0 || ptr_id_in_X_hd2free_nb [addr]-- == 0) {
	  sxfree (ptr);
	  ret_val = false;
	}
      }
      
      X_free (&X_hd);
      sxfree (ptr_id_in_X_hd2free_nb), ptr_id_in_X_hd2free_nb = NULL;
    }

    for (i = 1; i <= mem_signatures->tobereset_signature.top; i++) {
      ptr = mem_signatures->tobereset_signature.ptr_stack [i];
      memset (ptr, 0, mem_signatures->tobereset_sizes [i]);
#if BUG
      fprintf (stdout, "RAZing 0x%lX (%lu)\n", (SXUINT) ptr, (SXUINT) (mem_signatures->tobereset_sizes [i])); 
#endif /* BUG */
    }


    mem_signatures->alloc_signature.top = 0;
    mem_signatures->free_signature.top = 0;
    mem_signatures->tobereset_signature.top = 0;

    mem_signature_mode = true;
  }

#if BUG
    fprintf (stdout, "*** Leaving sxfree_mem_signatures_content\n");
#endif

  SYNTAX_is_in_critical_zone = false;

  return ret_val;
}





/* The function 'sx_alloc_and_copy()' should
   - allocate new memory for 'nbemb' elements of size 'size'
   - copy into this memory the content pointed by 'original_content' (size of the copy : 'copy_size')
   - return a pointer to the newly allocated memory

   This function must be used instead of sxrealloc(), when there is the need to have a pointer reallocated
   with a bigger size, and that the pointed memory has not been dynamically allocated. As an example,
   see $SX/src/sxp_rcvr.c, end of function recovery() :

   original_content = sxrealloc (original_content,
                                 strlen (original_content) + strlen (new_content) + 1,
                                 sizeof (char)); 

       becomes :

   original_content = sx_alloc_and_copy (strlen (original_content) + strlen (new_content) + 1,
                                         sizeof(char),
					 original_content,
					 strlen(original_contenat) );

   The difference from sxrealloc() is that sx_alloc_and_copy() is intended to handle the case
   where *original_content points to the stack (i.e., it denotes a block of memory allocated
   statically), whereas sxrealloc() only handles dynamically allocated memory (in the heap). 

*/

void *sx_alloc_and_copy (size_t nbemb, size_t size, void *original_content, size_t copy_size)
{
  char *ptr;

  if ( (nbemb * size) < copy_size ) {
    fprintf (sxstderr, "allocated size is too small (nbelem %lu/ size_elem %lu/ copy_size %lu)",
	     (SXUINT) nbemb, (SXUINT) size, (SXUINT) copy_size);
    sxtrap("sxmem_mngr","sx_alloc_and_copy()");
  }

  if ((ptr = sxcalloc (nbemb, size))==NULL)
    sxtrap ("sxmem_mngr", "sx_alloc_and_copy() : allocation failed");
  
  memcpy (ptr, original_content, copy_size);

  return ptr;

}
