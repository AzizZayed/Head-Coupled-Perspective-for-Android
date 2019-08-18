#include "facefeaturedetector.h"

/**
 * @brief FaceFeatureDetector::FaceFeatureDetector : constuctor
 * @param parent
 */
FaceFeatureDetector::FaceFeatureDetector(int imgWidth, int imgHeight, QObject *parent) :
    QObject(parent),
    imgSize(QSize(imgWidth, imgHeight))
{

    //Load face cascade classifier
    QFile xmlFace(":/haarcascade_frontalface_alt.xml");
    if (xmlFace.open(QFile::ReadOnly | QFile::Text)) {
        QTemporaryFile temp;
        if (temp.open()) {
            temp.write(xmlFace.readAll());
            temp.close();
            if (faceClassifier.load(temp.fileName().toStdString()))
                qDebug() << "Successfully loaded Face classifier!";
            else
                qDebug() << "Could not load Face classifier.";
        } else
            qDebug() << "Can't open Face temp file.";
    }

    //Load eye cascade classifier
    QFile xmlEye(":/haarcascade_eye.xml");
    if (xmlEye.open(QFile::ReadOnly | QFile::Text)) {
        QTemporaryFile temp;
        if (temp.open()) {
            temp.write(xmlEye.readAll());
            temp.close();
            if (eyeClassifier.load(temp.fileName().toStdString()))
                qDebug() << "Successfully loaded Eye classifier!";
            else
                qDebug() << "Could not load Eye classifier.";
        } else
            qDebug() << "Can't open Eye temp file.";
    }
}

/**
 * @brief FaceFeatureDetector::start : initialize the feature detector
 * @param qmlObj : to be able to interact with the QML side of the project
 */
void FaceFeatureDetector::start(QObject *qmlObj) {
    qmlObject = qmlObj;

    //Connect the QML frameReady SIGNAL to our frameReady SLOT
    connect(qmlObject, SIGNAL(frameReady(QVariant)), this, SLOT(frameReady(QVariant)));

    grab(); //Do the first grab to test for faces and eyes
}

/**
 * @brief FaceFeatureDetector::grab : call the function in QML
 */
void FaceFeatureDetector::grab() {
    QMetaObject::invokeMethod(qmlObject, "grab");
}

/**
 * @brief FaceFeatureDetector::frameReady:  extract frame from QML grab and process for faces and eyes
 * @param frameVariant
 */
void FaceFeatureDetector::frameReady(const QVariant &frameVariant) {
    //extract image from grab
    QQuickItemGrabResult *grab_result = qvariant_cast<QQuickItemGrabResult*>(frameVariant); //Cast from QVariant
    QImage frame = grab_result->image(); //Get the QImage
    grab_result->deleteLater(); //Release QQuickItemGrabResult

    //convert QImage to cv::Mat
    cv::Mat img(frame.height(), frame.width(), CV_8UC4, (void *)frame.constBits(), frame.bytesPerLine());
    cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);

    face = QRect(0, 0, 0, 0);
    rightEye = QRect(0, 0, 0, 0);
    leftEye = QRect(0, 0, 0, 0);

    //detecting faces and drawing:
    qDebug() << "detecting... ";
    std::vector<cv::Rect> cvfaces, cvfaceeyes;
    faceClassifier.detectMultiScale(img, cvfaces, 1.1, 3, cv::CASCADE_DO_ROUGH_SEARCH | cv::CASCADE_FIND_BIGGEST_OBJECT); //magic

    //process faces and eyes
    if (cvfaces.size() >= 1) {
        cv::Rect cvface = cvfaces[0];
        face = QRect(cvface.x, cvface.y, cvface.width, cvface.height);
        cv::Mat faceImg = img(cvface);
        eyeClassifier.detectMultiScale(faceImg, cvfaceeyes, 1.1, 3, cv::CASCADE_DO_ROUGH_SEARCH); //more magic

        if (cvfaceeyes.size() >= 2) {
            std::sort(cvfaceeyes.begin(), cvfaceeyes.end(), [](const cv::Rect& a, const cv::Rect& b) {
                return (a.y < b.y);
            });

            cv::Rect eye1 = cvfaceeyes[0];
            cv::Rect eye2 = cvfaceeyes[1];
            if (eye1.x < eye2.x){
                leftEye = QRect(eye1.x + cvface.x, eye1.y + cvface.y, eye1.width, eye1.height);
                rightEye = QRect(eye2.x + cvface.x, eye2.y + cvface.y, eye2.width, eye2.height);
            } else {
                leftEye = QRect(eye2.x + cvface.x, eye2.y + cvface.y, eye2.width, eye2.height);
                rightEye = QRect(eye1.x + cvface.x, eye1.y + cvface.y, eye1.width, eye1.height);
            }
            calculateDistance();
        }
    }

    qDebug() << "face: " << face;
    qDebug() << "right eye: " << rightEye;
    qDebug() << "left eye: " << leftEye;

    grab(); //Do the next frame grab
}

/**
 * @brief FaceFeatureDetector::calculateDistance: Rough estimation of the distance between the camera and the face
 */
void FaceFeatureDetector::calculateDistance() {

    /*
     * Estimation model:
     * l = 1470 / d
     *
     * l : distance between face and camera
     * d : distance between both eyes
     */

    distanceFromCamera = 1470.0f / (rightEye.x() - leftEye.x());
//    qDebug() << "Distance from cam in cm: " << distanceFromCamera;

}

/**
 * @brief FaceFeatureDetector::getImageSize
 * @return : the size of the img in qml
 */
QSize FaceFeatureDetector::getImageSize() {
    return imgSize;
}

/**
 * @brief FaceFeatureDetector::getFaceRect
 * @return : return the face info
 */
QRect FaceFeatureDetector::getFaceRect() {
    return face;
}

/**
 * @brief FaceFeatureDetector::getRightEyeRect :
 * @return : return the right rye info
 */
QRect FaceFeatureDetector::getRightEyeRect() {
    return rightEye;
}

/**
 * @brief FaceFeatureDetector::getLeftEyeRect :
 * @return : return the left eye info
 */
QRect FaceFeatureDetector::getLeftEyeRect() {
    return leftEye;
}

/**
 * @brief FaceFeatureDetector::getDistanceFromCamera
 * @return distance from camera
 */
float FaceFeatureDetector::getDistanceFromCamera() {
    return distanceFromCamera;
}
