all: final_pthread.c final_std.c
	gcc final_pthread.c -lallegro -lallegro_main -lallegro_image -lallegro_font -lallegro_ttf -pthread -o 2application
	gcc final_std.c -lallegro -lallegro_main -lallegro_image -lallegro_font -lallegro_ttf -o 1application
