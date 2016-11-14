To use this program. Simply do the following on a ubuntu 16.04 machine. Then you can use num lock to toggle it's functionallity.

sudo apt-get install libsdl1.2-dev libx11-dev libxtst-dev git

git clone "https://github.com/richard-jansson/veta"

cd veta

make 

./veta

Command line switches:
--dump-symbols
Reads your current keyboard layout and prints it out. Use this dump to create a config file that you can edit with your favorite text editor.

--load-symbols
Use this if you want to use a specific keymap. 

--zoom
Select cell by zooming in on the deeper cell. My guess is that this makes it easier to understand the concept.

--highlight
Highlight the cells that the user has selected. This option I would say is less distracting once you've got the hang of the system which shouldn't take to long. Even if you only are moderately gifted.
