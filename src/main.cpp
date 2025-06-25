#include <windows.h>
#include <windowsx.h>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#include "elevator.h"
#include "floor.h"
#include "button.h"

#define GET_X_WPARAM(wp) ( static_cast<short>( (wp) & 0xFFFF ) )
#define GET_Y_WPARAM(wp) ( static_cast<short>( ((wp) >> 16) & 0xFFFF ) )

#define ELEVATOR_MOVE (WM_USER+1)
#define ELEVATOR_DROP (WM_USER + 2)
#define ELEVATOR_GRAB (WM_USER + 3)
#define ELEVATOR_WAIT (WM_USER + 4)

#define PERSON_ANIMATION_END (WM_USER+5)
#define PERSON_LEFT (WM_USER + 6)

Elevator elevator(nullptr, 1);

VOID OnPaint(HWND hWnd)
{
   elevator.draw(); // Draw the building, elevator, and floors
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow)
{
   HWND                hWnd;
   MSG                 msg;
   WNDCLASS            wndClass;
   GdiplusStartupInput gdiplusStartupInput;
   ULONG_PTR           gdiplusToken;
   
   // Initialize GDI+.
   GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
   
   wndClass.style          = CS_HREDRAW | CS_VREDRAW;
   wndClass.lpfnWndProc    = WndProc;
   wndClass.cbClsExtra     = 0;
   wndClass.cbWndExtra     = 0;
   wndClass.hInstance      = hInstance;
   wndClass.hIcon          = LoadIcon(NULL, IDI_APPLICATION);
   wndClass.hCursor        = LoadCursor(NULL, IDC_ARROW);
   wndClass.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
   wndClass.lpszMenuName   = NULL;
   wndClass.lpszClassName  = TEXT("tp4");
   
   RegisterClass(&wndClass);
   
   hWnd = CreateWindow(
      TEXT("tp4"),   // window class name
      TEXT("tp4"),  // window caption
      WS_OVERLAPPEDWINDOW,      // window style
      CW_USEDEFAULT,            // initial x position
      CW_USEDEFAULT,            // initial y position
      600,                      // initial x size
      480,                      // initial y size
      NULL,                     // parent window handle
      NULL,                     // window menu handle
      hInstance,                // program instance handle
      NULL);                    // creation parameters

   // Reinitialize the object
   new (&elevator) Elevator(hWnd, 5);
      
   ShowWindow(hWnd, iCmdShow);
   UpdateWindow(hWnd);
   
   while(GetMessage(&msg, NULL, 0, 0))
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }
   
   GdiplusShutdown(gdiplusToken);
   return msg.wParam;
}  // WinMain

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, 
   WPARAM wParam, LPARAM lParam)
{
   HDC          hdc;
   PAINTSTRUCT  ps;
   
   switch(message)
   {
   case WM_PAINT:
      hdc = BeginPaint(hWnd, &ps);
      OnPaint(hWnd);
      EndPaint(hWnd, &ps);
      return 0;
   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   case WM_MOUSEMOVE:
   case WM_LBUTTONDOWN:
   case WM_LBUTTONUP: {
      int x = GET_X_LPARAM(lParam);
      int y = GET_Y_LPARAM(lParam);
      for (auto& btn : elevator.getButtons()) {
         // Ensure handleMouse exists in Button class
         std::tuple<Floor*, Floor*> button = btn.handleMouse(message, x, y);
         if(std::get<0>(button) != std::get<1>(button)){
            Floor* floor = std::get<0>(button);
            Floor* destination = std::get<1>(button);
            floor->spawnPerson(destination);
            elevator.queueFloor(floor);
            elevator.queueFloor(destination);
            elevator.resetWait();
         }
      }
      break;
   }
   case ELEVATOR_MOVE: {
      const Floor* floor = reinterpret_cast<Floor*>(lParam);
      auto it = std::find_if(elevator.getFloors().begin(), elevator.getFloors().end(),
         [floor](const Floor& f) { return &f == floor; });
      if(it != elevator.getFloors().end()) {
         elevator.moveToFloor(floor, 1000);
      };
      break;
   }
   case ELEVATOR_DROP:
      elevator.dropPassengers();
      break;
   case ELEVATOR_GRAB:
      elevator.grabPassengers();
      break;
   case ELEVATOR_WAIT:
      elevator.awaitInput();
      break;
   case PERSON_ANIMATION_END: {
      Person* ptr = reinterpret_cast<Person*>(lParam);
      ptr->killThread();
      break;
   }
   case PERSON_LEFT: {
      Person* ptr = reinterpret_cast<Person*>(lParam);
      ptr->killThread();
      RECT invalidate;
      invalidate.left = ptr->getX();
      invalidate.top = ptr->getY();
      invalidate.right = ptr->getX() + ptr->getWidth() + 1;
      invalidate.bottom = ptr->getY() + ptr->getHeight() + 1;
      std::vector<Person>& leaving = ptr->getDestination()->getLeaving();
      auto it = std::find_if(leaving.begin(), leaving.end(),
         [ptr](const Person& p) { return &p == ptr; });
      if (it != leaving.end()) {
         leaving.erase(it);
      }
      InvalidateRect(hWnd, &invalidate, TRUE);
      break;
   }
   default:
      return DefWindowProc(hWnd, message, wParam, lParam);
   }
} // WndProc