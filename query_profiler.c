/* query_profiler extension for PHP */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_query_profiler.h"
#include "query_profiler_arginfo.h"
#include "query_hooks.h"

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

PHP_FUNCTION(test1)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_printf("The extension %s is loaded and working!\r\n", "query_profiler");
}


PHP_RINIT_FUNCTION(query_profiler)
{
#if defined(ZTS) && defined(COMPILE_DL_QUERY_PROFILER)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	return SUCCESS;
}

PHP_MINFO_FUNCTION(query_profiler)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "query_profiler support", "enabled");
	php_info_print_table_end();
}

PHP_MSHUTDOWN_FUNCTION(query_profiler)
{
	if (query_table)
	{
		php_printf("printing table from shutdown\n");
		print_query_table(query_table);
		free_query_table(query_table);
	}
    
    return SUCCESS;
}

PHP_MINIT_FUNCTION(query_profiler)
{
	query_table = init_query_table();
	register_query_hooks();
	return SUCCESS;
}

zend_module_entry query_profiler_module_entry = {
	STANDARD_MODULE_HEADER,
	"query_profiler",				/* Extension name */
	ext_functions,					/* zend_function_entry */
	PHP_MINIT(query_profiler),		/* PHP_MINIT - Module initialization */
	PHP_MSHUTDOWN(query_profiler),	/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(query_profiler),		/* PHP_RINIT - Request initialization */
	NULL,							/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(query_profiler),		/* PHP_MINFO - Module info */
	PHP_QUERY_PROFILER_VERSION,		/* Version */
	STANDARD_MODULE_PROPERTIES
};



#ifdef COMPILE_DL_QUERY_PROFILER
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(query_profiler)
#endif
