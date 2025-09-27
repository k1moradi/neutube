/**@file tz_swc_arraylist.h
 * @brief arraylist of Swc_Node
 * @author Ting Zhao
 * @date 15-Feb-2008
 */

/* Swc prefix Swc_Node type struct element class (object or basic) */

#ifndef _TZ_Swc_ARRAYLIST_H_
#define _TZ_Swc_ARRAYLIST_H_

#include "tz_cdefs.h"


#include "tz_swc_cell.h"





__BEGIN_DECLS

/**@brief Swc array list
 **/
typedef struct _Swc_Arraylist {
  int length;
  int capacity;
  Swc_Node *array;
} Swc_Arraylist;

/*
 * Print_Swc_Arraylist() prints the arraylist <a> to the screen.
 * 
 */
void Print_Swc_Arraylist(const Swc_Arraylist *a);

Swc_Arraylist* New_Swc_Arraylist();

/*
 * Note: These functions are obsolete. Make_Swc_Arraylist() and 
 * Kill_Swc_Arraylist() are preferred.
 */
Swc_Arraylist* Swc_Arraylist_New(int length, int margin);
void Swc_Arraylist_Free(Swc_Arraylist *a);

/*
 * Make_Swc_Arraylist() returns a new array list with <length> elements. The
 * capacity of the array will be <length> + <margin>, where <margin> should be
 * non-negative. If <length> or <margin> is less than 0, it returns NULL.
 *
 * Kill_Swc_Arraylist() kills the array list <a>. It frees all associated 
 * memory.
 */
Swc_Arraylist* Make_Swc_Arraylist(int length, int margin);
void Kill_Swc_Arraylist(Swc_Arraylist *a);

Swc_Arraylist* Swc_Arraylist_Wrap(Swc_Node *array, int n);

/**@brief Retrieve an element.
 */
Swc_Node Swc_Arraylist_At(const Swc_Arraylist *a, int index);

/**@brief Retrieve the last element.
 */
Swc_Node Swc_Arraylist_Last(const Swc_Arraylist *a);

/**@brief Retrieve the reference of an element.
 */
Swc_Node* Swc_Arraylist_At_R(const Swc_Arraylist *a, int index);

/**@brief Add an element.
 *
 * Swc_Arraylist_Add() adds <item> to the end of the arraylist <a>.
 */
void Swc_Arraylist_Add(Swc_Arraylist *a, Swc_Node item);

/**@brief Insert an element.
 *
 * Swc_Arraylist_Insert() inserts <item> to <a> at the position <index>. If the
 * <index> exceeds the current range, the array will be grown. But the new
 * elements will not be initialized except the one at <index>, which is the last
 * element.
 */
void Swc_Arraylist_Insert(Swc_Arraylist *a, int index, Swc_Node item);

/**@brief Remove an element.
 *
 * Swc_Arraylist_Take() removes the element at <index> and returns its value.
 */
Swc_Node Swc_Arraylist_Take(Swc_Arraylist *a, int index);

/**@brief Remove an element.
 *
 * Swc_Arraylist_Take_Last() removes the last element and returns its value.
 */
Swc_Node Swc_Arraylist_Take_Last(Swc_Arraylist *a);

/**@brief Set the length of the arraylist.
 *
 * Swc_Arraylist_Set_Length() sets the length of <a>. It does not change the 
 * capacity if <length> does not exceed the capacity. 
 */
void Swc_Arraylist_Set_Length(Swc_Arraylist *a, int length);

/**@brief Set an element.
 *
 * Swc_Aarraylist_Set() sets the element with <index> to <item>. If the array 
 * is grown, the new elements will be set to 0 if possible except the one at 
 * <index>.
 */
void Swc_Arraylist_Set(Swc_Arraylist *a, int index, Swc_Node item);

/**@brief Swap elements.
 *
 * Swc_Arraylist_Swap() swaps two elements at the positions [idx1] and [idx2].
 * Nothing will be done when either of the positions is not valid.
 */
void Swc_Arraylist_Swap(Swc_Arraylist *a, int idx1, int idx2);



__END_DECLS

#endif
