#include "glperspectivescene.h"

glPerspectiveScene::glPerspectiveScene(FaceFeatureDetector *detector, QWidget *parent) :
    QOpenGLWidget(parent),
    featureDetector(detector),
    arrayBuffer(QOpenGLBuffer::VertexBuffer),
    indexBuffer(QOpenGLBuffer::IndexBuffer),
    wallArrayBuffer(QOpenGLBuffer::VertexBuffer),
    wallIndexBuffer(QOpenGLBuffer::IndexBuffer),
    cameraPosition(QVector3D(0.0f, 0.0f, distance))
{
    sceneWidth = 3.0;
    sceneHeight = sceneWidth / aspect;
}

glPerspectiveScene::~glPerspectiveScene()
{
    makeCurrent();

    arrayBuffer.destroy();
    indexBuffer.destroy();
    wallArrayBuffer.destroy();
    wallIndexBuffer.destroy();

    delete skyTexture;
    delete cubeTexture;

    doneCurrent();
}

void glPerspectiveScene::timerEvent(QTimerEvent *)
{
    update();
}

void glPerspectiveScene::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0, 0, 0, 1);

    initShaders();
    loadTextures();

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);
//    glEnable(GL_MULTISAMPLE);

    // Generate all VBOs
    arrayBuffer.create();
    indexBuffer.create();
    wallArrayBuffer.create();
    wallIndexBuffer.create();

    // Use QBasicTimer because its faster than QTimer
    timer.start(12, this);
}

void glPerspectiveScene::initShaders()
{
    // Compile vertex shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.glsl")) {
        qDebug() <<  "\nVERTEX SHADER ERROR\n";
        close();
    }

    // Compile fragment shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.glsl")) {
        qDebug() <<  "\nFRAGMENT SHADER ERROR\n";
        close();
    }

    // Link shader pipeline
    if (!program.link()) {
        qDebug() <<  "\nLINKING SHADERS ERROR\n";
        close();
    }

    // Bind shader pipeline for use
    if (!program.bind()) {
        qDebug() <<  "\nBINDING ERROR\n";
        close();
    }
}

void glPerspectiveScene::loadTextures()
{
    //cube textures
//    cubeTexture = new QOpenGLTexture(QImage(":/rubix_cube_texture.jpg"));
    cubeTexture = new QOpenGLTexture(QOpenGLTexture::Target2D);
    cubeTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    cubeTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    cubeTexture->setWrapMode(QOpenGLTexture::Repeat);
    cubeTexture->setData(QImage(":/rubix_cube_texture.jpg"));

    //skybox textures
//    skyTexture = new QOpenGLTexture(QImage(":/gridpat.png"));
    skyTexture = new QOpenGLTexture(QOpenGLTexture::Target2D);
    skyTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    skyTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    skyTexture->setWrapMode(QOpenGLTexture::Repeat);
    skyTexture->setData(QImage(":/gridpat.png"));
}

void glPerspectiveScene::resizeGL(int w, int h)
{
    // Recalculate aspect ratio
    aspect = qreal(w) / qreal(h ? h : 1);

    sceneWidth = 3.0;
    sceneHeight = sceneWidth / aspect;

}

void glPerspectiveScene::paintGL()
{
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //reset matrices
    viewFrustrum.setToIdentity();
    transform.setToIdentity();

    float z = 0.0f;
    const QVector3D pa = QVector3D(-sceneWidth, -sceneHeight, z);
    const QVector3D pb = QVector3D(sceneWidth, -sceneHeight, z);
    const QVector3D pc = QVector3D(-sceneWidth, sceneHeight, z);

    determineCameraPosition();

    viewFrustrum = projFrustum(pa, pb, pc, cameraPosition, zNear, zFar);
    program.setUniformValue("viewFrustrum", viewFrustrum);

    transform.scale(QVector3D(sceneWidth, sceneHeight, 5.0f));
    program.setUniformValue("transform", transform);

    glCullFace(GL_FRONT);
    drawSkyBox();

    transform.setToIdentity();
    transform.rotate((25.0f), QVector3D(1.0f, 0.0f, 0.0f));
    transform.rotate((45.0f), QVector3D(0.0f, 1.0f, 0.0f));
    program.setUniformValue("transform", transform);

    glCullFace(GL_BACK);
    drawCube();
}

void glPerspectiveScene::determineCameraPosition()
{
    QRect face = featureDetector->getFaceRect();
    QRect leye = featureDetector->getLeftEyeRect();
    QRect reye = featureDetector->getRightEyeRect();

    if (face.x() == 0 || reye.x() == 0 || leye.x() == 0)
        return;

    QSize imageSize = featureDetector->getImageSize();
    float distFromCamera = featureDetector->getDistanceFromCamera();

    int centerEyesX = (leye.x() + reye.right()) / 2;
    int centerEyesY = leye.y() + leye.height() / 2;

    float ratio = 0.2f;
    int x = centerEyesX - imageSize.width() / 2;
    int y = centerEyesY - imageSize.height() / 2;

    cameraPosition.setX(x * ratio);
    cameraPosition.setY(-y * ratio);
    cameraPosition.setZ(distFromCamera);
}

void glPerspectiveScene::initAttributes()
{
    // Offset for position
    int offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program.attributeLocation("aPosition");
    program.enableAttributeArray(vertexLocation);
    program.setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program.attributeLocation("aTexCoord");
    program.enableAttributeArray(texcoordLocation);
    program.setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));
}

void glPerspectiveScene::drawCube()
{
    arrayBuffer.bind();
    arrayBuffer.allocate(vertices, vertexArraySize * sizeof(VertexData));

    indexBuffer.bind();
    indexBuffer.allocate(indices, indexArraySize * sizeof(GLushort));

    initAttributes();

    cubeTexture->bind();

    // Draw cube geometry using indices from VBO 1
    glDrawElements(GL_TRIANGLE_STRIP, indexArraySize, GL_UNSIGNED_SHORT, nullptr);
}

void glPerspectiveScene::drawSkyBox()
{
    wallArrayBuffer.bind();
    wallArrayBuffer.allocate(backVertices, wallArrayBufferSize * sizeof(VertexData));

    wallIndexBuffer.bind();
    wallIndexBuffer.allocate(backIndices, wallIndexBufferSize * sizeof(GLushort));

    initAttributes();

    skyTexture->bind();

    // Draw cube geometry using indices from VBO 1
    glDrawElements(GL_TRIANGLE_STRIP, wallIndexBufferSize, GL_UNSIGNED_SHORT, nullptr);
}

QMatrix4x4 glPerspectiveScene::projFrustum(
        const QVector3D pa,
        const QVector3D pb,
        const QVector3D pc,
        const QVector3D pe,
        float n, float f)
{
    QVector3D va, vb, vc;
    QVector3D vr, vu, vn;

    QMatrix4x4 frustum;

    float l, r, b, t, d, M[16];

    // Compute an orthonormal basis for the screen.
    vr = pb - pa; //subtract(vr, pb, pa);
    vu = pc - pa; //subtract(vu, pc, pa);
    vr.normalize(); //normalize(vr);
    vu.normalize(); //normalize(vu);
    vn = QVector3D::crossProduct(vr, vu); //cross_product(vn, vr, vu);
    vn.normalize(); //normalize(vn);

    // Compute the screen corner vectors.
    va = pa - pe; //subtract(va, pa, pe);
    vb = pb - pe; //subtract(vb, pb, pe);
    vc = pc - pe; //subtract(vc, pc, pe);

    // Find the distance from the eye to screen plane.
    d = -QVector3D::dotProduct(va, vn); //d = -dot_product(va, vn);

    // Find the extent of the perpendicular projection.
    l = QVector3D::dotProduct(vr, va) * n / d; //    l = dot_product(vr, va) * n / d;
    r = QVector3D::dotProduct(vr, vb) * n / d; //    r = dot_product(vr, vb) * n / d;
    b = QVector3D::dotProduct(vu, va) * n / d; //    b = dot_product(vu, va) * n / d;
    t = QVector3D::dotProduct(vu, vc) * n / d; //    t = dot_product(vu, vc) * n / d;

    // Rotate the projection to be non-perpendicular.
    memset(M, 0, 16 * sizeof (float));
    M[0] = vr[0]; M[4] = vr[1]; M[ 8] = vr[2];
    M[1] = vu[0]; M[5] = vu[1]; M[ 9] = vu[2];
    M[2] = vn[0]; M[6] = vn[1]; M[10] = vn[2];
    M[15] = 1.0f;

    frustum.setToIdentity(); // glLoadIdentity();
    frustum.frustum(l, r, b, t, n, f); //    glFrustum(l, r, b, t, n, f);
    frustum = frustum * QMatrix4x4(M); //    glMultMatrixf(M);

    // Move the apex of the frustum to the origin.
    frustum.translate(-pe); //    glTranslatef(-pe[0], -pe[1], -pe[2]);

    return frustum;
}
