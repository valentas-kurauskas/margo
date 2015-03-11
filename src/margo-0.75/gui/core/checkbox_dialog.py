from PyQt4 import QtCore
from PyQt4 import QtGui
import sys

class CheckBoxDialog(QtGui.QDialog):
    def __init__(self, title, names, unchecked=[], parent = None):
        super(CheckBoxDialog, self).__init__(parent)
        self.layout = QtGui.QVBoxLayout(self)
        self.view = QtGui.QListView()
        model = QtGui.QStandardItemModel()

        item = QtGui.QStandardItem("--Select all--")
        item.setCheckState(0)
        item.setCheckable(True)
        model.appendRow(item)
        self.sa = item

        self.names = names
        self.items = []
        for x in names:
            item = QtGui.QStandardItem(x)
            item.setCheckState(2 if not x in unchecked else 0)
            item.setCheckable(True)
            model.appendRow(item)
            self.items.append(item)
        model.itemChanged.connect(self.item_changed)
        self.view.setModel(model)
        self.layout.addWidget(self.view)
        # OK and Cancel buttons
        self.buttons = QtGui.QDialogButtonBox(
            QtGui.QDialogButtonBox.Ok | QtGui.QDialogButtonBox.Cancel,
            QtCore.Qt.Horizontal, self)
        self.layout.addWidget(self.buttons)
        self.setWindowTitle(title)
        self.buttons.accepted.connect(self.accept)
        self.buttons.rejected.connect(self.reject)

    def item_changed(self, item):
        if item == self.sa:
            for x in self.items:
                x.setCheckState(item.checkState())

    # get current date and time from the dialog
    def unchecked(self):
        return [self.names[i] for i in range(len(self.names)) if 0==self.items[i].checkState()]

    # static method to create the dialog and return (uncecked, accepted)
    @staticmethod
    def getUnchecked(title, names, unchecked=[], parent = None):
        dialog = CheckBoxDialog(title, names, unchecked, parent)
        result = dialog.exec_()
        return (dialog.unchecked(), result == QtGui.QDialog.Accepted)


class CheckBoxDialog2(QtGui.QDialog):
    def __init__(self, title, names1, names2, checked1=[], checked2=[], joindist=10, parent = None):
        super(CheckBoxDialog2, self).__init__(parent)
        self.layout = QtGui.QVBoxLayout(self)
        self.layout2 = QtGui.QHBoxLayout()
        self.layout3 = QtGui.QVBoxLayout()
        self.layout4 = QtGui.QVBoxLayout()
        self.layout.addLayout(self.layout2)
        self.layout2.addLayout(self.layout3)
        self.layout2.addLayout(self.layout4)

        self.layout5 = QtGui.QHBoxLayout()
        self.layout.addLayout(self.layout5)

        self.view = QtGui.QListView()
        model = QtGui.QStandardItemModel()


        self.names = names1
        self.items = []
        for x in names1:
            item = QtGui.QStandardItem(x)
            item.setCheckState(2 if x in checked1 else 0)
            item.setCheckable(True)
            model.appendRow(item)
            self.items.append(item)
        self.layout3.addWidget(QtGui.QLabel("Columns to join:", self))
        self.view.setModel(model)
        self.layout3.addWidget(self.view)

        self.view2 = QtGui.QListView()
        model2 = QtGui.QStandardItemModel()
        self.names2 = names2
        self.items2 = []
        for x in names2:
            item = QtGui.QStandardItem(x)
            item.setCheckState(2 if  x in checked2 else 0)
            item.setCheckable(True)
            model2.appendRow(item)
            self.items2.append(item)
        self.layout4.addWidget(QtGui.QLabel("Columns to add/rewrite:", self))
        self.view2.setModel(model2)
        self.layout4.addWidget(self.view2)

        self.layout5.addWidget(QtGui.QLabel("Maximum distance (meters): "))
        self.spinBox = QtGui.QSpinBox()
        self.spinBox.setMinimum(0)
        self.spinBox.setMaximum(10000)
        self.spinBox.setValue(joindist)
        self.layout5.addWidget(self.spinBox)

        # OK and Cancel buttons
        self.buttons = QtGui.QDialogButtonBox(
            QtGui.QDialogButtonBox.Ok | QtGui.QDialogButtonBox.Cancel,
            QtCore.Qt.Horizontal, self)
        self.layout.addWidget(self.buttons)
        self.setWindowTitle(title)
        self.buttons.accepted.connect(self.accept)
        self.buttons.rejected.connect(self.reject)

    # get current date and time from the dialog
    def checked(self):
        return ([self.names[i] for i in range(len(self.names)) if 2==self.items[i].checkState()], [self.names2[i] for i in range(len(self.names2)) if 2==self.items2[i].checkState()])

    # static method to create the dialog and return (uncecked, accepted)
    @staticmethod
    def getChecked(title, names1, names2, checked1=[], checked2=[],  parent = None):
        dialog = CheckBoxDialog2(title, names1, names2, checked1, checked2, parent)
        result = dialog.exec_()
        c = dialog.checked()
        return (c[0], c[1], dialog.spinBox.value(), result == QtGui.QDialog.Accepted)



#app = QtGui.QApplication(sys.argv)
#print CheckBoxDialog.getUnchecked("Hi", ["Salad", "Meat", "Fish"])
