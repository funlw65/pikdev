# #####################################################################
# Automatically generated by qmake (2.01a) sam. sept. 17 22:52:59 2011
# #####################################################################
pkp_version = 1.4.2
TEMPLATE = app

DEPENDPATH += .
INCLUDEPATH += .
CONFIG += warn_on \
    build_all
CONFIG -= qt
unix { 
    OBJECTS_DIR = ./objects
    TARGET = pkp-$${pkp_version}
    count(PREFIX,1):target.path = $$PREFIX/bin
    isEmpty(PREFIX) { 
        PREFIX = /usr/share/pkp/$${pkp_version}
        target.path = /usr/bin/
    }
    INSTALLS += target
}


SOURCES += pkp.cc

# common files
include(pikdev-common.pri)
