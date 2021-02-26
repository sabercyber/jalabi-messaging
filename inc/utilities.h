/*
 * utilities.h
 *
 *  Created on: 14-Feb-2021
 *      Author: Karthik Rao
 */

#ifndef INC_UTILITIES_H_
#define INC_UTILITIES_H_
#include <string>
#include <vector>
#include <cstring>

#include <cassert>


#ifndef _MSC_VER
#include <stdint.h>
#else
namespace oscpkt {
  typedef __int32 int32_t;
  typedef unsigned __int32 uint32_t;
  typedef __int64 int64_t;
  typedef unsigned __int64 uint64_t;
}
#endif

const int32_t storage_alloaction = 2096;
/* the various types that we handle (OSC 1.0 specifies that INT32/FLOAT/STRING/BLOB are the bare minimum) */
enum {
  TYPE_TAG_TRUE = 'T',
  TYPE_TAG_FALSE = 'F',
  TYPE_TAG_INT32 = 'i',
  TYPE_TAG_INT64 = 'h',
  TYPE_TAG_FLOAT = 'f',
  TYPE_TAG_DOUBLE = 'd',
  TYPE_TAG_STRING = 's',
  TYPE_TAG_BLOB = 'b'
};

/* a few utility functions follow.. */

// round to the next multiple of 4, works for size_t and pointer arguments
template <typename Type> Type ceil4(Type p) { return (Type)((size_t(p) + 3)&(~size_t(3))); }

// check that a memory area is zero padded until the next address which is a multiple of 4
inline bool isZeroPaddingCorrect(const char *p) {
  const char *q = ceil4(p);
  for (;p < q; ++p)
    if (*p != 0) { return false; }
  return true;
}

// stuff for reading / writing POD ("Plain Old Data") variables to unaligned bytes.
template <typename POD> union PodBytes {
  char bytes[sizeof(POD)];
  POD  value;
};

inline bool isBigEndian() { // a compile-time constant would certainly improve performances..
  PodBytes<int32_t> p; p.value = 0x12345678;
  return p.bytes[0] == 0x12;
}

/** read unaligned bytes into a POD type, assuming the bytes are a little endian representation */
template <typename POD> POD bytes2pod(const char *bytes) {
  PodBytes<POD> p;
  for (size_t i=0; i < sizeof(POD); ++i) {
    if (isBigEndian())
      p.bytes[i] = bytes[i];
    else
      p.bytes[i] = bytes[sizeof(POD) - i - 1];
  }
  return p.value;
}

/** stored a POD type into an unaligned bytes array, using little endian representation */
template <typename POD> void pod2bytes(const POD value, char *bytes) {
  PodBytes<POD> p; p.value = value;
  for (size_t i=0; i < sizeof(POD); ++i) {
    if (isBigEndian())
      bytes[i] = p.bytes[i];
    else
      bytes[i] = p.bytes[sizeof(POD) - i - 1];
  }
}

/** internal stuff, handles the dynamic storage with correct alignments to 4 bytes */
struct Storage {
  std::vector<char> data;
  Storage() { data.reserve(storage_alloaction); }
  char *getBytes(size_t sz) {
    assert((data.size() & 3) == 0);
    if (data.size() + sz > data.capacity()) { data.reserve((data.size() + sz)*2); }
    size_t sz4 = ceil4(sz);
    size_t pos = data.size();
    data.resize(pos + sz4); // resize will fill with zeros, so the zero padding is OK
    return &(data[pos]);
  }
  char *begin() { return data.size() ? &data.front() : 0; }
  char *end() { return begin() + size(); }
  const char *begin() const { return data.size() ? &data.front() : 0; }
  const char *end() const { return begin() + size(); }
  size_t size() const { return data.size(); }
  void assign(const char *beg, const char *end) { data.assign(beg, end); }
  void clear() { data.resize(0); }
};


// see the OSC spec for the precise pattern matching rules
inline const char *internalPatternMatch(const char *pattern, const char *path) {
  while (*pattern) {
    const char *p = pattern;
    if (*p == '?' && *path) { ++p; ++path; }
    else if (*p == '[' && *path) { // bracketted range, e.g. [a-zABC]
      ++p;
      bool reverse = false;
      if (*p == '!') { reverse = true; ++p; }
      bool match = reverse;
      for (; *p && *p != ']'; ++p) {
        char c0 = *p, c1 = c0;
        if (p[1] == '-' && p[2] && p[2] != ']') { p += 2; c1 = *p; }
        if (*path >= c0 && *path <= c1) { match = !reverse; }
      }
      if (!match || *p != ']') return pattern;
      ++p; ++path;
    } else if (*p == '*') { // wildcard '*'
      while (*p == '*') ++p;
      const char *best = 0;
      while (true) {
        const char *ret = internalPatternMatch(p, path);
        if (ret && ret > best) best = ret;
        if (*path == 0 || *path == '/') break;
        else ++path;
      }
      return best;
    } else if (*p == '/' && *(p+1) == '/') { // the super-wildcard '//'
      while (*(p+1)=='/') ++p;
      const char *best = 0;
      while (true) {
        const char *ret = internalPatternMatch(p, path);
        if (ret && ret > best) best = ret;
        if (*path == 0) break;
        if (*path == 0 || (path = strchr(path+1, '/')) == 0) break;
      }
      return best;
    } else if (*p == '{') { // braced list {foo,bar,baz}
      const char *end = strchr(p, '}'), *q;
      if (!end) return 0; // syntax error in brace list..
      bool match = false;
      do {
        ++p;
        q = strchr(p, ',');
        if (q == 0 || q > end) q = end;
        if (strncmp(p, path, q-p)==0) {
          path += (q-p); p = end+1; match = true;
        } else p=q;
      } while (q != end && !match);
      if (!match) return pattern;
    } else if (*p == *path) { ++p; ++path; } // any other character
    else break;
    pattern = p;
  }
  return (*path == 0 ? pattern : 0);
}




bool partialPatternMatch(const std::string &pattern, const std::string &test) {
  const char *q = internalPatternMatch(pattern.c_str(), test.c_str());
  return q != 0;
}

bool fullPatternMatch(const std::string &pattern, const std::string &test) {
  const char *q = internalPatternMatch(pattern.c_str(), test.c_str());
  return q && *q == 0;
}

#if defined(PKT_DEBUG)
#define PKT_SET_ERROR(errcode) do { if (!err) { err = errcode; std::cerr << "set " #errcode << " at line " << __LINE__ << "\n"; } } while (0)
#else
#define PKT_SET_ERROR(errcode) do { if (!err) err = errcode; } while (0)
#endif

typedef enum { OK_NO_ERROR=0,
               // errors raised by the Message class:
               MALFORMED_ADDRESS_PATTERN, MALFORMED_TYPE_TAGS, MALFORMED_ARGUMENTS, UNHANDLED_TYPE_TAGS,
               // errors raised by ArgReader
               TYPE_MISMATCH, NOT_ENOUGH_ARG, PATTERN_MISMATCH,
               // errors raised by PacketReader/PacketWriter
               INVALID_BUNDLE, INVALID_PACKET_SIZE, BUNDLE_REQUIRED_FOR_MULTI_MESSAGES } ErrorCode;




#endif /* INC_UTILITIES_H_ */
