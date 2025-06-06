/* GIO - GLib Input, Output and Streaming Library
 *
 * Copyright (C) 2006-2007 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Author: Alexander Larsson <alexl@redhat.com>
 */

#if !defined (__GIO_GIO_H_INSIDE__) && !defined (GIO_COMPILATION)
#error "Only <gio/gio.h> can be included directly."
#endif

#ifndef __G_FILE_INFO_H__
#define __G_FILE_INFO_H__

#include <gio/giotypes.h>

G_BEGIN_DECLS

#define G_TYPE_FILE_INFO         (g_file_info_get_type ())
#define G_FILE_INFO(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), G_TYPE_FILE_INFO, GFileInfo))
#define G_FILE_INFO_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), G_TYPE_FILE_INFO, GFileInfoClass))
#define G_IS_FILE_INFO(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), G_TYPE_FILE_INFO))
#define G_IS_FILE_INFO_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), G_TYPE_FILE_INFO))
#define G_FILE_INFO_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), G_TYPE_FILE_INFO, GFileInfoClass))

/**
 * GFileInfo:
 *
 * Stores information about a file system object referenced by a #GFile.
 **/
typedef struct _GFileInfoClass   GFileInfoClass;


/* Common Attributes:  */
/**
 * G_FILE_ATTRIBUTE_STANDARD_TYPE:
 *
 * A key in the "standard" namespace for storing file types.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_UINT32.
 * The value for this key should contain a #GFileType.
 **/
#define G_FILE_ATTRIBUTE_STANDARD_TYPE "standard::type"                     /* uint32 (GFileType) */

/**
 * G_FILE_ATTRIBUTE_STANDARD_IS_HIDDEN:
 *
 * A key in the "standard" namespace for checking if a file is hidden.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_BOOLEAN.
 **/
#define G_FILE_ATTRIBUTE_STANDARD_IS_HIDDEN "standard::is-hidden"           /* boolean */

/**
 * G_FILE_ATTRIBUTE_STANDARD_IS_BACKUP:
 *
 * A key in the "standard" namespace for checking if a file is a backup file.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_BOOLEAN.
 **/
#define G_FILE_ATTRIBUTE_STANDARD_IS_BACKUP "standard::is-backup"           /* boolean */

/**
 * G_FILE_ATTRIBUTE_STANDARD_IS_SYMLINK:
 *
 * A key in the "standard" namespace for checking if the file is a symlink.
 * Typically the actual type is something else, if we followed the symlink
 * to get the type.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_BOOLEAN.
 **/
#define G_FILE_ATTRIBUTE_STANDARD_IS_SYMLINK "standard::is-symlink"         /* boolean */

/**
 * G_FILE_ATTRIBUTE_STANDARD_IS_VIRTUAL:
 *
 * A key in the "standard" namespace for checking if a file is virtual.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_BOOLEAN.
 **/
#define G_FILE_ATTRIBUTE_STANDARD_IS_VIRTUAL "standard::is-virtual"         /* boolean */

/**
 * G_FILE_ATTRIBUTE_STANDARD_NAME:
 *
 * A key in the "standard" namespace for getting the name of the file.
 * The name is the on-disk filename which may not be in any known encoding,
 * and can thus not be generally displayed as is.
 * Use #G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME if you need to display the
 * name in a user interface.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_BYTE_STRING.
 **/
#define G_FILE_ATTRIBUTE_STANDARD_NAME "standard::name"                     /* byte string */

/**
 * G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME:
 *
 * A key in the "standard" namespace for getting the display name of the file.
 * A display name is guaranteed to be in UTF8 and can thus be displayed in
 * the UI.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_STRING.
 **/
#define G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME "standard::display-name"     /* string */

/**
 * G_FILE_ATTRIBUTE_STANDARD_EDIT_NAME:
 *
 * A key in the "standard" namespace for edit name of the file.
 * An edit name is similar to the display name, but it is meant to be
 * used when you want to rename the file in the UI. The display name
 * might contain information you don't want in the new filename (such as
 * "(invalid unicode)" if the filename was in an invalid encoding).
 *
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_STRING.
 **/
#define G_FILE_ATTRIBUTE_STANDARD_EDIT_NAME "standard::edit-name"           /* string */

/**
 * G_FILE_ATTRIBUTE_STANDARD_COPY_NAME:
 *
 * A key in the "standard" namespace for getting the copy name of the file.
 * The copy name is an optional version of the name. If availible its always
 * in UTF8, and corresponds directly to the original filename (only transcoded to
 * UTF8). This is useful if you want to copy the file to another filesystem that
 * might have a different encoding. If the filename is not a valid string in the
 * encoding selected for the filesystem it is in then the copy name will not be set.
 *
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_STRING.
 **/
#define G_FILE_ATTRIBUTE_STANDARD_COPY_NAME "standard::copy-name"           /* string */

/**
 * G_FILE_ATTRIBUTE_STANDARD_DESCRIPTION:
 *
 * A key in the "standard" namespace for getting the description of the file.
 * The description is a utf8 string that describes the file, generally containing
 * the filename, but can also contain furter information. Example descriptions
 * could be "filename (on hostname)" for a remote file or "filename (in trash)"
 * for a file in the trash. This is useful for instance as the window title
 * when displaying a directory or for a bookmarks menu.
 *
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_STRING.
 **/
#define G_FILE_ATTRIBUTE_STANDARD_DESCRIPTION "standard::description"        /* string */

/**
 * G_FILE_ATTRIBUTE_STANDARD_ICON:
 *
 * A key in the "standard" namespace for getting the icon for the file.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_OBJECT.
 * The value for this key should contain a #GIcon.
 **/
#define G_FILE_ATTRIBUTE_STANDARD_ICON "standard::icon"                     /* object (GIcon) */

/**
 * G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE:
 *
 * A key in the "standard" namespace for getting the content type of the file.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_STRING.
 * The value for this key should contain a valid content type.
 **/
#define G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE "standard::content-type"     /* string */

/**
 * G_FILE_ATTRIBUTE_STANDARD_FAST_CONTENT_TYPE:
 *
 * A key in the "standard" namespace for getting the fast content type.
 * The fast content type isn't as reliable as the regular one, as it
 * only uses the filename to guess it, but it is faster to calculate than the
 * regular content type.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_STRING.
 *
 **/
#define G_FILE_ATTRIBUTE_STANDARD_FAST_CONTENT_TYPE "standard::fast-content-type" /* string */

/**
 * G_FILE_ATTRIBUTE_STANDARD_SIZE:
 *
 * A key in the "standard" namespace for getting the file's size (in bytes).
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_UINT64.
 **/
#define G_FILE_ATTRIBUTE_STANDARD_SIZE "standard::size"                     /* uint64 */

/**
 * G_FILE_ATTRIBUTE_ALLOCATED_SIZE:
 *
 * A key in the "standard" namespace for getting the amount of disk space
 * that is consumed by the file (in bytes).  This will generally be larger
 * than the file size (due to block size overhead) but can occasionally be
 * smaller (for example, for sparse files).
 *
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_UINT64.
 **/
#define G_FILE_ATTRIBUTE_STANDARD_ALLOCATED_SIZE "standard::allocated-size" /* uint64 */

/**
 * G_FILE_ATTRIBUTE_STANDARD_SYMLINK_TARGET:
 *
 * A key in the "standard" namespace for getting the symlink target, if the file
 * is a symlink. Corresponding #GFileAttributeType is
 * %G_FILE_ATTRIBUTE_TYPE_BYTE_STRING.
 **/
#define G_FILE_ATTRIBUTE_STANDARD_SYMLINK_TARGET "standard::symlink-target" /* byte string */

/**
 * G_FILE_ATTRIBUTE_STANDARD_TARGET_URI:
 *
 * A key in the "standard" namespace for getting the target URI for the file, in
 * the case of %G_FILE_TYPE_SHORTCUT or %G_FILE_TYPE_MOUNTABLE files.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_STRING.
 **/
#define G_FILE_ATTRIBUTE_STANDARD_TARGET_URI "standard::target-uri"         /* string */

/**
 * G_FILE_ATTRIBUTE_STANDARD_SORT_ORDER:
 *
 * A key in the "standard" namespace for setting the sort order of a file.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_INT32.
 * An example use would be in file managers, which would use this key
 * to set the order files are displayed. Files with smaller sort order
 * should be sorted first, and files without sort order as if sort order
 * was zero.
 **/
#define G_FILE_ATTRIBUTE_STANDARD_SORT_ORDER "standard::sort-order"         /* int32  */

/* Entity tags, used to avoid missing updates on save */

/**
 * G_FILE_ATTRIBUTE_ETAG_VALUE:
 *
 * A key in the "etag" namespace for getting the value of the file's
 * entity tag. Corresponding #GFileAttributeType is
 * %G_FILE_ATTRIBUTE_TYPE_STRING.
 **/
#define G_FILE_ATTRIBUTE_ETAG_VALUE "etag::value"                 /* string */

/* File identifier, for e.g. avoiding loops when doing recursive
 * directory scanning
 */

/**
 * G_FILE_ATTRIBUTE_ID_FILE:
 *
 * A key in the "id" namespace for getting a file identifier.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_STRING.
 * An example use would be during listing files, to avoid recursive
 * directory scanning.
 **/
#define G_FILE_ATTRIBUTE_ID_FILE "id::file"                     /* string */

/**
 * G_FILE_ATTRIBUTE_ID_FILESYSTEM:
 *
 * A key in the "id" namespace for getting the file system identifier.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_STRING.
 * An example use would be during drag and drop to see if the source
 * and target are on the same filesystem (default to move) or not (default
 * to copy).
 **/
#define G_FILE_ATTRIBUTE_ID_FILESYSTEM "id::filesystem"         /* string */

/* Calculated Access Rights for current user */

/**
 * G_FILE_ATTRIBUTE_ACCESS_CAN_READ:
 *
 * A key in the "access" namespace for getting read privileges.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_BOOLEAN.
 * This attribute will be %TRUE if the user is able to read the file.
 **/
#define G_FILE_ATTRIBUTE_ACCESS_CAN_READ "access::can-read"       /* boolean */

/**
 * G_FILE_ATTRIBUTE_ACCESS_CAN_WRITE:
 *
 * A key in the "access" namespace for getting write privileges.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_BOOLEAN.
 * This attribute will be %TRUE if the user is able to write to the file.
 **/
#define G_FILE_ATTRIBUTE_ACCESS_CAN_WRITE "access::can-write"     /* boolean */

/**
 * G_FILE_ATTRIBUTE_ACCESS_CAN_EXECUTE:
 *
 * A key in the "access" namespace for getting execution privileges.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_BOOLEAN.
 * This attribute will be %TRUE if the user is able to execute the file.
 **/
#define G_FILE_ATTRIBUTE_ACCESS_CAN_EXECUTE "access::can-execute" /* boolean */

/**
 * G_FILE_ATTRIBUTE_ACCESS_CAN_DELETE:
 *
 * A key in the "access" namespace for checking deletion privileges.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_BOOLEAN.
 * This attribute will be %TRUE if the user is able to delete the file.
 **/
#define G_FILE_ATTRIBUTE_ACCESS_CAN_DELETE "access::can-delete"   /* boolean */

/**
 * G_FILE_ATTRIBUTE_ACCESS_CAN_TRASH:
 *
 * A key in the "access" namespace for checking trashing privileges.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_BOOLEAN.
 * This attribute will be %TRUE if the user is able to move the file to
 * the trash.
 **/
#define G_FILE_ATTRIBUTE_ACCESS_CAN_TRASH "access::can-trash"     /* boolean */

/**
 * G_FILE_ATTRIBUTE_ACCESS_CAN_RENAME:
 *
 * A key in the "access" namespace for checking renaming privileges.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_BOOLEAN.
 * This attribute will be %TRUE if the user is able to rename the file.
 **/
#define G_FILE_ATTRIBUTE_ACCESS_CAN_RENAME "access::can-rename"   /* boolean */

/* TODO: Should we have special version for directories? can_enumerate, etc */

/* Mountable attributes */

/**
 * G_FILE_ATTRIBUTE_MOUNTABLE_CAN_MOUNT:
 *
 * A key in the "mountable" namespace for checking if a file (of type G_FILE_TYPE_MOUNTABLE) is mountable.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_BOOLEAN.
 **/
#define G_FILE_ATTRIBUTE_MOUNTABLE_CAN_MOUNT "mountable::can-mount"     /* boolean */

/**
 * G_FILE_ATTRIBUTE_MOUNTABLE_CAN_UNMOUNT:
 *
 * A key in the "mountable" namespace for checking if a file (of type G_FILE_TYPE_MOUNTABLE)  is unmountable.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_BOOLEAN.
 **/
#define G_FILE_ATTRIBUTE_MOUNTABLE_CAN_UNMOUNT "mountable::can-unmount" /* boolean */

/**
 * G_FILE_ATTRIBUTE_MOUNTABLE_CAN_EJECT:
 *
 * A key in the "mountable" namespace for checking if a file (of type G_FILE_TYPE_MOUNTABLE) can be ejected.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_BOOLEAN.
 **/
#define G_FILE_ATTRIBUTE_MOUNTABLE_CAN_EJECT "mountable::can-eject"     /* boolean */

/**
 * G_FILE_ATTRIBUTE_MOUNTABLE_UNIX_DEVICE:
 *
 * A key in the "mountable" namespace for getting the unix device.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_UINT32.
 **/
#define G_FILE_ATTRIBUTE_MOUNTABLE_UNIX_DEVICE "mountable::unix-device" /* uint32 */

/**
 * G_FILE_ATTRIBUTE_MOUNTABLE_HAL_UDI:
 *
 * A key in the "mountable" namespace for getting the HAL UDI for the mountable
 * file. Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_STRING.
 **/
#define G_FILE_ATTRIBUTE_MOUNTABLE_HAL_UDI "mountable::hal-udi"         /* string */

/* Time attributes */

/**
 * G_FILE_ATTRIBUTE_TIME_MODIFIED:
 *
 * A key in the "time" namespace for getting the time the file was last
 * modified. Corresponding #GFileAttributeType is
 * %G_FILE_ATTRIBUTE_TYPE_UINT64, and contains the UNIX time since the
 * file was modified.
 **/
#define G_FILE_ATTRIBUTE_TIME_MODIFIED "time::modified"           /* uint64 */

/**
 * G_FILE_ATTRIBUTE_TIME_MODIFIED_USEC:
 *
 * A key in the "time" namespace for getting the miliseconds of the time
 * the file was last modified. This should be used in conjunction with
 * #G_FILE_ATTRIBUTE_TIME_MODIFIED. Corresponding #GFileAttributeType is
 * %G_FILE_ATTRIBUTE_TYPE_UINT32.
 **/
#define G_FILE_ATTRIBUTE_TIME_MODIFIED_USEC "time::modified-usec" /* uint32 */

/**
 * G_FILE_ATTRIBUTE_TIME_ACCESS:
 *
 * A key in the "time" namespace for getting the time the file was last
 * accessed. Corresponding #GFileAttributeType is
 * %G_FILE_ATTRIBUTE_TYPE_UINT64, and contains the UNIX time since the
 * file was last accessed.
 **/
#define G_FILE_ATTRIBUTE_TIME_ACCESS "time::access"               /* uint64 */

/**
 * G_FILE_ATTRIBUTE_TIME_ACCESS_USEC:
 *
 * A key in the "time" namespace for getting the microseconds of the time
 * the file was last accessed. This should be used in conjunction with
 * #G_FILE_ATTRIBUTE_TIME_ACCESS. Corresponding #GFileAttributeType is
 * %G_FILE_ATTRIBUTE_TYPE_UINT32.
 **/
#define G_FILE_ATTRIBUTE_TIME_ACCESS_USEC "time::access-usec"     /* uint32 */

/**
 * G_FILE_ATTRIBUTE_TIME_CHANGED:
 *
 * A key in the "time" namespace for getting the time the file was last
 * changed. Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_UINT64,
 * and contains the UNIX time since the file was last changed.
 *
 * This corresponds to the traditional UNIX ctime.
 **/
#define G_FILE_ATTRIBUTE_TIME_CHANGED "time::changed"             /* uint64 */

/**
 * G_FILE_ATTRIBUTE_TIME_CHANGED_USEC:
 *
 * A key in the "time" namespace for getting the microseconds of the time
 * the file was last changed. This should be used in conjunction with
 * #G_FILE_ATTRIBUTE_TIME_CHANGED. Corresponding #GFileAttributeType is
 * %G_FILE_ATTRIBUTE_TYPE_UINT32.
 **/
#define G_FILE_ATTRIBUTE_TIME_CHANGED_USEC "time::changed-usec"   /* uint32 */

/**
 * G_FILE_ATTRIBUTE_TIME_CREATED:
 *
 * A key in the "time" namespace for getting the time the file was created.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_UINT64,
 * and contains the UNIX time since the file was created.
 *
 * This corresponds to the NTFS ctime.
 **/
#define G_FILE_ATTRIBUTE_TIME_CREATED "time::created"             /* uint64 */

/**
 * G_FILE_ATTRIBUTE_TIME_CREATED_USEC:
 *
 * A key in the "time" namespace for getting the microseconds of the time
 * the file was created. This should be used in conjunction with
 * #G_FILE_ATTRIBUTE_TIME_CREATED. Corresponding #GFileAttributeType is
 * %G_FILE_ATTRIBUTE_TYPE_UINT32.
 **/
#define G_FILE_ATTRIBUTE_TIME_CREATED_USEC "time::created-usec"   /* uint32 */

/* Unix specific attributes */

/**
 * G_FILE_ATTRIBUTE_UNIX_DEVICE:
 *
 * A key in the "unix" namespace for getting the device id of the device the
 * file is located on (see stat() documentation). This attribute is only
 * available for UNIX file systems. Corresponding #GFileAttributeType is
 * %G_FILE_ATTRIBUTE_TYPE_UINT32.
 **/
#define G_FILE_ATTRIBUTE_UNIX_DEVICE "unix::device"               /* uint32 */

/**
 * G_FILE_ATTRIBUTE_UNIX_INODE:
 *
 * A key in the "unix" namespace for getting the inode of the file.
 * This attribute is only available for UNIX file systems. Corresponding
 * #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_UINT64.
 **/
#define G_FILE_ATTRIBUTE_UNIX_INODE "unix::inode"                 /* uint64 */

/**
 * G_FILE_ATTRIBUTE_UNIX_MODE:
 *
 * A key in the "unix" namespace for getting the mode of the file
 * (e.g. whether the file is a regular file, symlink, etc). See lstat()
 * documentation. This attribute is only available for UNIX file systems.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_UINT32.
 **/
#define G_FILE_ATTRIBUTE_UNIX_MODE "unix::mode"                   /* uint32 */

/**
 * G_FILE_ATTRIBUTE_UNIX_NLINK:
 *
 * A key in the "unix" namespace for getting the number of hard links
 * for a file. See lstat() documentation. This attribute is only available
 * for UNIX file systems. Corresponding #GFileAttributeType is
 * %G_FILE_ATTRIBUTE_TYPE_UINT32.
 **/
#define G_FILE_ATTRIBUTE_UNIX_NLINK "unix::nlink"                 /* uint32 */

/**
 * G_FILE_ATTRIBUTE_UNIX_UID:
 *
 * A key in the "unix" namespace for getting the user ID for the file.
 * This attribute is only available for UNIX file systems.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_UINT32.
 **/
#define G_FILE_ATTRIBUTE_UNIX_UID "unix::uid"                     /* uint32 */

/**
 * G_FILE_ATTRIBUTE_UNIX_GID:
 *
 * A key in the "unix" namespace for getting the group ID for the file.
 * This attribute is only available for UNIX file systems.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_UINT32.
 **/
#define G_FILE_ATTRIBUTE_UNIX_GID "unix::gid"                     /* uint32 */

/**
 * G_FILE_ATTRIBUTE_UNIX_RDEV:
 *
 * A key in the "unix" namespace for getting the device ID for the file
 * (if it is a special file). See lstat() documentation. This attribute
 * is only available for UNIX file systems. Corresponding #GFileAttributeType
 * is %G_FILE_ATTRIBUTE_TYPE_UINT32.
 **/
#define G_FILE_ATTRIBUTE_UNIX_RDEV "unix::rdev"                   /* uint32 */

/**
 * G_FILE_ATTRIBUTE_UNIX_BLOCK_SIZE:
 *
 * A key in the "unix" namespace for getting the block size for the file
 * system. This attribute is only available for UNIX file systems.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_UINT32.
 **/
#define G_FILE_ATTRIBUTE_UNIX_BLOCK_SIZE "unix::block-size"       /* uint32 */

/**
 * G_FILE_ATTRIBUTE_UNIX_BLOCKS:
 *
 * A key in the "unix" namespace for getting the number of blocks allocated
 * for the file. This attribute is only available for UNIX file systems.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_UINT64.
 **/
#define G_FILE_ATTRIBUTE_UNIX_BLOCKS "unix::blocks"               /* uint64 */

/**
 * G_FILE_ATTRIBUTE_UNIX_IS_MOUNTPOINT:
 *
 * A key in the "unix" namespace for checking if the file represents a
 * UNIX mount point. This attribute is %TRUE if the file is a UNIX mount
 * point. This attribute is only available for UNIX file systems.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_BOOLEAN.
 **/
#define G_FILE_ATTRIBUTE_UNIX_IS_MOUNTPOINT "unix::is-mountpoint" /* boolean */

/* DOS specific attributes */

/**
 * G_FILE_ATTRIBUTE_DOS_IS_ARCHIVE:
 *
 * A key in the "dos" namespace for checking if the file's archive flag
 * is set. This attribute is %TRUE if the archive flag is set. This attribute
 * is only available for DOS file systems. Corresponding #GFileAttributeType
 * is %G_FILE_ATTRIBUTE_TYPE_BOOLEAN.
 **/
#define G_FILE_ATTRIBUTE_DOS_IS_ARCHIVE "dos::is-archive"         /* boolean */

/**
 * G_FILE_ATTRIBUTE_DOS_IS_SYSTEM:
 *
 * A key in the "dos" namespace for checking if the file's backup flag
 * is set. This attribute is %TRUE if the backup flag is set. This attribute
 * is only available for DOS file systems. Corresponding #GFileAttributeType
 * is %G_FILE_ATTRIBUTE_TYPE_BOOLEAN.
 **/
#define G_FILE_ATTRIBUTE_DOS_IS_SYSTEM "dos::is-system"           /* boolean */

/* Owner attributes */

/**
 * G_FILE_ATTRIBUTE_OWNER_USER:
 *
 * A key in the "owner" namespace for getting the user name of the
 * file's owner. Corresponding #GFileAttributeType is
 * %G_FILE_ATTRIBUTE_TYPE_STRING.
 **/
#define G_FILE_ATTRIBUTE_OWNER_USER "owner::user"                 /* string */

/**
 * G_FILE_ATTRIBUTE_OWNER_USER_REAL:
 *
 * A key in the "owner" namespace for getting the real name of the
 * user that owns the file. Corresponding #GFileAttributeType is
 * %G_FILE_ATTRIBUTE_TYPE_STRING.
 **/
#define G_FILE_ATTRIBUTE_OWNER_USER_REAL "owner::user-real"       /* string */

/**
 * G_FILE_ATTRIBUTE_OWNER_GROUP:
 *
 * A key in the "owner" namespace for getting the file owner's group.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_STRING.
 **/
#define G_FILE_ATTRIBUTE_OWNER_GROUP "owner::group"               /* string */

/* Thumbnails */

/**
 * G_FILE_ATTRIBUTE_THUMBNAIL_PATH:
 *
 * A key in the "thumbnail" namespace for getting the path to the thumbnail
 * image. Corresponding #GFileAttributeType is
 * %G_FILE_ATTRIBUTE_TYPE_BYTE_STRING.
 **/
#define G_FILE_ATTRIBUTE_THUMBNAIL_PATH "thumbnail::path"         /* bytestring */
/**
 * G_FILE_ATTRIBUTE_THUMBNAILING_FAILED:
 *
 * A key in the "thumbnail" namespace for checking if thumbnailing failed.
 * This attribute is %TRUE if thumbnailing failed. Corresponding
 * #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_BOOLEAN.
 **/
#define G_FILE_ATTRIBUTE_THUMBNAILING_FAILED "thumbnail::failed"         /* boolean */

/* Preview */

/**
 * G_FILE_ATTRIBUTE_PREVIEW_ICON:
 *
 * A key in the "preview" namespace for getting a #GIcon that can be
 * used to get preview of the file. For example, it may be a low
 * resolution thumbnail without metadata. Corresponding
 * #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_OBJECT.  The value
 * for this key should contain a #GIcon.
 *
 * Since: 2.20
 **/
#define G_FILE_ATTRIBUTE_PREVIEW_ICON "preview::icon"         /* object (GIcon) */

/* File system info (for g_file_get_filesystem_info) */

/**
 * G_FILE_ATTRIBUTE_FILESYSTEM_SIZE:
 *
 * A key in the "filesystem" namespace for getting the total size (in bytes) of the file system,
 * used in g_file_query_filesystem_info(). Corresponding #GFileAttributeType
 * is %G_FILE_ATTRIBUTE_TYPE_UINT64.
 **/
#define G_FILE_ATTRIBUTE_FILESYSTEM_SIZE "filesystem::size"                       /* uint64 */

/**
 * G_FILE_ATTRIBUTE_FILESYSTEM_FREE:
 *
 * A key in the "filesystem" namespace for getting the number of bytes of free space left on the
 * file system. Corresponding #GFileAttributeType is
 * %G_FILE_ATTRIBUTE_TYPE_UINT64.
 **/
#define G_FILE_ATTRIBUTE_FILESYSTEM_FREE "filesystem::free"                       /* uint64 */

/**
 * G_FILE_ATTRIBUTE_FILESYSTEM_TYPE:
 *
 * A key in the "filesystem" namespace for getting the file system's type.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_STRING.
 **/
#define G_FILE_ATTRIBUTE_FILESYSTEM_TYPE "filesystem::type"                       /* string */

/**
 * G_FILE_ATTRIBUTE_FILESYSTEM_READONLY:
 *
 * A key in the "filesystem" namespace for checking if the file system
 * is read only. Is set to %TRUE if the file system is read only.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_BOOLEAN.
 **/
#define G_FILE_ATTRIBUTE_FILESYSTEM_READONLY "filesystem::readonly"               /* boolean */

/**
 * G_FILE_ATTRIBUTE_FILESYSTEM_USE_PREVIEW:
 *
 * A key in the "filesystem" namespace for hinting a file manager
 * application whether it should preview (e.g. thumbnail) files on the
 * file system. The value for this key contain a
 * #GFilesystemPreviewType.
 **/
#define G_FILE_ATTRIBUTE_FILESYSTEM_USE_PREVIEW "filesystem::use-preview"        /* uint32 (GFilesystemPreviewType) */

/**
 * G_FILE_ATTRIBUTE_GVFS_BACKEND:
 *
 * A key in the "gvfs" namespace that gets the name of the current
 * GVFS backend in use. Corresponding #GFileAttributeType is
 * %G_FILE_ATTRIBUTE_TYPE_STRING.
 **/
#define G_FILE_ATTRIBUTE_GVFS_BACKEND "gvfs::backend"             /* string */

/**
 * G_FILE_ATTRIBUTE_SELINUX_CONTEXT:
 *
 * A key in the "selinux" namespace for getting the file's SELinux
 * context. Corresponding #GFileAttributeType is
 * %G_FILE_ATTRIBUTE_TYPE_STRING. Note that this attribute is only
 * available if GLib has been built with SELinux support.
 **/
#define G_FILE_ATTRIBUTE_SELINUX_CONTEXT "selinux::context"       /* string */

/**
 * G_FILE_ATTRIBUTE_TRASH_ITEM_COUNT:
 *
 * A key in the "trash" namespace.  When requested against
 * "trash:///" returns the number of (toplevel) items in the trash folder.
 * Corresponding #GFileAttributeType is %G_FILE_ATTRIBUTE_TYPE_UINT32.
 **/
#define G_FILE_ATTRIBUTE_TRASH_ITEM_COUNT "trash::item-count"     /* uint32 */

GType              g_file_info_get_type                  (void) G_GNUC_CONST;

GFileInfo *        g_file_info_new                       (void);
GFileInfo *        g_file_info_dup                       (GFileInfo  *other);
void               g_file_info_copy_into                 (GFileInfo  *src_info,
							  GFileInfo  *dest_info);
gboolean           g_file_info_has_attribute             (GFileInfo  *info,
							  const char *attribute);
char **            g_file_info_list_attributes           (GFileInfo  *info,
							  const char *name_space);
gboolean           g_file_info_get_attribute_data        (GFileInfo  *info,
							  const char *attribute,
							  GFileAttributeType *type,
							  gpointer   *value_pp,
							  GFileAttributeStatus *status);
GFileAttributeType g_file_info_get_attribute_type        (GFileInfo  *info,
							  const char *attribute);
void               g_file_info_remove_attribute          (GFileInfo  *info,
							  const char *attribute);
GFileAttributeStatus g_file_info_get_attribute_status    (GFileInfo  *info,
							  const char *attribute);
char *             g_file_info_get_attribute_as_string   (GFileInfo  *info,
							  const char *attribute);
const char *       g_file_info_get_attribute_string      (GFileInfo  *info,
							  const char *attribute);
const char *       g_file_info_get_attribute_byte_string (GFileInfo  *info,
							  const char *attribute);
gboolean           g_file_info_get_attribute_boolean     (GFileInfo  *info,
							  const char *attribute);
guint32            g_file_info_get_attribute_uint32      (GFileInfo  *info,
							  const char *attribute);
gint32             g_file_info_get_attribute_int32       (GFileInfo  *info,
							  const char *attribute);
guint64            g_file_info_get_attribute_uint64      (GFileInfo  *info,
							  const char *attribute);
gint64             g_file_info_get_attribute_int64       (GFileInfo  *info,
							  const char *attribute);
GObject *          g_file_info_get_attribute_object      (GFileInfo  *info,
							  const char *attribute);

void               g_file_info_set_attribute             (GFileInfo  *info,
							  const char *attribute,
							  GFileAttributeType type,
							  gpointer    value_p);
void               g_file_info_set_attribute_string      (GFileInfo  *info,
							  const char *attribute,
							  const char *attr_value);
void               g_file_info_set_attribute_byte_string (GFileInfo  *info,
							  const char *attribute,
							  const char *attr_value);
void               g_file_info_set_attribute_boolean     (GFileInfo  *info,
							  const char *attribute,
							  gboolean    attr_value);
void               g_file_info_set_attribute_uint32      (GFileInfo  *info,
							  const char *attribute,
							  guint32     attr_value);
void               g_file_info_set_attribute_int32       (GFileInfo  *info,
							  const char *attribute,
							  gint32      attr_value);
void               g_file_info_set_attribute_uint64      (GFileInfo  *info,
							  const char *attribute,
							  guint64     attr_value);
void               g_file_info_set_attribute_int64       (GFileInfo  *info,
							  const char *attribute,
							  gint64      attr_value);
void               g_file_info_set_attribute_object      (GFileInfo  *info,
							  const char *attribute,
							  GObject    *attr_value);

void               g_file_info_clear_status              (GFileInfo  *info);

/* Helper getters: */
GFileType         g_file_info_get_file_type          (GFileInfo         *info);
gboolean          g_file_info_get_is_hidden          (GFileInfo         *info);
gboolean          g_file_info_get_is_backup          (GFileInfo         *info);
gboolean          g_file_info_get_is_symlink         (GFileInfo         *info);
const char *      g_file_info_get_name               (GFileInfo         *info);
const char *      g_file_info_get_display_name       (GFileInfo         *info);
const char *      g_file_info_get_edit_name          (GFileInfo         *info);
GIcon *           g_file_info_get_icon               (GFileInfo         *info);
const char *      g_file_info_get_content_type       (GFileInfo         *info);
goffset           g_file_info_get_size               (GFileInfo         *info);
void              g_file_info_get_modification_time  (GFileInfo         *info,
						      GTimeVal          *result);
const char *      g_file_info_get_symlink_target     (GFileInfo         *info);
const char *      g_file_info_get_etag               (GFileInfo         *info);
gint32            g_file_info_get_sort_order         (GFileInfo         *info);

void              g_file_info_set_attribute_mask     (GFileInfo         *info,
						      GFileAttributeMatcher *mask);
void              g_file_info_unset_attribute_mask   (GFileInfo         *info);

/* Helper setters: */
void              g_file_info_set_file_type          (GFileInfo         *info,
						      GFileType          type);
void              g_file_info_set_is_hidden          (GFileInfo         *info,
						      gboolean           is_hidden);
void              g_file_info_set_is_symlink         (GFileInfo         *info,
						      gboolean           is_symlink);
void              g_file_info_set_name               (GFileInfo         *info,
						      const char        *name);
void              g_file_info_set_display_name       (GFileInfo         *info,
						      const char        *display_name);
void              g_file_info_set_edit_name          (GFileInfo         *info,
						      const char        *edit_name);
void              g_file_info_set_icon               (GFileInfo         *info,
						      GIcon             *icon);
void              g_file_info_set_content_type       (GFileInfo         *info,
						      const char        *content_type);
void              g_file_info_set_size               (GFileInfo         *info,
						      goffset            size);
void              g_file_info_set_modification_time  (GFileInfo         *info,
						      GTimeVal          *mtime);
void              g_file_info_set_symlink_target     (GFileInfo         *info,
						      const char        *symlink_target);
void              g_file_info_set_sort_order         (GFileInfo         *info,
						      gint32             sort_order);

GFileAttributeMatcher *g_file_attribute_matcher_new            (const char            *attributes);
GFileAttributeMatcher *g_file_attribute_matcher_ref            (GFileAttributeMatcher *matcher);
void                   g_file_attribute_matcher_unref          (GFileAttributeMatcher *matcher);
gboolean               g_file_attribute_matcher_matches        (GFileAttributeMatcher *matcher,
								const char            *attribute);
gboolean               g_file_attribute_matcher_matches_only   (GFileAttributeMatcher *matcher,
								const char            *attribute);
gboolean               g_file_attribute_matcher_enumerate_namespace (GFileAttributeMatcher *matcher,
								     const char            *ns);
const char *           g_file_attribute_matcher_enumerate_next (GFileAttributeMatcher *matcher);

G_END_DECLS

#endif /* __G_FILE_INFO_H__ */
