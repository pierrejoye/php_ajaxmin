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

typedef int (*csssettings_getter)(ze_csssettings_object *obj, zval **retval TSRMLS_DC);
typedef int (*csssettings_setter)(ze_csssettings_object *obj, zval *value TSRMLS_DC);

typedef struct _csssettings_prop_handler {
	csssettings_getter getter;
	csssettings_setter setter;
}csssettings_prop_handler;

static void ajaxmin_register_prop_handler(HashTable *prop_handler, char *name, csssettings_getter getter, csssettings_setter setter TSRMLS_DC) /* {{{ */
{
	csssettings_prop_handler hnd;

	hnd.getter = getter;
	hnd.setter = setter;
	zend_hash_add(prop_handler, name, strlen(name)+1, &hnd, sizeof(csssettings_prop_handler), NULL);
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

	retval.handlers = &csssettings_object_handlers;
	return retval;
}
/* }}} */
static int csssettings_get_IndentSpaces(ze_csssettings_object *obj, zval **retval TSRMLS_DC) /* {{{ */
{
	ALLOC_ZVAL(*retval);
	ZVAL_LONG(*retval, obj->settings->IndentSpaces);
	return SUCCESS;
}
/* }}} */

static int csssettings_set_IndentSpaces(ze_csssettings_object *obj, zval *value TSRMLS_DC) /* {{{ */
{
	obj->settings->IndentSpaces = _zval_to_long(value);
	return SUCCESS;
}
/* }}} */

static int csssettings_get_MinifyExpressions(ze_csssettings_object *obj, zval **retval TSRMLS_DC) /* {{{ */
{
	ALLOC_ZVAL(*retval);
	ZVAL_BOOL(*retval, obj->settings->MinifyExpressions);
	return SUCCESS;
}
/* }}} */

static int csssettings_set_MinifyExpressions(ze_csssettings_object *obj, zval *value TSRMLS_DC) /* {{{ */
{
	obj->settings->MinifyExpressions = _zval_to_bool(value);
	return SUCCESS;
}
/* }}} */

static int csssettings_get_TermSemicolons(ze_csssettings_object *obj, zval **retval TSRMLS_DC) /* {{{ */
{
	ALLOC_ZVAL(*retval);
	ZVAL_BOOL(*retval, obj->settings->TermSemicolons);
	return SUCCESS;
}
/* }}} */

static int csssettings_set_TermSemicolons(ze_csssettings_object *obj, zval *value TSRMLS_DC) /* {{{ */
{
	obj->settings->TermSemicolons = _zval_to_bool(value);
	return SUCCESS;
}
/* }}} */

static int csssettings_get_ExpandOutput(ze_csssettings_object *obj, zval **retval TSRMLS_DC) /* {{{ */
{
	ALLOC_ZVAL(*retval);
	ZVAL_BOOL(*retval, obj->settings->ExpandOutput);
	return SUCCESS;
}
/* }}} */

static int csssettings_set_ExpandOutput(ze_csssettings_object *obj, zval *value TSRMLS_DC) /* {{{ */
{
	obj->settings->ExpandOutput = _zval_to_bool(value);
	return SUCCESS;
}
/* }}} */

static int csssettings_get_CommentMode(ze_csssettings_object *obj, zval **retval TSRMLS_DC) /* {{{ */
{
	ALLOC_ZVAL(*retval);
	ZVAL_LONG(*retval, (long)obj->settings->CommentMode);
	return SUCCESS;
}
/* }}} */

static int csssettings_set_CommentMode(ze_csssettings_object *obj, zval *value TSRMLS_DC) /* {{{ */
{
	long val =_zval_to_long(value);
	if (val < 0 || val > 3) {
		return FAILURE;
	}
	obj->settings->CommentMode = (CssComment)_zval_to_long(value);
	return SUCCESS;
}
/* }}} */

static int csssettings_get_ColorNames(ze_csssettings_object *obj, zval **retval TSRMLS_DC) /* {{{ */
{
	ALLOC_ZVAL(*retval);
	ZVAL_LONG(*retval, (long)obj->settings->ColorNames);
	return SUCCESS;
}
/* }}} */

static int csssettings_set_ColorNames(ze_csssettings_object *obj, zval *value TSRMLS_DC) /* {{{ */
{
	long val =_zval_to_long(value);
	if (val < 0 || val > 2) {
		return FAILURE;
	}
	obj->settings->ColorNames = (CssColor)_zval_to_long(value);
	return SUCCESS;
}
/* }}} */

static HashTable *php_csssettings_get_properties(zval *object TSRMLS_DC) /* {{{ */
{
	HashTable *h;
	return h;
}
/* }}} */

static void php_csssettings_write_property(zval *object, zval *member, zval *value TSRMLS_DC) /* {{{ */
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
			ret = hnd->setter(obj, value TSRMLS_CC);
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

	if (member == &tmp_member) {
		zval_dtor(member);
	}
}
/* }}} */

static zval * php_csssettings_read_property(zval *object, zval *member, int type TSRMLS_DC) /* {{{ */
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
		if (hnd->getter(obj, &retval TSRMLS_CC) != SUCCESS) {
			retval = EG(uninitialized_zval_ptr);
		}
	}
	if (member == &tmp_member) {
		zval_dtor(member);
	}

	return retval;
}
/* }}} */

static int php_csssettings_has_property(zval *object, zval *member, int has_set_exists TSRMLS_DC) /* {{{ */
{
	return 1;
}
/* }}} */

/* {{{ proto bool ZipArchive::unchangeArchive()
Revert all global changes to the archive archive.  For now, this only reverts archive comment changes. */
static CSSSETTINGS_METHOD(test) /* {{{ */
{
	RETURN_TRUE;
}
/* }}} */

/* {{{ ze_zip_object_class_functions */
static const zend_function_entry csssettings_methods[] = {
	CSSSETTINGS_ME(test,			arginfo_csssettings__void, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ REGISTER_ZIP_CLASS_CONST_LONG */
#define REGISTER_CSSSETTINGS_CLASS_CONST_LONG(const_name, value) \
	    zend_declare_class_constant_long(csssettings_class_entry, const_name, sizeof(const_name)-1, (long)value TSRMLS_CC);
/* }}} */

PHP_MINIT_FUNCTION(csssettings) { /* {{{ */
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

	zend_hash_init(&csssettings_prop_handlers, 0, NULL, NULL, 1);
	ajaxmin_register_prop_handler(&csssettings_prop_handlers, "IndentSpaces", csssettings_get_IndentSpaces, csssettings_set_IndentSpaces TSRMLS_CC);
	ajaxmin_register_prop_handler(&csssettings_prop_handlers, "MinifyExpressions", csssettings_get_MinifyExpressions, csssettings_set_MinifyExpressions TSRMLS_CC);
	ajaxmin_register_prop_handler(&csssettings_prop_handlers, "TermSemicolons", csssettings_get_TermSemicolons, csssettings_set_TermSemicolons TSRMLS_CC);
	ajaxmin_register_prop_handler(&csssettings_prop_handlers, "ExpandOutput", csssettings_get_ExpandOutput, csssettings_set_ExpandOutput TSRMLS_CC);
	ajaxmin_register_prop_handler(&csssettings_prop_handlers, "CommentMode", csssettings_get_CommentMode, csssettings_set_CommentMode TSRMLS_CC);
	ajaxmin_register_prop_handler(&csssettings_prop_handlers, "ColorNames", csssettings_get_ColorNames, csssettings_set_ColorNames TSRMLS_CC);

	REGISTER_CSSSETTINGS_CLASS_CONST_LONG("COLOR_STRICT", 0);
	REGISTER_CSSSETTINGS_CLASS_CONST_LONG("COLOR_HEX", 1);
	REGISTER_CSSSETTINGS_CLASS_CONST_LONG("COLOR_MAJOR", 2);

	REGISTER_CSSSETTINGS_CLASS_CONST_LONG("COMMENT_IMPORTANT", 0);
	REGISTER_CSSSETTINGS_CLASS_CONST_LONG("COMMENT_NONE", 1);
	REGISTER_CSSSETTINGS_CLASS_CONST_LONG("COMMENT_ALL", 2);
	REGISTER_CSSSETTINGS_CLASS_CONST_LONG("COMMENT_HACK", 3);
	return SUCCESS;
}
