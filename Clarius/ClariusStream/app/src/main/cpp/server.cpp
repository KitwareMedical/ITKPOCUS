#include <iostream>
#include <thread>

#include "server.h"
#include <android/log.h>
#include <igtlImageMessage.h>

#define TAG "ImageServerNative"
#define MAX_CONNS 50

ImageServer::ImageServer() : m_running(false), m_serverSocket(igtl::ServerSocket::New()) {}

ImageServer::~ImageServer() {
  if (m_running) {
    m_running = false;
    m_serverThread.join();
    m_clients.clear();
  }
}

int ImageServer::CreateServer(int port) {
  m_serverSocket = igtl::ServerSocket::New();
  __android_log_print(ANDROID_LOG_VERBOSE, TAG, "is server socket null: %d", m_serverSocket.IsNull());
  if (m_serverSocket.IsNotNull()) {
    int res = m_serverSocket->CreateServer(port);
    __android_log_print(ANDROID_LOG_VERBOSE, TAG, "create server result: %d", res);
    if (res < 0) {
      return res;
    }

    m_running = true;
    m_serverThread = std::thread(&ImageServer::WaitForConnection, this);
    return 0;
  }
  return -1;
}

void ImageServer::WaitForConnection() {
  while (m_running && m_serverSocket) {
    igtl::Socket::Pointer sock;
    sock = m_serverSocket->WaitForConnection(1000);
    if (sock.IsNotNull()) {
      std::cout << "Found a client" << std::endl;
      // TODO clean up clients
      m_clients.push_back(sock);
      std::cout << "done" << std::endl;
    }
  }
}

void ImageServer::SendImageBasic(
  int dims[3],
  float spacing[3],
  int scalarType,
  int numComponents,
  char *deviceName,
  char *imageData,
  size_t len
) {
  igtl::ImageMessage::Pointer img = igtl::ImageMessage::New();
  img->SetDimensions(dims);
  img->SetSpacing(spacing);
  img->SetNumComponents(numComponents);
  img->SetScalarType(scalarType);
  img->SetDeviceName(deviceName);
  int offset[3] = {0, 0, 0};
  img->SetSubVolume(dims, offset);
  img->AllocateScalars();

  auto *scalars = (unsigned char*)img->GetScalarPointer();
  for (int i = 0; i < len; i++) {
      scalars[i] = imageData[i];
  }

  img->Pack();

  for (const auto & client : m_clients) {
    client->Send(img->GetPackPointer(), img->GetPackSize());
  }
}
