#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <memory>
#define NOMINMAX  //for directx

// SDL Headers
#include "SDL.h"
#include "SDL_syswm.h"
#include "SDL_surface.h"
#include "SDL_image.h"

// DirectX Headers
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>

// Framework Headers
#include "Timer.h"
#include "Math.h"

#define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p) = nullptr; } }

#define RED_TEXT(text) "\033[1;31m" text "\033[0m"
#define GREEN_TEXT(text) "\033[1;32m" text "\033[0m"
#define MAGENTA_TEXT(text) "\033[1;35m" text "\033[0m"
#define YELLOW_TEXT(text) "\033[1;33m" text "\033[0m"

#define ONE_TAB "\t"
#define TWO_TABS "\t\t"
#define THREE_TABS "\t\t\t"


