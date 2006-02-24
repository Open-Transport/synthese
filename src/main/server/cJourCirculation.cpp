/*!	\file cJourCirculation.cpp
	\brief Implémentation classes calendriers de circulations
	\date 2000-2002
	\author Hugues Romain
*/

#include "cJourCirculation.h"
#include "cDate.h"

using namespace std;

/** Constructor.
*/
cJC::cJC(const tAnnee& PremiereAnnee, const tAnnee& DerniereAnnee, const size_t& id, const string& newIntitule)
: _PremiereAnnee(PremiereAnnee)
, _DerniereAnnee(DerniereAnnee)
, _id(id)
{
	// Masque
	vJoursAnnee.resize((_DerniereAnnee.Valeur() - _PremiereAnnee.Valeur() + 1) * MOIS_PAR_AN);

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
bool cJC::SetCircule(const cDate& Date, InclusionType Sens)
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

void cJC::setCircule(const cDate& Date, InclusionType Sens)
{
	Mask Masque = 1;
	Masque <<= (Date.Jour() - 1);

	if (Sens == InclusionType_POSITIVE)
	{
		vJoursAnnee[getIndexMois(Date)] |= Masque;
		
	}
	else
	{
		Masque = ~Masque;
		vJoursAnnee[getIndexMois(Date)] &= Masque;
	}
}

void cJC::SetInclusionToMasque(Calendar& Masque, InclusionType Sens) const
{
	if (Sens == InclusionType_POSITIVE)
		for (tAnnee iAnnee = _PremiereAnnee.Valeur(); iAnnee <= _DerniereAnnee.Valeur(); iAnnee++)
			for (tMois iMois=1; iMois<= MOIS_PAR_AN; iMois++)
				Masque[getIndexMois(iAnnee, iMois)] |= vJoursAnnee[getIndexMois(iAnnee, iMois)];
	else
	{
		Mask tempMasque;
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

	Mask Masque = 1;
	Masque <<= (DateTest.Jour() - 1);
	return (Masque & vJoursAnnee[getIndexMois(DateTest)]) != 0;
}

bool cJC::UnPointCommun(const Calendar& AutreMasque) const
{
	for (tAnnee iAnnee = _PremiereAnnee.Valeur(); iAnnee<= _DerniereAnnee.Valeur(); iAnnee++)
		for (tMois iMois=1; iMois<=MOIS_PAR_AN; iMois++)
			if (AutreMasque[getIndexMois(iAnnee, iMois)] & vJoursAnnee[getIndexMois(iAnnee, iMois)])
				return true;
	return false;
}

bool cJC::TousPointsCommuns(const cJC& JCBase, const Calendar& Masque2) const
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

cJC::Calendar cJC::Et(const cJC& AutreJC) const
{
	Calendar newMasque;
	for (tAnnee iAnnee = _PremiereAnnee.Valeur(); iAnnee <= _DerniereAnnee.Valeur(); iAnnee++)
		for (tMois iMois=1; iMois<= MOIS_PAR_AN; iMois++)
			newMasque[getIndexMois(iAnnee, iMois)] = AutreJC.vJoursAnnee[getIndexMois(iAnnee, iMois)] & vJoursAnnee[getIndexMois(iAnnee, iMois)];
	return(newMasque);
}

cJC::Calendar cJC::ElementsNonInclus(const cJC& AutreJC) const
{
	// L'opérateur ! ne semble pas convenir: est ce du bit a bit ?
	Calendar newMasque;
	for (tAnnee iAnnee = _PremiereAnnee.Valeur(); iAnnee <= _DerniereAnnee.Valeur(); iAnnee++)
		for (tMois iMois=1; iMois<= MOIS_PAR_AN; iMois++)
			newMasque[getIndexMois(iAnnee, iMois)] = !vJoursAnnee[getIndexMois(iAnnee, iMois)] & AutreJC.vJoursAnnee[getIndexMois(iAnnee, iMois)];
	return(newMasque);
}

size_t cJC::Card(const Calendar& Masque) const
{
	size_t t=0;
	Mask tempMasque;
	Mask tempMasque2;
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
}

bool cJC::SetCircule(const cDate &DateDebut, const cDate &DateFin, InclusionType Sens, tDureeEnJours Pas)
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


void cJC::setMasque(const Calendar& AutreMasque)
{
	for (size_t i=0; i< (size_t)((_DerniereAnnee.Valeur() - _PremiereAnnee.Valeur() + 1) * MOIS_PAR_AN); i++)
		vJoursAnnee[i] = AutreMasque[i];
}

cDate cJC::PremierJourFonctionnement() const
{
	cDate curDate;
	Mask tempMasque;
	Mask tempMasque2;
	
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
size_t cJC::getIndexMois(const cDate& __Date)	const
{
	return getIndexMois(__Date.Annee(), __Date.Mois());
}



/** Calcul de l'index du tableau de bits à lire pour accéder à un mois.
	@param __Annee Année à lire
	@param __Mois Mois à lire
	@return L'index du tableau de bits à lire pour accéder au mois
*/
size_t cJC::getIndexMois(tAnnee __Annee, tMois __Mois)	const
{
	return (__Annee - _PremiereAnnee.Valeur()) * MOIS_PAR_AN + __Mois;
}


bool cJC::UnPointCommun(const cJC& AutreMasque) const
{
	return(UnPointCommun(AutreMasque.vJoursAnnee));
}

size_t cJC::Card(const cJC& AutreJC) const
{
	return(Card(AutreJC.vJoursAnnee));
}

const cJC::Category& cJC::Categorie() const
{
	return(vCategorie);
}

void cJC::setCategorie(Category newCategorie)
{
	vCategorie = newCategorie;
}

void cJC::setIntitule(const string& Texte)
{
	vIntitule = Texte;
}



/*!	\brief Accesseur index de l'objet dans l'environnement
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
	return(vJoursAnnee);
}

void cJC::SetInclusionToMasque(cJC &JourCirculation, InclusionType Sens) const
{
	SetInclusionToMasque(JourCirculation.vJoursAnnee, Sens);
}

