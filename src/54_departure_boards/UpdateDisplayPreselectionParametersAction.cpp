////////////////////////////////////////////////////////////////////////////////
/// UpdateDisplayPreselectionParametersAction class implementation.
///	@file UpdateDisplayPreselectionParametersAction.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "UpdateDisplayPreselectionParametersAction.h"

#include "Profile.h"
#include "Session.h"
#include "User.h"
#include "DisplayScreenTableSync.h"
#include "ArrivalDepartureTableLog.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"
#include "DBLogModule.h"
#include "ArrivalDepartureTableRight.h"
#include "ObjectNotFoundException.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace server;
	using namespace util;
	using namespace dblog;
	using namespace security;

	template<> const string util::FactorableTemplate<Action, departure_boards::UpdateDisplayPreselectionParametersAction>::FACTORY_KEY("udpp");

	namespace departure_boards
	{
		const string UpdateDisplayPreselectionParametersAction::PARAMETER_DISPLAY_SCREEN(Action_PARAMETER_PREFIX + "ds");
		const string UpdateDisplayPreselectionParametersAction::PARAMETER_PRESELECTION_DELAY = Action_PARAMETER_PREFIX + "pd";
		const string UpdateDisplayPreselectionParametersAction::PARAMETER_CLEANING_DELAY = Action_PARAMETER_PREFIX + "cd";
		const string UpdateDisplayPreselectionParametersAction::PARAMETER_DISPLAY_FUNCTION(Action_PARAMETER_PREFIX + "fu");
		const string UpdateDisplayPreselectionParametersAction::PARAMETER_DISPLAY_END_FILTER = Action_PARAMETER_PREFIX + "ef";
		const string UpdateDisplayPreselectionParametersAction::PARAMETER_DISPLAY_MAX_DELAY = Action_PARAMETER_PREFIX + "md";
		const string UpdateDisplayPreselectionParametersAction::PARAMETER_MAX_TRANSFER_DURATION = Action_PARAMETER_PREFIX + "max_transfer_duration";
		const string UpdateDisplayPreselectionParametersAction::PARAMETER_ALLOW_CANCELED = Action_PARAMETER_PREFIX + "allow_canceled";



		ParametersMap UpdateDisplayPreselectionParametersAction::getParametersMap() const
		{
			ParametersMap map;
			if (_screen.get() != NULL)
			{
				map.insert(PARAMETER_DISPLAY_SCREEN, _screen->getKey());
			}
			if(_allowCanceled)
			{
				map.insert(PARAMETER_ALLOW_CANCELED, *_allowCanceled);
			}
			return map;
		}

		void UpdateDisplayPreselectionParametersAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				setScreenId(map.get<RegistryKeyType>(PARAMETER_DISPLAY_SCREEN));
				_cleaningDelay = map.get<int>(PARAMETER_CLEANING_DELAY);
				_maxDelay = map.get<int>(PARAMETER_DISPLAY_MAX_DELAY);
				_maxTransferDuration = map.get<int>(PARAMETER_MAX_TRANSFER_DURATION);
				_function = static_cast<DisplayFunction>(map.get<int>(PARAMETER_DISPLAY_FUNCTION));
				_preselectionDelay = map.getOptional<int>(PARAMETER_PRESELECTION_DELAY);
				_endFilter = static_cast<EndFilter>(map.get<int>(PARAMETER_DISPLAY_END_FILTER));
				if(map.isDefined(PARAMETER_ALLOW_CANCELED))
				{
					_allowCanceled = map.get<bool>(PARAMETER_ALLOW_CANCELED);
				}
			}
			catch (ParametersMap::MissingParameterException& e)
			{
				throw ActionException(e, *this);
			}
		}

		void UpdateDisplayPreselectionParametersAction::run(Request& request)
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

			case ROUTE_PLANNING_WITH_TRANSFER:
			case ROUTE_PLANNING_WITHOUT_TRANSFER:
				_screen->setGenerationMethod(DisplayScreen::ROUTE_PLANNING);
				break;

			case DISPLAY_CHILDREN_ONLY:
				_screen->setGenerationMethod(DisplayScreen::DISPLAY_CHILDREN_ONLY);
				break;
			}
			switch (_function)
			{
			case DEPARTURES_CHRONOLOGICAL:
			case DEPARTURES_PRESELECTION:
			case ROUTE_PLANNING_WITHOUT_TRANSFER:
			case ROUTE_PLANNING_WITH_TRANSFER:
			case DISPLAY_CHILDREN_ONLY:
				_screen->setDirection(DISPLAY_DEPARTURES);
				break;

			case ARRIVAL_CHRONOLOGICAL:
			case ARRIVAL_PRESELECTION:
				_screen->setDirection(DISPLAY_ARRIVALS);
				break;
			}

			switch (_function)
			{
			case ROUTE_PLANNING_WITHOUT_TRANSFER:
				_screen->set<RoutePlanningWithTransfer>(false);
				break;

			case ROUTE_PLANNING_WITH_TRANSFER:
				_screen->set<RoutePlanningWithTransfer>(true);
				break;
				
			default:
				break;
			}

			// Preselection delay
			if (_preselectionDelay)
			{
				DBLogModule::appendToLogIfChange(
					t,
					"Délai de préselection",
					_screen->get<DestinationForceDelay>(),
					*_preselectionDelay
				);
				_screen->set<DestinationForceDelay>(*_preselectionDelay);
			}

			// Terminus
			DBLogModule::appendToLogIfChange(
				t, "Affichage des terminus seulement", _screen->getEndFilter(), _endFilter
			);
			_screen->setOriginsOnly(_endFilter);

			// Cleaning delay
			DBLogModule::appendToLogIfChange(
				t, "Délai d'effacement", _screen->get<ClearingDelay>(), _cleaningDelay
			);
			_screen->set<ClearingDelay>(_cleaningDelay);

			// Max delay
			DBLogModule::appendToLogIfChange(t, "Délai d'apparition", _screen->get<MaxDelay>(), _maxDelay);
			_screen->set<MaxDelay>(_maxDelay);

			// Max Transfer duration
			DBLogModule::appendToLogIfChange(t, "Délai de correspondance", _screen->get<MaxTransferDuration>(), _maxTransferDuration);
			_screen->set<MaxTransferDuration>(_maxTransferDuration);

			// Allow canceled
			if(_allowCanceled)
			{
				DBLogModule::appendToLogIfChange(t, "Afficher les services supprmés", _screen->get<AllowCanceled>(), *_allowCanceled);
				_screen->set<AllowCanceled>(*_allowCanceled);
			}

			// Saving
			DisplayScreenTableSync::Save(_screen.get());
			ArrivalDepartureTableLog::addUpdateEntry(*_screen, t.str(), *request.getUser());
		}



		UpdateDisplayPreselectionParametersAction::DisplayFunctionNames UpdateDisplayPreselectionParametersAction::GetFunctionList(
		){
			DisplayFunctionNames directionMap;
			directionMap.insert(make_pair(DEPARTURES_CHRONOLOGICAL, "Départs chronologiques"));
			directionMap.insert(make_pair(DEPARTURES_PRESELECTION, "Départs avec présélection"));
			directionMap.insert(make_pair(ARRIVAL_CHRONOLOGICAL, "Arrivées chronologiques"));
			directionMap.insert(make_pair(ARRIVAL_PRESELECTION, "Arrivées avec présélection"));
			directionMap.insert(make_pair(ROUTE_PLANNING_WITH_TRANSFER, "Calcul d'itinéraire avec correspondance"));
			directionMap.insert(make_pair(ROUTE_PLANNING_WITHOUT_TRANSFER, "Calcul d'itinéraire sans correspondance"));
			directionMap.insert(make_pair(DISPLAY_CHILDREN_ONLY, "Affichage des fils uniquement"));
			return directionMap;
		}



		UpdateDisplayPreselectionParametersAction::DelaysList UpdateDisplayPreselectionParametersAction::GetClearDelaysList(
		){
			DelaysList clearDelayMap;
			for (int i=-15; i<-1; ++i)
			{
				clearDelayMap.insert(make_pair(i, lexical_cast<string>(-i) + " minutes avant le départ"));
			}
			clearDelayMap.insert(make_pair(-1, "1 minute avant le départ"));
			clearDelayMap.insert(make_pair(0, "heure du départ"));
			clearDelayMap.insert(make_pair(1, "1 minute après le départ"));
			for (int i=2; i<16; ++i)
			{
				clearDelayMap.insert(make_pair(i, lexical_cast<string>(i) + " minutes après le départ"));
			}
			return clearDelayMap;
		}



		UpdateDisplayPreselectionParametersAction::DisplayFunction UpdateDisplayPreselectionParametersAction::GetFunction(
			const DisplayScreen& screen
		){
			if(screen.getGenerationMethod() == DisplayScreen::ROUTE_PLANNING)
			{
				return screen.get<RoutePlanningWithTransfer>() ? UpdateDisplayPreselectionParametersAction::ROUTE_PLANNING_WITH_TRANSFER : UpdateDisplayPreselectionParametersAction::ROUTE_PLANNING_WITHOUT_TRANSFER;
			}
			if(screen.getGenerationMethod() == DisplayScreen::STANDARD_METHOD)
			{
				return (screen.getDirection() == DISPLAY_DEPARTURES) ? DEPARTURES_CHRONOLOGICAL : ARRIVAL_CHRONOLOGICAL;
			}
			return (screen.getDirection() == DISPLAY_DEPARTURES) ? DEPARTURES_PRESELECTION : ARRIVAL_PRESELECTION;
		}



		bool UpdateDisplayPreselectionParametersAction::isAuthorized(const Session* session
		) const {
			assert(_screen.get() != NULL);
			if (_screen->getLocation() != NULL)
			{
				return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ArrivalDepartureTableRight>(WRITE, UNKNOWN_RIGHT_LEVEL, lexical_cast<string>(_screen->getLocation()->getKey()));
			}
			else
			{
				return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ArrivalDepartureTableRight>(WRITE);
			}
		}



		void UpdateDisplayPreselectionParametersAction::setScreenId(
			const util::RegistryKeyType id
		){
			try
			{
				_screen = DisplayScreenTableSync::GetEditable(id, *_env);
			}
			catch (ObjectNotFoundException<DisplayScreen>& e)
			{
				throw ActionException("display screen", e, *this);
			}
		}
	}
}
