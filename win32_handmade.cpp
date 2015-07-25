
#include <Windows.h>

#define internal static 
#define local_persist static 
#define global_variable static 

global_variable bool running;
global_variable BITMAPINFO bitmapInfo;
global_variable void *bitmapMemory;
global_variable HBITMAP bitmapHandle;
global_variable HDC bitmapDeviceContext;


internal void Win32UpdateWindow(HDC deviceContext, int x, int y, int width, int height) {

	StretchDIBits(
		deviceContext,
		x, y, width, height,
		x, y, width, height,
		&bitmapMemory,
		&bitmapInfo,
		DIB_RGB_COLORS, SRCCOPY);
}

// Device independent bitmap
internal void Win32ResizeDIBSection(int width, int height) {

	if (bitmapHandle) {
		DeleteObject(bitmapHandle);
	} else{
		bitmapDeviceContext = CreateCompatibleDC(0);
	}

	bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
	bitmapInfo.bmiHeader.biWidth = width;
	bitmapInfo.bmiHeader.biHeight = height;
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biBitCount = 32;
	bitmapInfo.bmiHeader.biCompression = BI_RGB;

	bitmapHandle = CreateDIBSection(
		bitmapDeviceContext,
		&bitmapInfo,
		DIB_RGB_COLORS,
		&bitmapMemory,
		0, 0);
}

LRESULT CALLBACK Win32MainWindowCallback(
	HWND   hwnd,
	UINT   uMsg,
	WPARAM wParam,
	LPARAM lParam) {

	LRESULT result = 0;

	switch (uMsg) {
		case WM_SIZE: {

			RECT ClientRect;
			GetClientRect(hwnd, &ClientRect);
			int height = ClientRect.bottom - ClientRect.top;
			int width = ClientRect.right - ClientRect.left;

			Win32ResizeDIBSection(width, height);
			OutputDebugString("WM_SIZE\n");
			break;
		} 

		case WM_DESTROY: {
			running = false;
			break;
		} 

		case WM_CLOSE: {
			running = false;
			break;
		} 

		case WM_ACTIVATEAPP: {
			OutputDebugString("WM_ACTIVATEAPP\n");
			break;
		} 

		case WM_PAINT: {
			PAINTSTRUCT Paint;
			HDC deviceContext = BeginPaint(hwnd, &Paint);
			int x = Paint.rcPaint.left;
			int y = Paint.rcPaint.top;
			int height = Paint.rcPaint.bottom - Paint.rcPaint.top;
			int width = Paint.rcPaint.right - Paint.rcPaint.left;

			Win32UpdateWindow(deviceContext, x, y, width, height);

			EndPaint(hwnd, &Paint);
			break;
		}

		default: {
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
	WindowClass.lpfnWndProc = Win32MainWindowCallback;
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

		running = true;
		MSG Message;

		while (running) {
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

