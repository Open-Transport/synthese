inline bool cLigne::SetAUtiliserDansCalculateur(bool __Valeur)
{
	_AUtiliserDansCalculateur = __Valeur;
	return true;
}

inline void cLigne::setImage(const cTexte& newImage)
{
	vImage.Vide();
	vImage << newImage;
}

inline void cLigne::setLibelleComplet(const cTexte& newLibelleComplet)
{
	vLibelleComplet.Vide();
	vLibelleComplet << newLibelleComplet;
}
	
inline void cLigne::setAAfficherSurTableauDeparts(bool newVal)
{
	vAAfficherSurTableauDeparts = newVal;
}

inline bool cLigne::AAfficherSurTableauDeparts() const
{
	return(vAAfficherSurTableauDeparts);
}

inline void cLigne::setResa(cModaliteReservation* newVal)
{
	vResa = newVal;
}

inline void cLigne::setVelo(cVelo* newVal)
{
	vVelo = newVal;
}

inline void cLigne::setGirouette(const cTexte& newGirouette)
{
	vGirouette.Vide();
	vGirouette << newGirouette;
}
	
// SET Gestion du filtre Velo
inline cVelo* cLigne::getVelo() const
{
	return(vVelo);
}

inline void cLigne::setHandicape(cHandicape* newVal)
{
	vHandicape = newVal;
}

inline cHandicape* cLigne::getHandicape() const
{
	return(vHandicape);
}

inline void cLigne::setTarif(cTarif* newVal)
{
	vTarif = newVal;
}

inline const cTarif* cLigne::getTarif() const
{
	return(vTarif);
}

inline void cLigne::setAlerteMessage(cTexte& message)
{
	vAlerte.setMessage(message);
}

inline void cLigne::setAlerteDebut(cMoment& momentDebut)
{
	vAlerte.setMomentDebut(momentDebut);
}

inline void cLigne::setAlerteFin(cMoment& momentFin)
{
	vAlerte.setMomentFin(momentFin);
}

inline const cAlerte* cLigne::getAlerte() const
{
	return(&vAlerte);
}

//END SET

inline void cLigne::setMateriel(cMateriel* newVal)
{
	vMateriel = newVal;
}

inline tNumeroService cLigne::NombreServices() const
{
	return(vNombreServices);
}

inline cGareLigne* cLigne::PremiereGareLigne() const
{
	return(vPremiereGareLigne);
}

inline void cLigne::setSuivant(cLigne* newVal)
{
	vSuivant = newVal;
}

inline cAxe* cLigne::Axe() const
{
	return(vAxe);
}

inline cMateriel* cLigne::Materiel() const
{
	return(vMateriel);
}

inline bool cLigne::EstUneRoute() const
{
	return(false);
}

inline cLigne* cLigne::Suivant() const
{
	return(vSuivant);
}

inline const cTexte& cLigne::getCode() const
{
	return(vCode);
}

inline const cTexte& cLigne::getNomPourIndicateur() const
{
	return(vNomPourIndicateur);
}

/*!	\brief Accesseur GareLigne Terminus
*/
inline cGareLigne* cLigne::DerniereGareLigne() const
{
	return(vDerniereGareLigne);
}

inline void cLigne::setStyle(const cTexte& newStyle)
{
	vStyle.Vide();
	vStyle << newStyle;
}

inline void cLigne::setLibelleSimple(const cTexte& newNom)
{
	vLibelleSimple.Vide();
	vLibelleSimple << newNom;
	if (!vNomPourIndicateur.Taille())
	{
		vNomPourIndicateur.Vide();
		vNomPourIndicateur << vLibelleSimple;
	}
}

inline void cLigne::setNomPourIndicateur(const cTexte& newNom)
{
	vNomPourIndicateur.Vide();
	vNomPourIndicateur << newNom;
}


inline void cLigne::setReseau(cReseau* newReseau)
{
	vReseau = newReseau;
}


inline cModaliteReservation* cLigne::GetResa() const
{
	return(vResa);
}

inline bool cLigne::AAfficherSurIndicateurs() const
{
	return(vAAfficherSurIndicateurs);
}

inline void cLigne::setAAfficherSurIndicateurs(bool newVal)
{
	vAAfficherSurIndicateurs = newVal;
}

#ifdef ClasseAdmin
inline void cLigne::setDepot(LogicalPlace* newVal)
{
	vDepot = newVal;
}

inline LogicalPlace* cLigne::getDepot() const
{
	return(vDepot);
}
#endif

inline cReseau* cLigne::getReseau() const
{
	return(vReseau);
}

/*!	\brief Accesseur libell� complet de la ligne
	\return Si un libell� complet est d�fini, renvoie le libell� complet de la ligne. Sinon renvoie le libell� simple de la ligne.
	\author Hugues Romain
	\date 2005
*/
inline const cTexte& cLigne::getLibelleComplet() const
{
	return vLibelleComplet;
}



/*!	\brief Accesseur libell� simple de la ligne
	\return Le libell� simple de la ligne.
	\author Hugues Romain
	\date 2005
*/
inline const cTexte& cLigne::getLibelleSimple() const
{
	return vLibelleSimple;
}



/*!	\brief Accesseur style CSS de la ligne
	\return R�f�rence constante vers le style de la ligne
	\author Hugues Romain
	\date 2005
*/
inline const cTexte& cLigne::getStyle() const
{
	return vStyle;
}



/*!	\brief Accesseur logo image de la ligne
	\return R�f�rence constante vers le chemin d'acc�s g�n�rique au logo image de la ligne
	\author Hugues Romain
	\date 2005
*/
inline const cTexte& cLigne::getImage() const
{
	return vImage;
}



/*!	\brief Code de la ligne sur nouvelle chaine de caract�res HTML
	\author Hugues Romain
	\date 2005
*/ /*
inline cTexte cLigne::Code() const
{
	cTexte tResultat;
	Code(tResultat);
	return tResultat;
} */


