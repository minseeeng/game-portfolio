#define _CRT_SECURE_NO_WARNINGS
#include "Sprite.h"

CSprite::CSprite()
{
}

CSprite::~CSprite()
{
}

bool CSprite::SaveSpriteFile(PCWSTR path, PCWSTR imageFile, const std::vector<RECT>& rects)
{
	if (rects.empty())
		return false;

	FILE* fp_write = nullptr;
	_wfopen_s(&fp_write, path, L"wb");
	if (fp_write == NULL)
		return false;

	int nameLen = (int)wcslen(imageFile);
	fwrite(&nameLen, sizeof(int), 1, fp_write);
	fwrite(imageFile, sizeof(wchar_t), nameLen, fp_write);

	int size = (int)rects.size();                   
	fwrite(&size, sizeof(int), 1, fp_write);
	fwrite(rects.data(), sizeof(RECT), size, fp_write);

	fclose(fp_write);
	return true;
}

bool CSprite::LoadSpriteFile(PCWSTR path, wchar_t* outImageFile, int maxLen, std::vector<RECT>& outRects)
{
	FILE* fp_read = nullptr;
	_wfopen_s(&fp_read, path, L"rb");
	if (fp_read == NULL)
		return false;

	int nameLen = 0;
	fread(&nameLen, sizeof(int), 1, fp_read);
	fread(outImageFile, sizeof(wchar_t), nameLen, fp_read);
	outImageFile[nameLen] = L'\0';

	int size = 0;
	fread(&size, sizeof(int), 1, fp_read);

	outRects.resize(size);
	fread(outRects.data(), sizeof(RECT), size, fp_read);

	fclose(fp_read);
	return true;
}
