#ifndef PHP_RLITE_H
#define PHP_RLITE_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"

#define PHP_RLITE_VERSION "0.0.1.0"
#define PHP_RLITE_EXTNAME "rlite"

PHP_MINIT_FUNCTION(rlite);
PHP_FUNCTION(rlite_connect);
PHP_FUNCTION(rlite_disconnect);
PHP_FUNCTION(rlite_command);

extern zend_module_entry rlite_module_entry;
#define phpext_rlite_ptr &rlite_module_entry

#endif
