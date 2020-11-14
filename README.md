# Library to communicate with LED1248 flexible displays

I found a number of flexible LED matrix panels that I've been using in my clothing and
accessories. The panels communicate via bluetooth from an Android app but I wanted to be able
to sychronise multiple displays and control them locally without bluetooth.

I wrote this library so you can communicate using your own code with these displays. I've tested it with both a RGB display (32 pixels by 16 pixels but can only show 7 colours) and the mini single colour display (48 by 12 pixels, sometimes these displays have blocks of different LEDs so they look multicolour even though they are not addressable).

There are a number of sellers of these displays on various marketplaces, and they come and go quite often. You can usually figure out if it will work with this library by looking at the app they ask you to use, if it's LED1248 it should work. The boards are relatively inexpensive (US$15-US$30) and are sold bare and as part of clothing, hats, and even dog collars.  A good way to get one if you are in the USA/Canada is from [Lumen Couture](https://www.wearabletutorials.com/buy-ultra-thin-led-matrix-panels/)

The displays come with a little dongle that provides power (3v from a lipo) and communication to the flexible panel. The dongle doesn't do much processing, it just provides the power on and off commands and passes through any other commands recieved by its bluetooth module. This does mean that via the serial interface you don't have full addressable control of the LEDs but instead can do the same sorts of things as the app does.

Unplug the dongle (sometimes they have connectors, sometimes not) and connect the power (3v), ground, and tx pins to an arduino (I used a Trinket M0 but anything should do as long as it runs at 3v, has hardware serial pins and enough RAM for any animations or bitmaps you want).

Note: This library doesn't use the rx pin and relies on timing for knowing when to send the next command. Firstly this saves connections, useful especially on wearables, but also the rx isn't actually UART serial data so we'd have to parse it.

This library should also work communicating over bluetooth to the boards, untested and you may want to handle the return acknowledgements. It uses the Adafruit GFX canvas library so you can draw text and other objects onto a canvas then push it to the board to display.

The included example displays a demo reel: a 5 frame animation with some text, some scrolling text, and then a random bargraph display.