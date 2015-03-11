#from core.classifier import CustomizedClassifier,isbad #classifier class

from core import xyz        #coorddb class (holds the data)
import math

#sklearn module - does the classification
from sklearn import neighbors 
import core.classifier as classifier
from core.classifier import *
import traceback

class KnnClassifier(classifier.CustomizedClassifier):
    '''
    Parameters: x=data("RAW_HEIGHTS"), y=exceeds("SCORE", 0.5), k = 10, result = "KNN_PROB".
    '''

    def __init__(self, x=data("RAW_HEIGHTS"), y=exceeds("SCORE", 0.5), k = 10, result = "KNN_PROB"):
        clf = neighbors.KNeighborsClassifier(n_neighbors=k)
        clf.predict = clf.predict_proba #we do not want to get just True or False, but a number in between
        classifier.CustomizedClassifier.__init__(self, x, y, result, clf)

    def test(self, db):
        if self.r_col not in db.column_names:
            db.column_names.append(self.r_col)
        try:
            db.data[self.r_col] = [x[1] for x in self.clf.predict(db.data["X"])]
        except:
            self.show_info("knn.py: failed to test (wrong data format?): \n" + traceback.format_exc() + "\n")
            nan = float("nan")
            print len(db.data["X"])
            db.data[self.r_col] = [nan for x in range(len(db.data["X"]))]
        self.last_result = db
        return db
    
    #@classmethod
    #def name(cl):
    #    return "k-nearest neighbours"

    #todo: extract from the signature?
    @classmethod
    def default_args(cl):
        return "k=10"


#core.classifier.cl_items.register(KnnClassifier, "k-nearest neighbours")
