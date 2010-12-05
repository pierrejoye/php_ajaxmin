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
#include < vcclr.h >

#using <mscorlib.dll>
#using <System.dll>
#using <AjaxMin.dll>

using namespace System;
using namespace System::Text;
using namespace System::IO;
using namespace Microsoft::Ajax::Utilities;

#pragma managed(push, off)
ZEND_GET_MODULE(ajaxmin)

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
	NULL,
	NULL, 
	NULL, 
	NULL, 
	PHP_MINFO(ajaxmin),
	PHP_AJAXMIN_VERSION,
	STANDARD_MODULE_PROPERTIES
};

PHP_MINFO_FUNCTION(ajaxmin)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "Microsoft Ajax Minifier", "enabled");
	php_info_print_table_header(2, "Version", PHP_AJAXMIN_VERSION);
	DISPLAY_INI_ENTRIES();
	php_info_print_table_end();
}
#pragma managed(push, on)

PHP_FUNCTION(ajaxmin_minify_js)
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

