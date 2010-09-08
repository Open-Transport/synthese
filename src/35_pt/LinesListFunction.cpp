////////////////////////////////////////////////////////////////////////////////
/// LinesListFunction class implementation.
///	@file LinesListFunction.cpp
///	@author Hugues Romain
///	@date 2008
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "RequestException.h"
#include "LinesListFunction.h"
#include "TransportNetwork.h"
#include "TransportNetworkTableSync.h"
#include "CommercialLine.h"
#include "CommercialLineTableSync.h"
#include "LineMarkerInterfacePage.h"
#include "Webpage.h"
#include "RollingStock.h"
#include "Path.h"
#include "JourneyPattern.hpp"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace pt;
	using namespace security;
	using namespace cms;
	using namespace graph;
	
	template<> const string util::FactorableTemplate<server::Function,pt::LinesListFunction>::FACTORY_KEY(
		"LinesListFunction2"
	);
	
	namespace pt
	{
		const string LinesListFunction::PARAMETER_OUTPUT_FORMAT("of");
		const string LinesListFunction::PARAMETER_NETWORK_ID("ni");
		const string LinesListFunction::PARAMETER_PAGE_ID("pi");

		ParametersMap LinesListFunction::_getParametersMap() const
		{
			ParametersMap result;
			if (_network.get() != NULL)
			{
				result.insert(PARAMETER_NETWORK_ID, _network->getKey());
			}
			return result;
		}

		void LinesListFunction::_setFromParametersMap(const ParametersMap& map)
		{
			setNetworkId(map.get<RegistryKeyType>(PARAMETER_NETWORK_ID));
			
			optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_PAGE_ID));
			if(id)
			try
			{
				_page = Env::GetOfficialEnv().get<Webpage>(*id);
			}
			catch (ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such page");
			}
			optional<string> idOf(map.getOptional<string>(PARAMETER_OUTPUT_FORMAT));
			_outputFormat = (*idOf);
		}


		void LinesListFunction::run( std::ostream& stream, const Request& request ) const
		{
			CommercialLineTableSync::SearchResult lines(
					CommercialLineTableSync::Search(*_env,
							                        _network->getKey(),
							                        boost::optional<std::string>(),
							                        boost::optional<std::string>(),
							                        0,
							                        boost::optional<std::size_t>(),
							                        true,//OrderByNetwork
							                        true,//OrderByName
							                        true)//is Ascendent ordering
			);
			if((!_page.get())&&(_outputFormat =="xml"))
			{
				// XML header
				stream <<
					"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" <<
					"<lines xsi:noNamespaceSchemaLocation=\"http://synthese.rcsmobility.com/include/35_pt/LinesListFunction.xsd\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
					;
			}
			size_t rank(0);
			BOOST_FOREACH(shared_ptr<const CommercialLine> line, lines)
			{
				if(_page.get())
				{
					LineMarkerInterfacePage::Display(
							stream,
							_page,
							request,
							*line,
							rank++
					);
				}
				else if(_outputFormat =="xml")
				{
					stream <<"<line id=\""<< line->getKey() <<
						"\" creatorId=\"" << line->getCreatorId() <<
						"\" name=\""      << line->getName() <<
						"\" shortName=\"" << line->getShortName() <<
						"\" longName=\""  << line->getLongName() <<
						"\" color=\""     << line->getColor() <<
						"\" style=\""     << line->getStyle() <<
						"\" image=\""     << line->getImage() <<
						"\" >";

					set<RollingStock *> rollingStocks;
					BOOST_FOREACH(Path* path, line->getPaths())
					{
						rollingStocks.insert(
								static_cast<const JourneyPattern*>(path)->getRollingStock()
						);
					}
					BOOST_FOREACH(RollingStock * rs, rollingStocks)
					{
						stream <<"<transportMode id=\""<< rs->getKey() <<
							"\" name=\""    << rs->getName() <<
							"\" article=\"" << rs->getArticle()<<
							"\" />";
					}
					stream <<"</line>";
				}
				else//default case : csv outputFormat
				{
					stream << line->getKey() << ";" << line->getShortName() << "\n";
				}
			}
			if((!_page.get())&&(_outputFormat =="xml"))
			{
				// XML footer
				stream << "</lines>";
			}
		}



		bool LinesListFunction::isAuthorized(
			const Session*
		) const {
			return true;
		}



		void LinesListFunction::setNetworkId(
			util::RegistryKeyType id
		){
			try
			{
				_network = TransportNetworkTableSync::Get(id, *_env);
			}
			catch (...)
			{
				throw RequestException("Transport network " + lexical_cast<string>(id) + " not found");
			}
		}

		std::string LinesListFunction::getOutputMimeType() const
		{
			std::string mimeType;
			if(_page.get())
			{
				mimeType = _page->getMimeType();
			}
			else if(_outputFormat =="xml")
			{
				mimeType = "text/xml";
			}
			else//default case : csv outputFormat
			{
				mimeType = "text/csv";
			}
			return mimeType;
		}
	}
}
