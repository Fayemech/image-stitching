#-------------------------------------------------
#
# Project created by QtCreator 2017-12-24T17:00:22
#
#-------------------------------------------------

QT       += core gui widgets
TARGET    = RUIStitchingApp
TEMPLATE  = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
           laumemoryobject.cpp

HEADERS += laumemoryobject.h

unix:macx {
    CONFIG         += c++11
    QMAKE_CXXFLAGS += -msse2 -msse3 -mssse3 -msse4.1
    INCLUDEPATH    += /usr/local/include/opencv4 /usr/local/include /usr/local/include/eigen3
    DEPENDPATH     += /usr/local/include/opencv4 /usr/local/include /usr/local/include/eigen3
    LIBS           += -L/usr/local/lib -lopencv_core -lopencv_objdetect -lopencv_imgproc -lopencv_calib3d -lopencv_highgui -lopencv_ml -lopencv_face -lopencv_features2d -lopencv_xfeatures2d -lopencv_imgcodecs
    LIBS           += /usr/local/lib/libtiff.5.dylib
}

unix:!macx {
    CONFIG         += c++11
    QMAKE_CXXFLAGS += -msse2 -msse3 -mssse3 -msse4.1
    INCLUDEPATH    += /usr/local/opt/opencv/include /usr/include /usr/include/eigen3
    DEPENDPATH     += /usr/local/opt/opencv/include /usr/include /usr/include/eigen3
    LIBS           += -L/usr/local/lib -lopencv_core -lopencv_objdetect -lopencv_imgproc -lopencv_calib3d -lopencv_highgui -lopencv_ml
    LIBS           += -ltiff
}

win32 {
    INCLUDEPATH   += $$quote(C:/usr/opencv/build/include) $$quote(C:/usr/include)
    DEPENDPATH    += $$quote(C:/usr/opencv/build/include) $$quote(C:/usr/include)
    LIBS          += -L$$quote(C:/usr/opencv/build/x64/vc12/lib)
    LIBS          += -L$$quote(C:/usr/lib) -llibtiff_i -lopengl32
    CONFIG(release, debug|release): LIBS += -lopencv_world310
    CONFIG(debug, debug|release):   LIBS += -lopencv_world310d
}
