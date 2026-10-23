
profile_chiquios_cli() {
    profile_standard

    hostname="chiquios"
    image_name="chiquios-cli"
    output_filename="chiquios-cli-${RELEASE}-${ARCH}.iso"
    output_format="iso"
    image_ext="iso"

    apks="$apks
        bash
        ca-certificates
        curl
        git
        nano
        sudo
        util-linux
        vim
    "

    # Bacus-Pkg se incorpora al sistema mediante el overlay
    apkovl="genapkovl-chiquios.sh"
}
