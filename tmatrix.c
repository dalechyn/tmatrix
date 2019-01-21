#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ncurses.h>
#include <ncursesw/curses.h>
#include <wchar.h>
#include <locale.h>
#include <malloc.h>
#include <time.h>

#define dig_length 14
#define symb_count 72
#define speed_delay 4000000

struct Digital {
  int x;
  int head;
  int length;
  wchar_t * data;
};

void colorizeSymbol(wchar_t * destination, wchar_t c, wchar_t * prefix) {
  const int sizePrefix = wcslen(prefix) * sizeof(wchar_t);
  memcpy(destination, prefix, sizePrefix);
  destination[sizePrefix] = c;
  destination[sizePrefix + 1] = L'\n';
}

int get_rand_in_range(int min, int max) {
  return (random() % ((max + 1) - min) + min);
}

wchar_t randomChar(wchar_t chars[symb_count]) {
  return chars[get_rand_in_range(0, symb_count - 1)];
}

wchar_t * generateRain(wchar_t chars[symb_count], int length) {
  wchar_t * arr = malloc(length * sizeof(wchar_t) + 1);
  for(int i = 0; i < length; i++)
    arr[i] = randomChar(chars);
  arr[length] = L'\0';
  return arr;
}

void randomizeDL(struct Digital * line, wchar_t chars[symb_count]) {
  for(int i = 0; i < dig_length; i++)
    //mixing chars with 33% chance
    if(get_rand_in_range(0, 2) == 0)
      line->data[i] = randomChar(chars);
}

void moveAndDrawDL(struct Digital * line, int y_size, wchar_t chars[symb_count]) {
  line->head++;
  //mixing DL
  randomizeDL(line, chars);
  if(line->head >= 0 && line->head < y_size) {
    //move all symbols by 1 to create empty space at the 0
    for(int i = 0; i < symb_count - 1; i++) line->data[i + 1] = line->data[i];
    line->data[0] = randomChar(chars);
    //initializing color buffer, it has fixed size of 68 bytes
    wchar_t buff[18];
    //printing first white bold symbol
    colorizeSymbol(buff, line->data[0], L"\033[01;38;05;15m");
    mvaddwstr(line->head, line->x, buff);
    //next two symbols are also white but not bold
    if(line->head - 1 >= 0 && line->head - 1 < y_size) {
      colorizeSymbol(buff, line->data[1], L"\033[38;05;15m");
      mvaddwstr(line->head - 1, line->x, buff);
    }
    if(line->head - 2 >= 0 && line->head - 2 < y_size) {
      colorizeSymbol(buff, line->data[2], L"\033[38;05;15m");
      mvaddwstr(line->head - 2, line->x, buff);
    }
    //other are green unbold
    for(int i = 3; i < symb_count; i++) {
      if(line->head - i > 0 && line->head - i < y_size) {
        colorizeSymbol(buff, line->data[i], L"\033[38;05;46m");
        mvaddwstr(line->head - i, line->x, buff);
      }
    }
  }
}

int main() {
  wchar_t chars[symb_count] = {
    L'ﾊ', L'ﾐ', L'ﾋ', L'ｰ', L'ｳ', L'ｼ', L'ﾅ', L'ﾓ', L'ﾆ', L'ｻ', L'ﾜ',
    L'ﾂ', L'ｵ', L'ﾘ', L'ｱ', L'ﾎ', L'ﾃ', L'ﾏ', L'ｹ', L'ﾒ', L'ｴ', L'ｶ',
    L'ｷ', L'ﾑ', L'ﾕ', L'ﾗ', L'ｾ', L'ﾈ', L'ｽ', L'ﾀ', L'ﾇ', L'ﾍ', L'ｦ',
    L'ｲ', L'ｸ', L'ｺ', L'ｿ', L'ﾁ', L'ﾄ', L'ﾉ', L'ﾌ', L'ﾔ', L'ﾖ', L'ﾙ',
    L'ﾚ', L'ﾛ', L'ﾝ', L'0', L'1', L'2', L'3', L'4', L'5', L'7', L'8',
    L'9', L':', L'・', L'.', L'"', L'=', L'*', L'+', L'-', L'<', L'>',
    L'¦', L'｜', L'╌', L'ç', L'ﾘ', L'ｸ'
  };

  //Random numbers seed
  srandom(time(NULL));
  setlocale(LC_ALL, "");

  wprintf(L"%sWhy? 为什么？\n", "\033[01;38;05;46m");

  
  initscr();
  noecho();
  curs_set(0);

  if (has_colors()) {
    start_color();
    if (use_default_colors() != ERR) {
      init_pair(COLOR_BLACK, -1, -1);
      init_pair(COLOR_GREEN, COLOR_GREEN, -1);
      init_pair(COLOR_WHITE, COLOR_WHITE, -1);
      init_pair(COLOR_RED, COLOR_RED, -1);
      init_pair(COLOR_CYAN, COLOR_CYAN, -1);
      init_pair(COLOR_MAGENTA, COLOR_MAGENTA, -1);
      init_pair(COLOR_BLUE, COLOR_BLUE, -1);
       init_pair(COLOR_YELLOW, COLOR_YELLOW, -1);
    } else {
      init_pair(COLOR_BLACK, COLOR_BLACK, COLOR_BLACK);
      init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
      init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
      init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
      init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
      init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
      init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
      init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
      }
    }


  int max_x = 0, max_y = 0;
  getmaxyx(stdscr, max_y, max_x);
  
  struct Digital * digitals = malloc(sizeof(struct Digital) * max_x);

  // init digital lines columns
  for(int j = 0; j < max_x; j++) {
    int l = get_rand_in_range(0, max_y);
    digitals[j] = (struct Digital){j, get_rand_in_range(-10, -1), l, generateRain(chars, dig_length)};
  }

  while(1) {
    //going through all digital lines
    for(int i = 0; i < max_x; i++) {
      //print them
      moveAndDrawDL(digitals + i, max_y, chars);
    }
    usleep(speed_delay);
    refresh(); 
  }

  endwin();
  return 0;
}

