#ifndef BUFFEREDINPUTSTREAM_H
#define BUFFEREDINPUTSTREAM_H

#include "DataInputStream.h"
#include <vector>

class BufferedInputStream : public InputStream
{
public:

  BufferedInputStream(InputStream *input);

  virtual ~BufferedInputStream();

  virtual size_t read(void *buffer, size_t len);

  size_t available();

protected:

  const size_t MAX_BUFFER_SIZE;
  const size_t INITIAL_BUFFER_SIZE;

  DataInputStream *m_input;

  std::vector<char> m_buffer;

  size_t m_have;
  size_t m_pos;

};

#endif // BUFFEREDINPUTSTREAM_H
