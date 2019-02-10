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
#include <signal.h>
#include <stdbool.h>

#define symb_count 70
#define colors_count 3

#define VERSION "v0.3-alpha"

#define COLOR_GREEN "\033[00;38;05;46m"
#define COLOR_BOLD_WHITE "\033[01;38;05;15m"
#define COLOR_WHITE "\033[00;38;05;15m"
#define COLOR_RED "\033[38;05;196m"
#define COLOR_PINK "\033[38;05;201m"
#define COLOR_YELLOW "\033[38;05;226m"
#define COLOR_CYAN "\033[38;05;129m"
#define COLOR_BLUE "\033[38;05;68m"
#define COLOR_AQUA "\033[38;05;51m"

static volatile int running = 1;

void intHandler(int dummy) {
    system("tput reset");
    running = 0;   
}

struct Digital {
    int x;
    int head;
    int length;
    int tick;
    int speed;
    wchar_t * data;
};

void printHelp() {
    printf("TMatrix help:\n");
    printf("\tColors:\n");
    printf("\t\t-a: AQUA\n");
    printf("\t\t-b: BLUE\n");
    printf("\t\t-c: CYAN\n");
    printf("\t\t-p: PINK\n");
    printf("\t\t-w: WHITE\n");
    printf("\t\t-r: RED\n");
    printf("\t\t-y: YELLOW\n");
    printf("\tProgram options:\n");
    printf("\t\t--chance [number]: Set random symbol chance spawn (1/chance)\n");
    printf("\t\t--speed [number]: Set speed of tmatrix (ms)\n");
    printf("\t\t--length [number]: Set falling line length\n");
    printf("\tOther:\n");
    printf("\t\t--help: Show help menu\n");
    printf("\t\t-v: Version\n");
}

void getTermSize(int *x, int *y) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    *x = w.ws_col;
    *y = w.ws_row;
}

int areEqual(char * s1, char * s2) {
    return strcmp(s1, s2) == 0;
}

void colorizeSymbol(wchar_t * destination, wchar_t c, wchar_t * prefix) {
    const int sizePrefix = wcslen(prefix) * sizeof(wchar_t);
    memcpy(destination, prefix, sizePrefix);
    destination[sizePrefix] = c;
    //destination[sizePrefix + 1] = L'\n';
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

void outSymb(int x, int y, char * prefix, wchar_t symb) {
    wprintf(L"\n\033[%d;%dH%s%lc\n", y, x, prefix, symb);
}

void randomizeDL(struct Digital * line, wchar_t chars[symb_count], int y_size, char * accent, int symbolChangeChance) {
    for(int i = 0; i < line->length; i++)
        //mixing chars with 5% chance, also check if it's visible
        if(get_rand_in_range(0, symbolChangeChance - 1) == 0 && line->head - i > 0 && line->head - i < y_size - 1) {
            line->data[i] = randomChar(chars);
            //redraw it 
            outSymb(line->x, line->head - i, (i > 2 ? accent: (i > 0 ? COLOR_WHITE  : COLOR_BOLD_WHITE)), line->data[i]);
        }
}

void moveAndDrawDL(struct Digital * line, int y_size, wchar_t chars[symb_count], char * accent, int symbolChangeChance) {
    line->tick++;
    int willMove = 0;
    //draw operations are made on 0-4st elements in line
    if(line->tick == line->speed) {
        willMove = 1;
        line->head++;
        line->tick = 0;
    }

  //check if line hasn't gone past bottom of the terminal
    if(line->head - line->length >= y_size - 1) {
        //if so respawn
        //free its data
        int l = get_rand_in_range(5, y_size / 2);
        free(line->data);
        line->length = l;
        line->head = get_rand_in_range(-30, -1);
        line->speed = get_rand_in_range(1, 6);
        line->tick = 0;
        line->data = generateRain(chars, l);
    }

    //mixing DL
    randomizeDL(line, chars, y_size, accent, symbolChangeChance);
    if(line->head > 0 && willMove) {
        //move all symbols by 1 to create empty space at the 0
        for(int i = line->length - 1; i >= 1; i--) line->data[i] = line->data[i - 1];
        line->data[0] = randomChar(chars);
        //printing first white bold symbol
        if(line->head > 0 && line->head < y_size - 1)
            outSymb(line->x, line->head, COLOR_BOLD_WHITE, line->data[0]);
        //next two symbols are also white but not bold
        if(line->head - 1 > 0 && line->head - 1 < y_size - 1)
            outSymb(line->x,line->head - 1, COLOR_WHITE, line->data[1]);

        if(line->head - 2 > 0 && line->head - 2 < y_size - 1)
            outSymb(line->x, line->head - 2, COLOR_WHITE, line->data[2]);

        //printing green symb
        if(line->head - 3 > 0 && line->head - 3 < y_size - 1)
            outSymb(line->x, line->head - 3, accent, line->data[3]);

        //erasing last symb and checking if last symbol not visible - redefine line
        if(line->head - line->length > 0) {
            outSymb(line->x, line->head - line->length, "", L' ');
        }    
    }
}

int main(int argc, char ** argv) {
    int argi = 1;
    char * accent = COLOR_GREEN;
    int symbolChangeChance = 20;
    int dlLength = -1;
    int dlLengthSet = 0;
    int refreshDelay = 50000;
    while(argi < argc) {
        if(areEqual(argv[argi], "--help")) {
            printHelp();
            return 0;
        } else if(areEqual(argv[argi], "-v")) {
            printf("TMatrix, version %s, created by Vladislav Dalechyn\n", VERSION);
            return 0;
        } else if(areEqual(argv[argi], "-r")) {
            accent = COLOR_RED;
        } else if(areEqual(argv[argi], "-b")) {
            accent = COLOR_BLUE;
        } else if(areEqual(argv[argi], "-a")) {
            accent = COLOR_AQUA;
        } else if(areEqual(argv[argi], "-p")) {
            accent = COLOR_PINK;
        } else if(areEqual(argv[argi], "-c")) {
            accent = COLOR_CYAN;
        } else if(areEqual(argv[argi], "-y")) {
            accent = COLOR_YELLOW;
        } else if(areEqual(argv[argi], "-w")) {
            accent = COLOR_WHITE;
        } else if(areEqual(argv[argi], "--chance")) {
            symbolChangeChance = atoi(argv[argi + 1]);
            if(symbolChangeChance == 0) {
                printf("Symbol change chance is incorrect\n");
                return 0;
            }
            argi++;
        } else if(areEqual(argv[argi], "--speed")) {
            refreshDelay = atoi(argv[argi + 1]) * 1000;
            if(refreshDelay == 0) {
                printf("Speed is incorrect\n");
                return 0;
            }
            argi++;
        } else if(areEqual(argv[argi], "--length")) {
            dlLength = atoi(argv[argi + 1]);
            if(dlLength <= 0) {
                printf("Length is incorrect\n");
                return 0;
            }
            dlLengthSet = 1;
            argi++;
        } else {
            printf("Wrong argument.\nSee --help for help page.\n");
            return 0;
        }
        argi++;
    }

    wchar_t chars[symb_count] = {
        L'ﾊ', L'ﾐ', L'ﾋ', L'ｰ', L'ｳ', L'ｼ', L'ﾅ', L'ﾓ', L'ﾆ', L'ｻ', L'ﾜ',
        L'ﾂ', L'ｵ', L'ﾘ', L'ｱ', L'ﾎ', L'ﾃ', L'ﾏ', L'ｹ', L'ﾒ', L'ｴ', L'ｶ',
        L'ｷ', L'ﾑ', L'ﾕ', L'ﾗ', L'ｾ', L'ﾈ', L'ｽ', L'ﾀ', L'ﾇ', L'ﾍ', L'ｦ',
        L'ｲ', L'ｸ', L'ｺ', L'ｿ', L'ﾁ', L'ﾄ', L'ﾉ', L'ﾌ', L'ﾔ', L'ﾖ', L'ﾙ',
        L'ﾚ', L'ﾛ', L'ﾝ', L'0', L'1', L'2', L'3', L'4', L'5', L'7', L'8',
        L'9', L':', L'.', L'"', L'=', L'*', L'+', L'-', L'<', L'>',
        L'¦', L'╌', L'ç', L'ﾘ', L'ｸ'
    };

    //Random numbers seed
    srandom(time(NULL));
    setlocale(LC_ALL, "");
  
    int max_x = 0, max_y = 0;
    getTermSize(&max_x, &max_y);
    struct Digital * digitals = malloc(sizeof(struct Digital) * max_x);
    system("tput reset");
    system("tput civis");
 
    if(!dlLengthSet) dlLength = max_y / 2; 

    // init digital lines columns
    for(int j = 1; j <= max_x; j++) {
        int l = get_rand_in_range(5, dlLength);
        digitals[j - 1] = (struct Digital){j,  get_rand_in_range(-30, -1), l, 0, get_rand_in_range(1, 6), generateRain(chars, l)};
    }

    signal(SIGINT, intHandler);
    while(running) {
        //resize check
        int new_x = 0, new_y = 0;
        getTermSize(&new_x, &new_y);
        if(max_x != new_x || max_y != new_y) {
            system("tput reset");
            max_y = new_y;
            max_x = new_x;
            if(!dlLengthSet) dlLength = max_y / 2;
            digitals = (struct Digital *) realloc(digitals, sizeof(struct Digital) * max_x);
            for(int j = 1; j <= max_x; j++) {
                int l = get_rand_in_range(5, dlLength);
                digitals[j - 1] = (struct Digital){j,  get_rand_in_range(-30, -1), l, 0, get_rand_in_range(1, 6), generateRain(chars, l)};
            }
        }
        //going through all digital lines
        for(int i = 0; i < max_x; i++) {
            //print them, if line ended, redefine it
            moveAndDrawDL(digitals + i, max_y, chars, accent, symbolChangeChance);
        }
        usleep(refreshDelay);
    }

    return 0;
}

