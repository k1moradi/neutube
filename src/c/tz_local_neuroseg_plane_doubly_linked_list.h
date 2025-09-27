/**@file tz_local_neuroseg_plane_doubly_linked_list.h
 * @brief doubly linked list of Local_Neuroseg_Plane_P
 * @author Ting Zhao
 */

/* tz_doubly_linked_list.h.t
 *
 * 27-Nov-2007 Initial write: Ting Zhao
 */

/* Local_Neuroseg_Plane prefix Local_Neuroseg_Plane_P type compact_pointer free type*/

#ifndef _TZ_Local_Neuroseg_Plane_DOUBLY_LINKED_LIST_H_
#define _TZ_Local_Neuroseg_Plane_DOUBLY_LINKED_LIST_H_

#include "tz_cdefs.h"
#include "tz_doubly_linked_list_defs.h"

#include "tz_local_neuroseg_plane.h"


__BEGIN_DECLS

typedef struct _Local_Neuroseg_Plane_Dlist {
  Local_Neuroseg_Plane_P data;                    /* data */
  struct _Local_Neuroseg_Plane_Dlist *prev;     /* previous node */
  struct _Local_Neuroseg_Plane_Dlist *next;     /* next node */
} Local_Neuroseg_Plane_Dlist;

/*
 * Local_Neuroseg_Plane_Dlist_New() news a list with one node.
 */
Local_Neuroseg_Plane_Dlist* Local_Neuroseg_Plane_Dlist_New();

/*
 * Local_Neuroseg_Plane_Dlist_Removeall() removes all nodes of <p>. All memories associated with
 * <p> will be freed.
 */
void Local_Neuroseg_Plane_Dlist_Remove_All(Local_Neuroseg_Plane_Dlist *p);

/*
 * Local_Neuroseg_Plane_Dlist_Set_Data() sets data for the current node of <p>.
 */
void Local_Neuroseg_Plane_Dlist_Set_Data(Local_Neuroseg_Plane_Dlist *p, Local_Neuroseg_Plane_P data);


/*
 * Local_Neuroseg_Plane_Dlist_Head() return the head of <p> and Local_Neuroseg_Plane_Dlist_Tail() returns the
 * tail of p.
 */
Local_Neuroseg_Plane_Dlist* Local_Neuroseg_Plane_Dlist_Head(const Local_Neuroseg_Plane_Dlist *p);
Local_Neuroseg_Plane_Dlist* Local_Neuroseg_Plane_Dlist_Tail(const Local_Neuroseg_Plane_Dlist *p);

/*
 * Local_Neuroseg_Plane_Dlist_Next() returns the next node of <p> along the direction <d>.
 */
Local_Neuroseg_Plane_Dlist* Local_Neuroseg_Plane_Dlist_Next(const Local_Neuroseg_Plane_Dlist *p, Dlist_Direction_e d);

/*
 * Local_Neuroseg_Plane_Dlist_Length() returns the length of <p> in the direction <d>, which
 * can be forward (DL_FORWARD), backward (DL_BACKWARD) or both ways 
* (DL_BOTHDIR).
 * When the length is achieved in both ways, it is the total length of the list.
 * It returns 0 when the list is NULL and a negative number when something is 
 * wrong in the function (e.g. invalid direction option).
 */
int Local_Neuroseg_Plane_Dlist_Length(const Local_Neuroseg_Plane_Dlist *p, Dlist_Direction_e d);

/*
 * Locate_Local_Neuroseg_Plane_Dlist locates a certain node that has offset <n> to <p>. <n> can
 * be negative, which means the located node is ahead of <p>. It returns NULL
 * if the location is out of range.
 */
Local_Neuroseg_Plane_Dlist* Locate_Local_Neuroseg_Plane_Dlist(const Local_Neuroseg_Plane_Dlist *p, int n);

/*
 * Append_Local_Neuroseg_Plane_Dlist() appends <p1> and <p2> and returns the head of the new
 * list.
 */
Local_Neuroseg_Plane_Dlist* Append_Local_Neuroseg_Plane_Dlist(Local_Neuroseg_Plane_Dlist *p1, Local_Neuroseg_Plane_Dlist *p2);

/*
 * Local_Neuroseg_Plane_Dlist_Remove_End() removes one of the ends of <p> according to <e>:
 * DL_HEAD for head and DL_TAIL for tail. It returns the new head or tail if
 * the new list is not empty; otherwise it returns NULL.
 */
Local_Neuroseg_Plane_Dlist* Local_Neuroseg_Plane_Dlist_Remove_End(Local_Neuroseg_Plane_Dlist *p, Dlist_End_e e);


/*
 * Local_Neuroseg_Plane_Dlist_Remove_Node() removes the node <p> from its list.
 * It returns the new head if the new list is not empty; otherwise it 
 * returns NULL.
 */
Local_Neuroseg_Plane_Dlist* Local_Neuroseg_Plane_Dlist_Remove_Node(Local_Neuroseg_Plane_Dlist *p);

/*
 * Local_Neuroseg_Plane_Dlist_Unlinkall() removes all nodes but keeps the associated data
 * intact if they are references.
 */
void Local_Neuroseg_Plane_Dlist_Unlinkall(Local_Neuroseg_Plane_Dlist *p);

/*
 * Local_Neuroseg_Plane_Dlist_Add() adds data as a new node to the list. <e> speficies where
 * where the new node is added. It can noly be only DL_HEAD or DL_TAIL. DL_HEAD
 * means the new node is added as the head and DL_TAIL means the new node is
 * added as the tail. It returns the new node.
 */
Local_Neuroseg_Plane_Dlist* Local_Neuroseg_Plane_Dlist_Add(Local_Neuroseg_Plane_Dlist *p, Local_Neuroseg_Plane_P data, Dlist_End_e e);

/*
 * Local_Neuroseg_Plane_Dlist_Insert() inserts <data> into <p> right before its current 
 * position.
 */
void Local_Neuroseg_Plane_Dlist_Insert(Local_Neuroseg_Plane_Dlist *p, Local_Neuroseg_Plane_P data);

/*
 * Local_Neuroseg_Plane_Dlist_Is_Empty() returns TRUE if <p> is empty; otherwise it returns 
 * FALSE.
 */
BOOL Local_Neuroseg_Plane_Dlist_Is_Empty(const Local_Neuroseg_Plane_Dlist *p);

/**@brief Turn a list to an array.
 *
 * Local_Neuroseg_Plane_Dlist_To_Array() copies a list to an array. 
 */
Local_Neuroseg_Plane_P* Local_Neuroseg_Plane_Dlist_To_Array(const Local_Neuroseg_Plane_Dlist *p, int step, Local_Neuroseg_Plane_P *array, 
			 int *length);

/*
 * Printing functions.
 */
void Print_Local_Neuroseg_Plane_Dlist(const Local_Neuroseg_Plane_Dlist *p);
void Print_Local_Neuroseg_Plane_Dlist_Compact(const Local_Neuroseg_Plane_Dlist *p);
void Print_Local_Neuroseg_Plane_Dlist_F(const Local_Neuroseg_Plane_Dlist *p, const char *format);


__END_DECLS

#endif
