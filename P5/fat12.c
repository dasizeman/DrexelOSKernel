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
  fatptr->FATPos = BOOTSECTOR_LENGTH;
  fatptr->rootPos = calculate_root_position(bs);
  fatptr->dataPos = calculate_data_position(bs);
  fatptr->numRootEntries = 0;
  fatptr->totalRootSize = 0;
  
  fatptr->rootEntries = read_root_directory(file, fatptr);

  return fatptr;

}


struct fat12_direntry **read_root_directory(FILE *image, struct fat12 *fat)
{
  // Recalculate root sectors
  uint16_t bps;
  bps = fat->bs->bpbBytesPerSector;
  uint16_t root_sectors = fat->bs->bpbRootEntries * DIRENTRY_SIZE / bps;
  size_t size = root_sectors*bps;

  // Allocate a buffer for the directory data
  uint8_t *buffer = NULL;
  if (!(buffer = malloc(size)))
  {
    fprintf(stderr, "read_root_directory: allocation failure\n");
    return NULL;
  }

  // Seek to the root directory
  fseek(image, fat->rootPos, SEEK_SET);

  // Read into the buffer
  if (!fread(buffer, size, 1, image))
  {
    fprintf(stderr, "read_root_directory: read failed\n");
    return NULL;
  }

  struct fat12_direntry **dirs = read_fat12_directory(buffer, fat->bs->bpbRootEntries, &(fat->numRootEntries), &(fat->totalRootSize));
  free(buffer);

  //printf("Buffer dump:\n");
  //int i;
  //for (i=0; i < 100; i++)
    //printf("%02X ", buffer[i]);

  return dirs;
}

struct fat12_direntry **read_fat12_directory(uint8_t *data, uint16_t rootentries, uint16_t *count, uint32_t *total)
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

    // I'm not sure what zero filesize items mean but we probably don't want
    // them
    if (entry->filesize == 0)
    {
      free(entry);
      data += DIRENTRY_SIZE;
      continue;
    }

    (*count)++;
    (*total) += entry->filesize;
    idx++;
    data += DIRENTRY_SIZE;
  }

  return dirs;
}

uint32_t extract_fat12_file(struct fat12_direntry *dir, struct fat12 *fat, FILE *image)
{
  uint32_t total_written = 0;

  // Open the file for writing
  char *outname = malloc(13);
  sprintf(outname, "%.*s.%.*s", 8, dir->filename, 3, dir->ext);
  FILE *outfile = fopen(outname, "w");

  uint32_t remaining_to_write = dir->filesize;
  uint16_t current_fat_index = dir->starting_cluster;

  while(!(lookup_fat_entry(current_fat_index, fat, image) >= 0xFF8 
        && lookup_fat_entry(current_fat_index, fat, image) <= 0xFFF))
  {
    // Write the cluster at the current index
    fseek(image, fat->dataPos, SEEK_SET);
    fseek(image, 512*current_fat_index, SEEK_CUR);

    uint8_t *buffer = malloc(512);
    fread(buffer, 512, 1, image);
    fwrite(buffer, 512, 1, outfile);
    remaining_to_write -= 512;
    total_written += 512;
    free(buffer);

    // Update the current fat index
    current_fat_index = lookup_fat_entry(current_fat_index, fat, image);
  }

  // Write any remaining bytes
  fseek(image, fat->dataPos, SEEK_SET);
  fseek(image, 512*current_fat_index, SEEK_CUR);

  uint8_t *buffer = malloc(remaining_to_write);
  fread(buffer, remaining_to_write, 1, image);
  fwrite(buffer, remaining_to_write, 1, outfile);
  total_written += remaining_to_write;
  free(buffer);

  fclose(outfile);

  return total_written;

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
  printf("%10d", dir->filesize);
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

uint16_t lookup_fat_entry(uint16_t idx, struct fat12 *fat, FILE *image)
{
  // Seek to the FAT
  fseek(image, fat->FATPos, SEEK_SET);

  // Which 3-byte group is it in?
  uint32_t bit_offset = 12 * idx;
  uint16_t group_idx = bit_offset / 24;

  // Seek to the correct 3-byte group
  fseek(image, group_idx*3, SEEK_CUR);

  // Read the bytes
  uint8_t *bytes = malloc(3);
  fread(bytes, 3, 1, image);

  // Is this particular entry the first three nibbles or second?
  return (bit_offset % 24 == 0)?first_12_bits(bytes):second_12_bits(bytes);

}

uint16_t first_12_bits(uint8_t *threebytes)
{
  // UV WX YZ = XUV YZW

  uint8_t first = *threebytes;
  uint8_t second = *(threebytes + 1);
  uint8_t third = *(threebytes + 2);

  uint16_t res = 0;

  // XUV
  res = ((second & 0xF) << 8) | first;

  return res;
}


uint16_t second_12_bits(uint8_t *threebytes)
{
  // UV WX YZ = XUV YZW

  uint8_t first = *threebytes;
  uint8_t second = *(threebytes + 1);
  uint8_t third = *(threebytes + 2);

  uint16_t res = 0;

  res = (third << 4) | ((second & 0xF0) >> 4);

  return res;
}
