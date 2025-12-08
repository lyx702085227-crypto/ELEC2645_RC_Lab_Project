#include "rc_tool.h"

int main() {
    // [Data Structure] Core struct holding all simulation parameters and state.
    // Passed by reference to all modules to maintain state consistency.
    CircuitParams circuit = {0};
    char input_buffer[50];
    int running = 1;

    print_splash_screen(); // startup animation

    // --- Main Application Loop ---
    while (running) {
        // [UI] Render the Dashboard Interface (Real-time status display)
        print_menu(&circuit);
        
        // [UX] Prompt for user input (supports fuzzy commands & smart units)
        printf(THEME_PROMPT "\n Command (e.g. 'run', 'set 1k 10u 5') > " COL_RESET);

        // [Input] Robust input reading using fgets (prevents buffer overflow)
        if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) continue;
        input_buffer[strcspn(input_buffer, "\n")] = 0; 

        // [Parsing] Logic to split input into Command and Arguments.
        // Enables "One-Line Command" feature (e.g., cmd="setup", args="1k 10u 5")
        char *cmd = input_buffer;
        char *args = NULL;
        char *sp = strchr(input_buffer, ' ');
        if (sp) { *sp = 0; args = sp+1; while(*args==' ') args++; }

        if (strlen(cmd) == 0) continue;
        // [Algorithm] Parse command using Levenshtein Distance (Fuzzy Logic).
        // This allows typos like "settup" or "cnfig" to be correctly identified.
        int choice = parse_menu_command(cmd);

        // [Routing] Dispatch logic to specific functional modules
        switch (choice) {
            case 1: setup_parameters(&circuit, args); break;
            case 2: run_simulation(&circuit); break; 
            case 3: plot_results(&circuit); break;   
            case 4: plot_results_vertical(&circuit); break;
            case 5: ai_assistant_menu(&circuit); break;
            case 6: save_data_to_csv(&circuit); break;
            case 7: 
                // ending animation
                print_exit_screen(); 
                running = 0; 
                break;
            default: 
                printf(THEME_ERROR "\n [!] Unknown command. Try 'help' or number.\n" COL_RESET);
                sleep_ms(800); 
                break;
        }
    }

    if (circuit.data_points) free(circuit.data_points);
    return 0;
}