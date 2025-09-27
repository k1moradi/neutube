/**@file tz_int64_arraylist.h
 * @brief arraylist of int64_t
 * @author Ting Zhao
 * @date 15-Feb-2008
 */

/* Int64 prefix int64_t type basic element class (object or basic) */

#ifndef _TZ_Int64_ARRAYLIST_H_
#define _TZ_Int64_ARRAYLIST_H_

#include "tz_cdefs.h"






__BEGIN_DECLS

/**@brief Int64 array list
 **/
typedef struct _Int64_Arraylist {
  int length;
  int capacity;
  int64_t *array;
} Int64_Arraylist;

/*
 * Print_Int64_Arraylist() prints the arraylist <a> to the screen.
 * <format> is the format of printing
 */
void Print_Int64_Arraylist(const Int64_Arraylist *a, const char *format);

Int64_Arraylist* New_Int64_Arraylist();

/*
 * Note: These functions are obsolete. Make_Int64_Arraylist() and 
 * Kill_Int64_Arraylist() are preferred.
 */
Int64_Arraylist* Int64_Arraylist_New(int length, int margin);
void Int64_Arraylist_Free(Int64_Arraylist *a);

/*
 * Make_Int64_Arraylist() returns a new array list with <length> elements. The
 * capacity of the array will be <length> + <margin>, where <margin> should be
 * non-negative. If <length> or <margin> is less than 0, it returns NULL.
 *
 * Kill_Int64_Arraylist() kills the array list <a>. It frees all associated 
 * memory.
 */
Int64_Arraylist* Make_Int64_Arraylist(int length, int margin);
void Kill_Int64_Arraylist(Int64_Arraylist *a);

Int64_Arraylist* Int64_Arraylist_Wrap(int64_t *array, int n);

/**@brief Retrieve an element.
 */
int64_t Int64_Arraylist_At(const Int64_Arraylist *a, int index);

/**@brief Retrieve the last element.
 */
int64_t Int64_Arraylist_Last(const Int64_Arraylist *a);

/**@brief Retrieve the reference of an element.
 */
int64_t* Int64_Arraylist_At_R(const Int64_Arraylist *a, int index);

/**@brief Add an element.
 *
 * Int64_Arraylist_Add() adds <item> to the end of the arraylist <a>.
 */
void Int64_Arraylist_Add(Int64_Arraylist *a, int64_t item);

/**@brief Insert an element.
 *
 * Int64_Arraylist_Insert() inserts <item> to <a> at the position <index>. If the
 * <index> exceeds the current range, the array will be grown. But the new
 * elements will not be initialized except the one at <index>, which is the last
 * element.
 */
void Int64_Arraylist_Insert(Int64_Arraylist *a, int index, int64_t item);

/**@brief Remove an element.
 *
 * Int64_Arraylist_Take() removes the element at <index> and returns its value.
 */
int64_t Int64_Arraylist_Take(Int64_Arraylist *a, int index);

/**@brief Remove an element.
 *
 * Int64_Arraylist_Take_Last() removes the last element and returns its value.
 */
int64_t Int64_Arraylist_Take_Last(Int64_Arraylist *a);

/**@brief Set the length of the arraylist.
 *
 * Int64_Arraylist_Set_Length() sets the length of <a>. It does not change the 
 * capacity if <length> does not exceed the capacity. 
 */
void Int64_Arraylist_Set_Length(Int64_Arraylist *a, int length);

/**@brief Set an element.
 *
 * Int64_Aarraylist_Set() sets the element with <index> to <item>. If the array 
 * is grown, the new elements will be set to 0 if possible except the one at 
 * <index>.
 */
void Int64_Arraylist_Set(Int64_Arraylist *a, int index, int64_t item);

/**@brief Swap elements.
 *
 * Int64_Arraylist_Swap() swaps two elements at the positions [idx1] and [idx2].
 * Nothing will be done when either of the positions is not valid.
 */
void Int64_Arraylist_Swap(Int64_Arraylist *a, int idx1, int idx2);



/* Routines for min-heap implementation 
 *
 * parent <= child
 *
 * The children of the node with index k are the nodes with indices 2k (left) 
 * and 2k+1 (right). Therefore the parent of the node is k/2. The root is the 
 * node with index 1. The node with index 0 is not used.
 */

/**@brief Depth of the heap.
 * 
 * Int64_Heap_Depth() returns the depth of the heap <h>.
 */
int Int64_Heap_Depth(const Int64_Arraylist *h);

/**@brief Node query.
 *
 * Int64_Heap_Node_At() returns a node of the heap at depth <depth>. <rank> is
 * the order of the node at its depth and it's 0-indexed. 
 */
int64_t Int64_Heap_Node_At(const Int64_Arraylist *h, int depth, int rank);

/**@brief Print a heap.
 *
 * Print_Int64_Heap() prints a heap to a screen. format is the format
 * to print each element.
 */
void Print_Int64_Heap(const Int64_Arraylist *h, const char *format);

/**@brief Parent of a node.
 *
 * Int64_Heap_Parent() returns the index of parent of the node with index 
 * <child_index> in the heap <h>. It returns 0 when the child node is not in 
 * <h> or it is the root.
 */
int Int64_Heap_Parent(const Int64_Arraylist *h, int child_index);

/**@brief Left child of a node
 *
 * Int64_Heap_Leftchild() returns the index of left child of the node with index 
 * <parent_index> in the heap <h>. It returns 0 when the parent node is not in 
 * <h> or it is a leaf.
 */
int Int64_Heap_Leftchild(const Int64_Arraylist *h, int parent_index);

/**@brief Smaller child of a node
 *
 * Int64_Heap_Small_Child() returns the index of the smaller child of the node 
 * with index  <parent_index> in the heap <h>. It returns 0 when the parent 
 * node is not in <h> or it is a leaf.
 */
int Int64_Heap_Small_Child(const Int64_Arraylist *h, int parent_index);

/**@brief Add a node.
 *
 * Int64_Heap_Add() adds a node to the end of the heap. The nodes will be 
 * adjusted automatically so that <h> is still a heap.
 */
void Int64_Heap_Add(Int64_Arraylist *h, int64_t item);

/*@brief Remove minimal.
 *
 * Int64_Heap_Remove() removes the root node in the heap <h> and returns its
 * value. The remained nodes will be adjusted automatically so that <h> is 
 * still a heap.
 */
int64_t Int64_Heap_Remove(Int64_Arraylist *h);

/**@brief Test is an arraylist is a heap
 *
 * Verify_Int64_Heap() returns 0 if <h> is a heap. Otherwise it returns the index
 * where violation happens.
 */
int Verify_Int64_Heap(Int64_Arraylist *h);



/*
 * Int64_Heap_Sort() sorts an array using heap sort alogrithm.
 */
void Int64_Heap_Sort(Int64_Arraylist *a);



__END_DECLS

#endif
