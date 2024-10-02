#include <iostream>
#include <Windows.h>
using namespace std;

// Helper functions
void error(const char* error_title, const char* error_message) {
    MessageBox(NULL, error_message, error_title, NULL);
    exit(-1); /*Closes the program with a code of negative 1*/
}

int main()
{
    string gameWindowTitle;
    cout << "Input the window title of your game: \n";
    cin >> gameWindowTitle;
    cout << "\n Input the path of the .dll file you want to inject: \n";
    string dllPath;
    cin >> dllPath;
    cout << "\n";
    DWORD procId = NULL;
    GetWindowThreadProcessId(FindWindow(NULL, (LPCSTR)gameWindowTitle.c_str()), &procId);
    if (procId == NULL) { error("getProcId", "Failed to get process id"); }
    HANDLE procHandle = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);
    if (!procHandle) { error("OpenProcess", "Failed to open a handle to process"); }
    void* allocatedMem = VirtualAllocEx(procHandle, nullptr, MAX_PATH, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!allocatedMem) { error("VirtualAllocex", "Failed to allocate memory"); }
    if (!WriteProcessMemory(procHandle, allocatedMem, (LPSTR)dllPath.c_str(), MAX_PATH, nullptr)) {
        error("WriteProcessMemory", "Write process memory failed to process memory");
    }
    HANDLE h_thread = CreateRemoteThread(procHandle, nullptr, NULL, LPTHREAD_START_ROUTINE(LoadLibraryA), allocatedMem, NULL, nullptr);
    if (!h_thread) {
        error("CreateRemoteThread", "Failed to create remote thread");
    }
    CloseHandle(procHandle);
    VirtualFreeEx(procHandle, allocatedMem, NULL, MEM_RELEASE);
}