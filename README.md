# Veta - Avant garde text input
Veta is a program that enables you to write, somewhat efficiently with only one hand. It utilizes a grid system. Where there are cells composing of several subcell. Imagine that you have a 3x3 matrix where all the cells contains another 3x3 cell. In this way you can reach 81 cells with only two keypresses. This is merely a demonstration of the concept not intended for serious use. 

## Use cases 
Places where one handed text input with this method is useful, would be for controlling something with the numpad of a remote controll. Another system which matematically is annoying would be entering text on a virtual keyboard, by moving a curser to the left or to the right. In this system worst case you would have to move the cursor - 50 steps or something. 

Another idea would be to train people to use a better version of this program and by doing so removing the need to move their right hand from the mouse every now and then. This is something I personally find really annoying and uncomfortable.

## Usage 
To use this program. Simply do the following on a ubuntu 16.04 machine. Then you can use num lock to toggle it's functionallity. 

sudo apt-get install libsdl1.2-dev libx11-dev libxtst-dev git

git clone "https://github.com/richard-jansson/veta"

cd veta

make 

./veta

## Command line switches:

--dump-symbols

Reads your current keyboard layout and prints it out. Use this dump to create a config file that you can edit with your favorite text editor.

--load-symbols

Use this if you want to use a specific keymap. 

--zoom

Select cell by zooming in on the deeper cell. My guess is that this makes it easier to understand the concept.

--highlight

Highlight the cells that the user has selected. This option I would say is less distracting once you've got the hang of the system which shouldn't take to long. Even if you only are moderately gifted.
