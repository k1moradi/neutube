/**@file tz_unipointer_linked_list.h
 * @brief Linked list of unipointer_t
 * @author Ting Zhao
 */

/* tz_linked_list.h.t
 *
 * 27-Nov-2007 Initial write: Ting Zhao
 */

/* Unipointer prefix unipointer_t type compact_pointer free type*/

#ifndef _TZ_Unipointer_LINKED_LIST_H_
#define _TZ_Unipointer_LINKED_LIST_H_

#include "tz_cdefs.h"



#include "tz_unipointer.h"


__BEGIN_DECLS

typedef struct _Unipointer_List {
  unipointer_t data;
  struct _Unipointer_List *next;
} Unipointer_List;

/*
 * Unipointer_List_New() news a list with one node.
 */
Unipointer_List* Unipointer_List_New();

/*
 * Unipointer_List_Set_Data() sets data for the current node of <p>.
 */
void Unipointer_List_Set_Data(Unipointer_List *p, unipointer_t data);

/*
 * Unipointer_List_Removeall() removes all nodes, including their data, from a list.
 * Unipointer_List_Unlinkall() also removes all nodes but keeps the associated data
 * intact if they are references.
 */
void Unipointer_List_Removeall(Unipointer_List **p);
void Unipointer_List_Unlinkall(Unipointer_List **p);

void Kill_Unipointer_List(Unipointer_List *p);

/*
 * Unipointer_List_Add() adds data as a new node at the beginning of the list.
 * It returns the new node. <p> will also point to the new head. 
 * Unipointer_List_Add_Last() adds data to the end of the list and returns the new
 * tail. <p> should not be NULL for this function.
 */
Unipointer_List* Unipointer_List_Add(Unipointer_List **p, unipointer_t data);
Unipointer_List* Unipointer_List_Add_Last(Unipointer_List *p, unipointer_t data);

/*
 * Unipointer_List_Remove() removes the first node from p. Unipointer_List_Unlink() also
 * removes the first node from p, but it will return the data in the removed
 * node.
 */
void Unipointer_List_Remove(Unipointer_List **p);
unipointer_t Unipointer_List_Unlink(Unipointer_List **p);

/*
 * Unipointer_List_Remove_Last() removes the last node of the list <p>. Its behavior
 * is not defined if <p> is the last node itself. 
 */
void Unipointer_List_Remove_Last(Unipointer_List *p);


Unipointer_List* Unipointer_List_Next(Unipointer_List *p);
Unipointer_List* Unipointer_List_Last(Unipointer_List *p);

BOOL Unipointer_List_Is_Empty(const Unipointer_List *p);
int Unipointer_List_Length(const Unipointer_List *p);

/*
 * Unipointer_List_To_Array() copies a list to an array. 
 */
unipointer_t* Unipointer_List_To_Array(const Unipointer_List *p, int step, unipointer_t *array, 
			 int *length);

void Print_Unipointer_List(const Unipointer_List *p);
void Print_Unipointer_List_Compact(const Unipointer_List *p);
void Print_Unipointer_List_F(const Unipointer_List *p, const char *format);

/*
 * The following routines implement a queue by a list.
 */

/**@brief Enqueue an element.
 *
 * Unipointer_Queue_En() adds <data> to the end of the queue <p> and returns the
 * new end. <*p> can be NULL, but after the function call, <*p> will be 
 * changed to the head position.
 */
Unipointer_List* Unipointer_Queue_En(Unipointer_List **p, unipointer_t data);

/**@brief Dequeue an element.
 *
 * Unipointer_Queue_De() removes the element at the head of the queue and returns
 * the removed element. <*p> is changed to the new head position or NULL if
 * it is empty.
 */
unipointer_t Unipointer_Queue_De(Unipointer_List **p);

unipointer_t Unipointer_Queue_Peek(const Unipointer_List *p); 

/*
 * The following routines implement a stack by a list.
 */
void Unipointer_Stack_Push(Unipointer_List **p, unipointer_t data);
unipointer_t Unipointer_Stack_Pop(Unipointer_List **p);
unipointer_t Unipointer_Stack_Peek(const Unipointer_List *p); 


__END_DECLS

#endif
