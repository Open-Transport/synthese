#ifndef SYNTHESE_CINTERFACE_OBJET_AEVALUER_H
#define SYNTHESE_CINTERFACE_OBJET_AEVALUER_H

#include "cInterface_Objet.h"



class cInterface_Objet_Element;
class cInterface_Objet_Element_Parametre_TexteConnu;

#include <iostream>


/** @addtogroup m11
 @{
*/






/*! \brief Objets d'interface nécessitant des informations complémentaires pour être évalué
 \author Hugues Romain
 \date 2005
 
Cette classe sert à réunir les sous-classes des types d'objet d'interface nécessitant des informations complémentaires pour être évalués. A l'usage, elle n'est pas instanciée directement.
 
Les informations complémentaires nécessaires peuvent être les suivantes :
 - fourniture d'un objet liste de paramètres connus
 - fourniture d'un pointeur vers un objet de données ou résultat d'un calcul
 
La fabrication des objets nécessitant des informations complémentaires pour être évalué s'opère par le chaînage d'élements de type quelconque (connus ou à évaluer à l'aide des informations complémentaires) à l'aide de la méthode cInterface_Objet_AEvaluer::Chaine. Les éléments de type non connu seront évalués lors de l'affichage.
*/
class cInterface_Objet_AEvaluer : public cInterface_Objet
{
    public:
        //! \name Modificateurs
        //@{
        cInterface_Objet_Element* Chaine( cInterface_Objet_Element* );
        //@}

        //! \name Accesseurs
        //@{
        const cInterface_Objet_Element_Parametre_TexteConnu* operator[] ( int ) const;
        //@}

        //! \name Constructeur et destructeur
        //@{
        cInterface_Objet_AEvaluer() : cInterface_Objet() {}

};


/** @} */

#endif

