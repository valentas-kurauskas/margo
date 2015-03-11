from PyQt4 import QtGui, Qt, QtCore
import config
#import subprocess
import os
#import StringIO
#import traceback

class ConvertSHPWindow(QtGui.QDialog):
    def __init__(self):
        super(ConvertSHPWindow, self).__init__() 
        self.initUI()
        #self.executing = False
        #self.output_buffer = StringIO.StringIO()
        
    def initUI(self):
        #self.ticker = QtCore.QTimer()
        #self.ticker.timeout.connect(self.update_status)
        #self.ticker.start(1000)
        self.setWindowFlags(self.windowFlags() | QtCore.Qt.WindowMinimizeButtonHint)

        self.inputEdit = QtGui.QLineEdit(config.get("convert_shp_last_hfz"))
        self.shpEdit = QtGui.QLineEdit(config.get("convert_shp_last_shp"))
        #self.locationEdit = QtGui.QLineEdit(config.get("margo_location"))
        #self.maskEdit = QtGui.QLineEdit(config.get("last_convert_shp_mask_dir"))
        self.maskEdit = QtGui.QLineEdit(config.get("convert_shp_last_dir"))

        inputButton = QtGui.QPushButton("Browse")
        shpButton = QtGui.QPushButton("Browse")
        #locationButton = QtGui.QPushButton("Browse")
        maskButton = QtGui.QPushButton("Browse")
        self.checkBox = QtGui.QCheckBox("Cut exact polygon")
        self.checkBox.setChecked(config.get("convert_shp_polygon") == 'True')
        #outputButton = QtGui.QPushButton("Browse")
        runButton = QtGui.QPushButton("Run")
        #killButton = QtGui.QPushButton("Stop")
        inputButton.clicked.connect(self.showInputDialog)
        shpButton.clicked.connect(self.showShpInputDialog)
        #locationButton.clicked.connect(self.showLocationDialog)
        maskButton.clicked.connect(self.showMaskDialog)
        #outputButton.clicked.connect(self.showOutputDialog)
        runButton.clicked.connect(self.run)
        #killButton.clicked.connect(self.kill_process)
        self.runButton = runButton
        #self.killButton = killButton

        layout = QtGui.QGridLayout()



        layout.addWidget(QtGui.QLabel('HFZ input dir:'), 0, 0)
        layout.addWidget(QtGui.QLabel('SHP input file:'), 1, 0)
        layout.addWidget(QtGui.QLabel('Output directory:'),2,0)
        #layout.addWidget(QtGui.QLabel('Output directory:'),3,0)
        #layout.addWidget(QtGui.QLabel('Other parameters:'),4,0)

        layout.addWidget(self.inputEdit,0,1)
        layout.addWidget(self.shpEdit,1,1)
        layout.addWidget(self.maskEdit,2,1)
        layout.addWidget(self.checkBox,3,1)


        #hsplit = QtGui.QSplitter()
        #hsplit.setOrientation(Qt.Qt.Vertical)
        
        #layout.addWidget(self.outputEdit,3,1)

        #hsplit.addWidget(self.paramsEdit) 
        self.outputbox = QtGui.QPlainTextEdit() #QtGui.QLabel()
        self.outputbox.setReadOnly(True)
        layout.addWidget(self.outputbox, 4, 1,2,2)

        layout.addWidget(inputButton,0,2)
        layout.addWidget(shpButton,1,2)
        layout.addWidget(maskButton,2,2)
        #layout.addWidget(outputButton,3,2)
        layout.addWidget(runButton,6,2)
        #layout.addWidget(killButton,6,0)
       
        self.setLayout(layout) 
        
        self.setGeometry(200, 200, 600, 400)
        self.setWindowTitle('Cut masks from SHP')   


    def update_output(self):
        s = self.process.readAllStandardOutput()
        self.outputbox.moveCursor(QtGui.QTextCursor.End)
        self.outputbox.insertPlainText(QtCore.QString(s))
        self.outputbox.moveCursor(QtGui.QTextCursor.End)
        print(s)

    def update_error(self):
        s = self.process.readAllStandardError()
        self.outputbox.moveCursor(QtGui.QTextCursor.End)
        self.outputbox.insertPlainText(QtCore.QString(s))
        self.outputbox.moveCursor(QtGui.QTextCursor.End)
        print(s)

    #    try:
    #        self.outputbox.setPlainText(self.output_buffer.getvalue())
    #    except:
    #        self.ticker.stop()
    #        print (traceback.format_exc())

    def showInputDialog(self):
        inp = str(self.inputEdit.text())
        #s = QtGui.QFileDialog.getOpenFileName(self, 'Open file', inp, "*.hfz")
        s = QtGui.QFileDialog.getExistingDirectory(self, 'Open directory', inp)
        if len(s) == 0: return
        #s = [str(x) for x in s]
        self.inputEdit.setText(s)

    def showShpInputDialog(self):
        inp = str(self.shpEdit.text()).split(";")
        inp = inp[0] if len(inp) > 0 else ""
        s = QtGui.QFileDialog.getOpenFileName(self, 'Open file', inp, "*.shp")
        if len(s) == 0: return
        #s = [str(x) for x in s]
        self.shpEdit.setText(s)

    def showMaskDialog(self):
        s = QtGui.QFileDialog.getExistingDirectory(self, 'Open directory', self.maskEdit.text())
        if s == "": return
        self.maskEdit.setText(s)

    def process_fail(self, error):
            #self.executing = False
            self.outputbox.moveCursor(QtGui.QTextCursor.End)
            self.outputbox.insertPlainText("ERROR:\n"+str(error))
            #self.runButton.setEnabled(True)
            #self.runButton.setEnabled(True)
            return

    def process_finished(self):
        #QtGui.QMessageBox.information(self, "Margo GUI", "Done.")
        self.runButton.setEnabled(True)
    #processing_finished = QtCore.pyqtSignal(QtCore.QString)

    def run(self):
        config.set("convert_shp_last_hfz", str(self.inputEdit.text()))
        config.set("convert_shp_last_shp", str(self.shpEdit.text()))
        config.set("convert_shp_last_dir", str(self.maskEdit.text()))
        config.set("convert_shp_polygon", str(self.checkBox.isChecked()))


        self.process = QtCore.QProcess(self)
        self.process.finished.connect(self.process_finished)
        self.process.error.connect(self.process_fail)
        #result = process.startDetached(self.current_command, [x] + self.current_args)
        self.runButton.setEnabled(False)

        self.outputbox.moveCursor(QtGui.QTextCursor.End)
        self.current_command = "../bin/convert_shp"
        self.current_args = [str(self.shpEdit.text()), str(self.maskEdit.text()), str(self.inputEdit.text())]
        if self.checkBox.isChecked():
            self.current_args += ["POLYGON"]

        self.outputbox.insertPlainText("STARTING\n"+ " ".join([self.current_command] + self.current_args) + "\n")
        self.outputbox.moveCursor(QtGui.QTextCursor.End)

        self.process.readyReadStandardOutput.connect(self.update_output)
        self.process.readyReadStandardError.connect(self.update_error)

        self.process.start(self.current_command, self.current_args)







