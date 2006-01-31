/*------------------------------------*
 |                                    |
 |  APDOS / APISAPI - SYNTHESE v0.6   |
 |  � Hugues Romain 2000/2001         |
 |  cArretPhysique.h                  |
 |  Header Classes ArretPhysique      |
 |                                    |
 *------------------------------------*/

#ifndef SYNTHESE_CQUAI_H
#define SYNTHESE_CQUAI_H

typedef short int tNumeroVoie;
class cArretPhysique;

#include "cTexte.h"
#include "cPhoto.h"
#include "Point.h"

/*! \brief Arr�ts physiques (quais, arr�ts de bus, etc.)
	@ingroup m05
*/
class cArretPhysique
{
protected:
	cTexte		_Nom;	//!< D�signation du quai
	cPhoto*		_Photo;	//!< Photo repr�sentative du quai
	cPoint		_Point;	//!< G�olocalisation du quai

public:
	//!	\name Accesseurs
	//@{
	const cPoint&	getPoint()		const;
	const cTexte&	getNom()		const;
	const cPhoto*	getPhoto()		const;
	//@}
	
	//!	\name Modificateurs
	//@{
	bool setX(CoordonneeKMM);
	bool setY(CoordonneeKMM);
	bool setPhoto(cPhoto*);
	bool setNom(const cTexte&);
	//@}
	
	//!	\name Constructeur
	//@{
	cArretPhysique();
	//@}
};



/*!	\brief Accesseur vers le nom du quai au sein du point d'arr�t
	\return R�f�rence vers le nom du quai au sein du point d'arr�t
	\author Hugues Romain
	\date 2001
*/
inline const cTexte& cArretPhysique::getNom() const
{
	return(_Nom);
}

/*!	\brief Accesseur vers le point de g�olocalisation du point d'arr�t
	\return R�f�rence vers le point de g�olocalisation du point d'arr�t
	\author Hugues Romain
	\date 2001
*/
inline const cPoint& cArretPhysique::getPoint() const
{
	return(_Point);
}

/*!	\brief Accesseur vers la photo du quai
	\return Pointeur vers la photo du quai
	\author Hugues Romain
	\date 2001
*/
inline const cPhoto* cArretPhysique::getPhoto() const
{
	return(_Photo);
}

/*!	\brief D�finit la photo du quai
	\param Photo Pointeur vers la photo � lier
	\return true si op�ration effectu�e avec succ�s
	\author Hugues Romain
	\date 2005
*/
bool inline cArretPhysique::setPhoto(cPhoto* Photo)
{
	_Photo = Photo;
	return true;
}

/*!	\brief D�finit le nom du quai au sein du point d'arr�t
	\param Nom Nom � donner au quai
	\return true si op�ration effectu�e avec succ�s
	\author Hugues Romain
	\date 2005
*/
bool inline cArretPhysique::setNom(const cTexte& Nom)
{
	_Nom.Vide();
	_Nom << Nom;
	return true;
}
#endif
