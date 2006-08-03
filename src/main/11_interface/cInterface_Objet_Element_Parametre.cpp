



        /*! \brief Accesseur �l�ment suivant dans la cha�ne
         \return Pointeur vers l'�l�ment suivant dans la cha�ne
         \author Hugues Romain
         \date 2000-2005
        */
        const cInterface_Objet_Element_Parametre* cInterface_Objet_Element_Parametre::getSuivant() const
        {
            return ( cInterface_Objet_Element_Parametre* ) _Suivant;
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
int cInterface_Objet_Element_Parametre::Evalue( std::ostream& __Flux, const cInterface_Objet_Connu_ListeParametres& __Parametres, const void* __Objet, const cSite* ) const
        {
            __Flux << Texte( __Parametres, __Objet );
            return INCONNU;
        }




