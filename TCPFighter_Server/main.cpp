#include "App.h"

int main()
{
	CApp app;

	if ((app.Init()) == false)
		return 0;

	if ((app.Start()) == false)
	{
		app.End();
		return 0;
	}

	app.Run();
	app.End();

	return 0;
}
