
profile_chiquios_cli() {
    profile_standard

    title="ChiquiOS CLI"
    desc="ChiquiOS minimalista basado en Alpine Linux"

    profile_abbrev="chiquios"
    arch="x86"
    image_name="chiquios-cli"
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
    "

    apkovl="genapkovl-chiquios.sh"
}
