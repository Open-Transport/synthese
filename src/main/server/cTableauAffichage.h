
#ifndef SYNTHESE_CTABLEAUAFFICHAGE_H
#define SYNTHESE_CTABLEAUAFFICHAGE_H

class cTableauAffichage;
class cLigne;
class cDescriptionPassage;
class cArretPhysique;
class LogicalPlace;
class cGareLigne;

#include "cSite.h"
#include "Parametres.h"
#include "cHeure.h"
#include <set>

/** @defgroup m34 34 Tableaux d'affichage
	@{
*/

/** Tableau d'affichage
*/
class cTableauAffichage : public cSite
{
protected:
	//!	\name Param�tres du tableau
	//@{
	cTexte								_Titre;				//!< Titre pour affichage
	tIndex								_NombreDeparts;		//!< Nombre de d�parts affich�s
	tIndex								_PremierDepart;		//!< Premier d�part affich�
	cHeure								_HeureBascule;		//!< Heure de basculement entre deux jours
	bool								_OriginesSeulement;	//!< Origines seulement ?
	tIndex								_NumeroPanneau;		//!< Num�ro du panneau dans la gare pour afficheur
	//@}

	//!	\name Donn�es
	//@{
	LogicalPlace*						_ArretLogique;		//!< Point d'arr�t affich�
	set<cArretPhysique*>		_ArretPhysiques;				//!< ArretPhysique(s) affich�s
	set<cLigne*>	_LignesInterdites;	//!< Lignes ne devant pas �tre affich�es
	set<LogicalPlace*>		_DestinationsAffichees;
	//@}

	//!	\name M�thodes prot�g�es
	//@{
	cMoment	_MomentFin(const cMoment& __MomentDebut)			const;
	bool	_LigneAutorisee(const cGareLigne*)					const;
	//@}

public:
	//!	\name Constructeur et destructeur
	//@{
	cTableauAffichage(const std::string& __Code);
	~cTableauAffichage(void);
	//@}

	//!	\name Modificateurs
	//@{
	virtual bool	SetArretLogique(LogicalPlace* const __ArretLogique);
	bool			SetNombreDeparts(tIndex __NombreDeparts);
	void			AddLigneInterdte(cLigne* __Ligne);
	void			AddArretPhysiqueAutorise(tIndex __NumeroArretPhysique);
	bool			SetTitre(const cTexte&);
	bool			SetOriginesSeulement(bool __Valeur);
	void			AddDestinationAffichee(LogicalPlace* __ArretLogique);
	void			SetNumeroPanneau(tIndex);
	//@}

	//!	\name Accesseurs
	//@{
	const cTexte&	getTitre()			const;
	tIndex			getNumeroPanneau()	const;
	//@}

	//!	\name Calculateurs
	//@{
	virtual cDescriptionPassage*	Calcule(const cMoment& __MomentDebut)			const;
	void							ListeArretsAffiches(cDescriptionPassage* __DP)	const;
	//@}

};

/** @} */

#endif
