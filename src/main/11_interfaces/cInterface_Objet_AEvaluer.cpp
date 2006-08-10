
#include "cInterface_Objet_AEvaluer.h"



/*! \brief Ajout d'un �l�ment en fin de cha�ne
  \param __Objet L'�l�ment � ajouter
  \return L'objet apr�s avoir �t� modifi�
  \author Hugues Romain
  \date 2005
         
  Cette impl�mentation sert � autoriser l'usage de cObjetInterface::_Chaine sur tout type d'�l�ment, dans le cas o� l'objet impact� est bien un objet permettant les �l�ments non connus.
*/
cInterface_Objet_Element* cInterface_Objet_AEvaluer::Chaine( cInterface_Objet_Element* __Objet )
{
    return ( cInterface_Objet_Element* ) _Chaine( __Objet );
}



const cInterface_Objet_Element_Parametre_TexteConnu* cInterface_Objet_AEvaluer::operator[] ( int __Index ) const
{
    return ( cInterface_Objet_Element_Parametre_TexteConnu* ) _getElement( __Index );
}
