#!/usr/bin/env python
#
#    Script to build and test Synthese, to be run from Jenkins.
#    @file jenkins_build.py
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
from optparse import OptionParser
import os
import subprocess
import sys

log = logging.getLogger(__name__)

default_config = {
    "mysql_params": "host=localhost,user=synthese,passwd=synthese",
    "mysql_db": "synthese_test",
    "port": "9000",
    "env": {},
}

# per slave config
config = {}

def run_synthesepy(command, tool, mode, global_args, command_args, environ,
                   ignore_failure=False):

    thisdir = os.path.abspath(os.path.dirname(__file__))
    synthesepy_path = os.path.join(thisdir, "synthese.py")
    args = ([sys.executable, synthesepy_path, "-t", tool, "-m", mode] +
        global_args + [command] + command_args)
    log.debug("Running command: %s", args)
    try:
        subprocess.check_call(args, env=environ)
    except subprocess.CalledProcessError, e:
        if ignore_failure:
            log.warn("Command failed. Not throwing exception: %s", e)
            return
        raise


def build(options):
    thirdparty_dir = os.environ.get(
        'SYNTHESE_THIRDPARTY_DIR', os.path.expanduser('~/.synthese')) 
    config_file = os.path.join(thirdparty_dir, 'jenkins_config.py')
    log.info("Reading config from %s", config_file)

    if os.path.isfile(config_file):
        execfile(config_file, {}, {
            "default_config": default_config,
            "config": config,
        })
    else:
        log.info("Config file doesn't exist")

    c = default_config.copy()
    c.update(config.get(options.slave, {}))

    log.debug("Config: %s", c)

    environ = os.environ.copy()
    environ["SYNTHESE_MYSQL_PARAMS"] = c["mysql_params"]
    environ["SYNTHESE_MYSQL_DB"] = c["mysql_db"]
    environ.update(c["env"])

    run_synthesepy(
        "build", options.tool, options.mode, [], [], environ)

    dbconns = [
        "mysql://debug=1,{mysql_params},db={mysql_db}".format(**c), "sqlite://"]
    run_synthesepy(
        "runtests", options.tool, options.mode,
        ["-v", "-s", "-p", c["port"]],
        ["--dbconns"] + dbconns,
        environ, options.test_failure_not_fatal)


if __name__ == "__main__":
    parser = OptionParser()

    parser.add_option("-s", "--slave", default=os.environ.get("slave"))
    parser.add_option("-t", "--tool", default=os.environ.get("tool", "cmake"))
    parser.add_option("-m", "--mode", default=os.environ.get("mode", "debug"))
    parser.add_option("-v", "--verbose", action="store_true",
         default=False, help="Print debug logging")
    parser.add_option("--test-failure-not-fatal", action="store_true",
         default=False,
         help="Failure of tests doesn't make this script return a non-zero code")

    (options, args) = parser.parse_args()

    logging.basicConfig(level=(logging.DEBUG if options.verbose else
                               logging.INFO))

    if options.tool not in ("scons", "cmake"):
        raise Exception("Invalid tool: %s" % options.tool)

    if options.mode not in ("debug", "release"):
        raise Exception("Invalid mode: %s" % options.mode)

    log.debug("Options: %s", options)

    build(options)
