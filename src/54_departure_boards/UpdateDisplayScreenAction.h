
/** UpdateDisplayScreenAction class header.
	@file UpdateDisplayScreenAction.h

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

#ifndef SYNTHESE_UpdateDisplayScreenAction_H__
#define SYNTHESE_UpdateDisplayScreenAction_H__

#include <map>

#include "Action.h"
#include "DeparturesTableTypes.h"
#include "DisplayScreen.h"

namespace synthese
{
	namespace pt
	{
		class StopPoint;
	}

	namespace departure_boards
	{
		class DisplayType;
		class DisplayScreenCPU;

		//////////////////////////////////////////////////////////////////////////
		/// Display screen technical properties update.
		///	@ingroup m54Actions refActions
		/// @author Hugues Romain
		/// Parameters :
		///		- id : id of the screen to update
		///		- na : new value for name
		///		- wc : new value for wiring code
		///		- ty : new value for display type id
		///		- cp : new value for com port (number)
		///		- cu : new value for connected CPU id (optional)
		///		- ma : new value for mac address (optional)
		///		- st : new value for type of sub screen (only for screens with parents)
		class UpdateDisplayScreenAction : public util::FactorableTemplate<server::Action, UpdateDisplayScreenAction>
		{
		public:
			static const std::string PARAMETER_DISPLAY_SCREEN;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_WIRING_CODE;
			static const std::string PARAMETER_TYPE;
			static const std::string PARAMETER_COM_PORT;
			static const std::string PARAMETER_CPU;
			static const std::string PARAMETER_MAC_ADDRESS;
			static const std::string PARAMETER_SUB_SCREEN_TYPE;
			static const std::string PARAMETER_X;
			static const std::string PARAMETER_Y;
			static const std::string PARAMETER_SRID;

		private:
			std::string									_name;
			boost::shared_ptr<DisplayScreen>			_screen;
			int											_wiringCode;
			boost::shared_ptr<const DisplayType>		_type;
			boost::shared_ptr<const DisplayScreenCPU>	_cpu;
			int											_comPort;
			std::string									_macAddress;
			DisplayScreen::SubScreenType				_subScreenType;
			boost::shared_ptr<geos::geom::Point>		_point;
			const CoordinatesSystem*                    _coordinatesSystem;


		protected:
			/** Conversion from attributes to generic parameter maps.
				@warning Not yet implemented;
			*/
			util::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				@param map Map to read
			*/
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run(server::Request& request);

			void setScreenId(util::RegistryKeyType id);

			void setPoint(boost::shared_ptr<geos::geom::Point> value);

			virtual bool isAuthorized(const server::Session* session) const;
		};
	}
}

#endif // SYNTHESE_UpdateDisplayScreenAction_H__
