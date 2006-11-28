
#ifndef SYNTHESE_SECURITY_PROFILE_H
#define SYNTHESE_SECURITY_PROFILE_H

#include <vector>
#include <string>

#include "01_util/Registrable.h"
#include "01_util/UId.h"

namespace synthese
{
	namespace security
	{
		class Right;

		/** Profil utilisateur.
			@ingroup m12
		
			Un profil utilisateur est un "modèle" de droits utilisateurs consistant en une liste d'@ref synthese::security::Right "habilitations" prédéfinies.

			Un profil utilisateur peut hériter d'un autre : l'ensemble des habilitations définies par le profil parent est inclus, à l'exception de celles qui sont redéfinies. Un profil peut ne pas avoir de parent.

			Exemples de profils utilisateurs :
				- Utilisateur non identifié : peut utiliser les services grand public
				- Utilisateur classique : mêmes droits qu'Anonymous, ainsi que les fonctionnalités de paramétrage utilisateur
				- Utilisateur identifié comme client du système, d'un réseau de transport : mêmes droits qu'User, et peut de plus utiliser la fonction de réservation
				- Conducteur de ligne de transport : même droits qu'User, et peut en outre consulter les listes de réservations
				- Opérateur système : peut gérer les utilisateurs, prendre les réservations, consulter les listes de réservations
				- Responsable de ligne : peut prendre les réservations, consulter les listes de réservations, éditer des messages sur la ligne
				- Administrateur : tous les droits

		*/
		class Profile : public util::Registrable<uid,Profile>
		{
		public:
			typedef std::vector<Right*> RightsVector;

		private:
			std::string		_name;
			RightsVector	_rights;
			uid				_parentId;

		public:
			/** Comparison operator between profiles.
				@param profile Profile to compare with
				@return true if the compared profile permits at least one thing that the current profile can not do.
			*/
			int operator<=(const Profile& profile) const;

			Profile(uid id);
			~Profile();

			void setName(const std::string& name);
			void setRights(const RightsVector& rightsvector);
			void cleanRights();
			void setParent(uid id);
			void setRights(const std::string& text);

			const std::string& getName() const;
		};
	}
}

#endif
