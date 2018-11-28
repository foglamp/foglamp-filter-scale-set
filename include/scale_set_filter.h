#ifndef _SCALE_SET_FILTER_H
#define _SCALE_SET_FILTER_H
/*
 * FogLAMP "RMS" filter plugin.
 *
 * Copyright (c) 2018 Dianomic Systems
 *
 * Released under the Apache 2.0 Licence
 *
 * Author: Mark Riddoch           
 */     
#include <filter.h>               
#include <reading_set.h>
#include <config_category.h>
#include <string>                 
#include <vector>
#include <regex>

/**
 * A FogLAMP filter that can apply a set of different scale and offset
 * values to a readings set. Different scale factors can be applied to
 * each asset and/or datapoint within the asset.
 */
class ScaleSetFilter : public FogLampFilter {
	public:
		ScaleSetFilter(const std::string& filterName,
                        ConfigCategory& filterConfig,
                        OUTPUT_HANDLE *outHandle,
                        OUTPUT_STREAM out);
		~ScaleSetFilter();
		void	ingest(const std::vector<Reading *>& readings);
	private:
		class ScaleSet {
			public:
				ScaleSet(const std::string& asset,const std::string& datapoint, double scale, double offset);
				~ScaleSet();
				void	apply(const Reading *reading) const;
			private:
				const std::string&	m_asset;
				const std::string&	m_datapoint;
				double			m_scale;
				double			m_offset;
				std::regex		*m_asset_re;
				std::regex		*m_datapoint_re;
		};
		void 			handleConfig(const ConfigCategory& conf);
		std::vector<ScaleSet *>	m_scaleSet;
};


#endif
