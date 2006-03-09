
#include "cInterface_Objet_Element_Goto.h"

int cInterface_Objet_Element_Goto::Evalue( std::ostream&, const cInterface_Objet_Connu_ListeParametres&, const void* ObjetAAfficher, const cSite* __Site ) const
{
    return _NumeroLigne;
}




/*! \brief Copie de l'élément
 \param __Parametres La chaine de paramètres de l'objet d'interface destiné à recevoir le nouvel élément (inutilisé)
 \return Element d'interface à ajouter à l'objet d'interface
 \author Hugues Romain
 \date 2005
*/
cInterface_Objet_Element* cInterface_Objet_Element_Goto::Copie( const cInterface_Objet_AEvaluer_ListeParametres& __Parametres ) const
{
    return new cInterface_Objet_Element_Goto( _NumeroLigne );
}

cInterface_Objet_Element_Goto::cInterface_Objet_Element_Goto( int __NumeroLigne ) : cInterface_Objet_Element()
{
    _NumeroLigne = __NumeroLigne;
}

