
#ifndef SYNTHESE_LOGIN_ADMIN_H
#define SYNTHESE_LOGIN_ADMIN_H

namespace synthese
{
	namespace interfaces
	{
		/** Ecran de connexion intégré à la console d'administration.
			@ingroup m05
		
			@image html cap_admin_login.png
			@image latex cap_admin_login.png "Maquette de l'écran de connexion intégré à la console d'administration" width=14cm

			<i>Titre de la fenêtre</i> :
				- SYNTHESE Admin - Login

			<i>Barre de navigation</i> :
				- Bienvenue sur SYNTHESE Admin

			<i>Zone de contenus</i> : Il s'agit d'un <b>Formulaire de connexion</b>.
				-# <tt>Utilisateur</tt> : Champ texte destiné à recevoir le nom d'utilisateur.
				-# <tt>Mot de passe</tt> : Champ texte caché destiné à recevoir le mot de passe de l'utilisateur.
				-# bouton <tt>Connexion</tt> : Lance la tentative d'identification :
					- Si l'utilisateur a le droit de se connecter (habilitation privée SecurityRight de niveau USE) et si le mot de passe est valide alors l'utilisateur est conduit sur la page d'accueil HomeAdmin.
					- Sinon un message d'erreur "Utilisateur incorrect" est retourné.
			
			<i>Sécurité</i>
				- Cette page est accessible sans restriction
				- Une habilitation privée SecurityRight de niveau USE est nécessaire pour pouvoir se connecter.


			<i>Journaux</i> : Les opérations suivantes sont consignées dans le journal de sécurité SecurityLog :
				- INFO : Connexion d'utilisateur
				- WARNING : Refus de connexion après cinq tentatives sur une même session

		*/
		class LoginAdmin: public AdminInterfaceElement
		{
		};
	}
}