#include "rc_tool.h"

// =========================================================
// SECTION 1: GLOBAL CONSTANTS 
// =========================================================
// (E24 Resistance/Capacitance)
const float E24_SERIES[] = {
    1.0, 1.1, 1.2, 1.3, 1.5, 1.6, 1.8, 2.0, 2.2, 2.4, 
    2.7, 3.0, 3.3, 3.6, 3.9, 4.3, 4.7, 5.1, 5.6, 6.2, 
    6.8, 7.5, 8.2, 9.1
};

// =========================================================
// SECTION 2: UI & VISUAL UTILITIES 
// =========================================================

// [UI] clear screen ( Windows & Linux/OnlineGDB)
void clear_screen() {
    #ifdef _WIN32
        system("cls");   //  Windows 
    #else
        system("clear"); //  Linux/Mac/OnlineGDB 
    #endif
}

void sleep_ms(int milliseconds) {
    usleep(milliseconds * 1000);
}

void show_loading_bar(const char *task) {
    printf("\n" COL_CYAN "%s" COL_RESET "\n", task);
    printf("[");
    for (int i = 0; i < 40; i++) {
        printf(COL_GREEN "#" COL_RESET);
        fflush(stdout);
        sleep_ms(20);
    }
    printf("] " COL_GREEN "Done!" COL_RESET "\n");
    sleep_ms(300);
}

void print_header(const char *subtitle) {
    clear_screen();
    printf(THEME_BORDER "============================================================" COL_RESET "\n");
    printf(COL_BG_BLUE COL_WHITE "   RC LAB SIMULATOR v2.5 (Pro Edition)                      " COL_RESET "\n");
    printf(THEME_BORDER "============================================================" COL_RESET "\n");
    if (subtitle) {
        printf(" >> %s\n", subtitle);
        printf(THEME_BORDER "------------------------------------------------------------" COL_RESET "\n");
    }
}

// [UI] Main menu
void print_menu(const CircuitParams *params) {
    clear_screen();

    // --- 1. Topic ---
    printf(COL_BG_BLUE COL_WHITE "   RC LAB CONTROL CENTER v3.0                                       " COL_RESET "\n");
    printf("\n");

    // --- 2. Dashboard ---
    printf(THEME_BORDER "   +---------------------- SYSTEM STATUS ----------------------+" COL_RESET "\n");
    
    if (params->is_configured) {
        printf(THEME_BORDER "   | " COL_RESET);
        printf("R : " THEME_VALUE "%-9.0f" COL_RESET " ", params->R); 
        
        if (params->C < 0.001) printf("C : " THEME_VALUE "%-8.1f uF" COL_RESET " ", params->C * 1e6);
        else                   printf("C : " THEME_VALUE "%-8.6f F " COL_RESET " ", params->C);
        
        printf("Vs: " THEME_VALUE "%-5.1f V" COL_RESET, params->Vs);
        printf(THEME_BORDER "                 |\n" COL_RESET);
        
        printf(THEME_BORDER "   | " COL_RESET);
        printf("Tau: " COL_CYAN "%-9.4f s" COL_RESET " ", params->tau);
        
        // Status
        if (params->is_calculated) printf("Mem: " THEME_VALUE "DATA READY   " COL_RESET);
        else                       printf("Mem: " COL_YELLOW "NO DATA      " COL_RESET);
        
        printf("Mode: " COL_MAGENTA "INTERACTIVE" COL_RESET);
        printf(THEME_BORDER "      |\n" COL_RESET);
        
    } else {
        printf(THEME_BORDER "   | " COL_RESET);
        printf(COL_RED "  [!] PARAMETERS NOT CONFIGURED. PLEASE RUN SETUP (1).   " COL_RESET);
        printf(THEME_BORDER " |\n" COL_RESET);
        printf(THEME_BORDER "   | " COL_RESET);
        printf("  R: ---        C: ---        Vs: ---                    ");
        printf(THEME_BORDER " |\n" COL_RESET);
    }
    printf(THEME_BORDER "   +-----------------------------------------------------------+" COL_RESET "\n");
    printf("\n");

    // --- 3. Function Menu ---
    
    // Group 1: Core Workflow
    printf("   " COL_WHITE "[ WORKFLOW ]" COL_RESET "\n");
    
    // Option 1
    printf("    " THEME_OPTION "1." COL_RESET " Configure Parameters");
    if (!params->is_configured) printf(COL_RED "  <-- Start Here" COL_RESET);
    printf("\n");
    
    // Option 2
    printf("    " THEME_OPTION "2." COL_RESET " Run Simulation");
    if (params->is_configured && !params->is_calculated) printf(COL_YELLOW "      <-- Ready to Run" COL_RESET);
    printf("\n");
    
    // Option 3 & 4
    printf("    " THEME_OPTION "3." COL_RESET " View Scope (Standard)");
    if (params->is_calculated) {
        printf(COL_CYAN "   (Trend Visualization)" COL_RESET);
    }
    printf("\n");

    printf("    " THEME_OPTION "4." COL_RESET " View Scope (Vertical)");
    if (params->is_calculated) {
        printf(COL_CYAN "   (Numeric Visualization)" COL_RESET);
    }
    printf("\n");

    printf("\n");

    // Group 2: Advanced Tools
    printf("   " COL_WHITE "[ UTILITIES ]" COL_RESET "\n");
    printf("    " THEME_OPTION "5." COL_RESET " AI Lab Assistant " COL_CYAN "(Smart Tools)" COL_RESET "\n");
    printf("    " THEME_OPTION "6." COL_RESET " Export Data to CSV\n");
    
    printf("\n");
    
    // Group 3: System
    printf("    " THEME_OPTION "7." COL_RESET " Exit System\n");
    
    printf("\n");
    
    
    // Tips
    static int tip_idx = 0;
    const char *t[] = {
        "Try 'set 1k 10u 5' for fast setup.",
        "Type 'run' to start simulation instantly.",
        "The AI Assistant can reverse-engineer circuits!",
        "Use 'Vertical Scope' for high-res waveforms."
    };
    printf(THEME_BORDER "------------------------------------------------------------" COL_RESET "\n");
    printf(COL_WHITE " Tip: %s" COL_RESET "\n", t[tip_idx]);
    tip_idx = (tip_idx + 1) % 4;
}

// [UI] Footer 
void print_footer() {
    printf(THEME_BORDER "------------------------------------------------------------" COL_RESET "\n");
    
    // Tips
    const char *tips[] = {
        "Did you know? Type " COL_YELLOW "'set 1k 10u 5'" COL_RESET " for instant setup.",
        "Fuzzy search is on! Try typing " COL_YELLOW "'run'" COL_RESET ", " COL_YELLOW "'sim'" COL_RESET ", or " COL_YELLOW "'start'" COL_RESET ".",
        "Smart Units: You can type " COL_YELLOW "'4.7k'" COL_RESET ", " COL_YELLOW "'100n'" COL_RESET ", or " COL_YELLOW "'5M'" COL_RESET ".",
        "AI Assistant: Type " COL_YELLOW "'help'" COL_RESET " or " COL_YELLOW "'ai'" COL_RESET " to access the lab assistant.",
        "After plotting, type " COL_YELLOW "'y'" COL_RESET " to quickly analyze data with AI.",
        "Type " COL_YELLOW "'exit'" COL_RESET " or " COL_YELLOW "'bye'" COL_RESET " to quit the application."
    };
    
    // Using static variables to achieve the effect of 'displaying the next item every refresh'
    static int tip_index = 0;
    int num_tips = sizeof(tips) / sizeof(tips[0]);

    printf(COL_WHITE " Tip: %s" COL_RESET "\n", tips[tip_index]);
    
    tip_index = (tip_index + 1) % num_tips;
}

// typewriter effect
void type_text(const char *text, int delay_ms) {
    for (int i = 0; text[i] != '\0'; i++) {
        printf("%c", text[i]);
        fflush(stdout); 
        usleep(delay_ms * 1000);
    }
}

// [UI] startup animation 
void print_splash_screen() {
    clear_screen();
    
    // 1. Logo
    const char *logo[] = {
        COL_CYAN "      _______  _______  _______  _______ " COL_RESET,
        COL_CYAN "     (  ____ )(  ____ \\(  ____ )(  ____ \\" COL_RESET,
        COL_CYAN "     | (    )|| (    \\/| (    )|| (    \\/" COL_RESET,
        COL_CYAN "     | (____)|| |      | (____)|| |      " COL_RESET,
        COL_CYAN "     |     __)| |      |  _____)| |      " COL_RESET,
        COL_CYAN "     | (\\ (   | |      | (      | |      " COL_RESET,
        COL_CYAN "     | ) \\ \\__| (____/\\| )      | (____/\\" COL_RESET,
        COL_CYAN "     |/   \\__/(_______/|/       (_______/" COL_RESET
    };
    
    printf("\n\n");
    for(int i = 0; i < 8; i++) {
        printf("%s\n", logo[i]);
        usleep(40000); 
    }

    // 2. topic
    printf("\n         ");
    type_text(COL_WHITE "Transient Response Analyzer" COL_RESET, 30); 
    printf("\n           ");
    type_text("University Project 2025", 30);
    printf("\n\n");

    // 3. Boot Sequence
    // [....] -> [ OK ] 
    const char *modules[] = {
        "Initializing Kernel...",
        "Loading Math Libraries (<math.h>)...",
        "Calibrating AI Modules...",
        "Checking Memory Integrity...",
        "Establishing User Interface..."
    };

    printf(THEME_BORDER "------------------------------------------------------------" COL_RESET "\n");
    
    for (int i = 0; i < 5; i++) {
        printf("   [" COL_YELLOW "...." COL_RESET "] %s", modules[i]);
        fflush(stdout);
        
        usleep(150000 + (rand() % 200000)); // random delay
        
        printf("\r   [" COL_GREEN " OK " COL_RESET "] %s\n", modules[i]);
        usleep(50000);
    }
    printf(THEME_BORDER "------------------------------------------------------------" COL_RESET "\n");

    // 4. Guide
    printf("\n");
    printf(COL_YELLOW "    Pro Features Detected:\n" COL_RESET);
    printf("   - " COL_CYAN "Smart Units:" COL_RESET " Use 'k', 'm', 'u' suffixes (e.g. 4.7k)\n");
    printf("   - " COL_CYAN "Fast Setup:" COL_RESET "  Type params in one line (e.g. 'set 1k 10u 5')\n");
    printf("   - " COL_CYAN "Fuzzy Cmds:" COL_RESET "  Typing 'config' or 'sim' also works!\n");
    printf("\n");

    printf(THEME_PROMPT ">>> System Ready. Press [Enter] to launch..." COL_RESET);
    char temp[10];
    fgets(temp, sizeof(temp), stdin);
}

void wait_for_user() {
    printf("\n" THEME_PROMPT ">>> Press [Enter] to continue..." COL_RESET);
    char temp[10];
    fgets(temp, sizeof(temp), stdin);
}

// =========================================================
// SECTION 3: PARSING & ALGORITHMS 
// =========================================================

void to_lower_case(char *str) {
    for(int i = 0; str[i]; i++) str[i] = tolower(str[i]);
}

float parse_smart_value(const char *str) {
    char *endptr;
    float value = strtof(str, &endptr);
    float multiplier = 1.0f;
    if (str == endptr) return -1.0f;
    while (*endptr == ' ') endptr++;
    switch (*endptr) {
        case 'p': case 'P': multiplier = 1e-12; break;
        case 'n': case 'N': multiplier = 1e-9;  break;
        case 'u': case 'U': multiplier = 1e-6;  break;
        case 'm':           multiplier = 1e-3;  break;
        case 'k': case 'K': multiplier = 1e3;   break;
        case 'M':           multiplier = 1e6;   break;
        case 'G': case 'g': multiplier = 1e9;   break;
        default:            multiplier = 1.0f;  break;
    }
    return value * multiplier;
}

float get_input_with_unit(const char* prompt) {
    char input[32];
    float value = 0.0f;
    int valid = 0;
    do {
        printf(THEME_PROMPT "%s" COL_RESET, prompt);
        if (fgets(input, sizeof(input), stdin) == NULL) continue;
        input[strcspn(input, "\n")] = 0;
        value = parse_smart_value(input);
        if (value <= 0) {
            printf(THEME_ERROR "  [!] Invalid positive number. Try again (e.g. 1k).\n" COL_RESET);
        } else {
            valid = 1;
        }
    } while (!valid);
    return value;
}

float find_closest_e24(float val) {
    if (val <= 0) return val;
    float exponent = floorf(log10f(val));
    float base_10 = powf(10.0f, exponent);
    float mantissa = val / base_10;
    float min_diff = 10.0f;
    float best_e24 = 1.0f;
    for (int i = 0; i < 24; i++) {
        float diff = fabsf(mantissa - E24_SERIES[i]);
        if (diff < min_diff) { min_diff = diff; best_e24 = E24_SERIES[i]; }
    }
    return best_e24 * base_10;
}

int levenshtein_distance(const char *s1, const char *s2) {
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    if (len1 == 0) return len2;
    if (len2 == 0) return len1;
    int d[50][50]; 
    for (int i = 0; i <= len1; i++) d[i][0] = i;
    for (int j = 0; j <= len2; j++) d[0][j] = j;
    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            int del = d[i - 1][j] + 1;
            int ins = d[i][j - 1] + 1;
            int sub = d[i - 1][j - 1] + cost;
            int min = del; if (ins < min) min = ins; if (sub < min) min = sub;
            d[i][j] = min;
        }
    }
    return d[len1][len2];
}

int parse_menu_command(const char *input) {
    int choice = atoi(input);
    if (choice > 0) return choice;
    
    char temp[50];
    strncpy(temp, input, 49); temp[49] = 0;
    to_lower_case(temp);

    struct { const char *k; int id; } cmds[] = {
        {"setup", 1}, {"config", 1}, {"set", 1}, {"param", 1},
        {"run", 2}, {"start", 2}, {"sim", 2},
        {"plot", 3}, {"view", 3}, {"graph", 3},
        {"vert", 4}, {"high", 4},
        {"ai", 5}, {"help", 5}, {"ask", 5},
        {"save", 6}, {"csv", 6},
        {"exit", 7}, {"quit", 7}, {"bye", 7}
    };
    
    int num_cmds = sizeof(cmds) / sizeof(cmds[0]);
    int best_dist = 100, best_id = 0;
    const char *best_match_str = ""; 
    for (int i = 0; i < 20; i++) {
        int dist = levenshtein_distance(temp, cmds[i].k);
        if (dist == 0) return cmds[i].id;
        if (dist < best_dist) { best_dist = dist; best_id = cmds[i].id; best_match_str = cmds[i].k; }
    }
    
    int threshold = (strlen(temp) <= 3) ? 1 : 2;
    if (best_dist <= threshold) {
        // If distance > 0 print feedback
        if (best_dist > 0) {
            printf(COL_YELLOW "  [Auto-Correct] Did you mean '%s'? (Levenshtein Dist: %d)\n" COL_RESET, 
                   best_match_str, best_dist);
            sleep_ms(1500); 
        }
        return best_id;
    }
    return 0;
}

// =========================================================
// SECTION 4: CORE SIMULATION LOGIC 
// =========================================================

void setup_parameters(CircuitParams *params, char *args) {
    print_header("Parameter Configuration");
    int quick_mode = 0;

    // --- 1. Command Line Arguments ---
    if (args != NULL && strlen(args) > 0) {
        if (strstr(args, "def") != NULL) {
            params->R = 1000.0f;   // 1k
            params->C = 10e-6f;    // 10u
            params->Vs = 5.0f;     // 5V
            quick_mode = 1;
            printf(COL_CYAN "\n[Debug Mode] Loaded Default Parameters: 1k, 10uF, 5V\n" COL_RESET);
        } 
        else {
            char *tr = strtok(args, " ");
            char *tc = strtok(NULL, " ");
            char *tv = strtok(NULL, " ");
            if (tr && tc && tv) {
                float r = parse_smart_value(tr);
                float c = parse_smart_value(tc);
                float v = parse_smart_value(tv);
                if (r > 0 && c > 0) {
                    params->R = r; params->C = c; params->Vs = v;
                    quick_mode = 1;
                    printf(COL_GREEN "\n[Fast Setup] Parameters applied successfully!\n" COL_RESET);
                } else {
                    printf(THEME_ERROR "\n[Error] Invalid values. Switching to interactive mode.\n" COL_RESET);
                }
            }
        }
    }

    // --- 2. Default inquiry ---
    if (!quick_mode) {
        printf(THEME_PROMPT ">> Load debug defaults (1k, 10uF, 5V)? [y/n]: " COL_RESET);
        char buf[10];
        char ans = 'n';
        if (fgets(buf, sizeof(buf), stdin)) ans = tolower(buf[0]);

        if (ans == 'y') {
            params->R = 1000.0f;
            params->C = 10e-6f;
            params->Vs = 5.0f;
            printf(COL_CYAN "\n[Debug Mode] Defaults loaded.\n" COL_RESET);
        } 
        else {
            printf("\n" COL_YELLOW "Tip: Use suffixes like 1k (1000) or 10u (0.00001)\n" COL_RESET);
            
            params->R = get_input_with_unit("Enter Resistance R (Ohms) : ");
            float std_R = find_closest_e24(params->R);
            if (fabsf(params->R - std_R) > 0.0001 * params->R)
                printf(COL_YELLOW "  -> Eng Tip: Closest E24 is %.0f Ohms\n" COL_RESET, std_R);

            params->C = get_input_with_unit("Enter Capacitance C (Farads): ");
            float std_C = find_closest_e24(params->C);
            if (fabsf(params->C - std_C) > 0.0001 * params->C)
                printf(COL_YELLOW "  -> Eng Tip: Closest E24 is %.2e F\n" COL_RESET, std_C);

            printf(THEME_PROMPT "Enter Source Voltage Vs (Volts): " COL_RESET);
            if (fgets(buf, 20, stdin)) params->Vs = strtof(buf, NULL);
        }
    }

    // --- 3. calculation ---
    if (params->R > 0 && params->C > 0) {
        params->tau = params->R * params->C;
        params->t_max = 5.0 * params->tau;
        params->step_size = params->tau / 20.0;
        params->is_configured = 1;
        params->is_calculated = 0;

        printf("\n" THEME_BORDER "----------------------------------------" COL_RESET "\n");
        printf("  R  = %-10.2f Ohms\n", params->R);
        printf("  C  = %-10.2e F\n", params->C);
        printf("  Vs = %-10.2f V\n", params->Vs);
        printf(THEME_BORDER "----------------------------------------" COL_RESET "\n");
        printf("  Tau = " COL_CYAN "%.4f s" COL_RESET "\n", params->tau);
        
        // coaching
        if (!quick_mode && (params->R != 1000.0f || params->C != 10e-6f || params->Vs != 5.0f)) {
            printf("\n" THEME_BORDER "----------------------------------------" COL_RESET "\n");
            printf(COL_CYAN "   Smart Coaching:" COL_RESET " Want to do this faster?\n");
            printf("    Type: " THEME_OPTION "> setup %g %g %g" COL_RESET "\n", params->R, params->C, params->Vs);
            printf("    Or:   " THEME_OPTION "> setup default" COL_RESET " (for 1k/10u/5V)\n");
            printf(THEME_BORDER "----------------------------------------" COL_RESET "\n");
        }

        // jump
        printf("\n" THEME_PROMPT ">>> Configuration ready. Run simulation now? (y/n): " COL_RESET);
        char buf[10];
        char ans = 'n';
        if (fgets(buf, sizeof(buf), stdin)) ans = buf[0];

        if (ans == 'y' || ans == 'Y') {
            run_simulation(params);
        } else {
            wait_for_user();
        }
    }
}

void run_simulation(CircuitParams *params) {
    print_header("Transient Simulation");
    if (!params->is_configured) {
        printf(THEME_ERROR "\n[!] Please configure parameters (Option 1) first.\n" COL_RESET);
        return;
    }

    if (params->data_points) free(params->data_points);
    if (params->step_size <= 0) params->step_size = params->tau / 20.0;
    
    params->num_points = (int)(params->t_max / params->step_size) + 1;
    params->data_points = (float *)malloc(params->num_points * sizeof(float));

    if (params->data_points == NULL) {
        printf(THEME_ERROR "[Error] Memory allocation failed!\n" COL_RESET);
        return;
    }

    // UI animation
    show_loading_bar("Computing differential equations...");

    for (int i = 0; i < params->num_points; i++) {
        float t = i * params->step_size;
        params->data_points[i] = params->Vs * (1.0f - expf(-t / params->tau));
    }
    params->is_calculated = 1;

    printf("\n" THEME_VALUE "[Success] Simulation generated %d data points." COL_RESET "\n", params->num_points);
    
    // jump
    printf("\n" THEME_PROMPT ">>> Would you like to plot the results now? (y/n): " COL_RESET);
    char buf[10];
    char ans = 'n';
    if (fgets(buf, sizeof(buf), stdin)) ans = buf[0];

    if (ans == 'y' || ans == 'Y') {
        plot_results(params);
    } else {
        printf(COL_YELLOW "[Info] Data stored. Access via Main Menu.\n" COL_RESET);
        wait_for_user();
    }
}

void plot_results(const CircuitParams *params) {
    print_header("Visual Analysis (Standard)");
    if (!params->data_points) {
        printf(THEME_ERROR "\n[!] No data available.\n" COL_RESET);
        wait_for_user();
        return;
    }

    printf(THEME_PROMPT "Select Resolution [1: Standard, 2: High-Res]: " COL_RESET);
    char buf[10];
    int height = 20;
    if (fgets(buf, sizeof(buf), stdin) && atoi(buf) == 2) height = 40;

    int width = 60;
    float max_v = (params->Vs == 0) ? 1.0f : params->Vs;

    printf("\n");
    for (int r = height; r >= 0; r--) {
        float v = (float)r / height * max_v;
        printf(COL_WHITE "%5.2fV |" COL_RESET, v);
        for (int c = 0; c < width; c++) {
            int idx = (int)((float)c / width * params->num_points);
            if (idx >= params->num_points) idx = params->num_points - 1;
            int r_val = (int)((params->data_points[idx] / max_v) * height + 0.5f);
            
            if (r_val == r) printf(COL_MAGENTA "*" COL_RESET);
            else if (r == 0) printf(THEME_BORDER "-" COL_RESET);
            else printf(" ");
        }
        printf("\n");
    }
    printf("       +"); for(int k=0; k<width; k++) printf("-"); printf("\n");
    printf("      0s%*s%.4fs\n", width-10, "", params->t_max);

    // jump
    printf("\n" THEME_PROMPT ">>> Analyze with AI Assistant? (y/n) Or you want to solve a specific time/voltage on the curve? (s) : " COL_RESET);
    char ans = 'n';
    if (fgets(buf, sizeof(buf), stdin)) ans = buf[0];
    
    if (ans == 'y' || ans == 'Y') {
        ai_assistant_menu(params);
    } 
    else if (ans == 's') {
        printf(COL_CYAN "\n-> Launching Quick Bidirectional Solver..." COL_RESET "\n");
        
        ai_quick_calc(params); 
        
    }
    else {
        wait_for_user();
    }
}

void plot_results_vertical(const CircuitParams *params) {
    print_header("Visual Analysis (High-Res Vertical)");
    if (!params->data_points) {
        printf(THEME_ERROR "\n[!] No data available.\n" COL_RESET);
        wait_for_user();
        return;
    }
    // Vertical plot
    int step = params->num_points / 40; 
    if (step < 1) step = 1;
    
    printf("   Time   | Voltage Trace\n");
    printf("----------+--------------------------------------------------\n");
    for(int i=0; i<params->num_points; i+=step) {
        float t = i * params->step_size;
        float v = params->data_points[i];
        int bars = (int)((v / params->Vs) * 50);
        printf("%7.4fs |", t);
        for(int j=0; j<bars; j++) printf(COL_MAGENTA "*" COL_RESET);
        printf(" %.2fV\n", v);
    }
    wait_for_user();
}

void save_data_to_csv(const CircuitParams *params) {
    print_header("Export Data");
    if (!params->data_points) {
        printf(THEME_ERROR "\n[!] No data to save.\n" COL_RESET);
        wait_for_user();
        return;
    }
    FILE *fp = fopen("rc_data.csv", "w");
    if (!fp) {
        printf(THEME_ERROR "[Error] File write permission denied.\n" COL_RESET);
        return;
    }
    fprintf(fp, "Time(s),Voltage(V)\n");
    for(int i=0; i<params->num_points; i++) {
        fprintf(fp, "%.6f,%.6f\n", i*params->step_size, params->data_points[i]);
    }
    fclose(fp);
    show_loading_bar("Exporting to CSV...");
    printf(COL_GREEN "\n[Success] Data saved to 'rc_data.csv'.\n" COL_RESET);
    wait_for_user();
}

// =========================================================
// SECTION 5: AI MODULE
// =========================================================

void ai_explain_concepts() {
    printf("\n" THEME_TITLE "---   Concept Explainer ---" COL_RESET "\n");
    printf(COL_CYAN "1. Time Constant (Tau = R * C):" COL_RESET "\n");
    printf("   It represents the speed of the circuit's response.\n");
    printf("   At t = 1 Tau, the voltage reaches approx " THEME_VALUE "63.2%%" COL_RESET " of Vs.\n");
    printf(COL_CYAN "\n2. Steady State (5 * Tau):" COL_RESET "\n");
    printf("   By 5 Tau, the capacitor is considered fully charged (" THEME_VALUE "99.3%%" COL_RESET ").\n");
    wait_for_user();
}

// Solver (V->t  t->V)
void ai_quick_calc(const CircuitParams *params) {
    if (!params->is_configured) {
        printf(THEME_ERROR "\n[AI] Please configure parameters first!\n" COL_RESET);
        wait_for_user();
        return;
    }

    char buf[20];
    int staying = 1; 

    while (staying) {
        clear_screen(); 
        
        printf("\n" THEME_TITLE "---   Quick Bidirectional Solver ---" COL_RESET "\n");
        printf("Current System: Vs = %.2f V, Tau = %.4f s\n", params->Vs, params->tau);
        printf(THEME_BORDER "----------------------------------------" COL_RESET "\n");
        
        printf("What do you want to calculate?\n");
        printf("  " THEME_OPTION "[T]" COL_RESET " Find " COL_CYAN "TIME" COL_RESET " (to reach a specific Voltage)\n");
        printf("  " THEME_OPTION "[V]" COL_RESET " Find " COL_CYAN "VOLTAGE" COL_RESET " (at a specific Time)\n");
        printf("  " THEME_OPTION "[Q]" COL_RESET " Return / Quit\n");
        
        printf(THEME_PROMPT "\nYour Choice > " COL_RESET);
        
        if (!fgets(buf, 20, stdin)) break;
        char mode = tolower(buf[0]);

        if (mode == 'q') {
            staying = 0; 
            printf(COL_YELLOW "\n[Info] Closing Solver.\n" COL_RESET);
        }
        // --- A:  V -> t ---
        else if (mode == 't') {
            float target_v;
            printf(THEME_PROMPT "Enter Target Voltage (V): " COL_RESET);
            if (fgets(buf, 20, stdin)) {
                target_v = parse_smart_value(buf); 

                // Error
                if (target_v >= params->Vs || target_v < 0) {
                    printf(THEME_ERROR "\n[Error] Voltage must be between 0 and Vs (%.2fV).\n" COL_RESET, params->Vs);
                } else {
                    float t_needed = -params->tau * logf(1.0f - (target_v / params->Vs));
                    printf("\n" COL_GREEN ">> Result:" COL_RESET " To reach %.2fV, wait for " THEME_VALUE "%.6f s" COL_RESET ".\n", target_v, t_needed);
                }
            }
            wait_for_user(); 
        }
        // --- B:  t -> V ---
        else if (mode == 'v') {
            float target_t;
            printf(THEME_PROMPT "Enter Time (seconds): " COL_RESET);
            if (fgets(buf, 20, stdin)) {
                target_t = parse_smart_value(buf); 
                
                if (target_t < 0) {
                    printf(THEME_ERROR "\n[Error] Time cannot be negative.\n" COL_RESET);
                } else {
                    float v_result = params->Vs * (1.0f - expf(-target_t / params->tau));
                    printf("\n" COL_GREEN ">> Result:" COL_RESET " At t=%.4fs, the voltage is " THEME_VALUE "%.4f V" COL_RESET ".\n", target_t, v_result);
                }
            }
            wait_for_user(); 
        } 
        // --- Error ---
        else {
            printf(THEME_ERROR "\n[!] Unknown option. Please enter T, V, or Q.\n" COL_RESET);
            sleep_ms(600); 
        }
    }
}

void ai_design_circuit() {
    printf("\n" THEME_TITLE "---   AI Circuit Design Optimizer ---" COL_RESET "\n");
    float target_tau = get_input_with_unit("Enter Target Tau (seconds): ");
    
    if (target_tau <= 0) {
        printf(THEME_ERROR "[AI] Invalid Tau.\n" COL_RESET);
        wait_for_user(); return;
    }

    float common_caps[] = {1e-9, 10e-9, 100e-9, 1e-6, 10e-6, 100e-6, 1000e-6};
    int num_caps = 7;
    float best_R = 0, best_C = 0, min_error = 100.0;

    printf(COL_CYAN "\n-> Analyzing combinations..." COL_RESET "\n");
    for (int i = 0; i < num_caps; i++) {
        float C_try = common_caps[i];
        float R_ideal = target_tau / C_try;
        float R_std = find_closest_e24(R_ideal);
        if (R_std < 10 || R_std > 10e6) continue;
        float tau_actual = R_std * C_try;
        float error = fabsf(tau_actual - target_tau) / target_tau * 100.0;
        if (error < min_error) { min_error = error; best_R = R_std; best_C = C_try; }
    }

    printf("\n[Recommendation] For Tau = %.5fs:\n", target_tau);
    printf("  R = %.0f Ohms, C = %.1e F (Error: %.2f%%)\n", best_R, best_C, min_error);
    wait_for_user();
}

// Quiz mode
void ai_quiz_mode() {
    typedef struct {
        const char *question;
        const char *options[3];
        int correct_opt;
        const char *explanation;
    } QuizItem;

    // Question bank (covering C language, circuit theory, mathematics)
    QuizItem quizzes[] = {
        {
            "Which C library header is required to use the exp() function?",
            {"<stdio.h>", "<stdlib.h>", "<math.h>"},
            3,
            "The exponential function resides in the math library."
        },
        {
            "In an RC circuit, what percentage of Vs is reached at t = 1 Tau?",
            {"50.0%", "63.2%", "99.3%"},
            2,
            "1 - e^(-1) is approximately 0.632."
        },
        {
            "If R = 1000 Ohms and C = 1000 uF, what is the Time Constant (Tau)?",
            {"1.0 seconds", "0.1 seconds", "10 seconds"},
            1,
            "Tau = R * C = 1000 * 0.001 = 1.0s. (Wait, 1000uF is 0.001F)" 
        }
    };

    int total_questions = 3;
    int score = 0;
    char buf[10];

    for (int i = 0; i < total_questions; i++) {
        clear_screen();
        
        printf("\n" THEME_BORDER "========================================" COL_RESET "\n");
        printf(COL_CYAN "     QUIZ MODE: Question %d of %d" COL_RESET "\n", i + 1, total_questions);
        printf(THEME_BORDER "========================================" COL_RESET "\n\n");

        printf(COL_WHITE "%s" COL_RESET "\n\n", quizzes[i].question);

        for (int j = 0; j < 3; j++) {
            printf("  " THEME_OPTION "%d." COL_RESET " %s\n", j + 1, quizzes[i].options[j]);
        }

        printf(THEME_PROMPT "\nYour Answer [1-3]: " COL_RESET);
        if (fgets(buf, sizeof(buf), stdin) == NULL) break;
        int answer = atoi(buf);

        if (answer == quizzes[i].correct_opt) {
            printf("\n" COL_GREEN "[Correct!] +1 Point" COL_RESET "\n");
            score++;
        } else {
            printf("\n" COL_RED "[Wrong!]" COL_RESET " The correct answer was %d.\n", quizzes[i].correct_opt);
        }
        
        // Explanation
        printf(COL_YELLOW ">> Note: %s" COL_RESET "\n", quizzes[i].explanation);

        if (i < total_questions - 1) {
            printf("\n" THEME_PROMPT ">>> Press [Enter] for next question..." COL_RESET);
            fgets(buf, sizeof(buf), stdin); 
        } else {
            printf("\n" THEME_PROMPT ">>> Press [Enter] to see results..." COL_RESET);
            fgets(buf, sizeof(buf), stdin); 
        }
    }

    // --- Grade ---
    clear_screen();
    printf("\n\n");
    printf(THEME_BORDER "########################################" COL_RESET "\n");
    printf(COL_BG_BLUE COL_WHITE "           QUIZ RESULTS                 " COL_RESET "\n");
    printf(THEME_BORDER "########################################" COL_RESET "\n\n");

    printf("   Final Score: " THEME_VALUE "%d / %d" COL_RESET "\n", score, total_questions);
    
    float percentage = (float)score / total_questions * 100.0f;
    printf("   Accuracy   : %.1f%%\n\n", percentage);

    // Comment
    if (score == 3) {
        printf(COL_GREEN "   Rating: *** EXPERT ***" COL_RESET "\n");
        printf("   Excellent! You have mastered the basics.\n");
    } else if (score == 2) {
        printf(COL_CYAN "   Rating: ** GOOD JOB **" COL_RESET "\n");
        printf("   Not bad, but room for improvement.\n");
    } else {
        printf(COL_RED "   Rating: * NEEDS PRACTICE *" COL_RESET "\n");
        printf("   You might want to review the 'Explain Concepts' module.\n");
    }

    printf("\n");
    wait_for_user();
}

// [AI fitting function]
void ai_noisy_data_fitting(const CircuitParams *params) {
    if (!params->is_configured) {
        printf(THEME_ERROR "\n[AI] Please configure parameters first.\n" COL_RESET);
        return;
    }

    #define CANVAS_ROWS 50  
    #define CANVAS_COLS 100 
    #define NUM_PASSES  4
    
    float plot_min_voltage = -params->Vs * 0.2f;
    float plot_max_voltage = params->Vs * 1.2f;
    float plot_range = plot_max_voltage - plot_min_voltage;
    
    char canvas[CANVAS_ROWS][CANVAS_COLS];
    
    for (int r = 0; r < CANVAS_ROWS; r++) {
        for (int c = 0; c < CANVAS_COLS; c++) canvas[r][c] = ' ';
    }
    
    int r_vs_line = (int)(((params->Vs - plot_min_voltage) / plot_range) * (CANVAS_ROWS - 1) + 0.5f);
    int r_0v_line = (int)(((0.0f - plot_min_voltage) / plot_range) * (CANVAS_ROWS - 1) + 0.5f);
    
    for (int c = 0; c < CANVAS_COLS; c++) {
        if (r_vs_line >= 0 && r_vs_line < CANVAS_ROWS) canvas[r_vs_line][c] = ':'; 
        if (r_0v_line >= 0 && r_0v_line < CANVAS_ROWS) canvas[r_0v_line][c] = '-'; 
    }

    printf("\n" THEME_TITLE "---   Noisy Trace Fitting (Full-Range Scope) ---" COL_RESET "\n");
    printf("Vertical Scale: %.2fV to %.2fV (Offset applied)\n", plot_min_voltage, plot_max_voltage);
    srand((unsigned int)time(NULL));

    float sum_tx = 0, sum_t2 = 0;

    for (int pass = 0; pass < NUM_PASSES; pass++) {
        for (int c = 0; c < CANVAS_COLS; c++) {
            float t = (float)c / (CANVAS_COLS - 1) * params->t_max;
            float ideal = params->Vs * (1.0f - expf(-t / params->tau));
            float noise_random = params->Vs * ((rand() % 100 - 50) / 1000.0f); 
            
            #ifndef M_PI
            #define M_PI 3.14159265358979323846
            #endif
            float hum_freq = 3.0f / params->t_max;
            float noise_hum = (params->Vs * 0.08f) * sinf(2 * M_PI * hum_freq * t);
            float ripple_freq = 20.0f / params->t_max;
            float noise_ripple = (params->Vs * 0.03f) * sinf(2 * M_PI * ripple_freq * t);

            float v_display = ideal + noise_random + noise_hum + noise_ripple;
            int r = (int)(((v_display - plot_min_voltage) / plot_range) * (CANVAS_ROWS - 1) + 0.5f);
            
            if (r >= 0 && r < CANVAS_ROWS) canvas[r][c] = '.'; 

            if (c > 0 && v_display > 0.001f && v_display < params->Vs) {
                float y_val = logf(1.0f - (v_display / params->Vs));
                sum_tx += t * y_val;
                sum_t2 += t * t;
            }
        }
    }

    float slope = sum_tx / sum_t2;
    float estimated_tau = -1.0f / slope;

    for (int c = 0; c < CANVAS_COLS; c++) {
        float t = (float)c / (CANVAS_COLS - 1) * params->t_max;
        float v_fit = params->Vs * (1.0f - expf(-t / estimated_tau));
        int r_fit = (int)(((v_fit - plot_min_voltage) / plot_range) * (CANVAS_ROWS - 1) + 0.5f);
        if (r_fit >= 0 && r_fit < CANVAS_ROWS) canvas[r_fit][c] = '*'; 
    }

    printf("\n" THEME_BORDER "----------------------------------------------------------------------------------------------------" COL_RESET "\n");
    printf("   Trace View: " COL_CYAN "." COL_RESET " = Signal   " COL_WHITE ":" COL_RESET "/" COL_WHITE "-" COL_RESET " = Refs   " COL_MAGENTA "*" COL_RESET " = AI Fit\n");
    printf(THEME_BORDER "----------------------------------------------------------------------------------------------------" COL_RESET "\n");

    for (int r = CANVAS_ROWS - 1; r >= 0; r--) {
        float label_val = plot_min_voltage + (float)r/(CANVAS_ROWS-1) * plot_range;
        float threshold = (plot_range / CANVAS_ROWS) * 0.5f;
        
        if (fabsf(label_val - params->Vs) < threshold || fabsf(label_val - 0.0f) < threshold) {
             printf(THEME_VALUE "%5.1fV |" COL_RESET, label_val);
        } else {
             printf(COL_WHITE "%5.1fV |" COL_RESET, label_val);
        }
        
        for (int c = 0; c < CANVAS_COLS; c++) {
            char cell = canvas[r][c];
            if (cell == '*') printf(COL_MAGENTA "*" COL_RESET);
            else if (cell == '.') printf(COL_CYAN "." COL_RESET);
            else if (cell == ':') printf(COL_WHITE ":" COL_RESET);
            else if (cell == '-') printf(THEME_BORDER "-" COL_RESET);
            else printf(" ");
        }
        printf("\n");
    }
    
    printf("       +");
    for(int k=0; k<CANVAS_COLS; k++) printf("-");
    printf("\n");
    printf("      0s");
    for(int k=0; k<CANVAS_COLS-10; k++) printf(" ");
    printf("%.2fs\n", params->t_max);

    float error_pct = fabsf(estimated_tau - params->tau) / params->tau * 100.0f;
    printf("\n-> True Tau: %.4fs | " THEME_VALUE "AI Estimated Tau: %.4fs" COL_RESET "\n", params->tau, estimated_tau);
    printf("[AI Conclusion]: Signal processed. Accuracy: " THEME_VALUE "%.1f%%" COL_RESET ".\n", 100.0f - error_pct);
    
    wait_for_user();
}

void ai_assistant_menu(const CircuitParams *params) {
    int stay = 1;
    char buf[50];
    while(stay) {
        clear_screen();
        
        // --- 1. topic 
        //  COL_MAGENTA 
        printf(COL_BG_BLUE COL_WHITE "   AI LABORATORY ASSISTANT v2.0                                     " COL_RESET "\n");
        printf("\n");

        // --- 2. (No.1 Featured) ---
        printf(THEME_BORDER "   +------------------ " COL_MAGENTA "FEATURED EXPERIMENT" THEME_BORDER " ------------------+" COL_RESET "\n");
        printf(THEME_BORDER "   |                                                         |" COL_RESET "\n");
        printf(THEME_BORDER "   |  " COL_RESET);
        
        printf(THEME_OPTION "1." COL_RESET " " COL_MAGENTA "Noisy Data Fitting & Recovery" COL_RESET "        " COL_YELLOW "[*****]" COL_RESET "        ");
        
        printf(THEME_BORDER "|\n" COL_RESET);
        printf(THEME_BORDER "   |      " COL_RESET);
        printf(COL_WHITE "Simulates oscilloscope noise & regression logic" COL_RESET "    ");
        printf(THEME_BORDER "|\n" COL_RESET);
        printf(THEME_BORDER "   |                                                         |" COL_RESET "\n");
        printf(THEME_BORDER "   +---------------------------------------------------------+" COL_RESET "\n");
        printf("\n");

        // --- 3. (No.2) ---
        printf("   " COL_WHITE "[ ENGINEERING TOOLS ]" COL_RESET "\n");
        printf("    " THEME_OPTION "2." COL_RESET " Design Optimizer   " COL_CYAN "(Target Tau -> E24 Parts)" COL_RESET "\n");
        printf("\n");

        // --- 4. (No.3, 4) ---
        printf("   " COL_WHITE "[ KNOWLEDGE BASE ]" COL_RESET "\n");
        printf("    " THEME_OPTION "3." COL_RESET " Explain Concepts   " COL_CYAN "(Theory of RC)" COL_RESET "\n");
        printf("    " THEME_OPTION "4." COL_RESET " Quick Quiz         " COL_CYAN "(Test your skills)" COL_RESET "\n");
        printf("\n");

        // --- 5. (No.5) ---
        printf(THEME_BORDER "------------------------------------------------------------" COL_RESET "\n");
        printf("    " THEME_OPTION "5." COL_RESET " Return to Main Menu\n");
        printf(THEME_BORDER "------------------------------------------------------------" COL_RESET "\n");

        // Tips
        printf(THEME_PROMPT "\n AI Command > " COL_RESET);
        
        if (!fgets(buf, 50, stdin)) continue;
        
        int choice = atoi(buf);
        to_lower_case(buf);
        // No.1: Noisy Data
        if (strstr(buf, "nois") || strstr(buf, "fit") || strstr(buf, "exp")) choice = 1;
               
        // No.2: Design Optimizer
        if (strstr(buf, "des") || strstr(buf, "opt")) choice = 2;
        
        // No.3: Explain Concepts
        if (strstr(buf, "explain") || strstr(buf, "con") || strstr(buf, "theor")) choice = 3;
        
        // No.4: Quiz
        if (strstr(buf, "quiz") || strstr(buf, "test")) choice = 4;
        
        // No.5: Return
        if (strstr(buf, "ret") || strstr(buf, "back") || strstr(buf, "exit")) choice = 5;

        switch(choice) {
            case 1: ai_noisy_data_fitting(params); break; 
            case 2: ai_design_circuit(); break;
            case 3: ai_explain_concepts(); break; 
            case 4: ai_quiz_mode(); break;        
            case 5: stay = 0; break;
            default: printf(THEME_ERROR "\n [AI] Unknown module. Try numbers 1-6.\n" COL_RESET); sleep_ms(500); break;
        }
    }
}

// =========================================================
// ENDING!!!!!!
// =========================================================
void print_exit_screen() {
    clear_screen();
    printf("\n\n");

    // ASCII image
    const char *art[] = {
    "[38;2;148;148;148m+[0m[38;2;156;156;156m*[0m[38;2;162;162;162m*[0m[38;2;165;165;165m*[0m[38;2;167;167;167m*[0m[38;2;167;167;167m*[0m[38;2;168;168;168m*[0m[38;2;170;170;170m*[0m[38;2;173;173;173m*[0m[38;2;178;178;178m*[0m[38;2;183;183;183m#[0m[38;2;192;192;192m#[0m[38;2;201;201;201m#[0m[38;2;208;208;208m%[0m[38;2;215;215;215m%[0m[38;2;222;222;222m%[0m[38;2;229;229;229m%[0m[38;2;232;233;233m@[0m[38;2;228;229;230m%[0m[38;2;215;216;216m%[0m[38;2;198;199;199m#[0m[38;2;177;179;179m#[0m[38;2;155;156;156m*[0m[38;2;135;135;134m+[0m[38;2;116;115;113m=[0m[38;2;108;105;104m=[0m[38;2;113;106;106m=[0m[38;2;121;116;116m=[0m[38;2;132;126;126m+[0m[38;2;144;136;136m+[0m[38;2;158;148;149m+[0m[38;2;169;158;160m*[0m[38;2;171;164;164m*[0m[38;2;151;152;153m+[0m[38;2;122;123;128m=[0m[38;2;114;113;118m=[0m[38;2;112;111;115m=[0m[38;2;110;109;113m=[0m[38;2;107;107;111m=[0m[38;2;102;102;106m=[0m[38;2;107;104;109m=[0m[38;2;109;106;109m=[0m[38;2;107;103;105m=[0m[38;2;107;103;102m=[0m[38;2;109;104;103m=[0m[38;2;116;112;111m=[0m[38;2;131;129;128m+[0m[38;2;149;146;145m+[0m[38;2;168;166;165m*[0m[38;2;183;184;184m#[0m[38;2;199;200;200m#[0m[38;2;211;212;212m%[0m[38;2;218;218;219m%[0m[38;2;218;218;218m%[0m[38;2;213;212;212m%[0m[38;2;209;209;209m%[0m[38;2;203;203;203m#[0m[38;2;194;194;194m#[0m[38;2;186;186;186m#[0m[38;2;176;176;176m*[0m[38;2;166;166;166m*[0m[38;2;157;157;157m*[0m[38;2;150;150;150m+[0m[38;2;146;146;146m+[0m[38;2;146;146;146m+[0m[38;2;146;146;146m+[0m[38;2;145;145;145m+[0m[38;2;146;146;146m+[0m[38;2;147;147;147m+[0m[38;2;146;146;146m+[0m",
    "[38;2;161;161;161m*[0m[38;2;165;165;165m*[0m[38;2;167;167;167m*[0m[38;2;168;168;168m*[0m[38;2;169;169;169m*[0m[38;2;174;174;174m*[0m[38;2;178;178;178m*[0m[38;2;183;183;183m#[0m[38;2;190;190;190m#[0m[38;2;201;201;201m#[0m[38;2;210;210;210m%[0m[38;2;216;216;216m%[0m[38;2;225;224;224m%[0m[38;2;233;233;233m@[0m[38;2;231;232;233m@[0m[38;2;217;218;219m%[0m[38;2;194;195;194m#[0m[38;2;169;166;164m*[0m[38;2;149;143;139m+[0m[38;2;131;123;123m=[0m[38;2;115;109;109m=[0m[38;2;103;95;96m-[0m[38;2;87;79;80m-[0m[38;2;93;87;88m-[0m[38;2;141;132;136m+[0m[38;2;168;157;162m*[0m[38;2;180;169;172m*[0m[38;2;188;178;179m#[0m[38;2;198;190;191m#[0m[38;2;207;198;199m#[0m[38;2;212;204;206m%[0m[38;2;216;209;210m%[0m[38;2;213;211;210m%[0m[38;2;197;204;204m#[0m[38;2;161;176;183m*[0m[38;2;121;141;148m+[0m[38;2;102;122;132m=[0m[38;2;108;127;137m=[0m[38;2;115;132;144m+[0m[38;2;115;136;146m+[0m[38;2;112;136;144m+[0m[38;2;112;134;144m+[0m[38;2;116;133;149m+[0m[38;2;116;133;149m+[0m[38;2;110;130;140m=[0m[38;2;102;122;128m=[0m[38;2;94;106;116m=[0m[38;2;92;96;104m-[0m[38;2;97;97;101m-[0m[38;2;110;103;106m=[0m[38;2;123;115;116m=[0m[38;2;137;130;130m+[0m[38;2;152;149;147m+[0m[38;2;174;173;172m*[0m[38;2;196;197;197m#[0m[38;2;213;213;213m%[0m[38;2;219;219;219m%[0m[38;2;215;215;215m%[0m[38;2;209;209;209m%[0m[38;2;204;204;204m%[0m[38;2;197;197;197m#[0m[38;2;189;189;189m#[0m[38;2;180;180;180m#[0m[38;2;169;169;169m*[0m[38;2;160;160;160m*[0m[38;2;155;155;155m*[0m[38;2;153;153;153m*[0m[38;2;152;152;152m+[0m[38;2;150;150;150m+[0m[38;2;148;148;148m+[0m",
    "[38;2;167;167;167m*[0m[38;2;170;170;170m*[0m[38;2;173;173;173m*[0m[38;2;178;178;178m*[0m[38;2;181;181;181m#[0m[38;2;187;187;187m#[0m[38;2;197;197;197m#[0m[38;2;207;207;207m%[0m[38;2;216;216;216m%[0m[38;2;223;223;223m%[0m[38;2;232;231;232m@[0m[38;2;232;233;233m@[0m[38;2;214;215;216m%[0m[38;2;182;183;182m#[0m[38;2;156;153;150m*[0m[38;2;135;130;125m+[0m[38;2;120;118;119m=[0m[38;2;100;110;126m=[0m[38;2;80;105;133m-[0m[38;2;63;96;133m-[0m[38;2;59;79;93m:[0m[38;2;66;74;85m:[0m[38;2;82;91;101m-[0m[38;2;100;119;122m=[0m[38;2;161;173;176m*[0m[38;2;206;208;211m%[0m[38;2;214;215;216m%[0m[38;2;219;218;218m%[0m[38;2;224;223;222m%[0m[38;2;226;225;224m%[0m[38;2;227;226;224m%[0m[38;2;228;228;226m%[0m[38;2;229;230;226m@[0m[38;2;232;232;227m@[0m[38;2;236;235;231m@[0m[38;2;235;234;231m@[0m[38;2;231;230;227m@[0m[38;2;229;227;223m%[0m[38;2;229;226;223m%[0m[38;2;229;227;224m%[0m[38;2;229;227;223m%[0m[38;2;230;230;225m@[0m[38;2;227;229;225m%[0m[38;2;224;229;226m%[0m[38;2;222;229;228m%[0m[38;2;219;228;229m%[0m[38;2;199;214;217m%[0m[38;2;165;189;198m#[0m[38;2;139;163;176m*[0m[38;2;120;144;156m+[0m[38;2;108;127;135m=[0m[38;2;98;110;120m=[0m[38;2;93;98;108m-[0m[38;2;92;92;95m-[0m[38;2;102;98;98m-[0m[38;2;122;118;117m=[0m[38;2;157;155;155m*[0m[38;2;194;195;195m#[0m[38;2;216;216;216m%[0m[38;2;218;218;218m%[0m[38;2;214;214;214m%[0m[38;2;208;208;208m%[0m[38;2;205;205;205m%[0m[38;2;201;201;201m#[0m[38;2;190;190;190m#[0m[38;2;180;180;180m#[0m[38;2;171;171;171m*[0m[38;2;164;164;164m*[0m[38;2;162;162;162m*[0m[38;2;159;159;159m*[0m",
    "[38;2;178;178;178m*[0m[38;2;182;182;182m#[0m[38;2;185;185;185m#[0m[38;2;188;188;188m#[0m[38;2;197;197;197m#[0m[38;2;209;209;209m%[0m[38;2;218;218;218m%[0m[38;2;226;226;226m%[0m[38;2;234;233;234m@[0m[38;2;227;227;228m%[0m[38;2;194;195;196m#[0m[38;2;156;154;153m*[0m[38;2;136;131;126m+[0m[38;2;124;123;124m=[0m[38;2;102;120;138m=[0m[38;2;72;109;142m=[0m[38;2;53;108;154m-[0m[38;2;60;120;168m=[0m[38;2;93;144;184m+[0m[38;2;139;171;193m*[0m[38;2;181;194;201m#[0m[38;2;209;219;220m%[0m[38;2;229;232;232m@[0m[38;2;235;236;234m@[0m[38;2;232;234;231m@[0m[38;2;231;232;229m@[0m[38;2;232;233;228m@[0m[38;2;231;233;228m@[0m[38;2;231;233;228m@[0m[38;2;231;232;227m@[0m[38;2;231;232;227m@[0m[38;2;230;232;227m@[0m[38;2;231;232;227m@[0m[38;2;230;231;226m@[0m[38;2;219;221;216m%[0m[38;2;207;209;204m%[0m[38;2;201;202;197m#[0m[38;2;198;201;196m#[0m[38;2;200;203;199m#[0m[38;2;198;201;196m#[0m[38;2;197;199;195m#[0m[38;2;197;198;195m#[0m[38;2;197;197;193m#[0m[38;2;199;199;194m#[0m[38;2;198;193;190m#[0m[38;2;192;183;182m#[0m[38;2;198;187;184m#[0m[38;2;209;197;194m#[0m[38;2;221;213;210m%[0m[38;2;227;224;224m%[0m[38;2;224;229;232m%[0m[38;2;205;218;225m%[0m[38;2;175;198;209m#[0m[38;2;144;170;185m*[0m[38;2;122;141;156m+[0m[38;2;106;117;128m=[0m[38;2;97;102;107m-[0m[38;2;109;105;105m=[0m[38;2;137;132;132m+[0m[38;2;174;173;174m*[0m[38;2;207;207;207m%[0m[38;2;219;219;219m%[0m[38;2;215;215;215m%[0m[38;2;212;212;212m%[0m[38;2;210;210;210m%[0m[38;2;204;204;204m%[0m[38;2;193;193;193m#[0m[38;2;183;183;183m#[0m[38;2;173;173;173m*[0m[38;2;168;168;168m*[0m",
    "[38;2;188;188;188m#[0m[38;2;190;190;190m#[0m[38;2;194;194;194m#[0m[38;2;208;208;208m%[0m[38;2;220;220;220m%[0m[38;2;226;226;226m%[0m[38;2;233;233;233m@[0m[38;2;227;227;227m%[0m[38;2;186;186;186m#[0m[38;2;147;143;142m+[0m[38;2;137;130;128m+[0m[38;2;128;131;139m+[0m[38;2;92;122;152m=[0m[38;2;50;109;160m-[0m[38;2;45;116;169m-[0m[38;2;96;147;184m+[0m[38;2;161;182;200m*[0m[38;2;210;214;218m%[0m[38;2;237;233;231m@[0m[38;2;242;236;234m@[0m[38;2;238;235;234m@[0m[38;2;233;231;230m@[0m[38;2;228;228;227m%[0m[38;2;226;228;226m%[0m[38;2;228;230;227m%[0m[38;2;229;231;228m@[0m[38;2;230;233;228m@[0m[38;2;230;233;228m@[0m[38;2;230;233;228m@[0m[38;2;230;233;227m@[0m[38;2;230;233;227m@[0m[38;2;230;233;228m@[0m[38;2;230;233;228m@[0m[38;2;230;234;228m@[0m[38;2;231;235;229m@[0m[38;2;232;236;231m@[0m[38;2;232;237;232m@[0m[38;2;233;237;231m@[0m[38;2;233;237;231m@[0m[38;2;233;237;231m@[0m[38;2;233;236;231m@[0m[38;2;231;235;230m@[0m[38;2;229;234;229m@[0m[38;2;228;232;227m@[0m[38;2;224;228;223m%[0m[38;2;215;219;215m%[0m[38;2;204;205;203m%[0m[38;2;191;193;190m#[0m[38;2;181;181;178m#[0m[38;2;174;167;166m*[0m[38;2;181;167;167m*[0m[38;2;203;187;187m#[0m[38;2;231;222;219m%[0m[38;2;240;242;240m@[0m[38;2;227;235;237m@[0m[38;2;206;218;223m%[0m[38;2;182;199;207m#[0m[38;2;153;168;181m*[0m[38;2;127;133;144m+[0m[38;2;116;113;120m=[0m[38;2;129;123;128m=[0m[38;2;166;165;166m*[0m[38;2;204;204;204m%[0m[38;2;218;218;218m%[0m[38;2;214;214;214m%[0m[38;2;212;212;212m%[0m[38;2;211;211;211m%[0m[38;2;204;204;204m%[0m[38;2;192;192;192m#[0m[38;2;180;180;180m#[0m",
    "[38;2;194;194;194m#[0m[38;2;204;204;204m%[0m[38;2;218;218;218m%[0m[38;2;224;224;224m%[0m[38;2;231;231;231m@[0m[38;2;233;233;233m@[0m[38;2;197;197;197m#[0m[38;2;146;147;146m+[0m[38;2;135;134;135m+[0m[38;2;132;142;154m+[0m[38;2;88;128;161m=[0m[38;2;32;100;153m-[0m[38;2;46;112;159m-[0m[38;2;121;160;183m+[0m[38;2;188;201;205m#[0m[38;2;222;221;219m%[0m[38;2;229;227;223m%[0m[38;2;226;228;224m%[0m[38;2;226;228;225m%[0m[38;2;226;228;227m%[0m[38;2;227;229;228m%[0m[38;2;230;233;228m@[0m[38;2;231;235;230m@[0m[38;2;231;234;229m@[0m[38;2;230;233;228m@[0m[38;2;230;233;228m@[0m[38;2;230;233;228m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;230;233;228m@[0m[38;2;229;232;227m@[0m[38;2;230;233;227m@[0m[38;2;230;232;227m@[0m[38;2;229;232;227m@[0m[38;2;230;232;228m@[0m[38;2;229;232;228m@[0m[38;2;229;232;228m@[0m[38;2;230;232;228m@[0m[38;2;230;233;228m@[0m[38;2;231;234;228m@[0m[38;2;231;234;229m@[0m[38;2;232;235;230m@[0m[38;2;234;237;231m@[0m[38;2;235;239;234m@[0m[38;2;236;241;235m@[0m[38;2;233;239;233m@[0m[38;2;223;229;228m%[0m[38;2;207;211;211m%[0m[38;2;180;179;180m#[0m[38;2;162;154;156m*[0m[38;2;178;163;165m*[0m[38;2;213;203;203m%[0m[38;2;240;238;235m@[0m[38;2;239;240;235m@[0m[38;2;228;236;228m@[0m[38;2;217;226;226m%[0m[38;2;189;196;206m#[0m[38;2;148;153;160m+[0m[38;2;120;120;125m=[0m[38;2;128;127;128m=[0m[38;2;172;172;172m*[0m[38;2;210;210;210m%[0m[38;2;218;218;218m%[0m[38;2;214;214;214m%[0m[38;2;214;214;214m%[0m[38;2;208;208;208m%[0m[38;2;197;197;197m#[0m",
    "[38;2;211;211;211m%[0m[38;2;222;222;222m%[0m[38;2;228;228;228m%[0m[38;2;234;234;234m@[0m[38;2;219;219;219m%[0m[38;2;164;164;164m*[0m[38;2;138;136;137m+[0m[38;2;154;148;151m+[0m[38;2;132;131;137m+[0m[38;2;65;87;101m-[0m[38;2;55;90;112m-[0m[38;2;114;141;160m+[0m[38;2;185;187;191m#[0m[38;2;219;213;215m%[0m[38;2;220;219;221m%[0m[38;2;221;224;225m%[0m[38;2;225;227;227m%[0m[38;2;227;229;227m%[0m[38;2;228;231;227m@[0m[38;2;230;232;229m@[0m[38;2;230;233;230m@[0m[38;2;230;233;230m@[0m[38;2;230;233;230m@[0m[38;2;230;233;230m@[0m[38;2;230;233;230m@[0m[38;2;230;233;230m@[0m[38;2;230;233;230m@[0m[38;2;230;233;229m@[0m[38;2;230;233;228m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;230;233;228m@[0m[38;2;230;233;228m@[0m[38;2;229;232;227m@[0m[38;2;229;232;227m@[0m[38;2;229;232;227m@[0m[38;2;231;233;229m@[0m[38;2;234;237;232m@[0m[38;2;235;241;236m@[0m[38;2;222;229;227m%[0m[38;2;190;193;193m#[0m[38;2;158;150;153m*[0m[38;2;172;154;157m*[0m[38;2;218;209;209m%[0m[38;2;237;236;232m@[0m[38;2;234;235;227m@[0m[38;2;234;238;232m@[0m[38;2;225;232;231m@[0m[38;2;199;204;209m#[0m[38;2;151;152;159m*[0m[38;2;122;121;124m=[0m[38;2;140;141;141m+[0m[38;2;190;190;190m#[0m[38;2;217;217;217m%[0m[38;2;214;214;214m%[0m[38;2;214;214;214m%[0m[38;2;212;212;212m%[0m",
    "[38;2;225;225;225m%[0m[38;2;228;228;228m%[0m[38;2;235;235;235m@[0m[38;2;204;204;204m%[0m[38;2;147;147;147m+[0m[38;2;152;147;149m+[0m[38;2;150;140;142m+[0m[38;2;88;77;77m-[0m[38;2;46;48;48m.[0m[38;2;100;112;114m=[0m[38;2;177;186;187m#[0m[38;2;209;210;213m%[0m[38;2;216;218;220m%[0m[38;2;222;225;227m%[0m[38;2;225;227;227m%[0m[38;2;226;228;227m%[0m[38;2;227;229;228m%[0m[38;2;228;230;228m@[0m[38;2;229;232;229m@[0m[38;2;230;233;230m@[0m[38;2;230;233;230m@[0m[38;2;230;233;230m@[0m[38;2;230;233;230m@[0m[38;2;230;233;230m@[0m[38;2;230;233;230m@[0m[38;2;230;233;230m@[0m[38;2;230;233;230m@[0m[38;2;230;233;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;230;233;228m@[0m[38;2;230;233;228m@[0m[38;2;230;233;228m@[0m[38;2;229;232;227m@[0m[38;2;229;232;228m@[0m[38;2;232;234;231m@[0m[38;2;235;239;235m@[0m[38;2;220;230;227m%[0m[38;2;175;177;179m*[0m[38;2;152;134;139m+[0m[38;2;196;175;179m#[0m[38;2;233;225;228m%[0m[38;2;232;231;229m@[0m[38;2;231;234;230m@[0m[38;2;233;238;234m@[0m[38;2;225;229;229m%[0m[38;2;189;189;191m#[0m[38;2;139;138;140m+[0m[38;2;130;130;132m+[0m[38;2;175;176;177m*[0m[38;2;215;215;216m%[0m[38;2;215;215;215m%[0m[38;2;214;214;214m%[0m",
    "[38;2;231;231;231m@[0m[38;2;234;234;234m@[0m[38;2;198;198;198m#[0m[38;2;144;144;143m+[0m[38;2;160;154;157m*[0m[38;2;138;127;129m+[0m[38;2;58;47;47m.[0m[38;2;49;53;62m:[0m[38;2;133;146;156m+[0m[38;2;196;204;212m#[0m[38;2;212;216;222m%[0m[38;2;219;222;226m%[0m[38;2;227;226;228m%[0m[38;2;227;227;227m%[0m[38;2;227;228;226m%[0m[38;2;228;229;226m%[0m[38;2;229;230;227m@[0m[38;2;230;230;228m@[0m[38;2;229;232;229m@[0m[38;2;229;233;229m@[0m[38;2;230;233;230m@[0m[38;2;230;233;230m@[0m[38;2;230;233;230m@[0m[38;2;230;233;230m@[0m[38;2;230;233;229m@[0m[38;2;229;232;229m@[0m[38;2;230;233;229m@[0m[38;2;230;233;229m@[0m[38;2;230;233;228m@[0m[38;2;230;233;228m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;235;229m@[0m[38;2;231;235;230m@[0m[38;2;230;233;230m@[0m[38;2;230;233;230m@[0m[38;2;231;234;230m@[0m[38;2;231;234;231m@[0m[38;2;231;234;232m@[0m[38;2;232;235;232m@[0m[38;2;231;234;231m@[0m[38;2;231;233;231m@[0m[38;2;231;233;230m@[0m[38;2;231;234;231m@[0m[38;2;231;234;231m@[0m[38;2;231;234;230m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;230;233;230m@[0m[38;2;229;233;230m@[0m[38;2;229;231;228m@[0m[38;2;232;233;230m@[0m[38;2;236;241;238m@[0m[38;2;204;211;209m%[0m[38;2;154;144;148m+[0m[38;2;185;168;172m*[0m[38;2;230;223;225m%[0m[38;2;232;230;229m@[0m[38;2;230;235;231m@[0m[38;2;231;236;232m@[0m[38;2;233;237;233m@[0m[38;2;210;208;207m%[0m[38;2;153;153;153m*[0m[38;2;128;130;132m+[0m[38;2;170;171;173m*[0m[38;2;214;214;216m%[0m[38;2;213;213;215m%[0m",
    "[38;2;231;231;229m@[0m[38;2;201;200;200m#[0m[38;2;150;150;151m+[0m[38;2;168;163;164m*[0m[38;2;146;134;137m+[0m[38;2;73;67;65m:[0m[38;2;74;86;88m-[0m[38;2;161;180;185m*[0m[38;2;208;214;219m%[0m[38;2;213;214;217m%[0m[38;2;220;220;222m%[0m[38;2;226;225;226m%[0m[38;2;228;225;226m%[0m[38;2;227;226;227m%[0m[38;2;226;227;226m%[0m[38;2;227;228;225m%[0m[38;2;228;229;226m%[0m[38;2;228;229;226m%[0m[38;2;228;230;226m%[0m[38;2;229;231;228m@[0m[38;2;229;232;229m@[0m[38;2;230;233;229m@[0m[38;2;232;234;230m@[0m[38;2;232;234;231m@[0m[38;2;230;234;231m@[0m[38;2;231;234;231m@[0m[38;2;231;234;231m@[0m[38;2;230;233;230m@[0m[38;2;230;233;228m@[0m[38;2;230;233;228m@[0m[38;2;231;234;229m@[0m[38;2;230;233;228m@[0m[38;2;230;233;228m@[0m[38;2;232;235;229m@[0m[38;2;233;236;232m@[0m[38;2;235;237;235m@[0m[38;2;235;239;236m@[0m[38;2;233;237;235m@[0m[38;2;231;234;231m@[0m[38;2;229;233;228m@[0m[38;2;228;231;227m@[0m[38;2;230;232;229m@[0m[38;2;233;238;234m@[0m[38;2;233;238;235m@[0m[38;2;232;236;232m@[0m[38;2;231;234;230m@[0m[38;2;230;233;229m@[0m[38;2;231;234;229m@[0m[38;2;231;234;229m@[0m[38;2;230;233;228m@[0m[38;2;230;233;228m@[0m[38;2;229;232;227m@[0m[38;2;230;233;228m@[0m[38;2;230;233;230m@[0m[38;2;229;233;230m@[0m[38;2;229;232;229m@[0m[38;2;236;238;235m@[0m[38;2;224;231;228m%[0m[38;2;209;209;208m%[0m[38;2;226;224;223m%[0m[38;2;232;232;231m@[0m[38;2;229;233;230m@[0m[38;2;230;235;231m@[0m[38;2;230;235;231m@[0m[38;2;232;236;233m@[0m[38;2;216;216;216m%[0m[38;2;164;161;163m*[0m[38;2;134;133;135m+[0m[38;2;171;171;174m*[0m[38;2;216;215;218m%[0m",
    "[38;2;215;215;213m%[0m[38;2;157;155;158m*[0m[38;2;169;164;163m*[0m[38;2;157;146;145m+[0m[38;2;84;81;80m-[0m[38;2;99;113;116m=[0m[38;2;181;195;203m#[0m[38;2;214;215;216m%[0m[38;2;214;215;217m%[0m[38;2;219;219;221m%[0m[38;2;224;223;224m%[0m[38;2;225;225;225m%[0m[38;2;225;225;225m%[0m[38;2;226;226;226m%[0m[38;2;227;227;225m%[0m[38;2;227;227;225m%[0m[38;2;226;227;225m%[0m[38;2;227;229;225m%[0m[38;2;229;232;229m@[0m[38;2;231;233;230m@[0m[38;2;230;233;229m@[0m[38;2;230;233;228m@[0m[38;2;231;233;228m@[0m[38;2;230;233;229m@[0m[38;2;230;233;230m@[0m[38;2;230;233;230m@[0m[38;2;230;233;230m@[0m[38;2;230;233;230m@[0m[38;2;229;232;228m@[0m[38;2;230;233;228m@[0m[38;2;230;233;228m@[0m[38;2;230;233;228m@[0m[38;2;231;234;229m@[0m[38;2;229;231;226m@[0m[38;2;220;219;218m%[0m[38;2;199;197;196m#[0m[38;2;178;171;171m*[0m[38;2;176;166;168m*[0m[38;2;177;172;178m*[0m[38;2;174;173;180m*[0m[38;2;169;168;171m*[0m[38;2;167;160;160m*[0m[38;2;177;166;168m*[0m[38;2;196;189;187m#[0m[38;2;215;215;211m%[0m[38;2;224;227;223m%[0m[38;2;227;231;227m@[0m[38;2;228;231;228m@[0m[38;2;229;232;229m@[0m[38;2;229;232;228m@[0m[38;2;228;231;227m@[0m[38;2;229;232;228m@[0m[38;2;230;233;229m@[0m[38;2;230;233;230m@[0m[38;2;230;233;230m@[0m[38;2;231;234;231m@[0m[38;2;230;233;230m@[0m[38;2;232;234;231m@[0m[38;2;235;238;235m@[0m[38;2;232;235;232m@[0m[38;2;231;234;231m@[0m[38;2;231;234;231m@[0m[38;2;231;234;231m@[0m[38;2;231;234;231m@[0m[38;2;230;233;230m@[0m[38;2;235;233;232m@[0m[38;2;216;210;211m%[0m[38;2;158;153;155m*[0m[38;2;136;138;140m+[0m[38;2;184;184;186m#[0m",
    "[38;2;166;164;164m*[0m[38;2;162;160;160m*[0m[38;2;173;164;161m*[0m[38;2;97;93;92m-[0m[38;2;65;77;82m:[0m[38;2;144;165;176m*[0m[38;2;211;214;215m%[0m[38;2;215;215;214m%[0m[38;2;224;225;224m%[0m[38;2;231;232;232m@[0m[38;2;231;231;231m@[0m[38;2;227;226;226m%[0m[38;2;223;223;223m%[0m[38;2;225;224;224m%[0m[38;2;227;227;225m%[0m[38;2;226;227;225m%[0m[38;2;226;229;225m%[0m[38;2;229;231;227m@[0m[38;2;229;232;229m@[0m[38;2;230;233;230m@[0m[38;2;229;233;229m@[0m[38;2;229;232;227m@[0m[38;2;230;233;228m@[0m[38;2;229;232;228m@[0m[38;2;229;232;229m@[0m[38;2;229;232;229m@[0m[38;2;230;233;229m@[0m[38;2;230;233;230m@[0m[38;2;229;232;227m@[0m[38;2;229;232;227m@[0m[38;2;230;233;228m@[0m[38;2;228;232;226m@[0m[38;2;219;222;216m%[0m[38;2;192;191;187m#[0m[38;2;145;140;137m+[0m[38;2;114;101;101m=[0m[38;2;122;115;120m=[0m[38;2;148;158;165m*[0m[38;2;176;189;203m#[0m[38;2;191;204;223m#[0m[38;2;193;205;223m%[0m[38;2;178;191;204m#[0m[38;2;164;168;181m*[0m[38;2;145;140;147m+[0m[38;2;143;133;135m+[0m[38;2;172;159;159m*[0m[38;2;207;204;202m%[0m[38;2;223;226;224m%[0m[38;2;227;230;227m%[0m[38;2;228;230;228m@[0m[38;2;230;233;230m@[0m[38;2;231;233;231m@[0m[38;2;231;234;231m@[0m[38;2;231;234;231m@[0m[38;2;232;235;232m@[0m[38;2;231;234;231m@[0m[38;2;231;234;231m@[0m[38;2;231;234;231m@[0m[38;2;230;233;230m@[0m[38;2;231;234;231m@[0m[38;2;231;234;231m@[0m[38;2;231;234;231m@[0m[38;2;231;234;231m@[0m[38;2;231;234;231m@[0m[38;2;231;234;231m@[0m[38;2;230;233;230m@[0m[38;2;234;232;230m@[0m[38;2;201;192;195m#[0m[38;2;145;142;145m+[0m[38;2;144;145;147m+[0m",
    "[38;2;154;153;148m*[0m[38;2;177;173;169m*[0m[38;2;141;134;132m+[0m[38;2;65;64;67m:[0m[38;2;97;116;128m=[0m[38;2;197;209;216m%[0m[38;2;215;219;216m%[0m[38;2;216;218;218m%[0m[38;2;207;207;207m%[0m[38;2;184;178;180m#[0m[38;2;180;177;177m*[0m[38;2;211;211;212m%[0m[38;2;226;227;225m%[0m[38;2;227;228;225m%[0m[38;2;228;230;227m%[0m[38;2;229;232;229m@[0m[38;2;230;233;229m@[0m[38;2;230;233;229m@[0m[38;2;229;232;229m@[0m[38;2;230;233;230m@[0m[38;2;229;232;228m@[0m[38;2;229;232;226m@[0m[38;2;230;233;228m@[0m[38;2;229;231;227m@[0m[38;2;229;232;228m@[0m[38;2;230;233;230m@[0m[38;2;230;233;230m@[0m[38;2;229;231;229m@[0m[38;2;227;231;227m@[0m[38;2;230;232;229m@[0m[38;2;227;232;227m@[0m[38;2;209;212;207m%[0m[38;2;170;164;162m*[0m[38;2;120;102;103m=[0m[38;2;96;73;74m-[0m[38;2;125;120;126m=[0m[38;2;160;179;195m*[0m[38;2;191;209;228m%[0m[38;2;209;224;242m%[0m[38;2;216;231;244m%[0m[38;2;216;230;243m%[0m[38;2;214;232;244m%[0m[38;2;212;233;248m%[0m[38;2;186;207;227m#[0m[38;2;138;147;160m+[0m[38;2;118;98;104m=[0m[38;2;150;121;125m+[0m[38;2;189;179;179m#[0m[38;2;216;218;218m%[0m[38;2;229;231;230m@[0m[38;2;229;231;230m@[0m[38;2;230;233;232m@[0m[38;2;231;234;233m@[0m[38;2;231;234;233m@[0m[38;2;231;234;233m@[0m[38;2;231;234;233m@[0m[38;2;231;234;231m@[0m[38;2;231;234;231m@[0m[38;2;231;234;231m@[0m[38;2;231;234;231m@[0m[38;2;231;234;231m@[0m[38;2;231;234;231m@[0m[38;2;231;234;231m@[0m[38;2;231;234;231m@[0m[38;2;231;234;231m@[0m[38;2;230;233;230m@[0m[38;2;232;235;232m@[0m[38;2;226;224;222m%[0m[38;2;170;161;163m*[0m[38;2;142;140;143m+[0m",
    "[38;2;157;156;151m*[0m[38;2;179;165;166m*[0m[38;2;110;104;101m=[0m[38;2;62;76;79m:[0m[38;2;151;175;187m*[0m[38;2;214;219;219m%[0m[38;2;218;222;221m%[0m[38;2;210;201;203m%[0m[38;2;133;125;134m+[0m[38;2;120;134;148m+[0m[38;2;139;142;153m+[0m[38;2;139;136;138m+[0m[38;2;193;202;204m#[0m[38;2;224;226;224m%[0m[38;2;228;231;227m@[0m[38;2;229;231;228m@[0m[38;2;228;231;227m@[0m[38;2;230;232;229m@[0m[38;2;229;232;230m@[0m[38;2;229;232;230m@[0m[38;2;228;231;228m@[0m[38;2;229;232;228m@[0m[38;2;230;233;230m@[0m[38;2;229;232;228m@[0m[38;2;229;232;228m@[0m[38;2;230;233;230m@[0m[38;2;231;234;230m@[0m[38;2;228;231;228m@[0m[38;2;226;229;226m%[0m[38;2;229;231;227m@[0m[38;2;219;221;217m%[0m[38;2;184;181;178m#[0m[38;2;124;110;111m=[0m[38;2;92;68;70m:[0m[38;2;92;70;72m-[0m[38;2;117;107;113m=[0m[38;2;156;167;179m*[0m[38;2;184;199;218m#[0m[38;2;201;216;235m%[0m[38;2;210;225;240m%[0m[38;2;214;229;245m%[0m[38;2;214;230;246m%[0m[38;2;206;227;242m%[0m[38;2;179;203;219m#[0m[38;2;134;141;151m+[0m[38;2;90;83;82m-[0m[38;2;111;82;82m-[0m[38;2;159;136;137m+[0m[38;2;204;201;201m#[0m[38;2;227;229;229m%[0m[38;2;229;231;230m@[0m[38;2;230;232;232m@[0m[38;2;231;233;233m@[0m[38;2;231;234;233m@[0m[38;2;231;234;233m@[0m[38;2;231;234;233m@[0m[38;2;231;234;231m@[0m[38;2;231;234;231m@[0m[38;2;231;234;231m@[0m[38;2;231;234;231m@[0m[38;2;231;232;229m@[0m[38;2;230;233;230m@[0m[38;2;231;235;231m@[0m[38;2;231;234;231m@[0m[38;2;231;234;231m@[0m[38;2;231;234;231m@[0m[38;2;231;234;231m@[0m[38;2;232;234;231m@[0m[38;2;191;184;185m#[0m[38;2;147;143;144m+[0m",
    "[38;2;156;153;150m*[0m[38;2;172;158;160m*[0m[38;2;93;87;91m-[0m[38;2;80;99;110m-[0m[38;2;188;203;210m#[0m[38;2;214;214;216m%[0m[38;2;224;227;230m%[0m[38;2;178;161;164m*[0m[38;2;106;116;131m=[0m[38;2;145;185;205m*[0m[38;2;167;177;188m*[0m[38;2;120;114;118m=[0m[38;2;166;181;192m*[0m[38;2;223;226;226m%[0m[38;2;227;228;224m%[0m[38;2;227;230;226m%[0m[38;2;228;231;227m@[0m[38;2;228;231;228m@[0m[38;2;229;232;229m@[0m[38;2;230;233;230m@[0m[38;2;230;233;230m@[0m[38;2;230;233;230m@[0m[38;2;230;233;230m@[0m[38;2;227;230;228m%[0m[38;2;227;230;226m%[0m[38;2;229;232;228m@[0m[38;2;230;233;230m@[0m[38;2;230;233;229m@[0m[38;2;227;230;226m%[0m[38;2;229;229;225m%[0m[38;2;222;222;217m%[0m[38;2;191;189;185m#[0m[38;2;135;124;123m=[0m[38;2;94;75;77m-[0m[38;2;88;68;70m:[0m[38;2;90;71;73m-[0m[38;2;104;90;95m-[0m[38;2;128;125;134m=[0m[38;2;145;147;159m+[0m[38;2;151;154;166m*[0m[38;2;152;153;167m*[0m[38;2;156;157;170m*[0m[38;2;147;149;162m+[0m[38;2;117;117;129m=[0m[38;2;88;80;82m-[0m[38;2;83;68;60m:[0m[38;2;112;83;80m-[0m[38;2;162;140;140m+[0m[38;2;207;204;203m%[0m[38;2;228;230;229m@[0m[38;2;229;231;230m@[0m[38;2;228;230;229m@[0m[38;2;228;230;228m@[0m[38;2;229;232;230m@[0m[38;2;230;233;229m@[0m[38;2;230;233;229m@[0m[38;2;229;233;229m@[0m[38;2;229;232;229m@[0m[38;2;229;232;229m@[0m[38;2;229;232;228m@[0m[38;2;228;232;228m@[0m[38;2;229;232;229m@[0m[38;2;229;232;229m@[0m[38;2;230;233;230m@[0m[38;2;232;235;232m@[0m[38;2;232;235;232m@[0m[38;2;230;234;230m@[0m[38;2;234;236;233m@[0m[38;2;199;193;193m#[0m[38;2;149;144;145m+[0m",
    "[38;2;151;145;150m+[0m[38;2;173;160;162m*[0m[38;2;106;103;108m=[0m[38;2;108;125;139m=[0m[38;2;206;215;220m%[0m[38;2;215;216;217m%[0m[38;2;221;223;223m%[0m[38;2;149;115;116m=[0m[38;2;80;80;85m-[0m[38;2;108;131;141m=[0m[38;2;107;103;110m=[0m[38;2;65;61;65m:[0m[38;2;147;164;181m*[0m[38;2;222;225;227m%[0m[38;2;225;229;224m%[0m[38;2;227;230;226m%[0m[38;2;228;231;227m@[0m[38;2;228;231;227m@[0m[38;2;228;231;227m@[0m[38;2;229;232;229m@[0m[38;2;229;233;229m@[0m[38;2;230;233;229m@[0m[38;2;230;232;229m@[0m[38;2;226;230;227m%[0m[38;2;227;229;226m%[0m[38;2;228;231;227m@[0m[38;2;230;233;230m@[0m[38;2;230;233;229m@[0m[38;2;227;231;227m@[0m[38;2;228;231;227m@[0m[38;2;232;231;227m@[0m[38;2;217;218;214m%[0m[38;2;181;178;175m#[0m[38;2;128;117;116m=[0m[38;2;94;76;77m-[0m[38;2;86;65;68m:[0m[38;2;101;82;87m-[0m[38;2;131;120;127m=[0m[38;2;150;149;159m+[0m[38;2;150;154;164m*[0m[38;2;149;152;162m+[0m[38;2;155;159;168m*[0m[38;2;149;162;171m*[0m[38;2;131;135;146m+[0m[38;2;110;100;102m=[0m[38;2;113;92;89m-[0m[38;2;151;128;127m+[0m[38;2;194;185;185m#[0m[38;2;223;223;223m%[0m[38;2;229;232;231m@[0m[38;2;228;231;230m@[0m[38;2;226;228;227m%[0m[38;2;225;227;225m%[0m[38;2;224;227;225m%[0m[38;2;224;226;223m%[0m[38;2;223;225;222m%[0m[38;2;224;226;224m%[0m[38;2;226;228;226m%[0m[38;2;226;228;226m%[0m[38;2;227;229;227m%[0m[38;2;227;229;227m%[0m[38;2;227;228;226m%[0m[38;2;225;227;225m%[0m[38;2;225;228;225m%[0m[38;2;227;230;227m%[0m[38;2;229;232;229m@[0m[38;2;229;232;229m@[0m[38;2;233;235;232m@[0m[38;2;198;192;191m#[0m[38;2;146;141;143m+[0m",
    "[38;2;145;139;145m+[0m[38;2;175;164;165m*[0m[38;2;134;134;138m+[0m[38;2;131;149;163m+[0m[38;2;201;206;211m%[0m[38;2;205;206;207m%[0m[38;2;214;216;217m%[0m[38;2;159;133;132m+[0m[38;2;65;50;52m:[0m[38;2;108;118;123m=[0m[38;2;108;111;116m=[0m[38;2;96;97;102m-[0m[38;2;177;184;195m#[0m[38;2;220;223;226m%[0m[38;2;226;227;227m%[0m[38;2;227;228;228m%[0m[38;2;227;229;228m%[0m[38;2;227;230;227m%[0m[38;2;228;231;227m@[0m[38;2;229;232;228m@[0m[38;2;228;230;227m%[0m[38;2;227;229;227m%[0m[38;2;227;228;227m%[0m[38;2;227;229;228m%[0m[38;2;228;231;228m@[0m[38;2;229;232;229m@[0m[38;2;229;232;229m@[0m[38;2;230;233;229m@[0m[38;2;229;232;229m@[0m[38;2;229;232;228m@[0m[38;2;230;232;229m@[0m[38;2;231;233;230m@[0m[38;2;223;225;222m%[0m[38;2;199;199;199m#[0m[38;2;161;158;161m*[0m[38;2;130;121;123m=[0m[38;2;134;120;121m=[0m[38;2;154;147;152m+[0m[38;2;168;174;186m*[0m[38;2;182;192;206m#[0m[38;2;191;203;217m#[0m[38;2;190;202;216m#[0m[38;2;183;195;208m#[0m[38;2;177;184;191m#[0m[38;2;175;173;176m*[0m[38;2;190;186;188m#[0m[38;2;211;210;210m%[0m[38;2;224;224;224m%[0m[38;2;226;227;226m%[0m[38;2;226;227;226m%[0m[38;2;227;227;228m%[0m[38;2;228;227;230m%[0m[38;2;230;228;231m%[0m[38;2;228;226;229m%[0m[38;2;226;220;224m%[0m[38;2;222;216;221m%[0m[38;2;224;215;224m%[0m[38;2;221;212;223m%[0m[38;2;215;203;215m%[0m[38;2;213;200;212m%[0m[38;2;216;203;215m%[0m[38;2;216;205;217m%[0m[38;2;224;212;224m%[0m[38;2;225;218;226m%[0m[38;2;221;217;221m%[0m[38;2;227;225;227m%[0m[38;2;230;229;232m@[0m[38;2;232;231;231m@[0m[38;2;188;184;184m#[0m[38;2;139;137;138m+[0m",
    "[38;2;132;129;133m+[0m[38;2;162;153;154m*[0m[38;2;161;163;165m*[0m[38;2;132;157;170m+[0m[38;2;190;189;203m#[0m[38;2;198;196;200m#[0m[38;2;207;209;211m%[0m[38;2;216;207;209m%[0m[38;2;156;137;141m+[0m[38;2;153;161;169m*[0m[38;2;174;185;193m#[0m[38;2;187;194;201m#[0m[38;2;216;220;224m%[0m[38;2;225;226;228m%[0m[38;2;228;227;228m%[0m[38;2;227;227;227m%[0m[38;2;227;227;227m%[0m[38;2;227;228;227m%[0m[38;2;227;229;227m%[0m[38;2;225;226;226m%[0m[38;2;224;226;226m%[0m[38;2;224;226;228m%[0m[38;2;223;225;227m%[0m[38;2;224;226;227m%[0m[38;2;227;229;229m%[0m[38;2;229;231;230m@[0m[38;2;228;230;229m@[0m[38;2;227;229;228m%[0m[38;2;228;230;229m@[0m[38;2;228;230;228m@[0m[38;2;228;230;228m@[0m[38;2;228;230;228m@[0m[38;2;228;230;229m@[0m[38;2;229;231;230m@[0m[38;2;226;228;228m%[0m[38;2;216;220;219m%[0m[38;2;207;206;207m%[0m[38;2;196;193;194m#[0m[38;2;192;190;192m#[0m[38;2;196;193;195m#[0m[38;2;203;200;203m#[0m[38;2;210;207;211m%[0m[38;2;214;212;215m%[0m[38;2;218;216;220m%[0m[38;2;223;224;226m%[0m[38;2;228;228;231m%[0m[38;2;227;227;230m%[0m[38;2;226;226;227m%[0m[38;2;227;226;227m%[0m[38;2;225;225;226m%[0m[38;2;222;222;223m%[0m[38;2;220;218;221m%[0m[38;2;197;193;197m#[0m[38;2;163;153;159m*[0m[38;2;187;173;181m*[0m[38;2;212;201;209m%[0m[38;2;198;191;199m#[0m[38;2;152;135;145m+[0m[38;2;174;147;160m*[0m[38;2;207;188;203m#[0m[38;2;174;168;176m*[0m[38;2;168;148;155m*[0m[38;2;192;184;200m#[0m[38;2;170;151;165m*[0m[38;2;206;188;197m#[0m[38;2;228;222;227m%[0m[38;2;231;230;233m@[0m[38;2;228;225;225m%[0m[38;2;166;160;162m*[0m[38;2;128;129;130m+[0m",
    "[38;2;142;138;142m+[0m[38;2;131;126;130m+[0m[38;2;174;168;170m*[0m[38;2;162;177;184m*[0m[38;2;182;177;191m#[0m[38;2;159;149;157m*[0m[38;2;152;148;153m+[0m[38;2;188;196;197m#[0m[38;2;215;217;219m%[0m[38;2;203;205;207m%[0m[38;2;204;208;211m%[0m[38;2;209;215;217m%[0m[38;2;212;218;219m%[0m[38;2;218;221;222m%[0m[38;2;221;223;224m%[0m[38;2;226;226;227m%[0m[38;2;225;225;227m%[0m[38;2;220;220;222m%[0m[38;2;219;222;226m%[0m[38;2;208;211;214m%[0m[38;2;178;174;175m*[0m[38;2;161;158;159m*[0m[38;2;185;190;192m#[0m[38;2;208;214;216m%[0m[38;2;217;219;220m%[0m[38;2;219;221;222m%[0m[38;2;219;222;223m%[0m[38;2;221;224;225m%[0m[38;2;225;227;228m%[0m[38;2;228;230;229m@[0m[38;2;227;230;229m%[0m[38;2;226;229;228m%[0m[38;2;226;228;227m%[0m[38;2;226;228;227m%[0m[38;2;228;229;228m%[0m[38;2;228;230;229m@[0m[38;2;231;233;233m@[0m[38;2;233;236;235m@[0m[38;2;233;235;233m@[0m[38;2;231;234;233m@[0m[38;2;230;232;231m@[0m[38;2;231;233;233m@[0m[38;2;231;233;233m@[0m[38;2;229;230;229m@[0m[38;2;227;228;227m%[0m[38;2;227;228;228m%[0m[38;2;229;229;230m@[0m[38;2;228;228;229m%[0m[38;2;228;226;228m%[0m[38;2;224;216;225m%[0m[38;2;211;211;220m%[0m[38;2;175;180;184m#[0m[38;2;132;117;121m=[0m[38;2;146;116;127m=[0m[38;2;196;177;190m#[0m[38;2;194;187;198m#[0m[38;2;147;131;141m+[0m[38;2;144;109;117m=[0m[38;2;198;176;186m#[0m[38;2;186;177;191m#[0m[38;2;136;115;118m=[0m[38;2;171;155;161m*[0m[38;2;148;141;151m+[0m[38;2;160;129;141m+[0m[38;2;214;202;213m%[0m[38;2;227;228;227m%[0m[38;2;236;235;235m@[0m[38;2;203;196;198m#[0m[38;2;134;130;134m+[0m[38;2;123;126;124m=[0m",
    "[38;2;198;199;201m#[0m[38;2;121;122;124m=[0m[38;2;138;132;135m+[0m[38;2;180;181;189m#[0m[38;2;183;186;201m#[0m[38;2;150;138;144m+[0m[38;2;121;113;119m=[0m[38;2;149;156;158m*[0m[38;2;198;206;208m%[0m[38;2;209;215;218m%[0m[38;2;208;214;217m%[0m[38;2;208;214;216m%[0m[38;2;208;215;216m%[0m[38;2;212;217;218m%[0m[38;2;209;207;211m%[0m[38;2;196;195;198m#[0m[38;2;205;208;210m%[0m[38;2;217;220;222m%[0m[38;2;217;221;225m%[0m[38;2;204;209;212m%[0m[38;2;155;143;147m+[0m[38;2;99;82;87m-[0m[38;2;125;125;129m=[0m[38;2;183;191;194m#[0m[38;2;205;210;212m%[0m[38;2;208;209;212m%[0m[38;2;211;212;214m%[0m[38;2;211;212;216m%[0m[38;2;211;214;218m%[0m[38;2;215;218;220m%[0m[38;2;220;223;223m%[0m[38;2;226;228;227m%[0m[38;2;228;230;229m@[0m[38;2;225;228;227m%[0m[38;2;225;228;227m%[0m[38;2;226;228;227m%[0m[38;2;226;228;227m%[0m[38;2;227;229;227m%[0m[38;2;227;230;226m%[0m[38;2;227;229;225m%[0m[38;2;227;229;226m%[0m[38;2;228;231;228m@[0m[38;2;228;231;227m@[0m[38;2;227;229;226m%[0m[38;2;227;229;226m%[0m[38;2;228;230;229m@[0m[38;2;228;230;230m@[0m[38;2;228;229;232m%[0m[38;2;225;224;229m%[0m[38;2;218;211;225m%[0m[38;2;179;174;189m*[0m[38;2;122;105;110m=[0m[38;2;146;110;116m=[0m[38;2;203;179;188m#[0m[38;2;217;206;216m%[0m[38;2;177;170;180m*[0m[38;2;152;125;134m+[0m[38;2;202;174;184m#[0m[38;2;217;204;220m%[0m[38;2;165;153;162m*[0m[38;2;176;148;159m*[0m[38;2;210;191;204m#[0m[38;2;190;168;177m*[0m[38;2;211;194;205m#[0m[38;2;226;226;226m%[0m[38;2;230;230;229m@[0m[38;2;219;214;215m%[0m[38;2;149;146;147m+[0m[38;2;114;113;118m=[0m[38;2;162;167;163m*[0m",
    "[38;2;218;220;221m%[0m[38;2;177;179;180m#[0m[38;2;109;107;110m=[0m[38;2;142;137;142m+[0m[38;2;188;191;197m#[0m[38;2;187;184;190m#[0m[38;2;172;159;168m*[0m[38;2;168;159;167m*[0m[38;2;184;186;190m#[0m[38;2;202;206;209m%[0m[38;2;204;209;212m%[0m[38;2;206;213;215m%[0m[38;2;208;213;216m%[0m[38;2;209;214;216m%[0m[38;2;202;194;198m#[0m[38;2;148;124;129m+[0m[38;2;118;108;110m=[0m[38;2;145;150;151m+[0m[38;2;163;168;170m*[0m[38;2;159;159;162m*[0m[38;2;127;116;121m=[0m[38;2;83;66;67m:[0m[38;2;77;65;65m:[0m[38;2;114;113;116m=[0m[38;2;160;166;172m*[0m[38;2;188;192;196m#[0m[38;2;192;196;199m#[0m[38;2;183;186;189m#[0m[38;2;152;150;154m+[0m[38;2;137;137;141m+[0m[38;2;175;176;179m*[0m[38;2;210;213;213m%[0m[38;2;223;226;226m%[0m[38;2;224;227;226m%[0m[38;2;224;227;226m%[0m[38;2;225;227;227m%[0m[38;2;227;229;229m%[0m[38;2;228;230;229m@[0m[38;2;228;230;228m@[0m[38;2;228;231;228m@[0m[38;2;227;229;226m%[0m[38;2;227;229;226m%[0m[38;2;227;229;226m%[0m[38;2;227;229;226m%[0m[38;2;227;229;226m%[0m[38;2;227;229;227m%[0m[38;2;228;229;228m%[0m[38;2;229;230;231m@[0m[38;2;226;226;231m%[0m[38;2;213;206;217m%[0m[38;2;183;169;179m*[0m[38;2;170;145;154m*[0m[38;2;194;169;181m*[0m[38;2;214;200;212m%[0m[38;2;215;202;215m%[0m[38;2;208;196;208m#[0m[38;2;214;196;208m#[0m[38;2;222;205;219m%[0m[38;2;219;203;219m%[0m[38;2;212;196;212m#[0m[38;2;218;204;218m%[0m[38;2;225;213;223m%[0m[38;2;234;225;235m%[0m[38;2;229;227;232m%[0m[38;2;231;232;233m@[0m[38;2;227;223;226m%[0m[38;2;161;157;157m*[0m[38;2;109;108;111m=[0m[38;2;149;150;153m+[0m[38;2;201;204;205m%[0m",
    "[38;2;209;211;213m%[0m[38;2;216;217;219m%[0m[38;2;172;171;174m*[0m[38;2;110;105;108m=[0m[38;2;138;132;135m+[0m[38;2;193;190;194m#[0m[38;2;194;195;198m#[0m[38;2;188;182;188m#[0m[38;2;196;189;195m#[0m[38;2;192;190;193m#[0m[38;2;194;193;196m#[0m[38;2;201;201;204m#[0m[38;2;203;204;207m%[0m[38;2;202;204;205m%[0m[38;2;208;210;210m%[0m[38;2;207;205;205m%[0m[38;2;177;163;164m*[0m[38;2;146;128;127m+[0m[38;2;122;102;103m=[0m[38;2;86;68;71m:[0m[38;2;88;81;84m-[0m[38;2;127;127;130m=[0m[38;2;147;138;140m+[0m[38;2;126;111;112m=[0m[38;2;106;90;93m-[0m[38;2;105;93;96m-[0m[38;2;108;98;102m-[0m[38;2;106;93;97m-[0m[38;2;92;74;80m-[0m[38;2;106;92;98m-[0m[38;2;163;155;162m*[0m[38;2;203;203;205m%[0m[38;2;217;219;218m%[0m[38;2;220;222;222m%[0m[38;2;220;222;221m%[0m[38;2;221;223;223m%[0m[38;2;224;226;226m%[0m[38;2;226;228;227m%[0m[38;2;227;229;228m%[0m[38;2;228;229;228m%[0m[38;2;225;228;226m%[0m[38;2;226;229;227m%[0m[38;2;227;230;227m%[0m[38;2;227;229;227m%[0m[38;2;226;228;226m%[0m[38;2;226;228;226m%[0m[38;2;227;229;228m%[0m[38;2;229;231;230m@[0m[38;2;230;232;231m@[0m[38;2;227;226;226m%[0m[38;2;225;219;221m%[0m[38;2;222;216;222m%[0m[38;2;213;205;214m%[0m[38;2;204;195;207m#[0m[38;2;209;201;212m%[0m[38;2;213;205;216m%[0m[38;2;217;209;219m%[0m[38;2;220;214;222m%[0m[38;2;220;215;222m%[0m[38;2;224;220;227m%[0m[38;2;227;226;229m%[0m[38;2;228;229;229m%[0m[38;2;227;230;229m%[0m[38;2;230;238;236m@[0m[38;2;222;221;223m%[0m[38;2;157;153;154m*[0m[38;2;106;105;107m=[0m[38;2;141;141;145m+[0m[38;2;194;196;198m#[0m[38;2;200;202;204m#[0m",
    "[38;2;203;205;207m%[0m[38;2;205;207;209m%[0m[38;2;213;216;217m%[0m[38;2;176;176;178m*[0m[38;2;110;107;111m=[0m[38;2;124;119;123m=[0m[38;2;177;175;178m*[0m[38;2;194;194;195m#[0m[38;2;194;194;198m#[0m[38;2;193;191;197m#[0m[38;2;191;189;193m#[0m[38;2;195;195;198m#[0m[38;2;198;198;200m#[0m[38;2;198;199;199m#[0m[38;2;200;201;199m#[0m[38;2;201;206;204m%[0m[38;2;209;212;212m%[0m[38;2;215;214;215m%[0m[38;2;199;191;191m#[0m[38;2;130;107;107m=[0m[38;2;92;75;78m-[0m[38;2;144;149;155m+[0m[38;2;191;204;213m#[0m[38;2;207;214;218m%[0m[38;2;198;196;196m#[0m[38;2;184;177;176m#[0m[38;2;171;164;164m*[0m[38;2;164;156;156m*[0m[38;2;170;159;162m*[0m[38;2;183;173;178m*[0m[38;2;199;193;197m#[0m[38;2;211;208;211m%[0m[38;2;212;212;212m%[0m[38;2;212;214;214m%[0m[38;2;214;216;216m%[0m[38;2;216;218;217m%[0m[38;2;219;221;221m%[0m[38;2;222;223;224m%[0m[38;2;223;225;225m%[0m[38;2;223;225;225m%[0m[38;2;222;224;224m%[0m[38;2;225;228;227m%[0m[38;2;228;230;229m@[0m[38;2;226;228;228m%[0m[38;2;225;228;227m%[0m[38;2;226;228;227m%[0m[38;2;227;229;229m%[0m[38;2;226;229;230m%[0m[38;2;227;229;231m%[0m[38;2;227;231;232m@[0m[38;2;228;231;232m@[0m[38;2;230;232;233m@[0m[38;2;230;230;232m@[0m[38;2;228;228;230m%[0m[38;2;230;230;232m@[0m[38;2;231;232;233m@[0m[38;2;232;233;233m@[0m[38;2;232;235;234m@[0m[38;2;231;234;233m@[0m[38;2;231;233;232m@[0m[38;2;229;231;230m@[0m[38;2;231;233;232m@[0m[38;2;235;237;236m@[0m[38;2;208;205;205m%[0m[38;2;135;132;133m+[0m[38;2;100;100;102m-[0m[38;2;147;149;151m+[0m[38;2;193;195;196m#[0m[38;2;196;199;200m#[0m[38;2;194;196;198m#[0m",
    "[38;2;206;208;210m%[0m[38;2;202;203;205m#[0m[38;2;201;202;204m#[0m[38;2;209;211;213m%[0m[38;2;191;192;193m#[0m[38;2;127;127;129m=[0m[38;2;104;101;105m=[0m[38;2;155;144;145m+[0m[38;2;204;189;175m#[0m[38;2;211;196;183m#[0m[38;2;199;189;186m#[0m[38;2;190;185;188m#[0m[38;2;188;187;190m#[0m[38;2;191;191;193m#[0m[38;2;195;194;195m#[0m[38;2;198;196;196m#[0m[38;2;198;196;196m#[0m[38;2;198;198;197m#[0m[38;2;204;205;204m%[0m[38;2;195;189;189m#[0m[38;2;153;136;136m+[0m[38;2;121;101;103m=[0m[38;2;117;104;110m=[0m[38;2;132;135;142m+[0m[38;2;173;178;184m*[0m[38;2;204;211;214m%[0m[38;2;211;219;221m%[0m[38;2;213;219;220m%[0m[38;2;216;219;221m%[0m[38;2;215;215;217m%[0m[38;2;213;213;215m%[0m[38;2;212;212;215m%[0m[38;2;212;212;214m%[0m[38;2;212;212;214m%[0m[38;2;212;213;214m%[0m[38;2;212;213;213m%[0m[38;2;214;215;215m%[0m[38;2;217;217;220m%[0m[38;2;219;219;221m%[0m[38;2;219;219;221m%[0m[38;2;220;220;222m%[0m[38;2;223;225;225m%[0m[38;2;226;229;228m%[0m[38;2;226;228;227m%[0m[38;2;226;228;227m%[0m[38;2;227;229;228m%[0m[38;2;228;230;231m@[0m[38;2;228;230;231m@[0m[38;2;227;229;230m%[0m[38;2;226;228;229m%[0m[38;2;225;227;229m%[0m[38;2;226;228;229m%[0m[38;2;227;229;231m%[0m[38;2;227;230;231m@[0m[38;2;226;229;229m%[0m[38;2;227;229;229m%[0m[38;2;229;231;231m@[0m[38;2;229;231;231m@[0m[38;2;228;230;231m@[0m[38;2;230;232;233m@[0m[38;2;236;237;238m@[0m[38;2;224;223;224m%[0m[38;2;171;166;169m*[0m[38;2;110;106;107m=[0m[38;2;111;110;110m=[0m[38;2;163;163;164m*[0m[38;2;194;194;195m#[0m[38;2;193;193;194m#[0m[38;2;190;192;194m#[0m[38;2;185;187;188m#[0m",
    "[38;2;218;218;220m%[0m[38;2;213;213;215m%[0m[38;2;203;204;206m%[0m[38;2;199;199;201m#[0m[38;2;203;203;206m%[0m[38;2;204;204;206m%[0m[38;2;159;159;162m*[0m[38;2;104;103;106m=[0m[38;2;113;103;103m=[0m[38;2;169;152;138m*[0m[38;2;213;192;172m#[0m[38;2;216;198;183m#[0m[38;2;203;190;184m#[0m[38;2;193;186;185m#[0m[38;2;189;185;187m#[0m[38;2;187;184;187m#[0m[38;2;190;187;189m#[0m[38;2;194;192;192m#[0m[38;2;194;194;193m#[0m[38;2;196;199;198m#[0m[38;2;201;205;203m%[0m[38;2;198;197;194m#[0m[38;2;186;177;176m#[0m[38;2;173;164;164m*[0m[38;2;173;169;170m*[0m[38;2;187;188;190m#[0m[38;2;197;201;203m#[0m[38;2;199;203;205m#[0m[38;2;202;205;208m%[0m[38;2;205;207;209m%[0m[38;2;205;206;209m%[0m[38;2;209;210;212m%[0m[38;2;211;213;214m%[0m[38;2;212;214;216m%[0m[38;2;213;214;216m%[0m[38;2;212;214;216m%[0m[38;2;213;215;216m%[0m[38;2;214;215;217m%[0m[38;2;216;216;218m%[0m[38;2;218;218;220m%[0m[38;2;219;219;221m%[0m[38;2;222;222;224m%[0m[38;2;222;222;225m%[0m[38;2;221;223;225m%[0m[38;2;221;223;225m%[0m[38;2;222;225;226m%[0m[38;2;224;226;228m%[0m[38;2;226;228;229m%[0m[38;2;226;228;229m%[0m[38;2;225;228;229m%[0m[38;2;225;228;229m%[0m[38;2;226;228;229m%[0m[38;2;226;228;229m%[0m[38;2;225;227;229m%[0m[38;2;226;228;230m%[0m[38;2;221;223;226m%[0m[38;2;220;222;224m%[0m[38;2;224;226;228m%[0m[38;2;230;232;234m@[0m[38;2;220;221;223m%[0m[38;2;174;172;175m*[0m[38;2;116;111;115m=[0m[38;2;102;99;100m-[0m[38;2;141;141;141m+[0m[38;2;184;184;184m#[0m[38;2;192;192;192m#[0m[38;2;185;185;184m#[0m[38;2;182;182;183m#[0m[38;2;182;181;182m#[0m[38;2;186;186;187m#[0m",
    "[38;2;217;216;219m%[0m[38;2;218;218;220m%[0m[38;2;216;215;218m%[0m[38;2;211;210;213m%[0m[38;2;201;201;203m#[0m[38;2;196;195;198m#[0m[38;2;202;202;204m#[0m[38;2;193;194;196m#[0m[38;2;144;146;149m+[0m[38;2;102;101;105m=[0m[38;2;112;105;103m=[0m[38;2;158;144;134m+[0m[38;2;194;173;156m*[0m[38;2;212;190;170m#[0m[38;2;209;190;178m#[0m[38;2;190;178;175m#[0m[38;2;178;172;173m*[0m[38;2;176;172;175m*[0m[38;2;178;176;179m*[0m[38;2;181;179;180m#[0m[38;2;183;181;182m#[0m[38;2;187;186;187m#[0m[38;2;193;193;193m#[0m[38;2;200;199;200m#[0m[38;2;200;199;199m#[0m[38;2;198;195;196m#[0m[38;2;198;196;198m#[0m[38;2;200;199;200m#[0m[38;2;200;200;201m#[0m[38;2;199;199;200m#[0m[38;2;199;200;201m#[0m[38;2;201;203;204m#[0m[38;2;202;203;205m#[0m[38;2;203;204;205m%[0m[38;2;204;204;205m%[0m[38;2;204;205;206m%[0m[38;2;204;205;206m%[0m[38;2;206;205;208m%[0m[38;2;208;207;210m%[0m[38;2;210;209;212m%[0m[38;2;213;212;215m%[0m[38;2;214;214;216m%[0m[38;2;215;215;217m%[0m[38;2;217;216;219m%[0m[38;2;219;218;221m%[0m[38;2;220;221;223m%[0m[38;2;220;222;223m%[0m[38;2;219;221;223m%[0m[38;2;220;222;223m%[0m[38;2;221;222;224m%[0m[38;2;221;222;224m%[0m[38;2;222;223;224m%[0m[38;2;222;223;225m%[0m[38;2;222;223;225m%[0m[38;2;225;227;229m%[0m[38;2;223;225;227m%[0m[38;2;217;217;219m%[0m[38;2;199;196;199m#[0m[38;2;160;155;159m*[0m[38;2;113;107;111m=[0m[38;2;95;90;93m-[0m[38;2;125;124;124m=[0m[38;2;167;167;167m*[0m[38;2;187;186;186m#[0m[38;2;183;183;183m#[0m[38;2;180;180;180m#[0m[38;2;181;181;181m#[0m[38;2;185;185;185m#[0m[38;2;195;195;194m#[0m[38;2;205;205;205m%[0m",
    "[38;2;213;213;215m%[0m[38;2;215;214;217m%[0m[38;2;217;216;219m%[0m[38;2;218;218;221m%[0m[38;2;216;216;218m%[0m[38;2;209;209;209m%[0m[38;2;198;198;199m#[0m[38;2;196;195;196m#[0m[38;2;200;200;200m#[0m[38;2;188;188;189m#[0m[38;2;147;147;150m+[0m[38;2;109;109;114m=[0m[38;2;99;97;100m-[0m[38;2;119;110;107m=[0m[38;2;159;144;133m+[0m[38;2;192;171;154m*[0m[38;2;208;184;170m#[0m[38;2;203;184;179m#[0m[38;2;191;177;176m#[0m[38;2;179;170;173m*[0m[38;2;174;167;172m*[0m[38;2;174;168;172m*[0m[38;2;175;169;172m*[0m[38;2;180;174;176m*[0m[38;2;184;179;181m#[0m[38;2;187;182;184m#[0m[38;2;190;185;186m#[0m[38;2;192;188;189m#[0m[38;2;193;191;191m#[0m[38;2;193;190;191m#[0m[38;2;195;193;193m#[0m[38;2;198;196;197m#[0m[38;2;198;198;199m#[0m[38;2;197;197;199m#[0m[38;2;198;199;200m#[0m[38;2;199;201;202m#[0m[38;2;202;203;205m#[0m[38;2;206;206;207m%[0m[38;2;208;208;209m%[0m[38;2;210;209;210m%[0m[38;2;212;212;213m%[0m[38;2;214;214;214m%[0m[38;2;214;214;214m%[0m[38;2;216;216;216m%[0m[38;2;217;217;218m%[0m[38;2;217;217;217m%[0m[38;2;217;217;219m%[0m[38;2;219;218;221m%[0m[38;2;218;217;220m%[0m[38;2;215;214;216m%[0m[38;2;215;214;217m%[0m[38;2;216;216;217m%[0m[38;2;213;213;214m%[0m[38;2;212;211;212m%[0m[38;2;201;195;197m#[0m[38;2;167;162;164m*[0m[38;2;128;122;127m=[0m[38;2;99;91;96m-[0m[38;2;100;93;98m-[0m[38;2;135;129;133m+[0m[38;2;172;169;171m*[0m[38;2;189;189;189m#[0m[38;2;185;185;185m#[0m[38;2;180;180;180m#[0m[38;2;181;181;181m#[0m[38;2;189;189;189m#[0m[38;2;193;193;193m#[0m[38;2;201;201;201m#[0m[38;2;204;204;204m%[0m[38;2;204;204;204m%[0m",
    "[38;2;213;213;215m%[0m[38;2;215;215;217m%[0m[38;2;217;216;219m%[0m[38;2;216;215;219m%[0m[38;2;216;215;217m%[0m[38;2;217;217;217m%[0m[38;2;215;215;215m%[0m[38;2;207;207;207m%[0m[38;2;197;197;197m#[0m[38;2;191;191;191m#[0m[38;2;191;190;190m#[0m[38;2;188;188;188m#[0m[38;2;167;167;168m*[0m[38;2;128;129;133m+[0m[38;2;95;97;100m-[0m[38;2;91;90;89m-[0m[38;2;111;103;97m=[0m[38;2;148;132;123m+[0m[38;2;178;156;147m*[0m[38;2;193;170;165m*[0m[38;2;200;179;175m#[0m[38;2;198;181;180m#[0m[38;2;192;177;179m#[0m[38;2;186;174;176m*[0m[38;2;181;171;173m*[0m[38;2;178;170;171m*[0m[38;2;176;169;171m*[0m[38;2;176;170;171m*[0m[38;2;176;170;171m*[0m[38;2;176;171;171m*[0m[38;2;182;177;177m#[0m[38;2;187;181;181m#[0m[38;2;187;182;183m#[0m[38;2;190;185;187m#[0m[38;2;194;190;192m#[0m[38;2;195;194;195m#[0m[38;2;196;196;197m#[0m[38;2;198;198;199m#[0m[38;2;202;201;201m#[0m[38;2;203;203;203m#[0m[38;2;206;207;206m%[0m[38;2;209;209;209m%[0m[38;2;209;209;209m%[0m[38;2;210;210;210m%[0m[38;2;212;213;212m%[0m[38;2;213;213;213m%[0m[38;2;214;214;216m%[0m[38;2;215;215;217m%[0m[38;2;213;213;216m%[0m[38;2;205;205;208m%[0m[38;2;191;191;193m#[0m[38;2;171;170;172m*[0m[38;2;144;142;142m+[0m[38;2;110;108;109m=[0m[38;2;90;86;87m-[0m[38;2;90;86;87m-[0m[38;2;113;111;114m=[0m[38;2;146;143;146m+[0m[38;2;170;167;169m*[0m[38;2;179;175;178m*[0m[38;2;181;177;180m#[0m[38;2;161;159;161m*[0m[38;2;163;162;163m*[0m[38;2;180;179;179m#[0m[38;2;189;189;189m#[0m[38;2;172;172;172m*[0m[38;2;196;196;196m#[0m[38;2;199;199;199m#[0m[38;2;198;198;198m#[0m[38;2;204;204;204m%[0m",
    "[38;2;213;213;215m%[0m[38;2;215;215;217m%[0m[38;2;216;215;218m%[0m[38;2;216;215;218m%[0m[38;2;213;213;214m%[0m[38;2;211;211;211m%[0m[38;2;213;213;213m%[0m[38;2;214;214;214m%[0m[38;2;213;213;213m%[0m[38;2;208;208;208m%[0m[38;2;199;199;199m#[0m[38;2;191;191;191m#[0m[38;2;190;189;189m#[0m[38;2;191;190;190m#[0m[38;2;180;180;180m#[0m[38;2;155;155;156m*[0m[38;2;126;127;129m=[0m[38;2;101;103;104m=[0m[38;2;89;89;90m-[0m[38;2;87;82;82m-[0m[38;2;104;95;92m-[0m[38;2;126;113;114m=[0m[38;2;145;130;127m+[0m[38;2;164;148;144m+[0m[38;2;178;162;158m*[0m[38;2;186;169;166m*[0m[38;2;189;173;172m*[0m[38;2;191;178;176m#[0m[38;2;187;177;176m#[0m[38;2;184;176;175m*[0m[38;2;182;174;175m*[0m[38;2;183;177;177m#[0m[38;2;185;179;178m#[0m[38;2;187;181;182m#[0m[38;2;191;186;187m#[0m[38;2;193;189;191m#[0m[38;2;193;189;190m#[0m[38;2;198;194;195m#[0m[38;2;204;200;201m#[0m[38;2;205;203;203m%[0m[38;2;206;205;205m%[0m[38;2;206;206;206m%[0m[38;2;209;208;208m%[0m[38;2;205;203;204m%[0m[38;2;201;197;198m#[0m[38;2;190;186;187m#[0m[38;2;172;167;170m*[0m[38;2;152;147;149m+[0m[38;2;129;124;126m=[0m[38;2;103;100;102m-[0m[38;2;87;84;87m-[0m[38;2;87;86;87m-[0m[38;2;94;93;93m-[0m[38;2;111;110;110m=[0m[38;2;138;137;137m+[0m[38;2;163;163;163m*[0m[38;2;174;173;173m*[0m[38;2;174;172;172m*[0m[38;2;171;169;170m*[0m[38;2;172;170;171m*[0m[38;2;183;178;182m#[0m[38;2;120;115;119m=[0m[38;2;116;114;117m=[0m[38;2;119;119;119m=[0m[38;2;131;131;131m+[0m[38;2;97;97;97m-[0m[38;2;127;127;127m=[0m[38;2;130;130;130m+[0m[38;2;183;183;183m#[0m[38;2;207;207;207m%[0m",
    "[38;2;212;211;214m%[0m[38;2;214;214;216m%[0m[38;2;215;215;217m%[0m[38;2;216;215;218m%[0m[38;2;213;213;214m%[0m[38;2;211;211;211m%[0m[38;2;212;212;212m%[0m[38;2;212;212;212m%[0m[38;2;210;210;210m%[0m[38;2;208;208;208m%[0m[38;2;208;208;208m%[0m[38;2;203;203;203m#[0m[38;2;199;199;199m#[0m[38;2;191;191;191m#[0m[38;2;187;187;187m#[0m[38;2;183;183;183m#[0m[38;2;180;180;180m#[0m[38;2;177;177;176m*[0m[38;2;165;165;165m*[0m[38;2;139;140;141m+[0m[38;2;115;117;118m=[0m[38;2;94;97;98m-[0m[38;2;81;82;82m-[0m[38;2;73;70;72m:[0m[38;2;72;67;70m:[0m[38;2;81;73;74m:[0m[38;2;94;84;84m-[0m[38;2;108;98;95m-[0m[38;2;122;113;108m=[0m[38;2;132;124;119m=[0m[38;2;140;132;128m+[0m[38;2;146;138;136m+[0m[38;2;155;146;145m+[0m[38;2;165;153;153m*[0m[38;2;167;157;156m*[0m[38;2;166;158;157m*[0m[38;2;165;157;157m*[0m[38;2;164;156;155m*[0m[38;2;159;151;150m*[0m[38;2;150;142;142m+[0m[38;2;140;132;133m+[0m[38;2;131;123;124m=[0m[38;2;117;110;110m=[0m[38;2;102;94;95m-[0m[38;2;88;82;82m-[0m[38;2;78;73;73m:[0m[38;2;72;69;68m:[0m[38;2;78;76;74m:[0m[38;2;90;89;87m-[0m[38;2;108;106;104m=[0m[38;2;130;127;128m+[0m[38;2;148;145;146m+[0m[38;2;159;155;156m*[0m[38;2;163;160;160m*[0m[38;2;167;164;165m*[0m[38;2;170;167;168m*[0m[38;2;172;170;171m*[0m[38;2;175;173;173m*[0m[38;2;180;178;179m#[0m[38;2;183;181;181m#[0m[38;2;186;181;185m#[0m[38;2;175;170;174m*[0m[38;2;169;165;168m*[0m[38;2;167;167;167m*[0m[38;2;160;160;160m*[0m[38;2;172;172;172m*[0m[38;2;169;169;169m*[0m[38;2;171;171;171m*[0m[38;2;196;196;196m#[0m[38;2;202;202;202m#[0m",
    };
    

    int rows = sizeof(art) / sizeof(art[0]);

    // (Glitch Effect)
    for (int i = 0; i < rows; i++) {
        // % included!!!!
        printf("    " COL_CYAN "%s" COL_RESET "\n", art[i]); 
        
        usleep(20000); 
    }

    printf("\n");
    printf("    >> ");
    type_text("System Shutdown Sequence Initiated...", 30);
    sleep_ms(500);
    printf(COL_GREEN " [DONE]" COL_RESET "\n");
    
    printf("    >> ");
    type_text("Session Log Saved.", 30);
    printf("\n\n    ");
    
    // Goodbye
    type_text(COL_RED "GOODBYE." COL_RESET, 100);
    
    sleep_ms(800);
    printf("\n\n");
}