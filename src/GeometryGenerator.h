#ifndef _GEO_GENERATOR_H_
#define _GEO_GENERATOR_H_

#include <glm/glm.hpp>
#include <string>
#include <vector>

class GeometryGenerator
{
public:
	struct Vertex
	{
		Vertex(){}
		Vertex(const glm::vec3& p,const glm::vec3& n,const glm::vec3& t,const glm::vec2& tex)
			:position(p),normal(n),tangent(t),uv(tex){}

		Vertex(
			float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v)
			: position(px, py, pz), normal(nx, ny, nz),
			tangent(tx, ty, tz), uv(u, v) {}

		glm::vec3 position;
		glm::vec3 normal;
		glm::vec3 tangent;
		glm::vec2 uv;
	};

	struct MeshData
	{
		std::string name;
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
	};

	///<summary>
	/// Creates a box centered at the origin with the given dimensions.
	///</summary>
	void createBox(float width, float height, float depth, MeshData& meshData);

	///<summary>
	/// Creates a sphere centered at the origin with the given radius.  The
	/// slices and stacks parameters control the degree of tessellation.
	///</summary>
	void createSphere(float radius, unsigned int sliceCount, unsigned int stackCount, MeshData& meshData);

	///<summary>
	/// Creates a geosphere centered at the origin with the given radius.  The
	/// depth controls the level of tessellation.
	///</summary>
	void createGeosphere(float radius, unsigned int numSubdivisions, MeshData& meshData);

	///<summary>
	/// Creates a cylinder parallel to the y-axis, and centered about the origin.  
	/// The bottom and top radius can vary to form various cone shapes rather than true
	// cylinders.  The slices and stacks parameters control the degree of tessellation.
	///</summary>
	void createCylinder(float bottomRadius, float topRadius, float height, unsigned int sliceCount, unsigned int stackCount, MeshData& meshData);

	///<summary>
	/// Creates an mxn grid in the xz-plane with m rows and n columns, centered
	/// at the origin with the specified width and depth.
	///</summary>
	void createGrid(float width, float depth, unsigned int m, unsigned int n, MeshData& meshData);

	void createScreenQuad(MeshData& meshData, float k, glm::vec2 offset);

private:
	void subdivide(MeshData& meshData);
	void buildCylinderTopCap(float bottomRadius, float topRadius, float height, unsigned int sliceCount, unsigned int stackCount, MeshData& meshData);
	void buildCylinderBottomCap(float bottomRadius, float topRadius, float height, unsigned int sliceCount, unsigned int stackCount, MeshData& meshData);
};
#endif
