/*!	\file cTexte.inline.h
	\brief Impl�mentation inline classes cTexte
*/



/*!	\brief Finalisation
	\author Hugues Romain
	\date 2005
*/
inline void cTexte::Finalise()
{
	_Element[_Taille] = 0;
}





/*!	\brief Ecriture sur l'objet depuis un tableau de caract�res
	\param Obj L'objet cTexte sur lequel �crire
	\param Data Le tableau de caract�res � copier
	\return L'objet cTexte
*/
inline cTexte& operator<<(cTexte& Obj, const char* Data)
{
	Obj.Copie(Data);
	return Obj;
}



/*!	\brief Ecriture sur l'objet depuis un entier
	\param Obj L'objet cTexte sur lequel �crire
	\param Data L'entier � copier
	\return L'objet cTexte
*/
inline cTexte& operator<<(cTexte& Obj, int Data)
{
	Obj.Copie(Data);
	return Obj;
}



/*!	\brief Ecriture sur l'objet depuis un autre
	\param Obj L'objet cTexte sur lequel �crire
	\param Data L'objet cTexte � copier
	\return L'objet cTexte
*/
inline cTexte& operator<<(cTexte& Obj, const cTexte& Data)
{
	Obj.Copie(Data);
	return Obj;
}



/*!	\brief Ecriture sur l'objet depuis un flux d'entr�e quelconque
	\param flux Le flux d'entr�e
	\param Obj L'objet cTexte sur lequel �crire
	\return Le flux d'entr�e
	\warning Cette fonction n'�tant pas utilis�e en production, elle utilise un espace m�moire inutilement important, et ne v�rifie pas correctement l'allocation m�moire de Tampon
*/
template <class T>
inline T& operator>>(T& flux, cTexte& Obj)
{
	char* Tampon = (char*) malloc(TAILLETAMPON * sizeof(char));
	flux >> Tampon;
	Obj.Vide();
	Obj.Copie(Tampon);
	free(Tampon);
	return(flux);
}


