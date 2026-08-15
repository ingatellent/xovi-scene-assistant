# Define the project name
TEMPLATE = lib
TARGET = scene-assistant
CONFIG += shared plugin no_plugin_name_prefix
QMAKE_LFLAGS += -Wl,--no-undefined

# Configure build directories
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
UI_DIR = build/ui
XOVI_DIR = build/xovi

xoviextension.target = build/xovi/xovi.c
xoviextension.commands = mkdir -p $$XOVI_DIR && python3 $$(XOVI_REPO)/util/xovigen.py -o $$XOVI_DIR/xovi.c -H $$XOVI_DIR/xovi.h scene-assistant.xovi
xoviextension.depends = scene-assistant.xovi

QMAKE_EXTRA_TARGETS += xoviextension
PRE_TARGETDEPS += $$XOVI_DIR/xovi.c

# Define the Qt modules required
QT += quick qml

# Define the C++ standard version
CONFIG += c++20

# Specify the source files
SOURCES += \
    main.cpp $$XOVI_DIR/xovi.c \
    SceneAssistant.cpp \
    rm_Line.cpp rm_SceneLineItem.cpp

HEADERS += SceneAssistant.hpp
INCLUDEPATH += $$XOVI_DIR

QMAKE_CXXFLAGS += -fPIC -Werror -Wno-invalid-offsetof

# QMAKE_CXX = aarch64-remarkable-linux-g++

