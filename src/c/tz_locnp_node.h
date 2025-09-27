/**@file tz_locnp_node.h
 * @brief node for Local_Neuroseg_Plane chain
 * @author Ting Zhao
 * @date 23-Oct-2008
 */

#ifndef _TZ_Locnp_NODE_H_
#define _TZ_Locnp_NODE_H_

#include "tz_cdefs.h"
#include "tz_local_neuroseg_plane.h"
#include "tz_trace_defs.h"

/* 1T Locseg 2T Local_Neuroseg */

__BEGIN_DECLS

/**@struct _Locnp_Node tz_locnp_node.h
 *
 * @{
 */
typedef struct _Locnp_Node {
  Local_Neuroseg_Plane *locseg; /**< local neuron segment */
  Trace_Record *tr;       /**< trace record */
} Locnp_Node;

/**@}*/

typedef Locnp_Node* Locnp_Node_P;

Locnp_Node* New_Locnp_Node();
void Delete_Locnp_Node(Locnp_Node *p);

Locnp_Node* Make_Locnp_Node(const Local_Neuroseg_Plane *locseg,
			      const Trace_Record *tr);
void Kill_Locnp_Node(Locnp_Node *p);

void Clean_Locnp_Node(Locnp_Node *p);

Locnp_Node* Copy_Locnp_Node(const Locnp_Node *p);

BOOL Locnp_Node_Is_Identical(const Locnp_Node *p1, const Locnp_Node *p2);

void Print_Locnp_Node(const Locnp_Node *p);
void Print_Locnp_Node_Info(const Locnp_Node *p);

void Locnp_Node_Fwrite(const Locnp_Node *p, FILE *stream);
Locnp_Node* Locnp_Node_Fread(Locnp_Node *p, FILE *stream);

void Locnp_Node_Fwrite_V(const Locnp_Node *p, FILE *stream, double version);
Locnp_Node* Locnp_Node_Fread_V(Locnp_Node *p, FILE *stream, double version);

void Locnp_Node_Copy_Trace_Record(Locnp_Node *p, const Trace_Record *tr);

void Locnp_Node_Set_Score(Locnp_Node *p, const Stack_Fit_Score *fs);
void Locnp_Node_Set_Hit_Region(Locnp_Node *p, int hit_region);
void Locnp_Node_Set_Index(Locnp_Node *p, int index);
void Locnp_Node_Set_Refit(Locnp_Node *p, BOOL is_refit);
void Locnp_Node_Set_Direction(Locnp_Node *p, Dlist_Direction_e direction);
void Locnp_Node_Set_Fit_Height(Locnp_Node *p, int index, int value);
void Locnp_Node_Set_Fix_Point(Locnp_Node *p, double value);

BOOL Locnp_Node_Fix_Pos(Locnp_Node *p, double *pos);

__END_DECLS

#endif
