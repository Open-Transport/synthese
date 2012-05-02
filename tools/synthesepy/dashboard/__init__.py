#    Synthese Dashboard.
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

import json
import logging
from os.path import join
import pdb

import flask
from flask import Flask
from flask import Flask, helpers, request, session, g, redirect, url_for, abort, \
     render_template, flash

import synthesepy.config
import synthesepy.env
from synthesepy import project_manager
from synthesepy.web import manager


log = logging.getLogger(__name__)
app = Flask(__name__)
env = None

def add_package_static_rule(package_name):
    def package_static(package_name):
        def send_package_static_file(filename):
            package_files_path = join(env.source_path, 'packages', package_name, 'files')
            return helpers.send_from_directory(package_files_path, filename)

        return send_package_static_file

    endpoint_name = 'static_{0}'.format(package_name)
    app.add_url_rule(
        '/{0}/<path:filename>'.format(endpoint_name),
        endpoint_name,
        package_static(package_name))

add_package_static_rule('core')

@app.route('/', defaults={'project': None, 'server': None, 'link': None})
@app.route('/p/<project>/<server>/<link>')
def dashboard(**kwargs):
    initial_state = False
    if kwargs["project"]:
        initial_state = json.dumps(kwargs)

    conf_path = join(env.c.conf_dir, 'dashboard_config.js')
    conf_local_path = join(env.c.conf_dir, 'dashboard_config_local.js')
    config = ''
    for f in (conf_path, conf_local_path):
        try:
            log.debug('Trying to read config %r', f)
            config += '\n // {0}\n\n'.format(f) + open(f, 'rb').read() + '\n'
            log.debug('read config %r', f)
        except IOError:
            pass

    return render_template('index.html', **locals())

def run(env_, args):
    global env
    env = env_

    app.debug = args.debug
    app.run(env.config.dashboard_listen_address, 5001, threaded=True)
