"""
Utility functions shared by all update scripts.
"""

__author__ = 'Sylvain Pasche <sylvain.pasche@gmail.com>'

import os

SKIP_DIRS = ['.svn', '.hg', 'CVS']
SKIP_EXTS = []


def files_generator(d, keep_exts, keep_files=[]):
    for path, dirlist, filelist in os.walk(d):
        for exclude in SKIP_DIRS:
            if exclude in dirlist:
                dirlist.remove(exclude)
        for name in filelist:
            if name.endswith(tuple('.' + e for e in SKIP_EXTS)):
                continue
            keep_file = name in keep_files or name.endswith(tuple('.' + e for e in keep_exts))
            if not keep_file:
                continue
            yield os.path.join(path, name)
