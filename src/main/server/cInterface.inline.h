/*!	\file cInterface.inline.h
	\brief Fonctions inline des classes d'interface
*/



/*!	\brief Accesseur Index
	\return L'index de l'interface
	\author Hugues Romain
	\date 2005
*/
inline tIndex cInterface::Index() const
{
	return _Index;
}



/*!	\brief Modificateur index
	\param Valeur Valeur à donner à l'index
	\return true si l'opération a été effectuée avec succès
	\author Hugues Romain
	\date 2005
*/
inline bool cInterface::SetIndex(tIndex Valeur)
{
	_Index = Valeur;
	return true;
}




/*!	\brief Contrôle de la validité de l'objet
	\return true si l'objet est utilisable, false sinon
	\author Hugues Romain
	\date 2005

L'objet est valide si :
 - Au moins une période de la journée est décrite
*/
inline bool cInterface::OK() const
{
	return _Periode.size() > 0;
}



/*!	\brief Modificateur libellé jour de semaine
	\param __Index Index du jour (0=Dimanche, 1=Lundi, ..., 6=Samedi)
	\param __Libelle Libellé du jour de la semaine
	\return true si la modification a été effectuée avec succès
	\author Hugues Romain
	\date 2005
*/
inline bool cInterface::SetLibelleJour(tIndex __Index, const cTexte& __Libelle)
{
	if (__Index < 0 || __Index >= JOURS_PAR_SEMAINE)
		return false;
		
	_LibelleJourSemaine[__Index] = __Libelle;
	return true;
}



/*!	\brief Modificateur libellé mois
	\param __Index Index du mois
	\param __Libelle Libellé du mois
	\return true si la modification a été effectuée avec succès
	\author Hugues Romain
	\date 2005
*/
inline bool cInterface::SetLibelleMois(tIndex __Index, const cTexte& __Libelle)
{
	if (__Index < 0 || __Index > MOIS_PAR_AN)
		return false;
		
	_LibelleMois[__Index] = __Libelle;
	return true;
}


/*!	\brief Accesseur libellé de jour de la semaine
	\param __Index Index du jour (0=Dimanche, 1=Lundi, ..., 6=Samedi)
	\return Libellé du jour demandé
	\author Hugues Romain
	\date 2005
*//*
inline const cTexte& cInterface::LibelleJourSemaine(tIndex __Index) const
{
	return _LibelleJourSemaine[__Index];
}*/



