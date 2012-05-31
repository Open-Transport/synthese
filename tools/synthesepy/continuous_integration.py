#    Continous integration script.
#    @file continuous_integration.py
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


import logging
import os

import synthesepy.build
import synthesepy.package
import synthesepy.test

log = logging.getLogger(__name__)


def _should_build_package(env):
    if env.config.force_create_package:
        return True

    if not env.config.should_build_package(env):
        log.info('should_build_package returned False. Not building package.')
        return False

    return True


def run(env, args):
    config = env.config

    env_type = env.type
    env_mode = env.mode
    # The ci tool can set these settings in the environment:
    if 'tool' in os.environ:
        # Note: backward compatibility: env_type should be used instead of tool.
        env_type = os.environ['tool']
    if 'env_type' in os.environ:
        env_type = os.environ['env_type']
    if 'mode' in os.environ:
        env_mode = os.environ['mode']

    # Create the env again, to take into account the tool/mode.
    env = synthesepy.env.create_env(
        env_type, config.env_path, env_mode, config)

    try:
        log.info('Building')
        synthesepy.build.build(env, 'build')
    except Exception, e:
        if args.no_clean_if_build_fails:
            raise
        log.warn('Build failed, cleaning and rebuilding')
        synthesepy.build.build(env, 'clean')
        synthesepy.build.build(env, 'build')

    if _should_build_package(env):
        log.info('Creating package')
        synthesepy.package.run(env, args)

    if args.no_tests:
        return

    log.info('Running tests')
    tester = synthesepy.test.Tester(env)
    tester.run_tests(config.suites)
