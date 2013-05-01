
/** DelayedEvaluationParametersMap class implementation.
	@file DelayedEvaluationParametersMap.cpp

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

#include "DelayedEvaluationParametersMap.hpp"

using namespace std;

namespace synthese
{
	namespace cms
	{
		const CMSScript DelayedEvaluationParametersMap::Field::emptyContent;



		DelayedEvaluationParametersMap::DelayedEvaluationParametersMap(
			const Fields& fields,
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			const Webpage& page,
			util::ParametersMap& variables
		):	_request(request),
			_additionalParametersMap(additionalParametersMap),
			_page(page),
			_variables(variables),
			_fields(fields)
		{
			// Backward compatibility with ParametersMap
			BOOST_FOREACH(const Fields::value_type& field, _fields)
			{
				insert(field.first, string());
			}
		}



		std::string DelayedEvaluationParametersMap::getValue(
			const std::string& parameterName,
			bool exceptionIfMissing /*= true */
		) const	{

			// Search for the specified parameter
			Fields::const_iterator it(_fields.find(parameterName));
			if(it == _fields.end())
			{
				if(exceptionIfMissing)
				{
					throw Exception("No such parameter");
				}
				else
				{
					return std::string();
				}
			}
			
			// Evaluation if not already done
			if(!it->second.result)
			{
				it->second.result = it->second.content.eval(
					_request,
					_additionalParametersMap,
					_page,
					_variables
				);
			}

			return *it->second.result;
		}



		bool DelayedEvaluationParametersMap::isDefined( const std::string& parameterName ) const
		{
			return _fields.find(parameterName) != _fields.end();
		}



		DelayedEvaluationParametersMap::Field::Field(
			const CMSScript& _content
		):	content(_content)
		{}



		DelayedEvaluationParametersMap::Field::Field(
			const std::string& constContent
		):	content(emptyContent),
			result(constContent)
		{}
}	}
