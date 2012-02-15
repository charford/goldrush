#include "goldchase.h"
#include "Map.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
int main()
{

  string line;
  int rows=0;
  int cols=0;
  ifstream myfile("mymap.txt");
  char map[10000]; //something arbitrarily large for this test
  char* mp=map;
  getline(myfile,line);
    while (!getline(myfile,line).eof()) {
      const char *ptr = line.c_str();
      cols = line.size();
        cout << *ptr << endl;
       while(*ptr!='\0') {
         cout << ptr++ << endl;
       }
      //need to add drop of gold randomly here

      //make map
    //  while(*ptr!='\0')
    //  {
    //    if(*ptr==' ')      *mp=0;
    //    else if(*ptr=='*') *mp=G_WALL; //A wall
    //    else if(*ptr=='1') *mp=G_PLR1; //The first player
    //    else if(*ptr=='2') *mp=G_PLR2; //The second player
    //    else if(*ptr=='G') *mp=G_GOLD; //Real gold!
    //    else if(*ptr=='F') *mp=G_FOOL; //Fool's gold
    //    ++ptr;
    //    ++mp;
    //  }
    //  rows++;
    }
    return 0;
   cout << rows << ", " << cols << endl;
   cout << rows * cols << endl;
   cout << mp[1] << endl;
/*  
   const char *theMine=
    "** ** *****  *****"
    "** ** ****** *****"
    "**  F *****  *****"
    "** *******  F   **"
    "** *******   *  **"
    " 2  ****  G  F  **"
    "* *****  *  * ****"
    "*  * *  ** 1  ****"
    "*      ***    ****"
    "**********   *****";

  char map[200]; //something arbitrarily large for this test
  const char* ptr=theMine;
  char* mp=map;
  //Convert the ASCII bytes into bit fields drawn from goldchase.h

while(*ptr!='\0')
  {
    if(*ptr==' ')      *mp=0;
    else if(*ptr=='*') *mp=G_WALL; //A wall
    else if(*ptr=='1') *mp=G_PLR1; //The first player
    else if(*ptr=='2') *mp=G_PLR2; //The second player
    else if(*ptr=='G') *mp=G_GOLD; //Real gold!
    else if(*ptr=='F') *mp=G_FOOL; //Fool's gold
    ++ptr;
    ++mp;
  }
  */
  return 0;
  Map goldMine(map,rows,cols);
  int a=0;
  goldMine.postNotice("This is a notice");
  //while(a=goldMine.getKey()!='Q')
  while(true)
  {
	 a=goldMine.getKey();

	 if(a=='Q') break; //quit

	 // move down
	 if(a=='j') { 
	   std::cout << "j key pressed" << std::endl;
	 }

	 // move up
     if(a=='k') {
     	std::cout << "k key pressed" << std::endl;
     }

	 // move right
     if(a=='l') {
     	std::cout << "l key pressed" << std::endl;
     }
	 // move left
     if(a=='h') {
     	std::cout << "h key pressed" << std::endl;
     }
     if(a==ERR) goldMine.postNotice("No key pressed");
  }
}
