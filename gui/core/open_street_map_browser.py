
#from PyQt6.QtWebKit import QWebView
from PyQt6.QtWebEngineWidgets import QWebEngineView
from PyQt6.QtCore import QObject, pyqtSlot, QTimer
import os


f = open("etc/open_street_map_template.html", 'r')
OPEN_STREETMAP_TEMPLATE = f.read()
f.close()

class OSMapsBrowser(QWebEngineView):

    def __init__(self):
        QWebEngineView.__init__(self)

        #self.settings().setMaximumPagesInCache(0)
        #self.settings().setObjectCacheCapacities(0, 0, 0)
        #self.settings().setOfflineStorageDefaultQuota(0)
        #self.settings().setOfflineWebApplicationCacheQuota(0)

        self.loadFinished.connect(self._result_available)
        print("!disabled autoload images!")
        #self.settings().setAttribute(self.settings().AutoLoadImages, True)

        #profile = QWebEngineProfile.defaultProfile()
        #profile.setOption(QtWebEngineWidgets.QWebEngineSettings.AutoLoadImages, True)

        #self.click_catcher = ClickCatcher(self, self.clicked)
        #self.page().mainFrame().addToJavaScriptWindowObject("outside", self.click_catcher)
        #self.networkAccessManager().finished.connect(self.inspect_finished)
        self.last_id_list = None
        self.last_marked = None
        self.last_selection = None
        self.last_event = {}
        self.initialized = False
        #print("Maps inited")
   
    def mark_selected(self, ID, internal = False):
        if not self.initialized:
            self.last_event["mark_selected"] = {"ID":ID, "internal":internal}
            return
        if ID == self.last_marked: 
            return
        line = ''
        #print("mark_selected", ID, self.last_marked, self.last_id_list)
        if self.last_id_list is None: return
        if self.last_marked is not None:
            line += "all_markers["+str(self.last_marked) +"].setIcon("+self.unselected_icon_str+");\n"
            #self.page().runJavaScript(line)
        if internal:
            newid = ID
            line += "all_markers["+str(newid) +"].setIcon("+self.current_icon_str +");\n"
        elif ID in self.last_id_list:
            newid = self.last_id_list.index(ID)
            line += "all_markers["+str(newid) +"].setIcon("+self.current_icon_str +");\n"
        else:
            newid = None
            #return
        print (line)
        self.page().runJavaScript(line)
        self.last_marked = newid

    def update_selection(self, IDS, internal = False):
        #print("update_selection", self.last_id_list, self.last_selection)
        if not self.initialized:
            self.last_event["update_selection"] = {"IDS":IDS, "internal":internal}
            return
        line = ''
        if self.last_id_list is None: return
        if self.last_selection is not None:
            for i in self.last_selection:
                line += "all_markers["+str(i) +"].setIcon("+self.unselected_icon_str+");\n"
        #print ("update selection", IDS)
        if IDS is not None:
            if internal:
                newsel = IDS
            else:
                newsel = [self.last_id_list.index(i) for i in IDS if i in self.last_id_list]
            for i in newsel:
                line += "all_markers["+str(i) +"].setIcon("+self.selected_icon_str +");\n"
            self.last_selection = newsel

        if line != '':
            print(line)
            self.page().runJavaScript(line)

    def _html(self, x):
        print("result2", len(x))

    def _result_available(self, ok):
        print ("_result_available")

        if not ok:
            print("Failed to load map!")
        else:
            if not self.initialized:
                self.initialized = True
            print("Map loaded successfully!")

        #self.count += 1
        #if (self.count % 2 == 1):
        #    return
        #frame = self.page().mainFrame()
        frame = self.page()
        #print(("result", len(frame.toHtml())))
        frame.toHtml(self._html)
        #print(frame.toHtml())
        #print(("result", ok))
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.run_events)
        self.timer.start(1000)

    def run_events(self):
        self.timer.stop()
        #print("last_id_list",self.last_id_list)
        for f in ["update_selection", "mark_selected"]:
            kwargs = self.last_event.get(f, None)
            if kwargs is not None:
                if f == "update_selection":
                    self.update_selection(**kwargs)
                if f == "mark_selected":
                    self.mark_selected(**kwargs)
            self.last_event[f] = None

    
    #def clicked(self, owner, what):
    #    print("Clicked!", self, owner, what)

    def show_points(self, id_list, lat_list, lon_list, name_list, lat_center, lon_center, zoom):
        list_str = ""
        for i in range(len(lat_list)):
            list_str += "['"+name_list[i]+"', "+str(lat_list[i])[:7]+", "+str(lon_list[i])[:7]+", "+str(1+i)+"]"
            if (i != len(lat_list)-1):
                list_str +=",\n"
        #if (marker_types == None):
        #    marker_types = [0] * len(lat_list)
        #types_str = ", ".join(map(str, marker_types))
        center_str = str(lat_center)[:7] +"," + str(lon_center)[:7]
        #print(types_str)
        print(("show_points", center_str))
        #does not work otherwise
        path = os.getcwd().replace(os.sep, "/")+"/etc/"

        self.unselected_icon_str = 'unselectedIcon' #"'file:///"+path+"detection.png'"
        self.current_icon_str =  'currentIcon' #"'file:///"+path+"current.png'"
        self.selected_icon_str =  'selectedIcon' #"'file:///"+path+"selected.png'"

        html = OPEN_STREETMAP_TEMPLATE.replace("__POINT__LIST__", list_str).replace("__CENTER__COORDS__", center_str).replace("__ZOOM__", str(zoom)).replace("__CURRENT__DIR__", path)
        #print((os.getcwd()))
        f = open("etc/last_osmap.html",'w')
        f.write(html)
        f.close()
        self.last_id_list = id_list
        self.last_selection = None
        self.last_marked = None

        self.setHtml(html)

    #def set_click_catcher(owner, f):
    #    self.click_catcher.owner = owner
    #    self.click_catcher.on_click = f
    
