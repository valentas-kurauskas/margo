from core.classifier import * #classifier class
import core.xyz as xyz        #coorddb class (holds the data)
import math

#sklearn module - does the classification
from sklearn import linear_model


class LinearClassifier(CustomizedClassifier):
    '''
    Parameters: x = data("AREA", "HEIGHT"), y = exceeds("SCORE", 0.5), result = "LR_PROB", preprocess_fun = None
    '''
    def __init__(self, x= data("AREA", "HEIGHT"), y = exceeds("SCORE", 0.5), result = "LR_PROB", preprocess_fun=None):
        CustomizedClassifier.__init__(self, 
                x,  
                y, 
                result, 
                linear_model.LinearRegression(fit_intercept = False), 
                preprocess_fun = preprocess_fun) 

    def show_stats(self, db):
        CustomizedClassifier.show_stats(self, db)
        self.show_info("Regression coefficients: " + str(self.clf.coef_)+"\n")
