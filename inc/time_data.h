


/**
   OSC timetag stuff, the highest 32-bit are seconds, the lowest are fraction of a second.
*/

#ifndef INC_TIMETAG_H_
#define INC_TIMETAG_H_

#include <stdint.h>
#include <chrono>
class time_data {
  uint64_t v;
public:
  time_data()  {
      std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();

    auto epoch = now.time_since_epoch();
    auto value = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
    v = value.count();

  }
  explicit time_data(uint64_t w): v(w) {}
  operator uint64_t() const { return v; }
  static time_data immediate() {
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();

    auto epoch = now.time_since_epoch();
    auto value = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
    long duration = value.count();
      return time_data(duration); }
};

#endif
