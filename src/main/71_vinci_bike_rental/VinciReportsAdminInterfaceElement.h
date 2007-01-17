
/** VinciReportsAdminInterfaceElement class header.
	@file VinciReportsAdminInterfaceElement.h

	This file belongs to the VINCI BIKE RENTAL SYNTHESE module
	Copyright (C) 2006 Vinci Park 
	Contact : Raphaël Murat - Vinci Park <rmurat@vincipark.com>

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

#ifndef SYNTHESE_VinciReportsAdminInterfaceElement_H__
#define SYNTHESE_VinciReportsAdminInterfaceElement_H__

#include "32_admin/AdminInterfaceElement.h"

namespace synthese
{
	namespace vinci
	{
		/** VinciReportsAdminInterfaceElement Class.
			@ingroup m71
		*/
		class VinciReportsAdminInterfaceElement : public admin::AdminInterfaceElement
		{
			static const std::string PARAM_START_DATE;
			static const std::string PARAM_END_DATE;


		public:
			VinciReportsAdminInterfaceElement();
			/** Initialization of the parameters from a request.
				@param request The request to use for the initialization.
			*/
			void setFromParametersMap(const server::Request::ParametersMap& map);
			std::string getTitle() const;
			void display(std::ostream& stream, const server::Request* request = NULL) const;
		};
	}
}

#endif // SYNTHESE_VinciReportsAdminInterfaceElement_H__
