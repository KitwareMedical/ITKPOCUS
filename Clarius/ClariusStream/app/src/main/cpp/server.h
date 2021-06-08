#ifndef _SERVER_H_
#define _SERVER_H_

#include <thread>

#include "igtlServerSocket.h"

class ImageServer {
public:
    struct ImgMsg {
      // copied from igtl::ImageMessage

      /// Coordinate system. Either left-posterior-superior (LPS) or right-anterior-superior (RAS).
      enum {
          COORDINATE_RAS=1,
          COORDINATE_LPS=2
      };

    /// Endian used in the bite array for the image data.
      enum {
          ENDIAN_BIG=1,
          ENDIAN_LITTLE=2
      };

    /// Pixel data type either scalar or vector.
      enum {
          DTYPE_SCALAR = 1,
          DTYPE_VECTOR = 3
      };

    /// Pixel data type.
      enum {
          TYPE_INT8    = 2,
          TYPE_UINT8   = 3,
          TYPE_INT16   = 4,
          TYPE_UINT16  = 5,
          TYPE_INT32   = 6,
          TYPE_UINT32  = 7,
          TYPE_FLOAT32 = 10,
          TYPE_FLOAT64 = 11
      };

  };

  ImageServer();
  ~ImageServer();
  int CreateServer(int);
  // provide arg names for imageData and len so swig can convert to byte[] in java
  void SendImageBasic(int[3], float[3], int, int, char *, char *imageData, size_t len);
private:
  void WaitForConnection(void);

  bool m_running;
  igtl::ServerSocket::Pointer m_serverSocket;
  std::thread m_serverThread;
  std::vector<igtl::Socket::Pointer> m_clients;
};

#endif /* _SERVER_H_ */
