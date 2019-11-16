// Davant Stewart
// Version 2.000 - Triangles
#include <iostream>
#include <memory>
#include <Windows.h>

#include <chrono>

//#include "agents.h"
//using namespace Concurrency;



#include "btBulletDynamicsCommon.h"
#include "Matthew5StandardMicrosoftHeaders.h"

#include "Matthew5Windows.h"
#include "Matthew5DxDesktop.h"
#include "Matthew5DxOculus.h"
#include "IMatthew5DrawInstructions.h"

using namespace Matthew5::Windows;
using namespace Matthew5::Matthew5Graphics;
using namespace Matthew5::Matthew5GraphicsOculus;

#define VectorForward       XMVectorSet(0,0,-1,0)

std::unique_ptr<IMatthew5Graphics> ChooseOculusOrDesktop(HWND vHWND)
{
	// First try to create the Oculus version and if that fails create the desktop version.

	std::unique_ptr<IMatthew5Graphics> vpIMatthew5Graphics = std::make_unique<Matthew5DxOculus>(vHWND);

	// Get the pointer/address from the smart pointer and cast it to the derived Matthew5DxOculus class 
	// so that we can call functions that only exist in the derived class.
	Matthew5DxOculus * vpMatthew5DxOculus = (Matthew5DxOculus *)vpIMatthew5Graphics.get();

	if ( ! vpMatthew5DxOculus->IsOculusAvailable())
	{
		// Oculus is Not Available, so create the desktop version.
		vpIMatthew5Graphics = std::make_unique<Matthew5DxDesktop>(vHWND);
	}

	return vpIMatthew5Graphics;
}

void AddRigidBody(std::vector<btRigidBody*> & vrRigidBodyList,
	int vIndex, 
	btDiscreteDynamicsWorld* vpDynamicsWorld,
	btTransform & vrTransform,
	btCollisionShape* vpColShapeBox,
	float vMass)
{

	btScalar massBox(vMass);

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamicBox = (massBox != 0.f);

	btVector3 localInertiaBox(1.0, 0, 0);
	if (isDynamicBox)
		vpColShapeBox->calculateLocalInertia(massBox, localInertiaBox);

	//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionStateBox = new btDefaultMotionState(vrTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfoBox(massBox, myMotionStateBox, 
		vpColShapeBox, localInertiaBox);
	btRigidBody* vpRigidBody = new btRigidBody(rbInfoBox);

	vrRigidBodyList.push_back(vpRigidBody);

	//add the body to the dynamics world
	vpDynamicsWorld->addRigidBody(vpRigidBody);

}

void AddGround(btDiscreteDynamicsWorld* vpDynamicsWorld, btCollisionShape* vpColShape)
{

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0, -2, 0));

	btScalar massGround(0.);

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamicGround = (massGround != 0.f);

	btVector3 localInertiaGround(0, 0, 0);
	if (isDynamicGround)
		vpColShape->calculateLocalInertia(massGround, localInertiaGround);

	//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionStateGround = new btDefaultMotionState(groundTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfoGround(massGround, myMotionStateGround, vpColShape,
		localInertiaGround);
	btRigidBody* bodyGround = new btRigidBody(rbInfoGround);

	//add the body to the dynamics world
	vpDynamicsWorld->addRigidBody(bodyGround);

}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	// Create needed bullet objects.
	std::vector<btRigidBody*> vRigidBodyList;

	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* overlappingPairCache;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;
	btAlignedObjectArray<btCollisionShape*> collisionShapes;
	btTransform mBtTransform;
	collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	dispatcher = new btCollisionDispatcher(collisionConfiguration);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	overlappingPairCache = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	solver = new btSequentialImpulseConstraintSolver;

	//btDiscreteDynamicsWorld* 
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0, -10, 0));


	// Ground object Shape
	btCollisionShape* colShapeGround = new btBoxShape(btVector3(50, 0.5, 50));
	collisionShapes.push_back(colShapeGround);

	// Regular object Shape
	btCollisionShape* colShapeBox = new btSphereShape(1.0f);
	collisionShapes.push_back(colShapeBox);

	AddGround(dynamicsWorld, colShapeGround);	

	// Create windows wrapper which provides one window.
	int vWidth = 800, vHeight = 600;
	std::string vFileNameElephantBox = "ElephantBox.obj";
	Matthew5Windows vMatthew5Windows(hInstance, vWidth, vHeight);

	// This will point to Oculus or Desktop; however, the interface does not care.
	std::unique_ptr<IMatthew5Graphics> vpIMatthew5Graphics = ChooseOculusOrDesktop(vMatthew5Windows.GetHWND());
		
	IMatthew5DrawInstructions vIMatthew5DrawInstructions;

	// We will keep the "Camera Look At" the same for now.. which is 1000 units in
	// the forward direction.
	vIMatthew5DrawInstructions.GetCameraLookAt().mPositionZ = -1000.0f;
		
	auto start_time = std::chrono::high_resolution_clock::now();
	auto current_time = std::chrono::high_resolution_clock::now();
	
	btScalar vStep; 
	long long vMillisecondsPassed = 0;
	while (true)
	{
		// Get User input
		Matthew5WindowsUserHoldingKeys & vUserInput = vMatthew5Windows.GetWindowsMessages();

		// Calculate how much time has passed since the last call to Bullet stepSimulation function.
		current_time = std::chrono::high_resolution_clock::now();
		vMillisecondsPassed = (std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count()) ;

		if (vMillisecondsPassed == 0)
		{
			continue;
		}

		vStep = vMillisecondsPassed / 1000.f;

		// Get ready for next loop iteration so time passed calculation can be done.
		start_time = current_time;
		
		// Ask Bullet to move all the objects in the world based on how much time has passed.
		dynamicsWorld->stepSimulation(vStep,10);
		
		// Create in index variable which is used to coordinate the bullet bodies and the 
		// cooresponding Matthew5 scene objects.
		int vIndex = 0;

		// Loop through all bullet physics bodies and update the cooresponding Matthew5 scene
		// object's Gpu memory.
		for(auto itemRigidBody = vRigidBodyList.begin(); itemRigidBody != vRigidBodyList.end(); itemRigidBody++)
		{	
			// Create simple pointer to the bullet rigid body so syntax is easier to 
			// read.
			btRigidBody* vpRigidBody = *itemRigidBody;

			// Get the bodies transformation, position and rotation, in a 4x4 matrix.
			btTransform vbtBoxTransform;
			auto vpMotionState = vpRigidBody->getMotionState();
				
			vpMotionState->getWorldTransform(vbtBoxTransform);

			float m[16];
			vbtBoxTransform.getOpenGLMatrix(m);

			// Create a Cpu Position that will hold the 4x4 matrix from bullet.
			M5SceneObjectCpuPosition vM5SceneObjectCpuPosition;

			vM5SceneObjectCpuPosition.m11 = m[0];    vM5SceneObjectCpuPosition.m12 = m[4];    vM5SceneObjectCpuPosition.m13 = m[8];    vM5SceneObjectCpuPosition.m14 = m[12];
			vM5SceneObjectCpuPosition.m21 = m[1];    vM5SceneObjectCpuPosition.m22 = m[5];    vM5SceneObjectCpuPosition.m23 = m[9];    vM5SceneObjectCpuPosition.m24 = m[13];
			vM5SceneObjectCpuPosition.m31 = m[2];    vM5SceneObjectCpuPosition.m32 = m[6];    vM5SceneObjectCpuPosition.m33 = m[10];    vM5SceneObjectCpuPosition.m34 = m[14];
			vM5SceneObjectCpuPosition.m41 = m[3];    vM5SceneObjectCpuPosition.m42 = m[7];    vM5SceneObjectCpuPosition.m43 = m[11];    vM5SceneObjectCpuPosition.m44 = m[15];

			// Update the Gpu, passing the index of the scene object we are updating.
			vpIMatthew5Graphics->GetSceneObjectList().UpdateSceneObject(
				vIndex,
				vM5SceneObjectCpuPosition);

			// Increment the index so bullet and Matthew5 scene objects are in-sync.
			vIndex++;
		}

		if (vUserInput.CharHit('W'))
		{
			// Move Forwards, reduce the z.
			// Right Handed Coordinates means larger z is backward; so to move camera forward reduce the z.
			vIMatthew5DrawInstructions.GetCameraPosition().mPositionZ -= 0.01f;
		}
		else if (vUserInput.CharHit('S'))
		{
			// Move Backwards, increase the z.
			// Right Handed Coordinates means larger z is backward; so to move camera backward increase the z.
			vIMatthew5DrawInstructions.GetCameraPosition().mPositionZ += 0.01f;
		}
		else if (vUserInput.CharHit('A'))
		{
			// Turn to the Left.
		}
		else if (vUserInput.CharHit('D'))
		{
			// Turn to the Right.
		}
		else if (vUserInput.CharHit('I'))
		{
			btTransform vTransform;
			vTransform.setIdentity();
			vTransform.setOrigin(btVector3(
				1.0f,
				+20.0f,
				-15.0f));

			float m[16];
			vTransform.getOpenGLMatrix(m);


			M5SceneObjectCpuPosition vM5SceneObjectCpuPosition;

			//XMFLOAT4X4& matrix = vM5SceneObjectCpuPosition.matrix;
			vM5SceneObjectCpuPosition.m11 = m[0];    vM5SceneObjectCpuPosition.m12 = m[4];    vM5SceneObjectCpuPosition.m13 = m[8];    vM5SceneObjectCpuPosition.m14 = m[12];
			vM5SceneObjectCpuPosition.m21 = m[1];    vM5SceneObjectCpuPosition.m22 = m[5];    vM5SceneObjectCpuPosition.m23 = m[9];    vM5SceneObjectCpuPosition.m24 = m[13];
			vM5SceneObjectCpuPosition.m31 = m[2];    vM5SceneObjectCpuPosition.m32 = m[6];    vM5SceneObjectCpuPosition.m33 = m[10];    vM5SceneObjectCpuPosition.m34 = m[14];
			vM5SceneObjectCpuPosition.m41 = m[3];    vM5SceneObjectCpuPosition.m42 = m[7];    vM5SceneObjectCpuPosition.m43 = m[11];    vM5SceneObjectCpuPosition.m44 = m[15];

			int vBoxId = vpIMatthew5Graphics->GetSceneObjectList().AddSceneObject(vM5SceneObjectCpuPosition, vFileNameElephantBox);
			
			AddRigidBody(vRigidBodyList,
				vBoxId,
				dynamicsWorld,
				vTransform,
				colShapeBox,
				1.0f);

			std::string vDebugString = "\nObjectAdded\n";
			
			OutputDebugStringA(vDebugString.c_str());
		}

		vpIMatthew5Graphics->Draw(vIMatthew5DrawInstructions);

		if (vUserInput.CloseProgram == true)
		{
			break;
		}
	}
	return 0;
}


