<a name="csc8503"></a>
<p align="center"><img width=100% src="Resources/Images/banner.png?raw=true"></p>

![build-date](https://img.shields.io/badge/Build%20Date-1.12.19-brightgreen)
![branch-features](https://img.shields.io/badge/Features-5-informational)

## About
This repo contains a series of physics, networking and AI exercises completed for CSC8503 at Newcastle University. 

## List of Features
[active]:https://img.shields.io/badge/-Active-success
[depreciated]:https://img.shields.io/badge/-Depreciated-inactive
[updating]:https://img.shields.io/badge/-Updating-purple
[passing]:https://img.shields.io/badge/-Passing-success
[outdated]:https://img.shields.io/badge/-Outdated-blue
[dev]:https://img.shields.io/badge/-Unreleased-important

[version-1.0.0]:https://img.shields.io/badge/Ver.-1.0.0-ff69b4
[version-1.0.1]:https://img.shields.io/badge/Ver.-1.0.1-ff69b4
[version-1.0.2]:https://img.shields.io/badge/Ver.-1.0.2-ff69b4
[version-1.0.3]:https://img.shields.io/badge/Ver.-1.0.3-ff69b4
[version-1.0.4]:https://img.shields.io/badge/Ver.-1.0.4-ff69b4
[version-1.0.5]:https://img.shields.io/badge/Ver.-1.0.5-ff69b4
[version-dev]:https://img.shields.io/badge/Ver.-dev-important

|**Category**|**Feature**|**Description**|**Version**|
|:------:|:------:|:-------------:|:---------:|
| Physics |[Raycasting](https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/physicstutorials/1raycasting/Physics%20-%20Raycasting.pdf)|Fires **infinitely thin lines** from a point in the world to see which objects collide with it.|![dev][version-1.0.0]|
| Physics |[Linear Motion](https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/physicstutorials/2linearmotion/Physics%20-%20Movement.pdf)|Uses a **semi-implicit Euler integrator** to simulate rigid body dynamics.|![dev][version-1.0.0]|
| Physics |[Angular Motion](https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/physicstutorials/3angularmotion/Physics%20-%20Rotation.pdf)|Simulates angular movement dynamics using **moments of inertia**.|![dev][version-1.0.0]|
| Physics |[Collision Detection + Resolution]()|**Sphere** and **AABB** detection capabilities, with a plan to eventually implement OBB, capsule and swept volume collisions.|![dev][version-1.0.0]|
| Debug |[Physics Volume Rendering]()|**Displays physics volumes** in the scene.|![dev][version-1.0.0]|

<a name="physics"></a>
### Physics 
--------------------------
<a name="raycasting"></a>
#### Raycasting 
- The implementation in this repo began as an exercise through Newcastle University Raycasting [Tutorial](https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/physicstutorials/1raycasting/Physics%20-%20Raycasting.pdf).
- [Mouse Picking](http://bookofhook.com/mousepick.pdf) using raycasting contains a complete derivation of the inverse of the projection matrix as a tool towards mouse picking.
- **Layers** are implemented as to allow for selective layer collision mechanisms.

The following image contains a snapshot of raycasting being used to highlight mouse selected gameobjects and those around them, with the cube object layer objects ignoring the sphere layer during raycasting:
<a name="raycasting-example"></a>
<p align="center"><img width=100% src="Resources/Images/Screenshots/Physics/raycasting.png?raw=true"></p>

<a name="linear-motion"></a>
#### Linear Motion
- The implementation in this repo bagan as an exercise through Newcastle University Linear Motion [Tutorial](https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/physicstutorials/2linearmotion/Physics%20-%20Movement.pdf).

The following gif contains an example of movement using additive forces on selectable objects:
[![Linear Motion Image](https://i.gyazo.com/1464ba7e4fd20b9d4dfc83fc2c1ff4fe.gif)](https://gyazo.com/1464ba7e4fd20b9d4dfc83fc2c1ff4fe)

#### Angular Motion
- The implementation in this repo bagan as an exercise through Newcastle University Angular Motion [Tutorial](https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/physicstutorials/3angularmotion/Physics%20-%20Rotation.pdf).
- [This](https://en.wikipedia.org/wiki/List_of_moments_of_inertia) is a list of moments of inertia, and was used to implement hollow spheres.

The following gif contains an example of angular movement using point specific additive forces on selectable objects:
[![Angular Motion](https://i.gyazo.com/70361ca93fc59f5d89219e1c99dc0352.gif)](https://gyazo.com/70361ca93fc59f5d89219e1c99dc0352)

#### Collision Detection 
- The implementation in this repo began as an exercise throughthe following tutorials: [1](https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/physicstutorials/4collisiondetection/Physics%20-%20Collision%20Detection.pdf) and [2](https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/physicstutorials/5collisionresponse/Physics%20-%20Collision%20Response.pdf).
- **Terminology**:
  - **Coefficient of Restitution**: The ratio of an object's velocity before and after a collision.
  - **Perfectly Elastic**: Combinations of restitution coefficients of 2 materials = **1**
  - **Inelastic**: Combinations of restitution coefficients of 2 materials = **0**

The following gif contains an example of collision resolution between AABBs and Spheres:
[![Collision Detection](https://i.gyazo.com/fdace9ca6c99c3e44b8698710e6167c5.gif)](https://gyazo.com/fdace9ca6c99c3e44b8698710e6167c5)
<a name="networking"></a>
### Networking 
--------------------------

<a name="ai"></a>
### Artificial Intelligence
--------------------------

### Debug Render View
--------------------------
Below is an example of **AABB** and **Sphere physics volumes** in debug render mode:
<a name="debug-render"></a>
<p align="center"><img width=100% src="Resources/Images/Screenshots/Debugging/BoundingVolume.png?raw=true"></p>

## Keybindings
|**Key**|**Tag**|**Description**|
|:------:|:------:|:-------------:|:---------:|
| ***P*** | Debug Render Toggle | Toggles rendering between the debug scene and the game scene.|
| ***Page_Up*** | Enable Console Window | Enables the console window.|
| ***Page_Down*** | Disable Console Window | Hides the console window.|

<a name="acknowledgments"></a>
## Acknowledgments 
I would like to thank Dr Rich Davison for his exciting and informative lectures and tutorial materials throughout my time at Newcastle University. 

The framework used for this repository was created and provided by him, and he has given permission for it to be used as learning material.
