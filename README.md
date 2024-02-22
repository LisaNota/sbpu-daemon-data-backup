# Project Objective:
The objective of this project is to create a daemon (system process running in the background) for automatically performing regular backups of data from one directory to another. The task involves the following steps:

1. **Configuration Setup:** Develop a configuration file where the source directory, backup directory, backup frequency, and other parameters can be specified.
2. **Daemon Creation:** Write C++ code for the daemon that will perform the following tasks:
   - Read the configuration from the configuration file.
   - Wait in the background for the next backup time.
   - Create backup copies of files from the source directory to the backup directory with timestamp added.
   - Log the execution of operations in the system log.
3. **System Integration:** Integrate the daemon into the system to run at the operating system startup and regularly perform backups according to the settings.

## Written Code:

### `backup.ini`:

```ini
source_dir = /home/olya/currPath
backup_dir = /home/olya/BackupPath
backup_frequency_minutes = 2
```

### `daemon.cpp`:

```cpp
// Include necessary libraries and headers

// Define global variables and data structures

// Function to read configuration from INI file
map<string, string> readIniFile(const string& filePath) {
    // Implementation details
}
```

Replace `"source_dir"`, `"backup_dir"`, and `"backup_frequency_minutes"` with the actual paths and frequency required for your backup setup.
