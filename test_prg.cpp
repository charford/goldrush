#include <map>
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

struct mapBoard {
    int rows;
    int cols;
    char board[0];
};

int checkForGold(int location, mapBoard* myptr, char key);

int main()
{
    int my_file_descriptor;
    my_file_descriptor=shm_open("/casey_goldmine", O_RDWR|O_CREAT,0600);

    ifstream inf("mymap.txt");

    if(!inf)
    {
        cerr << "Problem opening file!" << endl;
        exit(1);
    }
 
    int random;
    int myRows=0,myCols=0;
    int p1Location;

    srand(time(NULL));
    string allLines = "";
    string oneLine;

    int numbergold;
    //Remember, the first line is not part
    //of the map.  Therefore:
    getline(inf,oneLine); 
    numbergold = atoi(oneLine.c_str());
    cout << "first line = " << oneLine << endl;
    while(!getline(inf,oneLine).eof())
    {
        allLines+=oneLine;
        myCols=oneLine.size();
        myRows++;
        cout << oneLine << endl;
    }
    

    ftruncate(my_file_descriptor,myRows*myCols);
    write(my_file_descriptor, allLines.c_str(), myRows*myCols);
    mapBoard* myptr=(mapBoard*)mmap(0,sizeof(mapBoard)+myRows*myCols,PROT_READ|PROT_WRITE,MAP_SHARED,my_file_descriptor,0);
    myptr->rows = myRows;
    myptr->cols = myCols;
   
    char* mp=myptr->board;

    //drop the gold
    while(numbergold>0) {
        random = rand() % (myptr->rows*myptr->cols) + 1;
        if(myptr->board[random] == ' ') {
            if(numbergold==1) myptr->board[random]='G';
            else myptr->board[random]='F';
            numbergold--;
        }
    }

    int i=0; 
    //drop the players
    while(i<1) {
        random = rand() % (myptr->rows*myptr->cols) + 1;
        if(myptr->board[random] == ' ') {
            myptr->board[random]=G_PLR0;
            p1Location = random;
            i++;
        }
    }
    const char* ptr = myptr->board;
    while(*ptr!='\0')
    {
        if(*ptr==' ')      *mp=0;
        else if(*ptr=='*') *mp=G_WALL; //A wall
        else if(*ptr=='1') *mp=G_PLR0; //The first player
        else if(*ptr=='2') *mp=G_PLR1; //The second player
        else if(*ptr=='G') *mp=G_GOLD; //Real gold!
        else if(*ptr=='F') *mp=G_FOOL; //Fool's gold
        ++ptr;
        ++mp;
    }

    Map goldMine(myptr->board,myptr->rows,myptr->cols);
    int a=0;
    cout << "p1 x,y = " << p1Location % myCols << ", " << p1Location / myCols << endl;
    goldMine.postNotice("This is a notice.");
    while(true)
    {   
        a=goldMine.getKey();
        if(a=='Q') break;
        if(a==ERR) goldMine.postNotice("No key pressed");
        //down
        if(a=='j' || a==258) {
            p1Location=checkForGold(p1Location,myptr, 'j');
	    	goldMine.drawMap();
        }
        //up
        if(a=='k' || a==259) {
        	p1Location=checkForGold(p1Location,myptr, 'k');
	    	goldMine.drawMap();
        }
        //left
        if(a=='h' || a==260) {
    		p1Location=checkForGold(p1Location,myptr, 'h');
            goldMine.drawMap();
        }
        //right
        if(a=='l' || a==261) {
  			p1Location=checkForGold(p1Location,myptr, 'l');
            goldMine.drawMap();
        }
    }
    shm_unlink("/casey_goldmine");
}

int checkForGold(int location, mapBoard* myptr,char key ) {
    int x,y;
    //get current location
    x = location % myptr->cols;
    y = location / myptr->cols;
	
	switch(key) {
		//move up
		case 'k':
			if(myptr->board[x+myptr->cols*(y-1)] & G_WALL) {
				return location;
			}
			else {
				if(myptr->board[x+myptr->cols*(y-1)] & G_GOLD) {
					cout << "found gold" << endl;
				}			
				if(myptr->board[x+myptr->cols*(y-1)] & G_FOOL) {
					cout << "found fools gold" << endl;
				}			
				myptr->board[location]=0;
				myptr->board[x+myptr->cols*(y-1)]=G_PLR1;
				return x+myptr->cols*(y-1);
			}
			break;

		//move down
		case 'j':
			if(myptr->board[x+myptr->cols*(y+1)] & G_WALL) {
				return location;
			}
			else {
				if(myptr->board[x+myptr->cols*(y+1)] & G_GOLD) {
					cout << "found gold" << endl;
				}			
				if(myptr->board[x+myptr->cols*(y+1)] & G_FOOL) {
					cout << "found fools gold" << endl;
				}			
				myptr->board[location]=0;
				myptr->board[x+myptr->cols*(y+1)]=G_PLR1;
				return x+myptr->cols*(y+1);
			}
			break;

		//move left
		case 'h':
			if(myptr->board[(x-1)+myptr->cols*y] & G_WALL) {
				return location;
			}
			else {
				if(myptr->board[(x-1)+myptr->cols*y] & G_GOLD) {
					cout << "found gold" << endl;
				}			
				if(myptr->board[(x-1)+myptr->cols*y] & G_FOOL) {
					cout << "found fools gold" << endl;
				}			
				myptr->board[location]=0;
				myptr->board[(x-1)+myptr->cols*y]=G_PLR1;
				return (x-1)+myptr->cols*y;
			}
			break;
		
		//move right
		case 'l':
			if(myptr->board[(x+1)+myptr->cols*y] & G_WALL) {
				return location;
			}
			else {
				if(myptr->board[(x+1)+myptr->cols*y] & G_GOLD) {
					cout << "found gold" << endl;
				}			
				if(myptr->board[(x+1)+myptr->cols*y] & G_GOLD) {
					cout << "found fools gold" << endl;
				}			

				//at this point all checks should be done	
				myptr->board[location]=0;
				myptr->board[(x+1)+myptr->cols*y]=G_PLR1;
				return (x+1)+myptr->cols*y;
			}
			break;
	}
}
