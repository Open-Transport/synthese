/*!	\file cEnvironnement.inline.h
	\brief Fonctions inline classe cEnvironnement
*/




inline cJC* cEnvironnement::getJC(tIndex n) const
{
	return _JC[n];
}

inline tAnnee cEnvironnement::NombreAnnees() const
{
	return(vDerniereAnnee - vPremiereAnnee);
}

inline tAnnee cEnvironnement::NombreAnnees(tAnnee AutreAnnee) const
{
	return(AutreAnnee - vPremiereAnnee);
}

inline tAnnee cEnvironnement::PremiereAnnee() const
{
	return(vPremiereAnnee);
}

inline tAnnee cEnvironnement::DerniereAnnee() const
{
	return(vDerniereAnnee);
}

inline cLigne* cEnvironnement::PremiereLigne() const
{
	return(vPremiereLigne);
}



inline cDocument* cEnvironnement::GetDocument(tIndex n) const
{
	if (n >= 0 && n < _Documents.Taille())
		return(_Documents[n]);
	else
		return(NULL);
}


inline const cTexte& cEnvironnement::getNomRepertoireHoraires() const
{
	return(vNomRepertoireHoraires);
}


inline cModaliteReservation* cEnvironnement::getResa(tIndex n) const
{
	return vResa.at (n);
}


inline cReseau* cEnvironnement::getReseau(tIndex n) const
{
	return vReseau.at (n);
}



inline cVelo* cEnvironnement::getVelo(tIndex n) const
{
	return(vVelo.at (n));
}

inline cHandicape* cEnvironnement::getHandicape(tIndex n) const
{
	return(vHandicape. at (n));
}


inline cTarif* cEnvironnement::getTarif(tNumeroTarif n) const
{
	return vTarif.at (n);
}


inline bool cEnvironnement::isTarif(tNumeroTarif n) const
{
	if (n>=-1 && n<vNombreTarif)
		return(true);
		
	return false;
}

inline tNumeroTarif cEnvironnement::getNombreTarif() const
{
	return (vNombreTarif);
}

inline const cDate& cEnvironnement::DateMinReelle() const
{
	return (vDateMin);
}

inline const cDate& cEnvironnement::DateMaxReelle() const
{
	return (vDateMax);
}



/*!	\brief Modificateur de la premi�re date o� circule au moins un service de l'environnement
	\param newDate Date de circulation d'un service � prendre en compte
	\author Hugues Romain
	\date 2005
	
Cette m�thode met � jour la premi�re date o� circule au moins un service de l'environnement, si la date fournie est ant�rieure � la premi�re date connue.
*/
inline void cEnvironnement::SetDateMinReelle(const cDate& newDate)
{
	if (newDate < vDateMin)
		vDateMin = newDate;
}



/*!	\brief Modificateur de la derni�re date o� circule au moins un service de l'environnement
	\param newDate Date de circulation d'un service � prendre en compte
	\author Hugues Romain
	\date 2005
	
Cette m�thode met � jour la derni�re date o� circule au moins un service de l'environnement, si la date fournie est post�rieure � la derni�re date connue.
*/inline void cEnvironnement::SetDateMaxReelle(const cDate& newDate)
{
	if (newDate > vDateMax)
		vDateMax = newDate;
}



/*!	\brief Modificateur Index
*/
inline bool cEnvironnement::SetIndex(tIndex __Valeur)
{
	Code = __Valeur;
	return true;
}



/*!	\brief Accesseur Index
	\return L'index de l'environnement dans la base SYNTHESE
	\author Hugues Romain
	\date 2005
*/
inline tIndex cEnvironnement::Index() const
{
	return Code;
}


inline void cEnvironnement::SetDatesService(tAnnee __AnneeMin, tAnnee __AnneeMax)
{
	vPremiereAnnee = __AnneeMin;
	vDerniereAnnee = __AnneeMax;
}


inline bool cEnvironnement::ControleDate(const cDate& __Date) const
{
	return vDateMin <= __Date && __Date <= vDateMax;
}

inline cTableauDynamique<cJC*>& cEnvironnement::TableauJC()
{
	return _JC;
}
