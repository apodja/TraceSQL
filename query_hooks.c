#include "query_hooks.h"
#include "query_metrics.h"
#include "util.h"
#include <sys/time.h>

static zif_handler original_mysqli_query;
static zif_handler original_pdo_query;

static void override_mysqli_query();
static void override_pdo_query();
query_table_t *query_table = NULL;


PHP_FUNCTION(intercept_mysqli_query)
{
	char *query;
	size_t query_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &query, &query_len) == FAILURE)
	{
		RETURN_FALSE;
	}

	double start = get_time();
	original_mysqli_query(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	double end = get_time();
	
	char *fingerprinted = fingerprint_string(query);
	upsert_query_entry(query_table, fingerprinted, end - start);
}

PHP_FUNCTION(intercept_pdo_query) 
{
    char *query;
    size_t query_len;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "s", &query, &query_len) == FAILURE) {
        RETURN_FALSE;
    }

	double start = get_time();
	original_pdo_query(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	double end = get_time();
	
	char *fingerprinted = fingerprint_string(query);
	upsert_query_entry(query_table, fingerprinted, end - start);
}

static void override_mysqli_query() 
{
	zend_function *original_function;
	zend_class_entry *ce = zend_hash_str_find_ptr(CG(class_table), "mysqli", sizeof("mysqli") - 1);
	if (ce != NULL)
	{
		original_function = zend_hash_str_find_ptr(&ce->function_table, "query", sizeof("query") - 1);

		if (original_function != NULL)
		{	
			original_mysqli_query = original_function->internal_function.handler;
			original_function->internal_function.handler = PHP_FN(intercept_mysqli_query);
		}
	}
}

static void override_pdo_query() 
{
	zend_function *original_function;
	zend_class_entry *ce = zend_hash_str_find_ptr(CG(class_table), "pdo", sizeof("pdo") - 1);

	if (ce != NULL)
	{
		original_function = zend_hash_str_find_ptr(&ce->function_table, "query", sizeof("query") - 1);

		if (original_function != NULL)
		{	
			original_pdo_query = original_function->internal_function.handler;
			original_function->internal_function.handler = PHP_FN(intercept_pdo_query);
		}
	}
}


void register_query_hooks() {
    override_mysqli_query();
    override_pdo_query();
}