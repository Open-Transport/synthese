
//////////////////////////////////////////////////////////////////////////////////////////
/// DriverAllocationsListService class header.
///	@file DriverAllocationsListService.hpp
///	@author Hugues Romain
///	@date 2011
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_DriverAllocationsListService_H__
#define SYNTHESE_DriverAllocationsListService_H__

#include "FactorableTemplate.h"
#include "Function.h"

#include <boost/date_time/gregorian/greg_date.hpp>

namespace synthese
{
	namespace impex
	{
		class DataSource;
	}

	namespace cms
	{
		class Webpage;
	}

	namespace security
	{
		class User;
	}

	namespace pt_operation
	{
		//////////////////////////////////////////////////////////////////////////
		///	37.15 Function : DriverAllocationsListService.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Driver_allocations_list
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m37Functions refFunctions
		///	@author Hugues Romain
		///	@date 2011
		/// @since 3.2.1
		class DriverAllocationsListService:
			public util::FactorableTemplate<server::Function,DriverAllocationsListService>
		{
		public:
			static const std::string PARAMETER_DRIVER_ID;
			static const std::string PARAMETER_DATA_SOURCE_ID;
			static const std::string PARAMETER_MIN_DATE;
			static const std::string PARAMETER_PAGE_ID;

			static const std::string TAG_ALLOCATION;

		protected:
			//! \name Page parameters
			//@{
				boost::shared_ptr<const impex::DataSource> _dataSource;
				boost::shared_ptr<const security::User>	_driver;
				boost::gregorian::date _minDate;
				boost::shared_ptr<const cms::Webpage> _page;
			//@}


			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Compositions list#Request
			//////////////////////////////////////////////////////////////////////////
			///	@return Generated parameters map
			/// @author Hugues Romain
			/// @date 2011
			/// @since 3.2.1
			util::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Compositions list#Request
			//////////////////////////////////////////////////////////////////////////
			///	@param map Parameters map to interpret
			/// @author Hugues Romain
			/// @date 2011
			/// @since 3.2.1
			virtual void _setFromParametersMap(
				const util::ParametersMap& map
				);


		public:
			//! @name Setters
			//@{
			//	void setObject(boost::shared_ptr<const Object> value) { _object = value; }
			//@}



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// @param stream Stream to display the content on.
			/// @param request the current request
			/// @author Hugues Romain
			/// @date 2011
			virtual util::ParametersMap run(std::ostream& stream, const server::Request& request) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @param session the current session
			/// @return true if the function can be run
			/// @author Hugues Romain
			/// @date 2011
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author Hugues Romain
			/// @date 2011
			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_DriverAllocationsListService_H__
