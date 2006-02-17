/*! \file cPhoto.inline.h
	\brief Méthodes inline classes de gestion des photos
	\author Hugues Romain
	\date 2000-2003
*/



/*! \brief Ajout d'une zone cliquable liant une autre photo
	\warning Pas de controle d'erreur
	\author Hugues Romain
	\date 2002
*/
inline bool cPhoto::addMapPhoto(const cTexte& newCoords, tIndex newLienPhoto, const cTexteHTML& newAlt)
{
	// Recherche du premier index disponible
	tIndex NumeroMap = _Map.Active();
	if (NumeroMap == INCONNU)
		return false;

	return _Map.getElement(NumeroMap).SetDonnees(newCoords, newLienPhoto, newAlt);
}



/*! \brief Ajout d'une zone cliquable liant une URL
	\warning Pas de controle d'erreur
	\author Hugues Romain
	\date 2002
*/
inline bool cPhoto::addMapPhoto(const cTexte& newCoords, const cTexte& newURL, const cTexteHTML& newAlt)
{
	// Recherche du premier index disponible
	tIndex NumeroMap = _Map.Active();
	if (NumeroMap == INCONNU)
		return false;

	return _Map.getElement(NumeroMap).SetDonnees(newCoords, newURL, newAlt);
}

inline tIndex cPhoto::NombreMaps() const
{
	return _Map.Taille();
}





// cPhoto 1.0 - Map
// ____________________________________________________________________________
//
// 
// ____________________________________________________________________________ 
inline const cMapPhoto& cPhoto::Map(tIndex i) const
{
	return _Map[i];
}
// © Hugues Romain 2003
// ____________________________________________________________________________ 





inline const cTexteHTML&		cMapPhoto::Alt()		const
{
	return vAlt;
}

inline const cTexte&		cMapPhoto::Coords()	const
{
	return(vCoords);
}

inline const tIndex cMapPhoto::LienPhoto()	const
{
	return(vLienPhoto);
}

inline const cTexte&		cMapPhoto::URL()		const
{
	return(vURL);
}
