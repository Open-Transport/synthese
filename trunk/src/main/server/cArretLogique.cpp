/*!	\file cArretLogique.cpp
	\brief Impl�mentation Classes Point d'arr�t
*/


#include "cArretLogique.h"



/*!	\brief Constructeur
	\author Hugues Romain
	\date 2001-2005

Les objets point d'arr�t sont initialis�s par les valeurs par d�faut suivantes :
 - Aucune desserte de l'arr�t par des lignes
 - Aucun service en gare
 - Aucune photo
 - Aucun lien m�t�o
 - Vitesses max inconnues
 - G�olocalisation inconnue
 - Allocation d'une quantit� de d�signations par d�faut
 
Les valeurs suivantes sont d'ores et d�j� renseign�es:
 - Index du point d'arr�t
 - Type d'autorisation de correspondance
*/
cArretLogique::cArretLogique(tIndex newIndex, tNiveauCorrespondance newNiveauCorrespondance)
{
	// Aucune desserte par des lignes
	vPremiereGareLigneDep = NULL;
	vPremiereGareLigneArr = NULL;
	
	// Aucun service en gare
	vService = NULL;
	
	// Aucune photo
	vPhoto=NULL;
	vNombrePhotos=0;
	
	// Vitesses max inconnues	
	for (size_t i=0; i!=NOMBREVMAX; i++)
		vVitesseMax[i] = INCONNU;
	
	// Localisation inconnue
	SetPoint(INCONNU, INCONNU);

	// Valeurs initialis�es
	_Index = newIndex;
	vCorrespondanceAutorisee = newNiveauCorrespondance;
	if (vCorrespondanceAutorisee)
		vAttenteMinimale = 100;
	else
		vAttenteMinimale = 0;
}



/*!	\brief Destructeur
*/
cArretLogique::~cArretLogique()
{
	for (tNumeroVoie i=1; i<= vArretPhysique.Taille(); i++)
		delete [] vTableauAttente[i];
	delete [] vTableauAttente;
	delete [] vPireAttente;
}

tNiveauCorrespondance cArretLogique::NiveauCorrespondance(const cDistanceCarree& D) const
{
	if (vCorrespondanceAutorisee == CorrRecommandeeCourt)
		if (D.DistanceCarree() > O2COURTLONG)
			return(CorrAutorisee);
		else
			return(CorrRecommandee);

	return(vCorrespondanceAutorisee);
}

void cArretLogique::setNbArretPhysiques(tIndex newNombreArretPhysiques)
{
	cArretPhysique* __ArretPhysique = new cArretPhysique();
	vArretPhysique.AlloueSiBesoin(newNombreArretPhysiques+1);
	vArretPhysique.SetElement(__ArretPhysique,0);
	vArretPhysique.Active(newNombreArretPhysiques);
	vTableauAttente = (cDureeEnMinutes**) malloc((newNombreArretPhysiques+1) * sizeof(cDureeEnMinutes*));
	vPireAttente = new cDureeEnMinutes[newNombreArretPhysiques+1];
	for (tNumeroVoie i=1; i<= newNombreArretPhysiques; i++)
		vTableauAttente[i] = new cDureeEnMinutes[newNombreArretPhysiques+1];
}






/*! \brief Ajout d'un nouveau quai
	\param NumeroArretPhysique Index du quai au sein du point d'arr�t
	\param ArretPhysique Pointeur vers le quai � int�grer
	\return true si l'op�ration a �t� effectu�e avec succ�s
	\author Hugues Romain
	\todo Rajouter une gestion de l'allocation des tableaux de correspondance si augmentation du numero max de quai
	\version 2
	\date 2001-2005
*/
bool cArretLogique::AddArretPhysique(tIndex NumeroArretPhysique, cArretPhysique* ArretPhysique)
{
	// Cha�nage
	if (vArretPhysique.SetElement(ArretPhysique, NumeroArretPhysique) == INCONNU)
		return false;
	
	// Initialisation des indicateurs de d�lai de correspondance
	if (CorrespondanceAutorisee() != CorrInterdite)
		vPireAttente[NumeroArretPhysique] = 0;
		
	// Succ�s
	return true;
}

/*!	\brief Modificateur d'un d�lai minimal de correspondance entre deux quais
	\param NumeroArretPhysiqueDepart ArretPhysique de d�part (indique la ligne dans la matrice de d�lais de correspondance)
	\param NumeroArretPhysiqueArrivee ArretPhysique d'arriv�e (indique la colonne dans la matrice de d�lais de correspondance)
	\param Delai le d�lai minimal de correspondance entre les deux quais, indiqu� en minutes (format num�rique).
	Une dur�e de 99 minutes indique que la correspondance est interdite entre les deux quais sp�cifi�s.
	\return true si l'op�ration a �t� effectu�e avec succ�s
	\author Hugues Romain
	\date 2005
*/
bool cArretLogique::setDelaiCorrespondance(tIndex NumeroArretPhysiqueDepart, tIndex NumeroArretPhysiqueArrivee, tDureeEnMinutes Delai)
{
	// Ecriture de la donn�e
	vTableauAttente[NumeroArretPhysiqueDepart][NumeroArretPhysiqueArrivee] = Delai;
	
	// Ecriture du d�lai minimal le plus important attente au d�part du quai de d�part sp�cifi�
	if (	vTableauAttente[NumeroArretPhysiqueDepart][NumeroArretPhysiqueArrivee].Valeur() != 99 
	&& 	vTableauAttente[NumeroArretPhysiqueDepart][NumeroArretPhysiqueArrivee] > vPireAttente[NumeroArretPhysiqueDepart])
		vPireAttente[NumeroArretPhysiqueDepart] = vTableauAttente[NumeroArretPhysiqueDepart][NumeroArretPhysiqueArrivee];
	
	// Ecriture du d�lai minimal le plus faible du point d'arr�t
	if (vTableauAttente[NumeroArretPhysiqueDepart][NumeroArretPhysiqueArrivee] < vAttenteMinimale)
		vAttenteMinimale = vTableauAttente[NumeroArretPhysiqueDepart][NumeroArretPhysiqueArrivee];
	
	// Succ�s
	return true;
}


cMoment cArretLogique::MomentArriveePrecedente(const cMoment& MomentArrivee, const cMoment& MomentArriveeMin) const
{
	cMoment tempMomentArrivee;
	cMoment tempMomentArriveePrecedente = MomentArriveeMin;
	for (cGareLigne* curGLA = vPremiereGareLigneArr; curGLA != NULL; curGLA = curGLA->PAArriveeSuivante())
	{
		tempMomentArrivee = MomentArrivee;
		tempMomentArrivee -= cDureeEnMinutes(1);
		if (curGLA->Precedent(tempMomentArrivee, MomentArriveeMin) != -1)
		{
			if (tempMomentArrivee > tempMomentArriveePrecedente)
				tempMomentArriveePrecedente = tempMomentArrivee;
		}
	}
	return (tempMomentArriveePrecedente);
}
	
cMoment cArretLogique::MomentDepartSuivant(const cMoment& MomentDepart, const cMoment& MomentDepartMax, const cMoment& __MomentCalcul) const
{
	cMoment tempMomentDepart;
	cMoment tempMomentDepartSuivant = MomentDepartMax;
	for (cGareLigne* curGLD=vPremiereGareLigneDep; curGLD!=NULL; curGLD=curGLD->PADepartSuivant())
	{
		tempMomentDepart = MomentDepart;
		tempMomentDepart += cDureeEnMinutes(1);
		if (curGLD->Prochain(tempMomentDepart, MomentDepartMax, __MomentCalcul) != INCONNU)
		{
			if (tempMomentDepart < tempMomentDepartSuivant)
				tempMomentDepartSuivant = tempMomentDepart;
		}
	}
	return (tempMomentDepartSuivant);
}






/*!	\brief Constructeur
*/
cServiceEnGare::cServiceEnGare(char newType, cPhoto* newPhoto, const cTexte& newDesignation)
{
	vType = newType;
	vPhoto = newPhoto;
	vDesignation << newDesignation;
}




/*! \brief Recherche de la solution sans changement vers un autre PA
	\param ArretPhysiqueArriveePrecedente 0 si aucun.
	\author Hugues Romain
	\date 2001
*/
/*cElementTrajet* cArretLogique::ProchainDirect(cArretLogique* Destination, cMoment& MomentDepart, const cMoment& ArriveeMax, tNumeroVoie ArretPhysiqueArriveePrecedente) const
{
	cMoment tempMomentDepart;
	cMoment tempMomentArrivee;
	tNumeroService tempNumeroService;
	cGareLigne* curGLA;
	cElementTrajet* newET=NULL;
	
	for (cGareLigne* curGLD = vPremiereGareLigneDep; curGLD != NULL; curGLD = curGLD->PADepartSuivant())
		if (curGLD->Circule(MomentDepart, ArriveeMax) && (curGLA = Destination->DessertALArrivee(curGLD->Ligne())) && curGLD->getHoraireDepartPremier(0) < curGLA->getHoraireArriveePremier(0))
		{
			// Calcul de l'horaire ??
			tempMomentDepart = MomentDepart;
			if (vCorrespondanceAutorisee && ArretPhysiqueArriveePrecedente)
				tempMomentDepart += vTableauAttente[ArretPhysiqueArriveePrecedente][curGLD->ArretPhysique()];
			
			tempNumeroService = curGLD->Prochain(tempMomentDepart, ArriveeMax);
			if (tempNumeroService != -1)
			{
				tempMomentArrivee = tempMomentDepart;
				curGLA->CalculeArrivee(*curGLD, tempNumeroService, tempMomentDepart, tempMomentArrivee);

				// Stockage
				if (newET==NULL || tempMomentArrivee < newET->MomentArrivee())
				{
					if (newET == NULL)
						newET = new cElementTrajet;
					newET->setInformations(curGLA, curGLD, tempMomentDepart, tempMomentArrivee);
					newET->setService(tempNumeroService);
				}
			}
		}
	
	return(newET);
}*/



/*!	\brief Fermeture du point d'arr�t : contr�le et remplit les donn�es manquantes
	\return true si le point d'arr�t est utilisable avec les donn�es pr�sentes
	\author Hugues Romain
	\date 2003
*/
bool cArretLogique::Ferme()
{
	// D�clarations
	int 			MoyenneX = 0;
	int 			MoyenneY = 0;
	int 			Nombre = 0;
	const cPoint* 	curPoint;
	cArretPhysique* 			curArretPhysique;
	bool			OK = true;

	// G�n�ration automatique de Designation OD si besoin
	if (!vDesignationOD.Taille())
	{
		cTexte newDesignationOD(getNom(0).Taille() + getCommune(0)->GetNom().Taille() + 1);
		newDesignationOD << getCommune(0)->GetNom() << " " << getNom(0);
		setDesignationOD(newDesignationOD);
	}

	// Coordonn�es g�ographiques
	for (tNumeroVoie iNumeroVoie = 1; iNumeroVoie <= vArretPhysique.Taille(); iNumeroVoie++)
	{
		curArretPhysique = vArretPhysique[iNumeroVoie];
		if (curArretPhysique != NULL)
		{
			curPoint = &(curArretPhysique->getPoint());
			if (!curPoint->EstInconnu())
			{
				MoyenneX += curPoint->XKMM();
				MoyenneY += curPoint->YKMM();
				Nombre++;
			}
		}
		else
			OK = false;
	}
	if (Nombre)
	{
		vPoint.setX((CoordonneeKMM) (MoyenneX / Nombre));
		vPoint.setY((CoordonneeKMM) (MoyenneY / Nombre));
	}

/*	// M�t�o
	if (!vMeteo.GetTaille())
		vMeteo << Commune[0]->MeteoDefaut;

	// Maporama
	if (!vMaporama.GetTaille))
		vMaporama << Commune[0]->MaporamaDefaut;

	// Coordonn�es
	if (Point.EstNul())
		Point = Commune[0]->vGPSDefaut;
*/
	return OK;
}





/*size_t cArretLogique::TailleNomHTML(tNumeroDesignation i) const
{
	if (!ControleNumeroDesignation(i))
		i=0;
	return(vNomHTML[i].GetTaille() + vCommune[i]->GetNomHTML().GetTaille() + 1);
}*/

/* SET Enlever pour resa car non utilis�
cElementTrajet*	cArretLogique::AcheminementDepuisDepot(cArretLogique* Destination, const cMoment& MomentDepart, cReseau* Reseau) const
{
	for (cGareLigne* curGareLigne = PremiereGareLigneDep(); curGareLigne != NULL; curGareLigne = curGareLigne->PADepartSuivant())
		if (!curGareLigne->Ligne()->Axe()->Autorise() && (Reseau == NULL || curGareLigne->Ligne()->getReseau() == Reseau) && curGareLigne->Destination()->ArretLogique() == Destination)
		{
			cElementTrajet* curET = new cElementTrajet;
			cMoment MomentDepartNew;
			MomentDepartNew = MomentDepart;
			curGareLigne->CalculeDepart(*curGareLigne->Destination(), curGareLigne->Destination()->Precedent(MomentDepartNew, MomentDepart), MomentDepart, MomentDepartNew);
			curET->setInformations(curGareLigne->Destination(), curGareLigne, MomentDepartNew, MomentDepart);
			return(curET);
		}
	return(NULL);
}


cElementTrajet*	cArretLogique::AcheminementVersDepot(cArretLogique* Origine, const cMoment& MomentArrivee, cReseau* Reseau) const
{
	for (cGareLigne* curGareLigne = PremiereGareLigneArr(); curGareLigne != NULL; curGareLigne = curGareLigne->PAArriveeSuivante())
		if (!curGareLigne->Ligne()->Axe()->Autorise() && (Reseau == NULL || curGareLigne->Ligne()->getReseau() == Reseau) && curGareLigne->Origine()->ArretLogique() == Origine)
		{
			cElementTrajet* curET = new cElementTrajet;
			cMoment MomentArriveeNew;
			MomentArriveeNew = MomentArrivee;
			curGareLigne->CalculeArrivee(*curGareLigne->Origine(), curGareLigne->Origine()->Prochain(MomentArriveeNew, MomentArrivee), MomentArrivee, MomentArriveeNew);
			curET->setInformations(curGareLigne, curGareLigne->Origine(), MomentArrivee, MomentArriveeNew);
			return(curET);
		}
	return(NULL);
}
*/	

cGareLigne* cArretLogique::DessertAuDepart(const cLigne* Ligne) const
{
	// Recherche peut etre amelior�e en utilisant Reseau puis Axe
	for (cGareLigne* curGLD = vPremiereGareLigneDep; curGLD != NULL; curGLD = curGLD->PADepartSuivant())
		if (curGLD->Ligne() == Ligne)
			return(curGLD);
	return(NULL);
}

cGareLigne* cArretLogique::DessertALArrivee(const cLigne* Ligne) const
{
	// Recherche peut etre amelior�e en utilisant Reseau puis Axe
	for (cGareLigne* curGLA = vPremiereGareLigneArr; curGLA != NULL; curGLA = curGLA->PAArriveeSuivante())
		if (curGLA->Ligne() == Ligne)
			return(curGLA);
	return(NULL);
}

const cTexte& cArretLogique::getNom(tIndex i) const
{
	return(_AccesPADe[i]->getNom());
}

cCommune* cArretLogique::getCommune(tIndex i) const
{
	return(_AccesPADe[i]->getCommune());
}

// cArretLogique::setVMax - Edition de VMax
// ____________________________________________________________________________
//
// ____________________________________________________________________________ 
bool cArretLogique::setVMax(tCategorieDistance CategorieDistance, tVitesseKMH newVitesseKMH)
{
	vVitesseMax[CategorieDistance] = newVitesseKMH+1; 
	return(true);
}
// � Hugues Romain 2001
// ____________________________________________________________________________ 


/*!	\brief Modificateur d�signation courte pour indicateur papier
	\param newDesignationOD D�signation courte de l'arr�t
	\author Hugues Romain
	\date 2001-2005
*/
void cArretLogique::setDesignationOD(const cTexte& newDesignationOD)
{
	vDesignationOD.Vide();
	vDesignationOD << newDesignationOD;
}


bool cArretLogique::addService(char newType, cPhoto* newPhoto, const cTexte& newDesignation)
{
	// SET PORTAGE LINUX
	size_t i;
	for (i=0; vService[i]!=NULL; i++)
	{ }
	//END PORTAGE LINUX
	vService[i] = new cServiceEnGare(newType, newPhoto, newDesignation);
	return(true);
}


bool cArretLogique::SetPoint(int X, int Y)
{
	bool Succes = true;
	
	if (X < 0)
	{
		if (X != INCONNU)
			Succes = false;
	}
	else
		vPoint.setX(X);
	
	if (Y < 0)
	{
		if (Y != INCONNU)
			Succes = false;
	}
	else
		vPoint.setY(Y);
	
	return Succes;
}



/*!	\brief Nombre de quais � la gare
	\return Le nombre de quais dans la gare
*/
tIndex cArretLogique::NombreArretPhysiques() const
{
	return vArretPhysique.Taille() - 1;
}

void cArretLogique::setAlerteMessage(cTexte& message)
{
	vAlerte.setMessage(message);
}

void cArretLogique::setAlerteDebut(cMoment& momentDebut)
{
	vAlerte.setMomentDebut(momentDebut);
}

void cArretLogique::setAlerteFin(cMoment& momentFin)
{
	vAlerte.setMomentFin(momentFin);
}




/** Modificateur d�signation de 13 caract�res.
    @param __Designation13 Valeur � donner � la d�signation de 13 caract�res.En cas de cha�ne de longueur trop importante, la valeur est tronqu�e.
    return true si la copie a �t� effectu�e int�gralement, false si elle a �t� tronqu�e.
*/
bool cArretLogique::setDesignation13(const cTexte& __Designation13)
{
    _Designation13 = __Designation13.Extrait(0, 13);
    return _Designation13.Compare(__Designation13);
}



/** Modificateur d�signation de 26 caract�res.
    @param __Designation26 Valeur � donner � la d�signation de 26 caract�res.En cas de cha�ne de longueur trop importante, la valeur est tronqu�e.
    return true si la copie a �t� effectu�e int�gralement, false si elle a �t� tronqu�e.
*/
bool cArretLogique::setDesignation26(const cTexte& __Designation26)
{
    _Designation26 = __Designation26.Extrait(0, 26);
    return _Designation26.Compare(__Designation26);
}



/** Accesseur d�signation 13 caract�res.
    @return D�signation de 13 caract�res du point d'arr�t.
*/
const cTexte& 
cArretLogique::getDesignation13() const
{
    return _Designation13;
}



/** Accesseur d�signation 26 caract�res.
    @return D�signation de 26 caract�res du point d'arr�t.
*/
const cTexte& 
cArretLogique::getDesignation26() const
{
    return _Designation26;
}

