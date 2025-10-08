#include "Engine/App/App.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    App app;
    app.Initialize();
    app.Run();
    app.Finalize();

	return 0;
}