
/** CityListFunction class implementation.
	@file CityListFunction.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "CityListFunction.h"

#include "Webpage.h"
#include "PTServiceConfigTableSync.hpp"
#include "RequestException.h"
#include "City.h"
#include "StopArea.hpp"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace geography;
	using namespace server;
	using namespace util;
	using namespace lexical_matcher;
	using namespace pt_website;
	using namespace cms;
	using namespace pt;

	template<> const string util::FactorableTemplate<CityListFunction::_FunctionWithSite,CityListFunction>::FACTORY_KEY("lc");

	namespace pt_website
	{
		const string CityListFunction::PARAMETER_INPUT("t");
		const string CityListFunction::PARAMETER_NUMBER("n");
		const string CityListFunction::PARAMETER_PAGE("page_id");
		const string CityListFunction::PARAMETER_ITEM_PAGE("item_page_id");
		const string CityListFunction::PARAMETER_AT_LEAST_A_STOP("at_least_a_stop");
		const string CityListFunction::PARAMETER_SRID("srid");
		const string CityListFunction::PARAMETER_CONFIG_ID = "config_id";

		const std::string CityListFunction::DATA_RESULTS_SIZE("size");
		const std::string CityListFunction::DATA_CONTENT("content");

		const string CityListFunction::DATA_CITY("city");
		const string CityListFunction::DATA_CITIES("cities");
		const string CityListFunction::DATA_RANK("rank");



		ParametersMap CityListFunction::_getParametersMap() const
		{
			ParametersMap pm(FunctionWithSiteBase::_getParametersMap());
			pm.insert(PARAMETER_INPUT, _input);
			pm.insert(PARAMETER_NUMBER, _n.get_value_or(0));
			if(_page.get())
			{
				pm.insert(PARAMETER_PAGE, _page->getKey());
			}
			if(_itemPage.get())
			{
				pm.insert(PARAMETER_ITEM_PAGE, _itemPage->getKey());
			}
			else
			{
				pm.insert(PARAMETER_OUTPUT_FORMAT, _outputFormat);
			}
			pm.insert(PARAMETER_AT_LEAST_A_STOP, _atLeastAStop);
			if(_coordinatesSystem)
			{
				pm.insert(PARAMETER_SRID, static_cast<int>(_coordinatesSystem->getSRID()));
			}
			if(_config)
			{
				pm.insert(PARAMETER_CONFIG_ID, _config->getKey());
			}
			return pm;
		}



		void CityListFunction::_setFromParametersMap( const util::ParametersMap& map )
		{
			_FunctionWithSite::_setFromParametersMap(map);

			// Config
			RegistryKeyType configId(map.getDefault<RegistryKeyType>(PARAMETER_CONFIG_ID, 0));
			if(configId) try
			{
				_config = PTServiceConfigTableSync::Get(configId, *_env).get();
			}
			catch (ObjectNotFoundException<PTServiceConfig>&)
			{
				throw RequestException("No such config");
			}

			// Page
			if(map.getOptional<RegistryKeyType>(PARAMETER_PAGE))
			{
				_page = Env::GetOfficialEnv().get<Webpage>(map.get<RegistryKeyType>(PARAMETER_PAGE));
			}
			else
			{
				setOutputFormatFromMap(map, "");
			}
			if(map.getOptional<RegistryKeyType>(PARAMETER_ITEM_PAGE))
			{
				_itemPage = Env::GetOfficialEnv().get<Webpage>(map.get<RegistryKeyType>(PARAMETER_ITEM_PAGE));
			}
			_input = map.getDefault<string>(PARAMETER_INPUT);
			_atLeastAStop = map.isTrue(PARAMETER_AT_LEAST_A_STOP);

			_n = map.getOptional<size_t>(PARAMETER_NUMBER);
			if (!_input.empty() && !_n)
			{
				throw RequestException("Number of result must be limited");
			}

			CoordinatesSystem::SRID srid(
				map.getDefault<CoordinatesSystem::SRID>(PARAMETER_SRID, CoordinatesSystem::GetInstanceCoordinatesSystem().getSRID())
			);
			_coordinatesSystem = &CoordinatesSystem::GetCoordinatesSystem(srid);

			// Cleaning of template parameters for non CMS output
			if(!_outputFormat.empty())
			{
				_templateParameters.clear();
			}
		}



		util::ParametersMap CityListFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			// Choosing the matcher
			GeographyModule::CitiesMatcher matcher;
			if(_config)
			{
				matcher = _config->getCitiesMatcher();
			}
			else
			{
				matcher = (GeographyModule::GetCitiesMatcher());
			}


			GeographyModule::CityList citiesList;
			if(!_input.empty())
			{
				GeographyModule::CitiesMatcher::MatchResult matches(
					matcher.bestMatches(_input, (_atLeastAStop || !_n) ? 0 : *_n)
				);
				size_t nbmatches(0);
				BOOST_FOREACH(LexicalMatcher<boost::shared_ptr<City> >::MatchHit it, matches)
				{
					if(_atLeastAStop && it.value->getLexicalMatcher(StopArea::FACTORY_KEY).size() == 0)
					{
						continue;
					}
					citiesList.push_back(it.value);
					if(_n)
					{
						++nbmatches;
						if(nbmatches > *_n)
						{
							break;
						}
					}
				}

				if(!_page.get() && _outputFormat.empty())
				{
					stream <<
						"<?xml version=\"1.0\" encoding=\"UTF-8\"?>" <<
						"<options xsi:noNamespaceSchemaLocation=\"https://extranet.rcsmobility.com/projects/synthese/repository/raw/doc/include/56_transport_website/places_list.xsd\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
					;
					size_t nbmatches(0);
					BOOST_FOREACH(LexicalMatcher<boost::shared_ptr<City> >::MatchHit it, matches)
					{
						if(_atLeastAStop && it.value->getLexicalMatcher(StopArea::FACTORY_KEY).size() == 0)
						{
							continue;
						}
						stream << "<option type=\"city\" score=\"" << it.score.phoneticScore << "\">" << it.value->getName() << "</option>";
						if(_n)
						{
							++nbmatches;
							if(nbmatches > *_n)
							{
								break;
							}
						}
					}
					stream << "</options>";
				}
			}
			else
			{
				size_t c(0);
				BOOST_FOREACH(const LexicalMatcher<boost::shared_ptr<City> >::Map::value_type& it, matcher.entries())
				{
					if(_atLeastAStop && it.second->getLexicalMatcher(StopArea::FACTORY_KEY).size() == 0)
					{
						continue;
					}
					if(_n && ++c > *_n)
					{
						break;
					}
					citiesList.push_back(it.second);
				}
			}

			ParametersMap pm(getTemplateParameters());
			size_t i(0);
			BOOST_FOREACH(const GeographyModule::CityList::value_type& it, citiesList)
			{
				boost::shared_ptr<ParametersMap> cityPm(new ParametersMap(getTemplateParameters()));

				it->toParametersMap(*cityPm, _coordinatesSystem);
				cityPm->insert(DATA_RANK, i++);
				// backward compatibility.
				// TODO: check they are still used or could be removed.
				cityPm->insert("name", it->getName());
				cityPm->insert("roid", it->getKey());

				pm.insert(DATA_CITY, cityPm);
			}

			if(_page.get())
			{
				// Size
				pm.insert(DATA_RESULTS_SIZE, citiesList.size());

				// Content
				if(_itemPage.get())
				{
					stringstream content;
					_displayItems(content, pm, request);
					pm.insert(DATA_CONTENT, content.str());
				}

				_page->display(stream, request, pm);
			}
			else if(_itemPage.get())
			{
				_displayItems(stream, pm, request);
			}
			else
			{
				outputParametersMap(
					pm,
					stream,
					DATA_CITIES,
					"https://extranet.rcsmobility.com/svn/synthese3/trunk/src/56_pt_website/city_list.xsd"
				);
			}

			return pm;
		}



		bool CityListFunction::isAuthorized(const Session* session
		) const {
			return true;
		}



		std::string CityListFunction::getOutputMimeType() const
		{
			return _page.get() ? _page->getMimeType() : getOutputMimeTypeFromOutputFormat("text/xml");
		}



		void CityListFunction::_displayItems(
			ostream& stream,
			const ParametersMap& pm,
			const Request& request
		) const {

			BOOST_FOREACH(ParametersMap::SubParametersMap::mapped_type::value_type cityPm, pm.getSubMaps(DATA_CITY))
			{
				_itemPage->display(stream, request, *cityPm);
			}
		}
}	}
