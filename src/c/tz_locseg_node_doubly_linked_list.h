/**@file tz_locseg_node_doubly_linked_list.h
 * @brief doubly linked list of Locseg_Node_P
 * @author Ting Zhao
 */

/* tz_doubly_linked_list.h.t
 *
 * 27-Nov-2007 Initial write: Ting Zhao
 */

/* Locseg_Node prefix Locseg_Node_P type object free type*/

#ifndef _TZ_Locseg_Node_DOUBLY_LINKED_LIST_H_
#define _TZ_Locseg_Node_DOUBLY_LINKED_LIST_H_

#include "tz_cdefs.h"
#include "tz_doubly_linked_list_defs.h"

#include "tz_locseg_node.h"


__BEGIN_DECLS

typedef struct _Locseg_Node_Dlist {
  Locseg_Node_P data;                    /* data */
  struct _Locseg_Node_Dlist *prev;     /* previous node */
  struct _Locseg_Node_Dlist *next;     /* next node */
} Locseg_Node_Dlist;

/*
 * Locseg_Node_Dlist_New() news a list with one node.
 */
Locseg_Node_Dlist* Locseg_Node_Dlist_New();

/*
 * Locseg_Node_Dlist_Removeall() removes all nodes of <p>. All memories associated with
 * <p> will be freed.
 */
void Locseg_Node_Dlist_Remove_All(Locseg_Node_Dlist *p);

/*
 * Locseg_Node_Dlist_Set_Data() sets data for the current node of <p>.
 */
void Locseg_Node_Dlist_Set_Data(Locseg_Node_Dlist *p, Locseg_Node_P data);


/*
 * Locseg_Node_Dlist_Head() return the head of <p> and Locseg_Node_Dlist_Tail() returns the
 * tail of p.
 */
Locseg_Node_Dlist* Locseg_Node_Dlist_Head(const Locseg_Node_Dlist *p);
Locseg_Node_Dlist* Locseg_Node_Dlist_Tail(const Locseg_Node_Dlist *p);

/*
 * Locseg_Node_Dlist_Next() returns the next node of <p> along the direction <d>.
 */
Locseg_Node_Dlist* Locseg_Node_Dlist_Next(const Locseg_Node_Dlist *p, Dlist_Direction_e d);

/*
 * Locseg_Node_Dlist_Length() returns the length of <p> in the direction <d>, which
 * can be forward (DL_FORWARD), backward (DL_BACKWARD) or both ways 
* (DL_BOTHDIR).
 * When the length is achieved in both ways, it is the total length of the list.
 * It returns 0 when the list is NULL and a negative number when something is 
 * wrong in the function (e.g. invalid direction option).
 */
int Locseg_Node_Dlist_Length(const Locseg_Node_Dlist *p, Dlist_Direction_e d);

/*
 * Locate_Locseg_Node_Dlist locates a certain node that has offset <n> to <p>. <n> can
 * be negative, which means the located node is ahead of <p>. It returns NULL
 * if the location is out of range.
 */
Locseg_Node_Dlist* Locate_Locseg_Node_Dlist(const Locseg_Node_Dlist *p, int n);

/*
 * Append_Locseg_Node_Dlist() appends <p1> and <p2> and returns the head of the new
 * list.
 */
Locseg_Node_Dlist* Append_Locseg_Node_Dlist(Locseg_Node_Dlist *p1, Locseg_Node_Dlist *p2);

/*
 * Locseg_Node_Dlist_Remove_End() removes one of the ends of <p> according to <e>:
 * DL_HEAD for head and DL_TAIL for tail. It returns the new head or tail if
 * the new list is not empty; otherwise it returns NULL.
 */
Locseg_Node_Dlist* Locseg_Node_Dlist_Remove_End(Locseg_Node_Dlist *p, Dlist_End_e e);


/*
 * Locseg_Node_Dlist_Remove_Node() removes the node <p> from its list.
 * It returns the new head if the new list is not empty; otherwise it 
 * returns NULL.
 */
Locseg_Node_Dlist* Locseg_Node_Dlist_Remove_Node(Locseg_Node_Dlist *p);

/*
 * Locseg_Node_Dlist_Unlinkall() removes all nodes but keeps the associated data
 * intact if they are references.
 */
void Locseg_Node_Dlist_Unlinkall(Locseg_Node_Dlist *p);

/*
 * Locseg_Node_Dlist_Add() adds data as a new node to the list. <e> speficies where
 * where the new node is added. It can noly be only DL_HEAD or DL_TAIL. DL_HEAD
 * means the new node is added as the head and DL_TAIL means the new node is
 * added as the tail. It returns the new node.
 */
Locseg_Node_Dlist* Locseg_Node_Dlist_Add(Locseg_Node_Dlist *p, Locseg_Node_P data, Dlist_End_e e);

/*
 * Locseg_Node_Dlist_Insert() inserts <data> into <p> right before its current 
 * position.
 */
void Locseg_Node_Dlist_Insert(Locseg_Node_Dlist *p, Locseg_Node_P data);

/*
 * Locseg_Node_Dlist_Is_Empty() returns TRUE if <p> is empty; otherwise it returns 
 * FALSE.
 */
BOOL Locseg_Node_Dlist_Is_Empty(const Locseg_Node_Dlist *p);

/**@brief Turn a list to an array.
 *
 * Locseg_Node_Dlist_To_Array() copies a list to an array. 
 */
Locseg_Node_P* Locseg_Node_Dlist_To_Array(const Locseg_Node_Dlist *p, int step, Locseg_Node_P *array, 
			 int *length);

/*
 * Printing functions.
 */
void Print_Locseg_Node_Dlist(const Locseg_Node_Dlist *p);
void Print_Locseg_Node_Dlist_Compact(const Locseg_Node_Dlist *p);
void Print_Locseg_Node_Dlist_F(const Locseg_Node_Dlist *p, const char *format);


__END_DECLS

#endif
