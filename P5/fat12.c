/**
 * fat12.c
 * Author: Dave Sizer
 * Date: 08/2015
 */

#include "fat12.h"

struct fat12 *fat12_init(FILE *file)
{
  // Allocate a fat12 struct
  struct fat12 *fatptr = NULL;
  if (!(fatptr = malloc(sizeof *fatptr)))
  {
    fprintf(stderr, "Failed allocating fat12 struct\n");
    return NULL;
  }

  // Allocate a bootsector struct and read it in
  struct fat12_bs *bs = NULL;
  if (!(bs = malloc(sizeof *bs)))
  {
    fprintf(stderr, "Failed allocating fat12_bs struct\n");
    return NULL;
  }

  if(!fread(bs, sizeof(struct fat12_bs), 1, file))
  {
    fprintf(stderr, "Failed to read bootsector\n");
    return NULL;
  }

  // Set struct values
  fatptr->bs = bs;
  fatptr->FATPos = (BOOTSECTOR_LENGTH+1);
  fatptr->rootPos = calculate_root_position(bs);
  fatptr->dataPos = calculate_data_position(bs);
  fatptr->numRootEntries = 0;
  
  fatptr->rootEntries = read_root_directory(file, fatptr);

  return fatptr;

}


struct fat12_direntry **read_root_directory(FILE *file, struct fat12 *image)
{
  // Recalculate root sectors
  uint16_t bps;
  bps = image->bs->bpbBytesPerSector;
  uint16_t root_sectors = image->bs->bpbRootEntries * DIRENTRY_SIZE / bps;
  size_t size = root_sectors*bps;

  // Allocate a buffer for the directory data
  uint8_t *buffer = NULL;
  if (!(buffer = malloc(size)))
  {
    fprintf(stderr, "read_root_directory: allocation failure\n");
    return NULL;
  }

  // Seek to the root directory
  fseek(file, image->rootPos, SEEK_SET);

  // Read into the buffer
  if (!fread(buffer, size, 1, file))
  {
    fprintf(stderr, "read_root_directory: read failed\n");
    return NULL;
  }

  struct fat12_direntry **dirs = read_fat12_directory(buffer, image->bs->bpbRootEntries, &(image->numRootEntries));
  free(buffer);

  //printf("Buffer dump:\n");
  //int i;
  //for (i=0; i < 100; i++)
    //printf("%02X ", buffer[i]);

  return dirs;
}

struct fat12_direntry **read_fat12_directory(uint8_t *data, uint16_t rootentries, uint16_t *count)
{
  // NOTE:
  // This will only work for root directories right now.  For real directories
  // we will allocate in cluster-sized intervals.  

  int idx = 0;

  // Allocate our pointers
  struct fat12_direntry **dirs = NULL;
  if (!(dirs = malloc(rootentries * sizeof(struct fat12_direntry*))))
  {
    fprintf(stderr, "read_fat12_directory: failed allocating pointers");
    return NULL;
  }

  // Copy until we hit the end or unused space
  while (*data != 0)
  {
    if (*data == 0xE5) // Deleted entry
    {
      data += DIRENTRY_SIZE;
      continue;
    }

    // Allocate an entry structure
    struct fat12_direntry *entry = NULL;
    if (!(entry = malloc(sizeof *entry)))
    {
      fprintf(stderr, 
          "read_fat12_directory: failed allocating directory structure\n");
      return NULL;
    }
    
    // Copy into the structure (I suppose we could read directly in this loop if
    // we wanted)

    dirs[idx] = memcpy(entry, data, DIRENTRY_SIZE);

    (*count)++;
    idx++;
    data += DIRENTRY_SIZE;
  }

  return dirs;
}

uint8_t *read_fat12_file(struct fat12_direntry *dir)
{
 //TODO
}

uint32_t calculate_root_position(struct fat12_bs *bs)
{
  uint16_t bps;
  bps = bs->bpbBytesPerSector;

  return (BOOTSECTOR_LENGTH + (bps * bs->bpbSectorsPerFAT * bs->bpbNumberOfFATs));

}


uint32_t calculate_data_position(struct fat12_bs *bs)
{
  uint16_t bps;
  bps = bs->bpbBytesPerSector;
  uint16_t root_sectors = bs->bpbRootEntries * DIRENTRY_SIZE / bps;

  return (bs->bpbBytesPerSector * root_sectors + calculate_root_position(bs));

}

void print_directory_entry(struct fat12_direntry *dir)
{
  printf("%10.*s", 8, dir->filename);
  printf("%5.*s", 3, dir->ext);
  printf("%15s", unpack_fat12_date(dir->date));
  printf("%10s\n", unpack_fat12_time(dir->time));

}

void print_disk_information(struct fat12_bs *bootsector)
{
  printf("Volume label: %.*s\n",(int)sizeof(bootsector->bsVolumeLabel), 
          bootsector->bsVolumeLabel);

  printf("Volume serial: %1X\n", bootsector->bsSerialNumber);
}


char *unpack_fat12_time(uint16_t packed)
{
  uint16_t hour = (packed & 0xF800) >> 11;
  uint16_t minute = (packed & 0x7C0) >> 6;
  uint16_t second = (packed & 0x10) * 2;

  char *str = malloc(100);
  sprintf(str, "%02d:%02d:%02d", hour, minute, second);

  return str;
}

char *unpack_fat12_date(uint16_t packed)
{
  uint16_t year = ((packed & 0xFE00) >> 9) + 1980;
  uint16_t month = (packed & 0x1E0) >> 5;
  uint16_t day = (packed & 0x10) + 1;

  char *str = malloc(100);
  sprintf(str, "%02d-%02d-%d", month, day, year);

  return str;
}

uint16_t lookup_fat_entry(uint16_t idx)
{
  // TODO
  return 0;

}

uint16_t first_12_bits(uint8_t *threebytes)
{
  // TODO
  return 0;
}


uint16_t second_12_bits(uint8_t *threebytes)
{
  // TODO
  return 0;
}
