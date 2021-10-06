#pragma once
#include "LoadTex.h"

class DebugText : public LoadTex
{
	/*メンバ変数*/
public:
	static const int fontWidth = 8;      //文字の横幅
	static const int fontHeight = 16;    //文字の縦幅
	static const int fontLineCount = 16; //一行あたりの文字数
private:
	static UINT64 fontTexWidth;
	static UINT fontTexHeight;
	vector<IndexData> fontIndex;
	int charCount;

	/*メンバ関数*/
public:
	DebugText(const DirectXInit* w);
	~DebugText();

	HRESULT DrawString(const float& posX, const float& posY, const float& fontScale,
		const XMFLOAT4& color, const char* text, ...);

	void TextLoopEnd();
private:
	HRESULT DrawStringInit();
};
