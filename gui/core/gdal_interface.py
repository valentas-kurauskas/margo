from osgeo import gdal,osr
from osgeo.gdalconst import *
import sys
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
from matplotlib.colors import LightSource

def load_proj(filename):
        srs = osr.SpatialReference()
        f = open(filename)
        lines = [x for x in f]
        srs.ImportFromESRI(lines)
        return srs #.ExportToWkt()


class GdalMap:
    def __init__(self, filename, projection=None):
        self.filename = filename
        dataset = gdal.Open(filename, GA_ReadOnly )
        if dataset is None:
            print("is none")

        print 'Driver: ', dataset.GetDriver().ShortName,'/', \
            dataset.GetDriver().LongName
        print 'Size is ',dataset.RasterXSize,'x',dataset.RasterYSize, \
            'x',dataset.RasterCount
        if (projection is not None):
            dataset.SetProjection(projection)
        print 'Projection is ',dataset.GetProjection()

        geotransform = dataset.GetGeoTransform()
        if not geotransform is None:
            print 'Origin = (',geotransform[0], ',',geotransform[3],')'
            print 'Pixel Size = (',geotransform[1], ',',geotransform[5],')'
        self.dataset = dataset
        self.ox = geotransform[0]
        self.oy = geotransform[3]
        self.dx = geotransform[1]
        self.dy = geotransform[5]

    def to_raster_coords(self, x,y):
        return (int(round((x-self.ox)/self.dx)),int(round((y-self.oy)/self.dy)))

    def to_geo_coords(self, x,y):
        return (x * self.dx + self.ox, y * self.dy + self.oy)


    def get_actual_bounds(self, top_left, bottom_right):
        tlx,tly = self.to_raster_coords(*top_left)
        brx,bry = self.to_raster_coords(*bottom_right)
        tlx = min(max(0, tlx), self.dataset.RasterXSize-1)
        tly = min(max(0, tly), self.dataset.RasterYSize-1)
        brx = min(max(0, brx), self.dataset.RasterXSize-1)
        bry = min(max(0, bry), self.dataset.RasterYSize-1)
        return self.to_geo_coords(tlx, tly), self.to_geo_coords(brx,bry)


    def get_rectangle(self, top_left, bottom_right, width = None, height = None, xy=False):
    
        tlx,tly = self.to_raster_coords(*top_left)
        brx,bry = self.to_raster_coords(*bottom_right)
        #print ("tl:",tlx,tly)
        #print ("br:",brx,bry)
        '''
        sx = 1 #step size -- nyi
        sy = 1
        if width is not None:
            sx = (brx-tlx)/width
        if height is not  None:
            sy = (bry-tly)/height
        '''

        tlx = min(max(0, tlx), self.dataset.RasterXSize-1)
        tly = min(max(0, tly), self.dataset.RasterYSize-1)

        brx = min(max(0, brx), self.dataset.RasterXSize-1)
        bry = min(max(0, bry), self.dataset.RasterYSize-1)


        band = self.dataset.GetRasterBand(1)
        matrix = band.ReadAsArray(tlx,tly, (brx-tlx), (bry - tly))
        if xy:
            xrow = [int(top_left[0] + i * self.dx) for i in range(brx-tlx)]
            yrow = [int(top_left[1] + i * self.dy) for i in range(bry-tly)]
            x,y = np.meshgrid(xrow, yrow)
            return x,y,matrix
        else:
            #print matrix
            return matrix


def plot_matrix(x):
    #fig = plt.figure()
    #ax = fig.add_subplot(111, projection='3d')
    #ax.plot_surface(x,y,z,rstride=1, cstride=1, cmap=cm.jet, shade = True)
    #plt.show()
    plt.imshow(ls.shade(x, cm.gist_earth))
   

#m = Map(sys.argv[1], load_proj(sys.argv[2]))
#r = m.get_rectangle( (602196,6090695), (602396, 6090500))
#plot_matrix(r)
