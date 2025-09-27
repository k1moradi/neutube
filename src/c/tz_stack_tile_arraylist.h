/**@file tz_stack_tile_arraylist.h
 * @brief arraylist of Stack_Tile
 * @author Ting Zhao
 * @date 15-Feb-2008
 */

/* Stack_Tile prefix Stack_Tile type object element class (object or basic) */

#ifndef _TZ_Stack_Tile_ARRAYLIST_H_
#define _TZ_Stack_Tile_ARRAYLIST_H_

#include "tz_cdefs.h"



#include "tz_stack_tile.h"




__BEGIN_DECLS

/**@brief Stack_Tile array list
 **/
typedef struct _Stack_Tile_Arraylist {
  int length;
  int capacity;
  Stack_Tile *array;
} Stack_Tile_Arraylist;

/*
 * Print_Stack_Tile_Arraylist() prints the arraylist <a> to the screen.
 * 
 */
void Print_Stack_Tile_Arraylist(const Stack_Tile_Arraylist *a);

Stack_Tile_Arraylist* New_Stack_Tile_Arraylist();

/*
 * Note: These functions are obsolete. Make_Stack_Tile_Arraylist() and 
 * Kill_Stack_Tile_Arraylist() are preferred.
 */
Stack_Tile_Arraylist* Stack_Tile_Arraylist_New(int length, int margin);
void Stack_Tile_Arraylist_Free(Stack_Tile_Arraylist *a);

/*
 * Make_Stack_Tile_Arraylist() returns a new array list with <length> elements. The
 * capacity of the array will be <length> + <margin>, where <margin> should be
 * non-negative. If <length> or <margin> is less than 0, it returns NULL.
 *
 * Kill_Stack_Tile_Arraylist() kills the array list <a>. It frees all associated 
 * memory.
 */
Stack_Tile_Arraylist* Make_Stack_Tile_Arraylist(int length, int margin);
void Kill_Stack_Tile_Arraylist(Stack_Tile_Arraylist *a);

Stack_Tile_Arraylist* Stack_Tile_Arraylist_Wrap(Stack_Tile *array, int n);

/**@brief Retrieve an element.
 */
Stack_Tile Stack_Tile_Arraylist_At(const Stack_Tile_Arraylist *a, int index);

/**@brief Retrieve the last element.
 */
Stack_Tile Stack_Tile_Arraylist_Last(const Stack_Tile_Arraylist *a);

/**@brief Retrieve the reference of an element.
 */
Stack_Tile* Stack_Tile_Arraylist_At_R(const Stack_Tile_Arraylist *a, int index);

/**@brief Add an element.
 *
 * Stack_Tile_Arraylist_Add() adds <item> to the end of the arraylist <a>.
 */
void Stack_Tile_Arraylist_Add(Stack_Tile_Arraylist *a, Stack_Tile item);

/**@brief Insert an element.
 *
 * Stack_Tile_Arraylist_Insert() inserts <item> to <a> at the position <index>. If the
 * <index> exceeds the current range, the array will be grown. But the new
 * elements will not be initialized except the one at <index>, which is the last
 * element.
 */
void Stack_Tile_Arraylist_Insert(Stack_Tile_Arraylist *a, int index, Stack_Tile item);

/**@brief Remove an element.
 *
 * Stack_Tile_Arraylist_Take() removes the element at <index> and returns its value.
 */
Stack_Tile Stack_Tile_Arraylist_Take(Stack_Tile_Arraylist *a, int index);

/**@brief Remove an element.
 *
 * Stack_Tile_Arraylist_Take_Last() removes the last element and returns its value.
 */
Stack_Tile Stack_Tile_Arraylist_Take_Last(Stack_Tile_Arraylist *a);

/**@brief Set the length of the arraylist.
 *
 * Stack_Tile_Arraylist_Set_Length() sets the length of <a>. It does not change the 
 * capacity if <length> does not exceed the capacity. 
 */
void Stack_Tile_Arraylist_Set_Length(Stack_Tile_Arraylist *a, int length);

/**@brief Set an element.
 *
 * Stack_Tile_Aarraylist_Set() sets the element with <index> to <item>. If the array 
 * is grown, the new elements will be set to 0 if possible except the one at 
 * <index>.
 */
void Stack_Tile_Arraylist_Set(Stack_Tile_Arraylist *a, int index, Stack_Tile item);

/**@brief Swap elements.
 *
 * Stack_Tile_Arraylist_Swap() swaps two elements at the positions [idx1] and [idx2].
 * Nothing will be done when either of the positions is not valid.
 */
void Stack_Tile_Arraylist_Swap(Stack_Tile_Arraylist *a, int idx1, int idx2);



__END_DECLS

#endif
