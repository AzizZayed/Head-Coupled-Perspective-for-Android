#ifndef GLPERSPECTIVESCENE_H
#define GLPERSPECTIVESCENE_H

#include <QOpenGLWindow>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QVector3D>
#include <QBasicTimer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include "facefeaturedetector.h"

class glPerspectiveScene : public QOpenGLWindow, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit glPerspectiveScene(FaceFeatureDetector *detector, QOpenGLWindow::UpdateBehavior updateBehavior = NoPartialUpdate, QWindow *parent = nullptr);
    ~glPerspectiveScene() override;

protected:
    void timerEvent(QTimerEvent *e) override;

    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    void initShaders();
    void loadTextures();
    void drawCube();
    void drawSkyBox();
    void initAttributes();
    void determineCameraPosition();
    QMatrix4x4 projFrustum(
            const QVector3D pa,
            const QVector3D pb,
            const QVector3D pc,
            const QVector3D pe,
            float n, float f);

private:
    FaceFeatureDetector *featureDetector;

    struct VertexData
    {
        QVector3D position;
        QVector2D textureCoord;
    };

    QBasicTimer timer;

    QOpenGLShaderProgram program;

    VertexData vertices[24] =
    {
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(0.0f, 0.0f)},  // v0
        {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D(0.33f, 0.0f)}, // v1
        {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(0.0f, 0.5f)},  // v2
        {QVector3D( 1.0f,  1.0f,  1.0f), QVector2D(0.33f, 0.5f)}, // v3

        // Vertex data for face 1
        {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D( 0.0f, 0.5f)}, // v4
        {QVector3D( 1.0f, -1.0f, -1.0f), QVector2D(0.33f, 0.5f)}, // v5
        {QVector3D( 1.0f,  1.0f,  1.0f), QVector2D(0.0f, 1.0f)},  // v6
        {QVector3D( 1.0f,  1.0f, -1.0f), QVector2D(0.33f, 1.0f)}, // v7

        // Vertex data for face 2
        {QVector3D( 1.0f, -1.0f, -1.0f), QVector2D(0.66f, 0.5f)}, // v8
        {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D(1.0f, 0.5f)},  // v9
        {QVector3D( 1.0f,  1.0f, -1.0f), QVector2D(0.66f, 1.0f)}, // v10
        {QVector3D(-1.0f,  1.0f, -1.0f), QVector2D(1.0f, 1.0f)},  // v11

        // Vertex data for face 3
        {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D(0.66f, 0.0f)}, // v12
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(1.0f, 0.0f)},  // v13
        {QVector3D(-1.0f,  1.0f, -1.0f), QVector2D(0.66f, 0.5f)}, // v14
        {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(1.0f, 0.5f)},  // v15

        // Vertex data for face 4
        {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D(0.33f, 0.0f)}, // v16
        {QVector3D( 1.0f, -1.0f, -1.0f), QVector2D(0.66f, 0.0f)}, // v17
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(0.33f, 0.5f)}, // v18
        {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D(0.66f, 0.5f)}, // v19

        // Vertex data for face 5
        {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(0.33f, 0.5f)}, // v20
        {QVector3D( 1.0f,  1.0f,  1.0f), QVector2D(0.66f, 0.5f)}, // v21
        {QVector3D(-1.0f,  1.0f, -1.0f), QVector2D(0.33f, 1.0f)}, // v22
        {QVector3D( 1.0f,  1.0f, -1.0f), QVector2D(0.66f, 1.0f)}  // v23
    };
    GLushort indices[34] =
    {
        0,  1,  2,  3,  3,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
        4,  4,  5,  6,  7,  7, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
        8,  8,  9, 10, 11, 11, // Face 2 - triangle strip ( v8,  v9, v10, v11)
        12, 12, 13, 14, 15, 15, // Face 3 - triangle strip (v12, v13, v14, v15)
        16, 16, 17, 18, 19, 19, // Face 4 - triangle strip (v16, v17, v18, v19)
        20, 20, 21, 22, 23      // Face 5 - triangle strip (v20, v21, v22, v23)
    };
    QOpenGLTexture *cubeTexture;
    QOpenGLBuffer arrayBuffer;
    QOpenGLBuffer indexBuffer;
    int indexArraySize = 34, vertexArraySize = 24;

    VertexData wallVertices[20] =
    {
        // Vertex data for face 1
        {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D(1.0f, 1.0f)}, // v4
        {QVector3D( 1.0f, -1.0f, -1.0f), QVector2D(0.0f, 1.0f)}, // v5
        {QVector3D( 1.0f,  1.0f,  1.0f), QVector2D(1.0f, 0.0f)},  // v6
        {QVector3D( 1.0f,  1.0f, -1.0f), QVector2D(0.0f, 0.0f)}, // v7

        // Vertex data for face 2
        {QVector3D( 1.0f, -1.0f, -1.0f), QVector2D(1.0f, 1.0f)}, // v8
        {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D(0.0f, 1.0f)},  // v9
        {QVector3D( 1.0f,  1.0f, -1.0f), QVector2D(1.0f, 0.0f)}, // v10
        {QVector3D(-1.0f,  1.0f, -1.0f), QVector2D(0.0f, 0.0f)},  // v11

        // Vertex data for face 3
        {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D(1.0f, 1.0f)}, // v12
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(0.0f, 1.0f)},  // v13
        {QVector3D(-1.0f,  1.0f, -1.0f), QVector2D(1.0f, 0.0f)}, // v14
        {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(0.0f, 0.0f)},  // v15

        // Vertex data for face 4
        {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D(1.0f, 1.0f)}, // v16
        {QVector3D( 1.0f, -1.0f, -1.0f), QVector2D(0.0f, 1.0f)}, // v17
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(1.0f, 0.0f)}, // v18
        {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D(0.0f, 0.0f)}, // v19

        // Vertex data for face 5
        {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(1.0f, 1.0f)}, // v20
        {QVector3D( 1.0f,  1.0f,  1.0f), QVector2D(0.0f, 1.0f)}, // v21
        {QVector3D(-1.0f,  1.0f, -1.0f), QVector2D(1.0f, 0.0f)}, // v22
        {QVector3D( 1.0f,  1.0f, -1.0f), QVector2D(0.0f, 0.0f)}  // v23
    };
    GLushort wallIndices[29] =
    {
        0,  1,  2,  3,  3,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
        4,  4,  5,  6,  7,  7, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
        8,  8,  9, 10, 11, 11, // Face 2 - triangle strip ( v8,  v9, v10, v11)
        12, 12, 13, 14, 15, 15, // Face 3 - triangle strip (v12, v13, v14, v15)
        16, 16, 17, 18, 19, 19, // Face 4 - triangle strip (v16, v17, v18, v19)
    };
    QOpenGLTexture *skyTexture;
    QOpenGLBuffer wallArrayBuffer;
    QOpenGLBuffer wallIndexBuffer;
    int wallIndexBufferSize = 29, wallArrayBufferSize = 20;

    QMatrix4x4 viewFrustrum;
    QMatrix4x4 transform;

    float distance = 15.0f;
    QVector3D cameraPosition;
    float zNear = 0.1f, zFar = distance + 20.0f;
    float aspect;

    float sceneWidth, sceneHeight;
};

#endif // GLPERSPECTIVESCENE_H
