#include "line_reader.h"

bool LineReader::feed(char c) {
  if (ready) {
    length = 0;
    ready = false;
  }
  if (c == '\r' || c == '\n') {
    if (length == 0) {
      return false;
    }
    buffer[length] = '\0';
    ready = true;
    return true;
  }
  if (length < CAPACITY - 1) {
    buffer[length++] = c;
  }
  return false;
}
