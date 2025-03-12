/*
 $File: $
 $Date: $
 $Revision: $
 $Notice: (Copyright (c) 2025 VidalTorresMartinez. All Rights Reserved. $
=========================================================================
 */

/*typedef struct tagWNDCLASSA {
  UINT      style;
  WNDPROC   lpfnWndProc;
  int       cbClsExtra;
  int       cbWndExtra;
  HINSTANCE hInstance;
  HICON     hIcon;
  HCURSOR   hCursor;
  HBRUSH    hbrBackground;
  LPCSTR    lpszMenuName;
  LPCSTR    lpszClassName;
} WNDCLASSA, *PWNDCLASSA, *NPWNDCLASSA, *LPWNDCLASSA;*/

#include <cstdint>
#include <stdint.h>
#include <windows.h>

#define internal static
#define local_persist static
#define global_variable static

typedef uint8_t uint8;
typedef uint32_t uint32;

// TODO: GLOBAL FOR NOW
global_variable bool Running;

global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;
global_variable int BitmapWidth;
global_variable int BitmapHeight;

void internal Win32ResizeDIBSection(int Width, int Height) {

  if (BitmapMemory) {
    VirtualFree(BitmapMemory, 0, MEM_RELEASE);
  }
  BitmapWidth = Width;
  BitmapHeight = Height;

  BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
  BitmapInfo.bmiHeader.biWidth = BitmapWidth;
  BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
  BitmapInfo.bmiHeader.biPlanes = 1;
  BitmapInfo.bmiHeader.biBitCount = 32;
  BitmapInfo.bmiHeader.biCompression = BI_RGB;

  int BytesPerPixel = 4;
  int BitMapMemorySize = (Width * Height) * BytesPerPixel;
  BitmapMemory = VirtualAlloc(0, BitMapMemorySize, MEM_COMMIT, PAGE_READWRITE);

  int Pitch = Width * BytesPerPixel;
  uint8 *Row = (uint8 *)BitmapMemory;
  for (int Y = 0; Y < BitmapHeight; Y++) {
    uint32 *Pixel = (uint32 *)Row;
    for (int X = 0; X < BitmapWidth; X++) {
    }
    Row += Pitch;
  }
}

void internal Win32UpdateWindow(HDC DeviceContext, RECT *WindowRect, int X,
                                int Y, int Width, int Height) {
  /*StretchDIBits(DeviceContext, X, Y, Width, Height, X, Y, Width, Height,
                BitmapMemory, &BitmapInfo, DIB_RGB_COLORS, SRCCOPY);
  */
  int WindowWidth = WindowRect->right - WindowRect->left;
  int WindowHeight = WindowRect->bottom - WindowRect->top;

  StretchDIBits(DeviceContext, 0, 0, BitmapWidth, BitmapHeight, 0, 0,
                WindowWidth, WindowHeight, BitmapMemory, &BitmapInfo,
                DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK Win32MainWindowCallback(HWND Window, UINT Message,
                                         WPARAM WParam, LPARAM LParam) {
  LRESULT Result = 0;
  switch (Message) {

  case WM_SIZE: {
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    int width = ClientRect.right - ClientRect.left;
    int height = ClientRect.bottom - ClientRect.top;
    Win32ResizeDIBSection(width, height);
    OutputDebugString("WM_SIZE\n");
  } break;
  case WM_DESTROY: {
    // TODO: Try to recreate window?
    Running = false;
    OutputDebugString("WM_DESTROY\n");
  } break;
  case WM_CLOSE: {

    // TODO: Ask user if he is sure?
    // PostQuitMessage(0);
    Running = false;
    OutputDebugString("WM_CLOSE\n");
  } break;
  case WM_ACTIVATEAPP: {
    OutputDebugString("WM_ACTIVATEAPP\n");
  } break;
  case WM_PAINT: {
    PAINTSTRUCT Paint;
    HDC DeviceContext = BeginPaint(Window, &Paint);
    int X = Paint.rcPaint.left;
    int Y = Paint.rcPaint.top;
    int Width = Paint.rcPaint.right - Paint.rcPaint.left;
    int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;

    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    // int width = ClientRect.right - ClientRect.left;
    // int height = ClientRect.bottom - ClientRect.top;

    Win32UpdateWindow(DeviceContext, &ClientRect, X, Y, Width, Height);
    EndPaint(Window, &Paint);

  } break;
  default: {
    // OutputDebugString("DEFAULT\n");
    Result = DefWindowProc(Window, Message, WParam, LParam);
  } break;
  }
  return (Result);
}

int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE PrevInstance,
                     LPSTR CommandLine, int ShowCode) {
  WNDCLASS WindowClass = {}; // Sets all to 0

  WindowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
  WindowClass.lpfnWndProc = Win32MainWindowCallback;
  WindowClass.hInstance = Instance;
  // WindowClass.hIcon = ;
  WindowClass.lpszClassName = "HandmadeHeroWindowClass";
  if (RegisterClass(&WindowClass)) {
    HWND WindowHandle = CreateWindowEx(
        0, WindowClass.lpszClassName, "Handmade Hero",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, Instance, 0);

    if (WindowHandle) {
      Running = true;
      MSG Message;
      while (Running) {

        BOOL MessageResult = GetMessageA(&Message, 0, 0, 0);
        if (MessageResult > 0) {
          TranslateMessage(&Message);
          DispatchMessage(&Message);
        } else {
          break;
        }
      }

    } else {
      // TODO: Logging
    }
  } else {
    // TODO: Logging
  }

  return 0;
}
