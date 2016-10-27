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
#include <linux/hid.h>

#include "app.h"
#include "lang.h"
#include "git.h"
#include "log.h"

#define LOGITECH_G300S_VENDOR_ID   0x046d
#define LOGITECH_G300S_PRODUCT_ID  0xc246

typedef enum e_mode {
     mode_f3 = 0
    ,mode_f4
    ,mode_f5
    ,mode_COUNT
} t_mode;

const char *s_mode[] = {
     "F3"
    ,"F4"
    ,"F5"
    ,"INVALID"
};

libusb_context                     *usb_ctx        = NULL;
libusb_device_handle               *usb_dev_handle = NULL;
libusb_device                      *usb_device     = NULL;
struct libusb_device_descriptor    usb_desc;
int usb_interface_index = -1;

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
       %s [-s|--select <mode>]\n\
\n\
-h|--h[elp]             - %s\n\
-V|--v[ersion]          - %s %s %s\n\
-s|--s[elect]           - %s\n\
\n\
<mode>                  - %s\n\
\n\
%s: %s --selec F3\n\
",
     _(APP_SUMMARY)

    ,_("Usage"),    BIN_NAME /* -h|--h[elp] */
    ,               BIN_NAME /* -V|--v[ersion] */
    ,               BIN_NAME /* -s|--s[elect] ... */

    ,_("Displays this help")
    ,_("Displays"), APP_NAME, _("version")
    ,_("Switches to <mode>")
    ,_("A valid mode:          F3, F4 or F5")
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

    if (!usb_dev_handle) {
        // Look at the mouse based on vendor and usb_device id
        // FIXME: According to doco (
        // http://libusb.sourceforge.net/api-1.0/group__dev.html#ga11ba48adb896b1492bbd3d0bf7e0f665
        // ):
        // > This function is intended for those scenarios where you are using
        // > libusb to knock up a quick test application - it allows you to
        // > avoid calling libusb_get_device_list() and worrying about
        // > traversing/freeing the list.
        //
        // > This function has limitations and is hence not intended for use
        // > in real applications: if multiple devices have the same IDs it
        // > will only give you the first one, etc.

        usb_dev_handle = libusb_open_device_with_vid_pid(usb_ctx, vendor_id, product_id);
        if (!usb_dev_handle) {
            elog("Failed to find %s (%.4x:%.4x)\n", product_name, vendor_id, product_id);
            return NULL;
        }

        printf("Found %s (%.4x:%.4x) @ 0x%p\n", product_name, vendor_id, product_id, usb_dev_handle);
    }

    if (!usb_device) {
        usb_device = libusb_get_device(usb_dev_handle);
        if (!usb_device) {
            elog("ERROR: Failed to retrieve usb_device info @ 0x%p\n", usb_dev_handle);
            return NULL;
        }
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
    usb_device     = NULL;

    return 1;
}

static void display_mouse_hid(const uint16_t vendor_id, const uint16_t product_id) {
    uint8_t config_index    = 0;
    uint8_t iface_index     = 0;

    if (!usb_device) return;

    if (usb_desc.idVendor != vendor_id || usb_desc.idProduct != product_id) {
        elog("WARNING: Device descriptor doesn't match expected device\n");
        return;
    }

    for (config_index = 0; config_index < usb_desc.bNumConfigurations; ++config_index) {
        int                             altsetting_index = 0;
        struct libusb_config_descriptor *config          = NULL;

        libusb_get_config_descriptor(usb_device, config_index, &config);

        dlog(LOG_USB, "USB Device @ 0x%p : Config %d:\n", usb_dev_handle, config_index);
        dlog(LOG_USB, "  bLength:         %d\n", config->bLength);
        dlog(LOG_USB, "  bDescriptorType: %d\n", config->bDescriptorType);
        dlog(LOG_USB, "  wTotalLength:    %d\n", config->wTotalLength);
        dlog(LOG_USB, "  iConfiguration:  %d\n", config->iConfiguration);
        dlog(LOG_USB, "  bmAttributes:    %d\n", config->bmAttributes);
        dlog(LOG_USB, "  MaxPower:        %d\n", config->MaxPower);
        dlog(LOG_USB, "  extra (%d):     \"%s\"\n", config->extra_length, config->extra);
        dlog(LOG_USB, "  bNumInterfaces:  %d\n", config->bNumInterfaces);

        for (iface_index = 0; iface_index < config->bNumInterfaces; ++iface_index) {
            const struct libusb_interface *iface = &config->interface[iface_index];

            dlog(LOG_USB, "  Interface: %d\n", iface_index);
            dlog(LOG_USB, "    num_altsetting: %d\n", iface->num_altsetting);

            for (altsetting_index = 0; altsetting_index < iface->num_altsetting; ++altsetting_index) {
                const struct libusb_interface_descriptor *iface_desc = &iface->altsetting[altsetting_index];

                dlog(LOG_USB, "    AltSetting: %d\n", altsetting_index);
                dlog(LOG_USB, "      Length:     %d\n", iface_desc->bLength);
                dlog(LOG_USB, "      DescType:   %d\n", iface_desc->bDescriptorType);
                dlog(LOG_USB, "      IFNum:      %d\n", iface_desc->bInterfaceNumber);
                dlog(LOG_USB, "      AltSetting: %d\n", iface_desc->bAlternateSetting);
                dlog(LOG_USB, "      NumEndPnts: %d\n", iface_desc->bNumEndpoints);
                dlog(LOG_USB, "      IFClass:    %d\n", iface_desc->bInterfaceClass);
                dlog(LOG_USB, "      IFSubClass: %d\n", iface_desc->bInterfaceSubClass);
                dlog(LOG_USB, "      IFProtocol: %d\n", iface_desc->bInterfaceProtocol);
                dlog(LOG_USB, "      Interface:  %d\n", iface_desc->iInterface);
                dlog(LOG_USB, "      extra:      (%d) \"%s\"\n", iface_desc->extra_length, iface_desc->extra);
            }
            altsetting_index = 0;
        }
    }
}

int mouse_hid_detach_kernel(int iface) {
    int ret = 0;

    if (!usb_dev_handle || iface < 0) return -1;

    ret = libusb_detach_kernel_driver(usb_dev_handle, iface);
    if (ret != 0) {
        elog("ERROR: Failed to detach kernel driver: %s\n", libusb_strerror(ret));
        return ret;
    }

    ret = libusb_claim_interface(usb_dev_handle, iface);
    if (ret != 0) {
        elog("ERROR: Failed to claim interface: %s\n", libusb_strerror(ret));

        // Reattach the kernel driver
        ret = libusb_attach_kernel_driver(usb_dev_handle, iface);
        if (ret != 0) {
            elog("ERROR: Failed to attach kernel driver: %s\n", libusb_strerror(ret));
        }

        return ret;
    }

    return 0;
}

int mouse_hid_attach_kernel(int iface) {
    int ret = 0;

    if (!usb_dev_handle || iface < 0) return -1;

    ret = libusb_release_interface(usb_dev_handle, iface);
    if (ret != 0) {
        elog("ERROR: Failed to release interface: %s\n", libusb_strerror(ret));
    }

    ret = libusb_attach_kernel_driver(usb_dev_handle, iface);
    if (ret != 0) {
        elog("ERROR: Failed to attach kernel driver: %s\n", libusb_strerror(ret));
        return ret;
    }

    return 0;
}

static t_mode change_mode(libusb_device_handle *usb_dev_handle, t_mode mode) {
    unsigned char payload[] = "\xf0\xff\x00\x00";

    int ret;

    if (!usb_dev_handle || mode >= mode_COUNT) return mode_COUNT;

    if (mode == mode_f3) {
        // Top Mode
        // S Co:2:039:0 s 21 09 03f0 0001 0004 4 = f0800000
        // NOTE: b0, c0, f0 also seem to work
        payload[1] = '\x80';

    } else
    if (mode == mode_f4) {
        // (Bottom) Right Mode
        // S Co:2:039:0 s 21 09 03f0 0001 0004 4 = f0900000
        // NOTE: d0 also seems to work
        payload[1] = '\x90';

    } else
    if (mode == mode_f5) {
        // (Bottom) Left Mode
        // S Co:2:039:0 s 21 09 03f0 0001 0004 4 = f0a00000
        // NOTE: e0 also seems to work
        payload[1] = '\xa0';

    } else {
        return mode_COUNT;

    }

    ret = libusb_control_transfer(
         usb_dev_handle
        ,LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE|LIBUSB_ENDPOINT_OUT
        ,HID_REQ_SET_REPORT
        ,0x03f0
        ,0x0001
        ,payload
        ,sizeof(payload) - 1
        ,1000);

    // This process takes time
    usleep(10000);

    dlog(LOG_USB, "  --> %d\n", ret);

    return mode;
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
            {"select",      1, 0, 's'},

            {0,0,0,0}
        };

        c = getopt_long(argc, argv, "hVs:",
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

            // Select Mode
            case 's':
            {
                t_mode mnew = mode_COUNT;

                if (!optarg) {
                    elog("ERROR: Mode required for select option\n");
                    continue;
                }

                for (mnew = 0; mnew < mode_COUNT; ++mnew) {
                    if (strcasecmp(optarg, s_mode[mnew]) == 0) break;
                }

                if (mnew == mode_COUNT) {
                    elog("ERROR: Invalid mode for select option: %s\n", optarg);
                    continue;
                }

                printf("Selecting Mode: %s\n", s_mode[mnew]);

                // Initialise USB
                usb_init();

                // Initialise mouse
                // ID 046d:c246 == Logitech, Inc. Gaming Mouse G300
                if (!mouse_init(LOGITECH_G300S_VENDOR_ID, LOGITECH_G300S_PRODUCT_ID, "Logitech G300s")) {
                    // Fall through to the next command
                    continue;
                }

                display_mouse_hid(LOGITECH_G300S_VENDOR_ID, LOGITECH_G300S_PRODUCT_ID);

                // FIXME: Get interface index somehow
                usb_interface_index = 1;
                if (usb_interface_index < 0) {
                    // Fall through to the next command
                    continue;
                }

                printf("Detaching kernel driver...\n");
                if (mouse_hid_detach_kernel(usb_interface_index) != 0) {
                    // Fall through to the next command
                    continue;
                }

                change_mode(usb_dev_handle, mnew);
            }
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



    printf("Attaching kernel driver...\n");
    mouse_hid_attach_kernel(usb_interface_index);

    // De-initialise mouse
    mouse_deinit();

    // De-initialise USB
    usb_deinit();

    log_end();

    return 0;
}
