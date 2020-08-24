```
     ##### /##                          #######  ###
  ######  / ##                        /       ### ###
 /#   /  /  ##                 #     /         ##  ##
/    /  /   ##                ##     ##        #   ##
    /  /    /                 ##      ###          ##
   ## ##   /       /###     ######## ## ###        ##      /###       /###
   ## ##  /       / ###  / ########   ### ###      ##     / ###  /   / ###  /
   ## ###/       /   ###/     ##        ### ###    ##    /   ###/   /   ###/
   ## ##  ###   ##    ##      ##          ### /##  ##   ##    ##   ##    ##
   ## ##    ##  ##    ##      ##            #/ /## ##   ##    ##   ##    ##
   #  ##    ##  ##    ##      ##             #/ ## ##   ##    ##   ##    ##
      /     ##  ##    ##      ##              # /  ##   ##    ##   ##    ##
  /##/      ### ##    /#      ##    /##        /   ##   ##    /#   ##    ##
 /  ####    ##   ####/ ##     ##   /  ########/    ### / ####/ ##  #######
/    ##     #     ###   ##     ## /     #####       ##/   ###   ## ######
#                                 |                                ##
 ##                                \)                              ##
                                                                   ##
                                                                    ##
```
# RatSlap #

## Linux configuration tool for Logitech mice *(currently only G300/G300S)* ##

[![Total
alerts](https://img.shields.io/lgtm/alerts/g/krayon/ratslap.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/krayon/ratslap/alerts/)
[![Language grade:
C/C++](https://img.shields.io/lgtm/grade/cpp/g/krayon/ratslap.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/krayon/ratslap/context:cpp)

----
## Introduction ##

*RatSlap* aims to provide a way to configure configurable Logitech mice from
within Linux. It is licensed under the GNU GPL version 2. For more information,
see [LICENSE](LICENSE) / [COPYING](COPYING) .

For a list of authors and contributors, see [AUTHORS](AUTHORS.md) /
[CONTRIBUTORS](CONTRIBUTORS.md) .

For more information on contributing (new feature, bug fix, pull request etc),
please see [CONTRIBUTING](CONTRIBUTING.md) .



----
## Availability ##

### { BACKWARDS COMPATIBILITY WARNING FOR PRE-0.4.0 { ###

The default branch has changed from `master` to `main`. If you are building from
source, and have an existing fork/clone, you may need to build this in a clean
directory and/or update your remote tracking branch. Sorry about that.

### } BACKWARDS COMPATIBILITY WARNING FOR PRE-0.4.0 } ###

### { BACKWARDS COMPATIBILITY WARNING FOR PRE-0.3.0 { ###

***The key that was called `Menu` in now called `Application` (bringing it in line
with the standard USB HID naming scheme. If you have any scripts calling this,
and using `Menu`, they will need to be updated.***

Additionally, there was a bug whereby unrecognised keys would be silently
ignored - this is no longer the case, they are now reported as an error.

### } BACKWARDS COMPATIBILITY WARNING FOR PRE-0.3.0 } ###

The most current release is v0.4.1 and is available from:

* [GitLab (RELEASE v0.4.1)](https://gitlab.com/krayon/ratslap/tags/0.4.1)
* [GitHub (RELEASE v0.4.1)](https://github.com/krayon/ratslap/releases/tag/0.4.1)

*RatSlap* binary and archive should be signed with my GPG key (
[231A 94F4 81EC F212](http://pgp.mit.edu/pks/lookup?op=get&search=0x231A94F481ECF212)
).

Binary signature files end in .asc and can be verified using
[gpg/gpg2](https://www.gnupg.org/)
thus:

```console
$ gpg --verify ratslap-0.2.2.x86_64.tar.gz.asc
    gpg: assuming signed data in `ratslap-0.2.2.x86_64.tar.gz'
    gpg: Signature made 2018-05-01T01:00:52 AEST using RSA key ID 81ECF212
    gpg: Good signature from "Krayon (Code Signing Key) <krayon.git@qdnx.org>"
    gpg: WARNING: This key is not certified with a trusted signature!
    gpg:          There is no indication that the signature belongs to the owner.
    Primary key fingerprint: CDEC 1051 0874 06FB 8323  46DC 231A 94F4 81EC F212
```

You may first need to retrieve my public key if you haven't already done so:

```console
$ gpg --recv-keys 81ECF212
    gpg: keyring `/home/krayon/.gnupg/secring.gpg' created
    gpg: requesting key 81ECF212 from hkp server keys.gnupg.net
    gpg: /home/krayon/.gnupg/trustdb.gpg: trustdb created
    gpg: key 81ECF212: public key "Krayon (Code Signing Key) <krayon.git@qdnx.org>" imported
    gpg: no ultimately trusted keys found
    gpg: Total number processed: 1
    gpg:               imported: 1  (RSA: 1)
```

*RatSlap* source is available on
[GitHub](https://github.com/krayon/ratslap)
, [GitLab](https://gitlab.com/krayon/ratslap)
and [BitBucket](https://bitbucket.org/krayonx/ratslap)
with the primary (public) repository being
[GitHub](https://github.com/krayon/ratslap)
.



----
## What's New? ##

The main changes in the more recent versions is the DPI Support
( [Daniel Dawson](https://gitlab.com/danieldawson) ) and the new keys:
`NonUS#`, `CapsLock`, `NonUS\`, `Application`, `Power`, `Num=`, `F13`, `F14`,
`F15`, `F16`, `F17`, `F18`, `F19`, `F20`, `F21`, `F22`, `F23`, `F24`, `Execute`,
`Help`, `Menu`, `Select`, `Stop`, `Again`, `Undo`, `Cut`, `Copy`, `Paste`,
`Find`, `Mute`, `VolumeUp`, `VolumeDown`,
`Num,` (Brazillian keypad period (.)?),
`AS400Num=` (Keypad Equal Sign on AS/400 keyboards),
`SysReq` (SysReq/Attention), `Cancel`, `Clear`, `Prior`, `Return`, `Separator`,
`Out`, `Oper`, `ClearAgain`, `CrSelProps`, `ExSel`, `Num00`, `Num000`,
`Sep1000s` (Thousands separator - locale specific?),
`SepDec` (Decimal separator - locale specific?),
`CurrUnit` (Currency Unit - locale specific?),
`CurrSubUnit` (Currency Sub-Unit - locale specific?),
`Num(`, `Num)`, `Num{`, `Num}`, `NumTab`, `NumBackspace`, `NumA`, `NumB`,
`NumC`, `NumD`, `NumE`, `NumF`, `NumXOR`, `Num^`, `Num%`, `Num<`, `Num>`,
`Num&`, `Num&&`, `Num|`, `Num||`, `Num:`, `Num#`, `NumSpace`, `Num@`, `Num!`,
`NumMemStore`, `NumMemRecall`, `NumMemClear`, `NumMemAdd`, `NumMemSub`,
`NumMemMul`, `NumMemDiv`, `NumPlusMinus`, `NumClear`, `NumClearEntry`,
`NumBinary`, `NumOctal`, `NumDecimal`, `NumHex`.

Now a lot of the above keys haven't been tested and in some cases I have NO IDEA
what they are :P They come from the [USB HID keyboard codes](#key-names) though.

For other features and bug fixes see [Version History](#version-history) below.



----
## Bug Tracker ##

Bugs are tracked on the
[Quadronyx Bug Tracker](https://bugs.qdnx.org/project/5)
.



----
## Usage ##

NOTE: The *Logitech G300* and *G300s* differ only by name and physical
appearance, otherwise they are functionally (and USB VID:PID) identical.

The *Logitech G300/G300s* has 3 button modes, affectionately known as F3, F4
and F5. For each of these modes, you can assign an LED colour, report rate
(speed at which the mouse communicates with the computer), DPI setting(s) and
button/key combinations.

NOTE: You cannot remap the scrollwheel. These generate button 4 (up) and 5
(down) and cannot be changed.

The default mapping for F3 on the G300/G300s, for example, is currently:

```console
$ ratslap -pf3
Printing Mode: F3
  Colour:              cyan
  Report Rate:          500
  DPI #1:               500
  DPI #2:        (DEF) 1000
  DPI #3:              1500
  DPI #4:              2500
  DPI Shift:           NOT SET
  Left Click (But1):   Button1
  Right Click (But2):  Button2
  Middle Click (But3): Button3
  G4:                  Button6
  G5:                  Button7
  G6:                  LeftCtrl +
  G7:                  LeftAlt +
  G8:                  ModeSwitch
  G9:                  DPICycle
```

### Remapping to just mouse buttons ###

One usage for *RatSlap* may be to remap the buttons to generate standard mouse
buttons higher than the 1-3 + scrollwheel (4 and 5).

Above, we see a typical button 1/2/3 configuration for left, right and middle
respectively. We can also see the G4 button generates mouse button 6 and G5
generates mouse button 7. Unfortunately, G6-G9 do not generate the desired extra

We can rectify this easily, using *RatSlap*:

```console
$ ratslap --modify F3 --colour Magenta --G6 Button8 --G7 Button9 --G8 Button10 --G9 Button11
Modifying Mode: F3
    Setting colour: magenta
    Setting button 6: Button8
    Setting button 7: Button9
    Setting button 8: Button10
    Setting button 9: Button11
Saving Mode: F3
```

Now, we have all mouse buttons functioning as mouse buttons! Go figure! And as
an added bonus, we have a nice Magenta glow.

```console
$ ./ratslap --print F3
Printing Mode: F3
  Colour:              magenta
  Report Rate:          500
  DPI #1:               500
  DPI #2:        (DEF) 1000
  DPI #3:              1500
  DPI #4:              2500
  DPI Shift:           NOT SET
  Left Click (But1):   Button1
  Right Click (But2):  Button2
  Middle Click (But3): Button3
  G4:                  Button6
  G5:                  Button7
  G6:                  Button8
  G7:                  Button9
  G8:                  Button10
  G9:                  Button11
```

Finally, we can select the F3 mode (if we're not already using it):

```console
$ ./ratslap --select F3
Selecting Mode: F3
```

### Firefox tab switching buttons ###

Another option might be using the buttons behind the wheel (G8 and G9) to switch
forward and backward (respectively) between tabs in your favourite browser ...
or Firefox.

So, let's set the buttons to `Control-TAB` and `Control-Shift-TAB`, print out
our config, then switch to that mode:

```console
$ ratslap --modify F3 --G8 LeftCtrl+TAB --G9 LeftCtrl+LeftShift+TAB --print F3 --select F3
Modifying Mode: F3
    Setting button 8: LeftCtrl+TAB
    Setting button 9: LeftCtrl+LeftShift+TAB
Saving Mode: F3

Printing Mode: F3
  Colour:              magenta
  Report Rate:          250
  DPI #1:               500
  DPI #2:        (DEF) 1000
  DPI #3:              1500
  DPI #4:              2500
  DPI Shift:           NOT SET
  Left Click (But1):   Button1
  Right Click (But2):  Button2
  Middle Click (But3): Button3
  G4:                  Button6
  G5:                  Button7
  G6:                  Button8
  G7:                  Button9
  G8:                  LeftCtrl + Tab
  G9:                  LeftCtrl + LeftShift + Tab

Selecting Mode: F3
```

### ERROR: libusbx: error [_get_usbfs_fd] libusbx... ###

When you try to run *RatSlap*, you may receive an error similar to the
following:
```console
libusbx: error [_get_usbfs_fd] libusbx couldn't open USB device /dev/bus/usb/002/090: Permission denied
libusbx: error [_get_usbfs_fd] libusbx requires write access to USB device nodes.
20161115T002046+1100 [E]           main.c:00581:mouse_init      Failed to find Logitech G300s (046d:c246)
```

This is caused, as the error suggests, by the fact that you do not have write
access to the mouse (/dev/bus/usb/002/090 in the example above).

There are two possible ways around this issue:
  1. (NOT RECOMMENDED) Run ratslap as root;
  1. Grant permissions to the device for your user account.

The later is the preferred option here. To do this, simply ensure you have write
permissions to the device. One such way is to make it group-writable, change the
group ownership and place yourself in the newly owning group, eg:

```console
$ ls -lah /dev/bus/usb/002/090
crw-r--r-- 1 root root 189, 217 2016-11-14 19:22 /dev/bus/usb/002/090

$ groups
adm dialout cdrom sudo audio dip video plugdev netdev fuse lpadmin scanner
sambashare vboxsf vboxusers nonet

$ sudo chgrp adm /dev/bus/usb/002/090
[sudo] password for krayon:

$ sudo chmod g+w /dev/bus/usb/002/090
```



----
## History ##

I recently (October 2016) purchased a Logitech G300S due mainly to the many
extra buttons. I didn't care that the software didn't support Linux as I only
wanted the extra buttons to generate extra mouse buttons, not keys or anything
fancy.

Unfortunately, when I plugged it in, I discovered that none of the default 3
modes generated the extra mouse buttons I desired, instead generating useless
and annoying keypress events such as *CTRL-C*, *CTRL-X* and *CTRL-V*.

Worse still, I also discovered that the Windows software only allows you to
assign mouse buttons 1 through 7, skipping 4 and 5 (scroll-wheel). These are
*Left-Click* (1), *Middle-Click* (2), *Right-Click* (3),
*Browser Back* (6) and *Browser Forward* (7) respectively! Therefore,
Logitech provides no way of generating higher mouse button events, button 8 for
example. ~~Hopefully this is a limitation in the Logitech software and not in
the mouse itself~~ (see [Notes](#notes)).



----
## Features ##

* Configure the mouse options, same as the Logitech Windows only software.
* Bind all buttons, except the scroll wheel up and down, a total of 9 (Logitech
tool only allows 5).
* Assign mouse clicks or keys to the Left and Right buttons (which the Logitech
tool cannot do).
* Freely assign 'Mode Switch' to a mouse button in only one mode/profile.
* Assign mouse clicks with key modifiers (which Logitech doesn't offer).



----
## Version History ##

* v0.1.0
  * First version, ability to select mode
* v0.1.1
  * Hotfix for GCC 5 compatibility
  * Print current mode's mapping ( accidental check-in, oops :S )
* v0.2.0
  * [QB#104 - Modify mode - change refresh rate](https://bugs.qdnx.org/bug/104)
  * [QB#105 - Modify mode - change mode LED colour](https://bugs.qdnx.org/bug/105)
  * [QB#106 - Modify mode - change button assignment](https://bugs.qdnx.org/bug/106)
  * [QB#108 - kernel driver disconnect after --help/--version](https://bugs.qdnx.org/bug/108)
  * [QB#109 - kernel driver disconnect after --listkeys](https://bugs.qdnx.org/bug/109)
* v0.2.1
  * [QB#111 - missing libusb_strerror function](https://bugs.qdnx.org/bug/111)
  * [QB#117 - Compile fails when ctags missing](https://bugs.qdnx.org/bug/117)
  * Prettier Changelog
  * Added man page
* v0.2.2
  * [QB#121 - set_debug is deprecated, use libusb_set_option instead](https://bugs.qdnx.org/bug/121)
  * Added some documentation: [AUTHORS](AUTHORS.md), [CONTRIBUTING](CONTRIBUTING.md)
* v0.2.3
  * [QB#122 - Help shouldn't require USB permissions](https://bugs.qdnx.org/bug/122)
  * [QB#123 - Check mode_load() return code](https://bugs.qdnx.org/bug/123)
  * [QB#124 - Failed operations don't abort execution](https://bugs.qdnx.org/bug/124)
* v0.3.0 / v0.3.1 / v0.3.2
  * [QB#113 - Configure DPI #[1-4], DPI Default and * DPI Shift values](https://bugs.qdnx.org/bug/113) -
    Thanks [Daniel Dawson](https://gitlab.com/danieldawson) ( and
    [sanikk](https://github.com/sanikk) )
  * [QB#125 - Error assigning Num+](https://bugs.qdnx.org/bug/125)
  * [QB#126 - Doco should generate HTML versions](https://bugs.qdnx.org/task/126)
  * [QB#128 - Unrecognised keys silently ignored ](https://bugs.qdnx.org/task/128)
  * [QB#129 - Add missing keys](https://bugs.qdnx.org/bug/129)
* v0.4.0
  * Documentation fixes
  * Fixed recommendations by LGTM ( GitHub #22 )
* v0.4.1
  * Add automatic code scanning (CodeQL)
  * Renamed default branch: `master` to `main`
  * `dpishift` fix ( fixes GitHub #23 )



----
## TODO ##

* FEATURE: [QB#114 - Factory reset option](https://bugs.qdnx.org/bug/114)
* FEATURE: [QB#115 - Ability to save/load profiles](https://bugs.qdnx.org/bug/115)
* FEATURE: [QB#127 - musl build for lean standalone](https://bugs.qdnx.org/bug/127)



----
## Notes ##

### Key names ###

It seems the key names (and codes) are standard USB HID ones, who knew?! :D

More info at: 

  * https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf

### Technique to sniff USB traffic ###

https://julien.danjou.info/blog/2012/logitech-k750-linux-support shows a
technique for sniffing USB traffic using the *usbmon* kernel module.

Unfortunately it looks like I'll need to create a Windows VM to run the Logitech
software on :(

### Logitech Software's Artificial Restrictions ###

After considerable testing, I have found some operations the hardware supports
that Logitech's management software does not.

Firstly, I can confirm that we can actually set 9 mouse buttons (1, 2, 3, 6, 7,
8, 9, 10 and 11). This is good news as it was what I originally expected from
the Logitech G300s in the first place :D

We can also re-assign left and right buttons to different keys/buttons.
Logitech's software did not allow this, presumably to avoid accidentally
removing your ability to actually "left-click" anything :P

Once you assigned the 'Mode Switch' to a button in one profile/mode, it was also
set in the other profiles/modes. This is presumably so you can always change to
another mode. We shall impose no such limitation.

Lastly, and perhaps rather attractively, we can assign mouse buttons to perform
mouse clicks WITH modifiers! You can therefore bind Ctrl+Shift+Right-Click to
the left mouse button if you so desire.



----
[//]: # ( vim: set ts=4 sw=4 et cindent tw=80 ai si syn=markdown ft=markdown: )
