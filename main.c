/* vim:set ts=4 sw=4 tw=80 et cindent ai si cino=(0,ml,\:0:
 * ( settings from: http://datapax.com.au/code_conventions/ )
 */

/**********************************************************************
    RatSlap
    Copyright (C) 2016 Todd Harbour

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    version 2 ONLY, as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program, in the file COPYING or COPYING.txt; if
    not, see http://www.gnu.org/licenses/ , or write to:
      The Free Software Foundation, Inc.,
      51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 **********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <libusb-1.0/libusb.h>

#include "app.h"
#include "lang.h"
#include "git.h"
#include "log.h"

libusb_context                     *usb_ctx        = NULL;
libusb_device_handle               *usb_dev_handle = NULL;
libusb_device                      *usb_device     = NULL;
struct libusb_device_descriptor    usb_desc;

static void help_version(void) {
    printf("%s v%s (BUILT: %s)\n", (APP_NAME), (APP_VERSION), (BUILD_DATE));
    printf("%s\n", (APP_COPYRIGHT));
    printf("%s\n", (APP_SUMMARY));
    printf("%s\n", (APP_URL));
}

static void help_usage(void) {
    printf("\
\n\
%s\n\
\n\
%s: %s -h|--help\n\
       %s -V|--version\n\
\n\
-h|--h[elp]             - %s\n\
-V|--v[ersion]          - %s %s %s\n\
\n\
%s: %s --help\n\
",
     _(APP_SUMMARY)

    ,_("Usage"),    BIN_NAME /* -h|--h[elp] */
    ,               BIN_NAME /* -V|--v[ersion] */

    ,_("Displays this help")
    ,_("Displays"), APP_NAME, _("version")
    ,_("Example"),  BIN_NAME
    );
}

static libusb_context *usb_init(void) {
    if (usb_ctx) return usb_ctx;

    // Initialise the USB context
    libusb_init(&usb_ctx);

    if (!usb_ctx) {
        elog("ERROR: Failed to initialise USB interface\n");
        return NULL;
    }

    // FIXME: Set debug?
    libusb_set_debug(usb_ctx, 3);

    return usb_ctx;
}

static int usb_deinit(void) {
    // Finish up with USB context
    if (usb_ctx) libusb_exit(usb_ctx);
    usb_ctx = NULL;

    return 1;
}

static libusb_device_handle *mouse_init(const uint16_t vendor_id, const uint16_t product_id, const char* product_name) {
    if (!usb_ctx) return NULL;

    if (usb_dev_handle) return usb_dev_handle;

    // Look at the mouse based on vendor and usb_device id
    // FIXME: According to doco (
    // http://libusb.sourceforge.net/api-1.0/group__dev.html#ga11ba48adb896b1492bbd3d0bf7e0f665
    // ):
    // > This function is intended for those scenarios where you are using
    // > libusb to knock up a quick test application - it allows you to avoid
    // > calling libusb_get_device_list() and worrying about traversing/freeing
    // > the list.
    //
    // > This function has limitations and is hence not intended for use in real
    // > applications: if multiple devices have the same IDs it will only give
    // > you the first one, etc.

    usb_dev_handle = libusb_open_device_with_vid_pid(usb_ctx, vendor_id, product_id);
    if (!usb_dev_handle) {
        elog("Failed to find %s (%.4x:%.4x)\n", product_name, vendor_id, product_id);
        return NULL;
    }

    printf("Found %s (%.4x:%.4x) @ 0x%p\n", product_name, vendor_id, product_id, usb_dev_handle);

    usb_device = libusb_get_device(usb_dev_handle);
    if (!usb_device) {
        elog("ERROR: Failed to retrieve usb_device info @ 0x%p\n", usb_dev_handle);
        return NULL;
    }

    // Get usb_device descriptor
    if (libusb_get_device_descriptor(usb_device, &usb_desc) != 0) {
        elog("WARNING: Failed to retrieve usb_device descriptor @ 0x%p\n", usb_dev_handle);
    } else {
        dlog(LOG_USB, "USB Device (%.4x:%.4x @ 0x%p) Descriptor:\n", vendor_id, product_id, usb_dev_handle);
        dlog(LOG_USB, "  bLength:            %d\n",     usb_desc.bLength           );
        dlog(LOG_USB, "  bDescriptorType:    %d\n",     usb_desc.bDescriptorType   );
        dlog(LOG_USB, "  bcdUSB:             0x%.4x\n", usb_desc.bcdUSB            );
        dlog(LOG_USB, "  bDeviceClass:       %d\n",     usb_desc.bDeviceClass      );
        dlog(LOG_USB, "  bDeviceSubClass:    %d\n",     usb_desc.bDeviceSubClass   );
        dlog(LOG_USB, "  bDeviceProtocol:    %d\n",     usb_desc.bDeviceProtocol   );
        dlog(LOG_USB, "  bMaxPacketSize0:    %d\n",     usb_desc.bMaxPacketSize0   );
        dlog(LOG_USB, "  idVendor:           0x%.4x\n", usb_desc.idVendor          );
        dlog(LOG_USB, "  idProduct:          0x%.4x\n", usb_desc.idProduct         );
        dlog(LOG_USB, "  bcdDevice:          0x%.4x\n", usb_desc.bcdDevice         );
        dlog(LOG_USB, "  iManufacturer:      %d\n",     usb_desc.iManufacturer     );
        dlog(LOG_USB, "  iProduct:           %d\n",     usb_desc.iProduct          );
        dlog(LOG_USB, "  iSerialNumber:      %d\n",     usb_desc.iSerialNumber     );
        dlog(LOG_USB, "  bNumConfigurations: %d\n",     usb_desc.bNumConfigurations);
    }

    return usb_dev_handle;
}

static int mouse_deinit(void) {
    // Finish up with mouse
    if (usb_dev_handle) libusb_close(usb_dev_handle);
    usb_dev_handle = NULL;

    return 1;
}

int main (int argc, char *argv[]) {
    int c;

    log_init();

    help_version();

    while (1) {
        int option_index = 0;
        static struct option long_options[] = {
            /* name, has_arg, flag, val
             *   has_arg = 0 (no), 1 (reqd), 2 (opt)
             *   flag = NULL (getopt_long returns val),
             *     other (getopt_long returns 0, flag = val
             *     if option found)
             *   val = val to return, or load into var pointed to
             *     by flag
             */
            {"help",        0, 0, 'h'},
            {"version",     0, 0, 'V'},

            {0,0,0,0}
        };

        c = getopt_long(argc, argv, "hV",
                long_options, &option_index);

        if (c == -1)
            break;

        switch (c) {
            // Help
            case 'h':
                help_usage();
                exit(0);

            break;

            // Version
            case 'V':
                // Already printed it

                exit(0);

            break;

            case '?':
                break;

            default:
                printf("?? getopt returned character code 0%o ??\n", c);
        } // switch (c)
    } // while (1)

    if (optind < argc) {
        char optout[255]
             ,*po = &optout[0];

        while (optind < argc) {
            sprintf(po, "%s ", argv[optind++]);
            po += strlen(po);
        }
        elog("ERROR: Unrecognised options: %s\n", optout);
    }

    // Initialise USB
    usb_init();

    // Initialise mouse
    // ID 046d:c246 == Logitech, Inc. Gaming Mouse G300
    if (!mouse_init(0x046d, 0xc246, "Logitech G300s")) {
        mouse_deinit();
        usb_deinit();
        log_end();
        return 1;
    }

    // MAIN FUNCTIONALITY HERE

    // De-initialise mouse
    mouse_deinit();

    // De-initialise USB
    usb_deinit();

    log_end();

    return 0;
}
