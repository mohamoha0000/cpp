#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>

// No longer need GetThreadIdFromProcess, we will loop in main

int main() {
    DWORD pid;
    DWORD_PTR address;

    printf("Enter target process PID: ");
    scanf_s("%lu", &pid);

    printf("Enter memory address to monitor (hex): ");
    scanf_s("%p", &address);

    // Attach debugger
    if (!DebugActiveProcess(pid)) {
        printf("❌ Failed to attach to target process. Error: %lu\n", GetLastError());
        return 1;
    }

    printf("✅ Attached to process %lu\n", pid);

    // --- Loop through ALL threads and set the breakpoint ---
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        printf("❌ Failed to create thread snapshot.\n");
        DebugActiveProcessStop(pid);
        return 1;
    }

    THREADENTRY32 te32 = { sizeof(THREADENTRY32) };

    if (Thread32First(hSnapshot, &te32)) {
        do {
            if (te32.th32OwnerProcessID == pid) {
                HANDLE hThread = OpenThread(THREAD_SET_CONTEXT | THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION, FALSE, te32.th32ThreadID);
                if (hThread) {
                    CONTEXT ctx = { 0 };
                    ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;

                    if (GetThreadContext(hThread, &ctx)) {
                        ctx.Dr0 = (DWORD_PTR)address;
                        // Enable Dr0 (L0=1), for Read/Write (RW0=3), 4-byte length (LEN0=3)
                        ctx.Dr7 = (1 << 0) | (3 << 16) | (3 << 18); 

                        if(!SetThreadContext(hThread, &ctx)) {
                            // This might fail for some protected threads, which is okay.
                        } else {
                           printf("🔧 Breakpoint set for thread %lu\n", te32.th32ThreadID);
                        }
                    }
                    CloseHandle(hThread);
                }
            }
        } while (Thread32Next(hSnapshot, &te32));
    }
    CloseHandle(hSnapshot);
    // --------------------------------------------------------

    printf("\n✅ Breakpoints set on address 0x%p\n", (void*)address);
    printf("📡 Listening for access attempts...\n");

    DEBUG_EVENT dbgEvent;
    
    // Debug loop
    while (1) {
        if (WaitForDebugEvent(&dbgEvent, INFINITE)) {
            if (dbgEvent.dwDebugEventCode == EXCEPTION_DEBUG_EVENT &&
                dbgEvent.u.Exception.ExceptionRecord.ExceptionCode == EXCEPTION_SINGLE_STEP) {

                // Access Detected!
                printf("🔍 Access detected by thread %lu at instruction: %p\n",
                       dbgEvent.dwThreadId,
                       dbgEvent.u.Exception.ExceptionRecord.ExceptionAddress);
            }

            ContinueDebugEvent(dbgEvent.dwProcessId, dbgEvent.dwThreadId, DBG_CONTINUE);
        }
    }

    DebugActiveProcessStop(pid);
    return 0;
}