









/*! \brief Ajout d'un élément texte en fin de chaîne
 \warning La réussite de la copie n'est pas contrôlée.
*/
cInterface_Objet_Connu_ListeParametres& 
cInterface_Objet_Connu_ListeParametres::operator<<( const std::string& __Texte )
{
    _Chaine( new cInterface_Objet_Element_Parametre_TexteConnu( __Texte ) );
    return *this;
}






/*! \brief Ajout d'un élément numérique en fin de chaîne
 \warning La réussite de la copie n'est pas contrôlée.
*/
cInterface_Objet_Connu_ListeParametres& 
cInterface_Objet_Connu_ListeParametres::operator<<( int __Nombre )
{
    _Chaine( new cInterface_Objet_Element_Parametre_TexteConnu( __Nombre ) );
    return *this;
}



const cInterface_Objet_Element_Parametre* 
cInterface_Objet_Connu_ListeParametres::operator[] ( int __Index ) const
{
    return ( cInterface_Objet_Element_Parametre* ) _getElement( __Index );
}


