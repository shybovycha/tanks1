all:
	g++ -o test1 *.cpp -I/usr/include/OGRE/Overlay -lOgreOverlay -lboost_system `pkg-config --cflags --libs OGRE OIS`
