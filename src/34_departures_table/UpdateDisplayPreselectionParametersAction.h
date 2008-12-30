////////////////////////////////////////////////////////////////////////////////
/// UpdateDisplayPreselectionParametersAction class header.
///	@file UpdateDisplayPreselectionParametersAction.h
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_UpdateDisplayPreselectionParametersAction_H__
#define SYNTHESE_UpdateDisplayPreselectionParametersAction_H__

#include "Action.h"
#include "FactorableTemplate.h"
#include "34_departures_table/Types.h"

#include <map>
#include <string>

namespace synthese
{
	namespace departurestable
	{
		class DisplayScreen;

		/** Display screen content properties update action class.
			@ingroup m54Actions refActions
		*/
		class UpdateDisplayPreselectionParametersAction : public util::FactorableTemplate<server::Action, UpdateDisplayPreselectionParametersAction>
		{
		public:
			typedef enum
			{
				DEPARTURES_CHRONOLOGICAL,
				DEPARTURES_PRESELECTION,
				ARRIVAL_CHRONOLOGICAL,
				ARRIVAL_PRESELECTION,
				ROUTE_PLANNING
			} DisplayFunction;

			static const std::string PARAMETER_DISPLAY_SCREEN;
			static const std::string PARAMETER_PRESELECTION_DELAY;
			static const std::string PARAMETER_DISPLAY_FUNCTION;
			static const std::string PARAMETER_CLEANING_DELAY;
			static const std::string PARAMETER_DISPLAY_END_FILTER;
			static const std::string PARAMETER_DISPLAY_MAX_DELAY;

		private:
			boost::shared_ptr<DisplayScreen>	_screen;
			int									_preselectionDelay;
			DisplayFunction						_function;
			int									_cleaningDelay;
			EndFilter							_endFilter;
			int									_maxDelay;

		protected:
			/** Conversion from attributes to generic parameter maps.
			*/
			server::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
				@exception ActionException Occurs when some parameters are missing or incorrect.
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run();

			static std::map<DisplayFunction, std::string> GetFunctionList();
			static std::map<int, std::string> GetClearDelaysList();
			static DisplayFunction GetFunction(const DisplayScreen& screen);

			virtual bool _isAuthorized() const;

			
			
			////////////////////////////////////////////////////////////////////
			///	Screen loader.
			///	@param id ID of the screen
			///	@author Hugues Romain
			///	@date 2008
			/// @throws ActionException if the screen does not exist
			void setScreenId(const util::RegistryKeyType id);
		};
	}
}

#endif // SYNTHESE_UpdateDisplayPreselectionParametersAction_H__
