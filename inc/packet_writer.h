/*
 * packet_writer.h
 *
 *  Created on: 14-Feb-2021
 *      Author: Karthik Rao
 */

#ifndef INC_PACKET_WRITER_H_
#define INC_PACKET_WRITER_H_


/**
   Assemble messages into an OSC packet. Example of use:
   @code
   PacketWriter pkt;
   Message msg;
   pkt.startBundle();
   pkt.addMessage(msg.init("/foo").pushBool(true).pushStr("plop").pushFloat(3.14f));
   pkt.addMessage(msg.init("/bar").pushBool(false));
   pkt.endBundle();
   if (pkt.isOk()) {
     send(pkt.data(), pkt.size());
   }
   @endcode
*/

#include "time_data.h"
#include "utilities.h"
class packet_writer {
public:
  packet_writer() { init(); }
  packet_writer &init() { err = OK_NO_ERROR; storage.clear(); bundles.clear(); return *this; }

  /** begin a new bundle. If you plan to pack more than one message in the Osc packet, you have to
      put them in a bundle. Nested bundles inside bundles are also allowed. */
  packet_writer &startBundle(time_data ts = time_data::immediate()) {
    char *p;
    if (bundles.size()) storage.getBytes(4); // hold the bundle size
    p = storage.getBytes(8); strcpy(p, "#bundle"); bundles.push_back(p - storage.begin());
    p = storage.getBytes(8); pod2bytes<uint64_t>(ts, p);
    return *this;
  }
  /** close the current bundle. */
  packet_writer &endBundle() {
    if (bundles.size()) {
      if (storage.size() - bundles.back() == 16) {
        pod2bytes<uint32_t>(0, storage.getBytes(4)); // the 'empty bundle' case, not very elegant
      }
      if (bundles.size()>1) { // no size stored for the top-level bundle
        pod2bytes<uint32_t>(uint32_t(storage.size() - bundles.back()), storage.begin() + bundles.back()-4);
      }
      bundles.pop_back();
    } else PKT_SET_ERROR(INVALID_BUNDLE);
    return *this;
  }

  /** insert an Osc message into the current bundle / packet.
   */
  packet_writer &addMessage(const message &msg) {
    if (storage.size() != 0 && bundles.empty()) PKT_SET_ERROR(BUNDLE_REQUIRED_FOR_MULTI_MESSAGES);
    else msg.packMessage(storage, bundles.size()>0);
    if (!msg.isOk()) PKT_SET_ERROR(msg.getErr());
    return *this;
  }

  /** the error flag will be raised if an opened bundle is not closed, or if more than one message is
      inserted in the packet without a bundle */
  bool isOk() { return err == OK_NO_ERROR; }
  ErrorCode getErr() { return err; }

  /** return the number of bytes of the osc packet -- will always be a
      multiple of 4 -- returns 0 if the construction of the packet has
      failed. */
  uint32_t packetSize() { return err ? 0 : (uint32_t)storage.size(); }

  /** return the bytes of the osc packet (NULL if the construction of the packet has failed) */
  char *packetData() { return err ? 0 : storage.begin(); }
private:
  std::vector<size_t> bundles; // hold the position in the storage array of the beginning marker of each bundle
  Storage storage;
  ErrorCode err;
};









#endif /* INC_PACKET_WRITER_H_ */
