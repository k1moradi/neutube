/**@file tz_neuron_component_arraylist.h
 * @brief arraylist of Neuron_Component
 * @author Ting Zhao
 * @date 15-Feb-2008
 */

/* Neuron_Component prefix Neuron_Component type object element class (object or basic) */

#ifndef _TZ_Neuron_Component_ARRAYLIST_H_
#define _TZ_Neuron_Component_ARRAYLIST_H_

#include "tz_cdefs.h"

#include "tz_neuron_component.h"






__BEGIN_DECLS

/**@brief Neuron_Component array list
 **/
typedef struct _Neuron_Component_Arraylist {
  int length;
  int capacity;
  Neuron_Component *array;
} Neuron_Component_Arraylist;

/*
 * Print_Neuron_Component_Arraylist() prints the arraylist <a> to the screen.
 * 
 */
void Print_Neuron_Component_Arraylist(const Neuron_Component_Arraylist *a);

Neuron_Component_Arraylist* New_Neuron_Component_Arraylist();

/*
 * Note: These functions are obsolete. Make_Neuron_Component_Arraylist() and 
 * Kill_Neuron_Component_Arraylist() are preferred.
 */
Neuron_Component_Arraylist* Neuron_Component_Arraylist_New(int length, int margin);
void Neuron_Component_Arraylist_Free(Neuron_Component_Arraylist *a);

/*
 * Make_Neuron_Component_Arraylist() returns a new array list with <length> elements. The
 * capacity of the array will be <length> + <margin>, where <margin> should be
 * non-negative. If <length> or <margin> is less than 0, it returns NULL.
 *
 * Kill_Neuron_Component_Arraylist() kills the array list <a>. It frees all associated 
 * memory.
 */
Neuron_Component_Arraylist* Make_Neuron_Component_Arraylist(int length, int margin);
void Kill_Neuron_Component_Arraylist(Neuron_Component_Arraylist *a);

Neuron_Component_Arraylist* Neuron_Component_Arraylist_Wrap(Neuron_Component *array, int n);

/**@brief Retrieve an element.
 */
Neuron_Component Neuron_Component_Arraylist_At(const Neuron_Component_Arraylist *a, int index);

/**@brief Retrieve the last element.
 */
Neuron_Component Neuron_Component_Arraylist_Last(const Neuron_Component_Arraylist *a);

/**@brief Retrieve the reference of an element.
 */
Neuron_Component* Neuron_Component_Arraylist_At_R(const Neuron_Component_Arraylist *a, int index);

/**@brief Add an element.
 *
 * Neuron_Component_Arraylist_Add() adds <item> to the end of the arraylist <a>.
 */
void Neuron_Component_Arraylist_Add(Neuron_Component_Arraylist *a, Neuron_Component item);

/**@brief Insert an element.
 *
 * Neuron_Component_Arraylist_Insert() inserts <item> to <a> at the position <index>. If the
 * <index> exceeds the current range, the array will be grown. But the new
 * elements will not be initialized except the one at <index>, which is the last
 * element.
 */
void Neuron_Component_Arraylist_Insert(Neuron_Component_Arraylist *a, int index, Neuron_Component item);

/**@brief Remove an element.
 *
 * Neuron_Component_Arraylist_Take() removes the element at <index> and returns its value.
 */
Neuron_Component Neuron_Component_Arraylist_Take(Neuron_Component_Arraylist *a, int index);

/**@brief Remove an element.
 *
 * Neuron_Component_Arraylist_Take_Last() removes the last element and returns its value.
 */
Neuron_Component Neuron_Component_Arraylist_Take_Last(Neuron_Component_Arraylist *a);

/**@brief Set the length of the arraylist.
 *
 * Neuron_Component_Arraylist_Set_Length() sets the length of <a>. It does not change the 
 * capacity if <length> does not exceed the capacity. 
 */
void Neuron_Component_Arraylist_Set_Length(Neuron_Component_Arraylist *a, int length);

/**@brief Set an element.
 *
 * Neuron_Component_Aarraylist_Set() sets the element with <index> to <item>. If the array 
 * is grown, the new elements will be set to 0 if possible except the one at 
 * <index>.
 */
void Neuron_Component_Arraylist_Set(Neuron_Component_Arraylist *a, int index, Neuron_Component item);

/**@brief Swap elements.
 *
 * Neuron_Component_Arraylist_Swap() swaps two elements at the positions [idx1] and [idx2].
 * Nothing will be done when either of the positions is not valid.
 */
void Neuron_Component_Arraylist_Swap(Neuron_Component_Arraylist *a, int idx1, int idx2);



__END_DECLS

#endif
