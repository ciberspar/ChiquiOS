
#!/bin/sh -e

HOSTNAME="${1:-chiquios}"
EDITION="${CHIQUIOS_EDITION:-cli}"
BACUS_SOURCE="${BACUS_SOURCE:-/bacuspkg}"

tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT

makefile() {
    owner="$1"
    perms="$2"
    filename="$3"

    mkdir -p "$(dirname "$filename")"
    cat > "$filename"
    chown "$owner" "$filename"
    chmod "$perms" "$filename"
}

mkdir -p "$tmp/etc" "$tmp/etc/apk"
mkdir -p "$tmp/usr/local/share"
mkdir -p "$tmp/usr/local/bin"
mkdir -p "$tmp/etc/profile.d"

printf '%s\n' "$HOSTNAME" > "$tmp/etc/hostname"

cp -R "$BACUS_SOURCE" "$tmp/usr/local/share/bacuspkg"

find "$tmp/usr/local/share/bacuspkg" \
    -type f -name '*.sh' -exec chmod 755 {} \;

makefile root:root 0755 "$tmp/usr/local/bin/bacusp" <<'EOF'
#!/bin/sh
exec /usr/local/share/bacuspkg/bin/bacusp "$@"
EOF

makefile root:root 0644 "$tmp/etc/profile.d/bacusp.sh" <<'EOF'
# Bacus-Pkg is available as the bacusp command.
EOF

cat > "$tmp/etc/apk/world" <<EOF
alpine-base
git
curl
ca-certificates
nano
EOF

if [ "$EDITION" = "pixel" ]; then
    cat >> "$tmp/etc/apk/world" <<'EOF'
xorg-server
xf86-video-vesa
xf86-input-libinput
openbox
obconf
lxterminal
pcmanfm
lxpanel
lxsession
lxappearance
lxde-common
dbus
lightdm
lightdm-gtk-greeter
adwaita-icon-theme
EOF
fi

mkdir -p "$tmp/etc/network"
cat > "$tmp/etc/network/interfaces" <<'EOF'
auto lo
iface lo inet loopback

auto eth0
iface eth0 inet dhcp
EOF

mkdir -p "$tmp/etc/runlevels/sysinit"
mkdir -p "$tmp/etc/runlevels/boot"
mkdir -p "$tmp/etc/runlevels/default"
mkdir -p "$tmp/etc/runlevels/shutdown"

for service in devfs dmesg mdev hwdrivers; do
    ln -sf "/etc/init.d/$service" "$tmp/etc/runlevels/sysinit/$service"
done

for service in hwclock modules sysctl hostname bootmisc syslog; do
    ln -sf "/etc/init.d/$service" "$tmp/etc/runlevels/boot/$service"
done

for service in mount-ro killprocs savecache; do
    ln -sf "/etc/init.d/$service" "$tmp/etc/runlevels/shutdown/$service"
done

if [ "$EDITION" = "pixel" ]; then
    ln -sf /etc/init.d/dbus "$tmp/etc/runlevels/default/dbus"
    ln -sf /etc/init.d/lightdm "$tmp/etc/runlevels/default/lightdm"
fi

tar -czf "$HOSTNAME.apkovl.tar.gz" -C "$tmp" .
