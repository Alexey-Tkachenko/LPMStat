/*

 SD - a slightly more friendly wrapper for sdfatlib

 This library aims to expose a subset of SD card functionality
 in the form of a higher level "wrapper" object.

 License: GNU General Public License V3
          (Because sdfatlib is licensed with this.)

 (C) Copyright 2010 SparkFun Electronics

 */

#ifndef __SD_H__
#define __SD_H__

#include <Arduino.h>

#include "utility/SdFat.h"
#include "utility/SdFatUtil.h"

#define FILE_READ O_READ
#define FILE_WRITE (O_READ | O_WRITE | O_CREAT)

namespace SDLib {

class SDX_File : public Stream {
 private:
  char _name[13]; // our name
  SdFile _file;  // underlying file
  bool _fileValid;

public:
  SDX_File(const SdFile &f, const char *name);     // wraps an underlying SdFile
  SDX_File(void);      // 'empty' constructor
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *buf, size_t size);
  virtual int read();
  virtual int peek();
  virtual int available();
  virtual void flush();
  int read(void *buf, uint16_t nbyte);
  boolean seek(uint32_t pos);
  uint32_t position();
  uint32_t size();
  void close();
  operator bool();
  char * name();

  boolean isDirectory(void);
  SDX_File openNextFile(uint8_t mode = O_RDONLY);
  void rewindDirectory(void);
  
  using Print::write;
};

class SDX_SDClass {

private:
  // These are required for initialisation and use of sdfatlib
  Sd2Card card;
  SdVolume volume;
  SdFile root;
  
  // my quick&dirty iterator, should be replaced
  SdFile getParentDir(const char *filepath, int *indx);
public:
  // This needs to be called to set up the connection to the SD card
  // before other methods are used.
  boolean begin(uint8_t csPin = SD_CHIP_SELECT_PIN);
  boolean begin(uint32_t clock, uint8_t csPin);
  
  // Open the specified file/directory with the supplied mode (e.g. read or
  // write, etc). Returns a SDX_File object for interacting with the file.
  // Note that currently only one file can be open at a time.
  SDX_File open(const char *filename, uint8_t mode = FILE_READ);
  SDX_File open(const String &filename, uint8_t mode = FILE_READ) { return open( filename.c_str(), mode ); }

  // Methods to determine if the requested file path exists.
  boolean exists(const char *filepath);
  boolean exists(const String &filepath) { return exists(filepath.c_str()); }

  // Create the requested directory heirarchy--if intermediate directories
  // do not exist they will be created.
  boolean mkdir(const char *filepath);
  boolean mkdir(const String &filepath) { return mkdir(filepath.c_str()); }
  
  // Delete the file.
  boolean remove(const char *filepath);
  boolean remove(const String &filepath) { return remove(filepath.c_str()); }
  
  boolean rmdir(const char *filepath);
  boolean rmdir(const String &filepath) { return rmdir(filepath.c_str()); }

private:

  // This is used to determine the mode used to open a file
  // it's here because it's the easiest place to pass the 
  // information through the directory walking function. But
  // it's probably not the best place for it.
  // It shouldn't be set directly--it is set via the parameters to `open`.
  int fileOpenMode;
  
  friend class SDX_File;
  friend boolean callback_openPath(SdFile&, const char *, boolean, void *); 
};

extern SDX_SDClass SDX;

};

// We enclose SDX_File and SD classes in namespace SDLib to avoid conflicts
// with others legacy libraries that redefines SDX_File class.

// This ensure compatibility with sketches that uses only SD library
using namespace SDLib;

// This allows sketches to use SDLib::SDX_File with other libraries (in the
// sketch you must use SDFile instead of SDX_File to disambiguate)
typedef SDLib::SDX_File    SDFile;
typedef SDLib::SDX_SDClass SDFileSystemClass;
#define SDFileSystem   SDLib::SD

#endif
