#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;

void executeTaskMenu(int taskNumber, const string& taskName, const string& directoryPath) {
    int action = -1;

    while (action != 0) {
        cout << "\n----------------------------------------\n";
        cout << "   Task 0" << taskNumber << " - " << taskName << "\n";
        cout << "----------------------------------------\n";
        cout << "1. Build\n";
        cout << "2. Quick correctness check (small sizes)\n";
        cout << "3. Full benchmark (writes output/results.md)\n";
        cout << "4. Clean build files\n";
        cout << "0. Return to Main Menu\n";
        cout << "Select an action: ";
        if (!(cin >> action)) return;

        string baseCommand = "make -C " + directoryPath;

        switch (action) {
            case 1: system(baseCommand.c_str()); break;
            case 2: system((baseCommand + " check").c_str()); break;
            case 3: system((baseCommand + " bench").c_str()); break;
            case 4: system((baseCommand + " clean").c_str()); break;
            case 0: cout << "Returning to main menu...\n"; break;
            default: cout << "Error: Invalid selection. Please try again.\n";
        }
    }
}

int main() {
    int mainChoice = -1;

    while (mainChoice != 0) {
        cout << "\n========================================\n";
        cout << "  CS527 Lab: OpenMP Wrapper (Assignment 02) \n";
        cout << "========================================\n";
        cout << "1. Array Search (Task 01)\n";
        cout << "2. Array Sorting (Task 02)\n";
        cout << "3. Histogram Calculation (Task 03)\n";
        cout << "4. Matrix Transpose (Task 04)\n";
        cout << "5. 2D Convolution (Task 05)\n";
        cout << "0. Exit Program\n";
        cout << "\nEnter your choice: ";
        if (!(cin >> mainChoice)) return 0;

        switch (mainChoice) {
            case 1: executeTaskMenu(1, "Array Search", "Task_01"); break;
            case 2: executeTaskMenu(2, "Array Sorting", "Task_02"); break;
            case 3: executeTaskMenu(3, "Histogram Calculation", "Task_03"); break;
            case 4: executeTaskMenu(4, "Matrix Transpose", "Task_04"); break;
            case 5: executeTaskMenu(5, "2D Convolution", "Task_05"); break;
            case 0: cout << "\nTerminating Wrapper Environment. Goodbye!\n"; break;
            default: cout << "\nError: Unrecognized choice.\n";
        }
    }
    return 0;
}
