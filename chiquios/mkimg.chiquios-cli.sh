
#!/bin/sh

profile_chiquios_cli() {
    profile_standard

    title="ChiquiOS CLI"
    desc="ChiquiOS minimal x86 with Bacus-Pkg"
    hostname="chiquios"

    image_name="chiquios-x32-cli"
    output_filename="chiquios-x32-cli-${RELEASE}-${ARCH}.iso"

    apks="$apks git curl ca-certificates nano"
    apkovl="/tmp/aports/scripts/genapkovl-chiquios.sh"
}
