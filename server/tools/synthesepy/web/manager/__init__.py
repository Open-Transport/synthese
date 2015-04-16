#    Synthese Web Manager.
#    @file manager.py
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


from functools import wraps
import logging
import os

import flask
from flask import abort, helpers, Blueprint, flash, redirect, render_template, \
    request, session, url_for

import synthesepy.http_api
from synthesepy import i18n
from synthesepy import utils

log = logging.getLogger(__name__)

manager = Blueprint('manager', __name__, static_folder='static',
    template_folder='templates')


@manager.before_request
def before_request():
     if (request.endpoint not in ('manager.login', 'manager.static') and
         not session.get('logged_in')):
         return redirect(url_for('.login', next=url_for(request.endpoint, **request.view_args)))


@manager.context_processor
def inject_context():
    def is_admin():
        admins = flask.current_app.project.config.web_admins
        return session.get('username') in admins

    return dict(
        is_admin=is_admin,
    )


def admin_required(f):
    @wraps(f)
    def decorated_function(*args, **kwargs):
        admins = flask.current_app.project.config.web_admins
        if session.get('username') not in admins:
            flash('Admin privilege is required')
            return redirect(url_for('.login', next=request.url))
        return f(*args, **kwargs)
    return decorated_function


@manager.route('/')
def index():
    project = flask.current_app.project
    ADMIN_LOG_LINE_COUNT = 20
    try:
        last_admin_log = utils.tail(
            open(project.admin_log_path, 'rb'), ADMIN_LOG_LINE_COUNT)
    except IOError:
        last_admin_log = "[Not available]"

    deployer = project.deployer
    deployer.refresh()

    return render_template(
        'index.html',
        last_admin_log=last_admin_log,
        config=project.config,
        deployer=deployer)


class WrongPassword(Exception):
    pass


def synthese_login(username, password):
    http_api = synthesepy.http_api.HTTPApi(flask.current_app.project.env)
    res = http_api.call_action2('login', {
        'actionParamlogin': username,
        'actionParampwd': password,
        'nr': '1',
    }, send_sid=False)
    if 'sid' not in res.cookies:
        raise WrongPassword()


@manager.route('/login', methods=['GET', 'POST'])
def login():
    error = None
    if request.method == 'POST':
        error = None
        username = request.form['username']
        password = request.form['password']

        try:
            synthese_login(username, password)
        except WrongPassword:
            error = i18n.bad_credentials
        except Exception:
            error = i18n.cant_reach_synthese

        if not error:
            session['logged_in'] = True
            session['username'] = username
            flash(i18n.logged_in)
            next = request.form.get('next')
            return redirect(next if next else url_for('.index'))
    return render_template('login.html', error=error)

# Packages static paths

def add_package_static_rule(package_name):
    def package_static(package_name):
        def send_package_static_file(filename):
            system_packages = flask.current_app.project.packages_loader.system_packages
            package_files_path = system_packages[package_name].files_path
            return helpers.send_from_directory(package_files_path, filename)

        return send_package_static_file

    endpoint_name = 'static_{0}'.format(package_name)
    manager.add_url_rule(
        '/{0}/<path:filename>'.format(endpoint_name),
        endpoint_name,
        package_static(package_name))

add_package_static_rule('core')

@manager.route('/logout')
def logout():
    session.pop('logged_in', None)
    flash(i18n.logged_out)
    return redirect(url_for('.index'))

# Management commands

def _do_command(project_fun, *args, **kwargs):
    commands_result = project_fun(*args, **kwargs)
    return render_template('commands_result.html',
        commands_result=commands_result
    )


@manager.route('/update_synthese', methods=['POST'])
@admin_required
def update_synthese():
    project = flask.current_app.project

    install_url = request.form.get('install_url')
    return _do_command(project.update_synthese, install_url)


@manager.route('/update_project', methods=['POST'])
@admin_required
def update_project():
    project = flask.current_app.project

    system_install = request.form.get('system_install') == 'on'
    load_data = request.form.get('load_data') == 'on'
    overwrite = request.form.get('overwrite') == 'on'
    return _do_command(
        project.update_project, system_install, load_data, overwrite)


@manager.route('/svn_status', methods=['POST'])
@admin_required
def svn_status():
    project = flask.current_app.project

    return _do_command(project.svn, 'status')


@manager.route('/svn_update', methods=['POST'])
@admin_required
def svn_update():
    project = flask.current_app.project

    return _do_command(
        project.svn, 'update',
        request.form.get('svn_username'), request.form.get('svn_password'))


@manager.route('/deploy', methods=['POST'])
@admin_required
def deploy():
    project = flask.current_app.project

    if request.form.get('unlock'):
        flash('Deploy unlocked')
        project.deployer.locked = False
        return redirect(url_for('.index'))

    if request.form.get('lock'):
        flash('Deploy locked')
        project.deployer.locked = True
        return redirect(url_for('.index'))

    for k in request.form.iterkeys():
        if not k.startswith('restore_'):
            continue
        dump_id = k.split('restore_')[1]
        dump = project.deployer._get_dump(dump_id)
        if not dump:
            abort(404)
        template = _do_command(project.deploy_restore, dump.id)
        project.deployer.locked = True
        return template

    return _do_command(project.deploy)

@manager.route('/deploy_logs/<dump_id>')
@admin_required
def deploy_logs(dump_id):
    project = flask.current_app.project
    dump = project.deployer._get_dump(dump_id)
    if not dump:
        abort(404)
    return render_template('result.html',
        title='Deploy %s logs' % dump.id,
        output=dump.logs)


# Database commands


@manager.route('/db')
@admin_required
def db():
    return render_template('db.html')

@manager.route('/db_clean_datasource', methods=['POST'])
@admin_required
def db_clean_datasource():
    project = flask.current_app.project
    dummy = request.form.get('dummy') == 'on'

    messages = []

    def run_sql(sql, args):
        messages.append('Running SQL: {0} args={1}'.format(sql, args))
        if not dummy:
            project.db_backend.query(sql)

    datasource_id = request.form.get('datasource_id')

    # FIXME: this shouldn't delete objects used by other sources.
    run_sql("delete from t042_commercial_lines where creator_id like '?'",
        (datasource_id,))
    run_sql("delete from t007_connection_places where code_by_source like '?'"
        (datasource_id,))
    run_sql("delete from t012_physical_stops where operator_code like '?'"
        (datasource_id,))

    return render_template('result.html',
        title='Clean Datasource Result',
        output='\n'.join(messages)
    )


# Imports management
import imports
# Ineo
import ineo
