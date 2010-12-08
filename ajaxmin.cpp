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

/* $Id$ */
#include "php_ajaxmin.h"
#include "zend_exceptions.h"
#include <msclr\auto_gcroot.h>
#include <vcclr.h>

#using <mscorlib.dll>
#using <System.dll>
#using <AjaxMin.dll>

using namespace System;
using namespace System::Text;
using namespace System::IO;
using namespace Microsoft::Ajax::Utilities;
using namespace msclr;
#include "csssettings.hpp"
#include "codesettings.hpp"

static zend_object_value php_csssettings_object_new(zend_class_entry *class_type TSRMLS_DC);
static void php_csssettings_object_free_storage(void *object TSRMLS_DC);
static HashTable *php_csssettings_get_properties(zval *object TSRMLS_DC);
static void php_csssettings_write_property(zval *object, zval *member, zval *value TSRMLS_DC);
static zval * php_csssettings_read_property(zval *object, zval *member, int type TSRMLS_DC);
static int php_csssettings_has_property(zval *object, zval *member, int has_set_exists TSRMLS_DC);

ZEND_GET_MODULE(ajaxmin)
PHP_MINIT_FUNCTION(ajaxmin);
PHP_MSHUTDOWN_FUNCTION(ajaxmin);
PHP_MINFO_FUNCTION(ajaxmin);
PHP_FUNCTION(ajaxmin_minify_js);
PHP_FUNCTION(ajaxmin_minify_css);

ZEND_BEGIN_ARG_INFO_EX(arginfo_ajaxmin_minify_js, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, settings)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ajaxmin_minify_css, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, settings)
	ZEND_ARG_INFO(0, filename_out)
ZEND_END_ARG_INFO()

zend_function_entry ajaxmin_functions[] = {
	PHP_FE(ajaxmin_minify_js, arginfo_ajaxmin_minify_js)
	PHP_FE(ajaxmin_minify_css, arginfo_ajaxmin_minify_css)
	{NULL, NULL, NULL}
};

zend_module_entry ajaxmin_module_entry = 
{
	STANDARD_MODULE_HEADER,
	"ajaxmin", 
	ajaxmin_functions, // exported function table
	PHP_MINIT(ajaxmin),
	PHP_MSHUTDOWN(ajaxmin), 
	NULL, 
	NULL, 
	PHP_MINFO(ajaxmin),
	PHP_AJAXMIN_VERSION,
	STANDARD_MODULE_PROPERTIES
};


long _zval_to_long(zval *value) /* {{{ */
{
	if (Z_TYPE_P(value) != IS_LONG) {
		zval tmp_member;
		tmp_member = *value;
		zval_copy_ctor(&tmp_member);
		convert_to_long(&tmp_member);
		long val = Z_LVAL(tmp_member);
		zval_dtor(&tmp_member);
		return val;
	} else {
		return Z_LVAL(*value);
	}
}
/* }}} */

bool _zval_to_bool(zval *value) /* {{{ */
{
	if (Z_TYPE_P(value) != IS_BOOL) {
		zval tmp_member;
		tmp_member = *value;
		zval_copy_ctor(&tmp_member);
		convert_to_long(&tmp_member);
		bool val = (Z_LVAL(tmp_member) == 0 ? false : true);
		zval_dtor(&tmp_member);
		return val;
	} else {
		return (Z_LVAL(*value) == 0 ? false : true);
	}
}
/* }}} */

PHP_FUNCTION(ajaxmin_minify_js) /* {{{ */
{
	char *str = NULL;
	int str_len = 0;
	char *filename = NULL;
	int filename_len = 0;
 	zval *codesettings = NULL;
	ze_codesettings_object *obj = NULL;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|O", &str, &str_len, &codesettings, codesettings_class_entry) == FAILURE)
	{
			RETURN_FALSE;
	}

	if (str_len < 1) {
		zend_throw_exception_ex(zend_exception_get_default(TSRMLS_C), 0 TSRMLS_CC, "Invalid image format");
	}
	String^ strString = gcnew String(str, 0, str_len);

	if (codesettings) {
		obj = (ze_codesettings_object *)zend_objects_get_address(codesettings TSRMLS_CC);
	}

	try {
		String^ miniString;
		Minifier ^mini = gcnew Minifier;

		if (codesettings) {
			CodeSettings^ m_settings = obj->settings.get();
			miniString = mini->MinifyJavaScript(strString, m_settings);
		} else {
			miniString = mini->MinifyJavaScript(strString);
		}
		UTF8Encoding^ utf8 = gcnew UTF8Encoding;
		array<Byte>^encodedBytes = utf8->GetBytes(miniString);
		cli::pin_ptr<unsigned char> utf8_char = &encodedBytes[0];

		RETURN_STRING((const char*) utf8_char, 1);
	/*TODO: Check if Serializer actually throws exception not extending the base Exeption class 
	 * Did not manage to throw such ex so far. Have asked the devs.
	 */
	} catch (Exception ^ex) {
		zend_throw_exception_ex(zend_exception_get_default(TSRMLS_C), 0 TSRMLS_CC, "Could not parse input: %s (%s)", ex->GetType()->FullName, ex->Message);
	}

	RETURN_FALSE;
}
/* }}} */

PHP_FUNCTION(ajaxmin_minify_css) /* {{{ */
{
	char *str = NULL;
	int str_len = 0;
	char *filename = NULL;
	int filename_len = 0;
	zval *csssettings = NULL;
	ze_csssettings_object *obj = NULL;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|Os", &str, &str_len, &csssettings, csssettings_class_entry, &filename, &filename_len) == FAILURE)
	{
			RETURN_FALSE;
	}

	if (str_len < 1) {
		zend_throw_exception_ex(zend_exception_get_default(TSRMLS_C), 0 TSRMLS_CC, "Invalid image format");
	}

	String^ strString = gcnew String(str, 0, str_len);

	if (csssettings) {
		obj = (ze_csssettings_object *)zend_objects_get_address(csssettings TSRMLS_CC);
	}

	try {
		String^ miniString;
		Minifier ^mini = gcnew Minifier;

		if (csssettings) {
			CssSettings^ m_settings = obj->settings.get();
			miniString = mini->MinifyStyleSheet(strString, m_settings);
		} else {
			miniString = mini->MinifyStyleSheet(strString);
		}

		UTF8Encoding^ utf8 = gcnew UTF8Encoding;

		array<Byte>^encodedBytes = utf8->GetBytes(miniString);
		cli::pin_ptr<unsigned char> utf8_char = &encodedBytes[0];

		RETURN_STRING((const char*) utf8_char, 1);

	/*TODO: Check if Serializer actually throws exception not extending the base Exeption class 
	 * Did not manage to throw such ex so far. Have asked the devs.
	 */
	} catch (Exception ^ex) {
		zend_throw_exception_ex(zend_exception_get_default(TSRMLS_C), 0 TSRMLS_CC, "Could not parse input: %s (%s)", ex->GetType()->FullName, ex->Message);
	}

	RETURN_FALSE;
}
/* }}} */

PHP_MINIT_FUNCTION(ajaxmin) { /* {{{ */
	PHP_MINIT(csssettings)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(codesettings)(INIT_FUNC_ARGS_PASSTHRU);
	return SUCCESS;
}

static PHP_MINFO_FUNCTION(ajaxmin) /* {{{ */
{
	php_info_print_table_start();
	php_info_print_table_header(2, "Microsoft Ajax Minifier", "enabled");
	php_info_print_table_header(2, "Version", PHP_AJAXMIN_VERSION);
	DISPLAY_INI_ENTRIES();
	php_info_print_table_end();
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
static PHP_MSHUTDOWN_FUNCTION(ajaxmin)
{
	zend_hash_destroy(&csssettings_prop_handlers);
	return SUCCESS;
}
/* }}} */
