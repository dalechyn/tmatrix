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
#define speed_delay 40000

struct Digital {
  int x;
  int head;
  int length;
  wchar_t * data;
};

void getTermSize(int *x, int *y) {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  *x = w.ws_col;
  *y = w.ws_row;
}

void gotoxy(int row, int col) {
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
    gotoxy(line->x, line->head);
    wprintf(L"%s%lc", "\033[01;38;05;15m", line->data[0]);
    //next two symbols are also white but not bold
    if(line->head - 1 >= 0 && line->head - 1 < y_size) {
      gotoxy(line->x, line->head - 1);
      wprintf(L"%s%lc", "\033[38;05;15m", line->data[1]);
    }
    if(line->head - 2 >= 0 && line->head - 2 < y_size) {
      gotoxy(line->x, line->head - 2);
      wprintf(L"%s%lc", "\033[38;05;15m", line->data[2]);
    }
    //other are green unbold
    for(int i = 3; i < symb_count; i++) {
      if(line->head - i > 0 && line->head - i < y_size) {
       gotoxy(line->x, line->head - i);
       wprintf(L"%s%lc", "\033[38;05;46m", line->data[i]);     
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

  
  int max_x = 0, max_y = 0;
  getTermSize(&max_x, &max_y);
  
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
    system("tput reset"); 
  }

  return 0;
}

