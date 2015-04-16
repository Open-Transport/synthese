#    Synthese Web frontend.
#    @file __init__.py
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

import logging
import os

import flask
from flask import Flask
from flask import Flask, request, session, g, redirect, url_for, abort, \
     render_template, flash

import synthesepy.config
import synthesepy.env
from synthesepy import project_manager
from synthesepy.web import manager


log = logging.getLogger(__name__)


def get_application(bootstrap_config={}, project=None):
    if not project:
        assert bootstrap_config

        config = synthesepy.config.Config()

        env = synthesepy.env.create_env(
            bootstrap_config['env_type'],
            bootstrap_config['env_path'],
            bootstrap_config['mode'],
            config)

        project = project_manager.Project(bootstrap_config['project_path'], env=env)

        config.update_finished(env)

    app = Flask(__name__)
    app.project = project

    app.debug = project.config.web_debug
    app.secret_key = project.config.web_secret_key

    @app.context_processor
    def inject_context():
        return dict(
            i18n=synthesepy.i18n,
            project_name=project.config.project_name
        )

    app.register_blueprint(manager.manager, url_prefix='/manager')

    if project.manager_module and hasattr(project.manager_module, 'get_webapp'):
        app = project.manager_module.get_webapp(project, app)

    return app
