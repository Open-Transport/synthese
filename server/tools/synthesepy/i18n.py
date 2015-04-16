#    -*- coding: utf-8 -*-
#    Translated strings.
#    @file i18n.py
#    @author Sylvain Pasche
#
#    This file belongs to the SYNTHESE project (public transportation specialized software)
#    Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.


lang = "fr"
def set_lang(lang_):
    global lang
    lang = lang_

if lang == "fr":
    # NOTE: strings should be unicode.

    mail_summary = u"Synthese '{project_name}' sur '{hostname}' import '{template_label}': {detail}"

    # import labels
    state = u"Etat"
    description = u"Description"

    # import comment
    new_comment = u"nouveau commentaire"
    new_comment_body = u"Un commentaire a été ajouté par l'utilisateur {username}:\n\n{comment}"

    # import update
    import_update = u"mise à jour de l'import"
    import_updated_body = u"L'import a été mis à jour par l'utilisateur {username}."

    # import results
    import_result = u"résultat de l'exécution"
    import_exception = u"Une erreur s'est produite durant l'import:\n"
    start_of_summary = u"Liste des messages avec un niveau égal ou supérieur à {level}:\n\n"
    import_level_section = u"\nNiveau {level}:\n\n"
    end_of_messages = u"\n\nFin des messages\n\n"

    technical_infos = u"""Informations techniques:

Import à blanc: {dummy}

Appels syntheses:
{synthese_calls}

Durée de l'import (s): {duration}
"""

    # Web Manager

    # view messages
    bad_credentials = u"Nom d'utilisateur ou mot de passe invalide"
    cant_reach_synthese = u"Problème lors de la communication avec Synthese (démon arrêté ?)"
    logged_in = u"Vous êtes maintenant connecté"
    logged_out = u"Vous êtes maintenant déconnecté"

    # layout
    error = u"Erreur: "
    login = u"Connexion"
    logout = u"Déconnecter"

    # login
    username = u"Nom d'utilisateur"
    password = u"Mot de passe"

    # import template list
    no_templates = u"Pas de template d'import"
    import_created = u"Nouvel import créé"

    # import template
    no_imports = u"Pas d'import"

    # import, update
    import_updated = u"Import mis à jour"

    # import, new comment
    import_comment_added = u"Votre commentaire à été publié"

    # import, execute
    import_executed = u"Import executé"

    def format_datetime(datetime):
        return datetime.strftime("%d/%m/%Y %H:%M")


elif lang == 'en':

    logged_in = u"You were logged in"
    logged_out = u"You were logged out"
