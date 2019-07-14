from .linear import LinearClassifier
from .knn import KnnClassifier
from core.classifier import *

#Knearest neighbours and linear classifier combo


class KnnLinClassifier(LinearClassifier):
    '''
    Run knn classifier, then run a linear classifier on the result.
    Parameters:
    x = data(1, "NEIGHBOURS_GOOD", "KNN_PROB", "ELLIPSE_VERT_MSE"), y = indicator("SCORE"), result = "LR_PROB", k = 10, x_knn = normalized_profile, reuse=False
    To run with reuse first, make sure both test and train have KNN_PROB defined
    '''
    def __init__(self, 
            #x_getter = lambda r: (1, r["NEIGHBOURS_GOOD"], access(r, "KNN_PROB"), min(r["ELLIPSE_VERT_MSE"], 0.2)), 
            x = data(1, "NEIGHBOURS_GOOD", "KNN_PROB", "ELLIPSE_VERT_MSE"),
            y = exceeds("SCORE", 0.5), 
            result = "LR_PROB", 
            k = 10,
            x_knn = normalized_profile,
            preprocess_fun = None, reuse = False):
        LinearClassifier.__init__(self, x, y, result, preprocess_fun)
        self.knn = KnnClassifier(x_knn, y, k)
        if reuse:
            self.test_files = self.test_files_reuse
            self.train_files = self.train_files_reuse
       

    def custom_loader(self, i): #tweak
        r = self.knn.reattach_result(self.knn_result, self.knn_result.meta["files"].index(i))
        #print (r.column_names)
        return r

    def train_files(self, files):
        self.knn.show_info = self.show_info
        self.knn.train_files(files)
        self.knn_result = self.knn.test_files(files)
        #the points with KNN_PROB > 0.1 will always be larger than 1/k
        k = self.knn.clf.n_neighbors
        self.knn_result.data["KNN_PROB"] = [ max(0.0, x - 1.0/k) for x in self.knn_result.data["KNN_PROB"]]
        data = self.load_data(files, data_loader = self.custom_loader)
        return self.train(data)


    def test_files(self, files):
        self.knn.show_info = self.show_info
        self.knn_result = self.knn.test_files(files)
        data = self.load_data(files, data_loader = self.custom_loader)
        return self.test(data)

    def train_files_reuse(self, files):
        data = self.load_data(files)
        return self.train(data)


    def test_files_reuse(self, files):
        data = self.load_data(files)
        return self.test(data)
