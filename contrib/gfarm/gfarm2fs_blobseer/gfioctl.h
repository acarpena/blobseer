/*
    GFARM-ioctl: versioning support for gfarm
    written by : TRAN Viet Trung
*/

#include <sys/types.h>
#include <sys/uio.h>
#include <sys/ioctl.h>

enum {
  BLOB_GET_ACTIVE_VERSION = _IOR('E', 0, size_t),
  BLOB_GET_VERSION_COUNT = _IOR('E', 1, size_t),
  BLOB_SET_ACTIVE_VERSION = _IOW('E',2,size_t),
};
    
