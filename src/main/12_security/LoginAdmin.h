
#ifndef SYNTHESE_LOGIN_ADMIN_H
#define SYNTHESE_LOGIN_ADMIN_H

namespace synthese
{
	namespace interfaces
	{
		/** Ecran de connexion int�gr� � la console d'administration.
			@ingroup m05
		
			@image html cap_admin_login.png
			@image latex cap_admin_login.png "Maquette de l'�cran de connexion int�gr� � la console d'administration" width=14cm

			<i>Titre de la fen�tre</i> :
				- SYNTHESE Admin - Login

			<i>Barre de navigation</i> :
				- Bienvenue sur SYNTHESE Admin

			<i>Zone de contenus</i> : Il s'agit d'un <b>Formulaire de connexion</b>.
				-# <tt>Utilisateur</tt> : Champ texte destin� � recevoir le nom d'utilisateur.
				-# <tt>Mot de passe</tt> : Champ texte cach� destin� � recevoir le mot de passe de l'utilisateur.
				-# bouton <tt>Connexion</tt> : Lance la tentative d'identification :
					- Si l'utilisateur a le droit de se connecter (habilitation priv�e SecurityRight de niveau USE) et si le mot de passe est valide alors l'utilisateur est conduit sur la page d'accueil HomeAdmin.
					- Sinon un message d'erreur "Utilisateur incorrect" est retourn�.
			
			<i>S�curit�</i>
				- Cette page est accessible sans restriction
				- Une habilitation priv�e SecurityRight de niveau USE est n�cessaire pour pouvoir se connecter.


			<i>Journaux</i> : Les op�rations suivantes sont consign�es dans le journal de s�curit� SecurityLog :
				- INFO : Connexion d'utilisateur
				- WARNING : Refus de connexion apr�s cinq tentatives sur une m�me session

		*/
		class LoginAdmin: public AdminInterfaceElement
		{
		};
	}
}