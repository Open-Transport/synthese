/*!	\file cAccesPADe.cpp
	\brief Impl�mentation Classe AccesPADe
*/

#include "cAccesPADe.h"



/*! \brief Constructeur
	\param NewPA Point d'arr�t d�sign�
	\param newCommune Commune d'implantation
	\param newNom Nom de l'arr�t dans la commune
	\author Hugues Romain
	\date 2001
*/
cAccesPADe::cAccesPADe(cArretLogique* newPA, cCommune* newCommune, const cTexte& newNom)
{
	vArretLogique = newPA;
	_TousArretsDeCorrespondance = vArretLogique->CorrespondanceAutorisee() != CorrInterdite;
	vSuivant = NULL;
	vCommune = newCommune;
	vNomArretLogique = newNom;
}




/*!	\brief Constructeur de copie
	\param AccesPADeACopier Objet � copier
	\author Hugues Romain
	\date 2001
	
Ce constructeur copie les donn�es commune et arr�t depuis une autre d�signation.
*/
cAccesPADe::cAccesPADe(cAccesPADe* AccesPADeACopier)
{
	vArretLogique = AccesPADeACopier->getArretLogique();
	_TousArretsDeCorrespondance = AccesPADeACopier->TousArretsDeCorrespondance();
	vCommune = AccesPADeACopier->getCommune();
	vSuivant = NULL;
}



cMoment	cAccesPADe::momentArriveePrecedente(const cMoment& MomentArrivee, const cMoment& MomentArriveeMin)	const
{
	cMoment tempMoment;
	cMoment tempMoment2;
	tempMoment = MomentArriveeMin;
	for (const cAccesPADe* curAccesPADe = this; curAccesPADe != NULL; curAccesPADe = curAccesPADe->getSuivant())
	{
		tempMoment2 = curAccesPADe->getArretLogique()->MomentArriveePrecedente(MomentArrivee, MomentArriveeMin);
		if (tempMoment2 > tempMoment)
			tempMoment = tempMoment2;
	}
	return(tempMoment);
}


cMoment	cAccesPADe::momentDepartSuivant(const cMoment& MomentDepart, const cMoment& MomentDepartMax, const cMoment& __MomentCalcul) const
{
	cMoment tempMoment;
	cMoment tempMoment2;
	tempMoment = MomentDepartMax;
	for (const cAccesPADe* curAccesPADe = this; curAccesPADe != NULL; curAccesPADe = curAccesPADe->getSuivant())
	{
		tempMoment2 = curAccesPADe->getArretLogique()->MomentDepartSuivant(MomentDepart, MomentDepartMax, __MomentCalcul);
		if (tempMoment2 < tempMoment)
			tempMoment = tempMoment2;
	}
	return(tempMoment);
}





