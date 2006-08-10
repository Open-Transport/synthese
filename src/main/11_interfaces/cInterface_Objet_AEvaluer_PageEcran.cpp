
#include "cInterface_Objet_AEvaluer_PageEcran.h"
#include "cInterface_Objet_Element.h"


/*! \brief Constitution d'un objet d'interface définissant tout ou partie d'une page d'écran, par inclusion d'une copie des éléments d'un autre objet de même type
 \author Hugues Romain
 \date 2005
 \param __Objet Objet à copier
 \param __Parametres Parametres de la copie
 \return true si la copie a été effectuée avec succès, false sinon.
 
L'ensemble des éléments de l'objet à inclure sont copiés dans l'objet courant.
Selon le type d'élément à inclure, la copie peut tenir compte des paramètres de la copie :
 - Pour un élément connu, les paramètres sont ignorés
 - Pour un élément de type paramètre à évaluer à l'éxécution, l'objet copié est puisé de l'objet paramètres de la copie : l'élément du rang de l'index fourni dans l'objet inclu est recopié.
   
*/
bool cInterface_Objet_AEvaluer_PageEcran::InclureCopie( const cInterface_Objet_AEvaluer_PageEcran& __Objet, const cInterface_Objet_AEvaluer_ListeParametres& __Parametres )
{
    // Parcours des éléments de l'objet à copier
    for ( const cInterface_Objet_Element * __Element = __Objet._PremierElement; __Element != NULL; __Element = __Element->getSuivant() )
        if ( !_Chaine( __Element->Copie( __Parametres ) ) )
            return false;

    // Sortie succès
    return true;
}

