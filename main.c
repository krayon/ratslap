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

// http://www.tldp.org/LDP/abs/html/exitcodes.html
typedef enum e_exit {
     exit_none    = 0
    ,exit_param   = 63
    ,exit_usberr
    ,exit_modesel
} t_exit;

typedef enum e_mode {
     mode_f3 = 0
    ,mode_f4
    ,mode_f5
    ,mode_COUNT
} t_mode;

typedef enum e_colour {
     colour_black = 0
    ,colour_red
    ,colour_green
    ,colour_yellow
    ,colour_blue
    ,colour_magenta
    ,colour_cyan
    ,colour_white
    ,colour_COUNT
} t_colour;

const char *s_mode[] = {
     "F3"
    ,"F4"
    ,"F5"
    ,"INVALID"
};

const char *s_buttons[] = {
     "NONE"
    ,"Button1"
    ,"Button2"
    ,"Button3"
    ,"Button6"
    ,"Button7"
    ,"Button8"
    ,"Button9"
    ,"Button10"
    ,"Button11"
    ,"DPIUp"
    ,"DPIDown"
    ,"DPICycle"
    ,"ModeSwitch"
    ,"DPIShift"
    ,"DPIDefault"
};

const char *s_keys[] = {
     "NONE"
    ,"UNKNOWN:01"
    ,"UNKNOWN:02"
    ,"UNKNOWN:03"
    ,"A"          // 04 ==   4
    ,"B"
    ,"C"
    ,"D"
    ,"E"
    ,"F"
    ,"G"
    ,"H"
    ,"I"
    ,"J"
    ,"K"
    ,"L"
    ,"M"
    ,"N"
    ,"O"
    ,"P"
    ,"Q"
    ,"R"
    ,"S"
    ,"T"
    ,"U"
    ,"V"
    ,"W"
    ,"X"
    ,"Y"
    ,"Z"          // 1D ==  29
    ,"1"          // 1E ==  30
    ,"2"
    ,"3"
    ,"4"
    ,"5"
    ,"6"
    ,"7"
    ,"8"
    ,"9"
    ,"0"           // 27 ==  39
    ,"Enter"       // 28 ==  40
    ,"Escape"      // 29 ==  41
    ,"Backspace"   // 2a ==  42
    ,"Tab"         // 2b ==  43
    ,"Space"       // 2c ==  44
    ,"-"           // 2d ==  45
    ,"="           // 2e ==  46
    ,"["           // 2f ==  47
    ,"]"           // 30 ==  48
    ,"\\"          // 31 ==  49
    ,"UNKNOWN:32"  // 32 ==  50
    ,";"           // 33 ==  51
    ,"'"           // 34 ==  52
    ,"`"           // 35 ==  53
    ,","           // 36 ==  54
    ,"."           // 37 ==  55
    ,"/"           // 38 ==  56
    ,"UNKNOWN:39"  // 39 ==  57
    ,"F1"          // 3a ==  58
    ,"F2"          // 3b ==  59
    ,"F3"          // 3c ==  60
    ,"F4"          // 3d ==  61
    ,"F5"          // 3e ==  62
    ,"F6"          // 3f ==  63
    ,"F7"          // 40 ==  64
    ,"F8"          // 41 ==  65
    ,"F9"          // 42 ==  66
    ,"F10"         // 43 ==  67
    ,"F11"         // 44 ==  68
    ,"F12"         // 45 ==  69
    ,"PrintScreen" // 46 ==  70
    ,"ScrollLock"  // 47 ==  71
    ,"Pause"       // 48 ==  72
    ,"Insert"      // 49 ==  73
    ,"Home"        // 4a ==  74
    ,"PageUp"      // 4b ==  75
    ,"Delete"      // 4c ==  76
    ,"End"         // 4d ==  77
    ,"PageDown"    // 4e ==  78
    ,"Right"       // 4f ==  79
    ,"Left"        // 50 ==  80
    ,"Down"        // 51 ==  81
    ,"Up"          // 52 ==  82
    ,"NumLock"     // 53 ==  83
    ,"Num/"        // 54 ==  84
    ,"Num*"        // 55 ==  85
    ,"Num-"        // 56 ==  86
    ,"Num+"        // 57 ==  87
    ,"NumEnter"    // 58 ==  88
    ,"Num1"        // 59 ==  89
    ,"Num2"        // 5a ==  90
    ,"Num3"        // 5b ==  91
    ,"Num4"        // 5c ==  92
    ,"Num5"        // 5d ==  93
    ,"Num6"        // 5e ==  94
    ,"Num7"        // 5f ==  95
    ,"Num8"        // 60 ==  96
    ,"Num9"        // 61 ==  97
    ,"Num0"        // 62 ==  98
    ,"Num."        // 63 ==  99
    ,"UNKNOWN:64"  // 64 == 100
    ,"Menu"        // 65 == 101
    ,"UNKNOWN:66"  // 66 == 102
    ,"UNKNOWN:67"  // 67 == 103
    ,"UNKNOWN:68"  // 68 == 104
    ,"UNKNOWN:69"  // 69 == 105
    ,"UNKNOWN:6a"  // 6a == 106
    ,"UNKNOWN:6b"  // 6b == 107
    ,"UNKNOWN:6c"  // 6c == 108
    ,"UNKNOWN:6d"  // 6d == 109
    ,"UNKNOWN:6e"  // 6e == 110
    ,"UNKNOWN:6f"  // 6f == 111
    ,"UNKNOWN:70"  // 70 == 112
    ,"UNKNOWN:71"  // 71 == 113
    ,"UNKNOWN:72"  // 72 == 114
    ,"UNKNOWN:73"  // 73 == 115
    ,"UNKNOWN:74"  // 74 == 116
    ,"UNKNOWN:75"  // 75 == 117
    ,"UNKNOWN:76"  // 76 == 118
    ,"UNKNOWN:77"  // 77 == 119
    ,"UNKNOWN:78"  // 78 == 120
    ,"UNKNOWN:79"  // 79 == 121
    ,"UNKNOWN:7a"  // 7a == 122
    ,"UNKNOWN:7b"  // 7b == 123
    ,"UNKNOWN:7c"  // 7c == 124
    ,"UNKNOWN:7d"  // 7d == 125
    ,"UNKNOWN:7e"  // 7e == 126
    ,"UNKNOWN:7f"  // 7f == 127
    ,"UNKNOWN:80"  // 80 == 128
    ,"UNKNOWN:81"  // 81 == 129
    ,"UNKNOWN:82"  // 82 == 130
    ,"UNKNOWN:83"  // 83 == 131
    ,"UNKNOWN:84"  // 84 == 132
    ,"UNKNOWN:85"  // 85 == 133
    ,"UNKNOWN:86"  // 86 == 134
    ,"UNKNOWN:87"  // 87 == 135
    ,"UNKNOWN:88"  // 88 == 136
    ,"UNKNOWN:89"  // 89 == 137
    ,"UNKNOWN:8a"  // 8a == 138
    ,"UNKNOWN:8b"  // 8b == 139
    ,"UNKNOWN:8c"  // 8c == 140
    ,"UNKNOWN:8d"  // 8d == 141
    ,"UNKNOWN:8e"  // 8e == 142
    ,"UNKNOWN:8f"  // 8f == 143
    ,"UNKNOWN:90"  // 90 == 144
    ,"UNKNOWN:91"  // 91 == 145
    ,"UNKNOWN:92"  // 92 == 146
    ,"UNKNOWN:93"  // 93 == 147
    ,"UNKNOWN:94"  // 94 == 148
    ,"UNKNOWN:95"  // 95 == 149
    ,"UNKNOWN:96"  // 96 == 150
    ,"UNKNOWN:97"  // 97 == 151
    ,"UNKNOWN:98"  // 98 == 152
    ,"UNKNOWN:99"  // 99 == 153
    ,"UNKNOWN:9a"  // 9a == 154
    ,"UNKNOWN:9b"  // 9b == 155
    ,"UNKNOWN:9c"  // 9c == 156
    ,"UNKNOWN:9d"  // 9d == 157
    ,"UNKNOWN:9e"  // 9e == 158
    ,"UNKNOWN:9f"  // 9f == 159
    ,"UNKNOWN:a0"  // a0 == 160
    ,"UNKNOWN:a1"  // a1 == 161
    ,"UNKNOWN:a2"  // a2 == 162
    ,"UNKNOWN:a3"  // a3 == 163
    ,"UNKNOWN:a4"  // a4 == 164
    ,"UNKNOWN:a5"  // a5 == 165
    ,"UNKNOWN:a6"  // a6 == 166
    ,"UNKNOWN:a7"  // a7 == 167
    ,"UNKNOWN:a8"  // a8 == 168
    ,"UNKNOWN:a9"  // a9 == 169
    ,"UNKNOWN:aa"  // aa == 170
    ,"UNKNOWN:ab"  // ab == 171
    ,"UNKNOWN:ac"  // ac == 172
    ,"UNKNOWN:ad"  // ad == 173
    ,"UNKNOWN:ae"  // ae == 174
    ,"UNKNOWN:af"  // af == 175
    ,"UNKNOWN:b0"  // b0 == 176
    ,"UNKNOWN:b1"  // b1 == 177
    ,"UNKNOWN:b2"  // b2 == 178
    ,"UNKNOWN:b3"  // b3 == 179
    ,"UNKNOWN:b4"  // b4 == 180
    ,"UNKNOWN:b5"  // b5 == 181
    ,"UNKNOWN:b6"  // b6 == 182
    ,"UNKNOWN:b7"  // b7 == 183
    ,"UNKNOWN:b8"  // b8 == 184
    ,"UNKNOWN:b9"  // b9 == 185
    ,"UNKNOWN:ba"  // ba == 186
    ,"UNKNOWN:bb"  // bb == 187
    ,"UNKNOWN:bc"  // bc == 188
    ,"UNKNOWN:bd"  // bd == 189
    ,"UNKNOWN:be"  // be == 190
    ,"UNKNOWN:bf"  // bf == 191
    ,"UNKNOWN:c0"  // c0 == 192
    ,"UNKNOWN:c1"  // c1 == 193
    ,"UNKNOWN:c2"  // c2 == 194
    ,"UNKNOWN:c3"  // c3 == 195
    ,"UNKNOWN:c4"  // c4 == 196
    ,"UNKNOWN:c5"  // c5 == 197
    ,"UNKNOWN:c6"  // c6 == 198
    ,"UNKNOWN:c7"  // c7 == 199
    ,"UNKNOWN:c8"  // c8 == 200
    ,"UNKNOWN:c9"  // c9 == 201
    ,"UNKNOWN:ca"  // ca == 202
    ,"UNKNOWN:cb"  // cb == 203
    ,"UNKNOWN:cc"  // cc == 204
    ,"UNKNOWN:cd"  // cd == 205
    ,"UNKNOWN:ce"  // ce == 206
    ,"UNKNOWN:cf"  // cf == 207
    ,"UNKNOWN:d0"  // d0 == 208
    ,"UNKNOWN:d1"  // d1 == 209
    ,"UNKNOWN:d2"  // d2 == 210
    ,"UNKNOWN:d3"  // d3 == 211
    ,"UNKNOWN:d4"  // d4 == 212
    ,"UNKNOWN:d5"  // d5 == 213
    ,"UNKNOWN:d6"  // d6 == 214
    ,"UNKNOWN:d7"  // d7 == 215
    ,"UNKNOWN:d8"  // d8 == 216
    ,"UNKNOWN:d9"  // d9 == 217
    ,"UNKNOWN:da"  // da == 218
    ,"UNKNOWN:db"  // db == 219
    ,"UNKNOWN:dc"  // dc == 220
    ,"UNKNOWN:dd"  // dd == 221
    ,"UNKNOWN:de"  // de == 222
    ,"UNKNOWN:df"  // df == 223
    ,"LeftCtrl"    // e0 == 224
    ,"LeftShift"   // e1 == 225
    ,"LeftAlt"     // e2 == 226
    ,"Super_L"     // e3 == 227
    ,"RightCtrl"   // e4 == 228
    ,"RightShift"  // e5 == 229
    ,"RightAlt"    // e6 == 230
    ,"Super_R"     // e7 == 231
    ,"UNKNOWN:e8"  // e8 == 232
    ,"UNKNOWN:e9"  // e9 == 233
    ,"UNKNOWN:ea"  // ea == 234
    ,"UNKNOWN:eb"  // eb == 235
    ,"UNKNOWN:ec"  // ec == 236
    ,"UNKNOWN:ed"  // ed == 237
    ,"UNKNOWN:ee"  // ee == 238
    ,"UNKNOWN:ef"  // ef == 239
    ,"UNKNOWN:f0"  // f0 == 240
    ,"UNKNOWN:f1"  // f1 == 241
    ,"UNKNOWN:f2"  // f2 == 242
    ,"UNKNOWN:f3"  // f3 == 243
    ,"UNKNOWN:f4"  // f4 == 244
    ,"UNKNOWN:f5"  // f5 == 245
    ,"UNKNOWN:f6"  // f6 == 246
    ,"UNKNOWN:f7"  // f7 == 247
    ,"UNKNOWN:f8"  // f8 == 248
    ,"UNKNOWN:f9"  // f9 == 249
    ,"UNKNOWN:fa"  // fa == 250
    ,"UNKNOWN:fb"  // fb == 251
    ,"UNKNOWN:fc"  // fc == 252
    ,"UNKNOWN:fd"  // fd == 253
    ,"UNKNOWN:fe"  // fe == 254
    ,"UNKNOWN:ff"  // ff == 255
};

const char *s_colour[] = {
     "black"
    ,"red"
    ,"green"
    ,"yellow"
    ,"blue"
    ,"magenta"
    ,"cyan"
    ,"white"
    ,"INVALID"
};

const int report_rate[] = {
     1000
    , 125
    , 250
    , 500
};
const int n_report_rates = 4;

static int dpi_point(int dpip) {
    return dpip * 250;
}

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
       %s --listkeys\n\
       %s [-s|--select <mode>] [-p|--print <mode>]\n\
\n\
-h|--h[elp]             - %s\n\
-V|--v[ersion]          - %s %s %s\n\
--li[stkeys]            - %s\n\
-s|--s[elect]           - %s\n\
-p|--p[rint]            - %s\n\
\n\
<mode>                  - %s\n\
\n\
%s: %s -p f3 --selec F3\n\
",
     _(APP_SUMMARY)

    ,_("Usage"),    BIN_NAME /* -h|--h[elp] */
    ,               BIN_NAME /* -V|--v[ersion] */
    ,               BIN_NAME /* --listkeys */
    ,               BIN_NAME /* -s|--s[elect] ... */

    ,_("Displays this help")
    ,_("Displays"), APP_NAME, _("version")
    ,_("Lists all possible modifiers, buttons and keys for assignment")
    ,_("Switches to <mode>")
    ,_("Prints out <mode>'s button configuration")
    ,_("A valid mode:          F3, F4 or F5")
    ,_("Example"),  BIN_NAME
    );
}

static void keylist_print(void) {
    unsigned char bt;

    printf("\nMODIFIERS:\n");

    // 0xe0 - 0xe7 match modifiers 0x01, 0x02, 0x04 ... 0x80
    for (bt = 0xe0; bt <= 0xe7; ++bt) {
        printf("  %s\n", s_keys[bt]);
    }

    printf("\nBUTTONS/SPECIALS:\n");

    for (bt = 0x00; bt <= 0x0f; ++bt) {
        printf("  %s\n", s_buttons[bt]);
    }

    printf("\nKEYS:\n");

    for (bt = 1; bt < 0xff; ++bt) {
        if (strncmp(s_keys[bt], "UNKNOWN", 7) == 0) continue;

        if (s_keys[bt][0] == 'A') {
            printf("  A ... Z\n");
            bt += 25;
            continue;
        }

        if (s_keys[bt][0] == '1') {
            printf("  0 ... 9\n");
            bt +=  9;
            continue;
        }

        if (strncmp(s_keys[bt], "F1", 2) == 0) {
            printf("  F1 ... F12\n");
            bt += 11;
            continue;
        }

        if (strncmp(s_keys[bt], "Num1", 4) == 0) {
            printf("  Num0 ... Num9\n");
            bt +=  9;
            continue;
        }

        printf("  %s\n", s_keys[bt]);
    }
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

// Expected length: 35
static int mode_load(unsigned char *mode_data, libusb_device_handle *usb_dev_handle, t_mode mode) {
    const uint16_t exp_len = 35;
    uint16_t mi;
    int ret;

    if (!mode_data || !usb_dev_handle || mode >= mode_COUNT) return 0;

    if      (mode == mode_f3) mi = 0xf3;
    else if (mode == mode_f4) mi = 0xf4;
    else if (mode == mode_f5) mi = 0xf5;
    else return 0;

    ret = libusb_control_transfer(
         usb_dev_handle
        ,LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE|LIBUSB_ENDPOINT_IN
        ,HID_REQ_GET_REPORT
        ,0x0300|mi
        ,0x0001
        ,mode_data
        ,exp_len
        ,1000
    );
    usleep(10000);

    if (ret != exp_len) {
        elog("ERROR: Failed to retrieve current mapping for mode 0x%.2x\n", mi);
        return 0;
    }

    int bit;
    char bitout[255]
         ,*po = &bitout[0];
    for (bit = 0; bit < exp_len; ++bit) {
        sprintf(po, "%.2x", (mode_data)[bit]);
        if ((bit+1) % 4 == 0) sprintf(po, " ");
        po += strlen(po);
    }
    dlog(LOG_PARSE, "Mode 0x%.2x: %s\n", mi, bitout);

    return exp_len;
}

static int mode_print(unsigned char *mode_data, int len, t_mode mode) {
    unsigned char bit = 0;
    unsigned char but[3] = {0,0,0};
    int i = 0;
    int x = 0;

    char rawout[255]
         ,*po = &rawout[0];
    for (i = 0; i < len; ++i) {
        sprintf(po, "%.2x", (mode_data)[i]);
        if ((i+1) % 4 == 0) sprintf(po, " ");
        po += strlen(po);
    }
    dlog(LOG_PARSE, "RAW: %s\n", rawout);

    i = 0;

    // F5040302 84060844 01000002 00000300 00040000 05000006 00000700 00080000 090000
    // ^^
    //printf("MODE: %s\n", s_mode[mode]);
    ++i;

    // F5040302 84060844 01000002 00000300 00040000 05000006 00000700 00080000 090000
    //   ^^
    printf("  Colour:              %s\n", s_colour[ (mode_data)[i++] ]);

    // F5040302 84060844 01000002 00000300 00040000 05000006 00000700 00080000 090000
    //     ^^
    bit = (mode_data)[i++];
    printf("  Report Rate:         %4d\n",
        bit < n_report_rates
        ? report_rate[bit]
        : -1
    );

    // F5040302 84060844 01000002 00000300 00040000 05000006 00000700 00080000 090000
    //       ^^ ^^^^^^

    for (x = 1; x <= 4; ++x) {
        bit = (mode_data)[i++];
        printf("  DPI #%d:        %s %4d\n",
             x
            ,bit & 0x80 ? "(DEF)" : "     "
            ,dpi_point(bit & 0x0f)
            );
    }

    // F5040302 84060844 01000002 00000300 00040000 05000006 00000700 00080000 090000
    //                ^^

    bit = (mode_data)[i++];
    printf("  DPI Shift:           ");
    if (bit == 0x40) {
        printf("NOT SET");
    } else {
        printf("%d", dpi_point(bit & 0x0f));
    }
    printf("\n");

    // F5040302 84060844 01000002 00000300 00040000 05000006 00000700 00080000 090000
    //                   ^^

    for (x = 1; x <= 9; ++x) {
        but[0] = (mode_data)[i++];
        but[1] = (mode_data)[i++];
        but[2] = (mode_data)[i++];

        printf("  %s",
            x == 1 ? "Left Click (But1):   " :
            x == 2 ? "Right Click (But2):  " :
            x == 3 ? "Middle Click (But3): " :
            "G");
        if (x > 3) printf("%d:                  ", x);

        // Modifiers (but[1])
        // 0xe0 - 0xe7 match modifiers 0x01, 0x02, 0x04 ... 0x80
        {
            unsigned char ky = 0xe0;
            int m = 0x00;
            for (m = 0x01; m <= 0x80; m *= 2) {
                if (but[1] & m) printf("%s + ", s_keys[ky]);
                ++ky;
            }
        }

        // Buttons   (but[0])
        if (but[0] & 0x0f) {
            printf("%s", s_buttons[but[0] & 0x0f]);

            if (but[2] > 0) printf(" + ");
        }

        // Keys      (but[2])
        if (but[2] > 0) printf("%s", s_keys[but[2]]);
        printf("\n");
    }

    return 1;
}

int main (int argc, char *argv[]) {
    t_exit ret = exit_none;
    int c;

    log_init();

    help_version();

    // Initialise USB
    usb_init();

    // Initialise mouse
    // ID 046d:c246 == Logitech, Inc. Gaming Mouse G300
    if (!mouse_init(LOGITECH_G300S_VENDOR_ID, LOGITECH_G300S_PRODUCT_ID, "Logitech G300s")) {
        // De-initialise USB
        usb_deinit();

        return exit_usberr;
    }

    display_mouse_hid(LOGITECH_G300S_VENDOR_ID, LOGITECH_G300S_PRODUCT_ID);

    // FIXME: Get interface index somehow
    usb_interface_index = 1;
    if (usb_interface_index < 0) {
        // De-initialise USB
        usb_deinit();

        return exit_usberr;
    }

    printf("Detaching kernel driver...\n");
    if (mouse_hid_detach_kernel(usb_interface_index) != 0) {
        // De-initialise mouse
        mouse_deinit();

        // De-initialise USB
        usb_deinit();

        return exit_usberr;
    }

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
            {"listkeys",    0, 0,   0}, /* DON'T REORDER THIS, MUST BE 3rd */

            {"select",      1, 0, 's'},
            {"print",       1, 0, 'p'},

            {0,0,0,0}
        };

        c = getopt_long(argc, argv, "hVs:p:",
                long_options, &option_index);

        if (c == -1)
            break;

        switch (c) {
            // Help
            case 'h':
                help_usage();
                exit(ret);

            break;

            // Version
            case 'V':
                // Already printed it

                exit(ret);

            break;

            // Select Mode
            case 's':
            {
                t_mode mnew = mode_COUNT;

                if (!optarg) {
                    elog("ERROR: Mode required for select option\n");
                    ret = exit_param;
                    continue;
                }

                for (mnew = 0; mnew < mode_COUNT; ++mnew) {
                    if (strcasecmp(optarg, s_mode[mnew]) == 0) break;
                }

                if (mnew == mode_COUNT) {
                    elog("ERROR: Invalid mode for select option: %s\n", optarg);
                    ret = exit_modesel;
                    continue;
                }

                printf("Mode Selection Specified: %s\n", s_mode[mnew]);

                printf("Selecting Mode: %s\n", s_mode[mnew]);
                if (change_mode(usb_dev_handle, mnew) == mode_COUNT) ret = exit_modesel;
            }
            break;

            // Print mode
            case 'p':
            {
                unsigned char mode_data_p[255];
                int len = 0;
                t_mode mnew = mode_COUNT;

                if (!optarg) {
                    elog("ERROR: Mode required for print option\n");
                    continue;
                }

                for (mnew = 0; mnew < mode_COUNT; ++mnew) {
                    if (strcasecmp(optarg, s_mode[mnew]) == 0) break;
                }

                if (mnew == mode_COUNT) {
                    elog("ERROR: Invalid mode for print option: %s\n", optarg);
                    continue;
                }

                printf("Printing Mode: %s\n", s_mode[mnew]);

                len = mode_load(&mode_data_p[0], usb_dev_handle, mnew);
                mode_print(&mode_data_p[0], len, mnew);
            }
            break;

            // Long options without val set (if val set, short option will be
            // provided instead).
            case 0: {
                if (option_index == 2 /* listkeys */) {
                    keylist_print();
                    exit(ret);
                }
            }

            break;

            // Option provided but missing it's required argument
            case '?':
                ret = exit_param;

            break;

            default:
                printf("?? getopt returned character code 0%o ??\n", c);
                ret = exit_param;
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
        ret = exit_param;
    }

    // Re-attach kernel driver
    printf("Attaching kernel driver...\n");
    mouse_hid_attach_kernel(usb_interface_index);

    // De-initialise mouse
    mouse_deinit();

    // De-initialise USB
    usb_deinit();

    log_end();

    return ret;
}
