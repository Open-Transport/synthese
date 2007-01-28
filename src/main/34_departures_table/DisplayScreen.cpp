
/** DisplayScreen class implementation.
	@file DisplayScreen.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include "34_departures_table/DisplayScreen.h"
#include "34_departures_table/DisplayType.h"

namespace synthese
{
	using namespace util;
	using namespace env;
	using namespace time;

	namespace departurestable
	{
		DisplayScreen::DisplayScreen(const uid& id, GenerationMethod generationMethod)
			: Registrable<uid, DisplayScreen>(id)
			, _generationMethod(generationMethod)
			, _originsOnly(WITH_PASSING)
			, _localization(NULL)
			, _destinationForceDelay(120)	// default = 2 hours
			, _maxDelay(24 * 60)			// default = 24 hours
			, _displayType(NULL)
		{
		}

		DisplayScreen::~DisplayScreen(void)
		{
		}


		void DisplayScreen::setDestinationForceDelay(int delay)
		{
			_destinationForceDelay = delay;
		}

		void DisplayScreen::SetOriginesSeulement(const EndFilter& __Valeur)
		{
			_originsOnly = __Valeur;
		}

		void DisplayScreen::setMaxDelay(int maxDelay)
		{
			_maxDelay = maxDelay;
		}

		void DisplayScreen::addForbiddenPlace(const ConnectionPlace* place)
		{
			_forbiddenArrivalPlaces.insert(place);
		}


		/** Modificateur du point d'arrêt.
		*/
		void DisplayScreen::setLocalization(const BroadcastPoint* bp)
		{
			_localization = bp;
		}



		/** Ajout de quai à afficher. 

			Si aucun quai ajouté alors tous les quais du point d'arrêt sont affichés.
		*/
		void DisplayScreen::AddQuaiAutorise(const PhysicalStop* physicalStop)
		{
			_physicalStops.insert(physicalStop);
		}


		void DisplayScreen::SetTitre(const std::string&__Titre)
		{
			_title = __Titre;
		}

		void DisplayScreen::AddDestinationAffichee(const ConnectionPlace* __PointArret)
		{
			_displayedPlaces.insert(__PointArret);
		}

		const std::string& DisplayScreen::getTitre() const
		{
			return _title;
		}



		/** Modificateur numéro du panneau.
			@param __NumeroPanneau Numéro du panneau devant afficher les résultats
		*/
		void DisplayScreen::SetNumeroPanneau(int __NumeroPanneau)
		{
			_wiringCode = __NumeroPanneau;
		}


		void DisplayScreen::addForcedDestination(const ConnectionPlace* place)
		{
			_forcedDestinations.insert(place);
		}


		/** Accesseur numéro du panneau.
			@return Numéro du panneau
		*/
		int DisplayScreen::getNumeroPanneau() const
		{
			return _wiringCode;
		}


		ArrivalDepartureTableGenerator* DisplayScreen::getGenerator(const DateTime& startDateTime) const
		{
			// End time
			DateTime endDateTime = startDateTime;
			endDateTime += _maxDelay;

			// Construction of the generator
			switch (_generationMethod)
			{
			case STANDARD_METHOD:
				return (ArrivalDepartureTableGenerator*) new StandardArrivalDepartureTableGenerator(
					_physicalStops
					, _direction
					, _originsOnly
					, _forbiddenLines
					, _displayedPlaces
					, _forbiddenArrivalPlaces
					, startDateTime
					, endDateTime
					, _displayType->getRowNumber()
				);

			case WITH_FORCED_DESTINATIONS_METHOD:
				return (ArrivalDepartureTableGenerator*) new ForcedDestinationsArrivalDepartureTableGenerator(
					_physicalStops
					, _direction
					, _originsOnly
					, _forbiddenLines
					, _displayedPlaces
					, _forbiddenArrivalPlaces
					, startDateTime
					, endDateTime
					, _displayType->getRowNumber()
					, _forcedDestinations
					, _destinationForceDelay
				);
			}
		}

	}
}
