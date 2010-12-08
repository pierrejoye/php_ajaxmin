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

#include "codesettings.hpp"

ZEND_BEGIN_ARG_INFO(arginfo_codesettings__void, 0)
ZEND_END_ARG_INFO()

typedef int (*codesettings_getter)(ze_codesettings_object *obj, zval **retval TSRMLS_DC);
typedef int (*codesettings_setter)(ze_codesettings_object *obj, zval *value TSRMLS_DC);

typedef struct _codesettings_prop_handler {
	codesettings_getter getter;
	codesettings_setter setter;
}codesettings_prop_handler;

static void ajaxmin_register_prop_handler(HashTable *prop_handler, char *name, codesettings_getter getter, codesettings_setter setter TSRMLS_DC) /* {{{ */
{
	codesettings_prop_handler hnd;

	hnd.getter = getter;
	hnd.setter = setter;
	zend_hash_add(prop_handler, name, strlen(name)+1, &hnd, sizeof(codesettings_prop_handler), NULL);
}
/* }}} */

#define PJ_USE_NEW 1

static void php_codesettings_object_free_storage(void *object TSRMLS_DC) /* {{{ */
{
	ze_codesettings_object * intern = (ze_codesettings_object *) object;

	zend_object_std_dtor(&intern->zo TSRMLS_CC);
#if PJ_USE_NEW
	delete intern;
#else
	efree(intern);
#endif
}
/* }}} */

static zend_object_value php_codesettings_object_new(zend_class_entry *ce TSRMLS_DC) /* {{{ */
{
	ze_codesettings_object *intern;
	zend_object_value retval;

#if PJ_USE_NEW
	intern = new ze_codesettings_object;
	memset(&intern->zo, 0, sizeof(zend_object));
#else
	intern = (ze_codesettings_object *) ecalloc( 1, sizeof(ze_codesettings_object) );
#endif
	intern->prop_handler = &codesettings_prop_handlers;
	
	intern->settings = gcnew CodeSettings();
	zend_object_std_init(&intern->zo, ce TSRMLS_CC );

	retval.handle = zend_objects_store_put(
		intern,
		NULL,
		(zend_objects_free_object_storage_t)php_codesettings_object_free_storage,
		NULL TSRMLS_CC );

	retval.handlers = &codesettings_object_handlers;
	return retval;
}
/* }}} */

static int codesettings_get_CollapseToLiteral(ze_codesettings_object *obj, zval **retval TSRMLS_DC) /* {{{ */
{
	ALLOC_ZVAL(*retval);
	ZVAL_BOOL(*retval, obj->settings->CollapseToLiteral);
	return SUCCESS;
}
/* }}} */

static int codesettings_set_CollapseToLiteral(ze_codesettings_object *obj, zval *value TSRMLS_DC) /* {{{ */
{
	obj->settings->CollapseToLiteral = _zval_to_bool(value);
	return SUCCESS;
}
/* }}} */

static int codesettings_get_CombineDuplicateLiterals(ze_codesettings_object *obj, zval **retval TSRMLS_DC) /* {{{ */
{
	ALLOC_ZVAL(*retval);
	ZVAL_BOOL(*retval, obj->settings->CombineDuplicateLiterals);
	return SUCCESS;
}
/* }}} */

static int codesettings_set_CombineDuplicateLiterals(ze_codesettings_object *obj, zval *value TSRMLS_DC) /* {{{ */
{
	obj->settings->CombineDuplicateLiterals = _zval_to_bool(value);
	return SUCCESS;
}
/* }}} */

static int codesettings_get_IndentSize(ze_codesettings_object *obj, zval **retval TSRMLS_DC) /* {{{ */
{
	ALLOC_ZVAL(*retval);
	ZVAL_LONG(*retval, obj->settings->IndentSize);
	return SUCCESS;
}
/* }}} */

static int codesettings_set_IndentSize(ze_codesettings_object *obj, zval *value TSRMLS_DC) /* {{{ */
{
	obj->settings->IndentSize = _zval_to_long(value);
	return SUCCESS;
}
/* }}} */

static int codesettings_get_InlineSafeStrings(ze_codesettings_object *obj, zval **retval TSRMLS_DC) /* {{{ */
{
	ALLOC_ZVAL(*retval);
	ZVAL_BOOL(*retval, obj->settings->InlineSafeStrings);
	return SUCCESS;
}
/* }}} */

static int codesettings_set_InlineSafeStrings(ze_codesettings_object *obj, zval *value TSRMLS_DC) /* {{{ */
{
	obj->settings->InlineSafeStrings = _zval_to_bool(value);
	return SUCCESS;
}
/* }}} */

static HashTable *php_codesettings_get_properties(zval *object TSRMLS_DC) /* {{{ */
{
	HashTable *h;
	return h;
}
/* }}} */

static void php_codesettings_write_property(zval *object, zval *member, zval *value TSRMLS_DC) /* {{{ */
{
	ze_codesettings_object *obj;
	codesettings_prop_handler *hnd;
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
	obj = (ze_codesettings_object *)zend_objects_get_address(object TSRMLS_CC);

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

static zval * php_codesettings_read_property(zval *object, zval *member, int type TSRMLS_DC) /* {{{ */
{
	ze_codesettings_object *obj;
	zval tmp_member;
	zval *retval;
	codesettings_prop_handler *hnd;

	if (Z_TYPE_P(member) != IS_STRING) {
		tmp_member = *member;
		zval_copy_ctor(&tmp_member);
		convert_to_string(&tmp_member);
		member = &tmp_member;
	}

	obj = (ze_codesettings_object *)zend_objects_get_address(object TSRMLS_CC);

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

static int php_codesettings_has_property(zval *object, zval *member, int has_set_exists TSRMLS_DC) /* {{{ */
{
	return 1;
}
/* }}} */

/* {{{ proto bool ZipArchive::unchangeArchive()
Revert all global changes to the archive archive.  For now, this only reverts archive comment changes. */
static CODESETTINGS_METHOD(test) /* {{{ */
{
	RETURN_TRUE;
}
/* }}} */

/* {{{ ze_zip_object_class_functions */
static const zend_function_entry codesettings_methods[] = {
	CODESETTINGS_ME(test,			arginfo_codesettings__void, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ REGISTER_ZIP_CLASS_CONST_LONG */
#define REGISTER_CODESETTINGS_CLASS_CONST_LONG(const_name, value) \
	    zend_declare_class_constant_long(codesettings_class_entry, const_name, sizeof(const_name)-1, (long)value TSRMLS_CC);
/* }}} */

PHP_MINIT_FUNCTION(codesettings) { /* {{{ */
	zend_class_entry codesettings_ce;

	memcpy(&codesettings_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	codesettings_object_handlers.clone_obj		= NULL;
	codesettings_object_handlers.get_property_ptr_ptr = NULL;
	codesettings_object_handlers.get_properties = php_codesettings_get_properties;
	codesettings_object_handlers.write_property	= php_codesettings_write_property;
	codesettings_object_handlers.read_property	= php_codesettings_read_property;
	codesettings_object_handlers.has_property	= php_codesettings_has_property;

	INIT_CLASS_ENTRY(codesettings_ce, "CodeSettings", codesettings_methods);
	codesettings_ce.create_object = php_codesettings_object_new;
	codesettings_class_entry = zend_register_internal_class(&codesettings_ce TSRMLS_CC);

	zend_hash_init(&codesettings_prop_handlers, 0, NULL, NULL, 1);
	ajaxmin_register_prop_handler(&codesettings_prop_handlers, "CollapseToLiteral", codesettings_get_CollapseToLiteral, codesettings_set_CollapseToLiteral TSRMLS_CC);
	ajaxmin_register_prop_handler(&codesettings_prop_handlers, "CombineDuplicateLiterals", codesettings_get_CombineDuplicateLiterals, codesettings_set_CombineDuplicateLiterals TSRMLS_CC);
	ajaxmin_register_prop_handler(&codesettings_prop_handlers, "IndentSize", codesettings_get_IndentSize, codesettings_set_IndentSize TSRMLS_CC);
	ajaxmin_register_prop_handler(&codesettings_prop_handlers, "InlineSafeStrings", codesettings_get_InlineSafeStrings, codesettings_set_InlineSafeStrings TSRMLS_CC);

	REGISTER_CODESETTINGS_CLASS_CONST_LONG("COLOR_STRICT", 0);
	REGISTER_CODESETTINGS_CLASS_CONST_LONG("COLOR_HEX", 1);
	REGISTER_CODESETTINGS_CLASS_CONST_LONG("COLOR_MAJOR", 2);

	REGISTER_CODESETTINGS_CLASS_CONST_LONG("COMMENT_IMPORTANT", 0);
	REGISTER_CODESETTINGS_CLASS_CONST_LONG("COMMENT_NONE", 1);
	REGISTER_CODESETTINGS_CLASS_CONST_LONG("COMMENT_ALL", 2);
	REGISTER_CODESETTINGS_CLASS_CONST_LONG("COMMENT_HACK", 3);
	return SUCCESS;
}
