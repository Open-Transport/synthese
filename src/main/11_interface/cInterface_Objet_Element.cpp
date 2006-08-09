
#include "cInterface_Objet_Element.h"



/*! \brief Accesseur �l�ment suivant dans la cha�ne
  \return Pointeur vers l'�l�ment suivant dans la cha�ne
  \author Hugues Romain
  \date 2000-2005
*/
const cInterface_Objet_Element* cInterface_Objet_Element::getSuivant() const
{
    return _Suivant;
}



/*! \brief Modificateur �l�ment suivant dans la cha�ne
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
 
  Assure le cha�nage correct d'un �l�ment unique
*/
cInterface_Objet_Element::cInterface_Objet_Element()
{
    _Suivant = NULL;
}




/*! \brief Destructeur
 
Supprime l'ensemble des objets de la cha�ne
*/
cInterface_Objet_Element::~cInterface_Objet_Element()
{
    delete _Suivant;
}




