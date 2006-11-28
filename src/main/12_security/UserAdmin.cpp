
#include "12_security/UserAdmin.h"

namespace synthese
{
	namespace security
	{


		UserAdmin::UserAdmin()
			: AdminInterfaceElement("users", AdminInterfaceElement::DISPLAYED_IF_CURRENT)
		{
			
		}

		std::string UserAdmin::getTitle() const
		{
			return "user";
		}

		void UserAdmin::display( std::ostream& stream, const interfaces::ParametersVector& parameters, const void* rootObject /* = NULL */, const server::Request* request /* = NULL */ ) const
		{
			stream
				<< "<TABLE>"
				<< "<TR><Th colSpan=\"2\">Connexion</Th></TR>"
				<< "<TR><TD>Login</TD><TD><INPUT value=\"gerard\" type=\"text\" name=\"Text1\"></TD></TR>"
				<< "<TR><TD>Mot de passe</TD><TD><INPUT type=\"password\" value=\"\" name=\"Password1\"></TD></TR>"
				<< "<TR><TD>Mot de passe (vérification)</TD><TD><INPUT type=\"password\" value=\"\" name=\"Password1\"></TD></TR>"
				<< "<TR><th colSpan=\"2\">Coordonnées</th></TR>"
				<< "<TR><TD>Prénom</TD><TD><INPUT type=\"text\" value=\"Gérard\" name=\"Text1\"></TD></TR>"
				<< "<TR><TD>Nom</TD><TD><INPUT type=\"text\" value=\"Dupont\" name=\"Text1\"></TD></TR>"
				<< "<TR><TD>Adresse</TD><TD><TEXTAREA name=\"Textarea1\" rows=\"2\" cols=\"20\"></TEXTAREA></TD></TR>"
				<< "<TR><TD>Code postal</TD><TD><INPUT type=\"text\" name=\"Text1\"></TD></TR>"
				<< "<TR><TD>Ville</TD><TD><INPUT type=\"text\" name=\"Text1\"></TD></TR>"
				<< "<TR><TD>E-mail</TD><TD><INPUT type=\"text\" name=\"Text1\"></TD></TR>"
				<< "<TR><th colSpan=\"2\">Droits</TD></TR>"
				<< "<TR><td>Connexion autorisée</td><TD><INPUT type=\"radio\" CHECKED value=\"Radio1\" name=\"RadioGroupc\">OUI	<INPUT type=\"radio\" value=\"Radio1\" name=\"RadioGroupc\">NON</TD></TR>"
				<< "<TR><TD>Profil</TD><TD><SELECT name=\"Select1\">";
			
			// Profile list


			stream
				<< "</SELECT></TD></TR>"
				<< "<TR><TD style=\"text-align:center\" colSpan=\"2\"><INPUT type=\"submit\" value=\"Enregistrer les modifications\"></TD></TR>"
				<< "</TABLE>";
		}
	}
}