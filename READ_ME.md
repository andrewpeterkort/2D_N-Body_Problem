Andrew Peterkort
Oregon State University
Computer Arctecture

Disclaimer: there is only 2 of the four implementations complete: pthreads and regular.

Controls: Press ESC to leave program


*******************************IF YOU READ ANYTHING, READ THE FOLLOWING WARNING**************************************

Download ALLEGRO 5 TO COMPILE. MAKEFILE INCLUDES -lallegro_main FLAG WHICH IS ONLY NEEDED ON OSX..... I don't know if this will break compilation on other systems 

the screen renders a fixed 480 x 640 resolution that is "theoritically" suppose to scale to any display but notice the #define MACGLITCH!!!!!!!!!!!!!!!!
this is the most important detail becasue this is normally supose to be set to 1, but because allegro 5 mulitiplies the fetched native resolution by 2 for retina displays this value is set to 2! You will probably have to change this to 1 to get the entire 480x640 resting comfortably in your screen.

If unsure about screen positioning while trying to jimmy the code change "white.png" to "test.png" to print 480 x 640 test background

*******************************WARNING OVER**************************************

*************STD PROGRAM DESEIGN:

the standard program creates a basic linked list to keep track of all the particals on the screen. One header struct with partical strucks, singly linked. The basic functions will loop through the main linked list structure and do operations on all the particals sequentially. NOTE: you can't change the position untill you have calculated the impulse for all the particals for that frame, this is the only things that must be done in a pipline fashion... and the screen drawing, ovecoarse is the end othe pipline.

The program goes

SET UP ALLEGRO

SET UP PARTICAL LIST WITH RANDOM POSITION WITHIN THE SCREEN

LOOP------------------

----stage 1----

CHECK COLITION FOR EACH PARTICAL

----Pthreads start extra pipline stage here----

CALCULAT GRAVITATIONAL IMPULSE FOR EACH PARTICAL

ADD GRAVITATIONAL IMPULSE TO VELOCITY

----stage 2----

CHANGE POSITION OF EACH PARTICAL

----stage 3----

PRINT EACH PARTICAL TO 480 x 640 BITMAP.

SCALE BITMAP TO SCREEN RES AND PRINT

CHECK FOR ESCAPE PRESS

LOOP TOP--------------

in the 640/480 resolution each pixel is a spot a "partical" can occupy. Particals move like pieces on a grid, they are either in one space or another, NOT in between two spaces. This was done to increase simplicity of coalition detection and resolution! I did not want to do any overly complicated collision detection because I could not figure out a way to effectivly multithread collision without the overhead being rediculous.

each partical has a mass of 1 and this will increase as more particals collide into each other combining. The mass number works gravitationally and initailly... you know with inertia, but not spacialy!

Colition is very simple! it does not generate paths or lines to detect coalition when the particals start skipping pixels on each frame! Instead it just makes it so there is a lot of granuality of movement inside each pixel, defined by SCALLING_FACTOR! The helps some but particals still skip by each other when experience extreme accelerations as the got close to larger gravitational bodies. The final, and most funny, part of the collision algorithim is its "expanded detection" which just searches all the pixels around it's current pixel of occupany for a partical. If it finds one, it deletes it, adds its mass and momentum to its current values and moves on. Essentially the partical that gets axed is the one at the back of the partical list. Crude, I know.

GRAVITATION_CONSTANT is G in m1*m2*G / ( d^2 ); You know, physics. Number of particals and mass of particals can all by changed to test you system. Warning around 2000 particals, my sytem begins to want to kill me about there.

*************PTHREAD PROGRAM DESEIGN:

How is PTHREAD implemented? The collision, as said above is single threaded but the calculation of gravity and the changing of the particals position is multithreaded. Again, there is a #define that can be changed to change the number of threads. These new threads are "gravity" threads and their function is defined right before main. They essentialy loop through the list with a mutex lock on the particals, scanning to see if the partical has been processed yet. Once they find one that has not they mark it, save a pointer to it and add it to their personal partical list. From this point they do all the calculations on the partical, looping through the master list to read other particals for position and mass without mutex's! THe mutexs on the particals only protect the "marked" variable as position is read but never edited in this second stage of the pipline! Stage three of the pipline the pthreads now go through using their personal pointers and edit the positions of each partical, not again we don't have to lock because nobody is accessesing each others data. Then stage 4 ( or stage 3 for single threaded ) is still single threaded. Gravitational calculations are only done by gravitional threads as including the main thread in the calculation created a bottle neck that slowed framerate down to a crawl.

how are pthreads used in pipelines? I had to make a custome barrier to keep them waiting untill everyone is done. This was done because the pthread library for OSX does not come with barriers. Also the allegro 5 multithreading does not come with barriers but this never got implemented.

Problems that happened:

-tried to do missile tracking but failed misserably.
-found out that malloc gets a memorry error exit when combined with a specific Allegro command and pthreads!...... I don't even know how thats possible.

Results:

FPS on both programs are actually very similar..... to the point I did not think of trying to find out how similar. You would think that I just copy and pasted the code for each program, but you can check the source and the pthreads are indeed there. If you increase the threads to an absurd number you get some slowing.

