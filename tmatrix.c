#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <wchar.h>
#include <locale.h>
#include <malloc.h>
#include <time.h>

#define symb_count 72

struct Digital {
  int head;
  int length;
  wchar_t * data;
};

wchar_t * randomChars(wchar_t chars[symb_count], int length) {
  wchar_t * arr = malloc(length * sizeof(wchar_t));
  for(int i = 0; i < length; i++)
    arr[i] = chars[get_rand_in_range(0, symb_count - 1)];
  return arr;
}

int get_rand_in_range(int min, int max) {
  return (rand() % ((max + 1) - min) + min);
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

  struct Digital digitals[50];
  //Random numbers seed
  srand(time(NULL));
  setlocale(LC_ALL, "");

  wprintf(L"%sWhy? 为什么？\n", "\033[01;38;05;46m");
  
  initscr();
  noecho();
  curs_set(0);

  start_color();

  int max_x = 0, max_y = 0;
  getmaxyx(stdscr, max_y, max_x);
  
  // init digital lines columns
  for(int j = 0; j < max_x; j++) {
    int l = get_rand_in_range(0, max_y);
    digitals[j] = (struct Digital){-1, l, randomChars(chars, l)};
  }

  endwin();
  return 0;
}

