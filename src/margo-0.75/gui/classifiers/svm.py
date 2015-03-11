from core.classifier import * #classifier class
import core.xyz as xyz        #coorddb class (holds the data)
import math

#sklearn module - does the classification
from sklearn import svm

class SVMClassifier(CustomizedClassifier):
    '''
    Standard SVM classifier. Parameters: x=data("RAW_HEIGHTS"), y=exceeds("SCORE", 0.5), probability = False, class_weight = "auto", result = "SVM_CLASS"

    Classify: .svm.SVMClassifier(result="SVM_CLASS")
    Probability(slow): .svm.SVMClassifier(probability=True, result="SVM_PROB")

    For parameters and description see http://scikit-learn.org/stable/modules/svm.html.
    '''
    def __init__(self, x=data("RAW_HEIGHTS"), y=exceeds("SCORE", 0.5), class_weight = "auto", probability=False, result = "SVM_CLASS"):
        CustomizedClassifier.__init__(self, x, y, result, svm.SVC(class_weight = class_weight,probability=probability), predict_prob=probability)

    def show_stats(self, db):
        CustomizedClassifier.show_stats(self, db)
        #self.show_info("Regression coefficients: " + str(self.clf.coef_)+"\n")

