/**
 * fat12.c
 * Author: Dave Sizer
 * Date: 08/2015
 */

#include "fat12.h"

int fat12_init(FILE **file, struct fat12_bs **bs)
{
  if (!(*bs = malloc(sizeof(struct fat12_bs))))
    return -1;

  if(!fread(*bs, sizeof(struct fat12_bs), 1, *file))
    return -1;

  return 0;

}

void print_disk_information(struct fat12_bs *bootsector)
{
  printf("Volume label: %.*s\n",(int)sizeof(bootsector->bsVolumeLabel), 
          bootsector->bsVolumeLabel);

  printf("Volume serial: %1X\n", bootsector->bsSerialNumber);
}
