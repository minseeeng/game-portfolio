#pragma once
#include <windows.h>
#include <vector>

class CSprite
{
public:
	CSprite();
	~CSprite();

	static bool SaveSpriteFile(PCWSTR path, PCWSTR imageFile, const std::vector<RECT>& rects);
	static bool LoadSpriteFile(PCWSTR path, wchar_t* outImageFile, int maxLen, std::vector<RECT>& outRects);
};

