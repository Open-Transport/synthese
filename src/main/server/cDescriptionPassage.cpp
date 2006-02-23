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
void cDescriptionPassage::Remplit(cMoment& newMoment, cGareLigne* newGareLigne, tNumeroService newNumeroService, cHoraire& newHoraire, DureeEnMinutes newAttente)
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
	vSuivant = NULL;
	vSitPert = NULL;
}

cDescriptionPassage* cDescriptionPassage::Insere(cDescriptionPassage* newDP)
{
	// Recherche de la position
	cDescriptionPassage* curDP=this;
	for (; curDP != NULL && curDP->vMomentPrevu < newDP->vMomentPrevu; curDP = curDP->vSuivant)
	{ }

	newDP->vSuivant = curDP;
	
	if (curDP == NULL)
	{
		newDP->vPrecedent = vPrecedent;
		newDP->vPrecedent->vSuivant = newDP;
		vPrecedent = newDP;
	}
	else
	{
		newDP->vPrecedent = curDP->vPrecedent;
		curDP->vPrecedent = newDP;
		if (curDP != this)
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
void cDescriptionPassage::Remplit(cGareLigne* GareLigne, const cMoment& tempMomentDepart, const size_t& iNumeroService)
{
	vMomentPrevu = tempMomentDepart;
	vMomentReel = tempMomentDepart;
	vMomentReel = GareLigne->getHoraireDepartPremierReel(iNumeroService);
	
//	vSitPert = GareLigne->GetArretDepart(iNumeroService).SituationPerturbee();
	_Gare.SetElement(GareLigne->ArretPhysique()->getLogicalPlace(), 0);
	_GareLigne = GareLigne;
	vNumArret = iNumeroService;
}

// Calcul du moment d'arrivï¿½e au terminus
cMoment cDescriptionPassage::MomentArriveeDestination() const
{
	cMoment __MomentArrivee;
	_GareLigne->Ligne()->getLineStops().back()->CalculeArrivee(*_GareLigne, vNumArret, vMomentReel, __MomentArrivee);
	return __MomentArrivee;
}

const LogicalPlace* cDescriptionPassage::Destination()
{
	return _Gare.getDernier();
}

cGareLigne*	cDescriptionPassage::getGareLigne() const
{
	return _GareLigne;
}

tIndex cDescriptionPassage::NombreGares() const
{
	return _Gare.Taille();
}


cDescriptionPassage* cDescriptionPassage::GetDernierEtLibere()
{
	cDescriptionPassage* newDP = vPrecedent;
	vPrecedent = newDP->vPrecedent;
	vPrecedent->vSuivant = NULL;
	_Gare.Vide();
	return(newDP);
}

const cMoment& cDescriptionPassage::MomentFin() const
{	
	const cDescriptionPassage* curDP=this;
	for (; curDP->vSuivant != NULL; curDP = curDP->vPrecedent);
	return(curDP->vMomentPrevu);
}

const cMoment& cDescriptionPassage::getMomentPrevu() const
{
	return(vMomentPrevu);
}

cDescriptionPassage* cDescriptionPassage::Suivant() const
{
	return(vSuivant);
}

LogicalPlace* cDescriptionPassage::GetGare(tIndex __i) const
{
	return _Gare.IndexValide(__i) ? _Gare[__i] : NULL;
}

const cMoment& cDescriptionPassage::getMomentReel() const
{
	return(vMomentReel);
}

cSitPert* cDescriptionPassage::getSitPert() const
{
	return(vSitPert);
}

const size_t& cDescriptionPassage::NumArret() const
{
	return(vNumArret);
}
