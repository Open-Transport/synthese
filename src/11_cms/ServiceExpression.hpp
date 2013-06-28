
/** ServiceExpression class header.
	@file ServiceExpression.hpp

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

#ifndef SYNTHESE_cms_ServiceExpression_hpp__
#define SYNTHESE_cms_ServiceExpression_hpp__

#include "Expression.hpp"

#include "Factory.h"
#include "CMSScript.hpp"

#include <vector>

namespace synthese
{
	namespace server
	{
		class Function;
	}

	namespace cms
	{
		/** ServiceExpression class.
			@ingroup m36
		*/
		class ServiceExpression:
			public Expression
		{
		private:
			static const std::string PARAMETER_VAR;
			static const std::string PARAMETER_TEMPLATE;
			static const std::string PARAMETER_REPEAT_PARAMETERS;
			static const std::string VAR_EXCEPTIONS;



			const util::Factory<server::Function>::CreatorInterface* _functionCreator;
			typedef std::vector<std::pair<std::string, CMSScript> > Parameters;
			Parameters _serviceParameters;
			Parameters _templateParameters;
			CMSScript _inlineTemplate;
			bool _repeatParameters;



			//////////////////////////////////////////////////////////////////////////
			/// Adds exception message to the specific variable.
			/// @param variables the variables to update
			/// @param message the message to add
			/// @param functionCode the code of the current function
			static void _addExceptionToVariable(
				util::ParametersMap& variables,
				const std::string& message,
				const std::string& functionCode
			);

		public:
			const util::Factory<server::Function>::CreatorInterface* getFunctionCreator() const { return _functionCreator; }

			virtual std::string eval(
				const server::Request& request,
				const util::ParametersMap& additionalParametersMap,
				const Webpage& page,
				util::ParametersMap& variables
			) const;

			virtual void display(
				std::ostream& stream,
				const server::Request& request,
				const util::ParametersMap& additionalParametersMap,
				const Webpage& page,
				util::ParametersMap& variables
			) const;

			void runService(
				std::ostream& stream,
				util::ParametersMap& result,
				const server::Request& request,
				const util::ParametersMap& additionalParametersMap,
				const Webpage& page,
				util::ParametersMap& variables
			) const;


			//////////////////////////////////////////////////////////////////////////
			/// Constructor by partial string parsing
			ServiceExpression(
				std::string::const_iterator& it,
				std::string::const_iterator end
			);
		};
	}
}

#endif // SYNTHESE_cms_ServiceExpression_hpp__

