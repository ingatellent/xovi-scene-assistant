#include <QQmlApplicationEngine>
#include <QTimer>
#include <QCoreApplication>
#include "SceneAssistant.hpp"
#include <stdio.h>
#include "xovi.h"

extern "C" void _xovi_construct() {
    QTimer::singleShot(0, []() {
        qmlRegisterSingletonInstance<SceneAssistant>(
            "dk.ingatellent.SceneAssistant", 1, 0, "SceneAssistant", new SceneAssistant()
        );
    });
}

