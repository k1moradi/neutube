/**@file tz_locseg_node.h
 * @brief node for Local_Neuroseg chain
 * @author Ting Zhao
 * @date 23-Oct-2008
 */

#ifndef _TZ_Locseg_NODE_H_
#define _TZ_Locseg_NODE_H_

#include "tz_cdefs.h"
#include "tz_local_neuroseg.h"
#include "tz_trace_defs.h"

/* 1T Locseg 2T Local_Neuroseg */

__BEGIN_DECLS

/**@struct _Locseg_Node tz_locseg_node.h
 *
 * @{
 */
typedef struct _Locseg_Node {
  Local_Neuroseg *locseg; /**< local neuron segment */
  Trace_Record *tr;       /**< trace record */
} Locseg_Node;

/**@}*/

typedef Locseg_Node* Locseg_Node_P;

Locseg_Node* New_Locseg_Node();
void Delete_Locseg_Node(Locseg_Node *p);

Locseg_Node* Make_Locseg_Node(const Local_Neuroseg *locseg,
			      const Trace_Record *tr);
void Kill_Locseg_Node(Locseg_Node *p);

void Clean_Locseg_Node(Locseg_Node *p);

Locseg_Node* Copy_Locseg_Node(const Locseg_Node *p);

BOOL Locseg_Node_Is_Identical(const Locseg_Node *p1, const Locseg_Node *p2);

void Print_Locseg_Node(const Locseg_Node *p);
void Print_Locseg_Node_Info(const Locseg_Node *p);

void Locseg_Node_Fwrite(const Locseg_Node *p, FILE *stream);
Locseg_Node* Locseg_Node_Fread(Locseg_Node *p, FILE *stream);

void Locseg_Node_Fwrite_V(const Locseg_Node *p, FILE *stream, double version);
Locseg_Node* Locseg_Node_Fread_V(Locseg_Node *p, FILE *stream, double version);

void Locseg_Node_Copy_Trace_Record(Locseg_Node *p, const Trace_Record *tr);

void Locseg_Node_Set_Score(Locseg_Node *p, const Stack_Fit_Score *fs);
void Locseg_Node_Set_Hit_Region(Locseg_Node *p, int hit_region);
void Locseg_Node_Set_Index(Locseg_Node *p, int index);
void Locseg_Node_Set_Refit(Locseg_Node *p, BOOL is_refit);
void Locseg_Node_Set_Direction(Locseg_Node *p, Dlist_Direction_e direction);
void Locseg_Node_Set_Fit_Height(Locseg_Node *p, int index, int value);
void Locseg_Node_Set_Fix_Point(Locseg_Node *p, double value);

BOOL Locseg_Node_Fix_Pos(Locseg_Node *p, double *pos);

__END_DECLS

#endif
