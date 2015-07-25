
#include <Windows.h>
#include <stdint.h>

#define internal static 
#define local_persist static 
#define global_variable static 

global_variable bool running;
global_variable BITMAPINFO bitmapInfo;
global_variable void *bitmapMemory;
global_variable int bytesPerPixel = 4;
global_variable int bitmapWidth;
global_variable int bitmapHeight;

internal void RenderWeirdGradient(int blueOffset, int greenOffset) {

	uint8_t *row = (uint8_t *)bitmapMemory;
	int pitch = bitmapWidth * bytesPerPixel;

	for (int y = 0; y < bitmapHeight; ++y) {
		uint32_t *pixel = (uint32_t *)row;
		for (int x = 0; x < bitmapWidth; ++x) {
			/*
			Pixel in memory: 00 00 00 00
			Little endian architecture
			BB GG RR xx
			*/

			uint8_t blue = (x + blueOffset);
			uint8_t green = (y + greenOffset);

			*pixel++ = ((green << 8) | blue);
		}

		row += pitch;
	}
}

internal void Win32UpdateWindow(HDC deviceContext, RECT *windowRect, int x, int y, int width, int height) {

	int windowWidth = windowRect->right - windowRect->left;
	int windowHeight = windowRect->bottom - windowRect->top;

	StretchDIBits(
		deviceContext,
		//x, y, width, height,
		//x, y, width, height,
		0, 0, bitmapWidth, bitmapHeight,
		0, 0, windowWidth, windowHeight,
		bitmapMemory,
		&bitmapInfo,
		DIB_RGB_COLORS, SRCCOPY);
}

// Device independent bitmap
internal void Win32ResizeDIBSection(int width, int height) {

	if (bitmapMemory) {
		VirtualFree(bitmapMemory, 0, MEM_RELEASE);
	}

	bitmapWidth = width;
	bitmapHeight = height;

	bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
	bitmapInfo.bmiHeader.biWidth = bitmapWidth;
	bitmapInfo.bmiHeader.biHeight = -bitmapHeight; // top down
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biBitCount = 32;
	bitmapInfo.bmiHeader.biCompression = BI_RGB;

	int bitmapMemorySize = bytesPerPixel * bitmapWidth * bitmapHeight;

	bitmapMemory = VirtualAlloc(0, bitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);	
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
			OutputDebugString("WM_PAINT\n");
			PAINTSTRUCT Paint;
			HDC deviceContext = BeginPaint(hwnd, &Paint);
			int x = Paint.rcPaint.left;
			int y = Paint.rcPaint.top;
			int height = Paint.rcPaint.bottom - Paint.rcPaint.top;
			int width = Paint.rcPaint.right - Paint.rcPaint.left;

			RECT clientRect;
			GetClientRect(hwnd, &clientRect);

			Win32UpdateWindow(deviceContext, &clientRect, x, y, width, height);

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

		int XOffset = 0;
		int YOffset = 0;

		running = true;
		MSG Message;

		while (running) {

			while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE)) {

				if (Message.message == WM_QUIT) {
					running = false;
				}

				TranslateMessage(&Message);
				DispatchMessage(&Message);
			}

			RenderWeirdGradient(XOffset, YOffset);

			HDC DeviceContext = GetDC(WindowHandle);
			RECT ClientRect;
			GetClientRect(WindowHandle, &ClientRect);
			int WindowWidth = ClientRect.right - ClientRect.left;
			int WindowHeight = ClientRect.bottom - ClientRect.top;
			Win32UpdateWindow(DeviceContext, &ClientRect, 0, 0, WindowWidth, WindowHeight);
			ReleaseDC(WindowHandle, DeviceContext);

			++XOffset;
			YOffset += 2;

		}
	}

	return(0);
}

