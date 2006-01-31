/*!	\file cAccesPADe.inline.h
 	\brief M�thodes inline classe AccesPADe
*/

#ifndef SYNTHESE_CACCESPADE_INLINE_H
#define SYNTHESE_CACCESPADE_INLINE_H
 


inline tTypeAccesPADe cAccesPADe::getTypeAcces() const
{
	return(vTypeAcces);
}

inline cAccesPADe* cAccesPADe::getSuivant() const
{
	return(vSuivant);
}



/*!	\brief Accesseur index de la d�signation au sein du point d'arr�t
	\return Index de la d�signation au sein du point d'arr�t
	\author Hugues Romain
	\date 2000-2005
*/
inline tIndex cAccesPADe::numeroDesignation() const
{
	return(_Index);
}



/*!	\brief Accesseur nom du point d'arr�t tel que d�crit par la d�signation
	\return Nom du point d'arr�t tel que d�crit par la d�signation
	\author Hugues Romain
	\date 2001
*/
inline const cTexte& cAccesPADe::getNom() const
{
	return(vNomArretLogique);
}


inline cCommune* cAccesPADe::getCommune() const
{
	return(vCommune);
}

inline cArretLogique* cAccesPADe::getArretLogique() const
{
	return(vArretLogique);
}


inline void cAccesPADe::setSuivant(cAccesPADe* newAccesPADe)
{
	vSuivant = newAccesPADe;
}



/*!	\brief Test de l'existance d'un point d'arr�t parmi les lieux d�sign�s par l'objet
	\param __ArretLogique Point d'arr�t recherch�
	\return true si le point d'arr�t figure dans la liste des lieux de l'objet, false sinon
	\author Hugues Romain
	\date 2001
*/
inline bool cAccesPADe::inclue(const cArretLogique* __ArretLogique) const
{
	for (const cAccesPADe* __AccesPADe = this; __AccesPADe != NULL; __AccesPADe = __AccesPADe->getSuivant())
		if (__AccesPADe->getArretLogique() == __ArretLogique)
			return true;
	return false;
}



/*!	\brief Modificateur du num�ro de la d�signation au sein du point d'arr�t
	\param NumeroDesignation Num�ro de la d�signation au sein du point d'arr�t
	\return true si l'op�ration a �t� effectu�e avec succ�s
	\author Hugues Romain
	\date 2005
*/
inline bool cAccesPADe::setNumeroDesignation(tIndex NumeroDesignation)
{
	_Index = NumeroDesignation;
	return true;
}



/*!	\brief Ajoute un cAccesPADe � la fin de la liste cha�n�e
	\param Objet cAccesPADe � ajouter
	\return true si l'op�ration a �t� effectu�e avec succ�s
	\author Hugues Romain
	\date 2005
*/
inline bool cAccesPADe::Chaine(cAccesPADe* Objet)
{
	bool __TousArretsDeCorrespondance = Objet->TousArretsDeCorrespondance() && TousArretsDeCorrespondance();
	cAccesPADe* curAccesPADe;
	
	for (curAccesPADe = this; curAccesPADe->getSuivant() != NULL; curAccesPADe = curAccesPADe->getSuivant())
		curAccesPADe->_TousArretsDeCorrespondance = __TousArretsDeCorrespondance;
	curAccesPADe->setSuivant(Objet);
	
	for (; curAccesPADe != NULL; curAccesPADe = curAccesPADe->getSuivant())
		curAccesPADe->_TousArretsDeCorrespondance = __TousArretsDeCorrespondance;

	return true;
}



/*!	\brief Accesseur tous arr�ts de correspondance
	\author Hugues Romain
	\date 2005
*/
inline bool cAccesPADe::TousArretsDeCorrespondance() const
{
	return _TousArretsDeCorrespondance;
}


#endif
