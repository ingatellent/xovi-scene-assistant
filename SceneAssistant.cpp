#include "SceneAssistant.hpp"

#include <QColor>
#include <QString>
#include <QRandomGenerator>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDir>
#include <QFile>
#include <QUrl>
#include <QPolygonF>
#include <QTextStream>
#include <QVariantMap>
#include <QQmlEngine>
#include <QGuiApplication>
#include <QWindow>
#include <QQuickWindow>
#include <QQuickItem>
#include <QtQml>
#include <cmath>

#include "rm_SceneLineItem.hpp"


enum PenColor {
    BLACK = 0,
    GRAY = 1,
    WHITE = 2,
    YELLOW = 3,
    GREEN = 4,
    PINK = 5,
    BLUE = 6,
    RED = 7,
    GRAY_OVERLAP = 8,
    ARGB = 9,
    GREEN_2 = 10,
    CYAN = 11,
    MAGENTA = 12,
    YELLOW_2 = 13
};

struct Color {
    int r, g, b, a;
    constexpr Color(int _r, int _g, int _b, int _a) : r(_r), g(_g), b(_b), a(_a) {}
};

constexpr Color getColorFromPalette(const PenColor penColor) {
    switch (penColor) {
        case BLACK: return Color(0, 0, 0, 255);
        case GRAY: return Color(125, 125, 125, 255);
        case WHITE: return Color(255, 255, 255, 255);
        case YELLOW: return Color(255, 255, 99, 255);
        case GREEN: return Color(0, 255, 0, 255);
        case PINK: return Color(255, 20, 147, 255);
        case BLUE: return Color(0, 98, 204, 255);
        case RED: return Color(217, 7, 7, 255);
        case GRAY_OVERLAP: return Color(125, 125, 125, 255);
        case GREEN_2: return Color(145, 218, 113, 255);
        case CYAN: return Color(116, 210, 232, 255);
        case MAGENTA: return Color(192, 127, 210, 255);
        case YELLOW_2: return Color(250, 231, 25, 255);
        default: return Color(0, 0, 0, 255);
    }
}


struct Coordinate {
    float x;
    float y;
};

void SceneAssistant::sleepMs(int ms) { ::usleep(ms*1000); }

Line SceneAssistant::createLine(const QPointF& start, const QPointF& end) {
    QList<LinePoint> linePoints {
        {static_cast<float>(start.x()), static_cast<float>(start.y()), 25, 25, 0, 255},
            {static_cast<float>(end.x()),   static_cast<float>(end.y()),   25, 25, 0, 255}
    };
    QRectF bounds = { std::min(start.x(), end.x()), std::min(start.y(), end.y()),
        std::abs(end.x() - start.x()), std::abs(end.y() - start.y()) };
    return Line::fromPoints(std::move(linePoints), bounds);
}

Line SceneAssistant::createSelectionBox(const QPointF& start, const QPointF& end) {
    QList<LinePoint> linePoints {
        {static_cast<float>(start.x()), static_cast<float>(start.y()), 25, 25, 0, 255},
        {static_cast<float>(start.x()), static_cast<float>(end.y()), 25, 25, 0, 255},
        {static_cast<float>(end.x()),   static_cast<float>(end.y()),   25, 25, 0, 255},
        {static_cast<float>(end.x()), static_cast<float>(start.y()), 25, 25, 0, 255},
        {static_cast<float>(start.x()), static_cast<float>(start.y()), 25, 25, 0, 255}
    };
    QRectF bounds = { std::min(start.x(), end.x()), std::min(start.y(), end.y()),
        std::abs(end.x() - start.x()), std::abs(end.y() - start.y()) };
    return Line::selectionLineFromPoints(std::move(linePoints), bounds);
}

bool SceneAssistant::ensureVtable(QObject* activeController)
{
    if (SceneLineItem::vtable_ptr)
        return true;

    if (!activeController) {
        qWarning() << "[SceneAssistant] ensureVtable called with null controller (exiting)";
        return false;
    }


    QQmlEngine *enginePtr = nullptr;
    const auto windows = QGuiApplication::allWindows();
    for (QWindow *window : windows) {
        QQmlEngine *engine = qmlEngine(window);
        if (engine) {
            enginePtr = engine;
            break;
        }
    }

    if (!enginePtr) {
        qWarning() << "[SceneAssistant] Could not find QmlEngine";
        return false;
    }

    static const char *drawingQml = R"QML(
        import QtQml
        import com.remarkable 1.0 as RM

        QtObject {
            id: root

            function drawStroke(drawObj, controller) {
                if (!controller) {
                    return null;
                }

                controller.clearSelectedItems();
                
                controller.addDrawingLine(drawObj);
		return true;
            }

            function selectStrokes(selectObj, controller) {
                if (!controller) {
                    return null;
                }

                controller.selectWithLine(selectObj);

                return true;
            }

            function cloneAndReturnStrokes(controller) {
                if (!controller) {
                    return null;
                }

                var layer = controller.currentLayer;
                var clonedItems = controller.cloneSelectedItems(layer, 1.0);

                controller.deleteSelectedItems(layer);
                controller.clearSelectedItems();

                return clonedItems;
            }
 
         
        }
    )QML";

    QQmlComponent component(enginePtr);
    component.setData(drawingQml, QUrl());
    
    QObject *viewObj = component.create(enginePtr->rootContext());
    if (!viewObj) {
        const auto errors = component.errors();
        for (const QQmlError &error : errors)
            qWarning() << "[SceneAssistant] QML component error:" << error;
        return false;
    }
    viewObj->setParent(enginePtr);

    Line stroke = this->createLine(QPointF(-5, -10), QPointF(5, -10));
    Line select = this->createSelectionBox(QPointF(-10, -15), QPointF(10,-5));
    
    QVariant drawArg = QVariant::fromValue(stroke);
    QVariant selectArg = QVariant::fromValue(select);
    QVariant controllerArg = QVariant::fromValue(activeController);


    QMetaObject::invokeMethod(viewObj, "drawStroke", 
                             Q_ARG(QVariant, drawArg),
                             Q_ARG(QVariant, controllerArg));

    this->sleepMs(10);

    QMetaObject::invokeMethod(viewObj, "selectStrokes", 
                             Q_ARG(QVariant, selectArg),
                             Q_ARG(QVariant, controllerArg));

    this->sleepMs(10);

    QVariant returnedItems;
    bool success = QMetaObject::invokeMethod(viewObj, "cloneAndReturnStrokes", 
                             Q_RETURN_ARG(QVariant, returnedItems),
                             Q_ARG(QVariant, controllerArg));


    if (success && returnedItems.isValid()) {
        
        if (returnedItems.canConvert<QList<std::shared_ptr<SceneItem>>>()) {
            QList<std::shared_ptr<SceneItem>> items = returnedItems.value<QList<std::shared_ptr<SceneItem>>>();
            
            
            if (!items.empty()) {
                auto itemPtr = items.first();
                if (itemPtr) {
                    auto* item = reinterpret_cast<SceneLineItem*>(itemPtr.get());
                    
                    SceneLineItem::setupVtable(item->vtable);
                }
            }
        }
    }


    if (!SceneLineItem::vtable_ptr) {
        qWarning() << "[SceneAssistant] vtable discovery failed using passed controller";
    }

    viewObj->deleteLater();
    return SceneLineItem::vtable_ptr != nullptr;
}


void SceneAssistant::saveSceneItems(const QList<std::shared_ptr<SceneItem>>& items, const QString& filename) {
    QJsonArray jsonArray;

    for (const auto& itemPtr : items) {
        auto* lineItem = SceneLineItem::tryCast(itemPtr.get());
        if (!lineItem) continue;

        const Line& line = lineItem->line;
        QJsonArray pointsArray;
        for (const auto& pt : line.points) {
            QJsonArray ptArr = { pt.x, pt.y, pt.speed, pt.width, pt.direction, pt.pressure };
            pointsArray.append(ptArr);
        }

        QJsonObject obj;
        obj["points"] = pointsArray;
        obj["rgba"] = static_cast<qint64>(line.rgba);   // store color as hex string
        obj["color"] = line.color;
        obj["bounds"] = QJsonArray{ line.bounds.x(), line.bounds.y(),
            line.bounds.width(), line.bounds.height() };
        obj["tool"] = line.tool;
        obj["maskScale"] = line.maskScale;
        obj["thickness"] = line.thickness;
        jsonArray.append(obj);
    }

    if (jsonArray.isEmpty()) return;

    QJsonDocument doc(jsonArray);
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }

    QFileInfo fi(filename);
    QString svgFilename = fi.path() + "/" + fi.completeBaseName() + ".svg";

    saveSceneItemsAsSvg(items, svgFilename);
}


QList<std::shared_ptr<SceneItem>> SceneAssistant::loadSceneItems(const QString& filename,QObject* activeController) {
    ensureVtable(activeController);
    return loadSceneItems(filename);
}

QList<std::shared_ptr<SceneItem>> SceneAssistant::loadSceneItems(const QString& filename) {
    QList<std::shared_ptr<SceneItem>> items;
    if (!SceneLineItem::vtable_ptr)
        return items;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) return items;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonArray jsonArray = doc.array();

    for (const auto& val : jsonArray) {
        QJsonObject obj = val.toObject();
        QJsonArray pointsArray = obj["points"].toArray();
        QList<LinePoint> linePoints;

        for (const auto& ptVal : pointsArray) {
            QJsonArray ptArr = ptVal.toArray();
            LinePoint pt;
            pt.x = ptArr[0].toDouble();
            pt.y = ptArr[1].toDouble();
            pt.speed = static_cast<unsigned short>(ptArr[2].toInt());
            pt.width = static_cast<unsigned short>(ptArr[3].toInt());
            pt.direction = static_cast<unsigned char>(ptArr[4].toInt());
            pt.pressure = static_cast<unsigned char>(ptArr[5].toInt());
            linePoints.append(pt);
        }

        QJsonArray boundsArr = obj["bounds"].toArray();
        QRectF bounds(boundsArr[0].toDouble(), boundsArr[1].toDouble(),
                boundsArr[2].toDouble(), boundsArr[3].toDouble());

        Line line;
        line.points = std::move(linePoints);
        line.bounds = bounds;

        line.rgba = static_cast<quint32>(obj["rgba"].toInteger(0xFF000000));
        line.color = obj["color"].toInt(0x00);

        line.tool = obj["tool"].toInt(0x17);
        line.maskScale = obj["maskScale"].toDouble(1.0);
        line.thickness = obj["thickness"].toDouble(0.0);
        items.push_back(std::make_shared<SceneLineItem>(SceneLineItem::fromLine(std::move(line))));
    }

    return items;
}

    template <typename Func>
QList<std::shared_ptr<SceneItem>> SceneAssistant::transformSceneItems(
        const QList<std::shared_ptr<SceneItem>>& items,
        Func transform)
{
    QList<std::shared_ptr<SceneItem>> result;

    for (const auto& itemPtr : items) {
        auto* lineItem = SceneLineItem::tryCast(itemPtr.get());

        if (!lineItem) {
            result.push_back(itemPtr);
            continue;
        }

        auto newLineItem = std::make_shared<SceneLineItem>(*lineItem);

        transform(*newLineItem);

        result.push_back(newLineItem);
    }

    return result;
}

Q_INVOKABLE QList<std::shared_ptr<SceneItem>> SceneAssistant::setColorOnSceneItems(
        const QList<std::shared_ptr<SceneItem>>& items, uint colorEnum, quint32 rgba) 
{
    return transformSceneItems(items, [=](SceneLineItem& item) {
            item.line.color = colorEnum;
            item.line.rgba = rgba;
            });
}

Q_INVOKABLE QList<std::shared_ptr<SceneItem>> SceneAssistant::setToolOnSceneItems(
        const QList<std::shared_ptr<SceneItem>>& items, uint toolEnum)
{
    return transformSceneItems(items, [=](SceneLineItem& item) {
            item.line.tool = toolEnum;
            });
}

Q_INVOKABLE QList<std::shared_ptr<SceneItem>> SceneAssistant::increaseThicknessOnSceneItems(
        const QList<std::shared_ptr<SceneItem>>& items)
{
    return transformSceneItems(items, [](SceneLineItem& item) {
            for (auto& pt : item.line.points) {
            pt.width = static_cast<unsigned short>(std::ceil(pt.width * 1.25f));
            }
            });
}

Q_INVOKABLE QList<std::shared_ptr<SceneItem>> SceneAssistant::decreaseThicknessOnSceneItems(
        const QList<std::shared_ptr<SceneItem>>& items)
{
    return transformSceneItems(items, [](SceneLineItem& item) {
            for (auto& pt : item.line.points) {
            pt.width = std::max<unsigned short>(1, pt.width * 0.8f);
            }
            });
}

Q_INVOKABLE QList<std::shared_ptr<SceneItem>> SceneAssistant::setThicknessOnSceneItems(
        const QList<std::shared_ptr<SceneItem>>& items, const quint32 thickness)
{
    return transformSceneItems(items, [=](SceneLineItem& item) {
            for (auto& pt : item.line.points) {
            pt.width = thickness;
            }
            });
}

void SceneAssistant::ensureDirectory(const QString& path) {
    QDir dir(path);
    if (!dir.exists())
        dir.mkpath(".");
}

bool SceneAssistant::deleteFile(const QString& path) {
    return QFile::remove(path);
}


Q_INVOKABLE QVariantMap SceneAssistant::getPenInfoOfFirstItem(
        const QList<std::shared_ptr<SceneItem>>& items)
{
    QVariantMap info;
    if (items.isEmpty()) return info;

    for (auto& itemPtr : items) {

        auto* lineItem = SceneLineItem::tryCast(itemPtr.get());
        if (!lineItem) continue;

        info["currentTool"] = static_cast<int>(lineItem->line.tool);
        info["currentThickness"] = lineItem->line.points.isEmpty()
            ? 0
            : static_cast<int>(lineItem->line.points.first().width);
        info["currentColorCode"] = static_cast<int>(lineItem->line.color);
        info["currentRgb"] = static_cast<quint32>(lineItem->line.rgba);

        return info;
    }
    return info;
}

void SceneAssistant::saveSceneItemsAsSvg(
        const QList<std::shared_ptr<SceneItem>>& items,
        const QString& filename)
{
    if (items.isEmpty()) return;

    QRectF boundingBox;
    bool first = true;
    for (const auto& itemPtr : items) {
        auto* lineItem = SceneLineItem::tryCast(itemPtr.get());
        if (!lineItem) continue;

        if (first) {
            boundingBox = lineItem->line.bounds;
            first = false;
        } else {
            boundingBox = boundingBox.united(lineItem->line.bounds);
        }
    }

    double offsetX = boundingBox.left();
    double offsetY = boundingBox.top();
    double width = boundingBox.width();
    double height = boundingBox.height();

    QString svg;
    svg += QString("<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"%1\" height=\"%2\" viewBox=\"0 0 %1 %2\">\n")
        .arg(width).arg(height);

    for (const auto& itemPtr : items) {
        auto* lineItem = SceneLineItem::tryCast(itemPtr.get());
        if (!lineItem) continue;

        const Line& line = lineItem->line;

        // Map color code to actual RGBA
        Color c(0,0,0,255);
        if (line.color == 9) {  // ARGB mode
            c = Color((line.rgba >> 16) & 0xFF, (line.rgba >> 8) & 0xFF, line.rgba & 0xFF, (line.rgba >> 24) & 0xFF);
        } else {
            c = getColorFromPalette(static_cast<PenColor>(line.color));
        }

        QString strokeColor = QString("rgb(%1,%2,%3)").arg(c.r).arg(c.g).arg(c.b);
        double strokeOpacity = c.a / 255.0;

        // Build path
        QString pathData;
        bool firstPoint = true;
        for (const auto& pt : line.points) {
            double x = pt.x - offsetX;
            double y = pt.y - offsetY;

            if (firstPoint) {
                pathData += QString("M %1 %2 ").arg(x).arg(y);
                firstPoint = false;
            } else {
                pathData += QString("L %1 %2 ").arg(x).arg(y);
            }
        }

        svg += QString("<path d=\"%1\" fill=\"none\" stroke=\"%2\" stroke-width=\"%3\" stroke-opacity=\"%4\" stroke-linecap=\"round\" stroke-linejoin=\"round\" />\n")
            .arg(pathData)
            .arg(strokeColor)
            .arg(line.points.first().width)
            .arg(strokeOpacity, 0, 'f', 2);
    }

    svg += "</svg>\n";

    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(svg.toUtf8());
        file.close();
    }
}
