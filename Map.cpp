#include<ncurses.h>
#include<panel.h>
#include<cstdlib>
#include<cstring>
#include<utility> //for std::pair
#include<iostream>
#include<stdexcept>

#include"goldchase.h"
#include"Screen.h"
#include"Map.h"


//Initialize the object and draw the map
Map::Map(const char* mmem, int ylength, int xwidth) 
  : mapHeight(ylength), mapWidth(xwidth), mapmem(mmem), theMap(ylength, xwidth)
{
  drawMap();
}

int Map::getKey()
{
  return theMap.getKey();
}
void Map::postNotice(const char* msg)
{
  theMap.notice(msg);
}

//Calculate offset into memory array
char Map::operator()(int y, int x)
{
  if(y<0 || y>=mapHeight)
    throw std::out_of_range("Y coordinate out of range");
  if(x<0 || x>=mapWidth)
    throw std::out_of_range("X Coordinate out of range");
  return *(mapmem+y*mapWidth+x);
}

//Draw and refresh map from memory array
void Map::drawMap()
{
  Map& mymap=*this; //ease referencing ourself
  bool upper, lower, left, right;
  for(int y=0; y<mapHeight; ++y)
  {
    for(int x=0; x<mapWidth; ++x)
    {
      char ch=mymap(y,x);

      //Draw an empty square
      if(ch==0)
      {
        theMap.plot(y,x,' ');
        continue;
      }

      //Draw a wall
      if(ch & G_WALL)
      {
        //determine what walls, if any, surround us
        //the redundant &&true will force true values to 1
        upper = y==0 ? true : mymap(y-1,x) & G_WALL && true;
        lower = y==mapHeight-1 ? true : mymap(y+1,x) & G_WALL && true;
        left = x==0 ? true : mymap(y,x-1) & G_WALL && true;
        right = x==mapWidth-1 ? true : mymap(y,x+1) & G_WALL && true;
        int num_walls=upper+lower+left+right;
        // This switch statement plots the correct wall shape.
        // The wall shape changes depending on the presence
        // or absence of walls in the surrounding squars
        switch(num_walls)
        {
          case 0:
          case 4:
            theMap.plot(y,x,ACS_PLUS);
            break;
          case 3:
            if(!upper)
              theMap.plot(y,x,ACS_TTEE);
            if(!lower)
              theMap.plot(y,x,ACS_BTEE);
            if(!left)
              theMap.plot(y,x,ACS_LTEE);
            if(!right)
              theMap.plot(y,x,ACS_RTEE);
            break;
          case 2:
            if(!upper && !left)
              theMap.plot(y,x,ACS_ULCORNER);
            if(!lower && !left)
              theMap.plot(y,x,ACS_LLCORNER);
            if(!upper && !right)
              theMap.plot(y,x,ACS_URCORNER);
            if(!lower && !right)
              theMap.plot(y,x,ACS_LRCORNER);
            if(!lower && !upper)
              theMap.plot(y,x,ACS_HLINE);
            if(!left && !right)
              theMap.plot(y,x,ACS_VLINE);
            break;
          case 1:
            if(lower || upper)
              theMap.plot(y,x,ACS_VLINE);
            if(left || right)
              theMap.plot(y,x,ACS_HLINE);
            break;
        } //end switch
      }//end if ch & G_WALL

      //Draw gold
      if(ch & G_GOLD || ch & G_FOOL)
      {
        theMap.plot(y,x,'G',COLOR_PAIR(Screen::c_gold));
      }

      //Draw player
      if(ch & G_ANYP)
      {
        switch(ch & G_ANYP)
        {
          case G_PLR0:
            theMap.plot(y,x,'1',A_STANDOUT);
            break;
          case G_PLR1:
            theMap.plot(y,x,'2',A_STANDOUT);
            break;
          case G_PLR2:
            theMap.plot(y,x,'3',A_STANDOUT);
            break;
          case G_PLR3:
            theMap.plot(y,x,'4',A_STANDOUT);
            break;
        }
      }

    } //for(x...)
  } //for(y..)
  theMap.panelRefresh();
}
