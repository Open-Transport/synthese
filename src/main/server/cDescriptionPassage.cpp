/*---------------------------------------*
|                                       |
|  APDOS / APISAPI - SYNTHESE v0.5      |
|  ï¿½ Hugues Romain 2000/2001            |
|  cDescriptionPassage.h                |
|  Classes Description passages         |
|                                       |
*---------------------------------------*/

#include "cDescriptionPassage.h"
#include "cArretPhysique.h"
#include "cGareLigne.h"
#include "cLigne.h"

/*
void cDescriptionPassage::Remplit(synthese::time::DateTime& newMoment, cGareLigne* newGareLigne, tNumeroService newNumeroService, synthese::time::Schedule& newHoraire, DureeEnMinutes newAttente)
{
 Moment = newMoment;
 GareLigne = newGareLigne;
 NumArret = newNumeroService;
 Horaire = newHoraire;
 Attente = newAttente;
}
*/

cDescriptionPassage::cDescriptionPassage()
{
    // Respect des rï¿½gles de chainage
    vPrecedent = this;
    vSuivant = 0;
    vSitPert = 0;
}

cDescriptionPassage* cDescriptionPassage::Insere( cDescriptionPassage* newDP )
{
    // Recherche de la position
    cDescriptionPassage * curDP = this;
    for ( ; curDP != 0 && curDP->vMomentPrevu < newDP->vMomentPrevu; curDP = curDP->vSuivant )
    { }

    newDP->vSuivant = curDP;

    if ( curDP == 0 )
    {
        newDP->vPrecedent = vPrecedent;
        newDP->vPrecedent->vSuivant = newDP;
        vPrecedent = newDP;
    }
    else
    {
        newDP->vPrecedent = curDP->vPrecedent;
        curDP->vPrecedent = newDP;
        if ( curDP != this )
            newDP->vPrecedent->vSuivant = newDP;
        else
            return newDP;
    }
    return this;
}


/** Remplissage d'un objet de départ.
 @param GareLigne GareLigne devant être copiée
 @param tempMomentDepart Moment de départ prévu
 @param iNumeroService Rang de la circulation sur la ligne
 @todo URGENT Faire un controle sur l'horaire réel qui doit toujours être après l'horaire théorique (voir si on ne devrai pas changer HoraireReel par un delta)
*/
void cDescriptionPassage::Remplit( cGareLigne* GareLigne, const synthese::time::DateTime& tempMomentDepart, const size_t& iNumeroService )
{
    vMomentPrevu = tempMomentDepart;
    vMomentReel = tempMomentDepart;
    vMomentReel = GareLigne->getHoraireDepartPremierReel( iNumeroService );

    // vSitPert = GareLigne->GetArretDepart(iNumeroService).SituationPerturbee();
    _Gare.SetElement( GareLigne->ArretPhysique() ->getLogicalPlace(), 0 );
    _GareLigne = GareLigne;
    vNumArret = iNumeroService;
}

// Calcul du moment d'arrivï¿½e au terminus
synthese::time::DateTime cDescriptionPassage::MomentArriveeDestination() const
{
    synthese::time::DateTime __MomentArrivee;
    _GareLigne->Ligne() ->getLineStops().back() ->CalculeArrivee( *_GareLigne, vNumArret, vMomentReel, __MomentArrivee );
    return __MomentArrivee;
}

const LogicalPlace* cDescriptionPassage::Destination()
{
    return _Gare.getDernier();
}

cGareLigne* cDescriptionPassage::getGareLigne() const
{
    return _GareLigne;
}

int cDescriptionPassage::NombreGares() const
{
    return _Gare.Taille();
}


cDescriptionPassage* cDescriptionPassage::GetDernierEtLibere()
{
    cDescriptionPassage * newDP = vPrecedent;
    vPrecedent = newDP->vPrecedent;
    vPrecedent->vSuivant = 0;
    _Gare.Vide();
    return ( newDP );
}

const synthese::time::DateTime& cDescriptionPassage::MomentFin() const
{
    const cDescriptionPassage * curDP = this;
    for ( ; curDP->vSuivant != 0; curDP = curDP->vPrecedent )
        ;
    return ( curDP->vMomentPrevu );
}

const synthese::time::DateTime& cDescriptionPassage::getMomentPrevu() const
{
    return ( vMomentPrevu );
}

cDescriptionPassage* cDescriptionPassage::Suivant() const
{
    return ( vSuivant );
}

LogicalPlace* cDescriptionPassage::GetGare( int __i ) const
{
    return _Gare.IndexValide( __i ) ? _Gare[ __i ] : 0;
}

const synthese::time::DateTime& cDescriptionPassage::getMomentReel() const
{
    return ( vMomentReel );
}

cSitPert* cDescriptionPassage::getSitPert() const
{
    return ( vSitPert );
}

const size_t& cDescriptionPassage::NumArret() const
{
    return ( vNumArret );
}

