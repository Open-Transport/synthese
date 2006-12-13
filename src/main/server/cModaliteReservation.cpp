/*! \file cResa.cpp
\brief Impl�mentation classes Modalit�s de r�servation
*/

#include "cModaliteReservation.h"
#include "04_time/DateTime.h"
#include "04_time/Schedule.h"
#include "cTrain.h"
#include "cModaliteReservationEnLigne.h"


/*! \brief Constructeur
 \param newIndex Num�ro de la modalit� de r�servation dans l'environnement
 \author Hugues Romain
 \date 2001-2005
*/
cModaliteReservation::cModaliteReservation( const size_t& id )
        : _id( id )
{
    vReferenceEstLOrigine = false;
    vDelaiMaxJours = 0;
    vDelaiMinHeureMax.updateHour( synthese::time::TIME_MAX );
    vDelaiMinJours = 0;
    vDelaiMinMinutes = 0;
}


/** Destructor.
*/
cModaliteReservation::~cModaliteReservation()
{}

/*! \brief  Fonction de r�f�rence pour le calcul de la date de d�but de r�servation
 \param MomentResa Moment de la demande de r�servation
 \return Le moment minimal pour effectuer une r�servation. 
 
Si aucune r�gle ne le fixe, alors le moment exact de la demande de r�servation est retourn�e. Ainsi, la r�servation est autoris�e si le moment de r�servation est sup�rieur ou �gal au r�sultat produit par cette m�thode.
*/
synthese::time::DateTime cModaliteReservation::momentDebutReservation( const synthese::time::DateTime &MomentResa ) const
{
    synthese::time::DateTime momentDebut = MomentResa;

    if ( vDelaiMaxJours )
    {
        momentDebut.subDaysDuration( vDelaiMaxJours );
        momentDebut.updateHour( synthese::time::TIME_MIN );
    }

    return momentDebut;
}


/** Fonction de r�f�rence pour le calcul de la date limite
 
Le calcul du moment limite de r�servation est le suivant :
  - Choix du moment de r�f�rence (d�part du voyageur ou bien d�part de la circulation � l'origine)
  - Calcul m�thode 1 : x minutes avant moment de r�f�rence :
   - Diminution du nombre de minutes minimales avant r�f�rence
  - Calcul m�thode 2 : x jours avant moment de r�f�rence :
   - Diminution du nombre de jours minimal avant r�f�rence
   - Majoration de l'heure � l'heure maximale de r�servation du dernier jour
  - Le moment le plus t�t est choisi
  
Si aucune r�gle ne fixe le moment limite de r�servation, alors le moment exact du d�part est retourn� (la r�servation est autoris�e jusqu'au d�part effectif). Ainsi, la r�servation est autoris�e si le moment de r�servation est inf�rieur ou �gal au r�sultat produit par cette m�thode.
 
  \todo Am�liorer la vari�t� des param�tres pour diminuer le nombre de d�lais de r�servations dus aux r�servations la veille pour les premiers services du matin : cela �viterait notamment le doublage des lignes pour tenir compte de ce probl�me
*/
synthese::time::DateTime cModaliteReservation::momentLimiteReservation( const cTrain* tCirculation, const synthese::time::DateTime &MomentDepart ) const
{
    // Moment de r�f�rence
    synthese::time::DateTime MomentDeReference = MomentDepart;
    if ( vReferenceEstLOrigine )
    {
        // heure de d�part de l'origine sup�rieure � l'heure de mont�e : il a lieu la veille
        if ( *tCirculation->getHoraireDepartPremier() > MomentDepart.getHour () )
            MomentDeReference--;
        MomentDeReference = *tCirculation->getHoraireDepartPremier();
    }

    synthese::time::DateTime momentMinutes = MomentDeReference;
    synthese::time::DateTime momentJours = MomentDeReference;

    if ( vDelaiMinMinutes )
        momentMinutes -= vDelaiMinMinutes;

    if ( vDelaiMinJours )
    {
        momentJours.subDaysDuration( vDelaiMinJours );
        momentJours.updateHour( synthese::time::TIME_MAX );
    }

    if ( vDelaiMinHeureMax < momentJours.getHour () )
        momentJours.setHour( vDelaiMinHeureMax );

    if ( momentMinutes < momentJours )
        return momentMinutes;
    else
        return momentJours;
}


/*! \brief Modificateur sans contr�le de valeur du d�lai maximal en jours
 \param newVal D�lai maximal en jours
 \author Hugues Romain
 \date 2002
*/
void cModaliteReservation::setDelaiMaxJours( const int newVal )
{
    vDelaiMaxJours = newVal;
}


/*! \brief Modificateur sans contr�le de valeur du prix de la r�servation
 \param newVal Prix
 \author Hugues Romain
 \date 2002
*/ 
/*void cModaliteReservation::setPrix(const tPrix newVal)
{
 vPrix = newVal;
}
*/

/*! \brief Modificateur avec contr�le de valeur du prix de la r�servation
 \param newVal Prix
 \return true si la valeur propos�e a �t� accept�e
 \author Hugues Romain
 \date 2002
 
Pour �tre accept�, le prix doit �tre positif.
*/ 
/*bool cModaliteReservation::SetPrix(const float newVal)
{
 if (newVal >= 0)
 {
  setPrix((tPrix) newVal);
  return true;
 }
 else
  return false;
}
*/


/*! \brief Indique si la r�servation est possible sur un service � une date donn�e, en tenant compte des r�gles de d�lai
 \param Circulation Pointeur vers le service sur lequel la r�servation est demand�e
 \param MomentResa Moment de la demande de r�servation
 \param MomentDepart Moment du d�part souhait�
 \return true si la r�servation est possible, false sinon
 \author Hugues Romain
 \date 2005
 
Cette m�thode contr�le les �l�ments suivants :
 - le moment de r�servation doit se situer avant le moment limite de r�servation (voir cResa::momentLimiteReservation())
 - le moment de r�servation doit se situer apr�s le moment de d�but d'ouverture du service � la r�servation (voir cResa::momentDebutReservation())
*/
bool cModaliteReservation::reservationPossible( const cTrain* Circulation, const synthese::time::DateTime &MomentResa, const synthese::time::DateTime &MomentDepart ) const
{
    return MomentResa <= momentLimiteReservation( Circulation, MomentDepart )
           && MomentResa >= momentDebutReservation( MomentResa );
}



/*! \brief Indique si la circulation peut �tre emplrunt�e � une date donn�e, en tenant compte des r�gles de d�lai et des r�gles de r�servation
 \param tCirculation Pointeur vers le service � tester
 \param MomentResa Moment de l'�ventuelle demande de r�servation si n�cessaire
 \param MomentDepart Moment du d�part souhait�
 \return true si le service peut �tre emprunt�, false sinon
 \author Hugues Romain
 \date 2005
 
Cette m�thode contr�le les �l�ments suivants :
 - si la r�servation n'est pas obligatoire, alors la circulation peut �tre emprunt�e
 - si la r�servation est obligatoire, alors le moment de r�servation doit respecter les crit�res de d�lai :
  - se situer avant le moment limite de r�servation
  - se situer apr�s le moment de d�but d'ouverture du service � la r�servation
*/
bool cModaliteReservation::circulationPossible( const cTrain* tCirculation, const synthese::time::DateTime &MomentResa, const synthese::time::DateTime &MomentDepart ) const
{
    return vTypeResa == RuleType_IMPOSSIBLE
           || vTypeResa == RuleType_OPTIONNAL
           || reservationPossible( tCirculation, MomentResa, MomentDepart );
}



/*! \brief Accesseur Activation du type de la modalit� de r�servation
 \return Le type de la modalit� de r�servation
 \author Hugues Romain
 \date 2002
*/
const cModaliteReservation::RuleType& cModaliteReservation::TypeResa() const
{
    return vTypeResa;
}



/*! \brief Modificateur sans contr�le de valeur de l'heure maximale de r�servation le dernier jour de la p�riode d'ouverture
 \param newVal Heure maximale de r�servation le dernier jour de la p�riode d'ouverture
 \author Hugues Romain
 \date 2002
*/
void cModaliteReservation::setDelaiMinHeureMax( const synthese::time::Hour &newVal )
{
    vDelaiMinHeureMax = newVal;
}



/*! \brief Modificateur avec contr�le de valeur de l'heure maximale de r�servation le dernier jour de la p�riode d'ouverture
 \param newVal Heure maximale de r�servation le dernier jour de la p�riode d'ouverture
 \return true si la valeur a �t� accept�e, false sinon
 \author Hugues Romain
 \date 2002
 
Pour �tre accept�e, l'heure maximale de r�servation le dernier jour de la p�riode d'ouverture doit �tre une heure valide (voir synthese::time::Hour::OK())
*/
bool cModaliteReservation::SetDelaiMinHeureMax( const synthese::time::Hour &newVal )
{
    if ( newVal.isValid () )
    {
        setDelaiMinHeureMax( newVal );
        return true;
    }
    else
        return false;
}

const synthese::time::Hour& cModaliteReservation::GetDelaiMinHeureMax() const
{
    return vDelaiMinHeureMax;
}


bool cModaliteReservation::SetDoc( const std::string& newVal )
{
    vDescription = newVal;
    return ( true );
}

bool cModaliteReservation::SetTel( const std::string& newVal )
{
    vNumeroTelephone = newVal;
    return ( true );
}

bool cModaliteReservation::SetHorairesTel( const std::string& newVal )
{
    vHorairesTelephone = newVal;
    return ( true );
}

const std::string& cModaliteReservation::GetSiteWeb() const
{
    return ( vSiteWeb );
}

const std::string& cModaliteReservation::GetHorairesTelephone() const
{
    return ( vHorairesTelephone );
}



/*! \brief Accesseur index de l'objet dans l'environnement
 \return L'index de l'objet dans l'environnement
 \author Hugues Romain
 \date 2001-2005
*/
const size_t& cModaliteReservation::Index() const
{
    return _id;
}


bool cModaliteReservation::SetTypeResa( const char newVal )
{
    switch ( ( RuleType ) newVal )
    {
        case RuleType_AT_LEAST_ONE_REQUIRED:
        case RuleType_COMPULSORY:
        case RuleType_IMPOSSIBLE:
        case RuleType_OPTIONNAL:
            setTypeResa( ( RuleType ) newVal );
            return ( true );
    }
    return ( false );
}

void cModaliteReservation::setTypeResa( const RuleType& newVal )
{
    vTypeResa = newVal;
}

bool cModaliteReservation::SetDelaiMinMinutes( const int newVal )
{
    if ( newVal >= 0 )
    {
        int NewVal;
        setDelaiMinMinutes( NewVal = newVal );
        return ( true );
    }
    else
        return ( false );
}

void cModaliteReservation::setDelaiMinMinutes( const int& newVal )
{
    vDelaiMinMinutes = newVal;
}

void cModaliteReservation::setDelaiMinJours( const int newVal )
{
    vDelaiMinJours = newVal;
}

bool cModaliteReservation::SetDelaiMinJours( const int newVal )
{
    if ( newVal >= 0
            && ( !vDelaiMaxJours || newVal < vDelaiMaxJours )
       )
    {
        setDelaiMinJours( ( int ) newVal );
        return ( true );
    }
    else
        return ( false );
}

bool cModaliteReservation::SetDelaiMaxJours( const int newVal )
{
    if ( newVal >= 0
            && ( !vDelaiMinJours || newVal > vDelaiMinJours )
       )
    {
        setDelaiMaxJours( ( int ) newVal );
        return ( true );
    }
    else
        return ( false );
}



/*! \brief Accesseur Activation de la r�servation en ligne
 \return false car la r�servation en ligne est impossible sur cette classe
 \author Hugues Romain
 \date 2005
*/
const cModaliteReservationEnLigne* cModaliteReservation::ReservationEnLigne() const
{
    return dynamic_cast<const cModaliteReservationEnLigne*>( this );
}



/*! \brief Modificateurs du moment de r�f�rence pour le calcul de l'heure limite de r�servation
 \param newVal  - true : le moment de r�f�rence est le moment de d�part de la circulation � son origine
     - false : le moment de r�f�rence est le moment de d�part du voyageur � son arr�t
 \return true si la modification a r�ussi
 \author Hugues Romain
 \date 2005
*/
bool cModaliteReservation::setReferenceEstLOrigine ( const bool newVal )
{
    vReferenceEstLOrigine = newVal;
    return true;
}



/*! \brief Affichage XML de la modalit� de r�servation
 \todo Refaire cete m�thode (ils ont rien compris.......) et la mettre sous forme de flux std::stringXML
*/ 
/*template <class T> T& cModaliteReservation::toXML(T& Tampon, const cTrain* Circulation, synthese::time::DateTime momentDepart) const
{
 Tampon << "<reservation>";
 
 momentLimiteReservation(Circulation, momentDepart).toXML(Tampon,"datelimite");
 
 //ajout date limite
 if (GetTelephone().GetTaille())
 {
  Tampon << "<telephone ";
  if (GetHorairesTelephone().GetTaille())
   Tampon << "horaire=\"" << GetHorairesTelephone() << "\"";
   
  Tampon << ">" << GetTelephone() << "</telephone>"; 
 }
 
 //ajout site web
 if (GetSiteWeb().GetTaille())
  Tampon << "<web>"<< GetSiteWeb() << "</web>";
 
 Tampon << "</reservation>";
 
 return (Tampon);
}*/

bool cModaliteReservation::SetSiteWeb( const std::string &newVal )
{
    vSiteWeb = newVal;
    return ( true );
}

const std::string& cModaliteReservation::GetTelephone() const
{
    return ( vNumeroTelephone );
}


