//
// Created by Sebastian on 20.11.22.
//

#ifndef EPAPER_FIRMWARE_FILEHANDLER_H
#define EPAPER_FIRMWARE_FILEHANDLER_H
/* You only need to format SPIFFS the first time you run a
   test or else use the SPIFFS plugin to create a partition
   https://github.com/me−no−dev/arduino−esp32fs−plugin */
#define FORMAT_SPIFFS_IF_FAILED true

#include "FS.h"
#include "SPIFFS.h"

class FileHandler{
public:
    FileHandler();
    void readFile(FS &fs, const char *path);
    void writeFile(FS &fs, const char *path, const char *message);
    void listDir(FS &fs, const char *dirname, uint8_t levels);
    void appendFile(FS &fs, const char *path, const char *message);
    void renameFile(FS &fs, const char *path1, const char *path2);
    void deleteFile(FS &fs, const char *path);
};

#endif EPAPER_FIRMWARE_FILEHANDLER_H
