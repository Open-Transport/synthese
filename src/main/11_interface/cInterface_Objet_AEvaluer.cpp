
#include "cInterface_Objet_AEvaluer.h"



/*! \brief Ajout d'un élément en fin de chaîne
  \param __Objet L'élément à ajouter
  \return L'objet après avoir été modifié
  \author Hugues Romain
  \date 2005
         
  Cette implémentation sert à autoriser l'usage de cObjetInterface::_Chaine sur tout type d'élément, dans le cas où l'objet impacté est bien un objet permettant les éléments non connus.
*/
cInterface_Objet_Element* cInterface_Objet_AEvaluer::Chaine( cInterface_Objet_Element* __Objet )
{
    return ( cInterface_Objet_Element* ) _Chaine( __Objet );
}



const cInterface_Objet_Element_Parametre_TexteConnu* cInterface_Objet_AEvaluer::operator[] ( int __Index ) const
{
    return ( cInterface_Objet_Element_Parametre_TexteConnu* ) _getElement( __Index );
}
