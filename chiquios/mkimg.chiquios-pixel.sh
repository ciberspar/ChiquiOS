
profile_chiquios_pixel() {
    profile_standard

    hostname="chiquios"
    image_name="chiquios-pixel"
    output_filename="chiquios-pixel-${RELEASE}-${ARCH}.iso"
    output_format="iso"
    image_ext="iso"

    apks="$apks
        bash
        ca-certificates
        curl
        git
        dbus
        elogind
        font-dejavu
        mesa
        openbox
        xinit
        xterm
        lxpanel
        pcmanfm
        lxterminal
        lightdm
        lightdm-gtk-greeter
    "

    apkovl="genapkovl-chiquios.sh"
}
