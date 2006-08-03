#ifndef SYNTHESE_CINTERFACEOBJET_H
#define SYNTHESE_CINTERFACEOBJET_H

class cInterface_Objet;
class cInterface_Objet_Element;

class cSite;

#include <iostream>


/** @addtogroup m11
 @{
*/


/** Objet d'interface
 \author Hugues Romain
 \date 2005
 
Cette classe correspond à la description d'un objet pouvant être affiché dans l'interface qui le possède.
 
Un objet d'interface est décrit par une succession d'éléments (classes cInterface_Objet_Element et dérivées) devant être évalués successivement lors de l'affichage, en fonction des différents paramètres fournis d'après les données et résultats de calculs.
 
Les éléments composant l'objet sont indexés par :
 - un pointeur sur le premier élément : cInterface_Objet::_PremierElement (égal à NULL si l'objet n'est composé d'aucun élément)
 - un chaînage entre les éléments : cInterface_Objet_Element::_Suivant (égal à NULL sur le dernier élément)
 
La constitution de la suite d'éléments se fait par appels à la méthode cInterface_Objet::_Chaine assurant l'enregistrement des pointeurs de chaînage. Cependant, cette méthode ne garantit pas que les éléments chaînés soient compatibles avec le type d'objet d'interface. Cette méthode est donc protégée, et est donc appelée par des opérateurs de chaînages publics, définis pour chaque type d'objet.
 
L'affichage de l'objet en fonction des données nécessaires à celui-ci se fait par la méthode cInterface_Objet::Evalue.
 
Les objets d'interface peuvent être construits à plusieurs fins, dont chacune fait l'objet d'une classe définissant les possibilités de chaque utilisation, et permettant d'assurer certaines propriétés :
 - cInterface_Objet_Connu_ListeParametres : objet contenant une liste d'éléments destinés à être fournis ultérieurement en paramètre d'un autre objet, dont tous les éléments sont d'ores et déjà définis
 - cInterface_Objet_AEvaluer : ensemble des objets nécessitant des informations complémentaires pour être évalué :
  - cInterface_Objet_AEvaluer_PageEcran : objet décrivant une page écran destinée à être évaluée pour l'affichage
  - cInterface_Objet_AEvaluer_ListeParametres : objet contenant une liste d'éléments destinés à être fournis en paramètres, mais dont les éléments ne sont pas nécessairement connus
*/
class cInterface_Objet
{
    protected:
        cInterface_Objet_Element* _PremierElement; //!< Liste chaînée d'éléments d'interface définissant l'objet

        //! \name Modificateurs
        //@{
        cInterface_Objet_Element* _Chaine( cInterface_Objet_Element* );
        //@}

        //! \name Accesseurs
        //@{
        const cInterface_Objet_Element* _getElement( int ) const;
        //@}

    public:
        //! \name Calculateurs
        //@{
        bool EstVide() const;
        void Evalue( std::ostream&, 
		     const cInterface_Objet_Connu_ListeParametres&, 
		     const void* __Objet = 0
                     , const cSite* __Site = NULL ) const;
        //@}

        //! \name Constructeur et destructeur
        //@{
        cInterface_Objet();
        ~cInterface_Objet();
        //@}
};



#endif

