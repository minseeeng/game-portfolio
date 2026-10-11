#include "App.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	CApp app;

	if (!app.Init(hInstance))
		return 0;

	app.Show(nCmdShow);

	return app.Run();
}