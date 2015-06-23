import sys
import os
from core import xyz

if len(sys.argv) < 3:
    print "syntax is: python -m utils.add_score scorefile.xyz file1.xyz file2.xyz .. filen.xyz"


scores = xyz.load_from_file(sys.argv[1])
for fn in sys.argv[2:]:
    print ("rescoring "+ fn)
    data = xyz.parse_file(fn)
    data.join_neighbours(scores, [], ['SCORE'], radius=5)
    data.save_xyz(fn)

