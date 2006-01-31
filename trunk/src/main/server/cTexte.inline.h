/*!	\file cTexte.inline.h
	\brief Implémentation inline classes cTexte
*/



/*!	\brief Finalisation
	\author Hugues Romain
	\date 2005
*/
inline void cTexte::Finalise()
{
	_Element[_Taille] = 0;
}





/*!	\brief Ecriture sur l'objet depuis un tableau de caractères
	\param Obj L'objet cTexte sur lequel écrire
	\param Data Le tableau de caractères à copier
	\return L'objet cTexte
*/
inline cTexte& operator<<(cTexte& Obj, const char* Data)
{
	Obj.Copie(Data);
	return Obj;
}



/*!	\brief Ecriture sur l'objet depuis un entier
	\param Obj L'objet cTexte sur lequel écrire
	\param Data L'entier à copier
	\return L'objet cTexte
*/
inline cTexte& operator<<(cTexte& Obj, int Data)
{
	Obj.Copie(Data);
	return Obj;
}



/*!	\brief Ecriture sur l'objet depuis un autre
	\param Obj L'objet cTexte sur lequel écrire
	\param Data L'objet cTexte à copier
	\return L'objet cTexte
*/
inline cTexte& operator<<(cTexte& Obj, const cTexte& Data)
{
	Obj.Copie(Data);
	return Obj;
}



/*!	\brief Ecriture sur l'objet depuis un flux d'entrée quelconque
	\param flux Le flux d'entrée
	\param Obj L'objet cTexte sur lequel écrire
	\return Le flux d'entrée
	\warning Cette fonction n'étant pas utilisée en production, elle utilise un espace mémoire inutilement important, et ne vérifie pas correctement l'allocation mémoire de Tampon
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


