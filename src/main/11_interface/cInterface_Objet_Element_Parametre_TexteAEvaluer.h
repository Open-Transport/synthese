




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


