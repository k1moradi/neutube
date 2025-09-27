/**@file tz_int_arraylist.h
 * @brief arraylist of int
 * @author Ting Zhao
 * @date 15-Feb-2008
 */

/* Int prefix int type basic element class (object or basic) */

#ifndef _TZ_Int_ARRAYLIST_H_
#define _TZ_Int_ARRAYLIST_H_

#include "tz_cdefs.h"






__BEGIN_DECLS

/**@brief Int array list
 **/
typedef struct _Int_Arraylist {
  int length;
  int capacity;
  int *array;
} Int_Arraylist;

/*
 * Print_Int_Arraylist() prints the arraylist <a> to the screen.
 * <format> is the format of printing
 */
void Print_Int_Arraylist(const Int_Arraylist *a, const char *format);

Int_Arraylist* New_Int_Arraylist();

/*
 * Note: These functions are obsolete. Make_Int_Arraylist() and 
 * Kill_Int_Arraylist() are preferred.
 */
Int_Arraylist* Int_Arraylist_New(int length, int margin);
void Int_Arraylist_Free(Int_Arraylist *a);

/*
 * Make_Int_Arraylist() returns a new array list with <length> elements. The
 * capacity of the array will be <length> + <margin>, where <margin> should be
 * non-negative. If <length> or <margin> is less than 0, it returns NULL.
 *
 * Kill_Int_Arraylist() kills the array list <a>. It frees all associated 
 * memory.
 */
Int_Arraylist* Make_Int_Arraylist(int length, int margin);
void Kill_Int_Arraylist(Int_Arraylist *a);

Int_Arraylist* Int_Arraylist_Wrap(int *array, int n);

/**@brief Retrieve an element.
 */
int Int_Arraylist_At(const Int_Arraylist *a, int index);

/**@brief Retrieve the last element.
 */
int Int_Arraylist_Last(const Int_Arraylist *a);

/**@brief Retrieve the reference of an element.
 */
int* Int_Arraylist_At_R(const Int_Arraylist *a, int index);

/**@brief Add an element.
 *
 * Int_Arraylist_Add() adds <item> to the end of the arraylist <a>.
 */
void Int_Arraylist_Add(Int_Arraylist *a, int item);

/**@brief Insert an element.
 *
 * Int_Arraylist_Insert() inserts <item> to <a> at the position <index>. If the
 * <index> exceeds the current range, the array will be grown. But the new
 * elements will not be initialized except the one at <index>, which is the last
 * element.
 */
void Int_Arraylist_Insert(Int_Arraylist *a, int index, int item);

/**@brief Remove an element.
 *
 * Int_Arraylist_Take() removes the element at <index> and returns its value.
 */
int Int_Arraylist_Take(Int_Arraylist *a, int index);

/**@brief Remove an element.
 *
 * Int_Arraylist_Take_Last() removes the last element and returns its value.
 */
int Int_Arraylist_Take_Last(Int_Arraylist *a);

/**@brief Set the length of the arraylist.
 *
 * Int_Arraylist_Set_Length() sets the length of <a>. It does not change the 
 * capacity if <length> does not exceed the capacity. 
 */
void Int_Arraylist_Set_Length(Int_Arraylist *a, int length);

/**@brief Set an element.
 *
 * Int_Aarraylist_Set() sets the element with <index> to <item>. If the array 
 * is grown, the new elements will be set to 0 if possible except the one at 
 * <index>.
 */
void Int_Arraylist_Set(Int_Arraylist *a, int index, int item);

/**@brief Swap elements.
 *
 * Int_Arraylist_Swap() swaps two elements at the positions [idx1] and [idx2].
 * Nothing will be done when either of the positions is not valid.
 */
void Int_Arraylist_Swap(Int_Arraylist *a, int idx1, int idx2);



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
 * Int_Heap_Depth() returns the depth of the heap <h>.
 */
int Int_Heap_Depth(const Int_Arraylist *h);

/**@brief Node query.
 *
 * Int_Heap_Node_At() returns a node of the heap at depth <depth>. <rank> is
 * the order of the node at its depth and it's 0-indexed. 
 */
int Int_Heap_Node_At(const Int_Arraylist *h, int depth, int rank);

/**@brief Print a heap.
 *
 * Print_Int_Heap() prints a heap to a screen. format is the format
 * to print each element.
 */
void Print_Int_Heap(const Int_Arraylist *h, const char *format);

/**@brief Parent of a node.
 *
 * Int_Heap_Parent() returns the index of parent of the node with index 
 * <child_index> in the heap <h>. It returns 0 when the child node is not in 
 * <h> or it is the root.
 */
int Int_Heap_Parent(const Int_Arraylist *h, int child_index);

/**@brief Left child of a node
 *
 * Int_Heap_Leftchild() returns the index of left child of the node with index 
 * <parent_index> in the heap <h>. It returns 0 when the parent node is not in 
 * <h> or it is a leaf.
 */
int Int_Heap_Leftchild(const Int_Arraylist *h, int parent_index);

/**@brief Smaller child of a node
 *
 * Int_Heap_Small_Child() returns the index of the smaller child of the node 
 * with index  <parent_index> in the heap <h>. It returns 0 when the parent 
 * node is not in <h> or it is a leaf.
 */
int Int_Heap_Small_Child(const Int_Arraylist *h, int parent_index);

/**@brief Add a node.
 *
 * Int_Heap_Add() adds a node to the end of the heap. The nodes will be 
 * adjusted automatically so that <h> is still a heap.
 */
void Int_Heap_Add(Int_Arraylist *h, int item);

/*@brief Remove minimal.
 *
 * Int_Heap_Remove() removes the root node in the heap <h> and returns its
 * value. The remained nodes will be adjusted automatically so that <h> is 
 * still a heap.
 */
int Int_Heap_Remove(Int_Arraylist *h);

/**@brief Test is an arraylist is a heap
 *
 * Verify_Int_Heap() returns 0 if <h> is a heap. Otherwise it returns the index
 * where violation happens.
 */
int Verify_Int_Heap(Int_Arraylist *h);



/* The routines with suffix _I are for associated arrays. The values in the 
 * heap are integers, but the structure of the heap is determined by a double
 * array. Briefly speaking, the comparison of the two nodes is not to compare
 * their values directly. Instead, the value of a node is used as an index
 * to access the double array. The values achieved from the double array are
 * actual values for comparison. 
 */

/**@brief Smaller child of a node.
 */
int Int_Heap_Small_Child_I(const Int_Arraylist *h, int parent_index, 
			    double *value);

/**@brief Add a node to a heap.
 *
 * Int_Heap_Add_I() inserts an index to a heap. The values are stored in 
 * <value>. <checked> is an array to store indices of the heap so that we can
 * obtain the position of a node in the heap quickly.
 */
void Int_Heap_Add_I(Int_Arraylist *h, int item, double *value,
		     int *checked);

/**@brief Remove the root node from a heap.
 *
 * Int_Heap_Remove_I() removes the root from <h>. The corresponding element
 * in <checked> is set to 0.
 */
int Int_Heap_Remove_I(Int_Arraylist *h, double *value, int *checked);

/**@brief Remove a specified node from a heap.
 *
 * Int_Heap_Remove_I_At() removes the node that has id <index>. The 
 * corresponding element in <checked> is set to 0.
 */
int Int_Heap_Remove_I_At(Int_Arraylist *h, int index,
			   double *value, int *checked);

/**@brief Update a heap.
 *
 * Int_Heap_Update_I() updates <h> to respond to the change of node that has
 * value <item>, which means <value[item]> might be changed.
 */
void Int_Heap_Update_I(Int_Arraylist *h, int item, double *value,
			int *checked);

void Print_Int_Heap_I(const Int_Arraylist *h, double *value);
int Verify_Int_Heap_I(const Int_Arraylist *h, double *value);


/*
 * Int_Heap_Sort() sorts an array using heap sort alogrithm.
 */
void Int_Heap_Sort(Int_Arraylist *a);



__END_DECLS

#endif
