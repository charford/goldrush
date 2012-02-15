#include<ncurses.h>
#include<panel.h>
#include<cstdlib>
#include<cstring>
#include<utility> //for std::pair
#include<stdexcept>

#include"goldchase.h"
#include"Screen.h"

// Utility function for fatal errors
void Screen::_two_second_error(const char* errstr)
{
  attr_t attrs; //used in attr_(get|set)
  short pair; //used in attr_(get|set)
  attr_get(&attrs,&pair,NULL);//save terminal state
  attron(COLOR_PAIR(c_error)|A_BLINK);
  //Locate message in center of screen if room, else (0,0)
  int maxlen,maxwid;
  mvprintw(screenHeight>1 ? screenHeight/2 : 0, //y-coordinate for message
      screenWidth>std::strlen(errstr) ? screenWidth/2-strlen(errstr)/2 : 0, //x-coordinate for message
      errstr);
  ::refresh();
  attr_set(attrs,pair,NULL);//restore terminal state
  sleep(2);
}

//Screen ctor constructs a space with a box around it
Screen::Screen(int h, int w)
{
  //First, call initialization functions
  initscr();  // Start curses mode
  start_color(); // We'll use color
  cbreak();// Line buffering disabled, Pass everything to me
  noecho();// Don't echo characters user types
  curs_set(0); //make cursor invisible
  keypad(stdscr, TRUE); //enable keypad arrows

  //set up color schemes
  init_pair(c_error, COLOR_WHITE, COLOR_RED);
  init_pair(c_gold, COLOR_BLACK, COLOR_YELLOW);

  // Interrogate our (physical) window's dimensions
  int maxlen,maxwid;
  std::pair<int,int> maxes=_getScreenSize();
  screenHeight=maxes.first;
  screenWidth=maxes.second;
  // Abort if physical window is smaller than requested
  // we subtract 2 because of border we need to draw
  if(h > screenHeight-2 || w > screenWidth-2)
  {
    _two_second_error("WINDOW NOT LARGE ENOUGH!");
    endwin();
    std::exit(1);
  }
  //outerWindow is +2 because it boxes L*W inner contents
  WINDOW* outerWindow=newwin(h+2,w+2,0,0);;
  new_panel(outerWindow);
  box(outerWindow, 0, 0); //put frame around window (zeros mean use default chars)
  innerWindow=newwin(h, w, 1, 1); //note it's offset by one to miss the outer box
  panel=new_panel(innerWindow);
}

void Screen::notice(const char* msg)
{
  const char* dismiss="Press spacebar to dismiss";
  int greater=strlen(msg)>strlen(dismiss) ? strlen(msg) : strlen(dismiss);
  int ycoord= screenHeight>1 ? screenHeight/2-2 : 0;
  int xcoord=screenWidth>greater ? screenWidth/2-greater/2-1 : 0;
  WINDOW* dialog=newwin(4,greater+2,ycoord,xcoord);
  PANEL* dialog_panel=new_panel(dialog);
    box(dialog,0,0);
  mvwprintw(dialog,1,1+(greater-strlen(msg))/2,msg);
  mvwprintw(dialog,2,1+(greater-strlen(dismiss))/2,dismiss);
  panelRefresh();
  do;
    while(getch()!=' ');
  del_panel(dialog_panel);
  delwin(dialog);
  panelRefresh();

}

Screen::~Screen()
{
  notice("Exiting");
  endwin();
}

void Screen::panelRefresh()
{
  update_panels();
  doupdate();
}

std::pair<int,int> Screen::_getScreenSize()
{
  int x,y;
  getmaxyx(stdscr,y,x);
  return std::pair<int,int>(y,x);
}

void Screen::plot(int y, int x, chtype ch, unsigned int attr)
{
  attr_t attr_save; //used in wattr_(get|set)
  short pair; //used in wattr_(get|set)
  wattr_get(innerWindow,&attr_save,&pair,NULL);//save terminal state
  wattron(innerWindow,attr); //Turn on any attributes passed in
  mvwaddch(innerWindow,y,x,ch); //Write out the character
  wattr_set(innerWindow,attr_save,pair,NULL);//restore terminal state
}

int Screen::getKey()
{
  return getch();
}
