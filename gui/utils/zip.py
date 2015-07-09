import sys
import os
from core import xyz

if len(sys.argv) < 3:
    print "merges files from given directories with the same name and saves into a new directory"
    print "syntax is: python -m utils.zip dir1 .. dirn outputdir"
    exit(1)

dirs=sys.argv[:-1]

def zip_dirs(dirs):
    files = [os.listdir(dr) for dr in dirs];
    for f in files[0]:
        if not f.endswith(".xyz"): continue
        for ff in files[1:]:
            if f not in ff:
                continue
            dbs = [xyz.load_from_file(dr+os.sep+f) for dr in dirs]
            d = dbs[0]
            for db in dbs[1:]:
                d.union(db,rename_ids=True)
            yield f,d 
    

    
for filename, db in zip_dirs(sys.argv[1:-1]):
    print ("Merged: "+filename)
    db.save_xyz(sys.argv[-1]+os.sep+filename)

