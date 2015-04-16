
//////////////////////////////////////////////////////////////////////////
/// StopPointUpdateAction class header.
///	@file StopPointUpdateAction.hpp
///	@author Hugues Romain
///	@date 2010
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

#ifndef SYNTHESE_PhysicalStopUpdateAction_H__
#define SYNTHESE_PhysicalStopUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"
#include "BaseImportableUpdateAction.hpp"

namespace geos
{
	namespace geom
	{
		class Point;
	}
}

namespace synthese
{
	class CoordinatesSystem;

	namespace pt
	{
		class StopPoint;
		class StopArea;

		//////////////////////////////////////////////////////////////////////////
		/// 35.15 Action : StopPointUpdateAction.
		/// @ingroup m35Actions refActions
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.1.16
		//////////////////////////////////////////////////////////////////////////
		/// Key : StopPointUpdateAction
		///
		/// Parameters :
		///	<ul>
		///		<li>actionParamid : id of the object to update</li>
		///		<li>actionParamx : x</li>
		///		<li>actionParamy : y</li>
		///		<li>actionParamoc : operator code</li>
		///		<li>actionParamna : name</li>
		///		<li>actionParamsa : stop area</li>
		///	</ul>
		class StopPointUpdateAction:
			public util::FactorableTemplate<server::Action, StopPointUpdateAction>,
			public impex::BaseImportableUpdateAction
		{
		public:
			static const std::string PARAMETER_STOP_ID;
			static const std::string PARAMETER_X;
			static const std::string PARAMETER_Y;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_STOP_AREA;
			static const std::string PARAMETER_SRID;

		private:
			boost::shared_ptr<StopPoint> _stop;
			boost::shared_ptr<geos::geom::Point> _point;
			boost::optional<std::string> _name;
			boost::optional<boost::shared_ptr<const StopArea> > _stopArea;
			const CoordinatesSystem* _coordinatesSystem;


		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Generates a generic parameters map from the action parameters.
			/// @return The generated parameters map
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Reads the parameters of the action on a generic parameters map.
			/// @param map Parameters map to interpret
			/// @exception ActionException Occurs when some parameters are missing or incorrect.
			void _setFromParametersMap(const util::ParametersMap& map);


		public:
			//////////////////////////////////////////////////////////////////////////
			/// The action execution code.
			/// @param request the request which has launched the action
			void run(server::Request& request);



			//////////////////////////////////////////////////////////////////////////
			/// Tests if the action can be launched in the current session.
			/// @param session the current session
			/// @return true if the action can be launched in the current session
			virtual bool isAuthorized(const server::Session* session) const;



			//! @name Setters
			//@{
				void setStop(boost::shared_ptr<StopPoint> value) { _stop = value; }
				void setPoint(boost::shared_ptr<geos::geom::Point> value);
			//@}
		};
	}
}

#endif // SYNTHESE_PhysicalStopUpdateAction_H__
