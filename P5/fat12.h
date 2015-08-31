/**
 * fat12.h
 * Author: Dave Sizer
 * Date: 08/2015
 *
 * Structures and functions for reading a FAT12 filesystem
 */

#ifndef __FAT_12_H__
#define __FAT_12_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BOOTSECTOR_LENGTH 512
#define DIRENTRY_SIZE     32

struct fat12_bs;
struct fat12_direntry;

/**
 * Initialize a FAT12 session, and seek to the root directory
 * file - a STDIO file structure with opened in the desired FAT12 image file
 * Returns: A pointer to the new fat12 session structure
 */
struct fat12 *fat12_init(FILE *file);

/**
 * Read a FAT12 root directory into the corrosponding structures
 * file - The file pointer to the image
 * offset - The offset of the root directory in the file
 * Returns - An array of directory entry structures
 */
struct fat12_direntry **read_root_directory(FILE *file, uint32_t offset);

/**
 * Read a FAT12 directory into the corrosponding structures
 * data - A pointer to the directory data
 * size - The size of the directory data
 * Returns - An array of directory entry structures
 */
struct fat12_direntry **read_fat12_directory(uint8_t *data, uint32_t size);

/**
 * Read a FAT12 directory entry into the corrosponding structure
 * entry - A pointer to the entry data
 * Returns - A pointer to a directory entry structure
 */
struct fat12_direntry *read_directory_entry(uint8_t *entry);

/**
 * Read a file from a FAT12 image into a buffer using the FAT
 * file - The file pointer for the image file
 * dir  - A pointer to the directory entry structure for the file
 * Returns: A buffer containing the file data
 */
uint8_t *read_fat12_file(struct fat12_direntry *dir);

/**
 * Calculate the offset of the root directory in a FAT12 image using information
 * in the bootsector
 * bs - A pointer to the images bootsector structure
 * Returns - The offset of the root directory data
 */
uint32_t calculate_root_position(struct fat12_bs *bs);

/**
 * Calculate the offset of the data area in a FAT12 image using information in
 * the bootsector
 * bs - A pointer to the image's bootsector structure
 */
uint32_t calculate_data_position(struct fat12_bs *bs);

/**
 * Print an overview of the FAT12 image.
 * image - The FAT12 struct in question
 */
void print_disk_information(struct fat12_bs *bootsector);

/**
 * Print a directory entry
 * dir - The entry to print
 */
void print_directory_entry(struct fat12_direntry *dir);

/**
 * Structure for a FAT12 session
 */
struct fat12
{
  struct fat12_bs *bs;
  struct fat12_direntry **rootEntries;
  uint32_t FATPos;
  uint32_t rootPos;
  uint32_t dataPos;
};


/**
 * Boot sector
 */
struct fat12_bs
{
  uint8_t  bsJmp[3];              // Jump to boot code (not used)
  char     bsOemName[8];          // Name of manufacturer

  /** 
   * Bios parameter block
   */
  uint16_t bpbBytesPerSector;     /* 0x0B */
  uint8_t  bpbSectorsPerCluster;  /* 0x0D */
  uint16_t bpbReservedSectors;    /* 0x0E */
  uint8_t  bpbNumberOfFATs;       
  uint16_t bpbRootEntries;
  uint16_t bpbTotalSectors;
  uint8_t  bpbMedia;
  uint16_t bpbSectorsPerFAT;
  uint16_t bpbSectorsPerTrack;
  uint16_t bpbHeadsPerCylinder;
  uint32_t bpbHiddenSectors;
  uint32_t bpbTotalSectorsBig;
  /**/

  uint8_t  bsDriveNumber;
  uint8_t  bsUnused;
  uint8_t  bsExtBootSignature;
  uint32_t bsSerialNumber;
                                  
  char     bsVolumeLabel[11];     // The volume label
  char     bsFileSystem[9];       // The FS type

  // The rest of the sector is boot code, the root directory and FAT starts
  // after this sector
} __attribute__((packed));

/**
 * A directory entry
 */
struct fat12_direntry
{
  char     filename[8];
  char     ext[3];
  uint8_t  attribs;               // Bit-field for attributes
  uint8_t  reserved[10];          // "Reserved" space
  uint16_t time;                  // (H*2048+m*32+s/2)
  uint16_t date;                  // ((Y-1980)*512+M*32+d)
  uint16_t starting_cluster;      // Index into FAT
  uint32_t filesize;              // In  bytes
} __attribute__((packed));


#endif
