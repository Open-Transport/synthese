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







#endif

