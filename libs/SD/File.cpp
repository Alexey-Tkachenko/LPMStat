/*

 SD - a slightly more friendly wrapper for sdfatlib

 This library aims to expose a subset of SD card functionality
 in the form of a higher level "wrapper" object.

 License: GNU General Public License V3
          (Because sdfatlib is licensed with this.)

 (C) Copyright 2010 SparkFun Electronics

 */

#include "SD.h"

/* for debugging file open/close leaks
   uint8_t nfilecount=0;
*/

SDX_File::SDX_File(const SdFile &f, const char *n) : _name(), _file(f), _fileValid(true) {
  // oh man you are kidding me, new() doesnt exist? Ok we do it by hand!
  strncpy(_name, n, 12);
  _name[12] = 0;
}

SDX_File::SDX_File(void) :  _name(), _file(), _fileValid() {
}

// returns a pointer to the file name
char *SDX_File::name(void) {
  return _name;
}

// a directory is a special type of file
boolean SDX_File::isDirectory(void) {
  return (_fileValid && _file.isDir());
}


size_t SDX_File::write(uint8_t val) {
  return write(&val, 1);
}

size_t SDX_File::write(const uint8_t *buf, size_t size) {
  size_t t;
  if (!_fileValid) {
    setWriteError();
    return 0;
  }
  _file.clearWriteError();
  t = _file.write(buf, size);
  if (_file.getWriteError()) {
    setWriteError();
    return 0;
  }
  return t;
}

int SDX_File::peek() {
  if (! _fileValid) 
    return 0;

  int c = _file.read();
  if (c != -1) _file.seekCur(-1);
  return c;
}

int SDX_File::read() {
  if (_fileValid) 
    return _file.read();
  return -1;
}

// buffered read for more efficient, high speed reading
int SDX_File::read(void *buf, uint16_t nbyte) {
  if (_fileValid) 
    return _file.read(buf, nbyte);
  return 0;
}

int SDX_File::available() {
  if (! _fileValid) return 0;

  uint32_t n = size() - position();

  return n > 0X7FFF ? 0X7FFF : n;
}

void SDX_File::flush() {
  if (_fileValid)
    _file.sync();
}

boolean SDX_File::seek(uint32_t pos) {
  if (! _fileValid) return false;

  return _file.seekSet(pos);
}

uint32_t SDX_File::position() {
  if (! _fileValid) return -1;
  return _file.curPosition();
}

uint32_t SDX_File::size() {
  if (! _fileValid) return 0;
  return _file.fileSize();
}

void SDX_File::close() {
  if (_fileValid) {
    _file.close();
    _fileValid = 0;
    memset(&_file, 0, sizeof(_file));

    /* for debugging file open/close leaks
    nfilecount--;
    Serial.print("Deleted ");
    Serial.println(nfilecount, DEC);
    */
  }
}

SDX_File::operator bool() {
  if (_fileValid) 
    return  _file.isOpen();
  return false;
}

