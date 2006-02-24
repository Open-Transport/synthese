/*! \file cDistanceCarree.h
	\brief En-t�te classe cDistanceCarree
*/

#ifndef SYNTHESE_CDISTANCECARREE_H
#define SYNTHESE_CDISTANCECARREE_H

// Distance
typedef long int tDistanceM;
typedef long int tDistanceHM;
typedef long int tDistanceKM;
typedef short int tCategorieDistance;
tCategorieDistance CategorieDistance(tDistanceKM D, bool DAuCArre);

#include <cmath>
#include "Point.h"
#include "Parametres.h"

/*!	\brief Carr� de distance entre deux points avec approximation de 2 km�
	\author Hugues Romain
	\date 2005
	@ingroup m01
	
	Cette classe est utilis�e pour stocker des distances carr�es entre deux points, calcul�es � partir de coordonn�es g�ographiques dont la pr�cision n'exc�de pas le kilom�tre.
	
	Cette classe est issue d'un besoin de comparaison de distances en grande quantit� au niveau de la r�cursivit� du calcul d'itin�raires, � des fins d'optimisation du calcul, ce qui explique que la rapidit� soit ici privil�gi�e face � l'exactitude.
	 
	Cette logique explique �galement l'usage du carr� de la distance et non de la distance, �vitant ainsi le calcul de la racine carr�e du th�or�me de Pythagore (ce calcul est effectu� uniquement sur demande via l'accesseur Distance()) car ce calcul gourmand est inutile pour les comparaisons gr�ce � la stricte croissance de la fonction carr�.
		
	Le carr� de la distance entre deux points d�crits d'une telle mani�re (calcul� par exemple gr�ce au constructeur particulier cDistanceCarree(cPoint&,cPoint&) n'est alors connu qu'avec une pr�cision de 2 km� (soit une pr�cision de \f$ \sqrt{2} \f$ km pour la distance correspondante).
	
	Les op�rateurs classiques de comparaison entre deux distances doivent alors g�rer ce manque de pr�cision, en donnant non pas une comparaison directe des valeurs, mais en ne donnant que des informations s�res :  
		- les op�rateurs devant statuer sur une in�galit� large ou sur l'�galit� r�pondent vrai si le r�sultat est peut-�tre vrai.
		- les op�rateurs devant statuer sur une in�galit� stricte r�pondent vrai si le r�sultat est s�r d'�tre vrai.
	
	On d�finit ainsi une relation d'ordre partielle.
	
	La valeur de la distance carr�e suit les conventions suivantes :
		- une distance non encore sp�cifi�e vaut INCONNU (-1)
		- deux arr�ts faisant partie du m�me groupe d'acc�s (cAccesPADe) ont entre eux une distance carr�e nulle
		- deux arr�ts dont l'isobarycentre est situ� � moins de \f$ \sqrt{2} \f$ km l'un de l'autre ont une distance carr�e n�gligeable
		- deux arr�ts dont l'isobarycentre est situ� � plus de \f$ \sqrt{2} \f$ km l'un de l'autre ont une distance carr�e qui peut �tre compar�e avec justesse � une autre distance du m�me type
		
*/
class cDistanceCarree
{
	DistanceCarreeKM vDistanceCarreeKM;	//!< Valeur de la distance
	
public:
	//! \name Constructeurs
	//@{
	cDistanceCarree(const DistanceCarreeKM Valeur=INCONNU);
	cDistanceCarree(const cPoint&, const cPoint&);
	//@}

	//! \name Accesseurs
	//@{
	DistanceCarreeKM	DistanceCarree()	const;
	tDistanceKM			Distance()			const;
	bool				EstInconnu()		const;
	//@}
	
	//! \name Modificateurs
	//@{
	void setFromKM2(const DistanceCarreeKM);
	void setFromKM(const tDistanceKM);
	void setFromPoints(const cPoint&, const cPoint&);
	//@}
	
	//!\name Op�rateurs
	//@{
	int operator <= (const cDistanceCarree&)	const;
	int operator < (const cDistanceCarree&)		const;
	int operator == (const cDistanceCarree&)	const;
	//@}
};



/*!	\brief Fournit la valeur de l'objet en km�
	\return Distance au carr� en km�, avec une incertitude de 2 km�
	\author Hugues Romain
	\date 2005
	\warning La comparaison de deux distances carr�es issues de cette classe n'est certifi�e que si la diff�rence entre les deux distances carr�es est d'au moints 4 km� . Ne pas utiliser le r�sultat de cet accesseur pour des comparaisons. Effectuer des comparaisons uniquement avec les m�thodes de la classe cDistanceCarree.
*/
inline DistanceCarreeKM cDistanceCarree::DistanceCarree()	const
{
	return vDistanceCarreeKM;
}



/*!	\brief Fournit la valeur de l'objet en km
	\return Distance en km, avec une incertitude de \f$ \sqrt{2} \f$ km
	\author Hugues Romain
	\date 2005
	\warning La comparaison de deux distances issues de cette classe n'est certifi�e que si la diff�rence entre les deux distances est d'au moints \f$ 2\sqrt{2} \f$ km. Ne pas utiliser le r�sultat de cet accesseur pour des comparaisons. Effectuer des comparaisons uniquement avec les m�thodes de la classe cDistanceCarree.
*/
inline tDistanceKM cDistanceCarree::Distance()	const
{
	return (tDistanceKM) sqrt((double) vDistanceCarreeKM);
}



/*!	\brief Modifie la valeur d'apr�s un carr� de distance en km�
	\param Valeur Nouvelle valeur
	\author Hugues Romain
	\date 2005
*/
inline void cDistanceCarree::setFromKM2(const DistanceCarreeKM Valeur)
{
	vDistanceCarreeKM = Valeur;
}



/*!	\brief Modifie la valeur d'apr�s une distance en km
	\param Valeur Nouvelle valeur
	\author Hugues Romain
	\date 2005
*/
inline void cDistanceCarree::setFromKM(const tDistanceKM Valeur)
{
	vDistanceCarreeKM = (DistanceCarreeKM) (Valeur * Valeur);
}



/*!	\brief Op�rateur de comparaison inf�rieu ou �gal
	\param Autre Objet � comparer
	\author Hugues Romain
	\date 2005
	\return true si la distance � comparer est susceptible d'�tre sup�rieure ou �gale � l'objet en tenant compte de la pr�cision
*/
inline int cDistanceCarree::operator <= (const cDistanceCarree& Autre)	const
{
	return Autre.DistanceCarree() - DistanceCarree() > -4;
}



/*!	\brief Op�rateur de comparaison inf�rieu ou �gal strict
	\param Autre Objet � comparer
	\author Hugues Romain
	\date 2005
	\return true si la distance � comparer est certifi�e strictement sup�rieure � la l'objet en tenant compte de la pr�cision
*/
inline int cDistanceCarree::operator < (const cDistanceCarree& Autre)	const
{
	return Autre.DistanceCarree() - DistanceCarree() > 4;
}



/*!	\brief Op�rateur de comparaison d'�galit�
	\param Autre Objet � comparer
	\author Hugues Romain
	\date 2005
	\return true si les deux distances carr�es sont susceptibles d'�tre �gales en tenant compte de la pr�cision
*/
inline int cDistanceCarree::operator == (const cDistanceCarree& Autre)	const
{
	return Autre.DistanceCarree() - DistanceCarree() < 4 && Autre.DistanceCarree() - DistanceCarree() > -4;
}



/*! 	\brief Modifie l'objet avec un minorant du carr� de la distance entre deux points
	\author Hugues Romain
	\date 2005
	\param Point1,Point2 les deux points
	
	Ce modificateur est issu de la principale utilisation de l'objet cDistanceCarree, qui est la comparaison de distances lors d'�valuations effectu�es en grande quantit� au niveau de la r�cursivit� du calcul d'itin�raires.
	 
	 La logique d'utilisation de ce modificateur est � la source de la notion d'incertitude pr�sente dans la classe cDistanceCarree, issue des pertes d'information conjointes aux gains de performances obtenus gr�ce � l'utilisation de la seule partie kilom�trique arrondie des coordonn�es intervient dans le calcul.
	 
	 Cette logique explique �galement l'usage du carr� de la distance et non de la distance, �vitant ainsi le calcul de la racine carr�e du th�or�me de Pythagore (ce calcul est effectu� uniquement sur demande via l'accesseur Distance()) car ce calcul gourmand est inutile pour les comparaisons gr�ce � la stricte croissance de la fonction carr�.
	
	La distance carr�e exacte en m� vaut \f$ d=d_x^2+d_y^2 \f$, avec \f$ d_x=|x_{AutrePoint}-x_{this}| \f$ et \f$ d_y=|y_{AutrePoint}-y_{this}| \f$. 
	
	La distance carr�e calcul�e en km� vaut \f$ D=d_X^2+d_Y^2 \f$, avec \f$ d_X=|X_{AutrePoint}-X_{this}| \f$ et \f$ d_Y=|Y_{AutrePoint}-Y_{this}| \f$, o� \f$ X \f$ et \f$ Y \f$ sont les parties kilom�triques de \f$ x \f$ et \f$ y \f$.
	
	L'impr�cision du calcul vaut 2 km�. La racine carr�e du calcul donne une estimation de la distance entre les deux points � \f$ \sqrt{2} \f$ km pr�s.
	
	Si l'un des points poss�de des coordonn�es inconnus, alors la distance est inconnue.
*/
inline void cDistanceCarree::setFromPoints(const cPoint& Point1, const cPoint& Point2)
{
	if (Point1.unknownLocation() || Point2.unknownLocation())
		setFromKM2(INCONNU);
	else
		setFromKM2((Point1._XKM - Point2._XKM) * (Point1._XKM - Point2._XKM) + (Point1._YKM - Point2._YKM) * (Point1._YKM - Point2._YKM));
}



inline bool cDistanceCarree::EstInconnu() const
{
	return vDistanceCarreeKM == INCONNU;
}

#endif
