#include "goldchase.h"
#include "Map.h"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <sys/mman.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

int main() {
    int my_file_descriptor;
    my_file_descriptor=shm_open('/ch_goldmine', O_RDWR|O_CREAT,0600);
    ftruncate(my_file_descriptor,149);
    write(my_file_descriptor, "hi there folks", strlen("hi there folks"));
    read(my_file_descriptor,some_string,15);
    shm_unlink('/ch_goldmine');
}
