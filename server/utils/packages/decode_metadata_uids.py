#!/usr/bin/python2

import json
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("-f", "--metadata-file", default="metadata.json", help="Metadata file in JSON format ; default is metadata.json")

args = parser.parse_args()

metadata_file = args.metadata_file
metadata_content = open(metadata_file, 'r').read()

metadata_json = json.loads(metadata_content)

uids = {}

class UIDInfo(object):
    def __str__(self):
        return str(self.__dict__)

def decode_uid(uid):
    uid_info = UIDInfo()
    uid_info.table_id = (uid & 0xFFFF000000000000) >> 48
    uid_info.node_id = (uid & 0x0000FFFF00000000) >> 32
    uid_info.object_id = uid & 0x00000000FFFFFFFF
    return uid_info


def collect_uid_field(obj, field_name):
    if field_name not in obj.keys():
        return
    uid = obj[field_name]
    uids[uid] = decode_uid(int(uid))
    

def collect_uids(obj):
    collect_uid_field(obj, 'id')
    collect_uid_field(obj, 'up_id')
    collect_uid_field(obj, 'site_id')
    collect_uid_field(obj, 'specific_template_id')
        
    if 'object' in obj.keys():
        for sub_object in obj['object']:
            collect_uids(sub_object)

collect_uids(metadata_json)

for key, value in uids.iteritems():
    print("%s : %s" % (key, value))
    
