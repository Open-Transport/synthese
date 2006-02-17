
#ifndef SYNTHESE_CDOCUMENT_H
#define SYNTHESE_CDOCUMENT_H

typedef enum
{
	PDF,
	HTML,
	GIF,
	JPEG,
	PNG
} tFormatFichier;

class cDocument
{
protected:
	tIndex			_Index;					//!< Index du document dans l'environnement
	cTexte			_URL;					//!< URL d'affichage du document sur le serveur web
	tFormatFichier	_Format;				//!< Format du document
	cTexte			_DescriptionGenerale;	//!< Description du document � afficher hors de tout contexte
	cTexte			_DescriptionLocale;		//!< Description du document � afficher dans un contexte local au document (ex: si la photo est affich�e sur une page relative � l'arr�t qu'elle d�crit)

public:
	//!	\name Modificateurs
	//@{
	bool			SetURL(const cTexte&);
	bool			SetFormat(tFormatFichier);
	bool			setDescriptionGenerale(const cTexte&);
	bool			setDescriptionLocale(const cTexte&);
	//@}

	//!	\name Accesseurs
	//@{
	const cTexte&	DescriptionLocale()	const;
	const cTexte&	URL()				const;
	tFormatFichier	Format()			const;
	tIndex			Index()				const;
	//@}
};

#include "cDocument.inline.h"

#endif
