


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
        int Evalue( std::ostream&, const cInterface_Objet_Connu_ListeParametres&
                       , const void* ObjetAAfficher = NULL, const cSite* __Site = NULL ) const;
        cInterface_Objet_Element* Copie( const cInterface_Objet_AEvaluer_ListeParametres& __Parametres ) const;
        //@}

    public:

        //! \name Constructeur
        //@{
        explicit cInterface_Objet_Element_TestConditionnel() : cInterface_Objet_Element() {}
        explicit cInterface_Objet_Element_TestConditionnel( const std::string& );
        //@}
};




