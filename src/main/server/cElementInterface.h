/*! \file cElementInterface.h
\brief En-t�te classes �l�ments d'interface
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
 
Les �l�ments d'interface constituent les objets d'interface par cha�nage successifs de ceux-ci, d�termin� d'apr�s les fichiers de donn�es.
Les �l�ments d'interface peuvent �tre de plusieurs types :
 - cInterface_Objet_Interface_Parametre : �l�ments pouvant �tre inclus dans des objets de type param�tre :
   - cInterface_Objet_Element_Parametre_TexteConnu : �l�ment constant (texte)
   - cInterface_Objet_Element_Parametre_TexteAEvaluer : �l�ment non connu, qui sera puis� � l'�valuation dans l'objet des param�tres d'ex�cution
   - cInterface_Objet_Element_Parametre_DonneeEnvironnement : �l�ment non connu, qui sera puis� � l'�valuation directement dans l'environnement de donn�es
 - cInterface_Objet_Element_Bibliotheque : �l�ment qui sera fabriqu� � l'�valuation selon une proc�dure cod�e dans la biblioth�que interne de SYNTHESE
 - cInterface_Objet_Element_TestConditionnel : paire d'�l�ments non n�cessairement connus, avec d�termination de celui qui sera �valu� en fonction d'un �l�ment param�tre, �galement �valu� � l'affichage
 
La navigation dans le cha�nage des �l�ments d'un objet se fait gr�ce au pointeur _Suivant, accessible publiquement gr�ce � la m�thode getSuivant pour une lecture, ou Suivant pour obtenir un �l�ment avec les droits de modification.
 
Une m�thode virtuelle de copie permet � chaque �l�ment quel que soit son type d'�tre copi� lors des inclusions par exemple.
 
Enfin, l'�valuation des �l�ments sur un flux de donn�es, appell�e successivement lors du parcours de la cha�ne des �l�ments lors d'une �valuation de l'objet propri�taire, est possible avec la m�thode Evalue. 
En outre, pour les �l�ments pouvant �tre inclus dans des objets de type param�tre, il est possible de r�cup�rer directement la donn�e source du r�sultat de l'�valuation pour utilisation dans un algorithme par exemple, � l'aide des m�thodes cInterface_Objet_Element_Parametre::Texte et cInterface_Objet_Element_Parametre::Nombre.
 
Les m�thodes �value impl�ment�es par les classes inf�rieures retournent un index �gal :
 - � NULL si la lecture du module doit �tre interrompue (break : d�licat � utiliser)
 - au num�ro de ligne � rejoindre si une ligne doit �tre jointe
 - � INCONNU (-1) sinon
 
*/
class cInterface_Objet_Element
{
    protected:
        cInterface_Objet_Element* _Suivant;    //!< Cha�nage

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



/*! \brief El�ment d'interface pouvant �tre inclus dans des objets de type param�tre
 \author Hugues Romain
 \date 2005
 @ingroup m11
 
Cette classe d�clare l'aptitude d'un �l�ment d'interface � fournir une valeur r�f�rence texte ou num�rique � l'�x�cution, directement issue d'un objet en m�moire, � l'oppos� des autres objets qui ne peuvent qu'alimenter un flux de donn�es dans un processus d'�valuation plus complexe fabriquant le r�sultat en direct.
 
L'int�r�t de conna�tre cette propri�t� est de b�n�ficier de deux accesseurs suppl�mentaires permettant de stocker le r�sultat d'une future �valuation de l'objet, par exemple pour int�grer le r�sultat � une processus de d�cision :
 - La m�thode Texte fournir une r�f�rence vers une cha�ne de caract�res qui correspond � ce que produirait une �valuation
 - La m�thode Nombre fournit la traduction num�rique du texte pr�c�dent (0 si le texte n'est pas num�rique)
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


/*! \brief Element d'interface contenant une cha�ne de caract�res pr�te � afficher
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



        /*! \brief Element d'interface de type param�tre � contenu non connu
         \author Hugues Romain
         \date 2005
         @ingroup m11
         
        Cette classe d�finit un �l�ment qui d�termine quel param�tre connu devra �tre �valu� lors de la fabrication des r�sultats.
        */
    class cInterface_Objet_Element_Parametre_TexteAEvaluer : public cInterface_Objet_Element_Parametre
        {
            protected:
                int _NumeroParametre; //!< Num�ro du param�tre appelant qui sera affich�

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









        /*! \brief Accesseur �l�ment suivant dans la cha�ne
         \return Pointeur vers l'�l�ment suivant dans la cha�ne
         \author Hugues Romain
         \date 2000-2005
        */
        inline const cInterface_Objet_Element_Parametre* cInterface_Objet_Element_Parametre::getSuivant() const
        {
            return ( cInterface_Objet_Element_Parametre* ) _Suivant;
        }



        /*! \brief Accesseur �l�ment suivant dans la cha�ne
         \return Pointeur vers l'�l�ment suivant dans la cha�ne
         \author Hugues Romain
         \date 2000-2005
        */
        inline const cInterface_Objet_Element* cInterface_Objet_Element::getSuivant() const
        {
            return _Suivant;
        }



        /*! \brief Modificateur �l�ment suivant dans la cha�ne
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




        /*! \brief Evaluation d'un �l�ment de type param�tre
         \param __Flux Flux sur lequel �crire le r�sultat de l'�valuation
         \param __Parametres Param�tres � fournir � l'�l�ment pour l'�valuation
         \param __Objet Pointeur vers l'objet � fournir � l'�l�ment pour l'�valuation
         \return vrai
         \author Hugues Romain
         \date 2001-2005
         
        Affiche R�f�rence vers le flux sur lequel a �t� �crite l'�valuation de l'�l�ment en fonction des param�tres et de l'objet fournis
        */
        inline int cInterface_Objet_Element_Parametre::Evalue( std::ostream& __Flux, const cInterface_Objet_Connu_ListeParametres& __Parametres, const void* __Objet, const cSite* ) const
        {
            __Flux << Texte( __Parametres, __Objet );
            return INCONNU;
        }
#define SYNTHESE_CINTERFACE_OBJET_ELEMENT

#endif

#include "cElementInterfaceTestParametreDynamique.h"

