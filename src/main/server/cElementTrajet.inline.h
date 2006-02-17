inline void cElementTrajet::setService(const tNumeroService newVal)
{
	vNumeroService = newVal;
}

inline tNumeroService cElementTrajet::getService() const
{
	return(vNumeroService); 
}

inline tDistanceHM cElementTrajet::DistanceParcourue() const
{
	return(vDistanceParcourue);
}

inline const cElementTrajet* cElementTrajet::Suivant() const
{
	return vSuivant;
}

inline cElementTrajet* cElementTrajet::getSuivant()
{
	return vSuivant;
}


inline const cLigne* cElementTrajet::getLigne() const
{
	return vLigne;
}

inline const tDureeEnMinutes& cElementTrajet::AmplitudeServiceContinu() const
{
	return(vAmplitudeServiceContinu);
}



inline void cElementTrajet::setMomentArrivee(const cMoment &newVal)
{
	vMomentArrivee = newVal;
}

inline void cElementTrajet::setMomentDepart(const cMoment &newVal)
{
	vMomentDepart = newVal;
}

inline void cElementTrajet::setAmplitudeServiceContinu(const tDureeEnMinutes& newVal)
{
	vAmplitudeServiceContinu = newVal;
}

inline void cElementTrajet::setSuivant(cElementTrajet *newVal)
{
	if (newVal != NULL)
		newVal->_Precedent = this;
	vSuivant = newVal;
}

inline const cDistanceCarree& cElementTrajet::DistanceCarreeObjectif() const
{
	return vDistanceCarreeObjectif;
}

inline cDistanceCarree& cElementTrajet::getDistanceCarreeObjectif()
{
	return vDistanceCarreeObjectif;
}


inline void cElementTrajet::setDistanceCarreeObjectif(const cDistanceCarree& newVal)
{
	vDistanceCarreeObjectif = newVal;
}


inline void cElementTrajet::setLigne(const cLigne *newVal)
{
	vLigne = newVal;
}

inline const tDureeEnMinutes& cElementTrajet::DureeEnMinutesRoulee() const
{
	return(vDureeEnMinutesRoulee);
}

inline const cMoment& cElementTrajet::MomentDepart() const
{
	return(vMomentDepart);
}

inline const cMoment& cElementTrajet::MomentArrivee() const
{
	return(vMomentArrivee);
}

inline void cElementTrajet::CalculeDureeEnMinutesRoulee()
{
	vDureeEnMinutesRoulee = vMomentArrivee - vMomentDepart;
}


inline tTypeElementTrajet cElementTrajet::Type() const
{
	return(vType);
}

inline void cElementTrajet::setType(tTypeElementTrajet newVal)
{
	vType = newVal;
}


inline void cElementTrajet::setDureeEnMinutesRoulee(const tDureeEnMinutes& newVal)
{
	vDureeEnMinutesRoulee = newVal;
}



/*!	\brief Affichage standard d'un trajet
	\author Hugues Romain
	\date 2005
	\param flux Flux sur lequel afficher le trajet
	\param Obj Le trajet � afficher
	\return Le flux sur lequel le trajet a �t� affich�
	\todo A SUPPRIMER A METTRE DANS UNE INTERFACE TEXTE
*/
/*
template <class T>
inline T& operator<<(T& flux, const cElementTrajet& Obj)
{
	flux << "---------------TRAJET---------------\n";

	flux << "Duree: " << (Obj.Dernier()->MomentArrivee() - Obj.MomentDepart()) << "\n";
	flux << "Amplitude Service Continu: " << Obj.AmplitudeServiceContinu() << "\n";
		
	for (const cElementTrajet* TET = &Obj; TET != NULL; TET = TET->Suivant())
	{
		flux << "Depart  " << TET->getGareDepart() << " (" << TET->MomentDepart() << ")\n";
		flux << "     ligne ";
		TET->getLigne()->LibelleSimple(flux);
		flux << endl;
		flux << "Arriv�e " << 	TET->getGareArrivee() << " (" << TET->MomentArrivee() << ")\n";
	}
	return flux;
}
*/


/*!	\brief Copie du contenu d'un �l�ment de trajet dans l'objet � l'exception du chainage
	\param Op2 L'�l�ment � copier
	\return pointeur sur l'objet (� modifier par une r�f�rence �ventuellement)
	\author Hugues Romain
	\date 2001-2005
*/
inline cElementTrajet* cElementTrajet::operator = (const cElementTrajet& Op2)
{
	vAmplitudeServiceContinu= Op2.vAmplitudeServiceContinu;
	_origin = Op2._origin;
	vMomentDepart			= Op2.vMomentDepart;
	_destination	= Op2._destination;
	vMomentArrivee			= Op2.vMomentArrivee;
	vLigne					= Op2.vLigne;
	vNumeroService			= Op2.vNumeroService;
	vDistanceCarreeObjectif	= Op2.vDistanceCarreeObjectif;
	vDureeEnMinutesRoulee	= Op2.vDureeEnMinutesRoulee;
	return this;
}


inline const cElementTrajet* cElementTrajet::Precedent() const
{
	return _Precedent;
}
