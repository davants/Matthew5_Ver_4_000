#include "M5TriFactory.h"

namespace Matthew5
{
	namespace Matthew5Graphics
	{
		std::unique_ptr<M5TriCpuGpuResources> M5TriFactory::GetTriangleSetResourcesForSimpleTriangle(
			ID3D12Device & vrID3D12Device)
		{
			std::unique_ptr< M5TriCpuGpuResources> vpReturnObject
				= std::make_unique<M5TriCpuGpuResources>();

			vpReturnObject->mpM5TriCpuResources = GetCpuSimpleTriangle();

			vpReturnObject->mpM5TriGpuResources = GetM5TriGpuResources(
				vrID3D12Device, *vpReturnObject->mpM5TriCpuResources);

			return vpReturnObject;
		}

		std::unique_ptr<M5TriCpuGpuResources> M5TriFactory::GetTriangleSetResourcesForObjFile(
			ID3D12Device& vrID3D12Device, M5DxGpuCommand& vrM5DxGpuCommand, 
			std::string& vrObjFilePathDirectory,
			const std::string& vrObjFileName)
		{
			tinyobj::attrib_t vAttrib;
			std::vector<tinyobj::shape_t> vShapesList;
			std::vector<tinyobj::material_t> vMaterialsList;
			std::string vWarning;
			std::string vError;

			std::string vFullPathToObj = vrObjFilePathDirectory + vrObjFileName;

			bool vLoadResult = tinyobj::LoadObj(&vAttrib, &vShapesList, &vMaterialsList, &vWarning, &vError,
				vFullPathToObj.c_str(),
				vrObjFilePathDirectory.c_str());				

			if (!vError.empty()) {
				return nullptr;
			}

			if (vShapesList.size() == 0)
			{
				return nullptr;
			}

			if (vAttrib.vertices.size() == 0)
			{
				return nullptr;
			}

			tinyobj::shape_t& vrShape = vShapesList[0];


			if((vAttrib.vertices.size() % 3) != 0)
			{
				return nullptr;
			}

			if ((vrShape.mesh.indices.size() % 3) != 0)
			{
				return nullptr;
			}

			int32_t vNumberOfVertexItems = vAttrib.vertices.size() / 3;
			int32_t vNumberOfIndicies = vrShape.mesh.indices.size() ;
					   			 
			// A triangles needs 3 vertex points.
			M5TriVertex* vpM5TriVertex = new M5TriVertex[vNumberOfVertexItems];

			// A triangle needs 3 index numbers to show the vertex points needed 
			// for a triangle.
			int16_t* vpIndixies = new int16_t[vNumberOfIndicies];

			for (int i = 0; i < vNumberOfVertexItems; i++)
			{
				int vVerticiesOffset = i * 3;

				vpM5TriVertex[i].mPositionX = vAttrib.vertices[vVerticiesOffset];
				vpM5TriVertex[i].mPositionY = vAttrib.vertices[vVerticiesOffset + 1];
				vpM5TriVertex[i].mPositionZ = vAttrib.vertices[vVerticiesOffset + 2];
			}

			int iIndices = 0;

			for (auto item = vrShape.mesh.indices.begin(); item != vrShape.mesh.indices.end(); item++ ) //item++)
			{
				vpIndixies[iIndices] = (*item).vertex_index;
				iIndices++;
				
			}

			std::unique_ptr< M5TriCpuGpuResources> vpReturnObject
				= std::make_unique<M5TriCpuGpuResources>();

			// Create the return object on the heap; using "std::make_unique" instead of "new".
			std::unique_ptr<M5TriCpuResources> mpM5TriCpuResources =
				std::make_unique<M5TriCpuResources>();



			mpM5TriCpuResources->mpVertexArray = std::unique_ptr<M5TriVertex>(vpM5TriVertex);
			// At this point, both mpVertexArray and vpM5TriVertex have the same heap address.
			// Since mpVertexArray now "owns" it, set vpM5TriVertex to nullptr to be extra careful. 
			// Note that setting it to nullptr is not really needed as long as we don't delete it.
			vpM5TriVertex = nullptr;
			mpM5TriCpuResources->mNumberOfVerticies = vNumberOfVertexItems;

			// Place our indicies, that were allocated on the heap using "new", in 
			// unique pointers for safe keeping and "owership" control.
			mpM5TriCpuResources->mpIndexArray = std::unique_ptr<int16_t>(vpIndixies);
			vpIndixies = nullptr; // Set vpIndixies to nullptr to be extra careful.
			mpM5TriCpuResources->mNumberOfIndicies = vNumberOfIndicies;

			vpReturnObject->mpM5TriCpuResources = std::move(mpM5TriCpuResources);

			vpReturnObject->mpM5TriGpuResources = GetM5TriGpuDefaultBufferResources(
				vrID3D12Device, *vpReturnObject->mpM5TriCpuResources,
				vrM5DxGpuCommand);

			return vpReturnObject;
		}

		std::unique_ptr<M5TriGpuResources> M5TriFactory::GetM5TriGpuResources(ID3D12Device & vrID3D12Device, M5TriCpuResources & vrM5TriCpuResources)
		{
			std::unique_ptr<M5TriGpuResources> vpReturnValue =
				std::make_unique<M5TriGpuResources>();

			vpReturnValue->mpVertexArray = std::make_unique<M5DxGpuMemoryUploadBuffer>(
				vrID3D12Device,
				vrM5TriCpuResources.mpVertexArray.get(),
				vrM5TriCpuResources.GetVerticiesTotalSizeInBytes());

			vpReturnValue->mpIndexArray = std::make_unique<M5DxGpuMemoryUploadBuffer>(
				vrID3D12Device,
				vrM5TriCpuResources.mpIndexArray.get(),
				vrM5TriCpuResources.GetIndiciesTotalSizeInBytes());

			return vpReturnValue;
		}

		std::unique_ptr<M5TriGpuResources> M5TriFactory::GetM5TriGpuDefaultBufferResources(
			ID3D12Device& vrID3D12Device, M5TriCpuResources& vrM5TriCpuResources, M5DxGpuCommand& vrM5DxGpuCommand)
		{
			std::unique_ptr<M5TriGpuResources> vpReturnValue =
				std::make_unique<M5TriGpuResources>();

			vpReturnValue->mpVertexArray = std::make_unique<M5DxGpuMemoryUploadBuffer>(
				vrID3D12Device,
				vrM5TriCpuResources.mpVertexArray.get(),
				vrM5TriCpuResources.GetVerticiesTotalSizeInBytes(),
				vrM5DxGpuCommand);

			vpReturnValue->mpIndexArray = std::make_unique<M5DxGpuMemoryUploadBuffer>(
				vrID3D12Device,
				vrM5TriCpuResources.mpIndexArray.get(),
				vrM5TriCpuResources.GetIndiciesTotalSizeInBytes(),
				vrM5DxGpuCommand);

			return vpReturnValue;
		}

		std::unique_ptr<M5TriCpuResources> M5TriFactory::GetCpuSimpleTriangle(void)
		{
			// A triangles needs 3 vertex points.
			M5TriVertex * vpM5TriVertex = new M5TriVertex[3];

			// A triangle needs 3 index numbers to show the vertex points needed 
			// for a triangle.
			int16_t * vpIndixies = new int16_t[3];

			// Create the following triangle where "*" is local space 0,0,0.
			// Horizontal/Width is x and Up/Height is y. 
			// Also, the width and height are 1 unit; meaning the top left corner 
			// of the triangle is x(-0.5f) and y(0.5).
			//			 ----------.
			//			|        .
			//			|      .
			//			|    .* 
			//			|  .    
			//			|.

			// A 3D box requires multiple triangles for all the serfaces. 
			// Here, we are only drawing one triangle, like one size of a box.
			// Since we are drawing one triangle depth is not important so z
			// will be the same for all 3 verticies.
			float z = 1.0;

			// Fill in the front face vertex data. 
			// Left Bottom
			vpM5TriVertex[0] = M5TriVertex(-0.5f, -0.5f, z);

			// Left Top
			vpM5TriVertex[1] = M5TriVertex(-0.5f, +0.5f, z);

			// Right Top
			vpM5TriVertex[2] = M5TriVertex(+0.5f, +0.5f, z);

			// Fill in Index data
			vpIndixies[0] = 0;
			vpIndixies[1] = 1;
			vpIndixies[2] = 2;

			// Create the return object on the heap; using "std::make_unique" instead of "new".
			std::unique_ptr<M5TriCpuResources> mpM5TriCpuResources =
				std::make_unique<M5TriCpuResources>();

			// Place our verticies, that were allocated on the heap using "new", in 
			// unique pointers for safe keeping and "owership" control.
			mpM5TriCpuResources->mpVertexArray = std::unique_ptr<M5TriVertex>(vpM5TriVertex);
			// At this point, both mpVertexArray and vpM5TriVertex have the same heap address.
			// Since mpVertexArray now "owns" it, set vpM5TriVertex to nullptr to be extra careful. 
			// Note that setting it to nullptr is not really needed as long as we don't delete it.
			vpM5TriVertex = nullptr;
			mpM5TriCpuResources->mNumberOfVerticies = 3;

			// Place our indicies, that were allocated on the heap using "new", in 
			// unique pointers for safe keeping and "owership" control.
			mpM5TriCpuResources->mpIndexArray = std::unique_ptr<int16_t>(vpIndixies);
			vpIndixies = nullptr; // Set vpIndixies to nullptr to be extra careful.
			mpM5TriCpuResources->mNumberOfIndicies = 3;

			return mpM5TriCpuResources;

			// C++ Notes: The above M5TriVertex heap allocation code could have been done without 
			// using the "new" operator. 
			// For example, "std::make_unique<NewVector[]>(3)" could have been used. 
			// It was done this way to show the idea that variables that hold arrays on the heap 
			// are just holding a regular address; pointing at the first element in the array.
		}
		std::unique_ptr<M5TriCpuResources> M5TriFactory::GetCpuSimpleTriangle2(void)
		{
			// A triangles needs 3 vertex points.
			M5TriVertex* vpM5TriVertex = new M5TriVertex[4];

			// A triangle needs 3 index numbers to show the vertex points needed 
			// for a triangle.
			int16_t* vpIndixies = new int16_t[6];

			// Create the following triangle where "*" is local space 0,0,0.
			// Horizontal/Width is x and Up/Height is y. 
			// Also, the width and height are 1 unit; meaning the top left corner 
			// of the triangle is x(-0.5f) and y(0.5).
			//			 ----------.
			//			|        .
			//			|      .
			//			|    .* 
			//			|  .    
			//			|.

			// A 3D box requires multiple triangles for all the serfaces. 
			// Here, we are only drawing one triangle, like one size of a box.
			// Since we are drawing one triangle depth is not important so z
			// will be the same for all 3 verticies.
			float z = 1.0;

			// Fill in the front face vertex data. 
			// Left Bottom
			vpM5TriVertex[0] = M5TriVertex(-0.5f, -0.5f, z);

			// Left Top
			vpM5TriVertex[1] = M5TriVertex(-0.5f, +0.5f, z);

			// Right Top
			vpM5TriVertex[2] = M5TriVertex(+0.5f, +0.5f, z);

			vpM5TriVertex[3] = M5TriVertex(+1.57f, +2.7f, z);

			// Fill in Index data
			vpIndixies[0] = 0;
			vpIndixies[1] = 1;
			vpIndixies[2] = 2;

			vpIndixies[3] = 1;
			vpIndixies[4] = 3;
			vpIndixies[5] = 2;


			// Create the return object on the heap; using "std::make_unique" instead of "new".
			std::unique_ptr<M5TriCpuResources> mpM5TriCpuResources =
				std::make_unique<M5TriCpuResources>();

			// Place our verticies, that were allocated on the heap using "new", in 
			// unique pointers for safe keeping and "owership" control.
			mpM5TriCpuResources->mpVertexArray = std::unique_ptr<M5TriVertex>(vpM5TriVertex);
			// At this point, both mpVertexArray and vpM5TriVertex have the same heap address.
			// Since mpVertexArray now "owns" it, set vpM5TriVertex to nullptr to be extra careful. 
			// Note that setting it to nullptr is not really needed as long as we don't delete it.
			vpM5TriVertex = nullptr;
			mpM5TriCpuResources->mNumberOfVerticies = 4;

			// Place our indicies, that were allocated on the heap using "new", in 
			// unique pointers for safe keeping and "owership" control.
			mpM5TriCpuResources->mpIndexArray = std::unique_ptr<int16_t>(vpIndixies);
			vpIndixies = nullptr; // Set vpIndixies to nullptr to be extra careful.
			mpM5TriCpuResources->mNumberOfIndicies = 6;

			return mpM5TriCpuResources;

			// C++ Notes: The above M5TriVertex heap allocation code could have been done without 
			// using the "new" operator. 
			// For example, "std::make_unique<NewVector[]>(3)" could have been used. 
			// It was done this way to show the idea that variables that hold arrays on the heap 
			// are just holding a regular address; pointing at the first element in the array.
		}
	}
}