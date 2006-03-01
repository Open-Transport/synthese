/*! \file cElementInterfaceTestParametreDynamique.h
\brief En-t�te classe �l�ment d'interface test dynamique d'objet
*/


#ifdef SYNTHESE_CINTERFACE_OBJET_ELEMENT
#ifndef SYNTHESE_CELEMENTINTERFACETESTPARAMETREDYNAMIQUE_H
#define SYNTHESE_CELEMENTINTERFACETESTPARAMETREDYNAMIQUE_H

#include "cElementInterface.h"



/*! \brief Element d'interface de test dynamique d'objet
 @ingroup m11
 \author Hugues Romain
 \date 2005
 
L'�valuation des objets de cette classe constitue en une �valuation pr�alable dite �valuation test, qui va d�terminer quel objet est finalement �valu� vers le flux de sortie, en fonction du r�sultat vide ou non vide.
*/
class cInterface_Objet_Element_TestConditionnel : public cInterface_Objet_Element
{
        cInterface_Objet_Element_Parametre* _ObjetATester;  //!< Objet dont l'�valuation est test�e
        cInterface_Objet_Element* _ObjetSiVide;  //!< Objet �valu� si l'�valuation test donne vide
        cInterface_Objet_Element* _ObjetSiNonVide; //!< Objet �valu� si l'�valuation test donne non vide

        //! \name Calculateurs
        //@{
        int Evalue( ostream&, const cInterface_Objet_Connu_ListeParametres&
                       , const void* ObjetAAfficher = NULL, const cSite* __Site = NULL ) const;
        cInterface_Objet_Element* Copie( const cInterface_Objet_AEvaluer_ListeParametres& __Parametres ) const;
        //@}

    public:

        //! \name Constructeur
        //@{
        explicit cInterface_Objet_Element_TestConditionnel() : cInterface_Objet_Element() {}
        explicit cInterface_Objet_Element_TestConditionnel( const cTexte& );
        //@}
};



/*! \brief Evaluation de l'�l�ment
 \param __Flux Flux de sortie sur lequel �crire le r�sultat de l'�valuation
 \param __Parametres Param�tres � transmettre � l'�l�ment qui est �valu�
 \param __Objet Objet � transmettre � l'�l�ment qui est �valu�
 \param __Site Pointeur vers le site ayant d�clench� l'�valuation
 \return 
 \author Hugues Romain
 \date 2005
 
Lance l'�valuation de l'�l�ment "si vide" si l'objet � tester �valu� retourne un texte vide ou bien le texte "0", l'autre �l�ment sinon
 
*/
inline int cInterface_Objet_Element_TestConditionnel::Evalue( ostream& __Flux, const cInterface_Objet_Connu_ListeParametres& __Parametres, const void* __Objet, const cSite* __Site ) const
{
    const cTexte & __Texte = _ObjetATester->Texte( __Parametres, __Objet );
    if ( __Texte.Taille() && !__Texte.Compare( "0" ) )
        return _ObjetSiNonVide == NULL ? NULL : _ObjetSiNonVide->Evalue( __Flux, __Parametres, __Objet, __Site );
    else
        return _ObjetSiVide == NULL ? NULL : _ObjetSiVide->Evalue( __Flux, __Parametres, __Objet, __Site );
}



/*! \brief Copie de l'�l�ment
 \param __Parametres La chaine de param�tres de l'objet d'interface destin� � recevoir le nouvel �l�ment
 \return Element d'interface � ajouter � l'objet d'interface, construit en fonction de sa cha�ne de param�tres
 \author Hugues Romain
 \date 2005
*/
inline cInterface_Objet_Element* cInterface_Objet_Element_TestConditionnel::Copie( const cInterface_Objet_AEvaluer_ListeParametres& __Parametres ) const
{
    cInterface_Objet_Element_TestConditionnel * __EI = new cInterface_Objet_Element_TestConditionnel();
    __EI->_ObjetATester = ( cInterface_Objet_Element_Parametre* ) _ObjetATester->Copie( __Parametres );
    __EI->_ObjetSiNonVide = _ObjetSiNonVide == NULL ? NULL : _ObjetSiNonVide->Copie( __Parametres );
    __EI->_ObjetSiVide = _ObjetSiVide == NULL ? NULL : _ObjetSiVide->Copie( __Parametres );
    return __EI;
}

#endif
#endif
