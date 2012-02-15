#ifndef SCREEN_H
#define SCREEN_H

#include<ncurses.h>
#include<panel.h>
#include<cstdlib>
#include<cstring>
#include<utility> //for std::pair
#include<stdexcept>
#include"goldchase.h"

/////
// The Screen class provides a shallow interface to ncurses
/////
class Screen {
  private:
    int screenHeight;
    int screenWidth;
    WINDOW* innerWindow;
    PANEL* panel;
    std::pair<int,int> _getScreenSize();
    void _two_second_error(const char* errstr);

  public:
    enum colorSchemes {
      c_gold = 1,
      c_error
    };
    Screen(int h, int w);
    ~Screen();
    void panelRefresh();
    void plot(int y, int x, chtype ch, unsigned int attr=A_NORMAL);
    void notice(const char* msg);
    int getKey();
};


#endif //SCREEN_H
