# ascii-rain

Comfy rain for your console written in C with Ncurses.

<p align="center">
  <img src="https://25midi.com/f/asciirain2.gif" width="100%"/>
</p>


## Dependencies 

You'll need a ncurses library. For Ubuntu packages are: 'ncurses-dev' or 'libncurses-dev'.
For OSX try `$ brew install ncurses`.

## Installation

### Manual

First, download this repo:
- `$ git clone https://github.com/nkleemann/ascii-rain.git` 
- `$ cd ascii-rain`

To compile and link 'rain':

- `$ gcc rain.c -o rain -lncurses`

Now you can run 'rain' in your current working directory by just executing: ` ./rain`.

I you want to be able to run this program from every directory you have to copy the executable to `/usr/local/bin` or `/usr/bin`:
- `$ cp rain /usr/local/bin/rain`

### AUR

You can get it from the AUR right [here](https://aur.archlinux.org/packages/ascii-rain-git/).

## Notes & Troubleshooting

You'll (Or better, ncurses) need/s a somewhat modern terminal emulator with color support and the ability to hide the
cursor. Working examples are:

- iTerm 2 (OSX)
- OSX Terminal
- xfce4-terminal
- terminator

