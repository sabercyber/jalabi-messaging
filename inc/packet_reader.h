/*
 * packet_reader.h
 *
 *  Created on: 14-Feb-2021
 *      Author: Karthik Rao
 */

#ifndef INC_PACKET_READER_H_
#define INC_PACKET_READER_H_


#include <stdio.h>
#include <list>
#include "utilities.h"
#include "message.h"
#include "time_data.h"


/**
   parse an OSC packet and extracts the embedded OSC messages.
*/
class packet_reader {
public:
  packet_reader() { err = OK_NO_ERROR; }
  /** pointer and size of the osc packet to be parsed. */
  packet_reader(const void *ptr, size_t sz) { init(ptr, sz); }

  void init(const void *ptr, size_t sz) {
    err = OK_NO_ERROR; messages.clear();
    if ((sz%4) == 0) {
      parse((const char*)ptr, (const char *)ptr+sz, time_data::immediate());
    } else PKT_SET_ERROR(INVALID_PACKET_SIZE);
    it_messages = messages.begin();
  }

  /** extract the next osc message from the packet. return 0 when all messages have been read, or in case of error. */
  message *popMessage() {
    if (!err && !messages.empty() && it_messages != messages.end()) return &*it_messages++;
    else return 0;
  }
  bool isOk() const { return err == OK_NO_ERROR; }
  ErrorCode getErr() const { return err; }

private:
  std::list<message> messages;
  std::list<message>::iterator it_messages;
  ErrorCode err;

  void parse(const char *beg, const char *end, time_data time_tag) {
    assert(beg <= end && !err); assert(((end-beg)%4)==0);

    if (beg == end) return;
    if (*beg == '#') {
      /* it's a bundle */
      if (end - beg >= 20
          && memcmp(beg, "#bundle\0", 8) == 0) {
        time_data time_tag2(bytes2pod<uint64_t>(beg+8));
        const char *pos = beg + 16;
        do {
          uint32_t sz = bytes2pod<uint32_t>(pos); pos += 4;
          if ((sz&3) != 0 || pos + sz > end || pos+sz < pos) {
            PKT_SET_ERROR(INVALID_BUNDLE);
          } else {
            parse(pos, pos+sz, time_tag2);
            pos += sz;
          }
        } while (!err && pos != end);
      } else {
        PKT_SET_ERROR(INVALID_BUNDLE);
      }
    } else {
      messages.push_back(message(beg, end-beg, time_tag));
      if (!messages.back().isOk()) PKT_SET_ERROR(messages.back().getErr());
    }
  }
};




#endif /* INC_PACKET_READER_H_ */
