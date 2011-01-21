/*
 * Library for WinUSB/libusb automated driver installation
 * Copyright (c) 2010 Pete Batard <pbatard@gmail.com>
 * Parts of the code from libusb by Daniel Drake, Johannes Erdfelt et al.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
#pragma once
#include <windows.h>

#if !defined(bool)
#define bool BOOL
#endif
#if !defined(true)
#define true TRUE
#endif
#if !defined(false)
#define false FALSE
#endif

#if defined(DLL_EXPORT)
#define LIBWDI_EXP __declspec(dllexport)
#else
#define LIBWDI_EXP
#endif

/*
 * Set the default calling convention to WINAPI (__stdcall)
 */
#if !defined(LIBWDI_API)
#define LIBWDI_API WINAPI
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Type of driver to install
 */
enum wdi_driver_type {
	WDI_WINUSB,
	WDI_LIBUSB,
	WDI_USER,
	WDI_NB_DRIVERS	// Total number of drivers in the enum
};

/*
 * Log level
 */
enum wdi_log_level {
	WDI_LOG_LEVEL_DEBUG,
	WDI_LOG_LEVEL_INFO,
	WDI_LOG_LEVEL_WARNING,
	WDI_LOG_LEVEL_ERROR,
	WDI_LOG_LEVEL_NONE
};

/*
 * Error codes. Most libwdi functions return 0 on success or one of these
 * codes on failure.
 * You can use wdi_strerror() to retrieve a short string description of
 * a wdi_error enumeration value.
 */
enum wdi_error {
	/** Success (no error) */
	WDI_SUCCESS = 0,

	/** Input/output error */
	WDI_ERROR_IO = -1,

	/** Invalid parameter */
	WDI_ERROR_INVALID_PARAM = -2,

	/** Access denied (insufficient permissions) */
	WDI_ERROR_ACCESS = -3,

	/** No such device (it may have been disconnected) */
	WDI_ERROR_NO_DEVICE = -4,

	/** Entity not found */
	WDI_ERROR_NOT_FOUND = -5,

	/** Resource busy, or API call already running */
	WDI_ERROR_BUSY = -6,

	/** Operation timed out */
	WDI_ERROR_TIMEOUT = -7,

	/** Overflow */
	WDI_ERROR_OVERFLOW = -8,

	/** Another installation is pending */
	WDI_ERROR_PENDING_INSTALLATION = -9,

	/** System call interrupted (perhaps due to signal) */
	WDI_ERROR_INTERRUPTED = -10,

	/** Could not acquire resource (Insufficient memory, etc) */
	WDI_ERROR_RESOURCE = -11,

	/** Operation not supported or unimplemented on this platform */
	WDI_ERROR_NOT_SUPPORTED = -12,

	/** Entity already exists */
	WDI_ERROR_EXISTS = -13,

	/** Cancelled by user */
	WDI_ERROR_USER_CANCEL = -14,

	/** Couldn't run installer with required privileges */
	WDI_ERROR_NEEDS_ADMIN = -15,

	/** Attempted to run the 32 bit installer on 64 bit */
	WDI_ERROR_WOW64 = -16,

	/** Bad inf syntax */
	WDI_ERROR_INF_SYNTAX = -17,

	/** Missing cat file */
	WDI_ERROR_CAT_MISSING = -18,

	/** System policy prevents the installation of unsigned drivers */
	WDI_ERROR_UNSIGNED = -19,

	/** Other error */
	WDI_ERROR_OTHER = -99

	/** IMPORTANT: when adding new values to this enum, remember to
	   update the wdi_strerror() function implementation! */
};


/*
 * Device information structure, used by libwdi functions
 */
struct wdi_device_info {
	/** (Optional) Pointer to the next element in the chained list. NULL if unused */
	struct wdi_device_info *next;
	/** USB VID */
	unsigned short vid;
	/** USB PID */
	unsigned short pid;
	/** Whether the USB device is composite */
	bool is_composite;
	/** (Optional) Composite USB interface number */
	unsigned char mi;
	/** USB Device description, usually provided by the device irself */
	char* desc;
	/** Windows' driver (service) name */
	char* driver;
	/** (Optional) Microsoft's device URI string. NULL if unused */
	char* device_id;
	/** (Optional) Microsoft's hardware ID string. NULL if unused */
	char* hardware_id;
};

/*
 * Optional settings, used by libwdi functions
 */

// wdi_create_list options
struct wdi_options_create_list {
	/** list all devices, instead of just the ones that are driverless */
	bool list_all;
	/** also list generic hubs and composite parent devices */
	bool list_hubs;
	/** trim trailing whitespaces from the description string */
	bool trim_whitespaces;
};

// wdi_prepare_driver options:
struct wdi_options_prepare_driver {
	/** type of driver to use. Should be either WDI_WINUSB, WDI_LIBUSB or WDI_USER */
	int driver_type;
	/** Vendor name that should be used for the Manufacturer in the inf */
	char* vendor_name;
};

// wdi_install_driver options:
struct wdi_options_install_driver {
	/** Handle to a Window application that should receive a modal progress dialog */
	HWND hWnd;
};

/*
 * Convert a libwdi error to a human readable error message
 */
LIBWDI_EXP const char* LIBWDI_API wdi_strerror(int errcode);

/*
 * Check if a specific driver is supported (embedded) in the current version of libwdi
 */
LIBWDI_EXP bool LIBWDI_API wdi_is_driver_supported(int driver_type, VS_FIXEDFILEINFO* driver_info);

/*
 * Retrieve the full Vendor name from a Vendor ID (VID)
 */
LIBWDI_EXP const char* LIBWDI_API wdi_get_vendor_name(unsigned short vid);

/*
 * Return a wdi_device_info list of USB devices
 * parameter: driverless_only - boolean
 */
LIBWDI_EXP int LIBWDI_API wdi_create_list(struct wdi_device_info** list,
							   struct wdi_options_create_list* options);

/*
 * Release a wdi_device_info list allocated by the previous call
 */
LIBWDI_EXP int LIBWDI_API wdi_destroy_list(struct wdi_device_info* list);

/*
 * Create an inf file for a specific device
 */
LIBWDI_EXP int LIBWDI_API wdi_prepare_driver(struct wdi_device_info* device_info, char* path,
								  char* inf_name, struct wdi_options_prepare_driver* options);

/*
 * Install a driver for a specific device
 */
LIBWDI_EXP int LIBWDI_API wdi_install_driver(struct wdi_device_info* device_info, char* path,
								  char* inf_name, struct wdi_options_install_driver* options);

/*
 * Install a code signing certificate (from embedded resources) into
 * the Trusted Publisher repository. Requires elevated privileges when
 * running the application on Vista and later.
 */
LIBWDI_EXP int LIBWDI_API wdi_install_trusted_certificate(char* cert_name);

/*
 * Set the log verbosity
 */
LIBWDI_EXP int LIBWDI_API wdi_set_log_level(int level);

/*
 * Set the Windows callback message for log notification
 */
LIBWDI_EXP int LIBWDI_API wdi_register_logger(HWND hWnd, UINT message, DWORD buffsize);

/*
 * Unset the Windows callback message for log notification
 */
LIBWDI_EXP int LIBWDI_API wdi_unregister_logger(HWND hWnd);

/*
 * Read a log message after a log notification
 */
LIBWDI_EXP int LIBWDI_API wdi_read_logger(char* buffer, DWORD buffer_size, DWORD* message_size);

#ifdef __cplusplus
}
#endif
