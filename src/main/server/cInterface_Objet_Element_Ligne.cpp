

#include "cInterface_Objet_Element_Ligne.h"

tIndex cInterface_Objet_Element_Ligne::Evalue(ostream&, const cInterface_Objet_Connu_ListeParametres&, const void* ObjetAAfficher, const cSite* __Site) const
{
	return INCONNU;
}




/*!	\brief Copie de l'�l�ment
	\param __Parametres La chaine de param�tres de l'objet d'interface destin� � recevoir le nouvel �l�ment (inutilis�)
	\return Element d'interface � ajouter � l'objet d'interface
	\author Hugues Romain
	\date 2005
*/
cInterface_Objet_Element* cInterface_Objet_Element_Ligne::Copie(const cInterface_Objet_AEvaluer_ListeParametres& __Parametres) const
{
	return new cInterface_Objet_Element_Ligne(_NumeroLigne);
}

cInterface_Objet_Element_Ligne::cInterface_Objet_Element_Ligne(tIndex __NumeroLigne) : cInterface_Objet_Element()
{
	_NumeroLigne = __NumeroLigne;
}

tIndex cInterface_Objet_Element_Ligne::NumeroLigne() const
{
	return _NumeroLigne;
}

