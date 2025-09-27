/**@file tz_double_linked_list.h
 * @brief Linked list of double
 * @author Ting Zhao
 */

/* tz_linked_list.h.t
 *
 * 27-Nov-2007 Initial write: Ting Zhao
 */

/* Double prefix double type basic free type*/

#ifndef _TZ_Double_LINKED_LIST_H_
#define _TZ_Double_LINKED_LIST_H_

#include "tz_cdefs.h"




__BEGIN_DECLS

typedef struct _Double_List {
  double data;
  struct _Double_List *next;
} Double_List;

/*
 * Double_List_New() news a list with one node.
 */
Double_List* Double_List_New();

/*
 * Double_List_Set_Data() sets data for the current node of <p>.
 */
void Double_List_Set_Data(Double_List *p, double data);

/*
 * Double_List_Removeall() removes all nodes, including their data, from a list.
 * Double_List_Unlinkall() also removes all nodes but keeps the associated data
 * intact if they are references.
 */
void Double_List_Removeall(Double_List **p);
void Double_List_Unlinkall(Double_List **p);

void Kill_Double_List(Double_List *p);

/*
 * Double_List_Add() adds data as a new node at the beginning of the list.
 * It returns the new node. <p> will also point to the new head. 
 * Double_List_Add_Last() adds data to the end of the list and returns the new
 * tail. <p> should not be NULL for this function.
 */
Double_List* Double_List_Add(Double_List **p, double data);
Double_List* Double_List_Add_Last(Double_List *p, double data);

/*
 * Double_List_Remove() removes the first node from p. Double_List_Unlink() also
 * removes the first node from p, but it will return the data in the removed
 * node.
 */
void Double_List_Remove(Double_List **p);
double Double_List_Unlink(Double_List **p);

/*
 * Double_List_Remove_Last() removes the last node of the list <p>. Its behavior
 * is not defined if <p> is the last node itself. 
 */
void Double_List_Remove_Last(Double_List *p);


Double_List* Double_List_Next(Double_List *p);
Double_List* Double_List_Last(Double_List *p);

BOOL Double_List_Is_Empty(const Double_List *p);
int Double_List_Length(const Double_List *p);

/*
 * Double_List_To_Array() copies a list to an array. 
 */
double* Double_List_To_Array(const Double_List *p, int step, double *array, 
			 int *length);

void Print_Double_List(const Double_List *p);
void Print_Double_List_Compact(const Double_List *p);
void Print_Double_List_F(const Double_List *p, const char *format);

/*
 * The following routines implement a queue by a list.
 */

/**@brief Enqueue an element.
 *
 * Double_Queue_En() adds <data> to the end of the queue <p> and returns the
 * new end. <*p> can be NULL, but after the function call, <*p> will be 
 * changed to the head position.
 */
Double_List* Double_Queue_En(Double_List **p, double data);

/**@brief Dequeue an element.
 *
 * Double_Queue_De() removes the element at the head of the queue and returns
 * the removed element. <*p> is changed to the new head position or NULL if
 * it is empty.
 */
double Double_Queue_De(Double_List **p);

double Double_Queue_Peek(const Double_List *p); 

/*
 * The following routines implement a stack by a list.
 */
void Double_Stack_Push(Double_List **p, double data);
double Double_Stack_Pop(Double_List **p);
double Double_Stack_Peek(const Double_List *p); 


__END_DECLS

#endif
