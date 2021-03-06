#pragma once
#include <DirectXMath.h>

class Vector3 : public DirectX::XMFLOAT3
{
public:
	//float x;
	//float y;
	//float z;
public:
	//コンストラクタ
	Vector3();							//零ベクトルとする
	Vector3(float x, float y, float z);	//x成分、y成分、z成分を指定しての生成
	Vector3(const DirectX::XMFLOAT3& v);

	//メンバ関数
	static  float length(const Vector3 &v) ;				//ノルム（長さ）を求める
	static Vector3 normalize(const Vector3 &v);				//正規化する

	static float dot(const Vector3 &v1,const Vector3 &v2);	//内積を求める
	static Vector3 cross(const Vector3 &v1, const Vector3 &v2);//外積を求める

	//単項演算子オーバーロード
	Vector3 operator+() const;
	Vector3 operator-() const;

	//代入演算子オーバーロード
	Vector3 &operator=(const DirectX::XMFLOAT3 &v);
	Vector3 &operator+=(const Vector3 &v);
	Vector3 &operator-=(const Vector3 &v);
	Vector3 &operator*=(float s);
	Vector3 &operator/=(float s);
};

//２項演算子オーバーロード
const Vector3 operator+(const Vector3& v1,const Vector3& v2);
const Vector3 operator-(const Vector3& v1,const Vector3& v2);
const Vector3 operator*(const Vector3& v,float s);
const Vector3 operator*(float s,const Vector3& v);
const Vector3 operator/(const Vector3& v,float s);

//補間関数
//線形補間
const Vector3 lerp(const Vector3 &start, const Vector3 &end, const float t);
const Vector3 easeIn(const Vector3 &start, const Vector3 &end, const float t);
const Vector3 easeOut(const Vector3 &start, const Vector3 &end, const float t);
const Vector3 easeInOut(const Vector3 &start, const Vector3 &end, const float t);