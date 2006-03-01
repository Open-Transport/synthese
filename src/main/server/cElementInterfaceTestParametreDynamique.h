/*! \file cElementInterfaceTestParametreDynamique.h
\brief En-tête classe élément d'interface test dynamique d'objet
*/


#ifdef SYNTHESE_CINTERFACE_OBJET_ELEMENT
#ifndef SYNTHESE_CELEMENTINTERFACETESTPARAMETREDYNAMIQUE_H
#define SYNTHESE_CELEMENTINTERFACETESTPARAMETREDYNAMIQUE_H

#include "cElementInterface.h"



/*! \brief Element d'interface de test dynamique d'objet
 @ingroup m11
 \author Hugues Romain
 \date 2005
 
L'évaluation des objets de cette classe constitue en une évaluation préalable dite évaluation test, qui va déterminer quel objet est finalement évalué vers le flux de sortie, en fonction du résultat vide ou non vide.
*/
class cInterface_Objet_Element_TestConditionnel : public cInterface_Objet_Element
{
        cInterface_Objet_Element_Parametre* _ObjetATester;  //!< Objet dont l'évaluation est testée
        cInterface_Objet_Element* _ObjetSiVide;  //!< Objet évalué si l'évaluation test donne vide
        cInterface_Objet_Element* _ObjetSiNonVide; //!< Objet évalué si l'évaluation test donne non vide

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



/*! \brief Evaluation de l'élément
 \param __Flux Flux de sortie sur lequel écrire le résultat de l'évaluation
 \param __Parametres Paramètres à transmettre à l'élément qui est évalué
 \param __Objet Objet à transmettre à l'élément qui est évalué
 \param __Site Pointeur vers le site ayant déclenché l'évaluation
 \return 
 \author Hugues Romain
 \date 2005
 
Lance l'évaluation de l'élément "si vide" si l'objet à tester évalué retourne un texte vide ou bien le texte "0", l'autre élément sinon
 
*/
inline int cInterface_Objet_Element_TestConditionnel::Evalue( ostream& __Flux, const cInterface_Objet_Connu_ListeParametres& __Parametres, const void* __Objet, const cSite* __Site ) const
{
    const cTexte & __Texte = _ObjetATester->Texte( __Parametres, __Objet );
    if ( __Texte.Taille() && !__Texte.Compare( "0" ) )
        return _ObjetSiNonVide == NULL ? NULL : _ObjetSiNonVide->Evalue( __Flux, __Parametres, __Objet, __Site );
    else
        return _ObjetSiVide == NULL ? NULL : _ObjetSiVide->Evalue( __Flux, __Parametres, __Objet, __Site );
}



/*! \brief Copie de l'élément
 \param __Parametres La chaine de paramètres de l'objet d'interface destiné à recevoir le nouvel élément
 \return Element d'interface à ajouter à l'objet d'interface, construit en fonction de sa chaîne de paramètres
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
