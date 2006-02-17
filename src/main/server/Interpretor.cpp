

#include "Interpretor.h"

using namespace interpretor;


/** Constructeur.
*/
template <class T>
Interpretor<T>::Interpretor()
{
}

/** Ajout d'�l�ment dans le r�seau.

	@param key Cl� texte de l'�l�ment
	@param element El�ment � ajouter
	@param id Index de l'�l�ment (vide/INCONNU = auto : cr�ation d'un index si nouvel �l�ment, rien si �lement d�j� existant)
	@return tIndex L'index de l'�l�ment ajout�, ICONNU si ajout impossible

	Si l'index est fourni et si il existe, alors l'ajout consiste en l'ajout d'une cl� texte dans le r�seau vers celui-ci.
	Si l'index est fourni et si il n'existe pas, alors l'ajout consiste en l'ajout de l'�l�ment dans la liste, puis � l'ajout de la cl� texte dans le r�seau.
	Si l'index n'est pas fourni, et si aucun �l�ment de la liste n'est identique � l'�l�ment fourni, alors l'�l�ment est ajout� en queue de liste
	Si l'index n'est pas fourni et si un �l�ment de la liste est identique, alors une cl� texte est ajout�e au r�seau vers cet �lement

	Dans tous les cas l'index de l'�l�ment est retourn�, qu'il ait �t� cr�� ou non, sauf en cas de probl�me ou la valeur de retour est INCONNU (-1).

	@todo Mettre une section critique + CR impl�menter le lien r�seau neurone
*/
template <class T>
Index Interpretor<T>::add(const Key& key, const T& element, Index id)
{
	// Stockage dans le vecteur
	if (id == INCONNU)
	{
		_vector.push_back(element);
		id = _vector.size() - 1;
	}
	else
		_vector[id] = element;

	// Etablissement du lien dans le r�seau de neurone


	return id;
}


/** Suppression d'un lien de correspondance cl� => �l�ment.

	La suppression par cl� texte ne supprime que le lien texte => �l�ment et non la totalit� des liens vers l'�l�ment point� par la cl� texte.
	
	@warning Un �l�ment peut ne plus poss�der de cl� texte vers celui-ci si la derni�re a �t� supprim�e.

	@param __Index Index de l'�l�ment � supprimer
	@return true si Suppression OK (peut �tre chang�)

	@todo CR : Impl�menter, �a ne concerne que le r�seau de neurone
*/
template <class T>
void Interpretor<T>::erase(const Key& key)
{
	
}


/** Suppression d'�l�ment dans le r�seau d'apr�s son index.

	La suppression par �l�ment supprime la totalit� des liens vers l'�l�ment.

	@param id Index de l'�l�ment � supprimer
	@return true si Suppression OK (peut �tre chang�)

	@todo CR : faire la partie r�seau neurone
*/
template <class T>
bool Interpretor<T>::erase(Index id)
{
	// CR : ici l'effacement de toutes les cl�s dans le r�seau
	
	_vector.erase(id);
}


/** Accesseur d'apr�s l'index num�rique
	@param __Index Index de l'objet � acc�der
	@return Objet demand�
*/
template <class T>
T& Interpretor<T>::operator[](Index id)
{
	return _vector.operator [](id);
}


/** Nombre d'enregistrements contenus.
*/
template <class T>
Index Interpretor<T>::size () const
{
	return _vector.size();
}


/** Interpr�tation de texte.

	L'interpr�tation s'effectue par action de l'associateur sur le r�seau de neurones

	(d�tailler le fonctionnement � l'int�rieur du source avec tags doxygen pour prise en compte)

	@param key Texte � interpr�ter
	@param size Nombre de solutions � produire
	@return couple index �l�ment trouv� / score, tri� dans l'ordre dr�croissant de score

	@todo CR : A impl�menter
*/
template <class T>
vector<Result> Interpretor<T>::search(const Key& key, Index size) const
{
	vector<Result> result;

	return result;
}
