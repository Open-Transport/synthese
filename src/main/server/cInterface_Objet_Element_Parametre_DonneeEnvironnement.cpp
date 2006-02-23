
#include "cInterface_Objet_Element_Parametre_DonneeEnvironnement.h"
#include "LogicalPlace.h"
#include "cCommune.h"
#include "cLigne.h"
#include "cTrajets.h"
#include "cMateriel.h"


/*!	\brief Constructeur
	\param __NumeroObjet Num�ro de l'objet texte dynamique qui sera �valu�
	\author Hugues Romain
	\date 2005
*/
cInterface_Objet_Element_Parametre_DonneeEnvironnement::cInterface_Objet_Element_Parametre_DonneeEnvironnement(tIndex __NumeroObjet) : cInterface_Objet_Element_Parametre()
{
	_NumeroObjet = __NumeroObjet;
}



/*!	\brief Copie exacte d'un �l�ment de texte dynamique
	\author Hugues Romain
	\date 2005
*/
cInterface_Objet_Element* cInterface_Objet_Element_Parametre_DonneeEnvironnement::Copie() const
{
	return new cInterface_Objet_Element_Parametre_DonneeEnvironnement(_NumeroObjet);
}

cInterface_Objet_Element* cInterface_Objet_Element_Parametre_DonneeEnvironnement::Copie(const cInterface_Objet_AEvaluer_ListeParametres& __Parametres) const
{
	return Copie();
}



int cInterface_Objet_Element_Parametre_DonneeEnvironnement::Nombre(const cInterface_Objet_Connu_ListeParametres& __Parametres, const void* __Objet) const
{
	return Texte(__Parametres, __Objet).GetNombre();
}



/*!	\brief Extraction d'une valeur texte depuis l'objet
	\param __Parametres Liste cha�n�e de param�tres correspondant aux besoins de l'�l�ment (voir \ref InterfaceAttribut )
	\param __Objet Pointeur vers l'objet duquel extraire l'attribut
	\return Valeur de l'attribut de l'objet fourni, point� par l'�l�ment, �ventuellement fonction de param�tres fournis
	\author Hugues Romain
	\date 2005
*/
const cTexte& cInterface_Objet_Element_Parametre_DonneeEnvironnement::Texte(const cInterface_Objet_Connu_ListeParametres& __Parametres, const void* __Objet) const
{
	switch (_NumeroObjet)
	{
	case EI_ATTRIBUT_ArretLogique_Nom:
		return ((const LogicalPlace*) __Objet)->getName(); // getNom(__Parametres[EI_ATTRIBUT_ArretLogique_Parametre_IndexDesignation]->Nombre(__Parametres));

	case EI_ATTRIBUT_ArretLogique_NomCommune:
		return ((const LogicalPlace*) __Objet)->getTown()->getName(); // getCommune(__Parametres[EI_ATTRIBUT_ArretLogique_Parametre_IndexDesignation]->Nombre(__Parametres))->GetNom();
	
	case EI_ATTRIBUT_Ligne_LibelleSimple:
		return ((const cLigne*) __Objet)->getLibelleSimple();
	
	case EI_ATTRIBUT_Ligne_Image:
		return ((const cLigne*) __Objet)->getImage();
		
	case EI_ATTRIBUT_Ligne_LibelleComplet:
		return ((const cLigne*) __Objet)->getLibelleComplet();
		
	case EI_ATTRIBUT_Ligne_Style:
		return ((const cLigne*) __Objet)->getStyle();

	case EI_ATTRIBUT_Ligne_ArticleMateriel:
		return ((const cLigne*) __Objet)->Materiel()->getArticle();

	case EI_ATTRIBUT_Ligne_LibelleMateriel:
		return ((const cLigne*) __Objet)->Materiel()->getLibelleSimple();
	
	case EI_ATTRIBUT_TRAJETS_TAILLE:
		return ((const cTrajets*) __Objet)->GetTailleTexte();

	case EI_ATTRIBUT_TRAJETS_AUMOINSUNEALERTE:
		return ((const cTrajets*) __Objet)->getAuMoinsUneAlerte();

	}

	return _TexteVide;
}
