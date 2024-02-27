#define UNICODE
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#ifdef __cplusplus
#define DEFAULT_VALUE \
  {}
#else
#define DEFAULT_VALUE \
  { 0 }
#endif

#define PACK_RGB555(r, g, b)                                            \
  ((UINT16)((((UINT16)(r) & 0xF8) << 7) | (((UINT16)(g) & 0xF8) << 2) | \
            ((UINT16)(b) >> 3)))

#define CLAMP(value, min_val, max_val) \
  ((value) < (min_val) ? (min_val)     \
                       : ((value) > (max_val) ? (max_val) : (value)))

enum {
  kTimeStep = 33,
  kBufferX = 640,
  kBufferY = 480,
  kBufferSize = sizeof(UINT16) * kBufferX * kBufferY,
  kBitsPerPixel = sizeof(UINT16) * 8,
  kBackBufferX = kBufferX / 2,
  kBackBufferY = kBufferY / 2,
  kBackBufferPixelCount = kBackBufferX * kBackBufferY,
  kClientWidth = 640,
  kClientHeight = 480,
  kFrequency = 1000,
  kDuration = 100
};

const FLOAT kSpeed = 100.0F;
const FLOAT kDeltaTime = (FLOAT)(kTimeStep) / 1E3F;
static LPCWSTR kClassName = TEXT("Main");
static LPCWSTR kTitle = TEXT("App");

static UINT16* g_buffer = DEFAULT_VALUE;
static BITMAPINFO g_info = {
    {sizeof(BITMAPINFOHEADER), kBackBufferX, -kBackBufferY, 1,
     (WORD)(kBitsPerPixel), BI_RGB, 0, 0, 0, 0, 0},
    {{0, 0, 0, 0}}};
static INT g_page = DEFAULT_VALUE;
static UINT16* g_back_buffer = DEFAULT_VALUE;

LRESULT CALLBACK MainWindowCallback(HWND window, UINT msg, WPARAM w_param,
                                    LPARAM l_param);

static LONGLONG GetMilliseconds();
static void Warn(BOOL condition, LPCWSTR msg);
static HWND CreateMainWindow(HINSTANCE instance);

int WINAPI wWinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE prev_instance,
                    _In_ PWSTR cmd_line, _In_ int cmd_show) {
  UNREFERENCED_PARAMETER(prev_instance);
  UNREFERENCED_PARAMETER(cmd_line);
  UNREFERENCED_PARAMETER(cmd_show);

  HWND window = CreateMainWindow(instance);
  Warn(window != NULL, TEXT("Call to CreateMainWindow failed!"));

  g_buffer = (UINT16*)VirtualAlloc(0, kBufferSize, MEM_COMMIT | MEM_RESERVE,
                                   PAGE_READWRITE);
  g_back_buffer = g_buffer;

  INT x = kBackBufferX / 2;
  INT y = kBackBufferY / 2;
  FLOAT velocity_x = 0.0F;
  FLOAT velocity_y = 0.0F;
  LONGLONG simulation_time = 0;
  MSG msg = DEFAULT_VALUE;
  BOOL running = TRUE;
  LONGLONG last_time = GetMilliseconds();
  while (running) {
    const LONGLONG real_time = GetMilliseconds();
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT) {
        running = FALSE;
      }

      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    if (GetAsyncKeyState('W') & 0x8000) {
      velocity_y = -kSpeed;
    } else if (GetAsyncKeyState('S') & 0x8000) {
      velocity_y = kSpeed;
    } else {
      velocity_y = 0.0F;
    }

    if (GetAsyncKeyState('A') & 0x8000) {
      velocity_x = -kSpeed;
    } else if (GetAsyncKeyState('D') & 0x8000) {
      velocity_x = kSpeed;
    } else {
      velocity_x = 0.0F;
    }

    while (simulation_time < real_time) {
      x = CLAMP(x + (INT)(velocity_x * kDeltaTime), 0, kBackBufferX - 1);
      y = CLAMP(y + (INT)(velocity_y * kDeltaTime), 0, kBackBufferY - 1);
      simulation_time += kTimeStep;
    }

    // Clear.
    for (INT i = 0; i < kBackBufferPixelCount; ++i) {
      g_back_buffer[i] = PACK_RGB555(251, 126, 20);
    }

    // Draw.
    g_back_buffer[x + kBackBufferX * y] = PACK_RGB555(21, 21, 21);

    // Swap.
    g_page = 1 - g_page;
    g_back_buffer = g_buffer + kBackBufferPixelCount * g_page;

    // Display.
    HDC device_context = GetDC(window);
    StretchDIBits(device_context, 0, 0, kClientWidth, kClientHeight, 0, 0,
                  kBackBufferX, kBackBufferY, g_back_buffer, &g_info,
                  DIB_RGB_COLORS, SRCCOPY);
    ReleaseDC(window, device_context);

    // Cap framerate.
    LONGLONG current_time = GetMilliseconds();
    LONGLONG elapsed_time = current_time > last_time ? current_time - last_time
                                                     : last_time - current_time;
    if (elapsed_time < kTimeStep) {
      Sleep((DWORD)(kTimeStep - elapsed_time));
    }
    last_time = GetMilliseconds();
  }

  VirtualFree(g_buffer, 0, MEM_RELEASE);
  return 0;
}

static LONGLONG GetMilliseconds() {
  LARGE_INTEGER performance_frequency = DEFAULT_VALUE;
  QueryPerformanceFrequency(&performance_frequency);
  LARGE_INTEGER current_time = DEFAULT_VALUE;
  QueryPerformanceCounter(&current_time);
  return (current_time.QuadPart * 1000) / performance_frequency.QuadPart;
}

LRESULT CALLBACK MainWindowCallback(HWND window, UINT message, WPARAM w_param,
                                    LPARAM l_param) {
  LRESULT result = 0;

  switch (message) {
    case WM_CLOSE: {
      Beep(kFrequency, kDuration);
      if (MessageBox(window, TEXT("Really quit?"), TEXT("Quit"), MB_OKCANCEL) ==
          IDOK) {
        DestroyWindow(window);
      }
    } break;
    case WM_DESTROY: {
      PostQuitMessage(0);
    } break;
    default: {
      result = DefWindowProc(window, message, w_param, l_param);
    } break;
  }

  return result;
}

static void Warn(BOOL condition, LPCWSTR msg) {
  if (condition == FALSE) {
    MessageBox(NULL, msg, TEXT("Warning"), 0);
  }
}

static HWND CreateMainWindow(HINSTANCE instance) {
  // Register window class.
  WNDCLASSEXW window_class = DEFAULT_VALUE;
  window_class.cbSize = sizeof(WNDCLASSEX);
  window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  window_class.lpfnWndProc = MainWindowCallback;
  window_class.cbClsExtra = 0;
  window_class.cbWndExtra = 0;
  window_class.hInstance = instance;
  window_class.hIcon = LoadIcon(instance, IDI_APPLICATION);
  window_class.hCursor = LoadCursor(instance, IDC_ARROW);
  window_class.hbrBackground = (HBRUSH)(GetStockObject(BLACK_BRUSH));
  window_class.lpszMenuName = NULL;
  window_class.lpszClassName = kClassName;
  window_class.hIconSm = LoadIcon(instance, IDI_APPLICATION);
  const BOOL ok = RegisterClassEx(&window_class) != 0;
  if (ok == FALSE) {
    return NULL;
  }
  // Get the needed window size to fit the client area.
  RECT window_size = {0, 0, kClientWidth, kClientHeight};
  AdjustWindowRect(&window_size, WS_SYSMENU | WS_CAPTION, FALSE);
  // Create window.
  HWND window = CreateWindowEx(
      WS_OVERLAPPED, kClassName, kTitle, WS_SYSMENU | WS_CAPTION, CW_USEDEFAULT,
      CW_USEDEFAULT, window_size.right - window_size.left,
      window_size.bottom - window_size.top, NULL, NULL, instance, NULL);

  if (window != NULL) {
    ShowWindow(window, SW_SHOWDEFAULT);
    UpdateWindow(window);
  }
  return window;
}
