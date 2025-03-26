/* query_profiler extension for PHP */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_query_profiler.h"
#include "query_profiler_arginfo.h"
#include "query_hooks.h"

ZEND_BEGIN_MODULE_GLOBALS(query_profiler)
	zend_bool profile_request;
ZEND_END_MODULE_GLOBALS(query_profiler)

ZEND_DECLARE_MODULE_GLOBALS(query_profiler)

#ifdef ZTS
#   define QUERY_PROFILER_G(v) TSRMG(puery_profiler_globals_id, zend_query_profiler_globals *, v)
#else
#   define QUERY_PROFILER_G(v) (query_profiler_globals.v)
#endif

PHP_INI_BEGIN()
	STD_PHP_INI_BOOLEAN(
		"query_profiler.enable", "0", PHP_INI_ALL, OnUpdateBool, 
		profile_request, zend_query_profiler_globals, query_profiler_globals)
PHP_INI_END()

static zend_bool is_profiling_enabled() {
	if (QUERY_PROFILER_G(profile_request))
	{
		return 1;
	}

	const char *env_profiler = getenv("PROFILE_REQUEST");
	return env_profiler && strcmp(env_profiler, "1") == 0;	
}


PHP_RINIT_FUNCTION(query_profiler)
{
#if defined(ZTS) && defined(COMPILE_DL_QUERY_PROFILER)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	if (is_profiling_enabled()) {
		query_table = init_query_table();
		register_query_hooks();
	} 

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
	UNREGISTER_INI_ENTRIES();
    
    return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(query_profiler)
{
	if (query_table)
	{
		print_formatted_table(query_table);
		free_query_table(query_table);
	}
    
    return SUCCESS;
}

PHP_MINIT_FUNCTION(query_profiler)
{
    REGISTER_INI_ENTRIES();

    return SUCCESS;
}

zend_module_entry query_profiler_module_entry = {
	STANDARD_MODULE_HEADER,
	"query_profiler",				/* Extension name */
	NULL,							/* zend_function_entry */
	PHP_MINIT(query_profiler),		/* PHP_MINIT - Module initialization */
	PHP_MSHUTDOWN(query_profiler),	/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(query_profiler),		/* PHP_RINIT - Request initialization */
	PHP_RSHUTDOWN(query_profiler),							/* PHP_RSHUTDOWN - Request shutdown */
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
