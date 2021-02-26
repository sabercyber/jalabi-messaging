//Original source brought in from a download from open sound connect
// author unknown


/*
 * message.h
 *
 *  Created on: 14-Feb-2021
 *      Author: Karthik Rao
 */

#ifndef INC_MESSAGE_H_
#define INC_MESSAGE_H_

/**
   struct used to hold an OSC message that will be written or read.

   The list of arguments is exposed as a sort of queue. You "pop"
   arguments from the front of the queue when reading, you push
   arguments at the back of the queue when writing.

   Many functions return *this, so they can be chained: init("/foo").pushInt32(2).pushStr("kllk")...

   Example of use:

   creation of a message:
   @code
   msg.init("/foo").pushInt32(4).pushStr("bar");
   @endcode
   reading a message, with error detection:
   @code
   if (msg.match("/foo/b*ar/plop")) {
     int i; std::string s; std::vector<char> b;
     if (msg.arg().popInt32(i).popStr(s).popBlob(b).isOkNoMoreArgs()) {
       process message...;
     } else arguments mismatch;
   }
   @endcode
*/
#include "time_data.h"
#include "utilities.h"

class message {
  time_data time_tag;
  std::string address;
  std::string type_tags;
  std::vector<std::pair<size_t, size_t> > arguments; // array of pairs (pos,size), pos being an index into the 'storage' array.
  Storage storage; // the arguments data is stored here
  ErrorCode err;
public:
  /** ArgReader is used for popping arguments from a Message, holds a
      pointer to the original Message, and maintains a local error code */
  class ArgReader {
    const message *msg;
    ErrorCode err;
    size_t arg_idx; // arg index of the next arg that will be popped out.
  public:
    ArgReader(const message &m, ErrorCode e = OK_NO_ERROR) : msg(&m), err(msg->getErr()), arg_idx(0) {
      if (e != OK_NO_ERROR && err == OK_NO_ERROR) err=e;
    }
    ArgReader(const ArgReader &other) : msg(other.msg), err(other.err), arg_idx(other.arg_idx) {}
    bool isBool() { return currentTypeTag() == TYPE_TAG_TRUE || currentTypeTag() == TYPE_TAG_FALSE; }
    bool isInt32() { return currentTypeTag() == TYPE_TAG_INT32; }
    bool isInt64() { return currentTypeTag() == TYPE_TAG_INT64; }
    bool isFloat() { return currentTypeTag() == TYPE_TAG_FLOAT; }
    bool isDouble() { return currentTypeTag() == TYPE_TAG_DOUBLE; }
    bool isStr() { return currentTypeTag() == TYPE_TAG_STRING; }
    bool isBlob() { return currentTypeTag() == TYPE_TAG_BLOB; }

    size_t nbArgRemaining() const { return msg->arguments.size() - arg_idx; }
    bool isOk() const { return err == OK_NO_ERROR; }
    operator bool() const { return isOk(); } // implicit bool conversion is handy here
    /** call this at the end of the popXXX() chain to make sure everything is ok and
        all arguments have been popped */
    bool isOkNoMoreArgs() const { return err == OK_NO_ERROR && nbArgRemaining() == 0; }
    ErrorCode getErr() const { return err; }

    /** retrieve an int32 argument */
    ArgReader &popInt32(int32_t &i) { return popPod<int32_t>(TYPE_TAG_INT32, i); }
    /** retrieve an int64 argument */
    ArgReader &popInt64(int64_t &i) { return popPod<int64_t>(TYPE_TAG_INT64, i); }
    /** retrieve a single precision floating point argument */
    ArgReader &popFloat(float &f) { return popPod<float>(TYPE_TAG_FLOAT, f); }
    /** retrieve a double precision floating point argument */
    ArgReader &popDouble(double &d) { return popPod<double>(TYPE_TAG_DOUBLE, d); }
    /** retrieve a string argument (no check performed on its content, so it may contain any byte value except 0) */
    ArgReader &popStr(std::string &s) {
      if (precheck(TYPE_TAG_STRING)) {
        s = argBeg(arg_idx++);
      }
      return *this;
    }
    /** retrieve a binary blob */
    ArgReader &popBlob(std::vector<char> &b) {
      if (precheck(TYPE_TAG_BLOB)) {
        b.assign(argBeg(arg_idx)+4, argEnd(arg_idx));
        ++arg_idx;
      }
      return *this;
    }
    /** retrieve a boolean argument */
    ArgReader &popBool(bool &b) {
      b = false;
      if (arg_idx >= msg->arguments.size()) PKT_SET_ERROR(NOT_ENOUGH_ARG);
      else if (currentTypeTag() == TYPE_TAG_TRUE) b = true;
      else if (currentTypeTag() == TYPE_TAG_FALSE) b = false;
      else PKT_SET_ERROR(TYPE_MISMATCH);
      ++arg_idx;
      return *this;
    }
    /** skip whatever comes next */
    ArgReader &pop() {
      if (arg_idx >= msg->arguments.size()) PKT_SET_ERROR(NOT_ENOUGH_ARG);
      else ++arg_idx;
      return *this;
    }
  private:
    const char *argBeg(size_t idx) {
      if (err || idx >= msg->arguments.size()) return 0;
      else return msg->storage.begin() + msg->arguments[idx].first;
    }
    const char *argEnd(size_t idx) {
      if (err || idx >= msg->arguments.size()) return 0;
      else return msg->storage.begin() + msg->arguments[idx].first + msg->arguments[idx].second;
    }
    int currentTypeTag() {
      if (!err && arg_idx < msg->type_tags.size()) return msg->type_tags[arg_idx];
      else PKT_SET_ERROR(NOT_ENOUGH_ARG);
      return -1;
    }
    template <typename POD> ArgReader &popPod(int tag, POD &v) {
      if (precheck(tag)) {
        v = bytes2pod<POD>(argBeg(arg_idx));
        ++arg_idx;
      } else v = POD(0);
      return *this;
    }
    /* pre-check stuff before popping an argument from the message */
    bool precheck(int tag) {
      if (arg_idx >= msg->arguments.size()) PKT_SET_ERROR(NOT_ENOUGH_ARG);
      else if (!err && currentTypeTag() != tag) PKT_SET_ERROR(TYPE_MISMATCH);
      return err == OK_NO_ERROR;
    }
  };

  message() { clear(); }
  message(const std::string &s, time_data tt = time_data::immediate()) : time_tag(tt), address(s), err(OK_NO_ERROR) {}
  message(const void *ptr, size_t sz, time_data tt = time_data::immediate()) { buildFromRawData(ptr, sz); time_tag = tt; }

  bool isOk() const { return err == OK_NO_ERROR; }
  ErrorCode getErr() const { return err; }

  /** return the type_tags string, with its initial ',' stripped. */
  const std::string &typeTags() const { return type_tags; }
  /** retrieve the address pattern. If you want to follow to the whole OSC spec, you
      have to handle its matching rules for address specifications -- this file does
      not provide this functionality */
  const std::string &addressPattern() const { return address; }
  time_data timeTag() const { return time_tag; }
  /** clear the message and start a new message with the supplied address and time_tag. */
  message &init(const std::string &addr, time_data tt = time_data::immediate()) {
    clear();
    address = addr; time_tag = tt;
    if (address.empty() || address[0] != '/') PKT_SET_ERROR(MALFORMED_ADDRESS_PATTERN);
    return *this;
  }

  /** start a matching test. The typical use-case is to follow this by
      a sequence of calls to popXXX() and a final call to
      isOkNoMoreArgs() which will allow to check that everything went
      fine. For example:
      @code
      if (msg.match("/foo").popInt32(i).isOkNoMoreArgs()) { blah(i); }
      else if (msg.match("/bar").popStr(s).popInt32(i).isOkNoMoreArgs()) { plop(s,i); }
      else cerr << "unhandled message: " << msg << "\n";
      @endcode
  */
  ArgReader match(const std::string &test) const {
    return ArgReader(*this, fullPatternMatch(address.c_str(), test.c_str()) ? OK_NO_ERROR : PATTERN_MISMATCH);
  }
  /** return true if the 'test' path matched by the first characters of addressPattern().
      For ex. ("/foo/bar").partialMatch("/foo/") is true */
  ArgReader partialMatch(const std::string &test) const {
    return ArgReader(*this, partialPatternMatch(address.c_str(), test.c_str()) ? OK_NO_ERROR : PATTERN_MISMATCH);
  }
  ArgReader arg() const { return ArgReader(*this, OK_NO_ERROR); }

  /** build the osc message for raw data (the message will keep a copy of that data) */
  void buildFromRawData(const void *ptr, size_t sz) {
    clear();
    storage.assign((const char*)ptr, (const char*)ptr + sz);
    const char *address_beg = storage.begin();
    const char *address_end = (const char*)memchr(address_beg, 0, storage.end()-address_beg);
    if (!address_end || !isZeroPaddingCorrect(address_end+1) || address_beg[0] != '/') {
      PKT_SET_ERROR(MALFORMED_ADDRESS_PATTERN); return;
    } else address.assign(address_beg, address_end);

    const char *type_tags_beg = ceil4(address_end+1);
    const char *type_tags_end = (const char*)memchr(type_tags_beg, 0, storage.end()-type_tags_beg);
    if (!type_tags_end || !isZeroPaddingCorrect(type_tags_end+1) || type_tags_beg[0] != ',') {
      PKT_SET_ERROR(MALFORMED_TYPE_TAGS); return;
    } else type_tags.assign(type_tags_beg+1, type_tags_end); // we do not copy the initial ','

    const char *arg = ceil4(type_tags_end+1); assert(arg <= storage.end());
    size_t iarg = 0;
    while (isOk() && iarg < type_tags.size()) {
      assert(arg <= storage.end());
      size_t len = getArgSize(type_tags[iarg], arg);
      if (isOk()) arguments.push_back(std::make_pair(arg - storage.begin(), len));
      arg += ceil4(len); ++iarg;
    }
    if (iarg < type_tags.size() || arg != storage.end()) {
      PKT_SET_ERROR(MALFORMED_ARGUMENTS);
    }
  }

  /* below are all the functions that serve when *writing* a message */
  message &pushBool(bool b) {
    type_tags += (b ? TYPE_TAG_TRUE : TYPE_TAG_FALSE);
    arguments.push_back(std::make_pair(storage.size(), storage.size()));
    return *this;
  }
  message &pushInt32(int32_t i) { return pushPod(TYPE_TAG_INT32, i); }
  message &pushInt64(int64_t h) { return pushPod(TYPE_TAG_INT64, h); }
  message &pushFloat(float f) { return pushPod(TYPE_TAG_FLOAT, f); }
  message &pushDouble(double d) { return pushPod(TYPE_TAG_DOUBLE, d); }
  message &pushStr(const std::string &s) {
    assert(s.size() < 2147483647); // insane values are not welcome
    type_tags += TYPE_TAG_STRING;
    arguments.push_back(std::make_pair(storage.size(), s.size() + 1));
    strcpy(storage.getBytes(s.size()+1), s.c_str());
    return *this;
  }
  message &pushBlob(void *ptr, size_t num_bytes) {
    assert(num_bytes < 2147483647); // insane values are not welcome
    type_tags += TYPE_TAG_BLOB;
    arguments.push_back(std::make_pair(storage.size(), num_bytes+4));
    pod2bytes<int32_t>((int32_t)num_bytes, storage.getBytes(4));
    if (num_bytes)
      memcpy(storage.getBytes(num_bytes), ptr, num_bytes);
    return *this;
  }

  /** reset the message to a clean state */
  void clear() {
    address.clear(); type_tags.clear(); storage.clear(); arguments.clear();
    err = OK_NO_ERROR; time_tag = time_data::immediate();
  }

  /** write the raw message data (used by PacketWriter) */
  void packMessage(Storage &s, bool write_size) const {
    if (!isOk()) return;
    size_t l_addr = address.size()+1, l_type = type_tags.size()+2;
    if (write_size)
      pod2bytes<uint32_t>(uint32_t(ceil4(l_addr) + ceil4(l_type) + ceil4(storage.size())), s.getBytes(4));
    strcpy(s.getBytes(l_addr), address.c_str());
    strcpy(s.getBytes(l_type), ("," + type_tags).c_str());
    if (storage.size())
      memcpy(s.getBytes(storage.size()), const_cast<Storage&>(storage).begin(), storage.size());
  }

private:

  /* get the number of bytes occupied by the argument */
  size_t getArgSize(int type, const char *p) {
    if (err) return 0;
    size_t sz = 0;
    assert(p >= storage.begin() && p <= storage.end());
    switch (type) {
      case TYPE_TAG_TRUE:
      case TYPE_TAG_FALSE: sz = 0; break;
      case TYPE_TAG_INT32:
      case TYPE_TAG_FLOAT: sz = 4; break;
      case TYPE_TAG_INT64:
      case TYPE_TAG_DOUBLE: sz = 8; break;
      case TYPE_TAG_STRING: {
        const char *q = (const char*)memchr(p, 0, storage.end()-p);
        if (!q) PKT_SET_ERROR(MALFORMED_ARGUMENTS);
        else sz = (q-p)+1;
      } break;
      case TYPE_TAG_BLOB: {
        if (p == storage.end()) { PKT_SET_ERROR(MALFORMED_ARGUMENTS); return 0; }
        sz = 4+bytes2pod<uint32_t>(p);
      } break;
      default: {
        PKT_SET_ERROR(UNHANDLED_TYPE_TAGS); return 0;
      } break;
    }
    if (p+sz > storage.end() || /* string or blob too large.. */
        p+sz < p /* or even blob so large that it did overflow */) {
      PKT_SET_ERROR(MALFORMED_ARGUMENTS); return 0;
    }
    if (!isZeroPaddingCorrect(p+sz)) { PKT_SET_ERROR(MALFORMED_ARGUMENTS); return 0; }
    return sz;
  }

  template <typename POD> message &pushPod(int tag, POD v) {
    type_tags += (char)tag;
    arguments.push_back(std::make_pair(storage.size(), sizeof(POD)));
    pod2bytes(v, storage.getBytes(sizeof(POD)));
    return *this;
  }

#ifdef OSTREAM_OUTPUT
  friend std::ostream &operator<<(std::ostream &os, const message &msg) {
    os << "osc_address: '" << msg.address << "', types: '" << msg.type_tags << "', timetag=" << msg.time_tag << ", args=[";
    message::ArgReader arg(msg);
    while (arg.nbArgRemaining() && arg.isOk()) {
      if (arg.isBool()) { bool b; arg.popBool(b); os << (b?"True":"False"); }
      else if (arg.isInt32()) { int32_t i; arg.popInt32(i); os << i; }
      else if (arg.isInt64()) { int64_t h; arg.popInt64(h); os << h << "ll"; }
      else if (arg.isFloat()) { float f; arg.popFloat(f); os << f << "f"; }
      else if (arg.isDouble()) { double d; arg.popDouble(d); os << d; }
      else if (arg.isStr()) { std::string s; arg.popStr(s); os << "'" << s << "'"; }
      else if (arg.isBlob()) { std::vector<char> b; arg.popBlob(b); os << "Blob " << b.size() << " bytes"; }
      else {
        assert(0); // I forgot a case..
      }
      if (arg.nbArgRemaining()) os << ", ";
    }
    if (!arg.isOk()) { os << " ERROR#" << arg.getErr(); }
    os << "]";
    return os;
  }
#endif
};



#endif /* INC_MESSAGE_H_ */
