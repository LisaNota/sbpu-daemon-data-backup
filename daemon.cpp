#include <cstdlib>
#include <cstdio>
#include <sys/types.h>
#include <unistd.h>
#include <csignal>
#include <cerrno>
#include <string>
#include <chrono>
#include <thread>
#include <fstream>
#include <iostream>
#include <vector>
#include <syslog.h>
#include <map>
#include <cstring>
using namespace std;
map<string, string> config;

map<string, string> readIniFile(const string& filePath) {
        ifstream file(filePath);

        if (!file.is_open()) {
                syslog(LOG_ERR, "Failed to open config file: %s", filePath.c_str());
                return config;
        }

        string line;
        while (getline(file, line)) {
                size_t equalsPos = line.find('=');
                if (equalsPos != string::npos) {
                        string key = line.substr(0, equalsPos);
                        string value = line.substr(equalsPos + 1);
                        config[key] = value;
                }
        }
}

void initialize() {
        openlog("BackupDaemon", LOG_PID, LOG_DAEMON);
        readConfig(configPath);
        if (config.empty()) {
                syslog(LOG_ERR, "Empty or invalid configuration file: %s", configPath.c_str());
                closelog();
                exit(1);
        }
}

int createBackup(const string& sourceDir, const string& backupDir) {
        openlog("BackupDaemon", LOG_PID, LOG_DAEMON);
        time_t now = time(nullptr);
        char timestamp[20];
        strftime(timestamp, sizeof(timestamp), "%Y%m%d%H%M%S", localtime(&now));

        string rsyncCmd = "rsync -av --exclude=/dev --exclude=/proc --exclude=/sys --exclude=/mnt --exclude=/lost+found " + sourceDir + "/ " + backupDir + "/";

        if (system(rsyncCmd.c_str()) != 0) {
                syslog(LOG_ERR, "Error running rsync: %s", strerror(errno));
                return 1;
        }

        syslog(LOG_INFO, "Backup completed successfully.");
        return 0;
}
void daemonLoop(const string& configPath) {
        openlog("BackupDaemon", LOG_PID, LOG_DAEMON);
        while (true) {
                ifstream configFile(configPath);
                if (!configFile.is_open()) {
                    syslog(LOG_ERR, "Failed to open the config file.");
                    closelog();
                    exit(1);
                }
                string sourceDir, backupDir;
                int backupFrequencyMinutes;


                auto nextBackupTime = chrono::system_clock::now() + chrono::minutes(backupFrequencyMinutes);

                this_thread::sleep_until(nextBackupTime);

                int result = createBackup(sourceDir, backupDir);
                if (result == 0) {
                    syslog(LOG_INFO, "Backup completed successfully.");
                }
                else {
                        syslog(LOG_ERR, "Backup failed.");
                }
        }
        closelog();
}

string pidFilePath = "daemon.pid";
void savePIDToFile() {
        ofstream pidFile(pidFilePath);
        if (pidFile.is_open()) {
                pidFile << getpid();
                pidFile.close();
        }
}
int readPIDFromFile() {
        int pid = -1;
        ifstream pidFile(pidFilePath);
        if (pidFile.is_open()) {
                pidFile >> pid;
                pidFile.close();
        }
        return pid;
}

void stopDaemon() {
        int pid = readPIDFromFile();
        if (pid != -1) {
                if (kill(pid, SIGTERM) == 0) {
                        syslog(LOG_INFO, "Daemon stopped.");
                        isDaemonRunning = false;
                }
                else {
                        syslog(LOG_ERR, "Failed to stop the daemon: %s", strerror(errno));
                }}
        else {
                syslog(LOG_INFO, "Daemon is not running");
        }
}

void getStatus() {
        if (isDaemonRunning) {
                syslog(LOG_INFO, "Daemon is running.");
        }
        else {
                syslog(LOG_INFO, "Daemon is not running.");
        }
}

void changeConfig(const string& newConfigPath) {
        configPath = newConfigPath;
        syslog(LOG_INFO, "Configuration updated with new path: %s", newConfigPath.c_str());
}

void startDaemon() {
        daemonLoop(configPath);
        isDaemonRunning = true;
        syslog(LOG_INFO, "Daemon started.");
}

int main(int argc, char* argv[]) {
    openlog("BackupDaemon", LOG_PID, LOG_DAEMON);


    initialize();
    if (argc < 2) {
            syslog(LOG_ERR, "Usage: %s <command>", argv[0]);
            closelog();
            return 1;
    }

    string command = argv[1];

    if (command == "start") {
            startDaemon();
    }
    else if (command == "stop") {
            stopDaemon();
    }
    else if (command == "restart") {
            restartDaemon();
    }
    else if (command == "status") {
            getStatus();
    }
    else if (command == "config") {
            if (argc != 3) {
                    syslog(LOG_ERR, "Usage: %s config <path_to_config>", argv[0]);
                    closelog();
                    return 1;
            }
            changeConfig(argv[2]);
    }
    else {
            syslog(LOG_ERR, "Unknown command: %s", command.c_str());
            closelog();
            return 1;
    }

    closelog();
    return 0;
}







