#include <QApplication>
#include <QQmlApplicationEngine>
#include "facefeaturedetector.h"
#include "glperspectivescene.h"

int main(int argc, char *argv[])
{
//    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    int imageWidth = 240, imageHeight = 320;

    QSurfaceFormat format;
    format.setSamples(16);
    format.setDepthBufferSize(24);
    QSurfaceFormat::setDefaultFormat(format);

    app.setApplicationName("Head Coupled Display");
    app.setApplicationVersion("1.0");

    FaceFeatureDetector *detector = new FaceFeatureDetector(imageWidth, imageHeight);

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("w", imageWidth);
    engine.rootContext()->setContextProperty("h", imageHeight);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    detector->start(engine.rootObjects().first()); //start processing

    glPerspectiveScene scene(detector);
    scene.show();

    return app.exec();
}
