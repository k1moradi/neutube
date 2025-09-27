/**@file tz_locnp_chain_com.h
 * @brief chain of Local_Neuroseg_Plane
 * @author Ting Zhao
 * @date 23-Oct-2008
 */

/* 1T Locseg 2T Local_Neuroseg */

#ifndef _TZ_Locnp_CHAIN_COM_H_
#define _TZ_Locnp_CHAIN_COM_H_

#include "tz_trace_defs.h"
#include "tz_locnp_node_doubly_linked_list.h"
#include "tz_int_arraylist.h"
#include "tz_neuron_structure.h"
#include "tz_geo3d_circle.h"
#include "tz_vrml_defs.h"
#include "tz_stack_draw.h"

__BEGIN_DECLS

/**@addtogroup locnp_chain Locnp chain
 *
 * @{
 */

/* note for possible future revision: list should hold all the data */
/**@struct _Locnp_Chain tz_locnp_chain.h
 *
 * @brief data structure of neuron chain
 */
typedef struct _Locnp_Chain {
  Locnp_Node_Dlist *list;     /**< where the data are stored */
  Locnp_Node_Dlist *iterator; /**< be careful about nesting.*/
} Locnp_Chain;


/**@addtogroup locnp_chain_obj general neurochain object routines
 *
 * @{
 */
Locnp_Chain* New_Locnp_Chain();
Locnp_Chain* Free_Locnp_Chain();

void Delete_Locnp_Chain(Locnp_Chain *chain);

void Construct_Locnp_Chain(Locnp_Chain *chain, Locnp_Node *p);
void Clean_Locnp_Chain(Locnp_Chain *chain);

Locnp_Chain* Make_Locnp_Chain(Locnp_Node *p);
void Kill_Locnp_Chain(Locnp_Chain *chain);

/**@}
 */

/**@addtogroup locnp_chain_io_ I/O routines for locnp chain
 *
 * @{
 */
void Print_Locnp_Chain(Locnp_Chain *chain);
void Write_Locnp_Chain(const char *file_path, Locnp_Chain *chain);
Locnp_Chain* Read_Locnp_Chain(const char *file_path);

/*@brief Load chain files under a directory
 *
 * Dir_Locnp_Chain_N() reads all files with a certain pattern under 
 * \a dir_name. The pattern is specified by the regular expression \a pattern,
 * i.e. any name that matches \a pattern in terms of regular expression will be
 * read. The results are stored in the returned array, in which each element is
 * a chain from a file. The number of chains is stored in \a n and the 
 * corresponding file numbers is stored in \a file_num. Set \a file_num to NULL
 * if you want to ignore it.
 */
Locnp_Chain* Dir_Locnp_Chain_N(const char *dir_name, const char *pattern,
				 int *n, int *file_num);

/*@brief Load chain files under a directory
 *
 * Dir_Locnp_Chain_Nd() is similar to Dir_Locnp_Chain_N, except that the return 
 * value is a an array of chain pointers.
 */
Locnp_Chain** Dir_Locnp_Chain_Nd(const char *dir_name, const char *pattern,
			       int *n, int *file_num);

/**@}*/

/**@addtogroup locnp_chain_attr_ Chain attributes
 *
 * @{
 */

/*
 * Locnp_Chain_Length() return the number of nodes of <chain>.
 */
int Locnp_Chain_Length(Locnp_Chain *chain);
double Locnp_Chain_Geolen(Locnp_Chain *chain);

BOOL Locnp_Chain_Is_Empty(Locnp_Chain *chain);

/**@}*/

/**@addtogroup locnp_chain_trace_ Trace routines
 * @{
 */

/**@brief Initialize a chain for tracing.
 *
 * Locnp_Chain_Trace_Init() retruns a chain for tracing. The starting point
 * is <locnp>, which will be fit to <stack> after initialization. <fs> is used
 * to store fitting score.
 *
 * Note: <locnp> will become a part of the returned chain.
 */
Locnp_Chain* Locnp_Chain_Trace_Init(const Stack *stack, double z_scale, 
				  Local_Neuroseg_Plane *locnp, void *ws);

/**@brief Test tracing status.
 *
 * Test if tracing should stop.
 * Return code: See tz_trace_defs.h for more details.
 */
int Locnp_Chain_Trace_Test(void *argv[]);

Trace_Workspace* 
Locnp_Chain_Default_Trace_Workspace(Trace_Workspace *tw, 
				   const Stack *stack);

/*
 * Trace neuron. 
 * <tw->dyvar> has the following meaning:
 *    tw->dyvar[0] : max radius
 *    tw->dyvar[1] : min radius

 */
void Trace_Locnp(const Stack *stack, double z_scale, Locnp_Chain *chain, 
		Trace_Workspace *tw);

/*
 * Locnp_Chain_Trace_Np() 
 */
void Locnp_Chain_Trace_Np(const Stack *stack, double z_scale, 
			 Locnp_Chain *chain, Trace_Workspace *tw);

/**@}*/

/**@addtogroup locnp_chain_iter_ Traversing routines
 * @{
 */
/**@brief Start the iterator.
 *
 * Locnp_Chain_Iterator_Locate() moves the iterator of <chain> to one of the end
 * positions of <chain>. If <d> is DL_HEAD, it moves to the head. If <d> is 
 * DL_TAIL, it moves to the tail. It returns the old iterator.
 */
Locnp_Node_Dlist* Locnp_Chain_Iterator_Start(Locnp_Chain *chain, 
					       Dlist_End_e d);

/**@brief Locate the iterator.
 *
 * Locnp_Chain_Iterator_Locate() moves the iterator of <chain> to the position
 * <index>. It returns the old iterator.
 */
Locnp_Node_Dlist* Locnp_Chain_Iterator_Locate(Locnp_Chain *chain, 
					    int index);
/*
 * Locnp_Chain_Next() makes the iterator forward by 1. It returns the node
 * before forwarding.
 */
Locnp_Node* Locnp_Chain_Next(Locnp_Chain *chain);
Locnp_Node* Locnp_Chain_Prev(Locnp_Chain *chain);
Locnp_Node* Locnp_Chain_Next_D(Locnp_Chain *chain, Dlist_End_e d);

Local_Neuroseg_Plane* Locnp_Chain_Next_Seg(Locnp_Chain *chain);
Local_Neuroseg_Plane* Locnp_Chain_Prev_Seg(Locnp_Chain *chain);
Local_Neuroseg_Plane* Locnp_Chain_Next_Seg_D(Locnp_Chain *chain, Dlist_End_e d);

Locnp_Node* Locnp_Chain_Peek(const Locnp_Chain *chain);
Local_Neuroseg_Plane* Locnp_Chain_Peek_Seg(const Locnp_Chain *chain);

Locnp_Node* Locnp_Chain_Peek_Next(const Locnp_Chain *chain);
Locnp_Node* Locnp_Chain_Peek_Prev(const Locnp_Chain *chain);


BOOL Locnp_Chain_Has_More(const Locnp_Chain *chain);

Locnp_Node* Locnp_Chain_Head(Locnp_Chain *chain);
Locnp_Node* Locnp_Chain_Tail(Locnp_Chain *chain);

Local_Neuroseg_Plane* Locnp_Chain_Head_Seg(Locnp_Chain *chain);
Local_Neuroseg_Plane* Locnp_Chain_Tail_Seg(Locnp_Chain *chain);

Locnp_Node* Locnp_Chain_Peek_At(Locnp_Chain *chain, int index);
Local_Neuroseg_Plane* Locnp_Chain_Peek_Seg_At(Locnp_Chain *chain, int index);

/**@}*/


/**@addtogroup locnp_chain_opr_ Chain manipulation
 * @{
 */

/*
 * Locnp_Chain_Add() adds a node to the end of the chain.
 * Locnp_Chain_Cat() appends <chain2> to <chain1>. Notice that the nodes in
 * chain2 are appended in <chain1> by reference, so use Free_Locnp_Chain()
 * only to free memory allocated to chain2.
 */
void Locnp_Chain_Add(Locnp_Chain *chain, Local_Neuroseg_Plane *locnp, 
		      Trace_Record *tr, Dlist_End_e end);
void Locnp_Chain_Add_Node(Locnp_Chain *chain, Locnp_Node *p, Dlist_End_e end);
void Locnp_Chain_Cat(Locnp_Chain *chain1, const Locnp_Chain *chain2);

void Locnp_Chain_Insert(Locnp_Chain *chain, Local_Neuroseg_Plane *locnp, 
		       Trace_Record *tr, int index);

/**@brief Remove current node.
 *
 * Locnp_Chain_Remove_Current() removes the node at the current position of the
 * iterator of <chain>. The iterator is moved to the next node.
 */
void Locnp_Chain_Remove_Current(Locnp_Chain *chain);

/**@brief Take current node.
 *
 * Locnp_Chain_Take_Current() removes the node at the current position of the
 * iterator of <chain> and returns it. The iterator is moved to the next node.
 */
Locnp_Node* Locnp_Chain_Take_Current(Locnp_Chain *chain);

void Locnp_Chain_Remove_End(Locnp_Chain *chain, Dlist_End_e end);
int Locnp_Chain_Remove_Overlap_Ends(Locnp_Chain *chain);
void Locnp_Chain_Remove_Turn_Ends(Locnp_Chain *chain, double max_angle);
/**@}*/

void Locnp_Chain_Label(Locnp_Chain *chain, Stack *stack, double z_scale);
void Locnp_Chain_Erase(Locnp_Chain *chain, Stack *stack, double z_scale);

void Locnp_Chain_Label_E(Stack *stack, Locnp_Chain *chain, double z_scale,
			  int begin, int end, double ratio, double diff);
void Locnp_Chain_Erase_E(Stack *stack, Locnp_Chain *chain, double z_scale,
			  int begin, int end, double ratio, double diff);
			
void Locnp_Chain_Label_G(Locnp_Chain *chain, Stack *stack, double z_scale,
			int begin, int end, double ratio, double diff, 
			int flag, int value);

void Set_Locnp_Trace_Record(Local_Neuroseg_Plane *locnp, const Trace_Record *tr);

/**@brief Connection test for two locnp chains.
 *
 * Locnp_Chain_Connection_Test() tests if two chains are connected in \a stack.
 * It returns TRUE if the two chains are connected. More information is stored 
 * in \a conn, which specifies which mode is tested too. When the connection 
 * mode is hook-loop mode (set \a conn->mode to NEUROCOMP_CONN_HL), it tests if 
 * \a chain1 hooks to \a chain2. \a conn->info[0] stores the position of hook 
 * of \a chain1 (0 for head and 1 for tail). \a conn->info[1] stores the 
 * position of loop of \a chain2 and its value is the index of the hooked 
 * segment in \a chain2. The strenth of connection is \a conn->strength. 
 * \a ws is the workspace, which should be NULL in the current version.
 */
BOOL Locnp_Chain_Connection_Test(Locnp_Chain *chain1, Locnp_Chain *chain2,
				const Stack *stack, double z_scale, 
				Neurocomp_Conn *conn, 
				Connection_Test_Workspace *ws);

int Locnp_Chain_Hit_Test(Locnp_Chain *chain, Dlist_Direction_e d,
			  double x, double y, double z);
int Locnp_Chain_Hit_Test_Seg(Locnp_Chain *chain,
			    const Local_Neuroseg_Plane *locnp);

BOOL Locnp_Chain_Touched(Locnp_Chain *chain1, Locnp_Chain *chain2);

Geo3d_Circle* Locnp_Chain_To_Geo3d_Circle_Array(Locnp_Chain *chain,
						 Geo3d_Circle *circle, int *n);
void Locnp_Chain_Vrml_Fprint(FILE *fp, Locnp_Chain *chain,
			    const Vrml_Material *material, int indent);
void Locnp_Chain_To_Vrml_File(Locnp_Chain *chain, const char *file_path);


Graph* Locnp_Chain_Graph(Locnp_Chain *chain, int n, Int_Arraylist *hit_spots);

void Neuron_Structure_Set_Locnp_Chain_Array(Neuron_Structure *ns,
					   Locnp_Chain *chain_array);

/**@brief Reconstruct neuron structure.
 *
 * Locnp_Chain_Neurostruct() reconstructs neuron structure from \a chain and
 * returns the result. \a chain will be owned by \a Neuron_Structure.
 */
Neuron_Structure* Locnp_Chain_Neurostruct(Locnp_Chain *chain, int n, 
					   const Stack *stack, double z_scale,
					   void *ws);
 
int Locnp_Chain_End_Points(Locnp_Chain *chain, int n, coordinate_3d_t pts[]);
void Locnp_Chain_Hit_Stack(Locnp_Chain *chain, Stack *region, int hit[]);

int Locnp_Chain_Swc_Fprint(FILE *fp, Locnp_Chain *chain, int type,
  		          int start_id,
			  int parent_id, Dlist_Direction_e direction);

/*
 * Locnp_Chain_Reach_Conn() changes one of the ends of \a chain1 so that it
 * can touch \a chain2. How it reaches \a chain2 is defined in \a conn.
 */
void Locnp_Chain_Reach_Conn(Locnp_Chain *hook, Locnp_Chain *loop,
			     const Neurocomp_Conn *conn);

void Locnp_Chain_Draw_Stack(Locnp_Chain *chain, Stack *stack, 
			   const Stack_Draw_Workspace *ws);

/**@}*/

__END_DECLS

#endif
