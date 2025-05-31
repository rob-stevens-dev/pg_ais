#ifndef PG_AIS_H
#define PG_AIS_H

#include "postgres.h"

typedef struct varlena ais;

// PostgreSQL internal
ais *ais_from_cstring(const char *str);
char *ais_to_cstring(const ais *value);

// CMocka testable
ais *ais_from_cstring_external(const char *str);
char *ais_to_cstring_external(const ais *value);

#endif