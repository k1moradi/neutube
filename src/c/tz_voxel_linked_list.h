/**@file tz_voxel_linked_list.h
 * @brief Linked list of Voxel_P
 * @author Ting Zhao
 */

/* tz_linked_list.h.t
 *
 * 27-Nov-2007 Initial write: Ting Zhao
 */

/* Voxel prefix Voxel_P type compact_pointer free type*/

#ifndef _TZ_Voxel_LINKED_LIST_H_
#define _TZ_Voxel_LINKED_LIST_H_

#include "tz_cdefs.h"


#include "tz_voxel.h"



__BEGIN_DECLS

typedef struct _Voxel_List {
  Voxel_P data;
  struct _Voxel_List *next;
} Voxel_List;

/*
 * Voxel_List_New() news a list with one node.
 */
Voxel_List* Voxel_List_New();

/*
 * Voxel_List_Set_Data() sets data for the current node of <p>.
 */
void Voxel_List_Set_Data(Voxel_List *p, Voxel_P data);

/*
 * Voxel_List_Removeall() removes all nodes, including their data, from a list.
 * Voxel_List_Unlinkall() also removes all nodes but keeps the associated data
 * intact if they are references.
 */
void Voxel_List_Removeall(Voxel_List **p);
void Voxel_List_Unlinkall(Voxel_List **p);

void Kill_Voxel_List(Voxel_List *p);

/*
 * Voxel_List_Add() adds data as a new node at the beginning of the list.
 * It returns the new node. <p> will also point to the new head. 
 * Voxel_List_Add_Last() adds data to the end of the list and returns the new
 * tail. <p> should not be NULL for this function.
 */
Voxel_List* Voxel_List_Add(Voxel_List **p, Voxel_P data);
Voxel_List* Voxel_List_Add_Last(Voxel_List *p, Voxel_P data);

/*
 * Voxel_List_Remove() removes the first node from p. Voxel_List_Unlink() also
 * removes the first node from p, but it will return the data in the removed
 * node.
 */
void Voxel_List_Remove(Voxel_List **p);
Voxel_P Voxel_List_Unlink(Voxel_List **p);

/*
 * Voxel_List_Remove_Last() removes the last node of the list <p>. Its behavior
 * is not defined if <p> is the last node itself. 
 */
void Voxel_List_Remove_Last(Voxel_List *p);


Voxel_List* Voxel_List_Next(Voxel_List *p);
Voxel_List* Voxel_List_Last(Voxel_List *p);

BOOL Voxel_List_Is_Empty(const Voxel_List *p);
int Voxel_List_Length(const Voxel_List *p);

/*
 * Voxel_List_To_Array() copies a list to an array. 
 */
Voxel_P* Voxel_List_To_Array(const Voxel_List *p, int step, Voxel_P *array, 
			 int *length);

void Print_Voxel_List(const Voxel_List *p);
void Print_Voxel_List_Compact(const Voxel_List *p);
void Print_Voxel_List_F(const Voxel_List *p, const char *format);

/*
 * The following routines implement a queue by a list.
 */

/**@brief Enqueue an element.
 *
 * Voxel_Queue_En() adds <data> to the end of the queue <p> and returns the
 * new end. <*p> can be NULL, but after the function call, <*p> will be 
 * changed to the head position.
 */
Voxel_List* Voxel_Queue_En(Voxel_List **p, Voxel_P data);

/**@brief Dequeue an element.
 *
 * Voxel_Queue_De() removes the element at the head of the queue and returns
 * the removed element. <*p> is changed to the new head position or NULL if
 * it is empty.
 */
Voxel_P Voxel_Queue_De(Voxel_List **p);

Voxel_P Voxel_Queue_Peek(const Voxel_List *p); 

/*
 * The following routines implement a stack by a list.
 */
void Voxel_Stack_Push(Voxel_List **p, Voxel_P data);
Voxel_P Voxel_Stack_Pop(Voxel_List **p);
Voxel_P Voxel_Stack_Peek(const Voxel_List *p); 


__END_DECLS

#endif
