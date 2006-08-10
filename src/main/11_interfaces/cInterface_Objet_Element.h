#ifndef SYNTHESE_CINTERFACEOBJETELEMENT_H
#define SYNTHESE_CINTERFACEOBJETELEMENT_H

#include <iostream>
#include <string>
#include "Parametres.h"


class cInterface_Objet_Element;
class cInterface_Objet_Connu_ListeParametres;
class cInterface_Objet_AEvaluer_ListeParametres;
class cSite;



/** @addtogroup m11
 @{
*/
/*! \brief Element d'interface
 @ingroup m11
 \author Hugues Romain
 \date 2001-2005
 
Les éléments d'interface constituent les objets d'interface par chaînage successifs de ceux-ci, déterminé d'après les fichiers de données.
Les éléments d'interface peuvent être de plusieurs types :
 - cInterface_Objet_Interface_Parametre : éléments pouvant être inclus dans des objets de type paramètre :
   - cInterface_Objet_Element_Parametre_TexteConnu : élément constant (texte)
   - cInterface_Objet_Element_Parametre_TexteAEvaluer : élément non connu, qui sera puisé à l'évaluation dans l'objet des paramètres d'exécution
   - cInterface_Objet_Element_Parametre_DonneeEnvironnement : élément non connu, qui sera puisé à l'évaluation directement dans l'environnement de données
 - cInterface_Objet_Element_Bibliotheque : élément qui sera fabriqué à l'évaluation selon une procédure codée dans la bibliothèque interne de SYNTHESE
 - cInterface_Objet_Element_TestConditionnel : paire d'éléments non nécessairement connus, avec détermination de celui qui sera évalué en fonction d'un élément paramètre, également évalué à l'affichage
 
La navigation dans le chaînage des éléments d'un objet se fait grâce au pointeur _Suivant, accessible publiquement grâce à la méthode getSuivant pour une lecture, ou Suivant pour obtenir un élément avec les droits de modification.
 
Une méthode virtuelle de copie permet à chaque élément quel que soit son type d'être copié lors des inclusions par exemple.
 
Enfin, l'évaluation des éléments sur un flux de données, appellée successivement lors du parcours de la chaîne des éléments lors d'une évaluation de l'objet propriétaire, est possible avec la méthode Evalue. 
En outre, pour les éléments pouvant être inclus dans des objets de type paramètre, il est possible de récupérer directement la donnée source du résultat de l'évaluation pour utilisation dans un algorithme par exemple, à l'aide des méthodes cInterface_Objet_Element_Parametre::Texte et cInterface_Objet_Element_Parametre::Nombre.
 
Les méthodes évalue implémentées par les classes inférieures retournent un index égal :
 - à NULL si la lecture du module doit être interrompue (break : délicat à utiliser)
 - au numéro de ligne à rejoindre si une ligne doit être jointe
 - à INCONNU (-1) sinon
 
*/
class cInterface_Objet_Element
{
    protected:
        cInterface_Objet_Element* _Suivant;    //!< Chaînage

    public:
        //! \name Accesseurs
        //@{
        const cInterface_Objet_Element* getSuivant() const;
        //@}

        //! \name Modificateurs
        //@{
        void setSuivant( cInterface_Objet_Element* );
        cInterface_Objet_Element* Suivant();
        //@}

        //! \name Calculateurs
        //@{
        virtual int Evalue( std::ostream&, const cInterface_Objet_Connu_ListeParametres&
                               , const void* ObjetAAfficher = NULL, const cSite* __Site = NULL ) const = 0;
        virtual cInterface_Objet_Element* Copie( const cInterface_Objet_AEvaluer_ListeParametres& __Parametres ) const = 0;
        virtual int NumeroLigne() const { return INCONNU; }
        //@}

        //! \name Constructeur et destructeur
        //@{
        cInterface_Objet_Element();
        virtual ~cInterface_Objet_Element();
        //@}
};







#endif

