# OpenGL Animation System
 
An OpenGL animation system that I coded as part of a school project. More details on this project can be found on my [portfolio website](https://alexdiallo.com/portfolio/ps4-and-pc-engine-project/).

The mathematics I use are explained in a slides presentation that is also accessible from my [portfolio site](https://alexdiallo.com/2020/05/23/general-introduction-to-animations/).

The general system is explained here:

## AnimationDrawObject (in AnimationStructs.h)
Responsible for storing all the data required for drawing an animated draw object such as vertex buffer data and joint (i.e. skeleton) information
## Joint
Structure used to store a hierarchy of joints to calculate the associated bone matrices used in the shader
## Animation
Contains the animation data (e.g. key frames, joint positions) for an AnimationDrawObject which it uses to update the joint transform data
## AnimatedModel
The AnimatedModel stores one or multiple AnimationDrawObjects and is in charge of updating the animations and rendering the AnimatedModel instances
