#    Imports management.
#    @file imports.py
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

import collections
import json
import logging
import os
from os.path import join
import socket
import time
import traceback

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
        if self.templates is not None:
            return

        self.templates = {}
        config_path = join(self.project.path, 'manager', 'imports_config.py')
        config = {}
        if os.path.isfile(config_path):
            execfile(config_path, {
                'project': self.project,
            }, config)
        if 'import_templates' not in config:
            log.warn('No imports config')
            return
        for template_config in config['import_templates']:
            print template_config
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


class DirObjectLoader(object):
    def load_from_dir(self, directory, class_, *args):
        objects = OrderedDict()
        ids = [id for id in os.listdir(directory) if
            os.path.isdir(join(directory, id))]
        for id in sorted(ids, key=int):
            path = join(self.path, id)
            ctor_args = list(args) + [id, path]
            objects[id] = class_(*ctor_args)
        return objects

    def create_object(self, objects, directory, class_, *args):
        max_id = 0
        if objects:
            max_id = max(int(id) for id in objects.keys())
        new_id = str(max_id + 1)
        object_path = join(directory, new_id)
        os.makedirs(object_path)
        ctor_args = list(args) + [new_id, object_path]
        object = class_(*ctor_args)
        objects[object.id] = object
        return object


class ImportTemplate(DirObjectLoader):
    def __init__(self, manager, config):
        self.manager = manager

        # default config
        self.after_create = None
        self.do_import = None
        self.notifications = {}

        self.__dict__.update(config)
        if not hasattr(self, 'label'):
            self.label = self.id
        self.imports = None
        self.path = join(self.manager.project.path, 'imports', self.id)
        utils.maybe_makedirs(self.path)

    def _load_imports(self):
        if self.imports is not None:
            return

        self.imports = self.load_from_dir(self.path, Import, self)

    def get_imports(self):
        self._load_imports()
        return self.imports.values()

    def get_import(self, import_id):
        self._load_imports()
        if import_id == ':latest':
            import_id = str(max(int(id) for id in self.imports.keys()))

        return self.imports[import_id]

    def create_import(self, args={}):
        self._load_imports()
        import_ = self.create_object(self.imports, self.path, Import, self)
        if self.after_create:
            self.after_create(import_, args)
        return import_


class Param(object):
    def __init__(self):
        self.id = None
        self.label = None
        self.type = 'string'
        self.default = None
        self.value = None
        self.no_send = False

    def __repr__(self):
        return '<Param %s>' % self.__dict__


class Event(object):
    def __init__(self):
        self.user = None
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

        self.date = 0
        self.execution_time = -1
        self.dummy = False
        self.messages = dict((l, []) for l in self.LEVEL_NAMES)
        self.synthese_calls = []

    @property
    def successful(self):
        return len(self.messages['err']) == 0

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
            if value is None:
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
            line = line[4:]
            if line.startswith(' : '):
                line = line[3:]
            if not line:
                continue
            self.messages[level].append(line)

    def call_synthese(self, synthese_params):
        project = self.import_.template.manager.project
        http_api = synthesepy.http_api.HTTPApi(project.env)

        try:
            res = http_api.call_service2('ImportFunction', synthese_params)
        except (synthesepy.http_api.HTTPApiException, requests.RequestException) as e:
            raise ImportException('Failure while calling Synthese: %s', e)

        log.debug('Import HTTP result, %s', res)
        if 0:
            log.debug('Import HTTP body, %r', res.content)
        self._process_import_log(res.content)

        self.synthese_calls.append('URL: {0}, params: {1}'.format(
            res.request.url, res.request.data))

    def add_failure(self, exception):
        log.warn('Adding import failure: %s', exception)
        self.messages['err'].append(
            'Caught an exception inside import script: %s' %
            traceback.format_exc(exception))

    def finish(self):
        state_keys = ('date', 'execution_time', 'messages', 'dummy',
            'synthese_calls')
        state = dict((k, getattr(self, k)) for k in state_keys)
        json.dump(
            state, open(self.state_path, 'wb'),
            sort_keys=True, indent=2)

        with open(join(self.path, 'summary.txt'), 'wb') as f:
            subject, body = self.get_summary()
            f.write(subject)
            f.write('\n\n')
            f.write(body)

    def get_summary(self, min_level='unknown'):
        config = self.import_.template.manager.project.config
        summary = i18n.import_summary.format(
            template_label=self.import_.template.label,
            project_name=config.project_name,
            hostname=socket.gethostname(),
            level=min_level.upper(),
        )

        body = i18n.start_of_summary.format(level=min_level.upper())
        index = self.LEVEL_NAMES.index(min_level)
        all_levels = self.LEVEL_NAMES[index:]
        for level in all_levels:
            if not self.messages[level]:
                continue
            body += i18n.import_level_section.format(level=level.upper())
            for message in self.messages[level]:
                body += '{0}: {1}\n'.format(level.upper(), message)

        body += i18n.end_of_messages

        body += i18n.technical_infos.format(
            dummy=self.dummy,
            synthese_calls='\n\n'.join(self.synthese_calls))

        return (summary, body)


class Import(DirObjectLoader):
    def __init__(self, template, id, path):
        self.template = template
        self.id = id
        self.path = path
        self.runs = None
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
            'label': 'Etat',
            'id': 'state',
            'default': 'open',
            'no_send': True,
        }] + self.template.defaults + [{
            'label': 'Description',
            'id': 'description',
            'default': '',
            'no_send': True,
        }]

        for default_param in default_params:
            param = Param()
            if isinstance(default_param, tuple):
                param.id = default_param[0]
                param.label = param.id
                param.default = default_param[1]
            elif isinstance(default_param, dict):
                param.__dict__.update(default_param)

            param.value = param.default

            if param.id in saved_params:
                param.value = saved_params[param.id]

            self.params[param.id] = param

    def update_param(self, id, value):
        self.params[id].value = value
        saved_params = dict((id, param.value)
            for (id, param) in self.params.iteritems()
            if param.value != param.default)
        json.dump(
            saved_params, open(self.params_path, 'wb'),
            sort_keys=True, indent=2)

        # TODO: add event

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

    def _add_event(self, event):
        events = self.get_events()
        events.append(event)
        saved_events = [event.__dict__ for event in events]
        json.dump(
            saved_events, open(self.events_path, 'wb'),
            sort_keys=True, indent=2)

        # TODO: send notification

    def add_comment(self, username, comment):
        event = Event()
        event.type = 'comment'
        event.user = username
        event.content = comment
        self._add_event(event)

    def _load_runs(self):
        if self.runs is not None:
            return
        self.runs = self.load_from_dir(self.runs_path, ImportRun, self)

    def get_runs(self):
        self._load_runs()
        return self.runs.values()
    # TODO rename runs instance var _runs and uncomment (do the same for _imports above).
    #runs = property(get_runs)

    def get_run(self, run_id):
        self._load_runs()
        return self.runs[run_id]

    def _create_run(self):
        self._load_runs()
        return self.create_object(self.runs, self.runs_path, ImportRun, self)

    def send_run_finish_mail_notifications(self, run):
        level_to_mails = {}
        for level in ImportRun.LEVEL_NAMES:
            # TODO: also allow passing a username instead of an email
            level_to_mails[level] = self.template.notifications.get(
                'import_%s' % level, [])

        levels_with_messages = set()
        for level in ImportRun.LEVEL_NAMES:
            if run.messages[level]:
                levels_with_messages.add(level)

        levels_to_notify = set()
        for level in ImportRun.LEVEL_NAMES:
            index = ImportRun.LEVEL_NAMES.index(level)
            same_or_higher_levels = set(ImportRun.LEVEL_NAMES[index:])
            if same_or_higher_levels & levels_with_messages:
                levels_to_notify.add(level)
        log.debug('Levels with messages: %s', levels_to_notify)

        config = self.template.manager.project.env.config
        for level in levels_to_notify:
            emails = level_to_mails[level]
            if not emails:
                continue
            subject, body = run.get_summary(level)
            utils.send_mail(config, emails, subject, body)

    def execute(self, dummy=False, no_mail=False):
        log.info('Executing import')

        run = self._create_run()
        run.dummy = dummy
        try:
            if self.template.do_import:
                self.template.do_import(self, run)
            else:
                self._do_import(run)
        except Exception, e:
            run.add_failure(e)
        finally:
            run.finish()
            if not no_mail:
                self.send_run_finish_mail_notifications(run)
            if not run.successful:
                log.warn('Import failed!')
                if 0:
                    subject, body = run.get_summary()
                    log.debug('Summary: %s\n\n%s', subject, body)

    def _do_import(self, run):
        synthese_params = run.convert_params(self.params)

        if 'di' not in synthese_params:
            # Do import
            synthese_params['di'] = '0' if run.dummy else '1'

        run.call_synthese(synthese_params)

    def __repr__(self):
        return '<Import %s>' % self.__dict__
