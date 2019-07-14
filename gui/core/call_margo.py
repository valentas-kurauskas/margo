from PyQt5 import QtGui, Qt, QtCore, QtWidgets
from . import config
#import subprocess
import os
#import StringIO
#import traceback

class MargoWindow(QtWidgets.QDialog):
    def __init__(self):
        super(MargoWindow, self).__init__() 
        self.initUI()
        self.executing = False
        #self.output_buffer = StringIO.StringIO()
        
    def initUI(self):
        #self.ticker = QtCore.QTimer()
        #self.ticker.timeout.connect(self.update_status)
        #self.ticker.start(1000)
        self.setWindowFlags(self.windowFlags() | QtCore.Qt.WindowMinimizeButtonHint)

        self.inputEdit = QtWidgets.QLineEdit(";".join(config.get_multiline("last_hfz_input")))
        self.locationEdit = QtWidgets.QLineEdit(config.get("margo_location"))
        self.maskEdit = QtWidgets.QLineEdit(config.get("margo_mask_dir"))
        self.outputEdit = QtWidgets.QLineEdit(config.get("margo_output_dir"))
        self.paramsEdit = QtWidgets.QTextEdit()
        self.paramsEdit.setText(config.get("margo_params"))

        inputButton = QtWidgets.QPushButton("Browse")
        locationButton = QtWidgets.QPushButton("Browse")
        maskButton = QtWidgets.QPushButton("Browse")
        outputButton = QtWidgets.QPushButton("Browse")
        runButton = QtWidgets.QPushButton("Run")
        killButton = QtWidgets.QPushButton("Stop")
        inputButton.clicked.connect(self.showInputDialog)
        locationButton.clicked.connect(self.showLocationDialog)
        maskButton.clicked.connect(self.showMaskDialog)
        outputButton.clicked.connect(self.showOutputDialog)
        runButton.clicked.connect(self.run)
        killButton.clicked.connect(self.kill_process)
        self.runButton = runButton
        self.killButton = killButton

        layout = QtWidgets.QGridLayout()



        layout.addWidget(QtWidgets.QLabel('Input files:'), 0, 0)
        layout.addWidget(QtWidgets.QLabel('Program location:'), 1, 0)
        layout.addWidget(QtWidgets.QLabel('Mask directory:'),2,0)
        layout.addWidget(QtWidgets.QLabel('Output directory:'),3,0)
        layout.addWidget(QtWidgets.QLabel('Other parameters:'),4,0)

        layout.addWidget(self.inputEdit,0,1)
        layout.addWidget(self.locationEdit,1,1)
        layout.addWidget(self.maskEdit,2,1)


        hsplit = QtWidgets.QSplitter()
        hsplit.setOrientation(Qt.Qt.Vertical)
        
        layout.addWidget(self.outputEdit,3,1)

        hsplit.addWidget(self.paramsEdit) 
        self.outputbox = QtWidgets.QPlainTextEdit() #QtWidgets.QLabel()
        self.outputbox.setReadOnly(True)
        hsplit.addWidget(self.outputbox)
        layout.addWidget(hsplit, 4, 1, 3, 2)
        layout.addWidget(inputButton,0,2)
        layout.addWidget(locationButton,1,2)
        layout.addWidget(maskButton,2,2)
        layout.addWidget(outputButton,3,2)
        layout.addWidget(runButton,5,0)
        layout.addWidget(killButton,6,0)
       
        self.setLayout(layout) 
        
        self.setGeometry(200, 200, 600, 500)
        self.setWindowTitle('Process data')   


    def update_output(self):
        s = self.process.readAllStandardOutput()
        self.outputbox.moveCursor(QtGui.QTextCursor.End)
        self.outputbox.insertPlainText(s.data().decode("utf-8"))
        self.outputbox.moveCursor(QtGui.QTextCursor.End)
        print(s)
        print("decode", s.data().decode("utf-8"))

    def update_error(self):
        s = self.process.readAllStandardError()
        self.outputbox.moveCursor(QtGui.QTextCursor.End)
        self.outputbox.insertPlainText(s.data().decode("utf-8"))
        print(s)

    #    try:
    #        self.outputbox.setPlainText(self.output_buffer.getvalue())
    #    except:
    #        self.ticker.stop()
    #        print (traceback.format_exc())

    def showInputDialog(self):
        inp = str(self.inputEdit.text()).split(";")
        inp = inp[0] if len(inp) > 0 else ""
        s,_ = QtWidgets.QFileDialog.getOpenFileNames(self, 'Open file', inp, "*.hfz")
        if len(s) == 0: return
        s = [str(x) for x in s]
        self.inputEdit.setText(";".join(s))

    def showLocationDialog(self):
        s,_ = QtWidgets.QFileDialog.getOpenFileName(self, 'Open file', self.locationEdit.text())
        if s == "": return
        self.locationEdit.setText(s)

    def showOutputDialog(self):
        s = QtWidgets.QFileDialog.getExistingDirectory(self, 'Open directory', self.outputEdit.text())
        if s == "": return
        self.outputEdit.setText(s)

    def showMaskDialog(self):
        s = QtWidgets.QFileDialog.getExistingDirectory(self, 'Open directory', self.maskEdit.text())
        if s == "": return
        self.maskEdit.setText(s)

    def process_fail(self, error):
            #self.executing = False
            self.outputbox.moveCursor(QtGui.QTextCursor.End)
            self.outputbox.insertPlainText("FAILED:\n"+str(error))
            self.runButton.setEnabled(True)
            return

    processing_finished = QtCore.pyqtSignal(str)

    def next_job(self):
        self.executing = True
        if len(self.to_do) == 0:
            self.outputbox.appendPlainText("DONE!\n")
            self.runButton.setEnabled(True)
            self.executing = False

            #res = str(self.inputEdit.text()).split(";")[-1]
            #res = res.rsplit(".", 1)[0] + "_selectionsDB.xyz" #should instead take from the margo output
            text = str(self.outputbox.toPlainText())
            idx = text.rfind("Saving ")
            #print idx
            if idx > 0 and idx > text.rfind("FAILED"):
                text = text[idx + 7:text.rfind(".xyz")+4]
                print(text)
                self.processing_finished.emit(text)
            return;
        self.runButton.setEnabled(False)
        x = self.to_do.pop()
     
        self.outputbox.moveCursor(QtGui.QTextCursor.End)
        self.outputbox.insertPlainText("STARTING\n"+ " ".join([self.current_command] + [x] + self.current_args) + "\n")
        self.outputbox.moveCursor(QtGui.QTextCursor.End)

        self.process = QtCore.QProcess(self)
        self.process.finished.connect(self.next_job)
        self.process.readyReadStandardOutput.connect(self.update_output)
        self.process.readyReadStandardError.connect(self.update_error)
        self.process.error.connect(self.process_fail)
        #print([repr(z) for z in [x] + self.current_args])
        self.process.start(self.current_command, [x] + self.current_args)


        #self.process = process
    
    def kill_process(self):
        if self.executing:
            self.to_do = []
            print("Terminating executing process")
            self.process.kill()



    def run(self):
        #self.output_buffer.close()
        #self.output_buffer = StringIO.StringIO()
        config.set_multiline("last_hfz_input", str(self.inputEdit.text()).split(";"))
        config.set("margo_location", str(self.locationEdit.text()))
        config.set("margo_params", self.paramsEdit.toPlainText())
        config.set("margo_mask_dir", str(self.maskEdit.text()))
        config.set("margo_output_dir", str(self.outputEdit.text()))

        inputs = str(self.inputEdit.text()).split(";")
        ppath = str(self.locationEdit.text())
        pars = config.get("margo_params").split("\n")
        pars = [x for x in pars if x != '']
        mask_dir = str(self.maskEdit.text())
        output_dir = str(self.outputEdit.text())

        #prog = os.path.basename(ppath)
        #pdir = os.path.dirname(ppath)

        self.current_command = ppath
        self.to_do = inputs
        self.current_args = ["-mask="+mask_dir, "-output_dir="+output_dir] + pars
        self.next_job()
            
            #print(args)
            #self.output_buffer.write(" ".join(args) + "\n")
            #print(subprocess.list2cmdline(args))
            #p = subprocess.Popen(args, stdout = subprocess.PIPE, stderr = subprocess.STDOUT)
            #self.current_process = subprocess.Popen(args, stdout = self.output_buffer, stderr = self.output_buffer)
            #for line in iter(p.stdout.readline, ''):
            #    self.outputbox.appendPlainText(line)

            #p.stdout.close()
    
