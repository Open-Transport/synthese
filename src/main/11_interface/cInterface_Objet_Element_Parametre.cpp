



        /*! \brief Accesseur élément suivant dans la chaîne
         \return Pointeur vers l'élément suivant dans la chaîne
         \author Hugues Romain
         \date 2000-2005
        */
        const cInterface_Objet_Element_Parametre* cInterface_Objet_Element_Parametre::getSuivant() const
        {
            return ( cInterface_Objet_Element_Parametre* ) _Suivant;
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
int cInterface_Objet_Element_Parametre::Evalue( std::ostream& __Flux, const cInterface_Objet_Connu_ListeParametres& __Parametres, const void* __Objet, const cSite* ) const
        {
            __Flux << Texte( __Parametres, __Objet );
            return INCONNU;
        }




