#include "RoutePlanner.h"

#include "Journey.h"


#include "15_env/Vertex.h"
#include "15_env/Edge.h"
#include "15_env/Line.h"
#include "15_env/Service.h"
#include "15_env/SquareDistance.h"

#include "15_env/VertexAccessMap.h"


using synthese::time::DateTime;

using synthese::env::Place;
using synthese::env::Vertex;
using synthese::env::Edge;
using synthese::env::Path;
using synthese::env::Line;
using synthese::env::Service;
using synthese::env::SquareDistance;

using synthese::env::AccessParameters;
using synthese::env::AccessDirection;
using synthese::env::VertexAccessMap;
using synthese::env::VertexAccess;

using synthese::env::FROM_ORIGIN;
using synthese::env::TO_DESTINATION;
using synthese::env::UNKNOWN_VALUE;



namespace synthese
{

namespace routeplanner
{



RoutePlanner::RoutePlanner (const Place* origin,
			    const Place* destination,
			    const AccessParameters& accessParameters,
			    const PlanningOrder& planningOrder,
			    const DateTime& journeySheetStartTime,
			    const DateTime& journeySheetEndTime)
    : _origin (origin)
    , _destination (destination)
    , _accessParameters (accessParameters)
    , _planningOrder (planningOrder)
    , _journeySheetStartTime (journeySheetStartTime)
    , _journeySheetEndTime (journeySheetEndTime)
{
    origin->getImmediateVertices (_originVam, TO_DESTINATION, accessParameters);
    destination->getImmediateVertices (_destinationVam, FROM_ORIGIN, accessParameters);
    
}


 


   
RoutePlanner::~RoutePlanner ()
{

}




const Place* 
RoutePlanner::getOrigin () const
{
    return _origin;
}




const Place* 
RoutePlanner::getDestination () const
{
    return _destination;
}








bool 
RoutePlanner::isPathCompliant (const Path* path, 
			       const Journey* journey) const
{


    // TODO :
    /*
    // L'axe de la ligne autorise-t-il la prise des voyageurs ?
    if (!Ligne->Axe()->Autorise())
	return false;
	
    // tests sur la prise en charge des velos
    if (vBesoinVelo == Vrai)
    {
	if (Ligne->getVelo()==NULL
	    || Ligne->getVelo()->TypeVelo() == Faux)
	    return false;
    }
	
    // tests sur la prise en charge des handicapés
    if (vBesoinHandicape == Vrai)
    {
	if (Ligne->getHandicape()==NULL
	    || Ligne->getHandicape()->getTypeHandicape() == Faux)
	    return false;
    }

    // tests sur la restriction sur les taxi bus
    if (vBesoinTaxiBus == Vrai)
    {
	if (Ligne->GetResa()==NULL
	    || Ligne->GetResa()->TypeResa() != Obligatoire)
	    return false;
    }

    // tests sur le choix des tarifs
    if (vCodeTarif >-1)
    {
	//tarif ligne==tarif demande + tarif 0=> gratuit
	if ((Ligne->getTarif()== NULL)
	    || ((Ligne->getTarif()->getNumeroTarif() != vCodeTarif)
		&& (Ligne->getTarif()->getNumeroTarif() != 0)))
	    return false;
    }
		
    // Contrôle de l'axe vis à vis des axes déjà empruntés, effectué que si l'axe de la ligne est non libre et si un ET est fourni
    if (!Ligne->Axe()->Libre() && __Trajet.Taille())
    {
	for (const cElementTrajet* curET = __Trajet.PremierElement(); curET != NULL; curET = curET->Suivant())
	    if (curET->Axe() == Ligne->Axe())
		return false;
    }
	
    // Succès
    */
    return true;
}



/*
bool cCalculateur::DestinationUtilePourArriverTot(const cGare* __PointArret, const cMoment& __Moment, cDistanceCarree& __DistanceCarreeBut) const
{
	//! <li>Calcul de la distance carrée si non fournie</li>
	if (__DistanceCarreeBut.EstInconnu())
		__DistanceCarreeBut.setFromPoints(__PointArret->getPoint(), vPADeDestination->getPointArret()->getPoint());

	//! <li>Evaluation du moment "au plus tot" où peut démarrer</li>
	cMoment __MomentArriveeAvantInclusCorrespondance = __Moment;
	if (!vPADeDestination->inclue(__PointArret))
		__MomentArriveeAvantInclusCorrespondance += __PointArret->AttenteMinimale();

	//! <li>Test 1 : Non dépassement du moment d'arrivée maximal</li>
	if (__MomentArriveeAvantInclusCorrespondance > vArriveeMax)
		return false;
	
	//!	\todo Remettre ici un controle par VMAX
	
	return true;
}
*/





bool
RoutePlanner::isDestinationUsefulForSoonArrival (const VertexAccessMap* vam,
						 const DateTime& dateTime,
						 SquareDistance& sqd) const
{
/*    if (sqd.getSquareDistance () == UNKNOWN_VALUE)
    {
//	sqd.setFromPoints (*vertex, );  // which vertex from the arrival vam ?
    }
*/
    
    // TODO...
    return true;

}



    /* TODO  numerovoie ??
const DateTime& 
RoutePlanner::getBestArrival (const Vertex* curPA, tNumeroVoie NumeroVoie) const
{
    if (curPA->CorrespondanceAutorisee() && NumeroVoie && vMeilleurTempsQuai[curPA->Index()][NumeroVoie] < vMeilleurTemps[curPA->Index()])
	return(vMeilleurTempsQuai[curPA->Index()][NumeroVoie]);
    else
	return(vMeilleurTemps[curPA->Index()]);

}
    */




bool 
RoutePlanner::evaluateArrival (const Edge* arrivalEdge,
			       const DateTime& departureMoment,
			       const Edge* departureEdge,
			       int serviceNumber,
			       Journey& journeyPart,
			       const Journey& currentJourney,
			       bool strictTime,
			       int continuousServiceRange)
{
    if (arrivalEdge == 0) return true;
    
    // Arrival moment
    DateTime arrivalMoment = departureMoment;
    arrivalEdge->calculateArrival (*departureEdge, 
				   serviceNumber, 
				   departureMoment, 
				   arrivalMoment);

    SquareDistance sqd;
/*    if (isDestinationUsefulForSoonArrival (arrivalEdge->getFromVertex (),
					   arrivalMoment,
					   sqd) == false)
    {
	return false;
    }

*/

    // Continuous service breaking
    if (_previousContinuousServiceDuration)
    {
	if ( (currentJourney.getJourneyLegCount () > 0) &&
	     (currentJourney.getDepartureTime () <= _previousContinuousServiceLastDeparture) &&
	     (arrivalMoment - currentJourney.getDepartureTime () >= _previousContinuousServiceDuration) )
	{
	    return false;
	}
	else if ( (departureMoment < _previousContinuousServiceLastDeparture) && 
		  (arrivalMoment - departureMoment >= _previousContinuousServiceDuration) )
	{
	    return false;
	}
    }
    
/*
    // Add a journey leg if necessary
    if ( (arrivalMoment < getBestArrival (arrivalEdge->getFromVertex ())) ||
	 (strictTime && (arrivalMoment == getBestArrival (arrivalEdge->getFromVertex ()))) )
    {
	JourneyLeg journeyLeg;
	
	



    

    // Ecriture de l'ET si nécessaire
    if ((__MomentArrivee < GetMeilleureArrivee(__GareLigneArr->PointArret(), __GareLigneArr->Quai()))
	|| (__OptimisationAFaire && __MomentArrivee == GetMeilleureArrivee(__GareLigneArr->PointArret(), __GareLigneArr->Quai())))
    {
	cElementTrajet* __ElementTrajet;
	// On fait mieux, donc ET doit etre gardé.
	if (vPADeDestination->inclue(__GareLigneArr->PointArret()))
	    __ElementTrajet = GetET(__GareLigneArr->PointArret());
	else
	    __ElementTrajet = GetET(__GareLigneArr->PointArret(), __GareLigneArr->Quai());
	
	bool __ETCree = false;
	if (__ElementTrajet == NULL)
	{
	    __ElementTrajet = new cElementTrajet;								// Allocation
	    __SuiteElementsTrajets.LieEnPremier(__ElementTrajet);			__ElementTrajet->setPointArretArrivee(__GareLigneArr->PointArret());	// Ecriture gare d'arrivée
	    SetET(__GareLigneArr->PointArret(), __ElementTrajet, __GareLigneArr->Quai());	// Stockage dans la gare pour reutilisation eventuelle
	    __ElementTrajet->setQuaiArrivee(__GareLigneArr->Quai());
	    __ElementTrajet->setDistanceCarreeObjectif(__DistanceCarreeBut);
	    __ETCree = true;
	}
	__ElementTrajet->setPointArretDepart(__GareLigneDep->PointArret());		// Ecriture gare de départ
	__ElementTrajet->setAmplitudeServiceContinu(__AmplitudeServiceContinu);
	__ElementTrajet->setQuaiDepart(__GareLigneDep->Quai());
	__ElementTrajet->setLigne(__GareLigneDep->Ligne());						// Ecriture ligne
	__ElementTrajet->setMomentArrivee(__MomentArrivee);						// Ecriture Moment d'arrivée
	__ElementTrajet->setMomentDepart(__MomentDepart);						// Ecriture Moment de départ
	__ElementTrajet->setService(__IndexService);							//Ecriture du numéro de service
	
	
	if (vPADeDestination->inclue(__GareLigneArr->PointArret()))
	{
	    SetMeilleureArrivee(__GareLigneArr->PointArret(), __MomentArrivee);	// Enregistrement meilleure arrivée
	    
	    if (__OptimisationAFaire)
		vArriveeMax = __MomentArrivee;
	    else
		vArriveeMax = vPADeDestination->momentArriveePrecedente(__MomentArrivee, vMomentDebut);	// Ecriture MomentMax le cas échéant
	}
	else
	    SetMeilleureArrivee(__GareLigneArr->PointArret(), __MomentArrivee, __GareLigneArr->Quai());	// Enregistrement meilleure arrivée
    }
    
    // Retour arrêter le parcours de la ligne si la destination a été atteinte
    return !vPADeDestination->inclue(__GareLigneArr->PointArret());
*/
    
    

}
				    
				    


/*
inline bool cCalculateur::EvalueGareLigneArriveeCandidate(
    const cGareLigne* __GareLigneArr, 
    const cMoment& __MomentDepart, 
    const cGareLigne* __GareLigneDep, 
    tIndex __IndexService, 
    cTrajet& __SuiteElementsTrajets, 
    const cTrajet& __TrajetEffectue, 
    bool __OptimisationAFaire, 
    const cDureeEnMinutes& __AmplitudeServiceContinu, 
    cLog& __LogTrace)
{
	if (!__GareLigneArr)
		return true;
		
	// Heure d'arrivée
	cMoment	__MomentArrivee = __MomentDepart;
	__GareLigneArr->CalculeArrivee(*__GareLigneDep, __IndexService, __MomentDepart, __MomentArrivee);
							
	// Utilité de la ligne et du point d'arret
	cDistanceCarree __DistanceCarreeBut;
	if (!DestinationUtilePourArriverTot(__GareLigneArr->PointArret(), __MomentArrivee, __DistanceCarreeBut))
		return false;
							
	// Ruptures de services continus
	if (vDureeServiceContinuPrecedent.Valeur())
	{
		if (__TrajetEffectue.Taille())
		{
			if (__TrajetEffectue.getMomentDepart() <= vDernierDepartServiceContinuPrecedent 
			&& __MomentArrivee - __TrajetEffectue.getMomentDepart() >= vDureeServiceContinuPrecedent)
				return false;
		}
		else
		{
			if (__MomentDepart < vDernierDepartServiceContinuPrecedent && __MomentArrivee - __MomentDepart >= vDureeServiceContinuPrecedent)
				return false;
		}
	}

	// Ecriture de l'ET si nécessaire
	if ((__MomentArrivee < GetMeilleureArrivee(__GareLigneArr->PointArret(), __GareLigneArr->Quai()))
	|| (__OptimisationAFaire && __MomentArrivee == GetMeilleureArrivee(__GareLigneArr->PointArret(), __GareLigneArr->Quai())))
	{
		cElementTrajet* __ElementTrajet;
		// On fait mieux, donc ET doit etre gardé.
		if (vPADeDestination->inclue(__GareLigneArr->PointArret()))
			__ElementTrajet = GetET(__GareLigneArr->PointArret());
		else
			__ElementTrajet = GetET(__GareLigneArr->PointArret(), __GareLigneArr->Quai());

		bool __ETCree = false;
		if (__ElementTrajet == NULL)
		{
			__ElementTrajet = new cElementTrajet;								// Allocation
			__SuiteElementsTrajets.LieEnPremier(__ElementTrajet);			__ElementTrajet->setPointArretArrivee(__GareLigneArr->PointArret());	// Ecriture gare d'arrivée
			SetET(__GareLigneArr->PointArret(), __ElementTrajet, __GareLigneArr->Quai());	// Stockage dans la gare pour reutilisation eventuelle
			__ElementTrajet->setQuaiArrivee(__GareLigneArr->Quai());
			__ElementTrajet->setDistanceCarreeObjectif(__DistanceCarreeBut);
			__ETCree = true;
		}
		__ElementTrajet->setPointArretDepart(__GareLigneDep->PointArret());		// Ecriture gare de départ
		__ElementTrajet->setAmplitudeServiceContinu(__AmplitudeServiceContinu);
		__ElementTrajet->setQuaiDepart(__GareLigneDep->Quai());
		__ElementTrajet->setLigne(__GareLigneDep->Ligne());						// Ecriture ligne
		__ElementTrajet->setMomentArrivee(__MomentArrivee);						// Ecriture Moment d'arrivée
		__ElementTrajet->setMomentDepart(__MomentDepart);						// Ecriture Moment de départ
		__ElementTrajet->setService(__IndexService);							//Ecriture du numéro de service
		
		// Gestion de logs
		if (Synthese.getNiveauLog() <= LogDebug)
		{
			cTexte __Message;
			if (__ETCree)
				__Message << "***CREATION***";
			if (__MomentArrivee <= __MomentDepart)
			{
				// Placer un breakpoint ici pour gérer ce type d'erreur
				__Message << "***ERREUR CHRONOLOGIE***";
			}
			__LogTrace.Ecrit(LogDebug, __ElementTrajet, __Message, "");
		}
		
		if (vPADeDestination->inclue(__GareLigneArr->PointArret()))
		{
			SetMeilleureArrivee(__GareLigneArr->PointArret(), __MomentArrivee);	// Enregistrement meilleure arrivée

			if (__OptimisationAFaire)
				vArriveeMax = __MomentArrivee;
			else
				vArriveeMax = vPADeDestination->momentArriveePrecedente(__MomentArrivee, vMomentDebut);	// Ecriture MomentMax le cas échéant
		}
		else
			SetMeilleureArrivee(__GareLigneArr->PointArret(), __MomentArrivee, __GareLigneArr->Quai());	// Enregistrement meilleure arrivée
	}
	
	// Retour arrêter le parcours de la ligne si la destination a été atteinte
	return !vPADeDestination->inclue(__GareLigneArr->PointArret());
}
*/









JourneyVector 
RoutePlanner::integralSearch (const VertexAccessMap& vertices, 
			      const DateTime& desiredTime,
			      const AccessDirection& accessDirection,
			      Journey* currentJourney,
			      int maxDepth,
			      bool searchAddresses, 
			      bool searchPhysicalStops,
			      bool strictTime) const
{
    JourneyVector result;

    for (std::map<const Vertex*, VertexAccess>::const_iterator itVertex = vertices.getMap ().begin ();
	 itVertex != vertices.getMap ().end (); ++itVertex)
    {
	const Vertex* origin = itVertex->first;
	
	const std::set<const Edge*>& edges = (accessDirection == TO_DESTINATION) ?
	    origin->getDepartureEdges () :
	    origin->getArrivalEdges ();

	for (std::set<const Edge*>::const_iterator itEdge = edges.begin ();
	     itEdge != edges.end () ; ++itEdge)
	{
	    const Edge* edge = (*itEdge);

	    // Checks for path compliancy rules and axis constraint.
	    if (isPathCompliant (edge->getParentPath (), currentJourney) == false) continue;

	    int continuousServiceAmplitude = 0;
	    int serviceNumber = 0;
	    
            // TODO : make symetric getPreviousService and getNextService
	    if (accessDirection == TO_DESTINATION)
	    {

		DateTime departureMoment = desiredTime;
		departureMoment += (int) itVertex->second.approachTime;

		serviceNumber = edge->getNextService (departureMoment, 
						      _journeySheetEndTime,
						      _calculationTime);
		
		if (serviceNumber == UNKNOWN_VALUE) continue;
		if (strictTime && departureMoment != desiredTime) continue;
		
		const Service* service = edge->getParentPath ()->getService (serviceNumber);
		if ( serviceNumber != UNKNOWN_VALUE && 
		     service->isContinuous () )
		{
		    if ( departureMoment > edge->getDepartureEndSchedule (serviceNumber) )
		    {
			continuousServiceAmplitude = 
			    60*24 - ( departureMoment.getHour() - 
				      edge->getDepartureEndSchedule (serviceNumber).getHour() );
		    }
		    else
		    {
			continuousServiceAmplitude = 
			    edge->getDepartureEndSchedule (serviceNumber).getHour() - 
			    departureMoment.getHour();
		    }
		}


		const Line* line = dynamic_cast<const Line*> (edge->getParentPath ());
		if (line != 0) 
		{
		    
		    bool walkNonLineConnectableVertices (
			vertices.hasNonLineConnectableArrivalVertex (line));
		    
		    if (walkNonLineConnectableVertices)
		    {
			for (const Edge* curEdge = edge->getFollowingArrival ();
			     curEdge != 0; curEdge = edge->getFollowingArrival ())
			{
			    /* Evaluation de la gareligne rendant vers la destination si trouvée
			    EvalueGareLigneArriveeCandidate(CurrentGLA, MomentDepart, CurrentGLD, NumArret, __SuiteElementsTrajets, TrajetEffectue, OptimisationAFaire, AmplitudeServiceContinu, __LogTrace);
			    
			    if (!EvalueGareLigneArriveeCandidate(CurrentGLA, MomentDepart, CurrentGLD, NumArret, __SuiteElementsTrajets, TrajetEffectue, OptimisationAFaire, AmplitudeServiceContinu, __LogTrace))
			    break;
			    */
			    
			}
		    }
		    else
		    {
			// TODO connection for transport only !!
			for (const Edge* curEdge = edge->getFollowingLineConnectionArrival ();
			     curEdge != 0; curEdge = edge->getFollowingLineConnectionArrival ())
			{

			}
		    }
		}
		
	    }



/* 
	    else
	    {
		DateTime departureMoment = desiredTime - itVertex->second.approachTime;
		serviceNumber = edge->getPreviousService (departureMoment, 
							  _journeySheetStartTime);
		
		if (serviceNumber == UNKNOWN_VALUE) continue;
		if (strictTime && departureMoment != desiredTime) continue;

		const Service* service = getParentPath ()->getService (serviceNumber);
		if ( serviceNumber != UNKNOWN_VALUE && 
		     service->isContinuous() )
		{
		    if ( arrivalMoment > edge->getArrivalEndSchedule (serviceNumber) )
		    {
			continuousServiceAmplitude = 60*24 - ( 
			    arrivalMoment.getHour() - 
			    edge->getArrivalEndSchedule (serviceNumber).getHour () );
		    }
		    else
		    {
			continuousServiceAmplitude = 
			    edge->getArrivalEndSchedule (
				serviceNumber).getHour() - arrivalMoment.getHour ();
		    }
		}

		for (const Edge* curEdge = edge->getPreviousConnectionDeparture ();
		     curEdge != 0; 
		     curEdge = edge->getPreviousConnectionDeparture ())
		{

		}

	    }

	    const Path* path = edge->getParentPath ();
	    
	    double edgeDistance = edge->getLength ();
	    double edgeTime = edgeDistance / _accessParameters.approachSpeed;
	    
	    double totalDistance = itVertex->second.approachDistance + edgeTime;
	    double totalTime = itVertex->second.approachTime + edgeTime;
*/
	    
	    
/*	    
	   if (currentAccess.approachDistance + edgeDistance > accessParameters.maxApproachDistance) continue;
	    if (currentAccess.approachTime + edgeTime > accessParameters.maxApproachTime) continue;
	    
	    PhysicalStopAccess currentAccessCopy = currentAccess;
	    currentAccessCopy.approachDistance += edgeDistance;
	    currentAccessCopy.approachTime += edgeTime;
	    currentAccessCopy.path.push_back (this);
	    
*/
	    
	}
    }	

    return result;
    
}






}
}

