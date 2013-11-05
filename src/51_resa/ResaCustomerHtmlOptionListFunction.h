
/** ResaCustomerHtmlOptionListFunction class header.
	@file ResaCustomerHtmlOptionListFunction.h
	@author Hugues Romain
	@date 2008

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

#ifndef SYNTHESE_ResaCustomerHtmlOptionListFunction_H__
#define SYNTHESE_ResaCustomerHtmlOptionListFunction_H__

#include "FactorableTemplate.h"
#include "Function.h"

namespace synthese
{
	namespace resa
	{
		/** ResaCustomerHtmlOptionListFunction Function class.
			@author Hugues Romain
			@date 2008
			@ingroup m51Functions refFunctions
		*/
		class ResaCustomerHtmlOptionListFunction:
			public util::FactorableTemplate<server::Function,ResaCustomerHtmlOptionListFunction>
		{
		public:
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_SURNAME;
			static const std::string PARAMETER_NUMBER;
			static const std::string PARAMETER_PHONE;

		protected:
			//! \name Page parameters
			//@{
				boost::optional<std::string> _name;
				boost::optional<std::string> _surname;
				size_t						_number;
				boost::optional<std::string> _phone;
			//@}


			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			util::ParametersMap _getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				@param map Parameters map to interpret
			*/
			void _setFromParametersMap(const util::ParametersMap& map);



		public:
			ResaCustomerHtmlOptionListFunction();

			/** Action to run, defined by each subclass.
			*/
			util::ParametersMap run(std::ostream& stream, const server::Request& request) const;

			void setNumber(int number);

			virtual bool isAuthorized(const server::Session* session) const;

			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_ResaCustomerHtmlOptionListFunction_H__
