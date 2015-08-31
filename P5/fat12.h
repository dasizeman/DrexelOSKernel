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

struct fat12_bs;
struct fat12_direntry;

/**
 * Initialize a FAT12 session, and seek to the root directory
 * file - a STDIO file structure with opened in the desired FAT12 image file
 * bs   - where boot sector information will be stored
 */
int fat12_init(FILE **file, struct fat12_bs **bs);
void print_disk_information(struct fat12_bs *bootsector);


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
