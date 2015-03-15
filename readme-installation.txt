Margo: Archeological raster data processor and semi-automatic burrial mound classifier.

Licence for the original code (for dependencies see their separate licences): MIT, see LICENCE.

RUNNING GUI WITH PYTHON

Dependencies for gui: Python (2.7), numpy, scipy, matplotlib, PyQt4, gdal. Use standard instructions for your operating system to install them:

1. Install python 2.7 https://www.python.org/download/releases/2.7/.
2. Install PyQt4 for Python 2.7 http://www.riverbankcomputing.co.uk/software/pyqt/download.
3. Use "pip install -r gui/requirements.txt" to install the remaining dependencies, see https://pip.pypa.io/en/latest/user_guide.html.


To run the gui: go to gui/ and run python explore.py.


COMPILING THE C++ BACKEND

Go to margo/ and run make.

The source files have been built and tested on Ubuntu Linux and Windows 7 (mingw and msys). 
Dependencies for margo: libhfz (included), zlib, eigen (included).

NOTE FOR WINDOWS

On Windows, the Python mingw GDAL package needs a special version of
Microsoft Visual C++ redistributables. These can be installed from here:
http://www.microsoft.com/en-us/download/details.aspx?id=26368.
