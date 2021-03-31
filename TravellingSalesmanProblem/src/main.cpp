#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QVector2D>
#include <QQmlContext>

#include "graphdrawer.h"
#include "fruchtermanreingold.h"
#include "graphlayout.h"
#include "backend.h"
#include "treedrawer.h"
#include "pdfwriter.h"
#include "staticthreadpool.h"
#include "profilerbackend.h"

int main(int argc, char *argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    qmlRegisterType<GraphDrawer>("GraphDrawer", 1, 0, "GraphDrawer");
    qmlRegisterType<GraphLayout>("GraphDrawer", 1, 0, "GraphLayout");
    qmlRegisterType<FruchtermanReingold>("GraphDrawer", 1, 0, "FruchtermanReingold");
    qmlRegisterType<TreeDrawer>("BranchAndBound", 1, 0, "TreeDrawer");
    qmlRegisterType<PdfWriter>("Utils", 1, 0, "PdfWriter");
    StaticThreadPool pool;
    Backend *backend = new Backend();
    ProfilerBackend *profilerBackend = new ProfilerBackend();
    engine.rootContext()->setContextProperty("Backend", backend);
    engine.rootContext()->setContextProperty("ProfilerBackend", profilerBackend);

    engine.load(url);

    return app.exec();
}
