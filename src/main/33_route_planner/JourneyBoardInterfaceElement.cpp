
#include "JourneyBoardInterfaceElement.h"

#include "JourneyBoardStopCellInterfacePage.h"
#include "JourneyBoardServiceCellInterfacePage.h"
#include "JourneyBoardJunctionCellInterfacePage.h"
#include "Journey.h"
#include "JourneyLeg.h"
#include "01_util/Conversion.h"
#include "11_interfaces/Site.h"
#include "11_interfaces/Interface.h"
#include "11_interfaces/ValueElementList.h"
#include "04_time/module.h"
#include "15_env/Edge.h"
#include "15_env/Vertex.h"
#include "15_env/ConnectionPlace.h"
#include "15_env/Road.h"
#include "15_env/HandicappedCompliance.h"
#include "15_env/BikeCompliance.h"

using namespace synthese::routeplanner;

namespace synthese
{
	namespace interfaces
	{

		void JourneyBoardInterfaceElement::display( std::ostream& stream, const ParametersVector& parameters, const void* object /*= NULL*/, const Site* site /*= NULL*/ ) const
		{
			// Parameters
			const Journey* __Trajet = ( const Journey* ) object;
			int __FiltreHandicape = (synthese::util::Conversion::ToInt(_handicappedFilter->getValue(parameters)) > 1);
			int __FiltreVelo = ( synthese::util::Conversion::ToInt(_bikeFilter->getValue(parameters)) > 1 );

			// Resources
			const JourneyBoardStopCellInterfacePage* stopCellInterfacePage = site->getInterface()->getPage<JourneyBoardStopCellInterfacePage>();
			const JourneyBoardServiceCellInterfacePage* serviceCellInterfacePage = site->getInterface()->getPage<JourneyBoardServiceCellInterfacePage>();
			const JourneyBoardJunctionCellInterfacePage* junctionCellInterfacePage = site->getInterface()->getPage<JourneyBoardJunctionCellInterfacePage>();
			const synthese::time::Hour unknownHour( synthese::time::TIME_UNKNOWN );
			const synthese::time::DateTime unknownDateTime( synthese::time::TIME_UNKNOWN );

			// Loop on lines of the board
			bool __Couleur = false;
			for (int l=0; l<__Trajet->getJourneyLegCount (); ++l)
			{
				const JourneyLeg* __ET = __Trajet->getJourneyLeg (l);

				// LIGNE ARRET MONTEE Si premier point d'arr�t et si alerte
				if ( l == 0 )
				{
					synthese::time::DateTime debutPrem, finPrem;
					debutPrem = __ET->getDepartureTime();
					finPrem = debutPrem;
					if ( __Trajet->getContinuousServiceRange () )
						finPrem += __Trajet->getContinuousServiceRange ();

					if ( __ET->getOrigin()->getFromVertex ()->getConnectionPlace()
						->hasApplicableAlarm ( debutPrem, finPrem ) )
					{
						stopCellInterfacePage->display( stream, false
							, __ET->getOrigin() ->getConnectionPlace() ->getAlarm()
							, false, __ET->getOrigin() ->getConnectionPlace() ->getName()
							, __Couleur, unknownHour, unknownHour
							, site );
					}
				}

				if ( dynamic_cast<const synthese::env::Road*> (__ET->getService ()->getPath ()) == 0 )
				{
					// LIGNE CIRCULATIONS
					synthese::time::DateTime debutLigne, finLigne
						, lastDepartureTime(synthese::time::TIME_UNKNOWN), lastArrivalTime(synthese::time::TIME_UNKNOWN);
					debutLigne = __ET->getDepartureTime();
					finLigne = __ET->getArrivalTime ();

					if ( __Trajet->getContinuousServiceRange () )
					{
						lastDepartureTime = __ET->getDepartureTime();
						lastDepartureTime += __Trajet->getContinuousServiceRange ();
					}
					if ( __Trajet->getContinuousServiceRange () )
					{
						lastArrivalTime = __ET->getArrivalTime ();
						lastArrivalTime += __Trajet->getContinuousServiceRange ();
						finLigne = lastArrivalTime;
					}

					// 12/18 Reservation
					synthese::time::DateTime maintenant;
					const synthese::env::ReservationRule* reservationRule = __ET->getService ()->getPath ()->getReservationRule ();

					maintenant.updateDateTime();
					bool openedCompulsoryReservation = ( 
						(reservationRule->getType () == synthese::env::ReservationRule::RESERVATION_TYPE_COMPULSORY)
						&& (reservationRule->isReservationPossible( __ET->getService(), maintenant, __ET->getDepartureTime() )) 
						);
					maintenant.updateDateTime();
					bool openedOptionalReservation = (
						(reservationRule->getType () == synthese::env::ReservationRule::RESERVATION_TYPE_OPTIONAL) &&
						(reservationRule->isReservationPossible( __ET->getService(), maintenant, __ET->getDepartureTime() )) 
						);
					bool openedReservation = openedCompulsoryReservation || openedOptionalReservation;
					std::string syntheseOnlineBookingURL;
					if (openedReservation && site->onlineBookingAllowed() ) /// @todo implement && __ET->getLigne() ->GetResa() ->ReservationEnLigne()
					{	/** @todo implement this
						synthese::server::Request request;
						request.addParameter( synthese::server::PARAMETER_FUNCTION, synthese::server::FUNCTION_RESERVATION_FORM );
						request.addParameter( synthese::server::PARAMETER_SITE, __Site->getClef() );
						request.addParameter( synthese::server::PARAMETER_LINE_CODE, __ET->getLigne() ->getCode() );
						request.addParameter( synthese::server::PARAMETER_SERVICE_NUMBER, ( __ET->getService() ->getNumero() ) );
						request.addParameter( synthese::server::PARAMETER_SERVICE_NUMBER, __ET->getLigne() ->GetResa() ->Index() );
						request.addParameter( synthese::server::PARAMETER_DEPARTURE_STOP_NUMBER, __ET->getOrigin() ->getConnectionPlace() ->getId() );
						request.addParameter( synthese::server::PARAMETER_ARRIVAL_STOP_NUMBER, __ET->getDestination() ->getConnectionPlace() ->getId() );
						request.addParameter( synthese::server::PARAMETER_DATE, __ET->getDepartureTime() );
						
						syntheseOnLineBookingURL = site->getClientURL() + "?" + request.toInternalString (); //18
						*/
					}

					serviceCellInterfacePage->display( stream 
						, __ET->getDepartureTime().getHour()
						, lastDepartureTime.getHour()
						, __ET->getArrivalTime().getHour()
						, lastArrivalTime.getHour()
						, 0 /// @todo implement __ET->getLigne() ->Materiel() ->Code(); //4
						, "ligne" /// @todo implement __ET->getLigne() ->Materiel() ->getLibelleSimple() //5
						, "la ligne" /// @todo implement __ET->getLigne()->LibelleComplet(LibelleCompletMatosHTML);
						, "destination" /// @todo implement __ET->getLigne()->LibelleDestination(DestinationHTML);
						, __FiltreHandicape
						, __ET->getService ()->getHandicappedCompliance ()->getCapacity ()
						, __FiltreVelo
						, __ET->getService ()->getBikeCompliance ()->getCapacity ()
						, openedCompulsoryReservation
						, openedOptionalReservation
						, openedReservation ? reservationRule->getReservationDeadLine ( __ET->getService(), __ET->getDepartureTime() ) : unknownDateTime 
						, openedReservation ? reservationRule : NULL
						, openedReservation ? syntheseOnlineBookingURL : ""
						, __ET->getService ()->getPath ()->hasApplicableAlarm ( debutLigne, finLigne ) ? __ET->getService()->getPath ()->getAlarm() : NULL
						, __Couleur
						, __ET->getService ()->getPath ()
						, site );
					
					__Couleur = !__Couleur;


					// LIGNE ARRET DE DESCENTE

					synthese::time::DateTime debutArret, finArret, tempMoment(synthese::time::TIME_UNKNOWN);
					debutArret = __ET->getArrivalTime ();
					finArret = debutArret;
					if ( l < __Trajet->getJourneyLegCount ()-1)
						finArret = __Trajet->getJourneyLeg (l+1)->getDepartureTime();
					if ( __Trajet->getContinuousServiceRange () )
						finArret += __Trajet->getContinuousServiceRange ();

					
					if ( __Trajet->getContinuousServiceRange () )
					{
						tempMoment = __ET->getArrivalTime ();
						tempMoment += __Trajet->getContinuousServiceRange ();
					}
					
					stopCellInterfacePage->display( stream, true
						, __ET->getDestination() ->getConnectionPlace()->hasApplicableAlarm ( debutArret, finArret ) ? __ET->getDestination()->getConnectionPlace()->getAlarm() : NULL
						, __ET->getDestination()->getConnectionPlace() == __ET->getService ()->getPath ()->getEdges ().back()->getFromVertex ()->getConnectionPlace()
						, __ET->getDestination()->getConnectionPlace()->getName()
						, __Couleur, __ET->getArrivalTime().getHour(), tempMoment.getHour()
						, site);

					__Couleur = !__Couleur;

				}
				else
				{
					// 1/2 Alerte
					synthese::time::DateTime debutArret, finArret;
					debutArret = __ET->getArrivalTime ();
					finArret = debutArret;
					if ( l < __Trajet->getJourneyLegCount ()-1)
						finArret = __Trajet->getJourneyLeg (l+1)->getDepartureTime();
					if ( __Trajet->getContinuousServiceRange () )
						finArret += __Trajet->getContinuousServiceRange ();

					junctionCellInterfacePage->display( stream
						, __ET->getDestination()->getConnectionPlace()
						, __ET->getDestination()->getConnectionPlace()->hasApplicableAlarm(debutArret, finArret) ? __ET->getDestination()->getConnectionPlace()->getAlarm() : NULL
						, __Couleur
						, site);
				
					// 3 Couleur du fond de case
					__Couleur = !__Couleur;
				}
			}

		}

		void JourneyBoardInterfaceElement::parse( const std::string& text )
		{
			ValueElementList vel(text);
			_handicappedFilter = vel.front();
			_bikeFilter = vel.front();

		}

		JourneyBoardInterfaceElement::~JourneyBoardInterfaceElement()
		{
			delete _handicappedFilter;
			delete _bikeFilter;
		}
	}
}