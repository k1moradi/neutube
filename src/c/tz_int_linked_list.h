/**@file tz_int_linked_list.h
 * @brief Linked list of int
 * @author Ting Zhao
 */

/* tz_linked_list.h.t
 *
 * 27-Nov-2007 Initial write: Ting Zhao
 */

/* Int prefix int type basic free type*/

#ifndef _TZ_Int_LINKED_LIST_H_
#define _TZ_Int_LINKED_LIST_H_

#include "tz_cdefs.h"




__BEGIN_DECLS

typedef struct _Int_List {
  int data;
  struct _Int_List *next;
} Int_List;

/*
 * Int_List_New() news a list with one node.
 */
Int_List* Int_List_New();

/*
 * Int_List_Set_Data() sets data for the current node of <p>.
 */
void Int_List_Set_Data(Int_List *p, int data);

/*
 * Int_List_Removeall() removes all nodes, including their data, from a list.
 * Int_List_Unlinkall() also removes all nodes but keeps the associated data
 * intact if they are references.
 */
void Int_List_Removeall(Int_List **p);
void Int_List_Unlinkall(Int_List **p);

void Kill_Int_List(Int_List *p);

/*
 * Int_List_Add() adds data as a new node at the beginning of the list.
 * It returns the new node. <p> will also point to the new head. 
 * Int_List_Add_Last() adds data to the end of the list and returns the new
 * tail. <p> should not be NULL for this function.
 */
Int_List* Int_List_Add(Int_List **p, int data);
Int_List* Int_List_Add_Last(Int_List *p, int data);

/*
 * Int_List_Remove() removes the first node from p. Int_List_Unlink() also
 * removes the first node from p, but it will return the data in the removed
 * node.
 */
void Int_List_Remove(Int_List **p);
int Int_List_Unlink(Int_List **p);

/*
 * Int_List_Remove_Last() removes the last node of the list <p>. Its behavior
 * is not defined if <p> is the last node itself. 
 */
void Int_List_Remove_Last(Int_List *p);


Int_List* Int_List_Next(Int_List *p);
Int_List* Int_List_Last(Int_List *p);

BOOL Int_List_Is_Empty(const Int_List *p);
int Int_List_Length(const Int_List *p);

/*
 * Int_List_To_Array() copies a list to an array. 
 */
int* Int_List_To_Array(const Int_List *p, int step, int *array, 
			 int *length);

void Print_Int_List(const Int_List *p);
void Print_Int_List_Compact(const Int_List *p);
void Print_Int_List_F(const Int_List *p, const char *format);

/*
 * The following routines implement a queue by a list.
 */

/**@brief Enqueue an element.
 *
 * Int_Queue_En() adds <data> to the end of the queue <p> and returns the
 * new end. <*p> can be NULL, but after the function call, <*p> will be 
 * changed to the head position.
 */
Int_List* Int_Queue_En(Int_List **p, int data);

/**@brief Dequeue an element.
 *
 * Int_Queue_De() removes the element at the head of the queue and returns
 * the removed element. <*p> is changed to the new head position or NULL if
 * it is empty.
 */
int Int_Queue_De(Int_List **p);

int Int_Queue_Peek(const Int_List *p); 

/*
 * The following routines implement a stack by a list.
 */
void Int_Stack_Push(Int_List **p, int data);
int Int_Stack_Pop(Int_List **p);
int Int_Stack_Peek(const Int_List *p); 


__END_DECLS

#endif
