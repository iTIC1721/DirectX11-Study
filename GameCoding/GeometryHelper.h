#pragma once

/// <summary>
/// 기본 도형들을 미리 만들어둠
/// </summary>
class GeometryHelper
{
public:
	static void CreateRectangle(shared_ptr<Geometry<VertexTextureData>> geometry);
	static void CreateRectangle(shared_ptr<Geometry<VertexColorData>> geometry, Color color);

};

