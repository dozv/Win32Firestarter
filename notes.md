

# Notes

2024-02-24

## C89 Standard

#### Clever use of static initialization (2.1.2)
```c
static const struct Foo kEmptyFoo;
struct Foo foo = kEmptyFoo;
```

Warning from `MSVC` may be a false positive:

`warning C4132: 'kEmptyFoo': const object should be initialized`

Alternatively, you can use the below macro to initialize structures.

```c
#define DEFAULT_VALUE \
  {                   \
    0                 \
  }
```

(Rust's `T::default()` trait inspired me to name it `DEFAULT_MACRO`.)

In C23 (and C++), you can simply use brace initialization `Foo foo = {}`.

References:

- https://stackoverflow.com/questions/6891720/initialize-reset-struct-to-zero-null


## Win32

#### Useful Win32 functions
Event handler:
- DestroyWindow (on `WM_CLOSE`)
- PostQuitMessage (on `WM_DESTROY`)
- DefWindowProc

Window creation:
- GetStockObject
- RegisterClassEx
- AdjustWindowRect
- CreateWindowEx
- ShowWindow
- UpdateWindow

Event loop:
- PeekMessage
- TranslateMessage
- DispatchMessage
- GetAsyncKeyState
- GetDC
- StretchDIBits
- ReleaseDC

Timing:
- QueryPerformanceFrequency

Memory:
- VirtualAlloc
- VirtualFree

#### Minimum members to set inside the `WNDCLASS`
```c
WNDCLASS wc;
wc.lpfnWndProc   = WindowProc;
wc.hInstance     = hInstance;
wc.lpszClassName = CLASS_NAME;
```

#### Event driven programming
Things like keyboard inputs, mouse movements in Windows are events.

Windows has a system event queue (global) and each Windows application has its own local event queue.

Hence, a mouse click occurs (a global event) and gets to an application's local event queue.
From there, within a callback function `WinProc`, you can act on each event (e.g., `WM_KEYDOWN`).
For instance, `WM_KEYDOWN` is an `UINT` with value `0x0100`.

#### Steps for a Win32 application
- Register a window class (not related to OOP)
- Get the needed window size to fit the client area (`AdjustWindowRect`)
- CreateWindow
- Show and update window
- Create an event loop (send events over to `WinProc`)
- Decide what to do on a given event (e.g., `WM_PAINT`) inside the `WinProc` callback

#### Client area
It is possible to draw on the client area without a `WM_PAINT` event inside the window's callback function (`WNDPROC`).
However, if you draw outside the window callback function, you cannot use `BeginPaint` and `EndPaint`.
You need to use `GetDC` and `ReleaseDC` instead.

References:

- https://learn.microsoft.com/en-us/windows/win32/learnwin32/creating-a-window

- https://learn.microsoft.com/en-us/windows/win32/learnwin32/learn-to-program-for-windows

- https://learn.microsoft.com/en-us/windows/win32/apiindex/windows-api-list

#### MSVC

References:

- https://github.com/cpp-best-practices/cppbestpractices/
- https://caiorss.github.io/C-Cpp-Notes/

#### NMAKE

References:

- https://nullprogram.com/blog/2017/08/20/
