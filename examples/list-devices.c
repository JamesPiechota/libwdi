/*
* wdi-simple.c: Console Driver Installer for a single USB device
* Copyright (c) 2010-2016 Pete Batard <pete@akeo.ie>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 3 of the License, or (at your option) any later version.
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef _MSC_VER
#include "getopt/getopt.h"
#else
#include <getopt.h>
#endif
#include "libwdi.h"

#if defined(_PREFAST_)
/* Disable "Banned API Usage:" errors when using WDK's OACR/Prefast */
#pragma warning(disable:28719)
/* Disable "Consider using 'GetTickCount64' instead of 'GetTickCount'" when using WDK's OACR/Prefast */
#pragma warning(disable:28159)
#endif

void usage(void)
{
	printf("\n");
	printf("-v, --vid <id>             filter list to this vendor ID (VID, use 0x prefix for hex)\n");
	printf("-p, --pid <id>             filter list to this product ID (PID, use 0x prefix for hex)\n");
	printf("-h, --help                 display usage\n");
	printf("\n");
}

int __cdecl main(int argc, char** argv)
{
	static struct wdi_device_info *ldev;
	static struct wdi_options_create_list ocl = { 0 };
	static unsigned short vid = 0, pid = 0, mi = 0;
	int c, r;

	static struct option long_options[] = {
		{"vid", required_argument, 0, 'v'},
		{"pid", required_argument, 0, 'p'},
		{"iid", required_argument, 0, 'i'},
		{"help", no_argument, 0, 'h'},
		{0, 0, 0, 0}
	};

	ocl.list_all = TRUE;
	ocl.list_hubs = TRUE;
	ocl.trim_whitespaces = TRUE;

	while(1)
	{
		c = getopt_long(argc, argv, "v:p:i:h", long_options, NULL);
		if (c == -1)
			break;
		switch(c) {
		case 'v':
			vid = (unsigned short)strtol(optarg, NULL, 0);
			break;
		case 'p':
			pid = (unsigned short)strtol(optarg, NULL, 0);
			break;
		case 'i':
			mi = (unsigned char)strtol(optarg, NULL, 0);
			break;
		case 'h':
			usage();
			exit(0);
			break;
		default:
			usage();
			exit(0);
		}
	}

	// minimal output
	wdi_set_log_level(4);

	if (wdi_create_list(&ldev, &ocl) == WDI_SUCCESS) {
		r = WDI_SUCCESS;
		for (; (ldev != NULL) && (r == WDI_SUCCESS); ldev = ldev->next) {
			if ((0 == vid || (ldev->vid == vid)) &&
				(0 == pid || (ldev->pid == pid)) &&
				(0 == mi || (ldev->mi == mi))) {
				printf("%s,%d,%d,%d,%s\n", ldev->desc, ldev->vid, ldev->pid, ldev->mi, ldev->driver);
			}
		}
	}
	return r;
}
