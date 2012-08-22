#    Synthese Kiosk.
#    @file synthese_kiosk.py
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


import base64
import hashlib
import json
import logging
import os
import platform
import random
import sched
import shutil
import socket
import string
import subprocess
import sys
import threading
import time

import flask
from flask import g, request
import requests
from selenium import webdriver
from selenium.webdriver.firefox import firefox_binary

import utils

thisdir = os.path.abspath(os.path.dirname(__file__))

log = logging.getLogger(__name__)


def get_thirdparty_binary(dir_name):
    suffix = ''
    if sys.platform == 'win32':
        platform_dir = 'win'
        suffix = '.exe'
    elif sys.platform == 'linux2':
        if platform.architecture()[0] == '64bit':
            platform_dir = 'lin64'
        elif platform.architecture()[0] == '32bit':
            platform_dir = 'lin32'
        else:
            raise Exception('Unsupported architecture')
    else:
        raise Exception('Unsupported platform')

    binary_path = os.path.normpath(
        os.path.join(
            thisdir, os.pardir, 'third_party', dir_name,
            platform_dir, dir_name + suffix))

    if not os.path.isfile(binary_path):
        raise Exception('No binary available for %r '
            '(looked at %r)' % (dir_name, binary_path))
    return binary_path


class Proxy(object):
    """Class to manage the Polipo proxy process"""

    def __init__(self, config_dir):
        self._online = False
        self._running = False
        self._cache_dir = os.path.join(config_dir, 'polipo_cache')
        self._log_file = os.path.join(config_dir, 'polipo.log')
        utils.maybe_makedirs(self._cache_dir)
        self._host = 'localhost'
        self._port = 8123
        self._proc = None
        self._polipo_path = get_thirdparty_binary('polipo')

    def _ensure_stopped(self):
        if sys.platform == 'win32':
            subprocess.call('taskkill /f /im polipo.exe', shell=True)
        else:
            subprocess.call('pkill polipo', shell=True)

    def _convert_polipo_path(self, path):
        # Polipo on Windows expects Unix style paths, without the drive letter
        if sys.platform != 'win32':
            return path
        log.debug('Path %s, cwd: %s', path, os.getcwd())
        if (os.path.splitdrive(path)[0].lower() !=
            os.path.splitdrive(os.getcwd())[0].lower()):
            raise Exception('Wrong drive expectations')
        return os.path.splitdrive(path)[1].replace(os.sep, '/')

    def start(self):
        self._ensure_stopped()
        self._running = True

        options = {
            'diskCacheRoot': self._convert_polipo_path(self._cache_dir),
            'dnsUseGethostbyname': 'happily',
            'logFile': self._convert_polipo_path(self._log_file),
        }
        if not self._online:
            options['proxyOffline'] = 'true'

        cmd_line = [self._polipo_path]
        for name, value in options.iteritems():
            cmd_line.append('{0}={1}'.format(name, value))
        log.debug('Polipo command line: %s', cmd_line)

        self._proc = subprocess.Popen(cmd_line)

        time.sleep(2)
        if self._proc.poll() is not None:
            raise Exception('Failed to start polipo')

    def stop(self):
        if self._proc:
            self._proc.terminate()
        self._running = False

    @property
    def online(self):
        return self._online

    @online.setter
    def online(self, value):
        old_online = self._online
        self._online = value
        if not self._running or old_online != self._online:
            self.start()

    def clear_cache(self):
        self.stop()
        time.sleep(1)
        try:
            shutil.rmtree(self._cache_dir)
        except IOError, e:
            pass
        utils.maybe_makedirs(self._cache_dir)
        self.start()


class Display(object):
    def __init__(self, kiosk, index, name):
        config = kiosk.config
        self._kiosk = kiosk
        self._proxy = kiosk._proxy
        self._synthese_url = config['synthese_url']
        self._index = index
        self._name = name
        self._browser_name = config['browser']
        self._browser_path = config['browser_path']
        self._debug = config['debug']
        self._browser = None
        self._url = None

    def __str__(self):
        return '<Display %s>' % self.__dict__

    @property
    def browser(self):
        if self._browser:
            return self._browser
        self._browser = self._create_browser()
        return self._browser

    def _create_chrome_browser(self):
        if self._browser_path:
            raise Exception('browser_path not yet supported for Chrome')
        chromedriver_path = get_thirdparty_binary('chromedriver')

        options = webdriver.ChromeOptions()
        options.add_argument('--disable-translate')
        if not self._debug:
            options.add_argument('--kiosk')

        options.add_argument(
            '--proxy-server=http://{_host}:{_port}'.format(**self._proxy.__dict__))

        return webdriver.Chrome(
            executable_path=chromedriver_path,
            chrome_options=options)

    def _create_firefox_browser(self):
        profile = webdriver.FirefoxProfile()

        profile.set_preference('network.proxy.type', 1);
        profile.set_preference('network.proxy.http', self._proxy._host)
        profile.set_preference('network.proxy.http_port', self._proxy._port)
        xpi_path = os.path.join(
            thisdir, os.pardir, 'third_party', 'r_kiosk-0.9.0-fx.xpi')
        if not self._debug:
            profile.add_extension(xpi_path)

        binary = firefox_binary.FirefoxBinary(firefox_path=self._browser_path)

        return webdriver.Firefox(firefox_profile=profile, firefox_binary=binary)

    def _create_opera_browser(self):
        if self._browser_path:
            raise Exception('browser_path not yet supported for Chrome')

        selenium_jar = os.path.join(
            thisdir, os.pardir, 'third_party', 'selenium-server-standalone-2.18.0.jar')
        if not os.path.isfile(selenium_jar):
            raise Exception('Please download '
                'http://selenium.googlecode.com/files/selenium-server-standalone-2.18.0.jar '
                'to %r' % selenium_jar)

        # TODO: proxy.

        return webdriver.Opera(executable_path=selenium_jar)

    def _create_browser(self):
        if self._browser_name == 'chrome':
            return self._create_chrome_browser()
        elif self._browser_name == 'firefox':
            return self._create_firefox_browser()
        elif self._browser_name == 'opera':
            return self._create_opera_browser()
        else:
            raise Exception('Unsupported browser: %s', self._browser_name)

    def refresh(self, force_reload=False):
        KIOSK_PATH = '/kiosk/kiosk.html'
        kiosk_params = {
            'display': self._name,
        }
        if self._debug:
            kiosk_params['debug'] = True
        if not self._kiosk.online:
            kiosk_params['offline'] = True

        kiosk_url = '{0}{1}#{2}'.format(
            self._synthese_url, KIOSK_PATH, json.dumps(kiosk_params))

        old_url = self._url
        self._url = kiosk_url
        if self._url == old_url and not force_reload:
            log.debug('Browser already has the url loaded, not refreshing')
            return
        log.debug('Showing url: %s', self._url)
        self.browser.get(self._url)
        self.browser.refresh()

    def stop(self):
        self.browser.quit()


def gen_password():
    chars = string.letters + string.digits
    length = 8
    return ''.join([random.choice(chars) for i in range(length)])


DEFAULT_CONFIG = {
    'kiosk_name': socket.gethostname(),
    'synthese_url': None,
    'admin_password': gen_password(),
    'secret_key': gen_password(),
    'browser': 'firefox',
    'browser_path': None,
    'displays': [],
    'debug': False,
}


# Flask Admin app.


admin_app = flask.Flask(__name__)

@admin_app.before_request
def before_request():
    g.kiosk = flask.current_app.kiosk
    if (request.remote_addr != '127.0.0.1' and
        request.endpoint not in ('login', 'static') and
        not flask.session.get('logged_in')):
        return flask.redirect(flask.url_for('login'))

@admin_app.route('/login', methods=['GET', 'POST'])
def login():
    error = None
    if request.method == 'POST':
        if request.form['password'] != g.kiosk.config['admin_password']:
            error = 'Invalid password'
        else:
            flask.session['logged_in'] = True
            flask.flash('You were logged in')
            return flask.redirect(flask.url_for('index'))
    return flask.render_template('login.html', error=error)

@admin_app.route('/logout')
def logout():
    flask.session.pop('logged_in', None)
    flask.flash('You were logged out')
    return flask.redirect(flask.url_for('login'))

@admin_app.route('/')
def index():
    return flask.render_template('index.html')

@admin_app.route('/get_config')
def get_config():
    return flask.jsonify(g.kiosk.config)

@admin_app.route('/set_config', methods=['POST'])
def set_config():
    log.debug('Setting config from web app')
    old_config = g.kiosk.config.copy()
    config = json.loads(request.form['data'])
    g.kiosk.config = config
    g.kiosk.update_config(old_config)
    return flask.jsonify(g.kiosk.config)

@admin_app.route('/get_status')
def get_status():
    return flask.jsonify(g.kiosk.status)

@admin_app.route('/force_reload', methods=['POST'])
def force_reload():
    g.kiosk.refresh_displays(force_reload=True)
    flask.flash('Forced Display Reload')
    return flask.redirect(flask.url_for('index'))

@admin_app.route('/clear_proxy_cache', methods=['POST'])
def clear_proxy_cache():
    g.kiosk._proxy.clear_cache()
    flask.flash('Cleared Proxy Cache')
    return flask.redirect(flask.url_for('index'))

@admin_app.route('/stop_kiosk', methods=['POST'])
def stop_kiosk():
    g.kiosk.stop()
    flask.flash('Kiosk Stopped')
    return flask.redirect(flask.url_for('index'))

@admin_app.route('/display_rpc', methods=['POST'])
def display_rpc():
    response = flask.make_response()
    response.headers['Access-Control-Allow-Origin'] = '*'
    method = request.form['method']
    if method == 'message':
        level, message = request.form['args'].split(':', 1)
        getattr(log, level)('Browser message: %s', message)
    return response

@admin_app.route('/screenshot/<int:index>')
def screenshot(index):
    display = g.kiosk._displays[index]
    base64_png = display.browser.get_screenshot_as_base64()
    response = flask.make_response(base64.decodestring(base64_png))
    response.mimetype = 'image/png'
    return response


class SyntheseKiosk(object):
    WEBAPP_PORT = 5000

    def __init__(self, config_dir=os.path.expanduser('~/.synthese_kiosk'), verbose=False):
        self._verbose = verbose
        self._config = None
        self._config_dir = config_dir
        self._config_dir = os.path.normpath(os.path.abspath(self._config_dir))
        utils.maybe_makedirs(self._config_dir)
        self._online = False
        self._proxy = Proxy(self._config_dir)
        self._sched = sched.scheduler(time.time, time.sleep)

        self._config_path = os.path.join(self._config_dir, 'config.json')
        self._init_logging()
        log.debug('Config path: %s', self._config_path)
        self._init_displays()

    def _init_logging(self):
        self._log_path = os.path.join(self._config_dir, 'logs.txt')

        logger = logging.root
        formatter = logging.Formatter('%(asctime)s:%(levelname)s:%(message)s')

        file_handler = logging.FileHandler(self._log_path)
        file_handler.setFormatter(formatter)
        logger.addHandler(file_handler)

        if self._verbose:
            stream_handler = logging.StreamHandler()
            stream_handler.setFormatter(formatter)
            logger.addHandler(stream_handler)

        logger.setLevel(logging.DEBUG if self._verbose else logging.INFO)

        logging.getLogger('werkzeug').setLevel(logging.WARN)

    def _init_display(self, index):
        c = self.config
        name = c['displays'][index]
        return Display(self, index, name)

    def _init_displays(self):
        self._displays = []
        for index, name in enumerate(self.config['displays']):
            self._displays.append(self._init_display(index))

    def _is_online(self):
        url = '{0}/online_check?{1}'.format(
            self.config['synthese_url'], int(time.time()))
        ONLINE_CHECK_TIMEOUT_S = 10

        try:
            log.debug('Trying to fetch: %s', url)
            requests.get(url, timeout=ONLINE_CHECK_TIMEOUT_S)
        except requests.RequestException, e:
            log.warn('Exception while fetching request: %s', e)
            return False
        return True

    @property
    def online(self):
        return self._online

    @online.setter
    def online(self, value):
        log.debug('Setting online status to: %s', value)
        self._online = value
        self._proxy.online = value

    @property
    def config(self):
        if self._config:
            return self._config

        if not os.path.isfile(self._config_path):
            self.config = DEFAULT_CONFIG

        self._config = DEFAULT_CONFIG.copy()
        self._config.update(json.load(open(self._config_path)))
        return self._config

    @config.setter
    def config(self, value):
        json.dump(
            value, open(self._config_path, 'wb'),
            sort_keys=True, indent=4)
        # Let it be reloaded in the getter in order to load defaults.
        self._config = None

    @property
    def status(self):
        if not os.path.isfile(self._log_path):
            logs = 'Not available'
        else:
            logs = ''.join(open(self._log_path).readlines()[:-1000:-1])

        return {
            'online': self.online,
            'logs': unicode(logs, errors='ignore'),
        }

    def refresh_displays(self, poll_if_offline=False, force_reload=False):
        if not self._displays:
            log.info('No displays configured. Open http://localhost:5000/'
                ' to configure the application.')
            return

        self.online = self._is_online()
        for display in self._displays:
            display.refresh(force_reload)

        if not self.online:
            OFFLINE_POLL_INTERVAL_S = 10

            self._refresh_sched_event = self._sched.enter(
                OFFLINE_POLL_INTERVAL_S, 1, self.refresh_displays, (True,))

    def update_config(self, old_config):
        if (self.config['debug'] != old_config['debug'] or
            self.config['browser'] != old_config['browser'] or
            self.config['browser_path'] != old_config['browser_path'] or
            self.config['synthese_url'] != old_config['synthese_url'] or
            len(self.config['displays']) != len(old_config['displays'])):
            for display in self._displays:
                display.stop()
            self._init_displays()
            self.refresh_displays(True)
            return

        for index, name in enumerate(self.config['displays']):
            display = self._displays[index]
            if display._name == name:
                continue
            log.info('Reloading display at index=%s, name=%s', index, name)
            display.stop()
            self._displays[index] = self._init_display(index)

        self.refresh_displays()

    def start_admin_app(self):
        # In debug mode, the built-in server doesn't like to be run in a thread
        # when it tries to reload itself.
        # Set this to True when debugging the web app. In this case, other
        # things won't work.
        DEBUG_WEBAPP = False

        admin_app.config['DEBUG'] = DEBUG_WEBAPP
        admin_app.secret_key = self.config['secret_key'].encode('utf-8')
        admin_app.kiosk = self

        webapp_args = dict(host='0.0.0.0', port=self.WEBAPP_PORT, threaded=True)
        if self.config['debug'] and DEBUG_WEBAPP:
            admin_app.run(**webapp_args)
        else:
            t = threading.Thread(
                target=admin_app.run,
                kwargs=webapp_args)
            t.daemon = True
            t.start()

    def start(self):
        ports = [self._proxy._port, self.WEBAPP_PORT]
        for port in ports:
            utils.kill_listening_processes(port)
        self.start_admin_app()
        self.refresh_displays(True)

        log.info('Starting scheduler')
        self._sched.run()
        log.info('No more events.')
        while True:
            time.sleep(2)

        self.stop()

    def stop(self):
        # TODO: stop admin app.

        for display in self._displays:
            display.stop()

        self._proxy.stop()
        sys.exit(1)
