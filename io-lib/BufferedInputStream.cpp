#include "BufferedInputStream.h"
#include <string.h>
#include <istream>
#include <sstream>
#include <algorithm>

BufferedInputStream::BufferedInputStream(InputStream *input)
: m_have(0),
  INITIAL_BUFFER_SIZE(100000),
  MAX_BUFFER_SIZE(10000000),
  m_buffer(INITIAL_BUFFER_SIZE),
  m_pos(0)
{
  m_input = new DataInputStream(input);
}

BufferedInputStream::~BufferedInputStream() {
  delete m_input;
}

size_t BufferedInputStream::available() {
  return m_have;
}


size_t BufferedInputStream::read(void *buffer, size_t len) {
  char *buf = (char *)buffer;

  if (m_have == 0) {
    m_pos = 0;
  }

  if (len <= m_have) {
    memcpy(buf, &m_buffer[m_pos], len);
    m_have -= len;
    m_pos += len;
    return len;
  }
  memcpy(buf, &m_buffer[m_pos], m_have);
  size_t copied = m_have;
  m_pos = 0;
  if (m_buffer.size() > INITIAL_BUFFER_SIZE) {
    m_buffer.resize(INITIAL_BUFFER_SIZE);
  }  
  // FIXME: available() sometimes does not work for POCO 
  // Websocket (always for secure connection).
  size_t need = m_input->available();
  if (need > INITIAL_BUFFER_SIZE) {
    m_buffer.resize(std::min(need, MAX_BUFFER_SIZE));
  }
  try {
    m_have = m_input->read(&m_buffer[0], m_buffer.size());
  }
  catch (Exception &e) { 
    if (m_buffer.size() == MAX_BUFFER_SIZE) {
      throw e;
    }
    m_buffer.resize(MAX_BUFFER_SIZE);
    m_have = m_input->read(&m_buffer[0], m_buffer.size());
  }
  size_t remain = std::min(m_have, len - copied);
  memcpy(&buf[copied], &m_buffer[0], remain);
  copied += remain;
  m_have -= remain;
  m_pos += remain;
  if (m_have == 0) {
    m_pos = 0;
  }
  return copied;
}

