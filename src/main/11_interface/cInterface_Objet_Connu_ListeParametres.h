#ifndef SYNTHESE_CINTERFACEOBJETCONNULISTEPARAMETRES_H
#define SYNTHESE_CINTERFACEOBJETCONNULISTEPARAMETRES_H

class cSite;

#include <iostream>


/** @addtogroup m11
 @{
*/


/*! \brief Objet d'interface contenant une liste d'�l�ments destin�s � �tre fournis ult�rieurement en param�tre d'un autre objet, dont le contenu des �l�ments est connu
 \author Hugues Romain
 \date 2005
 
L'usage de cette classe garantit que tous ses �l�ments constituants peuvent �tre �valu�s sans autre besoin d'informations.
 
Ainsi, les �l�ments constituant les liste de param�tres connus sont n�c�ssairement des constantes texte ou num�riques (recod�es en texte)
 
Pour construire un objet de ce type, les �l�ments sont successivements encha�n�s gr�ce � l'utilisation des deux op�rateurs << pouvant fabriquer des �l�ments constants � partir de texte ou de nombre entier.
 
*/
class cInterface_Objet_Connu_ListeParametres : public cInterface_Objet
{
    public:
        //! \name Modificateurs
        //@{
        cInterface_Objet_Connu_ListeParametres& operator<<( const std::string& );
        cInterface_Objet_Connu_ListeParametres& operator<<( int );
        //@}

        //! \name Accesseurs
        //@{
        const cInterface_Objet_Element_Parametre* operator[] ( int ) const;
        //@}

};










#endif

