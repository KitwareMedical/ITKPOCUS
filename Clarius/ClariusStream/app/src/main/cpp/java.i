%module igtljava

%{
#include "server.h"
%}

%include "arrays_java.i"
%include "various.i"
/* converts args with (char *imageData, size_t len) to (byte[] arg) in java */
%apply (char *STRING, size_t LENGTH) { (char *imageData, size_t len) }

%include "server.h"
