
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
		
			Un profil utilisateur est un "mod�le" de droits utilisateurs consistant en une liste d'@ref synthese::security::Right "habilitations" pr�d�finies.

			Un profil utilisateur peut h�riter d'un autre : l'ensemble des habilitations d�finies par le profil parent est inclus, � l'exception de celles qui sont red�finies. Un profil peut ne pas avoir de parent.

			Exemples de profils utilisateurs :
				- Utilisateur non identifi� : peut utiliser les services grand public
				- Utilisateur classique : m�mes droits qu'Anonymous, ainsi que les fonctionnalit�s de param�trage utilisateur
				- Utilisateur identifi� comme client du syst�me, d'un r�seau de transport : m�mes droits qu'User, et peut de plus utiliser la fonction de r�servation
				- Conducteur de ligne de transport : m�me droits qu'User, et peut en outre consulter les listes de r�servations
				- Op�rateur syst�me : peut g�rer les utilisateurs, prendre les r�servations, consulter les listes de r�servations
				- Responsable de ligne : peut prendre les r�servations, consulter les listes de r�servations, �diter des messages sur la ligne
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
