# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'classifier_form.ui'
#
# Created: Tue Apr 15 17:24:24 2014
#      by: PyQt4 UI code generator 4.10.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    def _fromUtf8(s):
        return s

try:
    _encoding = QtGui.QApplication.UnicodeUTF8
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig, _encoding)
except AttributeError:
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig)

class Ui_Dialog(object):
    def setupUi(self, Dialog):
        Dialog.setObjectName(_fromUtf8("Dialog"))
        Dialog.resize(511, 516)
        self.listWidget = QtGui.QListWidget(Dialog)
        self.listWidget.setGeometry(QtCore.QRect(10, 10, 351, 101))
        self.listWidget.setObjectName(_fromUtf8("listWidget"))
        self.pushButton = QtGui.QPushButton(Dialog)
        self.pushButton.setGeometry(QtCore.QRect(390, 40, 98, 27))
        self.pushButton.setObjectName(_fromUtf8("pushButton"))
        self.listWidget_2 = QtGui.QListWidget(Dialog)
        self.listWidget_2.setGeometry(QtCore.QRect(10, 120, 351, 101))
        self.listWidget_2.setObjectName(_fromUtf8("listWidget_2"))
        self.pushButton_2 = QtGui.QPushButton(Dialog)
        self.pushButton_2.setGeometry(QtCore.QRect(390, 150, 98, 27))
        self.pushButton_2.setObjectName(_fromUtf8("pushButton_2"))
        self.lineEdit = QtGui.QLineEdit(Dialog)
        self.lineEdit.setGeometry(QtCore.QRect(10, 230, 351, 27))
        self.lineEdit.setObjectName(_fromUtf8("lineEdit"))
        self.pushButton_3 = QtGui.QPushButton(Dialog)
        self.pushButton_3.setGeometry(QtCore.QRect(390, 230, 101, 27))
        self.pushButton_3.setObjectName(_fromUtf8("pushButton_3"))
        self.lineEdit_2 = QtGui.QLineEdit(Dialog)
        self.lineEdit_2.setGeometry(QtCore.QRect(10, 310, 351, 27))
        self.lineEdit_2.setText(_fromUtf8(""))
        self.lineEdit_2.setObjectName(_fromUtf8("lineEdit_2"))
        self.checkBox = QtGui.QCheckBox(Dialog)
        self.checkBox.setGeometry(QtCore.QRect(10, 270, 171, 22))
        self.checkBox.setObjectName(_fromUtf8("checkBox"))
        self.checkBox_2 = QtGui.QCheckBox(Dialog)
        self.checkBox_2.setGeometry(QtCore.QRect(240, 270, 211, 22))
        self.checkBox_2.setObjectName(_fromUtf8("checkBox_2"))
        self.textEdit = QtGui.QTextEdit(Dialog)
        self.textEdit.setGeometry(QtCore.QRect(10, 350, 491, 111))
        self.textEdit.setObjectName(_fromUtf8("textEdit"))
        self.pushButton_5 = QtGui.QPushButton(Dialog)
        self.pushButton_5.setGeometry(QtCore.QRect(390, 480, 98, 27))
        self.pushButton_5.setObjectName(_fromUtf8("pushButton_5"))
        self.pushButton_6 = QtGui.QPushButton(Dialog)
        self.pushButton_6.setGeometry(QtCore.QRect(280, 480, 98, 27))
        self.pushButton_6.setObjectName(_fromUtf8("pushButton_6"))

        self.retranslateUi(Dialog)
        QtCore.QMetaObject.connectSlotsByName(Dialog)

    def retranslateUi(self, Dialog):
        Dialog.setWindowTitle(_translate("Dialog", "Run classifier", None))
        self.pushButton.setText(_translate("Dialog", "Train data...", None))
        self.pushButton_2.setText(_translate("Dialog", "Test data...", None))
        self.lineEdit.setText(_translate("Dialog", "../learned/", None))
        self.pushButton_3.setText(_translate("Dialog", "Output...", None))
        self.checkBox.setText(_translate("Dialog", "Save data columns", None))
        self.checkBox_2.setText(_translate("Dialog", "Combine results to one file", None))
        self.pushButton_5.setText(_translate("Dialog", "Run", None))
        self.pushButton_6.setText(_translate("Dialog", "Close", None))


# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'classifier_form.ui'
#
# Created: Mon Feb  2 18:52:14 2015
#      by: PyQt4 UI code generator 4.10.4
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    def _fromUtf8(s):
        return s

try:
    _encoding = QtGui.QApplication.UnicodeUTF8
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig, _encoding)
except AttributeError:
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig)

class Ui_Dialog(object):
    def setupUi(self, Dialog):
        Dialog.setObjectName(_fromUtf8("Dialog"))
        Dialog.resize(511, 563)
        self.listWidget = QtGui.QListWidget(Dialog)
        self.listWidget.setGeometry(QtCore.QRect(10, 10, 351, 101))
        self.listWidget.setObjectName(_fromUtf8("listWidget"))
        self.pushButton = QtGui.QPushButton(Dialog)
        self.pushButton.setGeometry(QtCore.QRect(390, 40, 98, 27))
        self.pushButton.setObjectName(_fromUtf8("pushButton"))
        self.listWidget_2 = QtGui.QListWidget(Dialog)
        self.listWidget_2.setGeometry(QtCore.QRect(10, 120, 351, 101))
        self.listWidget_2.setObjectName(_fromUtf8("listWidget_2"))
        self.pushButton_2 = QtGui.QPushButton(Dialog)
        self.pushButton_2.setGeometry(QtCore.QRect(390, 150, 98, 27))
        self.pushButton_2.setObjectName(_fromUtf8("pushButton_2"))
        self.lineEdit = QtGui.QLineEdit(Dialog)
        self.lineEdit.setGeometry(QtCore.QRect(10, 230, 351, 27))
        self.lineEdit.setObjectName(_fromUtf8("lineEdit"))
        self.pushButton_3 = QtGui.QPushButton(Dialog)
        self.pushButton_3.setGeometry(QtCore.QRect(390, 230, 101, 27))
        self.pushButton_3.setObjectName(_fromUtf8("pushButton_3"))
        self.checkBox = QtGui.QCheckBox(Dialog)
        self.checkBox.setGeometry(QtCore.QRect(10, 270, 171, 22))
        self.checkBox.setObjectName(_fromUtf8("checkBox"))
        self.checkBox_2 = QtGui.QCheckBox(Dialog)
        self.checkBox_2.setGeometry(QtCore.QRect(240, 270, 211, 22))
        self.checkBox_2.setObjectName(_fromUtf8("checkBox_2"))
        self.textEdit = QtGui.QTextEdit(Dialog)
        self.textEdit.setGeometry(QtCore.QRect(10, 390, 491, 111))
        self.textEdit.setObjectName(_fromUtf8("textEdit"))
        self.pushButton_5 = QtGui.QPushButton(Dialog)
        self.pushButton_5.setGeometry(QtCore.QRect(390, 520, 98, 27))
        self.pushButton_5.setObjectName(_fromUtf8("pushButton_5"))
        self.pushButton_6 = QtGui.QPushButton(Dialog)
        self.pushButton_6.setGeometry(QtCore.QRect(280, 520, 98, 27))
        self.pushButton_6.setObjectName(_fromUtf8("pushButton_6"))
        self.lineEdit_2 = QtGui.QTextEdit(Dialog)
        self.lineEdit_2.setGeometry(QtCore.QRect(10, 340, 491, 31))
        self.lineEdit_2.setObjectName(_fromUtf8("lineEdit_2"))
        self.comboBox = QtGui.QComboBox(Dialog)
        self.comboBox.setGeometry(QtCore.QRect(10, 300, 491, 27))
        self.comboBox.setObjectName(_fromUtf8("comboBox"))
        self.comboBox.addItem(_fromUtf8(""))

        self.retranslateUi(Dialog)
        QtCore.QMetaObject.connectSlotsByName(Dialog)

    def retranslateUi(self, Dialog):
        Dialog.setWindowTitle(_translate("Dialog", "Run classifier", None))
        self.pushButton.setText(_translate("Dialog", "Train data...", None))
        self.pushButton_2.setText(_translate("Dialog", "Test data...", None))
        self.lineEdit.setText(_translate("Dialog", "../learned/", None))
        self.pushButton_3.setText(_translate("Dialog", "Output...", None))
        self.checkBox.setText(_translate("Dialog", "Save data columns", None))
        self.checkBox_2.setText(_translate("Dialog", "Combine results to one file", None))
        self.pushButton_5.setText(_translate("Dialog", "Run", None))
        self.pushButton_6.setText(_translate("Dialog", "Close", None))
        self.comboBox.setItemText(0, _translate("Dialog", "- Select classifier -", None))

# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'classifier_form.ui'
#
# Created: Tue Feb 10 19:11:22 2015
#      by: PyQt4 UI code generator 4.10.4
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    def _fromUtf8(s):
        return s

try:
    _encoding = QtGui.QApplication.UnicodeUTF8
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig, _encoding)
except AttributeError:
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig)

class Ui_Dialog(object):
    def setupUi(self, Dialog):
        Dialog.setObjectName(_fromUtf8("Dialog"))
        Dialog.resize(487, 579)
        self.gridLayout = QtGui.QGridLayout(Dialog)
        self.gridLayout.setObjectName(_fromUtf8("gridLayout"))
        self.verticalLayout = QtGui.QVBoxLayout()
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.horizontalGroupBox = QtGui.QGroupBox(Dialog)
        self.horizontalGroupBox.setObjectName(_fromUtf8("horizontalGroupBox"))
        self.horizontalLayout = QtGui.QHBoxLayout(self.horizontalGroupBox)
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.listWidget = QtGui.QListWidget(self.horizontalGroupBox)
        self.listWidget.setObjectName(_fromUtf8("listWidget"))
        self.horizontalLayout.addWidget(self.listWidget)
        self.pushButton = QtGui.QPushButton(self.horizontalGroupBox)
        self.pushButton.setMinimumSize(QtCore.QSize(100, 0))
        self.pushButton.setObjectName(_fromUtf8("pushButton"))
        self.horizontalLayout.addWidget(self.pushButton)
        self.verticalLayout.addWidget(self.horizontalGroupBox)
        self.horizontalGroupBox_2 = QtGui.QGroupBox(Dialog)
        self.horizontalGroupBox_2.setObjectName(_fromUtf8("horizontalGroupBox_2"))
        self.horizontalLayout_2 = QtGui.QHBoxLayout(self.horizontalGroupBox_2)
        self.horizontalLayout_2.setObjectName(_fromUtf8("horizontalLayout_2"))
        self.listWidget_2 = QtGui.QListWidget(self.horizontalGroupBox_2)
        self.listWidget_2.setObjectName(_fromUtf8("listWidget_2"))
        self.horizontalLayout_2.addWidget(self.listWidget_2)
        self.pushButton_2 = QtGui.QPushButton(self.horizontalGroupBox_2)
        self.pushButton_2.setMinimumSize(QtCore.QSize(100, 0))
        self.pushButton_2.setObjectName(_fromUtf8("pushButton_2"))
        self.horizontalLayout_2.addWidget(self.pushButton_2)
        self.verticalLayout.addWidget(self.horizontalGroupBox_2)
        self.horizontalGroupBox_3 = QtGui.QGroupBox(Dialog)
        self.horizontalGroupBox_3.setObjectName(_fromUtf8("horizontalGroupBox_3"))
        self.horizontalLayout_3 = QtGui.QHBoxLayout(self.horizontalGroupBox_3)
        self.horizontalLayout_3.setObjectName(_fromUtf8("horizontalLayout_3"))
        self.lineEdit = QtGui.QLineEdit(self.horizontalGroupBox_3)
        self.lineEdit.setObjectName(_fromUtf8("lineEdit"))
        self.horizontalLayout_3.addWidget(self.lineEdit)
        self.pushButton_3 = QtGui.QPushButton(self.horizontalGroupBox_3)
        self.pushButton_3.setMinimumSize(QtCore.QSize(100, 0))
        self.pushButton_3.setObjectName(_fromUtf8("pushButton_3"))
        self.horizontalLayout_3.addWidget(self.pushButton_3)
        self.verticalLayout.addWidget(self.horizontalGroupBox_3)
        self.horizontalGroupBox_4 = QtGui.QGroupBox(Dialog)
        self.horizontalGroupBox_4.setObjectName(_fromUtf8("horizontalGroupBox_4"))
        self.horizontalLayout_4 = QtGui.QHBoxLayout(self.horizontalGroupBox_4)
        self.horizontalLayout_4.setObjectName(_fromUtf8("horizontalLayout_4"))
        self.checkBox_2 = QtGui.QCheckBox(self.horizontalGroupBox_4)
        self.checkBox_2.setObjectName(_fromUtf8("checkBox_2"))
        self.horizontalLayout_4.addWidget(self.checkBox_2)
        self.checkBox = QtGui.QCheckBox(self.horizontalGroupBox_4)
        self.checkBox.setObjectName(_fromUtf8("checkBox"))
        self.horizontalLayout_4.addWidget(self.checkBox)
        self.verticalLayout.addWidget(self.horizontalGroupBox_4)
        self.verticalGroupBox_2 = QtGui.QGroupBox(Dialog)
        self.verticalGroupBox_2.setObjectName(_fromUtf8("verticalGroupBox_2"))
        self.verticalLayout_2 = QtGui.QVBoxLayout(self.verticalGroupBox_2)
        self.verticalLayout_2.setObjectName(_fromUtf8("verticalLayout_2"))
        self.comboBox = QtGui.QComboBox(self.verticalGroupBox_2)
        self.comboBox.setObjectName(_fromUtf8("comboBox"))
        self.comboBox.addItem(_fromUtf8(""))
        self.verticalLayout_2.addWidget(self.comboBox)
        self.textEdit = QtGui.QTextEdit(self.verticalGroupBox_2)
        self.textEdit.setReadOnly(False)
        self.textEdit.setObjectName(_fromUtf8("textEdit"))
        self.verticalLayout_2.addWidget(self.textEdit)
        self.lineEdit_2 = QtGui.QTextEdit(self.verticalGroupBox_2)
        self.lineEdit_2.setAutoFillBackground(False)
        self.lineEdit_2.setReadOnly(True)
        self.lineEdit_2.setObjectName(_fromUtf8("lineEdit_2"))
        self.verticalLayout_2.addWidget(self.lineEdit_2)
        self.horizontalLayout_6 = QtGui.QHBoxLayout()
        self.horizontalLayout_6.setObjectName(_fromUtf8("horizontalLayout_6"))
        spacerItem = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_6.addItem(spacerItem)
        self.pushButton_6 = QtGui.QPushButton(self.verticalGroupBox_2)
        self.pushButton_6.setObjectName(_fromUtf8("pushButton_6"))
        self.horizontalLayout_6.addWidget(self.pushButton_6)
        self.pushButton_5 = QtGui.QPushButton(self.verticalGroupBox_2)
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
        self.checkBox_2.setText(_translate("Dialog", "Combine results to one file", None))
        self.checkBox.setText(_translate("Dialog", "Save data columns", None))
        self.comboBox.setItemText(0, _translate("Dialog", "- Select classifier -", None))
        self.pushButton_6.setText(_translate("Dialog", "Close", None))
        self.pushButton_5.setText(_translate("Dialog", "Run", None))

# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'classifier_form.ui'
#
# Created: Tue Feb 10 19:13:22 2015
#      by: PyQt4 UI code generator 4.10.4
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    def _fromUtf8(s):
        return s

try:
    _encoding = QtGui.QApplication.UnicodeUTF8
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig, _encoding)
except AttributeError:
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig)

class Ui_Dialog(object):
    def setupUi(self, Dialog):
        Dialog.setObjectName(_fromUtf8("Dialog"))
        Dialog.resize(487, 659)
        self.gridLayout = QtGui.QGridLayout(Dialog)
        self.gridLayout.setObjectName(_fromUtf8("gridLayout"))
        self.verticalLayout = QtGui.QVBoxLayout()
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.horizontalGroupBox = QtGui.QGroupBox(Dialog)
        self.horizontalGroupBox.setObjectName(_fromUtf8("horizontalGroupBox"))
        self.horizontalLayout = QtGui.QHBoxLayout(self.horizontalGroupBox)
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.listWidget = QtGui.QListWidget(self.horizontalGroupBox)
        self.listWidget.setObjectName(_fromUtf8("listWidget"))
        self.horizontalLayout.addWidget(self.listWidget)
        self.pushButton = QtGui.QPushButton(self.horizontalGroupBox)
        self.pushButton.setMinimumSize(QtCore.QSize(100, 0))
        self.pushButton.setObjectName(_fromUtf8("pushButton"))
        self.horizontalLayout.addWidget(self.pushButton)
        self.verticalLayout.addWidget(self.horizontalGroupBox)
        self.horizontalGroupBox_2 = QtGui.QGroupBox(Dialog)
        self.horizontalGroupBox_2.setObjectName(_fromUtf8("horizontalGroupBox_2"))
        self.horizontalLayout_2 = QtGui.QHBoxLayout(self.horizontalGroupBox_2)
        self.horizontalLayout_2.setObjectName(_fromUtf8("horizontalLayout_2"))
        self.listWidget_2 = QtGui.QListWidget(self.horizontalGroupBox_2)
        self.listWidget_2.setObjectName(_fromUtf8("listWidget_2"))
        self.horizontalLayout_2.addWidget(self.listWidget_2)
        self.pushButton_2 = QtGui.QPushButton(self.horizontalGroupBox_2)
        self.pushButton_2.setMinimumSize(QtCore.QSize(100, 0))
        self.pushButton_2.setObjectName(_fromUtf8("pushButton_2"))
        self.horizontalLayout_2.addWidget(self.pushButton_2)
        self.verticalLayout.addWidget(self.horizontalGroupBox_2)
        self.horizontalGroupBox_3 = QtGui.QGroupBox(Dialog)
        self.horizontalGroupBox_3.setObjectName(_fromUtf8("horizontalGroupBox_3"))
        self.horizontalLayout_3 = QtGui.QHBoxLayout(self.horizontalGroupBox_3)
        self.horizontalLayout_3.setObjectName(_fromUtf8("horizontalLayout_3"))
        self.lineEdit = QtGui.QLineEdit(self.horizontalGroupBox_3)
        self.lineEdit.setObjectName(_fromUtf8("lineEdit"))
        self.horizontalLayout_3.addWidget(self.lineEdit)
        self.pushButton_3 = QtGui.QPushButton(self.horizontalGroupBox_3)
        self.pushButton_3.setMinimumSize(QtCore.QSize(100, 0))
        self.pushButton_3.setObjectName(_fromUtf8("pushButton_3"))
        self.horizontalLayout_3.addWidget(self.pushButton_3)
        self.verticalLayout.addWidget(self.horizontalGroupBox_3)
        self.horizontalGroupBox_4 = QtGui.QGroupBox(Dialog)
        self.horizontalGroupBox_4.setObjectName(_fromUtf8("horizontalGroupBox_4"))
        self.horizontalLayout_4 = QtGui.QHBoxLayout(self.horizontalGroupBox_4)
        self.horizontalLayout_4.setObjectName(_fromUtf8("horizontalLayout_4"))
        self.checkBox_2 = QtGui.QCheckBox(self.horizontalGroupBox_4)
        self.checkBox_2.setObjectName(_fromUtf8("checkBox_2"))
        self.horizontalLayout_4.addWidget(self.checkBox_2)
        self.checkBox = QtGui.QCheckBox(self.horizontalGroupBox_4)
        self.checkBox.setObjectName(_fromUtf8("checkBox"))
        self.horizontalLayout_4.addWidget(self.checkBox)
        self.verticalLayout.addWidget(self.horizontalGroupBox_4)
        self.verticalGroupBox_2 = QtGui.QGroupBox(Dialog)
        self.verticalGroupBox_2.setObjectName(_fromUtf8("verticalGroupBox_2"))
        self.verticalLayout_2 = QtGui.QVBoxLayout(self.verticalGroupBox_2)
        self.verticalLayout_2.setObjectName(_fromUtf8("verticalLayout_2"))
        self.comboBox = QtGui.QComboBox(self.verticalGroupBox_2)
        self.comboBox.setObjectName(_fromUtf8("comboBox"))
        self.comboBox.addItem(_fromUtf8(""))
        self.verticalLayout_2.addWidget(self.comboBox)
        self.lineEdit_2 = QtGui.QTextEdit(self.verticalGroupBox_2)
        self.lineEdit_2.setAutoFillBackground(False)
        self.lineEdit_2.setReadOnly(False)
        self.lineEdit_2.setObjectName(_fromUtf8("lineEdit_2"))
        self.verticalLayout_2.addWidget(self.lineEdit_2)
        self.textEdit = QtGui.QTextEdit(self.verticalGroupBox_2)
        self.textEdit.setReadOnly(True)
        self.textEdit.setObjectName(_fromUtf8("textEdit"))
        self.verticalLayout_2.addWidget(self.textEdit)
        self.horizontalLayout_6 = QtGui.QHBoxLayout()
        self.horizontalLayout_6.setObjectName(_fromUtf8("horizontalLayout_6"))
        spacerItem = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_6.addItem(spacerItem)
        self.pushButton_6 = QtGui.QPushButton(self.verticalGroupBox_2)
        self.pushButton_6.setObjectName(_fromUtf8("pushButton_6"))
        self.horizontalLayout_6.addWidget(self.pushButton_6)
        self.pushButton_5 = QtGui.QPushButton(self.verticalGroupBox_2)
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
        self.checkBox_2.setText(_translate("Dialog", "Combine results to one file", None))
        self.checkBox.setText(_translate("Dialog", "Save data columns", None))
        self.comboBox.setItemText(0, _translate("Dialog", "- Select classifier -", None))
        self.pushButton_6.setText(_translate("Dialog", "Close", None))
        self.pushButton_5.setText(_translate("Dialog", "Run", None))

# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'classifier_form.ui'
#
# Created: Tue Feb 10 19:26:09 2015
#      by: PyQt4 UI code generator 4.10.4
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    def _fromUtf8(s):
        return s

try:
    _encoding = QtGui.QApplication.UnicodeUTF8
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig, _encoding)
except AttributeError:
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig)

class Ui_Dialog(object):
    def setupUi(self, Dialog):
        Dialog.setObjectName(_fromUtf8("Dialog"))
        Dialog.resize(476, 659)
        self.gridLayout = QtGui.QGridLayout(Dialog)
        self.gridLayout.setObjectName(_fromUtf8("gridLayout"))
        self.verticalLayout = QtGui.QVBoxLayout()
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.horizontalGroupBox = QtGui.QGroupBox(Dialog)
        self.horizontalGroupBox.setObjectName(_fromUtf8("horizontalGroupBox"))
        self.horizontalLayout = QtGui.QHBoxLayout(self.horizontalGroupBox)
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.listWidget = QtGui.QListWidget(self.horizontalGroupBox)
        self.listWidget.setObjectName(_fromUtf8("listWidget"))
        self.horizontalLayout.addWidget(self.listWidget)
        self.pushButton = QtGui.QPushButton(self.horizontalGroupBox)
        self.pushButton.setMinimumSize(QtCore.QSize(100, 0))
        self.pushButton.setObjectName(_fromUtf8("pushButton"))
        self.horizontalLayout.addWidget(self.pushButton)
        self.verticalLayout.addWidget(self.horizontalGroupBox)
        self.horizontalGroupBox_2 = QtGui.QGroupBox(Dialog)
        self.horizontalGroupBox_2.setObjectName(_fromUtf8("horizontalGroupBox_2"))
        self.horizontalLayout_2 = QtGui.QHBoxLayout(self.horizontalGroupBox_2)
        self.horizontalLayout_2.setObjectName(_fromUtf8("horizontalLayout_2"))
        self.listWidget_2 = QtGui.QListWidget(self.horizontalGroupBox_2)
        self.listWidget_2.setObjectName(_fromUtf8("listWidget_2"))
        self.horizontalLayout_2.addWidget(self.listWidget_2)
        self.pushButton_2 = QtGui.QPushButton(self.horizontalGroupBox_2)
        self.pushButton_2.setMinimumSize(QtCore.QSize(100, 0))
        self.pushButton_2.setObjectName(_fromUtf8("pushButton_2"))
        self.horizontalLayout_2.addWidget(self.pushButton_2)
        self.verticalLayout.addWidget(self.horizontalGroupBox_2)
        self.horizontalGroupBox_3 = QtGui.QGroupBox(Dialog)
        self.horizontalGroupBox_3.setObjectName(_fromUtf8("horizontalGroupBox_3"))
        self.horizontalLayout_3 = QtGui.QHBoxLayout(self.horizontalGroupBox_3)
        self.horizontalLayout_3.setObjectName(_fromUtf8("horizontalLayout_3"))
        self.lineEdit = QtGui.QLineEdit(self.horizontalGroupBox_3)
        self.lineEdit.setObjectName(_fromUtf8("lineEdit"))
        self.horizontalLayout_3.addWidget(self.lineEdit)
        self.pushButton_3 = QtGui.QPushButton(self.horizontalGroupBox_3)
        self.pushButton_3.setMinimumSize(QtCore.QSize(100, 0))
        self.pushButton_3.setObjectName(_fromUtf8("pushButton_3"))
        self.horizontalLayout_3.addWidget(self.pushButton_3)
        self.verticalLayout.addWidget(self.horizontalGroupBox_3)
        self.horizontalGroupBox_4 = QtGui.QGroupBox(Dialog)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Minimum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.horizontalGroupBox_4.sizePolicy().hasHeightForWidth())
        self.horizontalGroupBox_4.setSizePolicy(sizePolicy)
        self.horizontalGroupBox_4.setMinimumSize(QtCore.QSize(0, 0))
        self.horizontalGroupBox_4.setAlignment(QtCore.Qt.AlignLeading|QtCore.Qt.AlignLeft|QtCore.Qt.AlignTop)
        self.horizontalGroupBox_4.setObjectName(_fromUtf8("horizontalGroupBox_4"))
        self.horizontalLayout_4 = QtGui.QHBoxLayout(self.horizontalGroupBox_4)
        self.horizontalLayout_4.setContentsMargins(-1, 0, -1, 0)
        self.horizontalLayout_4.setObjectName(_fromUtf8("horizontalLayout_4"))
        self.checkBox_2 = QtGui.QCheckBox(self.horizontalGroupBox_4)
        self.checkBox_2.setObjectName(_fromUtf8("checkBox_2"))
        self.horizontalLayout_4.addWidget(self.checkBox_2)
        self.checkBox = QtGui.QCheckBox(self.horizontalGroupBox_4)
        self.checkBox.setObjectName(_fromUtf8("checkBox"))
        self.horizontalLayout_4.addWidget(self.checkBox)
        self.verticalLayout.addWidget(self.horizontalGroupBox_4)
        self.verticalGroupBox_2 = QtGui.QGroupBox(Dialog)
        self.verticalGroupBox_2.setAlignment(QtCore.Qt.AlignLeading|QtCore.Qt.AlignLeft|QtCore.Qt.AlignTop)
        self.verticalGroupBox_2.setObjectName(_fromUtf8("verticalGroupBox_2"))
        self.verticalLayout_2 = QtGui.QVBoxLayout(self.verticalGroupBox_2)
        self.verticalLayout_2.setContentsMargins(-1, 0, -1, -1)
        self.verticalLayout_2.setObjectName(_fromUtf8("verticalLayout_2"))
        self.comboBox = QtGui.QComboBox(self.verticalGroupBox_2)
        self.comboBox.setObjectName(_fromUtf8("comboBox"))
        self.comboBox.addItem(_fromUtf8(""))
        self.verticalLayout_2.addWidget(self.comboBox)
        self.lineEdit_2 = QtGui.QTextEdit(self.verticalGroupBox_2)
        self.lineEdit_2.setAutoFillBackground(False)
        self.lineEdit_2.setReadOnly(False)
        self.lineEdit_2.setObjectName(_fromUtf8("lineEdit_2"))
        self.verticalLayout_2.addWidget(self.lineEdit_2)
        self.textEdit = QtGui.QTextEdit(self.verticalGroupBox_2)
        self.textEdit.setReadOnly(True)
        self.textEdit.setObjectName(_fromUtf8("textEdit"))
        self.verticalLayout_2.addWidget(self.textEdit)
        self.horizontalLayout_6 = QtGui.QHBoxLayout()
        self.horizontalLayout_6.setObjectName(_fromUtf8("horizontalLayout_6"))
        spacerItem = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_6.addItem(spacerItem)
        self.pushButton_6 = QtGui.QPushButton(self.verticalGroupBox_2)
        self.pushButton_6.setObjectName(_fromUtf8("pushButton_6"))
        self.horizontalLayout_6.addWidget(self.pushButton_6)
        self.pushButton_5 = QtGui.QPushButton(self.verticalGroupBox_2)
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
        self.checkBox_2.setText(_translate("Dialog", "Combine results to one file", None))
        self.checkBox.setText(_translate("Dialog", "Save data columns", None))
        self.comboBox.setItemText(0, _translate("Dialog", "- Select classifier -", None))
        self.pushButton_6.setText(_translate("Dialog", "Close", None))
        self.pushButton_5.setText(_translate("Dialog", "Run", None))

