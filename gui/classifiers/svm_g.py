#coding: utf-8
from core.classifier import * #classifier class
import core.xyz as xyz        #coorddb class (holds the data)
import math

#sklearn module - does the classification
from sklearn import svm
from sklearn import preprocessing

class SVM_G (svm.SVC):
    def __init__(self, enhanced = False):
        self.enhanced = enhanced
        svm.SVC.__init__(self,C=1, gamma=0.05)

    def fit(self, X_train, y_train):
        X_train = np.array(X_train)
        y_train = np.array(y_train).astype(float)
        if self.enhanced:
            t = 10*sum(y_train > 0)
            X_train1 = X_train[y_train > 0]
            X_train2 = X_train[y_train == 0][:t] #will never throw an error ;)
            X_tr = np.concatenate((X_train1, X_train2), axis = 0)
            y_tr = np.concatenate((np.array([1.0] *len(X_train1)), np.array([0.] * len(X_train2))), axis = 0)
        else:
            X_tr = X_train
            y_tr = y_train

        self.std_scale = preprocessing.StandardScaler().fit(X_tr)
        X_tr = self.std_scale.transform(X_tr)
        return svm.SVC.fit(self,X_tr, y_tr)

    def predict(self, X_test):
        X_test = np.array(X_test)
        X_test = self.std_scale.transform(X_test)
        return svm.SVC.predict(self,X_test)



class SVMClassifier_G(CustomizedClassifier):
    '''
    Standard SVM classifier (GŠ). Parameters: x=data("RAW_HEIGHTS"), y=exceeds("SCORE", 0.5), enhanced=False, result = "SVM_G"
    '''
    def __init__(self, x=data("RAW_HEIGHTS"), y=exceeds("SCORE", 0.5), enhanced=False, result = "SVM_G"):
        CustomizedClassifier.__init__(self, x, y, result, SVM_G(enhanced))

    def show_stats(self, db):
        CustomizedClassifier.show_stats(self, db)
        #self.show_info("Regression coefficients: " + str(self.clf.coef_)+"\n")

