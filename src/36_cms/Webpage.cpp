
/** Webpage class implementation.
	@file Webpage.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include "Webpage.h"
#include "ParametersMap.h"
#include "DynamicRequest.h"
#include "FunctionWithSite.h"
#include "GetValueFunction.hpp"
#include "ServerModule.h"
#include "WebPageDisplayFunction.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;

	namespace util
	{
		template<> const string Registry<cms::Webpage>::KEY("Webpage");
	}

	namespace cms
	{
		Webpage::Webpage( util::RegistryKeyType id  ):
			Registrable(id),
			_startDate(posix_time::not_a_date_time),
			_endDate(posix_time::not_a_date_time),
			_template(NULL),
			_doNotUseTemplate(false),
			_hasForum(false)
		{
		}



		bool Webpage::mustBeDisplayed( boost::posix_time::ptime now /*= boost::posix_time::second_clock::local_time()*/ ) const
		{
			return
				(_startDate.is_not_a_date_time() || _startDate <= now) &&
				(_endDate.is_not_a_date_time() || _endDate >= now)
			;
		}



		std::string::const_iterator Webpage::_parse(
			std::ostream& stream,
			std::string::const_iterator it,
			std::string::const_iterator end,
			std::string termination,
			const server::Request& request,
			bool encodeSubResults,
			const server::ParametersMap& aditionalParametersMap
		) const {
			string labelToReach;
			while(it != end)
			{
				// Special characters
				if(*it == '\\' && it+1 != end)
				{
					++it;
					if(*it == 'n')
					{
						if(labelToReach.empty())
						{
							stream << endl;
						}
					}
					else if(*it == '\\')
					{
						if(labelToReach.empty())
						{
							stream << '\\';
						}
					}
					else if(*it == '<' && it+1 != end && *(it+1)=='@')
					{
						if(labelToReach.empty())
						{
							++it;
							stream << "<@";
						}
					}
					else if(*it == '<' && it+1 != end && *(it+1)=='?')
					{
						if(labelToReach.empty())
						{
							++it;
							stream << "<?";
						}
					}
					else if(*it == '<' && it+1 != end && *(it+1)=='#')
					{
						if(labelToReach.empty())
						{
							++it;
							stream << "<#";
						}
					}
					else if(*it == '@' && it+1 != end && *(it+1)=='>')
					{
						if(labelToReach.empty())
						{
							++it;
							stream << "@>";
						}
					}
					else if(*it == '%' && it+1 != end && *(it+1)=='>')
					{
						if(labelToReach.empty())
						{
							++it;
							stream << "%>";
						}
					}
					else if(*it == '<' && it+1 != end && *(it+1)=='>')
					{
						if(labelToReach.empty())
						{
							++it;
							stream << ">>";
						}
					}
					else if(*it == '?' && it+1 != end && *(it+1)=='>')
					{
						if(labelToReach.empty())
						{
							++it;
							stream << "?>";
						}
					}
					else if(*it == '#' && it+1 != end && *(it+1)=='>')
					{
						if(labelToReach.empty())
						{
							++it;
							stream << "#>";
						}
					}
					++it;
				} // Call to a public function
				else if(*it == '<' && it+1 != end && *(it+1)=='?' && it+2 != end)
				{
					stringstream query;
					query << Request::PARAMETER_FUNCTION << Request::PARAMETER_ASSIGNMENT;
					it = _parse(query, it+2, end, "?>", request, true, aditionalParametersMap);
					if(labelToReach.empty())
					{
						ParametersMap parametersMap(query.str());
						ParametersMap requestParametersMap(
							dynamic_cast<const DynamicRequest*>(&request) ?
							dynamic_cast<const DynamicRequest&>(request).getParametersMap() :
							ParametersMap()
						);
						requestParametersMap.remove(Request::PARAMETER_FUNCTION);
						parametersMap.merge(requestParametersMap);
						if(getRoot())
						{
							parametersMap.insert(FunctionWithSiteBase::PARAMETER_SITE, getRoot()->getKey());
						}
						string functionName(parametersMap.getDefault<string>(Request::PARAMETER_FUNCTION));
						if(!functionName.empty() && Factory<Function>::contains(functionName))
						{
							shared_ptr<Function> _function(Factory<Function>::create(functionName));
							if(_function.get())
							{
								try
								{
									_function->_setFromParametersMap(parametersMap);
									if (_function->isAuthorized(request.getSession()))
									{
										stringstream subresult;
										_function->run(subresult, request);
										stream << (encodeSubResults ? ServerModule::URLEncode(subresult.str()) : subresult.str());
									}
								}
								catch(...)
								{

								}
							}

						}
					}
				} // Shortcut to GetValueFunction
				else if(*it == '<' && it+1 != end && *(it+1)=='@' && it+2 != end)
				{
					stringstream parameter;
					it = _parse(parameter, it+2, end, "@>", request, true, aditionalParametersMap);
					if(labelToReach.empty())
					{
						stringstream subresult;
						GetValueFunction function;
						function.setParameter(parameter.str());
						function.setAditionnalParametersMap(aditionalParametersMap);
						function.run(subresult, request);
						stream << (encodeSubResults ? ServerModule::URLEncode(subresult.str()) : subresult.str());
					}
				} // Shortcut to WebPageDisplayFunction
				else if(*it == '<' && it+1 != end && *(it+1)=='#' && it+2 != end)
				{
					stringstream query;
					query << WebPageDisplayFunction::PARAMETER_SMART_URL << Request::PARAMETER_ASSIGNMENT << "!";
					it = _parse(query, it+2, end, "#>", request, true, aditionalParametersMap);
					if(labelToReach.empty())
					{
						ParametersMap parametersMap(query.str());
						ParametersMap requestParametersMap(
							dynamic_cast<const DynamicRequest*>(&request) ?
							dynamic_cast<const DynamicRequest&>(request).getParametersMap() :
							ParametersMap()
						);
						requestParametersMap.remove(Request::PARAMETER_FUNCTION);
						requestParametersMap.remove(WebPageDisplayFunction::PARAMETER_PAGE_ID);
						parametersMap.merge(requestParametersMap);
						if(getRoot())
						{
							parametersMap.insert(FunctionWithSiteBase::PARAMETER_SITE, getRoot()->getKey());
						}
						WebPageDisplayFunction function;
						function._setFromParametersMap(parametersMap);
						if (function.isAuthorized(request.getSession()))
						{
							stringstream subresult;
							function.run(subresult, request);
							stream << (encodeSubResults ? ServerModule::URLEncode(subresult.str()) : subresult.str());
						}
					}
				} // Goto
				else if(*it == '<' && it+1 != end && *(it+1)=='%' && it+2 != end)
				{
					stringstream label;
					it = _parse(label, it+2, end, "%>", request, false, aditionalParametersMap);
					if(labelToReach.empty() && !label.str().empty())
					{
						labelToReach = label.str();
					}
				} // Label
				else if(*it == '<' && it+1 != end && *(it+1)=='<' && it+2 != end)
				{
					stringstream label;
					it = _parse(label, it+2, end, ">>", request, false, aditionalParametersMap);
					if(labelToReach == label.str())
					{
						labelToReach.clear();
					}
				} // Reached the end of a recursion level
				else if(termination.size() == 2 && *it == termination[0] && it+1 != end && *(it+1)==termination[1])
				{
					return it+2;
				}
				else
				{
					if(labelToReach.empty())
					{
						stream << *it;
					}
					++it;
				}
			}
			return it;
		}



		void Webpage::display(
			std::ostream& stream,
			const server::Request& request,
			const server::ParametersMap& aditionalParametersMap
		) const	{
			_parse(stream, _content.begin(), _content.end(), string(), request, false, aditionalParametersMap);
		}



		std::string Webpage::getMimeType() const
		{
			return _mimeType.empty() ? "text/html" : _mimeType;
		}



		Webpage* Webpage::getTemplate() const
		{
			if(_doNotUseTemplate)
			{
				return NULL;
			}
			if(_template && _template != this)
			{
				return _template;
			}
			if(getRoot()->getDefaultTemplate() != this)
			{
				return getRoot()->getDefaultTemplate();
			}
			return NULL;
		}



		std::size_t Webpage::getDepth() const
		{
			size_t depth(0);
			for(Webpage* page(getParent()); page; page = page->getParent())
			{
				++depth;
			}
			return depth;
		}



		std::string Webpage::getFullName() const
		{
			string result;
			for(const Webpage* page(this); page; page = page->getParent())
			{
				result = page->getName() + (result.empty() ? "/" : string()) + result;
			}
			if(getRoot())
			{
				result = getRoot()->getName() + "/" + result;
			}
			return result;
		}
	}
}
