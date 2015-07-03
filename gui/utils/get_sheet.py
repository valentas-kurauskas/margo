#!/usr/bin/python
import os
import sys
sys.path.append(os.path.abspath ('..'))

def get_sheet_coords(LKS_x, LKS_y):
    return ((LKS_x - 200000) / 5000),( (LKS_y - 5900000) / 5000);

def fmt(lon,lat):
    return "{0}_{1}".format(lon,lat)

if len(sys.argv)==3:  print (fmt(*get_sheet_coords(int(sys.argv[1]), int(sys.argv[2]))))
elif len(sys.argv)==5:
    tl = get_sheet_coords(int(sys.argv[1]), int(sys.argv[2]))
    bl = get_sheet_coords(int(sys.argv[3]), int(sys.argv[4]))
    result = []
    for i in range(min(tl[0], bl[0]), max(tl[0], bl[0])):
        for j in range(min(tl[1], bl[1]), max(tl[1], bl[1])):
            print(fmt(i,j))

