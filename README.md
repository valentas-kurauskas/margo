# margo
Margo: open source burrow mound classifier with a graphical interface

This is an open source archeological data (burow mound) analyser and classifier developed in Vilnius University and
Institute of Mathematics and Informatics (Lithuania) by Valentas Kurauskas (modelling, programming and machine learning) and Renaldas Augustinavičius (archeology). 

It consists of two parts: a raster data processor, written in C++ and a graphical interface, written in Python (using PyQt, matplotlib, sklearn, gdal).

The current version of the raster processor works only with raster files in hfz format and supports only LKS-94 (Lithuanian) coordinate system. (The graphical interface should work with many projections and formats supported by GDAL). In the nearest future, arbitrary projections and many raster data formats will be supported via GDAL.

Documentation: to be added.

Comments and feedback is welcome: valentas.kurauskas@mif.vu.lt.
