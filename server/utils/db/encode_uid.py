#!/usr/bin/python2
import sys
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("table_id", help="Synthese table id")
parser.add_argument("row_id", help="Synthese row id")
args = parser.parse_args()


def encode_uid(table_id, row_id):
  node_id = 1
  uid = int(row_id)
  tmp = node_id
  uid |= (tmp << 32);
  tmp = table_id;
  uid |= (tmp << 48);
  return uid

uid = encode_uid(int(args.table_id), int(args.row_id))
print uid

