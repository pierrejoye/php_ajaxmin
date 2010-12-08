#ifndef PHP_CODESETTINGS_H
#define PHP_CODESETTINGS_H

typedef struct _ze_codesettings_object {
	zend_object zo;
	HashTable *prop_handler;
	auto_gcroot<CodeSettings^> settings;
} ze_codesettings_object;

#endif	/* PHP_CODESETTINGS_H */
