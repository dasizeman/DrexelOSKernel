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
  
  fatptr->rootEntries = read_root_directory(file, fatptr->rootPos);

  return fatptr;

}


struct fat12_direntry **read_root_directory(FILE *file, uint32_t offset)
{
 //TODO
 return NULL;
}

struct fat12_direntry **read_fat12_directory(uint8_t *data, uint32_t size)
{
  //TODO
}

struct fat12_direntry *read_directory_entry(uint8_t *entry)
{
 //TODO
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
  // TODO
}

void print_disk_information(struct fat12_bs *bootsector)
{
  printf("Volume label: %.*s\n",(int)sizeof(bootsector->bsVolumeLabel), 
          bootsector->bsVolumeLabel);

  printf("Volume serial: %1X\n", bootsector->bsSerialNumber);
}
