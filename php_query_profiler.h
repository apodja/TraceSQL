/* query_profiler extension for PHP */

#ifndef PHP_QUERY_PROFILER_H
# define PHP_QUERY_PROFILER_H

extern zend_module_entry query_profiler_module_entry;
# define phpext_query_profiler_ptr &query_profiler_module_entry

# define PHP_QUERY_PROFILER_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_QUERY_PROFILER)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_QUERY_PROFILER_H */
