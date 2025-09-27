/**@file tz_locseg_chain_com.h
 * @brief chain of Local_Neuroseg
 * @author Ting Zhao
 * @date 23-Oct-2008
 */

/* 1T Locseg 2T Local_Neuroseg */

#ifndef _TZ_Locseg_CHAIN_COM_H_
#define _TZ_Locseg_CHAIN_COM_H_

#include "tz_trace_defs.h"
#include "tz_locseg_node_doubly_linked_list.h"
#include "tz_int_arraylist.h"
#include "tz_neuron_structure.h"
#include "tz_geo3d_circle.h"
#include "tz_vrml_defs.h"
#include "tz_stack_draw.h"

__BEGIN_DECLS

/**@addtogroup locseg_chain Locseg chain
 *
 * @{
 */

/* note for possible future revision: list should hold all the data */
/**@struct _Locseg_Chain tz_locseg_chain.h
 *
 * @brief data structure of neuron chain
 */
typedef struct _Locseg_Chain {
  Locseg_Node_Dlist *list;     /**< where the data are stored */
  Locseg_Node_Dlist *iterator; /**< be careful about nesting.*/
} Locseg_Chain;


/**@addtogroup locseg_chain_obj general neurochain object routines
 *
 * @{
 */
Locseg_Chain* New_Locseg_Chain();
Locseg_Chain* Free_Locseg_Chain();

void Delete_Locseg_Chain(Locseg_Chain *chain);

void Construct_Locseg_Chain(Locseg_Chain *chain, Locseg_Node *p);
void Clean_Locseg_Chain(Locseg_Chain *chain);

Locseg_Chain* Make_Locseg_Chain(Locseg_Node *p);
void Kill_Locseg_Chain(Locseg_Chain *chain);

/**@}
 */

/**@addtogroup locseg_chain_io_ I/O routines for locseg chain
 *
 * @{
 */
void Print_Locseg_Chain(Locseg_Chain *chain);
void Write_Locseg_Chain(const char *file_path, Locseg_Chain *chain);
Locseg_Chain* Read_Locseg_Chain(const char *file_path);

/*@brief Load chain files under a directory
 *
 * Dir_Locseg_Chain_N() reads all files with a certain pattern under 
 * \a dir_name. The pattern is specified by the regular expression \a pattern,
 * i.e. any name that matches \a pattern in terms of regular expression will be
 * read. The results are stored in the returned array, in which each element is
 * a chain from a file. The number of chains is stored in \a n and the 
 * corresponding file numbers is stored in \a file_num. Set \a file_num to NULL
 * if you want to ignore it.
 */
Locseg_Chain* Dir_Locseg_Chain_N(const char *dir_name, const char *pattern,
				 int *n, int *file_num);

/*@brief Load chain files under a directory
 *
 * Dir_Locseg_Chain_Nd() is similar to Dir_Locseg_Chain_N, except that the return 
 * value is a an array of chain pointers.
 */
Locseg_Chain** Dir_Locseg_Chain_Nd(const char *dir_name, const char *pattern,
			       int *n, int *file_num);

/**@}*/

/**@addtogroup locseg_chain_attr_ Chain attributes
 *
 * @{
 */

/*
 * Locseg_Chain_Length() return the number of nodes of <chain>.
 */
int Locseg_Chain_Length(Locseg_Chain *chain);
double Locseg_Chain_Geolen(Locseg_Chain *chain);

BOOL Locseg_Chain_Is_Empty(Locseg_Chain *chain);

/**@}*/

/**@addtogroup locseg_chain_trace_ Trace routines
 * @{
 */

/**@brief Initialize a chain for tracing.
 *
 * Locseg_Chain_Trace_Init() retruns a chain for tracing. The starting point
 * is <locseg>, which will be fit to <stack> after initialization. <fs> is used
 * to store fitting score.
 *
 * Note: <locseg> will become a part of the returned chain.
 */
Locseg_Chain* Locseg_Chain_Trace_Init(const Stack *stack, double z_scale, 
				  Local_Neuroseg *locseg, void *ws);

/**@brief Test tracing status.
 *
 * Test if tracing should stop.
 * Return code: See tz_trace_defs.h for more details.
 */
int Locseg_Chain_Trace_Test(void *argv[]);

Trace_Workspace* 
Locseg_Chain_Default_Trace_Workspace(Trace_Workspace *tw, 
				   const Stack *stack);

/*
 * Trace neuron. 
 * <tw->dyvar> has the following meaning:
 *    tw->dyvar[0] : max radius
 *    tw->dyvar[1] : min radius

 *    tw->dyvar[2] : first-level score 

 */
void Trace_Locseg(const Stack *stack, double z_scale, Locseg_Chain *chain, 
		Trace_Workspace *tw);

/*
 * Locseg_Chain_Trace_Np() 
 */
void Locseg_Chain_Trace_Np(const Stack *stack, double z_scale, 
			 Locseg_Chain *chain, Trace_Workspace *tw);

/**@}*/

/**@addtogroup locseg_chain_iter_ Traversing routines
 * @{
 */
/**@brief Start the iterator.
 *
 * Locseg_Chain_Iterator_Locate() moves the iterator of <chain> to one of the end
 * positions of <chain>. If <d> is DL_HEAD, it moves to the head. If <d> is 
 * DL_TAIL, it moves to the tail. It returns the old iterator.
 */
Locseg_Node_Dlist* Locseg_Chain_Iterator_Start(Locseg_Chain *chain, 
					       Dlist_End_e d);

/**@brief Locate the iterator.
 *
 * Locseg_Chain_Iterator_Locate() moves the iterator of <chain> to the position
 * <index>. It returns the old iterator.
 */
Locseg_Node_Dlist* Locseg_Chain_Iterator_Locate(Locseg_Chain *chain, 
					    int index);
/*
 * Locseg_Chain_Next() makes the iterator forward by 1. It returns the node
 * before forwarding.
 */
Locseg_Node* Locseg_Chain_Next(Locseg_Chain *chain);
Locseg_Node* Locseg_Chain_Prev(Locseg_Chain *chain);
Locseg_Node* Locseg_Chain_Next_D(Locseg_Chain *chain, Dlist_End_e d);

Local_Neuroseg* Locseg_Chain_Next_Seg(Locseg_Chain *chain);
Local_Neuroseg* Locseg_Chain_Prev_Seg(Locseg_Chain *chain);
Local_Neuroseg* Locseg_Chain_Next_Seg_D(Locseg_Chain *chain, Dlist_End_e d);

Locseg_Node* Locseg_Chain_Peek(const Locseg_Chain *chain);
Local_Neuroseg* Locseg_Chain_Peek_Seg(const Locseg_Chain *chain);

Locseg_Node* Locseg_Chain_Peek_Next(const Locseg_Chain *chain);
Locseg_Node* Locseg_Chain_Peek_Prev(const Locseg_Chain *chain);


BOOL Locseg_Chain_Has_More(const Locseg_Chain *chain);

Locseg_Node* Locseg_Chain_Head(Locseg_Chain *chain);
Locseg_Node* Locseg_Chain_Tail(Locseg_Chain *chain);

Local_Neuroseg* Locseg_Chain_Head_Seg(Locseg_Chain *chain);
Local_Neuroseg* Locseg_Chain_Tail_Seg(Locseg_Chain *chain);

Locseg_Node* Locseg_Chain_Peek_At(Locseg_Chain *chain, int index);
Local_Neuroseg* Locseg_Chain_Peek_Seg_At(Locseg_Chain *chain, int index);

/**@}*/


/**@addtogroup locseg_chain_opr_ Chain manipulation
 * @{
 */

/*
 * Locseg_Chain_Add() adds a node to the end of the chain.
 * Locseg_Chain_Cat() appends <chain2> to <chain1>. Notice that the nodes in
 * chain2 are appended in <chain1> by reference, so use Free_Locseg_Chain()
 * only to free memory allocated to chain2.
 */
void Locseg_Chain_Add(Locseg_Chain *chain, Local_Neuroseg *locseg, 
		      Trace_Record *tr, Dlist_End_e end);
void Locseg_Chain_Add_Node(Locseg_Chain *chain, Locseg_Node *p, Dlist_End_e end);
void Locseg_Chain_Cat(Locseg_Chain *chain1, const Locseg_Chain *chain2);

void Locseg_Chain_Insert(Locseg_Chain *chain, Local_Neuroseg *locseg, 
		       Trace_Record *tr, int index);

/**@brief Remove current node.
 *
 * Locseg_Chain_Remove_Current() removes the node at the current position of the
 * iterator of <chain>. The iterator is moved to the next node.
 */
void Locseg_Chain_Remove_Current(Locseg_Chain *chain);

/**@brief Take current node.
 *
 * Locseg_Chain_Take_Current() removes the node at the current position of the
 * iterator of <chain> and returns it. The iterator is moved to the next node.
 */
Locseg_Node* Locseg_Chain_Take_Current(Locseg_Chain *chain);

void Locseg_Chain_Remove_End(Locseg_Chain *chain, Dlist_End_e end);
int Locseg_Chain_Remove_Overlap_Ends(Locseg_Chain *chain);
void Locseg_Chain_Remove_Turn_Ends(Locseg_Chain *chain, double max_angle);
/**@}*/

void Locseg_Chain_Label(Locseg_Chain *chain, Stack *stack, double z_scale);
void Locseg_Chain_Erase(Locseg_Chain *chain, Stack *stack, double z_scale);

void Locseg_Chain_Label_E(Stack *stack, Locseg_Chain *chain, double z_scale,
			  int begin, int end, double ratio, double diff);
void Locseg_Chain_Erase_E(Stack *stack, Locseg_Chain *chain, double z_scale,
			  int begin, int end, double ratio, double diff);
			
void Locseg_Chain_Label_G(Locseg_Chain *chain, Stack *stack, double z_scale,
			int begin, int end, double ratio, double diff, 
			int flag, int value);

void Set_Locseg_Trace_Record(Local_Neuroseg *locseg, const Trace_Record *tr);

/**@brief Connection test for two locseg chains.
 *
 * Locseg_Chain_Connection_Test() tests if two chains are connected in \a stack.
 * It returns TRUE if the two chains are connected. More information is stored 
 * in \a conn, which specifies which mode is tested too. When the connection 
 * mode is hook-loop mode (set \a conn->mode to NEUROCOMP_CONN_HL), it tests if 
 * \a chain1 hooks to \a chain2. \a conn->info[0] stores the position of hook 
 * of \a chain1 (0 for head and 1 for tail). \a conn->info[1] stores the 
 * position of loop of \a chain2 and its value is the index of the hooked 
 * segment in \a chain2. The strenth of connection is \a conn->strength. 
 * \a ws is the workspace, which should be NULL in the current version.
 */
BOOL Locseg_Chain_Connection_Test(Locseg_Chain *chain1, Locseg_Chain *chain2,
				const Stack *stack, double z_scale, 
				Neurocomp_Conn *conn, 
				Connection_Test_Workspace *ws);

int Locseg_Chain_Hit_Test(Locseg_Chain *chain, Dlist_Direction_e d,
			  double x, double y, double z);
int Locseg_Chain_Hit_Test_Seg(Locseg_Chain *chain,
			    const Local_Neuroseg *locseg);

BOOL Locseg_Chain_Touched(Locseg_Chain *chain1, Locseg_Chain *chain2);

Geo3d_Circle* Locseg_Chain_To_Geo3d_Circle_Array(Locseg_Chain *chain,
						 Geo3d_Circle *circle, int *n);
void Locseg_Chain_Vrml_Fprint(FILE *fp, Locseg_Chain *chain,
			    const Vrml_Material *material, int indent);
void Locseg_Chain_To_Vrml_File(Locseg_Chain *chain, const char *file_path);


Graph* Locseg_Chain_Graph(Locseg_Chain *chain, int n, Int_Arraylist *hit_spots);

void Neuron_Structure_Set_Locseg_Chain_Array(Neuron_Structure *ns,
					   Locseg_Chain *chain_array);

/**@brief Reconstruct neuron structure.
 *
 * Locseg_Chain_Neurostruct() reconstructs neuron structure from \a chain and
 * returns the result. \a chain will be owned by \a Neuron_Structure.
 */
Neuron_Structure* Locseg_Chain_Neurostruct(Locseg_Chain *chain, int n, 
					   const Stack *stack, double z_scale,
					   void *ws);
 
int Locseg_Chain_End_Points(Locseg_Chain *chain, int n, coordinate_3d_t pts[]);
void Locseg_Chain_Hit_Stack(Locseg_Chain *chain, Stack *region, int hit[]);

int Locseg_Chain_Swc_Fprint(FILE *fp, Locseg_Chain *chain, int type,
  		          int start_id,
			  int parent_id, Dlist_Direction_e direction);

/*
 * Locseg_Chain_Reach_Conn() changes one of the ends of \a chain1 so that it
 * can touch \a chain2. How it reaches \a chain2 is defined in \a conn.
 */
void Locseg_Chain_Reach_Conn(Locseg_Chain *hook, Locseg_Chain *loop,
			     const Neurocomp_Conn *conn);

void Locseg_Chain_Draw_Stack(Locseg_Chain *chain, Stack *stack, 
			   const Stack_Draw_Workspace *ws);

/**@}*/

__END_DECLS

#endif
