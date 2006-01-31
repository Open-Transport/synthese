/*---------------------------------------*
 |                                       |
 |  APDOS / APISAPI - SYNTHESE v0.5      |
 |  � Hugues Romain 2000/2001            |
 |  cDescriptionPassage.h                |
 |  Classes Description passages         |
 |                                       |
 *---------------------------------------*/

#include "cDescriptionPassage.h"

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
	// Respect des r�gles de chainage
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


/** Remplissage d'un objet de d�part.
	@param GareLigne GareLigne devant �tre copi�e
	@param tempMomentDepart Moment de d�part pr�vu
	@param iNumeroService Rang de la circulation sur la ligne
	@todo URGENT Faire un controle sur l'horaire r�el qui doit toujours �tre apr�s l'horaire th�orique (voir si on ne devrai pas changer HoraireReel par un delta)
*/
void cDescriptionPassage::Remplit(cGareLigne* GareLigne, const cMoment& tempMomentDepart, const tNumeroService& iNumeroService)
{
	vMomentPrevu = tempMomentDepart;
	vMomentReel = tempMomentDepart;
	vMomentReel = GareLigne->getHoraireDepartPremierReel(iNumeroService);
	
//	vSitPert = GareLigne->GetArretDepart(iNumeroService).SituationPerturbee();
	_Gare.SetElement(GareLigne->ArretLogique(), 0);
	_GareLigne = GareLigne;
	vNumArret = iNumeroService;
}

// Calcul du moment d'arriv�e au terminus
cMoment cDescriptionPassage::MomentArriveeDestination() const
{
	cMoment __MomentArrivee;
	_GareLigne->Destination()->CalculeArrivee(*_GareLigne, vNumArret, vMomentReel, __MomentArrivee);
	return __MomentArrivee;
}

const cArretLogique* cDescriptionPassage::Destination()
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