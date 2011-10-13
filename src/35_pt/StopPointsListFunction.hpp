
//////////////////////////////////////////////////////////////////////////////////////////
/// StopPointsListFunction class header.
///	@file StopPointsListFunction.hpp
///	@author Hugues Romain
///	@date 2010
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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

#ifndef SYNTHESE_PTPhysicalStopsListFunction_H__
#define SYNTHESE_PTPhysicalStopsListFunction_H__

#include "FactorableTemplate.h"
#include "Function.h"

#include <boost/optional.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <geos/geom/Envelope.h>

namespace synthese
{
	class CoordinatesSystem;

	namespace cms
	{
		class Webpage;
	}

	namespace pt
	{
		class StopArea;
		class StopPoint;

		//////////////////////////////////////////////////////////////////////////
		///	35.15 Function : Physical Stops Search.
		/// See https://extranet-rcsmobility.com/projects/synthese/wiki/Stops_list
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m35Functions refFunctions
		///	@author Hugues Romain, Xavier Raffin
		///	@date 2010
		/// @since 3.1.18
		class StopPointsListFunction:
			public util::FactorableTemplate<server::Function,StopPointsListFunction>
		{
		public:
				static const std::string PARAMETER_DATE;
				static const std::string PARAMETER_LINE_ID;
				static const std::string PARAMETER_PAGE_ID;
				static const std::string PARAMETER_BBOX;
				static const std::string PARAMETER_SRID;

		protected:
				static const std::string DATA_NAME;
				static const std::string DATA_STOPAREA_NAME;
                                static const std::string DATA_STOPAREA_CITY_NAME;
			//! \name Page parameters
			//@{
				boost::optional<boost::posix_time::ptime>	_date;
				boost::shared_ptr<const pt::StopArea>		_stopArea;
				boost::optional<util::RegistryKeyType>		_commercialLineID;
				boost::shared_ptr<const cms::Webpage>				_page;
				boost::optional<geos::geom::Envelope> _bbox;
				const CoordinatesSystem* _coordinatesSystem;
			//@}


			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// See https://extranet-rcsmobility.com/projects/synthese/wiki/Stops_list#Request
			//////////////////////////////////////////////////////////////////////////
			///	@return Generated parameters map
			/// @author Xavier Raffin
			/// @date 2010
			util::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// See https://extranet-rcsmobility.com/projects/synthese/wiki/Stops_list#Request
			//////////////////////////////////////////////////////////////////////////
			///	@param map Parameters map to interpret
			/// @author Hugues Romain
			/// @date 2010
			virtual void _setFromParametersMap(
				const util::ParametersMap& map
			);



			/// See https://extranet-rcsmobility.com/projects/synthese/wiki/Stops_list#Response
			//////////////////////////////////////////////////////////////////////////
			void _display(
				std::ostream& stream,
				const server::Request& request,
				const StopPoint& stop
			) const;


		public:
			//! @name Setters
			//@{
			//	void setObject(boost::shared_ptr<const Object> value) { _object = value; }
			//@}



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// See https://extranet-rcsmobility.com/projects/synthese/wiki/Stops_list#Response
			//////////////////////////////////////////////////////////////////////////
			/// @param stream Stream to display the content on.
			/// @param request the current request
			/// @author Xavier Raffin
			/// @date 2010
			virtual void run(std::ostream& stream, const server::Request& request) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @param session the current session
			/// @return true if the function can be run
			/// @author Hugues Romain
			/// @date 2010
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author Hugues Romain
			/// @date 2010
			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_PTPhysicalStopsListFunction_H__
