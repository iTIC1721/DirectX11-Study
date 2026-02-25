#pragma once

struct CameraData {
	Matrix matView = Matrix::Identity;
	Matrix matProjection = Matrix::Identity;
};

struct TransformData {
	Matrix matWorld = Matrix::Identity;
};

struct AnimationData {
	Vec2 spriteOffset;
	Vec2 spriteSize;
	Vec2 textureSize;
	float useAnimation;
	float padding; // GPU로 넘길때는 16바이트 단위 정렬이 필요하기에 더미 데이터 하나를 같이 넘김
};
