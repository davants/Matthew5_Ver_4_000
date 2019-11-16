#pragma once
// Added for Version 2.000

#include <memory>
#include "M5DxGpuDeviceNFactory.h"
#include "M5DxFrameResource.h"

namespace Matthew5
{
	namespace Matthew5Graphics
	{
		// To keep the Gpu busy, we want to keep its command queue busy. 
		// See "Training Instructions and Planning" notes below.
		class M5DxFrameResources
		{
		public:
			M5DxFrameResources(M5DxGpuDeviceNFactory & vrM5DxGpuDeviceNFactory, ID3D12CommandQueue & vrID3D12CommandQueue);

			// Get the available, "current", frame resource.
			M5DxFrameResource & GetAvailableResource(void);

			// The client can ask for the currently available resource index.
			int GetAvailableFrameResourceIndex(void);

			// When the client gets an event from the Gpu that a resource is available, it 
			// can call this function.
			void SetAvailableFrameResourceIndex(int vIndex);

			// Given an index value that is from 0 through 3 as a parameter, what is the previous value.
			int GetPreviousIndex(int vIndex);

			
			// Given an index value that is from 0 through 3 as a parameter, what is the Next value.
			int GetNextIndex(int vIndex);

			// Get the a frame resource based on its index.
			M5DxFrameResource& GetFrameResource(int vIndex);


		private:
			// Always 4 frame resources.
			std::unique_ptr<M5DxFrameResource> mM5DxFrameResourceList[4];

			// This will help keep track of which frame resource is available to be used.
			int mAvailableFrameResourceIndex;

			ID3D12CommandQueue & mrID3D12CommandQueue;

		};
	}
}


	// Training Instructions and Planning:
	//
	// The context for this Training Instruction is for the Desktop, not Oculus unless
	// specifically indicated.

	// A "frame" is one finished picture that is drawn to the screen like a photograph.
	// We want the Gpu to draw 60, or more, frames per second.
	// It is very important to make sure the Gpu stays busy so we don't want to give it 
	// one frame to draw and then have it wait for us to give it the next frame; we will
	// always give the Gpu the next 4 frames to draw.
	// Because we have given it 4 frames to draw, it is helpful for you to think of it
	// like the Gpu will draw all 4 at the same time; as if they are being drawn at the
	// same moment.Why think of it this way ? Because it makes you realise that each
	// frame will need to "own" its own commands and allocated memory.
	// Scene objects can move around; therefore, many will be in a different location
	// in each frame.This means when you allocate memory on the Gpu, you
	// will need to actually allocate 4. In reality it only uses one at a time; however,
	// we, in C++, don't know which one it is working on so they are all off limits for 
	// updating / modification until we know for sure it is done with one of the 4.
	// The camera can also move so you need to allocate 4 spots in memory for the camera.
	// If you want to draw 4 frames "at the same time" you will need 4 sets of "Draw"
	// commands; meaning 4 command allocators with recorded commands.And as we said, you
	// will also need 4 blobs of Gpu memory to hold the scene object's locations.
	// We call all of these things that are needed for an individual frame the "frame's 
	// resources"; so we will need 4 "frame resources".
	// We will create a class named "frame resource" that will manage the scene objects
	// data on the Gpu and the camera's data on the Gpu. Therefore, as we said, we will 
	// need 4 of these objects.

	// As we said above, we need 4 command allocators because each frame will need its
	// own set of draw commands.The existing M5DxGpuCommand class we have allows us to
	// specify how many allocators we want in its constructor so we will specify 4.

	// After a frame has been drawn, we don't need its resources anymore so we can re-use 
	// them again; however, we must check with the Gpu to make sure it is done with a
	// frame before re - using it's Gpu resources.

	// In reality the frames are drawn one after the other; however, we
	// give the Gpu 4 frames to draw so that it is always busy and does not have to wait
	// on us.As we said, we must treat the frame resource like they are run at the same time
	// otherwise we'll accidentally update some data on the Gpu when the Gpu hasn't even used
	// the old data yet!This would be bad.We give it 4 frames to work on and then we wait
	// for one of the frames to be completly drawn and finished.We confirm the Gpu has drawn
	// the frame and then we know we can re - use that frame's Gpu resourses. 
	// Re - use a frame's resources:
	// - We update the frame resources, like scene object locations and camera position
	// - We record new commands to the command allocator
	// - We add the commands back to the Gpu, at the end of the queue, to draw a new frame

	// The commands that do the drawing are stored in command allocators as we have said.
	// The commands are added to our one single command queue.
	// So from our side, the C++ side, let's walk through drawing a frame after we have 
	// already given it 4 frames to draw.
	// Imagin, in C++, we have already given the Gpu 4 frames to draw.
	// Each frame requires a bunch of commands that are recorded in one Gpu command
	// allocator, so each frame has its own Gpu command allocator.
	// Each frame requires its own Scene object locations and camera position, so each
	// frame has its own Gpu memory resources for this data.However, the geometry
	// which makes the actual shape is in local space and does not change for the
	// entire lifetime of the application so we don't need axtra copies of the 
	// geometry that makes up a shape.Meaning we can have a chair geometry and
	// have 20 chairs in our scene.Each chair, a scene object, has its own
	// location in the scene; but we only need the geometry for the chair one time
	// in Gpu memory because they are all the same chair shape.We do need the 20
	// world space locations for the chairs, multiplied by 4 frames; 80 chair
	// world space locations in memory.
	// Now, back to our 4 frames... we will call them frame numbers 1, 2, 3 and 4.
	// "Adding a frame" means we added its Gpu command allocator to a "ToDo" list on the
	// Gpu called the "Gpu Command Queue".
	// As we have said, for our use case we have said we already added 4 frames.The Gpu
	// now has 4 frames to draw so it is hard at work.It will work on them in	the order
	// they were added.
	// So, it will start with frame number 1. We know when it is done it will immediatly
	// start on frame 2; but for now it is still working on frame 1.
	// Now in C++ we want to keep loading frames to be drawn so the Gpu keeps busy;
	// however, we can't add anymore because the Gpu may get too far behind, so 
	// we wait.We wait for it to finish frame number 1.
	// Frame number 1 was the first one we added so the Gpu will be done with it before
	// the others.
	// Bam!The Gpu finishes all the command in the command allocator for frame 1. The
	// Gpu will signal us, in C++, that it has finished frame 1.
	// The Gpu will immediatly go to the "command queue" to see if there are any more
	// groups of commands, in command allocators, with work to do.There is of
	// course because we added 4.
	// Frame 2's commands are in the queue waiting to be processed. The Gpu 
	// immediatly goes to work on frame 2.
	// At the same time back in C++, the signal has been received that frame 1 is done
	// which means it has finished drawing to the back buffer.Note that the back
	// buffer is not part of our frame resouces, there are only 2 image buffers
	// needed, the one on the screen and a back buffer to draw to.One of the
	// commands recorded in our command allocator is which of these 2 buffer to
	// draw to.FYI, this means that if we give 4 frames to draw to the Gpu, every
	// other one will be the same "back buffer".In our example, frame 1 and 3 will
	// draw to the same buffer and 2 and 4 will draw to the same buffer.
	// Remember, the Gpu does actually use the frame resources one at a time, but
	// we don't know which one of the 4 it is working on so we must think of them 
	// all as off limits until we are told by the Gpu that one is available.
	// In C++, we now know for sure that the Gpu is done with all the frame resources
	// for frame 1. For example, this means the Gpu memory that stored the scene
	// object's world space locations that were stored on the Gpu can be 
	// updated(re - used).
	// It also means the command allocator that keeps all the "draw frame" commands
	// for frame 1 is also free to be re - used.
	// So here we go, in C++ we can start on frame number 5.
	// This means overwrite the Gpu memory, previously used for frame 1, with the
	// frame 5 scene object world space locations and the new camera position.
	// We also record all the new "draw commands" to the command allocator that
	// was used for frame 1.
	// Last, we add the newly recorded commands, recorded to the re - used command
	// allocator, for frame 5 to the Gpu command queue.
	// Next, in C++ we wait for the "frame 2 is done" signal from the Gpu and start
	// on frame 6 using frame 2's resources.  This goes on and on....

	// Let's take a step back and review an error situation.	
	// What if, in C++, we started overwriting frame 1's resource with frame 5's data
	// before the Gpu was done with frame 1 ? The Gpu would try to run the commands
	// but the data would be corrupted.The Gpu could draw frame 5 before frame 2.
	// Worse, the Gpu could run some of the commands for frame 1 and then half way
	// through start running frame 5 commands because in C++ the command allocator
	// was updated.
	// This is why we must wait for the Gpu to signal us, in C++, when it is done
	// with a frame.

	// Last, let's talk about Oculus. Oculus renders 2 scenes per frame, not one. So, 
	// it will need 8 command allocators because each eye will need its own
	// commands.
	// It will also need 8 frame resources for the camera because each eye
	// represents one camera and they are different.
	// However!You will not need 2 scene object world space locations for a
	// frame because they are in the same place in world space for each frame.
	// It is the camera's, eyes, that are different not the scene object's location.
	// For scene object location resources we will only need 4 like desktop.

	// Frame Resource Requirments - What does each frame need.
	// - Command Allocator to hold the recorded commands for that frame
	// - Scene Object World Space Location Data, to hold the 20 locations for the chair
	// - Camera world space location and the direction it is pointing
	// - A way to track which is the next available frame resource


	// Other notes and thoughts:
	// Some things don't move. 
	// You may be thinking that some things in a scene, like a mountain, don't move 
	// so why do I need 4 ? You are correct.However, for this tutorial it is
	// assumed that everything, even mountains, can move if you have enough faith.
	// Can't geometry move?
	// We have said we only need 1 copy of the actual local space geometry like the
	// chair; however, doesn't some geometry change shape? At this 
	// point in the tutorial geometry does not change its shape; the chair will stay
	// a chair.

