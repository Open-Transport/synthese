/*--------------------------------------*
 |                                      |
 |  APDOS - SYNTHESE v0.6               |
 |  © Hugues Romain 2000/2002           |
 |  CJourCirculation.cpp                |
 |  Header Classes JC                   |
 |                                      |
 *--------------------------------------*/

#ifndef SYNTHESE_CJOURCIRCULATION_H
#define SYNTHESE_CJOURCIRCULATION_H
 

// Jours de circulation
typedef unsigned long tMasque;
typedef unsigned char tCategorieJC;
enum tSens
{
	Positif = '+',
	Negatif = '-'
};
class cJC;

#include "cTexte.h"
#include "Temps.h"

/** Calendrier de circulation
	@ingroup m05
 
	Un tableau de 32 bits est créé par mois, stocké dans un int. Le bit de poids faible correspond au jour 1, le bit de poinds fort au jour 32 (inutilisé).
 */
class cJC
{
	tMasque*		vJoursAnnee; 		//!< Tableau contenant les bits correspondant à chaque jour (l'élement 0 est inutilisé)
	cAnnee			_PremiereAnnee;		//!< Année réelle correspondant au premier octet du masque
	cAnnee			_DerniereAnnee;		//!< Année réelle correspondant au dernier octet du masque
	tCategorieJC	vCategorie;
	cTexte			vIntitule;
	tIndex			vCode;

	tIndex			getIndexMois(const cDate& __Date)	const;
	tIndex			getIndexMois(tAnnee __Annee, tMois __Mois)	const;

public:
	
	//! \name Constructeurs, fonctions de construction, et destructeur
	//@{
	cJC();
	cJC(tAnnee, tAnnee, const cTexte&);
	cJC(const cJC&);
	~cJC();
	tMasque*	AlloueMasque()									const;
	//@}
	
	//! \name Modificateurs
	//@{
	void	RAZMasque(bool ValeurBase=false);
	void	ReAlloueMasque();
	void	setCategorie(tCategorieJC newCategorie=255); //!< Defaut = Reset
	bool	SetCircule(const cDate&, tSens Sens = Positif);
	void	setCircule(const cDate&, tSens Sens = Positif);
	bool	SetCircule(const cDate& DateDebut, const cDate& DateFin, tSens Sens = Positif, tDureeEnJours Pas=1);
	bool	setIndex(tIndex);
	void	setIntitule(const cTexte& Texte);
	void	setMasque(const tMasque*);
	void	setAnnees(tAnnee, tAnnee);
	//@}
		
	//! \name Calculateurs
	//@{
	void		SetInclusionToMasque(tMasque*, tSens Sens=Positif)	const;
	void		SetInclusionToMasque(cJC&, tSens Sens=Positif)		const;
	bool		TousPointsCommuns(const cJC&, const tMasque*)		const; //!< Sur le masque de this uniquement: ce n'est pas l'égalité
	bool		UnPointCommun(const tMasque*)						const;
	bool		UnPointCommun(const cJC&)							const;
	size_t		Card(const tMasque*)								const; //!< Sur le masque de this uniquement
	size_t		Card(const cJC&)									const; //!< Sur le masque de this uniquement
	bool		Circule(const cDate&)								const;
	tMasque*	ElementsNonInclus(const cJC& AutreJC)				const;
	tMasque*	Et(const cJC&)										const;
	//@}
	
	//! \name Accesseurs
	//@{
	tCategorieJC	Categorie()								const;
	tIndex			Index()									const;
	const tMasque*	JoursAnnee()								const;
	cDate			PremierJourFonctionnement()					const;
	//@}
	
	//! \name Affichage
	//@{
	template <class T> T& Intitule(T& Objet) const
	{
		return(Objet << vIntitule);
	}
	//@}
};

    

#endif
