# -*- coding: utf-8 -*-
#    Synchronization between web pages in the database and the filesystem.
#    @file db_sync.py
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


import json
import logging
import os
from os.path import join
import shutil
import string
import unicodedata
import urlparse

from synthesepy import http_api
from synthesepy import utils

log = logging.getLogger(__name__)

def _clean_directory(directory):
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


def _sync_site_to_files(project, site):
    pages = project._db_backend.query(
        'select * from t063_web_pages where site_id = ?', [site.id])
    id_to_page = dict((p['id'], p) for p in pages)

    def transform_filename(path):
        if '.' in path:
            return path
        return path + '.html'

    MAX_TITLE_LEN = 10

    id_to_path = {}
    for id, page in id_to_page.iteritems():
        if page['smart_url_path']:
            smart_url = page['smart_url_path']
            parsed_url = urlparse.urlparse(smart_url)
            path = ['by_smart_url']
            if parsed_url.netloc:
                path.append(parsed_url.netloc)
            path.extend(parsed_url.path.split('/')[1:])
            path = [p for p in path if p]
            if len(path) == 1:
                path.append('root')
            path[-1] = transform_filename(path[-1])
            id_to_path[id] = path
            continue
        p = page
        path = [transform_filename(page['title'])]
        while p['up_id']:
            p = id_to_page[p['up_id']]
            path.insert(0, p['title'][:MAX_TITLE_LEN] + '_dir')
        path.insert(0, 'by_title')
        id_to_path[id] = path

    for id, path in id_to_path.iteritems():
        def convert_part(part):
            if isinstance(part, unicode):
                part = unicodedata.normalize(
                    'NFKD', part).encode('ascii','ignore')
            return ''.join(
                c if (c in string.ascii_letters + '/.') else '_' for c in part)
        id_to_path[id] = [convert_part(p) for p in path]

    target_base = join(site.path, 'web_pages')
    _clean_directory(target_base)

    json.dump(
        id_to_path,
        open(join(target_base, 'id_to_path.json'), 'wb'),
        indent=2)

    for id, page in id_to_page.iteritems():
        path = id_to_path[id]
        target = join(*[target_base] + path)
        log.debug('Saving content to %r', target)
        if not os.path.isdir(os.path.dirname(target)):
            os.makedirs(os.path.dirname(target))
        with open(target, 'wb') as f:
            f.write(page['content1'].encode('utf-8'))


def sync_to_files(project):
    for site in project.sites:
        log.info('Syncing files from site %r', site.name)
        _sync_site_to_files(project, site)


def _sync_files_to_site(project, site):
    # TODO: USE_HTTP requires the daemon to be running.
    # Maybe have an option to automatically launch if not?

    USE_HTTP = True
    if USE_HTTP:
        api = http_api.HTTPApi(project.env)

    target_base = join(site.path, 'web_pages')
    id_to_path = json.load(open(join(target_base, 'id_to_path.json'), 'rb'))

    for id, path in id_to_path.iteritems():
        target = join(*[target_base] + path)
        log.debug('Loading content from %r', target)
        content = unicode(open(target).read(), 'utf-8')
        if USE_HTTP:
            api.update_web_page_content(id, content)
        else:
            project._db_backend.query(
                'update t063_web_pages set content1 = ? where id = ?',
                [content, id])


def sync_from_files(project):
    for site in project.sites:
        log.info('Syncing files from site %r', site.name)
        _sync_files_to_site(project, site)
