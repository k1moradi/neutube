/**@file tz_object_3d_linked_list.h
 * @brief Linked list of Object_3d_P
 * @author Ting Zhao
 */

/* tz_linked_list.h.t
 *
 * 27-Nov-2007 Initial write: Ting Zhao
 */

/* Object_3d prefix Object_3d_P type object free type*/

#ifndef _TZ_Object_3d_LINKED_LIST_H_
#define _TZ_Object_3d_LINKED_LIST_H_

#include "tz_cdefs.h"

#include "tz_object_3d.h"




__BEGIN_DECLS

typedef struct _Object_3d_List {
  Object_3d_P data;
  struct _Object_3d_List *next;
} Object_3d_List;

/*
 * Object_3d_List_New() news a list with one node.
 */
Object_3d_List* Object_3d_List_New();

/*
 * Object_3d_List_Set_Data() sets data for the current node of <p>.
 */
void Object_3d_List_Set_Data(Object_3d_List *p, Object_3d_P data);

/*
 * Object_3d_List_Removeall() removes all nodes, including their data, from a list.
 * Object_3d_List_Unlinkall() also removes all nodes but keeps the associated data
 * intact if they are references.
 */
void Object_3d_List_Removeall(Object_3d_List **p);
void Object_3d_List_Unlinkall(Object_3d_List **p);

void Kill_Object_3d_List(Object_3d_List *p);

/*
 * Object_3d_List_Add() adds data as a new node at the beginning of the list.
 * It returns the new node. <p> will also point to the new head. 
 * Object_3d_List_Add_Last() adds data to the end of the list and returns the new
 * tail. <p> should not be NULL for this function.
 */
Object_3d_List* Object_3d_List_Add(Object_3d_List **p, Object_3d_P data);
Object_3d_List* Object_3d_List_Add_Last(Object_3d_List *p, Object_3d_P data);

/*
 * Object_3d_List_Remove() removes the first node from p. Object_3d_List_Unlink() also
 * removes the first node from p, but it will return the data in the removed
 * node.
 */
void Object_3d_List_Remove(Object_3d_List **p);
Object_3d_P Object_3d_List_Unlink(Object_3d_List **p);

/*
 * Object_3d_List_Remove_Last() removes the last node of the list <p>. Its behavior
 * is not defined if <p> is the last node itself. 
 */
void Object_3d_List_Remove_Last(Object_3d_List *p);


Object_3d_List* Object_3d_List_Next(Object_3d_List *p);
Object_3d_List* Object_3d_List_Last(Object_3d_List *p);

BOOL Object_3d_List_Is_Empty(const Object_3d_List *p);
int Object_3d_List_Length(const Object_3d_List *p);

/*
 * Object_3d_List_To_Array() copies a list to an array. 
 */
Object_3d_P* Object_3d_List_To_Array(const Object_3d_List *p, int step, Object_3d_P *array, 
			 int *length);

void Print_Object_3d_List(const Object_3d_List *p);
void Print_Object_3d_List_Compact(const Object_3d_List *p);
void Print_Object_3d_List_F(const Object_3d_List *p, const char *format);

/*
 * The following routines implement a queue by a list.
 */

/**@brief Enqueue an element.
 *
 * Object_3d_Queue_En() adds <data> to the end of the queue <p> and returns the
 * new end. <*p> can be NULL, but after the function call, <*p> will be 
 * changed to the head position.
 */
Object_3d_List* Object_3d_Queue_En(Object_3d_List **p, Object_3d_P data);

/**@brief Dequeue an element.
 *
 * Object_3d_Queue_De() removes the element at the head of the queue and returns
 * the removed element. <*p> is changed to the new head position or NULL if
 * it is empty.
 */
Object_3d_P Object_3d_Queue_De(Object_3d_List **p);

Object_3d_P Object_3d_Queue_Peek(const Object_3d_List *p); 

/*
 * The following routines implement a stack by a list.
 */
void Object_3d_Stack_Push(Object_3d_List **p, Object_3d_P data);
Object_3d_P Object_3d_Stack_Pop(Object_3d_List **p);
Object_3d_P Object_3d_Stack_Peek(const Object_3d_List *p); 


__END_DECLS

#endif
