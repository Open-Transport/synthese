
#include "cElementTrajet.h"



/*!	\brief Constructeur
	\author Hugues Romain
	\date 2001-2005
*/
cElementTrajet::cElementTrajet()
{
	vGareDepart					= NULL;
	vGareArrivee				= NULL;
	vSuivant					= NULL;
	vLigne						= NULL;
	vNumeroService				= INCONNU;
	vAmplitudeServiceContinu	= 0;
	vDistanceCarreeObjectif		= 0;
}



cElementTrajet::~cElementTrajet()
{
}



void cElementTrajet::setInformations(const cGareLigne* GLA, const cGareLigne* GLD, const cMoment& newMomentDepart, const cMoment& newMomentArrivee)
{
	if (GLA != NULL)
	{
		vLigne = GLA->Ligne();
		vVoieArrivee = GLA->ArretPhysique();
		vMomentArrivee = newMomentArrivee;
		vGareArrivee = GLA->ArretLogique();
	}
	
	if (GLD != NULL)
	{
		vLigne = GLD->Ligne();
		vVoieDepart = GLD->ArretPhysique();
		vMomentDepart = newMomentDepart;
		vGareDepart = GLD->ArretLogique();
	}
	
	if (GLA != NULL && GLD != NULL)
	{
		if (GLA->PM() > GLD->PM())
			vDistanceParcourue = GLA->PM() - GLD->PM();
		else
			vDistanceParcourue = GLD->PM() - GLA->PM();
			
		vDureeEnMinutesRoulee = vMomentArrivee - vMomentDepart;
//		if (vSuivant)
//			vDureeEnMinutesRoulee += vSuivant->vDureeEnMinutesRoulee;
	}
}

/*! \warning Sur services � horaires uniquement pour l'instant
*/
void cElementTrajet::setInformations(const cGareLigne* GLA, const cGareLigne* GLD, const cDate& newDateDepart, tNumeroService iNumeroService)
{
	tDureeEnJours curJPlus=0;

	vMomentDepart = newDateDepart;
	if (GLD != NULL)
	{
		vLigne = GLD->Ligne();
		vVoieDepart = GLD->ArretPhysique();
		vMomentDepart = GLD->getHoraireDepartPremier(iNumeroService);
		curJPlus = GLD->getHoraireDepartPremier(iNumeroService).JPlus();
		vGareDepart = GLD->ArretLogique();
	}
	if (GLA != NULL)
	{
		vLigne = GLA->Ligne();
		vVoieArrivee = GLA->ArretPhysique();
		vGareArrivee = GLA->ArretLogique();
	}
	if (GLA != NULL && GLD != NULL)
	{
		if (GLA->PM() > GLD->PM())
			vDistanceParcourue = GLA->PM() - GLD->PM();
		else
			vDistanceParcourue = GLD->PM() - GLA->PM();
			
		vMomentArrivee = newDateDepart;
		vMomentArrivee.addDureeEnJours(GLA->getHoraireArriveePremier(iNumeroService).JPlus() - curJPlus);
		vMomentArrivee = GLA->getHoraireArriveePremier(iNumeroService);
		vDureeEnMinutesRoulee = vMomentArrivee - vMomentDepart;
//		if (vSuivant)
//			vDureeEnMinutesRoulee += vSuivant->vDureeEnMinutesRoulee;
	}
}


// Allocation
/*
void* cElementTrajet::operator new(size_t)
{
	void* Retour;

	if (StockET == NULL)
		InitStockET();
	Retour = StockET;
	StockET = StockET->Suivant;
	Alloues++;
	return(Retour);
}


// Desallocation
void cElementTrajet::operator delete(void* Element)
{
	((cElementTrajet*) Element)->Suivant = StockET;
	StockET = (cElementTrajet*) Element;
	Alloues--;
}
*/

//! \warning On suppose que les NombreElement des deux trajets sont corrects
/*
cElementTrajet* cElementTrajet::operator += (cElementTrajet* AutreElementTrajet)
{
	if (AutreElementTrajet != NULL)
	{

		cElementTrajet* pointeur;

		// Ajustement du dernier d�part
		if (AutreElementTrajet->vAmplitudeServiceContinu < vAmplitudeServiceContinu)
			vAmplitudeServiceContinu = AutreElementTrajet->vAmplitudeServiceContinu;

		// Pointeur Suivant
		vDernier->vSuivant = AutreElementTrajet;

		// Pointeur Avantdernier
		if (AutreElementTrajet->vAvantDernier == NULL)
			AutreElementTrajet->vAvantDernier = vDernier;

		// Pointeurs Dernier et Nombre d'�l�ments
		for (pointeur = this; pointeur != AutreElementTrajet; pointeur = pointeur->vSuivant)
		{
			pointeur->vDureeEnMinutesRoulee = vDureeEnMinutesRoulee.Valeur() + AutreElementTrajet->vDureeEnMinutesRoulee.Valeur();
			pointeur->vAvantDernier	 =  AutreElementTrajet->vAvantDernier;
			pointeur->vDernier		 =  AutreElementTrajet->vDernier;
			pointeur->vNombreElements += AutreElementTrajet->vNombreElements;
		}
	}

	return(this);
}
*/



/*! \brief Optimisation 3 - Fonction d'imitation d'un ET � un autre moment
	\warning Ne peut �tre utilis�e sur un service continu (renvoie NULL)
	\author Hugues Romain
	\date 2002
*/
/*cElementTrajet* cElementTrajet::Imite(const cMoment& MomentDepart, const cMoment& ArriveeMax) const
{
	tNumeroVoie tempArretPhysique=0;
	cMoment tempMomentDepart=MomentDepart;
	cElementTrajet* newET;
	cElementTrajet* firstET=NULL;

	if (vAmplitudeServiceContinu)
		return(NULL);

	for (const cElementTrajet* curET=this; curET!=NULL; curET=curET->vSuivant)
	{
		if((newET = curET->vGareDepart->ProchainDirect(curET->vGareArrivee,tempMomentDepart, ArriveeMax, tempArretPhysique)))
		{
			tempArretPhysique = newET->vVoieArrivee;
			tempMomentDepart = newET->vMomentArrivee;
			if (firstET == NULL)
				firstET = newET;
			else
				*firstET += newET;
		}
		else
			return(NULL);
	}
	return(firstET);
}*/

/*
cElementTrajet::setOD(const cArretLogique* Origine, const cArretLogique* Destination)
{
	vGareDepart = Origine;
	vGareArrivee = Destination;
}
*/


const cArretPhysique* cElementTrajet::getArretPhysiqueArrivee() const
{
	return(vGareArrivee->getArretPhysique(vVoieArrivee));
}

const cArretPhysique* cElementTrajet::getArretPhysiqueDepart() const
{
	return(vGareDepart->getArretPhysique(vVoieDepart));
}

const cAxe* cElementTrajet::Axe() const
{
	if (vLigne == NULL)
		return(NULL);
	else
		return(vLigne->Axe());
}

/*!	\brief Optimisation 2 - Fonction de comparaison pour qsort dans ListeProvenances
	\param ET1 Premier trajet � comparer
	\param ET2 Second trajet � comparer
	\return true si ET1 est jug� plus int�ressant que ET2, false sinon
	\author Hugues Romain
	\date 2002-2005
*/
int CompareUtiliteETPourMeilleurDepart(const void* ET1, const void* ET2)
{
	cElementTrajet* oET1 = * (cElementTrajet**) ET1;
	cElementTrajet* oET2 = * (cElementTrajet**) ET2;
	
	//! <li> depart du lieu souhaite</li>
	if (oET1->getDistanceCarreeObjectif() == 0)
		return true;
	if (oET2->getDistanceCarreeObjectif() == 0)
		return false;
	
	//! <li> diff�rence < 2 km</li>
	if (oET1->getDistanceCarreeObjectif() == oET2->getDistanceCarreeObjectif())
		return(false);

	//! <li>diff�rence de niveau</li>
	if (oET1->getGareDepart()->NiveauCorrespondance(oET1->getDistanceCarreeObjectif()) != oET2->getGareDepart()->NiveauCorrespondance(oET2->getDistanceCarreeObjectif()))
		return(oET2->getGareDepart()->NiveauCorrespondance(oET2->getDistanceCarreeObjectif()) - oET1->getGareDepart()->NiveauCorrespondance(oET1->getDistanceCarreeObjectif()));

	//! <li>Comparaison directe des distances carr�es</li>
	return(oET2->getDistanceCarreeObjectif() <= oET1->getDistanceCarreeObjectif());
}
	
/*!	\brief Optimisation 2 - Fonction de comparaison pour qsort dans ListeDestinations
	\param ET1 Premier trajet � comparer
	\param ET2 Second trajet � comparer
	\return true si ET1 est jug� plus int�ressant que ET2, false sinon
	\author Hugues Romain
	\date 2002-2005
*/
int CompareUtiliteETPourMeilleureArrivee(const void* ET1, const void* ET2)
{
	cElementTrajet* oET1 = * (cElementTrajet**) ET1;
	cElementTrajet* oET2 = * (cElementTrajet**) ET2;

	//! <li>Arrivee au lieu souhaite</li>
	if (oET1->getDistanceCarreeObjectif() == 0)
		return true;
	if (oET2->getDistanceCarreeObjectif() == 0)
		return false;
		
	//! <li>diff�rence < 2 km</li>
	if (oET1->getDistanceCarreeObjectif() == oET2->getDistanceCarreeObjectif())
		return false;

	//! <li>diff�rence de niveau</li>
	if (oET1->getGareArrivee()->NiveauCorrespondance(oET1->getDistanceCarreeObjectif()) != oET2->getGareArrivee()->NiveauCorrespondance(oET2->getDistanceCarreeObjectif()))
		return(oET2->getGareArrivee()->NiveauCorrespondance(oET2->getDistanceCarreeObjectif()) - oET1->getGareArrivee()->NiveauCorrespondance(oET1->getDistanceCarreeObjectif()));

	//! <li>Comparaison directe des distances vers l'objectif</li>
	return(oET2->getDistanceCarreeObjectif() <= oET1->getDistanceCarreeObjectif());
}



