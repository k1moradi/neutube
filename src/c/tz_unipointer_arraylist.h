/**@file tz_unipointer_arraylist.h
 * @brief arraylist of unipointer_t
 * @author Ting Zhao
 * @date 15-Feb-2008
 */

/* Unipointer prefix unipointer_t type compact_pointer element class (object or basic) */

#ifndef _TZ_Unipointer_ARRAYLIST_H_
#define _TZ_Unipointer_ARRAYLIST_H_

#include "tz_cdefs.h"





#include "tz_unipointer.h"


__BEGIN_DECLS

/**@brief Unipointer array list
 **/
typedef struct _Unipointer_Arraylist {
  int length;
  int capacity;
  unipointer_t *array;
} Unipointer_Arraylist;

/*
 * Print_Unipointer_Arraylist() prints the arraylist <a> to the screen.
 * 
 */
void Print_Unipointer_Arraylist(const Unipointer_Arraylist *a);

Unipointer_Arraylist* New_Unipointer_Arraylist();

/*
 * Note: These functions are obsolete. Make_Unipointer_Arraylist() and 
 * Kill_Unipointer_Arraylist() are preferred.
 */
Unipointer_Arraylist* Unipointer_Arraylist_New(int length, int margin);
void Unipointer_Arraylist_Free(Unipointer_Arraylist *a);

/*
 * Make_Unipointer_Arraylist() returns a new array list with <length> elements. The
 * capacity of the array will be <length> + <margin>, where <margin> should be
 * non-negative. If <length> or <margin> is less than 0, it returns NULL.
 *
 * Kill_Unipointer_Arraylist() kills the array list <a>. It frees all associated 
 * memory.
 */
Unipointer_Arraylist* Make_Unipointer_Arraylist(int length, int margin);
void Kill_Unipointer_Arraylist(Unipointer_Arraylist *a);

Unipointer_Arraylist* Unipointer_Arraylist_Wrap(unipointer_t *array, int n);

/**@brief Retrieve an element.
 */
unipointer_t Unipointer_Arraylist_At(const Unipointer_Arraylist *a, int index);

/**@brief Retrieve the last element.
 */
unipointer_t Unipointer_Arraylist_Last(const Unipointer_Arraylist *a);

/**@brief Retrieve the reference of an element.
 */
unipointer_t* Unipointer_Arraylist_At_R(const Unipointer_Arraylist *a, int index);

/**@brief Add an element.
 *
 * Unipointer_Arraylist_Add() adds <item> to the end of the arraylist <a>.
 */
void Unipointer_Arraylist_Add(Unipointer_Arraylist *a, unipointer_t item);

/**@brief Insert an element.
 *
 * Unipointer_Arraylist_Insert() inserts <item> to <a> at the position <index>. If the
 * <index> exceeds the current range, the array will be grown. But the new
 * elements will not be initialized except the one at <index>, which is the last
 * element.
 */
void Unipointer_Arraylist_Insert(Unipointer_Arraylist *a, int index, unipointer_t item);

/**@brief Remove an element.
 *
 * Unipointer_Arraylist_Take() removes the element at <index> and returns its value.
 */
unipointer_t Unipointer_Arraylist_Take(Unipointer_Arraylist *a, int index);

/**@brief Remove an element.
 *
 * Unipointer_Arraylist_Take_Last() removes the last element and returns its value.
 */
unipointer_t Unipointer_Arraylist_Take_Last(Unipointer_Arraylist *a);

/**@brief Set the length of the arraylist.
 *
 * Unipointer_Arraylist_Set_Length() sets the length of <a>. It does not change the 
 * capacity if <length> does not exceed the capacity. 
 */
void Unipointer_Arraylist_Set_Length(Unipointer_Arraylist *a, int length);

/**@brief Set an element.
 *
 * Unipointer_Aarraylist_Set() sets the element with <index> to <item>. If the array 
 * is grown, the new elements will be set to 0 if possible except the one at 
 * <index>.
 */
void Unipointer_Arraylist_Set(Unipointer_Arraylist *a, int index, unipointer_t item);

/**@brief Swap elements.
 *
 * Unipointer_Arraylist_Swap() swaps two elements at the positions [idx1] and [idx2].
 * Nothing will be done when either of the positions is not valid.
 */
void Unipointer_Arraylist_Swap(Unipointer_Arraylist *a, int idx1, int idx2);



__END_DECLS

#endif
