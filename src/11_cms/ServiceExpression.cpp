
/** ServiceExpression class implementation.
	@file ServiceExpression.cpp

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

#include "ServiceExpression.hpp"

#include "DelayedEvaluationParametersMap.hpp"
#include "FunctionWithSiteBase.hpp"
#include "ParametersMap.h"
#include "Request.h"
#include "RequestException.h"

#include <sstream>
#include <boost/foreach.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace server;
	using namespace util;

	namespace cms
	{
		const string ServiceExpression::PARAMETER_VAR = "VAR";
		const string ServiceExpression::PARAMETER_TEMPLATE = "template";
		const string ServiceExpression::PARAMETER_REPEAT_PARAMETERS = "repeat_parameters";
		const string ServiceExpression::VAR_EXCEPTIONS = "exceptions";



		std::string ServiceExpression::eval(
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			const Webpage& page,
			util::ParametersMap& variables
		) const	{
			stringstream s;
			display(s, request, additionalParametersMap, page, variables);
			return s.str();
		}



		void ServiceExpression::_addExceptionToVariable(
			util::ParametersMap& variables,
			const std::string& message,
			const std::string& functionCode
		){
			string existingExceptions(variables.getDefault<string>(VAR_EXCEPTIONS));
			stringstream s;
			if(!existingExceptions.empty())
			{
				s << existingExceptions << "<br />";
			}
			s << "<b>" << functionCode << "</b> error : " << message;
			variables.insert(VAR_EXCEPTIONS, s.str());
		}



		//////////////////////////////////////////////////////////////////////////
		/// Runs the service and get the parameters map result.
		/// Direct output is done on the stream.
		void ServiceExpression::runService(
			std::ostream& stream,
			ParametersMap& result,
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			const Webpage& page,
			util::ParametersMap& variables
		) const	{
			if(!_functionCreator)
			{
				return;
			}

			// Service parameters evaluation
			DelayedEvaluationParametersMap::Fields fields;

			// Repeat parameters from URL
			if(_repeatParameters)
			{
				BOOST_FOREACH(const ParametersMap::Map::value_type& param, additionalParametersMap.getMap())
				{
					fields.insert(
						make_pair(
							param.first,
							DelayedEvaluationParametersMap::Field(param.second)
					)	);
				}
			}

			// Get parameters defined in the template (prior to URL parameters is both defined)
			BOOST_FOREACH(const Parameters::value_type& param, _serviceParameters)
			{
				fields.insert(
					make_pair(
						param.first,
						DelayedEvaluationParametersMap::Field(param.second)
				)	);
			}

			// No output format if inline template is defined
			if(!_inlineTemplate.empty())
			{
				fields.insert(
					make_pair(
						Function::PARAMETER_OUTPUT_FORMAT,
						string()
				)	);
			}

			DelayedEvaluationParametersMap serviceParametersMap(
				fields,
				request,
				additionalParametersMap,
				page,
				variables
			);

			// Template parameters evaluation
			ParametersMap templateParametersMap(request.getFunction()->getTemplateParameters());
			BOOST_FOREACH(const Parameters::value_type& param, _templateParameters)
			{
				templateParametersMap.insert(
					param.first,
					param.second.eval(request, additionalParametersMap, page, variables)
				);
			}

			// Function
			boost::shared_ptr<Function> function(_functionCreator->create());
			if(dynamic_cast<FunctionWithSiteBase*>(function.get()))
			{
				static_cast<FunctionWithSiteBase*>(function.get())->setSite(page.getRoot());
			}
			function->setTemplateParameters(templateParametersMap);

			try
			{
				// Service initialization
				function->_setFromParametersMap(serviceParametersMap);

				// Right check
				if (function->isAuthorized(request.getSession().get()))
				{
					// Run of the service
					result = function->run(stream, request);
				}
				else // Output error message for forbidden service
				{
					_addExceptionToVariable(variables, "Forbidden", function->getFactoryKey());
				}
			}
			catch(RequestException&e) // Output error message for initialization or run exception
			{
				_addExceptionToVariable(variables, e.getMessage(), function->getFactoryKey());
			}
			catch(Request::RedirectException& e) // Allow redirection
			{
				throw e;
			}
			catch(...) // Output error message for non specified exception
			{
				_addExceptionToVariable(variables, "Unhandled exception", function->getFactoryKey());
			}
		}



		void ServiceExpression::display(
			std::ostream& stream,
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			const Webpage& page,
			util::ParametersMap& variables
		) const	{

			ParametersMap result;
			runService(stream, result, request, additionalParametersMap, page, variables);

			// Display of the result if inline template defined
			if(!_inlineTemplate.empty())
			{
				// Merge page parameters in result map
				result.merge(additionalParametersMap);

				// Display of each inline defined node
				_inlineTemplate.display(stream, request, result, page, variables);
			}
		}



		ServiceExpression::ServiceExpression(
			std::string::const_iterator& it,
			std::string::const_iterator end
		):	_functionCreator(NULL),
			_repeatParameters(false)
		{
			// function name
			string functionName;
			for(;it != end && *it != '&' && *it != '?'; ++it)
			{
				functionName.push_back(*it);
			}
			try
			{
				_functionCreator = Factory<Function>::GetCreator(ParametersMap::Trim(functionName));

				// parameters
				if(it != end && *it == '?')
				{
					it += 2;
				}
				else
				{
					set<string> functionTermination;
					functionTermination.insert("&");
					functionTermination.insert("?>");
					while(it != end && *it == '&')
					{
						stringstream parameterName;
						for(++it;
							it!= end && *it != '=' && !(*it == '?' && (it+1 == end || *(it+1)=='>'));
							++it
						){
							parameterName.put(*it);
						}

						if(it == end)
						{
							break;
						}

						if(*it == '?' && it+1 == end)
						{
							++it;
							break;
						}

						if(*it == '?' && *(it+1) == '>')
						{
							++it;
							++it;
							break;
						}
						
						// Parsing of the nodes
						++it;
						CMSScript parameterNodes(it, end, functionTermination);
						string parameterNameStr(ParametersMap::Trim(parameterName.str()));

						// Special template parameter
						if(parameterNameStr == PARAMETER_TEMPLATE)
						{
							_inlineTemplate = parameterNodes;
						}
						else if(parameterNameStr == PARAMETER_REPEAT_PARAMETERS)
						{
							_repeatParameters = true;
						}
						else
						{
							// Storage in template parameters if begins with VAR else in service parameters
							if(parameterNameStr.size() < PARAMETER_VAR.size() || parameterNameStr.substr(0, PARAMETER_VAR.size()) != PARAMETER_VAR)
							{
								_serviceParameters.push_back(make_pair(parameterNameStr, parameterNodes));
							}
							else
							{
								_templateParameters.push_back(make_pair(parameterNameStr.substr(PARAMETER_VAR.size()), parameterNodes));
							}
						}

						if(*(it-1) != '&')
						{
							break;
						}
						--it;
				}	}
			}
			catch(FactoryException<Function>&)
			{
				for(; it != end && it+1 != end && *it != '?' && *(it+1) != '>' ; ++it) ;
				if(it != end && *it == '?')
				{
					++it;
					if(it != end && *it == '>')
					{
						++it;
				}	}
			}
		}
}	}
