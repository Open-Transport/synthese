/*! \file cElementInterface.h
\brief En-tête classes éléments d'interface
*/


#ifndef SYNTHESE_CELEMENTINTERFACE_H
#define SYNTHESE_CELEMENTINTERFACE_H

#include "cSite.h"
#include "cObjetInterface.h"
#include "Parametres.h"
#include <string>

#include "01_util/Conversion.h"


class cInterface_Objet_Element;
class cInterface_Objet_Element_Parametre;
class cInterface_Objet_Element_Parametre_TexteConnu;
class cInterface_Objet_Element_Parametre_TexteAEvaluer;
class cInterface_Objet_Element_TestConditionnel;



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



/*! \brief Elément d'interface pouvant être inclus dans des objets de type paramètre
 \author Hugues Romain
 \date 2005
 @ingroup m11
 
Cette classe déclare l'aptitude d'un élément d'interface à fournir une valeur référence texte ou numérique à l'éxécution, directement issue d'un objet en mémoire, à l'opposé des autres objets qui ne peuvent qu'alimenter un flux de données dans un processus d'évaluation plus complexe fabriquant le résultat en direct.
 
L'intérêt de connaître cette propriété est de bénéficier de deux accesseurs supplémentaires permettant de stocker le résultat d'une future évaluation de l'objet, par exemple pour intégrer le résultat à une processus de décision :
 - La méthode Texte fournir une référence vers une chaîne de caractères qui correspond à ce que produirait une évaluation
 - La méthode Nombre fournit la traduction numérique du texte précédent (0 si le texte n'est pas numérique)
*/
class cInterface_Objet_Element_Parametre : public cInterface_Objet_Element
{
    public:
        //! \name Accesseurs
        //@{
        const cInterface_Objet_Element_Parametre* getSuivant() const;
        //@}

        //! \name Calculateurs
        //@{
        virtual cInterface_Objet_Element* Copie() const = 0;
        virtual cInterface_Objet_Element* Copie( const cInterface_Objet_AEvaluer_ListeParametres& __Parametres ) const = 0;
        virtual int Nombre( const cInterface_Objet_Connu_ListeParametres& __Parametres
                            , const void* __Objet = NULL ) const = 0;
        virtual const std::string& Texte( const cInterface_Objet_Connu_ListeParametres& __Parametres
                                     , const void* __Objet = NULL ) const = 0;
        int Evalue( std::ostream&, const cInterface_Objet_Connu_ListeParametres&
                       , const void* ObjetAAfficher = NULL, const cSite* __Site = NULL ) const;
        //@}

        cInterface_Objet_Element_Parametre() : cInterface_Objet_Element() {}
        ~cInterface_Objet_Element_Parametre() { }

};


/*! \brief Element d'interface contenant une chaîne de caractères prète à afficher
 \author Hugues Romain
 \date 2005
 @ingroup m11
*/
class cInterface_Objet_Element_Parametre_TexteConnu : public cInterface_Objet_Element_Parametre
{
    protected:
        std::string _Texte; //!< Texte contenu par l'objet

    public:

        //! \name Calculateurs
        //@{
        cInterface_Objet_Element* Copie() const;
        cInterface_Objet_Element* Copie( const cInterface_Objet_AEvaluer_ListeParametres& __Parametres ) const;
        int Nombre( const cInterface_Objet_Connu_ListeParametres& __Parametres
                    , const void* __Objet = NULL ) const;
        const std::string& Texte( const cInterface_Objet_Connu_ListeParametres& __Parametres
                             , const void* __Objet = NULL ) const;
        //@}

        //! \name Constructeur
        //@{
        explicit cInterface_Objet_Element_Parametre_TexteConnu( const std::string& );
        explicit cInterface_Objet_Element_Parametre_TexteConnu( const int );
        ~cInterface_Objet_Element_Parametre_TexteConnu() { }
        //@}

};



        /*! \brief Element d'interface de type paramètre à contenu non connu
         \author Hugues Romain
         \date 2005
         @ingroup m11
         
        Cette classe définit un élément qui détermine quel paramètre connu devra être évalué lors de la fabrication des résultats.
        */
    class cInterface_Objet_Element_Parametre_TexteAEvaluer : public cInterface_Objet_Element_Parametre
        {
            protected:
                int _NumeroParametre; //!< Numéro du paramètre appelant qui sera affiché

            public:
                //! \name Calculateurs
                //@{
                cInterface_Objet_Element* Copie() const;
                cInterface_Objet_Element* Copie( const cInterface_Objet_AEvaluer_ListeParametres& __Parametres ) const;
                int Nombre( const cInterface_Objet_Connu_ListeParametres& __Parametres
                            , const void* __Objet = NULL ) const;
                const std::string& Texte( const cInterface_Objet_Connu_ListeParametres& __Parametres
                                     , const void* __Objet = NULL ) const;
                //@}

                //! \name Constructeur
                //@{
                explicit cInterface_Objet_Element_Parametre_TexteAEvaluer( int );
                //@}
        };









        /*! \brief Accesseur élément suivant dans la chaîne
         \return Pointeur vers l'élément suivant dans la chaîne
         \author Hugues Romain
         \date 2000-2005
        */
        inline const cInterface_Objet_Element_Parametre* cInterface_Objet_Element_Parametre::getSuivant() const
        {
            return ( cInterface_Objet_Element_Parametre* ) _Suivant;
        }



        /*! \brief Accesseur élément suivant dans la chaîne
         \return Pointeur vers l'élément suivant dans la chaîne
         \author Hugues Romain
         \date 2000-2005
        */
        inline const cInterface_Objet_Element* cInterface_Objet_Element::getSuivant() const
        {
            return _Suivant;
        }



        /*! \brief Modificateur élément suivant dans la chaîne
         \return 
         \author Hugues Romain
         \date 2000-2005
        */
        inline void cInterface_Objet_Element::setSuivant( cInterface_Objet_Element* __Objet )
        {
            _Suivant = __Objet;
        }

        inline int cInterface_Objet_Element_Parametre_TexteConnu::Nombre( const cInterface_Objet_Connu_ListeParametres&, const void* ) const
        {
            return synthese::util::Conversion::ToInt (_Texte);
        }

        inline const std::string& cInterface_Objet_Element_Parametre_TexteConnu::Texte( const cInterface_Objet_Connu_ListeParametres&, const void* ) const
        {
            return _Texte;
        }

        inline cInterface_Objet_Element* cInterface_Objet_Element::Suivant()
        {
            return _Suivant;
        }




        /*! \brief Evaluation d'un élément de type paramètre
         \param __Flux Flux sur lequel écrire le résultat de l'évaluation
         \param __Parametres Paramètres à fournir à l'élément pour l'évaluation
         \param __Objet Pointeur vers l'objet à fournir à l'élément pour l'évaluation
         \return vrai
         \author Hugues Romain
         \date 2001-2005
         
        Affiche Référence vers le flux sur lequel a été écrite l'évaluation de l'élément en fonction des paramètres et de l'objet fournis
        */
        inline int cInterface_Objet_Element_Parametre::Evalue( std::ostream& __Flux, const cInterface_Objet_Connu_ListeParametres& __Parametres, const void* __Objet, const cSite* ) const
        {
            __Flux << Texte( __Parametres, __Objet );
            return INCONNU;
        }
#define SYNTHESE_CINTERFACE_OBJET_ELEMENT

#endif

#include "cElementInterfaceTestParametreDynamique.h"

