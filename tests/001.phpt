--TEST--
Check if query_profiler is loaded
--EXTENSIONS--
query_profiler
--FILE--
<?php
echo 'The extension "query_profiler" is available';
?>
--EXPECT--
The extension "query_profiler" is available
