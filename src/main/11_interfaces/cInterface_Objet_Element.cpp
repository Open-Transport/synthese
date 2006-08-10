
#include "cInterface_Objet_Element.h"



/*! \brief Accesseur élément suivant dans la chaîne
  \return Pointeur vers l'élément suivant dans la chaîne
  \author Hugues Romain
  \date 2000-2005
*/
const cInterface_Objet_Element* cInterface_Objet_Element::getSuivant() const
{
    return _Suivant;
}



/*! \brief Modificateur élément suivant dans la chaîne
  \return 
  \author Hugues Romain
  \date 2000-2005
*/
void cInterface_Objet_Element::setSuivant( cInterface_Objet_Element* __Objet )
{
    _Suivant = __Objet;
}


cInterface_Objet_Element* cInterface_Objet_Element::Suivant()
{
    return _Suivant;
}


/*! \brief Constructeur
  \author Hugues Romain
  \date 2000-2005
 
  Assure le chaï¿½nage correct d'un ï¿½lï¿½ment unique
*/
cInterface_Objet_Element::cInterface_Objet_Element()
{
    _Suivant = NULL;
}




/*! \brief Destructeur
 
Supprime l'ensemble des objets de la chaï¿½ne
*/
cInterface_Objet_Element::~cInterface_Objet_Element()
{
    delete _Suivant;
}




