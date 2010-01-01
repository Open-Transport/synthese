
/** PTRoutePlannerResult class implementation.
	@file PTRoutePlannerResult.cpp

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

#include "PTRoutePlannerResult.h"
#include "Journey.h"
#include "Edge.h"
#include "Crossing.h"
#include "ResultHTMLTable.h"
#include "LineStop.h"
#include "Road.h"
#include "City.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "CommercialLine.h"
#include "RoadPlace.h"
#include "Line.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace graph;
	using namespace road;
	using namespace geography;
	using namespace algorithm;
	using namespace html;
	using namespace time;
	using namespace env;

	namespace ptrouteplanner
	{
		bool PTRoutePlannerResult::getSamePlaces() const
		{
			return _samePlaces;
		}



		const PTRoutePlannerResult::Journeys& PTRoutePlannerResult::getJourneys() const
		{
			return _journeys;
		}



		const PTRoutePlannerResult::PlaceList& PTRoutePlannerResult::getOrderedPlaces() const
		{
			return _orderedPlaces;
		}



		PTRoutePlannerResult::PTRoutePlannerResult(
			const geography::Place* departurePlace,
			const geography::Place* arrivalPlace,
			bool samePlaces,
			const TimeSlotRoutePlanner::Result& journeys
		):	_departurePlace(departurePlace),
			_arrivalPlace(arrivalPlace),
			_samePlaces(samePlaces),
			_journeys(journeys)
		{
			// Variables locales
			size_t i;
			optional<size_t> dernieri;

			// Allocation
			_orderedPlaces.clear();

			// Horizontal loop
			for (Journeys::const_iterator it(_journeys.begin()); it != _journeys.end(); ++it)
			{
				i = 0;
				dernieri = optional<size_t>();

				// Vertical loop
				const Journey::ServiceUses& jl(it->getServiceUses());
				for (Journey::ServiceUses::const_iterator itl(jl.begin()); itl != jl.end(); ++itl)
				{
					const ServiceUse& curET(*itl);

					// Search of the place from the preceding one
					if (itl == jl.begin())
					{
						const NamedPlace* placeToSearch(
							dynamic_cast<const Crossing*>(curET.getDepartureEdge()->getHub()) ?
							dynamic_cast<const NamedPlace*>(_departurePlace) :
							dynamic_cast<const NamedPlace*>(curET.getDepartureEdge()->getHub())
						);
						assert(placeToSearch);

						if(_ordrePARechercheGare(i, placeToSearch))
						{
							if (dernieri && i < *dernieri )
								i = _ordrePAEchangeSiPossible(*dernieri, i );
						}
						else
						{
							i = _ordrePAInsere(placeToSearch, dernieri ? *dernieri + 1 : 0, itl == jl.begin(), false);
						}

						dernieri = i;
						++i;
					}

					const NamedPlace* placeToSearch(
						dynamic_cast<const Crossing*>(curET.getArrivalEdge()->getHub()) ?
						(itl == jl.end()-1 ? dynamic_cast<const NamedPlace*>(_arrivalPlace) : NULL) :
						dynamic_cast<const NamedPlace*>(curET.getArrivalEdge()->getHub())
					);
					if(placeToSearch)
					{
						if(_ordrePARechercheGare(i, placeToSearch))
						{
							if (dernieri && i < *dernieri )
							{
								i = _ordrePAEchangeSiPossible(*dernieri, i );
							}
						}
						else
						{
							i = _ordrePAInsere(placeToSearch, dernieri ? *dernieri + 1 : 0, false, itl == jl.end()-1);
						}
						dernieri = i;
					}
				}
			}
		}



		void PTRoutePlannerResult::operator=( const PTRoutePlannerResult& other )
		{
			assert(_departurePlace == other._departurePlace);
			assert(_arrivalPlace == other._arrivalPlace);
			_journeys = other._journeys;
			_orderedPlaces = other._orderedPlaces;
			_samePlaces = other._samePlaces;
		}



		bool PTRoutePlannerResult::_ordrePARechercheGare(
			size_t& i,
			const geography::NamedPlace* GareAChercher
		){
			// Recherche de la gare en suivant ï¿ partir de la position i
			for(;
				i < _orderedPlaces.size() && _orderedPlaces[i].place != NULL && _orderedPlaces[ i ].place != GareAChercher;
				++i
			);

			// Gare trouvï¿e en suivant avant la fin du tableau
			if (i < _orderedPlaces.size() && _orderedPlaces[i].place != NULL )
				return true;

			// Recherche de position antï¿rieure ï¿ i
			for(i = 0;
				i < _orderedPlaces.size() && _orderedPlaces[ i ].place != NULL && _orderedPlaces[i].place != GareAChercher;
				++i
			);

			return i < _orderedPlaces.size() && _orderedPlaces[ i ].place != NULL;
		}



		std::vector<bool> PTRoutePlannerResult::_ordrePAConstruitLignesAPermuter(
			const graph::Journey& __TrajetATester,
			size_t LigneMax
		) const {
			vector<bool> result;
			size_t l(0);
			const ServiceUse* curET((l >= __TrajetATester.size()) ? NULL : &__TrajetATester.getJourneyLeg (l));
			for (size_t i(0); i <= LigneMax && _orderedPlaces[ i ].place != NULL; i++ )
			{
				if(	curET != NULL &&
					_orderedPlaces[i].place == dynamic_cast<const NamedPlace*>(curET->getDepartureEdge()->getHub())
				){
					result.push_back(true);
					++l;
					curET = (l >= __TrajetATester.size()) ? NULL : &__TrajetATester.getJourneyLeg (l);
				}
				else
				{
					result.push_back(false);
				}
			}
			return result;
		}



		std::size_t PTRoutePlannerResult::_ordrePAInsere(
			const geography::NamedPlace* place,
			std::size_t position,
			bool isLockedAtTheTop,
			bool isLockedAtTheEnd
		){
			if (isLockedAtTheEnd)
				position = _orderedPlaces.size();
			else if (isLockedAtTheTop)
				position = 0;
			else
				for (; position < _orderedPlaces.size() && _orderedPlaces[position].isOrigin; ++position);

			// Insertion
			PlaceInformation pi;
			pi.isOrigin = isLockedAtTheTop;
			pi.isDestination = isLockedAtTheEnd;
			pi.place = place;

			_orderedPlaces.insert(_orderedPlaces.begin() + position, pi);

			// Retour de la position choisie
			return position;
		}



		std::size_t PTRoutePlannerResult::_ordrePAEchangeSiPossible(
			std::size_t PositionActuelle,
			std::size_t PositionSouhaitee
		){
			vector<bool> LignesAPermuter(PositionActuelle + 1, false);
			bool Echangeable(true);
			PlaceInformation tempGare;
			size_t i;
			size_t j;

			// Construction de l'ensemble des lignes a permuter
			LignesAPermuter[ PositionActuelle ] = true;
			for (Journeys::const_iterator it = _journeys.begin(); it != _journeys.end(); ++it)
			{
				vector<bool> curLignesET = _ordrePAConstruitLignesAPermuter(*it, PositionActuelle );
				for ( i = PositionActuelle; i > PositionSouhaitee; --i)
					if ( curLignesET[ i ] && LignesAPermuter[ i ] )
						break;
				for ( ; i > PositionSouhaitee; --i)
					if ( curLignesET[ i ] )
						LignesAPermuter[ i ] = true;
			}

			// Tests d'ï¿changeabilitï¿ binaire
			// A la premiere contradiction on s'arrete
			for(Journeys::const_iterator it = _journeys.begin(); it != _journeys.end(); ++it)
			{
				vector<bool> curLignesET = _ordrePAConstruitLignesAPermuter(*it, PositionActuelle );
				i = PositionSouhaitee;
				for ( j = PositionSouhaitee; true; ++j)
				{
					for (; i<LignesAPermuter.size() && !LignesAPermuter[i]; ++i);

					if ( i > PositionActuelle )
						break;

					if ( curLignesET[ i ] && curLignesET[ j ] && !LignesAPermuter[ j ] )
					{
						Echangeable = false;
						break;
					}
					i++;
				}
				if ( !Echangeable )
					break;
			}

			// Echange ou insertion
			if ( Echangeable )
			{
				for ( j = 0; true; ++j)
				{
					for(i = j; i < LignesAPermuter.size() && !LignesAPermuter[ i ] && i <= PositionActuelle; ++i);

					if ( i > PositionActuelle )
						break;

					LignesAPermuter[ i ] = false;

					tempGare = _orderedPlaces[ i ];
					for ( ; i > PositionSouhaitee + j; --i)
						_orderedPlaces[i] = _orderedPlaces[i-1];

					_orderedPlaces[ i ] = tempGare;
				}
				return PositionSouhaitee + j;
			}
			else
				return _ordrePAInsere(_orderedPlaces[ PositionSouhaitee ].place, PositionActuelle + 1, false, false);
		}



		const geography::Place* PTRoutePlannerResult::getDeparturePlace() const
		{
			return _departurePlace;
		}



		const geography::Place* PTRoutePlannerResult::getArrivalPlace() const
		{
			return _arrivalPlace;
		}



		void PTRoutePlannerResult::displayHTMLTable(
			ostream& stream,
			optional<HTMLForm&> resaForm,
			const string& resaRadioFieldName
		) const {

			if(_journeys.empty())
			{
				stream << "Aucun résultat trouvé de " << (
						dynamic_cast<const NamedPlace*>(_departurePlace) ?
						dynamic_cast<const NamedPlace*>(_departurePlace)->getFullName() :
						dynamic_cast<const City*>(_departurePlace)->getName()
					) << " à " << (
						dynamic_cast<const NamedPlace*>(_arrivalPlace) ?
						dynamic_cast<const NamedPlace*>(_arrivalPlace)->getFullName() :
						dynamic_cast<const City*>(_arrivalPlace)->getName()
					);
				return;
			}

			HTMLTable::ColsVector v;
			v.push_back("Départ<br />" + (
					dynamic_cast<const NamedPlace*>(_departurePlace) ?
					dynamic_cast<const NamedPlace*>(_departurePlace)->getFullName() :
					dynamic_cast<const City*>(_departurePlace)->getName()
			)	);
			v.push_back("Ligne");
			v.push_back("Arrivée");
			v.push_back("Correspondance");
			v.push_back("Départ");
			v.push_back("Ligne");
			v.push_back("Arrivée<br />" + (
					dynamic_cast<const NamedPlace*>(_arrivalPlace) ?
					dynamic_cast<const NamedPlace*>(_arrivalPlace)->getFullName() :
					dynamic_cast<const City*>(_arrivalPlace)->getName()
			)	);
			HTMLTable t(v, ResultHTMLTable::CSS_CLASS);

			// Solutions display loop
			int solution(1);
			DateTime now(TIME_CURRENT);
			stream << t.open();
			for (PTRoutePlannerResult::Journeys::const_iterator it(_journeys.begin()); it != _journeys.end(); ++it)
			{
				stream << t.row();
				stream << t.col(7, string(), true);
				if(	resaForm &&
					it->getReservationCompliance() &&
					it->getReservationDeadLine() > now
				){
					stream <<
						resaForm->getRadioInput(
							resaRadioFieldName,
							it->getDepartureTime(),
							(solution==1) ? it->getDepartureTime() : DateTime(UNKNOWN_VALUE),
							" Solution "+ lexical_cast<string>(solution)
						)
					;
				}
				else
					stream << "Solution " << solution;
				++solution;

				// Departure time
				Journey::ServiceUses::const_iterator its(it->getServiceUses().begin());

				if (it->getContinuousServiceRange() > 1)
				{
					DateTime endRange(its->getDepartureDateTime());
					endRange += it->getContinuousServiceRange();
					stream << " - Service continu jusqu'à " << endRange.toString();
				}
				if (it->getReservationCompliance() == true)
				{
					stream << " - " << HTMLModule::getHTMLImage("resa_compulsory.png", "Réservation obligatoire") << " Réservation obligatoire avant le " << it->getReservationDeadLine().toString();
				}
				if (it->getReservationCompliance() == boost::logic::indeterminate)
				{
					stream << " - " << HTMLModule::getHTMLImage("resa_optional.png", "Réservation facultative") << " Réservation facultative avant le " << it->getReservationDeadLine().toString();
				}
				if(dynamic_cast<const City*>(_departurePlace) || dynamic_cast<const City*>(_arrivalPlace))
				{
					stream << " (";
					if(dynamic_cast<const City*>(_departurePlace))
					{
						stream << "départ de " << 
							static_cast<const PublicTransportStopZoneConnectionPlace*>(
								its->getDepartureEdge()->getHub()
							)->getFullName()
						;
					}
					if(dynamic_cast<const City*>(_arrivalPlace))
					{
						if(dynamic_cast<const City*>(_departurePlace)) stream << " - ";
						Journey::ServiceUses::const_iterator ite(it->getServiceUses().end() - 1);
						stream << "arrivée à " << 
							static_cast<const PublicTransportStopZoneConnectionPlace*>(
								ite->getArrivalEdge()->getHub()
							)->getFullName()
						;
					}
					stream << ")";
				}


				stream << t.row();
				stream << t.col() << "<b>" << its->getDepartureDateTime().toString() << "</b>";

				// Line
				const LineStop* ls(dynamic_cast<const LineStop*>(its->getEdge()));
				const Road* road(dynamic_cast<const Road*>(its->getEdge()->getParentPath()));
				stream << t.col(1, ls ? ls->getLine()->getCommercialLine()->getStyle() : string());
				stream << (ls ? ls->getLine()->getCommercialLine()->getShortName() : road->getRoadPlace()->getName());

				// Transfers
				if (its == it->getServiceUses().end() -1)
				{
					stream << t.col(4) << "(trajet direct)";
				}
				else
				{
					while(true)
					{
						// Arrival
						stream << t.col() << its->getArrivalDateTime().toString();

						// Place
						stream << t.col();
						if(dynamic_cast<const PublicTransportStopZoneConnectionPlace*>(its->getArrivalEdge()->getHub()))
						{
							stream <<
								static_cast<const PublicTransportStopZoneConnectionPlace*>(
									its->getArrivalEdge()->getHub()
								)->getFullName();
						}

						// Next service use
						++its;

						// Departure
						stream << t.col() << its->getDepartureDateTime().toString();

						// Line
						const LineStop* ls(dynamic_cast<const LineStop*>(its->getEdge()));
						const Road* road(dynamic_cast<const Road*>(its->getEdge()->getParentPath()));
						stream << t.col(1, ls ? ls->getLine()->getCommercialLine()->getStyle() : string());
						stream << (ls ? ls->getLine()->getCommercialLine()->getShortName() : road->getRoadPlace()->getName());

						// Exit if last service use
						if (its == it->getServiceUses().end() -1)
							break;

						// Empty final arrival col
						stream << t.col();

						// New row and empty origin departure cols;
						stream << t.row();
						stream << t.col();
						stream << t.col();
					}
				}

				// Final arrival
				stream << t.col() << "<b>" << its->getArrivalDateTime().toString() << "</b>";
			}
			stream << t.close();

		}
	}
}
