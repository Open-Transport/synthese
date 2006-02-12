
#ifndef SYNTHESE_CTABLEAUAFFICHAGE_H
#define SYNTHESE_CTABLEAUAFFICHAGE_H

class cTableauAffichage;

#include "cSite.h"
#include "Parametres.h"
#include "cDescriptionPassage.h"
#include "LogicalPlace.h"
#include "cArretPhysique.h"
#include "cLigne.h"
#include "Temps.h"

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
	const LogicalPlace*						_ArretLogique;		//!< Point d'arr�t affich�
	cTableauDynamique<const cArretPhysique*>		_ArretPhysiques;				//!< ArretPhysique(s) affich�s
	cTableauDynamique<const cLigne*>	_LignesInterdites;	//!< Lignes ne devant pas �tre affich�es
	cTableauDynamique<const LogicalPlace*>		_DestinationsAffichees;
	//@}

	//!	\name M�thodes prot�g�es
	//@{
	cMoment	_MomentFin(const cMoment& __MomentDebut)			const;
	bool	_LigneAutorisee(const cGareLigne*)					const;
	//@}

public:
	//!	\name Constructeur et destructeur
	//@{
	cTableauAffichage(const cTexte& __Code);
	~cTableauAffichage(void);
	//@}

	//!	\name Modificateurs
	//@{
	virtual bool	SetArretLogique(const LogicalPlace* __ArretLogique);
	bool			SetNombreDeparts(tIndex __NombreDeparts);
	bool			AddLigneInterdte(const cLigne* __Ligne);
	bool			AddArretPhysiqueAutorise(tIndex __NumeroArretPhysique);
	bool			SetTitre(const cTexte&);
	bool			SetOriginesSeulement(bool __Valeur);
	tIndex			AddDestinationAffichee(const LogicalPlace* __ArretLogique);
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