#!/usr/bin/python

import sys;

def get_sheet_coords(LKS_x, LKS_y):
    return ((LKS_x - 200000) / 5000),( (LKS_y - 5900000) / 5000);

print get_sheet_coords(int(sys.argv[1]), int(sys.argv[2]))

