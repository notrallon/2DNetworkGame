#pragma once

#if _DEBUG
#pragma comment(lib, "sfml-system-d.lib")
#pragma comment(lib, "sfml-window-d.lib")
#pragma comment(lib, "sfml-graphics-d.lib")
#else
#pragma comment(lib, "sfml-system.lib")	
#pragma comment(lib, "sfml-window.lib")	
#pragma comment(lib, "sfml-graphics.lib")	
#endif