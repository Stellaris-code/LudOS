#include <errno.h>
#include <string.h>

char * _strerror_r(
        int errnum,
        int internal,
        int *errptr)
{
  const char *error;

  switch (errnum)
    {
    case 0:
      error = "Success";
      break;
/* go32 defines EPERM as EACCES */
#if defined (EPERM) && (!defined (EACCES) || (EPERM != EACCES))
    case EPERM:
      error = "Not owner";
      break;
#endif
#ifdef ENOENT
    case ENOENT:
      error = "No such file or directory";
      break;
#endif
#ifdef ESRCH
    case ESRCH:
      error = "No such process";
      break;
#endif
#ifdef EINTR
    case EINTR:
      error = "Interrupted system call";
      break;
#endif
#ifdef EIO
    case EIO:
      error = "I/O error";
      break;
#endif
/* go32 defines ENXIO as ENODEV */
#if defined (ENXIO) && (!defined (ENODEV) || (ENXIO != ENODEV))
    case ENXIO:
      error = "No such device or address";
      break;
#endif
#ifdef E2BIG
    case E2BIG:
      error = "Arg list too long";
      break;
#endif
#ifdef ENOEXEC
    case ENOEXEC:
      error = "Exec format error";
      break;
#endif
#ifdef EALREADY
    case EALREADY:
      error = "Socket already connected";
      break;
#endif
#ifdef EBADF
    case EBADF:
      error = "Bad file number";
      break;
#endif
#ifdef ECHILD
    case ECHILD:
      error = "No children";
      break;
#endif
#ifdef EDESTADDRREQ
    case EDESTADDRREQ:
      error = "Destination address required";
      break;
#endif
#ifdef EAGAIN
    case EAGAIN:
      error = "No more processes";
      break;
#endif
#ifdef ENOMEM
    case ENOMEM:
      error = "Not enough space";
      break;
#endif
#ifdef EACCES
    case EACCES:
      error = "Permission denied";
      break;
#endif
#ifdef EFAULT
    case EFAULT:
      error = "Bad address";
      break;
#endif
#ifdef ENOTBLK
    case ENOTBLK:
      error = "Block device required";
      break;
#endif
#ifdef EBUSY
    case EBUSY:
      error = "Device or resource busy";
      break;
#endif
#ifdef EEXIST
    case EEXIST:
      error = "File exists";
      break;
#endif
#ifdef EXDEV
    case EXDEV:
      error = "Cross-device link";
      break;
#endif
#ifdef ENODEV
    case ENODEV:
      error = "No such device";
      break;
#endif
#ifdef ENOTDIR
    case ENOTDIR:
      error = "Not a directory";
      break;
#endif
#ifdef EHOSTDOWN
    case EHOSTDOWN:
      error = "Host is down";
      break;
#endif
#ifdef EINPROGRESS
    case EINPROGRESS:
      error = "Connection already in progress";
      break;
#endif
#ifdef EISDIR
    case EISDIR:
      error = "Is a directory";
      break;
#endif
#ifdef EINVAL
    case EINVAL:
      error = "Invalid argument";
      break;
#endif
#ifdef ENETDOWN
    case ENETDOWN:
      error = "Network interface is not configured";
      break;
#endif
#ifdef ENFILE
    case ENFILE:
      error = "Too many open files in system";
      break;
#endif
#ifdef EMFILE
    case EMFILE:
      error = "Too many open files";
      break;
#endif
#ifdef ENOTTY
    case ENOTTY:
      error = "Not a character device";
      break;
#endif
#ifdef ETXTBSY
    case ETXTBSY:
      error = "Text file busy";
      break;
#endif
#ifdef EFBIG
    case EFBIG:
      error = "File too large";
      break;
#endif
#ifdef EHOSTUNREACH
    case EHOSTUNREACH:
      error = "Host is unreachable";
      break;
#endif
#ifdef ENOSPC
    case ENOSPC:
      error = "No space left on device";
      break;
#endif
#ifdef ENOTSUP
    case ENOTSUP:
      error = "Not supported";
      break;
#endif
#ifdef ESPIPE
    case ESPIPE:
      error = "Illegal seek";
      break;
#endif
#ifdef EROFS
    case EROFS:
      error = "Read-only file system";
      break;
#endif
#ifdef EMLINK
    case EMLINK:
      error = "Too many links";
      break;
#endif
#ifdef EPIPE
    case EPIPE:
      error = "Broken pipe";
      break;
#endif
#ifdef EDOM
    case EDOM:
      error = "Math argument";
      break;
#endif
#ifdef ERANGE
    case ERANGE:
      error = "Result too large";
      break;
#endif
#ifdef ENOMSG
    case ENOMSG:
      error = "No message of desired type";
      break;
#endif
#ifdef EIDRM
    case EIDRM:
      error = "Identifier removed";
      break;
#endif
#ifdef EDEADLK
    case EDEADLK:
      error = "Deadlock";
      break;
#endif
#ifdef ENETUNREACH
    case  ENETUNREACH:
      error = "Network is unreachable";
      break;
#endif
#ifdef ENOLCK
    case ENOLCK:
      error = "No lock";
      break;
#endif
#ifdef ENOSTR
    case ENOSTR:
      error = "Not a stream";
      break;
#endif
#ifdef ETIME
    case ETIME:
      error = "Stream ioctl timeout";
      break;
#endif
#ifdef ENOSR
    case ENOSR:
      error = "No stream resources";
      break;
#endif
#ifdef ENONET
    case ENONET:
      error = "Machine is not on the network";
      break;
#endif
#ifdef ENOPKG
    case ENOPKG:
      error = "No package";
      break;
#endif
#ifdef EREMOTE
    case EREMOTE:
      error = "Resource is remote";
      break;
#endif
#ifdef ENOLINK
    case ENOLINK:
      error = "Virtual circuit is gone";
      break;
#endif
#ifdef EADV
    case EADV:
      error = "Advertise error";
      break;
#endif
#ifdef ESRMNT
    case ESRMNT:
      error = "Srmount error";
      break;
#endif
#ifdef ECOMM
    case ECOMM:
      error = "Communication error";
      break;
#endif
#ifdef EPROTO
    case EPROTO:
      error = "Protocol error";
      break;
#endif
#ifdef EPROTONOSUPPORT
    case EPROTONOSUPPORT:
      error = "Unknown protocol";
      break;
#endif
#ifdef EMULTIHOP
    case EMULTIHOP:
      error = "Multihop attempted";
      break;
#endif
#ifdef EBADMSG
    case EBADMSG:
      error = "Bad message";
      break;
#endif
#ifdef ELIBACC
    case ELIBACC:
      error = "Cannot access a needed shared library";
      break;
#endif
#ifdef ELIBBAD
    case ELIBBAD:
      error = "Accessing a corrupted shared library";
      break;
#endif
#ifdef ELIBSCN
    case ELIBSCN:
      error = ".lib section in a.out corrupted";
      break;
#endif
#ifdef ELIBMAX
    case ELIBMAX:
      error = "Attempting to link in more shared libraries than system limit";
      break;
#endif
#ifdef ELIBEXEC
    case ELIBEXEC:
      error = "Cannot exec a shared library directly";
      break;
#endif
#ifdef ENOSYS
    case ENOSYS:
      error = "Function not implemented";
      break;
#endif
#ifdef ENMFILE
    case ENMFILE:
      error = "No more files";
      break;
#endif
#ifdef ENOTEMPTY
    case ENOTEMPTY:
      error = "Directory not empty";
      break;
#endif
#ifdef ENAMETOOLONG
    case ENAMETOOLONG:
      error = "File or path name too long";
      break;
#endif
#ifdef ELOOP
    case ELOOP:
      error = "Too many symbolic links";
      break;
#endif
#ifdef ENOBUFS
    case ENOBUFS:
      error = "No buffer space available";
      break;
#endif
#ifdef EAFNOSUPPORT
    case EAFNOSUPPORT:
      error = "Address family not supported by protocol family";
      break;
#endif
#ifdef EPROTOTYPE
    case EPROTOTYPE:
      error = "Protocol wrong type for socket";
      break;
#endif
#ifdef ENOTSOCK
    case ENOTSOCK:
      error = "Socket operation on non-socket";
      break;
#endif
#ifdef ENOPROTOOPT
    case ENOPROTOOPT:
      error = "Protocol not available";
      break;
#endif
#ifdef ESHUTDOWN
    case ESHUTDOWN:
      error = "Can't send after socket shutdown";
      break;
#endif
#ifdef ECONNREFUSED
    case ECONNREFUSED:
      error = "Connection refused";
      break;
#endif
#ifdef EADDRINUSE
    case EADDRINUSE:
      error = "Address already in use";
      break;
#endif
#ifdef ECONNABORTED
    case ECONNABORTED:
      error = "Software caused connection abort";
      break;
#endif
#if (defined(EWOULDBLOCK) && (!defined (EAGAIN) || (EWOULDBLOCK != EAGAIN)))
    case EWOULDBLOCK:
        error = "Operation would block";
        break;
#endif
#ifdef ENOTCONN
    case ENOTCONN:
        error = "Socket is not connected";
        break;
#endif
#ifdef ESOCKTNOSUPPORT
    case ESOCKTNOSUPPORT:
        error = "Socket type not supported";
        break;
#endif
#ifdef EISCONN
    case EISCONN:
        error = "Socket is already connected";
        break;
#endif
#ifdef ECANCELED
    case ECANCELED:
        error = "Operation canceled";
        break;
#endif
#ifdef ENOTRECOVERABLE
    case ENOTRECOVERABLE:
        error = "State not recoverable";
        break;
#endif
#ifdef EOWNERDEAD
    case EOWNERDEAD:
        error = "Previous owner died";
        break;
#endif
#ifdef ESTRPIPE
    case ESTRPIPE:
        error = "Streams pipe error";
        break;
#endif
#if defined(EOPNOTSUPP) && (!defined(ENOTSUP) || (ENOTSUP != EOPNOTSUPP))
    case EOPNOTSUPP:
        error = "Operation not supported on socket";
        break;
#endif
#ifdef EMSGSIZE
    case EMSGSIZE:
        error = "Message too long";
        break;
#endif
#ifdef ETIMEDOUT
    case ETIMEDOUT:
        error = "Connection timed out";
        break;
#endif
    default:
        error = "";
      break;
    }

  return (char*)error;
}

char * strerror(int errnum)
{
  return _strerror_r (errnum, 0, 0);
}
