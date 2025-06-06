/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/* dbus-sysdeps-unix.c Wrappers around UNIX system/libc features (internal to D-Bus implementation)
 *
 * Copyright (C) 2002, 2003, 2006  Red Hat, Inc.
 * Copyright (C) 2003 CodeFactory AB
 *
 * Licensed under the Academic Free License version 2.1
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <config.h>

#include "dbus-internals.h"
#include "dbus-sysdeps.h"
#include "dbus-sysdeps-unix.h"
#include "dbus-threads.h"
#include "dbus-protocol.h"
#include "dbus-transport.h"
#include "dbus-string.h"
#include "dbus-userdb.h"
#include "dbus-list.h"
#include "dbus-credentials.h"
#include "dbus-nonce.h"

#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <dirent.h>
#include <sys/un.h>
#include <pwd.h>
#include <time.h>
#include <locale.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <grp.h>

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_WRITEV
#include <sys/uio.h>
#endif
#ifdef HAVE_POLL
#include <sys/poll.h>
#endif
#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#endif
#ifdef HAVE_GETPEERUCRED
#include <ucred.h>
#endif

#ifdef HAVE_ADT
#include <bsm/adt.h>
#endif

#include "sd-daemon.h"

#ifndef O_BINARY
#define O_BINARY 0
#endif

#ifndef AI_ADDRCONFIG
#define AI_ADDRCONFIG 0
#endif

#ifndef HAVE_SOCKLEN_T
#define socklen_t int
#endif

static dbus_bool_t
_dbus_open_socket (int              *fd_p,
                   int               domain,
                   int               type,
                   int               protocol,
                   DBusError        *error)
{
#ifdef SOCK_CLOEXEC
  dbus_bool_t cloexec_done;

  *fd_p = socket (domain, type | SOCK_CLOEXEC, protocol);
  cloexec_done = *fd_p >= 0;

  /* Check if kernel seems to be too old to know SOCK_CLOEXEC */
  if (*fd_p < 0 && errno == EINVAL)
#endif
    {
      *fd_p = socket (domain, type, protocol);
    }

  if (*fd_p >= 0)
    {
#ifdef SOCK_CLOEXEC
      if (!cloexec_done)
#endif
        {
          _dbus_fd_set_close_on_exec(*fd_p);
        }

      _dbus_verbose ("socket fd %d opened\n", *fd_p);
      return TRUE;
    }
  else
    {
      dbus_set_error(error,
                     _dbus_error_from_errno (errno),
                     "Failed to open socket: %s",
                     _dbus_strerror (errno));
      return FALSE;
    }
}

dbus_bool_t
_dbus_open_tcp_socket (int              *fd,
                       DBusError        *error)
{
  return _dbus_open_socket(fd, AF_INET, SOCK_STREAM, 0, error);
}

/**
 * Opens a UNIX domain socket (as in the socket() call).
 * Does not bind the socket.
 *
 * This will set FD_CLOEXEC for the socket returned
 *
 * @param fd return location for socket descriptor
 * @param error return location for an error
 * @returns #FALSE if error is set
 */
dbus_bool_t
_dbus_open_unix_socket (int              *fd,
                        DBusError        *error)
{
  return _dbus_open_socket(fd, PF_UNIX, SOCK_STREAM, 0, error);
}

/**
 * Closes a socket. Should not be used on non-socket
 * file descriptors or handles.
 *
 * @param fd the socket
 * @param error return location for an error
 * @returns #FALSE if error is set
 */
dbus_bool_t
_dbus_close_socket (int               fd,
                    DBusError        *error)
{
  return _dbus_close (fd, error);
}

/**
 * Like _dbus_read(), but only works on sockets so is
 * available on Windows.
 *
 * @param fd the socket
 * @param buffer string to append data to
 * @param count max amount of data to read
 * @returns number of bytes appended to the string
 */
int
_dbus_read_socket (int               fd,
                   DBusString       *buffer,
                   int               count)
{
  return _dbus_read (fd, buffer, count);
}

/**
 * Like _dbus_write(), but only supports sockets
 * and is thus available on Windows.
 *
 * @param fd the file descriptor to write
 * @param buffer the buffer to write data from
 * @param start the first byte in the buffer to write
 * @param len the number of bytes to try to write
 * @returns the number of bytes written or -1 on error
 */
int
_dbus_write_socket (int               fd,
                    const DBusString *buffer,
                    int               start,
                    int               len)
{
#ifdef MSG_NOSIGNAL
  const char *data;
  int bytes_written;

  data = _dbus_string_get_const_data_len (buffer, start, len);

 again:

  bytes_written = send (fd, data, len, MSG_NOSIGNAL);

  if (bytes_written < 0 && errno == EINTR)
    goto again;

  return bytes_written;

#else
  return _dbus_write (fd, buffer, start, len);
#endif
}

/**
 * Like _dbus_read_socket() but also tries to read unix fds from the
 * socket. When there are more fds to read than space in the array
 * passed this function will fail with ENOSPC.
 *
 * @param fd the socket
 * @param buffer string to append data to
 * @param count max amount of data to read
 * @param fds array to place read file descriptors in
 * @param n_fds on input space in fds array, on output how many fds actually got read
 * @returns number of bytes appended to string
 */
int
_dbus_read_socket_with_unix_fds (int               fd,
                                 DBusString       *buffer,
                                 int               count,
                                 int              *fds,
                                 int              *n_fds) {
#ifndef HAVE_UNIX_FD_PASSING
  int r;

  if ((r = _dbus_read_socket(fd, buffer, count)) < 0)
    return r;

  *n_fds = 0;
  return r;

#else
  int bytes_read;
  int start;
  struct msghdr m;
  struct iovec iov;

  _dbus_assert (count >= 0);
  _dbus_assert (*n_fds >= 0);

  start = _dbus_string_get_length (buffer);

  if (!_dbus_string_lengthen (buffer, count))
    {
      errno = ENOMEM;
      return -1;
    }

  _DBUS_ZERO(iov);
  iov.iov_base = _dbus_string_get_data_len (buffer, start, count);
  iov.iov_len = count;

  _DBUS_ZERO(m);
  m.msg_iov = &iov;
  m.msg_iovlen = 1;

  /* Hmm, we have no clue how long the control data will actually be
     that is queued for us. The least we can do is assume that the
     caller knows. Hence let's make space for the number of fds that
     we shall read at max plus the cmsg header. */
  m.msg_controllen = CMSG_SPACE(*n_fds * sizeof(int));

  /* It's probably safe to assume that systems with SCM_RIGHTS also
     know alloca() */
  m.msg_control = alloca(m.msg_controllen);
  memset(m.msg_control, 0, m.msg_controllen);

 again:

  bytes_read = recvmsg(fd, &m, 0
#ifdef MSG_CMSG_CLOEXEC
                       |MSG_CMSG_CLOEXEC
#endif
                       );

  if (bytes_read < 0)
    {
      if (errno == EINTR)
        goto again;
      else
        {
          /* put length back (note that this doesn't actually realloc anything) */
          _dbus_string_set_length (buffer, start);
          return -1;
        }
    }
  else
    {
      struct cmsghdr *cm;
      dbus_bool_t found = FALSE;

      if (m.msg_flags & MSG_CTRUNC)
        {
          /* Hmm, apparently the control data was truncated. The bad
             thing is that we might have completely lost a couple of fds
             without chance to recover them. Hence let's treat this as a
             serious error. */

          errno = ENOSPC;
          _dbus_string_set_length (buffer, start);
          return -1;
        }

      for (cm = CMSG_FIRSTHDR(&m); cm; cm = CMSG_NXTHDR(&m, cm))
        if (cm->cmsg_level == SOL_SOCKET && cm->cmsg_type == SCM_RIGHTS)
          {
            unsigned i;

            _dbus_assert(cm->cmsg_len <= CMSG_LEN(*n_fds * sizeof(int)));
            *n_fds = (cm->cmsg_len - CMSG_LEN(0)) / sizeof(int);

            memcpy(fds, CMSG_DATA(cm), *n_fds * sizeof(int));
            found = TRUE;

            /* Linux doesn't tell us whether MSG_CMSG_CLOEXEC actually
               worked, hence we need to go through this list and set
               CLOEXEC everywhere in any case */
            for (i = 0; i < *n_fds; i++)
              _dbus_fd_set_close_on_exec(fds[i]);

            break;
          }

      if (!found)
        *n_fds = 0;

      /* put length back (doesn't actually realloc) */
      _dbus_string_set_length (buffer, start + bytes_read);

#if 0
      if (bytes_read > 0)
        _dbus_verbose_bytes_of_string (buffer, start, bytes_read);
#endif

      return bytes_read;
    }
#endif
}

int
_dbus_write_socket_with_unix_fds(int               fd,
                                 const DBusString *buffer,
                                 int               start,
                                 int               len,
                                 const int        *fds,
                                 int               n_fds) {

#ifndef HAVE_UNIX_FD_PASSING

  if (n_fds > 0) {
    errno = ENOTSUP;
    return -1;
  }

  return _dbus_write_socket(fd, buffer, start, len);
#else
  return _dbus_write_socket_with_unix_fds_two(fd, buffer, start, len, NULL, 0, 0, fds, n_fds);
#endif
}

int
_dbus_write_socket_with_unix_fds_two(int               fd,
                                     const DBusString *buffer1,
                                     int               start1,
                                     int               len1,
                                     const DBusString *buffer2,
                                     int               start2,
                                     int               len2,
                                     const int        *fds,
                                     int               n_fds) {

#ifndef HAVE_UNIX_FD_PASSING

  if (n_fds > 0) {
    errno = ENOTSUP;
    return -1;
  }

  return _dbus_write_socket_two(fd,
                                buffer1, start1, len1,
                                buffer2, start2, len2);
#else

  struct msghdr m;
  struct cmsghdr *cm;
  struct iovec iov[2];
  int bytes_written;

  _dbus_assert (len1 >= 0);
  _dbus_assert (len2 >= 0);
  _dbus_assert (n_fds >= 0);

  _DBUS_ZERO(iov);
  iov[0].iov_base = (char*) _dbus_string_get_const_data_len (buffer1, start1, len1);
  iov[0].iov_len = len1;

  if (buffer2)
    {
      iov[1].iov_base = (char*) _dbus_string_get_const_data_len (buffer2, start2, len2);
      iov[1].iov_len = len2;
    }

  _DBUS_ZERO(m);
  m.msg_iov = iov;
  m.msg_iovlen = buffer2 ? 2 : 1;

  if (n_fds > 0)
    {
      m.msg_controllen = CMSG_SPACE(n_fds * sizeof(int));
      m.msg_control = alloca(m.msg_controllen);
      memset(m.msg_control, 0, m.msg_controllen);

      cm = CMSG_FIRSTHDR(&m);
      cm->cmsg_level = SOL_SOCKET;
      cm->cmsg_type = SCM_RIGHTS;
      cm->cmsg_len = CMSG_LEN(n_fds * sizeof(int));
      memcpy(CMSG_DATA(cm), fds, n_fds * sizeof(int));
    }

 again:

  bytes_written = sendmsg (fd, &m, 0
#ifdef MSG_NOSIGNAL
                           |MSG_NOSIGNAL
#endif
                           );

  if (bytes_written < 0 && errno == EINTR)
    goto again;

#if 0
  if (bytes_written > 0)
    _dbus_verbose_bytes_of_string (buffer, start, bytes_written);
#endif

  return bytes_written;
#endif
}

/**
 * Like _dbus_write_two() but only works on sockets and is thus
 * available on Windows.
 *
 * @param fd the file descriptor
 * @param buffer1 first buffer
 * @param start1 first byte to write in first buffer
 * @param len1 number of bytes to write from first buffer
 * @param buffer2 second buffer, or #NULL
 * @param start2 first byte to write in second buffer
 * @param len2 number of bytes to write in second buffer
 * @returns total bytes written from both buffers, or -1 on error
 */
int
_dbus_write_socket_two (int               fd,
                        const DBusString *buffer1,
                        int               start1,
                        int               len1,
                        const DBusString *buffer2,
                        int               start2,
                        int               len2)
{
#ifdef MSG_NOSIGNAL
  struct iovec vectors[2];
  const char *data1;
  const char *data2;
  int bytes_written;
  struct msghdr m;

  _dbus_assert (buffer1 != NULL);
  _dbus_assert (start1 >= 0);
  _dbus_assert (start2 >= 0);
  _dbus_assert (len1 >= 0);
  _dbus_assert (len2 >= 0);

  data1 = _dbus_string_get_const_data_len (buffer1, start1, len1);

  if (buffer2 != NULL)
    data2 = _dbus_string_get_const_data_len (buffer2, start2, len2);
  else
    {
      data2 = NULL;
      start2 = 0;
      len2 = 0;
    }

  vectors[0].iov_base = (char*) data1;
  vectors[0].iov_len = len1;
  vectors[1].iov_base = (char*) data2;
  vectors[1].iov_len = len2;

  _DBUS_ZERO(m);
  m.msg_iov = vectors;
  m.msg_iovlen = data2 ? 2 : 1;

 again:

  bytes_written = sendmsg (fd, &m, MSG_NOSIGNAL);

  if (bytes_written < 0 && errno == EINTR)
    goto again;

  return bytes_written;

#else
  return _dbus_write_two (fd, buffer1, start1, len1,
                          buffer2, start2, len2);
#endif
}

dbus_bool_t
_dbus_socket_is_invalid (int fd)
{
    return fd < 0 ? TRUE : FALSE;
}

/**
 * Thin wrapper around the read() system call that appends
 * the data it reads to the DBusString buffer. It appends
 * up to the given count, and returns the same value
 * and same errno as read(). The only exception is that
 * _dbus_read() handles EINTR for you. Also, _dbus_read() can
 * return ENOMEM, even though regular UNIX read doesn't.
 *
 * Unlike _dbus_read_socket(), _dbus_read() is not available
 * on Windows.
 *
 * @param fd the file descriptor to read from
 * @param buffer the buffer to append data to
 * @param count the amount of data to read
 * @returns the number of bytes read or -1
 */
int
_dbus_read (int               fd,
            DBusString       *buffer,
            int               count)
{
  int bytes_read;
  int start;
  char *data;

  _dbus_assert (count >= 0);

  start = _dbus_string_get_length (buffer);

  if (!_dbus_string_lengthen (buffer, count))
    {
      errno = ENOMEM;
      return -1;
    }

  data = _dbus_string_get_data_len (buffer, start, count);

 again:

  bytes_read = read (fd, data, count);

  if (bytes_read < 0)
    {
      if (errno == EINTR)
        goto again;
      else
        {
          /* put length back (note that this doesn't actually realloc anything) */
          _dbus_string_set_length (buffer, start);
          return -1;
        }
    }
  else
    {
      /* put length back (doesn't actually realloc) */
      _dbus_string_set_length (buffer, start + bytes_read);

#if 0
      if (bytes_read > 0)
        _dbus_verbose_bytes_of_string (buffer, start, bytes_read);
#endif

      return bytes_read;
    }
}

/**
 * Thin wrapper around the write() system call that writes a part of a
 * DBusString and handles EINTR for you.
 *
 * @param fd the file descriptor to write
 * @param buffer the buffer to write data from
 * @param start the first byte in the buffer to write
 * @param len the number of bytes to try to write
 * @returns the number of bytes written or -1 on error
 */
int
_dbus_write (int               fd,
             const DBusString *buffer,
             int               start,
             int               len)
{
  const char *data;
  int bytes_written;

  data = _dbus_string_get_const_data_len (buffer, start, len);

 again:

  bytes_written = write (fd, data, len);

  if (bytes_written < 0 && errno == EINTR)
    goto again;

#if 0
  if (bytes_written > 0)
    _dbus_verbose_bytes_of_string (buffer, start, bytes_written);
#endif

  return bytes_written;
}

/**
 * Like _dbus_write() but will use writev() if possible
 * to write both buffers in sequence. The return value
 * is the number of bytes written in the first buffer,
 * plus the number written in the second. If the first
 * buffer is written successfully and an error occurs
 * writing the second, the number of bytes in the first
 * is returned (i.e. the error is ignored), on systems that
 * don't have writev. Handles EINTR for you.
 * The second buffer may be #NULL.
 *
 * @param fd the file descriptor
 * @param buffer1 first buffer
 * @param start1 first byte to write in first buffer
 * @param len1 number of bytes to write from first buffer
 * @param buffer2 second buffer, or #NULL
 * @param start2 first byte to write in second buffer
 * @param len2 number of bytes to write in second buffer
 * @returns total bytes written from both buffers, or -1 on error
 */
int
_dbus_write_two (int               fd,
                 const DBusString *buffer1,
                 int               start1,
                 int               len1,
                 const DBusString *buffer2,
                 int               start2,
                 int               len2)
{
  _dbus_assert (buffer1 != NULL);
  _dbus_assert (start1 >= 0);
  _dbus_assert (start2 >= 0);
  _dbus_assert (len1 >= 0);
  _dbus_assert (len2 >= 0);

#ifdef HAVE_WRITEV
  {
    struct iovec vectors[2];
    const char *data1;
    const char *data2;
    int bytes_written;

    data1 = _dbus_string_get_const_data_len (buffer1, start1, len1);

    if (buffer2 != NULL)
      data2 = _dbus_string_get_const_data_len (buffer2, start2, len2);
    else
      {
        data2 = NULL;
        start2 = 0;
        len2 = 0;
      }

    vectors[0].iov_base = (char*) data1;
    vectors[0].iov_len = len1;
    vectors[1].iov_base = (char*) data2;
    vectors[1].iov_len = len2;

  again:

    bytes_written = writev (fd,
                            vectors,
                            data2 ? 2 : 1);

    if (bytes_written < 0 && errno == EINTR)
      goto again;

    return bytes_written;
  }
#else /* HAVE_WRITEV */
  {
    int ret1;

    ret1 = _dbus_write (fd, buffer1, start1, len1);
    if (ret1 == len1 && buffer2 != NULL)
      {
        ret2 = _dbus_write (fd, buffer2, start2, len2);
        if (ret2 < 0)
          ret2 = 0; /* we can't report an error as the first write was OK */

        return ret1 + ret2;
      }
    else
      return ret1;
  }
#endif /* !HAVE_WRITEV */
}

#define _DBUS_MAX_SUN_PATH_LENGTH 99

/**
 * @def _DBUS_MAX_SUN_PATH_LENGTH
 *
 * Maximum length of the path to a UNIX domain socket,
 * sockaddr_un::sun_path member. POSIX requires that all systems
 * support at least 100 bytes here, including the nul termination.
 * We use 99 for the max value to allow for the nul.
 *
 * We could probably also do sizeof (addr.sun_path)
 * but this way we are the same on all platforms
 * which is probably a good idea.
 */

/**
 * Creates a socket and connects it to the UNIX domain socket at the
 * given path.  The connection fd is returned, and is set up as
 * nonblocking.
 *
 * Uses abstract sockets instead of filesystem-linked sockets if
 * requested (it's possible only on Linux; see "man 7 unix" on Linux).
 * On non-Linux abstract socket usage always fails.
 *
 * This will set FD_CLOEXEC for the socket returned.
 *
 * @param path the path to UNIX domain socket
 * @param abstract #TRUE to use abstract namespace
 * @param error return location for error code
 * @returns connection file descriptor or -1 on error
 */
int
_dbus_connect_unix_socket (const char     *path,
                           dbus_bool_t     abstract,
                           DBusError      *error)
{
  int fd;
  size_t path_len;
  struct sockaddr_un addr;

  _DBUS_ASSERT_ERROR_IS_CLEAR (error);

  _dbus_verbose ("connecting to unix socket %s abstract=%d\n",
                 path, abstract);


  if (!_dbus_open_unix_socket (&fd, error))
    {
      _DBUS_ASSERT_ERROR_IS_SET(error);
      return -1;
    }
  _DBUS_ASSERT_ERROR_IS_CLEAR(error);

  _DBUS_ZERO (addr);
  addr.sun_family = AF_UNIX;
  path_len = strlen (path);

  if (abstract)
    {
#ifdef HAVE_ABSTRACT_SOCKETS
      addr.sun_path[0] = '\0'; /* this is what says "use abstract" */
      path_len++; /* Account for the extra nul byte added to the start of sun_path */

      if (path_len > _DBUS_MAX_SUN_PATH_LENGTH)
        {
          dbus_set_error (error, DBUS_ERROR_BAD_ADDRESS,
                      "Abstract socket name too long\n");
          _dbus_close (fd, NULL);
          return -1;
	}

      strncpy (&addr.sun_path[1], path, path_len);
      /* _dbus_verbose_bytes (addr.sun_path, sizeof (addr.sun_path)); */
#else /* HAVE_ABSTRACT_SOCKETS */
      dbus_set_error (error, DBUS_ERROR_NOT_SUPPORTED,
                      "Operating system does not support abstract socket namespace\n");
      _dbus_close (fd, NULL);
      return -1;
#endif /* ! HAVE_ABSTRACT_SOCKETS */
    }
  else
    {
      if (path_len > _DBUS_MAX_SUN_PATH_LENGTH)
        {
          dbus_set_error (error, DBUS_ERROR_BAD_ADDRESS,
                      "Socket name too long\n");
          _dbus_close (fd, NULL);
          return -1;
	}

      strncpy (addr.sun_path, path, path_len);
    }

  if (connect (fd, (struct sockaddr*) &addr, _DBUS_STRUCT_OFFSET (struct sockaddr_un, sun_path) + path_len) < 0)
    {
      dbus_set_error (error,
                      _dbus_error_from_errno (errno),
                      "Failed to connect to socket %s: %s",
                      path, _dbus_strerror (errno));

      _dbus_close (fd, NULL);
      fd = -1;

      return -1;
    }

  if (!_dbus_set_fd_nonblocking (fd, error))
    {
      _DBUS_ASSERT_ERROR_IS_SET (error);

      _dbus_close (fd, NULL);
      fd = -1;

      return -1;
    }

  return fd;
}

/**
 * Enables or disables the reception of credentials on the given socket during
 * the next message transmission.  This is only effective if the #LOCAL_CREDS
 * system feature exists, in which case the other side of the connection does
 * not have to do anything special to send the credentials.
 *
 * @param fd socket on which to change the #LOCAL_CREDS flag.
 * @param on whether to enable or disable the #LOCAL_CREDS flag.
 */
static dbus_bool_t
_dbus_set_local_creds (int fd, dbus_bool_t on)
{
  dbus_bool_t retval = TRUE;

#if defined(HAVE_CMSGCRED)
  /* NOOP just to make sure only one codepath is used
   *      and to prefer CMSGCRED
   */
#elif defined(LOCAL_CREDS)
  int val = on ? 1 : 0;
  if (setsockopt (fd, 0, LOCAL_CREDS, &val, sizeof (val)) < 0)
    {
      _dbus_verbose ("Unable to set LOCAL_CREDS socket option on fd %d\n", fd);
      retval = FALSE;
    }
  else
    _dbus_verbose ("LOCAL_CREDS %s for further messages on fd %d\n",
                   on ? "enabled" : "disabled", fd);
#endif

  return retval;
}

/**
 * Creates a socket and binds it to the given path,
 * then listens on the socket. The socket is
 * set to be nonblocking.
 *
 * Uses abstract sockets instead of filesystem-linked
 * sockets if requested (it's possible only on Linux;
 * see "man 7 unix" on Linux).
 * On non-Linux abstract socket usage always fails.
 *
 * This will set FD_CLOEXEC for the socket returned
 *
 * @param path the socket name
 * @param abstract #TRUE to use abstract namespace
 * @param error return location for errors
 * @returns the listening file descriptor or -1 on error
 */
int
_dbus_listen_unix_socket (const char     *path,
                          dbus_bool_t     abstract,
                          DBusError      *error)
{
  int listen_fd;
  struct sockaddr_un addr;
  size_t path_len;
  unsigned int reuseaddr;

  _DBUS_ASSERT_ERROR_IS_CLEAR (error);

  _dbus_verbose ("listening on unix socket %s abstract=%d\n",
                 path, abstract);

  if (!_dbus_open_unix_socket (&listen_fd, error))
    {
      _DBUS_ASSERT_ERROR_IS_SET(error);
      return -1;
    }
  _DBUS_ASSERT_ERROR_IS_CLEAR(error);

  _DBUS_ZERO (addr);
  addr.sun_family = AF_UNIX;
  path_len = strlen (path);

  if (abstract)
    {
#ifdef HAVE_ABSTRACT_SOCKETS
      /* remember that abstract names aren't nul-terminated so we rely
       * on sun_path being filled in with zeroes above.
       */
      addr.sun_path[0] = '\0'; /* this is what says "use abstract" */
      path_len++; /* Account for the extra nul byte added to the start of sun_path */

      if (path_len > _DBUS_MAX_SUN_PATH_LENGTH)
        {
          dbus_set_error (error, DBUS_ERROR_BAD_ADDRESS,
                      "Abstract socket name too long\n");
          _dbus_close (listen_fd, NULL);
          return -1;
	}

      strncpy (&addr.sun_path[1], path, path_len);
      /* _dbus_verbose_bytes (addr.sun_path, sizeof (addr.sun_path)); */
#else /* HAVE_ABSTRACT_SOCKETS */
      dbus_set_error (error, DBUS_ERROR_NOT_SUPPORTED,
                      "Operating system does not support abstract socket namespace\n");
      _dbus_close (listen_fd, NULL);
      return -1;
#endif /* ! HAVE_ABSTRACT_SOCKETS */
    }
  else
    {
      /* Discussed security implications of this with Nalin,
       * and we couldn't think of where it would kick our ass, but
       * it still seems a bit sucky. It also has non-security suckage;
       * really we'd prefer to exit if the socket is already in use.
       * But there doesn't seem to be a good way to do this.
       *
       * Just to be extra careful, I threw in the stat() - clearly
       * the stat() can't *fix* any security issue, but it at least
       * avoids inadvertent/accidental data loss.
       */
      {
        struct stat sb;

        if (stat (path, &sb) == 0 &&
            S_ISSOCK (sb.st_mode))
          unlink (path);
      }

      if (path_len > _DBUS_MAX_SUN_PATH_LENGTH)
        {
          dbus_set_error (error, DBUS_ERROR_BAD_ADDRESS,
                      "Abstract socket name too long\n");
          _dbus_close (listen_fd, NULL);
          return -1;
	}

      strncpy (addr.sun_path, path, path_len);
    }

  reuseaddr = 1;
  if (setsockopt  (listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr))==-1)
    {
      _dbus_warn ("Failed to set socket option\"%s\": %s",
                  path, _dbus_strerror (errno));
    }

  if (bind (listen_fd, (struct sockaddr*) &addr, _DBUS_STRUCT_OFFSET (struct sockaddr_un, sun_path) + path_len) < 0)
    {
      dbus_set_error (error, _dbus_error_from_errno (errno),
                      "Failed to bind socket \"%s\": %s",
                      path, _dbus_strerror (errno));
      _dbus_close (listen_fd, NULL);
      return -1;
    }

  if (listen (listen_fd, 30 /* backlog */) < 0)
    {
      dbus_set_error (error, _dbus_error_from_errno (errno),
                      "Failed to listen on socket \"%s\": %s",
                      path, _dbus_strerror (errno));
      _dbus_close (listen_fd, NULL);
      return -1;
    }

  if (!_dbus_set_local_creds (listen_fd, TRUE))
    {
      dbus_set_error (error, _dbus_error_from_errno (errno),
                      "Failed to enable LOCAL_CREDS on socket \"%s\": %s",
                      path, _dbus_strerror (errno));
      close (listen_fd);
      return -1;
    }

  if (!_dbus_set_fd_nonblocking (listen_fd, error))
    {
      _DBUS_ASSERT_ERROR_IS_SET (error);
      _dbus_close (listen_fd, NULL);
      return -1;
    }

  /* Try opening up the permissions, but if we can't, just go ahead
   * and continue, maybe it will be good enough.
   */
  if (!abstract && chmod (path, 0777) < 0)
    _dbus_warn ("Could not set mode 0777 on socket %s\n",
                path);

  return listen_fd;
}

/**
 * Acquires one or more sockets passed in from systemd. The sockets
 * are set to be nonblocking.
 *
 * This will set FD_CLOEXEC for the sockets returned.
 *
 * @oaram fds the file descriptors
 * @param error return location for errors
 * @returns the number of file descriptors
 */
int
_dbus_listen_systemd_sockets (int       **fds,
                              DBusError *error)
{
  int r, n;
  unsigned fd;
  int *new_fds;

  _DBUS_ASSERT_ERROR_IS_CLEAR (error);

  n = sd_listen_fds (TRUE);
  if (n < 0)
    {
      dbus_set_error (error, _dbus_error_from_errno (-n),
                      "Failed to acquire systemd socket: %s",
                      _dbus_strerror (-n));
      return -1;
    }

  if (n <= 0)
    {
      dbus_set_error (error, DBUS_ERROR_BAD_ADDRESS,
                      "No socket received.");
      return -1;
    }

  for (fd = SD_LISTEN_FDS_START; fd < SD_LISTEN_FDS_START + n; fd ++)
    {
      r = sd_is_socket (fd, AF_UNSPEC, SOCK_STREAM, 1);
      if (r < 0)
        {
          dbus_set_error (error, _dbus_error_from_errno (-r),
                          "Failed to verify systemd socket type: %s",
                          _dbus_strerror (-r));
          return -1;
        }

      if (!r)
        {
          dbus_set_error (error, DBUS_ERROR_BAD_ADDRESS,
                          "Passed socket has wrong type.");
          return -1;
        }
    }

  /* OK, the file descriptors are all good, so let's take posession of
     them then. */

  new_fds = dbus_new (int, n);
  if (!new_fds)
    {
      dbus_set_error (error, DBUS_ERROR_NO_MEMORY,
                      "Failed to allocate file handle array.");
      goto fail;
    }

  for (fd = SD_LISTEN_FDS_START; fd < SD_LISTEN_FDS_START + n; fd ++)
    {
      if (!_dbus_set_local_creds (fd, TRUE))
        {
          dbus_set_error (error, _dbus_error_from_errno (errno),
                          "Failed to enable LOCAL_CREDS on systemd socket: %s",
                          _dbus_strerror (errno));
          goto fail;
        }

      if (!_dbus_set_fd_nonblocking (fd, error))
        {
          _DBUS_ASSERT_ERROR_IS_SET (error);
          goto fail;
        }

      new_fds[fd - SD_LISTEN_FDS_START] = fd;
    }

  *fds = new_fds;
  return n;

 fail:

  for (fd = SD_LISTEN_FDS_START; fd < SD_LISTEN_FDS_START + n; fd ++)
    {
      _dbus_close (fd, NULL);
    }

  dbus_free (new_fds);
  return -1;
}

/**
 * Creates a socket and connects to a socket at the given host
 * and port. The connection fd is returned, and is set up as
 * nonblocking.
 *
 * This will set FD_CLOEXEC for the socket returned
 *
 * @param host the host name to connect to
 * @param port the port to connect to
 * @param family the address family to listen on, NULL for all
 * @param error return location for error code
 * @returns connection file descriptor or -1 on error
 */
int
_dbus_connect_tcp_socket (const char     *host,
                          const char     *port,
                          const char     *family,
                          DBusError      *error)
{
    return _dbus_connect_tcp_socket_with_nonce (host, port, family, (const char*)NULL, error);
}

int
_dbus_connect_tcp_socket_with_nonce (const char     *host,
                                     const char     *port,
                                     const char     *family,
                                     const char     *noncefile,
                                     DBusError      *error)
{
  int saved_errno = 0;
  int fd = -1, res;
  struct addrinfo hints;
  struct addrinfo *ai, *tmp;

  _DBUS_ASSERT_ERROR_IS_CLEAR (error);

  if (!_dbus_open_tcp_socket (&fd, error))
    {
      _DBUS_ASSERT_ERROR_IS_SET(error);
      return -1;
    }

  _DBUS_ASSERT_ERROR_IS_CLEAR(error);

  _DBUS_ZERO (hints);

  if (!family)
    hints.ai_family = AF_UNSPEC;
  else if (!strcmp(family, "ipv4"))
    hints.ai_family = AF_INET;
  else if (!strcmp(family, "ipv6"))
    hints.ai_family = AF_INET6;
  else
    {
      dbus_set_error (error,
                      DBUS_ERROR_BAD_ADDRESS,
                      "Unknown address family %s", family);
      return -1;
    }
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_ADDRCONFIG;

  if ((res = getaddrinfo(host, port, &hints, &ai)) != 0)
    {
      dbus_set_error (error,
                      _dbus_error_from_errno (errno),
                      "Failed to lookup host/port: \"%s:%s\": %s (%d)",
                      host, port, gai_strerror(res), res);
      _dbus_close (fd, NULL);
      return -1;
    }

  tmp = ai;
  while (tmp)
    {
      if (!_dbus_open_socket (&fd, tmp->ai_family, SOCK_STREAM, 0, error))
        {
          freeaddrinfo(ai);
          _DBUS_ASSERT_ERROR_IS_SET(error);
          return -1;
        }
      _DBUS_ASSERT_ERROR_IS_CLEAR(error);

      if (connect (fd, (struct sockaddr*) tmp->ai_addr, tmp->ai_addrlen) < 0)
        {
          saved_errno = errno;
          _dbus_close(fd, NULL);
          fd = -1;
          tmp = tmp->ai_next;
          continue;
        }

      break;
    }
  freeaddrinfo(ai);

  if (fd == -1)
    {
      dbus_set_error (error,
                      _dbus_error_from_errno (saved_errno),
                      "Failed to connect to socket \"%s:%s\" %s",
                      host, port, _dbus_strerror(saved_errno));
      return -1;
    }

  if (noncefile != NULL)
    {
      DBusString noncefileStr;
      dbus_bool_t ret;
      _dbus_string_init_const (&noncefileStr, noncefile);
      ret = _dbus_send_nonce (fd, &noncefileStr, error);
      _dbus_string_free (&noncefileStr);

      if (!ret)
    {
      _dbus_close (fd, NULL);
          return -1;
        }
    }

  if (!_dbus_set_fd_nonblocking (fd, error))
    {
      _dbus_close (fd, NULL);
      return -1;
    }

  return fd;
}

/**
 * Creates a socket and binds it to the given path, then listens on
 * the socket. The socket is set to be nonblocking.  In case of port=0
 * a random free port is used and returned in the port parameter.
 * If inaddr_any is specified, the hostname is ignored.
 *
 * This will set FD_CLOEXEC for the socket returned
 *
 * @param host the host name to listen on
 * @param port the port to listen on, if zero a free port will be used
 * @param family the address family to listen on, NULL for all
 * @param retport string to return the actual port listened on
 * @param fds_p location to store returned file descriptors
 * @param error return location for errors
 * @returns the number of listening file descriptors or -1 on error
 */
int
_dbus_listen_tcp_socket (const char     *host,
                         const char     *port,
                         const char     *family,
                         DBusString     *retport,
                         int           **fds_p,
                         DBusError      *error)
{
  int saved_errno;
  int nlisten_fd = 0, *listen_fd = NULL, res, i;
  struct addrinfo hints;
  struct addrinfo *ai, *tmp;
  unsigned int reuseaddr;

  *fds_p = NULL;
  _DBUS_ASSERT_ERROR_IS_CLEAR (error);

  _DBUS_ZERO (hints);

  if (!family)
    hints.ai_family = AF_UNSPEC;
  else if (!strcmp(family, "ipv4"))
    hints.ai_family = AF_INET;
  else if (!strcmp(family, "ipv6"))
    hints.ai_family = AF_INET6;
  else
    {
      dbus_set_error (error,
                      DBUS_ERROR_BAD_ADDRESS,
                      "Unknown address family %s", family);
      return -1;
    }

  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_ADDRCONFIG | AI_PASSIVE;

 redo_lookup_with_port:
  if ((res = getaddrinfo(host, port, &hints, &ai)) != 0 || !ai)
    {
      dbus_set_error (error,
                      _dbus_error_from_errno (errno),
                      "Failed to lookup host/port: \"%s:%s\": %s (%d)",
                      host ? host : "*", port, gai_strerror(res), res);
      return -1;
    }

  tmp = ai;
  while (tmp)
    {
      int fd = -1, *newlisten_fd;
      if (!_dbus_open_socket (&fd, tmp->ai_family, SOCK_STREAM, 0, error))
        {
          _DBUS_ASSERT_ERROR_IS_SET(error);
          goto failed;
        }
      _DBUS_ASSERT_ERROR_IS_CLEAR(error);

      reuseaddr = 1;
      if (setsockopt (fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr))==-1)
        {
          _dbus_warn ("Failed to set socket option \"%s:%s\": %s",
                      host ? host : "*", port, _dbus_strerror (errno));
        }

      if (bind (fd, (struct sockaddr*) tmp->ai_addr, tmp->ai_addrlen) < 0)
        {
          saved_errno = errno;
          _dbus_close(fd, NULL);
          if (saved_errno == EADDRINUSE)
            {
              /* Depending on kernel policy, it may or may not
                 be neccessary to bind to both IPv4 & 6 addresses
                 so ignore EADDRINUSE here */
              tmp = tmp->ai_next;
              continue;
            }
          dbus_set_error (error, _dbus_error_from_errno (saved_errno),
                          "Failed to bind socket \"%s:%s\": %s",
                          host ? host : "*", port, _dbus_strerror (saved_errno));
          goto failed;
        }

      if (listen (fd, 30 /* backlog */) < 0)
        {
          saved_errno = errno;
          _dbus_close (fd, NULL);
          dbus_set_error (error, _dbus_error_from_errno (saved_errno),
                          "Failed to listen on socket \"%s:%s\": %s",
                          host ? host : "*", port, _dbus_strerror (saved_errno));
          goto failed;
        }

      newlisten_fd = dbus_realloc(listen_fd, sizeof(int)*(nlisten_fd+1));
      if (!newlisten_fd)
        {
          saved_errno = errno;
          _dbus_close (fd, NULL);
          dbus_set_error (error, _dbus_error_from_errno (saved_errno),
                          "Failed to allocate file handle array: %s",
                          _dbus_strerror (saved_errno));
          goto failed;
        }
      listen_fd = newlisten_fd;
      listen_fd[nlisten_fd] = fd;
      nlisten_fd++;

      if (!_dbus_string_get_length(retport))
        {
          /* If the user didn't specify a port, or used 0, then
             the kernel chooses a port. After the first address
             is bound to, we need to force all remaining addresses
             to use the same port */
          if (!port || !strcmp(port, "0"))
            {
              struct sockaddr_storage addr;
              socklen_t addrlen;
              char portbuf[50];

              addrlen = sizeof(addr);
              getsockname(fd, (struct sockaddr*) &addr, &addrlen);

              if ((res = getnameinfo((struct sockaddr*)&addr, addrlen, NULL, 0,
                                     portbuf, sizeof(portbuf),
                                     NI_NUMERICHOST)) != 0)
                {
                  dbus_set_error (error, _dbus_error_from_errno (errno),
                                  "Failed to resolve port \"%s:%s\": %s (%s)",
                                  host ? host : "*", port, gai_strerror(res), res);
                  goto failed;
                }
              if (!_dbus_string_append(retport, portbuf))
                {
                  dbus_set_error (error, DBUS_ERROR_NO_MEMORY, NULL);
                  goto failed;
                }

              /* Release current address list & redo lookup */
              port = _dbus_string_get_const_data(retport);
              freeaddrinfo(ai);
              goto redo_lookup_with_port;
            }
          else
            {
              if (!_dbus_string_append(retport, port))
                {
                    dbus_set_error (error, DBUS_ERROR_NO_MEMORY, NULL);
                    goto failed;
                }
            }
        }

      tmp = tmp->ai_next;
    }
  freeaddrinfo(ai);
  ai = NULL;

  if (!nlisten_fd)
    {
      errno = EADDRINUSE;
      dbus_set_error (error, _dbus_error_from_errno (errno),
                      "Failed to bind socket \"%s:%s\": %s",
                      host ? host : "*", port, _dbus_strerror (errno));
      return -1;
    }

  for (i = 0 ; i < nlisten_fd ; i++)
    {
      if (!_dbus_set_fd_nonblocking (listen_fd[i], error))
        {
          goto failed;
        }
    }

  *fds_p = listen_fd;

  return nlisten_fd;

 failed:
  if (ai)
    freeaddrinfo(ai);
  for (i = 0 ; i < nlisten_fd ; i++)
    _dbus_close(listen_fd[i], NULL);
  dbus_free(listen_fd);
  return -1;
}

static dbus_bool_t
write_credentials_byte (int             server_fd,
                        DBusError      *error)
{
  int bytes_written;
  char buf[1] = { '\0' };
#if defined(HAVE_CMSGCRED)
  union {
	  struct cmsghdr hdr;
	  char cred[CMSG_SPACE (sizeof (struct cmsgcred))];
  } cmsg;
  struct iovec iov;
  struct msghdr msg;
  iov.iov_base = buf;
  iov.iov_len = 1;

  _DBUS_ZERO(msg);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

  msg.msg_control = (caddr_t) &cmsg;
  msg.msg_controllen = CMSG_SPACE (sizeof (struct cmsgcred));
  _DBUS_ZERO(cmsg);
  cmsg.hdr.cmsg_len = CMSG_LEN (sizeof (struct cmsgcred));
  cmsg.hdr.cmsg_level = SOL_SOCKET;
  cmsg.hdr.cmsg_type = SCM_CREDS;
#endif

  _DBUS_ASSERT_ERROR_IS_CLEAR (error);

 again:

#if defined(HAVE_CMSGCRED)
  bytes_written = sendmsg (server_fd, &msg, 0);
#else
  bytes_written = write (server_fd, buf, 1);
#endif

  if (bytes_written < 0 && errno == EINTR)
    goto again;

  if (bytes_written < 0)
    {
      dbus_set_error (error, _dbus_error_from_errno (errno),
                      "Failed to write credentials byte: %s",
                     _dbus_strerror (errno));
      return FALSE;
    }
  else if (bytes_written == 0)
    {
      dbus_set_error (error, DBUS_ERROR_IO_ERROR,
                      "wrote zero bytes writing credentials byte");
      return FALSE;
    }
  else
    {
      _dbus_assert (bytes_written == 1);
      _dbus_verbose ("wrote credentials byte\n");
      return TRUE;
    }
}

/**
 * Reads a single byte which must be nul (an error occurs otherwise),
 * and reads unix credentials if available. Clears the credentials
 * object, then adds pid/uid if available, so any previous credentials
 * stored in the object are lost.
 *
 * Return value indicates whether a byte was read, not whether
 * we got valid credentials. On some systems, such as Linux,
 * reading/writing the byte isn't actually required, but we do it
 * anyway just to avoid multiple codepaths.
 *
 * Fails if no byte is available, so you must select() first.
 *
 * The point of the byte is that on some systems we have to
 * use sendmsg()/recvmsg() to transmit credentials.
 *
 * @param client_fd the client file descriptor
 * @param credentials object to add client credentials to
 * @param error location to store error code
 * @returns #TRUE on success
 */
dbus_bool_t
_dbus_read_credentials_socket  (int              client_fd,
                                DBusCredentials *credentials,
                                DBusError       *error)
{
  struct msghdr msg;
  struct iovec iov;
  char buf;
  dbus_uid_t uid_read;
  dbus_pid_t pid_read;
  int bytes_read;

#ifdef HAVE_CMSGCRED
  union {
    struct cmsghdr hdr;
    char cred[CMSG_SPACE (sizeof (struct cmsgcred))];
  } cmsg;

#elif defined(LOCAL_CREDS)
  struct {
    struct cmsghdr hdr;
    struct sockcred cred;
  } cmsg;
#endif

  uid_read = DBUS_UID_UNSET;
  pid_read = DBUS_PID_UNSET;

  _DBUS_ASSERT_ERROR_IS_CLEAR (error);

  /* The POSIX spec certainly doesn't promise this, but
   * we need these assertions to fail as soon as we're wrong about
   * it so we can do the porting fixups
   */
  _dbus_assert (sizeof (pid_t) <= sizeof (dbus_pid_t));
  _dbus_assert (sizeof (uid_t) <= sizeof (dbus_uid_t));
  _dbus_assert (sizeof (gid_t) <= sizeof (dbus_gid_t));

  _dbus_credentials_clear (credentials);

  /* Systems supporting LOCAL_CREDS are configured to have this feature
   * enabled (if it does not conflict with HAVE_CMSGCRED) prior accepting
   * the connection.  Therefore, the received message must carry the
   * credentials information without doing anything special.
   */

  iov.iov_base = &buf;
  iov.iov_len = 1;

  _DBUS_ZERO(msg);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

#if defined(HAVE_CMSGCRED) || defined(LOCAL_CREDS)
  _DBUS_ZERO(cmsg);
  msg.msg_control = (caddr_t) &cmsg;
  msg.msg_controllen = CMSG_SPACE (sizeof (struct cmsgcred));
#endif

 again:
  bytes_read = recvmsg (client_fd, &msg, 0);

  if (bytes_read < 0)
    {
      if (errno == EINTR)
	goto again;

      /* EAGAIN or EWOULDBLOCK would be unexpected here since we would
       * normally only call read_credentials if the socket was ready
       * for reading
       */

      dbus_set_error (error, _dbus_error_from_errno (errno),
                      "Failed to read credentials byte: %s",
                      _dbus_strerror (errno));
      return FALSE;
    }
  else if (bytes_read == 0)
    {
      /* this should not happen unless we are using recvmsg wrong,
       * so is essentially here for paranoia
       */
      dbus_set_error (error, DBUS_ERROR_FAILED,
                      "Failed to read credentials byte (zero-length read)");
      return FALSE;
    }
  else if (buf != '\0')
    {
      dbus_set_error (error, DBUS_ERROR_FAILED,
                      "Credentials byte was not nul");
      return FALSE;
    }

#if defined(HAVE_CMSGCRED) || defined(LOCAL_CREDS)
  if (cmsg.hdr.cmsg_len < CMSG_LEN (sizeof (struct cmsgcred))
		  || cmsg.hdr.cmsg_type != SCM_CREDS)
    {
      dbus_set_error (error, DBUS_ERROR_FAILED,
                      "Message from recvmsg() was not SCM_CREDS");
      return FALSE;
    }
#endif

  _dbus_verbose ("read credentials byte\n");

  {
#ifdef SO_PEERCRED
    struct ucred cr;
    int cr_len = sizeof (cr);

    if (getsockopt (client_fd, SOL_SOCKET, SO_PEERCRED, &cr, &cr_len) == 0 &&
	cr_len == sizeof (cr))
      {
	pid_read = cr.pid;
	uid_read = cr.uid;
      }
    else
      {
	_dbus_verbose ("Failed to getsockopt() credentials, returned len %d/%d: %s\n",
		       cr_len, (int) sizeof (cr), _dbus_strerror (errno));
      }
#elif defined(HAVE_CMSGCRED)
    struct cmsgcred *cred;

    cred = (struct cmsgcred *) CMSG_DATA (&cmsg.hdr);
    pid_read = cred->cmcred_pid;
    uid_read = cred->cmcred_euid;
#elif defined(LOCAL_CREDS)
    pid_read = DBUS_PID_UNSET;
    uid_read = cmsg.cred.sc_uid;
    /* Since we have already got the credentials from this socket, we can
     * disable its LOCAL_CREDS flag if it was ever set. */
    _dbus_set_local_creds (client_fd, FALSE);
#elif defined(HAVE_GETPEEREID)
    uid_t euid;
    gid_t egid;
    if (getpeereid (client_fd, &euid, &egid) == 0)
      {
        uid_read = euid;
      }
    else
      {
        _dbus_verbose ("Failed to getpeereid() credentials: %s\n", _dbus_strerror (errno));
      }
#elif defined(HAVE_GETPEERUCRED)
    ucred_t * ucred = NULL;
    if (getpeerucred (client_fd, &ucred) == 0)
      {
        pid_read = ucred_getpid (ucred);
        uid_read = ucred_geteuid (ucred);
#ifdef HAVE_ADT
        /* generate audit session data based on socket ucred */
        adt_session_data_t *adth = NULL;
        adt_export_data_t *data = NULL;
        size_t size = 0;
        if (adt_start_session (&adth, NULL, 0) || (adth == NULL))
          {
            _dbus_verbose ("Failed to adt_start_session(): %s\n", _dbus_strerror (errno));
          }
        else
          {
            if (adt_set_from_ucred (adth, ucred, ADT_NEW))
              {
                _dbus_verbose ("Failed to adt_set_from_ucred(): %s\n", _dbus_strerror (errno));
              }
            else
              {
                size = adt_export_session_data (adth, &data);
                if (size <= 0)
                  {
                    _dbus_verbose ("Failed to adt_export_session_data(): %s\n", _dbus_strerror (errno));
                  }
                else
                  {
                    _dbus_credentials_add_adt_audit_data (credentials, data, size);
                    free (data);
                  }
              }
            (void) adt_end_session (adth);
          }
#endif /* HAVE_ADT */
      }
    else
      {
        _dbus_verbose ("Failed to getpeerucred() credentials: %s\n", _dbus_strerror (errno));
      }
    if (ucred != NULL)
      ucred_free (ucred);
#else /* !SO_PEERCRED && !HAVE_CMSGCRED && !HAVE_GETPEEREID && !HAVE_GETPEERUCRED */
    _dbus_verbose ("Socket credentials not supported on this OS\n");
#endif
  }

  _dbus_verbose ("Credentials:"
                 "  pid "DBUS_PID_FORMAT
                 "  uid "DBUS_UID_FORMAT
                 "\n",
		 pid_read,
		 uid_read);

  if (pid_read != DBUS_PID_UNSET)
    {
      if (!_dbus_credentials_add_unix_pid (credentials, pid_read))
        {
          _DBUS_SET_OOM (error);
          return FALSE;
        }
    }

  if (uid_read != DBUS_UID_UNSET)
    {
      if (!_dbus_credentials_add_unix_uid (credentials, uid_read))
        {
          _DBUS_SET_OOM (error);
          return FALSE;
        }
    }

  return TRUE;
}

/**
 * Sends a single nul byte with our UNIX credentials as ancillary
 * data.  Returns #TRUE if the data was successfully written.  On
 * systems that don't support sending credentials, just writes a byte,
 * doesn't send any credentials.  On some systems, such as Linux,
 * reading/writing the byte isn't actually required, but we do it
 * anyway just to avoid multiple codepaths.
 *
 * Fails if no byte can be written, so you must select() first.
 *
 * The point of the byte is that on some systems we have to
 * use sendmsg()/recvmsg() to transmit credentials.
 *
 * @param server_fd file descriptor for connection to server
 * @param error return location for error code
 * @returns #TRUE if the byte was sent
 */
dbus_bool_t
_dbus_send_credentials_socket  (int              server_fd,
                                DBusError       *error)
{
  _DBUS_ASSERT_ERROR_IS_CLEAR (error);

  if (write_credentials_byte (server_fd, error))
    return TRUE;
  else
    return FALSE;
}

/**
 * Accepts a connection on a listening socket.
 * Handles EINTR for you.
 *
 * This will enable FD_CLOEXEC for the returned socket.
 *
 * @param listen_fd the listen file descriptor
 * @returns the connection fd of the client, or -1 on error
 */
int
_dbus_accept  (int listen_fd)
{
  int client_fd;
  struct sockaddr addr;
  socklen_t addrlen;
#ifdef HAVE_ACCEPT4
  dbus_bool_t cloexec_done;
#endif

  addrlen = sizeof (addr);

 retry:

#ifdef HAVE_ACCEPT4
  /* We assume that if accept4 is available SOCK_CLOEXEC is too */
  client_fd = accept4 (listen_fd, &addr, &addrlen, SOCK_CLOEXEC);
  cloexec_done = client_fd >= 0;

  if (client_fd < 0 && errno == ENOSYS)
#endif
    {
      client_fd = accept (listen_fd, &addr, &addrlen);
    }

  if (client_fd < 0)
    {
      if (errno == EINTR)
        goto retry;
    }

  _dbus_verbose ("client fd %d accepted\n", client_fd);

#ifdef HAVE_ACCEPT4
  if (!cloexec_done)
#endif
    {
      _dbus_fd_set_close_on_exec(client_fd);
    }

  return client_fd;
}

/**
 * Checks to make sure the given directory is
 * private to the user
 *
 * @param dir the name of the directory
 * @param error error return
 * @returns #FALSE on failure
 **/
dbus_bool_t
_dbus_check_dir_is_private_to_user (DBusString *dir, DBusError *error)
{
  const char *directory;
  struct stat sb;

  _DBUS_ASSERT_ERROR_IS_CLEAR (error);

  directory = _dbus_string_get_const_data (dir);

  if (stat (directory, &sb) < 0)
    {
      dbus_set_error (error, _dbus_error_from_errno (errno),
                      "%s", _dbus_strerror (errno));

      return FALSE;
    }

  if ((S_IROTH & sb.st_mode) || (S_IWOTH & sb.st_mode) ||
      (S_IRGRP & sb.st_mode) || (S_IWGRP & sb.st_mode))
    {
      dbus_set_error (error, DBUS_ERROR_FAILED,
                     "%s directory is not private to the user", directory);
      return FALSE;
    }

  return TRUE;
}

static dbus_bool_t
fill_user_info_from_passwd (struct passwd *p,
                            DBusUserInfo  *info,
                            DBusError     *error)
{
  _dbus_assert (p->pw_name != NULL);
  _dbus_assert (p->pw_dir != NULL);

  info->uid = p->pw_uid;
  info->primary_gid = p->pw_gid;
  info->username = _dbus_strdup (p->pw_name);
  info->homedir = _dbus_strdup (p->pw_dir);

  if (info->username == NULL ||
      info->homedir == NULL)
    {
      dbus_set_error (error, DBUS_ERROR_NO_MEMORY, NULL);
      return FALSE;
    }

  return TRUE;
}

static dbus_bool_t
fill_user_info (DBusUserInfo       *info,
                dbus_uid_t          uid,
                const DBusString   *username,
                DBusError          *error)
{
  const char *username_c;

  /* exactly one of username/uid provided */
  _dbus_assert (username != NULL || uid != DBUS_UID_UNSET);
  _dbus_assert (username == NULL || uid == DBUS_UID_UNSET);

  info->uid = DBUS_UID_UNSET;
  info->primary_gid = DBUS_GID_UNSET;
  info->group_ids = NULL;
  info->n_group_ids = 0;
  info->username = NULL;
  info->homedir = NULL;

  if (username != NULL)
    username_c = _dbus_string_get_const_data (username);
  else
    username_c = NULL;

  /* For now assuming that the getpwnam() and getpwuid() flavors
   * are always symmetrical, if not we have to add more configure
   * checks
   */

#if defined (HAVE_POSIX_GETPWNAM_R) || defined (HAVE_NONPOSIX_GETPWNAM_R)
  {
    struct passwd *p;
    int result;
    size_t buflen;
    char *buf;
    struct passwd p_str;

    /* retrieve maximum needed size for buf */
    buflen = sysconf (_SC_GETPW_R_SIZE_MAX);

    /* sysconf actually returns a long, but everything else expects size_t,
     * so just recast here.
     * https://bugs.freedesktop.org/show_bug.cgi?id=17061
     */
    if ((long) buflen <= 0)
      buflen = 1024;

    result = -1;
    while (1)
      {
        buf = dbus_malloc (buflen);
        if (buf == NULL)
          {
            dbus_set_error (error, DBUS_ERROR_NO_MEMORY, NULL);
            return FALSE;
          }

        p = NULL;
#ifdef HAVE_POSIX_GETPWNAM_R
        if (uid != DBUS_UID_UNSET)
          result = getpwuid_r (uid, &p_str, buf, buflen,
                               &p);
        else
          result = getpwnam_r (username_c, &p_str, buf, buflen,
                               &p);
#else
        if (uid != DBUS_UID_UNSET)
          p = getpwuid_r (uid, &p_str, buf, buflen);
        else
          p = getpwnam_r (username_c, &p_str, buf, buflen);
        result = 0;
#endif /* !HAVE_POSIX_GETPWNAM_R */
        //Try a bigger buffer if ERANGE was returned
        if (result == ERANGE && buflen < 512 * 1024)
          {
            dbus_free (buf);
            buflen *= 2;
          }
        else
          {
            break;
          }
      }
    if (result == 0 && p == &p_str)
      {
        if (!fill_user_info_from_passwd (p, info, error))
          {
            dbus_free (buf);
            return FALSE;
          }
        dbus_free (buf);
      }
    else
      {
        dbus_set_error (error, _dbus_error_from_errno (errno),
                        "User \"%s\" unknown or no memory to allocate password entry\n",
                        username_c ? username_c : "???");
        _dbus_verbose ("User %s unknown\n", username_c ? username_c : "???");
        dbus_free (buf);
        return FALSE;
      }
  }
#else /* ! HAVE_GETPWNAM_R */
  {
    /* I guess we're screwed on thread safety here */
    struct passwd *p;

    if (uid != DBUS_UID_UNSET)
      p = getpwuid (uid);
    else
      p = getpwnam (username_c);

    if (p != NULL)
      {
        if (!fill_user_info_from_passwd (p, info, error))
          {
            return FALSE;
          }
      }
    else
      {
        dbus_set_error (error, _dbus_error_from_errno (errno),
                        "User \"%s\" unknown or no memory to allocate password entry\n",
                        username_c ? username_c : "???");
        _dbus_verbose ("User %s unknown\n", username_c ? username_c : "???");
        return FALSE;
      }
  }
#endif  /* ! HAVE_GETPWNAM_R */

  /* Fill this in so we can use it to get groups */
  username_c = info->username;

#ifdef HAVE_GETGROUPLIST
  {
    gid_t *buf;
    int buf_count;
    int i;
    int initial_buf_count;

    initial_buf_count = 17;
    buf_count = initial_buf_count;
    buf = dbus_new (gid_t, buf_count);
    if (buf == NULL)
      {
        dbus_set_error (error, DBUS_ERROR_NO_MEMORY, NULL);
        goto failed;
      }

    if (getgrouplist (username_c,
                      info->primary_gid,
                      buf, &buf_count) < 0)
      {
        gid_t *new;
        /* Presumed cause of negative return code: buf has insufficient
           entries to hold the entire group list. The Linux behavior in this
           case is to pass back the actual number of groups in buf_count, but
           on Mac OS X 10.5, buf_count is unhelpfully left alone.
           So as a hack, try to help out a bit by guessing a larger
           number of groups, within reason.. might still fail, of course,
           but we can at least print a more informative message.  I looked up
           the "right way" to do this by downloading Apple's own source code
           for the "id" command, and it turns out that they use an
           undocumented library function getgrouplist_2 (!) which is not
           declared in any header in /usr/include (!!). That did not seem
           like the way to go here.
        */
        if (buf_count == initial_buf_count)
          {
            buf_count *= 16; /* Retry with an arbitrarily scaled-up array */
          }
        new = dbus_realloc (buf, buf_count * sizeof (buf[0]));
        if (new == NULL)
          {
            dbus_set_error (error, DBUS_ERROR_NO_MEMORY, NULL);
            dbus_free (buf);
            goto failed;
          }

        buf = new;

        errno = 0;
        if (getgrouplist (username_c, info->primary_gid, buf, &buf_count) < 0)
          {
            if (errno == 0)
              {
                _dbus_warn ("It appears that username \"%s\" is in more than %d groups.\nProceeding with just the first %d groups.",
                            username_c, buf_count, buf_count);
              }
            else
              {
                dbus_set_error (error,
                                _dbus_error_from_errno (errno),
                                "Failed to get groups for username \"%s\" primary GID "
                                DBUS_GID_FORMAT ": %s\n",
                                username_c, info->primary_gid,
                                _dbus_strerror (errno));
                dbus_free (buf);
                goto failed;
              }
          }
      }

    info->group_ids = dbus_new (dbus_gid_t, buf_count);
    if (info->group_ids == NULL)
      {
        dbus_set_error (error, DBUS_ERROR_NO_MEMORY, NULL);
        dbus_free (buf);
        goto failed;
      }

    for (i = 0; i < buf_count; ++i)
      info->group_ids[i] = buf[i];

    info->n_group_ids = buf_count;

    dbus_free (buf);
  }
#else  /* HAVE_GETGROUPLIST */
  {
    /* We just get the one group ID */
    info->group_ids = dbus_new (dbus_gid_t, 1);
    if (info->group_ids == NULL)
      {
        dbus_set_error (error, DBUS_ERROR_NO_MEMORY, NULL);
        goto failed;
      }

    info->n_group_ids = 1;

    (info->group_ids)[0] = info->primary_gid;
  }
#endif /* HAVE_GETGROUPLIST */

  _DBUS_ASSERT_ERROR_IS_CLEAR (error);

  return TRUE;

 failed:
  _DBUS_ASSERT_ERROR_IS_SET (error);
  return FALSE;
}

/**
 * Gets user info for the given username.
 *
 * @param info user info object to initialize
 * @param username the username
 * @param error error return
 * @returns #TRUE on success
 */
dbus_bool_t
_dbus_user_info_fill (DBusUserInfo     *info,
                      const DBusString *username,
                      DBusError        *error)
{
  return fill_user_info (info, DBUS_UID_UNSET,
                         username, error);
}

/**
 * Gets user info for the given user ID.
 *
 * @param info user info object to initialize
 * @param uid the user ID
 * @param error error return
 * @returns #TRUE on success
 */
dbus_bool_t
_dbus_user_info_fill_uid (DBusUserInfo *info,
                          dbus_uid_t    uid,
                          DBusError    *error)
{
  return fill_user_info (info, uid,
                         NULL, error);
}

/**
 * Adds the credentials of the current process to the
 * passed-in credentials object.
 *
 * @param credentials credentials to add to
 * @returns #FALSE if no memory; does not properly roll back on failure, so only some credentials may have been added
 */
dbus_bool_t
_dbus_credentials_add_from_current_process (DBusCredentials *credentials)
{
  /* The POSIX spec certainly doesn't promise this, but
   * we need these assertions to fail as soon as we're wrong about
   * it so we can do the porting fixups
   */
  _dbus_assert (sizeof (pid_t) <= sizeof (dbus_pid_t));
  _dbus_assert (sizeof (uid_t) <= sizeof (dbus_uid_t));
  _dbus_assert (sizeof (gid_t) <= sizeof (dbus_gid_t));

  if (!_dbus_credentials_add_unix_pid(credentials, _dbus_getpid()))
    return FALSE;
  if (!_dbus_credentials_add_unix_uid(credentials, _dbus_geteuid()))
    return FALSE;

  return TRUE;
}

/**
 * Append to the string the identity we would like to have when we
 * authenticate, on UNIX this is the current process UID and on
 * Windows something else, probably a Windows SID string.  No escaping
 * is required, that is done in dbus-auth.c. The username here
 * need not be anything human-readable, it can be the machine-readable
 * form i.e. a user id.
 *
 * @param str the string to append to
 * @returns #FALSE on no memory
 */
dbus_bool_t
_dbus_append_user_from_current_process (DBusString *str)
{
  return _dbus_string_append_uint (str,
                                   _dbus_geteuid ());
}

/**
 * Gets our process ID
 * @returns process ID
 */
dbus_pid_t
_dbus_getpid (void)
{
  return getpid ();
}

/** Gets our UID
 * @returns process UID
 */
dbus_uid_t
_dbus_getuid (void)
{
  return getuid ();
}

/** Gets our effective UID
 * @returns process effective UID
 */
dbus_uid_t
_dbus_geteuid (void)
{
  return geteuid ();
}

/**
 * The only reason this is separate from _dbus_getpid() is to allow it
 * on Windows for logging but not for other purposes.
 *
 * @returns process ID to put in log messages
 */
unsigned long
_dbus_pid_for_log (void)
{
  return getpid ();
}

/**
 * Gets a UID from a UID string.
 *
 * @param uid_str the UID in string form
 * @param uid UID to fill in
 * @returns #TRUE if successfully filled in UID
 */
dbus_bool_t
_dbus_parse_uid (const DBusString      *uid_str,
                 dbus_uid_t            *uid)
{
  int end;
  long val;

  if (_dbus_string_get_length (uid_str) == 0)
    {
      _dbus_verbose ("UID string was zero length\n");
      return FALSE;
    }

  val = -1;
  end = 0;
  if (!_dbus_string_parse_int (uid_str, 0, &val,
                               &end))
    {
      _dbus_verbose ("could not parse string as a UID\n");
      return FALSE;
    }

  if (end != _dbus_string_get_length (uid_str))
    {
      _dbus_verbose ("string contained trailing stuff after UID\n");
      return FALSE;
    }

  *uid = val;

  return TRUE;
}

#if !DBUS_USE_SYNC
_DBUS_DEFINE_GLOBAL_LOCK (atomic);
#endif

/**
 * Atomically increments an integer
 *
 * @param atomic pointer to the integer to increment
 * @returns the value before incrementing
 */
dbus_int32_t
_dbus_atomic_inc (DBusAtomic *atomic)
{
#if DBUS_USE_SYNC
  return __sync_add_and_fetch(&atomic->value, 1)-1;
#else
  dbus_int32_t res;
  _DBUS_LOCK (atomic);
  res = atomic->value;
  atomic->value += 1;
  _DBUS_UNLOCK (atomic);
  return res;
#endif
}

/**
 * Atomically decrement an integer
 *
 * @param atomic pointer to the integer to decrement
 * @returns the value before decrementing
 */
dbus_int32_t
_dbus_atomic_dec (DBusAtomic *atomic)
{
#if DBUS_USE_SYNC
  return __sync_sub_and_fetch(&atomic->value, 1)+1;
#else
  dbus_int32_t res;

  _DBUS_LOCK (atomic);
  res = atomic->value;
  atomic->value -= 1;
  _DBUS_UNLOCK (atomic);
  return res;
#endif
}

#ifdef DBUS_BUILD_TESTS
/** Gets our GID
 * @returns process GID
 */
dbus_gid_t
_dbus_getgid (void)
{
  return getgid ();
}
#endif

/**
 * Wrapper for poll().
 *
 * @param fds the file descriptors to poll
 * @param n_fds number of descriptors in the array
 * @param timeout_milliseconds timeout or -1 for infinite
 * @returns numbers of fds with revents, or <0 on error
 */
int
_dbus_poll (DBusPollFD *fds,
            int         n_fds,
            int         timeout_milliseconds)
{
#if defined(HAVE_POLL) && !defined(BROKEN_POLL)
  /* This big thing is a constant expression and should get optimized
   * out of existence. So it's more robust than a configure check at
   * no cost.
   */
  if (_DBUS_POLLIN == POLLIN &&
      _DBUS_POLLPRI == POLLPRI &&
      _DBUS_POLLOUT == POLLOUT &&
      _DBUS_POLLERR == POLLERR &&
      _DBUS_POLLHUP == POLLHUP &&
      _DBUS_POLLNVAL == POLLNVAL &&
      sizeof (DBusPollFD) == sizeof (struct pollfd) &&
      _DBUS_STRUCT_OFFSET (DBusPollFD, fd) ==
      _DBUS_STRUCT_OFFSET (struct pollfd, fd) &&
      _DBUS_STRUCT_OFFSET (DBusPollFD, events) ==
      _DBUS_STRUCT_OFFSET (struct pollfd, events) &&
      _DBUS_STRUCT_OFFSET (DBusPollFD, revents) ==
      _DBUS_STRUCT_OFFSET (struct pollfd, revents))
    {
      return poll ((struct pollfd*) fds,
                   n_fds,
                   timeout_milliseconds);
    }
  else
    {
      /* We have to convert the DBusPollFD to an array of
       * struct pollfd, poll, and convert back.
       */
      _dbus_warn ("didn't implement poll() properly for this system yet\n");
      return -1;
    }
#else /* ! HAVE_POLL */

  fd_set read_set, write_set, err_set;
  int max_fd = 0;
  int i;
  struct timeval tv;
  int ready;

  FD_ZERO (&read_set);
  FD_ZERO (&write_set);
  FD_ZERO (&err_set);

  for (i = 0; i < n_fds; i++)
    {
      DBusPollFD *fdp = &fds[i];

      if (fdp->events & _DBUS_POLLIN)
	FD_SET (fdp->fd, &read_set);

      if (fdp->events & _DBUS_POLLOUT)
	FD_SET (fdp->fd, &write_set);

      FD_SET (fdp->fd, &err_set);

      max_fd = MAX (max_fd, fdp->fd);
    }

  tv.tv_sec = timeout_milliseconds / 1000;
  tv.tv_usec = (timeout_milliseconds % 1000) * 1000;

  ready = select (max_fd + 1, &read_set, &write_set, &err_set,
                  timeout_milliseconds < 0 ? NULL : &tv);

  if (ready > 0)
    {
      for (i = 0; i < n_fds; i++)
	{
	  DBusPollFD *fdp = &fds[i];

	  fdp->revents = 0;

	  if (FD_ISSET (fdp->fd, &read_set))
	    fdp->revents |= _DBUS_POLLIN;

	  if (FD_ISSET (fdp->fd, &write_set))
	    fdp->revents |= _DBUS_POLLOUT;

	  if (FD_ISSET (fdp->fd, &err_set))
	    fdp->revents |= _DBUS_POLLERR;
	}
    }

  return ready;
#endif
}

/**
 * Get current time, as in gettimeofday(). Use the monotonic clock if
 * available, to avoid problems when the system time changes.
 *
 * @param tv_sec return location for number of seconds
 * @param tv_usec return location for number of microseconds (thousandths)
 */
void
_dbus_get_current_time (long *tv_sec,
                        long *tv_usec)
{
  struct timeval t;

#ifdef HAVE_MONOTONIC_CLOCK
  struct timespec ts;
  clock_gettime (CLOCK_MONOTONIC, &ts);

  if (tv_sec)
    *tv_sec = ts.tv_sec;
  if (tv_usec)
    *tv_usec = ts.tv_nsec / 1000;
#else
  gettimeofday (&t, NULL);

  if (tv_sec)
    *tv_sec = t.tv_sec;
  if (tv_usec)
    *tv_usec = t.tv_usec;
#endif
}

/**
 * Creates a directory; succeeds if the directory
 * is created or already existed.
 *
 * @param filename directory filename
 * @param error initialized error object
 * @returns #TRUE on success
 */
dbus_bool_t
_dbus_create_directory (const DBusString *filename,
                        DBusError        *error)
{
  const char *filename_c;

  _DBUS_ASSERT_ERROR_IS_CLEAR (error);

  filename_c = _dbus_string_get_const_data (filename);

  if (mkdir (filename_c, 0700) < 0)
    {
      if (errno == EEXIST)
        return TRUE;

      dbus_set_error (error, DBUS_ERROR_FAILED,
                      "Failed to create directory %s: %s\n",
                      filename_c, _dbus_strerror (errno));
      return FALSE;
    }
  else
    return TRUE;
}

/**
 * Appends the given filename to the given directory.
 *
 * @todo it might be cute to collapse multiple '/' such as "foo//"
 * concat "//bar"
 *
 * @param dir the directory name
 * @param next_component the filename
 * @returns #TRUE on success
 */
dbus_bool_t
_dbus_concat_dir_and_file (DBusString       *dir,
                           const DBusString *next_component)
{
  dbus_bool_t dir_ends_in_slash;
  dbus_bool_t file_starts_with_slash;

  if (_dbus_string_get_length (dir) == 0 ||
      _dbus_string_get_length (next_component) == 0)
    return TRUE;

  dir_ends_in_slash = '/' == _dbus_string_get_byte (dir,
                                                    _dbus_string_get_length (dir) - 1);

  file_starts_with_slash = '/' == _dbus_string_get_byte (next_component, 0);

  if (dir_ends_in_slash && file_starts_with_slash)
    {
      _dbus_string_shorten (dir, 1);
    }
  else if (!(dir_ends_in_slash || file_starts_with_slash))
    {
      if (!_dbus_string_append_byte (dir, '/'))
        return FALSE;
    }

  return _dbus_string_copy (next_component, 0, dir,
                            _dbus_string_get_length (dir));
}

/** nanoseconds in a second */
#define NANOSECONDS_PER_SECOND       1000000000
/** microseconds in a second */
#define MICROSECONDS_PER_SECOND      1000000
/** milliseconds in a second */
#define MILLISECONDS_PER_SECOND      1000
/** nanoseconds in a millisecond */
#define NANOSECONDS_PER_MILLISECOND  1000000
/** microseconds in a millisecond */
#define MICROSECONDS_PER_MILLISECOND 1000

/**
 * Sleeps the given number of milliseconds.
 * @param milliseconds number of milliseconds
 */
void
_dbus_sleep_milliseconds (int milliseconds)
{
#ifdef HAVE_NANOSLEEP
  struct timespec req;
  struct timespec rem;

  req.tv_sec = milliseconds / MILLISECONDS_PER_SECOND;
  req.tv_nsec = (milliseconds % MILLISECONDS_PER_SECOND) * NANOSECONDS_PER_MILLISECOND;
  rem.tv_sec = 0;
  rem.tv_nsec = 0;

  while (nanosleep (&req, &rem) < 0 && errno == EINTR)
    req = rem;
#elif defined (HAVE_USLEEP)
  usleep (milliseconds * MICROSECONDS_PER_MILLISECOND);
#else /* ! HAVE_USLEEP */
  sleep (MAX (milliseconds / 1000, 1));
#endif
}

static dbus_bool_t
_dbus_generate_pseudorandom_bytes (DBusString *str,
                                   int         n_bytes)
{
  int old_len;
  char *p;

  old_len = _dbus_string_get_length (str);

  if (!_dbus_string_lengthen (str, n_bytes))
    return FALSE;

  p = _dbus_string_get_data_len (str, old_len, n_bytes);

  _dbus_generate_pseudorandom_bytes_buffer (p, n_bytes);

  return TRUE;
}

/**
 * Generates the given number of random bytes,
 * using the best mechanism we can come up with.
 *
 * @param str the string
 * @param n_bytes the number of random bytes to append to string
 * @returns #TRUE on success, #FALSE if no memory
 */
dbus_bool_t
_dbus_generate_random_bytes (DBusString *str,
                             int         n_bytes)
{
  int old_len;
  int fd;

  /* FALSE return means "no memory", if it could
   * mean something else then we'd need to return
   * a DBusError. So we always fall back to pseudorandom
   * if the I/O fails.
   */

  old_len = _dbus_string_get_length (str);
  fd = -1;

  /* note, urandom on linux will fall back to pseudorandom */
  fd = open ("/dev/urandom", O_RDONLY);
  if (fd < 0)
    return _dbus_generate_pseudorandom_bytes (str, n_bytes);

  _dbus_verbose ("/dev/urandom fd %d opened\n", fd);

  if (_dbus_read (fd, str, n_bytes) != n_bytes)
    {
      _dbus_close (fd, NULL);
      _dbus_string_set_length (str, old_len);
      return _dbus_generate_pseudorandom_bytes (str, n_bytes);
    }

  _dbus_verbose ("Read %d bytes from /dev/urandom\n",
                 n_bytes);

  _dbus_close (fd, NULL);

  return TRUE;
}

/**
 * Exit the process, returning the given value.
 *
 * @param code the exit code
 */
void
_dbus_exit (int code)
{
  _exit (code);
}

/**
 * A wrapper around strerror() because some platforms
 * may be lame and not have strerror(). Also, never
 * returns NULL.
 *
 * @param error_number errno.
 * @returns error description.
 */
const char*
_dbus_strerror (int error_number)
{
  const char *msg;

  msg = strerror (error_number);
  if (msg == NULL)
    msg = "unknown";

  return msg;
}

/**
 * signal (SIGPIPE, SIG_IGN);
 */
void
_dbus_disable_sigpipe (void)
{
  signal (SIGPIPE, SIG_IGN);
}

/**
 * Sets the file descriptor to be close
 * on exec. Should be called for all file
 * descriptors in D-Bus code.
 *
 * @param fd the file descriptor
 */
void
_dbus_fd_set_close_on_exec (intptr_t fd)
{
  int val;

  val = fcntl (fd, F_GETFD, 0);

  if (val < 0)
    return;

  val |= FD_CLOEXEC;

  fcntl (fd, F_SETFD, val);
}

/**
 * Closes a file descriptor.
 *
 * @param fd the file descriptor
 * @param error error object
 * @returns #FALSE if error set
 */
dbus_bool_t
_dbus_close (int        fd,
             DBusError *error)
{
  _DBUS_ASSERT_ERROR_IS_CLEAR (error);

 again:
  if (close (fd) < 0)
    {
      if (errno == EINTR)
        goto again;

      dbus_set_error (error, _dbus_error_from_errno (errno),
                      "Could not close fd %d", fd);
      return FALSE;
    }

  return TRUE;
}

/**
 * Duplicates a file descriptor. Makes sure the fd returned is >= 3
 * (i.e. avoids stdin/stdout/stderr). Sets O_CLOEXEC.
 *
 * @param fd the file descriptor to duplicate
 * @returns duplicated file descriptor
 * */
int
_dbus_dup(int        fd,
          DBusError *error)
{
  int new_fd;

#ifdef F_DUPFD_CLOEXEC
  dbus_bool_t cloexec_done;

  new_fd = fcntl(fd, F_DUPFD_CLOEXEC, 3);
  cloexec_done = new_fd >= 0;

  if (new_fd < 0 && errno == EINVAL)
#endif
    {
      new_fd = fcntl(fd, F_DUPFD, 3);
    }

  if (new_fd < 0) {

    dbus_set_error (error, _dbus_error_from_errno (errno),
                    "Could not duplicate fd %d", fd);
    return -1;
  }

#ifdef F_DUPFD_CLOEXEC
  if (!cloexec_done)
#endif
    {
      _dbus_fd_set_close_on_exec(new_fd);
    }

  return new_fd;
}

/**
 * Sets a file descriptor to be nonblocking.
 *
 * @param fd the file descriptor.
 * @param error address of error location.
 * @returns #TRUE on success.
 */
dbus_bool_t
_dbus_set_fd_nonblocking (int             fd,
                          DBusError      *error)
{
  int val;

  _DBUS_ASSERT_ERROR_IS_CLEAR (error);

  val = fcntl (fd, F_GETFL, 0);
  if (val < 0)
    {
      dbus_set_error (error, _dbus_error_from_errno (errno),
                      "Failed to get flags from file descriptor %d: %s",
                      fd, _dbus_strerror (errno));
      _dbus_verbose ("Failed to get flags for fd %d: %s\n", fd,
                     _dbus_strerror (errno));
      return FALSE;
    }

  if (fcntl (fd, F_SETFL, val | O_NONBLOCK) < 0)
    {
      dbus_set_error (error, _dbus_error_from_errno (errno),
                      "Failed to set nonblocking flag of file descriptor %d: %s",
                      fd, _dbus_strerror (errno));
      _dbus_verbose ("Failed to set fd %d nonblocking: %s\n",
                     fd, _dbus_strerror (errno));

      return FALSE;
    }

  return TRUE;
}

/**
 * On GNU libc systems, print a crude backtrace to stderr.  On other
 * systems, print "no backtrace support" and block for possible gdb
 * attachment if an appropriate environment variable is set.
 */
void
_dbus_print_backtrace (void)
{
#if defined (HAVE_BACKTRACE) && defined (DBUS_BUILT_R_DYNAMIC)
  void *bt[500];
  int bt_size;
  int i;
  char **syms;

  bt_size = backtrace (bt, 500);

  syms = backtrace_symbols (bt, bt_size);

  i = 0;
  while (i < bt_size)
    {
      /* don't use dbus_warn since it can _dbus_abort() */
      fprintf (stderr, "  %s\n", syms[i]);
      ++i;
    }
  fflush (stderr);

  free (syms);
#elif defined (HAVE_BACKTRACE) && ! defined (DBUS_BUILT_R_DYNAMIC)
  fprintf (stderr, "  D-Bus not built with -rdynamic so unable to print a backtrace\n");
#else
  fprintf (stderr, "  D-Bus not compiled with backtrace support so unable to print a backtrace\n");
#endif
}

/**
 * Creates a full-duplex pipe (as in socketpair()).
 * Sets both ends of the pipe nonblocking.
 *
 * Marks both file descriptors as close-on-exec
 *
 * @todo libdbus only uses this for the debug-pipe server, so in
 * principle it could be in dbus-sysdeps-util.c, except that
 * dbus-sysdeps-util.c isn't in libdbus when tests are enabled and the
 * debug-pipe server is used.
 *
 * @param fd1 return location for one end
 * @param fd2 return location for the other end
 * @param blocking #TRUE if pipe should be blocking
 * @param error error return
 * @returns #FALSE on failure (if error is set)
 */
dbus_bool_t
_dbus_full_duplex_pipe (int        *fd1,
                        int        *fd2,
                        dbus_bool_t blocking,
                        DBusError  *error)
{
#ifdef HAVE_SOCKETPAIR
  int fds[2];
  int retval;

#ifdef SOCK_CLOEXEC
  dbus_bool_t cloexec_done;

  retval = socketpair(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC, 0, fds);
  cloexec_done = retval >= 0;

  if (retval < 0 && errno == EINVAL)
#endif
    {
      retval = socketpair(AF_UNIX, SOCK_STREAM, 0, fds);
    }

  if (retval < 0)
    {
      dbus_set_error (error, _dbus_error_from_errno (errno),
                      "Could not create full-duplex pipe");
      return FALSE;
    }

  _DBUS_ASSERT_ERROR_IS_CLEAR (error);

#ifdef SOCK_CLOEXEC
  if (!cloexec_done)
#endif
    {
      _dbus_fd_set_close_on_exec (fds[0]);
      _dbus_fd_set_close_on_exec (fds[1]);
    }

  if (!blocking &&
      (!_dbus_set_fd_nonblocking (fds[0], NULL) ||
       !_dbus_set_fd_nonblocking (fds[1], NULL)))
    {
      dbus_set_error (error, _dbus_error_from_errno (errno),
                      "Could not set full-duplex pipe nonblocking");

      _dbus_close (fds[0], NULL);
      _dbus_close (fds[1], NULL);

      return FALSE;
    }

  *fd1 = fds[0];
  *fd2 = fds[1];

  _dbus_verbose ("full-duplex pipe %d <-> %d\n",
                 *fd1, *fd2);

  return TRUE;
#else
  _dbus_warn ("_dbus_full_duplex_pipe() not implemented on this OS\n");
  dbus_set_error (error, DBUS_ERROR_FAILED,
                  "_dbus_full_duplex_pipe() not implemented on this OS");
  return FALSE;
#endif
}

/**
 * Measure the length of the given format string and arguments,
 * not including the terminating nul.
 *
 * @param format a printf-style format string
 * @param args arguments for the format string
 * @returns length of the given format string and args
 */
int
_dbus_printf_string_upper_bound (const char *format,
                                 va_list     args)
{
  char c;
  return vsnprintf (&c, 1, format, args);
}

/**
 * Gets the temporary files directory by inspecting the environment variables
 * TMPDIR, TMP, and TEMP in that order. If none of those are set "/tmp" is returned
 *
 * @returns location of temp directory
 */
const char*
_dbus_get_tmpdir(void)
{
  static const char* tmpdir = NULL;

  if (tmpdir == NULL)
    {
      /* TMPDIR is what glibc uses, then
       * glibc falls back to the P_tmpdir macro which
       * just expands to "/tmp"
       */
      if (tmpdir == NULL)
        tmpdir = getenv("TMPDIR");

      /* These two env variables are probably
       * broken, but maybe some OS uses them?
       */
      if (tmpdir == NULL)
        tmpdir = getenv("TMP");
      if (tmpdir == NULL)
        tmpdir = getenv("TEMP");

      /* And this is the sane fallback. */
      if (tmpdir == NULL)
        tmpdir = "/tmp";
    }

  _dbus_assert(tmpdir != NULL);

  return tmpdir;
}

/**
 * Execute a subprocess, returning up to 1024 bytes of output
 * into @p result.
 *
 * If successful, returns #TRUE and appends the output to @p
 * result. If a failure happens, returns #FALSE and
 * sets an error in @p error.
 *
 * @note It's not an error if the subprocess terminates normally
 * without writing any data to stdout. Verify the @p result length
 * before and after this function call to cover this case.
 *
 * @param progname initial path to exec (may or may not be absolute)
 * @param path_fallback if %TRUE, search PATH for executable
 * @param argv NULL-terminated list of arguments
 * @param result a DBusString where the output can be append
 * @param error a DBusError to store the error in case of failure
 * @returns #TRUE on success, #FALSE if an error happened
 */
static dbus_bool_t
_read_subprocess_line_argv (const char *progpath,
                            dbus_bool_t path_fallback,
                            char       * const *argv,
                            DBusString *result,
                            DBusError  *error)
{
  int result_pipe[2] = { -1, -1 };
  int errors_pipe[2] = { -1, -1 };
  pid_t pid;
  int ret;
  int status;
  int orig_len;
  int i;

  dbus_bool_t retval;
  sigset_t new_set, old_set;

  _DBUS_ASSERT_ERROR_IS_CLEAR (error);
  retval = FALSE;

  /* We need to block any existing handlers for SIGCHLD temporarily; they
   * will cause waitpid() below to fail.
   * https://bugs.freedesktop.org/show_bug.cgi?id=21347
   */
  sigemptyset (&new_set);
  sigaddset (&new_set, SIGCHLD);
  sigprocmask (SIG_BLOCK, &new_set, &old_set);

  orig_len = _dbus_string_get_length (result);

#define READ_END        0
#define WRITE_END       1
  if (pipe (result_pipe) < 0)
    {
      dbus_set_error (error, _dbus_error_from_errno (errno),
                      "Failed to create a pipe to call %s: %s",
                      progpath, _dbus_strerror (errno));
      _dbus_verbose ("Failed to create a pipe to call %s: %s\n",
                     progpath, _dbus_strerror (errno));
      goto out;
    }
  if (pipe (errors_pipe) < 0)
    {
      dbus_set_error (error, _dbus_error_from_errno (errno),
                      "Failed to create a pipe to call %s: %s",
                      progpath, _dbus_strerror (errno));
      _dbus_verbose ("Failed to create a pipe to call %s: %s\n",
                     progpath, _dbus_strerror (errno));
      goto out;
    }

  pid = fork ();
  if (pid < 0)
    {
      dbus_set_error (error, _dbus_error_from_errno (errno),
                      "Failed to fork() to call %s: %s",
                      progpath, _dbus_strerror (errno));
      _dbus_verbose ("Failed to fork() to call %s: %s\n",
                     progpath, _dbus_strerror (errno));
      goto out;
    }

  if (pid == 0)
    {
      /* child process */
      int maxfds;
      int fd;

      fd = open ("/dev/null", O_RDWR);
      if (fd == -1)
        /* huh?! can't open /dev/null? */
        _exit (1);

      _dbus_verbose ("/dev/null fd %d opened\n", fd);

      /* set-up stdXXX */
      close (result_pipe[READ_END]);
      close (errors_pipe[READ_END]);
      close (0);                /* close stdin */
      close (1);                /* close stdout */
      close (2);                /* close stderr */

      if (dup2 (fd, 0) == -1)
        _exit (1);
      if (dup2 (result_pipe[WRITE_END], 1) == -1)
        _exit (1);
      if (dup2 (errors_pipe[WRITE_END], 2) == -1)
        _exit (1);

      maxfds = sysconf (_SC_OPEN_MAX);
      /* Pick something reasonable if for some reason sysconf
       * says unlimited.
       */
      if (maxfds < 0)
        maxfds = 1024;
      /* close all inherited fds */
      for (i = 3; i < maxfds; i++)
        close (i);

      sigprocmask (SIG_SETMASK, &old_set, NULL);

      /* If it looks fully-qualified, try execv first */
      if (progpath[0] == '/')
        {
          execv (progpath, argv);
          /* Ok, that failed.  Now if path_fallback is given, let's
           * try unqualified.  This is mostly a hack to work
           * around systems which ship dbus-launch in /usr/bin
           * but everything else in /bin (because dbus-launch
           * depends on X11).
           */
          if (path_fallback)
            /* We must have a slash, because we checked above */
            execvp (strrchr (progpath, '/')+1, argv);
        }
      else
        execvp (progpath, argv);

      /* still nothing, we failed */
      _exit (1);
    }

  /* parent process */
  close (result_pipe[WRITE_END]);
  close (errors_pipe[WRITE_END]);
  result_pipe[WRITE_END] = -1;
  errors_pipe[WRITE_END] = -1;

  ret = 0;
  do
    {
      ret = _dbus_read (result_pipe[READ_END], result, 1024);
    }
  while (ret > 0);

  /* reap the child process to avoid it lingering as zombie */
  do
    {
      ret = waitpid (pid, &status, 0);
    }
  while (ret == -1 && errno == EINTR);

  /* We succeeded if the process exited with status 0 and
     anything was read */
  if (!WIFEXITED (status) || WEXITSTATUS (status) != 0 )
    {
      /* The process ended with error */
      DBusString error_message;
      _dbus_string_init (&error_message);
      ret = 0;
      do
        {
          ret = _dbus_read (errors_pipe[READ_END], &error_message, 1024);
        }
      while (ret > 0);

      _dbus_string_set_length (result, orig_len);
      if (_dbus_string_get_length (&error_message) > 0)
        dbus_set_error (error, DBUS_ERROR_SPAWN_EXEC_FAILED,
                        "%s terminated abnormally with the following error: %s",
                        progpath, _dbus_string_get_data (&error_message));
      else
        dbus_set_error (error, DBUS_ERROR_SPAWN_EXEC_FAILED,
                        "%s terminated abnormally without any error message",
                        progpath);
      goto out;
    }

  retval = TRUE;

 out:
  sigprocmask (SIG_SETMASK, &old_set, NULL);

  if (retval)
    _DBUS_ASSERT_ERROR_IS_CLEAR (error);
  else
    _DBUS_ASSERT_ERROR_IS_SET (error);

  if (result_pipe[0] != -1)
    close (result_pipe[0]);
  if (result_pipe[1] != -1)
    close (result_pipe[1]);
  if (errors_pipe[0] != -1)
    close (errors_pipe[0]);
  if (errors_pipe[1] != -1)
    close (errors_pipe[1]);

  return retval;
}

/**
 * Returns the address of a new session bus.
 *
 * If successful, returns #TRUE and appends the address to @p
 * address. If a failure happens, returns #FALSE and
 * sets an error in @p error.
 *
 * @param address a DBusString where the address can be stored
 * @param error a DBusError to store the error in case of failure
 * @returns #TRUE on success, #FALSE if an error happened
 */
dbus_bool_t
_dbus_get_autolaunch_address (DBusString *address,
                              DBusError  *error)
{
  static char *argv[6];
  int i;
  DBusString uuid;
  dbus_bool_t retval;

  _DBUS_ASSERT_ERROR_IS_CLEAR (error);
  retval = FALSE;

  if (!_dbus_string_init (&uuid))
    {
      _DBUS_SET_OOM (error);
      return FALSE;
    }

  if (!_dbus_get_local_machine_uuid_encoded (&uuid))
    {
      _DBUS_SET_OOM (error);
      goto out;
    }

  i = 0;
  argv[i] = "dbus-launch";
  ++i;
  argv[i] = "--autolaunch";
  ++i;
  argv[i] = _dbus_string_get_data (&uuid);
  ++i;
  argv[i] = "--binary-syntax";
  ++i;
  argv[i] = "--close-stderr";
  ++i;
  argv[i] = NULL;
  ++i;

  _dbus_assert (i == _DBUS_N_ELEMENTS (argv));

  retval = _read_subprocess_line_argv (DBUS_BINDIR "/dbus-launch",
                                       TRUE,
                                       argv, address, error);

 out:
  _dbus_string_free (&uuid);
  return retval;
}

/**
 * Reads the uuid of the machine we're running on from
 * the dbus configuration. Optionally try to create it
 * (only root can do this usually).
 *
 * On UNIX, reads a file that gets created by dbus-uuidgen
 * in a post-install script. On Windows, if there's a standard
 * machine uuid we could just use that, but I can't find one
 * with the right properties (the hardware profile guid can change
 * without rebooting I believe). If there's no standard one
 * we might want to use the registry instead of a file for
 * this, and I'm not sure how we'd ensure the uuid gets created.
 *
 * @param machine_id guid to init with the machine's uuid
 * @param create_if_not_found try to create the uuid if it doesn't exist
 * @param error the error return
 * @returns #FALSE if the error is set
 */
dbus_bool_t
_dbus_read_local_machine_uuid (DBusGUID   *machine_id,
                               dbus_bool_t create_if_not_found,
                               DBusError  *error)
{
  DBusString filename;
  _dbus_string_init_const (&filename, DBUS_MACHINE_UUID_FILE);
  return _dbus_read_uuid_file (&filename, machine_id, create_if_not_found, error);
}

#define DBUS_UNIX_STANDARD_SESSION_SERVICEDIR "/dbus-1/services"
#define DBUS_UNIX_STANDARD_SYSTEM_SERVICEDIR "/dbus-1/system-services"

/**
 * Determines the address of the session bus by querying a
 * platform-specific method.
 *
 * The first parameter will be a boolean specifying whether
 * or not a dynamic session lookup is supported on this platform.
 *
 * If supported is TRUE and the return value is #TRUE, the
 * address will be  appended to @p address.
 * If a failure happens, returns #FALSE and sets an error in
 * @p error.
 *
 * If supported is FALSE, ignore the return value.
 *
 * @param supported returns whether this method is supported
 * @param address a DBusString where the address can be stored
 * @param error a DBusError to store the error in case of failure
 * @returns #TRUE on success, #FALSE if an error happened
 */
dbus_bool_t
_dbus_lookup_session_address (dbus_bool_t *supported,
                              DBusString  *address,
                              DBusError   *error)
{
  /* On non-Mac Unix platforms, if the session address isn't already
   * set in DBUS_SESSION_BUS_ADDRESS environment variable, we punt and
   * fall back to the autolaunch: global default; see
   * init_session_address in dbus/dbus-bus.c. */
  *supported = FALSE;
  return TRUE;
}

/**
 * Returns the standard directories for a session bus to look for service
 * activation files
 *
 * On UNIX this should be the standard xdg freedesktop.org data directories:
 *
 * XDG_DATA_HOME=${XDG_DATA_HOME-$HOME/.local/share}
 * XDG_DATA_DIRS=${XDG_DATA_DIRS-/usr/local/share:/usr/share}
 *
 * and
 *
 * DBUS_DATADIR
 *
 * @param dirs the directory list we are returning
 * @returns #FALSE on OOM
 */

dbus_bool_t
_dbus_get_standard_session_servicedirs (DBusList **dirs)
{
  const char *xdg_data_home;
  const char *xdg_data_dirs;
  DBusString servicedir_path;

  if (!_dbus_string_init (&servicedir_path))
    return FALSE;

  xdg_data_home = _dbus_getenv ("XDG_DATA_HOME");
  xdg_data_dirs = _dbus_getenv ("XDG_DATA_DIRS");

  if (xdg_data_dirs != NULL)
    {
      if (!_dbus_string_append (&servicedir_path, xdg_data_dirs))
        goto oom;

      if (!_dbus_string_append (&servicedir_path, ":"))
        goto oom;
    }
  else
    {
      if (!_dbus_string_append (&servicedir_path, "/usr/local/share:/usr/share:"))
        goto oom;
    }

  /*
   * add configured datadir to defaults
   * this may be the same as an xdg dir
   * however the config parser should take
   * care of duplicates
   */
  if (!_dbus_string_append (&servicedir_path, DBUS_DATADIR":"))
        goto oom;

  if (xdg_data_home != NULL)
    {
      if (!_dbus_string_append (&servicedir_path, xdg_data_home))
        goto oom;
    }
  else
    {
      const DBusString *homedir;
      DBusString local_share;

      if (!_dbus_homedir_from_current_process (&homedir))
        goto oom;

      if (!_dbus_string_append (&servicedir_path, _dbus_string_get_const_data (homedir)))
        goto oom;

      _dbus_string_init_const (&local_share, "/.local/share");
      if (!_dbus_concat_dir_and_file (&servicedir_path, &local_share))
        goto oom;
    }

  if (!_dbus_split_paths_and_append (&servicedir_path,
                                     DBUS_UNIX_STANDARD_SESSION_SERVICEDIR,
                                     dirs))
    goto oom;

  _dbus_string_free (&servicedir_path);
  return TRUE;

 oom:
  _dbus_string_free (&servicedir_path);
  return FALSE;
}


/**
 * Returns the standard directories for a system bus to look for service
 * activation files
 *
 * On UNIX this should be the standard xdg freedesktop.org data directories:
 *
 * XDG_DATA_DIRS=${XDG_DATA_DIRS-/usr/local/share:/usr/share}
 *
 * and
 *
 * DBUS_DATADIR
 *
 * On Windows there is no system bus and this function can return nothing.
 *
 * @param dirs the directory list we are returning
 * @returns #FALSE on OOM
 */

dbus_bool_t
_dbus_get_standard_system_servicedirs (DBusList **dirs)
{
  const char *xdg_data_dirs;
  DBusString servicedir_path;

  if (!_dbus_string_init (&servicedir_path))
    return FALSE;

  xdg_data_dirs = _dbus_getenv ("XDG_DATA_DIRS");

  if (xdg_data_dirs != NULL)
    {
      if (!_dbus_string_append (&servicedir_path, xdg_data_dirs))
        goto oom;

      if (!_dbus_string_append (&servicedir_path, ":"))
        goto oom;
    }
  else
    {
      if (!_dbus_string_append (&servicedir_path, "/usr/local/share:/usr/share:"))
        goto oom;
    }

  /*
   * add configured datadir to defaults
   * this may be the same as an xdg dir
   * however the config parser should take
   * care of duplicates
   */
  if (!_dbus_string_append (&servicedir_path, DBUS_DATADIR":"))
        goto oom;

  if (!_dbus_split_paths_and_append (&servicedir_path,
                                     DBUS_UNIX_STANDARD_SYSTEM_SERVICEDIR,
                                     dirs))
    goto oom;

  _dbus_string_free (&servicedir_path);
  return TRUE;

 oom:
  _dbus_string_free (&servicedir_path);
  return FALSE;
}

/**
 * Append the absolute path of the system.conf file
 * (there is no system bus on Windows so this can just
 * return FALSE and print a warning or something)
 *
 * @param str the string to append to
 * @returns #FALSE if no memory
 */
dbus_bool_t
_dbus_append_system_config_file (DBusString *str)
{
  return _dbus_string_append (str, DBUS_SYSTEM_CONFIG_FILE);
}

/**
 * Append the absolute path of the session.conf file.
 *
 * @param str the string to append to
 * @returns #FALSE if no memory
 */
dbus_bool_t
_dbus_append_session_config_file (DBusString *str)
{
  return _dbus_string_append (str, DBUS_SESSION_CONFIG_FILE);
}

/**
 * Called when the bus daemon is signaled to reload its configuration; any
 * caches should be nuked. Of course any caches that need explicit reload
 * are probably broken, but c'est la vie.
 *
 *
 */
void
_dbus_flush_caches (void)
{
  _dbus_user_database_flush_system ();
}

/**
 * Appends the directory in which a keyring for the given credentials
 * should be stored.  The credentials should have either a Windows or
 * UNIX user in them.  The directory should be an absolute path.
 *
 * On UNIX the directory is ~/.dbus-keyrings while on Windows it should probably
 * be something else, since the dotfile convention is not normal on Windows.
 *
 * @param directory string to append directory to
 * @param credentials credentials the directory should be for
 *
 * @returns #FALSE on no memory
 */
dbus_bool_t
_dbus_append_keyring_directory_for_credentials (DBusString      *directory,
                                                DBusCredentials *credentials)
{
  DBusString homedir;
  DBusString dotdir;
  dbus_uid_t uid;

  _dbus_assert (credentials != NULL);
  _dbus_assert (!_dbus_credentials_are_anonymous (credentials));

  if (!_dbus_string_init (&homedir))
    return FALSE;

  uid = _dbus_credentials_get_unix_uid (credentials);
  _dbus_assert (uid != DBUS_UID_UNSET);

  if (!_dbus_homedir_from_uid (uid, &homedir))
    goto failed;

#ifdef DBUS_BUILD_TESTS
  {
    const char *override;

    override = _dbus_getenv ("DBUS_TEST_HOMEDIR");
    if (override != NULL && *override != '\0')
      {
        _dbus_string_set_length (&homedir, 0);
        if (!_dbus_string_append (&homedir, override))
          goto failed;

        _dbus_verbose ("Using fake homedir for testing: %s\n",
                       _dbus_string_get_const_data (&homedir));
      }
    else
      {
        static dbus_bool_t already_warned = FALSE;
        if (!already_warned)
          {
            _dbus_warn ("Using your real home directory for testing, set DBUS_TEST_HOMEDIR to avoid\n");
            already_warned = TRUE;
          }
      }
  }
#endif

  _dbus_string_init_const (&dotdir, ".dbus-keyrings");
  if (!_dbus_concat_dir_and_file (&homedir,
                                  &dotdir))
    goto failed;

  if (!_dbus_string_copy (&homedir, 0,
                          directory, _dbus_string_get_length (directory))) {
    goto failed;
  }

  _dbus_string_free (&homedir);
  return TRUE;

 failed:
  _dbus_string_free (&homedir);
  return FALSE;
}

//PENDING(kdab) docs
void
_dbus_daemon_publish_session_bus_address (const char* addr)
{

}

//PENDING(kdab) docs
void
_dbus_daemon_unpublish_session_bus_address (void)
{

}

/**
 * See if errno is EAGAIN or EWOULDBLOCK (this has to be done differently
 * for Winsock so is abstracted)
 *
 * @returns #TRUE if errno == EAGAIN or errno == EWOULDBLOCK
 */
dbus_bool_t
_dbus_get_is_errno_eagain_or_ewouldblock (void)
{
  return errno == EAGAIN || errno == EWOULDBLOCK;
}

/**
 * Removes a directory; Directory must be empty
 *
 * @param filename directory filename
 * @param error initialized error object
 * @returns #TRUE on success
 */
dbus_bool_t
_dbus_delete_directory (const DBusString *filename,
                        DBusError        *error)
{
  const char *filename_c;

  _DBUS_ASSERT_ERROR_IS_CLEAR (error);

  filename_c = _dbus_string_get_const_data (filename);

  if (rmdir (filename_c) != 0)
    {
      dbus_set_error (error, DBUS_ERROR_FAILED,
                      "Failed to remove directory %s: %s\n",
                      filename_c, _dbus_strerror (errno));
      return FALSE;
    }

  return TRUE;
}

/**
 *  Checks whether file descriptors may be passed via the socket
 *
 *  @param fd the socket
 *  @return TRUE when fd passing over this socket is supported
 *
 */
dbus_bool_t
_dbus_socket_can_pass_unix_fd(int fd) {

#ifdef SCM_RIGHTS
  union {
    struct sockaddr sa;
    struct sockaddr_storage storage;
    struct sockaddr_un un;
  } sa_buf;

  socklen_t sa_len = sizeof(sa_buf);

  _DBUS_ZERO(sa_buf);

  if (getsockname(fd, &sa_buf.sa, &sa_len) < 0)
    return FALSE;

  return sa_buf.sa.sa_family == AF_UNIX;

#else
  return FALSE;

#endif
}


/*
 * replaces the term DBUS_PREFIX in configure_time_path by the
 * current dbus installation directory. On unix this function is a noop
 *
 * @param configure_time_path
 * @return real path
 */
const char *
_dbus_replace_install_prefix (const char *configure_time_path)
{
  return configure_time_path;
}

/* tests in dbus-sysdeps-util.c */
