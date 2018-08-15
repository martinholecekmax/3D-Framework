#include "Model.h"

Model::Model()
{

}

Model::~Model()
{
}

std::vector<Polygon3D>& Model::GetPolygons()
{
	return _polygons;
}

std::vector<Vertex>& Model::GetVertices()
{
	return _transformedVertices;
}

std::vector<UVCoordinates>& Model::GetUVCoordinates()
{
	return _uvCoordinates;
}

size_t Model::GetPolygonCount() const
{
	return _polygons.size();
}

size_t Model::GetVertexCount() const
{
	return _vertices.size();
}

Texture & Model::GetTexture()
{
	return _texture;
}

void Model::AddVertex(float x, float y, float z)
{
	_vertices.emplace_back(x, y, z);
	_transformedVertices.emplace_back(x, y, z);
}

void Model::AddPolygon(int i0, int i1, int i2, int uvIndex0, int uvIndex1, int uvIndex2)
{
	_polygons.emplace_back(i0, i1, i2, uvIndex0, uvIndex1, uvIndex2);
}

void Model::AddTextureUV(float u, float v)
{
	_uvCoordinates.emplace_back(u, v);
}

// Apply transformation to original model from MD2 file and store result
// in transformed vertices collection
void Model::ApplyTransformToLocalVertices(const Matrix4x4 & transform)
{
	size_t transformVerticesSize = _transformedVertices.size();
	for (size_t i = 0; i < transformVerticesSize; i++)
	{
		_transformedVertices[i] = transform * _vertices[i];
	}
}

// Apply transformation to all vertices in the transformed vertices collection
void Model::ApplyTransformToTransformedVertices(const Matrix4x4 & transform)
{
	size_t transformVerticesSize = _transformedVertices.size();
	for (size_t i = 0; i < transformVerticesSize; i++)
	{
		_transformedVertices[i] = transform * _transformedVertices[i];
	}
}

void Model::DehomogenizeModel()
{
	size_t transformVerticesSize = _transformedVertices.size();
	for (size_t i = 0; i < transformVerticesSize; i++)
	{
		_transformedVertices[i].Dehomogenize();
	}
}

void Model::CalculateBackFaces(Vertex cameraPosition)
{
	size_t polygonSize = _polygons.size();
	for (int i = 0; i < polygonSize; i++)
	{
		// Get vertices of the polygons
		const Vertex vertex0 = Vertex(_transformedVertices[_polygons[i].GetIndex(0)]);
		const Vertex vertex1 = Vertex(_transformedVertices[_polygons[i].GetIndex(1)]);
		const Vertex vertex2 = Vertex(_transformedVertices[_polygons[i].GetIndex(2)]);

		// Construct vector A by subtracting vertex 1 from vertex 0
		const Vector3D vectorA = vertex1 - vertex0;

		// Construct vector B by subtracting vertex 2 from vertex 0
		const Vector3D vectorB = vertex2 - vertex0;

		// Calculate normal from vector B and A
		// (v1 - v0) X (v2 - v0) <= order matters
		_polygons[i].SetNormal(Vector3D::Cross(vectorA, vectorB));

		// Create eye-vector = vertex 0 - camera position
		const Vector3D eyeVector = vertex0 - cameraPosition;

		// Take dot product of the normal and eye-vector
		// If result is less then 0 mark polygon for culling
		if(Vector3D::DotProduct(_polygons[i].GetNormal().GetNormalizeVec(), eyeVector.GetNormalizeVec()) > 0.0f)
		{
			_polygons[i].SetCullFlag(true);
		}
		else
		{
			_polygons[i].SetCullFlag(false);
		}
	}
}

void Model::Sort()
{
	size_t polygonSize = _polygons.size();
	for (size_t i = 0; i < polygonSize; i++)
	{
		// Calculate an average z depth for the polygon vertices
		//float average = (vertex0.GetZ() + vertex1.GetZ() + vertex2.GetZ()) / 3;
		// Store this value back in the Polygon
		_polygons[i].SetDepthValue(
			(_transformedVertices[_polygons[i].GetIndex(0)].GetZ() +
			_transformedVertices[_polygons[i].GetIndex(1)].GetZ() +
			_transformedVertices[_polygons[i].GetIndex(2)].GetZ()) / 3.0f);
	}

	// Sort the polygon collection array to put furthest away first
	std::sort(_polygons.begin(), _polygons.end());
}

// Call this method before CalculateLightDirectional and CalculateLightingPoint method
void Model::CalculateLightAmbient(AmbientLight ambientLight)
{
	//// Equation to calculate Ambient light: ////
	// Iamb = ka * Ia
	// Ia – intensity (direction of the incident light)
	// Ka – reflection coefficient (ambient reflectance of the material)

	float total[3] = { 0, 0, 0 };

	// Iterate through all polygons and get color
	size_t polygonSize = _polygons.size();
	for (size_t i = 0; i < polygonSize; i++)
	{
		// Set temp to light intensity from directionalLight class ( Id - intensity from equation )
		total[0] = (float)(ambientLight.GetLightIntensity().GetRed());
		total[1] = (float)((ambientLight.GetLightIntensity().GetGreen()));
		total[2] = (float)((ambientLight.GetLightIntensity().GetBlue()));

		// Multiply by temp diffuse reflectance in model class ( Kd - reflection coefficient from equation )
		total[0] *= _reflectanceCoefficients.GetAmbientReflectanceCoefficients()[0];
		total[1] *= _reflectanceCoefficients.GetAmbientReflectanceCoefficients()[1];
		total[2] *= _reflectanceCoefficients.GetAmbientReflectanceCoefficients()[2];

		// Clamp total r,g,b values
		total[0] = total[0] < 0 ? 0 : (total[0] > 256 ? 255 : total[0]);
		total[1] = total[1] < 0 ? 0 : (total[1] > 256 ? 255 : total[1]);
		total[2] = total[2] < 0 ? 0 : (total[2] > 256 ? 255 : total[2]);

		// Store color in polygon class
		_polygons[i].SetColor((int)total[0], (int)total[1], (int)total[2]);
	}
}

void Model::CalculateLightDirectional(const std::vector<DirectionalLight> directionalLight)
{
	float total[3] = { 0, 0, 0 };
	float temp[3] = { 0, 0, 0 };

	// Iterate through all polygons and get color
	size_t polygonSize = _polygons.size();
	for (size_t i = 0; i < polygonSize; i++)
	{
		// Set total to black color
		total[0] = float(_polygons[i].GetColor() & 0xFF);
		total[1] = float((_polygons[i].GetColor() >> 8) & 0xFF);
		total[2] = float((_polygons[i].GetColor() >> 16) & 0xFF);
		
		// Loop through directional light collection
		size_t dirLight = directionalLight.size();
		for (size_t y = 0; y < dirLight; y++)
		{
			//// Equation to calculate directional light ////
			// Idirection = Kd * Id * ( L dotproduct N ) 
			// Id - intensity, Kd - reflection coefficient, 
			// N - unit normal to the surface, L - unit vector to the light source

			// Set temp to light intensity from directionalLight class ( Id - intensity from equation )
			temp[0] = (float)(directionalLight[y].GetLightIntensity().GetRed());
			temp[1] = (float)((directionalLight[y].GetLightIntensity().GetGreen()));
			temp[2] = (float)((directionalLight[y].GetLightIntensity().GetBlue()));

			// Multiply by temp diffuse reflectance in model class ( Kd - reflection coefficient from equation )
			temp[0] *= _reflectanceCoefficients.GetDiffuseReflectanceCoefficients()[0];
			temp[1] *= _reflectanceCoefficients.GetDiffuseReflectanceCoefficients()[1];
			temp[2] *= _reflectanceCoefficients.GetDiffuseReflectanceCoefficients()[2];

			//// Calculate dot product of normal vector light source and normal vector in polygon class ////

			// ( N - unit normal to the surface from equation )
			const Vector3D polygonsNormal = _polygons[i].GetNormal().GetNormalizeVec();

			// ( L - unit vector to the light source from equation )
			const Vector3D lightNormal = directionalLight[y].GetDirection();

			// Dot product of the light source with normal to the surface ( L dot N )
			const float dotProd = Vector3D::DotProduct(lightNormal, polygonsNormal);

			// Multiply temp by dot product
			temp[0] *= dotProd;
			temp[1] *= dotProd;
			temp[2] *= dotProd;
			     
			// Add temp to total
			total[0] += temp[0];
			total[1] += temp[1];
			total[2] += temp[2];
		}
		// Clamp total r,g,b values
		total[0] = total[0] < 0 ? 0 : (total[0] > 256 ? 255 : total[0]);
		total[1] = total[1] < 0 ? 0 : (total[1] > 256 ? 255 : total[1]);
		total[2] = total[2] < 0 ? 0 : (total[2] > 256 ? 255 : total[2]);
		
		// Store color in polygon class
		_polygons[i].SetColor((int)total[0], (int)total[1], (int)total[2]);
	}
}

void Model::CalculateLightDirectionalSmooth(const std::vector<DirectionalLight> directionalLight)
{
	float total[3] = { 0, 0, 0 };
	float temp[3] = { 0, 0, 0 };

	// Iterate through all vertices and get color
	size_t transformedVerticesSize = _transformedVertices.size();
	for (size_t i = 0; i < transformedVerticesSize; i++)
	{
		// Set total to black color
		total[0] = float(_transformedVertices[i].GetColor().GetRed());
		total[1] = float((_transformedVertices[i].GetColor().GetGreen()));
		total[2] = float((_transformedVertices[i].GetColor().GetBlue()));

		// Loop through directional light collection
		size_t dirLight = directionalLight.size();
		for (size_t y = 0; y < dirLight; y++)
		{
			//// Equation to calculate directional light ////
			// Idirection = Kd * Id * ( L dotproduct N ) 
			// Id - intensity, Kd - reflection coefficient, 
			// N - unit normal to the surface, L - unit vector to the light source

			// Set temp to light intensity from directionalLight class ( Id - intensity from equation )
			temp[0] = (float)(directionalLight[y].GetLightIntensity().GetRed());
			temp[1] = (float)((directionalLight[y].GetLightIntensity().GetGreen()));
			temp[2] = (float)((directionalLight[y].GetLightIntensity().GetBlue()));

			// Multiply by temp diffuse reflectance in model class ( Kd - reflection coefficient from equation )
			temp[0] *= _reflectanceCoefficients.GetDiffuseReflectanceCoefficients()[0];
			temp[1] *= _reflectanceCoefficients.GetDiffuseReflectanceCoefficients()[1];
			temp[2] *= _reflectanceCoefficients.GetDiffuseReflectanceCoefficients()[2];

			//// Calculate dot product of normal vector light source and normal vector in polygon class ////

			// ( N - unit normal to the surface from equation )
			const Vector3D transformVerticesNormal = _transformedVertices[i].GetNormal().GetNormalizeVec();

			// ( L - unit vector to the light source from equation )
			const Vector3D lightNormal = directionalLight[y].GetDirection();

			// Dot product of the light source with normal to the surface ( L dot N )
			const float dotProd = Vector3D::DotProduct(lightNormal, transformVerticesNormal);

			// Multiply temp by dot product
			temp[0] *= dotProd;
			temp[1] *= dotProd;
			temp[2] *= dotProd;

			// Add temp to total
			total[0] += temp[0];
			total[1] += temp[1];
			total[2] += temp[2];
		}
		// Clamp total r,g,b values
		total[0] = total[0] < 0 ? 0 : (total[0] > 256 ? 255 : total[0]);
		total[1] = total[1] < 0 ? 0 : (total[1] > 256 ? 255 : total[1]);
		total[2] = total[2] < 0 ? 0 : (total[2] > 256 ? 255 : total[2]);
		
		// Store color in polygon class
		_transformedVertices[i].SetColor((int)total[0], (int)total[1], (int)total[2]);
	}
}

void Model::CalculateLightingPoint(const std::vector<PointLight> pointLight)
{
	float total[3] = { 0, 0, 0 };
	float temp[3] = { 0, 0, 0 };

	// Iterate through all polygons and get color
	size_t polygonSize = _polygons.size();
	for (size_t i = 0; i < polygonSize; i++)
	{
		// Set total to black color
		total[0] = float(_polygons[i].GetColor() & 0xFF);
		total[1] = float((_polygons[i].GetColor() >> 8) & 0xFF);
		total[2] = float((_polygons[i].GetColor() >> 16) & 0xFF);
		
		// Loop through directional light collection
		size_t pointLightSize = pointLight.size();
		for (size_t y = 0; y < pointLightSize; y++)
		{
			//// Equation to calculate point light ////
			// Idirection = Kd * Id * ( L dotproduct N ) * Attenuation
			// Id - intensity, Kd - reflection coefficient, 
			// N - unit normal to the surface, L - unit vector to the light source

			// Set temp to light intensity from PointLight class ( Ip - intensity from equation )
			temp[0] = (float)(pointLight[y].GetLightIntensity().GetRed());
			temp[1] = (float)((pointLight[y].GetLightIntensity().GetGreen()));
			temp[2] = (float)((pointLight[y].GetLightIntensity().GetBlue()));

			// Multiply by temp diffuse reflectance in model class ( Kd - reflection coefficient from equation )
			temp[0] *= _reflectanceCoefficients.GetDiffuseReflectanceCoefficients()[0];
			temp[1] *= _reflectanceCoefficients.GetDiffuseReflectanceCoefficients()[1];
			temp[2] *= _reflectanceCoefficients.GetDiffuseReflectanceCoefficients()[2];

			// Get first vertex in the polygon
			const Vector3D vecFirstVertex(
				_transformedVertices[_polygons[i].GetIndex(0)].GetX(),
				_transformedVertices[_polygons[i].GetIndex(0)].GetY(),
				_transformedVertices[_polygons[i].GetIndex(0)].GetZ());

			// Light position
			const Vector3D lightPosition = pointLight[y].GetPosition();

			// Calculate Vector to the light source
			// Subtract position of the light source from position of the first vertex
			// Vector to the light source = Light position - Vertex 0
			//Vector3D lightDirection = vecFirstVertex - lightPosition;
			Vector3D lightDirection = lightPosition - vecFirstVertex;

			const float distance = lightDirection.Length();

			// Get Attenuation coefficients
			const float coefficientA = pointLight[y].GetAttenuation().a;
			const float coefficientB = pointLight[y].GetAttenuation().b;
			const float coefficientC = pointLight[y].GetAttenuation().c;

			// Calculate Attenuation
			const float attenuation = 1.0f / (coefficientA + coefficientB * distance + coefficientC * (distance * distance));
				
			// Multiply color by attenuation
			temp[0] *= attenuation;
			temp[1] *= attenuation;
			temp[2] *= attenuation;

			// ( N - unit normal to the surface from equation )
			const Vector3D polygonsNormal = _polygons[i].GetNormal().GetNormalizeVec();

			const Vector3D lightNormalized = lightPosition.GetNormalizeVec();

			// Dot product of the light source with normal to the surface ( L dot N )
			const float dotProd = Vector3D::DotProduct(lightNormalized, polygonsNormal);
			//float diffuse = dotProd < 0.0f ? 0.0f : dotProd;
			//using namespace std;
			const float diffuse = max(dotProd, 0.0f);
			//float diffuse = dotProd;

			// Multiply temp by diffuese product
			temp[0] *= diffuse;
			temp[1] *= diffuse;
			temp[2] *= diffuse;

			// Add temp to total
			total[0] += temp[0];
			total[1] += temp[1];
			total[2] += temp[2];
		}
		// Clamp total r,g,b values
		total[0] = total[0] < 0 ? 0 : (total[0] > 256 ? 255 : total[0]);
		total[1] = total[1] < 0 ? 0 : (total[1] > 256 ? 255 : total[1]);
		total[2] = total[2] < 0 ? 0 : (total[2] > 256 ? 255 : total[2]);

		// Store color in polygon class
		_polygons[i].SetColor((int)total[0], (int)total[1], (int)total[2]);
	}
}

void Model::CalculateNormalVertices()
{
	size_t transformVerticesSize = _transformedVertices.size();
	for (size_t i = 0; i < transformVerticesSize; i++)
	{
		_transformedVertices[i].SetNormal(Vector3D());
		_transformedVertices[i].SetCountRelatedPolygons(0);
	}

	size_t polygonSize = _polygons.size();
	for (size_t i = 0; i < polygonSize; i++)
	{
		for (int y = 0; y < 3; y++)
		{
			_transformedVertices[_polygons[i].GetIndex(y)].AddNormal(_polygons[i].GetNormal());
			_transformedVertices[_polygons[i].GetIndex(y)].AddPolygonCounter();
		}
	}

	for (size_t i = 0; i < transformVerticesSize; i++)
	{
		_transformedVertices[i].SetNormal(
			(_transformedVertices[i].GetNormal() / (float)_transformedVertices[i].GetCountRelatedPolygons()));
	}
}

