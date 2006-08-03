



/*! \brief Extraction d'une valeur num�rique depuis l'objet
*/
int cInterface_Objet_Element_Parametre_TexteAEvaluer::Nombre( const cInterface_Objet_Connu_ListeParametres& __Parametres, const void* ) const
{
    return __Parametres[ _NumeroParametre ] ->Nombre( __Parametres );
}



/*! \brief Extraction d'une valeur texte depuis l'objet
 \param __Parametres Liste cha�n�e de param�tres o� extraire la valeur texte
 \return La valeur du param�tre fourni de rang l'index contenu dans l'�l�ment
 \author Hugues Romain
 \date 2001-2005
*/
const std::string& cInterface_Objet_Element_Parametre_TexteAEvaluer::Texte( const cInterface_Objet_Connu_ListeParametres& __Parametres, const void* ) const
{
    return __Parametres[ _NumeroParametre ] ->Texte( __Parametres );
}



/*! \brief Constructeur
 \param Valeur Num�ro du param�tre de l'appelant qui sera �valu�
 \author Hugues Romain
 \date 2005
*/
cInterface_Objet_Element_Parametre_TexteAEvaluer::cInterface_Objet_Element_Parametre_TexteAEvaluer( int Valeur ) : cInterface_Objet_Element_Parametre()
{
    _NumeroParametre = Valeur;
}




/*! \brief Copie d'un �l�ment de param�tre dynamique, selon les param�tres fournis
 \param Parametres Chaine de param�tres desquels tenir compte
 \author Hugues Romain
 \date 2000-2005
 
La copie d'un �l�ment de param�tre dynamique consiste � cr�er une copie du ni�me param�tre fourni, avec n �tant le num�ro du param�tre de l'objet (transalation), quelle que soit sa nature.
*/
cInterface_Objet_Element* cInterface_Objet_Element_Parametre_TexteAEvaluer::Copie( const cInterface_Objet_AEvaluer_ListeParametres& __Parametres ) const
{
    return __Parametres[ _NumeroParametre ] ->Copie();
}



/*! \brief Copie exacte d'un �l�ment de param�tre dynamique
 \author Hugues Romain
 \date 2000-2005
*/
cInterface_Objet_Element* cInterface_Objet_Element_Parametre_TexteAEvaluer::Copie() const
{
    return new cInterface_Objet_Element_Parametre_TexteAEvaluer( _NumeroParametre );
}



