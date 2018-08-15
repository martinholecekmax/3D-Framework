#include "Rasteriser.h"

Rasteriser app;

Rasteriser::Rasteriser()
{
	
}

bool Rasteriser::Initialise()
{
	// Load model from the file
	if (!MD2Loader::LoadModel(modelName.c_str(), textureName.c_str(), _model, &Model::AddPolygon, &Model::AddVertex, &Model::AddTextureUV))
	{
		return false;
	}

	// Correction of the model initial scale factor
	_modelTransformation = Matrix4x4::Scaling(SCALING_FACTOR_X, SCALING_FACTOR_Y, SCALING_FACTOR_Z) * _modelTransformation;
	
	// Adding Camera
	Vertex cameraPosition(0.0f, 0.0f, -50.0f);
	_camera = Camera(0.0f, 0.0f, 0.0f, cameraPosition);

	// Adding Lights
	_ambientLight = AmbientLight(_ambLightRed, _ambLightGreen, _ambLightBlue);

	// Directional Light
	Vector3D directionalLightDirection(_dirLightX, _dirLightY, _dirLightZ);
	_directionalLights.push_back(DirectionalLight(_dirLightIntensityRed, _dirLightIntensityGreen, 
		_dirLightIntensityBlue, directionalLightDirection));
	
	// Point Light
	Vector3D pointLightPosition(_pointLightX, _pointLightY, _pointLightZ);
	_pointLights.push_back(PointLight(_pointLightIntensityRed, _pointLightIntensityGreen, 
		_pointLightIntensityBlue, pointLightPosition, _attenuation));

	return true;
}

void Rasteriser::Update(Bitmap &bitmap)
{
	// Rotate model around X, Y and Z axis
	if (_modelPhase == MODEL_ROTATION_X)
	{
		if (_rotationMeasurement <= _revolutionAngle)
		{
			_modelTransformation = Matrix4x4::RotationX(_rotationAngle) * _modelTransformation;
			_rotationMeasurement += _rotationAngle;
		}
	}
	
	if (_modelPhase == MODEL_ROTATION_Y)
	{
		if (_rotationMeasurement <= _revolutionAngle)
		{
			_modelTransformation = Matrix4x4::RotationY(_rotationAngle) * _modelTransformation;
			_rotationMeasurement += _rotationAngle;
		}
	}

	if (_modelPhase == MODEL_ROTATION_Z)
	{
		if (_rotationMeasurement <= _revolutionAngle)
		{
			_modelTransformation = Matrix4x4::RotationZ(_rotationAngle) * _modelTransformation;
			_rotationMeasurement += _rotationAngle;
		}
	}

	if (_state != MODEL_PHASES)
	{
		_modelTransformation = Matrix4x4::RotationY(0.030f) * _modelTransformation;
	}

	// Translate model around X, Y and Z axis
	if (_modelPhase == MODEL_TRANSLATION_X_LEFT)
	{
		_modelTransformation = Matrix4x4::Translate(0.3f, 0.0f, 0.0f) * _modelTransformation;
	}

	if (_modelPhase == MODEL_TRANSLATION_X_RIGHT)
	{
		_modelTransformation = Matrix4x4::Translate(-0.3f, 0.0f, 0.0f) * _modelTransformation;
	}

	if (_modelPhase == MODEL_TRANSLATION_Y_DOWN)
	{
		_modelTransformation = Matrix4x4::Translate(0.0f, -0.3f, 0.0f) * _modelTransformation;
	}

	if (_modelPhase == MODEL_TRANSLATION_Y_UP)
	{
		_modelTransformation = Matrix4x4::Translate(0.0f, 0.3f, 0.0f) * _modelTransformation;
	}

	// Scale model up and down
	if (_modelPhase == MODEL_SCALE_UP)
	{
		_camera.SetCameraPosition(Matrix4x4::Translate(0.0f, 0.0f, 2.0f) * _camera.GetPosition());
	}

	if (_modelPhase == MODEL_SCALE_DOWN)
	{
		_camera.SetCameraPosition(Matrix4x4::Translate(0.0f, 0.0f, -2.0f) * _camera.GetPosition());
	}

	// Generate projection and sreen matrices
	GeneratePerspectiveMatrix(_focalDistance, (float)((float)bitmap.GetWidth() / bitmap.GetHeight()));
	GenerateViewMatrix(_focalDistance, bitmap.GetWidth(), bitmap.GetHeight());
}

void Rasteriser::Render(Bitmap &bitmap)
{
	HDC hDc = bitmap.GetDC();

	// Set origin in the middle of the screen
	_origin.SetX(((float)bitmap.GetWidth() / 2));
	_origin.SetY(((float)bitmap.GetHeight() / 2));
	_origin.SetZ(0.0f);

	// Clear the window to black
	bitmap.Clear(RGB(0, 0, 0));

	// Draw X and Y axis, and Cube
	DrawAxis(bitmap);
	
	DrawStateText(bitmap);

	// World Transformation
	_model.ApplyTransformToLocalVertices(_modelTransformation);

	// Apply camera transformation to the model
	_model.CalculateBackFaces(_camera.GetPosition());

	// Sort depth values of each triangle
	_model.Sort();

	_model.CalculateNormalVertices();

	// Calculate Ambient Light
	_model.CalculateLightAmbient(_ambientLight);

	// Calculate Directional Light
	if (_state == SOLID_FLAT || _state == MY_SOLID || _state == MODEL_PHASES)
	{
		_model.CalculateLightDirectional(_directionalLights);
	}

	// Calculate Directional Light for Gourand Shading
	if (_state == GOURAUD_SHADING || _state == TEXTURE)
	{
		_model.CalculateLightDirectionalSmooth(_directionalLights);
	}

	// Calculate Point Light
	if (_modelPhase == POINT_LIGHT)
	{
		_model.CalculateLightingPoint(_pointLights);
	}

	// Camera Transformation
	_model.ApplyTransformToTransformedVertices(_camera.GetView());

	// Perspective Transformation
	_model.ApplyTransformToTransformedVertices(_perspectiveTransform);
	
	// Dehomogenize the transformed vertices
	_model.DehomogenizeModel();

	// Apply the View (Screen) Transformation on transformed vertices in the model
	_model.ApplyTransformToTransformedVertices(_screenTransform);
	
	// Draw wireframe
	if (_state == WIREFRAME)
	{
		DrawWireFrame(bitmap);
	}

	// Draw solid flat
	if (_state == SOLID_FLAT || _state == MODEL_PHASES)
	{
		DrawSolidFlat(bitmap);
	}

	// Draw solid flat with setpixel method
	if (_state == MY_SOLID)
	{
		MyDrawSolidFlat(bitmap);
	}

	// Draw solid with Gourand shading
	if (_state == GOURAUD_SHADING)
	{
		DrawGouraud(bitmap);
	}

	// Draw Textured model
	if (_state == TEXTURE)
	{
		ChangeScene();
		//DrawSolidTextured(bitmap);
	}
	
	// Change Scene
	if (_state != MODEL_PHASES)
	{
		if (TimeProcessing(PHASE_DURATION))
		{
			ChangeScene();
		}
	}
	else if(_modelPhase == MODEL_ROTATION_X || _modelPhase == MODEL_ROTATION_Y || _modelPhase == MODEL_ROTATION_Z)
	{
		if (TimeProcessing(ROTATE_DURATION))
		{
			ChangeScene();
		}
	}
	else
	{
		if (TimeProcessing(SCENE_DURATION))
		{
			ChangeScene();
		}
	}
}

void Rasteriser::DrawAxis(Bitmap &bitmap)
{
	HDC hDc = bitmap.GetDC();

	HPEN hPen, hPenGrey;
	hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	hPenGrey = CreatePen(PS_SOLID, 1, RGB(93, 93, 93));

	// Select Pen for Drawing Axis X and Y
	SelectObject(hDc, hPen);

	// Draw Y axis
	MoveToEx(hDc, bitmap.GetWidth() / 2, 0, NULL);
	LineTo(hDc, bitmap.GetWidth() / 2, bitmap.GetHeight());

	// Draw X axis
	MoveToEx(hDc, 0, bitmap.GetHeight() / 2, NULL);
	LineTo(hDc, bitmap.GetWidth(), bitmap.GetHeight() / 2);

	DeleteObject(hPen);

	// Draw Grid
	SelectObject(hDc, hPenGrey);

	for (int i = 0; i < 20; i++)
	{
		MoveToEx(hDc, bitmap.GetWidth() / 2 + (_gridSize * i) + _gridSize, 0, NULL);
		LineTo(hDc, bitmap.GetWidth() / 2 + (_gridSize * i) + _gridSize, bitmap.GetHeight());

		MoveToEx(hDc, 0, bitmap.GetHeight() / 2 + (_gridSize * i) + _gridSize, NULL);
		LineTo(hDc, bitmap.GetWidth(), bitmap.GetHeight() / 2 + (_gridSize * i) + _gridSize);

		MoveToEx(hDc, bitmap.GetWidth() / 2 + (-_gridSize * i) - _gridSize, 0, NULL);
		LineTo(hDc, bitmap.GetWidth() / 2 + (-_gridSize * i) - _gridSize, bitmap.GetHeight());

		MoveToEx(hDc, 0, bitmap.GetHeight() / 2 + (-_gridSize * i) - _gridSize, NULL);
		LineTo(hDc, bitmap.GetWidth(), bitmap.GetHeight() / 2 + (-_gridSize * i) - _gridSize);
	}

	DeleteObject(hPenGrey);

	// Draw Coordinates Text form
	SetTextColor(hDc, RGB(255, 255, 255));
	SetBkColor(hDc, RGB(0, 0, 0));

	// Coordinates
	TCHAR text[] = L"(0,0)";
	TextOut(hDc, (int)_origin.GetX() + 5, (int)_origin.GetY() + 5, text, ARRAYSIZE(text));
	
	TCHAR textX[] = L"X";
	TextOut(hDc, (int)bitmap.GetWidth() - 20, (int)_origin.GetY() + 5, textX, ARRAYSIZE(textX));
	
	TCHAR textNegX[] = L"- X";
	TextOut(hDc, 10, (int)_origin.GetY() + 5, textNegX, ARRAYSIZE(textNegX));

	TCHAR textY[] = L"Y";
	TextOut(hDc, (int)_origin.GetX() + 15, 10, textY, ARRAYSIZE(textY));
	
	TCHAR textNegY[] = L"- Y";
	TextOut(hDc, (int)_origin.GetX() + 10, (int)bitmap.GetHeight() - 25, textNegY, ARRAYSIZE(textNegY));
}

void Rasteriser::DrawStateText( Bitmap & bitmap)
{
	HDC hDc = bitmap.GetDC();

	SetTextColor(hDc, RGB(255, 255, 255));
	SetBkColor(hDc, RGB(0, 0, 0));

	HFONT hFontHeader, hOldFont, hFontSubheader;

	// Retrieve a handle to the variable stock font.  
	hFontHeader = CreateFont(48, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Header"));

	hFontSubheader = CreateFont(24, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Subheader"));

	// Draw Header
	if (hOldFont = (HFONT)SelectObject(hDc, hFontHeader))
	{
		if (_state == WIREFRAME)
		{
			std::wstring text = L"Wireframe";
			TextOut(hDc, 10, 10, text.c_str(), (int)text.length());
		}
		else if (_state == SOLID_FLAT || _state == MODEL_PHASES)
		{
			std::wstring text = L"Solid Flat";
			TextOut(hDc, 10, 10, text.c_str(), (int)text.length());
		}
		else if (_state == MY_SOLID)
		{
			std::wstring text = L"My Solid Flat";
			TextOut(hDc, 10, 10, text.c_str(), (int)text.length());
		}
		else if (_state == GOURAUD_SHADING)
		{
			std::wstring text = L"Gouraud Shading";
			TextOut(hDc, 10, 10, text.c_str(), (int)text.length());
		}
		else if (_state == TEXTURE)
		{
			std::wstring text = L"Applying Texture";
			TextOut(hDc, 10, 10, text.c_str(), (int)text.length());
		}
	}
	DeleteObject(hFontHeader);

	// Draw Subheader
	if (hOldFont = (HFONT)SelectObject(hDc, hFontSubheader))
	{
		if (_state == WIREFRAME)
		{
			std::wstring description = L"Backface Culling";
			TextOut(hDc, 10, 70, description.c_str(), (int)description.length());
		}

		if (_modelPhase == DIRECTIONAL_LIGHT_X && _state == MODEL_PHASES)
		{
			std::wstring description = L"Directional Light X direction";
			TextOut(hDc, 10, 70, description.c_str(), (int)description.length());
		}
		else if (_modelPhase == DIRECTIONAL_LIGHT_Y)
		{
			std::wstring description = L"Directional Light Y direction";
			TextOut(hDc, 10, 70, description.c_str(), (int)description.length());
		}
		else if (_modelPhase == DIRECTIONAL_LIGHT_Z)
		{
			std::wstring description = L"Directional Light Z direction";
			TextOut(hDc, 10, 70, description.c_str(), (int)description.length());
		}
		else if (_modelPhase == POINT_LIGHT)
		{
			std::wstring description = L"Point Light Top-Right";
			TextOut(hDc, 10, 70, description.c_str(), (int)description.length());
		}
		else if (_modelPhase == MODEL_ROTATION_X)
		{
			std::wstring description = L"Rotate model around X axis";
			TextOut(hDc, 10, 70, description.c_str(), (int)description.length());
		}
		else if (_modelPhase == MODEL_ROTATION_Y)
		{
			std::wstring description = L"Rotate model around Y axis";
			TextOut(hDc, 10, 70, description.c_str(), (int)description.length());
		}
		else if (_modelPhase == MODEL_ROTATION_Z)
		{
			std::wstring description = L"Rotate model around Z axis";
			TextOut(hDc, 10, 70, description.c_str(), (int)description.length());
		}
		else if (_modelPhase == MODEL_TRANSLATION_X_LEFT)
		{
			std::wstring description = L"Translate model in X direction to the right";
			TextOut(hDc, 10, 70, description.c_str(), (int)description.length());
		}
		else if (_modelPhase == MODEL_TRANSLATION_X_RIGHT)
		{
			std::wstring description = L"Translate model in X direction to the left";
			TextOut(hDc, 10, 70, description.c_str(), (int)description.length());
		}
		else if (_modelPhase == MODEL_TRANSLATION_Y_DOWN)
		{
			std::wstring description = L"Translate model in Y direction downwards";
			TextOut(hDc, 10, 70, description.c_str(), (int)description.length());
		}
		else if (_modelPhase == MODEL_TRANSLATION_Y_UP)
		{
			std::wstring description = L"Translate model in Y direction upwards";
			TextOut(hDc, 10, 70, description.c_str(), (int)description.length());
		}
		else if (_modelPhase == MODEL_SCALE_DOWN)
		{
			std::wstring description = L"Scale Model Down";
			TextOut(hDc, 10, 70, description.c_str(), (int)description.length());
		}
		else if (_modelPhase == MODEL_SCALE_UP)
		{
			std::wstring description = L"Scale Model Up";
			TextOut(hDc, 10, 70, description.c_str(), (int)description.length());
		}	
		SelectObject(hDc, hOldFont);
	}
	DeleteObject(hFontSubheader);
}

void Rasteriser::DrawWireFrame(Bitmap & bitmap)
{
	HDC hDc = bitmap.GetDC();

	HPEN hPen;
	hPen = CreatePen(PS_SOLID, 1, RGB(10, 187, 255));

	// Draw Cube
	SelectObject(bitmap.GetDC(), hPen);

	int polygonSize = (int)_model.GetPolygons().size();
	for (int i = 0; i < polygonSize; i++)
	{
		if (!_model.GetPolygons()[i].GetCullFlag())
		{
			MoveToEx(hDc, (int)_model.GetVertices()[_model.GetPolygons()[i].GetIndex(0)].GetX(), (int)_model.GetVertices()[_model.GetPolygons()[i].GetIndex(0)].GetY(), NULL);
			LineTo(hDc, (int)_model.GetVertices()[_model.GetPolygons()[i].GetIndex(1)].GetX(), (int)_model.GetVertices()[_model.GetPolygons()[i].GetIndex(1)].GetY());
			LineTo(hDc, (int)_model.GetVertices()[_model.GetPolygons()[i].GetIndex(2)].GetX(), (int)_model.GetVertices()[_model.GetPolygons()[i].GetIndex(2)].GetY());
			LineTo(hDc, (int)_model.GetVertices()[_model.GetPolygons()[i].GetIndex(0)].GetX(), (int)_model.GetVertices()[_model.GetPolygons()[i].GetIndex(0)].GetY());
		}
	}

	DeleteObject(hPen);
}

void Rasteriser::DrawSolidFlat(Bitmap & bitmap)
{
	HDC hDc = bitmap.GetDC();

	int polygonSize = (int)_model.GetPolygons().size();
	for (int i = 0; i < polygonSize; i++)
	{
		if (!_model.GetPolygons()[i].GetCullFlag())
		{
			HPEN hPen = CreatePen(PS_SOLID, 1, (_model.GetPolygons()[i].GetColor()));
			SelectObject(hDc, hPen);

			HBRUSH hBrush = CreateSolidBrush(_model.GetPolygons()[i].GetColor());
			SelectObject(hDc, hBrush);
			
			POINT vertices[] = { 
				{ (int)_model.GetVertices()[_model.GetPolygons()[i].GetIndex(0)].GetX(), (int)_model.GetVertices()[_model.GetPolygons()[i].GetIndex(0)].GetY() }, 
				{ (int)_model.GetVertices()[_model.GetPolygons()[i].GetIndex(1)].GetX(), (int)_model.GetVertices()[_model.GetPolygons()[i].GetIndex(1)].GetY() }, 
				{ (int)_model.GetVertices()[_model.GetPolygons()[i].GetIndex(2)].GetX(), (int)_model.GetVertices()[_model.GetPolygons()[i].GetIndex(2)].GetY() } };
			Polygon(hDc, vertices, sizeof(vertices) / sizeof(vertices[0]));
			
			DeleteObject(hBrush);
			DeleteObject(hPen);
		}
	}
}

void Rasteriser::MyDrawSolidFlat(Bitmap & bitmap)
{
	HDC hDc = bitmap.GetDC();
	int polygonSize = (int)_model.GetPolygons().size();
	for (int i = 0; i < polygonSize; i++)
	{
		if (!_model.GetPolygons()[i].GetCullFlag())
		{
			const Vertex v0(_model.GetVertices()[_model.GetPolygons()[i].GetIndex(0)]);
			const Vertex v1(_model.GetVertices()[_model.GetPolygons()[i].GetIndex(1)]);
			const Vertex v2(_model.GetVertices()[_model.GetPolygons()[i].GetIndex(2)]);
			const Color color = _model.GetPolygons()[i].GetColor();
			FillPolygonFlat(hDc, v0, v1, v2, color);
		}
	}
}

void Rasteriser::FillPolygonFlat(HDC hdc, const Vertex & vertex0, const Vertex & vertex1, 
	const Vertex & vertex2, const Color color)
{
	const Vertex * pVertex0 = &vertex0;
	const Vertex * pVertex1 = &vertex1;
	const Vertex * pVertex2 = &vertex2;

	// Sorting
	if (pVertex1->GetY() < pVertex0->GetY()) 
	{
		std::swap(pVertex0, pVertex1);
	}
	if (pVertex2->GetY() < pVertex1->GetY()) 
	{
		std::swap(pVertex1, pVertex2);
	}
	if (pVertex1->GetY() < pVertex0->GetY())
	{
		std::swap(pVertex0, pVertex1);
	}

	if (pVertex0->GetY() == pVertex1->GetY())
	{
		if (pVertex1->GetX() < pVertex0->GetX())
		{
			std::swap(pVertex0, pVertex1);
		}
		DrawFlatTopTriangle(hdc, *pVertex0, *pVertex1, *pVertex2, color);
	}
	else if (pVertex1->GetY() == pVertex2->GetY())
	{
		if (pVertex2->GetX() < pVertex1->GetX()) std::swap(pVertex1, pVertex2);
		DrawFlatBottomTriangle(hdc, *pVertex0, *pVertex1, *pVertex2, color);
	}
	else
	{
		// Find split Vertex
		const float split = (pVertex1->GetY() - pVertex0->GetY()) / (pVertex2->GetY() - pVertex0->GetY());

		const Vertex subtract(pVertex2->GetX() - pVertex0->GetX(),
			pVertex2->GetY() - pVertex0->GetY(),
			pVertex2->GetZ() - pVertex0->GetZ());

		const Vertex splitVertex = *pVertex0 + subtract * split;

		// Major Right
		if (pVertex1->GetX() < splitVertex.GetX())
		{
			DrawFlatBottomTriangle(hdc, *pVertex0, *pVertex1, splitVertex, color);
			DrawFlatTopTriangle(hdc, *pVertex1, splitVertex, *pVertex2, color);
		}
		else
		{
			DrawFlatBottomTriangle(hdc, *pVertex0, splitVertex, *pVertex1, color);
			DrawFlatTopTriangle(hdc, splitVertex, *pVertex1, *pVertex2, color);
		}
	}
}

void Rasteriser::DrawFlatTopTriangle(HDC hdc, const Vertex & vertex0, const Vertex & vertex1, 
	const Vertex & vertex2, const Color color)
{
	// calulcate slopes in screen space
	const float slope0 = (vertex2.GetX() - vertex0.GetX()) / (vertex2.GetY() - vertex0.GetY());
	const float slope1 = (vertex2.GetX() - vertex1.GetX()) / (vertex2.GetY() - vertex1.GetY());

	// calculate start and end points of Y value
	const int startY = (int)ceil(vertex0.GetY() - 0.5f);
	const int endY = (int)ceil(vertex2.GetY() - 0.5f);

	for (int positionY = startY; positionY < endY; positionY++)
	{
		// caluclate start and end points of X value
		const float point0 = slope0 * (float(positionY) + 0.5f - vertex0.GetY()) + vertex0.GetX();
		const float point1 = slope1 * (float(positionY) + 0.5f - vertex1.GetY()) + vertex1.GetX();

		// calculate start and end pixels
		const int startX = (int)ceil(point0 - 0.5f);
		const int endX = (int)ceil(point1 - 0.5f);

		for (int positionX = startX; positionX < endX; positionX++)
		{
			SetPixel(hdc, positionX, positionY, color.GetColor());
		}
	}
}

void Rasteriser::DrawFlatBottomTriangle(HDC hdc, const Vertex & vertex0, const Vertex & vertex1, 
	const Vertex & vertex2, const Color color)
{
	// calulcate slopes in screen space
	const float slope0 = (vertex1.GetX() - vertex0.GetX()) / (vertex1.GetY() - vertex0.GetY());
	const float slope1 = (vertex2.GetX() - vertex0.GetX()) / (vertex2.GetY() - vertex0.GetY());

	// calculate start and end points of Y value
	const int startY = (int)ceil(vertex0.GetY() - 0.5f);
	const int endY = (int)ceil(vertex2.GetY() - 0.5f);

	for (int y = startY; y < endY; y++)
	{
		// caluclate start and end points of X value
		const float point0 = slope0 * (float(y) + 0.5f - vertex0.GetY()) + vertex0.GetX();
		const float point1 = slope1 * (float(y) + 0.5f - vertex0.GetY()) + vertex0.GetX();

		// calculate start and end pixels
		const int startX = (int)ceil(point0 - 0.5f);
		const int endX = (int)ceil(point1 - 0.5f);

		for (int x = startX; x < endX; x++)
		{
			SetPixel(hdc, x, y, color.GetColor());
		}
	}
}

void Rasteriser::DrawGouraud(Bitmap & bitmap)
{
	HDC hDc = bitmap.GetDC();

	int polygonSize = (int)_model.GetPolygons().size();
	for (int i = 0; i < polygonSize; i++)
	{
		if (!_model.GetPolygons()[i].GetCullFlag())
		{
			const Vertex v1(_model.GetVertices()[_model.GetPolygons()[i].GetIndex(1)]);
			const Vertex v0(_model.GetVertices()[_model.GetPolygons()[i].GetIndex(0)]);
			const Vertex v2(_model.GetVertices()[_model.GetPolygons()[i].GetIndex(2)]);

			FillPolygonGouraud(hDc, v0, v1, v2, v0.GetColor(), v1.GetColor(), v2.GetColor());
		}
	}
}

void Rasteriser::FillPolygonGouraud(HDC hdc, const Vertex & vertex0, const Vertex & vertex1, 
	const Vertex & vertex2, const Color colorVer0, const Color colorVer1, const Color colorVer2)
{
	const Vertex * pVertex0 = &vertex0;
	const Vertex * pVertex1 = &vertex1;
	const Vertex * pVertex2 = &vertex2;

	const Color * pColorVertex0 = &colorVer0;
	const Color * pColorVertex1 = &colorVer1;
	const Color * pColorVertex2 = &colorVer2;

	// Sorting
	if (pVertex1->GetY() < pVertex0->GetY())
	{
		std::swap(pVertex0, pVertex1);
		std::swap(pColorVertex0, pColorVertex1);
	}

	if (pVertex2->GetY() < pVertex1->GetY()) 
	{
		std::swap(pVertex1, pVertex2);
		std::swap(pColorVertex1, pColorVertex2);
	}

	if (pVertex1->GetY() < pVertex0->GetY())
	{
		std::swap(pVertex0, pVertex1);
		std::swap(pColorVertex0, pColorVertex1);
	}

	if (pVertex0->GetY() == pVertex1->GetY())
	{
		if (pVertex1->GetX() < pVertex0->GetX())
		{
			std::swap(pVertex0, pVertex1);
			std::swap(pColorVertex0, pColorVertex1);
		}

		DrawGouraudTopTriangle(hdc, *pVertex0, *pVertex1, *pVertex2, *pColorVertex0, *pColorVertex1, *pColorVertex2);
	}
	else if (pVertex1->GetY() == pVertex2->GetY())
	{
		if (pVertex2->GetX() < pVertex1->GetX())
		{
			std::swap(pVertex1, pVertex2);
			std::swap(pColorVertex1, pColorVertex2);
		}

		DrawGouraudBottomTriangle(hdc, *pVertex0, *pVertex1, *pVertex2, *pColorVertex0, *pColorVertex1, *pColorVertex2);
	}
	else
	{
		// Find split Vertex
		const float split = (pVertex1->GetY() - pVertex0->GetY()) / (pVertex2->GetY() - pVertex0->GetY());

		const Vertex subtract(pVertex2->GetX() - pVertex0->GetX(),
			pVertex2->GetY() - pVertex0->GetY(),
			pVertex2->GetZ() - pVertex0->GetZ());
		
		const Vertex splitVertex= *pVertex0 + subtract * split;

		const float colRedV0 = (float)pColorVertex0->GetRed();
		const float colGreenV0 = (float)pColorVertex0->GetGreen();
		const float colBlueV0 = (float)pColorVertex0->GetBlue();

		const float colRedV1 = (float)pColorVertex1->GetRed();
		const float colGreenV1 = (float)pColorVertex1->GetGreen();
		const float colBlueV1 = (float)pColorVertex1->GetBlue();

		const float colRedV2 = (float)pColorVertex2->GetRed();
		const float colGreenV2 = (float)pColorVertex2->GetGreen();
		const float colBlueV2 = (float)pColorVertex2->GetBlue();

		const float cRed = colRedV0 + split * (colRedV2 - colRedV0);
		const float cGreen = colGreenV0 + split * (colGreenV2 - colGreenV0);
		const float cBlue = colBlueV0 + split * (colBlueV2 - colBlueV0);
		
		Color colTemp((int)cRed, (int)cGreen, (int)cBlue);

		// Major Right
		if (pVertex1->GetX() < splitVertex.GetX())
		{
			DrawGouraudBottomTriangle(hdc, *pVertex0, *pVertex1, splitVertex, *pColorVertex0, *pColorVertex1, colTemp);
			DrawGouraudTopTriangle(hdc, *pVertex1, splitVertex, *pVertex2, *pColorVertex1, colTemp, *pColorVertex2);
		}
		else
		{
			DrawGouraudBottomTriangle(hdc, *pVertex0, splitVertex, *pVertex1, *pColorVertex0, colTemp, *pColorVertex1);
			DrawGouraudTopTriangle(hdc, splitVertex, *pVertex1, *pVertex2, colTemp, *pColorVertex1, *pColorVertex2);
		}
	}
}

void Rasteriser::DrawGouraudTopTriangle(HDC hdc, const Vertex & vertex0, const Vertex & vertex1, 
	const Vertex & vertex2, const Color colorVer0, const Color colorVer1, const Color colorVer2)
{
	// calulcate slopes in screen space
	const float slope1 = (vertex2.GetX() - vertex0.GetX()) / (vertex2.GetY() - vertex0.GetY());
	const float slope2 = (vertex2.GetX() - vertex1.GetX()) / (vertex2.GetY() - vertex1.GetY());

	const float colRedV0 = (float)colorVer0.GetRed();
	const float colGreenV0 = (float)colorVer0.GetGreen();
	const float colBlueV0 = (float)colorVer0.GetBlue();

	const float colRedV1 = (float)colorVer1.GetRed();
	const float colGreenV1 = (float)colorVer1.GetGreen();
	const float colBlueV1 = (float)colorVer1.GetBlue();

	const float colRedV2 = (float)colorVer2.GetRed();
	const float colGreenV2 = (float)colorVer2.GetGreen();
	const float colBlueV2 = (float)colorVer2.GetBlue();

	// get the change of color components along edge (v3,v1)
	const float v2v0Diff = (float)(vertex2.GetY() - vertex0.GetY());
	const float colorSlopeBlue1 = (float)(colBlueV2 - colBlueV0) / v2v0Diff;
	const float colorSlopeRed1 = (float)(colRedV2 - colRedV0) / v2v0Diff;
	const float colorSlopeGreen1 = (float)(colGreenV2 - colGreenV0) / v2v0Diff;

	// get the change of color components along edge (v3,v2)
	const float v2v1Diff = (float)(vertex2.GetY() - vertex1.GetY());
	const float colorSlopeBlue2 = (float)(colBlueV2 - colBlueV1) / v2v1Diff;
	const float colorSlopeRed2 = (float)(colRedV2 - colRedV1) / v2v1Diff;
	const float colorSlopeGreen2 = (float)(colGreenV2 - colGreenV1) / v2v1Diff;

	// set starting values
	float cBlue1 = colBlueV2;
	float cRed1 = colRedV2;
	float cGreen1 = colGreenV2;
	float cBlue2 = colBlueV2;
	float cRed2 = colRedV2;
	float cGreen2 = colGreenV2;
	
	const int startY = (int)floor(vertex0.GetY() - 0.5f);
	const int endY = (int)floor(vertex2.GetY() - 0.5f);

	for (int positionY = endY; positionY > startY; positionY--)
	{
		const float point0 = slope1 * (float(positionY) + 0.5f - vertex0.GetY()) + vertex0.GetX();
		const float point1 = slope2 * (float(positionY) + 0.5f - vertex1.GetY()) + vertex1.GetX();

		const int startX = (int)ceil(point0 - 0.5f);
		const int endX = (int)ceil(point1 - 0.5f); 

		for (int positionX = startX; positionX < endX; positionX++)
		{
			const float interpolant = (positionX - point0) / (point1 - point0);

			int red = (int)((1 - interpolant) * cRed1 + interpolant * cRed2);
			int green = (int)((1 - interpolant) * cGreen1 + interpolant * cGreen2);
			int blue = (int)((1 - interpolant) * cBlue1 + interpolant * cBlue2);

			// Color Clamping
			red = red < 0 ? 0 : (red > 256 ? 255 : red);
			green = green < 0 ? 0 : (green > 256 ? 255 : green);
			blue = blue < 0 ? 0 : (blue > 256 ? 255 : blue);

			SetPixel(hdc, positionX, positionY, RGB(red, green, blue));
		}

		// get new color of left endpoint of horizontal line
		cRed1 -= colorSlopeRed1;
		cGreen1 -= colorSlopeGreen1;
		cBlue1 -= colorSlopeBlue1;

		// get new color of right endpoint of horizontal line
		cRed2 -= colorSlopeRed2;
		cGreen2 -= colorSlopeGreen2;
		cBlue2 -= colorSlopeBlue2;
	}
}

void Rasteriser::DrawGouraudBottomTriangle(HDC hdc, const Vertex & vertex0, const Vertex & vertex1,
	const Vertex & vertex2, const Color colorVer0, const Color colorVer1, const Color colorVer2)
{
	// calulcate slopes in screen space
	const float slope1 = (vertex1.GetX() - vertex0.GetX()) / (vertex1.GetY() - vertex0.GetY());
	const float slope2 = (vertex2.GetX() - vertex0.GetX()) / (vertex2.GetY() - vertex0.GetY());

	const float colRedV0 = (float)colorVer0.GetRed();
	const float colGreenV0 = (float)colorVer0.GetGreen();
	const float colBlueV0 = (float)colorVer0.GetBlue();

	const float colRedV1 = (float)colorVer1.GetRed();
	const float colGreenV1 = (float)colorVer1.GetGreen();
	const float colBlueV1 = (float)colorVer1.GetBlue();

	const float colRedV2 = (float)colorVer2.GetRed();
	const float colGreenV2 = (float)colorVer2.GetGreen();
	const float colBlueV2 = (float)colorVer2.GetBlue();

	// get the change of color components along edge (v3,v1)
	const float v1v0Diff = (float)(vertex1.GetY() - vertex0.GetY());
	const float colorSlopeBlue1 = (float)(colBlueV1 - colBlueV0) / v1v0Diff;
	const float colorSlopeRed1 = (float)(colRedV1 - colRedV0) / v1v0Diff;
	const float colorSlopeGreen1 = (float)(colGreenV1 - colGreenV0) / v1v0Diff;

	// get the change of color components along edge (v3,v2)
	const float v2v0Diff = (float)(vertex2.GetY() - vertex0.GetY());
	const float colorSlopeBlue2 = (float)(colBlueV2 - colBlueV0) / v2v0Diff;
	const float colorSlopeRed2 = (float)(colRedV2 - colRedV0) / v2v0Diff;
	const float colorSlopeGreen2 = (float)(colGreenV2 - colGreenV0) / v2v0Diff;

	// set starting values
	float cBlue1 = colBlueV0;
	float cRed1 = colRedV0;
	float cGreen1 = colGreenV0;
	float cBlue2 = colBlueV0;
	float cRed2 = colRedV0;
	float cGreen2 = colGreenV0;

	const int startY = (int)ceil(vertex0.GetY() - 0.5f); 
	const int endY = (int)ceil(vertex2.GetY() - 0.5f);

	for (int positionY = startY; positionY < endY; positionY++)
	{
		const float point0 = slope1 * (float(positionY) + 0.5f - vertex0.GetY()) + vertex0.GetX();
		const float point1 = slope2 * (float(positionY) + 0.5f - vertex0.GetY()) + vertex0.GetX();

		const int startX = (int)ceil(point0 - 0.5f);
		const int endX = (int)ceil(point1 - 0.5f);

		for (int positionX = startX; positionX < endX; positionX++)
		{
			const float interpolant = (positionX - point0) / (point1 - point0);

			int red = (int)((1 - interpolant) * cRed1 + interpolant * cRed2);
			int green = (int)((1 - interpolant) * cGreen1 + interpolant * cGreen2);
			int blue = (int)((1 - interpolant) * cBlue1 + interpolant * cBlue2);

			// Color Clamping
			red = red < 0 ? 0 : (red > 256 ? 255 : red);
			green = green < 0 ? 0 : (green > 256 ? 255 : green);
			blue = blue < 0 ? 0 : (blue > 256 ? 255 : blue);

			SetPixel(hdc, positionX, positionY, RGB(red, green, blue));
		}

		// get new color of left endpoint of horizontal line 
		cRed1 += colorSlopeRed1;
		cGreen1 += colorSlopeGreen1;
		cBlue1 += colorSlopeBlue1;

		// get new color of right endpoint of horizontal line 
		cRed2 += colorSlopeRed2;
		cGreen2 += colorSlopeGreen2;
		cBlue2 += colorSlopeBlue2;
	}
}

void Rasteriser::DrawSolidTextured(Bitmap & bitmap)
{
	HDC hDc = bitmap.GetDC();

	int polygonSize = (int)_model.GetPolygons().size();
	for (int i = 0; i < polygonSize; i++)
	{
		if (!_model.GetPolygons()[i].GetCullFlag())
		{
			const int index0 = _model.GetPolygons()[i].GetIndex(0);
			const int index1 = _model.GetPolygons()[i].GetIndex(1);
			const int index2 = _model.GetPolygons()[i].GetIndex(2);

			const int UVIndex0 = _model.GetPolygons()[i].GetUVIndex(0);
			const int UVIndex1 = _model.GetPolygons()[i].GetUVIndex(1);
			const int UVIndex2 = _model.GetPolygons()[i].GetUVIndex(2);

			const Vertex v0(_model.GetVertices()[index0]);
			const Vertex v1(_model.GetVertices()[index1]);
			const Vertex v2(_model.GetVertices()[index2]);

			_model.GetVertices()[index0].SetUVIndex(UVIndex0);
			_model.GetVertices()[index1].SetUVIndex(UVIndex1);
			_model.GetVertices()[index2].SetUVIndex(UVIndex2);

			FillSolidTextured(hDc, v0, v1, v2, v0.GetColor(), v1.GetColor(), v2.GetColor());
		}
	}
}

void Rasteriser::FillSolidTextured(HDC hdc, const Vertex & vertex0, const Vertex & vertex1, 
	const Vertex & vertex2, const Color colorVer0, const Color colorVer1, const Color colorVer2)
{
	const Vertex * pVertex0 = &vertex0;
	const Vertex * pVertex1 = &vertex1;
	const Vertex * pVertex2 = &vertex2;

	const UVCoordinates * pUV0 = &_model.GetUVCoordinates()[vertex0.GetUVIndex()];
	const UVCoordinates * pUV1 = &_model.GetUVCoordinates()[vertex1.GetUVIndex()];
	const UVCoordinates * pUV2 = &_model.GetUVCoordinates()[vertex2.GetUVIndex()];

	const Color * pColorVertex0 = &colorVer0;
	const Color * pColorVertex1 = &colorVer1;
	const Color * pColorVertex2 = &colorVer2;

	// Sorting
 	if (pVertex1->GetY() < pVertex0->GetY())
	{
		std::swap(pVertex0, pVertex1);
		std::swap(pColorVertex0, pColorVertex1);
		std::swap(pUV0, pUV1);
	}

	if (pVertex2->GetY() < pVertex1->GetY())
	{
		std::swap(pVertex1, pVertex2);
		std::swap(pColorVertex1, pColorVertex2);
		std::swap(pUV1, pUV2); 
	}

	if (pVertex1->GetY() < pVertex0->GetY())
	{
		std::swap(pVertex0, pVertex1);
		std::swap(pColorVertex0, pColorVertex1);
		std::swap(pUV0, pUV1);
	}

	if (pVertex0->GetY() == pVertex1->GetY())
	{
		if (pVertex1->GetX() < pVertex0->GetX())
		{
			std::swap(pVertex0, pVertex1);
			std::swap(pColorVertex0, pColorVertex1);
			std::swap(pUV0, pUV1);
		}

		DrawTextureTopTriangle(hdc, *pVertex0, *pVertex1, *pVertex2, *pColorVertex0, *pColorVertex1, *pColorVertex2, *pUV0, *pUV1, *pUV2);
	}
	else if (pVertex1->GetY() == pVertex2->GetY())
	{
		if (pVertex2->GetX() < pVertex1->GetX())
		{
			std::swap(pVertex1, pVertex2);
			std::swap(pColorVertex1, pColorVertex2);
			std::swap(pUV1, pUV2);
		}

		DrawTextureBottomTriangle(hdc, *pVertex0, *pVertex1, *pVertex2, *pColorVertex0, *pColorVertex1, *pColorVertex2, *pUV0, *pUV1, *pUV2);
	}
	else
	{
		// Find split Vertex
		const float split = (pVertex1->GetY() - pVertex0->GetY()) / (pVertex2->GetY() - pVertex0->GetY());

		const Vertex subtract(pVertex2->GetX() - pVertex0->GetX(),
			pVertex2->GetY() - pVertex0->GetY(),
			pVertex2->GetZ() - pVertex0->GetZ());

		const Vertex splitVertex = *pVertex0 + subtract * split;

		const float colRedV0 = (float)pColorVertex0->GetRed();
		const float colGreenV0 = (float)pColorVertex0->GetGreen();
		const float colBlueV0 = (float)pColorVertex0->GetBlue();

		const float colRedV1 = (float)pColorVertex1->GetRed();
		const float colGreenV1 = (float)pColorVertex1->GetGreen();
		const float colBlueV1 = (float)pColorVertex1->GetBlue();

		const float colRedV2 = (float)pColorVertex2->GetRed();
		const float colGreenV2 = (float)pColorVertex2->GetGreen();
		const float colBlueV2 = (float)pColorVertex2->GetBlue();

		const float cRed = colRedV0 + split * (colRedV2 - colRedV0);
		const float cGreen = colGreenV0 + split * (colGreenV2 - colGreenV0);
		const float cBlue = colBlueV0 + split * (colBlueV2 - colBlueV0);

		const Color colTemp((int)cRed, (int)cGreen, (int)cBlue);

		const float uValue0 = pUV0->GetU();
		const float vValue0 = pUV0->GetV();
		const float uValue2 = pUV2->GetU();
		const float vValue2 = pUV2->GetV();

		const float uValue = uValue0 + split * (uValue2 - uValue0);
		const float vValue = vValue0 + split * (vValue2 - vValue0);

		const UVCoordinates uvTemp(uValue, vValue);

		// Major Right
		if (pVertex1->GetX() < splitVertex.GetX())
		{
			DrawTextureBottomTriangle(hdc, *pVertex0, *pVertex1, splitVertex, *pColorVertex0, *pColorVertex1, colTemp, *pUV0, *pUV1, uvTemp);
			DrawTextureTopTriangle(hdc, *pVertex1, splitVertex, *pVertex2, *pColorVertex1, colTemp, *pColorVertex2, *pUV1, uvTemp, *pUV2);
		}
		else
		{
			DrawTextureBottomTriangle(hdc, *pVertex0, splitVertex, *pVertex1, *pColorVertex0, colTemp, *pColorVertex1, *pUV0, uvTemp, *pUV1);
			DrawTextureTopTriangle(hdc, splitVertex, *pVertex1, *pVertex2, colTemp, *pColorVertex1, *pColorVertex2, uvTemp, *pUV1, *pUV2);
		}
	}
}

void Rasteriser::DrawTextureTopTriangle(HDC hdc, const Vertex & vertex0, const Vertex & vertex1, 
	const Vertex & vertex2, const Color colorVer0, const Color colorVer1, const Color colorVer2, const UVCoordinates uv0,
	const UVCoordinates uv1, const UVCoordinates uv2)
{
	// calulcate slopes in screen space
	const float slope1 = (vertex2.GetX() - vertex0.GetX()) / (vertex2.GetY() - vertex0.GetY());
	const float slope2 = (vertex2.GetX() - vertex1.GetX()) / (vertex2.GetY() - vertex1.GetY());

	const float colRedV0 = (float)colorVer0.GetRed();
	const float colGreenV0 = (float)colorVer0.GetGreen();
	const float colBlueV0 = (float)colorVer0.GetBlue();

	const float colRedV1 = (float)colorVer1.GetRed();
	const float colGreenV1 = (float)colorVer1.GetGreen();
	const float colBlueV1 = (float)colorVer1.GetBlue();

	const float colRedV2 = (float)colorVer2.GetRed();
	const float colGreenV2 = (float)colorVer2.GetGreen();
	const float colBlueV2 = (float)colorVer2.GetBlue();

	// get the change of color components along edge (v3,v1)
	const float v2v0Diff = (float)(vertex2.GetY() - vertex0.GetY());
	const float colorSlopeBlue1 = (float)(colBlueV2 - colBlueV0) / v2v0Diff;
	const float colorSlopeRed1 = (float)(colRedV2 - colRedV0) / v2v0Diff;
	const float colorSlopeGreen1 = (float)(colGreenV2 - colGreenV0) / v2v0Diff;

	// get the change of color components along edge (v3,v2)
	const float v2v1Diff = (float)(vertex2.GetY() - vertex1.GetY());
	const float colorSlopeBlue2 = (float)(colBlueV2 - colBlueV1) / v2v1Diff;
	const float colorSlopeRed2 = (float)(colRedV2 - colRedV1) / v2v1Diff;
	const float colorSlopeGreen2 = (float)(colGreenV2 - colGreenV1) / v2v1Diff;

	// set starting values
	float cBlue1 = colBlueV2;
	float cRed1 = colRedV2;
	float cGreen1 = colGreenV2;
	float cBlue2 = colBlueV2;
	float cRed2 = colRedV2;
	float cGreen2 = colGreenV2;

	// UV slopes
	const UVCoordinates uvSlope1 = (uv2 - uv0) / v2v0Diff;
	const UVCoordinates uvSlope2 = (uv2 - uv1) / v2v1Diff;

	// Starting UV values
	UVCoordinates uvStart1 = uv2;
	UVCoordinates uvStart2 = uv2;

	const int startY = (int)floor(vertex0.GetY() - 0.5f);
	const int endY = (int)floor(vertex2.GetY() - 0.5f);

	for (int positionY = endY; positionY > startY; positionY--)
	{
		const float uTemp1 = uvStart1.GetU();
		const float vTemp1 = uvStart1.GetV();
		const float uTemp2 = uvStart2.GetU();
		const float vTemp2 = uvStart2.GetV();

		const float point0 = slope1 * (float(positionY) + 0.5f - vertex0.GetY()) + vertex0.GetX();
		const float point1 = slope2 * (float(positionY) + 0.5f - vertex1.GetY()) + vertex1.GetX();

		const int startX = (int)ceil(point0 - 0.5f);
		const int endX = (int)ceil(point1 - 0.5f);

		for (int positionX = startX; positionX < endX; positionX++)
		{
			const float interpolant = (positionX - point0) / (point1 - point0);

			int red = (int)((1 - interpolant) * cRed1 + interpolant * cRed2);
			int green = (int)((1 - interpolant) * cGreen1 + interpolant * cGreen2);
			int blue = (int)((1 - interpolant) * cBlue1 + interpolant * cBlue2);

			const float uValue = (1 - interpolant) * uTemp1 + interpolant * uTemp2;
			const float vValue = (1 - interpolant) * vTemp1 + interpolant * vTemp2;

			const Color textureColour = _model.GetTexture().GetTextureValue((int)uValue, (int)vValue);

			float finalColorRed = (float)(textureColour.GetRed() * red) / 255;
			float finalColorGreen = (float)(textureColour.GetGreen() * green) / 255;
			float finalColorBlue = (float)(textureColour.GetBlue() * blue) / 255;
			
			// Color Clamping
			finalColorRed = finalColorRed < 0 ? 0 : (finalColorRed > 256 ? 255 : finalColorRed);
			finalColorGreen = finalColorGreen < 0 ? 0 : (finalColorGreen > 256 ? 255 : finalColorGreen);
			finalColorBlue = finalColorBlue < 0 ? 0 : (finalColorBlue > 256 ? 255 : finalColorBlue);

			SetPixel(hdc, positionX, positionY, RGB(finalColorRed, finalColorGreen, finalColorBlue));
		}

		// get new color of left endpoint of horizontal line
		cRed1 -= colorSlopeRed1;
		cGreen1 -= colorSlopeGreen1;
		cBlue1 -= colorSlopeBlue1;

		// get new color of right endpoint of horizontal line
		cRed2 -= colorSlopeRed2;
		cGreen2 -= colorSlopeGreen2;
		cBlue2 -= colorSlopeBlue2;

		uvStart1 -= uvSlope1;
		uvStart2 -= uvSlope2;
	}
}

void Rasteriser::DrawTextureBottomTriangle(HDC hdc, const Vertex & vertex0, const Vertex & vertex1,
	const Vertex & vertex2, const Color colorVer0, const Color colorVer1, const Color colorVer2, const UVCoordinates uv0,
	const UVCoordinates uv1, const UVCoordinates uv2)
{
	// calulcate slopes in screen space
	const float slope1 = (vertex1.GetX() - vertex0.GetX()) / (vertex1.GetY() - vertex0.GetY());
	const float slope2 = (vertex2.GetX() - vertex0.GetX()) / (vertex2.GetY() - vertex0.GetY());

	const float colRedV0 = (float)colorVer0.GetRed();
	const float colGreenV0 = (float)colorVer0.GetGreen();
	const float colBlueV0 = (float)colorVer0.GetBlue();

	const float colRedV1 = (float)colorVer1.GetRed();
	const float colGreenV1 = (float)colorVer1.GetGreen();
	const float colBlueV1 = (float)colorVer1.GetBlue();

	const float colRedV2 = (float)colorVer2.GetRed();
	const float colGreenV2 = (float)colorVer2.GetGreen();
	const float colBlueV2 = (float)colorVer2.GetBlue();

	// get the change of color components along edge (v3,v1)
	const float v1v0Diff = (float)(vertex1.GetY() - vertex0.GetY());
	const float colorSlopeBlue1 = (float)(colBlueV1 - colBlueV0) / v1v0Diff;
	const float colorSlopeRed1 = (float)(colRedV1 - colRedV0) / v1v0Diff;
	const float colorSlopeGreen1 = (float)(colGreenV1 - colGreenV0) / v1v0Diff;

	// get the change of color components along edge (v3,v2)
	const float v2v0Diff = (float)(vertex2.GetY() - vertex0.GetY());
	const float colorSlopeBlue2 = (float)(colBlueV2 - colBlueV0) / v2v0Diff;
	const float colorSlopeRed2 = (float)(colRedV2 - colRedV0) / v2v0Diff;
	const float colorSlopeGreen2 = (float)(colGreenV2 - colGreenV0) / v2v0Diff;

	// set starting values
	float cBlue1 = colBlueV0;
	float cRed1 = colRedV0;
	float cGreen1 = colGreenV0;
	float cBlue2 = colBlueV0;
	float cRed2 = colRedV0;
	float cGreen2 = colGreenV0;

	// UV slopes
	const UVCoordinates uvSlope1 = (uv1 - uv0) / v1v0Diff;
	const UVCoordinates uvSlope2 = (uv2 - uv0) / v2v0Diff;

	// Starting UV values
	UVCoordinates uvStart1 = uv0;
	UVCoordinates uvStart2 = uv0;

	const int startY = (int)ceil(vertex0.GetY() - 0.5f);
	const int endY = (int)ceil(vertex2.GetY() - 0.5f);
	
	for (int positionY = startY; positionY < endY; positionY++)
	{
		const float uTemp1 = uvStart1.GetU();
		const float vTemp1 = uvStart1.GetV();
		const float uTemp2 = uvStart2.GetU();
		const float vTemp2 = uvStart2.GetV();

		const float point0 = slope1 * (float(positionY) + 0.5f - vertex0.GetY()) + vertex0.GetX();
		const float point1 = slope2 * (float(positionY) + 0.5f - vertex0.GetY()) + vertex0.GetX();

		const int startX = (int)ceil(point0 - 0.5f);
		const int endX = (int)ceil(point1 - 0.5f);

		for (int positionX = startX; positionX < endX; positionX++)
		{
			const float interpolant = (positionX - point0) / (point1 - point0);

			int red = (int)((1 - interpolant) * cRed1 + interpolant * cRed2);
			int green = (int)((1 - interpolant) * cGreen1 + interpolant * cGreen2);
			int blue = (int)((1 - interpolant) * cBlue1 + interpolant * cBlue2);

			const float uValue = (1 - interpolant) * uTemp1 + interpolant * uTemp2;
			const float vValue = (1 - interpolant) * vTemp1 + interpolant * vTemp2;

			const Color textureColour = _model.GetTexture().GetTextureValue((int)uValue, (int)vValue);

			float finalColorRed = (float)(textureColour.GetRed() * red) / 255;
			float finalColorGreen = (float)(textureColour.GetGreen() * green) / 255;
			float finalColorBlue = (float)(textureColour.GetBlue() * blue) / 255;

			// Color Clamping
			finalColorRed = finalColorRed < 0 ? 0 : (finalColorRed > 256 ? 255 : finalColorRed);
			finalColorGreen = finalColorGreen < 0 ? 0 : (finalColorGreen > 256 ? 255 : finalColorGreen);
			finalColorBlue = finalColorBlue < 0 ? 0 : (finalColorBlue > 256 ? 255 : finalColorBlue);

			SetPixel(hdc, positionX, positionY, RGB(finalColorRed, finalColorGreen, finalColorBlue));
		}

		// get new color of left endpoint of horizontal line 
		cRed1 += colorSlopeRed1;
		cGreen1 += colorSlopeGreen1;
		cBlue1 += colorSlopeBlue1;

		// get new color of right endpoint of horizontal line 
		cRed2 += colorSlopeRed2;
		cGreen2 += colorSlopeGreen2;
		cBlue2 += colorSlopeBlue2;

		uvStart1 += uvSlope1;
		uvStart2 += uvSlope2;
	}
}

// This method check which key is pressed which will caused 
// matrix operation on the _object which is the 3d cube and 
// also on the model which is loaded from file by MD2 loader
void Rasteriser::KeyPressed(WPARAM wParam)
{
	// Translate up
	if (wParam == 'W')
	{
		_camera.SetCameraPosition(Matrix4x4::Translate(0.0f, -3.0f, 0.0f) * _camera.GetPosition());
	}

	// Translate down
	if (wParam == 'S')
	{
		_camera.SetCameraPosition(Matrix4x4::Translate(0.0f, 3.0f, 0.0f) * _camera.GetPosition());
	}

	// Translate left
	if (wParam == 'A')
	{
		_camera.SetCameraPosition(Matrix4x4::Translate(3.0f, 0.0f, 0.0f) * _camera.GetPosition());
	}

	// Translate right
	if (wParam == 'D')
	{
		_camera.SetCameraPosition(Matrix4x4::Translate(-3.0f, 0.0f, 0.0f) * _camera.GetPosition());
	}

	// Scale object (zoom up)
	if (wParam == '9')
	{
		_camera.SetCameraPosition(Matrix4x4::Translate(0.0f, 0.0f, 2.0f) * _camera.GetPosition());
	}

	// Scale object (zoom down)
	if (wParam == '0')
	{
		_camera.SetCameraPosition(Matrix4x4::Translate(0.0f, 0.0f, -2.0f) * _camera.GetPosition());	
	}

	// Rotate Camera
	if (wParam == 'I')
	{
		_camera.RotateX(0.10f);
	}
	if (wParam == 'L')
	{
		_camera.RotateY(0.10f);
	}
	if (wParam == 'U')
	{
		_camera.RotateZ(0.10f);
	}
	if (wParam == 'K')
	{
		_camera.RotateX(-0.10f);
	}
	if (wParam == 'J')
	{
		_camera.RotateY(-0.10f);
	}
	if (wParam == 'O')
	{
		_camera.RotateZ(-0.10f);
	}

	// Focal distance
	if (wParam == '7')
	{
		_focalDistance = _focalDistance + 0.10f;
	}

	if (wParam == '8')
	{
		_focalDistance = _focalDistance - 0.10f;
	}
}

// Change scene and features
void Rasteriser::ChangeScene()
{
	if (_state == WIREFRAME)
	{
		_state = SOLID_FLAT;
	}
	else if (_state == SOLID_FLAT)
	{
		_state = MY_SOLID;
	}
	else if (_state == MY_SOLID)
	{
		_state = GOURAUD_SHADING;
	}
	else if (_state == GOURAUD_SHADING)
	{
		_state = TEXTURE;
	}
	else if (_state == TEXTURE)
	{
		_state = MODEL_PHASES;
		_modelTransformation = Matrix4x4::Scaling(SCALING_FACTOR_X, SCALING_FACTOR_Y, SCALING_FACTOR_Z) * Matrix4x4::Identity();
		_directionalLights[0].SetDirection(Vector3D(_dirLightX + 1.0f, _dirLightY, _dirLightZ + 1.0f));
	}
	else if (_state == MODEL_PHASES)
	{
		ChangeModelPhase();
	}
}

void Rasteriser::ChangeModelPhase()
{
	if (_modelPhase == DIRECTIONAL_LIGHT_X)
	{
		_modelTransformation = Matrix4x4::Scaling(SCALING_FACTOR_X, SCALING_FACTOR_Y, SCALING_FACTOR_Z) * Matrix4x4::Identity();
		_directionalLights[0].SetDirection(Vector3D(_dirLightX, _dirLightY + 1.0f, _dirLightZ + 1.0f));
		_modelPhase = DIRECTIONAL_LIGHT_Y;
	}
	else if (_modelPhase == DIRECTIONAL_LIGHT_Y)
	{
		_modelTransformation = Matrix4x4::Scaling(SCALING_FACTOR_X, SCALING_FACTOR_Y, SCALING_FACTOR_Z) * Matrix4x4::Identity();
		_directionalLights[0].SetDirection(Vector3D(_dirLightX, _dirLightY, _dirLightZ));
		_modelPhase = DIRECTIONAL_LIGHT_Z;
	}
	else if (_modelPhase == DIRECTIONAL_LIGHT_Z)
	{
		_modelTransformation = Matrix4x4::Scaling(SCALING_FACTOR_X, SCALING_FACTOR_Y, SCALING_FACTOR_Z) * Matrix4x4::Identity();
		_modelPhase = POINT_LIGHT;
	}
	else if (_modelPhase == POINT_LIGHT)
	{
		_modelTransformation = Matrix4x4::Scaling(SCALING_FACTOR_X, SCALING_FACTOR_Y, SCALING_FACTOR_Z) * Matrix4x4::Identity();
		_modelPhase = MODEL_ROTATION_X;
		_rotationMeasurement = 0;
	}
	else if (_modelPhase == MODEL_ROTATION_X)
	{
		_modelPhase = MODEL_ROTATION_Y;
		_rotationMeasurement = 0;
	}
	else if (_modelPhase == MODEL_ROTATION_Y)
	{
		_modelPhase = MODEL_ROTATION_Z;
		_rotationMeasurement = 0;
	}
	else if (_modelPhase == MODEL_ROTATION_Z)
	{
		_modelTransformation = Matrix4x4::Scaling(SCALING_FACTOR_X, SCALING_FACTOR_Y, SCALING_FACTOR_Z) * Matrix4x4::Identity();
		_modelPhase = MODEL_TRANSLATION_X_LEFT;
	}
	else if (_modelPhase == MODEL_TRANSLATION_X_LEFT)
	{
		_modelPhase = MODEL_TRANSLATION_X_RIGHT;
	}
	else if (_modelPhase == MODEL_TRANSLATION_X_RIGHT)
	{
		_modelPhase = MODEL_TRANSLATION_Y_DOWN;
	}
	else if (_modelPhase == MODEL_TRANSLATION_Y_DOWN)
	{
		_modelPhase = MODEL_TRANSLATION_Y_UP;
	}
	else if (_modelPhase == MODEL_TRANSLATION_Y_UP)
	{
		_modelPhase = MODEL_SCALE_DOWN;
	}
	else if (_modelPhase == MODEL_SCALE_DOWN)
	{
		_modelPhase = MODEL_SCALE_UP;
	}
	else if (_modelPhase == MODEL_SCALE_UP)
	{
		_modelPhase = DIRECTIONAL_LIGHT_X;
		_state = WIREFRAME;
		_modelTransformation = Matrix4x4::Scaling(SCALING_FACTOR_X, SCALING_FACTOR_Y, SCALING_FACTOR_Z) * Matrix4x4::Identity();
	}
}

bool Rasteriser::TimeProcessing(int seconds)
{
	if (_timeDifference > seconds)
	{
		_startTime = (long int)time(NULL);
		_timeDifference = 0;
		return true;
	}

	_timeDifference = _endTime - _startTime;

	_endTime = (long int)time(NULL);
	return false;
}

void Rasteriser::GeneratePerspectiveMatrix(float d, float aspectRatio)
{
	_perspectiveTransform = Matrix4x4::ProjectionMatrix(d, aspectRatio);
}

void Rasteriser::GenerateViewMatrix(float d, int width, int height)
{
	_screenTransform = Matrix4x4::ScreenTransformationMatrix(((float)width), ((float)height), d);
}


