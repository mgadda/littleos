apt-get update
apt-get install -y build-essential gdb nasm genisoimage bochs bochs-sdl bochs-term git-all vim
ln -s /usr/share/bochs /usr/local/share/bochs

sudo hwclock -w # the hardware clock is wrong, mysteriously
echo "US/Pacific" > /etc/timezone
dpkg-reconfigure -f noninteractive tzdata


