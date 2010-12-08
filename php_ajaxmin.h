/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2010 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Pierre Joye <pierre@php.net>                                |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_AJAXMINIFIER_H
#define PHP_AJAXMINIFIER_H

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include <msclr\auto_gcroot.h>

extern "C" {
#define PHP_AJAXMIN_VERSION "0.1.0"

#pragma warning(push)
#pragma warning( disable: 4561 )

#ifdef ZTS
# include "TSRM.h"
#endif

#include "php.h"
#include "php_globals.h"
#include "ext/standard/php_standard.h"
#include "ext/standard/info.h"

#if PHP_MAJOR_VERSION < 5 || ( PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION < 2 )
# error Trying to compile "ajaxmin Extension" with an unsupported version of PHP
#endif
#if ZEND_DEBUG
// debug build causes warning C4505 to pop up from the Zend header files
#pragma warning( disable: 4505 )
#endif


#define CSSSETTINGS_ME(name, arg_info, flags)	ZEND_FENTRY(name, c_csssettings_ ##name, arg_info, flags)
#define CSSSETTINGS_METHOD(name)	ZEND_NAMED_FUNCTION(c_csssettings_##name)
static zend_class_entry *csssettings_class_entry;
static zend_object_handlers csssettings_object_handlers;
static HashTable csssettings_prop_handlers;


#define CODESETTINGS_ME(name, arg_info, flags)	ZEND_FENTRY(name, c_codesettings_ ##name, arg_info, flags)
#define CODESETTINGS_METHOD(name)	ZEND_NAMED_FUNCTION(c_codesettings_##name)
static zend_class_entry *codesettings_class_entry;
static zend_object_handlers codesettings_object_handlers;
static HashTable codesettings_prop_handlers;


PHP_MINIT_FUNCTION(csssettings);
PHP_MINIT_FUNCTION(codesettings);
extern zend_module_entry ajaxmin_module_entry;

} // extern "C"

bool _zval_to_bool(zval *value);
long _zval_to_long(zval *value);

#endif	/* PHP_AJAXMINIFIER_H */
