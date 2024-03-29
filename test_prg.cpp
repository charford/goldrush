// GOLD CHASE
// IMPLEMENTATION BY CASEY HARFORD
// CSCI340
// 02/17/2012
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
#include <semaphore.h>
using namespace std;

// use this to store the rows/cols/gameboard
struct mapBoard {
    int rows;
    int cols;
    char board[0];
};

// sets up the shared memory(if not already created), and returns true if 
// everything goes successful
int setupGame(mapBoard*& myptr, char currentplayer, bool& newgame);

// will be used to check locations on the map, and returns the new player location
int checkForGold(int& numberfools_found, int location, mapBoard* myptr, char key, Map& goldMine, bool& foundGold, char currentplayer);

int main(int argc, char* argv[])
{
    bool foundGold = false;
	bool newgame = false;
	mapBoard* myptr;
	int p1Location,a;
	// the current player is determined by the first command line argument(1-4)
	char currentplayer;
	int numberfools_found = 0;
	sem_t* my_sem_ptr;
	my_sem_ptr=sem_open("/casey_semaphore",O_RDWR|O_CREAT,0600,1);

	if(argc==1) {
		cerr << "No player specified. Please specify the player number at command line." << endl;
		exit(1);
	}
	else {
		//set the player for this process
		switch(atoi(argv[1])) {
			case 1:
				currentplayer = G_PLR0;
				break;
			case 2:
				currentplayer = G_PLR1;
				break;
			case 3:
				currentplayer = G_PLR2;
				break;
			case 4:
				currentplayer = G_PLR3;
				break;
		}
	}
	
	// setup the game
    sem_wait(my_sem_ptr);
	p1Location = setupGame(myptr,currentplayer,newgame);
    sem_post(my_sem_ptr);

	// start the game
	Map goldMine(myptr->board,myptr->rows,myptr->cols);
    goldMine.postNotice("Welcome to GoldRush!");

    while(true)
    {   
        a=goldMine.getKey();
        if(a=='Q') { 
			myptr->board[p1Location]=0;
			break;
		}
        if(a==ERR) goldMine.postNotice("No key pressed");
        //down
        if(a=='j' || a==258) {
            sem_wait(my_sem_ptr);
            p1Location=checkForGold(numberfools_found,p1Location,myptr, 'j',goldMine,foundGold,currentplayer);
            sem_post(my_sem_ptr);
	    	goldMine.drawMap();
        }
        //up
        if(a=='k' || a==259) {
            sem_wait(my_sem_ptr);
        	p1Location=checkForGold(numberfools_found,p1Location,myptr, 'k',goldMine,foundGold,currentplayer);
            sem_post(my_sem_ptr);
	    	goldMine.drawMap();
        }
        //left
        if(a=='h' || a==260) {
            sem_wait(my_sem_ptr);
    		p1Location=checkForGold(numberfools_found,p1Location,myptr, 'h',goldMine,foundGold,currentplayer);
            sem_post(my_sem_ptr);
            goldMine.drawMap();
        }
        //right
        if(a=='l' || a==261) {
            sem_wait(my_sem_ptr);
  			p1Location=checkForGold(numberfools_found,p1Location,myptr, 'l',goldMine,foundGold,currentplayer);
            sem_post(my_sem_ptr);
            goldMine.drawMap();
        }
    }
	if(newgame) {
    	shm_unlink("/casey_goldmine");
		sem_close(my_sem_ptr);
		sem_unlink("/casey_semaphore");
	}
}

int setupGame(mapBoard*& myptr, char currentplayer, bool& newgame) {

	int p1Location,i,numbergold,random,my_file_descriptor,myRows=0,myCols=0;
	char* mp;
 	const char* ptr;
    string allLines = "";
    string oneLine;
 
	// open the shared memory
    my_file_descriptor=shm_open("/casey_goldmine", O_RDWR,0600);
	if(my_file_descriptor==-1) {
		//this is a new game
		newgame = true;
    	my_file_descriptor=shm_open("/casey_goldmine", O_RDWR|O_CREAT,0600);
	}

	// the map file
    ifstream inf("mymap.txt");

    if(!inf)
    {
        cerr << "Problem opening file!" << endl;
        exit(1);
    }
 
	//using this to generate random numbers
    srand(time(NULL));

	//get the number of gold to drop(including fools gold)
    getline(inf,oneLine); 
    numbergold = atoi(oneLine.c_str());

	//read in each line of map
    while(!getline(inf,oneLine).eof())
    {
        allLines+=oneLine;
        myCols=oneLine.size();
        myRows++;
    }

	if(newgame) {
		//truncate and write to shared memory
    	ftruncate(my_file_descriptor,sizeof(mapBoard)+myRows*myCols);
   		lseek(my_file_descriptor,sizeof(mapBoard), SEEK_SET);
	    write(my_file_descriptor, allLines.c_str(), myRows*myCols);
	}
	
	//read shared memory, and configure game board rows/cols
    myptr=(mapBoard*)mmap(0,sizeof(mapBoard)+myRows*myCols,PROT_READ|PROT_WRITE,MAP_SHARED,my_file_descriptor,0);
    myptr->rows = myRows;
    myptr->cols = myCols;

	if(newgame) {
	    //drop the gold randomly
	    while(numbergold>0) {
	        random = rand() % (myptr->rows*myptr->cols) + 1;
	        if(myptr->board[random] == ' ') {
	            if(numbergold==1) myptr->board[random]='G';
	            else myptr->board[random]='F';
	            numbergold--;
	        }
	    }

	    mp=myptr->board;
	    ptr = myptr->board;
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
	}

    i=0; 
    //drop the player randomly
    while(i<1) {
        random = rand() % (myptr->rows*myptr->cols) + 1;
        if(myptr->board[random] == 0) {
            myptr->board[random]=currentplayer;
            p1Location = random;
            i++;
        }
    }
	// if it makes it this far, success!
	return p1Location;
}

int checkForGold(int& numberfools_found,int location, mapBoard* myptr,char key, Map& goldMine,bool& foundGold,char currentplayer) {
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
			// check for walls and other players
			if(myptr->board[x+myptr->cols*(y-1)] & G_WALL 
				|| myptr->board[x+myptr->cols*(y-1)] & G_PLR0 
				|| myptr->board[x+myptr->cols*(y-1)] & G_PLR1 
				|| myptr->board[x+myptr->cols*(y-1)] & G_PLR2 
				|| myptr->board[x+myptr->cols*(y-1)] & G_PLR3 
				|| myptr->board[x+myptr->cols*(y-1)] & G_PLR4) {
				return location;
			}
			else {
				if(myptr->board[x+myptr->cols*(y-1)] & G_GOLD) {
                    goldMine.postNotice("You found gold!");
                    foundGold=true;
				}			
				if(myptr->board[x+myptr->cols*(y-1)] & G_FOOL) {
                    goldMine.postNotice("To bad, that was some fools gold! Try again!");
					numberfools_found++;
				}			
				//at this point all checks should be done	
				myptr->board[location]=0;
				myptr->board[x+myptr->cols*(y-1)]=currentplayer;
				return x+myptr->cols*(y-1);
			}
			break;

		//move down
		case 'j':
			if((y+1) >= myptr->rows) {
                if(foundGold) goldMine.postNotice("You win!");
                return location;
            }
			// check for walls and other players
			if(myptr->board[x+myptr->cols*(y+1)] & G_WALL
				|| myptr->board[x+myptr->cols*(y+1)] & G_PLR0 
				|| myptr->board[x+myptr->cols*(y+1)] & G_PLR1 
				|| myptr->board[x+myptr->cols*(y+1)] & G_PLR2 
				|| myptr->board[x+myptr->cols*(y+1)] & G_PLR3 
				|| myptr->board[x+myptr->cols*(y+1)] & G_PLR4) {
				return location;
			}
			else {
				if(myptr->board[x+myptr->cols*(y+1)] & G_GOLD) {
                    goldMine.postNotice("You found gold!");
                    foundGold=true;
				}			
				if(myptr->board[x+myptr->cols*(y+1)] & G_FOOL) {
                    goldMine.postNotice("To bad, that was some fools gold! Try again!");
					numberfools_found++;
				}			
				//at this point all checks should be done	
				myptr->board[location]=0;
				myptr->board[x+myptr->cols*(y+1)]=currentplayer;
				return x+myptr->cols*(y+1);
			}
			break;

		//move left
		case 'h':
            if((x-1) < 0) {
                if(foundGold) goldMine.postNotice("You win!");
                return location;
            }
			// check for walls and other players
			if(myptr->board[(x-1)+myptr->cols*y] & G_WALL
				|| myptr->board[(x-1)+myptr->cols*y] & G_PLR0
				|| myptr->board[(x-1)+myptr->cols*y] & G_PLR1
				|| myptr->board[(x-1)+myptr->cols*y] & G_PLR2
				|| myptr->board[(x-1)+myptr->cols*y] & G_PLR3
				|| myptr->board[(x-1)+myptr->cols*y] & G_PLR4) {
				return location;
			}
			else {
				if(myptr->board[(x-1)+myptr->cols*y] & G_GOLD) {
                    goldMine.postNotice("You found gold!");
                    foundGold=true;
				}			
				if(myptr->board[(x-1)+myptr->cols*y] & G_FOOL) {
                    goldMine.postNotice("To bad, that was some fools gold! Try again!");
					numberfools_found++;
				}			
				//at this point all checks should be done	
				myptr->board[location]=0;
				myptr->board[(x-1)+myptr->cols*y]=currentplayer;
				return (x-1)+myptr->cols*y;
			}
			break;
		
		//move right
		case 'l':
            if((x+1) >= myptr->cols) {
                if(foundGold) goldMine.postNotice("You win!");
                return location;
            }
			// check for walls and other players
			if(myptr->board[(x+1)+myptr->cols*y] & G_WALL
				|| myptr->board[(x+1)+myptr->cols*y] & G_PLR0
				|| myptr->board[(x+1)+myptr->cols*y] & G_PLR1
				|| myptr->board[(x+1)+myptr->cols*y] & G_PLR2
				|| myptr->board[(x+1)+myptr->cols*y] & G_PLR3
				|| myptr->board[(x+1)+myptr->cols*y] & G_PLR4) {
				return location;
			}
			else {
				if(myptr->board[(x+1)+myptr->cols*y] & G_GOLD) {
                    goldMine.postNotice("You found gold!");
                    foundGold=true;
				}			
				if(myptr->board[(x+1)+myptr->cols*y] & G_FOOL) {
                    goldMine.postNotice("To bad, that was some fools gold! Try again!");
					numberfools_found++;
				}			

				//at this point all checks should be done	
				myptr->board[location]=0;
				myptr->board[(x+1)+myptr->cols*y]=currentplayer;
				return (x+1)+myptr->cols*y;
			}
			break;
	}
}
