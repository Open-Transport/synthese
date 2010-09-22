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
#include "CommercialLine.h"
#include "LineMarkerInterfacePage.h"
#include "Webpage.h"
#include "RollingStock.h"
#include "Path.h"
#include "JourneyPattern.hpp"
#include "TransportNetworkTableSync.h"
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

		//Class used for trim:
		class sortableNumber
		{
		private:
			string _value;
			string _begin;// = _value except for 12s it is 12
			string _end;  // =  ""    except for 12s it is  s
			long int _numericalValue;// = 12 for 12 and 12s, = -1 for A

			typedef enum {
				isAnInteger,   //example : 12
				beginIsInteger,   //example : 12s
				beginIsNotInteger //example : A, or T1
			} numberType;
			numberType _numberType;

		public:
			sortableNumber(string str)
			{
				_numericalValue = -1;
				_value = str;

				char * buffer = strdup(_value.c_str());
				char * pEnd;

				errno = 0;

				_numericalValue = strtol (buffer, &pEnd, 10);
				if (errno != 0 || pEnd == buffer)//Is A form
				{
					_numberType = beginIsNotInteger;
					_begin = _value;
					_end = "";
				}
				else
				{
					if(*pEnd != 0 )//Is 12s form so pEnd is "s";
					{
						_numberType = beginIsInteger;
						_end = string(pEnd);
						*pEnd = '\0';
						_begin = string(buffer);
					}
					else//Is 12 form
					{
						_numberType = isAnInteger;
						_begin = _value;
						_end = "";
					}
				}
			}

			bool operator<(sortableNumber const &otherNumber) const
			{
				if((_numberType != beginIsInteger)
						&& (otherNumber._numberType != beginIsInteger))//No number have form "12S"
				{
					if((_numberType == beginIsNotInteger)
							|| (otherNumber._numberType == beginIsNotInteger))//At least one number have form "A"
					{
						return _value < otherNumber._value;
					}
					else//The two numbers have form 12
					{
						return _numericalValue < otherNumber._numericalValue;
					}
				}
				else if(_begin != otherNumber._begin)//At least one number have form 12S, and the other one have a different begin (eg. 13)
				{
					if((_numberType == isAnInteger)
							|| (otherNumber._numberType == isAnInteger))//The second number have 13 form
					{
						return _numericalValue < otherNumber._numericalValue;
					}
					else//The second number have A form
					{
						return _value < otherNumber._value;
					}
				}
				else // Case 12/12s or 12s/12k : sort based on end
				{
					return _end < otherNumber._end;
				}
			}
		};

		void LinesListFunction::run( std::ostream& stream, const Request& request ) const
		{
			//Sortering is made on numerical order
			//   (except for line number which doesn't begin by a number)
			//   then alphabetic order.
			//
			//So, linesMap is like that:
			//
			// [1]  -> ligne 1 : XXX - XXX
			// [2]  -> ligne 2 : XXX - XXX
			// [2S] -> ligne 2S: XXX - XXX
			// [A]  -> ligne A : XXX - XXX
			// [A1] -> ligne A1: XXX - XXX
			//

			typedef map<sortableNumber,shared_ptr<const CommercialLine> > linesMapType;
			linesMapType linesMap;

			//Get CommercialLine Global Registry
			typedef const pair<const RegistryKeyType, shared_ptr<CommercialLine> > myType;
			BOOST_FOREACH(myType&  myLine,Env::GetOfficialEnv().getRegistry<CommercialLine>())
			{
				//Insert respecting order described up there
				linesMap[sortableNumber(myLine.second->getShortName())] = myLine.second;
			}

			if((!_page.get())&&(_outputFormat =="xml"))
			{
				// XML header
				stream <<
					"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" <<
					"<lines xsi:noNamespaceSchemaLocation=\"http://synthese.rcsmobility.com/include/35_pt/LinesListFunction.xsd\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
					;
			}
			size_t rank(0);
			BOOST_FOREACH(linesMapType::value_type it, linesMap)
			{
				shared_ptr<const CommercialLine> line = it.second;
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
