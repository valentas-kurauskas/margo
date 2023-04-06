from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt5agg import NavigationToolbar2QT as NavigationToolbar
#from matplotlib.backends.backend_qt4 import FigureCanvasQT as FigureCanvas
#from matplotlib.backends.backend_qt4 import NavigationToolbar2QT as NavigationToolbar

import matplotlib.pyplot as plt
from PyQt5 import QtGui, Qt, QtCore, QtWidgets
#import os
from . import gdal_interface
from matplotlib.colors import LightSource
from matplotlib import cm
from . import config
import time
import numpy as np
from matplotlib.widgets import RectangleSelector


class SurfaceBrowser(QtWidgets.QWidget):

    def __init__(self, source):
        QtWidgets.QWidget.__init__(self)

        self.source = source

        splitter = QtWidgets.QSplitter(Qt.Qt.Vertical, self)
        
        self.figure = plt.figure(frameon = False) #(3)?
        self.figure.add_axes([0,0,1,1])
        #self.figure.gca().invert_yaxis()
        self.canvas = FigureCanvas(self.figure)
        self.canvas.mpl_connect("motion_notify_event", self.mouse_move)
        self.canvas.mpl_connect("pick_event", self.on_pick)
        self.canvas.mpl_connect("button_release_event", self.mouse_release)
        self.canvas.mpl_connect("button_press_event", self.mouse_press)
        self.canvas.mpl_connect("resize_event", lambda x: self.replot(center_at_cr = False, autozoom=True))

        self.canvasMenu = QtWidgets.QMenu(self)
        ca = QtWidgets.QAction('Insert a non-detection here', self)
        ca.triggered.connect(self.insert_nondetection)
        self.canvasMenu.addAction(ca)

        self.canvas.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
        self.canvas.customContextMenuRequested.connect(self.exec_canvas_menu)

        self.RS = RectangleSelector(self.figure.gca(), self.rectangle_selected,
                                       #drawtype='box', 
                                       useblit=True,
                                       button=[1], #[1,3], # don't use middle button
                                       minspanx=5, minspany=5,
                                       spancoords='pixels')
        self.light_source = LightSource(azdeg = 135, altdeg = 30)
        self.image = None
        self.raster = None
        self.last_x = None
        self.points = None
        self.selected = None
        self.current = None
        self.last_hidden = []
        self.last_selected = []
        self.last_current = []
        self.last_plotted_rows = []
        self.last_pick_time = time.time()
        self.last_x = None
        self.last_y = None
        self.last_top_left = None
        self.last_bottom_right = None

        self.size_slider = QtWidgets.QSpinBox () # (Qt.Qt.Horizontal)
        self.size_slider.setMinimum(5)
        self.size_slider.setMaximum(1000)
        self.size_slider.setSingleStep(10)
        self.size_slider.setSuffix("px")
        
        self.altitude_slider = QtWidgets.QDial()
        self.altitude_slider.valueChanged.connect(self.altitude_slider_change)
        self.altitude_slider.setMinimum(0)
        self.altitude_slider.setMaximum(360)
        self.altitude_slider.setWrapping(True)
        try:
            sv = int(config.get("raster_light_altitude"))
            self.altitude_slider.setValue( (-sv - 90) % 360 )
        except:
            self.altitude_slider.setValue( (-30 - 90) % 360 )

        self.angle_slider = QtWidgets.QDial() #QSlider(Qt.Qt.Vertical)
        self.angle_slider.setWrapping(True)
        self.angle_slider.setMinimum(0)
        self.angle_slider.setMaximum(360)

        self.angle_label = QtWidgets.QLabel("Azimuth")
        #self.angle_label.setSizePolicy(QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Minimum)
        #self.angle_label.setStyleSheet("QLabel { background-color : red; color : blue; }");

        self.angle_slider.setMaximumSize(40, 40)
        self.altitude_slider.setMaximumSize(40, 40)

        #self.altitude_slider.setMinimum(0)
        #self.altitude_slider.setMaximum(90)

        try:
            sv = int(config.get("raster_window_size"))
            self.size_slider.setValue(sv)
        except:
            self.size_slider.setValue(100)

        self.angle_slider.valueChanged.connect(self.angle_slider_change)
        try:
            sv = int(config.get("raster_light_angle"))
            self.angle_slider.setValue( (- sv) % 360)
        except:
            self.angle_slider.setValue( (- 135) % 360)

        self.size_slider.valueChanged.connect(self.size_slider_change)
 
        #layout = QtGui.QHBoxLayout()
        #layout.addWidget()
        #hsplit.setOrientation(Qt.Qt.Vertical)
        clayout = QtWidgets.QHBoxLayout()
        
        
        self.nav = NavigationToolbar(self.canvas, self, coordinates = False)
        a = self.nav.addAction(QtGui.QIcon('etc/arrow.png'), 'Point', self.nav_pointer)
        a.setToolTip('Turn off pan/zoom')
        #self.nav.configure_subplots.setVisible(False)
        #self.nav.save_figure.setVisible(False)
        for i,x in enumerate(self.nav.findChildren(QtWidgets.QAction)):
            #print i,x
            if x.text() in ['Subplots', 'Save', 'Customize', 'Back', 'Forward']:
                x.setVisible(False)

        #self.nav.DeleteToolByPos(6)
        #self.nav.setMaximumWidth(200)
        clayout.addWidget(self.nav, 0, QtCore.Qt.AlignLeft)
        clayout.setAlignment(QtCore.Qt.AlignLeft)
        clayout.addItem(QtWidgets.QSpacerItem(20,20, QtWidgets.QSizePolicy.Minimum, QtWidgets.QSizePolicy.Expanding))

        clayout2 = QtWidgets.QHBoxLayout()

        #clayout2.setAlignment(QtCore.Qt.AlignLeft)


        clayout2.addWidget(QtWidgets.QLabel("Area"))
        clayout2.addWidget(self.size_slider)
         #clayout.addWidget(QtGui.QLabel("Light source control"))
        s = str((- self.angle_slider.value()) % 360)
        #clayout_ang = QtGui.QHBoxLayout()
        clayout2.addWidget(QtWidgets.QLabel("Altitude"))
        clayout2.addWidget(self.altitude_slider)

        clayout2.addWidget(self.angle_label)
        clayout2.addWidget(self.angle_slider)
        #clayout.addLayout(clayout_ang)
        clayout2.setContentsMargins(0,0,0,0)
        clayout.addLayout(clayout2)
     
        w = QtWidgets.QWidget() #QGroupBox("Raster parameters")
        w.setLayout(clayout)
        splitter.addWidget(w)
        splitter.addWidget(self.canvas)
        
        layout = QtWidgets.QVBoxLayout()
        layout.setAlignment(QtCore.Qt.AlignTop)
        layout.addWidget(splitter)
        clayout.setContentsMargins(0,0,0,0)
        self.setLayout(layout)

    def mouse_press(self,event):
        self.RS.set_active(self.raster is not None and not self.nav._active in ['PAN', 'ZOOM'])

    def rectangle_selected(self, eclick, erelease):
        x1, y1 = eclick.xdata, eclick.ydata
        x2, y2 = erelease.xdata, erelease.ydata
        print(("rectangle", x1, y1, x2,y2))
        rows = self.source.db().filter_rectangle(min(x1,x2), max(x1,x2), min(y1,y2), max(y1,y2))
        self.source.select_rows(rows)

    def nav_pointer(self):
        if self.nav._active == 'PAN': self.nav.pan()
        if self.nav._active == 'ZOOM': self.nav.zoom()

    status_changed = QtCore.pyqtSignal(str)
    point_selected = QtCore.pyqtSignal(int)
    point_inserted = QtCore.pyqtSignal(float,float)

    def mouse_move(self, event):
        if self.raster is not None:
            #print (event.xdata, event.ydata)
            if event.xdata is None: return
            #cx,cy = self.raster.to_raster_coords(self.last_x, self.last_y)
            self.status_changed.emit(str(tuple(map(int, [event.xdata, event.ydata]))))
            #self.status_changed.emit(str(tuple(map(int, self.raster.to_geo_coords(self.last_tlr[0]+event.xdata, self.last_tlr[1] + event.ydata)))))
        #c = self.nav.findChildren(QtGui.QLabel)
        #if len(c) > 0:
        #    print c[-1].text()
        #    c[-1].hide()

    def mouse_release(self, event):
        self.RS.set_active(False)
        if self.last_top_left is None: return
        ax = self.figure.gca()
        corners =  ax.transData.inverted().transform( ax.transAxes.transform( [(0.1, 0.1), (0.1,0.9), (0.9,0.1), (0.9,0.9)]))
        lon,lat = ax.transData.inverted().transform( ax.transAxes.transform( (0.5, 0.5)))
        #tlr = self.raster.to_raster_coords(*self.last_top_left)
        #lon,lat=self.raster.to_geo_coords(tlr[0] + xy[0], self.last_tlr[1]+xy[1])
        tl,br= self.last_top_left, self.last_bottom_right     
        #print (lon,lat,tl,br)
        mx,Mx = min(tl[0], br[0]), max(tl[0], br[0])
        my,My = min(tl[1], br[1]), max(tl[1], br[1])

        inside = lambda x: ((mx <= x[0] <= Mx) and (my <= x[1] <= My))

        if not min(map(inside, corners)):
            #print ("replot from release")
            self.last_x=lon
            self.last_y=lat
            self.replot(center_at_cr = False, autozoom = True, keep_lims = True)
      

    def on_pick(self, event):
        t = time.time()
        if 1000 * (t - self.last_pick_time) < 500:
            #print "Ignore quick pick"
            self.last_pick_time = t
            return                        #bug?? picks the press for the new point
        self.last_pick_time = t

        artist = event.artist
        #xmouse, ymouse = event.mouseevent.xdata, event.mouseevent.ydata
        #x, y = artist.get_xdata(), artist.get_ydata()
        ind = event.ind
        #print 'Artist picked:', event.artist
        #print '{} vertices picked'.format(len(ind))
        #print 'Pick between vertices {} and {}'.format(min(ind), max(ind)+1)
        #print 'x, y of mouse: {:.2f},{:.2f}'.format(xmouse, ymouse)
        #print 'Data point:', x[ind[0]], y[ind[0]]
        
        
        #if self.current is not None:
        #    xc,yc = self.current.get_xdata(), self.current.get_ydata()
        #    if (len(xc) > 0):
        #        xc[0] = x[ind[0]]
        #        yc[0] = y[ind[0]]
        #        self.current.set_xdata(xc)
        #        self.current.set_ydata(yc)
        self.point_selected.emit(artist.ids[ind[0]])
        #print ("replot from pick")
        self.replot(only_points=True)
        #self.canvas.draw() 

    def angle_slider_change(self):
        v = (- self.angle_slider.value()) % 360
        config.set("raster_light_angle", str(v))
        self.light_source.azdeg = v
        s = str((- self.angle_slider.value()) % 360)
        self.status_changed.emit("Light azimuth: "+s.rjust(6, " "))
        self.replot()

    def altitude_slider_change(self):
        v = (- self.altitude_slider.value()-90) % 360
        if v > 90 and v <= 180:
            v = 90
            self.altitude_slider.setValue( (-90 - v) % 360)
        if v > 180:
            v = 0
            self.altitude_slider.setValue( (-90-v) % 360)
        config.set("raster_light_altitude", str(v))
        self.light_source.altdeg = v
        self.status_changed.emit("Light altitude: "+str(v).rjust(6, " "))
        self.replot()

    def size_slider_change(self):
        config.set("raster_window_size", str(self.size_slider.value()))
        self.replot(autozoom = True)

    def pan_to(self, coord):
        if coord == 'cr':
            cr = self.source.current_row()
            if (center_at_cr) and (cr is not None):
                x,y = self.source.db().get_item(cr, "LONGITUDE"), self.source.db().get_item(cr, "LATITUDE")
        else:
            x,y = coord
        xl = self.figure.gca().get_xlim()
        yl = self.figure.gca().get_ylim()

        hx = (xl[1] - xl[0])/2.0
        hy = (yl[1] - yl[0])/2.0
        ax.set_xlim(x -hx , x+ hx)
        ax.set_xlim(y -hy , y+ hy)

    def replot(self, only_points = False, center_at_cr = False, autozoom = False, hidden =None, keep_lims = False):
        if self.last_x is None:
            return
        x,y = self.last_x, self.last_y
        cr = self.source.current_row()
        if (center_at_cr) and (cr is not None):
            x,y = self.source.db().get_item(cr, "LONGITUDE"), self.source.db().get_item(cr, "LATITUDE")

        if hidden is None:
            hidden = self.last_hidden

        if (self.last_x is not None):
            self.plot_point(x, y, hidden, self.source.selected_rows(), [self.source.current_row()], only_points, autozoom, keep_lims)


    def set_raster(self, filename):
        if filename == "":
            self.raster = None
            return
        self.raster = gdal_interface.GdalMap(filename)

    def set_projection(self, filename):
        print(("Setting projection: "+filename))
        if filename == "":
            return
        if self.raster is not None:
            self.raster.dataset.SetProjection(gdal_interface.load_proj(filename).ExportToWkt()) #strange.

    #always autozoom??? or not autozoom = keep_lims?
    def plot_point(self, x, y, hidden = [],  selected = [], current = [], only_points = False, autozoom=True, keep_lims = False):
        if autozoom or self.last_top_left is None:
            v = self.size_slider.value()
            width,height = self.canvas.get_width_height()
            #print width, height
            if height < width:
                dy = v
                dx = (v * width) / height
            else:
                dx = v
                dy = (v* height) / width
            
            #print (dx, dy)

            self.plot_area((x-dx, y + dy), (x+dx, y-dy), hidden, selected, current, only_points, autozoom, keep_lims)
        else:
            self.plot_area(self.last_top_left, self.last_bottom_right, hidden, selected, current, only_points, autozoom, keep_lims)


        self.last_x = x
        self.last_y = y
        self.last_hidden =hidden
        self.last_selected = selected
        self.last_current = current

    #coords in LKS
    #so far hidden are direct ids
    #and selected, current are "ID"s
    def plot_area(self, top_left0, bottom_right0, hidden = [], selected = [], current = [], only_points=False, autozoom = True, keep_lims=False):
        if self.raster == None:
            return
        
        if autozoom:
            try:
                self.nav._views.clear()
                self.nav._positions.clear()
            except:
                print ("failed to clear views")

        #print ("plot_area", top_left0, bottom_right0)
        

        ax = self.figure.gca()

        top_left, bottom_right = self.raster.get_actual_bounds(top_left0, bottom_right0)
        xl = ax.get_xlim()
        yl=ax.get_ylim()
        

        if not only_points:
            #ax.set_aspect("equal", "datalim")
            r = self.raster.get_rectangle(top_left, bottom_right)
            if len(r) == 0: 
                print("No raster at this point")
                return
            #print (r.max(), r.min())
            r= np.ma.array(r, mask= (r<-9999)) #mask
            gist_tampered = cm.gist_earth
            gist_tampered.set_bad('k', 1.0)
            #print (r)
            r = self.light_source.shade(r, gist_tampered) #create shadows
            dim = (len(r), len(r[0]))

            e = [top_left[0], bottom_right[0], bottom_right[1], top_left[1]]
            #e = [top_left[0], bottom_right[0], top_left[1], bottom_right[1]]

            if (self.image is None) or (dim != self.last_dimension):
                #print ("new imshow")
                ax.autoscale(True)
                #ax.get_xaxis().set_visible(False)
                #ax.get_yaxis().set_visible(False)
                ax.set_axis_off()
                self.image = ax.imshow(r, aspect = 'equal', extent = e)
                ax.set_aspect("equal", "datalim")
                #self.image = ax.imshow(r, aspect = 'auto')
                ax.autoscale(False)
            else:
                #print ("update imshow")
                self.image.set_array(r)
                self.image.set_extent(e)

            self.last_dimension = dim

        #print ('TOPLEFT', top_left)
        #print ('BOTTOMRIGHT', bottom_right)

        if keep_lims:
            ax.set_xlim(xl)
            ax.set_ylim(yl)
        elif autozoom:
            ax.set_xlim(top_left0[0], bottom_right0[0])
            #ax.set_ylim(top_left0[1], bottom_right0[1]) #upside down!!
            ax.set_ylim(bottom_right0[1], top_left0[1])


        self.last_top_left = top_left0
        self.last_bottom_right = bottom_right0
 
        #print ("actual bounds: ", top_left, bottom_right)
        ##tlr = self.raster.to_raster_coords(top_left[0], top_left[1])
        ##self.last_tlr = tlr
        #print ("tlr:",tlr)
        #brr = self.raster.to_raster_coords(bottom_right)

        db = self.source.db()
        rows = db.filter_rectangle(top_left[0], bottom_right[0], bottom_right[1], top_left[1])
        rows = [rr for rr in rows if not rr in hidden]
        x = db.get_projection("LONGITUDE", rows)
        y = db.get_projection("LATITUDE", rows)
        #ids = db.get_projection("ID", rows)
        
        ##xy_tr = [self.raster.to_raster_coords(*z) for z in zip(x,y)]
        ##x_tr,y_tr = zip(*xy_tr)

        # subtract top left coordinates, because the image is shifted to (0,0)
        ##x_tr = [xx - tlr[0] for xx in x_tr]
        ##y_tr = [yy - tlr[1] for yy in y_tr]

        x_tr, y_tr = x,y

        rrows = range(len(rows))
        self.last_plotted_rows = rows
        rselected = list(filter(lambda x: (rows[x] in selected) and (not rows[x] in current), rrows))
        rsimple = list(filter(lambda x: (not rows[x] in current) and (not rows[x] in selected), rrows))
        rcurrent = list(filter(lambda x: rows[x] in current, rrows))


        x_cur = [x_tr[i] for i in rcurrent]
        y_cur = [y_tr[i] for i in rcurrent]
         
        x_sel = [x_tr[i] for i in rselected]
        y_sel = [y_tr[i] for i in rselected]

        x_sim = [x_tr[i] for i in rsimple]
        y_sim = [y_tr[i] for i in rsimple]

        
        #print ("Current", current)
        #print ("Selected", selected)

        if self.points is None:
            tol = 10
            self.points,  = ax.plot(x_sim, y_sim, 'k+', picker = tol)
            self.selected,  = ax.plot(x_sel, y_sel, 'rx', zorder = 10, picker = tol)
            self.current, = ax.plot(x_cur, y_cur, 'o', markeredgecolor = 'red', markerfacecolor = 'none', markersize = 20, zorder = 20, picker=tol)
        else:
            self.points.set_xdata(x_sim)
            self.points.set_ydata(y_sim)
            self.selected.set_xdata(x_sel)
            self.selected.set_ydata(y_sel)
            self.current.set_xdata(x_cur)
            self.current.set_ydata(y_cur)
        self.points.ids = [rows[i] for i in rsimple]
        self.selected.ids = [rows[i] for i in rselected]
        self.current.ids = [rows[i] for i in rcurrent]
        self.canvas.draw()

    def exec_canvas_menu(self, point):
        self.last_point = point
        self.canvasMenu.exec_(self.canvas.mapToGlobal(point))

    def insert_nondetection(self):
        ax = self.figure.gca()
        #print "LAST POINT: ", (self.last_point.x(), self.canvas.height() - self.last_point.y())
        xy = ax.transData.inverted().transform((self.last_point.x(),  self.canvas.height() - self.last_point.y()))
        #print ("will insert a missing detection at:", xy)
        self.point_inserted.emit(xy[0], xy[1])
        


