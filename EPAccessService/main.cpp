#include <QcoreApplication>
#include <QTime>
#include <QDate>
#include <QFile>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include "qservice.h"

#define SLEEP_TIME 5000

SERVICE_STATUS serviceStatus;
SERVICE_STATUS_HANDLE hStatus;

void ServiceMain(int argc, char** argv);
void ControlHandler(DWORD request);

static const QtMessageHandler QT_DEFAULT_MESSAGE_HANDLER = qInstallMessageHandler(0);
bool logToFile = false;

void myCustomMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QHash<QtMsgType, QString> msgLevelHash({
                                            {QtDebugMsg, "(Debug)   "},
                                            {QtInfoMsg, "(Info)    "},
                                            {QtWarningMsg, "(Warning) "},
                                            {QtCriticalMsg, "(Critical)"},
                                            {QtFatalMsg, "(Fatal)   "}
                                            });
    QByteArray localMsg = msg.toLocal8Bit();
    QTime time = QTime::currentTime();
    QDate date = QDate::currentDate();
    QString formattedTime = time.toString("hh:mm:ss");
    QString formattedDate = date.toString("dd.MM.yyyy");
    QByteArray formattedTimeMsg = formattedTime.toLocal8Bit();
    QString logLevelName = msgLevelHash[type];
    QByteArray logLevelMsg = logLevelName.toLocal8Bit();

    if (logToFile) {
        QString txt = QString("%1 %2 %3 :: %4").arg(formattedDate, formattedTime, logLevelName, msg);
        QFile outFile(QCoreApplication::applicationDirPath() + "/access.log");

        if (outFile.size() > 1073741824) outFile.remove();
        outFile.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream ts(&outFile);
        ts << txt << Qt::endl;
        outFile.close();
    } else {
        fprintf(stderr, "%s %s %s :: %s)\n",
                (char*)formattedDate.constData(),
                formattedTimeMsg.constData(),
                logLevelMsg.constData(),
                localMsg.constData());
        fflush(stderr);
    }

    if (type == QtFatalMsg)
        abort();

    // Call the default handler.
    (*QT_DEFAULT_MESSAGE_HANDLER)(type, context, msg);
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
    //RunAgent();

    serviceStatus.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus (hStatus, &serviceStatus);

    /*while (serviceStatus.dwCurrentState == SERVICE_RUNNING) {
        Sleep(SLEEP_TIME);
        serviceStatus.dwCurrentState = SERVICE_STOP;
        QString txt = "12345";
        QFile outFile("C:\\files\\debug.log");
        outFile.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream ts(&outFile);
        ts << txt << Qt::endl;
        outFile.close();
    }
    return;*/

    QByteArray envVar = qgetenv("QTDIR");
    if (envVar.isEmpty()) logToFile = true;
    qInstallMessageHandler(myCustomMessageHandler);
    QCoreApplication a(argc, argv);
    QService service = QService(a.applicationDirPath() + "/settings.ini");
    qInfo() << "= = = = = = = = = = = = = = START SERVICE = = = = = = = = = = = = = = =";
    a.exec();
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
        break;
    }
    SetServiceStatus(hStatus,  &serviceStatus);
    return;
}
