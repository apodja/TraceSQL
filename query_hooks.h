#ifndef QUERY_HOOKS
#define QUERY_HOOKS

#include "php.h"
#include "query_metrics.h"

extern query_table_t *query_table;

void register_query_hooks();

#endif