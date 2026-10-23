
profile_chiquios_desktop() {
    profile_standard

    title="ChiquiOS Desktop"
    desc="ChiquiOS con escritorio ligero basado en Alpine Linux"

    profile_abbrev="chiquios"
    arch="x86"
    image_name="chiquios-desktop"
    output_format="iso"
    image_ext="iso"

    apks="$apks
        bash
        ca-certificates
        curl
        dbus
        git
        mesa
        openbox
        xinit
        xterm
        lxpanel
        pcmanfm
        lxterminal
        lightdm
        lightdm-gtk-greeter
        font-dejavu
    "

    apkovl="genapkovl-chiquios.sh"
}
