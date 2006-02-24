
#ifndef SYNTHESE_CDOCUMENT_H
#define SYNTHESE_CDOCUMENT_H

#include <string>

class cDocument
{
public:
	typedef enum
	{
		FileFormat_PDF,
		FileFormat_HTML,
		FileFormat_GIF,
		FileFormat_JPEG,
		FileFormat_PNG
	} FileFormat;


protected:
	const size_t			_id;					//!< Index du document dans l'environnement
	std::string			_URL;					//!< URL d'affichage du document sur le serveur web
	FileFormat	_Format;				//!< Format du document
	std::string			_DescriptionGenerale;	//!< Description du document à afficher hors de tout contexte
	std::string			_DescriptionLocale;		//!< Description du document à afficher dans un contexte local au document (ex: si la photo est affichée sur une page relative à l'arrêt qu'elle décrit)

public:
	//!	\name Modificateurs
	//@{
	bool			SetURL(const std::string&);
	bool			SetFormat(const FileFormat&);
	bool			setDescriptionGenerale(const std::string&);
	bool			setDescriptionLocale(const std::string&);
	//@}

	//!	\name Accesseurs
	//@{
	const std::string&	DescriptionLocale()	const;
	const std::string&	URL()				const;
	FileFormat	Format()			const;
	const size_t&			getId()				const;
	//@}

	cDocument(const size_t&);
	virtual ~cDocument();
};

#endif
