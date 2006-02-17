
/*! \brief Modificateur de la description générale de la photo
	\param newDescriptionGenerale Nouvelle description générale
	\author Hugues Romain
	\date 2002
*/
inline bool cDocument::setDescriptionGenerale(const cTexte& __DescriptionGenerale)
{
	_DescriptionGenerale = __DescriptionGenerale;
	return _DescriptionGenerale.Taille() == __DescriptionGenerale.Taille();
}



/*! \brief Modificateur de la description locale de la photo
	\param newDescriptionLocale Nouvelle description locale
	\author Hugues Romain
	\date 2002
*/
inline bool cDocument::setDescriptionLocale(const cTexte& __DescriptionLocale)
{
	_DescriptionLocale = __DescriptionLocale;
	return _DescriptionLocale.Taille() == __DescriptionLocale.Taille();
}



/*! \brief Modificateur du nom du fichier contenant la photo
	\param newNomFichier Nouveau nom du fichier
	\author Hugues Romain
	\date 2002
*/
inline bool cDocument::SetURL(const cTexte& __URL)
{
	_URL = __URL;
	return _URL.Taille() == __URL.Taille();
}



/*!	\brief DescriptionLocale
	\author Hugues Romain
	\date 2003-2005
*/
inline const cTexte& cDocument::DescriptionLocale() const
{
	return _DescriptionLocale;
}



inline const cTexte& cDocument::URL()		const
{
	return _URL;
}



// cPhoto 1.0 - Code
// ____________________________________________________________________________
//
// 
// ____________________________________________________________________________ 
inline tIndex cDocument::Index() const
{
	return _Index;
}
// © Hugues Romain 2002
// ____________________________________________________________________________ 
