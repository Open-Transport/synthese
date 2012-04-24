#    Imports management.
#    @file imports.py
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

import collections
import gzip
import json
import logging
import os
from os.path import join
import socket
import time
import traceback
import urllib

import requests

import synthesepy.http_api
from synthesepy import i18n
from synthesepy import utils
from synthesepy.third_party.ordered_dict import OrderedDict

log = logging.getLogger(__name__)


class ImportsManager(object):
    def __init__(self, project):
        self.project = project
        self.templates = None

    def _load_templates(self):
        # TODO: detect if the config file changed, and reload.
        if self.templates is not None:
            return

        self.templates = OrderedDict()
        config_path = join(self.project.path, 'manager', 'imports_config.py')
        config = {
            # Give access to the project
            'project': self.project,
        }
        if os.path.isfile(config_path):
            execfile(config_path, {
                'project': self.project,
            }, config)
        if 'import_templates' not in config:
            log.warn('No imports config')
            return
        for template_config in config['import_templates']:
            template = ImportTemplate(self, template_config)
            self.templates[template.id] = template

    def get_import_templates(self):
        self._load_templates()
        # TODO: sort by key
        return self.templates.values()

    def get_import_template(self, template_id):
        self._load_templates()
        return self.templates[template_id]

    def get_import(self, template_id, import_id):
        import_template = self.get_import_template(template_id)
        return import_template.get_import(import_id)


class ImportTemplate(utils.DirObjectLoader):
    def __init__(self, manager, config):
        self.manager = manager

        # default config
        self.after_create = None
        self.do_import = None
        self.run_results_notifications = {}
        self.admins = []
        self.uploaders = []

        self.__dict__.update(config)

        def parse_emails(emails):
            username_to_emails = dict((username, [email_or_emails] if
                isinstance(email_or_emails, basestring) else list(email_or_emails)) for
                (username, email_or_emails) in emails)
            emails = sum(username_to_emails.itervalues(), [])
            return username_to_emails, set(emails)

        if not self.admins:
            self.admins = [('root', manager.project.config.mail_admins)]
        self.admins, self.admin_emails = parse_emails(self.admins)
        self.uploaders, self.uploader_emails = parse_emails(self.uploaders)

        if not hasattr(self, 'label'):
            self.label = self.id
        if not isinstance(self.label, unicode):
            self.label = unicode(self.label, 'utf-8')

        self._imports = None
        self.path = join(self.manager.project.path, 'imports', self.id)
        utils.maybe_makedirs(self.path)

    def _load_imports(self):
        if self._imports is not None:
            return

        self._imports = self.load_from_dir(self.path, Import, self)

    def get_imports(self):
        self._load_imports()
        return self._imports.values()
    imports = property(get_imports)

    def get_import(self, import_id):
        self._load_imports()
        if import_id == ':latest':
            import_id = str(max(int(id) for id in self._imports.keys()))

        return self._imports[import_id]

    def create_import(self, args={}):
        self._load_imports()
        import_ = self.create_object(self._imports, self.path, Import, self)
        if self.after_create:
            self.after_create(import_, args)
        return import_

    def has_access(self, username, admin_only=False):
        if username == 'root':
            return True
        if username in self.admins:
            return True
        if admin_only:
            return False
        return username in self.uploaders


class Param(object):
    def __init__(self):
        self.id = None
        self.label = None
        self.type = 'string'
        self.default = None
        self.value = None
        self.no_send = False
        self.uploader_visible = False
        self.uploader_readonly = False

    def __repr__(self):
        return '<Param %s>' % self.__dict__


class Event(object):
    def __init__(self):
        self.username = None
        self.time = time.time()
        self.type = 'general'
        self.content = ''


class ImportException(Exception):
    pass


class ImportRun(object):
    # Ordered from less important to most important.
    LEVEL_NAMES = ('unknown', 'crea', 'load', 'info', 'warn', 'noti', 'err')

    def __init__(self, import_, id, path):
        self.import_ = import_
        self.id = id
        self.path = path
        self.state_path = join(self.path, 'state.json')
        self._summary = None
        self.summary_path = join(self.path, 'summary.txt.gz')
        self._log = None
        self.log_path = join(self.path, 'log.txt.gz')

        self.successful = True
        self.date = 0
        self.execution_time = -1
        self.dummy = False
        self.messages = None
        self.duration = -1
        self.synthese_calls = []

    def __repr__(self):
        return '<ImportRun id:{id} success:{successful}>'.format(**self.__dict__)

    def convert_params(self, params):
        '''
        Convert a dict of Param objects into a dict of string suitable for
        Synthese import parameters
        '''
        synthese_params = {}
        for param in params.itervalues():
            if param.no_send:
                continue
            value = param.value
            if value == '':
                continue

            if param.type in ('file', 'directory'):
                if not os.path.isabs(param.value):
                    # relative paths are relative to the import directory and
                    # should be separated with forward slashes.
                    relative_path = param.value.replace('/', os.sep)
                    value = os.path.join(self.import_.path, relative_path)
                if ((param.type == 'file' and not os.path.isfile(value)) or
                    (param.type == 'directory' and not os.path.isdir(value))):
                    raise ImportException('%s to import can\'t be found at %r' %
                        ('File' if param.type == 'file' else 'Directory', value))

            synthese_params[param.id] = value
        return synthese_params

    def _process_import_log(self, log):
        for line in log.split('<br />'):
            level = line[:4].strip().lower()
            if level not in self.LEVEL_NAMES:
                level = 'unknown'
            else:
                line = line[4:]
            if line.startswith(' : '):
                line = line[3:]
            if not line:
                continue
            self.messages[level].append(line)
            if level == 'err':
                self.successful = False

    def call_synthese(self, synthese_params):
        if self.messages is None:
            self.messages = dict((l, []) for l in self.LEVEL_NAMES)
        project = self.import_.template.manager.project
        http_api = synthesepy.http_api.HTTPApi(project.env)

        try:
            res = http_api.call_service2('ImportFunction', synthese_params)
        except (synthesepy.http_api.HTTPApiException, requests.RequestException) as e:
            raise ImportException('Failure while calling Synthese: %s' % e)

        log.debug('Import HTTP result, %s', res)
        if 0:
            log.debug('Import HTTP body, %r', res.content)
        self._process_import_log(res.content)

        synthese_call = 'URL: {0}, params: {1} url: {2}'.format(
            res.request.url, res.request.data, '{0}?{1}'.format(
                res.request.url, urllib.urlencode(res.request.data)))
        self.synthese_calls.append(synthese_call)

        if self._log is None:
            self._log = u''
        self._log += synthese_call + '\n\n'
        content = res.content.replace('<br />', '\n')
        if not isinstance(content, unicode):
            content = unicode(content, 'utf-8', 'replace')
        self._log += content
        self._log += '\n\n' + ('-' * 80)

    def add_failure(self, exception):
        log.warn('Adding import failure: %s', exception)
        self.successful = False
        if self.messages is None:
            self.messages = dict((l, []) for l in self.LEVEL_NAMES)
        self.messages['err'].append(
            'Caught an exception inside import script: %s' %
            traceback.format_exc(exception))

    def finish(self):
        state_keys = ('successful', 'date', 'execution_time', 'messages',
            'dummy', 'synthese_calls', 'duration')
        state = dict((k, getattr(self, k)) for k in state_keys)
        json.dump(
            state, open(self.state_path, 'wb'),
            sort_keys=True, indent=2)

        # Note: this should use "with" once Python >= 2.7 is a requirement.
        f = gzip.open(self.summary_path, 'wb')
        f.write(self.summary.encode('utf-8'))
        f.close()

        f = gzip.open(self.log_path, 'wb')
        f.write(self.log.encode('utf-8'))
        f.close()

    def get_summary(self, min_level='unknown'):
        if self.messages is None:
            if self._summary:
                return self._summary
            if os.path.isfile(self.summary_path):
                self._summary = gzip.open(
                    self.summary_path, 'rb').read().decode('utf-8')
            else:
                self._summary = 'Not available'
            return self._summary

        lines = []
        lines.append(i18n.start_of_summary.format(level=min_level.upper()))
        index = self.LEVEL_NAMES.index(min_level)
        all_levels = self.LEVEL_NAMES[index:]

        for level in all_levels:
            if not self.messages[level]:
                continue
            level_upper = level.upper()
            lines.append(i18n.import_level_section.format(level=level_upper))
            for message in self.messages[level]:
                lines.append(u'{0}: {1}\n'.format(
                    level_upper, unicode(message, 'utf-8', 'replace')))

        lines.append(i18n.end_of_messages)

        lines.append(i18n.technical_infos.format(
            dummy=self.dummy,
            synthese_calls='\n\n'.join(self.synthese_calls),
            duration=self.duration))

        return ''.join(lines)
    summary = property(get_summary)

    def get_log(self):
        if self._log:
            return self._log
        if os.path.isfile(self.log_path):
            self._log = gzip.open(self.log_path, 'rb').read().decode('utf-8')
        else:
            self._log = 'Not available'
        return self._log
    log = property(get_log)


class Import(utils.DirObjectLoader):
    def __init__(self, template, id, path):
        self.template = template
        self.id = id
        self.path = path
        self._runs = None
        self.params_path = join(self.path, 'params.json')
        self.events_path = join(self.path, 'events.json')
        self.runs_path = join(self.path, 'runs')
        utils.maybe_makedirs(self.runs_path)

        config_path = join(self.path, 'config.py')
        if os.path.isfile(config_path):
            config = {}
            execfile(config_path, {}, config)
            self.config = config['config']
        else:
            self.config = {}

        self._read_params()

    def _read_params(self):
        try:
            saved_params = json.load(open(self.params_path))
        except IOError:
            saved_params = {}

        self.params = OrderedDict()

        # built-in params
        default_params = [{
            'label': i18n.state,
            'id': 'state',
            'default': 'open',
            'no_send': True,
            'uploader_visible': True,
            'uploader_readonly': True,
        }, {
            'label': i18n.description,
            'id': 'description',
            'default': '',
            'no_send': True,
            'uploader_visible': True,
        }] + self.template.defaults

        for default_param in default_params:
            param = Param()
            if isinstance(default_param, tuple):
                param.id = default_param[0]
                param.label = param.id
                param.default = default_param[1]
            elif isinstance(default_param, dict):
                param.__dict__.update(default_param)

            if param.default is None:
                param.default = ''
            param.default = str(param.default)
            param.value = param.default
            if not isinstance(param.label, unicode):
                param.label = unicode(param.label, 'utf-8')

            if param.id in saved_params:
                param.value = saved_params[param.id]

            self.params[param.id] = param

    def save_params(self, username=None, no_mail=False):
        saved_params = dict((id, param.value) for
            (id, param) in self.params.iteritems() if
            param.value != param.default)
        json.dump(
            saved_params, open(self.params_path, 'wb'),
            sort_keys=True, indent=2)

        event = Event()
        event.type = 'update'
        event.username = username
        self._add_event(event, None, no_mail)

    def get_events(self):
        try:
            saved_events = json.load(open(self.events_path))
        except IOError:
            saved_events = []
        events = []
        for saved_event in saved_events:
            event = Event()
            event.__dict__.update(saved_event)
            events.append(event)
        return events
    events = property(get_events)

    def _add_event(self, event, extra=None, no_mail=False):
        events = self.get_events()
        events.append(event)
        saved_events = [event.__dict__ for event in events]
        json.dump(
            saved_events, open(self.events_path, 'wb'),
            sort_keys=True, indent=2)

        if not no_mail:
            self._send_event_notifications(event, extra)

    def add_comment(self, username, comment):
        event = Event()
        event.type = 'comment'
        event.username = username
        event.content = comment
        self._add_event(event)

    def _load_runs(self):
        if self._runs is not None:
            return
        self._runs = self.load_from_dir(self.runs_path, ImportRun, self)

    def get_runs(self):
        self._load_runs()
        return self._runs.values()
    runs = property(get_runs)

    def get_run(self, run_id):
        self._load_runs()
        return self._runs[run_id]

    def _create_run(self):
        self._load_runs()
        return self.create_object(self._runs, self.runs_path, ImportRun, self)

    def _send_event_notifications(self, event, extra):
        t = self.template
        to_send = []

        if event.type == 'comment':
            emails = t.admin_emails | t.uploader_emails
            detail = i18n.new_comment
            body = i18n.new_comment_body.format(
                username=event.username,
                comment=event.content
            )
            to_send = ((emails, body),)

        elif event.type == 'run':
            run = extra
            detail = i18n.import_result
            to_send = self._get_run_finish_notifications(run)

        elif event.type == 'update':
            emails = t.admin_emails | t.uploader_emails
            detail = i18n.import_update
            body =  i18n.import_updated_body.format(
                username=event.username)
            to_send = ((emails, body),)

        else:
            raise Exception('Unknown event type %r' % event.type)

        config = self.template.manager.project.env.config

        subject = i18n.mail_summary.format(
            project_name=config.project_name,
            hostname=socket.gethostname(),
            template_label=self.template.label,
            detail=detail)

        for emails, body in to_send:
            if not emails:
                continue
            utils.send_mail(config, list(emails), subject, body)

    def _get_run_finish_notifications(self, run):
        level_to_mails = {}
        for level in ImportRun.LEVEL_NAMES:
            # TODO: also allow passing a username instead of an email
            level_to_mails[level] = set(
                self.template.run_results_notifications.get(level, []))

        # admins always get errors.
        level_to_mails['err'].update(self.template.admin_emails)

        levels_with_messages = set()
        for level in ImportRun.LEVEL_NAMES:
            if run.messages[level]:
                levels_with_messages.add(level)
        log.debug('Levels with messages: %s', levels_with_messages)

        levels_to_notify = set()
        for level in ImportRun.LEVEL_NAMES:
            index = ImportRun.LEVEL_NAMES.index(level)
            same_or_higher_levels = set(ImportRun.LEVEL_NAMES[index:])
            if same_or_higher_levels & levels_with_messages:
                levels_to_notify.add(level)
        log.debug('Levels to notify: %s', levels_to_notify)

        config = self.template.manager.project.env.config
        to_send = []
        notified_emails = set()

        for level in ImportRun.LEVEL_NAMES:
            if level not in levels_to_notify:
                continue
            emails = level_to_mails[level] - notified_emails
            if not emails:
                continue
            notified_emails.update(emails)
            body = run.get_summary(level)
            to_send.append((emails, body))

        return to_send

    def execute(self, username=None, dummy=False, no_mail=False):
        log.info('Executing import')

        run = self._create_run()
        run.dummy = dummy
        try:
            start = time.time()
            if self.template.do_import:
                self.template.do_import(self, run)
            else:
                self._do_import(run)
        except Exception, e:
            log.warn('Import exception: %s', traceback.format_exc(e))
            run.add_failure(e)
        finally:
            run.duration = time.time() - start
            run.finish()
            event = Event()
            event.type = 'run'
            event.username = username
            event.id = run.id
            event.successful = run.successful
            self._add_event(event, run, no_mail)

            if not run.successful:
                log.warn('Import failed!')
                if 0:
                    subject, body = run.get_summary()
                    log.debug('Summary: %s\n\n%s', subject, body)
            return run

    def _do_import(self, run):
        synthese_params = run.convert_params(self.params)

        if 'di' not in synthese_params:
            # Do import
            synthese_params['di'] = '0' if run.dummy else '1'

        run.call_synthese(synthese_params)

    def __repr__(self):
        return '<Import %s>' % self.__dict__
