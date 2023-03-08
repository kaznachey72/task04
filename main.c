#include <stdio.h>
#include "weather.h"

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("USAGE: %s area_name\n", argv[0]);
        return 1;
    }

    show_weather(argv[1]);
    
    return 0;
}
