#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <malloc.h>
#include <time.h>

#define dig_length 14
#define symb_count 72
#define speed_delay 80000

#define COLOR_GREEN "\033[00;38;05;46m"
#define COLOR_BOLD_WHITE "\033[01;38;05;15m"
#define COLOR_WHITE "\033[00;38;05;15m"

struct Digital {
  int x;
  int head;
  int length;
  int tick;
  int speed;
  wchar_t * data;
};

void getTermSize(int *x, int *y) {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  *x = w.ws_col;
  *y = w.ws_row;
}

void gotoxy(int col, int row) {
  printf("\n\033[%d;%dH", row, col);
}

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
    //mixing chars with 10% chance
    if(get_rand_in_range(0, 9) == 0)
      line->data[i] = randomChar(chars);
}

void outSymb(int x, int y, char * prefix, wchar_t symb) {
  wprintf(L"\n\033[%d;%dH%s%lc\n", y, x, prefix, symb);
}

int moveAndDrawDL(struct Digital * line, int y_size, wchar_t chars[symb_count]) {
  line->tick++;
  int willMove = 0;
  //draw operations are made on 0-4st elements in line
  if(line->tick == line->speed) {
    willMove = 1;
    line->head++;
    line->tick = 0;
  }
  //mixing DL
  randomizeDL(line, chars);
  if(line->head >= 0 && willMove) {
    //move all symbols by 1 to create empty space at the 0
    for(int i = line->length - 1; i >= 1; i--) line->data[i] = line->data[i - 1];
    line->data[0] = randomChar(chars);
    //printing first white bold symbol
    //gotoxy(line->x, line->head);
    if(line->head < y_size - 1)
      outSymb(line->x, line->head, COLOR_BOLD_WHITE, line->data[0]);
    //next two symbols are also white but not bold
    if(line->head - 1 >= 0 && line->head - 1 < y_size - 1)
      outSymb(line->x,line->head - 1, COLOR_WHITE, line->data[1]);

    if(line->head - 2 >= 0 && line->head - 2 < y_size - 1)
      outSymb(line->x, line->head - 2, COLOR_WHITE, line->data[2]);

    //printing green symb
    if(line->head - 3 >= 0 && line->head - 3 < y_size - 1)
      outSymb(line->x, line->head - 3, COLOR_GREEN, line->data[3]);

    //erasing last symb and checking if last symbol not visible - redefine line
    if(line->head - line->length >= 0) {
      outSymb(line->x, line->head - line->length, "", L' ');
    } else if(line->head - line->length < y_size - 1 && line->head > y_size - 1) { //means it's under screen 
      return 1;
    }
    //other are green unbold
    /*for(int i = 3; i < line->length; i++) {
      if(line->head - i > 0 && line->head - i < y_size - 1) {
        wprintf(L"\n\033[%d;%dH%s%lc\n", line->head - i, line->x, COLOR_GREEN, line->data[i]);
      }
    }*/
  }
  return 0;
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

  int max_x = 0, max_y = 0;
  getTermSize(&max_x, &max_y);
  
  struct Digital * digitals = malloc(sizeof(struct Digital) * max_x);

  system("tput reset");
  // init digital lines columns
  for(int j = 0; j <= max_x; j++) {
    int l = get_rand_in_range(5, max_y/2);
    digitals[j] = (struct Digital){j, get_rand_in_range(-30, -1), l, 0, get_rand_in_range(1, 6), generateRain(chars, dig_length)};
  }

  while(1) {
    //going through all digital lines
    for(int i = 0; i < max_x; i++) {
      //print them, if line ended, redefine it
      if(moveAndDrawDL(digitals + i, max_y, chars)) {
        int l = get_rand_in_range(5, max_y/2);
        digitals[i] = (struct Digital){i, get_rand_in_range(-30, -1), l, 0, get_rand_in_range(1, 6), generateRain(chars, dig_length)};
      }
    }
    usleep(speed_delay);
    //system("tput reset"); 
  }

  return 0;
}

