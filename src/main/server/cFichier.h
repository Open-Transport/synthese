/*!	\file cFichier.h
	\brief En-t�te, inline, template classe fichier
*/

#ifndef SYNTHESE_CFICHIER_H
#define SYNTHESE_CFICHIER_H


#include "cFormatFichier.h"
#include "cTexte.h"
#include <fstream>

/** Fichier en lecture
	
*/
class cFichier
{
protected:
	ifstream				_Fichier;				//!< Variable d'acc�s au fichier \todo Voir si on peut g�rer un fichier en lecture/�criture
	const cFormatFichier*	_Format;				//!< Format du fichier
	cTexte					_CheminFichierFormats;	//!< Chemin du fichier des formats
	cTexte					_Chemin;				//!< Chemin d'acc�s au fichier
	
public:
	//!	\name Calculateurs
	//@{
	TypeSection	LireLigne(cTexte&);
	int		 	LireLigneFormat(cTexte&);
	int			NumeroMaxElement(int Position=1);
	tIndex		NombreElementsAAllouer();
	bool		Ouvrir();
	bool		ProchaineSection(cTexte& Tampon, TypeSection TS);
	bool		RechercheSection(cTexte& Tampon, const cTexte& Intitule, char CarEntreeSection);
	void		Fermer();
	//@}
	
	//!	\name Constructeur et destructeur
	//@{
	cFichier(const cTexte& CheminFichier, const cFormatFichier* Format);
	~cFichier();
	//@}
};

/** @} */

#endif
