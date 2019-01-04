/*
 * FogLAMP "scale-set" filter plugin.
 *
 * Copyright (c) 2018 Dianomic Systems
 *
 * Released under the Apache 2.0 Licence
 *
 * Author:  Mark Riddoch
 */

#include <plugin_api.h>
#include <config_category.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string>
#include <iostream>
#include <filter_plugin.h>
#include <scale_set_filter.h>

#define FILTER_NAME "scale-set"
#define DEFAULT_CONFIG "{\"plugin\" : { \"description\" : \"Scale filter plugin, apply a set of per asset and datapoint scale factors and offsets\", " \
                       		"\"type\" : \"string\", " \
				"\"default\" : \"" FILTER_NAME "\", \"readonly\":\"true\" }, " \
			 "\"enable\": {\"description\": \"A switch that can be used to enable or disable execution of " \
					 "the scale filter.\", " \
				"\"type\": \"boolean\", " \
				"\"displayName\" : \"Enabled\", " \
				"\"default\": \"false\" }, " \
			"\"factors\" : {\"description\" : \"Scale factor and offset configuration.\", " \
				"\"type\": \"JSON\", " \
				"\"default\": \"{ \\\"factors\\\" : [" \
							"{ \\\"asset\\\" : \\\".*\\\", " \
							"  \\\"datapoint\\\" : \\\".*\\\", " \
							"  \\\"scale\\\" : 1.0, " \
							"  \\\"offset\\\" : 0.0 " \
							" }" \
						"] }" \
			"\", \"order\": \"1\", \"displayName\":\"Scale factors\"} }"

using namespace std;

/**
 * The Filter plugin interface
 */
extern "C" {

/**
 * The plugin information structure
 */
static PLUGIN_INFORMATION info = {
        FILTER_NAME,              // Name
        "1.0.0",                  // Version
        0,                        // Flags
        PLUGIN_TYPE_FILTER,       // Type
        "1.0.0",                  // Interface version
	DEFAULT_CONFIG	          // Default plugin configuration
};

/**
 * Return the information about this plugin
 */
PLUGIN_INFORMATION *plugin_info()
{
	return &info;
}

/**
 * Initialise the ScaleSet plugin. This creates the underlying
 * object and prepares the filter for operation. This will be
 * called before any data is ingested
 *
 * @param config	The configuration category for the filter
 * @param outHandle	A handle that will be passed to the output stream
 * @param output	The output stream (function pointer) to which data is passed
 * @return		An opaque handle that is used in all subsequent calls to the plugin
 */
PLUGIN_HANDLE plugin_init(ConfigCategory *config,
			  OUTPUT_HANDLE *outHandle,
			  OUTPUT_STREAM output)
{
	ScaleSetFilter *handle = new ScaleSetFilter(FILTER_NAME,
						  *config,
						  outHandle,
						  output);
	
	return (PLUGIN_HANDLE)handle;
}

/**
 * Ingest a set of readings into the plugin for processing
 *
 * @param handle	The plugin handle returned from plugin_init
 * @param readingSet	The readings to process
 */
void plugin_ingest(PLUGIN_HANDLE *handle,
		   READINGSET *readingSet)
{
	ScaleSetFilter *filter = (ScaleSetFilter *)handle;
	if (!filter->isEnabled())
	{
		// Current filter is not active: just pass the readings set
		filter->m_func(filter->m_data, readingSet);
		return;
	}

	filter->ingest(((ReadingSet *)readingSet)->getAllReadings());
	filter->m_func(filter->m_data, readingSet);
}

/**
 * Call the shutdown method in the plugin
 */
void plugin_shutdown(PLUGIN_HANDLE *handle)
{
	ScaleSetFilter *filter = (ScaleSetFilter *)handle;
	delete filter;
}

};
