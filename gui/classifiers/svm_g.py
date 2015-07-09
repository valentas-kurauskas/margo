#coding: utf-8
from core.classifier import * #classifier class
import core.xyz as xyz        #coorddb class (holds the data)
import math

#sklearn module - does the classification
from sklearn import svm
from sklearn import linear_model
from sklearn import preprocessing

class SVM_G (svm.SVC):
    def __init__(self, enhanced = False):
        self.enhanced = enhanced
        svm.SVC.__init__(self, C=1, gamma=0.05)

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

class SVM_GV2 (svm.SVC):
    def __init__(self):
        svm.SVC.__init__(self, C=1, gamma=0.0, probability=True)
        max_dist = 100
        self.dist = max_dist**2
        self.std_scale = None
        self.clf_linear = None

    def fit(self, X, y):
        
        LO = [x[0] for x in X]
        LA = [x[1] for x in X]
        X = [x[2:] for x in X]
        
        X = np.array(X)
        y = np.array(y).astype(float)
        
        self.std_scale = preprocessing.StandardScaler().fit(X)
        X = self.std_scale.transform(X)
        svm.SVC.fit(self, X, y)
       

        prob_train = svm.SVC.predict_proba(self, X)
        prediction = svm.SVC.predict(self, X)

        prb_train1 = prob_train[:,1]
        prb_train = np.maximum(prediction, prb_train1)
        #prb_train = prob_train.tolist()
        #prb_train = [p[1] for p in prb_train]
        
        X_up = []
        n = prob_train.shape[0]

        for i in range(n):
            vector = [prb_train[i]]
            neighbours_train = []
            for j in range(n):
                if j != i:
                    d2 = (LO[j] - LO[i])**2 + (LA[j] - LA[i])**2
                    if d2 < self.dist:
                        neighbours_train.append(prb_train[j])
            vector.append(len(neighbours_train))
            vector.append(sum(neighbours_train))
            X_up.append(vector)
        
        X_updated = np.array(X_up)
        self.clf_linear = linear_model.LogisticRegression(C=0.02)
        return self.clf_linear.fit(X_updated, y)
    
    def predict(self, X):
        """
        X_val, LO_val, LA_val - lists
        """
        LO_val = [x[0] for x in X]
        LA_val = [x[1] for x in X]
        X_val = [x[2:] for x in X]

        X_val = np.array(X_val)
        X_val = self.std_scale.transform(X_val)
        
        prob_val = svm.SVC.predict_proba(self, X_val)
        prediction_val = svm.SVC.predict(self, X_val)
        prb_val1 = prob_val[:,1]
        prb_val = np.maximum(prediction_val, prb_val1)

        X_val_up = []
        n = prob_val.shape[0]
        for i in range(n):
            vector = [prb_val[i]]
            neighbours_val = []
            for j in range(n):
                if j != i:
                    d2 = (LO_val[j] - LO_val[i])**2 + (LA_val[j] - LA_val[i])**2
                    if d2 < self.dist:
                        neighbours_val.append(prb_val[j])
            vector.append(len(neighbours_val))
            vector.append(sum(neighbours_val))
            X_val_up.append(vector)

        X_val_up = np.array(X_val_up)
        return np.maximum(self.clf_linear.predict(X_val_up), prediction_val)


class SVMClassifier_GV2(CustomizedClassifier):
    #TODO change this!
    '''
    Standard SVM classifier (GŠ). Parameters: x=data("RAW_HEIGHTS"),
    y=exceeds("SCORE", 0.5), result = "SVM_GV2.1"
    '''
    #def __init__(self, x=data("RAW_HEIGHTS"), y=exceeds("SCORE", 0.5), result = "SVM_GV2"):
    def __init__(self, x=lambda z:data("LONGITUDE", "LATITUDE")(z) +
                 data("RAW_HEIGHTS")(z), y=exceeds("SCORE", 0.5), result =
                 "SVM_GV2.1"):
        CustomizedClassifier.__init__(self, x, y, result, SVM_GV2())

    def show_stats(self, db):
        CustomizedClassifier.show_stats(self, db)
        #self.show_info("Regression coefficients: " + str(self.clf.coef_)+"\n")


class SVMClassifier_G(CustomizedClassifier):
    '''
    Standard SVM classifier (GŠ). Parameters: x=data("RAW_HEIGHTS"), y=exceeds("SCORE", 0.5), enhanced=False, result = "SVM_G"
    '''
    def __init__(self, x=data("RAW_HEIGHTS"), y=exceeds("SCORE", 0.5), enhanced=False, result = "SVM_G"):
        CustomizedClassifier.__init__(self, x, y, result, SVM_G(enhanced))

    def show_stats(self, db):
        CustomizedClassifier.show_stats(self, db)
        #self.show_info("Regression coefficients: " + str(self.clf.coef_)+"\n")

