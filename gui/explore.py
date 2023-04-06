#!/usr/bin/python3
# -*- coding: utf-8 -*-

#(C) Valentas Kurauskas: 

import os
import sys
from PyQt5 import QtGui, Qt, QtCore, QtWidgets

from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt5agg import NavigationToolbar2QT as NavigationToolbar
import matplotlib.pyplot as plt
import numpy as np
from matplotlib import cm

from mpl_toolkits.mplot3d import Axes3D
#from PyQt5.QtWebKit import QWebView, QWebSettings

import os
#from classifier_form import ClassifierDialog
import core.classifier_form as classifier_form
import traceback,sys

from core import xyz, config
from core.call_margo import MargoWindow
from core.call_convert_shp import ConvertSHPWindow
from core.surface_browser import SurfaceBrowser
from core.lks_wgs import lks94_to_wgs
#from core.gmaps_browser import GMapsBrowser
from core.open_street_map_browser import OSMapsBrowser
from core import gdal_interface
from core.checkbox_dialog import CheckBoxDialog, CheckBoxDialog2
from core.classifier import data, exceeds, remove_nans
import csv
import random

#dummy packages to compile
import dummy




#approximate LKS to WGS obtained by taking values from 
#three points using online sources and solving system of 3 linear eqs on Wolfram Alpha
#this took me several hours
#however, the approximation is not enough for the size of Lithuania
#instead of the horrible formula in lks_wgs, we might similarly use a higher order approximation.
def lks94_to_wgs_approx(LONGITUDE, LATITUDE):
    return (3.2997504490130304E-8 * LONGITUDE + 8.974118525140475E-6 * LATITUDE + 0.278353,
            0.0000160334 * LONGITUDE - 1.4967795746354386E-8 * LATITUDE + 16.0763)




#TODO: a nicer way
#perhaps as here
#http://www.saltycrane.com/blog/2007/12/pyqt-43-qtableview-qabstracttablemodel/


class CoordDBModel(QtCore.QAbstractTableModel):
    hide_cols = ["RAW_COLS", "RAW_ROWS", "RESCALED_WIDTH", "RAW_HEIGHTS", "RESCALED"]

    def __init__(self, coordDB):
        super(CoordDBModel, self).__init__()
        self.coordDB = None
        self.set_data(coordDB)

    #warning: modifies the coordDB
    def set_data(self, coordDB):
        coordDB.column_names = [x for x in coordDB.column_names if not x in self.hide_cols] + [x for x in coordDB.column_names if x in self.hide_cols]
        if not "SCORE" in coordDB.column_names:
            coordDB.column_names.insert(2, "SCORE")
            coordDB.data["SCORE"] = [" "] * coordDB.size

        if not "COMMENT" in coordDB.column_names:
            coordDB.column_names.insert(3, "COMMENT")
            coordDB.data["COMMENT"] = [" "] * coordDB.size

        self.coordDB = coordDB
        self.nrows = coordDB.size
        self.ncols = len(coordDB.column_names)

    def rowCount(self, parent = QtCore.QModelIndex()):
        return self.nrows

    def columnCount(self, parent = QtCore.QModelIndex()):
        return self.ncols

    def data(self, index, role = QtCore.Qt.DisplayRole):
        row = index.row()
        col = index.column()
        if role == QtCore.Qt.DisplayRole:
            item = self.coordDB.get_item(row, col)
            if self.coordDB.column_names[col] in xyz.NUMERIC_FEATURES:
                return QtCore.QVariant(item)
            elif item == None:
                return QtCore.QVariant()
            else:
                return (str(item))
        return QtCore.QVariant()

    def headerData(self, section, orientation, role):
        if role == QtCore.Qt.DisplayRole:
            if orientation == QtCore.Qt.Horizontal:
                return (self.coordDB.column_names[section])
        return QtCore.QVariant()

    def setData(self, index, value, role):
        if (role == QtCore.Qt.EditRole):
            if self.coordDB.column_names[index.column()] in ["SCORE", "COMMENT"]:
                self.coordDB.set_item(index.row(), index.column(), str(value.toString()))
                #self.editCompleted.emit(QtCore.QVariant())
            else: 
                QtWidgets.QMessageBox.information(self, "Read only", "This should not happen.", "OK")
                return False
            return True

    def flags(self, index):
            if self.coordDB.column_names[index.column()] in ["SCORE", "COMMENT"]:

                return QtCore.Qt.ItemIsSelectable | QtCore.Qt.ItemIsEditable | QtCore.Qt.ItemIsEnabled
            else:
                return QtCore.Qt.ItemIsSelectable | QtCore.Qt.ItemIsEnabled

class MyQSortFilterProxyModel(QtCore.QSortFilterProxyModel):
    def __init__(self, *args):
        super(MyQSortFilterProxyModel,self).__init__(*args)
        self.hide_row_indices = []

    def set_rows_to_hide(self, rows):
        #print ("Will hide rows: ", rows)
        self.hide_row_indices = rows
        self.invalidateFilter()

    def filterAcceptsRow(self, row, parent):
        return not row in self.hide_row_indices

class CoordDBView(QtWidgets.QTableView):
    def __init__(self): 
        #QtGui.QTableWidget.__init__(self, *args)
        super(CoordDBView, self).__init__()
        self.setSelectionBehavior(QtWidgets.QAbstractItemView.SelectRows)

    def sort_items(self, index):
        self.sort_order = (self.sort_order + 1) % 2
        p = [QtCore.Qt.AscendingOrder, QtCore.Qt.DescendingOrder][self.sort_order]
        print((index, p))
        self.sortItems(index, p)

    def current_row(self):
        #self.selectionModel().currentIndex().row()
        return self.model().mapToSource(self.selectionModel().currentIndex()).row()  #todo put the proxy inside this class.

    def selected_rows(self):
        m = self.model()
        if m is None: return []
        #print ("indexes:", self.selectionModel().selection().indexes())
        #return [m.mapToSource(r).row() for r in self.selectionModel().selection().indexes()]
        return [m.mapToSource(r).row() for r in self.selectionModel().selectedRows()]


    def select_rows(self, rows):
        source_indices = [self.model().sourceModel().index(rr, self.selectionModel().currentIndex().column(), QtCore.QModelIndex()) for rr in rows]
        for x in source_indices:
            if not x.isValid():
                print ("invalid selection")
                return
        modifiers = QtWidgets.QApplication.keyboardModifiers()

        if modifiers == QtCore.Qt.ControlModifier:
            flags =  QtCore.QItemSelectionModel.Rows |QtCore.QItemSelectionModel.Toggle
        else:
            flags  = QtCore.QItemSelectionModel.Rows | QtCore.QItemSelectionModel.ClearAndSelect
        r = [self.model().mapFromSource(x) for x in source_indices]
        s = QtCore.QItemSelection()
        for rr in r:
            s.merge(QtCore.QItemSelection(rr,rr), QtCore.QItemSelectionModel.Rows | QtCore.QItemSelectionModel.Select)
        #L = QtGui.QList(QtCore.QModelIndex)
        #for x in source_indices:
        #    L.push_back(self.model().mapFromSource(x))
        self.selectionModel().select(s, flags) #ClearAndSelect)




    def save_xyz(self, fname):
        h = self.model().hide_row_indices
        hc = config.get_multiline("hidden_columns")
        r = 0
        if len(h) > 0 or len(hc) > 0:
         r = QtWidgets.QMessageBox.question(self, "Margo GUI", "A row or column filter is active. Do you want to save all entries", "Save all", "Only filtered")
        if (r != 0):
            self.db().save_xyz(fname, h, hc)
        else:
            self.db().save_xyz(fname)

    def save_shp(self, fname):
        try:
            proj = gdal_interface.load_proj(config.get("projection_filename"))
        except IOError:
            QtWidgets.QMessageBox.information(self, "Margo GUI", "Please load a valid projection file.")
            return
        h = self.model().hide_row_indices
        self.db().save_shp(fname, proj, h, config.get_multiline("hidden_columns"))

    def export_to_csv(self, fname):
        h = self.model().hide_row_indices
        self.db().save_csv(fname, h, config.get_multiline("hidden_columns"))


    def db(self):
        if self.model() is None:
            return None
        return self.model().sourceModel().coordDB
    
    rcChange = QtCore.pyqtSignal()

    def rowCountChanged(self, new, old):
        QtWidgets.QTableView.rowCountChanged(self, new, old)
        self.rcChange.emit()

    def set_hidden_columns(self, hidden):
       # print "hiding columns"
        for i in range(self.model().columnCount()):
            name = self.model().headerData(i, QtCore.Qt.Horizontal, QtCore.Qt.DisplayRole)
            #print name
            self.setColumnHidden(i, name in hidden)

class FilterObject(QtCore.QObject):
    def __init__(self, windowObj):
        QtCore.QObject.__init__(self)
        self.windowObj = windowObj

    def eventFilter(self, obj, event):
        if (event.type() == QtCore.QEvent.KeyPress):
            key = event.key()
            if (event.modifiers() == Qt.ControlModifier):
                if(key == Qt.Key_S):
                    print('standard response')

class MainWindowContents(QtWidgets.QWidget):
    
    def __init__(self):
        super(MainWindowContents, self).__init__()
        
        self.initUI()
        self.show_only_filtered = False
        try:
            self.max_points_gmap = int(config.get("max_points_gmap"))
            print(("max_points_gmap", self.max_points_gmap))
        except:
            self.max_points_gmap = 2000

    @QtCore.pyqtSlot(str, result=int)
    def map_clicked(self, value):
        #print(value)
        #self.table.setCurrentCell(self.table.coordDB.find_name(value), 1)
        #self.table.setCurrentCell(self.table.find_row(int(value.split(":")[0])), self.table.currentColumn())
        
        row = int(value.split(":")[0])
        self.select_record(row, "gmap")
        return 0

    def select_record(self, ID, caller):
        #print ("select_record", ID, caller) #TODO!!! ID should be row index!
        self.last_update_trigger = caller
        if ID in self.table.model().hide_row_indices: #for some reason trying to index outside screws up everything
            return 0
        source_index = self.table.model().sourceModel().index(ID, self.table.selectionModel().currentIndex().column(), QtCore.QModelIndex())
        #print (source_index.column(), source_index.row(), source_index.isValid())
        if (not source_index.isValid()):
            return 0
        #self.table.setCurrentCell(int(value.split(":")[0]), self.table.currentColumn())
        new_index = self.table.model().mapFromSource(source_index)
        #print (new_index.column(), new_index.row())
        
        #self.table.selectionModel().select(new_index, QtWidgets.QItemSelectionModel.Toggle | QtWidgets.QItemSelectionModel.Rows)

        modifiers = QtWidgets.QApplication.keyboardModifiers()
        flags  = QtCore.QItemSelectionModel.Rows |QtCore.QItemSelectionModel.ClearAndSelect
        if modifiers == QtCore.Qt.ControlModifier:
                flags =  QtCore.QItemSelectionModel.Rows |QtCore.QItemSelectionModel.Toggle
        self.table.selectionModel().setCurrentIndex(new_index, flags) #ClearAndSelect)

        #self.table.selectRow(new_index.row())
        #self.jiggggg();
        return 0

    def insert_missing_point(self, x,y):
        if len(self.table.model().hide_row_indices) != 0:
            QtWidgets.QMessageBox.information(self, "Margo GUI", "Please clear the row filter first.")
            return
        else:
            #self.table.model().setDynamicSortFilter(True)
            n = self.table.db().size
            self.table.model().sourceModel().beginInsertRows(QtCore.QModelIndex(), n, n)
            print(n)
            self.table.db().insert_row({"LATITUDE": y, "LONGITUDE": x, "COMMENT": "MISSED"})
            print(self.table.db().size)
            print("data set")
            #print "index created"
            
            #index = self.table.model().sourceModel().createIndex(n-1, 0)
            
            #http://qt-project.org/doc/qt-4.8/qabstractitemmodel.html#beginInsertRows

            #self.table.model().sourceModel().insertRows(n, 1)
            self.table.model().sourceModel().set_data(self.table.db())
            self.table.model().sourceModel().endInsertRows()
            #self.table.model().sourceModel().rowsInserted.emit(index, 0, 0)
            
            self.select_record(n, "raster")
            self.table.update()
            self.update_all()

    def initUI(self):
        self.table = CoordDBView()

        # a figure instance to plot on
        self.figure = plt.figure(1)

        # this is the Canvas Widget that displays the `figure`
        # it takes the `figure` instance as a parameter to __init__
        self.canvas = FigureCanvas(self.figure)

        self.figure3d = plt.figure(2)
        self.canvas3d = FigureCanvas(self.figure3d)
        
        #self.map_figure = plt.figure(3)
        #self.map_canvas = FigureCanvas(self.map_figure)

        self.infobox = QtWidgets.QPlainTextEdit() #QtGui.QLabel()
        self.infobox.setReadOnly(True)
 
        self.the_map = OSMapsBrowser()
        #!tmp 
        ##2019## self.the_map.page().mainFrame().addToJavaScriptWindowObject("outside", self)

        self.last_mapped = None #last mapped coordinate
        #self.the_map.linkClicked.connect(self.map_click)

        self.last_update_trigger = None
        self.raster_map = SurfaceBrowser(self.table)

        self.filter_box = QtWidgets.QTextEdit()
        self.figure = plt.figure()
        self.canvas = FigureCanvas(self.figure)
        
        self.filter_button = QtWidgets.QPushButton("Filter")
        self.unfilter_button = QtWidgets.QPushButton("Clear filter")
        self.selected_filter_button = QtWidgets.QPushButton("Selection")
        hgroup = QtWidgets.QGroupBox()
        hlayout = QtWidgets.QHBoxLayout()
        hlayout.setAlignment(QtCore.Qt.AlignRight)
        hlayout.addWidget(self.filter_button)
        hlayout.addWidget(self.selected_filter_button)
        hlayout.addWidget(self.unfilter_button)
        self.filter_box.setText("\n".join(config.get_multiline("filter")))
        self.filter_button.clicked.connect(self.set_filter)
        self.selected_filter_button.clicked.connect(self.filter_selected)
        self.unfilter_button.clicked.connect(self.unfilter)

        
        filter_layout = QtWidgets.QVBoxLayout()
        
        hgroup.setLayout(hlayout)
        filter_layout.addSpacing(30) 
        filter_layout.addWidget(self.filter_box)
        filter_layout.addWidget(hgroup)
        self.filter_tab = QtWidgets.QGroupBox()
        self.filter_tab.setLayout(filter_layout)
       
        #self.data_tabs = QtGui.QTabWidget()



        #self.data_tabs.addTab(self.infobox, "Info")
        #self.data_tabs.addTab(self.canvas, "Elliptic Profile")
        #self.data_tabs.addTab(self.canvas3d, "3D")
        #self.data_tabs.addTab(self.the_map, "Google Maps")
        #self.data_tabs.addTab(self.raster_map, "Raster")
        #self.data_tabs.addTab(self.filter_tab, "Filter")

        #self.data_tabs.addTab(self.map_canvas, "Point map") TODO: if we want to make interactive, etc, there will be some work 

        self.canvasMenu = QtWidgets.QMenu(self)
        ca = QtWidgets.QAction('Normalize by height', self)
        ca.triggered.connect(self.update_profile)
        ca.setCheckable(True)
        self.canvasMenu.addAction(ca)
        self.normalize_profile_action = ca

        ca = QtWidgets.QAction('Normalize by ditch distance', self)
        ca.triggered.connect(self.update_profile)
        ca.setCheckable(True)
        self.canvasMenu.addAction(ca)
        self.hnormalize_profile_action = ca

        ca = QtWidgets.QAction('Align top', self)
        ca.triggered.connect(self.update_profile)
        ca.setCheckable(True)
        self.canvasMenu.addAction(ca)
        self.align_top_action = ca

        self.canvas.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
        self.canvas.customContextMenuRequested.connect(self.exec_canvas_menu)

        self.canvas3dMenu = QtWidgets.QMenu(self)
        ca = QtWidgets.QAction('Draw RESCALED', self)
        ca.triggered.connect(self.update_3d)
        #ca.triggered.connect(lambda x: self.figure3d.gca(projection='3d').relim())
        ca.setCheckable(True)
        self.canvas3dMenu.addAction(ca)
        self.draw3d_rescaled_action = ca
 
        self.canvas3d.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
        self.canvas3d.customContextMenuRequested.connect(self.exec_canvas3d_menu)

        self.gmaps_show_column = config.get("gmaps_show_column")
        #self.data_tabs.currentChanged.connect(self.update_map2)
        #self.data_tabs.currentChanged.connect(self.update_profile)
        #self.data_tabs.currentChanged.connect(self.update_raster)
        #self.table.itemSelectionChanged.connect(self.update_map2)
        #grid = QtGui.QGridLayout()
        #grid.setSpacing(10)
        #grid.addWidget(self.table, 0, 1, 3, 1)
        #grid.addWidget(self.canvas, 3, 1, 5, 1)
        #self.setLayout(grid)

        layout = QtWidgets.QVBoxLayout(self)
        #hsplit = QtGui.QSplitter()
        #hsplit.setOrientation(Qt.Qt.Vertical)
        #hsplit.addWidget(self.table)
        #hsplit.addWidget(self.data_tabs)
        #layout.addWidget(hsplit)
        layout.addWidget(self.table)
        self.setLayout(layout)




#    def normalize(self):
#        #self.normalize_profile = not self.normalize_profile
#        update_profile()


    def exec_canvas_menu(self, point):
        self.canvasMenu.exec_(self.canvas.mapToGlobal(point))

    def exec_canvas3d_menu(self, point):
        self.canvas3dMenu.exec_(self.canvas3d.mapToGlobal(point))
    #def map_click(self, info): #not used
    #    print ("Link clicked", info)
    
    def update_3d(self):
        if (not self.canvas3d.corresponding_dock.isVisible()):
            return;
        ax3 = self.figure3d.add_subplot(projection='3d')
        #ax3.cla()
        #ax3.grid(b=False)
        try:
            self.surface3dplot.remove()
        except: pass

        if self.table.model() is None: return
        if self.draw3d_rescaled_action.isChecked():
            ncol = "RESCALED_WIDTH"
            mcol = "RESCALED_WIDTH"
            hcol = "RESCALED"
        else:
            ncol = "RAW_COLS"
            mcol = "RAW_ROWS"
            hcol = "RAW_HEIGHTS"

        if (self.table.current_row() is None):
            return

        n = self.table.db().get_item(self.table.current_row(), ncol)
        m = self.table.db().get_item(self.table.current_row(), mcol)
        try:
            n = int(n)
            m = int(m)
        except (TypeError):
            return
        if n <= 0 or m <=0: return

        z = self.table.db().get_item(self.table.current_row(), hcol)
        
        if z == None:
            self.canvas3d.draw()
            return

        x = []
        y = []

        mz = np.nanmin(z)
        z = [zz - mz for zz in z]
        for i in range(m):
            for j in range(n):
                x.append(j)
                y.append(i)
        x = np.matrix(x).reshape(m, n)
        y = np.matrix(y).reshape(m, n)
        z = np.matrix(z).reshape(m, n)

        #ax3 = Axes3d(self.figure3d)
        #ax3 = self.figure3d.add_subplot(111, projection='3d')

        ###2019### ax3.hold(False)
        self.surface3dplot = ax3.plot_surface(x,y,z, rstride=1, cstride=1, cmap=cm.jet, vmin = np.nanmin(z), vmax=np.nanmax(z))
        ax3.set_xlabel("width, px")
        ax3.set_ylabel("length, px")
        ax3.set_zlabel("height, m")
        
        ax3.relim() #does not work... todo: fix

        self.canvas3d.draw()


    def update_map2(self):
        #print (self.data_tabs.currentWidget())
        #print("update map2")
        #if (self.data_tabs.currentWidget() != self.the_map):  #do not always load the map
        if (not self.the_map.corresponding_dock.isVisible()):
            return;
        if (self.table.model() is None): return
        cr = self.table.current_row()
        if (cr == None):
            return;
        #print(cr)
        x = int(self.table.db().get_item(cr, "LONGITUDE"))
        y = int(self.table.db().get_item(cr, "LATITUDE"))
        #print("lm", self.last_mapped, x, y)
        if ((x,y) == self.last_mapped):
            return
        if (self.last_mapped == None):
            #self.the_map.page().mainFrame().evaluateJavaScript("map.setZoom(16)")
            print("!setZoom disabled!")
            #self.the_map.page().runJavaScript("map.setZoom(16)")
        
        self.last_mapped = (x,y)
       
        wgs_x,wgs_y = lks94_to_wgs(x,y)


        s = "maexplore_map.panTo( new L.LatLng("+str(wgs_x)[:7] + ", " + str(wgs_y)[:7] + "));"
        #print(s)
        self.the_map.page().runJavaScript(s);
        #self.the_map.mark_selected(self.table.db().get_item(cr, "ID"))
        self.the_map.mark_selected(cr)

        #print(x,y)
        #self.map_show_ids(self.table.coordDB.closest(x, y, 30, 100), cr,zoom=17) #plot at most 30 closest points, with distance at most 100m from current


    def map_show_ids(self, ids, cr = None, zoom = None): 
        print("show_ids")
        if cr == None:
            cr = self.table.current_row()


        if zoom == None:
            ##S = self.the_map.page().runJavaScript("map.getZoom()").toInt();
            ##if S[1]:
            ##    zoom = S[0]
            ##else:
                print('!getting map.getZoom() turned off!')
                zoom = 13

        
        x = int(self.table.db().get_item(cr, "LONGITUDE"))
        y = int(self.table.db().get_item(cr, "LATITUDE"))

        wgs_x,wgs_y = lks94_to_wgs(x,y)

        #selected = self.table.selected_rows()

        lats = []
        lons = []
        names = []
        types = []
        IDs = []

        if len(ids) > self.max_points_gmap:
            r = QtWidgets.QMessageBox.question(self, "Margo GUI", "The number of points ("+str(len(ids)) +") is large. Show only a random selection of "+ str(self.max_points_gmap)+"? Later you can zoom in to a smaller area and use 'Update map'.", "Truncate", "Show all points")
            if (r == 0):
                ids = list(sorted(random.sample(ids, self.max_points_gmap))) #ids[:self.max_points_gmap]

        for r in ids:
            x = self.table.db().get_item(r, "LONGITUDE")
            y = self.table.db().get_item(r, "LATITUDE")
            name = str(r) + ":"
            show_col = self.gmaps_show_column 
            if (show_col in self.table.db().column_names):
                name += str(self.table.db().get_item(r, show_col))
            x,y = lks94_to_wgs(x,y)
            #IDs.append(self.table.db().get_item(r, "ID"))
            IDs.append(r)
            lats.append(x)
            lons.append(y)
            #types.append(int(r in selected))
            names.append(name)
        self.the_map.show_points(IDs, lats, lons, names, wgs_x, wgs_y, zoom)
        self.update_map_selection()
        #!tmp
        print("old line skip")
        ##2019##self.the_map.page().mainFrame().addToJavaScriptWindowObject("outside", self)

    def show_map(self, map_all = False): 
        if self.table.model() is None:
                return
        if self.table.db().size == 0:
            return
        selected = self.table.selected_rows()
        #if len(selected) <= 1:
        if True:
            bounds = None
            try:
                q = "map.getBounds().toString()"
                S = self.the_map.page().runJavaScript(q);
                print(("bounds", S.typeName(), str(S.toString())))
                #[(x, y.toMap()) for x,y in S.toMap().iteritems()]
                bounds = eval(str(S.toString()))
                print (bounds)
            except:
                print ("Could not autodetect bounds")

            if (map_all or bounds == None):
                rows = range(self.table.db().size)
            else:
                rows = []
                db = self.table.db()
                rows = db.filter_rectangle(bounds[0][0], bounds[1][0], bounds[0][1], bounds[1][1], convert_to_wgs=True)
                x = db.get_projection("LONGITUDE", rows)
                y = db.get_projection("LATITUDE", rows) 
                #for r in range(self.table.db().size):
                #    x = self.table.db().get_item(r, "LONGITUDE")
                #    y = self.table.db().get_item(r, "LATITUDE")
                #    x,y = lks94_to_wgs(x,y)
                #    #print (x,bounds[0][0], bounds[1][0])
                #    #print (y,bounds[1][0], bounds[1][1])
                #    if (x > bounds[0][0]) and (x < bounds[1][0]) and (y > bounds[0][1]) and (y < bounds[1][1]):
                #        rows.append(r)
                print(("Showing "+str(len(rows))+" rows in the bounds"))
        else:
            rows = selected

        if (self.show_only_filtered):
            rows = [x for x in rows if not x in self.table.model().hide_row_indices]
        #if len(selected) > 500:
        #    selected = selected[:500]; #todo: show message box
        #    QMessageBox.about(self, "My message box", "Showing only the first 500 points. You can make a manual selection and click View->Show Map.")
        middle = None
        if (len(rows) > 0):
            middle = rows[len(rows)//2];
        cr = self.table.current_row()
        #if ccr == -1:
        #    return
        if cr in rows:
            middle = cr
        #FIX! self.the_map.page().mainFrame().addToJavaScriptWindowObject("outside", self)
        self.map_show_ids(rows, middle)
        #self.the_map.corresponding_dock.raise_()
        #data_tabs.setCurrentWidget(self.the_map)
        self.last_mapped = None
    
    def update_map_selection(self):
        cr = self.table.current_row()
        #ids = [self.table.db().get_item(r, "ID") for r in self.table.selected_rows() if not r == cr]
        ids = [r for r in self.table.selected_rows() if not r == cr]
        #print ("update selection", cr, ids)
        #self.the_map.mark_selected(self.table.db().get_item(cr, "ID"))
        self.the_map.mark_selected(cr)
        self.the_map.update_selection(ids) #now plotting selection at once is redundant



    def update_infobox(self):
        s = ""
        for name in self.table.db().column_names:
            y = self.table.db().get_item(self.table.current_row(), name)
            s += name + " = " + str(y) + "\n"
        self.infobox.setPlainText(s)
        #print(s)


    def update_profile(self):
        #if (self.data_tabs.currentWidget() != self.canvas):  #do not always draw the profile
        if (not self.canvas.corresponding_dock.isVisible()):
            return;
        ax = self.figure.gca()
        ax.clear()
        #ax.hold(False)
        #print ("upd")
        #print (self.table.selected_rows())
        for r in self.table.selected_rows():
            #print ("selected row: ", r)
            y = self.table.db().get_item(r, "ELLIPSE_PROFILE")
            if y == None or len(y) == 0:
                continue
            hfactor = 1.0

            mn = min(y)
            ax.set_ylabel("Height, m")
            ax.set_xlabel("Distance from centre, m", labelpad = -3)
            if self.hnormalize_profile_action.isChecked():
                i = 2
                while (i+1 < len(y)) and (y[i+1]-y[i] < 0):
                    i+=1
                if i+1 >= len(y):
                    continue #don't draw
                mn = y[i] #normalize by height measured from ditch bottom
                hfactor = 1.0/i
                ax.set_xlabel("Normalized distance from centre, 1 = ditch", labelpad=-3)
            if self.normalize_profile_action.isChecked():
                y = [yy - mn for yy in y]
                mx = max(y)
                y = [yy * 1.0 / mx for yy in y]
                ax.set_ylabel("Normalized height")

            if self.align_top_action.isChecked():
                t=max(y)
                y = [yy - t for yy in y]
                ax.set_ylabel(ax.get_ylabel() + " (top aligned)")

            l = len(y)
            y = (y[1:])[::-1] + y
            ax.plot([ll * hfactor for ll in range(-l+1, l)], y, "o-")
            ax.grid(True)

        self.canvas.draw()

    def update_raster(self):
        if self.last_update_trigger == "raster": return #raster will replot itself
        #if (self.data_tabs.currentWidget() != self.raster_map):
        if (not self.raster_map.corresponding_dock.isVisible()):
            return
        if self.raster_map.raster == None:
            print ("Raster file not loaded")
            return
        #print ("Updating raster, because it is visible")
        cr = self.table.current_row()
        #print ("cr", cr)
        x = self.table.db().get_item(cr, "LONGITUDE")
        y = self.table.db().get_item(cr, "LATITUDE")
        sel = self.table.selected_rows()
        #self.raster_map.plot_area( (x-100, y+100), (x+100, y-100), self.table.db())
        hidden  = self.table.model().hide_row_indices if self.show_only_filtered else []
        if cr is None or cr in self.raster_map.last_plotted_rows:    
            self.raster_map.replot(only_points=True, autozoom = True, hidden=hidden)
            #if cr is not None:
            #    self.raster_map.pan_to((x,y))
            #print(">>replot from update raster")
        else:
            #print(">>plot point from update raster")
            self.raster_map.plot_point( x, y,  hidden, sel, [cr]) 

    def update_all(self):
        self.update_infobox()
        #self.update_profile()
        self.update_map2()
        self.update_raster()
        self.update_3d()
        self.last_update_trigger = None
        #print ("-")


    def set_filter(self):
        if self.table.model() is None:
            return
        text = str(self.filter_box.toPlainText());
        config.set_multiline("filter", text.split("\n"))
        selected_indices = self.table.selected_rows()
        try:
            f = eval(text)
            db = self.table.db()
            to_hide = []
            for i in range (db.size):
                x = db.get_row_as_dict(i)
                x['_SELECTED'] = i in selected_indices
                if not f(x):
                        to_hide.append(i)
            self.table.model().set_rows_to_hide(to_hide)
            #self.filter_box.setTextColor(QtGui.QApplication.palette().text().color())
            self.filter_box.setStyleSheet("QTextEdit {}")
            self.show_map(map_all = True)
            print ("Filter OK")
        except:
            traceback.print_exc(file=sys.stdout)
            self.filter_box.setStyleSheet("QTextEdit { color : red; }")
            self.filter_box.update()
            self.table.model().set_rows_to_hide([])
        self.table.update()
        self.update_all()

    def filter_selected(self):
        self.filter_box.setText("lambda x: x['_SELECTED']")
        self.set_filter()

    def unfilter(self):
        self.filter_box.setText("lambda x: True")
        self.set_filter()
        #self.filter_box.setStyleSheet("QTextEdit {}")
        #config.set_multiline("filter", str(self.filter_box.toPlainText()).split("\n"))
        #self.table.model().set_rows_to_hide([])
        #self.show_map(map_all = True)
        #self.update_all()

    def select_visible_columns(self):
        unchecked = config.get_multiline("hidden_columns")
        #print unchecked
        cols = self.table.db().column_names
        unchecked,ok = CheckBoxDialog.getUnchecked("Show/hide columns", cols, unchecked, self)
        if not ok: return
        self.table.set_hidden_columns(unchecked)
        config.set_multiline("hidden_columns",unchecked)




class MainWindow(QtWidgets.QMainWindow):
    def __init__(self):
        super(MainWindow, self).__init__() 
        self.initUI()
    
    def createDockWidgets(self, widgets, titles):
        self.docks = []
        self.toolsMenu = QtWidgets.QMenu("&Tools")
        for i in range(len(widgets)):
            d = QtWidgets.QDockWidget(titles[i], self)
            d.setWidget(widgets[i])
            d.setObjectName(titles[i])
            self.addDockWidget(QtCore.Qt.BottomDockWidgetArea, d)
            self.docks.append(d)
            widgets[i].corresponding_dock = d
            if i != 0:
                self.tabifyDockWidget(self.docks[i-1], self.docks[i])
            a = d.toggleViewAction()
            a.setObjectName(titles[i])
            self.toolsMenu.addAction(a)

        if len(self.docks) > 0:
            self.docks[0].raise_()

    def show_msg(self, x):
        print("RM VisibilityChange:")
        print(self.mwc.raster_map.corresponding_dock.isVisible())
    
    def suggest_open(self, filename):
        filename = str(filename)
        if not os.path.isfile(filename): return 
        r = QtWidgets.QMessageBox.question(self, "Margo GUI", "Processing finished. Do you want to open the last result?", QtWidgets.QMessageBox.Yes | QtWidgets.QMessageBox.No)
        if r == QtWidgets.QMessageBox.Yes:
            self.load_file(filename)
            self.margoWindow.hide()
        else:
            self.margoWindow.raise_()

    def initUI(self):
        self.setWindowIcon(QtGui.QIcon("etc/boa.ico"))
        mwc = MainWindowContents()
        self.mwc = mwc # I should use signals, but I am lazy
        self.setCentralWidget(mwc)

        self.createDockWidgets(*zip(*[(mwc.infobox, "Info"),
                           (mwc.canvas, "Profile"),
                            (mwc.canvas3d, "3D"),
                            (mwc.the_map, "Google Maps"),
                            (mwc.raster_map, "Raster"),
                            (mwc.filter_tab, "Filter")]))
        mwc.the_map.corresponding_dock.visibilityChanged.connect(mwc.update_map2)
        mwc.canvas.corresponding_dock.visibilityChanged.connect(mwc.update_profile)
        mwc.canvas3d.corresponding_dock.visibilityChanged.connect(mwc.update_3d)
        mwc.raster_map.corresponding_dock.visibilityChanged.connect(mwc.update_raster)
        
        #mwc.raster_map.corresponding_dock.visibilityChanged.connect(self.show_msg)

        mwc.raster_map.status_changed.connect(self.update_coord_status)
        mwc.raster_map.point_inserted.connect(self.mwc.insert_missing_point)
        mwc.raster_map.point_selected.connect(lambda x: self.mwc.select_record(x, "raster"))

        self.margoWindow = MargoWindow()
        self.convertSHPWindow = ConvertSHPWindow()
        self.margoWindow.processing_finished.connect(self.suggest_open)
        
        #ef = FilterObject(self)
        #self.installEventFilter(ef)
        #mwc.installEventFilter(ef)
        #mwc.table.installEventFilter(ef)

        openFile = QtWidgets.QAction(QtGui.QIcon('open.png'), 'Open..', self)
        openFile.setShortcut('Ctrl+O')
        openFile.setStatusTip('Load XYZ or SHP File')
        openFile.triggered.connect(self.showDialog)


        openMultiFile = QtWidgets.QAction('Open multiple..', self)
        openMultiFile.setStatusTip('Merge multiple files')
        openMultiFile.triggered.connect(self.showMultiDialog)

        importJoin = QtWidgets.QAction('Join..', self)
        importJoin.setStatusTip('Join another table to the current one')
        importJoin.triggered.connect(self.showImportJoinDialog)


        saveFile = QtWidgets.QAction('Save', self)
        saveFile.setShortcut('Ctrl+S')
        saveFile.setStatusTip('Save XYZ File')
        saveFile.triggered.connect(self.save_xyz)

        saveAs = QtWidgets.QAction('Save As..', self)
        saveAs.setStatusTip('Save XYZ File As')
        saveAs.triggered.connect(self.showSaveDialog)

        exportCSV = QtWidgets.QAction('&Comma-separated text file (CSV)..', self)
        exportCSV.setStatusTip('Export current table as csv')
        exportCSV.triggered.connect(self.showExportCSVDialog)

        exportSHP = QtWidgets.QAction('&ESRI shapefile..', self)
        exportSHP.setStatusTip('Export current table as shp')
        exportSHP.triggered.connect(self.showExportSHPDialog)

        loadRaster = QtWidgets.QAction('Load raster..', self)
        loadRaster.setStatusTip('Load raster file')
        self.raster_filename = config.get("last_raster_fname")
        loadRaster.triggered.connect(self.showRasterDialog)

        loadProj = QtWidgets.QAction('Load projection..', self)
        loadProj.setStatusTip('Load ESRI projection file')
        loadProj.triggered.connect(self.showProjectionDialog)

        #clr = QtGui.QAction('Clear memory', self)
        #clr.triggered.connect(self.releaseDamnedMemory)



        showSelected = QtWidgets.QAction('Refresh Google map', self)
        #openFile.setShortcut('Ctrl+O')
        showSelected.setStatusTip('Show all visible objects')
        showSelected.triggered.connect(self.mwc.show_map)

        selectVisible = QtWidgets.QAction('Show/hide columns', self)
        #openFile.setShortcut('Ctrl+O')
        selectVisible.setStatusTip('Select visible/exportable columns')
        selectVisible.triggered.connect(self.mwc.select_visible_columns)


        convertshp = QtWidgets.QAction('Cut masks..', self)
        convertshp.setStatusTip('Cut shapes from raster file')
        convertshp.triggered.connect(self.convertSHPWindow.show)

        process = QtWidgets.QAction('Generate detections..', self)
        #openFile.setShortcut('Ctrl+O')
        process.setStatusTip('Generate detections using Margo backend')
        process.triggered.connect(self.show_margo_dialog)



        self.classifier_form = classifier_form.ClassifierDialog() 
        classify = QtWidgets.QAction('Classify detections..', self)
        #openFile.setShortcut('Ctrl+O')
        classify.setStatusTip('Select and run classification')
        classify.triggered.connect(self.classifier_form.dialog.show)

        self.show_only_filtered = QtWidgets.QAction('Show only filter results', self)
        self.show_only_filtered.setStatusTip('Show only filter results')
        self.show_only_filtered.setCheckable(True)
        b = config.get("show_only_filtered")=="True"
        self.show_only_filtered.setChecked(b)
        self.mwc.show_only_filtered = b
        self.show_only_filtered.toggled.connect(self.show_only_filtered_toggled)

        self.statusbar = self.statusBar()
        self.coordStatus = QtWidgets.QLabel("()")
        self.coordStatus.setTextInteractionFlags(QtCore.Qt.TextSelectableByMouse)
        self.statusbar.showMessage("Welcome")
        self.statusbar.addPermanentWidget(self.coordStatus)

        menubar = self.menuBar()
        fileMenu = menubar.addMenu('&File')
        fileMenu.addAction(openFile)
        fileMenu.addAction(openMultiFile)
        fileMenu.addAction(importJoin)
        fileMenu.addAction(saveFile)
        fileMenu.addAction(saveAs)
        exportMenu = fileMenu.addMenu('&Export')
        exportMenu.addAction(exportCSV)
        exportMenu.addAction(exportSHP)
        #fileMenu.addAction(clr)

        viewMenu = menubar.addMenu('&View')
        viewMenu.addAction(self.show_only_filtered)
        viewMenu.addAction(showSelected)
        viewMenu.addAction(selectVisible)
        viewMenu.addMenu(self.toolsMenu)


        dataMenu = menubar.addMenu('&Analyse')
        dataMenu.addAction(convertshp)
        dataMenu.addAction(process)
        dataMenu.addAction(classify)
        
        rasterMenu = menubar.addMenu('&Raster')
        rasterMenu.addAction(loadRaster)
        rasterMenu.addAction(loadProj)
       
        self.current_fname = config.get("last_data_fname")
        self.setGeometry(200, 100, 800, 600)
        self.setWindowTitle('Margo GUI')
        self.restoreMWState()
        self.show()
    
    def update_coord_status(self, text):
        self.coordStatus.setText(text)

    #def releaseDamnedMemory(self):
    #    print (self.mwc.the_map.page().history().count())
    #    self.mwc.the_map.page().history().setMaximumItemCount(1)
    #    self.mwc.the_map.page().history().clear()
    #    self.mwc.the_map.settings().clearMemoryCaches()
    #    self.mwc.the_map.load(QtCore.QUrl("http://www.delfi.lt"))


    def restoreMWState(self):
        settings = QtCore.QSettings("layout.ini", QtCore.QSettings.IniFormat)
        if (not settings.contains("geometry")): return
        self.restoreGeometry(settings.value("geometry"))
        self.restoreState(settings.value("windowState"))


 
    def showRC(self):
        #print ("Rows moved")
        msg = str(self.mwc.table.model().rowCount()) + "/" + str(self.mwc.table.db().size) +  " entries"
        m = self.mwc.table.selectionModel()
        sel = len(m.selectedRows())
        if (sel > 0):
            msg = msg + " (selected: "+ str(sel)+  ")"
        self.statusBar().showMessage(msg)

    def show_margo_dialog(self):
        self.margoWindow.show()

    def save_xyz(self):
        self.mwc.table.save_xyz(self.current_fname)


    def showRasterDialog(self):
        name,_ = QtWidgets.QFileDialog.getOpenFileName(self, 'Open file', self.raster_filename)
        if name == "":
            return
        try:
            self.mwc.raster_map.set_raster(name)
            self.mwc.raster_map.set_projection(config.get("projection_filename"))
            self.raster_filename = name
            self.mwc.raster_map.corresponding_dock.raise_()
            config.set("last_raster_fname", name)
        except:
            QtWidgets.QMessageBox.information(self, "Error", "Could not load raster file or projection.")
   
        self.mwc.update_raster()

    def showProjectionDialog(self):
        name,_ = QtWidgets.QFileDialog.getOpenFileName(self, 'Open file', config.get("projection_filename"), "*.prj")
        if name == "":
            return
        config.set("projection_filename", name)
        self.mwc.raster_map.set_projection(name)
        self.mwc.update_raster()
    
    def change_status_coord(self):
        cr = self.mwc.table.current_row()
        if cr is None:
            self.update_coord_status("")
            return
        lon = self.mwc.table.db().get_item(cr, "LONGITUDE")
        lat = self.mwc.table.db().get_item(cr, "LATITUDE")
        self.update_coord_status(str( (int(lon),int(lat))))

    def showDialog(self):
        name,_= QtWidgets.QFileDialog.getOpenFileName(self, 'Open file', self.current_fname, "xyz, shp (*.xyz *.shp)")
        if name == "":
            return
        self.load_file(name)


    def showMultiDialog(self):
        names= QtWidgets.QFileDialog.getOpenFileNames(self, 'Select files to merge', self.current_fname, "xyz, shp (*.xyz *.shp)")
        if len(names) == 0: return
        names = [str(x) for x in names]

        self.load_file(names)
    

    #def load_data(self, name):
    #    ext = os.path.splitext(name)[1]
    #    if ext == ".shp":
    #        try:
    #            proj = gdal_interface.load_proj(config.get("projection_filename"))
    #            return xyz.load_shp(name, proj)
    #        except IOError:
    #            QtGui.QMessageBox.information(self, "Margo GUI", "Please select your projection first.") #could offer to take the shp proj
    #            raise
    #    elif ext == ".xyz":
    #        return xyz.parse_file(name)            #projection should be loaded in this case as well, and not assumed to be LKS
    #    else:
    #        raise RuntimeError("Unknown file extension")
        



    def load_file(self, names):
        if type(names) == str:
            self.current_fname = names
            newdb = xyz.load_from_file(names)
        else:
            newdb = xyz.merge_files(names,add_fname=True, rename_ids=True)
            self.current_fname = os.path.dirname(names[0]) + os.sep+ "Untitled.xyz"
        self.setWindowTitle('Margo GUI - '+os.path.basename(self.current_fname))
        #self.mwc.table.set_data( xyz.parse_file(self.current_fname) )
        proxy = MyQSortFilterProxyModel(self)
        proxy.setSourceModel(CoordDBModel(newdb))
        self.mwc.table.setModel(proxy)
        self.mwc.table.set_hidden_columns(config.get_multiline("hidden_columns"))

        
        self.mwc.table.selectionModel().currentChanged.connect(self.mwc.update_all)
        self.mwc.table.selectionModel().currentChanged.connect(self.change_status_coord)
        self.mwc.table.selectionModel().selectionChanged.connect(self.mwc.update_profile)
        self.mwc.table.selectionModel().selectionChanged.connect(lambda x: self.mwc.raster_map.replot(only_points=True))
        self.mwc.table.selectionModel().selectionChanged.connect(self.mwc.update_map_selection)
        self.mwc.table.selectionModel().selectionChanged.connect(self.showRC)
        self.mwc.table.setSelectionBehavior(QtWidgets.QAbstractItemView.SelectRows)
        #self.mwc.table.setSelectionMode(QtGui.QAbstractItemView.MultiSelection)

        self.mwc.table.setSortingEnabled(True)
        self.mwc.show_map(map_all = True)
       
        self.mwc.table.model().rowsInserted.connect(self.showRC)
        self.mwc.table.model().rowsRemoved.connect(self.showRC)
        self.showRC()
        config.set("last_data_fname", self.current_fname)
        if self.mwc.table.db().size > 0:
               self.mwc.table.selectRow(0) 

    def showSaveDialog(self):
        name = str(QtWidgets.QFileDialog.getSaveFileName(self, 'Save file', self.current_fname))
        if name is None or name == "": return
        self.current_fname = name
        self.save_xyz()
        self.setWindowTitle('Margo GUI - '+os.path.basename(self.current_fname))
        config.set("last_data_fname", self.current_fname)

    def showImportJoinDialog(self):
        path = config.get("last_import_join")
        name,_ = QtWidgets.QFileDialog.getOpenFileName(self, 'Import and join file', path, "xyz, shp (*.xyz *.shp)")
        if name is None or name == "": return
        name = str(name)
        db = xyz.load_from_file(name) #xyz.load_from_file(name)
        last_join_key = config.get_multiline("last_join_key")
        last_join_cols = config.get_multiline("last_join_cols")
        try:
            join_dist = int(config.get("last_join_distance"))
        except:
            join_dist = 20
        k = [x for x in self.mwc.table.db().column_names if x in db.column_names]
        join_key = [x for x in last_join_key if x in k]
        join_cols = [x for x in last_join_cols if x in db.column_names]
        join_key, join_cols, join_dist, ok = CheckBoxDialog2.getChecked("Join opened table", k, db.column_names, join_key, join_cols, join_dist)
        if not ok: return

        self.mwc.table.db().join_neighbours(db, join_key, join_cols, join_dist)
        config.set("last_import_join", name)
        config.set("last_join_distance", str(join_dist))
        config.set_multiline("last_join_key", join_key)
        config.set_multiline("last_join_cols", join_cols)
        print(self.mwc.table.db().column_names)
        self.mwc.table.model().setSourceModel(CoordDBModel(self.mwc.table.db()))
    
    
    def showExportCSVDialog(self):
        path = config.get("last_csv_fname")
        csvname = QtWidgets.QFileDialog.getSaveFileName(self, 'Export file', path, "*.csv")
        if csvname is None or csvname == "": return
        self.mwc.table.export_to_csv(str(csvname))
        config.set("last_csv_fname", csvname)
        #self.save_xyz()
        #self.setWindowTitle('Margo GUI - '+os.path.basename(self.current_fname))
        #config.set("last_data_fname", self.current_fname)

      #QtGui.QMessageBox.information(self, "The selected file is..", fname, "OK")

    def showExportSHPDialog(self):
        path = config.get("last_shp_fname")
        name = QtWidgets.QFileDialog.getSaveFileName(self, 'Export file', path, "*.shp")
        if name is None or name == "": return
        self.mwc.table.save_shp(str(name))
        config.set("last_shp_fname", name)

    def show_only_filtered_toggled(self):
        config.set("show_only_filtered", str(self.show_only_filtered.isChecked()))
        self.mwc.show_only_filtered = self.show_only_filtered.isChecked()
        self.mwc.show_map()
        self.mwc.update_raster()

    def saveMWState(self):
        settings = QtCore.QSettings("layout.ini", QtCore.QSettings.IniFormat)
        settings.setValue("geometry", self.saveGeometry())
        settings.setValue("windowState", self.saveState())



app = None
        
def main():
    global mainWindow
    app = QtWidgets.QApplication(sys.argv+["--disable-web-security"])
    app.setOrganizationName("margo")
    app.setWindowIcon(QtGui.QIcon("etc/boa.ico"))
    print ("App created")
    mainWindow = MainWindow()
    mainWindow.classifier_form.set_current_app(app)
    print("MainWindow created")
    app.lastWindowClosed.connect(mainWindow.saveMWState)
    app.lastWindowClosed.connect(mainWindow.margoWindow.kill_process)
    sys.exit(app.exec_())

'''
import traceback

sys._excepthook = sys.excepthook
def exception_hook(exctype, value, tb):
       traceback.print_stack()
       print exctype, value, tb
       sys._excepthook(exctype, value, tb)
       #print traceback.format_exc()
               #or
       #print sys.exc_info()[0]
       #print exctype, value, tb
       #raise RuntimeError("Fail fail fail")
       sys.exit(1)
sys.excepthook = exception_hook
'''

if __name__ == '__main__':
    main()
