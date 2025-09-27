/**@file tz_locrect_node.h
 * @brief node for Local_R2_Rect chain
 * @author Ting Zhao
 * @date 23-Oct-2008
 */

#ifndef _TZ_Locrect_NODE_H_
#define _TZ_Locrect_NODE_H_

#include "tz_cdefs.h"
#include "tz_local_r2_rect.h"
#include "tz_trace_defs.h"

/* 1T Locseg 2T Local_Neuroseg */

__BEGIN_DECLS

/**@struct _Locrect_Node tz_locrect_node.h
 *
 * @{
 */
typedef struct _Locrect_Node {
  Local_R2_Rect *locseg; /**< local neuron segment */
  Trace_Record *tr;       /**< trace record */
} Locrect_Node;

/**@}*/

typedef Locrect_Node* Locrect_Node_P;

Locrect_Node* New_Locrect_Node();
void Delete_Locrect_Node(Locrect_Node *p);

Locrect_Node* Make_Locrect_Node(const Local_R2_Rect *locseg,
			      const Trace_Record *tr);
void Kill_Locrect_Node(Locrect_Node *p);

void Clean_Locrect_Node(Locrect_Node *p);

Locrect_Node* Copy_Locrect_Node(const Locrect_Node *p);

BOOL Locrect_Node_Is_Identical(const Locrect_Node *p1, const Locrect_Node *p2);

void Print_Locrect_Node(const Locrect_Node *p);
void Print_Locrect_Node_Info(const Locrect_Node *p);

void Locrect_Node_Fwrite(const Locrect_Node *p, FILE *stream);
Locrect_Node* Locrect_Node_Fread(Locrect_Node *p, FILE *stream);

void Locrect_Node_Fwrite_V(const Locrect_Node *p, FILE *stream, double version);
Locrect_Node* Locrect_Node_Fread_V(Locrect_Node *p, FILE *stream, double version);

void Locrect_Node_Copy_Trace_Record(Locrect_Node *p, const Trace_Record *tr);

void Locrect_Node_Set_Score(Locrect_Node *p, const Stack_Fit_Score *fs);
void Locrect_Node_Set_Hit_Region(Locrect_Node *p, int hit_region);
void Locrect_Node_Set_Index(Locrect_Node *p, int index);
void Locrect_Node_Set_Refit(Locrect_Node *p, BOOL is_refit);
void Locrect_Node_Set_Direction(Locrect_Node *p, Dlist_Direction_e direction);
void Locrect_Node_Set_Fit_Height(Locrect_Node *p, int index, int value);
void Locrect_Node_Set_Fix_Point(Locrect_Node *p, double value);

BOOL Locrect_Node_Fix_Pos(Locrect_Node *p, double *pos);

__END_DECLS

#endif
