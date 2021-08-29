#pragma once

#include <Arduino.h>
#include <FS.h>

#define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
const char empty[] = "";

class MemFile : public File
{
public:

  virtual size_t write(const void *buf, size_t nbyte) {
    if (base == nullptr) return 0;
    if (readonly) return 0;
    if (ofs + nbyte > (unsigned)sz) nbyte = sz - ofs;
    if (nbyte > 0) memcpy(base + ofs, buf, nbyte);
    ofs += nbyte;    
    return nbyte;
  }
  virtual int peek() {
    return -1; // TODO...
  }
  virtual int available() {
    if (base == nullptr) return 0;
    int s = sz - ofs;
    if (s < 0) s = 0;
    return s;
  }
  virtual void flush() {
  }
  virtual size_t read(void *buf, size_t nbyte) {
    if (ofs + nbyte > (unsigned)sz) nbyte = sz - ofs;
    if (nbyte > 0) memcpy(buf, base + ofs, nbyte);
    ofs += nbyte;
    return nbyte;
  }
  virtual bool truncate(uint64_t UNUSED(size)) {
    return false;
  }
  virtual bool seek(uint64_t pos, int mode = SeekSet) {
    if (base == nullptr) return false;
    int p = pos;    
    if (mode == SeekCur) p = ofs + pos;
    else if (mode == SeekEnd) p = ofs + sz - pos;
    if (p < 0 || p > sz) return false;
    ofs = p;
    return true;
  }
  virtual uint64_t position() {
    if (base == nullptr) return 0;
    return ofs;
  }
  virtual uint64_t size() {
    return sz;
  }
  virtual void close() {
     base = nullptr;
     ofs = 0;
     sz = 0;
  }
  virtual operator bool() {
    return base != nullptr;
  }
  virtual const char * name() {
    return empty;
  }
  virtual boolean isDirectory(void) {
    return false;
  }
  virtual File openNextFile(uint8_t UNUSED(mode)) {
    return File();
  }
  virtual void rewindDirectory(void) {
  }

  using Print::write;
private:
  friend class MemFS;
  MemFile(char *p, size_t size, uint8_t mode) {
    base = p;
    ofs = 0;
    this->sz = size;    
    readonly = (mode == FILE_READ);
  }  
  char *base = nullptr;
  int ofs = 0;
  int sz = 0;
  bool readonly = false;
};




class MemFS : public FS
{
public:
  MemFS() {
  }  
  File open(const char *ptr, uint8_t mode = FILE_READ) {
    size = 4096;
    return File(new MemFile((char*)ptr, size, mode));
  }
  File open(char *ptr, size_t size, uint8_t mode = FILE_READ) {
    this->size = size;
    return File(new MemFile(ptr, size, mode));
  }  
  bool exists(const char *UNUSED(filepath)) {
    return true;
  }
  bool mkdir(const char *UNUSED(filepath)) {
    return false;
  }
  bool rename(const char *UNUSED(oldfilepath), const char *UNUSED(newfilepath)) {
    return false;
  }
  bool remove(const char *UNUSED(filepath)) {
    return false;
  }
  bool rmdir(const char *UNUSED(filepath)) {
    return false;
  }
  uint64_t usedSize() {
    return size;
  }
  uint64_t totalSize() {
    return size;
  }
  
protected:
  size_t size;
};
