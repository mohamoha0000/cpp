#include <stdio.h>
#include <windows.h>

int main() {
    // Declare the variable with value 689
    int x = 689;
    
    // Get and print the Process ID (PID)
    DWORD pid = GetCurrentProcessId();
    printf("PID: %u\n", pid);
    
    // Print the address of x
    printf("Address of x: %p\n", (void*)&x);
    while (1)
    {
        printf("Press Enter to exit...\n");
        getchar();
        if(x==55){
            printf("good");
        }
        printf("%d",x);
    }
    
    // Keep the program running until the user presses Enter
    printf("Press Enter to exit...\n");
    getchar();
    
    return 0;
}