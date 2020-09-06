# How to integrate Bullet3 physics into your engine

[Bullet3](https://github.com/Bulletphysics/Bullet3) is one of the most popular open-source physics engines. 
It is used commonly in industry and particulary, in game engines. In this small article I will explain how to
integrate it into your engine on example of my own engine - MxEngine.

### Installation
I used the simple approach, and added Bullet3 as a git submodule to build it from sources as a static library using CMake. 
It was not that hard, I simply added it as a subdirectory, built it, then added the following libraries to my project:
- **BulletDynamics**
- **BulletCollision**
- **LinearMath**

Careful, do not mess up with order, as one library can require another and this can lead to very weird errors. Also, I turned off physics
samples, as they are useless. Here is a list of options from my CMake:
```CMake
set(BUILD_CPU_DEMOS OFF)
set(BUILD_BULLET2_DEMOS OFF)
set(BUILD_OPENGL3_DEMOS OFF)
set(BUILD_UNIT_TESTS OFF)
set(USE_GRAPHICAL_BENCHMARK OFF)
set(BUILD_CLSOCKET OFF)
set(BUILD_ENET OFF)
set(BUILD_EXTRAS OFF)
```

### Initialization
In this article I suppose that your engine can draw geometric primitives (like cubes, spheres, planes and etc.) and 
also your engine somehow allows to set translation, rotation, scale for each object (usually this properties
are stores in transform component). This is required as we will need to pull rigid bodies parameters from the Bullet physics simulation
and submit them to engine's renderer. I strongly recommend to visualize everything as soon as possible, as it will certanly reduce bugs
which will happen during the process of integrating the library. So, first what we will do is to create global instance of our simulation world:
```cpp
struct PhysicsModuleData
{
    btCollisionConfiguration* CollisionConfiguration;
    btDispatcher* Dispatcher;
    btBroadphaseInterface*  Broadphase;
    btConstraintSolver* Solver;
    btDiscreteDynamicsWorld* World;
};

void InitPhysicsModule(PhysicsModuleData* data)
{
    data->CollisionConfiguration = new btDefaultCollisionConfiguration();
    data->Dispatcher = new btCollisionDispatcher(data->CollisionConfiguration);
    data->Broadphase = new btDbvtBroadphase();
    data->Solver = new btSequentialImpulseConstraintSolver();
    data->Solver->reset(); // reset solver seed
    data->World = new btDiscreteDynamicsWorld(
        data->Dispatcher, data->Broadphase, data->Solver, data->CollisionConfiguration
    );
    
    data->World->setGravity(btVector3(0.0f, -9.8f, 0.0f));
}
```
The process is very straightforward and described in the Bullet manual. I want to point out only one thing here: `data->Solver->reset();`.
This line is required if you want yout simulations to be the same across multiple engine launches. Without it even if you perform same
actions each time you launch your application, objects will behave different. This is ok if you need to make the simulation unpredictable
to user, but makes all tests unreliable as there is no way to compare object behaviour after tweeking parameters in code.

### Simulation loop
The physics simulation world needs to be updated every frame. The ideal case is when you game runs at fixed fps (30/60), 
then you can just pass this framerate to Bullet. If fps in your game is not fixed, you still can pass delta between frames,
but be aware that simulation can become unpredictable and on low framerate will still slow down.
```cpp
void OnPhysicsUpdate(float dt)
{
    data->World->stepSimulation(dt);
}
```
The line above the only thing you need to perform physics simulation. If you want more precise results, you can call it multiple times,
but be aware that this will take more time if your scene contains large number of physics objects.

### Creating rigid bodies
For now, in this article I will only discuss how to create simple rigid bodies (complex collision bodies, soft bodies and constraints are not supported in MxEngine yet,
so I cannot tell much about them). In Bullet library rigid bodies can be divided in three types: dynamic, static and kinematic.
- **Static objects**: cannot be moved or affected, dynamic objects can collide with them
- **Dynamic objects**: can be moved by physical forces, can collide with every other object
- **Kinematic objects**: can be moved by user through setting their position/rotation, collide only with dynamic objects, but not affected by forces

Usually you want to make most of your scene through static objects, as you do not expect your house or trees to move by other objects.
Static objects are cheap, as they never change their position, so Bullet can perform different optimizations and ease collision test based
on that. If your object need to be changed somehow, you can always re-add it to simulation with new position or change its type, so use static objects most of the time.

Dynamic objects are more interesting, as they are controlled by physics simulation. This can be items that player throw, granades, are boxes which can be interacted with.
You should not change their position manually, and need to apply forces or impulses. The easiest way to interact with them is to set their linear or
angular velocity, as that will instantly make them to move. But be aware that body should be active, or you actions may have no effect.

Kinematic objects are usually used when you want to perform some specific actions with body (for example animations or creating moving platform).
They are controlled by setting their transform, can interact with dynamic objects, but do not affected by physics, so you need to handle all collisions
yourself through callbacks and decide if it should pass through object or stop. Player character is a good example of such kinematic object.

To add rigid body first you need to create `btDefaultMotionState`, `btRigidBody` and `btCollisionShape`. Also I recommend to inherit 
your own motion state object from default one, as it will allow us to query if a physics object changed its transform and 
not to update its world transform if it remains still:
```cpp
class MotionStateNotifier : public btDefaultMotionState
{
public:
    MotionStateNotifier(btTransform& tr) : btDefaultMotionState(tr) { }
    
    bool TransformUpdated = true;
    
    virtual void setWorldTransform(const btTransform& centerOfMassWorldTrans) override
    {
        this->TransformUpdated = true;
        btDefaultMotionState::setWorldTransform(centerOfMassWorldTrans);
    }
};
```
The code above simply forwards transform to base class, but also puts flag if transform was updated. With that we can finally create our rigid body:
```cpp
btRigidBody* CreateBoxRigidBody(btDiscreteDynamicsWorld* world, float mass, btVector3 boxSizes, btVector3 position)
{
    btTransform tr;
    tr.setIdentity();
    tr.setOrigin(position);

    btCollisionShape* shape = new btBoxShape(boxSizes * 0.5f);
    btVector3 inertia(0.0f, 0.0f, 0.0f);
    if(mass != 0.0f) shape->calculateLocalInertia(mass, inertia);

    MotionStateNotifier* state = new MotionStateNotifier(tr);
    btRigidBody* body = new btRigidBody(mass, state, shape);

    world->addRigidBody(body);
    return body
}

void DestroyRigidBody(btDiscreteDynamicsWorld* world, btRigidBody* body)
{
    world->removeRigidBody(body);
    delete body->getCollisionShape();
    delete body->getMotionState();
    delete body;
}
```
Here we create box shape by providing its mass, position and sizes to `CreateBoxRigidBody` function. If mass is 0, the rigid body will be static, 
so we do not need to compute its inertia, otherwise our body is dynamic. If you want to create objects other than boxes, you can replace btBoxShape with other shape,
for example btSphereShape. Also note that Bullet accepts half extents of object size, so we need to multiply our sizes by a half. 

### Drawing objects on screen
After we created some rigid bodies, we of course want to draw them. It's not that hard as you might think. We simply can store pointers to all created rigid bodies
in a array and then just retrieve them every frame. Also note that I use `FromBulletVector3` and `FromBulletQuaternion` - these are just functions which convert Bullet
mathematical objects into your engine's ones. To implement them, vectors you can just copy each of their components, for quaternions you can simply reinterpret_cast the
pointer and then copy data. I understand that your game loop is probably much more complicated, but to get the general idea the following code will be enough:
```cpp
struct GameObject
{
    btRigidBody* physicsBody;
    Vector3 position;
    Vector3 scale;
    Quaternion rotation;
}

std::vector<GameObject> boxes;

void InitScene()
{
    for(size_t i = 0; i < 10; i++)
    {
        float mass = 1.0f;
        btVector3 size(1.0f, 1.0f, 1.0f);
        btVector3 position(0.0f, float(i * 2), 0.0f);
        
        GameObject object;
        object.scale = FromBulletVector3(size);
        object.position = FromBulletVector3(position);
        object.rotation = Quaternion{ };
        object.physicsBody = CreateBoxRigidBody(world, mass, size, position);
        
        boxes.push_back(object);
    }
}
```
Now we go through every game object, update their position/rotation if needed, and then submit them to Renderer:
```cpp
for(GameObject& box : boxes)
{
    bool needUpdate = ((MotionStateNotifier*)box.physicsBody->getMotionState())->TransformUpdated;
    if(needUpdate) // update game object only if transform has changed
    {
        btQuaternion rotation = box->getWorldTransform()->getRotation();
        btVector3 position = box->getWorldTransform()->getOrigin();

        box.position = FromBulletVector3(position);
        box.rotation = fromBulletQuaternion(rotation);
    }

    // draw every rigid body
    Renderer::DrawSimpleBox(box.position, box.rotation, box.scale);
}
```
You won't believe but it is actually enough for simple interaction with Bullet physics. In the next sections we will discuss only some specific physics engine's features,
but you can freely go on your own. I hope you enjoyed this article and it helped you to integrate Bullet physics into your engine!

---

### Creating kinematic objects
Kinematic objects are a bit tricky to create, as they are neither static nor dynamic. You can think of kinematic object is a **static** object, which can be moved by the
game logic (not the physics logic). If we suppose that we already have **configured** body with zero mass and collider, we can make it kinematic with following lines of code:
```cpp
void MakeRigidBodyKinematic(btRigidBody* body)
{
    body->setCollisionFlags(this->body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
    body->forceActivationState(DISABLE_DEACTIVATION);
}
```
Also we need to change the logic of our update loop to account kinematic objects. Instead of setting position and rotation of the game object using rigid body transform,
we use motion state to update rigid body transform (we suppose that user or other engine subsystem sets game object position/rotation somewhere):
```cpp
for(GameObject& box : boxes)
{
    if(box.physicsBody->isKinematic())
    {
        btTransform newTransform;
        newTransform.setOrigin(ToBulletVector3(box.position));
        newTransform.setRotation(ToBulletQuaternion(box.rotation));
        
        box.physicsBody->getMotionState()->setWorldTransform(newTransform);
    }
    else // same as before, get position and rotation from rigid body transform
    {
        bool needUpdate = ((MotionStateNotifier*)box.physicsBody->getMotionState())->TransformUpdated;
        if(needUpdate) // update game object only if transform has changed
        {
            btQuaternion rotation = box.physicsBody->getWorldTransform()->getRotation();
            btVector3 position = box.physicsBody->getWorldTransform()->getOrigin();
            
            box.position = FromBulletVector3(position);
            box.rotation = fromBulletQuaternion(rotation);
        }
    }
    
    // draw every rigid body
    Renderer::DrawSimpleBox(box.position, box.rotation, box.scale);
}
```
### Raycasting your rigid bodies
With Bullet physics we can not only simulate rigid body dynamics and handle collisions, but also cast rays on our objects. First of all, we should change our initialization
stage to get the ability to access game object if we only have rigid body. Luckly, Bullet provided a *user-pointer* field for our needs which we can set when create rigid body:
```cpp
struct GameObject
{
    btRigidBody* physicsBody;
    Vector3 position;
    Vector3 scale;
    Quaternion rotation;
}

std::vector<GameObject> boxes;

void InitScene()
{
    for(size_t i = 0; i < 10; i++)
    {
        float mass = 1.0f;
        btVector3 size(1.0f, 1.0f, 1.0f);
        btVector3 position(0.0f, float(i * 2), 0.0f);
        
        GameObject object;
        object.scale = FromBulletVector3(size);
        object.position = FromBulletVector3(position);
        object.rotation = Quaternion{ };
        object.physicsBody = CreateBoxRigidBody(world, mass, size, position);
        
        // set user pointer value to index in boxes array
        size_t gameObjectIndex = boxes.size();
        object.physicsBody->setUserPointer(reinterpret_cast<void*>(gameObjectIndex));
        
        boxes.push_back(object);
    }
}
```
For simple needs we can use raycast callback provided by Bullet. If we only care about first ray hit, the `ClosestRayResultCallback` will be enough. In the following
code section we simply create callback object, pass it to `rayTest` method of our physics world and then check if it hit the object. If it succeeded, `m_collisionObject` field
will contain pointer to rigid body:
```cpp
size_t RayCast(btDynamicsWorld* world, btbtVector3 raySource, btVector3 rayEnd)
{
    btCollisionWorld::ClosestRayResultCallback callback(raySource, rayEnd);
    world->rayTest(raySource, rayEnd, callback);
    
    if(callback.hasHit())
    {
        void* userPointer = callback.m_collisionObject->getUserPointer();
        return reinterpret_cast<size_t>(userPointer);
    }
    else return (size_t)-1; // some invalid value
}
```
After raycasting we can check if the ray hit game object and access it by index which was returned from `RayCast` function:
```cpp
if(gameObjectIndex < boxes.size())
{
    GameObject& object = boxes[gameObjectIndex];
    // do some fancy stuff with object
}
```