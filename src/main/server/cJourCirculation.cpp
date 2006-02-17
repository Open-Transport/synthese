/*!	\file cJourCirculation.cpp
	\brief Implémentation classes calendriers de circulations
	\date 2000-2002
	\author Hugues Romain
*/

#include "cJourCirculation.h"

cJC::cJC()
{
	vJoursAnnee = NULL;
}

cJC::cJC(tAnnee PremiereAnnee, tAnnee DerniereAnnee, const cTexte& newIntitule)
{
	// Masque
	_PremiereAnnee = PremiereAnnee;
	_DerniereAnnee = DerniereAnnee;
	vJoursAnnee = AlloueMasque();
	
	// Intitule
	setIntitule(newIntitule);

	// Catégorie
	setCategorie();
}



/** Modification du statut d'une date dans le calendrier
	@param Date Date à modifier
	@param Sens Statut de la date : circule ou ne circule pas
	@return true si la date fournie existe dans le calendrier, false sinon
*/
bool cJC::SetCircule(const cDate& Date, tSens Sens)
{
	if (Date.OK())
	{
		if (Date.AnneeEstInconnue())
		{
			cDate CurDate;
			for (cAnnee Annee = _PremiereAnnee; Annee.Valeur() <= _DerniereAnnee.Valeur(); Annee++)
			{
				CurDate.setDate(Date.Jour(), Date.Mois(), Annee.Valeur());
				setCircule(CurDate, Sens);
			}
		}
		else
			if (Date.Annee() >= _PremiereAnnee.Valeur() && Date.Annee() <= _DerniereAnnee.Valeur())
				setCircule(Date, Sens);
		return true;
	}
	else
		return false;
}

void cJC::setCircule(const cDate& Date, tSens Sens)
{
	tMasque Masque = 1;
	Masque <<= (Date.Jour() - 1);

	if (Sens == Positif)
	{
		vJoursAnnee[getIndexMois(Date)] |= Masque;
		
	}
	else
	{
		Masque = ~Masque;
		vJoursAnnee[getIndexMois(Date)] &= Masque;
	}
}

void cJC::SetInclusionToMasque(tMasque* Masque, tSens Sens) const
{
	if (Sens == Positif)
		for (tAnnee iAnnee = _PremiereAnnee.Valeur(); iAnnee <= _DerniereAnnee.Valeur(); iAnnee++)
			for (tMois iMois=1; iMois<= MOIS_PAR_AN; iMois++)
				Masque[getIndexMois(iAnnee, iMois)] |= vJoursAnnee[getIndexMois(iAnnee, iMois)];
	else
	{
		tMasque tempMasque;
		for (tAnnee iAnnee = _PremiereAnnee.Valeur(); iAnnee <= _DerniereAnnee.Valeur(); iAnnee++)
			for (tMois iMois=1; iMois<= MOIS_PAR_AN; iMois++)
			{
				tempMasque = vJoursAnnee[getIndexMois(iAnnee, iMois)];
				tempMasque = ~tempMasque;
				Masque[getIndexMois(iAnnee, iMois)] &= tempMasque;
			}
	}
}



/*!	\brief Test de circulation à une date donnée
	\param DateTest Jour de référence
	\return si le régime de circulation indique une circulation le jour du départ du service de son origine
*/
bool cJC::Circule(const cDate& DateTest) const
{
	if (DateTest.EstInconnue())
		return false;

	tMasque Masque = 1;
	Masque <<= (DateTest.Jour() - 1);
	return (Masque & vJoursAnnee[getIndexMois(DateTest)]) != 0;
}

bool cJC::UnPointCommun(const tMasque* AutreMasque) const
{
	for (tAnnee iAnnee = _PremiereAnnee.Valeur(); iAnnee<= _DerniereAnnee.Valeur(); iAnnee++)
		for (tMois iMois=1; iMois<=MOIS_PAR_AN; iMois++)
			if (AutreMasque[getIndexMois(iAnnee, iMois)] & vJoursAnnee[getIndexMois(iAnnee, iMois)])
				return true;
	return false;
}

bool cJC::TousPointsCommuns(const cJC& JCBase, const tMasque* Masque2) const
{
	for (tAnnee iAnnee = _PremiereAnnee.Valeur(); iAnnee <= _DerniereAnnee.Valeur(); iAnnee++)
		for (tMois iMois=1; iMois<=MOIS_PAR_AN; iMois++)
			if ((JCBase.vJoursAnnee[getIndexMois(iAnnee, iMois)] & vJoursAnnee[getIndexMois(iAnnee, iMois)]) != (Masque2[getIndexMois(iAnnee, iMois)] & vJoursAnnee[getIndexMois(iAnnee, iMois)]))
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

tMasque* cJC::Et(const cJC& AutreJC) const
{
	tMasque* newMasque = AlloueMasque();
	for (tAnnee iAnnee = _PremiereAnnee.Valeur(); iAnnee <= _DerniereAnnee.Valeur(); iAnnee++)
		for (tMois iMois=1; iMois<= MOIS_PAR_AN; iMois++)
			newMasque[getIndexMois(iAnnee, iMois)] = AutreJC.vJoursAnnee[getIndexMois(iAnnee, iMois)] & vJoursAnnee[getIndexMois(iAnnee, iMois)];
	return(newMasque);
}

tMasque* cJC::ElementsNonInclus(const cJC& AutreJC) const
{
	// L'opérateur ! ne semble pas convenir: est ce du bit a bit ?
	tMasque* newMasque = AlloueMasque();
	for (tAnnee iAnnee = _PremiereAnnee.Valeur(); iAnnee <= _DerniereAnnee.Valeur(); iAnnee++)
		for (tMois iMois=1; iMois<= MOIS_PAR_AN; iMois++)
			newMasque[getIndexMois(iAnnee, iMois)] = !vJoursAnnee[getIndexMois(iAnnee, iMois)] & AutreJC.vJoursAnnee[getIndexMois(iAnnee, iMois)];
	return(newMasque);
}

size_t cJC::Card(const tMasque* Masque) const
{
	size_t t=0;
	tMasque tempMasque;
	tMasque tempMasque2;
	for (tAnnee iAnnee=_PremiereAnnee.Valeur(); iAnnee <= _DerniereAnnee.Valeur(); iAnnee++)
		for (tMois iMois=1; iMois<= MOIS_PAR_AN; iMois++)
		{
			tempMasque = 1;
			tempMasque2 = vJoursAnnee[getIndexMois(iAnnee, iMois)];
			for (tJour iJour=1; iJour <=31; iJour++)
			{
				if (tempMasque2 & tempMasque & Masque[getIndexMois(iAnnee, iMois)])
					t++;
				tempMasque <<= 1;
			}
		}
	return(t);
}

void cJC::RAZMasque(bool ValeurBase)
{
	for (tAnnee iAnnee = _PremiereAnnee.Valeur(); iAnnee<= _DerniereAnnee.Valeur(); iAnnee++)
		for (tMois iMois=1; iMois<= MOIS_PAR_AN; iMois++)
			if (ValeurBase)
			{
				//SET PORTAGE LINUX
				//vJoursAnnee[iAnnee*MoisParAn+iMois]=4294967295;
				vJoursAnnee[getIndexMois(iAnnee, iMois)]= 4294967295UL;
				//END PORTAGE
			}
			else
				vJoursAnnee[getIndexMois(iAnnee, iMois)] = 0;		
}



/** Destructeur.
*/
cJC::~cJC()
{
	free(vJoursAnnee);
}

bool cJC::SetCircule(const cDate &DateDebut, const cDate &DateFin, tSens Sens, tDureeEnJours Pas)
{
	if (DateDebut.OK() && DateFin.OK() && Pas > 0 && DateDebut.AnneeEstInconnue() == DateFin.AnneeEstInconnue())
	{
		if (DateDebut.AnneeEstInconnue())
		{
			cDate CurDate;
			cDate CurDateFin = DateFin;
			for (cAnnee Annee = _PremiereAnnee; Annee <= _DerniereAnnee; Annee++)
			{
				CurDate.setDate(DateDebut.Jour(), DateDebut.Mois(), Annee.Valeur());
				if (DateDebut <= DateFin)
					CurDateFin.setDate(CurDateFin.Jour(), CurDateFin.Mois(), Annee.Valeur());
				else if (Annee == _DerniereAnnee)
					CurDateFin.setDate(TEMPS_MAX, TEMPS_MAX, _DerniereAnnee.Valeur());
				else
					CurDateFin.setDate(CurDateFin.Jour(), CurDateFin.Mois(), Annee.Valeur()+1);
					
				for (; CurDate <= CurDateFin; CurDate += Pas)
					SetCircule(CurDate, Sens);
			}
		}
		else
			for (cDate CurDate = DateDebut; CurDate <= DateFin; CurDate += Pas)
				SetCircule(CurDate, Sens);
		return(true);
	}
	else
		return(false);
}

tMasque* cJC::AlloueMasque() const
{
	if (_PremiereAnnee.Valeur() == INCONNU)
		return(NULL);
	else
		return((tMasque*) calloc((_DerniereAnnee.Valeur() - _PremiereAnnee.Valeur() + 1) * (MOIS_PAR_AN + 1), sizeof(tMasque)));
}

void cJC::setMasque(const tMasque* AutreMasque)
{
	for (size_t i=0; i< (size_t)((_DerniereAnnee.Valeur() - _PremiereAnnee.Valeur() + 1) * MOIS_PAR_AN); i++)
		vJoursAnnee[i] = AutreMasque[i];
}

cDate cJC::PremierJourFonctionnement() const
{
	cDate curDate;
	tMasque tempMasque;
	tMasque tempMasque2;
	
	for (tAnnee iAnnee= _PremiereAnnee.Valeur(); iAnnee<= _DerniereAnnee.Valeur(); iAnnee++)
		for (tMois iMois=1; iMois<= MOIS_PAR_AN; iMois++)
		{
			tempMasque = 1;
			tempMasque2 = vJoursAnnee[getIndexMois(iAnnee, iMois)];
			for (tJour iJour=1; iJour <=31; iJour++)
			{
				if (tempMasque2 & tempMasque)
				{
					curDate.setDate(iJour, iMois, iAnnee);
					return(curDate);
				}
				tempMasque <<= 1;
			}
		}
	return(curDate);
}



/** Calcul de l'index du tableau de bits à lire pour accéder à un jour du mois.
	@param __Date Jour quelconque dans le mois à lire
	@return L'index du tableau de bits à lire pour accéder à un jour du mois
*/
tIndex cJC::getIndexMois(const cDate& __Date)	const
{
	return getIndexMois(__Date.Annee(), __Date.Mois());
}



/** Calcul de l'index du tableau de bits à lire pour accéder à un mois.
	@param __Annee Année à lire
	@param __Mois Mois à lire
	@return L'index du tableau de bits à lire pour accéder au mois
*/
tIndex cJC::getIndexMois(tAnnee __Annee, tMois __Mois)	const
{
	return (__Annee - _PremiereAnnee.Valeur()) * MOIS_PAR_AN + __Mois;
}


void cJC::ReAlloueMasque()
{
	if (vJoursAnnee)
		free(vJoursAnnee);
	vJoursAnnee = AlloueMasque();
}

bool cJC::UnPointCommun(const cJC& AutreMasque) const
{
	return(UnPointCommun(AutreMasque.vJoursAnnee));
}

size_t cJC::Card(const cJC& AutreJC) const
{
	return(Card(AutreJC.vJoursAnnee));
}

tCategorieJC cJC::Categorie() const
{
	return(vCategorie);
}

void cJC::setCategorie(tCategorieJC newCategorie)
{
	vCategorie = newCategorie;
}

void cJC::setIntitule(const cTexte& Texte)
{
	vIntitule.Vide();
	vIntitule << Texte;
}



/*!	\brief Accesseur index de l'objet dans l'environnement
	\return L'index de l'objet dans l'environnement
	\author Hugues Romain
	\date 2001-2005
*/
tIndex cJC::Index() const
{
	return(vCode);
}

const tMasque* cJC::JoursAnnee() const
{
	return(vJoursAnnee);
}

void cJC::SetInclusionToMasque(cJC &JourCirculation, tSens Sens) const
{
	SetInclusionToMasque(JourCirculation.vJoursAnnee, Sens);
}



/*!	\brief Modificateur sans contrôle de valeur de l'index de l'objet dans l'environnement
	\param newVal Index de l'objet dans l'environnement
	\return true si la modification a été effectuée avec succès
	\author Hugues Romain
	\date 2001-2005
*/
bool cJC::setIndex(tIndex newVal)
{
	vCode = newVal;
	return true;
}

void cJC::setAnnees(tAnnee PremiereAnnee, tAnnee DerniereAnnee)
{
	_PremiereAnnee = PremiereAnnee;
	_DerniereAnnee = DerniereAnnee;
	ReAlloueMasque();
}
