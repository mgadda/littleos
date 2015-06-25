# littleos
A small OS. 

### What does it do?

Not much just yet. The kernel is loaded via GRUB legacy, identity maps the first 3MB of memory and enables paging, configures software and hardware interrupts, sets up the keyboard for input, writes some data to COM1 serial port and logs what it's doing to the screen.

## Building

1. Install bochs (OS X): `brew install bochs`
2. [Install VirtualBox](https://www.virtualbox.org/wiki/Downloads)
2. [Install Vagrant](http://www.vagrantup.com/downloads)
3. `cd ~/path/to/repo`
4. `vagrant up` – *cross fingers here*
5. `vagrant ssh`
6. `cd /vagrant`
7. make

In another shell on the host machine, run: `make run`

While it's possible to run bochs from within Vagrant (some modifications to bochssrc.txt are required), it's a terrible idea. Bochs is already a pretty slow emulator, then compounded by running it inside Virtual Box. So it's better to do the compilation within vagrant (virtual box) to ensure a consistent development environment but then test/run the kernel on the host development machine. 

The takehome is that you have to remember to run `make` inside the Vagrant box before `make run` otherwise your build experience will be at the mercy of the host development machine.

