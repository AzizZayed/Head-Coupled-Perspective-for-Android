# Head-Coupled-Perspective-for-Android

This project is a Head Coupled Perspective for Android made with C++, OpenGL and [Qt](qt.io). It is based on the research paper [Using the Userʼs Point of View for Interaction on Mobile Devices](https://hal.archives-ouvertes.fr/hal-00760343/document) by Jérémie Francone and Laurence Nigay. I did this during my internship at the Medical Imagery Lab at Concordia University, supervised by Étienne Léger. [Here](https://www.youtube.com/watch?feature=player_embedded&v=Jd3-eiid-Uw) is a really good video explaining it. [Here](https://www.youtube.com/watch?v=bBQQEcfkHoE) is a YouTube video with a demo on a mobile device.

Basically, this android app tracks your head and shifts the perspective on the objects on screen according to what you should see if you were actually holding those objects in your hand. But you're not, they're virtual. So if you're holding a cube for example, and you move your head to the side, you'll see more of the side of the cube, you're perspective on the objects shifted. This app does the same thing. 

No. Simply rotating the object to replicate such a perspective will not work.

__Methodology__: The Qml engine grabs a frame from the Camera and VideoOutput objects and sends it to the face feature detector. The detector finds the user's facial features and stores the coordinates and dimensions. The scene renderer accesses that information when it needs to draw a frame and calculate the projection matrix. This projection matrix is fed into the vertex shader and OpenGL magic happens.
___

A perspective like the one in the videos is created by something called an [Off-Axis Projection](https://medium.com/@michel.brisis/off-axis-projection-in-unity-1572d826541e). This projection creates the effect of you looking at a landscape outside a window. Here is a demo I created of exaggerated off-axis projection with a cube in a small room:

![](projectiondemo.gif)

As you can see, it's like looking at the cube through a small frame like your phone screen. Some knowledge of Linear Algebra may be required to understand the code behind this projection. The function that creates it is at line 225 in the *__glperspectivescene.cpp__* file.

___

There are many potential applications in medical imagery for such a project. Among them is to shift the perspective on an organ according to a surgeon's head. With this, he can just slightly shift his head to see the different perspectives on the part of the body he is performing his operation on.