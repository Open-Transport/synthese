#include "cSite.h"
#include "cInterface_Objet.h"

#include "11_interfaces/Interface.h"

using namespace std;

using synthese::env::Environment;
using synthese::interfaces::Interface;



/*!  \brief Constructeur
 \param clef Cl� d'identification du site
 \author Hugues Romain
 \date 2005
 
Les objets site d'affichage sont initialis�s par les valeurs par d�faut suivantes :
 - Interface et environnement non d�clar�s
 - Validit� permanente
 - R�servation en ligne autoris�e
 - Pas d'affichage des solutions pass�es
 
Par ailleurs, le constructeur initialise d'ores et d�j� les valeurs suivantes :
 - Cl� d'identification du site
*/
cSite::cSite( const string& clef )
        : vClef( clef )
{
    // Interface et environnement non d�clar�s
    _Interface = NULL;
    _Environnement = NULL;

    // Validit� permanente
    vDateDebut.updateDate( synthese::time::TIME_MIN );
    vDateFin.updateDate( synthese::time::TIME_MAX );

    // R�servation en ligne autoris�e
    vResaEnLigne = true;

    // Solutions pass�es non affich�es
    _SolutionsPassees = false;
}



/*!  \brief V�rification de la validit� du site
 \return true si le site peut �tre utilis�, false sinon
 \author Hugues Romain
 \date 2005
 
La validit� d'un site client est d�finie de la mani�re suivante :
 - Le site doit �tre d�crit par une interface et un environnement
 - La date courante doit �tre incluse dans la p�riode de validit� du site
*/
bool cSite::valide() const
{
    // Test de la d�claration pr�alable d'une interface
    if ( !_Interface )
        return false;

    // Test de la d�claration pr�alable d'un environnement
    if ( !_Environnement )
        return false;

    // Test de la validit� de la date du jour
    synthese::time::Date maintenant;
    maintenant.updateDate();
    if ( maintenant < vDateDebut || maintenant > vDateFin )
        return false;

    // Retour OK
    return true;
}



/*! \brief Affichage d'un �l�ment de l'interface du site selon les param�tres fournis
*/
void cSite::Affiche( ostream& __Flux, int __IndexObjet, const cInterface_Objet_Connu_ListeParametres& __Parametres, const void* __Objet ) const
{
    return ( ( cInterface_Objet& ) getInterface() ->operator[] ( __IndexObjet ) ).Evalue( __Flux, __Parametres, __Objet, this );
}



/*! \brief Modificateur de l'URL du binaire client, destin� � construire des liens
 \param __URLClient Valeur
 \return true si la modification a �t� effectu�e avec succ�s
*/
bool cSite::setURLClient( const string& __URLClient )
{
    _URLClient = __URLClient;
    return true;
}



const string& cSite::getURLClient() const
{
    return _URLClient;
}


const string& cSite::getClef() const
{
    return vClef;
}

const string& cSite::getIdentifiant() const
{
    return ( vIdentifiant );
}

void cSite::setIdentifiant( const string& id )
{
    vIdentifiant = id;
}

void cSite::setDateDebut( const synthese::time::Date& dateDebut )
{
    vDateDebut = dateDebut;
}

void cSite::setDateFin( const synthese::time::Date& dateFin )
{
    vDateFin = dateFin;
}

const synthese::time::Date& cSite::getDateDebut() const
{
    return ( vDateDebut );
}

const synthese::time::Date& cSite::getDateFin() const
{
    return ( vDateFin );
}

bool cSite::ResaEnLigne() const
{
    return vResaEnLigne;
}

bool cSite::setResaEnLigne( const bool valeur )
{
    vResaEnLigne = valeur;
    return true;
}


/*! \brief Modificateur Environnement
 \param __Objet L'environnement � lier au site
 \return true si l'op�ration a �t� effectu�e avec succ�s
*/
bool cSite::SetEnvironnement( Environment* __Objet )
{
    _Environnement = __Objet;

    if ( __Objet == NULL )
        return false;

    return true;
}



/*! \brief Modificateur Interface
 \param __Objet L'interface � lier au site
 \return true si l'op�ration a �t� effectu�e avec succ�s
*/
bool cSite::SetInterface( const Interface* __Objet )
{
    _Interface = __Objet;

    if ( __Objet == NULL )
        return false;

    return true;
}



/*! \brief Accesseur Environnement
*/
Environment* cSite::getEnvironnement() const
{
    return _Environnement;
}



/*! \brief Accesseur Interface
*/
const Interface* cSite::getInterface() const
{
    return _Interface;
}

/*! \brief Accesseur SolutionsPassees
*/
bool cSite::getSolutionsPassees() const
{
    return _SolutionsPassees;
}

/*! \brief Modificateur Solutions pass�es
 \param __SolutionsPassees Etat du filtre solutions pass�es
*/
bool cSite::setSolutionsPassees( bool __SolutionsPassees )
{
    _SolutionsPassees = __SolutionsPassees;
    return true;
}

