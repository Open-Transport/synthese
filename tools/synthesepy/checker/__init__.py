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

import copy
import datetime
import hashlib
import logging
import os
from os.path import join
import pdb
import re
import socket
import subprocess
import time
import urllib
import urlparse

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
    def __init__(self, project, data):
        self.kind = 'url'
        self.project = project
        self.result = None
        self.same = None
        self._diff_name = None
        self._site_to_id = None
        self.username = None
        self.password = None

        self.label = None
        path = data
        if isinstance(data, (list, tuple)):
            self.label, path = data
        if path.startswith('**'):
            self.kind = 'category'
            self.label = path[2:]

        self._parse_path(path)

    def __repr__(self):
        return '<Url %s>' % self.__dict__

    def _get_site_id(self, site_name):
        if self._site_to_id is not None:
            return self._site_to_id.get(site_name)

        self._site_to_id = {}
        if self.project.synthese_project:
            for site in self.project.synthese_project.sites:
                self._site_to_id[site.name] = site.id
        return self._site_to_id.get(site_name)

    def _parse_path(self, path):
        username, site_name, path = re.match('(?:<([^>]+)>)?(?:\[([^\]]+)\])?(.*)', path).groups()
        if username:
            self.username = username
            self.password = self.project.passwords.get(username, '')

        self.site_name = site_name if site_name else 'N/A'

        # Variables expansion
        dt = datetime.datetime
        today = dt.strftime(dt.today(), '%Y-%m-%d')
        path = path.format(today=today)

        if not site_name:
            self.path = path
            return

        site_id = self._get_site_id(site_name)
        if not site_id:
            self.path = 'Check out project first!'
            return

        path_only = urlparse.urlparse(path).path
        qs = dict(urlparse.parse_qsl(urlparse.urlparse(path).query))
        qs.update(dict(
            SERVICE='page',
            si=site_id,
            smart_url=path_only,
        ))

        self.path = '{alias_path}?{qs}'.format(
            alias_path=self.project.alias_path, qs=urllib.urlencode(qs))

    @property
    def ref_url(self):
        return 'http://' + self.project.reference_host + self.path

    @property
    def test_url(self):
        return 'http://' + self.project.test_host + self.path

    @property
    def localhost_url(self):
        return 'http://' + self.project.localhost_host + self.path

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
        assert kind in ('ref', 'test', 'diff', 'diff_space', 'rerun')

    def _get_path_hash(self, url):
        return hashlib.md5(url.path).hexdigest()

    def _get_result_path(self, url, kind=None):
        path_hash = self._get_path_hash(url)
        if kind:
            return join(self.path, path_hash, kind)
        else:
            return join(self.path, path_hash)

    def check_url(self, url, sid_cache={}):
        results_path = self._get_result_path(url)
        utils.RemoveDirectory(results_path)
        utils.maybe_makedirs(results_path)

        def get_sid(url, host, u):
            key = '{0}:{1}'.format(host, url.username)
            if key in sid_cache:
                return sid_cache[key]

            login_url = 'http://' + host + self.project.alias_path
            r = requests.post(login_url, data={
                'a': 'login',
                'actionParamlogin': url.username,
                'actionParampwd': url.password,
                'nr': '1',
            }, stream=False)

            if 'sid' not in r.cookies:
                raise Exception('Authentication failure')
            sid = r.cookies['sid']
            sid_cache[key] = sid
            return sid_cache[key]

        def fetch(url, host, u):
            sid = None
            if url.username:
                sid = get_sid(url, host, u)
                u_parts = urlparse.urlparse(u)
                qs = dict(urlparse.parse_qsl(u_parts.query))
                qs['sid'] = sid
                u_parts = list(u_parts)
                u_parts[4] = urllib.urlencode(qs)
                u = urlparse.urlunparse(u_parts)

            return self.project.requests_session.get(u, stream=False), sid

        def save(url, host, u, kind):
            result = None
            content = ''
            try:
                result, sid = fetch(url, host, u)
            except Exception, e:
                content = '<pre>Exception while fetching {0}:\n\n{1}\n</pre>\n'.format(u, e)
            if result is not None:
                content = '<!--'
                if result.status_code != 200:
                    content += 'WARNING: http status for {0} is not 200\n'.format(u)
                content += 'http status: {0}-->'.format(result.status_code)
                # The session identifier (sid) might be different, so it is
                # replaced to something common.
                c = result.content
                if sid:
                    c = c.replace(sid, '__SID__')
                content += c
            open(self._get_result_path(url, kind), 'wb').write(content)
            return content

        ref_content = save(url, url.project.reference_host, url.ref_url, 'ref')
        test_content = save(url, url.project.test_host, url.test_url, 'test')

        utils.call('diff -u ref test > diff || true', shell=True, cwd=results_path)
        # Diff with options to ignore space doesn't ignore changes spread
        # accross several lines. Thus, they are compared manually here.
        utils.call('touch diff_space', shell=True, cwd=results_path)
        ref_content_nospace = re.sub('\s', '', ref_content)
        test_content_nospace = re.sub('\s', '', test_content)
        if ref_content_nospace != test_content_nospace:
            utils.call('diff -u -EbwB ref test > diff_space || true',
                shell=True, cwd=results_path)

    def save(self, urls):
        # todo
        pass

    def clear(self):
        utils.RemoveDirectory(self.path)

    def get_result_status(self, url):
        """
        'not_avail': not available
        'same': same content
        'diff_space': differs by space only
        'diff: differs not only by space
        """

        diff_space_path = self._get_result_path(url, 'diff_space')
        if not os.path.isfile(diff_space_path):
            return 'not_avail'
        if os.path.getsize(diff_space_path) > 0:
            return 'diff'
        diff_path = self._get_result_path(url, 'diff')
        if not os.path.isfile(diff_path):
            return 'not_avail'
        if os.path.getsize(diff_path) > 0:
            return 'diff_space'
        return 'same'

    def get_result_url(self, url, kind):
        self._check_kind(kind)
        return url_for('project_result', project_name=self.project.name,
            path_hash=self._get_path_hash(url), kind=kind)

    def get_result_path_from_hash(self, path_hash, kind):
        self._check_kind(kind)
        return join(self.path, path_hash, kind)

    def get_url_from_hash(self, path_hash):
        for url in self.project.urls:
            if self._get_path_hash(url) == path_hash:
                return url
        return None


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
        self.alias_path = None
        self.after_fetch_sql = None
        self.common_project_config = {}
        self.passwords = {}

    def __repr__(self):
        return '<Project %s>' % self.__dict__

    def init(self):
        if not self.reference_host:
            self.reference_host = conf['reference_host_template'].format(
                project_name=self.name)
        self.test_host = self.test_host.format(local_domain=socket.gethostname())
        self.localhost_host = self.test_host.format(local_domain='localhost')
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

        if not self.alias_path:
            self.alias_path = '/synthese'

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
            # Note: copy the env to avoid sharing the same config, which is
            # modified by projects.
            env_copy = copy.deepcopy(env)
            self._synthese_project = project_manager.Project(
                self.path, env=env_copy)
            env_copy.config.__dict__.update(
                conf.get('common_project_config', {}))
            env_copy.config.update_finished(env_copy)
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

    def run_checks(self):
        self.result.start_time = time.time()

        sid_cache = {}
        for url in self.urls:
            if url.kind != 'url':
                continue
            self.result.check_url(url, sid_cache)

        self.result.end_time = time.time()

    def clear_results(self):
        self.result.clear()


def load_conf(env):
    global conf, projects, name_to_project

    conf_path = join(env.c.conf_dir, 'checker_config.py')
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
                flash('Error in configuration: %s' % e, 'error')
            except:
                pass
            log.warn('Error while reading config: %s', e)
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
    if request.endpoint and not request.endpoint.startswith('static'):
        load_conf(env)

    if ((request.endpoint and not request.endpoint.startswith('static')) and
        not request.endpoint == 'login' and
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
    config = unicode(open(conf['conf_path'], 'rb').read(), 'utf-8')

    return render_template('config.html', config=config)

@app.route('/config/update', methods=['POST'])
def config_update():
    config = request.form.get('config')

    if not config:
        abort(500)

    open(conf['conf_path'], 'wb').write(config.encode('utf-8'))

    flash('Config updated')
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
    response.mimetype =  'text/plain' if kind.startswith('diff') else 'text/html'
    if 'plain' in request.values:
        response.mimetype =  'text/plain'
    return response

@app.route('/p/<project_name>/result/<path_hash>/rerun', methods=['POST'])
def project_result_rerun(project_name, path_hash):
    project = name_to_project.get(project_name)
    if not project or not project.synthese_project:
        abort(404)

    url = project.result.get_url_from_hash(path_hash)
    if not url:
        abort(404)

    project.result.check_url(url)

    flash('Result reexecuted')
    return redirect(url_for('project', project_name=project.name))

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
    project = name_to_project.get(project_name)
    if not project or not project.synthese_project:
        abort(404)

    project.fetch_db()
    if project.after_fetch_sql:
        project.synthese_project.db_backend.import_sql(project.after_fetch_sql)
    project.stop()
    project.start()

    flash('Database fetched. Project was restarted.')
    return redirect(url_for('project', project_name=project.name))

@app.route('/p/<project_name>/run_after_fetch_sql', methods=['POST'])
def project_run_after_fetch_sql(project_name):
    project = name_to_project.get(project_name)
    if not project or not project.synthese_project:
        abort(404)

    if project.after_fetch_sql:
        project.synthese_project.db_backend.import_sql(project.after_fetch_sql)

    flash('SQL Executed. You may want to restart Synthese.')
    return redirect(url_for('project', project_name=project.name))

@app.route('/p/<project_name>/start', methods=['POST'])
def project_start(project_name):
    return _project_command(project_name, 'start', 'Project started')

@app.route('/p/<project_name>/stop', methods=['POST'])
def project_stop(project_name):
    return _project_command(project_name, 'stop', 'Project stopped')

@app.route('/p/<project_name>/run_sql', methods=['POST'])
def project_run_sql(project_name):
    project = name_to_project.get(project_name)
    if not project or not project.synthese_project:
        abort(404)

    sql = request.form.get('sql')

    try:
        return unicode(project.synthese_project.db_backend.shell(sql), 'utf-8')
    except Exception, e:
        return unicode(e)

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
