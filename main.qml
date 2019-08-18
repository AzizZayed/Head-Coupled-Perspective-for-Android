import QtQuick 2.12
import QtQuick.Window 2.12
import QtMultimedia 5.12
import QtQuick.Controls 2.12

Window {
    visible: true
    color: "white"
    width: 1080 //change for your screen
    height: 2280 //change for your screen

    signal frameReady(var frame)

    Camera
    {
        id: camera
        captureMode: Camera.CaptureViewfinder
        position: Camera.FrontFace

        // camera is rotated FOR MY DEVICE (Huawei p20). It may or may not be for yours.
        // Testing is required to verify this. Switch 'w' and 'h' to see if it works.
        viewfinder.resolution: Qt.size(h, w)

        focus {
            focusMode: Camera.FocusContinuous
        }
    }

    VideoOutput
    {
        id: videoOutput
        source: camera

        // camera is rotated FOR MY DEVICE (Huawei p20). It may or may not be for yours.
        // Testing is required to verify this. try -180, -90, 0, 90, 180...etc to see what works
        orientation: -90

        visible: false
    }

    //function called to send a frame to C++
    function grab()
    {
        videoOutput.grabToImage(function(frame) {
            frameReady(frame) //Emit frameReady SIGNAL
        }, Qt.size(w, h))
    }
}
