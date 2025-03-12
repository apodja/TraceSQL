#ifndef QUERY_METRICS_H
#define QUERY_METRICS_H

#define INITIAL_BUCKET_SIZE 4
#define MAX_LOAD_FACTOR 0.7
#define MIN_LOAD_FACTOR 0.3

typedef struct {
    char *query_str;
    int occurrs;
    double exec_time;
    double min_exec_time;
    double max_exec_time;
} query_metrics_t;

typedef struct query_entry_t {
    unsigned int key;
    query_metrics_t *value;
    struct query_entry_t *next;
} query_entry_t;

typedef struct {
    query_entry_t **buckets;
    int size;
    int count;
} query_table_t;

query_table_t* init_query_table();
query_entry_t* get_query_entry(query_table_t *table, char *query);
void upsert_query_entry(query_table_t *table, char *query, double exec_time);
void free_query_table(query_table_t *table);
void free_chain(query_entry_t *entry);
void print_query_table(query_table_t *table);

#endif