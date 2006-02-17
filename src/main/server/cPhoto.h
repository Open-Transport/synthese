/*! \file cPhoto.h
	\brief En-tête classes gestion des photos
	\author Hugues Romain
	\date 2000-2002
*/

#ifndef SYNTHESE_CPHOTO_H
#define SYNTHESE_CPHOTO_H

#include "cTexte.h"
#include "cDocument.h"

/*! \brief Zone cliquable sur photo, donnant accès à une autre photo ou une URL
	\author Hugues Romain
	\date 2002
*/
class cMapPhoto
{
	cTexte			vCoords;	//!< Chaîne de caractères décrivant les coordonnées de la zone cliquable, au format des MAP du langage HTML
	tIndex			vLienPhoto;	//!< Numéro de la photo liée par la zone cliquable
	cTexte			vURL;		//!< URL liée par la zone cliquable
	cTexteHTML		vAlt;		//!< Chaînes de caractères décrivant la zone cliquable

public:
	//! \name Accesseurs
	//@{
	const cTexteHTML&	Alt()			const;
	const cTexte&		Coords()		const;
	const tIndex		LienPhoto()		const;
	const cTexte&		URL()			const;
	//@}
	
	//! \name Modificateurs
	//@{
	bool SetDonnees(const cTexte& newCoords, tIndex newLienPhoto, const cTexteHTML& newAlt);
	bool SetDonnees(const cTexte& newCoords, const cTexte& newURL, const cTexteHTML& newAlt);
	//@}
};

/*!  \brief Photo à afficher dans la fiche arrêt
	\author Hugues Romain
	\date 2002
*/
class cPhoto : public cDocument
{
	cTableauDynamiqueObjets<cMapPhoto>	_Map;	//!< Liste des zones cliquables
	
public:

	//! \name Accesseurs
	//@{
	const cMapPhoto&	Map(tIndex)		const;
	tIndex				NombreMaps()	const;
	//@}
	
	//! \name Constructeur et fonctions de construction
	//@{
	cPhoto(tIndex);
	//@}
	
	//! \name Modificateurs
	//@{
	bool addMapPhoto(const cTexte& newCoords, tIndex newLienPhoto, const cTexteHTML& newAlt);
	bool addMapPhoto(const cTexte& newCoords, const cTexte& newURL, const cTexteHTML& newAlt);
	//@}
};

#include "cPhoto.inline.h"

#endif
