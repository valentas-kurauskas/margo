# margo
Margo: open source burial mound classifier with a graphical interface

This is an open source archeological data (burial mound) detector, analyser and classifier developed in Vilnius University and
Institute of Mathematics and Informatics (Lithuania) by Valentas Kurauskas (modelling, programming and machine learning) and Renaldas Augustinavičius (archeology). 

It consists of two parts: a raster data processor, written in C++ and a graphical interface, written in Python (using PyQt, matplotlib, sklearn, gdal).

At present the raster processor works only with raster files in hfz format and supports only LKS-94 (Lithuanian) coordinate system. The graphical interface should work with many projections and formats supported by GDAL. 

Documentation: see doc/ (to be added more).

Comments and feedback is welcome: myfirstname@gmail.com.

*2019-07-14 update: Check the branch python3-dev for a not yet complete python3 and PyQt5 version.*
*2023-05-07 update: Check the branch python3-PyQt6 for a not yet complete, but working python3 and PyQt6 version.*
