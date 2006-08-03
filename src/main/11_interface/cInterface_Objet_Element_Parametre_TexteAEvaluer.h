




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


