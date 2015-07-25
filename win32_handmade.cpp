
#include <Windows.h>


LRESULT CALLBACK MainWindowCallback(
	HWND   hwnd,
	UINT   uMsg,
	WPARAM wParam,
	LPARAM lParam) {

	LRESULT result = 0;


	switch (uMsg) {
		case WM_SIZE: {
			OutputDebugStringA("WM_SIZE\n");
			break;
		} 
		case WM_DESTROY: {
			OutputDebugStringA("WM_DESTROY\n");
			break;
		} 
		case WM_CLOSE: {
			OutputDebugStringA("WM_CLOSE\n");
			break;
		} 
		case WM_ACTIVATEAPP: {
			OutputDebugStringA("WM_ACTIVATEAPP\n");
			break;
		} 
		case WM_PAINT: {
			PAINTSTRUCT Paint;
			HDC DeviceContext = BeginPaint(hwnd, &Paint);
			int x = Paint.rcPaint.left;
			int y = Paint.rcPaint.top;
			int height = Paint.rcPaint.bottom - Paint.rcPaint.top;
			int width = Paint.rcPaint.right - Paint.rcPaint.left;
			PatBlt(DeviceContext, x, y, width, height, WHITENESS);
			EndPaint(hwnd, &Paint);
			break;
		}
		default: {
			//OutputDebugStringA("default\n");
			result = DefWindowProc(hwnd, uMsg, wParam, lParam);
			break;
		} 
	}

	return result;

}

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow) {

	WNDCLASS WindowClass = {};
	
	WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
	WindowClass.lpfnWndProc = MainWindowCallback;
	WindowClass.hInstance = hInstance;
	WindowClass.lpszClassName = "HandmadeHeroWindowClass";

	RegisterClass(&WindowClass);
	HWND WindowHandle = CreateWindowEx(
		0,
		WindowClass.lpszClassName,
		"Handmade Hero",
		WS_OVERLAPPEDWINDOW|WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		0,
		0,
		hInstance,
		0);

	if (WindowHandle) {
		MSG Message;
		for (;;) {
			BOOL MesageResult = GetMessage(&Message, 0, 0, 0);
			if (MesageResult > 0) {
				TranslateMessage(&Message);
				DispatchMessage(&Message);
			}
			else {
				break;
			}
		}
	}

	return(0);
}

