
#!/bin/sh

profile_chiquios_pixel() {
    profile_standard

    title="ChiquiOS Pixel Edition"
    desc="ChiquiOS x86 desktop inspired by Raspberry Pi PIXEL"
    hostname="chiquios"

    image_name="chiquios-x32-pixel"
    output_filename="chiquios-x32-pixel-${RELEASE}-${ARCH}.iso"

    apks="$apks
        git curl ca-certificates nano
        xorg-server xf86-video-vesa xf86-input-libinput
        openbox obconf
        lxterminal pcmanfm lxpanel lxsession
        lxappearance lxde-common
        dbus lightdm lightdm-gtk-greeter
        adwaita-icon-theme
    "

    apkovl="/tmp/aports/scripts/genapkovl-chiquios.sh"
}
