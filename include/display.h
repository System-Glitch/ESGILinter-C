#ifndef DISPLAY_H
#define DISPLAY_H

/* FORMAT */
#define FORMAT_BOLD       "\e[1m"
#define FORMAT_DIM        "\e[2m"
#define FORMAT_UNDERLINED "\e[4m"
#define FORMAT_RESET      "\e[0m"

/* COLORS */
#define COLOR_RED     "\e[31m"
#define COLOR_GREEN   "\e[32m"
#define COLOR_YELLOW  "\e[33m"
#define COLOR_BLUE    "\e[34m"
#define COLOR_MAGENTA "\e[35m"
#define COLOR_CYAN    "\e[36m"
#define COLOR_GREY    "\e[1;30m"
#define COLOR_BLACK   "\e[30m"
#define COLOR_DEFAULT "\e[39m"

/* COLORS BOLD */
#define COLOR_RED_BOLD     "\e[1;31m"
#define COLOR_GREEN_BOLD   "\e[1;32m"
#define COLOR_YELLOW_BOLD  "\e[1;33m"
#define COLOR_BLUE_BOLD    "\e[1;34m"
#define COLOR_MAGENTA_BOLD "\e[1;35m"
#define COLOR_CYAN_BOLD    "\e[1;36m"

/* BACKGROUNDS */
#define BACKGROUND_RED       "\e[41m"
#define BACKGROUND_GREEN     "\e[42m"
#define BACKGROUND_YELLOW    "\e[43m"
#define BACKGROUND_BLUE      "\e[44m"
#define BACKGROUND_MAGENTA   "\e[45m"
#define BACKGROUND_CYAN      "\e[46m"
#define BACKGROUND_L_GREY    "\e[47m"
#define BACKGROUND_BLACK     "\e[40m"
#define BACKGROUND_DEFAULT   "\e[49m"

void print_warning(char *file, int line_index, char *line, const char *message);
void print_error(char *file, int line_index, char *line, const char *message);
void print_message(char *file, int line_index, char *line, const char *message, const char *type, const char *color);

#endif
