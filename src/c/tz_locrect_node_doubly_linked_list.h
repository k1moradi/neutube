/**@file tz_locrect_node_doubly_linked_list.h
 * @brief doubly linked list of Locrect_Node_P
 * @author Ting Zhao
 */

/* tz_doubly_linked_list.h.t
 *
 * 27-Nov-2007 Initial write: Ting Zhao
 */

/* Locrect_Node prefix Locrect_Node_P type object free type*/

#ifndef _TZ_Locrect_Node_DOUBLY_LINKED_LIST_H_
#define _TZ_Locrect_Node_DOUBLY_LINKED_LIST_H_

#include "tz_cdefs.h"
#include "tz_doubly_linked_list_defs.h"

#include "tz_locrect_node.h"


__BEGIN_DECLS

typedef struct _Locrect_Node_Dlist {
  Locrect_Node_P data;                    /* data */
  struct _Locrect_Node_Dlist *prev;     /* previous node */
  struct _Locrect_Node_Dlist *next;     /* next node */
} Locrect_Node_Dlist;

/*
 * Locrect_Node_Dlist_New() news a list with one node.
 */
Locrect_Node_Dlist* Locrect_Node_Dlist_New();

/*
 * Locrect_Node_Dlist_Removeall() removes all nodes of <p>. All memories associated with
 * <p> will be freed.
 */
void Locrect_Node_Dlist_Remove_All(Locrect_Node_Dlist *p);

/*
 * Locrect_Node_Dlist_Set_Data() sets data for the current node of <p>.
 */
void Locrect_Node_Dlist_Set_Data(Locrect_Node_Dlist *p, Locrect_Node_P data);


/*
 * Locrect_Node_Dlist_Head() return the head of <p> and Locrect_Node_Dlist_Tail() returns the
 * tail of p.
 */
Locrect_Node_Dlist* Locrect_Node_Dlist_Head(const Locrect_Node_Dlist *p);
Locrect_Node_Dlist* Locrect_Node_Dlist_Tail(const Locrect_Node_Dlist *p);

/*
 * Locrect_Node_Dlist_Next() returns the next node of <p> along the direction <d>.
 */
Locrect_Node_Dlist* Locrect_Node_Dlist_Next(const Locrect_Node_Dlist *p, Dlist_Direction_e d);

/*
 * Locrect_Node_Dlist_Length() returns the length of <p> in the direction <d>, which
 * can be forward (DL_FORWARD), backward (DL_BACKWARD) or both ways 
* (DL_BOTHDIR).
 * When the length is achieved in both ways, it is the total length of the list.
 * It returns 0 when the list is NULL and a negative number when something is 
 * wrong in the function (e.g. invalid direction option).
 */
int Locrect_Node_Dlist_Length(const Locrect_Node_Dlist *p, Dlist_Direction_e d);

/*
 * Locate_Locrect_Node_Dlist locates a certain node that has offset <n> to <p>. <n> can
 * be negative, which means the located node is ahead of <p>. It returns NULL
 * if the location is out of range.
 */
Locrect_Node_Dlist* Locate_Locrect_Node_Dlist(const Locrect_Node_Dlist *p, int n);

/*
 * Append_Locrect_Node_Dlist() appends <p1> and <p2> and returns the head of the new
 * list.
 */
Locrect_Node_Dlist* Append_Locrect_Node_Dlist(Locrect_Node_Dlist *p1, Locrect_Node_Dlist *p2);

/*
 * Locrect_Node_Dlist_Remove_End() removes one of the ends of <p> according to <e>:
 * DL_HEAD for head and DL_TAIL for tail. It returns the new head or tail if
 * the new list is not empty; otherwise it returns NULL.
 */
Locrect_Node_Dlist* Locrect_Node_Dlist_Remove_End(Locrect_Node_Dlist *p, Dlist_End_e e);


/*
 * Locrect_Node_Dlist_Remove_Node() removes the node <p> from its list.
 * It returns the new head if the new list is not empty; otherwise it 
 * returns NULL.
 */
Locrect_Node_Dlist* Locrect_Node_Dlist_Remove_Node(Locrect_Node_Dlist *p);

/*
 * Locrect_Node_Dlist_Unlinkall() removes all nodes but keeps the associated data
 * intact if they are references.
 */
void Locrect_Node_Dlist_Unlinkall(Locrect_Node_Dlist *p);

/*
 * Locrect_Node_Dlist_Add() adds data as a new node to the list. <e> speficies where
 * where the new node is added. It can noly be only DL_HEAD or DL_TAIL. DL_HEAD
 * means the new node is added as the head and DL_TAIL means the new node is
 * added as the tail. It returns the new node.
 */
Locrect_Node_Dlist* Locrect_Node_Dlist_Add(Locrect_Node_Dlist *p, Locrect_Node_P data, Dlist_End_e e);

/*
 * Locrect_Node_Dlist_Insert() inserts <data> into <p> right before its current 
 * position.
 */
void Locrect_Node_Dlist_Insert(Locrect_Node_Dlist *p, Locrect_Node_P data);

/*
 * Locrect_Node_Dlist_Is_Empty() returns TRUE if <p> is empty; otherwise it returns 
 * FALSE.
 */
BOOL Locrect_Node_Dlist_Is_Empty(const Locrect_Node_Dlist *p);

/**@brief Turn a list to an array.
 *
 * Locrect_Node_Dlist_To_Array() copies a list to an array. 
 */
Locrect_Node_P* Locrect_Node_Dlist_To_Array(const Locrect_Node_Dlist *p, int step, Locrect_Node_P *array, 
			 int *length);

/*
 * Printing functions.
 */
void Print_Locrect_Node_Dlist(const Locrect_Node_Dlist *p);
void Print_Locrect_Node_Dlist_Compact(const Locrect_Node_Dlist *p);
void Print_Locrect_Node_Dlist_F(const Locrect_Node_Dlist *p, const char *format);


__END_DECLS

#endif
