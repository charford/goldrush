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

int checkForGold(int location, mapBoard* myptr, char key, Map& goldMine, bool& foundGold);

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
    bool foundGold = false;

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

    ftruncate(my_file_descriptor,sizeof(mapBoard)+myRows*myCols);
    lseek(my_file_descriptor,sizeof(mapBoard), SEEK_SET);
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
            p1Location=checkForGold(p1Location,myptr, 'j',goldMine,foundGold);
	    	goldMine.drawMap();
        }
        //up
        if(a=='k' || a==259) {
        	p1Location=checkForGold(p1Location,myptr, 'k',goldMine,foundGold);
	    	goldMine.drawMap();
        }
        //left
        if(a=='h' || a==260) {
    		p1Location=checkForGold(p1Location,myptr, 'h',goldMine,foundGold);
            goldMine.drawMap();
        }
        //right
        if(a=='l' || a==261) {
  			p1Location=checkForGold(p1Location,myptr, 'l',goldMine,foundGold);
            goldMine.drawMap();
        }
    }
    shm_unlink("/casey_goldmine");
}

int checkForGold(int location, mapBoard* myptr,char key, Map& goldMine,bool& foundGold) {
    int x,y;
    //get current location
    x = location % myptr->cols;
    y = location / myptr->cols;
	
	switch(key) {
		//move up
		case 'k':
            if((y-1) < 0) {
                if(foundGold) goldMine.postNotice("You win!");
                return location;
            }
			if(myptr->board[x+myptr->cols*(y-1)] & G_WALL) {
				return location;
			}
			else {
				if(myptr->board[x+myptr->cols*(y-1)] & G_GOLD) {
                    goldMine.postNotice("You found gold!");
                    foundGold=true;
				}			
				if(myptr->board[x+myptr->cols*(y-1)] & G_FOOL) {
                    goldMine.postNotice("To bad, that was some fools gold! Try again!");
				}			
				myptr->board[location]=0;
				myptr->board[x+myptr->cols*(y-1)]=G_PLR0;
				return x+myptr->cols*(y-1);
			}
			break;

		//move down
		case 'j':
            if((y+1) >= myptr->rows) {
                if(foundGold) goldMine.postNotice("You win!");
                return location;
            }
			if(myptr->board[x+myptr->cols*(y+1)] & G_WALL) {
				return location;
			}
			else {
				if(myptr->board[x+myptr->cols*(y+1)] & G_GOLD) {
                    goldMine.postNotice("You found gold!");
                    foundGold=true;
				}			
				if(myptr->board[x+myptr->cols*(y+1)] & G_FOOL) {
                    goldMine.postNotice("To bad, that was some fools gold! Try again!");
				}			
				myptr->board[location]=0;
				myptr->board[x+myptr->cols*(y+1)]=G_PLR0;
				return x+myptr->cols*(y+1);
			}
			break;

		//move left
		case 'h':
            if((x-1) < 0) {
                if(foundGold) goldMine.postNotice("You win!");
                return location;
            }
			if(myptr->board[(x-1)+myptr->cols*y] & G_WALL) {
				return location;
			}
			else {
				if(myptr->board[(x-1)+myptr->cols*y] & G_GOLD) {
                    goldMine.postNotice("You found gold!");
                    foundGold=true;
				}			
				if(myptr->board[(x-1)+myptr->cols*y] & G_FOOL) {
                    goldMine.postNotice("To bad, that was some fools gold! Try again!");
				}			
				myptr->board[location]=0;
				myptr->board[(x-1)+myptr->cols*y]=G_PLR0;
				return (x-1)+myptr->cols*y;
			}
			break;
		
		//move right
		case 'l':
            if((x+1) >= myptr->cols) {
                if(foundGold) goldMine.postNotice("You win!");
                return location;
            }
			if(myptr->board[(x+1)+myptr->cols*y] & G_WALL) {
				return location;
			}
			else {
				if(myptr->board[(x+1)+myptr->cols*y] & G_GOLD) {
                    goldMine.postNotice("You found gold!");
                    foundGold=true;
				}			
				if(myptr->board[(x+1)+myptr->cols*y] & G_GOLD) {
                    goldMine.postNotice("To bad, that was some fools gold! Try again!");
				}			

				//at this point all checks should be done	
				myptr->board[location]=0;
				myptr->board[(x+1)+myptr->cols*y]=G_PLR0;
				return (x+1)+myptr->cols*y;
			}
			break;
	}
}
