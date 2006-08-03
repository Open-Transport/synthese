#ifndef SYNTHESE_CINTERFACEOBJETCONNULISTEPARAMETRES_H
#define SYNTHESE_CINTERFACEOBJETCONNULISTEPARAMETRES_H

class cSite;

#include <iostream>


/** @addtogroup m11
 @{
*/


/*! \brief Objet d'interface contenant une liste d'éléments destinés à être fournis ultérieurement en paramètre d'un autre objet, dont le contenu des éléments est connu
 \author Hugues Romain
 \date 2005
 
L'usage de cette classe garantit que tous ses éléments constituants peuvent être évalués sans autre besoin d'informations.
 
Ainsi, les éléments constituant les liste de paramètres connus sont nécéssairement des constantes texte ou numériques (recodées en texte)
 
Pour construire un objet de ce type, les éléments sont successivements enchaînés grâce à l'utilisation des deux opérateurs << pouvant fabriquer des éléments constants à partir de texte ou de nombre entier.
 
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

