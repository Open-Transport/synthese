
/** CustomerPasswordEMailSubjectInterfacePage class header.
	@file CustomerPasswordEMailSubjectInterfacePage.h
	@author Hugues
	@date 2009

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

#ifndef SYNTHESE_CustomerPasswordEMailSubjectInterfacePage_H__
#define SYNTHESE_CustomerPasswordEMailSubjectInterfacePage_H__

#include "InterfacePage.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace security
	{
		class User;
	}

	namespace resa
	{
		/** CustomerPasswordEMailSubjectInterfacePage Interface Page Class.
			@ingroup m51Pages refPages
			@author Hugues
			@date 2009

			@code customer_password_email_subject @endcode

			Parameters :
				- 0 : User login

			Object : User
		*/
		class CustomerPasswordEMailSubjectInterfacePage
			: public util::FactorableTemplate<interfaces::InterfacePage, CustomerPasswordEMailSubjectInterfacePage>
		{
		public:
			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.
				@param stream Stream to write on
				@param ...
				@param variables Execution variables
				@param request Source request
			*/
			void display(
				std::ostream& stream,
				const security::User& object,
				interfaces::VariablesMap& variables,
				const server::Request* request = NULL
			) const;

			CustomerPasswordEMailSubjectInterfacePage();
		};
	}
}

#endif // SYNTHESE_CustomerPasswordEMailSubjectInterfacePage_H__
