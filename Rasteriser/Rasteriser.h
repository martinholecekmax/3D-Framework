#pragma once
#include <vector>
#include <ctime>

#include "Framework.h"
#include "Vertex.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"
#include "MD2Loader.h"
#include "Camera.h"
#include "Model.h"
#include "Color.h"
#include "UVCoordinates.h"

class Rasteriser : public Framework
{
public:
	Rasteriser();
	bool Initialise();

	void Update(Bitmap &bitmap);
	void Render(Bitmap &bitmap);

	// Draw model and axis to the screen
	void DrawAxis(Bitmap &bitmap);
	void DrawStateText(Bitmap &bitmap);
	void DrawWireFrame(Bitmap &bitmap);
	void DrawSolidFlat(Bitmap &bitmap);

	// Solid Flat model using set pixel function
	void MyDrawSolidFlat(Bitmap &bitmap);
	void FillPolygonFlat(HDC hdc, const Vertex& vertex0, const Vertex& vertex1, const Vertex& vertex2, Color color);
	void DrawFlatTopTriangle(HDC hdc, const Vertex& vertex0, const Vertex& vertex1, const Vertex& vertex2, Color color);
	void DrawFlatBottomTriangle(HDC hdc, const Vertex& vertex0, const Vertex& vertex1, const Vertex& vertex2, Color color);

	// Gouraud Shading
	void DrawGouraud(Bitmap & bitmap);
	void FillPolygonGouraud(HDC hdc, const Vertex& vertex0, const Vertex& vertex1, const Vertex& vertex2, Color colorVer0, Color colorVer1, Color colorVer2);
	void DrawGouraudTopTriangle(HDC hdc, const Vertex& vertex0, const Vertex& vertex1, const Vertex& vertex2, Color colorVer0, Color colorVer1, Color colorVer2);
	void DrawGouraudBottomTriangle(HDC hdc, const Vertex& vertex0, const Vertex& vertex1, const Vertex& vertex2, Color colorVer0, Color colorVer1, Color colorVer2);

	// Texture
	void DrawSolidTextured(Bitmap &bitmap);
	void FillSolidTextured(HDC hdc, const Vertex& vertex0, const Vertex& vertex1, const Vertex& vertex2, const Color colorVer0, const Color colorVer1, const Color colorVer2);
	void DrawTextureTopTriangle(HDC hdc, const Vertex& vertex0, const Vertex& vertex1, const Vertex& vertex2, const Color colorVer0, const Color colorVer1, const Color colorVer2, const UVCoordinates uv0, const UVCoordinates uv1, const UVCoordinates uv2);
	void DrawTextureBottomTriangle(HDC hdc, const Vertex& vertex0, const Vertex& vertex1, const Vertex& vertex2, const Color colorVer0, const Color colorVer1, const Color colorVer2, const UVCoordinates uv0, const UVCoordinates uv1, const UVCoordinates uv2);

	// Key listeners
	void KeyPressed(WPARAM wParam);
	void ChangeScene();
	void ChangeModelPhase();
	bool TimeProcessing(int seconds);

	// Generate Perspective and Screen matrix
	void GeneratePerspectiveMatrix(float d, float aspectRatio);
	void GenerateViewMatrix(float d, int width, int height);

private:
	// Scenes Controls
	enum State 
	{ 
		WIREFRAME, 
		SOLID_FLAT,
		MY_SOLID, 
		GOURAUD_SHADING, 
		TEXTURE,
		MODEL_PHASES
	};

	// Applying features to the model
	enum ModelPhases 
	{
		DIRECTIONAL_LIGHT_X,
		DIRECTIONAL_LIGHT_Y,
		DIRECTIONAL_LIGHT_Z,
		POINT_LIGHT,
		MODEL_ROTATION_X,
		MODEL_ROTATION_Y,
		MODEL_ROTATION_Z,
		MODEL_TRANSLATION_X_LEFT,
		MODEL_TRANSLATION_X_RIGHT,
		MODEL_TRANSLATION_Y_DOWN,
		MODEL_TRANSLATION_Y_UP,
		MODEL_SCALE_DOWN,
		MODEL_SCALE_UP
	};

	State _state = WIREFRAME;

	ModelPhases _modelPhase = DIRECTIONAL_LIGHT_X;

	long int _startTime = (long int)time(NULL);
	long int _endTime = (long int)time(NULL);
	long int _timeDifference = 0;

	const int PHASE_DURATION = 4;
	const int ROTATE_DURATION = 3;
	const int SCENE_DURATION = 2;

	float _rotationAngle = 0.06f;
	float _rotationMeasurement = 0;

	// Full rotation or complete turn in radians
	float _revolutionAngle = 6.28319f;

	// Teapod
	const float SCALING_FACTOR_X = 2.90f;
	const float SCALING_FACTOR_Y = 2.90f;
	const float SCALING_FACTOR_Z = 2.90f;

	//// Marvin and cube
	//const float SCALING_FACTOR_X = 0.5f;
	//const float SCALING_FACTOR_Y = 0.5f;
	//const float SCALING_FACTOR_Z = 0.5f;

	float _focalDistance = 2.0f;
	int _gridSize = 50;

	Camera _camera;
	Model _model;

	// Transformations
	Matrix4x4 _modelTransformation = Matrix4x4::Identity();
	Matrix4x4 _screenTransform = Matrix4x4::Identity();
	Matrix4x4 _perspectiveTransform = Matrix4x4::Identity();

	Vertex _origin = Vertex(0.0f, 0.0f, 0.0f);

	// Lights
	AmbientLight _ambientLight;
	std::vector<DirectionalLight> _directionalLights;
	std::vector<PointLight> _pointLights;

	// Ambient Light
	int _ambLightRed = 10;
	int _ambLightGreen = 10;
	int _ambLightBlue = 10;

	// Directional light
	float _dirLightX = 0.0f;
	float _dirLightY = 0.0f;
	float _dirLightZ = -1.0f;
	int _dirLightIntensityRed = 240;
	int _dirLightIntensityGreen = 0;
	int _dirLightIntensityBlue = 0;

	// Point Light
	float _pointLightX = 2.0f;
	float _pointLightY = 2.0f;
	float _pointLightZ = -5.0f;
	int _pointLightIntensityRed = 200;
	int _pointLightIntensityGreen = 200;
	int _pointLightIntensityBlue = 200;
	Attenuation _attenuation = { 0.0f, 1.0f, 0.0f };

	// Different Models
	//std::string modelName = "MD2//cube.md2";
	//std::string modelName = "MD2//monkey.md2";
	//std::string modelName = "MD2//policecar.md2";
	std::string modelName = "MD2//teapot.md2";
	//std::string modelName = "MD2//van.md2";
	//std::string modelName = "MD2//marvin.md2";
	//std::string modelName = "MD2//kenny.md2";
	//std::string modelName = "MD2//helicopter.md2";
	//std::string modelName = "MD2//fighter.md2";
	//std::string modelName = "MD2//cow.md2";
	//std::string modelName = "MD2//chef.md2";
	//std::string modelName = "MD2//cartman.md2";
	//std::string modelName = "MD2//car.md2";
	//std::string modelName = "MD2//hummer.md2";

	// Texture for cube and marvin
	std::string textureName = "Textures//lines.pcx";
	//std::string textureName = "Textures//marvin.pcx";
};

