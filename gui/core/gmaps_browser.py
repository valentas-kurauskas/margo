
#from PyQt5.QtWebKit import QWebView
from PyQt5.QtWebEngineWidgets import QWebEngineView
from PyQt5.QtCore import QObject, pyqtSlot
import os


f = open("etc/gmaps_template.html", 'r')
GMAPS_TEMPLATE = f.read()
f.close()

#https://gist.github.com/agateau/1647398
#class ClickCatcher(QObject):
#    def __init__(self, method_owner, on_click):
#        QObject.__init__(self)
#        self.on_click = on_click
#        self.method_owner = method_owner
#        print ("initin")
#
#    @pyqtSlot(str, result=int)
#    def clicked(self, value):
#        print("clicking..")
#        self.on_click(self.method_owner, value)
#        return 0


class GMapsBrowser(QWebEngineView):

    def __init__(self):
        QWebEngineView.__init__(self)

        #self.settings().setMaximumPagesInCache(0)
        #self.settings().setObjectCacheCapacities(0, 0, 0)
        #self.settings().setOfflineStorageDefaultQuota(0)
        #self.settings().setOfflineWebApplicationCacheQuota(0)

        self.loadFinished.connect(self._result_available)
        self.settings().setAttribute(self.settings().AutoLoadImages, True)
        #self.click_catcher = ClickCatcher(self, self.clicked)
        #self.page().mainFrame().addToJavaScriptWindowObject("outside", self.click_catcher)
        #self.networkAccessManager().finished.connect(self.inspect_finished)
        self.last_id_list = None
        self.last_marked = None
        self.last_selection = None
   
    def mark_selected(self, ID, internal = False):
        if self.last_id_list is None: return
        if self.last_marked is not None:
            line = "markers["+str(self.last_marked) +"].setIcon("+self.unselected_icon_str+")"
            #print line
            self.page().runJavaScript(line)
        if internal:
            newid = ID
        elif ID in self.last_id_list:
            newid = self.last_id_list.index(ID)
        else:
            self.last_marked = None
            return
        line = "markers["+str(newid) +"].setIcon("+self.current_icon_str +")"
        #print line
        self.page().runJavaScript(line)
        self.last_marked = newid

    def update_selection(self, IDS, internal = False):
        if self.last_id_list is None: return
        if self.last_selection is not None:
            for i in self.last_selection:
                line = "markers["+str(i) +"].setIcon("+self.unselected_icon_str+")"
                self.page().runJavaScript(line)
        #print ("update selection", IDS)
        if IDS is None:
            return
        if internal:
            newsel = IDS
        else:
            newsel = [self.last_id_list.index(i) for i in IDS if i in self.last_id_list]
        for i in newsel:
            line = "markers["+str(i) +"].setIcon("+self.selected_icon_str +")"
            self.page().runJavaScript(line)
        self.last_selection = newsel

    def _html(self, x):
        print("result2", len(x))

    def _result_available(self, ok):
        #self.count += 1
        #if (self.count % 2 == 1):
        #    return
        #frame = self.page().mainFrame()
        frame = self.page()
        #print(("result", len(frame.toHtml())))
        frame.toHtml(self._html)
        #print(frame.toHtml())
        #print(("result", ok))
        self.update_selection(self.last_selection, True)
        self.mark_selected(self.last_marked, True)

    
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

        self.unselected_icon_str = "'file:///"+path+"detection.png'"
        self.current_icon_str = "'file:///"+path+"current.png'"
        self.selected_icon_str = "'file:///"+path+"selected.png'"

        html = GMAPS_TEMPLATE.replace("__POINT__LIST__", list_str).replace("__CENTER__COORDS__", center_str).replace("__ZOOM__", str(zoom)).replace("__CURRENT__DIR__", path)
        print((os.getcwd()))
        f = open("etc/last_map.html",'w')
        f.write(html)
        f.close()
        self.last_id_list = id_list
        self.last_selection = None
        self.last_marked = None
        self.setHtml(html)

    #def set_click_catcher(owner, f):
    #    self.click_catcher.owner = owner
    #    self.click_catcher.on_click = f
