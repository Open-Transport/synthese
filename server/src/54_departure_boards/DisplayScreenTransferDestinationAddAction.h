
/** DisplayScreenTransferDestinationAddAction class header.
	@file DisplayScreenTransferDestinationAddAction.h
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

#ifndef SYNTHESE_DisplayScreenTransferDestinationAddAction_H__
#define SYNTHESE_DisplayScreenTransferDestinationAddAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace pt
	{
		class StopArea;
	}

	namespace departure_boards
	{
		class DisplayScreen;

		/** DisplayScreenTransferDestinationAddAction action class.
			@ingroup m54Actions refActions
		*/
		class DisplayScreenTransferDestinationAddAction:
			public util::FactorableTemplate<server::Action, DisplayScreenTransferDestinationAddAction>
		{
		public:
			static const std::string PARAMETER_DISPLAY_SCREEN_ID;
			static const std::string PARAMETER_TRANSFER_PLACE_ID;
			static const std::string PARAMETER_DESTINATION_PLACE_CITY_NAME;
			static const std::string PARAMETER_DESTINATION_PLACE_NAME;

		private:
			boost::shared_ptr<DisplayScreen> _screen;
			boost::shared_ptr<const pt::StopArea> _transferPlace;
			const pt::StopArea* _destinationPlace;

		protected:
			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			util::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
				@param map Parameters map to interpret
				@exception ActionException Occurs when some parameters are missing or incorrect.
			*/
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			DisplayScreenTransferDestinationAddAction();

			/** Action to run, defined by each subclass.
			*/
			void run(server::Request& request);

			virtual bool isAuthorized(const server::Session* session) const;

			void setScreen(boost::shared_ptr<const DisplayScreen> value);
		};
	}
}

#endif // SYNTHESE_DisplayScreenTransferDestinationAddAction_H__
