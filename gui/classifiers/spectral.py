#from core.classifier import CustomizedClassifier,isbad #classifier class

from core import xyz        #coorddb class (holds the data)
import math

#sklearn module - does the classification
import core.classifier as classifier
from core.classifier import *
import traceback
from sklearn import semi_supervised

class SpectralClassifier(classifier.CustomizedClassifier):
    '''
    Parameters: cltype="LabelSpreading", x=data("RAW_HEIGHTS"), y=exceeds("SCORE", 0.5), unlabelled_files = None, result = "SPECTRAL_PROB", **args
    where 
    cltype is a classifier in sklearn.semi_supervised (i.e. LabelSpreading or LabelPropagation)
    ** args are as in sklearn (e.g. kernel='knn', n_neighbors=3)
    '''

    def __init__(self, cltype="LabelSpreading", x=data("RAW_HEIGHTS"), y=exceeds("SCORE", 0.5), unlabelled_files = None, result = "SPECTRAL_PROB", y_is_1d=True, predict_prob = False, preprocess_fun = None, **args):
        f = getattr(semi_supervised, cltype)
        clf = f(**args)
        self.unlabelled_files = unlabelled_files if unlabelled_files is not None else []
        #clf.predict = clf.predict_proba #we do not want to get just True or False, but a number in between
        classifier.CustomizedClassifier.__init__(self, x, y, result, clf, y_is_1d, predict_prob, preprocess_fun)

    def train_files(self, files):
        data = self.load_data(files, True)
        udata = self.load_data(self.unlabelled_files, False)
        self.train(data, udata)

    #could create intermediate SemisupervisedClassifier
    def train(self, db, udb):
        #print db.data["Y"][:10]
        y = self.y_to_1d(db.data["Y"]) + [-1] * udb.size
        positive = len([yy for yy in y if yy > 0])
        self.show_info("Train total:"+ str(len(y))+"\n")
        self.show_info("Train positive:"+ str(positive)+"\n")
        self.show_info("Train unlabeled:"+ str(udb.size)+"\n")
        try:
            self.clf.fit(db.data["X"] + udb.data["X"], y)
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

    #todo: extract from the signature?
    @classmethod
    def default_args(cl):
        return ""


#core.classifier.cl_items.register(KnnClassifier, "k-nearest neighbours")
