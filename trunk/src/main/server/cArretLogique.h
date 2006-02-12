/*! \file cArretLogique.h
	\brief En-t�te classes points d'arr�t
*/

#ifndef SYNTHESE_CPOINTARRET_H
#define SYNTHESE_CPOINTARRET_H

/**
	@ingroup m05
*/
enum tNiveauCorrespondance
{
	CorrInterdite = 0,
	CorrAutorisee = 1,
	CorrRecommandeeCourt = 2,
	CorrRecommandee = 3
};
class cArretLogique;
class cServiceEnGare;
class cLieuLogique;

#include "cAccesPADe.h"
#include "cAlerte.h"
#include "Point.h"
#include "cTexte.h"
#include "Temps.h"
#include "cArretPhysique.h"
#include "cPhoto.h"
#include "Parametres.h"
#include "cGareLigne.h"
#include "cDistanceCarree.h"
#include <vector>

#define POINTS_ARRETS_NOMBRE_DESIGNATIONS_DEFAUT 5



/** Arrêts logiques.
	@ingroup m05
	@author Hugues Romain
	@date 2000-2006
*/
class cArretLogique
{
	//! \name D�signations et localisation
	//@{
	//cTableauDynamique<cAccesPADe*>	_AccesPADe;		//!< Tableau des d�signations du point d'arr�t
	vector<cLieuLogique*>			_Designations;		//!< Tableau des désignations du point d'arrêt
	cPoint							vPoint;				//!< Point de g�olocalisation
    cTexte                          _Designation13;		//!< D�signation de 13 caract�res de long
    cTexte                          _Designation26;		//!< D�signation de 26 caract�res de long
	//@}
	
	//! \name Documentation
	//@{
	cAlerte					vAlerte;			//!< Alerte en cours de validit�
	cTextePostScript		vDesignationOD;		//!< D�signation pour affichage en tant que destination ou origine d'un service (indicateurs)
	cTexteHTML				vHTML;				//!< Description HTML (inutilis� : A SUPPRIMER ?)
	//@}
	
	//! \name Gestion des correspondances
	//@{
	cDureeEnMinutes			vAttenteMinimale;			//!< D�lai minimal de correspondance entre quais le plus faible du point d'arr�t
	tNiveauCorrespondance	vCorrespondanceAutorisee;	//!< Type d'autorisation de correspondance
	cDureeEnMinutes*		vPireAttente;				//!< Tableau des plus longs d�lais minimaux de correspondance au d�part de chaque quai
	cDureeEnMinutes**		vTableauAttente;			//!< Tableau des d�lais minimaux de correspondance entre quais
	//@}
	
	//! \name ArretPhysiques
	//@{
	cTableauDynamiquePointeurs<cArretPhysique*>		vArretPhysique;						//!< Tableau des quais du point d'arr�t
	//@}
	
	//! \name Donn�es compl�mentaires
	//@{
	tIndex				vNombrePhotos;			//!< Nombre de photos
	cPhoto**			vPhoto;				//!< Tableau des photos du point d'arr�t hors photos des quais
	cServiceEnGare**	vService;				//!< Tableau des services en gare
	tVitesseKMH			vVitesseMax[NOMBREVMAX];	//!< Tableau des vitesses maximales par tranche de distance (temporairement inutilis�)
	//@}

	//! \name Chainage et indexation
	//@{
	tIndex			_Index;				//!< Index du point d'arr�t dans l'environnement
	cGareLigne*		vPremiereGareLigneDep;	//!< Pointeur vers le premier d�part de ligne (ordre al�atoire) \todo trier par orientation ?
	cGareLigne*		vPremiereGareLigneArr;	//!< Pointeur vers la premi�re arriv�e de ligne (ordre al�atoire) \todo trier par orientation ?
	//@}

public:
	//!	\name Accesseurs tableaux avec droit de modification
	//@{
	cTableauDynamique<cAccesPADe*>&	TableauAccesPADe();
	//@}
	
	//! \name Accesseurs
	//@{
	cDureeEnMinutes			AttenteCorrespondance(tIndex Dep, tIndex Arr)			const;
	const cDureeEnMinutes&	AttenteMinimale()										const;
	tNiveauCorrespondance	CorrespondanceAutorisee()								const;
	cAccesPADe*				GetAccesPADe(int i)										const;
	cAccesPADe*				getAccesPADe(tIndex i=0)								const;
	const cAlerte*			getAlerte() 											const;
	cCommune*				GetCommune(int i)										const;
	cCommune*				getCommune(tIndex i=0)									const;
	const cTexte&			getDesignationOD()										const;
	const cTexte&			GetNom(int i)											const;
	const cTexte&			getNom(tIndex i=0)										const;
	const cPhoto*			getPhoto(tIndex)										const;
	const cPoint&			getPoint()												const;
	cArretPhysique*					GetArretPhysique(int)											const;
	cArretPhysique*					getArretPhysique(tIndex)											const;
	cServiceEnGare*			GetService(int i)										const;
    const cTexte&           getDesignation13()                                      const;
    const cTexte&           getDesignation26()                                      const;
    
	tIndex					Index()													const;
	tIndex					NombrePhotos()											const;
	tIndex					NombreArretPhysiques()											const;
	const cDureeEnMinutes&	PireAttente(tIndex i)									const;
	cGareLigne*				PremiereGareLigneArr()									const;
	cGareLigne*				PremiereGareLigneDep()									const;
	tVitesseKMH				vitesseMax(size_t Categorie)							const;
	//@}
	
	//! \name Sorties
	//@{
/*	template <class T> T& toXML(T& Objet, cMoment debutArret, cMoment finArret, bool complet=true,tIndex IndexDesignation=0) const
	{
		if (!ControleNumeroDesignation(IndexDesignation))
			IndexDesignation=0;
		
		//ajout balise point arret
		Objet << "\n<pointarret numero=\"" << Index() << "\" ";
		Objet << "x=\""<< vPoint.XKMM() << "\" y=\""<< vPoint.YKMM() << "\">";
		
		//ajout balise commune
		getCommune(IndexDesignation)->toXML(Objet);
		
		//ajout balise nom
		Objet << "<nom>" << getNom(IndexDesignation)<< "</nom>";
		
		//ajout warnings si existe
		if (complet && this->getAlerte()->showMessage(debutArret, finArret))
		{
			Objet << "<warnings>";
			getAlerte()->toXML(Objet);
			Objet << "</warnings>";
		}
		
		//fermeture balise
		Objet << "</pointarret>";
		
		return(Objet);
	}*/
	//@}
	
	//! \name Calculateurs
	//@{	
//	cElementTrajet*			ProchainDirect(cArretLogique* Destination, cMoment& MomentDepart, const cMoment& ArriveeMax
//								, tNumeroVoie ArretPhysiqueArriveePrecedente)										const;
	cMoment					MomentArriveePrecedente (const cMoment& MomentArrivee, const cMoment& MomentArriveeMin)		const;
	cMoment					MomentDepartSuivant(const cMoment& MomentDepart, const cMoment& MomentDepartMax
												, const cMoment& __MomentCalcul)										const;
	cGareLigne*				DessertAuDepart	(const cLigne*)																const;
	cGareLigne*				DessertALArrivee				(const cLigne*)												const;
	tNiveauCorrespondance	NiveauCorrespondance			(const cDistanceCarree& D)									const;
	//@}

	//! \name Modificateurs
	//@{
	void	setAlerteDebut(cMoment& momentDebut);
	void	setAlerteFin(cMoment& momentFin);
	void	setAlerteMessage(cTexte& message);
	bool	SetPoint(int X, int Y);
	void	setPremiereGareLigneDep(cGareLigne*);
	void	setPremiereGareLigneArr(cGareLigne*);
	void	setNbArretPhysiques(tIndex);
	bool	AddArretPhysique(tIndex, cArretPhysique*);
	bool	setDelaiCorrespondance(tIndex __VoieDepart, tIndex __VoieArrivee, tDureeEnMinutes);
	void	setDesignationOD(const cTexte&);
	bool	setVMax(tCategorieDistance, tVitesseKMH);
	bool	addPhoto(cPhoto*);
	bool	declServices(size_t);
	bool	addService(char newType, cPhoto* newPhoto, const cTexte& newDesignation);
	bool	Ferme();
    bool    setDesignation13(const cTexte&);
    bool    setDesignation26(const cTexte&);
    
	//@}

	//! \name Constructeur et destructeur
	//@{
	cArretLogique(tIndex, tNiveauCorrespondance newNiveauCorrespondance);
	~cArretLogique();
	//@}
};

/** Service disponible aux abords du point d'arr�t
	@ingroup m05
*/
class cServiceEnGare
{
	char		vType;
	cPhoto*	vPhoto;
	cTexte	vDesignation;

public:
	//! \name Accesseurs
	//@{
	const cTexte&	Designation()	const;
	cPhoto*		getPhoto()	const;
	//@}

	//! \name Constructeur
	//@{
	cServiceEnGare(char newType, cPhoto* newPhoto, const cTexte& newDesignation);
	//@}
};

#define SYNTHESE_CPOINTARRET_CLASSE

#include "cArretLogique.inline.h"
#include "cArretLogiqueAccesPADe.inline.h"

#endif

