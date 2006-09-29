#ifndef SYNTHESE_CINTERFACE_OBJET_AEVALUER_H
#define SYNTHESE_CINTERFACE_OBJET_AEVALUER_H

#include "cInterface_Objet.h"



class cInterface_Objet_Element;
class cInterface_Objet_Element_Parametre_TexteConnu;

#include <iostream>


/** @addtogroup m11
 @{
*/






/*! \brief Objets d'interface n�cessitant des informations compl�mentaires pour �tre �valu�
 \author Hugues Romain
 \date 2005
 
Cette classe sert � r�unir les sous-classes des types d'objet d'interface n�cessitant des informations compl�mentaires pour �tre �valu�s. A l'usage, elle n'est pas instanci�e directement.
 
Les informations compl�mentaires n�cessaires peuvent �tre les suivantes :
 - fourniture d'un objet liste de param�tres connus
 - fourniture d'un pointeur vers un objet de donn�es ou r�sultat d'un calcul
 
La fabrication des objets n�cessitant des informations compl�mentaires pour �tre �valu� s'op�re par le cha�nage d'�lements de type quelconque (connus ou � �valuer � l'aide des informations compl�mentaires) � l'aide de la m�thode cInterface_Objet_AEvaluer::Chaine. Les �l�ments de type non connu seront �valu�s lors de l'affichage.
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

