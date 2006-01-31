inline const cAlerte* cArretLogique::getAlerte() const
{
	return(&vAlerte);
}



/*!	\brief Contr�le du num�ro de d�signation de l'arr�t
	\param n Num�ro de d�signation � contr�ler
	\return true si le num�ro de la d�signation envoie vers une d�signation existante
	\author Hugues Romain
	\date 2000-2005
*/
/*inline bool cArretLogique::ControleNumeroDesignation(int n) const
{
	return n >= 0 && n < _NombreAccesPADe && _AccesPADe[n] != NULL;
}
*/
inline const cTexte& cArretLogique::GetNom(int i) const
{
	if (!_AccesPADe[i])
		i=0;
	return(getNom(i));
}

inline cCommune* cArretLogique::GetCommune(int i) const
{
	if (!_AccesPADe[i])
		i=0;
	return(getCommune(i));
}

inline const cPoint& cArretLogique::getPoint() const
{
	return vPoint;
}

inline cArretPhysique* cArretLogique::GetArretPhysique(int n) const
{
	if (n < 1 || n > NombreArretPhysiques())
		return NULL;
	else
		return getArretPhysique(n);
}



inline cArretPhysique* cArretLogique::getArretPhysique(tIndex n) const
{
	return vArretPhysique[n];
}




inline cAccesPADe* cArretLogique::GetAccesPADe(int i) const
{
	if (!_AccesPADe.IndexValide(i) || !_AccesPADe[i])
		return NULL;
	return getAccesPADe(i);
}

inline cAccesPADe* cArretLogique::getAccesPADe(tIndex i) const
{
	return _AccesPADe[i];
}

inline cGareLigne* cArretLogique::PremiereGareLigneDep() const
{
	return vPremiereGareLigneDep;
}

inline cGareLigne* cArretLogique::PremiereGareLigneArr() const
{
	return(vPremiereGareLigneArr);
}

inline tIndex cArretLogique::Index() const
{
	return(_Index);
}


inline void cArretLogique::setPremiereGareLigneDep(cGareLigne* curGareLigne)
{
	vPremiereGareLigneDep = curGareLigne;
}

inline void cArretLogique::setPremiereGareLigneArr(cGareLigne* curGareLigne)
{
	vPremiereGareLigneArr = curGareLigne;
}
	
inline const cTexte& cArretLogique::getDesignationOD() const
{
	return(vDesignationOD);
}



inline tNiveauCorrespondance cArretLogique::CorrespondanceAutorisee() const
{
	return(vCorrespondanceAutorisee);
}

inline cDureeEnMinutes cArretLogique::AttenteCorrespondance(tIndex Dep, tIndex Arr) const
{
	if (vCorrespondanceAutorisee)
		return(vTableauAttente[Dep][Arr]);
	else
		return(cDureeEnMinutes(0));
}

inline const cDureeEnMinutes& cArretLogique::PireAttente(tIndex i) const
{
	if (i<1 || i> NombreArretPhysiques())
		i=0;
	return(vPireAttente[i]);
}

inline bool cArretLogique::declServices(size_t newNombreServices)
{
	vService = (cServiceEnGare**) calloc(newNombreServices+1, sizeof(cPhoto));
	return(true);
}



inline bool cArretLogique::addPhoto(cPhoto* Photo)
{
	// SET PORTAGE LINUX
	size_t i;
	for (i=0; vPhoto[i]!=NULL; i++)
	{ }
	//END PORTAGE LINUX
	vPhoto[i] = Photo;
	return(true);

}


inline const cDureeEnMinutes& cArretLogique::AttenteMinimale() const
{
	return(vAttenteMinimale);
}

inline tVitesseKMH cArretLogique::vitesseMax(size_t Categorie) const
{
	return(vVitesseMax[Categorie]);
}

inline tIndex	cArretLogique::NombrePhotos() const
{
	return(vNombrePhotos);
}
	
inline const cPhoto* cArretLogique::getPhoto(tIndex iNumeroPhoto) const
{
	return(vPhoto[iNumeroPhoto]);
}

inline cServiceEnGare* cArretLogique::GetService(int i) const
{
	if (vService)
		return(vService[i]);
	else
		return(NULL);
}

inline cPhoto* cServiceEnGare::getPhoto() const
{
	return(vPhoto);
}

inline const cTexte& cServiceEnGare::Designation() const
{
	return(vDesignation);
}

template <class T>
inline T& operator<<(T& flux, const cArretLogique& Obj)
{
	flux << *Obj.getAccesPADe();
	return flux;
}

inline cTableauDynamique<cAccesPADe*>& cArretLogique::TableauAccesPADe()
{
	return _AccesPADe;
}
