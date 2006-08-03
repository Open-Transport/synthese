



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

