Margo: Archeological raster data processor and semi-automatic burrow mound classifier.

Author: Valentas Kurauskas (Vilnius University)

Licence for the original code (for dependencies see their separate licences): MIT

The source files have been built and tested on Ubuntu Linux and Windows 7 (mingw and msys). 

To build margo go to margo/ and run make.

Dependencies for margo: libhfz (included), zlib, eigen (included).

To run the gui: go to gui/ and run python explore.py.

Dependencies for gui: Python (2.7), numpy, scipy, matplotlib, PyQt4, gdal. Use standard instructions for your operating system to install them.
1. Install python 2.7 https://www.python.org/download/releases/2.7/.
2. Install PyQt4 for Python 2.7 http://www.riverbankcomputing.co.uk/software/pyqt/download.
3. Use "pip install -r gui/requirements.txt" to install the remaining dependencies, see https://pip.pypa.io/en/latest/user_guide.html.

Standalone windows binaries available from me (valentas.kurauskas@mif.vu.lt).
