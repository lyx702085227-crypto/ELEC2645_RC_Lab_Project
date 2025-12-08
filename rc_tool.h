#ifndef RC_TOOL_H
#define RC_TOOL_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h> 
#include <string.h>
#include <ctype.h> 
#include <time.h>
#include <unistd.h> // (usleep)

// --- 0. ANSI Color Palette & Theme ---
#define COL_RESET   "\033[0m"
#define COL_BLACK   "\033[0;30m"
#define COL_RED     "\033[1;31m"
#define COL_GREEN   "\033[1;32m"
#define COL_YELLOW  "\033[1;33m"
#define COL_BLUE    "\033[1;34m"
#define COL_MAGENTA "\033[1;35m"
#define COL_CYAN    "\033[1;36m"
#define COL_WHITE   "\033[1;37m"
#define COL_BG_BLUE "\033[44m"

// Semantic Theme 
#define THEME_TITLE   COL_CYAN    
#define THEME_BORDER  COL_BLUE    
#define THEME_OPTION  COL_YELLOW  
#define THEME_VALUE   COL_GREEN   
#define THEME_ERROR   COL_RED     
#define THEME_PROMPT  COL_WHITE   
#define THEME_PLOT    COL_MAGENTA 

// --- 1. Data Structures ---
typedef struct {
    float R;
    float C;
    float Vs;
    float tau;
    float t_max;
    float step_size;
    int num_points;
    
    
    float *data_points; 

    int is_configured;
    int is_calculated;
} CircuitParams;

// extern
extern const float E24_SERIES[];

// --- 2. Function Prototypes ---

// UI & Helper Functions
void clear_screen();
void sleep_ms(int milliseconds);
void show_loading_bar(const char *task);
void type_text(const char *text, int delay_ms);
void print_header(const char *subtitle);
void print_menu(const CircuitParams *params);
void print_footer();
void print_splash_screen();
void print_exit_screen();
void wait_for_user();
void to_lower_case(char *str);

// Parser & Algorithm Functions
float parse_smart_value(const char *str);
float get_input_with_unit(const char* prompt);
float find_closest_e24(float val);
int levenshtein_distance(const char *s1, const char *s2);
int parse_menu_command(const char *input);

// Core Logic Functions
void setup_parameters(CircuitParams *params, char *args);
void run_simulation(CircuitParams *params);
void plot_results(const CircuitParams *params);
void plot_results_vertical(const CircuitParams *params);
void save_data_to_csv(const CircuitParams *params);

// AI Module Functions
void ai_noisy_data_fitting(const CircuitParams *params);
void ai_assistant_menu(const CircuitParams *params);
void ai_quick_calc(const CircuitParams *params);

#endif // RC_TOOL_H