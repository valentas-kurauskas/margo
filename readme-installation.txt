Margo: Archeological raster data processor and semi-automatic burrial mound classifier.

Licence for the original code (for dependencies see their separate licences): MIT, see LICENCE.

RUNNING GUI WITH PYTHON 3

Tested only on Ubuntu 22.04


1. Better to use a new environment: `python3 -m venv env_margo`; `source env_margo/bin/activate`
2. `sudo apt install libgdal-dev`. If there is a package conflict: `sudo apt remove mariadb-dev`.
3. `pip install -r gui/requirements.txt`. The GDAL package version needs to match the GDAL version installed in the previous step.

There is a strange known bug related with `chromium` browser internally used by PyQt and Linux. If the map does not work for you,
check that all `ulimit` limits are unset.

RUNNING GUI WITH PYTHON 2

Dependencies for gui: Python (2.7), numpy, scipy, matplotlib, PyQt4, gdal. Use standard instructions for your operating system to install them:

1. Install python 2.7 https://www.python.org/download/releases/2.7/.
2. Install PyQt4 for Python 2.7 http://www.riverbankcomputing.co.uk/software/pyqt/download.
3. sudo apt get install python-tk
4. Use "pip install -r gui/requirements.txt" to install the remaining dependencies, see https://pip.pypa.io/en/latest/user_guide.html.

If gdal installation fails (Ubuntu):

sudo apt-get install libgdal-dev
export CPLUS_INCLUDE_PATH=/usr/include/gdal
export C_INCLUDE_PATH=/usr/include/gdal
pip install GDAL==1.10.0


RUNNING THE GUI

Go to `gui/` and run `python explore.py`.


COMPILING THE C++ BACKEND

Go to margo/ and run make.

The source files have been built and tested on Ubuntu Linux and Windows 7 (mingw and msys). 
Dependencies for margo: libhfz (included), zlib, eigen (included).

NOTE FOR WINDOWS

On Windows, the Python mingw GDAL package needs a special version of
Microsoft Visual C++ redistributables. These can be installed from here:
http://www.microsoft.com/en-us/download/details.aspx?id=26368.
