<a name="csc8503"></a>
<p align="center"><img width=100% src="Resources/Images/banner.png?raw=true"></p>

![build-date](https://img.shields.io/badge/Build%20Date-8.12.19-brightgreen)
![branch-features](https://img.shields.io/badge/Features-8-informational)

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
| Physics |[Broad & Narrow Phase]()|**Octree** Broad and Narrow **spatial acceleration structure** implementations.|![dev][version-1.0.0]|
| AI |[State Machines]()|Using a templated **generic FSM**.|![dev][version-1.0.0]|
| AI |[A* Pathfinding]()|Using **Manhattan** heuristic on **grid-based** paths.|![dev][version-1.0.0]|
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

The following gif contains an example of **collision resolution** between AABBs and Spheres using **Projection Seperation** and **Impulsing**:
[![Collision Resolution](https://i.gyazo.com/fdace9ca6c99c3e44b8698710e6167c5.gif)](https://gyazo.com/fdace9ca6c99c3e44b8698710e6167c5)


The following gif contains an example of **collision resolution** between AABBs and Spheres using the **Penalty Method** utilising **Hook's Law**:
[![Collision Resolution](https://i.gyazo.com/02919f829766bfb6037fcce8eea89e6a.gif)](https://gyazo.com/02919f829766bfb6037fcce8eea89e6a)
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

[//]: # (Insert keybindings regarding turning and twisting of objects in selection mode)
## Keybindings

|**Key**|**Tag**|**Description**|
|:------:|:------:|:-------------:|
| ***Escape*** | Exit | Terminates the program.|
| ***Page_Up*** | Enable Console Window | Enables the console window.|
| ***Page_Down*** | Disable Console Window | Hides the console window.|
| ****DEBUGGING**** | ---- | *The keybindings listed below are used to **perform debugging** on the simulation.* |
| ***Mouse 1*** | Object Selection |If used over a gameobject on a raycastable layer, will trigger the **Object Selection State** on that object.|
| ***P*** | Debug Render Toggle | Toggles rendering between the debug scene and the game scene.|
| ***R*** | Camera Movement State Toggle | Toggles between free camera movement and no camera movement.|
| ***L*** | Object Lock Camera Movement | If an object is selected, toggles between object locked camera space and free camera movement.|
| ****SIMULATION**** | ---- | *The keybindings listed below are used to alter the simulation settings.* |
| ***F1*** | Reset | Resets the simulation world.|
| ***F2*** | Camera Reset | Resets the simulation camera to a pre-defined location in the world. |
| ***F7*** | Enable Object Shuffle | Enable object shuffling to reduce physics calculation bias. |
| ***F8*** | Disable Object Shuffle | Disable object shuffling. |
| ***F9*** | Enable Constraints Shuffle | Enable constraint shuffling to reduce physics calculation bias. |
| ***F10*** | Disable Constraints Shuffle | Disable constraint shuffling. |
| ***G*** | Gravity Toggle | Toggles the gravity of the simulation between an active and non-active state.|
| ****CAMERA**** | ---- | *The keybindings listed below are only active when the **camera movement state** is active.* |
| ***W*** | Forward | Moves the camera in the forward direction. |
| ***A*** | Left | Moves the camera in the left direction. |
| ***S*** | Backwards | Moves the camera in the backwards direction. |
| ***D*** | Right | Moves the camera in the right direction. |
| ***Q*** | Up | Moves the camera in the upwards direction. |
| ***E*** | Down | Moves the camera in the downwards direction. |
| ****OBJECT SELECTION**** | ---- | *The keybindings listed below are only active when the **gameObject selection state** is active.* |
| ***W*** | Forward | Moves the object in the forward direction. |
| ***A*** | Left | Moves the object in the left direction. |
| ***S*** | Backwards | Moves the object in the backwards direction. |
| ***D*** | Right | Moves the object in the right direction. |
| ***Q*** | Up | Moves the object in the upwards direction. |
| ***E*** | Down | Moves the object in the downwards direction. |


<a name="acknowledgments"></a>
## Acknowledgments 
I would like to thank Dr Rich Davison for his exciting and informative lectures and tutorial materials throughout my time at Newcastle University. 

The framework used for this repository was created and provided by him, and he has given permission for it to be used as learning material.
