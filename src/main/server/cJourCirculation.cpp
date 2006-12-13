/*! \file cJourCirculation.cpp
\brief Implémentation classes calendriers de circulations
\date 2000-2002
\author Hugues Romain
*/

#include "cJourCirculation.h"
#include "04_time/Date.h"

using namespace std;

/** Constructor.
*/
cJC::cJC( const int& PremiereAnnee, const int& DerniereAnnee, const size_t& id, const string& newIntitule )
        : _PremiereAnnee( PremiereAnnee )
        , _DerniereAnnee( DerniereAnnee )
        , _id( id )
{
    // Masque
    vJoursAnnee.resize( ( _DerniereAnnee.getValue () - _PremiereAnnee.getValue () + 1 ) * synthese::time::MONTHS_PER_YEAR );

    // Intitule
    setIntitule( newIntitule );

    // Catégorie
    setCategorie();
}



/** Modification du statut d'une date dans le calendrier
 @param Date Date à modifier
 @param Sens Statut de la date : circule ou ne circule pas
 @return true si la date fournie existe dans le calendrier, false sinon
*/
bool cJC::SetCircule( const synthese::time::Date& Date, InclusionType Sens )
{
    if ( Date.isValid () )
    {
        if ( Date.isYearUnknown() )
        {
            synthese::time::Date CurDate;
            for ( synthese::time::Year Annee = _PremiereAnnee; Annee.getValue () <= _DerniereAnnee.getValue (); Annee++ )
            {
                CurDate.updateDate( Date.getDay(), Date.getMonth (), Annee.getValue () );
                setCircule( CurDate, Sens );
            }
        }
        else
            if ( Date.getYear () >= _PremiereAnnee.getValue () && Date.getYear () <= _DerniereAnnee.getValue () )
                setCircule( Date, Sens );
        return true;
    }
    else
        return false;
}

void cJC::setCircule( const synthese::time::Date& Date, InclusionType Sens )
{
    Mask Masque = 1;
    Masque <<= ( Date.getDay() - 1 );

    if ( Sens == InclusionType_POSITIVE )
    {
        vJoursAnnee[ getIndexMois( Date ) ] |= Masque;

    }
    else
    {
        Masque = ~Masque;
        vJoursAnnee[ getIndexMois( Date ) ] &= Masque;
    }
}

void cJC::SetInclusionToMasque( Calendar& Masque, InclusionType Sens ) const
{
    if ( Sens == InclusionType_POSITIVE )
        for ( int iAnnee = _PremiereAnnee.getValue (); iAnnee <= _DerniereAnnee.getValue (); iAnnee++ )
            for ( int iMois = 1; iMois <= synthese::time::MONTHS_PER_YEAR; iMois++ )
                Masque[ getIndexMois( iAnnee, iMois ) ] |= vJoursAnnee[ getIndexMois( iAnnee, iMois ) ];
    else
    {
        Mask tempMasque;
        for ( int iAnnee = _PremiereAnnee.getValue (); iAnnee <= _DerniereAnnee.getValue (); iAnnee++ )
            for ( int iMois = 1; iMois <= synthese::time::MONTHS_PER_YEAR; iMois++ )
            {
                tempMasque = vJoursAnnee[ getIndexMois( iAnnee, iMois ) ];
                tempMasque = ~tempMasque;
                Masque[ getIndexMois( iAnnee, iMois ) ] &= tempMasque;
            }
    }
}



/*! \brief Test de circulation à une date donnée
 \param DateTest Jour de référence
 \return si le régime de circulation indique une circulation le jour du départ du service de son origine
*/
bool cJC::Circule( const synthese::time::Date& DateTest ) const
{
    if ( DateTest.isUnknown () )
        return false;

    Mask Masque = 1;
    Masque <<= ( DateTest.getDay() - 1 );
    return ( Masque & vJoursAnnee[ getIndexMois( DateTest ) ] ) != 0;
}

bool cJC::UnPointCommun( const Calendar& AutreMasque ) const
{
    for ( int iAnnee = _PremiereAnnee.getValue (); iAnnee <= _DerniereAnnee.getValue (); iAnnee++ )
        for ( int iMois = 1; iMois <= synthese::time::MONTHS_PER_YEAR; iMois++ )
            if ( AutreMasque[ getIndexMois( iAnnee, iMois ) ] & vJoursAnnee[ getIndexMois( iAnnee, iMois ) ] )
                return true;
    return false;
}

bool cJC::TousPointsCommuns( const cJC& JCBase, const Calendar& Masque2 ) const
{
    for ( int iAnnee = _PremiereAnnee.getValue (); iAnnee <= _DerniereAnnee.getValue (); iAnnee++ )
        for ( int iMois = 1; iMois <= synthese::time::MONTHS_PER_YEAR; iMois++ )
            if ( ( JCBase.vJoursAnnee[ getIndexMois( iAnnee, iMois ) ] & vJoursAnnee[ getIndexMois( iAnnee, iMois ) ] ) != ( Masque2[ getIndexMois( iAnnee, iMois ) ] & vJoursAnnee[ getIndexMois( iAnnee, iMois ) ] ) )
                return false;
    return true;
}

/*
cJC* cJC::MeilleurJC(cJC* AutreJC)
{
 tMasque* newMasque = Et(AutreJC);
 for (tNumeroJC iNumeroJC=0; iNumeroJC<Env->NombreJC; iNumeroJC++)
  if (Env->JC[iNumeroJC]->TousPointsCommuns(newMasque))
   return(Env->JC[iNumeroJC]);
 return(AutreJC);
}
*/

cJC::Calendar cJC::Et( const cJC& AutreJC ) const
{
    Calendar newMasque;
    for ( int iAnnee = _PremiereAnnee.getValue (); iAnnee <= _DerniereAnnee.getValue (); iAnnee++ )
        for ( int iMois = 1; iMois <= synthese::time::MONTHS_PER_YEAR; iMois++ )
            newMasque[ getIndexMois( iAnnee, iMois ) ] = AutreJC.vJoursAnnee[ getIndexMois( iAnnee, iMois ) ] & vJoursAnnee[ getIndexMois( iAnnee, iMois ) ];
    return ( newMasque );
}

cJC::Calendar cJC::ElementsNonInclus( const cJC& AutreJC ) const
{
    // L'opérateur ! ne semble pas convenir: est ce du bit a bit ?
    Calendar newMasque;
    for ( int iAnnee = _PremiereAnnee.getValue (); iAnnee <= _DerniereAnnee.getValue (); iAnnee++ )
        for ( int iMois = 1; iMois <= synthese::time::MONTHS_PER_YEAR; iMois++ )
            newMasque[ getIndexMois( iAnnee, iMois ) ] = !vJoursAnnee[ getIndexMois( iAnnee, iMois ) ] & AutreJC.vJoursAnnee[ getIndexMois( iAnnee, iMois ) ];
    return ( newMasque );
}

size_t cJC::Card( const Calendar& Masque ) const
{
    size_t t = 0;
    Mask tempMasque;
    Mask tempMasque2;
    for ( int iAnnee = _PremiereAnnee.getValue (); iAnnee <= _DerniereAnnee.getValue (); iAnnee++ )
        for ( int iMois = 1; iMois <= synthese::time::MONTHS_PER_YEAR; iMois++ )
        {
            tempMasque = 1;
            tempMasque2 = vJoursAnnee[ getIndexMois( iAnnee, iMois ) ];
            for ( int iJour = 1; iJour <= 31; iJour++ )
            {
                if ( tempMasque2 & tempMasque & Masque[ getIndexMois( iAnnee, iMois ) ] )
                    t++;
                tempMasque <<= 1;
            }
        }
    return ( t );
}

void cJC::RAZMasque( bool ValeurBase )
{
    for ( int iAnnee = _PremiereAnnee.getValue (); iAnnee <= _DerniereAnnee.getValue (); iAnnee++ )
        for ( int iMois = 1; iMois <= synthese::time::MONTHS_PER_YEAR; iMois++ )
            if ( ValeurBase )
            {
                //SET PORTAGE LINUX
                //vJoursAnnee[iAnnee*MoisParAn+iMois]=4294967295;
                vJoursAnnee[ getIndexMois( iAnnee, iMois ) ] = 4294967295UL;
                //END PORTAGE
            }
            else
                vJoursAnnee[ getIndexMois( iAnnee, iMois ) ] = 0;
}



/** Destructeur.
*/
cJC::~cJC()
{}

bool cJC::SetCircule( const synthese::time::Date &DateDebut, const synthese::time::Date &DateFin, InclusionType Sens, int Pas )
{
    if ( DateDebut.isValid () && DateFin.isValid () && Pas > 0 && DateDebut.isYearUnknown () == DateFin.isYearUnknown () )
    {
        if ( DateDebut.isYearUnknown () )
        {
            synthese::time::Date CurDate;
            synthese::time::Date CurDateFin = DateFin;
            for ( synthese::time::Year Annee = _PremiereAnnee; Annee <= _DerniereAnnee; Annee++ )
            {
                CurDate.updateDate( DateDebut.getDay(), DateDebut.getMonth (), Annee.getValue () );
                if ( DateDebut <= DateFin )
                    CurDateFin.updateDate( CurDateFin.getDay(), CurDateFin.getMonth (), Annee.getValue () );
                else if ( Annee == _DerniereAnnee )
                    CurDateFin.updateDate( synthese::time::TIME_MAX, synthese::time::TIME_MAX, _DerniereAnnee.getValue () );
                else
                    CurDateFin.updateDate( CurDateFin.getDay(), CurDateFin.getMonth (), Annee.getValue () + 1 );

                for ( ; CurDate <= CurDateFin; CurDate += Pas )
                    SetCircule( CurDate, Sens );
            }
        }
        else
            for ( synthese::time::Date CurDate = DateDebut; CurDate <= DateFin; CurDate += Pas )
                SetCircule( CurDate, Sens );
        return ( true );
    }
    else
        return ( false );
}


void cJC::setMasque( const Calendar& AutreMasque )
{
    for ( size_t i = 0; i < ( size_t ) ( ( _DerniereAnnee.getValue () - _PremiereAnnee.getValue () + 1 ) * synthese::time::MONTHS_PER_YEAR ); i++ )
        vJoursAnnee[ i ] = AutreMasque[ i ];
}

synthese::time::Date cJC::PremierJourFonctionnement() const
{
    synthese::time::Date curDate;
    Mask tempMasque;
    Mask tempMasque2;

    for ( int iAnnee = _PremiereAnnee.getValue (); iAnnee <= _DerniereAnnee.getValue (); iAnnee++ )
        for ( int iMois = 1; iMois <= synthese::time::MONTHS_PER_YEAR; iMois++ )
        {
            tempMasque = 1;
            tempMasque2 = vJoursAnnee[ getIndexMois( iAnnee, iMois ) ];
            for ( int iJour = 1; iJour <= 31; iJour++ )
            {
                if ( tempMasque2 & tempMasque )
                {
                    curDate.updateDate( iJour, iMois, iAnnee );
                    return ( curDate );
                }
                tempMasque <<= 1;
            }
        }
    return ( curDate );
}



/** Calcul de l'index du tableau de bits à lire pour accéder à un jour du mois.
 @param __Date Jour quelconque dans le mois à lire
 @return L'index du tableau de bits à lire pour accéder à un jour du mois
*/
size_t cJC::getIndexMois( const synthese::time::Date& __Date ) const
{
    return getIndexMois( __Date.getYear (), __Date.getMonth () );
}



/** Calcul de l'index du tableau de bits à lire pour accéder à un mois.
 @param __Annee Année à lire
 @param __Mois Mois à lire
 @return L'index du tableau de bits à lire pour accéder au mois
*/
size_t cJC::getIndexMois( int __Annee, int __Mois ) const
{
    return ( __Annee - _PremiereAnnee.getValue () ) * synthese::time::MONTHS_PER_YEAR + __Mois;
}


bool cJC::UnPointCommun( const cJC& AutreMasque ) const
{
    return ( UnPointCommun( AutreMasque.vJoursAnnee ) );
}

size_t cJC::Card( const cJC& AutreJC ) const
{
    return ( Card( AutreJC.vJoursAnnee ) );
}

const cJC::Category& cJC::Categorie() const
{
    return ( vCategorie );
}

void cJC::setCategorie( Category newCategorie )
{
    vCategorie = newCategorie;
}

void cJC::setIntitule( const string& Texte )
{
    vIntitule = Texte;
}



/*! \brief Accesseur index de l'objet dans l'environnement
 \return L'index de l'objet dans l'environnement
 \author Hugues Romain
 \date 2001-2005
*/
const size_t& cJC::getId() const
{
    return _id;
}

const cJC::Calendar& cJC::JoursAnnee() const
{
    return ( vJoursAnnee );
}

void cJC::SetInclusionToMasque( cJC &JourCirculation, InclusionType Sens ) const
{
    SetInclusionToMasque( JourCirculation.vJoursAnnee, Sens );
}


