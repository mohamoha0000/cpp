#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>

DWORD GetThreadIdFromProcess(DWORD pid) {
    THREADENTRY32 te32 = { sizeof(THREADENTRY32) };
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

    if (Thread32First(hSnapshot, &te32)) {
        do {
            if (te32.th32OwnerProcessID == pid) {
                CloseHandle(hSnapshot);
                return te32.th32ThreadID;
            }
        } while (Thread32Next(hSnapshot, &te32));
    }

    CloseHandle(hSnapshot);
    return 0;
}

int main() {
    DWORD pid;
    DWORD_PTR address;

    printf("Enter target process PID: ");
    scanf("%lu", &pid);

    printf("Enter memory address to monitor (hex): ");
    scanf("%p", &address);

    // Attach debugger
    if (!DebugActiveProcess(pid)) {
        printf("❌ Failed to attach to target process.\n");
        return 1;
    }

    // Get a thread ID
    DWORD tid = GetThreadIdFromProcess(pid);
    if (!tid) {
        printf("❌ Failed to get thread ID.\n");
        return 1;
    }

    HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, tid);
    if (!hThread) {
        printf("❌ Failed to open thread.\n");
        return 1;
    }

    // Setup hardware breakpoint
    CONTEXT ctx = { 0 };
    ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;

    if (!GetThreadContext(hThread, &ctx)) {
        printf("❌ Failed to get thread context.\n");
        return 1;
    }

    ctx.Dr0 = (DWORD_PTR)address;
    ctx.Dr7 = 0x00000001 | (3 << 16);  // Watch read/write access, 4 bytes

    if (!SetThreadContext(hThread, &ctx)) {
        printf("❌ Failed to set hardware breakpoint.\n");
        return 1;
    }

    CloseHandle(hThread);

    printf("✅ Breakpoint set on address 0x%p\n", (void*)address);
    printf("📡 Listening for access attempts...\n");

    DEBUG_EVENT dbgEvent;

    // Debug loop
    while (1) {
        if (WaitForDebugEvent(&dbgEvent, INFINITE)) {
            if (dbgEvent.dwDebugEventCode == EXCEPTION_DEBUG_EVENT &&
                dbgEvent.u.Exception.ExceptionRecord.ExceptionCode == EXCEPTION_SINGLE_STEP) {

                // Print the address of the instruction that accessed the memory
                printf("🔍 Access detected at instruction: %p\n",
                       dbgEvent.u.Exception.ExceptionRecord.ExceptionAddress);
            }

            ContinueDebugEvent(dbgEvent.dwProcessId, dbgEvent.dwThreadId, DBG_CONTINUE);
        }
    }

    return 0;
}
