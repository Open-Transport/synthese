
#ifndef SYNTHESE_CTRAJETS_H
#define SYNTHESE_CTRAJETS_H

class cTrajets;

#include "cTrajet.h"
#include "cTableauDynamique.h"

#ifdef SYNTHESE_CTRAJET

/** Liste de trajets
	@ingroup m33
	\author Hugues Romain
	\date 2005
*/
class cTrajets : public cTableauDynamiqueObjets<cTrajet>
{
protected:
	cTexte							_txtTaille;					//!< Nombre de trajets contenus dans l'objet (format chaine de caract�res)
	cTexte							_AuMoinsUneAlerte;			//!< Indique si au moins un trajet de la liste comporte une alerte en activit�
	cTableauDynamique<const cArretLogique*>	_ListeOrdonneePointsArret;	//!< Liste ordonn�e des points d'arr�ts pour repr�sentation en fiche horaire
	cTableauDynamique<bool>			_LigneVerrouillee;			//!< Etat v�rouill� de la position de certaines lignes

	//! \name M�thodes de gestion de la repr�sentation en tableau
	//@{
	int				OrdrePAEchangeSiPossible(int PositionOrigine, int PositionSouhaitee);
	int				OrdrePAInsere(const cArretLogique* ArretLogique, tIndex Position);
	void			OrdrePAConstruitLignesAPermuter(const cTrajet& __TrajetATester, bool* Resultat, int LigneMax) const;
	bool			OrdrePARechercheGare(int& i, const cArretLogique* GareAChercher);
	void			GenererNiveauxEtAuMoinsUneAlerte();
	//@}

	//! \name M�thodes de finalisation
	//@{
	void			GenererListeOrdonneePointsArrets();
	//@}
	
public:
	//!	\name Modificateurs
	//@{
	void			Finalise();
	//@}
	
	//!	\name Accesseurs
	//@{
	const cArretLogique*	getListeOrdonneePointsArret(tIndex)	const;
 	int				TailleListeOrdonneePointsArret()	const;
	const cTexte&	GetTailleTexte()					const;
	const cTexte&	getAuMoinsUneAlerte()				const;
	//@}
	
	cTrajets() : cTableauDynamiqueObjets<cTrajet>() { }
	~cTrajets();
};

#define SYNTHESE_CTRAJETS

inline int cTrajets::TailleListeOrdonneePointsArret() const
{
	return _ListeOrdonneePointsArret.Taille();
}

inline const cArretLogique* cTrajets::getListeOrdonneePointsArret(tIndex __i) const
{
	return _ListeOrdonneePointsArret[__i];
}

inline const cTexte& cTrajets::GetTailleTexte() const
{
	return _txtTaille;
}

#endif
#endif
