
#ifdef SYNTHESE_CACCESPADE_CLASSE
#ifdef SYNTHESE_CGARELIGNE_CLASSE

#ifndef SYNTHESE_CGARELIGNEACCESPADE_H
#define SYNTHESE_CGARELIGNEACCESPADE_H

/*!	\brief Recherche de liaison directe vers un groupe d'arr�ts en empruntant la ligne
	\param __Destination Groupe d'arr�t � rejoindre
	\return Adresse de la GareLigne permettant d'arriver au plus t�t � destination en empruntant la ligne courante, � partir de l'arr�t courant. NULL si liaison impossible selon ces conditions.
	\author Hugues Romain
	\date 2005
*/
inline const cGareLigne* cGareLigne::getLiaisonDirecteVers(const cAccesPADe* __Destination) const
{
	// Initialisation � la gareligne en cours
	const cGareLigne* __GareLigne = this; 
	
	while (true)
	{
		// Avancement de noeud en noeud si la destination est forc�ment un noeud (plus rapide), avncement d'arr�t en arr�t sinon
		if(__Destination->TousArretsDeCorrespondance())
			__GareLigne = __GareLigne->getArriveeCorrespondanceSuivante();
		else
			__GareLigne = __GareLigne->getArriveeSuivante();
		
		// Sortie si fin de ligne ou si arr�t reli� faisant partie de la destination
		if (__GareLigne == NULL || __Destination->inclue(__GareLigne->ArretLogique()))
			break;
	}
	
	// Sortie
	return __GareLigne;
}



/*!	\brief Recherche de liaison directe depuis un groupe d'arr�ts en empruntant la ligne
	\param __Provenance Groupe d'arr�t depuis lequel rejoindre
	\return Adresse de la GareLigne permettant de partir au plus tard de la provenance en empruntant la ligne courante, vers l'arr�t courant. NULL si liaison impossible selon ces conditions.
	\author Hugues Romain
	\date 2005
*/
inline const cGareLigne* cGareLigne::getLiaisonDirecteDepuis(const cAccesPADe* __Provenance) const
{
	// Initialisation � la gareligne en cours
	const cGareLigne* __GareLigne = this; 
	
	while (true)
	{
		// Avancement de noeud en noeud si la destination est forc�ment un noeud (plus rapide), avncement d'arr�t en arr�t sinon
		if(__Provenance->TousArretsDeCorrespondance())
			__GareLigne = __GareLigne->getDepartCorrespondancePrecedent();
		else
			__GareLigne = __GareLigne->getDepartPrecedent();
		
		// Sortie si fin de ligne ou si arr�t reli� faisant partie de la destination
		if (__GareLigne == NULL || __Provenance->inclue(__GareLigne->ArretLogique()))
			break;
	}
	
	// Sortie
	return __GareLigne;
}

#endif
#endif
#endif
