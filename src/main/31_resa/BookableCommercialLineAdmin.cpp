
/** BookableCommercialLineAdmin class implementation.
	@file BookableCommercialLineAdmin.cpp
	@author Hugues Romain
	@date 2008

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

#include "BookableCommercialLineAdmin.h"

#include "31_resa/ResaModule.h"
#include "31_resa/ServiceReservations.h"
#include "31_resa/ReservationTransactionTableSync.h"
#include "31_resa/ReservationTableSync.h"
#include "31_resa/ReservationTransaction.h"
#include "31_resa/Reservation.h"

#include "05_html/HTMLTable.h"

#include "15_env/CommercialLine.h"
#include "15_env/AdvancedSelectTableSync.h"
#include "15_env/CommercialLineTableSync.h"
#include "15_env/ScheduledService.h"
#include "15_env/ScheduledServiceTableSync.h"
#include "15_env/ReservationRule.h"

#include "30_server/QueryString.h"
#include "30_server/RequestException.h"

#include "32_admin/AdminParametersException.h"
#include "32_admin/ModuleAdmin.h"

#include <map>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace resa;
	using namespace time;
	using namespace env;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, BookableCommercialLineAdmin>::FACTORY_KEY("BookableCommercialLineAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<BookableCommercialLineAdmin>::ICON("chart_line.png");
		template<> const string AdminInterfaceElementTemplate<BookableCommercialLineAdmin>::DEFAULT_TITLE("Ligne inconnue");
	}

	namespace resa
	{
		const string BookableCommercialLineAdmin::PARAMETER_DATE("da");
		const string BookableCommercialLineAdmin::PARAMETER_DISPLAY_CANCELLED("dc");


		BookableCommercialLineAdmin::BookableCommercialLineAdmin()
			: AdminInterfaceElementTemplate<BookableCommercialLineAdmin>()
			, _startDateTime(TIME_CURRENT)
			, _endDateTime(TIME_CURRENT, TIME_CURRENT, TIME_CURRENT, TIME_MAX, TIME_MAX)
		{ }
		
		void BookableCommercialLineAdmin::setFromParametersMap(const ParametersMap& map)
		{

			// Date
			try
			{
				DateTime dateTime(map.getDateTime(PARAMETER_DATE, false, getFactoryKey()));
				if (!dateTime.isUnknown())
				{
					_startDateTime = dateTime;
					_endDateTime = DateTime(dateTime.getDate(), Hour(TIME_MAX));
				}
			}
			catch (...)
			{
				throw RequestException("Bad value for date");
			}


			uid lineId(map.getUid(QueryString::PARAMETER_OBJECT_ID, true, FACTORY_KEY));
			try
			{
				_line = CommercialLineTableSync::Get(lineId);
			}
			catch (...)
			{
				throw RequestException("Bad value for line ID");
			}
		
			_displayCancelled = map.getBool(PARAMETER_DISPLAY_CANCELLED, false, false, FACTORY_KEY);


		}
		
		void BookableCommercialLineAdmin::display(ostream& stream, VariablesMap& variables, const FunctionRequest<AdminRequest>* request) const
		{
			// Local variables
			DateTime now(TIME_CURRENT);

			// Temporary variables
			int seatsNumber(0);
			bool round_trip = false;
			bool next_overflow = false;
			bool course = false;

			HTMLTable t(0,"adminresults");
			stream << t.open();

			// Boucle sur les circulations
			vector<shared_ptr<ScheduledService> > services(ScheduledServiceTableSync::search(
					_line.get()
					, _startDateTime.getDate()
			)	);

			// Download reservations
			map<const ScheduledService*, ServiceReservations> reservations;
			for(vector<shared_ptr<ScheduledService> >::const_iterator it(services.begin()); it != services.end(); ++it)
			{
				const ScheduledService* service(it->get());
				const ReservationRule* rule(service->getReservationRule());
				ServiceReservations obj;
				obj.reservations = ReservationTransactionTableSync::search(
					service
					, _startDateTime.getDate()
					, _displayCancelled
					);
				obj.seatsNumber = 0;
				for (ServiceReservations::Reservations::const_iterator itr(obj.reservations.begin()); itr != obj.reservations.end(); ++itr)
					if ((*itr)->getCancellationTime().isUnknown())
						obj.seatsNumber += (*itr)->getSeats();

				obj.overflow = rule->getCapacity() && (obj.seatsNumber > rule->getCapacity());

				int lastDepartureLineStop(getRankOfLastDepartureLineStop(service->getPathId()));

				obj.status = rule->isReservationPossible(
					DateTime(_startDateTime.getDate(), service->getDepartureSchedule())
					, now
					, DateTime(_startDateTime.getDate(), service->getDepartureSchedule(lastDepartureLineStop))
					);
				obj.service = service;

				reservations.insert(make_pair(service, obj));

			}

			for	(vector<shared_ptr<ScheduledService> >::const_iterator it(services.begin()); it != services.end(); ++it)
			{
				ScheduledService* service(it->get());

				stream << t.row();
				stream << t.col(6, string(), true) << "Service " << service->getServiceNumber() << " - départ de " << service->getDepartureSchedule().getHour().toString();

				/*				if ((retour || course) && $circulation = $circulation_suivante)
				{
				$reservations_table = $next_reservations_table;
				$nombre_places = $next_nombre_places;
				$overflow = $next_overflow;
				$departure_date = $next_departure_date;
				}
				elseif($circulation = $circulations->GetSuivant())
				{
				*/					// Dates of departure
				DateTime originDateTime(DateTime(_startDateTime.getDate(), (*it)->getDepartureSchedule()));

				const ServiceReservations& serviceReservations (reservations[it->get()]);

				for (ServiceReservations::Reservations::const_iterator itr(serviceReservations.reservations.begin()); itr != serviceReservations.reservations.end(); ++itr)
				{
					ResaModule::DisplayReservation(stream, t, serviceReservations.getReservation(itr->get()).get());
				}
				/*				}
				else
				break;

				// Initialisation
				$course = false;
				$retour = false;

				// Chargement des objets
				$parcours_type = new cParcoursType($circulation->GetDonnee(CHAMP_PARCOURS_TYPE));
				$lieu_depart = $parcours_type->GetOrigine();


				// Handling of basis logical place (round trip organization)
				if ($this->GetDonnee(FIELD_BASIS_LOGICAL_STOP))
				{
				// Recherche de circulation retour sur ligne à arret de base
				if ($lieu_depart->GetID() == $this->GetDonnee(FIELD_BASIS_LOGICAL_STOP) or !$round_trip)
				{
				if ($circulation_suivante = $circulations->GetSuivant())
				{
				$course = true;
				$retour = (
				$circulation_suivante->GetDonnee(FIELD_CIRCULATION_NUMBER) 
				== $circulation->GetDonnee(FIELD_CIRCULATION_NUMBER) + 1
				);

				// Departure dates
				$next_departure_date = new Schedule($circulation_suivante->GetDonnee(FIELD_DEPARTURE_TIME));
				$next_departure_date = $next_departure_date->getDate($date_debut); 
				$last_departure_date = new Schedule($circulation_suivante->GetDonnee(FIELD_LAST_DEPARTURE_TIME));
				$last_departure_date = $last_departure_date->getDate($date_debut);

				// Reading of the reservation table
				$next_reservations_table = array();
				$next_nombre_places = 0;

				// Reservations loop
				$reservations = new cObjets(sqlGetCirculationReservations(
				$circulation_suivante->GetDonnee(CHAMP_PARCOURS_TYPE)
				, $circulation_suivante->GetDonnee(FIELD_CIRCULATION_NUMBER)
				, DateHeureSQL($next_departure_date)
				, DateHeureSQL($last_departure_date)
				));
				while ($reservation = $reservations->GetSuivant())
				{
				$arret_depart = $reservation->GetObjet('arret_depart');
				$arret_arrivee = $reservation->GetObjet('arret_arrivee');
				$nombre_places_page += $reservation->GetDonnee('Nombre_places');
				$next_nombre_places += $reservation->GetDonnee('Nombre_places');

				$next_reservations_table[] = array(
				$reservation->GetDonnee('Numéro')
				, $reservation->GetDonnee('Nom')
				, $reservation->GetDonnee('Nombre_places')
				, $reservation->GetDonnee('Téléphone')
				, $reservation->GetDonnee('Date_départ')
				, $arret_depart->GetNom()
				, $arret_arrivee->GetNom()
				);
				}
				$next_overflow = ($this->GetDonnee(FIELD_CAPACITY) and $next_nombre_places > $this->GetDonnee(FIELD_CAPACITY));

				$status = $circulation_suivante
				->GetObjet(FIELD_RESERVATION_RULE, true)
				->getReservationAbilityStatus($last_departure_date, time());

				$nombre_places_total = $nombre_places + $next_nombre_places;
				}

				$tableau->Ligne('titre2');
				$tableau->Colonne(1,1,getBackgroundCss($status == STATUS_TODO, $status == STATUS_OPTION, $status == STATUS_DONE));
				print '&nbsp;';

				$tableau->Colonne(7,1,'',''
				, $nombre_places_total 
				? 'background-color:'
				. (($overflow or $next_overflow) ? 'orange' : 'green')
				. ';'
				: ''
				);
				print "Course n°" . $circulation->GetDonnee(FIELD_CIRCULATION_NUMBER) .' - Feuille de route : '
				. $detail_screen->getHTMLPopupLink(
				'IMPRIMER'
				, array('circulation' => $circulation->GetID(), 'date' => date('Y-m-d', $date_debut), 'imprimer' => true)
				). '&nbsp;'
				. $detail_screen->getHTMLPopupLink(
				'VOIR'
				, array('circulation' => $circulation->GetID(), 'date' => date('Y-m-d', $date_debut), 'imprimer' => false)
				)
				;

				$round_trip = $lieu_depart->GetID() == $this->GetDonnee(FIELD_BASIS_LOGICAL_STOP);
				}
				}

				$tableau->Ligne('titre3');
				$tableau->Colonne(1,1,getBackgroundCss($status == STATUS_TODO, $status == STATUS_OPTION, $status == STATUS_DONE));
				print '&nbsp;';

				$tableau->Colonne(7, 1, '', ''
				, $nombre_places_total
				? 'background-color:'
				. (($overflow or $next_overflow) ? '#F0D0A0' : '#C0E8C0')
				. ';border-top-color:'
				. (($overflow or $next_overflow) ? '#F0D0A0' : '#C0E8C0')
				: ''
				);

				print
				'Service n°'. $circulation->GetDonnee(FIELD_CIRCULATION_NUMBER)
				.' - Départ de '. $circulation->GetDonnee('heure_depart') .' de '. $lieu_depart->GetNom() 
				.' à '. date('H:i', $departure_date)
				.($nombre_places ? ' - '. $nombre_places .' places réservées' : '')
				;

				if (!$this->GetDonnee(FIELD_BASIS_LOGICAL_STOP))
				print ' - Feuille de route : '
				. $detail_screen->getHTMLPopupLink(
				'IMPRIMER'
				, array('circulation' => $circulation->GetID(), 'date' => date('Y-m-d', $date_debut), 'imprimer' => true)
				). '&nbsp;'
				. $detail_screen->getHTMLPopupLink(
				'VOIR'
				, array('circulation' => $circulation->GetID(), 'date' => date('Y-m-d', $date_debut), 'imprimer' => false)
				)
				;

				if ($nombre_places)
				{
				foreach ($reservations_table as $reservation)
				{
				$tableau->Ligne();
				$tableau->Colonne(1,1,getBackgroundCss($status == STATUS_TODO, $status == STATUS_OPTION, $status == STATUS_DONE));
				print '&nbsp;';
				$tableau->Colonnes($reservation);
				}
				}
				else
				{
				$tableau->Ligne();
				$tableau->Colonne(1,1,getBackgroundCss($status == STATUS_TODO, $status == STATUS_OPTION, $status == STATUS_DONE));
				print '&nbsp;';
				$tableau->Colonne(7);
				print "Pas de réservation";
				}

				if ($course and !$retour)
				{
				$tableau->Ligne();
				$tableau->Colonne(1,1,getBackgroundCss($status == STATUS_TODO, $status == STATUS_OPTION, $status == STATUS_DONE));
				print '&nbsp;';
				$tableau->Colonne(7);
				print "Pas de retour";
				}
				*/			} // End services loop

			/*			$tableau->Fermer();

			if (!$temps_reel)
			{
			print '<p>'. $nombre_places_page .' places réservées au total.</p>';
			}
			*/
			stream << t.close();
		}

		bool BookableCommercialLineAdmin::isAuthorized(const FunctionRequest<AdminRequest>* request) const
		{
			/// @todo Implement the right control;
			return false;
		}
		
		AdminInterfaceElement::PageLinks BookableCommercialLineAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const	{
			AdminInterfaceElement::PageLinks links;
			/// @todo Implement it or leave empty
			// Example
			// if(parentLink.factoryKey == admin::ModuleAdmin::FACTORY_KEY && parentLink.parameterValue == ResaModule::FACTORY_KEY)
			//	links.push_back(getPageLink());
			return links;
		}
		
		AdminInterfaceElement::PageLinks BookableCommercialLineAdmin::getSubPages(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const {
			AdminInterfaceElement::PageLinks links;
			/// @todo Implement it or remove the method to get the default behaviour
			return links;
		}


		std::string BookableCommercialLineAdmin::getTitle() const
		{
			return _line.get() ? _line->getName() : DEFAULT_TITLE;
		}

		std::string BookableCommercialLineAdmin::getParameterName() const
		{
			return _line.get() ? QueryString::PARAMETER_OBJECT_ID : string();
		}

		std::string BookableCommercialLineAdmin::getParameterValue() const
		{
			return _line.get() ? Conversion::ToString(_line->getKey()) : string();
		}
	}
}
