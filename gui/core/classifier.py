import sys
import math
import random
import os
from xyz import parse_file, CoordDB
import traceback
import numpy as np
import pickle

def isbad(v):
    try:
        if v == None:
            return True
        if not ((type(v) is list) or (type(v) is tuple)):
            return False
        if len(v) == 0:
            return True
        for x in v:
            if x == None: return True
            if math.isnan(x): return True
    except:
        print(v)
        raise

    return False



class Classifier(object):
    def __init__(self, use_fcache = True):
        self.last_data = None
        self.y_is_1d = True
        self.preprocess = lambda x: x #to do: find a nicer way to add new columns?
        self.use_fcache = use_fcache
        self.fcache = {}

    def show_info(self, x):
        print(x)

    @classmethod
    def name(cls):
        return cls.__name__
    
    def load_file(self, filename, data_loader,info=None):
            cn = ["LONGITUDE", "LATITUDE", "FNAME", "INDEX", "X"]
            if filename in self.fcache: 
                r = self.fcache[filename]
                if "Y" in r.data:
                    cn = cn + ["Y"]
                for x in r.data.keys():
                    if not x in cn: r.data.pop(x)
                r.column_names = [x for x in r.column_names if x in cn]
                return r
            if info is not None:
                self.show_info(info)
            res = data_loader(filename)
            self.preprocess(res)
            x = self.get_x(res)
            good =[i for i in range(res.size) if not isbad(x[i])]

            load_y = self.has_y(res) #loads Y if it is in the data
            self.show_info("Labels present: "+str(load_y)+"\n")

            if load_y:
                y = self.get_y(res)
                good = [i for i in good if not isbad(y[i])]
            
            X = [x[i] for i in good]
            cx = [res.data["LONGITUDE"][i] for i in good]
            cy = [res.data["LATITUDE"][i] for i in good]
            fnames = len(good) * [os.path.basename(filename)]
            dat = {"LONGITUDE": cx, "LATITUDE": cy, "FNAME": fnames, "INDEX": good, "X": X}
            res = CoordDB(cn, dat)
            if load_y:
                res.column_names.append("Y")
                res.data["Y"] = [y[i] for i in good]
            if self.use_fcache:
                self.fcache[filename] = res
            return res


    def load_data(self, files, load_y = None, data_loader = parse_file):
        #print ("Loading", str(files), data_loader)
        for j in range(len(files)):
            new = self.load_file(files[j], data_loader, "Loading file "+str(1+j) + " of "+str(len(files)) + " " + os.path.basename(files[j]) +"\n")
            if load_y and not "Y" in new.data:
                raise RuntimeError("Y (score) column is missing")
            if load_y==False and "Y" in new.data:
                new.data.pop("Y")
                new.column_names.remove("Y")
            new.data["FILE_ID"] = new.size * [j]
            new.column_names.insert(2,"FILE_ID")
            if j == 0:
                data = new.get_copy()
            else:
                data.union(new)
        data.meta = {"files": files} 
        self.last_result = data
        return self.last_result

    
    def train_files(self, files):
        data = self.load_data(files, True)
        return self.train(data)
      
    
    def test_files(self, files):
        data = self.load_data(files)
        return self.test(data)

    def reattach_result(self, db, file_id): # may be quite slow
        original = parse_file(db.meta["files"][file_id])
        #new_cols = [x for x in db.column_names if not x in  (original.column_names + ["FILE_ID", "INDEX"])]
        new_cols = [x for x in db.column_names if not x in ["LONGITUDE", "LATITUDE", "FILE_ID", "INDEX"]]
        if "FNAME" in db.column_names:
            new_cols.remove("FNAME")
        r = {}
        #print(new_cols)
        for c in new_cols:
            r[c] = [None] * original.size
        F = db.data["FILE_ID"]
        I = db.data["INDEX"]
        for i in range(db.size):
            if F[i] == file_id:
                for c in new_cols:
                    r[c][I[i]] = db.data[c][i]
        original.data.update(r)
        original.column_names = original.column_names + [x for x in new_cols if not (x in original.column_names)]
        return original

    def reattach_and_save_all(self, db, output_dir):
        for i in range(len(db.meta["files"])):
            r = self.reattach_result(db, i)
            p = output_dir + os.path.basename(db.meta["files"][i])
            self.show_info("Saving: "+p+"\n")
            r.save_xyz(p)

    def reattach_combine_and_save_all(self, db, output_dir, fname = "clf.xyz"):
        for i in range(len(db.meta["files"])):
            r = self.reattach_result(db, i)
            if i == 0:
                combined = r
            else:
                combined.union(r)
        p = output_dir + fname
        self.show_info("Saving: "+p+"\n")
        r.save_xyz(p)


    def save_all(self, db, output_dir):
        raise Exception("Not yet implemented")

    @classmethod 
    def default_args(cl):
        return ""

#def summarize_last_result(self):
#def save_last_result(self, directory, prefix, save_all_data = False, merge = False):

def descendants (cls):
    r = cls.__subclasses__()
    for x in cls.__subclasses__():
        r.extend(descendants(x))
    return r

class ClassifierContainer:
    def __init__(self):
        self.classes = []

    def register(classifier_class, caption):
        if classifier_class == CustomizedClassifier: return
        print ("register", classifier_class, caption)
        self.classes.append(classifier_class)

    def refresh(self):
        self.classes = [x for x in descendants(Classifier) if not x == CustomizedClassifier]
        
    def names(self):
        return [x.name() for x in self.classes]

    def __getitem__(self, i):
        return self.classes[i]

cl_items = ClassifierContainer()

def pit_index(y):
    i = 2
    while (i+1 < len(y)) and (y[i+1]-y[i] < -0.05): 
        #decrease less than 5 cm in 2m, a safer value is 0
        i+=1
    return i

def access(r, k, default = None):
    if not k in r.keys(): return default
    return r[k]


#creates X/Y, getters
def data_int(c):
    if isinstance(c, str):
        return lambda x: x[c] if c in x else None
    if isinstance(c, (int, long, float, complex)):
        return lambda x: c
    if isinstance(c, tuple):
        return lambda x: x[c[0]][c[1]] if c[0] in x else None
    if isinstance(c, list):
        f = [data_int(cc) for cc in c]
        return lambda x: [ff(x) for ff in f] 
    raise TypeError("The data getter input format is incorrect")

def data(*args):
    if len(args) == 1:
        return data_int(args[0])
    else: 
        return data_int(list(args))

#convert nans to zero
def remove_nans(f, x):
    r = f(x)
    if r is not None: r= list(np.nan_to_num(r))
    return r

def exceeds_int(x,v):
    try:
        fx = float(x)
        return fx > v
    except (TypeError, ValueError):
        return False


def exceeds(c, value = 0.5):
    return lambda x: exceeds_int(x[c], value)


def pit_var(r):
    p = r["ELLIPSE_PROFILE"]
    v = r["ELLIPSE_PROFILE_VAR"]
    if (p == None or v == None):
        return None
    u = min(pit_index(p), len(p)-1)
    return v[u]

def normalized_profile(r):
    if type(r) == dict:
        y = r["ELLIPSE_PROFILE"]
    else:
        y = r
    if isbad(y):
        return None
    mn = min(y)
    i = pit_index(y)
    if i+1 <= len(y): #continue #don't draw
        mn = y[i] #normalize by height measured from pit bottom
    hfactor = 1.0/i
    y = [yy - mn for yy in y]
    mx = max(y)
    if (mx > 0):
        y = [yy * 1.0 / mx for yy in y]

    r = []
    for step in range(1,len(y)):
        x = 0.1 * step / hfactor
        ii = int(math.floor(x))
        y1 = y[ii] if ii < len(y) else y[-1]
        y2 = y[ii+1] if ii+1 < len(y) else y[-1]
        r.append(y1 + (x - ii) * (y2-y1))
    return r

def round2(x):
        if type(x)!=float:
            return x
        return 0.01 * round(x  * 100)

def histogram(x, n_bins_max=4):
    n = min(len(set(x)), n_bins_max)
    mx = min(x)
    step = ((max(x) - mx) * 1.0 / n)
    if step == 0:
        return [ (mx, mx, len(x))]
    istep = 1.0/step
    #counts = dict([(i, 0) for i in range(n+1)])o
    counts = {}
    mins = {}
    maxs = {}
    for xx in x:
        k = int(math.floor( (xx - mx) * istep + 0.5))
        if not k in counts:
            counts[k]=1
            mins[k] = xx
            maxs[k] = xx
        else:
            counts[k]+=1
            mins[k] = min(xx, mins[k])
            maxs[k] = max(xx, maxs[k])
    return [ (mins[a], maxs[a], b) for (a,b) in sorted(counts.iteritems())]

#print(histogram([1,2,21,1,11,1,1,2,1,1]))

#todo: computes fun(db, nn[i]) for each i = 1..size, where nn[i] is the list of points within max_dist from the i-th point
#def nn_update(colname, fun, max_dist, include_self=True):
#this: a more primitive version
def nn_update(db, colname, newcolname, max_dist, include_self=True):
    #the dumb way
    n = db.size
    x = db.data["LONGITUDE"]
    y = db.data["LATITUDE"]
    z = db.data[colname]
    max_dist_sqr = max_dist**2
    result = []
    for i in range(db.size):
        neighbours = []
        for j in range(db.size):
            if i == j and not include_self: continue
            d2 = (x[j] - x[i])**2 + (y[j] - y[i])**2
            if d2 < max_dist_sqr:
                neighbours.append(j)
        result.append( sum(z[k] for k in neighbours if z[k] is not None) )
    db.data[newcolname] = result
    db.column_names = [x for x in db.column_names if x != newcolname] + [newcolname]
    print ("added new column: ", newcolname, db.data[newcolname][:15])





class CustomizedClassifier(Classifier):
    def __init__(self, x_getter, y_getter, r_col, clf, y_is_1d=True, predict_prob = False, preprocess_fun = None):
        Classifier.__init__(self)
        self.clf = clf
        self.x_getter = x_getter #lambda row: normalize_profile(row["TEMPLATE_PROFILE"])
        self.y_getter = y_getter #lambda row: float(row["NICE_V" > 0)]
        self.r_col = r_col
        self.y_is_1d = y_is_1d
        self.predict_prob = predict_prob
        if preprocess_fun is not None:
            self.preprocess = preprocess_fun

    def get_x(self,db):
        return [self.x_getter(db.get_row_as_dict(i)) for i in range(db.size)]

    def get_y(self,db):
        return [self.y_getter(db.get_row_as_dict(i)) for i in range(db.size)]
    #def get_x(self, db):
    #    if self.x_col in xyz.VECTOR_FEATURES: 
    #        return [t for t in db.data[self.x_col]]
    #    return [[t] for t in db.data[self.x_col]]

    #def get_y(self, db):
    #    return [[float(t > 0)] for t in db.data[self.y_col]]

    def has_y(self, db):
        #return self.y_col in db.data.keys()
        try:
            self.y_getter(db.get_row_as_dict(0))
            return True
        except:
            return False

    def train(self, db):
        #print db.data["Y"][:10]
        y = self.y_to_1d(db.data["Y"])
        positive = len([yy for yy in y if yy > 0])
        self.show_info("Train total:"+ str(len(y))+"\n")
        self.show_info("Train positive:"+ str(positive)+"\n")
        try:
            self.clf.fit(db.data["X"], y)
        except MemoryError:
            self.show_info("Out of memory.")
            raise
        except:
            self.show_info(traceback.format_exc())
            print ('''Saving X to X.pickle; python -ic "import pickle; X = pickle.load(open('X.pickle','rb'))" to find problem''')
            f = open("X.pickle", "wb")
            pickle.dump(db.data["X"],f)
            f.close()
            raise


    def y_to_1d(self, y):
        if self.y_is_1d:
            return  [yy for yy in y ] #1d
        else:
            return [yy[0] for yy in y ] #not 1d

    def test(self, db):
        if self.r_col not in db.column_names:
            db.column_names.append(self.r_col)
        if self.predict_prob:
            pred = [x[1] for x in self.clf.predict_proba(db.data["X"])]
        else:
            pred = self.y_to_1d(self.clf.predict(db.data["X"]))
        #print (pred[:3], type(pred), type(pred[0]))
        db.data[self.r_col] = pred
        self.last_result = db
        return db

    def format1(self, l):
        return "\t".join(map(lambda x: "{0:.2f}".format(x[0]) + "-"+"{0:.2f}".format(x[1]) + ": " + str(x[2]), l))


    def show_stats(self, db):
        r = {}
        try:
            if "Y" in db.data.keys():
                Y = self.y_to_1d(db.data["Y"])
                prob = map(round2, db.data[self.r_col])
                for i in range(db.size):
                    if Y[i] in r:
                        r[Y[i]].append(prob[i])
                    else:
                        r[Y[i]] = [prob[i]]
                r2 = [ (k, histogram(v)) for (k,v) in sorted(r.iteritems())]
                for k,v in r2:
                    self.show_info(str(k) + ": \n")
                    self.show_info(self.format1(v) + "\n")
                mse = sum((Y[j] - prob[j])**2 for j in range(len(Y))) / (1.0 * len(Y))
                self.show_info("MSE: "+"{0:.4f}".format(mse) + "\n")
                avgpred = sum(prob) / len(prob)
                #print("SSSSSSSum: ", sum(Y))
                avgy = sum(Y) * 1.0 / len(Y)
                self.show_info("Avg prediction: "+"{0:.4f}".format(avgpred) + ", avg Y: " + "{0:.4f}".format(avgy)+"\n")
            else:
                self.show_info(self.format1(histogram(map(round2, db.data[self.r_col]))) + "\n")
        except:
            self.show_info("Unexpected error in show_info: "+traceback.format_exc() + "\n")

