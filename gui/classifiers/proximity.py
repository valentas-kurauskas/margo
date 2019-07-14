from .knnlin import KnnLinClassifier
from core.classifier import *
#Knearest neighbours and linear classifier combo


class ProximityClassifier(KnnLinClassifier):
    '''
    1. Sum KNN_PROB for detections closer than 40 meters (attribute KNN_PROB_NN). 
    2. Use linear classifier on KNN_PROB and KNN_PROB_NN.

    Parameters: x_knn = data("RAW_HEIGHTS"), y = exceeds("SCORE", 0.5), k=10, distance = 40, result = "LR_PROB", reuse=False
    '''
    def __init__(self, x_knn = data("RAW_HEIGHTS"), y = exceeds("SCORE", 0.5), k=10, distance = 40, result="LR_PROB",reuse=False):
        KnnLinClassifier.__init__(self, x=data(1, "KNN_PROB", "KNN_PROB_NN"), x_knn=x_knn, y=y, k=k,  preprocess_fun = lambda db: nn_update(db, "KNN_PROB", "KNN_PROB_NN", distance,False),result=result, reuse=reuse)



