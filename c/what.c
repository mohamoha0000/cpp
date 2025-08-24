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
    printf("pid: ");
    scanf("%lu", &pid);

    // اربط نفسك كمصّحح مع العملية
    if (!DebugActiveProcess(pid)) {
        printf("❌ فشل ربط العملية للتصحيح.\n");
        return 1;
    }

    // الحصول على Thread ID من PID
    DWORD tid = GetThreadIdFromProcess(pid);
    if (!tid) {
        printf("❌ لم يتم العثور على Thread.\n");
        return 1;
    }

    HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, tid);
    if (!hThread) {
        printf("❌ فشل في فتح الـ Thread.\n");
        return 1;
    }

    CONTEXT ctx;
    ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;

    if (!GetThreadContext(hThread, &ctx)) {
        printf("❌ فشل الحصول على السياق.\n");
        return 1;
    }

    DWORD_PTR address;
    printf("📍 أدخل عنوان الذاكرة (hex): ");
    scanf("%p", &address);

    // وضع Breakpoint على DR0
    ctx.Dr0 = (DWORD_PTR)address;
    ctx.Dr7 = 0x00000001 | (3 << 16);  // مراقبة قراءة/كتابة، حجم 4 بايت

    if (!SetThreadContext(hThread, &ctx)) {
        printf("❌ فشل تعيين Breakpoint.\n");
        return 1;
    }

    CloseHandle(hThread);

    printf("✅ تم تثبيت Breakpoint على العنوان 0x%p\n", (void*)address);
    printf("📡 جاري التتبع...\n");

    // حلقة الانتظار والتجسس
    DEBUG_EVENT dbgEvent;
    while (1) {
        if (WaitForDebugEvent(&dbgEvent, INFINITE)) {
            if (dbgEvent.dwDebugEventCode == EXCEPTION_DEBUG_EVENT &&
                dbgEvent.u.Exception.ExceptionRecord.ExceptionCode == EXCEPTION_SINGLE_STEP) {

                // طباعة عنوان التعليمة التي وصلت إلى العنوان المراقب
                printf("🔍 An access attempt was made at the instruction address: %p\n", 
                    dbgEvent.u.Exception.ExceptionRecord.ExceptionAddress);
            }

        ContinueDebugEvent(dbgEvent.dwProcessId, dbgEvent.dwThreadId, DBG_CONTINUE);
        }
    }

    return 0;
}
