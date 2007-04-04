
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

#include "71_vinci_bike_rental/AdvancedSelectTableSync.h"

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

			std::map<time::Date, RentReportResult> _resultsPerDay;
			std::map<int, int> _rentsByDay;
			std::map<int, int> _returnsByDay;
			std::map<int, int> _renewByDay;
			std::map<int, int> _rentsByFareDay;
			std::map<int, int> _results;
			time::Date _startDate;
			time::Date _endDate;

		public:
			VinciReportsAdminInterfaceElement();
			/** Initialization of the parameters from a request.
				@param request The request to use for the initialization.
			*/
			void setFromParametersMap(const admin::AdminRequest::ParametersMap& map);
			std::string getTitle() const;
			void display(std::ostream& stream, interfaces::VariablesMap& variables, const admin::AdminRequest* request = NULL) const;
		};
	}
}

#endif // SYNTHESE_VinciReportsAdminInterfaceElement_H__
