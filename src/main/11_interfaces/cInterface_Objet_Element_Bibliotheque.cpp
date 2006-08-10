
#include "cInterface_Objet_Element_Bibliotheque.h"
#include "cInterface_Objet_Element_Parametre.h"
#include "cInterface_Objet_Element_Parametre_TexteConnu.h"
#include "cInterface_Objet_AEvaluer_PageEcran.h"
#include "cInterface_Objet_Connu_ListeParametres.h"

#include "cSite.h"

#include "Interface.h"

#include "34_departures_table/PassThroughDescription.h"
#include "33_route_planner/Journey.h"
#include "33_route_planner/JourneyLeg.h"

#include "15_env/Environment.h"
#include "15_env/City.h"
#include "15_env/ConnectionPlace.h"
#include "15_env/HandicappedCompliance.h"
#include "15_env/BikeCompliance.h"
#include "15_env/PedestrianCompliance.h"
#include "15_env/Service.h"
#include "15_env/ReservationRule.h"
#include "15_env/LineStop.h"
#include "15_env/Line.h"
#include "15_env/Road.h"

#include "cTrajets.h"

#include "01_util/LowerCaseFilter.h"
#include "01_util/PlainCharFilter.h"
#include "01_util/Conversion.h"

#include <boost/iostreams/filtering_stream.hpp>
#include <sstream>


using synthese::env::Environment;
using synthese::env::ConnectionPlace;
using synthese::env::City;
using synthese::env::Road;
using synthese::env::Line;
using synthese::env::ReservationRule;

using synthese::interfaces::Interface;

using synthese::routeplanner::JourneyLeg;
using synthese::routeplanner::Journey;
using synthese::departurestable::PassThroughDescription;


/*! \brief Copie d'un ï¿½lï¿½ment objet dynamique, selon les paramï¿½tres fournis
  \param Parametres Chaine de paramï¿½tres desquels tenir compte
  \author Hugues Romain
  \date 2000-2005
 
  La copie d'un ï¿½lï¿½ment de paramï¿½tre dynamique consiste ï¿½ crï¿½er une copie du niï¿½me paramï¿½tre fourni, avec n ï¿½tant le numï¿½ro du paramï¿½tre de l'objet (transalation), quelle que soit sa nature.
*/
cInterface_Objet_Element* cInterface_Objet_Element_Bibliotheque::Copie( const cInterface_Objet_AEvaluer_ListeParametres& __Parametres ) const
{
    cInterface_Objet_Element_Bibliotheque * __EI = new cInterface_Objet_Element_Bibliotheque( _Index );
    for ( const cInterface_Objet_Element_Parametre * __Parametre = _Parametres[ 0 ]; __Parametre != NULL; __Parametre = __Parametre->getSuivant() )
        __EI->_Parametres.Chaine( __Parametre->Copie( __Parametres ) );
    return __EI;
}




/*! \brief Constructeur
 */
cInterface_Objet_Element_Bibliotheque::cInterface_Objet_Element_Bibliotheque( int __Index )
{
    _Index = __Index;
}



/*! \brief Constructeur
 */
cInterface_Objet_Element_Bibliotheque::cInterface_Objet_Element_Bibliotheque( int __Index, const std::string& __Texte )
{
    _Index = __Index;
    _Parametres.InterpreteTexte( cInterface_Objet_AEvaluer_ListeParametres(), __Texte );
}




      /*! \brief Date interprï¿½tï¿½e en fonction d'un texte descriptif et des donnï¿½es de l'environnement
	\param Texte Texte contenant la date ou la commande
	\return La date interprï¿½tï¿½e d'aprï¿½s le texte
	\author Hugues Romain
	\date 2005
	\warning Les paramï¿½tres ne sont pas contrï¿½lï¿½s
         
        La date retournï¿½e est la suivante, selon le texte entrï¿½ :
	- date au format texte interne : date transcrite (pas de contrï¿½le) (ex : 20070201 => 1/2/2007)
	- commande de date classique (synthese::time::TIME_MIN ('m'), synthese::time::TIME_MAX ('M'), synthese::time::TIME_CURRENT ('A'), synthese::time::TIME_UNKNOWN ('?')) : la date correspondante (voir synthese::time::Date::setDate())
	- texte vide : identique ï¿½ synthese::time::TIME_CURRENT
	- synthese::time::TIME_MIN_CIRCULATIONS ('r') : Premiï¿½re date oï¿½ circule au moins un service (voir cEnvironnement::DateMinReelle())
	- TEMPS_MAX_CIRCULATIONS ('R') : Derniï¿½re date oï¿½ circule au moins un service (voir cEnvironnement::DateMaxReelle())
         
        La relation suivante est vï¿½rifiï¿½e en fonctionnement nominal : \f$ TEMPS_{INCONNU}<=TEMPS_{MIN}<=TEMPS_{MIN ENVIRONNEMENT}<=TEMPS_{MIN CIRCULATIONS}<=TEMPS_{ACTUEL}<=TEMPS_{MAX CIRCULATIONS}<=TEMPS_{MAX ENVIRONNEMENT}<=TEMPS_{MAX} \f$.
      */
synthese::time::Date 
cInterface_Objet_Element_Bibliotheque::dateInterpretee( const synthese::env::Environment* env, 
							const std::string& Texte ) const
{
    synthese::time::Date tempDate;
    if ( !Texte.size () )
	tempDate.updateDate( synthese::time::TIME_CURRENT );
    
    else if ( Texte.size () == 1 )
	switch ( Texte[ 0 ] )
	{
	case TEMPS_MIN_CIRCULATIONS:
	    tempDate = env->getMinDateInUse ();
	    break;
	case TEMPS_MAX_CIRCULATIONS:
	    tempDate = env->getMaxDateInUse ();
	    break;
	default:
	    tempDate.updateDate( Texte[ 0 ] );
	}
    else
	tempDate = Texte;
    return tempDate;
}




/*! \brief Evaluation d'un objet dynamique
  \param pCtxt Le flux de sortie sur lequel ï¿½crire le rï¿½sultat de l'ï¿½valuation
  \param __Parametres Les paramï¿½tres ï¿½ fournir ï¿½ l'ï¿½lï¿½ment d'interface pour l'ï¿½valuation
  \param __Objet Pointeur vers un objet ï¿½ fournir ï¿½ l'ï¿½lï¿½ment d'interface pour l'ï¿½valuation
  \param __Site Pointeur vers le site ayant gï¿½nï¿½rï¿½ l'ï¿½valuation
  \return Rï¿½fï¿½rence vers le flux de sortie
  \author Hugues Romain
  \date 2001-2005
 
*/
int cInterface_Objet_Element_Bibliotheque::Evalue( std::ostream& pCtxt, const cInterface_Objet_Connu_ListeParametres& __Parametres
						   , const void* __Objet, const cSite* __Site ) const
{
    // PROVISOIRE
#define __RepBI "/bi/" << _Interface->Index() << "/"

    switch ( _Index )
    {
    case ELEMENTINTERFACELieuArret:
    {
	const Environment * __Environnement = __Site->getEnvironnement();

	// Initialisation des paramï¿½tres
	int iNumeroDesignation = _Parametres[ ELEMENTINTERFACELieuArretNumeroDesignation ] ->Nombre( __Parametres );
	const ConnectionPlace* curPA = __Environnement->getConnectionPlaces ().get ( _Parametres[ ELEMENTINTERFACELieuArretNumeroArretLogique ] ->Nombre( __Parametres ) );

	// Affichage
	// PROVISOIRE LANGAGE IMPOSE
	pCtxt << (curPA->getName());
    }
    break;

    case ELEMENTINTERFACEFicheHoraire:
	if ( __Objet == NULL )  // Cas Pas de solution
	    __Site->Affiche( pCtxt, INTERFACEAucuneSolutionFicheHoraire, __Parametres );
	else
	{
	    // Evaluation des paramï¿½tres
	    const cTrajets* __Trajets = ( const cTrajets* ) __Objet;

	    const JourneyLeg* curET;
	    size_t __Ligne;

	    synthese::time::DateTime tempMoment;

	    // Initialisation des lignes de texte
	    std::stringstream* __Tampons = new std::stringstream [ __Trajets->TailleListeOrdonneePointsArret() ];

	    // Parcours de chaque trajet
	    for ( int __IndexTrajet = 0; __IndexTrajet < __Trajets->Taille(); __IndexTrajet++ )
	    {
		const Journey* __Trajet = &__Trajets->operator[] ( __IndexTrajet );

		// Parcours de chaque ï¿½lï¿½ment de trajet
		__Ligne=0;
		for (int l=0; l<__Trajet->getJourneyLegCount (); ++l)
		{
		    curET = __Trajet->getJourneyLeg (l);
		    bool lastLeg = (l == __Trajet->getJourneyLegCount ()-1);

		    // Prï¿½paration des cases dï¿½part et arrivï¿½e
		    cInterface_Objet_Connu_ListeParametres __ParametresCaseDepart;
		    cInterface_Objet_Connu_ListeParametres __ParametresCaseArrivee;

		    //0 Premiere ligne ?
		    __ParametresCaseDepart << ( __Ligne == 0 ? "1" : "" );
		    __ParametresCaseArrivee << "1";

		    //1 Derniere ligne ?
		    __ParametresCaseDepart << "1";
		    __ParametresCaseArrivee << ( lastLeg ? "1" : "" );

		    //2 Numï¿½ro de colonne
		    __ParametresCaseDepart << __IndexTrajet + 1;
		    __ParametresCaseArrivee << __IndexTrajet + 1;

		    //3 Ligne ï¿½ pied ?
		    // lignes a pied
		    __ParametresCaseDepart << "";
		    __ParametresCaseArrivee << "";

		    // Note : il faudra convertir la ligne à pied en trajet routier (avec road) au niveau
		    // du calcul d'itineraire (avec un booleen pour garantir/ou pas que le trajet routier passe
		    // exactement par les memes points dans le meme ordre que la ligne a pied) .

		    // le test est juste une verif de cast en road
		    __ParametresCaseDepart << ( dynamic_cast<const Road*> (curET->getService ()->getPath ()) ? "1" : "" );
		    __ParametresCaseArrivee << ( dynamic_cast<const Road*> (curET->getService ()->getPath ()) ? "1" : "" );

		    //4 Premier dï¿½part
		    __ParametresCaseDepart << curET->getDepartureTime().getHour ().toInternalString(); //4 Premier dï¿½part
		    __ParametresCaseArrivee << curET->getArrivalTime ().getHour ().toInternalString ();

		    //5,6
		    if ( __Trajet->getContinuousServiceRange () )
		    {
			tempMoment = curET->getDepartureTime();
			tempMoment += __Trajet->getContinuousServiceRange ();
			__ParametresCaseDepart << tempMoment.getHour ().toInternalString (); //5 Dernier dï¿½part

			tempMoment = curET->getArrivalTime ();
			tempMoment += __Trajet->getContinuousServiceRange ();
			__ParametresCaseArrivee << tempMoment.getHour ().toInternalString (); //5 Dernier dï¿½part

			//6 Ligne service continu ?
			__ParametresCaseDepart << "1";
			__ParametresCaseArrivee << "1";
		    }
		    else
		    {
			//5 Dernier dï¿½part
			__ParametresCaseDepart << ""; //5
			__ParametresCaseArrivee << "";

			//6 Ligne service continu ?
			__ParametresCaseDepart << ""; //6
			__ParametresCaseArrivee << "";
		    }

		    // Prï¿½paration paramï¿½tres case vide
		    cInterface_Objet_Connu_ListeParametres __ParametresCaseVide;
		    __ParametresCaseVide << "1";
		    __ParametresCaseVide << "1";
		    __ParametresCaseVide << __IndexTrajet;
		    __ParametresCaseVide << "";
		    __ParametresCaseVide << "";
		    __ParametresCaseVide << "";
		    __ParametresCaseVide << "";

		    // Affichage
		    __Site->Affiche( __Tampons[ __Ligne ], INTERFACEFicheHoraireColonne, __ParametresCaseDepart );
		    for ( __Ligne++; __Trajets->getListeOrdonneePointsArret( __Ligne ) != curET->getDestination() ->getConnectionPlace(); __Ligne++ )
			__Site->Affiche( __Tampons[ __Ligne ], INTERFACEFicheHoraireColonne, __ParametresCaseVide );
		    __Site->Affiche( __Tampons[ __Ligne ], INTERFACEFicheHoraireColonne, __ParametresCaseArrivee );
		}
	    }

	    // Prï¿½paration des lignes
	    bool __Couleur = false;
	    for ( __Ligne = 0; __Ligne < __Trajets->TailleListeOrdonneePointsArret(); __Ligne++ )
	    {
		cInterface_Objet_Connu_ListeParametres __ParametresLigne;
		__ParametresLigne << __Tampons[ __Ligne ].str();
		__ParametresLigne << ( __Couleur ? "1" : "" );
		__Couleur = !__Couleur;
		__Site->Affiche( pCtxt, INTERFACEFicheHoraireLigne, __ParametresLigne, __Trajets->getListeOrdonneePointsArret( __Ligne ) );
	    }

	    delete[] __Tampons;

	    /*
	    // GESTION DES ALERTES
	    // Gestion des alertes : 3 cas possibles :
	    // Alerte sur arrï¿½t de dï¿½part
	    // Circulation ï¿½ rï¿½servation
	    // Alerte sur circulation
	    // Alerte sur arrï¿½t d'arrivï¿½e
	    synthese::time::DateTime __debutAlerte, __finAlerte;
                  
	    // Alerte sur arrï¿½t de dï¿½part
	    // Dï¿½but alerte = premier dï¿½part
	    // Fin alerte = dernier dï¿½part
	    synthese::time::DateTime debutPrem = curET->getDepartureTime();
	    synthese::time::DateTime finPrem = debutPrem;
	    if (__Trajet->getContinuousServiceRange ().Valeur())
	    finPrem += __Trajet->getContinuousServiceRange ();
	    if (curET->getGareDepart()->getAlarm().showMessage(__debutAlerte, __finAlerte)
	    && __NiveauRenvoiColonne < curET->getGareDepart()->getAlarm().Niveau())
	    __NiveauRenvoiColonne = curET->getGareDepart()->getAlarm().Niveau();
                  
	    // Circulation ï¿½ rï¿½servation obligatoire
	    synthese::time::DateTime maintenant;
	    maintenant.setMoment();
	    if (curET->getLigne()->GetResa()->TypeResa() == Obligatoire
	    && curET->getLigne()->GetResa()->reservationPossible(curET->getLigne()->GetTrain(curET->getService()), maintenant, curET->getDepartureTime())
	    && __NiveauRenvoiColonne < ALERTE_ATTENTION)
	    __NiveauRenvoiColonne = ALERTE_ATTENTION;
                  
	    // Circulation ï¿½ rï¿½servation possible
	    maintenant.setMoment();
	    if (curET->getLigne()->GetResa()->TypeResa() == Facultative
	    && curET->getLigne()->GetResa()->reservationPossible(curET->getLigne()->GetTrain(curET->getService()), maintenant, curET->getDepartureTime())
	    && __NiveauRenvoiColonne < ALERTE_INFO)
	    __NiveauRenvoiColonne = ALERTE_INFO;
                  
	    // Alerte sur circulation
	    // Dï¿½but alerte = premier dï¿½part
	    // Fin alerte = derniï¿½re arrivï¿½e
	    debutPrem = curET->getDepartureTime();
	    finPrem = curET->getArrivalTime ();
	    if (__Trajet->getContinuousServiceRange ().Valeur())
	    finPrem += __Trajet->getContinuousServiceRange ();
	    if (curET->getLigne()->getAlarm().showMessage(__debutAlerte, __finAlerte)
	    && __NiveauRenvoiColonne < curET->getLigne()->getAlarm().Niveau())
	    __NiveauRenvoiColonne = curET->getLigne()->getAlarm().Niveau();
                  
	    // Alerte sur arrï¿½t d'arrivï¿½e
	    // Dï¿½but alerte = premiï¿½re arrivï¿½e
	    // Fin alerte = dernier dï¿½part de l'arrï¿½t si correspondnce, derniï¿½re arrivï¿½e sinon
	    __debutAlerte = curET->getArrivalTime ();
	    __finAlerte = __debutAlerte;
	    if (curET->Suivant() != NULL)
	    __finAlerte = curET->Suivant()->getDepartureTime();
	    if (__Trajet->getContinuousServiceRange ().Valeur())
	    __finAlerte += __Trajet->getContinuousServiceRange ();
	    if (curET->getGareArrivee()->getAlarm().showMessage(__debutAlerte, __finAlerte)
	    && __NiveauRenvoiColonne < curET->getGareArrivee()->getAlarm().Niveau())
	    __NiveauRenvoiColonne = curET->getGareArrivee()->getAlarm().Niveau();
	    }
                 
	    // Affichage du renvoi si necessaire
	    //    if (__NiveauRenvoiColonne)
	    //    {
	    //     TamponRenvois << "<img src=\"" << __RepBI << "img/warning.gif\" alt=\"Cliquez sur la colonne pour plus d'informations\" />";
	    //     __MontrerLigneRenvois = true;
	    //    }

	    for (l++;l!=m;l++)
	    {
	    Tampons[l] << "<td class=\"tdHoraires";
	    if (Couleur1)
	    Tampons[l] << "1";
	    else
	    Tampons[l] << "2";
	    Tampons[l] << "\" onclick=\"show('divFiche";
	    Tampons[l].Copie(n,3);
	    Tampons[l] << "')\">";
	    if (Couleur1)
	    Couleur1=false;
	    else
	    Couleur1=true;
	    }

	    //    pCtxt << "</tr></table></div>";
	    //    TamponRenvois << "</td>";
	    }
	    */

	    //    delete [] Tampons;
	}
	break;


    case EI_OBJETDYNAMIQUE_ListeLignesTrajets:
    {
	// Collecte des paramï¿½tres
	cTrajets* __Trajets = ( cTrajets* ) __Objet;

	// Fabrication de l'affichage
	for ( int __n = 0; __n < __Trajets->Taille(); __n++ )
	{
	    // Prï¿½paration des paramï¿½tres de la case
	    cInterface_Objet_Connu_ListeParametres __ParametresCaseLigne;
	    // 0 : Numï¿½ro de la colonne
	    __ParametresCaseLigne << __n + 1; // 0 : Numï¿½ro de la colonne

	    // Lancement de l'affichage
	    __Site->Affiche( pCtxt, INTERFACECaseLignes, __ParametresCaseLigne, __Trajets->GetElement( __n ) );
	}
    }
    break;

    // 7
    case EI_OBJETDYNAMIQUE_ListeLignesTrajet:
    {
	// Collecte des paramï¿½tres
	const Journey* __Trajet = ( const Journey* ) __Objet;
	bool __AfficherLignesPied = _Parametres[ EI_OBJETDYNAMIQUE_ListeLignesTrajet_AffichePietons ] ->Nombre( __Parametres ) > 0;

	cInterface_Objet_Connu_ListeParametres __ParametresCaseLigne;
	__ParametresCaseLigne << _Parametres[ EI_OBJETDYNAMIQUE_ListeLignesTrajet_HTMLDebutLigne ] ->Texte( __Parametres );
	__ParametresCaseLigne << _Parametres[ EI_OBJETDYNAMIQUE_ListeLignesTrajet_HTMLFinLigne ] ->Texte( __Parametres );
	__ParametresCaseLigne << _Parametres[ EI_OBJETDYNAMIQUE_ListeLignesTrajet_LargeurCase ] ->Texte( __Parametres );
	__ParametresCaseLigne << _Parametres[ EI_OBJETDYNAMIQUE_ListeLignesTrajet_HauteurCase ] ->Texte( __Parametres );

	// Fabrication de l'affichage
	for (int l=0; l<__Trajet->getJourneyLegCount (); ++l)
	{
	    const JourneyLeg* __ET = __Trajet->getJourneyLeg (l);
	    if ( __AfficherLignesPied || !dynamic_cast<const Road*> (__ET->getService ()->getPath ()) )
		__Site->Affiche( pCtxt, INTERFACECartoucheLigne, __ParametresCaseLigne, __ET->getService ()->getPath () );
	}
    }
    break;

    case ELEMENTINTERFACEMiniTbDepGare:
    { /*
	cDescriptionPassage* curDP = (cDescriptionPassage*) ObjetAAfficher;
                   
	// SET PORTAGE LINUX
	//cAlternance curAlternance("tdHoraires2",2);
	cAlternance curAlternance(std::string("tdHoraires2"),2);
	//END PORTAGE LINUX
	if (curDP==NULL)
	pCtxt << "<tr><td style=\"text-align:center\">Pas de dï¿½part d'ici 24h";
	else
	{
	for (; curDP!=NULL; curDP=curDP->Suivant())
	{
	pCtxt << "<tr><td style=\"width:25px;\" class=\"" << curAlternance << "\">";
	curDP->getGareLigne()->Ligne()->Cartouche(23,16, pCtxt);
	pCtxt << "<td style=\"width:35px;\" class=\"" << curAlternance << "\"><b>";
	if (curDP->getSitPert() == NULL)
	curDP->getMomentPrevu().getHour ().toInternalString(pCtxt);
	else
	curDP->getMomentReel().getHour ().toInternalString(pCtxt);
	pCtxt << "</b><td class=\"" << curAlternance << "\">";
	ConnectionPlace* PAT = curDP->getGareLigne()->Destination()->ArretLogique();
	if (curDP->getSitPert() == NULL)
	{ 
	PAT->NomHTML(pCtxt);
	}
	else
	{
	pCtxt << "<marquee>";
	PAT->NomHTML(pCtxt);
                 
	pCtxt << " - Dï¿½part initialement prï¿½vu ï¿½ ";
                 
	curDP->getMomentPrevu().getHour ().toInternalString(pCtxt);
	pCtxt << " - " << curDP->getSitPert()->Motif << " - </marquee>";
	}
	pCtxt << "</tr>";
	curAlternance.Incrementer();
	}
	}
      */
    }
    break;

    case EI_BIBLIOTHEQUE_PERIODE:
	pCtxt << __Site->getInterface() ->GetPeriode( _Parametres[ EI_BIBLIOTHEQUE_PERIODE_INDEX ] ->Nombre( __Parametres ) ) ->getCaption ();
	break;

    case ELEMENTINTERFACELienPhotosArretPhysiques:

    {
	/*   // Initialisation des paramï¿½tres
	     const ConnectionPlace* __ArretLogique = (const ConnectionPlace*) __Objet;
	     std::string __TexteEnTete = _Parametres[ELEMENTINTERFACELienPhotosArretPhysiquesDebut]->Texte(__Parametres);
	     std::string __TexteLibelle = _Parametres[ELEMENTINTERFACELienPhotosArretPhysiquesLibelleDefaut]->Texte(__Parametres);
	     std::string __TextePied = _Parametres[ELEMENTINTERFACELienPhotosArretPhysiquesFin]->Texte(__Parametres);
                   
	     for (int __ArretPhysique = 1; __ArretPhysique <= __ArretLogique->getPhysicalStops().size(); __ArretPhysique++)
	     {
	     pCtxt << __TexteEnTete << "<a href=\"javascript:showdiv('Q" << __ArretPhysique << "');\">";
	     if (__ArretLogique->getArretPhysique(__ArretPhysique)->getNom().size () == 0)
	     pCtxt << __TexteLibelle << __ArretPhysique;
	     else
	     pCtxt << __ArretLogique->getArretPhysique(__ArretPhysique)->getNom();
	     pCtxt << "</a>" << __TextePied;
	     }
	*/  
    }

    break;

    case ELEMENTINTERFACELienAutresPhotos:
    {
	/*   const ConnectionPlace* ArretLogique = (const ConnectionPlace*) __Objet;
	     std::string __TexteEnTete = _Parametres[ELEMENTINTERFACELienAutresPhotosOuverture]->Texte(__Parametres);
	     std::string __TexteLienDebut = _Parametres[ELEMENTINTERFACELienAutresPhotosDebut]->Texte(__Parametres);
	     std::string __TexteLienFin = _Parametres[ELEMENTINTERFACELienAutresPhotosFin]->Texte(__Parametres);
	     std::string __TextePied = _Parametres[ELEMENTINTERFACELienAutresPhotosFermeture]->Texte(__Parametres);
                   
	     if (ArretLogique->NombrePhotos())
	     {
	     pCtxt << __TexteEnTete;
	     for (int iArretPhysique=0; iArretPhysique!=ArretLogique->NombrePhotos(); iArretPhysique++)
	     pCtxt << __TexteLienDebut << "<a href=\"javascript:showdiv('P" << iArretPhysique << "');\">" << ArretLogique->getPhoto(iArretPhysique)->DescriptionLocale() << "</a>" << __TexteLienFin;
	     pCtxt << __TextePied;
	     }
	*/  
    }
    break;

    case ELEMENTINTERFACELienServices:
    {
	/*   const ConnectionPlace* ArretLogique = (const ConnectionPlace*) __Objet;
	     std::string __TexteEnTete = _Parametres[ELEMENTINTERFACELienServicesOuverture]->Texte(__Parametres);
	     std::string __TexteLienDebut = _Parametres[ELEMENTINTERFACELienServicesDebut]->Texte(__Parametres);
	     std::string __TexteLienFin = _Parametres[ELEMENTINTERFACELienServicesFin]->Texte(__Parametres);
	     std::string __TextePied = _Parametres[ELEMENTINTERFACELienServicesFermeture]->Texte(__Parametres);
                      
	     if (ArretLogique->GetService(0))
	     {
	     pCtxt << __TexteEnTete;
	     for (int iArretPhysique=0; ArretLogique->GetService(iArretPhysique); iArretPhysique++)
	     {
	     pCtxt << __TexteLienDebut;
	     if (ArretLogique->GetService(iArretPhysique)->getPhoto())
	     pCtxt << "<a href=\"javascript:showdiv('S" << TXT(iArretPhysique) << "');\">";
	     pCtxt << ArretLogique->GetService(iArretPhysique)->Designation();
	     if (ArretLogique->GetService(iArretPhysique)->getPhoto())
	     pCtxt << "</a>" << __TexteLienFin;
	     }
	     pCtxt << __TextePied;
	     }
	*/  
    }
    break;


    case ELEMENTINTERFACEDivArretPhysiques:
    {
	/*   // Collecte des paramï¿½tres
	     const ConnectionPlace* ArretLogique = (const ConnectionPlace*) __Objet;
	     const Environment* __Environnement = __Site->getEnvironnement();
                   
	     // Affichage
	     const cPhoto* curPhoto;
	     int iArretPhysique2;
	     for (int iArretPhysique=1; iArretPhysique <= ArretLogique->NombreArretPhysiques(); iArretPhysique++)
	     {
	     pCtxt << "<div id=\"Q" << iArretPhysique << "\" class=\"Q\">";
	     curPhoto = ArretLogique->getArretPhysique(iArretPhysique)->getPhoto();
	     if (curPhoto)
	     {
	     pCtxt << "<table style=\"\" width=\"512\" border=\"0\" cellpadding=\"3\" cellspacing=\"0\"><tr>";
	     pCtxt << "<th class=\"thHorairesPopUp\" align=\"left\"><nobr>" << curPhoto->DescriptionLocale() << "</nobr></th></tr></table>";
	     pCtxt << "<img src=\"/ph/" << curPhoto->URL() << "\" width=\"512\" height=\"384\" border=\"0\" usemap=\"#MQ" << TXT(iArretPhysique) << "\" />";
	     //! \todo METTRE A JOUR
	     //      pCtxt << "<img src=\"" << __RepBI << "img/legendephotos.gif\" />";
	     pCtxt << "<map name=\"MQ" << iArretPhysique << "\">";
	     for (int iMapPhoto=0; iMapPhoto<curPhoto->NombreMaps(); iMapPhoto++)
	     {
	     pCtxt << "<area shape=\"rect\" alt=\"";
	     pCtxt << curPhoto->Map(iMapPhoto).Alt();
	     pCtxt << "\" coords=\"";
	     pCtxt << curPhoto->Map(iMapPhoto).Coords();
	     if (curPhoto->Map(iMapPhoto).LienPhoto() == 0)
	     pCtxt << "\" target=\"_new\" href=\"" << curPhoto->Map(iMapPhoto).URL() << "\">";
	     else
	     {
	     pCtxt << "\"";
	     for (iArretPhysique2=1; iArretPhysique2<=ArretLogique->NombreArretPhysiques(); iArretPhysique2++)
	     if (ArretLogique->getArretPhysique(iArretPhysique2)->getPhoto() == __Environnement->GetDocument(curPhoto->Map(iMapPhoto).LienPhoto()))
	     {
	     pCtxt << " href=\"javascript:showdiv('Q" << iArretPhysique2 << "')\"";
	     break;
	     }
	     for (iArretPhysique2=0; iArretPhysique2<=ArretLogique->NombrePhotos(); iArretPhysique2++)
	     if (ArretLogique->getPhoto(iArretPhysique2) == __Environnement->GetDocument(curPhoto->Map(iMapPhoto).
	     LienPhoto()))
	     {
	     pCtxt << " href=\"javascript:showdiv('P" << iArretPhysique2 << "')\"";
	     break;
	     }
	     for (iArretPhysique2=0; ArretLogique->GetService(iArretPhysique2); iArretPhysique2++)
	     if (ArretLogique->GetService(iArretPhysique2)->getPhoto() == __Environnement->GetDocument(curPhoto->Map(iMapPhoto).LienPhoto()))
	     {
	     pCtxt << " href=\"javascript:showdiv('S" << iArretPhysique2 << "')\"";
	     break;
	     }
	     pCtxt << " />";
	     }
	     }
	     pCtxt << "</map>";
	     }
	     pCtxt << "</div>";
	     }
	*/  
    }
    break;

    case ELEMENTINTERFACEDivPhotos:
    {
	/*   const ConnectionPlace* ArretLogique = (const ConnectionPlace*) __Objet;
	     const Environment* __Environnement = __Site->getEnvironnement();
                   
	     const cPhoto* curPhoto;
	     int iArretPhysique2;
	     for (int iArretPhysique=0; iArretPhysique!=ArretLogique->NombrePhotos(); iArretPhysique++)
	     {
	     pCtxt << "<div id=\"P" << TXT(iArretPhysique) << "\" class=\"P\">";
	     curPhoto = ArretLogique->getPhoto(iArretPhysique);
	     if (curPhoto)
	     {
	     pCtxt << "<table style=\"\" width=\"512\" border=\"0\" cellpadding=\"3\" cellspacing=\"0\"><tr>";
	     pCtxt << "<th class=\"thHorairesPopUp\" align=\"left\"><nobr>" << curPhoto->DescriptionLocale() << "</nobr></th></tr></table>";
	     pCtxt << "<img src=\"/ph/" << curPhoto->URL() << "\" width=\"512\" height=\"384\" border=\"0\" usemap=\"#MP" << TXT(iArretPhysique) << "\" />";
	     //! \todo METTRE A JOUR
	     //     pCtxt << "<img src=\"" << __RepBI << "img/legendephotos.gif\" />";
	     pCtxt << "<map name=\"MP" << TXT(iArretPhysique) << "\">";
	     for (int iMapPhoto=0; iMapPhoto<curPhoto->NombreMaps(); iMapPhoto++)
	     {
	     pCtxt << "<area shape=\"rect\" alt=\"";
	     pCtxt << curPhoto->Map(iMapPhoto).Alt();
	     pCtxt << "\" coords=\"";
	     pCtxt << curPhoto->Map(iMapPhoto).Coords();
	     if (curPhoto->Map(iMapPhoto).LienPhoto() == 0)
	     pCtxt << "\" target=\"_new\" href=\"" << curPhoto->Map(iMapPhoto).URL() << "\">";
	     else
	     {
	     pCtxt << "\"";
	     for (iArretPhysique2=1; iArretPhysique2<=ArretLogique->NombreArretPhysiques(); iArretPhysique2++)
	     if (ArretLogique->getArretPhysique(iArretPhysique2)->getPhoto() == __Environnement->GetDocument(curPhoto->Map(iMapPhoto).LienPhoto()))
	     {
	     pCtxt << " href=\"javascript:showdiv('Q" << iArretPhysique2 << "')\"";
	     break;
	     }
	     for (iArretPhysique2=0; iArretPhysique2<=ArretLogique->NombrePhotos(); iArretPhysique2++)
	     if (ArretLogique->getPhoto(iArretPhysique2) == __Environnement->GetDocument(curPhoto->Map(iMapPhoto).LienPhoto()))
	     {
	     pCtxt << " href=\"javascript:showdiv('P" << iArretPhysique2 << "')\"";
	     break;
	     }
	     for (iArretPhysique2=0; ArretLogique->GetService(iArretPhysique2); iArretPhysique2++)
	     if (ArretLogique->GetService(iArretPhysique2)->getPhoto() == __Environnement->GetDocument(curPhoto->Map(iMapPhoto).LienPhoto()))
	     {
	     pCtxt << " href=\"javascript:showdiv('S" << iArretPhysique2 << "')\"";
	     break;
	     }
	     pCtxt << " />";
	     }
	     }
	     pCtxt << "</map>"; 
	     }
	     pCtxt << "</div>";
	     }
	*/  
    }
    break;


    case ELEMENTINTERFACEDivServices:
	/* {
	   ConnectionPlace* ArretLogique = (ConnectionPlace*) ObjetAAfficher;
	   cPhoto* curPhoto;
	   int iArretPhysique2;
	   if (ArretLogique->Service != NULL)
	   {         
	   for (int iArretPhysique=0; ArretLogique->Service[iArretPhysique]!=NULL; iArretPhysique++)
	   {
	   pCtxt << "<div id=\"S" << (short int) iArretPhysique << "\" class=\"S\">";
	   curPhoto = ArretLogique->Service[iArretPhysique]->Photo;
	   if (curPhoto != NULL)
	   {
	   pCtxt << "<table style=\"\" width=\"512\" border=\"0\" cellpadding=\"3\" cellspacing=\"0\"><tr>";
	   pCtxt << "<th class=\"thHorairesPopUp\" align=\"left\"><nobr>" << curPhoto->DescriptionLocale << "</nobr></th></tr></table>";
	   pCtxt << "<img src=\"/photosynthese/" << curPhoto->NomFichier << "\" width=\"512\" height=\"384\" border=\"0\" usemap=\"#MS" << (short int) iArretPhysique << "\" />";
	   pCtxt << "<img src=\"/photosynthese/legendephotos.gif\" />";
	   pCtxt << "<map name=\"MS" << (short int) iArretPhysique << "\">";
	   for (size_t iMapPhoto=0; curPhoto->Map[iMapPhoto]!=NULL; iMapPhoto++)
	   {
	   pCtxt << "<area shape=\"rect\" alt=\"";
	   pCtxt << curPhoto->Map[iMapPhoto]->Alt;
	   pCtxt << "\" coords=\"";
	   pCtxt << curPhoto->Map[iMapPhoto]->Coords;
	   if (curPhoto->Map[iMapPhoto]->LienPhoto==0)
	   pCtxt << "\" target=\"_new\" href=\"" << curPhoto->Map[iMapPhoto]->URL << "\">";
	   else
	   {
	   pCtxt << "\"";
	   for (iArretPhysique2=1; iArretPhysique2<=ArretLogique->NombreVoies; iArretPhysique2++)
	   if (ArretLogique->vArretPhysique[iArretPhysique2]->Photo == vEnvironnement->Photo[curPhoto->Map[iMapPhoto]->LienPhoto])
	   {
	   pCtxt << " href=\"javascript:showdiv('Q" << (short int) iArretPhysique2 << "')\"";
	   break;
	   }
	   for (iArretPhysique2=0; iArretPhysique2<=ArretLogique->NombrePhotos; iArretPhysique2++)
	   if (ArretLogique->Photo[iArretPhysique2] == vEnvironnement->Photo[curPhoto->Map[iMapPhoto]->LienPhoto])
	   {
	   pCtxt << " href=\"javascript:showdiv('P" << (short int) iArretPhysique2 << "')\"";
	   break;
	   }
	   if (ArretLogique->Service != NULL)
	   for (iArretPhysique2=0; ArretLogique->Service[iArretPhysique2]!=NULL; iArretPhysique2++)
	   if (ArretLogique->Service[iArretPhysique2]->Photo == vEnvironnement->Photo[curPhoto->Map[iMapPhoto]->LienPhoto])
	   {
	   pCtxt << " href=\"javascript:showdiv('S" << (short int) iArretPhysique2 << "')\"";
	   break;
	   }
	   pCtxt << ">";
	   }
	   }
	   pCtxt << "</map>"; 
	   }
	   pCtxt << "</div>";
	   }
	   }
	   }
	*/
	break;

    case ELEMENTINTERFACEFicheArretScript:
    { // A VIRER
	/*   const ConnectionPlace* ArretLogique = (const ConnectionPlace*) __Objet;
	     pCtxt << "<script> function showdiv(s) {";
	     //pCtxt << "if (s=='PS') document.all.PS.style.visibility = 'visible'; else document.all.PS.style.visibility = 'hidden';";
	     pCtxt << "for (var k=1; k<=" << ArretLogique->NombreArretPhysiques() << "; k++) if (s=='Q'+k) eval(\"document.all.Q\" + k.toInternalString() + \".style.visibility = 'visible';\"); else eval(\"document.all.Q\" + k.toString() + \".style.visibility = 'hidden';\");";
	     if (ArretLogique->NombrePhotos())
	     pCtxt << "for (k=0; k<" << ArretLogique->NombrePhotos() << "; k++) if (s=='P'+k) eval(\"document.all.P\" + k.toString() + \".style.visibility = 'visible';\"); else eval(\"document.all.P\" + k.toString() + \".style.visibility = 'hidden';\");";
	     for (int iService=0; ArretLogique->GetService(iService); iService++)
	     if (ArretLogique->GetService(iService)->getPhoto())
	     pCtxt << "if (s=='S" << iService << "') eval(\"document.all.S" << iService << ".style.visibility = 'visible';\"); else eval(\"document.all.S" << iService << ".style.visibility = 'hidden';\");";
	     pCtxt << "} </script>";
	*/  
    }
    break;

    case EI_BIBLIOTHEQUE_Date:
    {
	synthese::time::Date curDate;
	curDate = _Parametres[ EI_BIBLIOTHEQUE_Date_Date ] ->Texte( __Parametres );
	synthese::time::Date& refDate = curDate;
	__Site->getInterface() ->AfficheDate( pCtxt, refDate );
    }
    break;

    case ELEMENTINTERFACEListeCommunes:
    {
	const Environment* __Environnement = __Site->getEnvironnement();
	int n = _Parametres[ ELEMENTINTERFACEListeCommunesNombre ] ->Nombre( __Parametres );
	std::vector<const City*> tbCommunes = __Environnement->searchCity ( _Parametres[ ELEMENTINTERFACEListeCommunesEntree ] ->Texte( __Parametres ), n );


	if ( tbCommunes[ 1 ] == NULL )
	    pCtxt << _Parametres[ ELEMENTINTERFACEListeCommunesMessErreur ] ->Texte( __Parametres );
	else
	{
	    pCtxt << "<script>Nom = new Array; Num = new Array;</script>";
	    for ( int i = 1; i <= n; i++ )
		if ( tbCommunes[ i ] != NULL )
		{
		    pCtxt << "<script>Nom[" << i << "]=\"" << tbCommunes[ i ] ->getName() << "\";Num[" << i << "]=" << tbCommunes[ i ] ->getId() << ";</script>";
		    pCtxt << _Parametres[ ELEMENTINTERFACEListeCommunesOuverture ] ->Texte( __Parametres );
		    pCtxt << "<a href=\"javascript:MAJ(" << i << ")\">" << tbCommunes[ i ] ->getName() << "</a>";
		    pCtxt << _Parametres[ ELEMENTINTERFACEListeCommunesFermeture ] ->Texte( __Parametres );
		}
		else
		    pCtxt << _Parametres[ ELEMENTINTERFACEListeCommunesTexteSiVide ] ->Texte( __Parametres );
	}
    }
    break;

    case ELEMENTINTERFACEListeArrets:
    {
	/*   const Environment* __Environnement = __Site->getEnvironnement();
	     int n = _Parametres[ELEMENTINTERFACEListeArretsNombre]->Nombre(__Parametres);
	     cAccesPADe** tbAccesPADe = __Environnement->getCommune(_Parametres[ELEMENTINTERFACEListeArretsCommune]->Nombre(__Parametres))
	     ->textToPADe(_Parametres[ELEMENTINTERFACEListeArretsEntree]->Texte(__Parametres),n);
	     if (tbAccesPADe[1]==NULL)
	     pCtxt << _Parametres[ELEMENTINTERFACEListeArretsMessErreur]->Texte(__Parametres);
	     else
	     {
	     pCtxt << "<script>Nom = new Array; NumPA = new Array; NumDes = new Array;</script>";
	     for (int i = 1; i <= n; i++)
	     if (tbAccesPADe[i]!=NULL)
	     {
	     pCtxt << "<script>Nom[" << i << "]=\"" << tbAccesPADe[i]->getNom() << "\";NumDes[" << i << "]=" << tbAccesPADe[i]->numeroDesignation() << "; NumPA[" << i << "]=" << tbAccesPADe[i]->numeroArretLogique() << ";</script>"
	     << _Parametres[ELEMENTINTERFACEListeArretsOuverture]->Texte(__Parametres)
	     << "<a href=\"javascript:MAJ(" << i << ")\">" << tbAccesPADe[i]->getNom() << "</a>"
	     << _Parametres[ELEMENTINTERFACEListeArretsFermeture]->Texte(__Parametres);
	     }
	     else
	     pCtxt << _Parametres[ELEMENTINTERFACEListeArretsTexteSiVide]->Texte(__Parametres);
	     }
	*/  
    }
    break;

    // 25 A mettre dans donnï¿½e environnement
    case ELEMENTINTERFACENomCommune:
    {
	const Environment* __Environnement = __Site->getEnvironnement();
	const City* curCommune = __Environnement->getCities ().get ( _Parametres[ ELEMENTINTERFACENomCommuneNumero ] ->Nombre( __Parametres ) );
	pCtxt << curCommune->getName();
    }
    break;

    // 26
    case ELEMENTINTERFACETbDepGare:
    {
	// Lecture des parametres
        // mettre une assertion pour detecter qu'on a bien un vector<PassThroughDescription*>
	const std::vector<const PassThroughDescription*>* ptds = reinterpret_cast<const std::vector<const PassThroughDescription*>*> (__Objet);
	assert (ptds != 0);

	int __MultiplicateurRangee = _Parametres[ 0 ] ->Nombre( __Parametres ) ? 
	    _Parametres[ 0 ] ->Nombre( __Parametres ) : 1;

	const std::string& __Pages = _Parametres[ 1 ] ->Texte( __Parametres );
	const std::string& __SeparateurPage = _Parametres[ 2 ] ->Texte( __Parametres );
	
	// Gestion des pages
	int __NombrePages = 1;
	if ( (__Pages == "intermediate" ) || 
	     (__Pages == "destination" ) )
	{
	    for (int i=0; i<ptds->size (); ++i)
	    {
		const PassThroughDescription * ___DP = (*ptds)[i];
		if ( ___DP->getDisplayedConnectionPlaces ().size () - 2 > __NombrePages )
		    __NombrePages = ___DP->getDisplayedConnectionPlaces ().size () - 2;
	    }
	}

	if (__Pages == "destination" ) __NombrePages++;

        // Boucle sur les pages
	for ( int __NumeroPage = 1; __NumeroPage <= __NombrePages; __NumeroPage++ )
	{
	    // Separateur de page
	    if ( __NumeroPage > 1 )
		pCtxt << __SeparateurPage;

	    // Boucle sur les rangï¿½es
	    int __Rangee = __MultiplicateurRangee;
	    for (int i=0; i<ptds->size (); ++i)
	    {
		const PassThroughDescription * ___DP = (*ptds)[i];

		// Preparation des paramï¿½tres
		cInterface_Objet_Connu_ListeParametres __ParametresColonne;
		__ParametresColonne << __Rangee;  // 0 : Numero de rangee

		int __NombrePagesRangee = ___DP->getDisplayedConnectionPlaces ().size () - 2 + ( __Pages == "destination" ? 1 : 0 );
		if ( !__NombrePagesRangee || __NumeroPage > __NombrePagesRangee * ( __NombrePages / __NombrePagesRangee ) )
		    __ParametresColonne << __NumeroPage;
		else
		    __ParametresColonne << 1 + __NumeroPage % __NombrePagesRangee;     // 1 : Numï¿½ro de page
		
		// Lancement de l'affichage de la rangee
		__Site->Affiche( pCtxt, INTERFACELigneTableauDepart, __ParametresColonne, ( const void* ) ___DP );

		// Incrementation du numï¿½ro de rangï¿½e
		__Rangee += __MultiplicateurRangee;
	    }
	}
    }
    break;
	    
    // 27
    case ELEMENTINTERFACETbDep:
    { /*
	PassThroughDescription* curDP = (PassThroughDescription*) ObjetAAfficher;
                   
	// Curseurs
	cGareLigne* curGLA;
                   
	PassThroughDescription* curDP2;
	int DerniereHeureAffichee = 25;
	short int NombreReponses = 0;
                   
	cAlternance curAlternance(std::string("tdHoraires2"),2);
                   
	while (curDP!=NULL)
	{
	NombreReponses++;
	if (curDP->getMomentPrevu().Heures() != DerniereHeureAffichee)
	{
	if (DerniereHeureAffichee != 25)
	pCtxt << "</table>";
	DerniereHeureAffichee = curDP->getMomentPrevu().Heures();
	pCtxt << "<h2>" << TXT(DerniereHeureAffichee) << "h</h2><table style=\"border-collapse:collapse\">";
	}
	pCtxt << "<tr class=\"";
	curAlternance.Afficher(pCtxt);
	pCtxt << "\"><td class=\"";
	curAlternance.Afficher(pCtxt);
	pCtxt << "\">";
	curDP->getGareLigne()->Ligne()->Cartouche(23, 16, pCtxt);
                    
	pCtxt << "<td class=\"";
	curAlternance.Afficher(pCtxt);
	pCtxt << "\"><b>";
	curDP->getMomentPrevu().getHour ().toInternalString(pCtxt);
	pCtxt << "</b><td class=\"";
	curAlternance.Afficher(pCtxt);
	pCtxt << "\">";
	for (curGLA = curDP->getGareLigne()->getArriveeSuivante(); curGLA != NULL; curGLA = curGLA->getArriveeSuivante())
	if (curGLA->HorairesSaisis())
	{     
	curGLA->ArretLogique()->NomHTML(pCtxt);
	pCtxt << " (";
	curGLA->CalculeArrivee(curDP).getHour ().toInternalString(pCtxt);
	pCtxt << ") ";
	}
	pCtxt << "\n";
                 
	curAlternance.Incrementer();
                 
	curDP2 = curDP;
	curDP = curDP->Suivant();
	delete curDP2;
	}
      */
    }
    break;

    // 29 A mettre dans 41
    case ELEMENTINTERFACESelecteurVelo:
    {
	tBool3 VeloDefaut = ( tBool3 ) _Parametres[ ELEMENTINTERFACESelecteurVeloDefaut ] ->Nombre( __Parametres );

	if ( VeloDefaut == Vrai )
	    pCtxt << " checked>";
	else
	    pCtxt << " >";
    }
    break;

    // 31 A mettre dans 41
    case ELEMENTINTERFACESelecteurTaxiBus:
    {
	tBool3 TaxiBusDefaut = ( tBool3 ) _Parametres[ ELEMENTINTERFACESelecteurTaxiBusDefaut ] ->Nombre( __Parametres );

	if ( TaxiBusDefaut == Vrai )
	    pCtxt << " checked>";
	else
	    pCtxt << " >";
    }
    break;

    // 34
    case ELEMENTINTERFACEChampsReservation:
    {
	// affichage du champ nom
	pCtxt << "<tr>" << _Parametres[ ELEMENTINTERFACEChampsReservationNom ] ->Texte( __Parametres ) << "<td><input type=\"text\" name=\"" << synthese::server::PARAMETER_CLIENT_NAME << "\" class=\"frmField\" size=\"35\">";
	pCtxt << "*<input type=\"hidden\" name=\"" << CHECK_NOM << "\"/>";
	pCtxt << "</td></tr>";
	/* MJ TODO à laisser en commentaire jusqu'à ecriture de la resa

	// affichage du champ prï¿½nom
	cModaliteReservationEnLigne::FieldNeed bPrenom = ( cModaliteReservationEnLigne::FieldNeed ) _Parametres[ ELEMENTINTERFACEChampsReservationPrenomObligatoire ] ->Nombre( __Parametres );
	if ( bPrenom == cModaliteReservationEnLigne::FieldNeed_OPTIONAL || bPrenom == cModaliteReservationEnLigne::FieldNeed_COMPULSORY )
	{
	pCtxt << "<tr>" << _Parametres[ ELEMENTINTERFACEChampsReservationPrenom ] ->Texte( __Parametres ) << "<td><input type=\"text\" name=\"" << synthese::server::PARAMETER_CLIENT_FIRST_NAME << "\" class=\"frmField\" size=\"35\">";
	if ( bPrenom == cModaliteReservationEnLigne::FieldNeed_COMPULSORY )
	{
	pCtxt << "*<input type=\"hidden\" name=\"" << CHECK_PRENOM << "\"/>";
	}
	pCtxt << "</td></tr>";
	}

	// affichage du champ adresse
	cModaliteReservationEnLigne::FieldNeed bAdresse = ( cModaliteReservationEnLigne::FieldNeed ) _Parametres[ ELEMENTINTERFACEChampsReservationAdresseObligatoire ] ->Nombre( __Parametres );
	if ( bAdresse == cModaliteReservationEnLigne::FieldNeed_OPTIONAL || bAdresse == cModaliteReservationEnLigne::FieldNeed_COMPULSORY )
	{
	pCtxt << "<tr>" << _Parametres[ ELEMENTINTERFACEChampsReservationAdresse ] ->Texte( __Parametres ) << "<td><input type=\"text\" name=\"" << synthese::server::PARAMETER_CLIENT_ADDRESS << "\" class=\"frmField\" size=\"35\">";
	if ( bAdresse == cModaliteReservationEnLigne::FieldNeed_COMPULSORY )
	{
	pCtxt << "*<input type=\"hidden\" name=\"" << CHECK_ADRESSE << "\"/>";
	}
	pCtxt << "</td></tr>";
	}

	// affichage du champ email
	cModaliteReservationEnLigne::FieldNeed bEmail = ( cModaliteReservationEnLigne::FieldNeed ) _Parametres[ ELEMENTINTERFACEChampsReservationEmailObligatoire ] ->Nombre( __Parametres );
	if ( bEmail == cModaliteReservationEnLigne::FieldNeed_OPTIONAL || bEmail == cModaliteReservationEnLigne::FieldNeed_COMPULSORY )
	{
	pCtxt << "<tr>" << _Parametres[ ELEMENTINTERFACEChampsReservationEmail ] ->Texte( __Parametres ) << "<td><input type=\"text\" name=\"" << synthese::server::PARAMETER_CLIENT_EMAIL << "\" class=\"frmField\" size=\"35\">";
	if ( bEmail == cModaliteReservationEnLigne::FieldNeed_COMPULSORY )
	{
	pCtxt << "*<input type=\"hidden\" name=\"" << CHECK_EMAIL << "\"/>";
	}
	pCtxt << "</td></tr>";
	}

	// affichage du champ tï¿½lï¿½phone
	cModaliteReservationEnLigne::FieldNeed bTelephone = ( cModaliteReservationEnLigne::FieldNeed ) _Parametres[ ELEMENTINTERFACEChampsReservationTelephoneObligatoire ] ->Nombre( __Parametres );
	if ( bTelephone == cModaliteReservationEnLigne::FieldNeed_OPTIONAL || bTelephone == cModaliteReservationEnLigne::FieldNeed_COMPULSORY )
	{
	pCtxt << "<tr>" << _Parametres[ ELEMENTINTERFACEChampsReservationTelephone ] ->Texte( __Parametres ) << "<td><input type=\"text\" name=\"" << synthese::server::PARAMETER_CLIENT_PHONE << "\" class=\"frmField\" size=\"35\">";
	if ( bTelephone == cModaliteReservationEnLigne::FieldNeed_COMPULSORY )
	{
	pCtxt << "*<input type=\"hidden\" name=\"" << CHECK_TELEPHONE << "\"/>";
	}
	pCtxt << "</td></tr>";
	}

	// affichage du champ numï¿½ro d'abonnï¿½
	cModaliteReservationEnLigne::FieldNeed bNumAbonne = ( cModaliteReservationEnLigne::FieldNeed ) _Parametres[ ELEMENTINTERFACEChampsReservationNumeroAbonneObligatoire ] ->Nombre( __Parametres );
	if ( bNumAbonne == cModaliteReservationEnLigne::FieldNeed_OPTIONAL || bNumAbonne == cModaliteReservationEnLigne::FieldNeed_COMPULSORY )
	{
	pCtxt << "<tr>" << _Parametres[ ELEMENTINTERFACEChampsReservationNumeroAbonne ] ->Texte( __Parametres ) << "<td><input type=\"text\" name=\"" << synthese::server::PARAMETER_CLIENT_REGISTRATION_NUMBER << "\" class=\"frmField\" size=\"35\">";
	if ( bNumAbonne == cModaliteReservationEnLigne::FieldNeed_COMPULSORY )
	{
	pCtxt << "*<input type=\"hidden\" name=\"" << CHECK_NUMERO_ABONNE << "\"/>";
	}
	pCtxt << "</td></tr>";
	}

	// affichage du champ adresse dï¿½part
	cModaliteReservationEnLigne::FieldNeed bAdresseDepart = ( cModaliteReservationEnLigne::FieldNeed ) _Parametres[ ELEMENTINTERFACEChampsReservationAdresseDepartObligatoire ] ->Nombre( __Parametres );
	if ( bAdresseDepart == cModaliteReservationEnLigne::FieldNeed_COMPULSORY )
	{
	pCtxt << "<tr>" << _Parametres[ ELEMENTINTERFACEChampsReservationAdresseDepart ] ->Texte( __Parametres ) << "<td><input type=\"text\" name=\"" << synthese::server::PARAMETER_DEPARTURE_ADDRESS << "\" class=\"frmField\" size=\"35\">";
	pCtxt << "*<input type=\"hidden\" name=\"" << CHECK_ADRESSE_DEPART << "\"/></td></tr>";
	}

	// affichage du champ adresse arrivï¿½e
	cModaliteReservationEnLigne::FieldNeed bAdresseArrivee = ( cModaliteReservationEnLigne::FieldNeed ) _Parametres[ ELEMENTINTERFACEChampsReservationAdresseArriveeObligatoire ] ->Nombre( __Parametres );
	if ( bAdresseArrivee == cModaliteReservationEnLigne::FieldNeed_COMPULSORY )
	{
	pCtxt << "<tr>" << _Parametres[ ELEMENTINTERFACEChampsReservationAdresseArrivee ] ->Texte( __Parametres ) << "<td><input type=\"text\" name=\"" << synthese::server::PARAMETER_ARRIVAL_ADDRESS << "\" class=\"frmField\" size=\"35\">";
	pCtxt << "*<input type=\"hidden\" name=\"" << CHECK_ADRESSE_ARRIVEE << "\"/></td></tr>";
	}

	// affichage du champ nombre de places
	pCtxt << "<tr>" << _Parametres[ ELEMENTINTERFACEChampsReservationNbPlaces ] ->Texte( __Parametres ) << "<td><input type=\"text\" name=\"" << synthese::server::PARAMETER_RESERVATION_COUNT << "\" class=\"frmField\" size=\"35\">";
	pCtxt << "*<input type=\"hidden\" name=\"" << CHECK_NOMBRE_PLACES << "\"/>";
	pCtxt << "</td></tr>";

	pCtxt << "<tr>" << _Parametres[ ELEMENTINTERFACEChampsReservationWarning ] ->Texte( __Parametres );
	pCtxt << "</tr>";
	*/
    }
    break;


    case ELEMENTINTERFACEFeuillesRoute:
    {
	// Collecte des paramï¿½tres
	const cTrajets* __Trajets = ( const cTrajets* ) __Objet;

	// Affichage de chaque feuille de route
	for ( int __n = 0; __n < __Trajets->Taille(); __n++ )
	{
	    // Crï¿½ation de la liste de paramï¿½tres
	    cInterface_Objet_Connu_ListeParametres __ParametresColonne;

	    // 0 : Index du trajet
	    __ParametresColonne << __n + 1;

	    // 1 : Etat filtre handicapï¿½
	    __ParametresColonne << _Parametres[ ELEMENTINTERFACEFeuillesRouteFiltreHandicape ] ->Nombre( __Parametres );

	    // 2 : Etat filtre vï¿½lo
	    __ParametresColonne << _Parametres[ ELEMENTINTERFACEFeuillesRouteFiltreVelo ] ->Nombre( __Parametres );

	    // 3 : Index trajet prï¿½cï¿½dent
	    __ParametresColonne << ( __n > 0 ? __n : 0 );

	    // 4 : Index trajet suivant
	    __ParametresColonne << ( __n < __Trajets->Taille() - 1 ? __n + 2 : 0 );

	    // 5 : Date du dï¿½part (format interne)
	    __ParametresColonne << __Trajets->operator [] ( __n ).getDepartureTime().getDate().toInternalString ();
		    
	    // Lancement de l'affichage
	    __Site->Affiche( pCtxt, INTERFACEFeuilleRoute, __ParametresColonne, ( const void* ) & ( __Trajets->operator[] ( __n ) ) );
	}
    }
    break;


    case ELEMENTINTERFACEFeuilleRoute:
    {
	// Collecte des paramï¿½tres
	const Journey* __Trajet = ( const Journey* ) __Objet;
	int __FiltreHandicape = ( _Parametres[ ELEMENTINTERFACEFeuilleRouteFiltreHandicape ] ->Nombre( __Parametres ) > 1 );
	int __FiltreVelo = ( _Parametres[ ELEMENTINTERFACEFeuilleRouteFiltreVelo ] ->Nombre( __Parametres ) > 1 );

	// Affichage de chaque ligne de la feuille de route
	bool __Couleur = false;
	for (int l=0; l<__Trajet->getJourneyLegCount (); ++l)
	{
	    const JourneyLeg* __ET = __Trajet->getJourneyLeg (l);

	    // LIGNE ARRET MONTEE Si premier point d'arrï¿½t et si alerte
	    if ( l == 0 )
	    {
		synthese::time::DateTime debutPrem, finPrem;
		debutPrem = __ET->getDepartureTime();
		finPrem = debutPrem;
		if ( __Trajet->getContinuousServiceRange () )
		    finPrem += __Trajet->getContinuousServiceRange ();

		if ( __ET->getOrigin() ->getConnectionPlace()->hasApplicableAlarm ( debutPrem, finPrem ) )
		{
		    cInterface_Objet_Connu_ListeParametres __ParametresMontee;
		    __ParametresMontee << 0;
		    __ParametresMontee << __ET->getOrigin() ->getConnectionPlace() ->getAlarm()->getMessage();
		    __ParametresMontee << __ET->getOrigin() ->getConnectionPlace() ->getAlarm()->getLevel ();
		    __ParametresMontee << "";
		    __ParametresMontee << __ET->getOrigin() ->getConnectionPlace() ->getName();
		    __ParametresMontee << ( __Couleur ? "1" : "" );
		    __Couleur = !__Couleur;

		    __Site->Affiche( pCtxt, INTERFACELigneFeuilleRouteArret, __ParametresMontee, NULL );
		}
	    }

	    if ( dynamic_cast<const Road*> (__ET->getService ()->getPath ()) == 0 )
	    {
		// LIGNE CIRCULATIONS
		synthese::time::DateTime debutLigne, finLigne, tempMoment;
		debutLigne = __ET->getDepartureTime();
		finLigne = __ET->getArrivalTime ();

		cInterface_Objet_Connu_ListeParametres __ParametresLigne;

		// 0/1 Heures de dï¿½part
		__ParametresLigne << __ET->getDepartureTime().getHour ().toInternalString (); //0
		if ( __Trajet->getContinuousServiceRange () )
		{
		    tempMoment = __ET->getDepartureTime();
		    tempMoment += __Trajet->getContinuousServiceRange ();
		    __ParametresLigne << tempMoment.getHour ().toInternalString ();  //1
		}
		else
		    __ParametresLigne << "";      //1

		// 2/3 Heures d'arrivï¿½e
		__ParametresLigne << __ET->getArrivalTime ().getHour ().toInternalString (); //2
		if ( __Trajet->getContinuousServiceRange () )
		{
		    tempMoment = __ET->getArrivalTime ();
		    tempMoment += __Trajet->getContinuousServiceRange ();
		    __ParametresLigne << tempMoment.getHour ().toInternalString (); //3
		    finLigne = tempMoment;
		}
		else
		    __ParametresLigne << "";     //3

		// 4/5/6 Description du matï¿½riel roulant
		/* MJ TODO
		__ParametresLigne << __ET->getLigne() ->Materiel() ->Code(); //4
		__ParametresLigne << __ET->getLigne() ->Materiel() ->getLibelleSimple(); //5
		*/
		__ParametresLigne << 0; //4 temporary!!
		__ParametresLigne << "materiel roulant"; //5 temporary

		std::string LibelleCompletMatosHTML; //!< \todo PROVISOIRE FAIRE UN OBJET STANDARD LIBELLECOMPLETMATOS
		//     __ET->getLigne()->LibelleComplet(LibelleCompletMatosHTML);
		__ParametresLigne << LibelleCompletMatosHTML; //6

		// 7 Destination du vï¿½hicule
		std::string DestinationHTML; //!< \todo PROVISOIRE FAIRE UN OBJET STANDARD POINT D'ARRET pas si simple : il faut gï¿½rer la destination forcï¿½e
		//     __ET->getLigne()->LibelleDestination(DestinationHTML);
		__ParametresLigne << DestinationHTML; //7

		// 8/9 Prise en charge des handicapï¿½s
		__ParametresLigne << __FiltreHandicape; //8
		__ParametresLigne << __ET->getService ()->getHandicappedCompliance ()->getCapacity (); //9

		// 10/11 Prise en charge des vï¿½los
		__ParametresLigne << __FiltreVelo; //10
		__ParametresLigne << __ET->getService ()->getBikeCompliance ()->getCapacity (); //11

		// 12/18 Rï¿½servation
		synthese::time::DateTime maintenant;
		bool __ResaOuverte = false;
		maintenant.updateDateTime();
		const ReservationRule* reservationRule = __ET->getService ()->getPath ()->getReservationRule ();

		if ( (reservationRule->getType () == ReservationRule::RESERVATION_TYPE_COMPULSORY) &&
		     (reservationRule->isReservationPossible( __ET->getService(), maintenant, __ET->getDepartureTime() )) )
		{
		    __ParametresLigne << "1"; //12
		    __ResaOuverte = true;
		}
		else
		    __ParametresLigne << ""; //12

		maintenant.updateDateTime();
		if ( (reservationRule->getType () == ReservationRule::RESERVATION_TYPE_OPTIONAL) &&
		     (reservationRule->isReservationPossible( __ET->getService(), maintenant, __ET->getDepartureTime() )) )
		{
		    __ParametresLigne << "1"; //13
		    __ResaOuverte = true;
		}
		else
		    __ParametresLigne << ""; //13

		if ( __ResaOuverte )
		{
		    __ParametresLigne << reservationRule->getReservationDeadLine ( __ET->getService(), 
										   __ET->getDepartureTime() ).toInternalString (); //14

		    __ParametresLigne << reservationRule->getPhoneExchangeNumber (); //15
		    __ParametresLigne << reservationRule->getPhoneExchangeOpeningHours (); //16
		    __ParametresLigne << reservationRule->getWebSiteUrl (); //17

		    /* MJ TODO
		    if ( __Site->ResaEnLigne() && __ET->getLigne() ->GetResa() ->ReservationEnLigne() )
		    {
			synthese::server::Request request;
			request.addParameter( synthese::server::PARAMETER_FUNCTION, synthese::server::FUNCTION_RESERVATION_FORM );
			request.addParameter( synthese::server::PARAMETER_SITE, __Site->getClef() );
			request.addParameter( synthese::server::PARAMETER_LINE_CODE, __ET->getLigne() ->getCode() );
			request.addParameter( synthese::server::PARAMETER_SERVICE_NUMBER, ( __ET->getService() ->getNumero() ) );
			request.addParameter( synthese::server::PARAMETER_SERVICE_NUMBER, __ET->getLigne() ->GetResa() ->Index() );
			request.addParameter( synthese::server::PARAMETER_DEPARTURE_STOP_NUMBER, __ET->getOrigin() ->getConnectionPlace() ->getId() );
			request.addParameter( synthese::server::PARAMETER_ARRIVAL_STOP_NUMBER, __ET->getDestination() ->getConnectionPlace() ->getId() );

			request.addParameter( synthese::server::PARAMETER_DATE, __ET->getDepartureTime() );
			__ParametresLigne << __Site->getURLClient() << "?" << request.toInternalString (); //18
		    }
		    else
		    */
		    {
			__ParametresLigne << ""; //18
		    }
		}
		else
		{
		    __ParametresLigne << ""; //14
		    __ParametresLigne << ""; //15
		    __ParametresLigne << ""; //16
		    __ParametresLigne << ""; //17
		    __ParametresLigne << ""; //18
		}

		// 19/20 Alertes
		if ( __ET->getService ()->getPath ()->hasApplicableAlarm ( debutLigne, finLigne ) )
		{
		    __ParametresLigne << __ET->getService()->getPath ()->getAlarm()->getMessage (); //19
		    __ParametresLigne << __ET->getService()->getPath ()->getAlarm()->getLevel ();  //20
		}
		else
		{
		    __ParametresLigne << ""; //19
		    __ParametresLigne << ""; //20
		}

		// 21 Couleur de la ligne de tableau
		__ParametresLigne << ( __Couleur ? "1" : "" );
		__Couleur = !__Couleur;

		// Lancement de l'affichage
		__Site->Affiche( pCtxt, INTERFACELigneFeuilleRouteCirculation, __ParametresLigne, __ET->getService ()->getPath () );
		
		// LIGNE ARRET DE DESCENTE
		cInterface_Objet_Connu_ListeParametres __ParametresDescente;


		// 0 Descente
		__ParametresDescente << "1"; //0

		// 1/2 Message d'alerte
		synthese::time::DateTime debutArret, finArret;
		debutArret = __ET->getArrivalTime ();
		finArret = debutArret;
		if ( l < __Trajet->getJourneyLegCount ()-1)
		    finArret = __Trajet->getJourneyLeg (l+1)->getDepartureTime();
		if ( __Trajet->getContinuousServiceRange () )
		    finArret += __Trajet->getContinuousServiceRange ();

		if ( __ET->getDestination() ->getConnectionPlace()->hasApplicableAlarm ( debutArret, finArret ) )
		{
		    __ParametresDescente << __ET->getDestination()->getConnectionPlace()->getAlarm()->getMessage (); //1
		    __ParametresDescente << __ET->getDestination()->getConnectionPlace()->getAlarm()->getLevel ();  //2
		}
		else
		{
		    __ParametresDescente << ""; //1
		    __ParametresDescente << ""; //2
		}

		// 3/4 Informations sur le point d'arrï¿½t
		__ParametresDescente << ( __ET->getDestination()->getConnectionPlace() == 
					  __ET->getService ()->getPath ()->getEdges ().back()->getFromVertex ()->getConnectionPlace() 
					  ? "1" : "" );

		__ParametresDescente << __ET->getDestination()->getConnectionPlace()->getName(); //4

		// 5 Couleur du fond de case
		__ParametresDescente << ( __Couleur ? "1" : "" );
		__Couleur = !__Couleur;

		// 6/7 Heures d'arrivï¿½e
		__ParametresDescente << __ET->getArrivalTime ().getHour ().toInternalString (); //6
		if ( __Trajet->getContinuousServiceRange () )
		{
		    tempMoment = __ET->getArrivalTime ();
		    tempMoment += __Trajet->getContinuousServiceRange ();
		    __ParametresDescente << tempMoment.getHour ().toInternalString (); //7
		}
		else
		    __ParametresDescente << "";     //7

		__Site->Affiche( pCtxt, INTERFACELigneFeuilleRouteArret, __ParametresDescente );
	    }
	    else
	    {
		// LIGNE JONCTION A PIED (si applicable)
		cInterface_Objet_Connu_ListeParametres __ParametresJonction;

		__ParametresJonction << __ET->getDestination() ->getConnectionPlace()->getKey (); // 0

		// 1/2 Alerte
		synthese::time::DateTime debutArret, finArret;
		debutArret = __ET->getArrivalTime ();
		finArret = debutArret;
		if ( l < __Trajet->getJourneyLegCount ()-1)
		    finArret = __Trajet->getJourneyLeg (l+1)->getDepartureTime();

		if ( __Trajet->getContinuousServiceRange () )
		    finArret += __Trajet->getContinuousServiceRange ();
		if ( __ET->getDestination() ->getConnectionPlace()->hasApplicableAlarm ( debutArret, finArret ) )
		{
		    __ParametresJonction << __ET->getDestination() ->getConnectionPlace() ->getAlarm()->getMessage(); // 1
		    __ParametresJonction << __ET->getDestination() ->getConnectionPlace() ->getAlarm()->getLevel (); // 2
		}
		else
		{
		    __ParametresJonction << ""; // 1
		    __ParametresJonction << ""; // 2
		}

		// 3 Couleur du fond de case
		__ParametresJonction << ( __Couleur ? "1" : "" );
		__Couleur = !__Couleur;

		// Lancement de l'affichage
		__Site->Affiche( pCtxt, INTERFACELigneFeuilleRouteJonction, __ParametresJonction );
	    }
	}
    }
    break;

    case EI_BIBLIOTHEQUE_Trajet_Duree:
    {
	// Collecte des paramï¿½tres
	const Journey* __Trajet = ( const Journey* ) __Objet;

	cInterface_Objet_Connu_ListeParametres __Parametres;

	//0 : Durï¿½e du trajet
	__Parametres << __Trajet->getDuration ();

	//1 : Durï¿½e < 1h ?
	__Parametres << ( __Trajet->getDuration () < synthese::time::MINUTES_PER_HOUR ? "1" : "" );

	//2 : Nombre d'heures
	__Parametres << ( __Trajet->getDuration () / synthese::time::MINUTES_PER_HOUR );

	//3 : Nombre de minutes
	__Parametres << ( __Trajet->getDuration () % synthese::time::MINUTES_PER_HOUR );

	//4 : Nombre de minutes superieur a 10 ou inferieur a 60 ?
	__Parametres << ( __Trajet->getDuration () < synthese::time::MINUTES_PER_HOUR || __Trajet->getDuration () % synthese::time::MINUTES_PER_HOUR >= 10 ? "1" : "" );

	// Lancement de l'affichage
	__Site->Affiche( pCtxt, INTERFACEDuree, __Parametres, NULL );
    }
    break;

    case EI_BIBLIOTHEQUE_FicheHoraire_LigneDurees:
    {
	// Collecte des paramï¿½tres
	const cTrajets* __Trajets = ( const cTrajets* ) __Objet;

	// Affichage de chaque feuille de route
	for ( int __n = 0; __n < __Trajets->Taille(); __n++ )
	{
	    cInterface_Objet_Connu_ListeParametres __ParametresCase;
	    __ParametresCase << __n + 1;

	    __Site->Affiche( pCtxt, INTERFACECaseDuree, __ParametresCase, ( const void* ) & ( __Trajets->operator[] ( __n ) ) );
	}

    }
    break;

    case ELEMENTINTERFACEURLFormulaire:
    {
	// Initialisation des paramï¿½tres
	const std::string& __TypeSortie = _Parametres[ ELEMENTINTERFACEURLFormulaireTypeSortie ] ->Texte( __Parametres );
	const std::string& __Fonction = _Parametres[ ELEMENTINTERFACEURLFormulaireFonction ] ->Texte( __Parametres );

	// Fabrication de la requï¿½te
	synthese::server::Request request;

	// Site d'affichage
	request.addParameter( synthese::server::PARAMETER_SITE, __Site->getClef() );

	// Parametres cas validation fiche horaire
	if ( __Fonction == "timetable validation" )
	{
	    request.addParameter( synthese::server::PARAMETER_FUNCTION, synthese::server::FUNCTION_SCHEDULE_SHEET_VALIDATION );

	    request.addParameter( synthese::server::PARAMETER_DEPARTURE_CITY_NUMBER
				  , _Parametres[ ELEMENTINTERFACEURLFormulaireVFHNumeroCommuneDepart ] ->Texte( __Parametres ) );
	    request.addParameter( synthese::server::PARAMETER_DEPARTURE_STOP_NUMBER
				  , _Parametres[ ELEMENTINTERFACEURLFormulaireVFHNumeroArretDepart ] ->Texte( __Parametres ) );
	    request.addParameter( synthese::server::PARAMETER_DEPARTURE_WORDING_NUMBER
				  , _Parametres[ ELEMENTINTERFACEURLFormulaireVFHNumeroDesignationDepart ] ->Texte( __Parametres ) );
	    request.addParameter( synthese::server::PARAMETER_ARRIVAL_CITY_NUMBER
				  , _Parametres[ ELEMENTINTERFACEURLFormulaireVFHNumeroCommuneArrivee ] ->Texte( __Parametres ) );
	    request.addParameter( synthese::server::PARAMETER_ARRIVAL_STOP_NUMBER
				  , _Parametres[ ELEMENTINTERFACEURLFormulaireVFHNumeroArretArrivee ] ->Texte( __Parametres ) );
	    request.addParameter( synthese::server::PARAMETER_ARRIVAL_WORDING_NUMBER
				  , _Parametres[ ELEMENTINTERFACEURLFormulaireVFHNumeroDesignationArrivee ] ->Texte( __Parametres ) );
	}
	// Pour fiche horaire seulement
	else if ( __Fonction == "timetable" )
	{
	    request.addParameter( synthese::server::PARAMETER_FUNCTION, 
				  synthese::server::FUNCTION_SCHEDULE_SHEET );

	    request.addParameter( synthese::server::PARAMETER_DATE
				  , _Parametres[ ELEMENTINTERFACEURLFormulaireFHDate ] ->Texte( __Parametres ) );
	    request.addParameter( synthese::server::PARAMETER_PERIOD
				  , _Parametres[ ELEMENTINTERFACEURLFormulaireFHPeriode ] ->Texte( __Parametres ) );
	    request.addParameter( synthese::server::PARAMETER_BIKE
				  , _Parametres[ ELEMENTINTERFACEURLFormulaireFHVelo ] ->Texte( __Parametres ) );
	    request.addParameter( synthese::server::PARAMETER_HANDICAPPED
				  , _Parametres[ ELEMENTINTERFACEURLFormulaireFHHandicape ] ->Texte( __Parametres ) );
	    request.addParameter( synthese::server::PARAMETER_TAXIBUS
				  , _Parametres[ ELEMENTINTERFACEURLFormulaireFHResa ] ->Texte( __Parametres ) );
	    request.addParameter( synthese::server::PARAMETER_PRICE
				  , _Parametres[ ELEMENTINTERFACEURLFormulaireFHTarif ] ->Texte( __Parametres ) );
	    request.addParameter( synthese::server::PARAMETER_DEPARTURE_STOP_NUMBER
				  , _Parametres[ ELEMENTINTERFACEURLFormulaireFHNumeroArretDepart ] ->Texte( __Parametres ) );
	    request.addParameter( synthese::server::PARAMETER_DEPARTURE_WORDING_NUMBER
				  , _Parametres[ ELEMENTINTERFACEURLFormulaireFHNumeroDesignationDepart ] ->Texte( __Parametres ) );
	    request.addParameter( synthese::server::PARAMETER_ARRIVAL_STOP_NUMBER
				  , _Parametres[ ELEMENTINTERFACEURLFormulaireFHNumeroArretArrivee ] ->Texte( __Parametres ) );
	    request.addParameter( synthese::server::PARAMETER_ARRIVAL_WORDING_NUMBER
				  , _Parametres[ ELEMENTINTERFACEURLFormulaireFHNumeroDesignationArrivee ] ->Texte( __Parametres ) );
	}
	else if ( __Fonction == ( "from city list" ) )
	{
	    request.addParameter( synthese::server::PARAMETER_FUNCTION, synthese::server::FUNCTION_CITY_LIST );
	    request.addParameter( synthese::server::PARAMETER_DIRECTION, 1 );
	}
	else if ( __Fonction == ( "to city list" ) )
	{
	    request.addParameter( synthese::server::PARAMETER_FUNCTION, synthese::server::FUNCTION_CITY_LIST );
	    request.addParameter( synthese::server::PARAMETER_DIRECTION, 0 );
	}
	else if ( __Fonction == ( "from station list" ) )
	{
	    request.addParameter( synthese::server::PARAMETER_FUNCTION, synthese::server::FUNCTION_STOP_LIST );
	    request.addParameter( synthese::server::PARAMETER_DIRECTION, 1 );
	}
	else if ( __Fonction == ( "to station list" ) )
	{
	    request.addParameter( synthese::server::PARAMETER_FUNCTION, synthese::server::FUNCTION_STOP_LIST );
	    request.addParameter( synthese::server::PARAMETER_DIRECTION, 0 );
	}

	// Affichage de la requï¿½te au format voulu
	if ( __TypeSortie == ( "url" ) )
	{
	    // Partie exï¿½cutable de l'url
	    pCtxt << __Site->getURLClient() << "?";

	    // Champ fonction
	    pCtxt << request.toInternalString ();
	}
	else if ( __TypeSortie.substr (0, 4) == "form" )
	{
	    // Tag d'ouverture du formulaire
	    pCtxt << "<form method=\"get\" action=\"" << __Site->getURLClient() << "\" "
		  << __TypeSortie.substr (5) << ">";
		    
	    for ( std::map<std::string, std::string>::const_iterator iter = request.getParameters().begin();
		  iter != request.getParameters().end(); 
		  ++iter )
	    {
		pCtxt << "<input type=\"hidden\" name=\"" << iter->first << "\" value=\"" << iter->second << "\" />";
	    }
	}
    }
    break;

    case ELEMENTINTERFACEInputHTML:  //41
    {
	// Initialisation des paramï¿½tres
	const std::string& __Champ = _Parametres[ ELEMENTINTERFACEInputHTMLChamp ] ->Texte( __Parametres );
	const std::string& __Type = _Parametres[ ELEMENTINTERFACEInputHTMLType ] ->Texte( __Parametres );
	
	// Balise d'ouverture
	std::string __Balise;
	if ( !__Type.size () )
	{
	if ( __Champ == ( "date" ) || __Champ == ( "period" ) )
	__Balise = "select";
	else if ( __Champ == ( "handicap filter" ) )
	__Balise = "input type=\"checkbox\"";
	else
	__Balise = "input type=\"text\"";
	}
	else
	{
	if ( __Type == ( "text" ) )
	__Balise = "input type=\"text\"";
	else if ( __Type == ( "select" ) )
	__Balise = "select";
	else if ( __Type.substr (0, 8) == "checkbox" )
	__Balise = "input type=\"checkbox\"";
	}

	// Ecriture de l'ouverture
	pCtxt << "<" << __Balise << " " << _Parametres[ ELEMENTINTERFACEInputHTMLSuite ] ->Texte( __Parametres );

	// Ecriture du nom du champ
	pCtxt << " name=\"";
	if ( __Champ == "date" )
	pCtxt << synthese::server::PARAMETER_DATE;
	else if ( __Champ == ( "period" ) )
	pCtxt << synthese::server::PARAMETER_PERIOD;
	else if ( __Champ == ( "from city" ) )
	pCtxt << synthese::server::PARAMETER_DEPARTURE_CITY;
	else if ( __Champ == ( "from station" ) )
	pCtxt << synthese::server::PARAMETER_DEPARTURE_STOP;
	else if ( __Champ == ( "to city" ) )
	pCtxt << synthese::server::PARAMETER_ARRIVAL_CITY;
	else if ( __Champ == ( "to station" ) )
	pCtxt << synthese::server::PARAMETER_ARRIVAL_STOP;
	else if ( __Champ == ( "handicap filter" ) )
	pCtxt << synthese::server::PARAMETER_HANDICAPPED;
	else if ( __Champ == ( "tariff" ) )
	pCtxt << synthese::server::PARAMETER_PRICE;
	pCtxt << "\"";

	// Cas champ checkbox
	if ( __Balise == ( "input type=\"checkbox\"" ) )
	{
	// Etat cochï¿½
	tBool3 __Bool3Defaut = Faux;

	// Chackbox filtre sur valeur
	if ( __Type.size () > 9 )
	{
	if ( _Parametres[ ELEMENTINTERFACEInputHTMLValeurDefaut ] ->Texte( __Parametres ) ==  __Type.substr (9) )
	__Bool3Defaut = Vrai;
	pCtxt << " value=\"" << __Type.substr( 9 ) << "\"";
	}
	else //Checkbox boolï¿½en
	{
	__Bool3Defaut = ( tBool3 ) _Parametres[ ELEMENTINTERFACEInputHTMLValeurDefaut ] ->Nombre( __Parametres );
	pCtxt << " value=\"1\"";
	}
	if ( __Bool3Defaut == Vrai )
	pCtxt << " checked=\"1\"";
	pCtxt << " />";
	}
	if ( __Balise == ( "input type=\"text\"" ) )  // Cas champ input texte
	{
	pCtxt << " value=\"" << _Parametres[ ELEMENTINTERFACEInputHTMLValeurDefaut ] ->Texte( __Parametres ) << "\" />";
	}
	else if ( __Balise == ( "select" ) )  // Cas champ select
	{
	pCtxt << ">";

	// Si select auto alors fourniture des choix
	if ( !__Type.size () )
	{
	if ( __Champ == ( "date" ) )
	{
	// Collecte des paramï¿½tres spï¿½cifiques
	const Environment * __Environnement = __Site->getEnvironnement();
	synthese::time::Date DateDefaut = 
	    dateInterpretee( __Environnement, _Parametres[ ELEMENTINTERFACEInputHTMLValeurDefaut ] ->Texte( __Parametres ) );
	synthese::time::Date DateMin = dateInterpretee( __Environnement, _Parametres[ ELEMENTINTERFACEInputHTMLListeDatesMin ] ->Texte( __Parametres ) );
	synthese::time::Date __DateJour;
	__DateJour.updateDate();
	if ( !__Site->getSolutionsPassees() && DateMin < __DateJour )
	DateMin = __DateJour;
	synthese::time::Date DateMax = dateInterpretee( __Environnement, _Parametres[ ELEMENTINTERFACEInputHTMLListeDatesMax ] ->Texte( __Parametres ) );

	// Construction de l'objet HTML
	for ( synthese::time::Date iDate = DateMin; iDate <= DateMax; iDate++ )
	{
	pCtxt << "<option ";
	if ( iDate == DateDefaut )
	pCtxt << "selected=\"1\" ";
	pCtxt << "value=\"" << iDate.toInternalString() << "\">";
	__Site->getInterface() ->AfficheDate( pCtxt, iDate );
	pCtxt << "</option>";
	}
	}
	else if ( __Champ == ( "period" ) )
	{
	int __IndexPeriodeDefaut = _Parametres[ ELEMENTINTERFACEInputHTMLValeurDefaut ] ->Nombre( __Parametres );

	for ( int iPeriode = 0; __Site->getInterface() ->GetPeriode( iPeriode ); iPeriode++ )
	{
	pCtxt << "<option ";
	if ( iPeriode == __IndexPeriodeDefaut )
	pCtxt << "selected ";
	pCtxt << "value=\"" << iPeriode << "\">" << __Site->getInterface() ->GetPeriode( iPeriode ) ->getCaption () << "</option>";
	}
	}
	pCtxt << "</select>";
	}
	}

		

	}
	break;

    case EI_BIBLIOTHEQUE_Interface_PrefixeAlerte:  //42
    {
	// Lecture des paramï¿½tres
	int __Niveau = _Parametres[ EI_BIBLIOTHEQUE_Interface_PrefixeAlerte_Niveau ] ->Nombre( __Parametres );

	// Sortie
	if ( __Niveau > 0 )
	    pCtxt << __Site->getInterface() ->getPrefixeAlerte( __Niveau );
    }
    break;


    case EI_BIBLIOTHEQUE_Ligne_Destination:  //43
    {
	// Lecture des paramï¿½tres
	const Line* __Ligne = ( const Line* ) __Objet;

	// Affichage de la girouette
	if ( __Ligne->getDirection().size () )
	{
	    pCtxt << __Ligne->getDirection ();
	}
	else // Affichage du terminus
	{
	    // Crï¿½ation d'un objet de paramï¿½tres
	    cInterface_Objet_Connu_ListeParametres __ParametresLigne;

	    // 0: Dï¿½signation principale
	    __ParametresLigne << "0";

	    // Lancement de l'affichage
	    __Site->Affiche( pCtxt, INTERFACENomArret, __ParametresLigne, ( const void* ) __Ligne->getEdges ().back()->getFromVertex ()->getConnectionPlace() );
	}
    }
    break;

    case EI_BIBLIOTHEQUE_DescriptionPassage_Ligne:  //44
    {
	const Line* __Ligne = dynamic_cast<const Line*> (( ( const PassThroughDescription* ) __Objet ) ->getLineStop () ->getParentPath ());
	
	cInterface_Objet_Connu_ListeParametres __ParametresCaseLigne;
	__ParametresCaseLigne << _Parametres[ EI_BIBLIOTHEQUE_DescriptionPassage_Ligne_HTMLDebutLigne ] ->Texte( __Parametres );
	__ParametresCaseLigne << _Parametres[ EI_BIBLIOTHEQUE_DescriptionPassage_Ligne_HTMLFinLigne ] ->Texte( __Parametres );
	__ParametresCaseLigne << _Parametres[ EI_BIBLIOTHEQUE_DescriptionPassage_Ligne_LargeurCase ] ->Texte( __Parametres );
	__ParametresCaseLigne << _Parametres[ EI_BIBLIOTHEQUE_DescriptionPassage_Ligne_HauteurCase ] ->Texte( __Parametres );

	__Site->Affiche( pCtxt, INTERFACECartoucheLigne, __ParametresCaseLigne, ( const void* ) __Ligne );
    }
    break;

    // 45
    case EI_BIBLIOTHEQUE_Nombre_Formate:
    {
	// Lecture des paramï¿½tres
	int __Nombre = _Parametres[ 0 ] ->Nombre( __Parametres ) + _Parametres[ 2 ] ->Nombre( __Parametres );
	int __Chiffres = _Parametres[ 1 ] ->Nombre( __Parametres );


	for ( ; __Chiffres; __Chiffres-- )
	{
	    if ( __Nombre < pow( 10.0, __Chiffres - 1 ) )
		pCtxt << "0";
	}
	pCtxt << __Nombre;
    }
    break;

    case EI_BIBLIOTHEQUE_DescriptionPassage_Destination:  //46
    {
	// Lecture des paramï¿½tres
	const PassThroughDescription* __DP = ( const PassThroughDescription* ) __Objet;
	std::string __DestinationsAAfficher = _Parametres[ 0 ] ->Texte( __Parametres );
	bool __AfficherTerminus = ( _Parametres[ 1 ] ->Nombre( __Parametres ) == 1 );
	std::string __TypeAffichage = _Parametres[ 2 ] ->Texte( __Parametres );
	std::string __SeparateurEntreArrets = _Parametres[ 3 ] ->Texte( __Parametres );
	std::string __AvantCommune = _Parametres[ 4 ] ->Texte( __Parametres );
	std::string __ApresCommune = _Parametres[ 5 ] ->Texte( __Parametres );

	const City* __DerniereCommune = __DP->getDisplayedConnectionPlaces () [0]->getCity ();

	for ( int __i = 1; __i < __DP->getDisplayedConnectionPlaces ().size (); __i++ )
	{
	    if ( __DestinationsAAfficher == ( "all" ) && ( __i < __DP->getDisplayedConnectionPlaces ().size () - 1 || __AfficherTerminus )
		 || __DestinationsAAfficher == ( "terminus" ) && __i == __DP->getDisplayedConnectionPlaces ().size () - 1
		 || synthese::util::Conversion::ToInt(__DestinationsAAfficher) == __i && ( __i < __DP->getDisplayedConnectionPlaces ().size () - 1 || __AfficherTerminus )
		)
	    {
		if ( __i > 1 )
		    pCtxt << __SeparateurEntreArrets;

		// Affichage de la commune dans les cas oï¿½ nï¿½cessaire
		if ( __TypeAffichage == ( "station_city" )
		     || __TypeAffichage == ( "station_city_if_new" ) && __DP->getDisplayedConnectionPlaces ()[__i] ->getCity() != __DerniereCommune
		    )
		{
		    std::stringstream ss;
		    boost::iostreams::filtering_ostream out;
		    out.push (synthese::util::LowerCaseFilter());
		    out.push (synthese::util::PlainCharFilter());
		    out.push (ss);
			    
		    out << __DP->getDisplayedConnectionPlaces () [__i] ->getCity () ->getName();

		    // std::stringMinuscules __TexteMinuscule;
		    // __TexteMinuscule << __DP->GetGare( __i ) ->getTown() ->getName();

		    pCtxt << __AvantCommune << ss.str () << __ApresCommune;
		    __DerniereCommune = __DP->getDisplayedConnectionPlaces () [__i]->getCity();


		}

		// Affichage du nom d'arrï¿½t dans les cas oï¿½ nï¿½cessaire
		if ( __TypeAffichage.substr (0, 6) == "station" )
		    pCtxt << __DP->getDisplayedConnectionPlaces () [__i] ->getName();

		pCtxt << __DP->getDisplayedConnectionPlaces () [__i]->getName (); // temporary
                /* TODO hugues // Affichage de la destination 13 caracteres dans les cas ou necessaire
		   if ( __TypeAffichage == "char(13)" )
		   pCtxt << __DP->getDisplayedConnectionPlaces () [__i]->getDesignation13();

		   // Affichage de la destination 26 caracteres dans les cas ou necessaire
		   if ( __TypeAffichage == "char(26)" )
		   pCtxt << __DP->getDisplayedConnectionPlaces () [__i]->getDesignation26();
		*/
	    }
	}
    }
    break;

    case EI_BIBLIOTHEQUE_DescriptionPassage_ArretPhysique:  //47
	break;

    case EI_BIBLIOTHEQUE_DescriptionPassage_Heure:  //48
    {
	// Lecture des paramï¿½tres
	const synthese::time::DateTime& __Moment = ( ( const PassThroughDescription* ) __Objet )->getRealMoment ();
	std::string __Zero = _Parametres[ 0 ] ->Texte( __Parametres );
	std::string __AvantSiImminent = _Parametres[ 1 ] ->Texte( __Parametres );
	std::string __ApresSiImminent = _Parametres[ 2 ] ->Texte( __Parametres );

	synthese::time::DateTime __Maintenant;
	__Maintenant.updateDateTime( synthese::time::TIME_CURRENT );
	int __Duree = __Moment - __Maintenant;

	if ( __Duree <= 1 )
	    pCtxt << __AvantSiImminent;
	if ( __Moment.getHour ().getHours() < 10 )
	    pCtxt << __Zero;
	pCtxt << __Moment.getHour ();
	if ( __Duree <= 1 )
	    pCtxt << __ApresSiImminent;
    }
    break;

    case EI_BIBLIOTHEQUE_Cases_Particularites:  //49
    {
	const cTrajets* __Trajets = ( const cTrajets* ) __Objet;

	for ( int __i = 0; __i < __Trajets->Taille(); __i++ )
	{
	    // Crï¿½ation d'un objet de paramï¿½tres
	    cInterface_Objet_Connu_ListeParametres __ParametresCase;

	    //0
	    __ParametresCase << __i + 1;

	    // 1: Dï¿½signation principale
	    __ParametresCase << ( __Trajets->operator [] ( __i ) ).getMaxAlarmLevel ();

	    // Lancement de l'affichage
	    __Site->Affiche( pCtxt, INTERFACECaseParticularite, __ParametresCase, NULL );

	}
    }
    break;

    case EI_BIBLIOTHEQUE_Tbdep_NumeroPanneau:  //50
    {
	// Lecture des paramï¿½tres
	int __Nombre = _Parametres[ 0 ] ->Nombre( __Parametres );
	std::string __Format = _Parametres[ 1 ] ->Texte( __Parametres );

	if ( __Format == "char(2)")
	{
	    __Nombre = __Nombre % 100;
	    pCtxt << __Nombre / 10;
	    pCtxt << __Nombre % 10;
	}

    }
    break;


    }


    return INCONNU;
    }




    
    
