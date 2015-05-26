#!/usr/bin/python

import sys
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("uid", help="Synthese UID to decode")
args = parser.parse_args()

class UIDInfo(object):
    def __str__(self):
        return str(self.__dict__)

def decode_uid(uid):
    uid_info = UIDInfo()
    uid_info.table_id = (uid & 0xFFFF000000000000) >> 48
    uid_info.node_id = (uid & 0x0000FFFF00000000) >> 32
    uid_info.object_id = uid & 0x00000000FFFFFFFF
    return uid_info

uid_info = decode_uid(int(args.uid))
print(uid_info)

