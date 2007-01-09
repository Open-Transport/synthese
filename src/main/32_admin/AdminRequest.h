
/** AdminRequest class header.
	@file AdminRequest.h

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

#ifndef SYNTHESE_AdminRequest_H__
#define SYNTHESE_AdminRequest_H__

#include "30_server/Request.h"

namespace synthese
{
	namespace admin
	{
		class AdminInterfaceElement;

		class AdminRequest : public server::Request
		{
			static const std::string PARAMETER_PAGE;
			
			//! \name Page parameters
			//@{
			const AdminInterfaceElement*	_page;
			ParametersMap			_parameters;
			//@}


			/** Conversion from attributes to generic parameter maps.
			*/
			ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
			*/
			void setFromParametersMap(const ParametersMap& map);

		public:
			AdminRequest();
			~AdminRequest();

			void setPage(const AdminInterfaceElement* aie);
			const AdminInterfaceElement* getPage() const;

			/** Action to run, defined by each subclass.
			*/
			void run(std::ostream& stream) const;

			std::string getHTMLFormHeader(const std::string& name) const;

			void setParameter(const std::string& name, const std::string value);

		};
	}
}
#endif // SYNTHESE_AdminRequest_H__
