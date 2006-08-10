/*! \brief Gestion des chaînes de caractères
*/

#ifndef SYNTHESE_CTEXTE_H
#define SYNTHESE_CTEXTE_H

#include <cstdlib>
using namespace std;

class cTexte;

//! \name Format général des fichiers
//@{
// Type d'une section déterminé par le premier caractère de la ligne
// Type de section voir les define en début de FormatsFichiers.h
typedef int TypeSection;
#define TYPESection		'#'
#define TYPESousSection	'['
#define TYPELien		'>'
#define TYPECommentaire	'/'
#define TYPEAutre		2
#define TYPEVide		1
#define TYPEError		3
//@}

#define LONGUEURNUMEROS	6

#include <fstream>
#include "Parametres.h"
#include "cTableauDynamique.h"

/*!	\brief Classe chaîne de caracètres améliorée, assurant la conversion éventuelle en un format propre

La gestion de l'allocation mémoire est automatique.

\todo Nettoyer les fonctions de copie
*/
class cTexte : public cTableauDynamique<char>
{
protected:
	//!	\name Gestion interne
	//@{
	virtual tIndex	ConvertChar(const char c, char* Tampon=NULL) const;
	virtual bool	CompareChar(const char, const char) const;
	virtual void	Finalise();
	//@}
	
public:
	//!	\name Gestion de fichiers (à supprimer dès que possible)
	//@{
	TypeSection	LireLigne(ifstream&);
	bool		ProchaineSection(ifstream&, int TypeSection);
	bool		RechercheSection(ifstream&, const cTexte& Intitule, char CarEntreeSection);
	TypeSection	TypeSectionTampon()																const;
	//@}
	
	//! \name Contructeurs et destructeurs
	//@{
	explicit cTexte(tIndex TailleMax=0, bool AugmentationParDoublement=false);
	cTexte(const char *);
	cTexte(const string);
	cTexte(const cTexte&);
	virtual ~cTexte();
	//@}

	//! \name Modificateurs
	//@{
	cTexte& 	setTampon();
	cTexte&		ChangeTexte(char* newTexte, tIndex newTailleMax=0);
	cTexte&		ChangeTexte(cTexte& newTexte);
	cTexte&		Copie(const cTexte& Source);
	cTexte&		Copie(const cTexte& Source, tIndex LongueurMax);
	cTexte&		Copie(const char* Source);
	cTexte&		Copie(const char* Source, tIndex LongueurMax);
	cTexte&		Copie(const int Source, tIndex NombreChiffres=0);
	cTexte&		fAdresseComplete(const cTexte& AdresseBase);
	cTexte&		Repertoire();
	cTexte&		Trim();
	cTexte&		operator=(const cTexte&);
	//@}

	//! \name Calculateurs
	//@{
	bool		Compare(const cTexte& Autre, tIndex NombreChiffres=0, tIndex Position1=0, tIndex Position2=0) 	const;
	tIndex 		RechercheOccurenceGauche(char curChar, tIndex NombreOccurences=1, tIndex Position=0) 		const;
	cTexte 		Extrait(tIndex Position, int Longueur=INCONNU) 										const;
	tIndex		TailleApresCodage(const char*) 													const;
	int			operator==(const char*)															const;
	//@}
	
	//! \name Accesseurs
	//@{
	int			GetNombre(tIndex NombreChiffres=0, tIndex Position=0)		const;
	char*		Texte()											const;
	//@}
};

ostream& operator<<(ostream& flux, const cTexte& Obj);



/*!	\brief Classe de texte destinée à recevoir du texte sans caractère accentué
*/
class cTexteSansAccent : public cTexte
{
protected:
	tIndex ConvertChar(const char c, char* Tampon=NULL) const;
	bool CompareChar(const char, const char) const;
	
public:
	//! \name Constructeurs et destructeurs
	//@{
	//@}
};



/*!	\brief Classe de texte destinée à recevoir du texte au langage PostScript
*/
class cTextePostScript : public cTexte
{
protected:
	tIndex ConvertChar(const char c, char* Tampon=NULL) const;
	
public:
	//! \name Constructeurs et destructeurs
	//@{
	//@}
};



/*!	\brief Classe de texte destinée à recevoir du texte au langage HTML

Le but de cette classe est en particulier de remplacer les caractères accentués par les combinaisons standard de remplacement ex: é => &eacute;
*/
class cTexteHTML : public cTexte
{
protected:
	tIndex ConvertChar(const char c, char* Tampon=NULL) const;
	
public:
	//! \name Constructeurs et destructeurs
	//@{
	cTexteHTML(tIndex TailleMax=0, bool AugmentationParDoublement=false) : cTexte(TailleMax, AugmentationParDoublement) { }
	//@}
};




/*!	\brief Classe de texte destinée à recevoir des données suivant le format de codage interne à SYNTHESE (pour passage dans les URLs par exemple)
*/
class cTexteCodageInterne : public cTexte
{
public:
	cTexteCodageInterne() : cTexte() { } //!< Constructeur
};



/*!	\brief Classe de texte destinée à recevoir des données uniquement en minuscules et sans accent (voir cTexteMinuscules::ConvertChar() pour les détails des règles)
	\author Hugues Romain
	\date 2005
*/
class cTexteMinuscules : public cTexteSansAccent
{
protected:
	tIndex ConvertChar(const char c, char* Tampon=NULL) const;
	
public:
};


/*!	\brief Classe de texte destinée à gérer du texte devant être comparé avec évaluation de pertinence, pour intégration à un moteur de recherche d'objets par leur nom
	\author Hugues Romain
	\date 2005
*/
class cTexteRecherche : public cTexte
{
	
protected:
	cTexteMinuscules* 	_Mots;		//!< Liste des mots contenus dans le texte de base
	tIndex			_NombreMots;	//!< Nombre de mots

	void Finalise();
	
public:
	cTexteRecherche();
	~cTexteRecherche();
};


#include "cTexte.inline.h"

#endif
