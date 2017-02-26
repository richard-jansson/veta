# Veta - Avant garde text input
Veta is a program that enables you to write, somewhat efficiently with only one hand. It utilizes a grid system. Where there are cells composing of several subcell. Imagine that you have a 3x3 matrix where all the cells contains another 3x3 cell. In this way you can reach 81 cells with only two keypresses. This is merely a demonstration of the concept not intended for serious use. 

[![A youtube video says more than a thousand words.](http://eruditenow.com/img/1.jpg)](https://youtu.be/8kuppby-KJ0?t=170)  


[A youtube video says more than a thousand words.](https://youtu.be/8kuppby-KJ0?t=170)

[Webpage](http://eruditenow.com/)

## Use cases 
Places where one handed text input with this method is useful, would be for controlling something with the numpad of a remote controll. Another system which matematically is annoying would be entering text on a virtual keyboard, by moving a curser to the left or to the right. In this system worst case you would have to move the cursor - 50 steps or something. 

Another idea would be to train people to use a better version of this program and by doing so removing the need to move their right hand from the mouse every now and then. This is something I personally find really annoying and uncomfortable. What we end up with then is making office worker work in a mode more reminiscent of how FPS gamers use their computers.

## Usage 
Once you've started the program you can enable or disable the grabbing of the keys with num lock. This is acctually a bug but I found it to becomme a useful feature. 

Then go on by pressing one of the following keys:

| *   | *   | *   | *   |
| --- | --- | --- | --- |
| Q   | W   | E   | R   | 
| A   | S   | D   | F   |
| Z   | X   | C   | Z   |

Each of these keys correspond to a cell onscreen. Press Q for the first cell and W for the second. If you want to cancel the selection and select a higher level press SHIFT.

To quit thep program press the § key.

The keybindings are currently hardcoded in the keybindings array in x11.c. With the help of the nifty program xev. You could easily figure out how to change the keycodes and change them. This might cause problems for all of you who not are using a Finish keyboard layout. Please contact me with any questions.


## Compilation A.K.A. How to run
To run the program you'll need a UNIX machine. The instructions below are intended for an Ubuntu 16.04 machine. If you are trying to use something else you might run into some problem. This is because I programmed the UI with to the metal X11 code which might make it run into problems if you are using another windowmanager. If you run into such problems please contact me. 

sudo apt-get install libx11-dev libxtst-dev git pkg-config libjansson-dev

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

# Configuration 
You can configure the program with the help of a json based configuration file. I went with json since I've lost the code which was the attempt of building a flex / bison parser from scratch. And since it gives me some flexability and is in general a well known format and so on. What I don't like with this option is that it reorders the file whenever new data is saved with the program, which is not very nice at all. Also it adds some unecessery extra stuff which is really neat. However since it saves me from hours of hours of creating a configuraiton file format which if I know myself would probably also be Turing-complete. If it sounds tempting get me in touch and you might convince me that I am wrong.  

Enough with the tech talk. What can you do with it. The configuration options that we have so far is: 

keybindings 
	cell0-cell11 - selected whatever keycode you want to bind selecting cell N with. Use xev to get the keycodes. (This is not supported on windows yet)
	reset - jump back to previous selection 
	quit - currently mapped to § on a Finnish keyboard so I guess this default ought to be changed. You can also look at the meybindings struct in x11.c if you want to change this. But it's available in conf.json
	position { x : int , y:  int) the position which the window will start at, not tested in windows yet. This is saved automagically when the program is closed  
	width - window width in pixels 
	height - window height in pixels		

Symbols: the symbols are saved as an array consisting of an object with a key for the description and the keycode to be sent 
	Expect all of this to be changed to support a wider range of features than just single keypresses. 
## Feedback 

If you are interested by this contact me via the form in the link below. 
[Contact](http://eruditenow.com/#contact)
