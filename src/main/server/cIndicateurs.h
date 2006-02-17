/*! \file cIndicateurs.h
	\brief En-t�te classes Indicateurs horaires
*/

#ifndef SYNTHESE_CINDICATEURS_H
#define SYNTHESE_CINDICATEURS_H

/** @degroup m35 35 Indicateurs papier

	Le module d'indicateur papier doit �tre mis aux normes :
		- r�duction du module � la simple fonction de calcul
		- mise en place d'objets de biblioth�que d'interface
		- mise en interface du rendu
	@{
*/

// Indicateurs
enum tTypeGareIndicateur
{
	PassageFacultatif = 'F',
	PassageObligatoire = 'O',
	PassageSuffisant = 'S'
};
enum tTypeOD
{
	Texte,
	Terminus,
	Indetermine
};
class cIndicateurs;
class cColonneIndicateurs;
class cGareIndicateurs;
class cRenvoiIndicateurs;
typedef int tNumeroIndicateur;
typedef float tDistancePt;

//! @name Mesures (� mettre dans interface)
//@{
#define IndicateursMarge 50
#define IndicateursHaut 841
#define IndicateursDroite 595
#define IndicateursDistanceTableauHaut 30
#define IndicateursLargeurColonneCode 22
#define IndicateursLargeurColonneCommune 50
#define IndicateursLargeurColonnePA 60
#define IndicateursLargeurColonneDA 6
#define IndicateursLargeurColonneTrain 20.8
#define IndicateursHauteurLigne 7
#define IndicateursNombreColonnesMax 17
#define IndicateursNombreLignesSupplementaires 5
//@}

#include "cTexte.h"
#include "Temps.h"
#include "cEnvironnement.h"

/** Ligne de tableau d'indicateur
	@author Hugues Romain
	@date 2001
 
 caract�ris�e par:
   - un point d'arr�t
   - un type de dessete (Depart/Arrivee/Passage)
   - un caractere obligatoire ou facultatif (pour la selection des lignes du tableau)
 Le chainage des colonnes se fait par le biais du pointeur Suivant.
*/
class cGareIndicateurs
{
private:
	// Variables
	LogicalPlace*				vPA;
	tTypeGareLigneDA	vTypeDA;	//!< Gare de d�part, d'arriv�e, ou de passage
	tTypeGareIndicateur	vObligatoire;
	cGareIndicateurs*		vSuivant;

public:
	// Accesseurs
	LogicalPlace*				getArretLogique()	const;
	cGareIndicateurs*		getSuivant()		const;
	tTypeGareIndicateur 	Obligatoire() 		const;
	tTypeGareLigneDA	TypeDA()		const;
	
	// Calculateurs
	bool EstDepart() const;
	bool EstArrivee() const;

	// Modificateur
	void setSuivant(cGareIndicateurs* newVal);
	
	// Constructeur
	cGareIndicateurs(LogicalPlace*, tTypeGareLigneDA, tTypeGareIndicateur);
};


/** Colonne de tableau d'indicateur papier
	@author Hugues Romain
	@date 2001

 caract�ris�e par:
   - des pointeurs vers les cArrets d�crits
   - le code Postscript de la colonne
   - le nombre de lignes de la colonne
 Le chainage des colonnes se fait par le biais du pointeur Suivant.
*/
class cColonneIndicateurs
{
private:
	// Variables
	const cHoraire**		vColonne;
	cColonneIndicateurs*	vSuivant;
	cTexte				vPostScript;
	tMasque*			vMasque;
	cRenvoiIndicateurs*	vRenvoi;
	cLigne*				vLigne;
	size_t				vNombreGares;
	tTypeOD			vOrigineSpeciale;
	tTypeOD			vDestinationSpeciale;
	
public:
	// Modificateurs
	void CopiePostScript(const cTexte& newVal);
	void setColonne(size_t n, const cHoraire *newVal);
	void setSuivant(cColonneIndicateurs* newVal);
	void setDestinationSpeciale(tTypeOD newVal);
	void setOrigineSpeciale(tTypeOD newVal);
	void setRenvoi(cRenvoiIndicateurs* newVal);
	
	// Calculateurs
	int	operator <= (const cColonneIndicateurs&) const;
	
	// Accesseurs
	tTypeOD			DestinationSpeciale()	const;
	cLigne*				getLigne()			const;
	const cTexte&		getPostScript()		const;
	tMasque*			getMasque()			const;
	cColonneIndicateurs*	getSuivant()			const;
	cRenvoiIndicateurs*	getRenvoi()			const;
	tTypeOD			OrigineSpeciale()		const;
	
	// Constructeur
	cColonneIndicateurs(size_t NombreGares, cLigne*, cJC*);
};


/** Renvoi d'indicateur papier, caract�ris�e par:
   - un num�ro
   - un libell�
   - un code de jour de circulation

   Le libell� est clacul� en fonction du JC du renvoi, et de celui de la page d'indicateur (diff�rence).
   @author Hugues Romain
   @date 2001
*/
class cRenvoiIndicateurs
{
private:
	// Variables
	size_t	vNumero;
	cJC*	vJC;

public:
	// Accesseurs
	size_t	Numero()	const;
	cJC*	getJC()		const;
	
	// Constructeur
	cRenvoiIndicateurs(cJC*, size_t);
};


/** Tableau d'indicateur papier, caract�ris�e par:
   - des gares (cGareIndicateurs)
   - des colonnes (cColonneIndicateurs)
   - un masque de circulation de base (JC)
   - Un titre
	@author Hugues Romain
	@date 2001
*/
class cIndicateurs
{
private:
	// Variables
	bool				vCommencePage;
	cGareIndicateurs*		vDerniereGI;
	cEnvironnement*		vEnvironnement;
	cJC					vJC;
    	size_t				vNombreColonnes;
	size_t				vNombreGares;
	size_t				vNombreRenvois;
	cColonneIndicateurs*	vPremiereCI;
	cGareIndicateurs*		vPremiereGI;
	cTexte				vTitre;
	
public:
	
	// Fonction
	bool Add(cColonneIndicateurs*, cJC*);
	
	// Calculateurs
	void EcritTableaux(size_t HDispo, size_t NumeroColonne, size_t NombreTableaux, bool RenvoisAEcrire, size_t NumeroPageRelatif, ofstream& FichierSortie) const;

	// Accesseurs
	cColonneIndicateurs*	Colonne(size_t)		const;
	bool				CommencePage()	const;
	const cJC&			getJC()				const;
	cGareIndicateurs*		getPremiereGI()		const;
	const cTexte&		getTitre()			const;
	size_t				NombreColonnes()	const;
	size_t				NombreGares()		const;
	size_t				NombreRenvois()		const;
	cRenvoiIndicateurs*	Renvoi(size_t)		const;
	
	// Modificateurs
	void addArretLogique(LogicalPlace* newArretLogique, tTypeGareLigneDA newTypeDA, tTypeGareIndicateur newTypeGI);
	void ConstruitRenvois();
	void Reset();
	void setJC(const cJC& newVal, const cJC& newVal2);
	void setCommencePage(bool newVal);

	// Constructeur
	cIndicateurs(const cTexte& newTitre, cEnvironnement* newEnvironnement);
};

/** @} */

#endif
