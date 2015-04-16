# -*- coding: utf-8 -*-
#    Synchronization between web pages in the database and the filesystem.
#    @file db_sync.py
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
import json
import hashlib
import logging
import os
from os.path import join
import shutil
import string
import unicodedata
import urlparse

import mechanize

from synthesepy import http_api
from synthesepy import utils


log = logging.getLogger(__name__)


class Page(object):
    def __init__(self, id):
        self.id = id
        self.title = ''
        self.content1 = ''
        self.save = True

    def __repr__(self):
        p = copy.copy(self)
        MAX_CONTENT = 200
        p.content1 = p.content1[:MAX_CONTENT]
        return '<Page %r>' % p.__dict__


class PagesIO(object):
    def __init__(self, project, site):
        self.project = project
        self.site = site


class FilesPageMixin(object):
    def __init__(self, project, site):
        super(FilesPageMixin, self).__init__(project, site)
        self.target_base = join(self.site.path, 'web_pages')
        self.pages_file = join(self.target_base, 'pages.json')


class HTTPPageMixin(object):
    def __init__(self, project, site, host=None):
        super(HTTPPageMixin, self).__init__(project, site)
        self.host = host
        self.http_api = http_api.HTTPApi(self.project.env, self.host)


class PagesReader(PagesIO):
    def read(self):
        raise NotImplementedError()


class DBPagesReader(PagesReader):
    def read(self):
        results = self.project.db_backend.query(
            'select * from t063_web_pages where site_id = ?', [self.site.id])
        pages = []
        for result in results:
            page = Page(-1)
            page.__dict__.update(result)
            pages.append(page)
        return pages


class FilesPagesReader(FilesPageMixin, PagesReader):
    def read(self):
        pages_dict = {}
        if os.path.isfile(self.pages_file):
            pages_dict = json.load(open(self.pages_file, 'rb'))
        pages = []
        for id, page_obj in pages_dict.iteritems():
            page = Page(-1)
            page.__dict__.update(page_obj)
            target = join(*[self.target_base] + page.path)
            log.debug('Loading content from %r', target)
            page.content1 = unicode(open(target).read(), 'utf-8')
            pages.append(page)
        return pages


class HTTPPagesReader(HTTPPageMixin, PagesReader):
    # TODO: implement a way of retrieving all existing pages
    def __init__(self, project, site, host, existing_pages):
        super(HTTPPagesReader, self).__init__(project, site, host)
        self.existing_pages = existing_pages

    def read(self):
        br = self.http_api.get_admin_browser()

        pages = []
        for existing_page in self.existing_pages:
            br.open(self.http_api.get_admin_url('WebPageAdmin', {
                'roid': existing_page.id,
            }))
            page = copy.copy(existing_page)
            try:
                br.select_form('update_content')
                form = br.form
            except mechanize.FormNotFoundError:
                # Tiny MCE form
                # TODO: this returns a text serialization
                # Convert (temporarily?) to raw editor to fetch content.
                form = br.forms().next()
                page.save = False

            page.content1 = unicode(form['actionParamc1'], 'utf-8')

            # TODO: copy other properties that could have changed.

            # XXX not sure at what stage they are introduced converted.
            page.content1 = page.content1.replace('\r\n', '\n')
            pages.append(page)
        return pages


class PagesWriter(PagesIO):
    def write(self, pages):
        raise NotImplementedError()

    def update(selfs, pages):
        raise NotImplementedError()


class DBPagesWriter(PagesWriter):
    def write(self, pages):
        # TODO: should clear the db first
        self.update(pages)

    def update(self, pages):
        if self.project.config.dummy:
            return

        for page in pages:
            self.project.db_backend.query(
                'update t063_web_pages set content1 = ? where id = ?',
                [page.content1, page.id])


class FilesPagesWriter(FilesPageMixin, PagesWriter):
    def _clean_directory(self, directory):
        """
        Delete the given directory, or all the files inside that directory if it
        is a subversion checkout.
        """
        if not os.path.isdir(join(directory, '.svn')):
            utils.RemoveDirectory(directory)
            os.makedirs(directory)
            return

        for path, dirlist, filelist in os.walk(directory):
            if '.svn' in dirlist:
                dirlist.remove('.svn')
            for f in filelist:
                os.unlink(join(path, f))

    def write(self, pages):
        self._clean_directory(self.target_base)
        self.update(pages)

    def update(self, pages):
        all_pages = FilesPagesReader(self.project, self.site).read()

        pages = [p for p in pages if p.save]
        id_to_all_page = dict((p.id, p) for p in all_pages + pages)
        id_to_page = dict((p.id, p) for p in pages)

        def transform_filename(path):
            if '.' in path:
                return path
            return path + '.html'

        MAX_TITLE_LEN = 10

        for id, page in id_to_page.iteritems():
            if page.smart_url_path:
                parsed_url = urlparse.urlparse(page.smart_url_path)
                path = ['by_smart_url']
                if parsed_url.netloc:
                    path.append(parsed_url.netloc)
                path.extend(parsed_url.path.split('/')[1:])
                path = [p for p in path if p]
                if len(path) == 1:
                    path.append('root')
                path[-1] = transform_filename(path[-1])
                page.path = path
                continue
            p = page
            path = [transform_filename(page.title)]
            while p.up_id:
                p = id_to_all_page[p.up_id]
                path.insert(0, p.title[:MAX_TITLE_LEN] + '_dir')
            path.insert(0, 'by_title')
            page.path = path

        for id, page in id_to_page.iteritems():
            def convert_part(part):
                if isinstance(part, unicode):
                    part = unicodedata.normalize(
                        'NFKD', part).encode('ascii','ignore')
                return ''.join(
                    c if (c in string.ascii_letters + '/.') else '_' for c in part)
            page.path = [convert_part(p) for p in page.path]

        pages_dict = {}
        if os.path.isfile(self.pages_file):
            pages_dict = json.load(open(self.pages_file, 'rb'))

        KEEP_KEYS = set(('id', 'title', 'smart_url_path', 'up_id'))
        for page in pages:
            page_obj = page.__dict__
            page_obj = dict((key, value) for key, value in page_obj.iteritems() if key in KEEP_KEYS)
            page_obj['path'] = page.path
            pages_dict[str(page.id)] = page_obj

        if self.project.config.dummy:
            return

        json.dump(
            pages_dict,
            open(self.pages_file, 'wb'),
            indent=2, sort_keys=True)

        for id, page in id_to_page.iteritems():
            target = join(*[self.target_base] + page.path)
            log.debug('Saving content to %r', target)
            if not os.path.isdir(os.path.dirname(target)):
                os.makedirs(os.path.dirname(target))
            with open(target, 'wb') as f:
                f.write(page.content1.encode('utf-8'))


class HTTPPagesWriter(HTTPPageMixin, PagesWriter):
    def write(self, pages):
        # TODO: should clear the db first
        self.update(pages)

    def update(self, pages):
        if self.project.config.dummy:
            return

        for page in pages:
            if not page.save:
                continue
            self.http_api.update_web_page_content(page.id, page.content1)


class SyncState(object):
    def __init__(self, project, site):
        self.project = project
        self.site = site
        target_base = join(self.site.path, 'web_pages')
        self.sync_file = join(target_base, 'sync_state.json')
        self.sync_state = {}
        if os.path.isfile(self.sync_file):
            self.sync_state = json.load(open(self.sync_file, 'rb'))
            # json transforms int keys to string
            self.sync_state = dict(
                (int(key), value) for key, value in self.sync_state.iteritems())

    def get_modifed_pages(self, pages):
        modified_pages = []
        for page in pages:
            page_sync_state = self.sync_state.get(page.id)
            if page.save and page_sync_state:
                hash = hashlib.md5(page.content1.encode('utf-8')).hexdigest()
                if hash == page_sync_state['content1_hash']:
                    continue
                #log.debug('Hash mismatch. page: %r sync: %r',
                #    hash, page_sync_state['content1_hash'])
            modified_pages.append(page)
        return modified_pages

    def _save_sync_state(self):
        json.dump(
            self.sync_state,
            open(self.sync_file, 'wb'),
            indent=2, sort_keys=True)

    def set_synced_pages(self, pages):
        self.sync_state = {}
        self.update_synced_pages(pages)

    def update_synced_pages(self, pages):
        for page in pages:
            if not page.save:
                continue
            self.sync_state[page.id] = {
                'content1_hash': hashlib.md5(
                    page.content1.encode('utf-8')).hexdigest()
            }
        self._save_sync_state()


def _sync_site_to_files(project, site):
    reader = DBPagesReader(project, site)
    pages = reader.read()
    writer = FilesPagesWriter(project, site)
    writer.write(pages)
    SyncState(project, site).set_synced_pages(pages)


def sync_to_files(project):
    for site in project.sites:
        log.info('Syncing files from site %r', site.name)
        _sync_site_to_files(project, site)


def _sync_files_to_site(project, site, host, db_writer=False):
    reader = FilesPagesReader(project, site)
    pages = reader.read()

    if db_writer:
        writer = DBPagesWriter(project, site)
    else:
        writer = HTTPPagesWriter(project, site, host)

    writer.write(pages)
    SyncState(project, site).set_synced_pages(pages)


def sync_from_files(project, host, use_http):
    for site in project.sites:
        log.info('Syncing files from site %r', site.name)
        _sync_files_to_site(project, site, host, db_writer=not use_http)


def sync_site(project, site, host):
    files_reader = FilesPagesReader(project, site)
    files_pages = files_reader.read()
    log.debug('Files pages: %r', files_pages)

    if host:
        db_reader = HTTPPagesReader(
            project, site, host, files_pages)
    else:
        db_reader = DBPagesReader(project, site)
    db_pages = db_reader.read()
    log.debug('DB pages: %r', db_pages)

    files_pages_ids = set(p.id for p in files_pages)
    db_pages_ids = set(p.id for p in db_pages)
    unique_ids = files_pages_ids ^ db_pages_ids
    if unique_ids:
        log.info('Page ids only in files: %r', files_pages_ids - db_pages_ids)
        log.info('Page ids only in db: %r', db_pages_ids - files_pages_ids)
        raise Exception(
            'Detected unique pages in db or files (%r).'
            ' Manual sync is required' % unique_ids)

    if 0:
        log.debug('=' * 160)
        log.debug('Files pages: %r', files_pages)
        log.debug('=' * 160)
        log.debug('DB pages: %r', db_pages)
        log.debug('=' * 160)

    sync_state = SyncState(project, site)
    files_modified_pages = sync_state.get_modifed_pages(files_pages)
    log.info('files modified pages: %r', files_modified_pages)
    db_modified_pages = sync_state.get_modifed_pages(db_pages)
    log.info('db modified pages: %r (%i not to save)',
        [p for p in db_modified_pages if p.save],
        len([p for p in db_modified_pages if not p.save]))

    common_modified_page_ids = (set(p.id for p in files_modified_pages) &
        set(p.id for p in db_modified_pages))

    if common_modified_page_ids:
        raise Exception('Found pages modified both in files and db.'
            ' Manual sync required (%r)' % common_modified_page_ids)

    if project.config.dummy:
        return

    db_writer = HTTPPagesWriter(project, site, host)
    db_writer.update(files_modified_pages)

    files_writer = FilesPagesWriter(project, site)
    files_writer.update(db_modified_pages)

    sync_state.update_synced_pages(files_modified_pages + db_modified_pages)


def sync(project, host=None):
    for site in project.sites:
        sync_site(project, site, host)

