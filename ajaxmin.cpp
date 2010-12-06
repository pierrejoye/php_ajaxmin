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

ZEND_BEGIN_ARG_INFO(arginfo_csssettings__void, 0)
ZEND_END_ARG_INFO()

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
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ajaxmin_minify_css, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, options)
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

static zend_class_entry *csssettings_class_entry;
static zend_object_handlers csssettings_object_handlers;
static HashTable csssettings_prop_handlers;

typedef struct _ze_csssettings_object {
	zend_object zo;
	HashTable *prop_handler;
	auto_gcroot<CssSettings^> settings;
} ze_csssettings_object;

//typedef int (*csssettings_getter)(auto_gcroot<CssSettings^> settings, zval **retval);
typedef int (*csssettings_getter)(ze_csssettings_object *obj, zval **retval);
typedef int (*csssettings_setter)(ze_csssettings_object *obj, zval *value);

typedef struct _csssettings_prop_handler {
	csssettings_getter getter;
	csssettings_setter setter;
}csssettings_prop_handler;

static void ajaxmin_register_prop_handler(HashTable *prop_handler, char *name, csssettings_getter getter, csssettings_setter setter) /* {{{ */
{
	csssettings_prop_handler hnd;

	hnd.getter = getter;
	hnd.setter = setter;
	zend_hash_add(prop_handler, name, strlen(name)+1, &hnd, sizeof(csssettings_prop_handler), NULL);
}
/* }}} */

static int csssettings_find_functions(ze_csssettings_object *obj, zval *member, csssettings_prop_handler **hnd) /* {{{ */
{
	return zend_hash_find(obj->prop_handler, Z_STRVAL_P(member), Z_STRLEN_P(member)+1, (void **) hnd);
}
/* }}} */

#define PJ_USE_NEW 1

static void php_csssettings_object_free_storage(void *object TSRMLS_DC) /* {{{ */
{
	ze_csssettings_object * intern = (ze_csssettings_object *) object;

	zend_object_std_dtor(&intern->zo TSRMLS_CC);
#if PJ_USE_NEW
	delete intern;
#else
	efree(intern);
#endif
}
/* }}} */

static zend_object_value php_csssettings_object_new(zend_class_entry *ce TSRMLS_DC) /* {{{ */
{
	ze_csssettings_object *intern;
	zend_object_value retval;

#if PJ_USE_NEW
	intern = new ze_csssettings_object;
	memset(&intern->zo, 0, sizeof(zend_object));
#else
	intern = (ze_csssettings_object *) ecalloc( 1, sizeof(ze_csssettings_object) );
#endif
	intern->prop_handler = &csssettings_prop_handlers;
	
	intern->settings = gcnew CssSettings();
	zend_object_std_init(&intern->zo, ce TSRMLS_CC );

	retval.handle = zend_objects_store_put(
		intern,
		NULL,
		(zend_objects_free_object_storage_t)php_csssettings_object_free_storage,
		NULL TSRMLS_CC );

	//retval.handlers = zend_get_std_object_handlers();
	retval.handlers = &csssettings_object_handlers;
	return retval;
}
/* }}} */

//static int get_IndentSpaces(auto_gcroot<CssSettings^> settings, zval **retval TSRMLS_DC)
static int get_IndentSpaces(ze_csssettings_object *obj, zval **retval TSRMLS_DC)

{
	ALLOC_ZVAL(*retval);
	ZVAL_LONG(*retval, obj->settings->IndentSpaces);
	return SUCCESS;
}

static int set_IndentSpaces(ze_csssettings_object *obj, zval *value TSRMLS_DC)
{
	long val;

	if (Z_TYPE_P(value) != IS_LONG) {
		zval tmp_member;
		tmp_member = *value;
		zval_copy_ctor(&tmp_member);
		convert_to_long(&tmp_member);
		val = Z_LVAL(tmp_member) == 0 ? false : true;
		zval_dtor(&tmp_member);
	} else {
		val = Z_LVAL(*value);
	}

	obj->settings->IndentSpaces = val;
	return SUCCESS;
		return SUCCESS;
}

static int get_MinifyExpressions(ze_csssettings_object *obj, zval **retval TSRMLS_DC)
{
	ALLOC_ZVAL(*retval);
	ZVAL_BOOL(*retval, obj->settings->MinifyExpressions);
	return SUCCESS;
}

static int set_MinifyExpressions(ze_csssettings_object *obj, zval *value TSRMLS_DC)
{
	bool val;

	if (Z_TYPE_P(value) != IS_BOOL) {
		zval tmp_member;
		tmp_member = *value;
		zval_copy_ctor(&tmp_member);
		convert_to_long(&tmp_member);
		val = Z_LVAL(tmp_member) == 0 ? false : true;
		zval_dtor(&tmp_member);
	} else {
		val = Z_LVAL(*value) == 0 ? false : true;
	}

	obj->settings->MinifyExpressions = val;
	return SUCCESS;
}

static HashTable *php_csssettings_get_properties(zval *object TSRMLS_DC) /* {{{ */
{
	HashTable *h;
	return h;
}


/* {{{ xmlreader_write_property */
static void php_csssettings_write_property(zval *object, zval *member, zval *value TSRMLS_DC)
{
	ze_csssettings_object *obj;
	csssettings_prop_handler *hnd;
	zval tmp_member;
 	zend_object_handlers *std_hnd;
	int ret;

 	if (member->type != IS_STRING) {
		tmp_member = *member;
		zval_copy_ctor(&tmp_member);
		convert_to_string(&tmp_member);
		member = &tmp_member;
	}

	ret = FAILURE;
	obj = (ze_csssettings_object *)zend_objects_get_address(object TSRMLS_CC);

	if (obj->prop_handler != NULL) {
		ret = zend_hash_find((HashTable *)obj->prop_handler, Z_STRVAL_P(member), Z_STRLEN_P(member)+1, (void **) &hnd);
		if (ret == SUCCESS) {
			ret = hnd->setter(obj, value);
			if (ret != SUCCESS) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Fail to set property value");
			}
		} else {
			std_hnd = zend_get_std_object_handlers();
			std_hnd->write_property(object, member, value TSRMLS_CC);
		}
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Fail to set property value");
	}

	if (ret == FAILURE) {
		
	}

	if (member == &tmp_member) {
		zval_dtor(member);
	}
}
/* }}} */

static zval * php_csssettings_read_property(zval *object, zval *member, int type TSRMLS_DC)/* {{{ */
{
	ze_csssettings_object *obj;
	zval tmp_member;
	zval *retval;
	csssettings_prop_handler *hnd;

	if (Z_TYPE_P(member) != IS_STRING) {
		tmp_member = *member;
		zval_copy_ctor(&tmp_member);
		convert_to_string(&tmp_member);
		member = &tmp_member;
	}

	obj = (ze_csssettings_object *)zend_objects_get_address(object TSRMLS_CC);

	if (zend_hash_find(obj->prop_handler, Z_STRVAL_P(member), Z_STRLEN_P(member)+1, (void **)&hnd) != SUCCESS) {
		retval = EG(uninitialized_zval_ptr);
	} else {
		if (hnd->getter(obj, &retval) != SUCCESS) {
			retval = EG(uninitialized_zval_ptr);
		}
	}
	if (member == &tmp_member) {
		zval_dtor(member);
	}

	return retval;
}

static int php_csssettings_has_property(zval *object, zval *member, int has_set_exists TSRMLS_DC)/* {{{ */
{
	return 1;
}

/* {{{ proto bool ZipArchive::unchangeArchive()
Revert all global changes to the archive archive.  For now, this only reverts archive comment changes. */
static CSSSETTINGS_METHOD(test)
{
	RETURN_TRUE;
}
/* }}} */

PHP_FUNCTION(ajaxmin_minify_js)
{
	char *str = NULL;
	int str_len = 0;
	char *filename = NULL;
	int filename_len = 0;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE)
	{
			RETURN_FALSE;
	}

	if (str_len < 1) {
		zend_throw_exception_ex(zend_exception_get_default(TSRMLS_C), 0 TSRMLS_CC, "Invalid image format");
	}
	String^ strString = gcnew String(str, 0, str_len);

	Minifier ^mini = gcnew Minifier;

	try {
		String^ miniString;
#if 0 /* TODO: add settings support for fine grained options */
		CodeSettings^ settings = gcnew CodeSettings();
		settings->CollapseToLiteral = true;
		settings->CombineDuplicateLiterals = true;
		settings->InlineSafeStrings = true;
		settings->LocalRenaming = LocalRenaming::CrunchAll;
		settings->MinifyCode = true;
		settings->PreserveFunctionNames = false;
		settings->RemoveUnneededCode = true;


		miniString = mini->MinifyJavaScript(strString, settings);
#endif
		miniString = mini->MinifyJavaScript(strString);

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

PHP_FUNCTION(ajaxmin_minify_css)
{
	char *str = NULL;
	int str_len = 0;
	char *filename = NULL;
	int filename_len = 0;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ls", &str, &str_len, &filename, &filename_len) == FAILURE)
	{
			RETURN_FALSE;
	}

	if (str_len < 1) {
		zend_throw_exception_ex(zend_exception_get_default(TSRMLS_C), 0 TSRMLS_CC, "Invalid image format");
	}
	String^ strString = gcnew String(str, 0, str_len);

	Minifier ^mini = gcnew Minifier;

	try {
		String^ miniString;
		CssSettings^ settings = gcnew CssSettings();

#if 0 /* TODO: add settings support for fine grained options */
		CssSettings^ settings = gcnew CssSettings();
		miniString = mini->MinifyStyleSheet(strString, settings);
#endif

		miniString = mini->MinifyStyleSheet(strString);

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



/* {{{ ze_zip_object_class_functions */
static const zend_function_entry csssettings_methods[] = {
	CSSSETTINGS_ME(test,			arginfo_csssettings__void, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

PHP_MINIT_FUNCTION(ajaxmin) {
	zend_class_entry csssettings_ce;

	memcpy(&csssettings_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	csssettings_object_handlers.clone_obj		= NULL;
	csssettings_object_handlers.get_property_ptr_ptr = NULL;
	csssettings_object_handlers.get_properties = php_csssettings_get_properties;
	csssettings_object_handlers.write_property	= php_csssettings_write_property;
	csssettings_object_handlers.read_property	= php_csssettings_read_property;
	csssettings_object_handlers.has_property	= php_csssettings_has_property;

	INIT_CLASS_ENTRY(csssettings_ce, "CssSettings", csssettings_methods);
	csssettings_ce.create_object = php_csssettings_object_new;
	csssettings_class_entry = zend_register_internal_class(&csssettings_ce TSRMLS_CC);
//	csssettings_ce.ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS | ZEND_ACC_FINAL_CLASS; 

	zend_hash_init(&csssettings_prop_handlers, 0, NULL, NULL, 1);
	ajaxmin_register_prop_handler(&csssettings_prop_handlers, "IndentSpaces", get_IndentSpaces, set_IndentSpaces TSRMLS_CC);
	ajaxmin_register_prop_handler(&csssettings_prop_handlers, "MinifyExpressions", get_MinifyExpressions, set_MinifyExpressions TSRMLS_CC);

	return SUCCESS;
}

static PHP_MINFO_FUNCTION(ajaxmin)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "Microsoft Ajax Minifier", "enabled");
	php_info_print_table_header(2, "Version", PHP_AJAXMIN_VERSION);
	DISPLAY_INI_ENTRIES();
	php_info_print_table_end();
}

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
static PHP_MSHUTDOWN_FUNCTION(ajaxmin)
{
	zend_hash_destroy(&csssettings_prop_handlers);
	return SUCCESS;
}
/* }}} */
/* }}} */

