/* Minimal stub to unblock builds where the generated header is absent.
   The real project’s build system normally generates tz_voxel_linked_list.h.
   This stub only provides types needed by prototypes elsewhere. */

#ifndef _TZ_VOXEL_LINKED_LIST_H_
#define _TZ_VOXEL_LINKED_LIST_H_

#include "tz_cdefs.h"
#include <stddef.h> /* size_t */

__BEGIN_DECLS

/* Forward-compatible, lightweight definitions.
   If the real header is later present, it can replace this stub cleanly. */

typedef struct _Voxel_Node {
  int x, y, z;
  struct _Voxel_Node* next;
} Voxel_Node;

typedef struct _Voxel_List {
  Voxel_Node* head;
  size_t size;
} Voxel_List;

/* Intentionally no function prototypes here:
   - If code doesn’t call them, the linker won’t care.
   - If you later restore the generated header, you’ll get the real APIs. */

__END_DECLS

#endif /* _TZ_VOXEL_LINKED_LIST_H_ */
