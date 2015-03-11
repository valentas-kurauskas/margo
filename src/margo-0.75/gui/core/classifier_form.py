# -*- coding: utf-8 -*-


from PyQt4 import QtCore, QtGui

from classifier_form_ui import Ui_Dialog
import config

#from classifier import *
#from knn_classifier import *
#from linear_classifier import *
import traceback
import os
import classifier
from classifiers import *
# classifiers.knn_classifier
from classifier import *
import numpy as np
import sklearn

class ClassifierDialog(Ui_Dialog):
    def __init__(self):
        self.dialog = QtGui.QDialog()
        self.setupUi(self.dialog)
        self.retranslateUi(self.dialog)

        self.dialog.setWindowFlags(self.dialog.windowFlags() | QtCore.Qt.WindowMinimizeButtonHint)
        
        self.comboBox.clear()
        self.comboBox.addItem("-Select classifier-")
        classifier.cl_items.refresh()
        for x in classifier.cl_items:
            module = x.__module__
            if module[:12] == "classifiers.":
                module = module[12:]
            if module != "": module = module +"."
            self.comboBox.addItem(module + x.__name__)
        #print classifier.cl_items.names()

        self.comboBox.activated[int].connect(self.set_classifier)
       
        self.load_from_cfg()
        self.pushButton.clicked.connect(self.addInputTrainDialog)
        self.pushButton_2.clicked.connect(self.addInputTestDialog)
        self.pushButton_3.clicked.connect(self.selectOutputDirDialog)
        #self.pushButton_4.clicked.connect(self.selectClassifierDialog)
        self.pushButton_5.clicked.connect(self.run)
        self.pushButton_6.clicked.connect(self.dialog.close)
        self.current_app =  None

    def set_classifier(self, i):
        if i == 0: return
        cl = classifier.cl_items[i-1]
        if not cl.__doc__ is None:
            self.clearOutput()
            self.appendOutput(cl.__name__ + ":" + str(cl.__doc__))
        self.update_clf_script()

    def update_clf_script(self):
        if self.comboBox.currentIndex() == 0:
            self.lineEdit_2.setText("")
            return
        #try:
        text = config.get("last_clf_script_" + str(self.comboBox.currentText()))
        #except:
        if (text == ""):
            text = self.comboBox.currentText() + "("+  classifier.cl_items[self.comboBox.currentIndex()-1].default_args()+")"
        self.lineEdit_2.setText(text)

    def set_current_app(self, app):
        self.current_app = app

    def load_from_cfg(self):
        self.listWidget.addItems(config.get_multiline("last_clf_train"))
        self.listWidget_2.addItems(config.get_multiline("last_clf_test"))
        self.lineEdit.setText(config.get("last_clf_output_dir"))
        #self.lineEdit_2.setText(config.get("last_clf_script"))
        self.checkBox.setChecked(config.get("last_clf_save_data_cols")=="True")
        self.checkBox_2.setChecked(config.get("last_clf_merge_output")=="True")
        ind = max(0,self.comboBox.findText(config.get("last_clf_classifier")))
        self.comboBox.setCurrentIndex(ind)
        self.update_clf_script()


    def addInputTrainDialog(self):        
        ddir = "" if self.listWidget.count() == 0 else os.path.dirname(str(self.listWidget.item(0).text()))
        s = QtGui.QFileDialog.getOpenFileNames(self.dialog, 'Select train files',ddir)
        if len(s) == 0: return
        self.listWidget.clear()
        s = [str(x) for x in s]
        self.listWidget.addItems(s)

    def addInputTestDialog(self):
        ddir = "" if self.listWidget_2.count() == 0 else os.path.dirname(str(self.listWidget_2.item(0).text()))
        s = QtGui.QFileDialog.getOpenFileNames(self.dialog, 'Select test files',ddir)
        if len(s) == 0: return
        self.listWidget_2.clear()
        s = [str(x) for x in s]
        self.listWidget_2.addItems(s)

    def selectOutputDirDialog(self):
        d = QtGui.QFileDialog.getExistingDirectory(self.dialog, 'Select output directory', self.lineEdit.text())
        if d != "":
            self.lineEdit.setText(d)

    #def selectClassifierDialog(self):
    #    s = QtGui.QFileDialog.getOpenFileName(self.dialog, 'Open classifier script', '*.py')
    #    self.lineEdit_2.setText(s)

    def clearOutput(self):
        self.textEdit.setText("")

    def appendOutput(self, text):
        self.textEdit.moveCursor(QtGui.QTextCursor.End)
        self.textEdit.insertPlainText(text)
        self.textEdit.moveCursor(QtGui.QTextCursor.End)
        self.textEdit.update()
        if self.current_app is not None:
            self.current_app.processEvents()

    def run(self):
        try:
            train_items = [str(self.listWidget.item(i).text()) for i in range(self.listWidget.count())]
            test_items =  [str(self.listWidget_2.item(i).text()) for i in range(self.listWidget_2.count())]
            config.set_multiline("last_clf_train", train_items)
            config.set_multiline("last_clf_test", test_items)
            config.set("last_clf_output_dir", self.lineEdit.text())
            #config.set("last_clf_script", self.lineEdit_2.toPlainText())
            config.set("last_clf_save_data_cols", str(self.checkBox.isChecked()))
            config.set("last_clf_merge_output", str(self.checkBox_2.isChecked()))
            config.set("last_clf_classifier", str(self.comboBox.currentText()))
            config.set("last_clf_script_" + str(self.comboBox.currentText()), self.lineEdit_2.toPlainText())
            self.appendOutput("Run pressed\n")

            cl = eval(str(self.lineEdit_2.toPlainText())) #expects something like KnnClassifier("ELLIPSE_PROFILE", "SCORE", 10)
            cl.show_info = self.appendOutput
            cl.train_files(train_items)

            result = cl.test_files(test_items)
            self.appendOutput("Statistics:\n")
            cl.show_stats(result)

            if self.checkBox.isChecked() and not self.checkBox_2.isChecked(): #save data columns, not combine
                cl.reattach_and_save_all(result, str(self.lineEdit.text())+os.sep)
            if self.checkBox.isChecked() and self.checkBox_2.isChecked(): #save data columns, combine
                cl.reattach_combine_and_save_all(result, str(self.lineEdit.text())+os.sep)
            if not self.checkBox.isChecked() and self.checkBox_2.isChecked(): #do not save data columns and combine
                path = str(self.lineEdit.text()) + os.sep + "clf.xyz"
                self.appendOutput("Saving to "+path)
                result.save_xyz(path)
            if not self.checkBox.isChecked() and not self.checkBox_2.isChecked(): #do not save data columns and do not combine
                cl.save_all(result,  str(self.lineEdit.text())+os.sep)
        except:
            self.appendOutput("Error: " + traceback.format_exc()+"\n")