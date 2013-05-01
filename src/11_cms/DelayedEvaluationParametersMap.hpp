
/** DelayedEvaluationParametersMap class header.
	@file DelayedEvaluationParametersMap.hpp

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

#ifndef SYNTHESE_cms_DelayedEvaluationParametersMap_hpp__
#define SYNTHESE_cms_DelayedEvaluationParametersMap_hpp__

#include "ParametersMap.h"

#include "CMSScript.hpp"

namespace synthese
{
	namespace cms
	{
		/** DelayedEvaluationParametersMap class.
			@ingroup m36
		*/
		class DelayedEvaluationParametersMap:
			public util::ParametersMap
		{
		public:
			struct Field
			{
				static const CMSScript emptyContent;

				const CMSScript& content;
				mutable boost::optional<std::string> result;

				Field(const CMSScript& _content);
				Field(const std::string& constContent);
			};

			typedef std::map<std::string, Field> Fields;

		private:
			const server::Request& _request;
			const util::ParametersMap& _additionalParametersMap;
			const Webpage& _page;
			util::ParametersMap& _variables;
			const Fields _fields;

		public:
			DelayedEvaluationParametersMap(
				const Fields& fields,
				const server::Request& request,
				const util::ParametersMap& additionalParametersMap,
				const Webpage& page,
				util::ParametersMap& variables
			);

			//////////////////////////////////////////////////////////////////////////
			/// Gets the value of a field
			/// @param parameterName name of the parameter to read
			/// @param exceptionIfMissing throws an exception if the parameter is undefined, else returns an empty string
			/// @return the value of the parameter
			virtual std::string getValue(
				const std::string& parameterName,
				bool exceptionIfMissing = true
			) const;






			//////////////////////////////////////////////////////////////////////////
			/// Tests if the specified parameter is defined in the map.
			/// @param parameterName key of the parameter to test
			/// @return true if the parameter is present in the map even if its value
			/// is empty
			/// @author Hugues Romain
			/// @since 3.2.0
			/// @date 2010
			virtual bool isDefined(
				const std::string& parameterName
			) const;

		};
}	}

#endif // SYNTHESE_cms_DelayedEvaluationParametersMap_hpp__

