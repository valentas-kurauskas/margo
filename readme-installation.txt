Margo: Archeological raster data processor and semi-automatic burrial mound classifier.

Licence for the original code (for dependencies see their separate licences): MIT, see LICENCE.

INSTALLING GUI - PYTHON 3

(Tested only on Ubuntu 22.04, for other OSes see https://github.com/valentas-kurauskas/margo/releases/tag/v0.76)

1. `sudo apt install libgdal-dev`. If there is a package conflict: `sudo apt remove mariadb-dev`.
2. `cd gui; poetry install`. The GDAL package version needs to match the GDAL version installed in the previous step.

There is a strange known bug related with `chromium` browser internally used by PyQt and Linux. If the map does not work for you,
check that all `ulimit` limits are unset.

RUNNING THE GUI

Go to `gui/` and run `poetry shell`, then `python explore.py`.

COMPILING THE C++ BACKEND

Go to margo/ and run make.

The source files have been built and tested on Ubuntu Linux and Windows 7 (mingw and msys). 
Dependencies for margo: libhfz (included), zlib, eigen (included).

NOTE FOR WINDOWS (2015-03-12)

On Windows, the Python mingw GDAL package needs a special version of
Microsoft Visual C++ redistributables. These can be installed from here:
http://www.microsoft.com/en-us/download/details.aspx?id=26368.
