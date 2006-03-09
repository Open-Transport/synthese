/*! \file cObjetInterface.h
\brief En-tête classes objets d'interface
*/

#ifndef SYNTHESE_COBJETINTERFACE_H
#define SYNTHESE_COBJETINTERFACE_H

class cInterface_Objet;
class cInterface_Objet_Connu_ListeParametres;
class cInterface_Objet_AEvaluer;
class cInterface_Objet_AEvaluer_ListeParametres;
class cInterface_Objet_Element;
class cInterface_Objet_Element_Parametre;
class cInterface_Objet_Element_Parametre_TexteConnu;

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



/*! \brief Objets d'interface contenant une liste d'éléments destinés à être fournis en paramètres, mais dont les éléments ne sont pas nécessairement connus
 \author Hugues Romain
 \date 2005
 
Pour construire les objets de cette classe, le chaînage classique d'éléments est naturellement possible. S'ajoute cependant une fonctionnalité d'interprétation d'une chaîne de caractères décrivant l'ensemble des éléments le constituant, assurée par la méthode cInterface_Objet_AEvaluer_ListeParametres::InterpreteTexte.
*/
class cInterface_Objet_AEvaluer_ListeParametres : public cInterface_Objet_AEvaluer
{
    public:
        //! \name Modificateurs
        //@{
        bool InterpreteTexte( const cInterface_Objet_AEvaluer_ListeParametres&, const std::string& );
        //@}

        //! \name Constructeur et destructeurs
        //@{
        cInterface_Objet_AEvaluer_ListeParametres() : cInterface_Objet_AEvaluer() {}
        //@}

};

        /** @} */
#define SYNTHESE_CINTERFACE_OBJET_AEVALUER_LISTE_PARAMETRES






        /*! \brief Ajout d'un élément en fin de chaîne
         \param __Objet L'élément à ajouter
         \return L'objet après avoir été modifié
         \author Hugues Romain
         \date 2005
         
        Cette implémentation sert à autoriser l'usage de cObjetInterface::_Chaine sur tout type d'élément, dans le cas où l'objet impacté est bien un objet permettant les éléments non connus.
        */
        inline cInterface_Objet_Element* cInterface_Objet_AEvaluer::Chaine( cInterface_Objet_Element* __Objet )
        {
            return ( cInterface_Objet_Element* ) _Chaine( __Objet );
        }

        inline bool cInterface_Objet::EstVide() const
        {
            return _PremierElement == NULL;
        }


#endif

