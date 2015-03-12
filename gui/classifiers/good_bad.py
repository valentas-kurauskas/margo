from knnlin import KnnLinClassifier
from core.classifier import *
#Knearest neighbours and linear classifier combo

#Count the number of good neighbours and bad neighbours around the point
#Unfortunately, the weight on bad neighbours is slightly positive


def add_good_and_bad_neighbours(db):
    #the dumb way
    n = db.size
    x = db.data["LKS_x"]
    y = db.data["LKS_y"]
    z = db.data["KNN_PROB"]
    max_dist = 40
    include_self = False
    max_dist_sqr = max_dist**2
    r_good = []
    r_bad = []
    for i in range(db.size):
        good_neighbours = 0
        bad_neighbours = 0
        for j in range(db.size):
            if i == j and not include_self: continue
            d2 = (x[j] - x[i])**2 + (y[j] - y[i])**2
            if d2 < max_dist_sqr:
                if z[j] >=0.1:
                    good_neighbours+=1
                else:
                    bad_neighbours+=1
        r_good.append(good_neighbours)
        r_bad.append(bad_neighbours)
    db.data["GOOD_NB"] = r_good
    db.data["BAD_NB"] = r_bad
    db.column_names = db.column_names + ["GOOD_NB", "BAD_NB"]


class GoodBadNeighbours(KnnLinClassifier):
    '''
    Custom classifier example: manually create attributes BAD_NB and GOOD_NB
    Parameters: x = data(1, "KNN_PROB", "BAD_NB", "GOOD_NB"), x_knn = normalized_profile, preprocess_fun = good_bad.add_good_and_bad_neighbours
    '''
    def __init__(self):
        KnnLinClassifier.__init__(self,
                x = data(1, "KNN_PROB", "BAD_NB", "GOOD_NB"),
                x_knn = normalized_profile,
                preprocess_fun = add_good_and_bad_neighbours)

