import sys
import math
import random
from core.xyz import parse_file, CoordDB

class Classifier:
    def __init__(self):
        self.last_data = None

    def load_data(self, files):
        data = []
        X = []
        Y = []
        F = []
        I = []
        for i in range(len(files)):
            res = parse_file(self.inputs[i])
            x = get_x(res)
            y = get_y(res)
            good =[i for i in range(len(res)) if (x[i] != None) and (y[i] != None)]
            F.extend([i] * len(good)) # file ids
            I.extend(good)
            X.extend(x)
            Y.extend(y)
            cx.extend([res.data["LONGITUDE"][i] for i in good])
            cy.extend([res.data["LATITUDE"][i] for i in good])
        result = {}
        result["LONGITUDE"] = cx
        result["LATITUDE"] = cy
        result["INDEX"] = I
        result["FILE_ID"] = F
        result["X"]  = X
        result["Y"] = Y
        self.last_data = CoordDB(["LONGITUDE", "LATITUDE", "FILE_ID", "INDEX", "X", "Y"], res, {"files": files})
        return self.last_data
    
    def train_files(self, files):
        data = self.load_data(files)
        self.train(data)
    
    def test_files(self, files):
        data = self.load_data(files)
        self.test(data)
#def summarize_last_result(self):
#def save_last_result(self, directory, prefix, save_all_data = False, merge = False):

