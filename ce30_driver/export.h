#ifndef EXPORT_H
#define EXPORT_H

#ifdef _WIN32
#	ifdef ce30_driver_EXPORTS
#		define API __declspec(dllexport)
#	else
#		define API __declspec(dllimport)
#	endif
#else
#	define API
#endif // WIN32

#endif // EXPORT_H
