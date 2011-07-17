#    Continous integration script.
#    @file continuous_integration.py
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


import logging
import os

import synthesepy.build
import synthesepy.test

log = logging.getLogger(__name__)


def run(env, args):
    config = env.config

    # The ci tool can set these settings in the environment:
    if 'tool' in os.environ:
        config.tool = os.environ['tool']
    if 'mode' in os.environ:
        config.mode = os.environ['mode']

    try:
        synthesepy.build.build(env)
    except Exception, e:
        if not args.no_clean_if_build_fails:
            raise
        log.warn('Build failed, cleaning and rebuilding')
        synthesepy.build.clean(env, args.dummy)
        synthesepy.build.build(env)

    # FIXME: this should be extracted from the mysql connection string.
    os.environ.update(
        config.test_env
    )

    tester = synthesepy.test.main.Tester(env)
    tester.run_tests(config.suites)
