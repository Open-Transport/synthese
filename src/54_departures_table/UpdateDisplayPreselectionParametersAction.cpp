////////////////////////////////////////////////////////////////////////////////
/// UpdateDisplayPreselectionParametersAction class implementation.
///	@file UpdateDisplayPreselectionParametersAction.cpp
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

#include "UpdateDisplayPreselectionParametersAction.h"
#include "DisplayScreenTableSync.h"
#include "ArrivalDepartureTableLog.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"
#include "DBLogModule.h"
#include "ArrivalDepartureTableRight.h"
#include "Conversion.h"
#include "ObjectNotFoundException.h"
#include "RequestMissingParameterException.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace server;
	using namespace util;
	using namespace dblog;
	using namespace security;

	template<> const string util::FactorableTemplate<Action, departurestable::UpdateDisplayPreselectionParametersAction>::FACTORY_KEY("udpp");

	namespace departurestable
	{
		const string UpdateDisplayPreselectionParametersAction::PARAMETER_DISPLAY_SCREEN(Action_PARAMETER_PREFIX + "ds");
		const string UpdateDisplayPreselectionParametersAction::PARAMETER_PRESELECTION_DELAY = Action_PARAMETER_PREFIX + "pd";
		const string UpdateDisplayPreselectionParametersAction::PARAMETER_CLEANING_DELAY = Action_PARAMETER_PREFIX + "cd";
		const string UpdateDisplayPreselectionParametersAction::PARAMETER_DISPLAY_FUNCTION(Action_PARAMETER_PREFIX + "fu");
		const string UpdateDisplayPreselectionParametersAction::PARAMETER_DISPLAY_END_FILTER = Action_PARAMETER_PREFIX + "ef";
		const string UpdateDisplayPreselectionParametersAction::PARAMETER_DISPLAY_MAX_DELAY = Action_PARAMETER_PREFIX + "md";



		ParametersMap UpdateDisplayPreselectionParametersAction::getParametersMap() const
		{
			ParametersMap map;
			if (_screen.get() != NULL) map.insert(PARAMETER_DISPLAY_SCREEN, _screen->getKey());
			return map;
		}

		void UpdateDisplayPreselectionParametersAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				setScreenId(map.getUid(PARAMETER_DISPLAY_SCREEN, true, FACTORY_KEY));
				_cleaningDelay = map.getInt(PARAMETER_CLEANING_DELAY, true, FACTORY_KEY);
				_maxDelay = map.getInt(PARAMETER_DISPLAY_MAX_DELAY, true, FACTORY_KEY);
				_function = static_cast<DisplayFunction>(map.getInt(PARAMETER_DISPLAY_FUNCTION, true, FACTORY_KEY));
				_preselectionDelay = map.getInt(PARAMETER_PRESELECTION_DELAY, false, FACTORY_KEY);
				_endFilter = static_cast<EndFilter>(map.getInt(PARAMETER_DISPLAY_END_FILTER, true, FACTORY_KEY));
			}
			catch (RequestMissingParameterException& e)
			{
				throw ActionException(e.getMessage());
			}
		}

		void UpdateDisplayPreselectionParametersAction::run()
		{
			stringstream t;
			
			// Function
			DBLogModule::appendToLogIfChange(t, "Fonction", GetFunctionList()[GetFunction(*_screen)], GetFunctionList()[_function]);
			switch (_function)
			{
			case DEPARTURES_CHRONOLOGICAL:
			case ARRIVAL_CHRONOLOGICAL:
				_screen->setGenerationMethod(DisplayScreen::STANDARD_METHOD);
				break;

			case DEPARTURES_PRESELECTION:
			case ARRIVAL_PRESELECTION:
				_screen->setGenerationMethod(DisplayScreen::WITH_FORCED_DESTINATIONS_METHOD);
				break;

			case ROUTE_PLANNING:
				_screen->setGenerationMethod(DisplayScreen::ROUTE_PLANNING);
				break;
			}
			switch (_function)
			{
			case DEPARTURES_CHRONOLOGICAL:
			case DEPARTURES_PRESELECTION:
			case ROUTE_PLANNING:
				_screen->setDirection(DISPLAY_DEPARTURES);
				break;

			case ARRIVAL_CHRONOLOGICAL:
			case ARRIVAL_PRESELECTION:
				_screen->setDirection(DISPLAY_ARRIVALS);
				break;
			}
			
			// Preselection delay
			if (_preselectionDelay > 0)
			{
				DBLogModule::appendToLogIfChange(
					t,
					"Délai de préselection",
					_screen->getForceDestinationDelay(),
					_preselectionDelay
				);
				_screen->setDestinationForceDelay(_preselectionDelay);
			}
			
			// Terminus
			DBLogModule::appendToLogIfChange(
				t, "Affichage des terminus seulement", _screen->getEndFilter(), _endFilter
			);
			_screen->setOriginsOnly(_endFilter);
			
			// Cleaning delay
			DBLogModule::appendToLogIfChange(
				t, "Délai d'effacement", _screen->getClearingDelay(), _cleaningDelay
			);
			_screen->setClearingDelay(_cleaningDelay);
			
			// Max delay
			DBLogModule::appendToLogIfChange(t, "Délai d'apparition", _screen->getMaxDelay(), _maxDelay);
			_screen->setMaxDelay(_maxDelay);
			
			// Saving
			DisplayScreenTableSync::Save(_screen.get());
			ArrivalDepartureTableLog::addUpdateEntry(*_screen, t.str(), *_request->getUser());
		}



		UpdateDisplayPreselectionParametersAction::DisplayFunctionNames UpdateDisplayPreselectionParametersAction::GetFunctionList(
		){
			map<DisplayFunction, string> directionMap;
			directionMap.insert(make_pair(DEPARTURES_CHRONOLOGICAL, "Départs chronologiques"));
			directionMap.insert(make_pair(DEPARTURES_PRESELECTION, "Départs avec présélection"));
			directionMap.insert(make_pair(ARRIVAL_CHRONOLOGICAL, "Arrivées chronologiques"));
			directionMap.insert(make_pair(ARRIVAL_PRESELECTION, "Arrivées avec présélection"));
			directionMap.insert(make_pair(ROUTE_PLANNING, "Calcul d'itinéraire"));
			return directionMap;
		}



		map<int, std::string> UpdateDisplayPreselectionParametersAction::GetClearDelaysList(
		){
			map<int, string> clearDelayMap;
			for (int i=-5; i<-1; ++i)
			{
				clearDelayMap.insert(make_pair(i, Conversion::ToString(-i) + " minutes avant le départ"));
			}
			clearDelayMap.insert(make_pair(-1, "1 minute avant le départ"));
			clearDelayMap.insert(make_pair(0, "heure du départ"));
			clearDelayMap.insert(make_pair(1, "1 minute après le départ"));
			for (int i=2; i<6; ++i)
			{
				clearDelayMap.insert(make_pair(i, Conversion::ToString(i) + " minutes après le départ"));
			}
			return clearDelayMap;
		}



		UpdateDisplayPreselectionParametersAction::DisplayFunction UpdateDisplayPreselectionParametersAction::GetFunction(
			const DisplayScreen& screen
		){
			if(screen.getGenerationMethod() == DisplayScreen::ROUTE_PLANNING) return UpdateDisplayPreselectionParametersAction::ROUTE_PLANNING;
			if(screen.getGenerationMethod() == DisplayScreen::STANDARD_METHOD) return (screen.getDirection() == DISPLAY_DEPARTURES) ? DEPARTURES_CHRONOLOGICAL : ARRIVAL_CHRONOLOGICAL;
			return (screen.getDirection() == DISPLAY_DEPARTURES) ? DEPARTURES_PRESELECTION : ARRIVAL_PRESELECTION;
		}



		bool UpdateDisplayPreselectionParametersAction::_isAuthorized(
		) const {
			assert(_screen.get() != NULL);
			return _request->isAuthorized<ArrivalDepartureTableRight>(
				WRITE,
				UNKNOWN_RIGHT_LEVEL,
				Conversion::ToString(_screen->getKey())
			);
		}



		void UpdateDisplayPreselectionParametersAction::setScreenId(
			const util::RegistryKeyType id
		){
			try
			{
				_screen = DisplayScreenTableSync::GetEditable(id, _env);
			}
			catch (ObjectNotFoundException<DisplayScreen>& e)
			{
				throw ActionException("display screen", id, FACTORY_KEY, e);
			}
		}
	}
}
