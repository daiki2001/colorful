#pragma once
#include <DirectXMath.h>

class Camera
{
	/*namespaceΌΘͺ*/
private:
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

	/*oΦ*/
public:
	Camera() {}
	~Camera() {}

	// LN^p¨sρΜΆ¬
	static DirectX::XMMATRIX CreateCamera(const XMVECTOR& pos, const XMVECTOR& target, const XMVECTOR& upVector);
	// LN^p¨©sρΜΆ¬
	static DirectX::XMMATRIX CreateCameraFix(const XMVECTOR& pos, const XMVECTOR& target, const XMVECTOR& upVector);
};
