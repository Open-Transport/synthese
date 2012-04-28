#    Synthese Checker.
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

import hashlib
import logging
import os
from os.path import join
import pdb
import socket
import subprocess
import time

import flask
from flask import Flask
from flask import Flask, helpers, request, session, g, redirect, url_for, abort, \
     render_template, flash
import requests

import synthesepy.config
import synthesepy.env
from synthesepy import project_manager
from synthesepy.web import manager
from synthesepy import utils


log = logging.getLogger(__name__)


# Checker classes


env = None
conf = {}
projects = []
name_to_project = {}


class Url(object):
    def __init__(self, project, path):
        self.path = self._convert_path(path)
        self.project = project
        self.result = None
        self.same = None
        self._diff_name = None

    def __repr__(self):
        return '<Url %s>' % self.__dict__

    def _convert_path(self, path):
        # TODO
        return path

    @property
    def ref_url(self):
        return 'http://' + self.project.reference_host + self.path

    @property
    def test_url(self):
        return 'http://' + self.project.test_host + self.path

    @property
    def result_status(self):
        return self.project.result.get_result_status(self)

    def get_result_url(self, kind):
        return self.project.result.get_result_url(self, kind)


class Result(object):
    def __init__(self, project):
        self.project = project
        self.path = join(conf['projects_base'], 'checker_results', project.name)

    def load(self, urls):
        # todo
        pass

    def _check_kind(self, kind):
        assert kind in ('ref', 'test', 'diff')

    def _get_path_hash(self, url):
        return hashlib.md5(url.path).hexdigest()

    def _get_result_path(self, url, kind=None):

        path_hash = self._get_path_hash(url)
        if kind:
            return join(self.path, path_hash, kind)
        else:
            return join(self.path, path_hash)

    def save_url_result(self, url, ref_content, test_content):
        results_path = self._get_result_path(url)
        utils.maybe_makedirs(results_path)

        open(self._get_result_path(url, 'ref'), 'wb').write(ref_content)
        open(self._get_result_path(url, 'test'), 'wb').write(test_content)

        utils.call('diff -u ref test > diff || true', shell=True, cwd=results_path)

    def save(self, urls):
        # todo
        pass

    def clear(self):
        utils.RemoveDirectory(self.path)

    def get_result_status(self, url):
        """
        None: no result available
        True: same content
        False: different content
        """
        diff_path = self._get_result_path(url, "diff")
        if not os.path.isfile(diff_path):
            return None
        return os.path.getsize(diff_path) == 0

    def get_result_url(self, url, kind):
        self._check_kind(kind)
        return url_for('project_result', project_name=self.project.name,
            path_hash=self._get_path_hash(url), kind=kind)

    def get_result_path_from_hash(self, path_hash, kind):
        self._check_kind(kind)
        return join(self.path, path_hash, kind)


class Project(object):
    def __init__(self):
        self.name = None
        self.reference_host = None
        self.test_host = None
        self.svn_project_name = None
        self.svn_url = None
        self.path = None
        self._synthese_project = None
        self.remote_db_path = None
        self.requests_session = requests.session()

    def __repr__(self):
        return '<Project %s>' % self.__dict__

    def init(self):
        if not self.reference_host:
            self.reference_host = conf['reference_host_template'].format(
                project_name=self.name)
        self.test_host = self.test_host.format(local_domain=socket.gethostname())
        if not self.svn_project_name:
            self.svn_project_name = self.name
        if not self.svn_url:
            self.svn_url = conf['svn_template'].format(
                svn_project_name=self.svn_project_name)

        if not self.path:
            self.path = join(conf['projects_base'], self.name)

        if not self.remote_db_path:
            self.remote_db_path = '/srv/synthese/{project_name}/db/config.db3'.format(
                project_name=self.name)

        self.urls = [Url(self, u) for u in self.urls]
        self._load_result()

    def checkout(self, svn_username, svn_password):
        utils.call('svn co {svn_url} --no-auth-cache --username={username} '
            '--password={password} {path}'.format(
            svn_url=self.svn_url,
            username=svn_username,
            password=svn_password,
            path=utils.to_cygwin_path(self.path)))

    @property
    def synthese_project(self):
        if self._synthese_project:
            return self._synthese_project
        try:
            self._synthese_project = project_manager.Project(self.path, env=env)
        except Exception, e:
            log.warn('Unable to retrieve synthese project: %s', e)
            self._synthese_project = None
        return self._synthese_project

    def fetch_db(self):
        self.synthese_project.env.config.remote_server = self.reference_host
        self.synthese_project.env.config.remote_db_path = self.remote_db_path
        self.synthese_project.db_remote_restore()

    def start(self):
        # FIXME: hack to let the project run on Windows.
        self.synthese_project.config.remote_server = 'localhost'

        self.synthese_project.config.bg_process_manager = 'python'
        self.synthese_project.bgstart()

    def stop(self):
        # FIXME: hack to let the project run on Windows.
        self.synthese_project.config.remote_server = 'localhost'

        self.synthese_project.config.bg_process_manager = 'python'
        self.synthese_project.bgstop()

    @property
    def running(self):
        if not self.synthese_project:
            return False
        return utils.can_connect(self.synthese_project.config.port)

    def _load_result(self):
        self.result = Result(self)
        self.result.load(self.urls)

    def _save_result(self):
        self.result.save(self.urls)

    def _compare_urls(self, url):
        log.info("Comparing urls: %s", url)
        ref_content = self.requests_session.get(url.ref_url, prefetch=True).content
        test_content = self.requests_session.get(url.test_url, prefetch=True).content
        self.result.save_url_result(url, ref_content, test_content)

    def run_checks(self):
        self.result.start_time = time.time()

        for url in self.urls:
            self._compare_urls(url)

        self.result.end_time = time.time()

    def clear_results(self):
        self.result.clear()


def load_conf(env):
    global conf, projects, name_to_project

    conf_path = join(env.c.config_path, 'checker_config.py')
    mtime = os.path.getmtime(conf_path)

    if conf.get('mtime') == mtime and len(projects) > 0:
        return
    log.info('Loading or reloading config')

    conf = {}
    projects = []
    name_to_project = {}

    conf['mtime'] = mtime
    conf['conf_path'] = conf_path
    if os.path.isfile(conf_path):
        try:
            execfile(conf_path, {}, conf)
        except Exception, e:
            # FIXME: this shouldn't depend on web stuff.
            try:
                flash('Error in configuration: %s' % e)
            except:
                pass
            log.warn("Error while reading config: %s", e)
    log.debug('Checker config: %s', conf)

    for project_config in conf.get('projects', []):
        project = Project()
        project.__dict__.update(project_config)
        project.init()
        projects.append(project)
        name_to_project[project.name] = project

    log.debug('Loaded projects: %s', projects)


# Web interface


app = Flask(__name__)

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


@app.before_request
def before_request():
    if request.endpoint and not request.endpoint.startswith("static"):
        load_conf(env)

    if ((request.endpoint and not request.endpoint.startswith("static")) and
        not request.endpoint == "login" and
        not session.get('logged_in')):
        return redirect(url_for('login'))

@app.route('/login', methods=['GET', 'POST'])
def login():
    error = None
    if request.method == 'POST':
        if request.form['password'] != conf['password']:
            error = 'Invalid password'
        else:
            session['logged_in'] = True
            flash('You were logged in')
            return redirect(url_for('index'))
    return render_template('login.html', error=error)


@app.route('/logout')
def logout():
    session.pop('logged_in', None)
    flash('You were logged out')
    return redirect(url_for('login'))

@app.route('/')
def index():
    return render_template('index.html', projects=projects)

@app.route('/config')
def config():
    config = open(conf['conf_path'], 'rb').read()

    return render_template('config.html', config=config)

@app.route('/config/update', methods=['POST'])
def config_update():
    config = request.form.get('config')

    open(conf['conf_path'], 'wb').write(config)

    flash("Config updated")
    return redirect(url_for('config'))


@app.route('/p/<project_name>')
def project(project_name):

    project = name_to_project.get(project_name)
    if not project:
        abort(404)

    return render_template('project.html', project=project)

@app.route('/p/<project_name>/logs')
def project_logs(project_name):
    project = name_to_project.get(project_name)
    if not project or not project.synthese_project:
        abort(404)

    if not os.path.isfile(project.synthese_project.config.log_file):
        abort(404)

    response = flask.current_app.make_response(
        open(project.synthese_project.config.log_file, 'rb').read())
    response.mimetype = 'text/plain'
    return response

@app.route('/p/<project_name>/result/<path_hash>/<kind>')
def project_result(project_name, path_hash, kind):
    project = name_to_project.get(project_name)
    if not project or not project.synthese_project:
        abort(404)

    result_path = project.result.get_result_path_from_hash(path_hash, kind)
    if not os.path.isfile(result_path):
        abort(404)

    response = flask.current_app.make_response(
        open(result_path, 'rb').read())
    response.mimetype =  'text/html' if 'html' in request.values else 'text/plain'
    return response

def _project_command(project_name, method, message, args=[]):
    project = name_to_project.get(project_name)
    if not project:
        abort(404)

    getattr(project, method)(*args)

    flash(message)
    return redirect(url_for('project', project_name=project.name))

@app.route('/p/<project_name>/checkout', methods=['POST'])
def project_checkout(project_name):
    return _project_command(project_name, 'checkout',
        'Project checked out or updated', args=[
            request.form.get('svn_username'),
            request.form.get('svn_password')])

@app.route('/p/<project_name>/fetch_db', methods=['POST'])
def project_fetch_db(project_name):
    return _project_command(project_name, 'fetch_db', 'Database fetched')

@app.route('/p/<project_name>/start', methods=['POST'])
def project_start(project_name):
    return _project_command(project_name, 'start', 'Project started')

@app.route('/p/<project_name>/stop', methods=['POST'])
def project_stop(project_name):
    return _project_command(project_name, 'stop', 'Project stopped')

@app.route('/p/<project_name>/run_checks', methods=['POST'])
def run_checks(project_name):
    return _project_command(project_name, 'run_checks', 'Checks Run')

@app.route('/p/<project_name>/clear_results', methods=['POST'])
def clear_results(project_name):
    return _project_command(project_name, 'clear_results', 'Results Cleared')


def run(env_, args):
    global env
    env = env_

    load_conf(env)

    app.debug = args.debug
    app.secret_key = env.config.web_secret_key
    app.run('0.0.0.0', 5002, threaded=True)
