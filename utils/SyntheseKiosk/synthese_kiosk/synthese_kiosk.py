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
import shutil
from datetime import datetime, timedelta

import flask
from flask import g, request
import requests
try:
    from selenium import webdriver
    from selenium.webdriver.firefox import firefox_binary
except ImportError:
    print "Warning: unable to import webdrive"

import utils

thisdir = os.path.abspath(os.path.dirname(__file__))

log = logging.getLogger(__name__)

CONFIG_FILE = 'kiosk_config.json'

def get_thirdparty_binary(dir_name, fatal=True):
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
        if fatal:
            raise Exception('No binary available for %r '
                '(looked at %r)' % (dir_name, binary_path))
        else:
            return None
    return binary_path


class CustomBrowser(object):
    """
    Used for browsers that aren't supported by webdriver.
    This class replicates the webdriver API and launches the browser manually.
    """
    def __init__(self, browser_path, browser_args):
        self._path = browser_path
        self._args = browser_args
        log.debug("Browser path %s", self._path)
        self._proc = None

    def quit(self):
        log.debug("Quitting browser")
        if self._proc:
            self._proc.terminate()

    def refresh(self):
        log.debug("dummy refresh")

    def get(self, url):
        self.quit()
        cmd_line = [url if a == "URL" else a for a in self._args]
        cmd_line.insert(0, self._path)
        log.debug("Launching browser with cmdline: %s", cmd_line)
        try:
            self._proc = subprocess.Popen(cmd_line)
        except Exception, e:
            log.error("Failed to launch browser: %s", e)


class Display(object):
    def __init__(self, kiosk, index, name, url):
        config = kiosk.config
        self._kiosk = kiosk
        self._synthese_url = url
        self._index = index
        self._name = name
        self._browser_name = config['browser']
        self._browser_path = config['browser_path']
        self._browser_args = config['browser_args']
        self._debug = config['debug']
        self._browser = None
        self._url = None
        log.debug('Creating display ' + name + ' ' + self._synthese_url);

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
            log.warn('browser_path not yet supported for Chrome')
        chromedriver_path = get_thirdparty_binary('chromedriver')

        options = webdriver.ChromeOptions()
        # webdriver shares arguments between instances :-/
        options._arguments = []

        options.add_argument('--disable-translate')
        if not self._debug:
            options.add_argument('--kiosk')

        return webdriver.Chrome(
            executable_path=chromedriver_path,
            chrome_options=options)

    def _create_firefox_browser(self):
        profile = webdriver.FirefoxProfile()

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

        return webdriver.Opera(executable_path=selenium_jar)

    def _create_custom_browser(self):
        return CustomBrowser(self._browser_path, self._browser_args)

    def _create_browser(self):
        if self._browser_name == 'chrome':
            return self._create_chrome_browser()
        elif self._browser_name == 'firefox':
            return self._create_firefox_browser()
        elif self._browser_name == 'opera':
            return self._create_opera_browser()
        elif self._browser_name == 'custom':
            return self._create_custom_browser()
        else:
            raise Exception('Unsupported browser: %s', self._browser_name)

    def refresh(self, force_reload=False, new_url=None):
        old_url = self._url
        if new_url:
            self._url = new_url
        else:
            self._url = self._synthese_url

        if self._url == old_url and not force_reload:
            log.debug('Browser already has the url ' + self._url + 'loaded, not refreshing')
            return
        log.debug('Showing url: %s', self._url)
        self.browser.get(self._url)
        log.info('refreshing browser ' + self._url)
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
    'browser_args': ['URL'],
    'displays': [],
    'debug': False,
    'offline_cache_dir': None,
    'auto_refresh_timeout': 0,
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
    elif method == 'force_offline':
        log.info('Kiosk ' + request.form['display'] + " requested to be set offline and to check url '" + request.form['args'] + "'")
        g.kiosk.force_offline(True, request.form['args'])
    elif method == 'force_online':
        log.info('Kiosk ' + request.form['display'] + " requested to be set online'")
        g.kiosk.force_offline(False, None)
    return response

@admin_app.route('/screenshot/<int:index>')
def screenshot(index):
    display = g.kiosk._displays[index]
    base64_png = display.browser.get_screenshot_as_base64()
    response = flask.make_response(base64.decodestring(base64_png))
    response.mimetype = 'image/png'
    return response

def fallback(name):
    log.info('Serving file ' + g.kiosk.config['offline_cache_dir'] + "/" + name)
    if not os.path.isfile(g.kiosk.config['offline_cache_dir'] + "/" + name):
          log.error('File not found for internal http server ' + 
                    g.kiosk.config['offline_cache_dir'] + "/" + name)
          return flask.make_response()

    f = open(g.kiosk.config['offline_cache_dir'] + "/" + name)
    result = f.read()
    f.close()
    response = flask.make_response(result)
    if name.endswith('.css'):
        response.mimetype = 'text/css'
    elif name.endswith('.jpg') or name.endswith('.jpeg'):
        response.mimetype = 'image/jpeg'
    elif name.endswith('.png'):
        response.mimetype = 'image/png'
    elif name.endswith('.swf'):
        response.mimetype = 'application/x-shockwave-flash'
    else:
        response.mimetype = 'text/html'
    return response

#
# SyntheseKiosk
# -------------
#
class SyntheseKiosk(object):
    WEBAPP_PORT = 5000

    def __init__(self, config_dir=os.path.expanduser('~/.synthese_kiosk'), verbose=False):
        self._verbose = verbose
        self._config = None
        self._config_dir = config_dir
        self._config_dir = os.path.normpath(os.path.abspath(self._config_dir))
        utils.maybe_makedirs(self._config_dir)
        self._online = False
        self._force_offline = False
        self._force_offline_url_to_check = None
        self._sched = sched.scheduler(time.time, time.sleep)

        self._config_path = os.path.join(self._config_dir, 'config.json')
        self._init_logging()
        log.debug('Config path: %s', self._config_path)

        if not self.config['offline_cache_dir']:
            print "ERROR: Missing parameter 'offline_cache_dir' in your json config"
            sys.exit(1)

        self._cache_manager = CacheManager(self.config['offline_cache_dir'],
                                           self.config['synthese_url'])
        self._cache_manager.refresh_kiosk_config()
        self._kiosk_config = KioskConfig(self.config['offline_cache_dir'])
        # If we don't have the kiosk_config.json file localy, we
        # download it now.
        while not self._kiosk_config.load():
            log.info("We have no config yet, will retry later in 10s")
            time.sleep(10)
            self._cache_manager.refresh_kiosk_config()

        # Init refresh timers
        self._next_config_refresh_date = datetime.now() + \
            timedelta(seconds=self._kiosk_config.getConfigRefreshTimeout())

        self._next_fallback_refresh_date = datetime.now() + \
            timedelta(seconds=self._kiosk_config.getFallBackRefreshTimeout())

        if self.config['auto_refresh_timeout']:
            self._next_browser_refresh_date = datetime.now() + \
                timedelta(seconds=self.config['auto_refresh_timeout'])
        else:
            self._next_browser_refresh_date = None


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
        name = self.config['displays'][index]
        self.init_offline_server(name)
        self._cache_manager.refresh_offline_cache(name,
                                                  self._kiosk_config.getFallBackUrl())
        return Display(self, index, name, 
                       self._config['synthese_url'] +
                       self._kiosk_config.getDisplayUrl(name))

    def _init_displays(self):
        self._displays = []
        for index, name in enumerate(self.config['displays']):
            self._displays.append(self._init_display(index))

    def _is_url_online(self, url, timeout):
        try:
            log.debug('Trying to fetch: %s', url)
            r = requests.get(url, timeout=timeout)
        except requests.RequestException, e:
            log.warn('Exception while fetching request: %s', e)
            return False

        if r.status_code != 200:
            log.warn("Error HTTP %d returned for url '%s', go offline"
                     % (r.status_code, url))
            return False

        return True

    def _is_online(self):
        ONLINE_CHECK_TIMEOUT_S = 10
        if not self._force_offline:
            url = '{0}/?SERVICE=version&{1}'.format(
                self.config['synthese_url'], int(time.time()))
            return self._is_url_online(url, ONLINE_CHECK_TIMEOUT_S)
        elif self._force_offline_url_to_check:
            return self._is_url_online( \
                self.config['synthese_url'] + self._force_offline_url_to_check, ONLINE_CHECK_TIMEOUT_S)
        else:
            return False
        return True

    @property
    def online(self):
        return self._online

    @online.setter
    def online(self, value):
        log.debug('Setting online status to: %s', value)
        self._online = value

    def force_offline(self, value, url_to_check):
        log.debug("Setting force offline status to: %s with url '%s'" 
                  % (value, url_to_check))
        self._force_offline = value
        self._force_offline_url_to_check = url_to_check

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

    def refresh_displays(self, force_reload=False):
        if not self._displays:
            log.info('No displays configured. Open http://localhost:5000/'
                ' to configure the application.')
            return

        if self.refresh_kiosk_config_if_needed():
            force_reload = True

        self.refresh_offline_cache_if_needed()

        if self.config['auto_refresh_timeout']:
            if not self._next_browser_refresh_date:
                self._next_browser_refresh_date = datetime.now() + \
                    timedelta(seconds=self.config['auto_refresh_timeout'])

            if self._next_browser_refresh_date < datetime.now():
                self._next_browser_refresh_date = datetime.now() + \
                    timedelta(seconds=self.config['auto_refresh_timeout'])
                force_reload = True
                log.info("Refreshing browser")

        self.online = self._is_online()
        log.debug("online=" + str(self.online))
        for display in self._displays:
            if self.online:
                display.refresh(force_reload,
                                self._config['synthese_url'] +
                                self._kiosk_config.getDisplayUrl(display._name))
            else:
                display.refresh(force_reload,
                                'http://localhost:' + str(self.WEBAPP_PORT) +
                                '/fallback/' + display._name + self._kiosk_config.getFallBackUrl())

        OFFLINE_POLL_INTERVAL_S = self._kiosk_config.getFallBackTimeout()

        self._refresh_sched_event = self._sched.enter(
            OFFLINE_POLL_INTERVAL_S, 1, self.refresh_displays, ())


    def update_config(self, old_config):
        if (self.config['debug'] != old_config['debug'] or
            self.config['offline_cache_dir'] != old_config['offline_cache_dir'] or
            self.config['auto_refresh_timeout'] != old_config['auto_refresh_timeout'] or
            self.config['browser'] != old_config['browser'] or
            self.config['browser_path'] != old_config['browser_path'] or
            self.config['browser_args'] != old_config['browser_args'] or
            self.config['synthese_url'] != old_config['synthese_url'] or
            len(self.config['displays']) != len(old_config['displays'])):
            for display in self._displays:
                display.stop()
            self._init_displays()
            self.refresh_displays()
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
        ports = [self.WEBAPP_PORT]

        for port in ports:
            utils.kill_listening_processes(port)
        self.start_admin_app()
        self.refresh_displays()

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

        sys.exit(1)

    def init_offline_server(self, display):
        admin_app.add_url_rule('/fallback/<path:name>', 'fallback', fallback)

    def refresh_kiosk_config_if_needed(self):
        """ Return True if the config has changed """
        config_changed = False
        if self._next_config_refresh_date < datetime.now():
            self._next_config_refresh_date = datetime.now() + \
                timedelta(seconds=self._kiosk_config.getConfigRefreshTimeout())
            log.info("Refreshing the kiosk config")
            self._cache_manager.refresh_kiosk_config()

            # Load the new configuration
            config_changed = self._kiosk_config.reload()

            if config_changed:
                # The fallback url has changed, wget this site now
                self.refresh_offline_cache_if_needed()

        return config_changed

    def refresh_offline_cache_if_needed(self):
        if self._next_fallback_refresh_date < datetime.now():
            self._next_fallback_refresh_date = datetime.now() + \
                timedelta(seconds=self._kiosk_config.getFallBackRefreshTimeout())
            log.info("Refreshing offline cache")
            for display in self._displays:
                self._cache_manager.refresh_offline_cache(display._name,
                                                          self._kiosk_config.getFallBackUrl())

class CacheManager(object):

    def __init__(self, cache_dir, synthese_url):
        self._cache_dir = cache_dir
        self._synthese_url = synthese_url

    def _wget(self, directory, url):
        log.debug('running wget in ' + directory + " for url " + url)
        utils.maybe_makedirs(directory)
        cmd_line = ["wget"]
        cmd_line.append('--recursive')
        cmd_line.append('--quiet')
        cmd_line.append('--no-host-directories')
        cmd_line.append('--no-parent')
        cmd_line.append('--page-requisites')
        cmd_line.append('--convert-links')
        cmd_line.append('--no-verbose')
        cmd_line.append('--directory-prefix')
        cmd_line.append(directory)
        cmd_line.append(url)
        log.debug('Wget command line: %s', cmd_line)
        try:
            retval = subprocess.call(cmd_line)
            # Return True if the wget succeeded
            return retval == 0
        except Exception, e:
            log.error("Failed to launch wget: %s", e)

        return False

    def refresh_offline_cache(self, display, url):
        log.debug("refresh_offline_cache " + display + url)
        cache_dir = self._cache_dir + "/" + display
        if self._wget(cache_dir + "_NEW",
                      self._synthese_url + url):
            if os.path.isdir(cache_dir):
                os.rename(cache_dir, cache_dir + "_TODELETE")
            os.rename(cache_dir + "_NEW", cache_dir)
            if os.path.isdir(cache_dir + "_TODELETE"):
                shutil.rmtree(cache_dir + "_TODELETE")
        else:
            # Remove the uncomplete wget directory
            if os.path.isdir(cache_dir + "_NEW"):
                shutil.rmtree(cache_dir + "_NEW")


    def refresh_kiosk_config(self):
        log.debug("refresh_kiosk_config")
        cache_file = self._cache_dir + "/" + CONFIG_FILE
        if self._wget(self._cache_dir,
                   self._synthese_url + "/" + CONFIG_FILE):
            if os.path.isfile(cache_file + '.1'):
                os.rename(cache_file + '.1', cache_file)

class KioskConfig(object):

    def __init__(self, cache_dir):
        self._cache_dir = cache_dir
        self._config = None

    def _load(self):
        """
        Load the config file on disk and return a tupple of 2 boolean
        (loaded, changed)
        """
        previous_config = self._config
        config_file = self._cache_dir + "/" + CONFIG_FILE
        log.debug("load_kiosk_config " + config_file)
        if not os.path.isfile(config_file):
            print "Configuration file " + config_file + " not found"
            return (False, previous_config != self._config)

        try:
            self._config = (json.load(open(config_file)))
            return (True, previous_config != self._config)
        except Exception, e:
            log.error("Failed to load json config file '%s': %s", config_file, e)
            return (False, previous_config != self._config)

    def load(self):
        """ Return True if the configuration has been loaded """
        return self._load()[0]

    def reload(self):
        """ return True if the configuration has changed """
        return self._load()[1]

    def getFallBackUrl(self):
        try:
            return self._config['defaults']['fallbackUrl']
        except:
            return None

    def getFallBackTimeout(self):
        try:
            return self._config['defaults']['fallbackTimeout']
        except:
            return 10

    def getFallBackRefreshTimeout(self):
        try:
            return self._config['defaults']['fallbackRefreshTimeout']
        except:
            return 300

    def getConfigRefreshTimeout(self):
        try:
            return self._config['defaults']['configRefreshTimeout']
        except:
            return 60

    def getDisplayUrl(self, display):
        log.debug('getDisplayUrl ' + display + ":" + self._config['displays'][display]['url'])
        try:
            return self._config['displays'][display]['url']
        except:
            return None
