#ifndef PHP_CSSSETTINGS_H
#define PHP_CSSSETTINGS_H

typedef struct _ze_csssettings_object {
	zend_object zo;
	HashTable *prop_handler;
	auto_gcroot<CssSettings^> settings;
} ze_csssettings_object;

#endif	/* PHP_CSSSETTINGS_H */
