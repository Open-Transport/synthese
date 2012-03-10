import logging
import time
import urllib
import urllib2
import urlparse
import sys

log = logging.getLogger(__name__)

logging.basicConfig(level=logging.DEBUG)


def get_sid(url, user, pw):

    params = {
        'actionParamlogin': user,
        'actionParampwd': pw,
        'a': 'login',
        'fonction': 'admin',
    }

    res = urllib2.urlopen(url, urllib.urlencode(params))
    return dict(urlparse.parse_qsl(res.url))['sid']


def get_journey(url, sid, branch, trace=False):
    params = {
        'ac': '35001',
        'dt': '2011-03-14 10:00',
        'ec': 'Les Diablerets',
        'ep': 'Col du Pillon',
        'fonction': 'admin',
        'ja': '1',
        'lo': '1' if trace else '0',
        'mt': '17732923532771328',
        #'pt': '17732923532771330',
        'rhtf': '0',
        'rhto': 'pr',
        'rhtr': '1',
        'rhts': '30',
        'rn': '1',
        'roid': '7036874417766411',
        'rub': 'TransportSiteAdmin',
        'sc': 'Le Sepey',
        'sid': sid,
        'sp': 'Les Planches',
        'tab': 'rp',
        #'tt': '17732923532771329'
    }

    out_file = ('/home/spasche/rcs/mysql/synthese3/s3-admin/deb/opt/rcs/'
        's3-admin/files/iti_result_%s.html' % branch)

    log.info('Saving into %s' % out_file)

    with open(out_file, 'wb') as f:
        f.write('PENDING ' + time.strftime("%Y-%m-%d %H:%M:%S", time.gmtime()))

    output = urllib2.urlopen(url, data=urllib.urlencode(params)).read()

    META_TAG = '<meta charset="utf-8">'
    if META_TAG not in output:
        output = output.replace('</head>', META_TAG + '</head>')

    with open(out_file, 'wb') as f:
        f.write(output)


if 'trunk' in sys.argv:
    branch = 'trunk'
    url = 'http://localhost:8090/synthese3_2/admin'
else:
    branch = 'my'
    url = 'http://localhost:8081/synthese3/admin'

sid = get_sid(url, 'root', 'root')
get_journey(url, sid, branch, True)
