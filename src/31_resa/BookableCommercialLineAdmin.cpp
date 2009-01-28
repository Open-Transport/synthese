
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

#include "ResaModule.h"
#include "ServiceReservations.h"
#include "ReservationTransactionTableSync.h"
#include "ReservationTableSync.h"
#include "ReservationTransaction.h"
#include "Reservation.h"
#include "CancelReservationAction.h"
#include "ResaCustomerAdmin.h"
#include "ResaRight.h"

#include "SearchFormHTMLTable.h"

#include "CommercialLine.h"
#include "AdvancedSelectTableSync.h"
#include "CommercialLineTableSync.h"
#include "ScheduledService.h"
#include "ScheduledServiceTableSync.h"

#include "QueryString.h"
#include "RequestException.h"
#include "ActionFunctionRequest.h"

#include "AdminParametersException.h"
#include "ModuleAdmin.h"
#include "AdminRequest.h"

#include <map>
#include <boost/foreach.hpp>

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
	using namespace security;
	using namespace graph;

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
				_line = CommercialLineTableSync::Get(lineId, _env);
			}
			catch (...)
			{
				throw RequestException("Bad value for line ID");
			}
		
			_displayCancelled = map.getBool(PARAMETER_DISPLAY_CANCELLED, false, false, FACTORY_KEY);


		}
		
		void BookableCommercialLineAdmin::display(ostream& stream, VariablesMap& variables) const
		{
			// Rights
			bool globalReadRight(_request->isAuthorized<ResaRight>(security::READ,UNKNOWN_RIGHT_LEVEL));
			bool globalDeleteRight(_request->isAuthorized<ResaRight>(security::DELETE_RIGHT,UNKNOWN_RIGHT_LEVEL));

			// Requests
			FunctionRequest<AdminRequest> searchRequest(_request);
			searchRequest.getFunction()->setPage<BookableCommercialLineAdmin>();
			searchRequest.setObjectId(_line->getKey());

			ActionFunctionRequest<CancelReservationAction,AdminRequest> cancelRequest(_request);
			cancelRequest.getFunction()->setPage<BookableCommercialLineAdmin>();
			cancelRequest.setObjectId(_line->getKey());

			FunctionRequest<AdminRequest> customerRequest(_request);
			customerRequest.getFunction()->setPage<ResaCustomerAdmin>();
			
			// Local variables
			DateTime now(TIME_CURRENT);

			// Temporary variables
			int seatsNumber(0);
			bool round_trip = false;
			bool next_overflow = false;
			bool course = false;

			stream << "<h1>Recherche</h1>";

			SearchFormHTMLTable st(searchRequest.getHTMLForm());
			stream << st.open();
			stream << st.cell("Date", st.getForm().getCalendarInput(PARAMETER_DATE, _startDateTime.getDate()));
			stream << st.cell("Afficher annulations", st.getForm().getOuiNonRadioInput(PARAMETER_DISPLAY_CANCELLED, _displayCancelled));
			stream << st.close();

			stream << "<h1>Résultats</h1>";

			HTMLTable::ColsVector c;
			c.push_back("Statut");
			c.push_back("Heure départ");
			c.push_back("Arrêt départ");
			c.push_back("Arrêt arrivée");
			c.push_back("Heure arrivée");
			c.push_back("Places");
			c.push_back("Client");
			if (globalDeleteRight)
				c.push_back("Actions");
			HTMLTable t(c,"adminresults");
			stream << t.open();

			// Boucle sur les circulations
			Env env;
			ScheduledServiceTableSync::Search(
				env,
				UNKNOWN_VALUE
				, _line->getKey()
				, _startDateTime.getDate()
			);

			// Download reservations
			map<const ScheduledService*, ServiceReservations> reservations;
			BOOST_FOREACH(shared_ptr<ScheduledService> service, env.getRegistry<ScheduledService>())
			{
				ServiceReservations obj;

				ReservationTransactionTableSync::Search(
					obj.reservationsEnv,
					service->getKey()
					, _startDateTime.getDate()
					, _displayCancelled
					);
				obj.seatsNumber = 0;
				BOOST_FOREACH(shared_ptr<ReservationTransaction> resa, obj.reservationsEnv.getRegistry<ReservationTransaction>())
				{
					if (resa->getCancellationTime().isUnknown())
						obj.seatsNumber += resa->getSeats();
				}

				obj.overflow = false; // rule->getCapacity() && (obj.seatsNumber > rule->getCapacity());

				int lastDepartureLineStop(getRankOfLastDepartureLineStop(service->getPathId()));

// 				const ReservationRule* rule(service->getReservationRule().get());
// 				obj.status = rule->isReservationPossible(
// 					DateTime(_startDateTime.getDate(), service->getDepartureSchedule())
// 					, now
// 					, DateTime(_startDateTime.getDate(), service->getDepartureSchedule(lastDepartureLineStop))
// 					);
				obj.service = service.get();

				reservations.insert(make_pair(service.get(), obj));

			}

			BOOST_FOREACH(shared_ptr<ScheduledService> service, env.getRegistry<ScheduledService>())
			{
				const ServiceReservations& serviceReservations (reservations[service.get()]);
				string plural((serviceReservations.seatsNumber > 1) ? "s" : "");


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
				DateTime originDateTime(DateTime(_startDateTime.getDate(), service->getDepartureSchedule()));



				// Display
				stream << t.row();
				stream << t.col(8, string(), true) << "Service " << service->getServiceNumber() << " - départ de " << service->getDepartureSchedule().getHour().toString();
				if (serviceReservations.seatsNumber > 0)
					stream << " - " << serviceReservations.seatsNumber << " place" << plural << " réservée" << plural;

				if (serviceReservations.reservationsEnv.getRegistry<Reservation>().empty())
				{
					stream << t.row();
					stream << t.col(8) << "Aucune réservation";
				}
				else
				{
					BOOST_FOREACH(shared_ptr<ReservationTransaction> transac, serviceReservations.reservationsEnv.getRegistry<ReservationTransaction>())
					{
						const Reservation* reservation(serviceReservations.getReservation(transac.get()).get());
						ReservationStatus status(reservation->getStatus());

						customerRequest.setObjectId(reservation->getTransaction()->getCancelUserId());
						
						cancelRequest.getAction()->setTransaction(ReservationTransactionTableSync::GetEditable(reservation->getTransaction()->getKey(), _env));

						stream << t.row();
						stream << t.col() << HTMLModule::getHTMLImage(ResaModule::GetStatusIcon(status), reservation->getFullStatusText());
						stream << t.col() << reservation->getDepartureTime().toString();
						stream << t.col() << reservation->getDeparturePlaceName();
						stream << t.col() << reservation->getArrivalPlaceName();
						stream << t.col() << reservation->getArrivalTime().toString();
						stream << t.col() << reservation->getTransaction()->getSeats();

						// Customer name
						stream << t.col();
						if (globalReadRight)
							stream  << HTMLModule::getHTMLLink(customerRequest.getURL(), reservation->getTransaction()->getCustomerName());
						else
							stream << reservation->getTransaction()->getCustomerName();

						// Cancel link
						if (globalDeleteRight)
						{
							stream << t.col();
							switch(status)
							{
							case OPTION:
								stream << HTMLModule::getLinkButton(cancelRequest.getURL(), "Annuler", "Etes-vous sûr de vouloir annuler la réservation ?", "bullet_delete.png");
								break;

							case TO_BE_DONE:
								stream << HTMLModule::getLinkButton(cancelRequest.getURL(), "Annuler hors délai", "Etes-vous sûr de vouloir annuler la réservation (hors délai) ?", "error.png");
								break;

							case AT_WORK:
								stream << HTMLModule::getLinkButton(cancelRequest.getURL(), "Noter absence", "Etes-vous sûr de noter l'absence du client à l'arrêt ?", "exclamation.png");
								break;
							}
						}

					}
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

		bool BookableCommercialLineAdmin::isAuthorized() const
		{
			if (!_line.get())
				return false;

			return _request->isAuthorized<ResaRight>(READ, UNKNOWN_RIGHT_LEVEL, Conversion::ToString(_line->getKey()));
		}
		
		AdminInterfaceElement::PageLinks BookableCommercialLineAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
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
