# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'classifier_form.ui'
#
# Created: Thu Jul  9 10:36:32 2015
#      by: PyQt4 UI code generator 4.10.4
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtWidgets

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    def _fromUtf8(s):
        return s

try:
    _encoding = QtWidgets.QApplication.UnicodeUTF8
    def _translate(context, text, disambig):
        return QtWidgets.QApplication.translate(context, text, disambig, _encoding)
except AttributeError:
    def _translate(context, text, disambig):
        return QtWidgets.QApplication.translate(context, text, disambig)

class Ui_Dialog(object):
    def setupUi(self, Dialog):
        Dialog.setObjectName(_fromUtf8("Dialog"))
        Dialog.resize(502, 720)
        self.gridLayout = QtWidgets.QGridLayout(Dialog)
        self.gridLayout.setObjectName(_fromUtf8("gridLayout"))
        self.verticalLayout = QtWidgets.QVBoxLayout()
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.horizontalGroupBox = QtWidgets.QGroupBox(Dialog)
        self.horizontalGroupBox.setObjectName(_fromUtf8("horizontalGroupBox"))
        self.horizontalLayout = QtWidgets.QHBoxLayout(self.horizontalGroupBox)
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.listWidget = QtWidgets.QListWidget(self.horizontalGroupBox)
        self.listWidget.setObjectName(_fromUtf8("listWidget"))
        self.horizontalLayout.addWidget(self.listWidget)
        self.pushButton = QtWidgets.QPushButton(self.horizontalGroupBox)
        self.pushButton.setMinimumSize(QtCore.QSize(100, 0))
        self.pushButton.setObjectName(_fromUtf8("pushButton"))
        self.horizontalLayout.addWidget(self.pushButton)
        self.verticalLayout.addWidget(self.horizontalGroupBox)
        self.horizontalGroupBox_2 = QtWidgets.QGroupBox(Dialog)
        self.horizontalGroupBox_2.setObjectName(_fromUtf8("horizontalGroupBox_2"))
        self.horizontalLayout_2 = QtWidgets.QHBoxLayout(self.horizontalGroupBox_2)
        self.horizontalLayout_2.setObjectName(_fromUtf8("horizontalLayout_2"))
        self.listWidget_2 = QtWidgets.QListWidget(self.horizontalGroupBox_2)
        self.listWidget_2.setObjectName(_fromUtf8("listWidget_2"))
        self.horizontalLayout_2.addWidget(self.listWidget_2)
        self.pushButton_2 = QtWidgets.QPushButton(self.horizontalGroupBox_2)
        self.pushButton_2.setMinimumSize(QtCore.QSize(100, 0))
        self.pushButton_2.setObjectName(_fromUtf8("pushButton_2"))
        self.horizontalLayout_2.addWidget(self.pushButton_2)
        self.verticalLayout.addWidget(self.horizontalGroupBox_2)
        self.horizontalGroupBox_3 = QtWidgets.QGroupBox(Dialog)
        self.horizontalGroupBox_3.setObjectName(_fromUtf8("horizontalGroupBox_3"))
        self.horizontalLayout_3 = QtWidgets.QHBoxLayout(self.horizontalGroupBox_3)
        self.horizontalLayout_3.setObjectName(_fromUtf8("horizontalLayout_3"))
        self.lineEdit = QtWidgets.QLineEdit(self.horizontalGroupBox_3)
        self.lineEdit.setObjectName(_fromUtf8("lineEdit"))
        self.horizontalLayout_3.addWidget(self.lineEdit)
        self.pushButton_3 = QtWidgets.QPushButton(self.horizontalGroupBox_3)
        self.pushButton_3.setMinimumSize(QtCore.QSize(100, 0))
        self.pushButton_3.setObjectName(_fromUtf8("pushButton_3"))
        self.horizontalLayout_3.addWidget(self.pushButton_3)
        self.verticalLayout.addWidget(self.horizontalGroupBox_3)
        self.horizontalGroupBox_4 = QtWidgets.QGroupBox(Dialog)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Preferred, QtWidgets.QSizePolicy.Minimum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.horizontalGroupBox_4.sizePolicy().hasHeightForWidth())
        self.horizontalGroupBox_4.setSizePolicy(sizePolicy)
        self.horizontalGroupBox_4.setMinimumSize(QtCore.QSize(0, 0))
        self.horizontalGroupBox_4.setAlignment(QtCore.Qt.AlignLeading|QtCore.Qt.AlignLeft|QtCore.Qt.AlignTop)
        self.horizontalGroupBox_4.setObjectName(_fromUtf8("horizontalGroupBox_4"))
        self.horizontalLayout_4 = QtWidgets.QHBoxLayout(self.horizontalGroupBox_4)
        self.horizontalLayout_4.setContentsMargins(-1, 0, -1, 0)
        self.horizontalLayout_4.setObjectName(_fromUtf8("horizontalLayout_4"))
        self.checkBox_2 = QtWidgets.QCheckBox(self.horizontalGroupBox_4)
        self.checkBox_2.setObjectName(_fromUtf8("checkBox_2"))
        self.horizontalLayout_4.addWidget(self.checkBox_2)
        self.checkBox = QtWidgets.QCheckBox(self.horizontalGroupBox_4)
        self.checkBox.setObjectName(_fromUtf8("checkBox"))
        self.horizontalLayout_4.addWidget(self.checkBox)
        self.checkBox_3 = QtWidgets.QCheckBox(self.horizontalGroupBox_4)
        self.checkBox_3.setObjectName(_fromUtf8("checkBox_3"))
        self.horizontalLayout_4.addWidget(self.checkBox_3)
        self.verticalLayout.addWidget(self.horizontalGroupBox_4)
        self.verticalGroupBox_2 = QtWidgets.QGroupBox(Dialog)
        self.verticalGroupBox_2.setAlignment(QtCore.Qt.AlignLeading|QtCore.Qt.AlignLeft|QtCore.Qt.AlignTop)
        self.verticalGroupBox_2.setObjectName(_fromUtf8("verticalGroupBox_2"))
        self.verticalLayout_2 = QtWidgets.QVBoxLayout(self.verticalGroupBox_2)
        self.verticalLayout_2.setContentsMargins(-1, 0, -1, -1)
        self.verticalLayout_2.setObjectName(_fromUtf8("verticalLayout_2"))
        self.comboBox = QtWidgets.QComboBox(self.verticalGroupBox_2)
        self.comboBox.setObjectName(_fromUtf8("comboBox"))
        self.comboBox.addItem(_fromUtf8(""))
        self.verticalLayout_2.addWidget(self.comboBox)
        self.lineEdit_2 = QtWidgets.QTextEdit(self.verticalGroupBox_2)
        self.lineEdit_2.setAutoFillBackground(False)
        self.lineEdit_2.setReadOnly(False)
        self.lineEdit_2.setObjectName(_fromUtf8("lineEdit_2"))
        self.verticalLayout_2.addWidget(self.lineEdit_2)
        self.textEdit = QtWidgets.QTextEdit(self.verticalGroupBox_2)
        self.textEdit.setReadOnly(True)
        self.textEdit.setObjectName(_fromUtf8("textEdit"))
        self.verticalLayout_2.addWidget(self.textEdit)
        self.horizontalLayout_6 = QtWidgets.QHBoxLayout()
        self.horizontalLayout_6.setObjectName(_fromUtf8("horizontalLayout_6"))
        spacerItem = QtWidgets.QSpacerItem(40, 20, QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Minimum)
        self.horizontalLayout_6.addItem(spacerItem)
        self.pushButton_6 = QtWidgets.QPushButton(self.verticalGroupBox_2)
        self.pushButton_6.setObjectName(_fromUtf8("pushButton_6"))
        self.horizontalLayout_6.addWidget(self.pushButton_6)
        self.pushButton_5 = QtWidgets.QPushButton(self.verticalGroupBox_2)
        self.pushButton_5.setObjectName(_fromUtf8("pushButton_5"))
        self.horizontalLayout_6.addWidget(self.pushButton_5)
        self.verticalLayout_2.addLayout(self.horizontalLayout_6)
        self.verticalLayout.addWidget(self.verticalGroupBox_2)
        self.gridLayout.addLayout(self.verticalLayout, 0, 0, 1, 1)

        self.retranslateUi(Dialog)
        QtCore.QMetaObject.connectSlotsByName(Dialog)

    def retranslateUi(self, Dialog):
        Dialog.setWindowTitle(_translate("Dialog", "Run classifier", None))
        self.pushButton.setText(_translate("Dialog", "Train data...", None))
        self.pushButton_2.setText(_translate("Dialog", "Test data...", None))
        self.lineEdit.setText(_translate("Dialog", "../learned/", None))
        self.pushButton_3.setText(_translate("Dialog", "Output...", None))
        self.checkBox_2.setText(_translate("Dialog", "Combine results", None))
        self.checkBox.setText(_translate("Dialog", "Save all columns", None))
        self.checkBox_3.setText(_translate("Dialog", "Test separately", None))
        self.comboBox.setItemText(0, _translate("Dialog", "- Select classifier -", None))
        self.pushButton_6.setText(_translate("Dialog", "Close", None))
        self.pushButton_5.setText(_translate("Dialog", "Run", None))

