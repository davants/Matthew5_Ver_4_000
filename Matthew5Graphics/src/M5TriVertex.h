#pragma once

namespace Matthew5
{
	namespace Matthew5Graphics
	{
		// A triangle has 3 vertex points. This class, M5TriVertex, is one Vertex.
		// Therefore, you must instantiate 3 of these classes, into objects, to make up one triangle.
		class M5TriVertex
		{
		public:
			M5TriVertex(float vPositionX, float vPositionY, float vPositionZ);
			M5TriVertex(void);

			float mPositionX, mPositionY, mPositionZ = 0.0f;
		};
	}
}