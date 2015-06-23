from lks_wgs import lks94_to_wgs
from osgeo import ogr,osr
import os
import csv

VECTOR_FEATURES = ["LOCAL_VARIANCE", "TEMPLATE_CORRELATION", "ELLIPSE_A", "ELLIPSE_B", "ELLIPSE_PHI", "ELLIPSE_MSE", "RAW_HEIGHTS", "ELLIPSE_PROFILE", "ELLIPSE_PROFILE_VAR", "RESCALED"]
INT_FEATURES = ["ID",  "NICE_V", "CONFIRMED", "NEIGHBOURS_GOOD", "NEIGHBOURS_TOTAL", "RAW_COLS", "RAW_ROWS", "RESCALED_WIDTH", "ELLIPSE_N_GOOD"]

NUMERIC_FEATURES = INT_FEATURES + ["MAX_HEIGHT","AREA", "MEDIAN_CORRELATION", "CIRCLENESS", "ELLIPSE_COMBINED_MSE", "ELLIPSE_ASYMMETRY", "NICE_V", "ELEVATION", "NEIGHBOURS_GOOD", "NEIGHBOURS_TOTAL", "RAW_COLS", "RAW_ROWS", "KNN_PROB","ELLIPSE_VERT_MSE", "LR_PROB", "SCORE", "FURROW_DISTANCE", "DITCH_DISTANCE", "NOISE1", "NOISE2", "NOISE3"]
#!/usr/bin/python3
# -*- coding: utf-8 -*-

#stores values by column
class CoordDB:
    def __init__(self, column_names, data, meta = None):
        self.data = {}
        self.meta = meta if meta is not None else {}
        column_names = [x for x in column_names if not x in VECTOR_FEATURES] + [x for x in column_names if x in VECTOR_FEATURES]
        self.column_names = column_names

        if type(data) == dict: #dict of lists also accepted
            self.data = data
            self.size = len(data.values()[0]) if len(data) > 0 else 0
            return

        self.size = len(data)
        for nm in column_names:
            self.data[nm] = []
        for element in data:
            for c in column_names:
                if c in element.keys():
                    self.data[c].append(element[c])
                else:
                    self.data[c].append(None)
    
    def get_copy(self):
        k = self.data.keys()
        v = [[x for x in self.data[kk]] for kk in k]
        return CoordDB(self.column_names, dict(zip(k,v)), self.meta)

    #slow function to get all points inside a bounding box, returns indices
    def filter_rectangle(self, minx, maxx, miny, maxy, convert_to_wgs=False):
        x = self.data["LONGITUDE"]
        y = self.data["LATITUDE"]
        if convert_to_wgs:
            x,y = zip(*[lks94_to_wgs(a,b) for a,b in zip(x,y)])
        return [i for i in range(self.size)
                       if x[i] >= minx and x[i] <= maxx
                       and y[i] >= miny and y[i] <= maxy]

    def get_projection(self, column_name, indices):
        v = self.data[column_name]
        return [v[i] for i in indices]

    def get_row(self, i):
        return [self.data[x][i] for x in self.column_names]

    def get_row_as_dict(self, i):
        return dict(zip(self.column_names, [self.data[x][i] for x in self.column_names]))


    def insert_row(self, rowdict):
        if not "ID" in rowdict:
            rowdict["ID"] = self.size
        print (rowdict)
        for k in self.column_names:
            if k in rowdict:
                z = rowdict[k]
            else:
                z = default_value(k)
            self.data[k].append(z)
        self.size +=1

    def get_item(self, i, j):
        if type(j) == type(0):
            j = self.column_names[j]
        if (not (j in self.data.keys())):
            return None
        return self.data[j][i]

    def set_item(self, i, j, x):
        if type(j) == type(0):
            j = self.column_names[j]
        self.data[j][i] = x


    def find_name(self, name):
        return self.data["NAME"].index(name)

    def closest(self, LONGITUDE, LATITUDE, max_n, max_dist, restrict_to_rows=None):
        X = self.data["LONGITUDE"]
        Y = self.data["LATITUDE"]
        if restrict_to_rows is not None:
            rows = restrict_to_rows
        else: 
            rows = range(self.size)
        ds = [ (X[i] - LONGITUDE)*(X[i]-LONGITUDE) + (Y[i] - LATITUDE) * (Y[i] - LATITUDE) for i in rows]
        ds = [(ds[i], rows[i]) for i in range(len(rows)) if ds[i] <= max_dist * max_dist * 4]
        ds.sort(key = lambda x: x[0])
        m = min(max_n, len(ds))
        return [x[1] for x in ds[:m]]

    def save_xyz(self, fname, skip_indices = [], skip_columns = []):
        print ("saving table: ",fname)
        f = open(fname, "w")
        for i in range(self.size):
            if i in skip_indices: continue
            for key in self.column_names:
              if (key != "LONGITUDE") and (key != "LATITUDE"):
                if key in skip_columns: continue
                #uncomment to colour points in Global Mapper
                #if (key == "NAME"):
                #        f.write("GM_TYPE=Unknown Point Feature\n")
                #        f.write("NAME = \n")
                #        continue
                #if (key == "PROBA"):
                #        if val > 0.19:
                #            f.write("POINT_SYMBOL=Dot - Red\n")
                #        elif val > 0.01:
                #            f.write("POINT_SYMBOL=Dot - Yellow\n")
                #        else:
                #            f.write("POINT_SYMBOL=Dot\n")
                f.write(key)
                f.write("=")
                val = self.data[key][i]
                if (key in VECTOR_FEATURES) and (val !=None):
                    for x in val[:-1]:
                        f.write(str(x))
                        f.write(" ")
                    if len(val) > 0:
                        f.write (str(val[-1]))
                else:
                        f.write(str(val))
                f.write("\n")
            f.write(str(self.data["LONGITUDE"][i]) + ","+str(self.data["LATITUDE"][i])+"\n\n")
        f.close()

    def save_csv(self, fname, skip_indices = None, skip_columns = None):
            if skip_indices is None: skip_indices = []
            if skip_columns is None: skip_columns = []
            with open(fname, "wb") as f:
                writer = csv.writer(f)
                cols_to_save = [x for x in self.column_names if not x in skip_columns]
                #cols = [m.headerData(i,QtCore.Qt.Horizontal, QtCore.Qt.DisplayRole) for i in range(m.columnCount())]
                cols =  [unicode(c).encode('utf8') for c in cols_to_save]
                writer.writerow(cols)

                for i in range(self.size):
                    if i in skip_indices: continue 
                    rowdata = []
                    for col in cols_to_save:
                        d = self.data[col][i]
                        if d is not None:
                            rowdata.append(unicode(str(d)).encode('utf8'))
                        else:
                            rowdata.append('')
                    writer.writerow(rowdata)
    

    def save_shp(self, fname, projection, skip_indices=None, skip_columns =None):
        if skip_indices is None: skip_indices = []
        if skip_columns is None: skip_columns = []
        driver = ogr.GetDriverByName("ESRI Shapefile")
        if os.path.exists(fname):
           os.remove(fname)
        data_source = driver.CreateDataSource(fname)
        #layerName = os.path.splitext(os.path.split(fname)[1])[0]
        layer = data_source.CreateLayer("data", projection, ogr.wkbPoint)
        cols_to_save = [x for x in self.column_names if not x in skip_columns]
        #cols_to_save = ["AREA", "MAX_HEIGHT", "SCORE"]

        namedict = {}

        #layer.CreateField(ogr.FieldDefn("ID", ogr.OFTInteger))

        for col in cols_to_save:
           if col in INT_FEATURES:
               f = ogr.FieldDefn(col, ogr.OFTInteger)
           elif col in NUMERIC_FEATURES + ["LONGITUDE", "LATITUDE"]:
               f=ogr.FieldDefn(col, ogr.OFTReal)
           else:
               f = ogr.FieldDefn(col, ogr.OFTString)
               f.SetWidth(24)
           field = layer.CreateField(f)
           ld = layer.GetLayerDefn()
           namedict[col] = ld.GetFieldDefn(ld.GetFieldCount()-1).GetName()

        #print namedict
        for i in range(self.size):
            if i in skip_indices: continue
            feature = ogr.Feature(layer.GetLayerDefn())
            #feature.SetField("ID", i)
            for col in cols_to_save:
                 d = self.data[col][i]
                 if not d in NUMERIC_FEATURES:
                     d = str(d)
                 feature.SetField(namedict[col], d)
            wkt = "POINT(%f %f)" %  (float(self.data['LONGITUDE'][i]) , float(self.data['LATITUDE'][i]))
            point = ogr.CreateGeometryFromWkt(wkt)
            feature.SetGeometry(point)
            layer.CreateFeature(feature)
            feature.Destroy()
        data_source.Destroy()

    def union(self, db, rename_ids = False):
        a = set(self.column_names) - set(db.column_names)
        a = set.union(a, set(db.column_names) - set(self.column_names))
        if len(a) > 0:
            print ("Inconsistent columns for CoordDB union:", a)
            raise RuntimeError("Inconsistent columns: "+str(a))
        for x in self.column_names:
            self.data[x] += db.data[x]
        self.size += db.size
        if rename_ids:
            self.data["ID"] = range(self.size)


    def join(self, db, cols, cols_to_add =None):     
        if cols_to_add is None:
            cols_to_add = [x for x in db.column_names if not x in self.column_names]
        D = {}
        for i in range(db.size):
            D[tuple(db.data[x][i] for x in cols)] = i
            
        new_names = [x for x in cols_to_add if not x in self.column_names]

        for x in new_names:
            self.data[x] = self.size * [None]
        
        for i in range(self.size):
            k = tuple(self.data[x][i] for x in cols)
            if not k in D: continue
            ii = D[k]
            for c in cols_to_add:
                self.data[c][i] = db.data[c][ii]
        self.column_names += new_names

    def join_neighbours(self, db, cols, cols_to_add =None, radius = 10): 
        radius = radius
        print "Joining with radius: "+str(radius)
        #to do: use projection info.
        binx = [ int(x * 2.0 /radius) for x in self.data['LONGITUDE']]
        biny = [ int(y * 2.0 /radius) for y in self.data['LATITUDE']]

        binx2 = [ int(x * 2.0/radius) for x in db.data['LONGITUDE']]
        biny2 = [ int(y * 2.0/radius) for y in db.data['LATITUDE']]

        if cols_to_add is None:
            cols_to_add = [x for x in db.column_names if not x in self.column_names]
        D = {}
        for i in range(db.size):
            l = [db.data[x][i] for x in cols]
            xx= binx2[i]
            yy = biny2[i]
            for a in range(-1, 2):
                for b in range (-1, 2):
                    k = tuple( [xx+a, yy+b] + l)
                    if not k in D:
                        D[k] = [i]
                    else:
                        D[k] += [i]
            
        new_names = [x for x in cols_to_add if not x in self.column_names]

        for x in new_names:
            self.data[x] = self.size * [None]
        
        lon = self.data["LONGITUDE"]
        lat = self.data["LATITUDE"]
        for i in range(self.size):
            k = tuple([binx[i], biny[i]] + [self.data[x][i] for x in cols])
            #if (i == 64): print i, k, k in D, D[k]
            if not k in D: continue
            nearest = db.closest(lon[i], lat[i], 1, radius, D[k]) #not optimal speed - quadratic
            #print D[k]
            #print nearest
            if len(nearest) == 0: continue
            for c in cols_to_add:
                self.data[c][i] = db.data[c][nearest[0]]
        self.column_names += new_names


VERBOSITY = 1

def default_value(col):
        if col in INT_FEATURES: return 0
        if col in NUMERIC_FEATURES: return float("nan")
        if col in VECTOR_FEATURES: return []
        else: return None


def parse_file(fname): #if too large, we should store keys in a header
    f = open(fname, "r")
    #line = f.readline()
    current_object = {'ID': 0}
    result = []
    all_cols = ["LONGITUDE", "LATITUDE", "ID"]
    lineno = -1
    exceptions = 0
    while (True):
        line = f.readline()
        if (line == ""):
            break
        lineno+=1
        if ( (line == "\r\n") or (line == "\n") ):
            if ( len(current_object.keys()) !=1):
                result.append(current_object)
                #print "append"
            current_object = {'ID': len(result)}
        eqpos = line.find("=")
        if eqpos != -1:
            k = line[:eqpos]

            if not (k in all_cols):
                all_cols.append(k)
            v = line[eqpos+1:]
            v = v.strip()
            v = v.replace("1.#QNAN0", "nan") #windows gives different strange output
            v = v.replace("1.#QNAN", "nan") #windows gives different output
            if k in NUMERIC_FEATURES:
                try:
                    current_object[k] = float(v)
                except:
                    exceptions += 1
                    if (VERBOSITY >= 2):
                        print("parse exception", lineno, k, v)
                    continue
                if k in INT_FEATURES:
                    current_object[k] = int(current_object[k])
            elif k in VECTOR_FEATURES:
                try:
                    current_object[k] = list(map(float, v.split(" ")))
                except:
                    exceptions += 1
                    if (VERBOSITY>=2):
                        print ("exception: ", lineno,  k, v.split(" "))
                    continue
            else: current_object[k] = v;
        else:
            words = line.split(",")
            if len(words)>=2:
                current_object["LONGITUDE"] = float(words[0])
                current_object["LATITUDE"] = float(words[1])
        #print "read"
    if (len(current_object.keys()) > 1):
        result.append(current_object)
    f.close()
    if (exceptions > 0 and VERBOSITY >= 1):
        print (str(exceptions) + " parse exceptions. Set xyz.VERBOSITY=2 to show each exception")
    return CoordDB(all_cols, result)



def load_shp(fname, convert_to_projection=None):
    driver = ogr.GetDriverByName("ESRI Shapefile")
    data = driver.Open(fname)
    layer = data.GetLayerByIndex(0)
    
    trans = None
    if convert_to_projection != None:
        origproj = layer.GetSpatialRef()
        trans= osr.CoordinateTransformation(origproj, convert_to_projection)

    ld = layer.GetLayerDefn()
    fields = [ld.GetFieldDefn(x) for x in  range(ld.GetFieldCount())]
    field_names = [x.GetName() for x in fields]
    if not "ID" in field_names: field_names = ["ID"] + field_names
    records = []
    for i in range(layer.GetFeatureCount()):
        feat = layer.GetFeature(i)
        geom = feat.GetGeometryRef().Centroid()
        if trans is not None:
            geom.Transform(trans)
        pt = geom.GetPoint()
        items = feat.items()
        if not "ID" in items:
            items["ID"] = i
        record = list(pt[:2]) + [items[k] for k in field_names]
        records.append(record)
    records = zip(*records)
    names = ["LONGITUDE", "LATITUDE"] + field_names
    return CoordDB(names, dict(zip(names,records)))

def load_from_file(fname, add_fname = False):
    ext = os.path.splitext(fname)[1]
    if ext == ".shp":
        result = load_shp(fname)
    elif ext == ".xyz":
        result = parse_file(fname)
    else: raise RuntimeError("Unknown extension")
    if add_fname:
        col = "FNAME"
        while col in result.column_names:
            col=col + "_"
        result.column_names.append(col)
        result.data[col] = result.size * [os.path.basename(fname)]
    return result


def merge_files(fnames, rename_ids = False, add_fname = False):
    db = load_from_file(fnames[0], add_fname)
    for f in fnames[1:]:
        db.union(load_from_file(f, add_fname), rename_ids)
    return db
