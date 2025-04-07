#include <stdio.h>
#include <windows.h>

int main() {
    DWORD pid;
    char addr_str[20];
    int n;
    // Get the target process ID from the user
    printf("Enter the PID of the target process: ");
    scanf("%u", &pid);
    
    // Get the memory address from the user
    printf("Enter the memory address (in hex, e.g., 0x12345678): ");
    scanf("%19s", addr_str);
    
    // Convert the address string to a pointer
    void* addr;
    sscanf(addr_str, "%p", &addr);
    
    // Open the target process with read access
    HANDLE hProcess = OpenProcess(PROCESS_VM_READ, FALSE, pid);
    if (hProcess == NULL) {
        printf("Failed to open process. Error code: %u\n", GetLastError());
        return 1;
    }
    
    // Read the memory at the specified address
    int value;
    SIZE_T bytesRead;
    if (ReadProcessMemory(hProcess, addr, &value, sizeof(int), &bytesRead)) {
        if (bytesRead == sizeof(int)) {
            printf("Value at address %p: %d\n", addr, value);
        } else {
            printf("Failed to read the full value.\n");
        }
    } else {
        printf("ReadProcessMemory failed. Error code: %u\n", GetLastError());
    }
    
    // Clean up by closing the process handle
    CloseHandle(hProcess);
    scanf("%d",&n);
    return 0;
}