
/** AddDepartureStopToDisplayScreenAction class header.
	@file AddDepartureStopToDisplayScreenAction.h

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

#ifndef SYNTHESE_AddDepartureStopToDisplayScreenAction_H__
#define SYNTHESE_AddDepartureStopToDisplayScreenAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace pt
	{
		class StopPoint;
	}

	namespace departure_boards
	{
		class DisplayScreen;

		/** Action class : Adds a stop to the selection of a departures table.

			@ingroup m54Actions refActions
		*/
		class AddDepartureStopToDisplayScreenAction
		:	public util::FactorableTemplate<server::Action, AddDepartureStopToDisplayScreenAction>
		{
		public:
			static const std::string PARAMETER_SCREEN_ID;
			static const std::string PARAMETER_STOP;

		private:
			boost::shared_ptr<DisplayScreen>			_screen;
			boost::shared_ptr<const pt::StopPoint>	_stop;

		protected:
			/** Conversion from attributes to generic parameter maps.
			*/
			util::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
				@exception ActionException Occurs when some parameters are missing or incorrect.
			*/
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run(server::Request& request);

			virtual bool isAuthorized(const server::Session* session) const;

			void setStopId(
				util::RegistryKeyType id
			);
			void setScreen(
				boost::shared_ptr<DisplayScreen> screen
			);
			void setScreen(
				boost::shared_ptr<const DisplayScreen> screen
			);
		};
	}
}

#endif // SYNTHESE_AddDepartureStopToDisplayScreenAction_H__
