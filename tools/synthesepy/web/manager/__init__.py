#    Synthese Web Manager.
#    @file manager.py
#    @author Sylvain Pasche
#
#    This file belongs to the SYNTHESE project (public transportation specialized software)
#    Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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


import datetime
from functools import wraps
import logging
import os
from os.path import join

import flask
import werkzeug
from flask import Blueprint, render_template, abort, request, session, url_for, redirect, flash
from jinja2 import TemplateNotFound

import synthesepy.http_api
from synthesepy import i18n
from synthesepy import utils


log = logging.getLogger(__name__)

manager = Blueprint('manager', __name__, static_folder='static', template_folder='templates')


@manager.before_request
def before_request():
     if (request.endpoint not in ('manager.login', 'manager.static') and
         not session.get('logged_in')):
         return redirect(url_for('.login', next=url_for(request.endpoint, **request.view_args)))


@manager.route('/')
def index():
    return render_template('index.html')


class WrongPassword(Exception):
    pass


def synthese_login(username, password):
    http_api = synthesepy.http_api.HTTPApi(flask.current_app.project.env)
    res = http_api.call_action2('login', {
        'actionParamlogin': username,
        'actionParampwd': password,
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


@manager.route('/logout')
def logout():
    session.pop('logged_in', None)
    flash(i18n.logged_out)
    return redirect(url_for('.index'))


@manager.route('/imports')
def import_template_list():
    if not session.get('logged_in'):
        return redirect(url_for('.login', next=url_for('.imports')))
    imports_manager = flask.current_app.project.imports_manager
    templates = imports_manager.get_import_templates()

    return render_template('import_template_list.html', templates=templates)


@manager.route('/imports/<template_id>')
def import_template(template_id):
    imports_manager = flask.current_app.project.imports_manager
    try:
        template = imports_manager.get_import_template(template_id)
    except KeyError:
        abort(404)

    if not template.has_access(session['username']):
        abort(401)

    is_admin = template.has_access(session['username'], True)

    return render_template(
        'import_template.html', template=template, is_admin=is_admin)


@manager.route('/imports/<template_id>/create_import', methods=['POST'])
def import_template_create_import(template_id):
    imports_manager = flask.current_app.project.imports_manager
    try:
        template = imports_manager.get_import_template(template_id)
    except KeyError:
        abort(404)

    if not template.has_access(session['username'], True):
        abort(401)
    
    template.create_import()
    flash(i18n.import_created)

    return redirect(url_for('.import_template', template_id=template_id))


def _get_import(template_id, import_id):
    imports_manager = flask.current_app.project.imports_manager
    try:
        import_ = imports_manager.get_import(template_id, import_id)
    except KeyError:
        abort(404)

    if not import_.template.has_access(session['username']):
        abort(401)

    return import_


@manager.route('/imports/<template_id>/<import_id>')
def import_(template_id, import_id):
    import_ = _get_import(template_id, import_id)

    events = import_.events
    for event in events:
        event.time_string = i18n.format_datetime(
            datetime.datetime.fromtimestamp(event.time))

    is_admin = import_.template.has_access(session['username'], True)

    return render_template(
        'import.html', import_=import_, events=events, is_admin=is_admin)


@manager.route('/import/<template_id>/<import_id>/update', methods=['POST'])
def import_update(template_id, import_id):
    import_ = _get_import(template_id, import_id)

    f = request.form
    for param in import_.params.itervalues():
        if param.type == 'file':
            file = request.files[param.id]
            filename = werkzeug.secure_filename(file.filename)
            if not filename:
                continue
            relative_path = u'files/{0}'.format(filename)
            target_path = join(import_.path, 'files', filename)
            utils.maybe_makedirs(os.path.dirname(target_path))

            file.save(target_path)

            param.value = relative_path
        else:
            value = f.get(param.id)
            if value is not None:
                param.value = value
            
            # TODO: bool type: convert checkbox values to 0 or 1.

    import_.save_params(session['username'], f.get('no_mail'))
    flash(i18n.import_updated)

    return redirect(url_for('.import_', template_id=template_id, import_id=import_id))


@manager.route('/import/<template_id>/<import_id>/add_comment', methods=['POST'])
def import_add_comment(template_id, import_id):
    import_ = _get_import(template_id, import_id)

    import_.add_comment(session['username'], request.form['comment'])
    flash(i18n.import_comment_added)

    return redirect(url_for('.import_', template_id=template_id, import_id=import_id))


@manager.route('/import/<template_id>/<import_id>/execute', methods=['POST'])
def import_execute(template_id, import_id):
    import_ = _get_import(template_id, import_id)
    if not import_.template.has_access(session['username'], True):
        abort(401)

    f = request.form
    import_.execute(session['username'], f.get('dummy'), f.get('no_mail'))
    flash(i18n.import_executed)

    return redirect(url_for('.import_', template_id=template_id, import_id=import_id))


@manager.route('/import_execute/<template_id>/<import_id>/run/<run_id>')
def import_run_detail(template_id, import_id, run_id):
    import_ = _get_import(template_id, import_id)

    try:
        run = import_.get_run(run_id)
    except KeyError:
        abort(404)

    return render_template('import_run.html', import_=import_, run=run)
