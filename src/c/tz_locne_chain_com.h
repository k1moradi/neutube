/**@file tz_locne_chain_com.h
 * @brief chain of Local_Neuroseg_Ellipse
 * @author Ting Zhao
 * @date 23-Oct-2008
 */

/* 1T Locseg 2T Local_Neuroseg */

#ifndef _TZ_Locne_CHAIN_COM_H_
#define _TZ_Locne_CHAIN_COM_H_

#include "tz_trace_defs.h"
#include "tz_locne_node_doubly_linked_list.h"
#include "tz_int_arraylist.h"
#include "tz_neuron_structure.h"
#include "tz_geo3d_circle.h"
#include "tz_vrml_defs.h"
#include "tz_stack_draw.h"

__BEGIN_DECLS

/**@addtogroup locne_chain Locne chain
 *
 * @{
 */

/* note for possible future revision: list should hold all the data */
/**@struct _Locne_Chain tz_locne_chain.h
 *
 * @brief data structure of neuron chain
 */
typedef struct _Locne_Chain {
  Locne_Node_Dlist *list;     /**< where the data are stored */
  Locne_Node_Dlist *iterator; /**< be careful about nesting.*/
} Locne_Chain;


/**@addtogroup locne_chain_obj general neurochain object routines
 *
 * @{
 */
Locne_Chain* New_Locne_Chain();
Locne_Chain* Free_Locne_Chain();

void Delete_Locne_Chain(Locne_Chain *chain);

void Construct_Locne_Chain(Locne_Chain *chain, Locne_Node *p);
void Clean_Locne_Chain(Locne_Chain *chain);

Locne_Chain* Make_Locne_Chain(Locne_Node *p);
void Kill_Locne_Chain(Locne_Chain *chain);

/**@}
 */

/**@addtogroup locne_chain_io_ I/O routines for locne chain
 *
 * @{
 */
void Print_Locne_Chain(Locne_Chain *chain);
void Write_Locne_Chain(const char *file_path, Locne_Chain *chain);
Locne_Chain* Read_Locne_Chain(const char *file_path);

/*@brief Load chain files under a directory
 *
 * Dir_Locne_Chain_N() reads all files with a certain pattern under 
 * \a dir_name. The pattern is specified by the regular expression \a pattern,
 * i.e. any name that matches \a pattern in terms of regular expression will be
 * read. The results are stored in the returned array, in which each element is
 * a chain from a file. The number of chains is stored in \a n and the 
 * corresponding file numbers is stored in \a file_num. Set \a file_num to NULL
 * if you want to ignore it.
 */
Locne_Chain* Dir_Locne_Chain_N(const char *dir_name, const char *pattern,
				 int *n, int *file_num);

/*@brief Load chain files under a directory
 *
 * Dir_Locne_Chain_Nd() is similar to Dir_Locne_Chain_N, except that the return 
 * value is a an array of chain pointers.
 */
Locne_Chain** Dir_Locne_Chain_Nd(const char *dir_name, const char *pattern,
			       int *n, int *file_num);

/**@}*/

/**@addtogroup locne_chain_attr_ Chain attributes
 *
 * @{
 */

/*
 * Locne_Chain_Length() return the number of nodes of <chain>.
 */
int Locne_Chain_Length(Locne_Chain *chain);
double Locne_Chain_Geolen(Locne_Chain *chain);

BOOL Locne_Chain_Is_Empty(Locne_Chain *chain);

/**@}*/

/**@addtogroup locne_chain_trace_ Trace routines
 * @{
 */

/**@brief Initialize a chain for tracing.
 *
 * Locne_Chain_Trace_Init() retruns a chain for tracing. The starting point
 * is <locne>, which will be fit to <stack> after initialization. <fs> is used
 * to store fitting score.
 *
 * Note: <locne> will become a part of the returned chain.
 */
Locne_Chain* Locne_Chain_Trace_Init(const Stack *stack, double z_scale, 
				  Local_Neuroseg_Ellipse *locne, void *ws);

/**@brief Test tracing status.
 *
 * Test if tracing should stop.
 * Return code: See tz_trace_defs.h for more details.
 */
int Locne_Chain_Trace_Test(void *argv[]);

Trace_Workspace* 
Locne_Chain_Default_Trace_Workspace(Trace_Workspace *tw, 
				   const Stack *stack);

/*
 * Trace neuron. 
 * <tw->dyvar> has the following meaning:
 *    tw->dyvar[0] : max radius
 *    tw->dyvar[1] : min radius

 */
void Trace_Locne(const Stack *stack, double z_scale, Locne_Chain *chain, 
		Trace_Workspace *tw);

/*
 * Locne_Chain_Trace_Np() 
 */
void Locne_Chain_Trace_Np(const Stack *stack, double z_scale, 
			 Locne_Chain *chain, Trace_Workspace *tw);

/**@}*/

/**@addtogroup locne_chain_iter_ Traversing routines
 * @{
 */
/**@brief Start the iterator.
 *
 * Locne_Chain_Iterator_Locate() moves the iterator of <chain> to one of the end
 * positions of <chain>. If <d> is DL_HEAD, it moves to the head. If <d> is 
 * DL_TAIL, it moves to the tail. It returns the old iterator.
 */
Locne_Node_Dlist* Locne_Chain_Iterator_Start(Locne_Chain *chain, 
					       Dlist_End_e d);

/**@brief Locate the iterator.
 *
 * Locne_Chain_Iterator_Locate() moves the iterator of <chain> to the position
 * <index>. It returns the old iterator.
 */
Locne_Node_Dlist* Locne_Chain_Iterator_Locate(Locne_Chain *chain, 
					    int index);
/*
 * Locne_Chain_Next() makes the iterator forward by 1. It returns the node
 * before forwarding.
 */
Locne_Node* Locne_Chain_Next(Locne_Chain *chain);
Locne_Node* Locne_Chain_Prev(Locne_Chain *chain);
Locne_Node* Locne_Chain_Next_D(Locne_Chain *chain, Dlist_End_e d);

Local_Neuroseg_Ellipse* Locne_Chain_Next_Seg(Locne_Chain *chain);
Local_Neuroseg_Ellipse* Locne_Chain_Prev_Seg(Locne_Chain *chain);
Local_Neuroseg_Ellipse* Locne_Chain_Next_Seg_D(Locne_Chain *chain, Dlist_End_e d);

Locne_Node* Locne_Chain_Peek(const Locne_Chain *chain);
Local_Neuroseg_Ellipse* Locne_Chain_Peek_Seg(const Locne_Chain *chain);

Locne_Node* Locne_Chain_Peek_Next(const Locne_Chain *chain);
Locne_Node* Locne_Chain_Peek_Prev(const Locne_Chain *chain);


BOOL Locne_Chain_Has_More(const Locne_Chain *chain);

Locne_Node* Locne_Chain_Head(Locne_Chain *chain);
Locne_Node* Locne_Chain_Tail(Locne_Chain *chain);

Local_Neuroseg_Ellipse* Locne_Chain_Head_Seg(Locne_Chain *chain);
Local_Neuroseg_Ellipse* Locne_Chain_Tail_Seg(Locne_Chain *chain);

Locne_Node* Locne_Chain_Peek_At(Locne_Chain *chain, int index);
Local_Neuroseg_Ellipse* Locne_Chain_Peek_Seg_At(Locne_Chain *chain, int index);

/**@}*/


/**@addtogroup locne_chain_opr_ Chain manipulation
 * @{
 */

/*
 * Locne_Chain_Add() adds a node to the end of the chain.
 * Locne_Chain_Cat() appends <chain2> to <chain1>. Notice that the nodes in
 * chain2 are appended in <chain1> by reference, so use Free_Locne_Chain()
 * only to free memory allocated to chain2.
 */
void Locne_Chain_Add(Locne_Chain *chain, Local_Neuroseg_Ellipse *locne, 
		      Trace_Record *tr, Dlist_End_e end);
void Locne_Chain_Add_Node(Locne_Chain *chain, Locne_Node *p, Dlist_End_e end);
void Locne_Chain_Cat(Locne_Chain *chain1, const Locne_Chain *chain2);

void Locne_Chain_Insert(Locne_Chain *chain, Local_Neuroseg_Ellipse *locne, 
		       Trace_Record *tr, int index);

/**@brief Remove current node.
 *
 * Locne_Chain_Remove_Current() removes the node at the current position of the
 * iterator of <chain>. The iterator is moved to the next node.
 */
void Locne_Chain_Remove_Current(Locne_Chain *chain);

/**@brief Take current node.
 *
 * Locne_Chain_Take_Current() removes the node at the current position of the
 * iterator of <chain> and returns it. The iterator is moved to the next node.
 */
Locne_Node* Locne_Chain_Take_Current(Locne_Chain *chain);

void Locne_Chain_Remove_End(Locne_Chain *chain, Dlist_End_e end);
int Locne_Chain_Remove_Overlap_Ends(Locne_Chain *chain);
void Locne_Chain_Remove_Turn_Ends(Locne_Chain *chain, double max_angle);
/**@}*/

void Locne_Chain_Label(Locne_Chain *chain, Stack *stack, double z_scale);
void Locne_Chain_Erase(Locne_Chain *chain, Stack *stack, double z_scale);

void Locne_Chain_Label_E(Stack *stack, Locne_Chain *chain, double z_scale,
			  int begin, int end, double ratio, double diff);
void Locne_Chain_Erase_E(Stack *stack, Locne_Chain *chain, double z_scale,
			  int begin, int end, double ratio, double diff);
			
void Locne_Chain_Label_G(Locne_Chain *chain, Stack *stack, double z_scale,
			int begin, int end, double ratio, double diff, 
			int flag, int value);

void Set_Locne_Trace_Record(Local_Neuroseg_Ellipse *locne, const Trace_Record *tr);

/**@brief Connection test for two locne chains.
 *
 * Locne_Chain_Connection_Test() tests if two chains are connected in \a stack.
 * It returns TRUE if the two chains are connected. More information is stored 
 * in \a conn, which specifies which mode is tested too. When the connection 
 * mode is hook-loop mode (set \a conn->mode to NEUROCOMP_CONN_HL), it tests if 
 * \a chain1 hooks to \a chain2. \a conn->info[0] stores the position of hook 
 * of \a chain1 (0 for head and 1 for tail). \a conn->info[1] stores the 
 * position of loop of \a chain2 and its value is the index of the hooked 
 * segment in \a chain2. The strenth of connection is \a conn->strength. 
 * \a ws is the workspace, which should be NULL in the current version.
 */
BOOL Locne_Chain_Connection_Test(Locne_Chain *chain1, Locne_Chain *chain2,
				const Stack *stack, double z_scale, 
				Neurocomp_Conn *conn, 
				Connection_Test_Workspace *ws);

int Locne_Chain_Hit_Test(Locne_Chain *chain, Dlist_Direction_e d,
			  double x, double y, double z);
int Locne_Chain_Hit_Test_Seg(Locne_Chain *chain,
			    const Local_Neuroseg_Ellipse *locne);

BOOL Locne_Chain_Touched(Locne_Chain *chain1, Locne_Chain *chain2);

Geo3d_Circle* Locne_Chain_To_Geo3d_Circle_Array(Locne_Chain *chain,
						 Geo3d_Circle *circle, int *n);
void Locne_Chain_Vrml_Fprint(FILE *fp, Locne_Chain *chain,
			    const Vrml_Material *material, int indent);
void Locne_Chain_To_Vrml_File(Locne_Chain *chain, const char *file_path);


Graph* Locne_Chain_Graph(Locne_Chain *chain, int n, Int_Arraylist *hit_spots);

void Neuron_Structure_Set_Locne_Chain_Array(Neuron_Structure *ns,
					   Locne_Chain *chain_array);

/**@brief Reconstruct neuron structure.
 *
 * Locne_Chain_Neurostruct() reconstructs neuron structure from \a chain and
 * returns the result. \a chain will be owned by \a Neuron_Structure.
 */
Neuron_Structure* Locne_Chain_Neurostruct(Locne_Chain *chain, int n, 
					   const Stack *stack, double z_scale,
					   void *ws);
 
int Locne_Chain_End_Points(Locne_Chain *chain, int n, coordinate_3d_t pts[]);
void Locne_Chain_Hit_Stack(Locne_Chain *chain, Stack *region, int hit[]);

int Locne_Chain_Swc_Fprint(FILE *fp, Locne_Chain *chain, int type,
  		          int start_id,
			  int parent_id, Dlist_Direction_e direction);

/*
 * Locne_Chain_Reach_Conn() changes one of the ends of \a chain1 so that it
 * can touch \a chain2. How it reaches \a chain2 is defined in \a conn.
 */
void Locne_Chain_Reach_Conn(Locne_Chain *hook, Locne_Chain *loop,
			     const Neurocomp_Conn *conn);

void Locne_Chain_Draw_Stack(Locne_Chain *chain, Stack *stack, 
			   const Stack_Draw_Workspace *ws);

/**@}*/

__END_DECLS

#endif
