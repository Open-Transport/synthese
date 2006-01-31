/*! \file cMateriel.h
	\brief En-t�te classe cMateriel
*/

#ifndef SYNTHESE_CMATERIEL_H
#define SYNTHESE_CMATERIEL_H

class cMateriel;

//! Indique le code obligatoire pour le mat�riel repr�sentant les jonctions � pied (Provisoire : � remplacer par un param�tre)
#define MATERIELPied 12

#include "cTexte.h"


/** Impl�mentation de la notion de @ref defMateriel
	@ingroup m05
	@author Hugues Romain
	@date 2001
*/
class cMateriel
{
	cTexte			vLibelleSimple;
	cTexte			vArticle;
	cTexte			vDoc;
	cTexte			vIndicateur;
	tVitesseKMH		vVitesseMoyenneMax;
	tIndex			vCode;

public:
	
	//! \name Accesseurs
	//@{
	bool			EstAPied()			const;
	const cTexte&	getArticle()		const;
	const cTexte&	getIndicateur()		const;
	const cTexte&	getLibelleSimple()	const;
	tIndex			Code()				const;
	tVitesseKMH		VitesseMoyenneMax()	const;
	//@}
	
	//! \name Modificateurs
	//@{
	void setArticle(const cTexte& newArticle);
	void setDoc(const cTexte& newVal);
	void setIndicateur(const cTexte& newVal);
	void setLibelleSimple(const cTexte& newLibelleSimple);
	bool setVitesseMoyenneMax(tVitesseKMH);
	//@}

	//! \name Constructeur et destructeur
	//@{
	cMateriel(tIndex);
	~cMateriel();
	//@}
	
	//! \name Sorties
	//@{
	/*template <class T> T& toXML(T& Tampon) const
	{
		Tampon << "<materiel code=\"" << vCode << "\">";
		Tampon << "<libelle>" << vLibelleSimple << "</libelle></materiel>";
		
		return (Tampon);
	}*/
	//@}
};

#endif
