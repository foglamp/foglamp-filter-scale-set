/*
 * FogLAMP "scale-set" filter plugin.
 *
 * Copyright (c) 2018 Dianomic Systems
 *
 * Released under the Apache 2.0 Licence
 *
 * Author: Mark Riddoch           
 */     
#include <reading.h>              
#include <reading_set.h>
#include <utility>                
#include <logger.h>
#include <scale_set_filter.h>

using namespace std;
using namespace rapidjson;

/**
 * Construct a ScaleSetFilter, callthe base class constructor and handle the
 * parsing of the configuration category to create the vector of ScaleSets
 */
ScaleSetFilter::ScaleSetFilter(const std::string& filterName,
			       ConfigCategory& filterConfig,
                               OUTPUT_HANDLE *outHandle,
                               OUTPUT_STREAM out) :
                                  FogLampFilter(filterName, filterConfig,
                                                outHandle, out)
{
	handleConfig(filterConfig);
}

/**
 * Destructor for the filter. Deletes any ScaleSet items
 * that have been allocated.
 */
ScaleSetFilter::~ScaleSetFilter()
{
	for (auto it = m_scaleSet.cbegin(); it != m_scaleSet.cend(); it++)
		delete *it;
}

/**
 * Called with a set of readings, iterates over the readings applying
 * the scale and offset to each as appropriate.
 *
 * @param readings	The readings to process
 */
void ScaleSetFilter::ingest(const vector<Reading *>& readings)
{
	// Iterate over the readings
	for (vector<Reading *>::const_iterator reading = readings.begin();
						      reading != readings.end();
						      ++reading)
	{
		for (vector<ScaleSet *>::const_iterator it = m_scaleSet.cbegin();
					it != m_scaleSet.cend(); it++)
		{
			(*it)->apply(*reading);
		}
	}
}

/**
 * Handle the configuration of the plugin.
 *
 * Creates the vector of scale/offset values to apply to the reading set.
 *
 * @param conf	The configuration category for the filter.
 */
void ScaleSetFilter::handleConfig(const ConfigCategory& conf)
{
Document	doc;

	if (conf.itemExists("factors"))
	{
		doc.Parse(conf.getValue("factors").c_str());
		if (! doc.HasParseError())
		{
			if (doc.HasMember("factors") && doc["factors"].IsArray())
			{
				for (Value::ConstValueIterator it = doc["factors"].Begin();
						it != doc["factors"].End(); it++)
				{
					string	asset, datapoint;
					double	scale = 1.0, offset = 0.0;
					if (it->HasMember("asset"))
						asset = (*it)["asset"].GetString();
					if (it->HasMember("datapoint"))
						datapoint = (*it)["datapoint"].GetString();
					if (it->HasMember("scale") && (*it)["scale"].IsFloat())
						scale = (*it)["scale"].GetFloat();
					if (it->HasMember("offset"))
						offset = (*it)["offset"].GetFloat();
					m_scaleSet.push_back(new ScaleSet(asset, datapoint, scale, offset));
				}
			}
		}
	}
}

/**
 * Create a ScaleSet instance
 */
ScaleSetFilter::ScaleSet::ScaleSet(const string& asset, const string& datapoint,
				double scale, double offset) : m_asset(asset), m_datapoint(datapoint),
							m_scale(scale), m_offset(offset)
{
	m_asset_re = new regex(asset);
	if (! m_datapoint.empty())
		m_datapoint_re = new regex(datapoint);
}

/**
 * Desctructor for a scale set
 */
ScaleSetFilter::ScaleSet::~ScaleSet()
{
	delete m_asset_re;
	if (! m_datapoint.empty())
		delete m_datapoint_re;
}

/**
 * Apply a scale set to a reading if it matches the asset name and data point name
 *
 * @param reading	The reading to scale
 */
void ScaleSetFilter::ScaleSet::apply(const Reading *reading) const
{
	if (! regex_match(reading->getAssetName(), *m_asset_re))
	{
		return;
	}
	const vector<Datapoint *>& dataPoints = reading->getReadingData();
	for (vector<Datapoint *>::const_iterator it = dataPoints.begin(); it != dataPoints.end(); ++it)
	{
			if (m_datapoint.empty() == false && regex_match((*it)->getName(), *m_datapoint_re) == false)
				continue;
			DatapointValue& value = (*it)->getData();

			/*
			 * Deal with the T_INTEGER and T_FLOAT types.
			 * Try to preserve the typoe if possible but
			 * of a flaoting point scale or offset is applied
			 * then T_INTEGER values will turn into T_FLOAT.
			 */
			if (value.getType() == DatapointValue::T_INTEGER)
			{
				double newValue = value.toInt() * m_scale + m_offset;
				if (newValue == floor(newValue))
				{
					value.setValue(newValue);
				}
				else
				{
					value.setValue((long)newValue);
				}
			}
			else if (value.getType() == DatapointValue::T_FLOAT)
			{
				value.setValue(value.toDouble() * m_scale + m_offset);
			}
			else
			{
				// do nothing
			}
	}
}
