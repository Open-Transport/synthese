


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




