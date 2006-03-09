
#include "cTrajet.h"
#include "cAlerte.h"
#include "cModaliteReservation.h"
#include "LogicalPlace.h"

#include "01_util/Conversion.h"


/*! \brief Constructeur
 \author Hugues Romain
 \date 2005
*/
cTrajet::cTrajet()
{
    _PremierET = NULL;
    _AmplitudeServiceContinu = INCONNU;
    Vide();
}



/*! \brief Destructeur
 \author Hugues Romain
 \date 2005
*/
cTrajet::~cTrajet()
{
    Vide();
}



/*! \brief D�tection des situations d'alerte dans le trajet
 \return Le niveau d'alerte maximal du trajet
 \author Hugues Romain
 \date 2005
 
Pour chaque �l�ment de trajet, 4 cas d'alerte sont possibles :
 - Message d'alerte sur arr�t de d�part
 - Circulation � r�servation
 - Message d'alerte sur circulation
 - Message d'alerte sur arr�t d'arriv�e
 
 Cette m�thode stocke le niveau d'alerte maximal du trajet dans la variable appropri�e, sous forme de texte pour exploitabilit� par le module d'interface.
*/
int cTrajet::GenererNiveauxAlerte()
{
    // Variables locales
    synthese::time::DateTime __debutAlerte, __finAlerte, maintenant;
    int __NiveauMaxAlerte = 0;

    // Parcours du trajet
    for ( cElementTrajet * __ET = _PremierET; __ET != NULL; __ET = __ET->getSuivant() )
    {
        // Alerte sur arr�t de d�part
        // D�but alerte = premier d�part
        // Fin alerte = dernier d�part
        __debutAlerte = __ET->MomentDepart();
        __finAlerte = __debutAlerte;
        if ( getAmplitudeServiceContinu() )
            __finAlerte += getAmplitudeServiceContinu();
        if ( __ET->getOrigin() ->getLogicalPlace() ->getAlerte().showMessage( __debutAlerte, __finAlerte )
                && __NiveauMaxAlerte < __ET->getOrigin() ->getLogicalPlace() ->getAlerte().Niveau() )
            __NiveauMaxAlerte = __ET->getOrigin() ->getLogicalPlace() ->getAlerte().Niveau();

        // Circulation � r�servation obligatoire
        maintenant.updateDateTime();
        if ( __ET->getLigne() ->GetResa() ->TypeResa() == cModaliteReservation::RuleType_COMPULSORY
                && __ET->getLigne() ->GetResa() ->reservationPossible( __ET->getService(), maintenant, __ET->MomentDepart() )
                && __NiveauMaxAlerte < cAlerte::LEVEL_WARNING )
            __NiveauMaxAlerte = cAlerte::LEVEL_WARNING;

        // Circulation � r�servation possible
        maintenant.updateDateTime();
        if ( __ET->getLigne() ->GetResa() ->TypeResa() == cModaliteReservation::RuleType_OPTIONNAL
                && __ET->getLigne() ->GetResa() ->reservationPossible( __ET->getService(), maintenant, __ET->MomentDepart() )
                && __NiveauMaxAlerte < cAlerte::LEVEL_INFO )
            __NiveauMaxAlerte = cAlerte::LEVEL_INFO;

        // Alerte sur circulation
        // D�but alerte = premier d�part
        // Fin alerte = derni�re arriv�e
        __debutAlerte = __ET->MomentDepart();
        __finAlerte = __ET->MomentArrivee();
        if ( getAmplitudeServiceContinu() )
            __finAlerte += getAmplitudeServiceContinu();
        if ( __ET->getLigne() ->getAlerte().showMessage( __debutAlerte, __finAlerte )
                && __NiveauMaxAlerte < __ET->getLigne() ->getAlerte().Niveau() )
            __NiveauMaxAlerte = __ET->getLigne() ->getAlerte().Niveau();

        // Alerte sur arr�t d'arriv�e
        // D�but alerte = premi�re arriv�e
        // Fin alerte = dernier d�part de l'arr�t si correspondnce, derni�re arriv�e sinon
        __debutAlerte = __ET->MomentArrivee();
        __finAlerte = __debutAlerte;
        if ( __ET->getSuivant() != NULL )
            __finAlerte = __ET->getSuivant() ->MomentDepart();
        if ( getAmplitudeServiceContinu() )
            __finAlerte += getAmplitudeServiceContinu();
        if ( __ET->getDestination() ->getLogicalPlace() ->getAlerte().showMessage( __debutAlerte, __finAlerte )
                && __NiveauMaxAlerte < __ET->getDestination() ->getLogicalPlace() ->getAlerte().Niveau() )
            __NiveauMaxAlerte = __ET->getDestination() ->getLogicalPlace() ->getAlerte().Niveau();
    }

    // Stockage du r�sultat au format texte pour module d'interface
    _NiveauMaxAlerte = synthese::util::Conversion::ToString (__NiveauMaxAlerte);

    // Retour du r�sultat au format num�rique
    return __NiveauMaxAlerte;
}

/*! \brief Ecriture de l'objet vers un flux de sortie quelconque
 \param flux Le flux de sortie
 \param Obj L'objet std::string � �crire
 \author Hugues Romain
 \date 2005
 \return Le flux de sortie
*/
std::ostream& operator<<( std::ostream& flux, const std::string& Obj )
{
    flux << Obj;
    return flux;
}



/*! \brief Suppression du lien vers le premier �l�ment de trajet sans le supprimer (le trajet commence au second)
 \author Hugues Romain
 \date 2005
 \warning Si l'�l�ment de trajet supprim� n'est pas point� par ailleurs, ils sera laiss� en m�moire sans moyen de le retrouver. Il s'agirait alors d'une fuite m�moire. A utiliser avec pr�caution sur ce point l�.
*/
void cTrajet::DeliePremier()
{
    if ( _DernierET == _PremierET )
        DelieTous();
    else
    {
        _DureeRoulee -= _PremierET->DureeEnMinutesRoulee();
        _PremierET->vSuivant->_Precedent = NULL;
        _PremierET = _PremierET->vSuivant;
        CalculeDuree();
        _Taille--;
    }
}


/*! \brief Suppression du lien vers le dernier �l�ment de trajet sans le supprimer (le trajet termine � l'avant-dernier)
 \author Hugues Romain
 \date 2005
 \warning Si l'�l�ment de trajet supprim� n'est pas point� par ailleurs, ils sera laiss� en m�moire sans moyen de le retrouver. Il s'agirait alors d'une fuite m�moire. A utiliser avec pr�caution sur ce point l�.
*/
void cTrajet::DelieDernier()
{
    if ( _DernierET == _PremierET )
        DelieTous();
    else
    {
        _DureeRoulee -= _DernierET->DureeEnMinutesRoulee();
        _DernierET->_Precedent->setSuivant( NULL );
        _DernierET = _DernierET->_Precedent;
        CalculeDuree();
        _Taille--;
    }
}


/*! \brief Calcul de la dur�e totale du trajet
 \author Hugues Romain
 \date 2005
*/
void cTrajet::CalculeDuree()
{
    if ( !getPremierElement() || !getDernierElement() || getMomentArrivee().getHour().isUnknown () || getMomentDepart().getHour().isUnknown () )
        _Duree = INCONNU;
    else
        _Duree = getMomentArrivee() - getMomentDepart();
}



const synthese::time::DateTime& cTrajet::getMomentArrivee() const
{
    return _DernierET->MomentArrivee();
}



const synthese::time::DateTime& cTrajet::getMomentDepart() const
{
    return _PremierET->MomentDepart();
}




void cTrajet::LieEnPremier( cElementTrajet* __ET )
{
    if ( _PremierET == NULL )
    {
        _PremierET = __ET;
        _DernierET = __ET;
    }
    else
    {
        __ET->setSuivant( _PremierET );
        _PremierET = __ET;
    }
    CalculeDuree();
    _DureeRoulee += __ET->DureeEnMinutesRoulee();
    _AmplitudeServiceContinu = INCONNU;

    _Taille++;
}



/*! \brief Ajout d'un �l�ment de trajet en fin de trajet
 \param __ElementTrajet El�ment de trajet � ajouter
 \author Hugues Romain
 \date 2005
 \warning L'�l�ment est ajout� seul au trajet (tout chainage existant est rompu)
*/
void cTrajet::LieEnDernier( cElementTrajet* __ElementTrajet )
{
    // Chainage
    if ( _PremierET )
        _DernierET->setSuivant( __ElementTrajet );
    else
        _PremierET = __ElementTrajet;
    _DernierET = __ElementTrajet;
    _DernierET->setSuivant( NULL );

    // Dur�es
    _Duree = getMomentArrivee() - getMomentDepart();
    _DureeRoulee += __ElementTrajet->DureeEnMinutesRoulee();
    _AmplitudeServiceContinu = INCONNU;

    // Taille
    _Taille++;
}


void cTrajet::SupprimeDernier()
{
    if ( _PremierET )
    {
        if ( _Taille == 1 )
            Vide();
        else
        {
            cElementTrajet* __ET;
            for ( __ET = _PremierET; __ET->getSuivant() != _DernierET; __ET = __ET->getSuivant() )
            { }

            _DureeRoulee -= _DernierET->DureeEnMinutesRoulee();
            delete _DernierET;
            _DernierET = __ET;
            _Taille--;
            _DernierET->setSuivant( NULL );
            CalculeDuree();
            _AmplitudeServiceContinu = INCONNU;
        }
    }
}


void cTrajet::SupprimePremier()
{
    if ( _PremierET )
    {
        if ( _Taille == 1 )
            Vide();
        else
        {
            _DureeRoulee -= _PremierET->DureeEnMinutesRoulee();
            cElementTrajet* __ET = _PremierET->getSuivant();
            delete _PremierET;
            _PremierET = __ET;
            _Taille--;
            CalculeDuree();
            _AmplitudeServiceContinu = INCONNU;
        }
    }
}



/*! \brief Suppression de tous les �l�ments du trajet
 \author Hugues Romain
 \date 2005
*/
void cTrajet::Vide()
{
    cElementTrajet * __ET;
    cElementTrajet* __ET2;
    __ET = _PremierET;

    while ( __ET != NULL )
    {
        __ET2 = __ET;
        __ET = __ET->getSuivant();
        delete __ET2;
    }

    _PremierET = NULL;
    _Taille = 0;
    _DernierET = NULL;
    _DureeRoulee = 0;
    _Duree = 0;
    _AmplitudeServiceContinu = INCONNU;
}



/*! \brief Amplitude de la plage horaire couverte par la solution
 \return Dur�e en minute d'amplitude, 0 si service unique
*/
int cTrajet::getAmplitudeServiceContinu() const
{
    return _AmplitudeServiceContinu;
}

void cTrajet::setAmplitudeServiceContinu( const int& __Duree )
{
    _AmplitudeServiceContinu = __Duree;
}

void cTrajet::Finalise()
{
    // Calcul de l'amplitude du service continu
    _AmplitudeServiceContinu = INCONNU;
    for ( const cElementTrajet * __ET = _PremierET; __ET; __ET = __ET->getSuivant() )
    {
        if ( _AmplitudeServiceContinu == INCONNU || __ET->AmplitudeServiceContinu() < _AmplitudeServiceContinu )
            _AmplitudeServiceContinu = __ET->AmplitudeServiceContinu();
        if ( !_AmplitudeServiceContinu )
            break;
    }
}

const NetworkAccessPoint* cTrajet::getOrigin() const
{
    return _PremierET->getOrigin();
}

const NetworkAccessPoint* cTrajet::getDestination() const
{
    return _DernierET->getDestination();
}
