
#should be cleaned before compiling on a new architecture

CFLAGS = -g -Wall
#-g
#-g -Wall

all: libshp libhfz margo convert_shp



margo: main.o MImage.o MProcessor.o MFilter.o MRadialFilter.o utils.o MCoordDB.o MObject.o MGeom.o MConstraints.o ../libhfz/libhfz_margo.o 
	g++  $(CFLAGS) ../libhfz/libhfz_margo.o  main.o MImage.o MFilter.o MProcessor.o utils.o MRadialFilter.o MCoordDB.o MObject.o MGeom.o MConstraints.o -o ../bin/margo -lz

#compiled without any problems with mingw. 1) install mingw as they ask it in their webpage 2) download libz from the mingw tool 3) compile as on linux
#do I need -L??
#margo.exe: main.o MImage.o MProcessor.o MFilter.o MRadialFilter.o utils.o MCoordDB.o MObject.o MGeom.o MConstraints.o ..\libhfz\libhfz_margo.o
#	g++  $(CFLAGS) -Lc:\code\arch\arch\src\margo ..\libhfz\libhfz_margo.o  main.o MImage.o MFilter.o MProcessor.o utils.o MRadialFilter.o MCoordDB.o MObject.o MGeom.o MConstraints.o -o margo.exe -lz


convert_shp: convert_shp.o utils.o MImage.o ../libhfz/libhfz_margo.o
	g++ $(CFLAGS) ../libhfz/libhfz_margo.o  utils.o MImage.o convert_shp.o ../shapelib/shapelib-1.3.0/libshp.a -o ../bin/convert_shp -lz


libshp: ../shapelib/shapelib-1.3.0/libshp.a

../shapelib/shapelib-1.3.0/libshp.a:
	cd ../shapelib/shapelib-1.3.0/; $(MAKE)

libhfz: ../libhfz/libhfz_margo.o

../libhfz/libhfz_margo.o:
	cd ../libhfz; g++ -c $(CFLAGS) libhfz_margo.cpp


convert_shp.o: convert_shp.cpp
	g++ $(CFLAGS) -c convert_shp.cpp

main.o: main.cpp
	g++ $(CFLAGS) -c main.cpp

MProcessor.o: MProcessor.h MProcessor.cpp
	g++ $(CFLAGS) -c MProcessor.cpp

MFilter.o: MFilter.h MFilter.cpp
	g++ $(CFLAGS) -c MFilter.cpp

MImage.o: MImage.h MImage.cpp
	g++ $(CFLAGS) -c MImage.cpp

MRadialFilter.o: MRadialFilter.h MRadialFilter.cpp
	g++ $(CFLAGS) -c MRadialFilter.cpp

utils.o: utils.h utils.cpp
	g++ $(CFLAGS) -c utils.cpp

MCoordDB.o: MCoordDB.h MCoordDB.cpp
	g++ $(CFLAGS) -c MCoordDB.cpp

MObject.o: MObject.h MObject.cpp
	g++ $(CFLAGS) -c MObject.cpp

MGeom.o: MGeom.h MGeom.cpp
	g++ $(CFLAGS) -c MGeom.cpp

MConstraints.o: MConstraints.h MConstraints.cpp
	g++ $(CFLAGS) -c MConstraints.cpp



clean:
	rm -f *.o margo convert_shp

clean_win:
	del *.o margo.exe
