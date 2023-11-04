/*********************************************
  Andrew Peterkort
  final_std.c
  CS 472
  Kevin McGrath
 *********************************************/

#include <stdlib.h>
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

#define MACGLITCH 1//use 2 on macs with retna display, use 1 otherwise
#define PARTICAL_NUM 200
#define GRAVITATIONAL_CONSTANT 1000000000//9 zeros are regular, 10
#define SCALING_FACTOR 1000//3 zeros are regular, 4 for crazy time with 10 above
#define STARTING_MASS 1

struct partical{

   struct partical* next;// the next partical in the partical list
   ALLEGRO_BITMAP* image;// stores the bitmap for allegro to render
   uint32_t mass;

   int64_t xPosition;// 0.500 - 480.500 Decmal point is implied in designed format
   int64_t yPosition;// 0.500 - 640.500

   int64_t xVelocity;//the particals current x and y velocity
   int64_t yVelocity;

};



struct vector{//these vectors are used to store data for the hard math.

   double mag;//the magnitude of the acceleration crated by another partical
   double unitX;//the unit vector x component (basicly the angle of the force)
   double unitY;//Y component of above discriptions

};


struct turnHead{

   struct partical* next;//the head of the list of particals

};


struct vector* getVector( struct partical* you , struct partical* target ){

   struct vector* dir = malloc( sizeof( struct vector ) );

   double xDifference = (double)( you->xPosition - target->xPosition );//the two components of the distance vector between the two particals is calculated
   double yDifference = (double)( you->yPosition - target->yPosition );

   dir->mag = sqrt( ( xDifference * xDifference ) + ( yDifference * yDifference ) );//Pithagerins theorim is used to calculate the magnitude of the acceleration
   dir->unitX = xDifference / dir->mag;//calculting the component vector the angle
   dir->unitY = yDifference / dir->mag;

   return dir;

}


//ads up all the impulses generated from all the different particals
void impulseCalc( struct partical* you, struct turnHead* head ){

   int64_t xImpulse = 0;
   int64_t yImpulse = 0;
   struct vector* grav = NULL;


   struct partical* read;
   struct partical* readPrev;//cycles through the linked list of particals
   struct vector* vec = NULL;

   if( head->next == NULL ){

      return;//if its empty, get out 

   }

   read = head->next;

   while( read != NULL ){

      readPrev = read; 
      read = read->next;

      if( readPrev != you ){//if there is a value

	 grav = getVector( you, readPrev );//calculate the gravitational pull created by the partical
	 xImpulse += (int64_t)( grav->unitX * ( ( GRAVITATIONAL_CONSTANT * you->mass * readPrev->mass ) / pow( grav->mag, 2 ) ) );//find the impulse in each direction by using the scalled unit vector component
	 yImpulse += (int64_t)( grav->unitY * ( ( GRAVITATIONAL_CONSTANT * you->mass * readPrev->mass ) / pow( grav->mag, 2) ) );//and the formula for gravitational force

      }

   } 

   //printf( "\nX IMPULSE: %lld\n", xImpulse );
   //printf( "\nY IMPULSE: %lld\n", yImpulse );

   you->xVelocity -= ( xImpulse / you->mass );//subtract the impulse from the velocity
   you->yVelocity -= ( yImpulse / you->mass );// impulse is quickly scalled by mass

}



//just cycles through the partical list, calculating impulse for each partical
void impulseParticalList( struct turnHead* head ){

   struct partical* read;
   struct partical* readPrev;

   if( head->next == NULL ){

      return;

   }

   read = head->next;

   while( read != NULL ){

      readPrev = read; 
      read = read->next;
      impulseCalc( readPrev, head );

   } 

}


//inits the partical structs
void initPartical( struct partical* new ){

   new->next = NULL;
   new->image = al_create_bitmap( 1, 1 ); 
   al_set_target_bitmap( new->image );
   al_clear_to_color( al_map_rgb( 0, 0, 0) );
   new->xPosition = ( rand() % ( 640 * SCALING_FACTOR ) );
   new->yPosition = ( rand() % ( 480 * SCALING_FACTOR ) );//the scaling factor is simply the number of positions in each pixel
   new->xVelocity = 0;
   new->yVelocity = 0;
   new->mass = STARTING_MASS;

}

//inits the head of the partical list
void initTurnHead( struct turnHead* head ){

   head->next = NULL;

}

// adds a partical to the paprtical list. The new partical is simply added to the end of the list
void addPartical( struct turnHead* head, struct partical* nextPartical ){

   struct partical* read;
   struct partical* readPrev;

   if( head->next == NULL ){

      head->next = nextPartical;
      return; 

   }

   read = head->next;

   while( read != NULL ){//loop through untill we get the end of the list or partical->next == NULL

      readPrev = read; 
      read = read->next;

   } 

   readPrev->next = nextPartical;

}

//removed a partical from the partical list
void removePartical( struct partical* remove, struct partical* previous ){

   previous->next = remove->next;
   remove->next = NULL;
   free( remove );

}



//the function loops through all the particals checking to see if they have collided with each other yet
void checkCollision( struct turnHead* head ){


   struct partical* read;
   struct partical* readPrev;
   struct partical* innerRead;
   struct partical* innerReadPrev = NULL;
   struct partical* innerReadPrevPrev= NULL;

   if( head->next == NULL ){

      return; 

   }

   read = head->next;

   while( read != NULL ){//each partical is looped through

      innerRead = read;
      readPrev = read; 
      read = read->next;

      while( innerRead != NULL ){//while each partical is being looped through for each partial

	 innerReadPrevPrev = innerReadPrev;
	 innerReadPrev = innerRead;
	 innerRead = innerRead->next;	

	 if( innerReadPrev != readPrev ){//if the two particals are no the samme

	    if( ( ( readPrev->xPosition / SCALING_FACTOR ) >= ( ( innerReadPrev->xPosition / SCALING_FACTOR )  - 1 ) ) && ( ( readPrev->yPosition / SCALING_FACTOR ) >= ( ( innerReadPrev->yPosition / SCALING_FACTOR ) - 1 ) ) ){//but they are touching each other

	       if( ( ( readPrev->xPosition / SCALING_FACTOR ) <= ( ( innerReadPrev->xPosition / SCALING_FACTOR )  + 1 ) ) && ( ( readPrev->yPosition / SCALING_FACTOR ) <= ( ( innerReadPrev->yPosition / SCALING_FACTOR ) + 1 ) ) ){//within on pixel of each other

		  readPrev->xVelocity = ( readPrev->xVelocity * readPrev->mass ) + ( innerReadPrev->xVelocity * innerReadPrev->mass );//add their momentum
		  readPrev->mass += innerReadPrev->mass;//add their mass
		  readPrev->xVelocity = readPrev->xVelocity / readPrev->mass;//find their new velocity

		  readPrev->yVelocity = ( readPrev->yVelocity * readPrev->mass ) + ( innerReadPrev->yVelocity * innerReadPrev->mass );//same for the other axes
		  readPrev->mass += innerReadPrev->mass;
		  readPrev->yVelocity = readPrev->yVelocity / readPrev->mass;

		  if( innerReadPrevPrev != NULL ){//Removes the partical we add to the new partical

		     removePartical( innerReadPrev, innerReadPrevPrev );

		  }else{//removes if it was at the front of the list

		     head->next = innerReadPrev->next;
		     innerReadPrev->next = NULL;
		     free( innerReadPrev );

		  }

	       }

	    }

	 }

      }

   }

}




//draws all the particals onto the screen
void drawParticalList( struct turnHead* head ){

   struct partical* read;
   struct partical* readPrev;

   if( head->next == NULL ){

      return; 

   }

   read = head->next;

   while( read != NULL ){//loop through all the particals

      readPrev = read; 
      read = read->next;
      al_draw_bitmap( readPrev->image, ( readPrev->xPosition / SCALING_FACTOR ), ( readPrev->yPosition / SCALING_FACTOR ), 0 );//prints the image somewhere on the display
      //by a scalling factor, or how many bits are in each pixel
      //printf( "\nPARTICAL POSITION X: %lld Y: %lld\n", readPrev->xPosition, readPrev->yPosition );
      //printf( "\nPARITCAL MASS: %d\n", readPrev->mass );

   } 

}


//moves the particals by simply looping through all of them and ading their velocity to their position
void moveParticals( struct turnHead* head ){

   struct partical* read;
   struct partical* readPrev;

   if( head->next == NULL ){

      return; 

   }

   read = head->next;

   while( read != NULL ){

      readPrev = read; 
      read = read->next;

      readPrev->xPosition += readPrev->xVelocity;
      readPrev->yPosition += readPrev->yVelocity;

   }

}

//finds the min. The standard function that does this is not supported an I need it to letterbox the screen on start up
float min( float x, float y ){

   if( x < y ){

      return x; 

   }

   return y;

}


//NOTE: main loop is composed of code fore three perposes. Main loop for driving simulation. Initing ALlegro 5 and making the screen resolution independent
int main(int argc, char **argv){

   uint64_t count = 48;
   char fpsPrint[50];
   int monitorWidth, monitorHeight, scaleWidth, scaleHeight;
   float scaleX, scaleY, scale, xDisplacement, yDisplacement; 
   uint8_t exit = 0;
   int i;

   srand( time( NULL ) );//init srand and all the special allegro structures
   ALLEGRO_DISPLAY *display = NULL;
   ALLEGRO_DISPLAY_MODE displayData;
   ALLEGRO_BITMAP *buffer = NULL;
   ALLEGRO_BITMAP *test= NULL;
   ALLEGRO_EVENT_QUEUE *event_queue = NULL;
   ALLEGRO_FONT *font = NULL;


   if( !al_init() ){//boot up all the parts of allegro and check if their working

      fprintf( stderr, "ALEGRO 5 HAS FAILED INITIALIZATION\n" );
      return -1;

   }

   if( !al_init_image_addon() ){

      fprintf( stderr, "ALLEGRO 5 HAS FAILED TO INITIALIZE IMAGE ADDON\n" );
      return -1;

   }

   if( !al_init_font_addon() ){

      fprintf( stderr, "ALLEGRO 5 HAS FAILED TO INITIALIZE FONT ADDON\n" );
      return -1;

   }

   if( !al_init_ttf_addon() ){

      fprintf( stderr, "ALLEGRO 5 HAS FAILED TO INITIALIZE TTF FONT\n" );
      return -1; 

   }

   al_set_new_display_flags( ALLEGRO_FULLSCREEN_WINDOW );//set the display flag to FULLSCREEN WINDOWED which is full screen minus the glitches in FULLSCREEN flag
   display = al_create_display( displayData.width, displayData.height );//create a display. Entered values don't actuall matter because full screen flag is set

   if( !display ){//check to make sure the display actually created

      fprintf( stderr, "ALLEGRO 5 HAS FAILED TO CREATE DISPLAY\n" );
      return -1;

   }

   font = al_load_ttf_font( "FantasqueSansMono-Regular.ttf", 25, 0 );

   if( !font ){

      fprintf( stderr, "ALLEGRO 5 HAS FAILED TO LOAD FONT\n" );
      return -1;   

   }

   monitorWidth = al_get_display_width( display ) / MACGLITCH;//this is the part the glitches out on OSX with retna display
   monitorHeight = al_get_display_height( display ) / MACGLITCH;//Retina display double size of display for no aparent reason
   //monitorWidth = 2560;//bug testing
   //monitorHeight = 1600;

   //printf( "\nMONITOR_WIDTH: %d\n", monitorWidth );
   //printf( "\nMONITOR_HIGHT: %d\n", monitorHeight );
   buffer = al_create_bitmap( 640, 480 );//the actual screen though is locked at 640 by 480

   if( !buffer){//check to see if the buffer was made

      fprintf( stderr, "ALLEGRO 5 HAS FAILED TO CREATE BUFFER\n" );
      return -1;

   }

   test = al_load_bitmap( "white.png" );//Though I don't actually need to load a png for the background, one is used so display can be switched out to test.png for... testing 

   if( !test ){

      fprintf( stderr, "ALLEGRO 5 HAS FAILED TO LOAD IMAGE\n" ); 
      return -1;

   }

   printf( "\nMONITOR_WIDTH: %d\n", monitorWidth );//more testing prints at init
   printf( "\nMONITOR_HIGHT: %d\n", monitorHeight );
   scaleX = (float)monitorWidth / 640;//the float that the screen is scaled by to fit any display is calculated
   scaleY = (float)monitorHeight / 480;

   printf( "THIS IS SCALE X: %f\n", scaleX );
   printf( "THIS IS SCALE Y: %f\n", scaleY );

   scale = min( scaleX, scaleY );//this is to calculate the letter boxing of the screen.
   printf( "THIS IS SCALE: %f\n", scale );
   scaleWidth = 640 * scale;//the atual width and height of the screen after scalled by our scalling factor
   scaleHeight = 480 * scale;
   xDisplacement = ( (float)monitorWidth - scaleWidth ) / 2;//the displacement or where we are going to print the white box.
   yDisplacement = ( (float)monitorHeight - scaleHeight ) / 2;//aka leterboxing displacement

   printf( "\nxDISPLACEMENT: %f", xDisplacement );
   printf( "\nyDISPLACEMENT: %f", yDisplacement );
   al_install_keyboard();//yes, aparently this is a thing
   event_queue = al_create_event_queue();//and this

   if( !event_queue ){//same old same old, check to see if allegro actually works

      fprintf( stderr, "ALLEGRO 5 HAS FAILED TO CREATE EVENT QUEUE\n" ); 
      return -1;

   }

   struct turnHead* particalHead = malloc( sizeof( struct turnHead ) );//allocate memory for head
   initTurnHead( particalHead );//init the head... basicly make it's partical pointer NULL

   for( i = 0; i < PARTICAL_NUM; i++ ){//spawn particals for each PARTICAL_NUM

      struct partical* Part = malloc( sizeof( struct partical ) );
      initPartical( Part );
      addPartical( particalHead, Part );//init it and add it to the list

   }



   al_register_event_source( event_queue, al_get_keyboard_event_source() );//Need to register event sources as well
   double oldTime = al_get_time();
   double newTime, delta, fps;

   while( !exit ){

      count++;
      newTime = al_get_time();//get time using allegro becaues you want OS independence
      delta = newTime - oldTime;//calculate difference
      fps = 1.0 / ( delta );//devide by one frame, the one loop of the while main loop, to find fps
      oldTime = newTime;//update time
      al_set_target_bitmap( buffer );//you have to set targets for allegro to draw on using functions
      al_clear_to_color( al_map_rgb( 0, 0, 0 ) );//clear to black 
      al_draw_bitmap( test, 0, 0, 0 );//draw our test  bitmap, also known as white.png
      checkCollision( particalHead );//check for collision between the dots
      drawParticalList( particalHead );//draw the dots
      impulseParticalList( particalHead );//give the dots a hit of acceleration bassed on gravity
      moveParticals( particalHead );//move the dots accordingly

      if( count > 50 ){//every 50 frames update the FPS counter

	 sprintf( fpsPrint, "%d FPS", (int)fps );//basicly atoi using sprintf
	 count = 0;//reset the frame clock

      }

      al_draw_text( font, al_map_rgb( 0, 0, 0 ), 5, 5, ALLEGRO_ALIGN_LEFT, fpsPrint );//draw the fps reading... in black using a fancy upon source font
      //printf( "\nMONITOR_FINAL_WIDTH: %d\n", scaleWidth );
      //printf( "\nMONITOR_FINAL_HEIGHT: %d\n", scaleHeight );

      al_set_target_backbuffer( display );//set the display as the target now, everything before this was being drawn on buffer( 640 x 480 ), not display ( your actual display size)
      al_clear_to_color( al_map_rgb( 0, 0, 0 ) );//clear to black
      al_draw_scaled_bitmap( buffer, 0, 0, 640, 480, xDisplacement, ( yDisplacement + scaleHeight ), scaleWidth, scaleHeight, 0 );//draw a scalled version of buffer onto the display. Scale by the float we calculated earlier, aliegn by the letter box and the extra +scaleHeight seems to be a glitch

      al_flip_display();//now actually show the display

      ALLEGRO_EVENT event;// get an event
      al_get_next_event( event_queue, &event ); 

      if( event.type == ALLEGRO_EVENT_KEY_DOWN ){//check to see if it was a key being pressed down

	 if( event.keyboard.keycode == ALLEGRO_KEY_ESCAPE ) {//check to see if it was the escape key

	    exit = 1;//if it was, get out of the main loop

	 }

      }

   } 

   al_destroy_display( display );//destroy the display and end the program

   return 0;

}
