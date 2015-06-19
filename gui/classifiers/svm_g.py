#coding: utf-8
from core.classifier import * #classifier class
import core.xyz as xyz        #coorddb class (holds the data)
import math

#sklearn module - does the classification
from sklearn import svm

from sklearn import preprocessing

class SVM_G (svm.SVC):
    def __init__(self):
        svm.SVC.__init__(self,C=1, gamma=0.05)

    def fit(self, X_train, y_train):
        X_train = np.array(X_train)
        std_scale = preprocessing.StandardScaler().fit(X_train)
        X_train = std_scale.transform(X_train)
        return self.fit(X_train, y_train)

    def predict(self, X_test):
        X_test = np.array(X_test)
        X_test = std_scale.transform(X_test)
        return clf.predict(X_test)



class SVMClassifier_G(CustomizedClassifier):
    '''
    Standard SVM classifier (GŠ). Parameters: x=data("RAW_HEIGHTS"), y=exceeds("SCORE", 0.5), result = "SVM_G"
    '''
    def __init__(self, x=data("RAW_HEIGHTS"), y=exceeds("SCORE", 0.5), result = "SVM_G"):
        CustomizedClassifier.__init__(self, x, y, result, SVM_G())

    def show_stats(self, db):
        CustomizedClassifier.show_stats(self, db)
        #self.show_info("Regression coefficients: " + str(self.clf.coef_)+"\n")

