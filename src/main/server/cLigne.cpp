/*! \file cLigne.cpp
\brief Impl�mentation classe cLigne
*/

#include "cGareLigne.h"
#include "cArretPhysique.h"
#include "LogicalPlace.h"
#include "cTrain.h"
#include "cMateriel.h"
#include "cLigne.h"
#include "cEnvironnement.h"

#include "04_time/Schedule.h"

#include <sstream>
#include <01_util/PlainCharFilter.h>
#include <boost/iostreams/filtering_stream.hpp>


/*! \brief Destructeur
 \author Hugues Romain
 \date 2002
*/
cLigne::~cLigne()
{}




/*! \brief Constructeur avec copie d'une autre ligne
 \author Hugues Romain
 \date 2002
 \param LigneACopier Ligne servant de mod�le � la copie
 \param Environnement Environnement auquel appartient la ligne r�sultat
*/ 
/*#ifdef ClasseAdmin
cLigne::cLigne(cLigne* LigneACopier, cEnvironnement* Environnement, cImport* Import)
#else
cLigne::cLigne(cLigne* LigneACopier, cEnvironnement* Environnement)
#endif
{
 operator =(*LigneACopier);
 vAxe = LigneACopier->vAxe;
 vCode.Vide();
 vCode << LigneACopier->vCode;
 Environnement->NomLigneUnique(vCode);
 
 // Ligne � chainer
 vSuivant = Environnement->PremiereLigne();
 Environnement->setPremiereLigne(this);
 
 // Initialisation
 vCirculation.setAnnees(Environnement->PremiereAnnee(), Environnement->DerniereAnnee());
 vPremiereGareLigne = NULL;
 vTarif =NULL;
 vVelo=NULL;
 vResa=NULL;
 vHandicape=NULL;
 vNombreServices = 0;
 vAAfficherSurIndicateurs = true;
 
 // Construction de la ligne
 for (cGareLigne* curGareLigne=LigneACopier->PremiereGareLigne(); curGareLigne!=NULL; curGareLigne=curGareLigne->Suivant())
  addGareLigne(new cGareLigne(this, *curGareLigne));
 Ferme();
 
}*/


/*! \brief Constructeur de base avec nom de code
 \param newCode Code de la ligne r�sultat
 \param newAxe Axe auquel appartiendra la ligne
 \param environment Environnement auquel appartiendra la ligne
 \author Hugues Romain
 \date 2002
 */
cLigne::cLigne( const std::string& newCode, cAxe* const newAxe, cEnvironnement* const environment )
        : vAxe( newAxe )
        , vCirculation( environment->PremiereAnnee(), environment->DerniereAnnee(), 0, "" )
        , vResa( environment->getResa( 0 ) )
        , vVelo( environment->getVelo( 0 ) )
        , vHandicape( environment->getHandicape( 0 ) )
        , vCode( newCode )
{
    // Valeurs par d�faut
    vReseau = NULL;
    vMateriel = NULL;
    vTarif = NULL;
    vAAfficherSurTableauDeparts = true;
    vAAfficherSurIndicateurs = true;
}





bool cLigne::allowAddServiceHoraire( const synthese::time::Schedule* tbHoraires ) const
{
    bool ControleDepassementPositif = true;
    bool ControleDepassementNegatif = true;

    // D�termination du numero de service futur
    // Insertion du service: d�termination du rang
    size_t iNumeroService;

    for ( iNumeroService = 0; iNumeroService < vTrain.size(); iNumeroService++ )
        if ( tbHoraires[ 1 ] < _lineStops.front() ->getHoraireDepartPremier( iNumeroService ) )
            break;
    //END PORTAGE LINUX
    if ( iNumeroService == vTrain.size() )
        ControleDepassementPositif = false;
    if ( iNumeroService == 0 )
        ControleDepassementNegatif = false;

    size_t i = 0;
    for ( LineStops::const_iterator iter = _lineStops.begin();
            iter != _lineStops.end();
            ++iter )
    {
        cGareLigne* curGareLigne = *iter;
        if ( curGareLigne->HorairesSaisis() )
        {
            // Depassement negatif
            if ( ControleDepassementNegatif )
            {
                if ( curGareLigne->EstArrivee() && curGareLigne->getHoraireArriveeDernier( iNumeroService - 1 ) >= tbHoraires[ i ] )
                    return ( false );
                if ( curGareLigne->EstDepart() && curGareLigne->getHoraireDepartDernier( iNumeroService - 1 ) >= tbHoraires[ i + 1 ] )
                    return ( false );
            }
            if ( ControleDepassementPositif )
            {
                if ( curGareLigne->EstArrivee() && curGareLigne->getHoraireArriveePremier( iNumeroService ) <= tbHoraires[ i ] )
                    return ( false );
                if ( curGareLigne->EstDepart() && curGareLigne->getHoraireDepartPremier( iNumeroService ) <= tbHoraires[ i + 1 ] )
                    return ( false );
            }
            i += 2;
        }
    }
    return true;
}


/*! \brief Ajout d'un point d'arr�t en fin de ligne
 \param GLigneACopier Objet � copier (NULL = cr�ation d'un nouvel objet vide)
 \param newPM Point m�trique du nouvel arr�t
 \param newArretPhysique Num�ro de l'arr�t physique au sein de l'arr�t loguque
 \param newType Type de desserte � l'arr�t (d�part et/ou arriv�e)
 \param newPA Arr�t logique desservi
 \param newHorairesSaisis Indique si les horaires sont fournis dans le fichier source
 \param Route ???
 \warning Cette fonction ne peut �tre utilis�e que si la ligne ne poss�de aucune circulation
 \author Hugues Romain
 \date 2002
 
 En cas de succession d'arr�ts identiques, la relation entre les deux arr�ts n'est pas possible.
*/
void cLigne::addGareLigne( cGareLigne* newGareLigne )
{
    _lineStops.push_back( newGareLigne );

    if ( _lineStops.size() > 1 )
    {
        // Chainages d�part/arriv�e
        for ( LineStops::reverse_iterator riter = _lineStops.rbegin();
                ( riter != _lineStops.rend() )
                && (
                    ( *riter ) ->getArriveeSuivante() == NULL
                    || ( *riter ) ->getArriveeCorrespondanceSuivante() == NULL
                    || newGareLigne->getDepartPrecedent() == NULL
                    || newGareLigne->getDepartCorrespondancePrecedent() == NULL
                );
                ++riter )
        {
            cGareLigne* __GL = *riter;
            // On chaine uniquement les relations entre A et A, D et D, A et D si arr�ts diff�rents, D et A si arr�ts diff�rents
            if ( __GL->ArretPhysique() ->getLogicalPlace() != newGareLigne->ArretPhysique() ->getLogicalPlace() || __GL->TypeDA() == newGareLigne->TypeDA() )
            {
                // Chainage arriv�es suivantes
                if ( __GL->getArriveeSuivante() == NULL && newGareLigne->EstArrivee() )
                    __GL->setArriveeSuivante( newGareLigne );
                if ( __GL->getArriveeCorrespondanceSuivante() == NULL && newGareLigne->ArretPhysique() ->getLogicalPlace() ->CorrespondanceAutorisee() )
                    __GL->setArriveeCorrespondanceSuivante( newGareLigne );

                if ( __GL->EstDepart() && newGareLigne->getDepartPrecedent() == NULL )
                    newGareLigne->setDepartPrecedent( __GL );
                if ( __GL->EstDepart() && newGareLigne->getDepartCorrespondancePrecedent() == NULL && __GL->ArretPhysique() ->getLogicalPlace() ->CorrespondanceAutorisee() )
                    newGareLigne->setDepartCorrespondancePrecedent( __GL );
            }
        }
    }
}


/*! \brief Cloture d'une ligne
 \author Hugues Romain
 \date 2002
*/
void cLigne::Ferme()
{
    // Suppression du statut arret de d�part de pas d'arriv�e � desservir et vv
    for ( LineStops::const_iterator iter = _lineStops.begin();
            iter != _lineStops.end();
            ++iter )
    {
        cGareLigne* __GL = *iter;
        if ( __GL->getArriveeSuivante() == NULL )
            __GL->setTypeDA( cGareLigne::Arrivee );
        if ( __GL->getDepartPrecedent() == NULL )
            __GL->setTypeDA( cGareLigne::Depart );
    }
}


/*! \brief Met � jour le calendrier g�n�ral de la ligne
 \author Hugues Romain
 \date 2002
 
 Le calendrier g�n�r� indique chaque jour o� au moins une circulation de la ligne fonctionne.
 Il tient compte des circulations passant minuit : si au moins une minute d'une journ�e est concern�e par l'une des circulations, alors la date est s�lectionn�e.
 De ce fait, si la demande de calcul se fait sur un jour non s�lectionn� dans le calendrier g�n�r�, alors la ligne peut �tre filtr�e.
*/
void cLigne::MajCirculation()
{
    vCirculation.RAZMasque();
    cGareLigne* lastGL = _lineStops.back();
    for ( size_t iNumeroService = 0; iNumeroService < vTrain.size(); iNumeroService++ )
    {
        if ( lastGL->getHoraireArriveeDernier( iNumeroService ).getDaysSinceDeparture() != _lineStops.front() ->getHoraireDepartPremier( iNumeroService ).getDaysSinceDeparture() )
        {
            vCirculation.RAZMasque( true );
            break;
        }
        vTrain.at( iNumeroService ) ->getJC() ->SetInclusionToMasque( vCirculation );
    }
}


/*! \brief Copie d'une ligne
 \author Hugues Romain
 \date 2001
 \warning Cette fonction ne copie pas les circulations, horaires, arr�ts desservis...
 \return La ligne elle-m�me
 */
cLigne* cLigne::operator =( const cLigne& LigneACopier )
{
    // Copie des parametres
    vLibelleSimple = LigneACopier.vLibelleSimple;
    vLibelleComplet = LigneACopier.vLibelleComplet;
    vStyle = LigneACopier.vStyle;
    vImage = LigneACopier.vImage;
    vNomPourIndicateur = LigneACopier.vNomPourIndicateur;
    vResa = LigneACopier.vResa;
    vVelo = LigneACopier.vVelo;
    vMateriel = LigneACopier.vMateriel;
    vAAfficherSurTableauDeparts = LigneACopier.vAAfficherSurTableauDeparts;

    return ( this );
}




/*! \brief Indique si la ligne correspond � une jonction � pied
*/
bool cLigne::EstUneLigneAPied() const
{
    return Materiel() ->EstAPied();
}




/*! \brief Accesseur Girouette
*/
const std::string& cLigne::getGirouette() const
{
    return vGirouette;
}



/*! \brief Accesseur train
*/
cTrain* cLigne::getTrain( size_t __NumeroService ) const
{
    return ( __NumeroService < vTrain.size() ) ? vTrain.at( __NumeroService ) : NULL;
}


/*! \brief Calcul de la possibilit� qu'au moins un service circule le jour donn�, selon le calendrier g�n�ral de la ligne
 \param __Date Date � tester
 \author Hugues Romain
 \date 2005
*/
bool cLigne::PeutCirculer( const synthese::time::Date& __Date ) const
{
    return vCirculation.Circule( __Date );
}

bool cLigne::SetAUtiliserDansCalculateur( bool __Valeur )
{
    _AUtiliserDansCalculateur = __Valeur;
    return true;
}

void cLigne::setImage( const std::string& newImage )
{
    vImage = newImage;
}

void cLigne::setLibelleComplet( const std::string& newLibelleComplet )
{
    vLibelleComplet = newLibelleComplet;
}

void cLigne::setAAfficherSurTableauDeparts( bool newVal )
{
    vAAfficherSurTableauDeparts = newVal;
}

bool cLigne::AAfficherSurTableauDeparts() const
{
    return ( vAAfficherSurTableauDeparts );
}

void cLigne::setResa( cModaliteReservation* newVal )
{
    vResa = newVal;
}

void cLigne::setVelo( cVelo* newVal )
{
    vVelo = newVal;
}

void cLigne::setGirouette( const std::string& newGirouette )
{
    vGirouette = newGirouette;
}

// SET Gestion du filtre Velo
cVelo* cLigne::getVelo() const
{
    return ( vVelo );
}

void cLigne::setHandicape( cHandicape* newVal )
{
    vHandicape = newVal;
}

cHandicape* cLigne::getHandicape() const
{
    return ( vHandicape );
}

void cLigne::setTarif( cTarif* newVal )
{
    vTarif = newVal;
}

const cTarif* cLigne::getTarif() const
{
    return ( vTarif );
}

const cAlerte& cLigne::getAlerte() const
{
    return vAlerte;
}

//END SET

void cLigne::setMateriel( cMateriel* newVal )
{
    vMateriel = newVal;
}


cAxe* cLigne::Axe() const
{
    return ( vAxe );
}

cMateriel* cLigne::Materiel() const
{
    return ( vMateriel );
}

const std::string& cLigne::getCode() const
{
    return ( vCode );
}

const std::string& cLigne::getNomPourIndicateur() const
{
    return ( vNomPourIndicateur );
}


void cLigne::setStyle( const std::string& newStyle )
{
    vStyle = newStyle;
}

void cLigne::setLibelleSimple( const std::string& newNom )
{
    vLibelleSimple = newNom;
    if ( vNomPourIndicateur.size() == 0)
    {
	setNomPourIndicateur (vLibelleSimple);
    }
}

void cLigne::setNomPourIndicateur( const std::string& newNom )
{
    std::stringstream sout;
    synthese::util::PlainCharFilter filter;
    boost::iostreams::filtering_ostream out;
    out.push (filter);
    out.push (sout);
    
    out << newNom << std::flush;
    vNomPourIndicateur = sout.str ();
}


void cLigne::setReseau( cReseau* newReseau )
{
    vReseau = newReseau;
}


cModaliteReservation* cLigne::GetResa() const
{
    return ( vResa );
}

bool cLigne::AAfficherSurIndicateurs() const
{
    return ( vAAfficherSurIndicateurs );
}

void cLigne::setAAfficherSurIndicateurs( bool newVal )
{
    vAAfficherSurIndicateurs = newVal;
}

cReseau* cLigne::getReseau() const
{
    return ( vReseau );
}

/*! \brief Accesseur libell� complet de la ligne
 \return Si un libell� complet est d�fini, renvoie le libell� complet de la ligne. Sinon renvoie le libell� simple de la ligne.
 \author Hugues Romain
 \date 2005
*/
const std::string& cLigne::getLibelleComplet() const
{
    return vLibelleComplet;
}



/*! \brief Accesseur libell� simple de la ligne
 \return Le libell� simple de la ligne.
 \author Hugues Romain
 \date 2005
*/
const std::string& cLigne::getLibelleSimple() const
{
    return vLibelleSimple;
}



/*! \brief Accesseur style CSS de la ligne
 \return R�f�rence constante vers le style de la ligne
 \author Hugues Romain
 \date 2005
*/
const std::string& cLigne::getStyle() const
{
    return vStyle;
}



/*! \brief Accesseur logo image de la ligne
 \return R�f�rence constante vers le chemin d'acc�s g�n�rique au logo image de la ligne
 \author Hugues Romain
 \date 2005
*/
const std::string& cLigne::getImage() const
{
    return vImage;
}



/*! \brief Code de la ligne sur nouvelle chaine de caract�res HTML
 \author Hugues Romain
 \date 2005
*/ /*
inline std::string cLigne::Code() const
{
 std::string tResultat;
 Code(tResultat);
 return tResultat;
} */


void cLigne::addService( cTrain* const service )
{
    vTrain.push_back( service );
}

cAlerte& cLigne::getAlerteForModification()
{
    return vAlerte;
}
