#include "query_metrics.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static unsigned int hash(char *str, int num_buckets) {
    unsigned long hash = 5381;
    int c;
    while((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash % num_buckets;
}

static void htable_resize(query_table_t *table, size_t new_size) {
    // Allocate new buckets
    query_entry_t **new_buckets = malloc(new_size * sizeof(query_entry_t *));
    if (!new_buckets) {
        return;
    }

    memset(new_buckets, 0, new_size * sizeof(query_entry_t *));

    for (size_t i = 0; i < table->size; i++) {
        query_entry_t *entry = table->buckets[i];

        while (entry) {
            query_entry_t *next = entry->next;

            size_t new_index = hash(entry->value->query_str, new_size);

            entry->next = new_buckets[new_index];  
            new_buckets[new_index] = entry;

            entry = next;
        }
    }

    free(table->buckets);

    table->buckets = new_buckets;
    table->size = new_size;
}

query_table_t* init_query_table() {
    query_table_t *table = malloc(sizeof(query_table_t));

    if (!table)
    {
        return NULL;
    }

    table->count = 0;
    table->size = INITIAL_BUCKET_SIZE;
    table->buckets = calloc(table->size, sizeof(query_entry_t*));

    if (!table->buckets)
    {
        free(table);
        return NULL;
    }
    
    return table;
}

query_entry_t* get_query_entry(query_table_t *table, char *query) {
    unsigned int index = hash(query, table->size);
    query_entry_t *entry = table->buckets[index];

    while (entry)
    {
        if (strcmp(query, entry->value->query_str) == 0)
        {
            return entry;
        }
        entry = entry->next;
    }
    
    return NULL;
}

void upsert_query_entry(query_table_t *table, char *query, double exec_time) {
    query_entry_t *entry = get_query_entry(table, query);
    if (entry)
    {
        entry->value->occurrs++;
        entry->value->exec_time += exec_time;
        if (exec_time > entry->value->max_exec_time)
        {
            entry->value->max_exec_time = exec_time;
        }

        if (exec_time < entry->value->min_exec_time)
        {
            entry->value->min_exec_time = exec_time;
        }
        
        return;
    }

    if (table->count >= table->size * MAX_LOAD_FACTOR)
    {
        htable_resize(table, table->size * 2);
    }

    unsigned int index = hash(query, table->size);
    entry = table->buckets[index];
    
    query_entry_t *new_entry = malloc(sizeof(query_entry_t));
    if (!new_entry)
    {
        return;
    }
    
    new_entry->key = index;
    new_entry->value = malloc(sizeof(query_metrics_t));
    if (!new_entry->value)
    {
        free(new_entry);
        return;
    }
    
    new_entry->value->query_str = query;
    new_entry->value->occurrs = 1;
    new_entry->value->exec_time = exec_time;
    new_entry->value->max_exec_time = exec_time;
    new_entry->value->min_exec_time = exec_time;
    new_entry->next = table->buckets[index];
    table->buckets[index] = new_entry;
    table->count++;

    if (table->count < table->size * MIN_LOAD_FACTOR && table->size > INITIAL_BUCKET_SIZE)
    {
        htable_resize(table, table->size / 2);
    }
}

void free_query_table(query_table_t *table) {
    for (int i = 0; i < table->count; i++)
    {
        free_chain(table->buckets[i]);
    }

    free(table);
}

void free_chain(query_entry_t *entry) {
    while (entry)
    {
        query_entry_t *next = entry->next;
        free(entry->value->query_str);
        free(entry->value);
        free(entry);
        entry = next;
    }
}

void print_formatted_table(query_table_t *table)
{
    printf("\n");
    printf("+-----------------------------------------------------------------------------------------------------------------+\n");
    printf("| %-40s | %-8s | %-12s | %-12s | %-12s | %-12s |\n", "Query", "Count", "Total Time", "Avg Time", "Min Time", "Max Time");
    printf("+-----------------------------------------------------------------------------------------------------------------+\n");

    for (int i = 0; i < table->size; i++)
    {
        query_entry_t *entry = table->buckets[i];

        while (entry) {
            printf("| %-40s | %-8d | %-12.6f | %-12.6f | %-12.6f | %-12.6f |\n",
                    entry->value->query_str,
                    entry->value->occurrs,
                    entry->value->exec_time,
                    entry->value->exec_time / entry->value->occurrs,
                    entry->value->min_exec_time,
                    entry->value->max_exec_time);
            entry = entry->next;
        }
    }

    printf("+-----------------------------------------------------------------------------------------------------------------+\n");
}
