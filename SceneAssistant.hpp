#pragma once

#include <QList>
#include <QObject>
#include <QPointF>
#include <QVariant>
#include "rm_Line.hpp"
#include "rm_SceneItem.hpp"

class SceneAssistant : public QObject
{
    Q_OBJECT
    public:
        explicit SceneAssistant(QObject *parent = nullptr) : QObject(parent) {}

	Q_INVOKABLE bool ensureVtable(QObject* activeController);
        Q_INVOKABLE Line createLine(const QPointF& start, const QPointF& end);
        Q_INVOKABLE Line createSelectionBox(const QPointF& start, const QPointF& end);

        Q_INVOKABLE void saveSceneItems(const QList<std::shared_ptr<SceneItem>>& items, const QString& filename);
        Q_INVOKABLE QList<std::shared_ptr<SceneItem>> loadSceneItems(const QString& filename, QObject* activeController);
        Q_INVOKABLE QList<std::shared_ptr<SceneItem>> loadSceneItems(const QString& filename);
        Q_INVOKABLE QList<std::shared_ptr<SceneItem>> setColorOnSceneItems(const QList<std::shared_ptr<SceneItem>>& items, const uint colorEnum, quint32 rgba);
        Q_INVOKABLE QList<std::shared_ptr<SceneItem>> setToolOnSceneItems(const QList<std::shared_ptr<SceneItem>>& items, const uint toolEnum);
        Q_INVOKABLE QList<std::shared_ptr<SceneItem>> setThicknessOnSceneItems(const QList<std::shared_ptr<SceneItem>>& items, const quint32 thickness);
        Q_INVOKABLE QList<std::shared_ptr<SceneItem>> increaseThicknessOnSceneItems(const QList<std::shared_ptr<SceneItem>>& items);
        Q_INVOKABLE QList<std::shared_ptr<SceneItem>> decreaseThicknessOnSceneItems(const QList<std::shared_ptr<SceneItem>>& items);

        Q_INVOKABLE void ensureDirectory(const QString& path);
        Q_INVOKABLE bool deleteFile(const QString& path);
        Q_INVOKABLE QVariantMap getPenInfoOfFirstItem(const QList<std::shared_ptr<SceneItem>>& items);
    private:
        void saveSceneItemsAsSvg( const QList<std::shared_ptr<SceneItem>>& items, const QString& filename);
        void sleepMs(int ms);
        template <typename Func>
        QList<std::shared_ptr<SceneItem>> transformSceneItems(
            const QList<std::shared_ptr<SceneItem>>& items,
            Func transform);
};
