/**@file tz_locne_node.h
 * @brief node for Local_Neuroseg_Ellipse chain
 * @author Ting Zhao
 * @date 23-Oct-2008
 */

#ifndef _TZ_Locne_NODE_H_
#define _TZ_Locne_NODE_H_

#include "tz_cdefs.h"
#include "tz_local_neuroseg_ellipse.h"
#include "tz_trace_defs.h"

/* 1T Locseg 2T Local_Neuroseg */

__BEGIN_DECLS

/**@struct _Locne_Node tz_locne_node.h
 *
 * @{
 */
typedef struct _Locne_Node {
  Local_Neuroseg_Ellipse *locseg; /**< local neuron segment */
  Trace_Record *tr;       /**< trace record */
} Locne_Node;

/**@}*/

typedef Locne_Node* Locne_Node_P;

Locne_Node* New_Locne_Node();
void Delete_Locne_Node(Locne_Node *p);

Locne_Node* Make_Locne_Node(const Local_Neuroseg_Ellipse *locseg,
			      const Trace_Record *tr);
void Kill_Locne_Node(Locne_Node *p);

void Clean_Locne_Node(Locne_Node *p);

Locne_Node* Copy_Locne_Node(const Locne_Node *p);

BOOL Locne_Node_Is_Identical(const Locne_Node *p1, const Locne_Node *p2);

void Print_Locne_Node(const Locne_Node *p);
void Print_Locne_Node_Info(const Locne_Node *p);

void Locne_Node_Fwrite(const Locne_Node *p, FILE *stream);
Locne_Node* Locne_Node_Fread(Locne_Node *p, FILE *stream);

void Locne_Node_Fwrite_V(const Locne_Node *p, FILE *stream, double version);
Locne_Node* Locne_Node_Fread_V(Locne_Node *p, FILE *stream, double version);

void Locne_Node_Copy_Trace_Record(Locne_Node *p, const Trace_Record *tr);

void Locne_Node_Set_Score(Locne_Node *p, const Stack_Fit_Score *fs);
void Locne_Node_Set_Hit_Region(Locne_Node *p, int hit_region);
void Locne_Node_Set_Index(Locne_Node *p, int index);
void Locne_Node_Set_Refit(Locne_Node *p, BOOL is_refit);
void Locne_Node_Set_Direction(Locne_Node *p, Dlist_Direction_e direction);
void Locne_Node_Set_Fit_Height(Locne_Node *p, int index, int value);
void Locne_Node_Set_Fix_Point(Locne_Node *p, double value);

BOOL Locne_Node_Fix_Pos(Locne_Node *p, double *pos);

__END_DECLS

#endif
