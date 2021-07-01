#ifdef _WIN32
	#pragma once

	#include <winsock2.h>
	#include <windows.h>
	#include <Ws2tcpip.h>
#else
	#include <cstring>
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <netdb.h>
	#include <unistd.h>
	#include <math.h>
#endif

#include <string>
#include <vector>
#include <sstream>
#include <memory>
#include <algorithm>
#include <filesystem>
#include <exception>
