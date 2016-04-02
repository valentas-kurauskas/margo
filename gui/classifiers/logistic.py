from core.classifier import * #classifier class
import core.xyz as xyz        #coorddb class (holds the data)
import math

#sklearn module - does the classification
from sklearn import linear_model


class LogisticClassifier(CustomizedClassifier):
    '''
    Parameters: x = data("AREA", "MAX_HEIGHT"), y = exceeds("SCORE", 0.5), result = "LC_PROB", preprocess_fun = None, fit_intercept=False
    '''
    def __init__(self, x= data("AREA", "MAX_HEIGHT"), y = exceeds("SCORE", 0.5), result = "LC_PROB", preprocess_fun=None, fit_intercept=False):
        CustomizedClassifier.__init__(self, 
                x,  
                y, 
                result, 
                linear_model.LinearRegression(fit_intercept = fit_intercept), 
                preprocess_fun = preprocess_fun) 

    def show_stats(self, db):
        CustomizedClassifier.show_stats(self, db)
        self.show_info("Regression coefficients: " + str(self.clf.coef_)+"\n")

