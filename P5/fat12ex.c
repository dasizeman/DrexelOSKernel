#include "fat12.h"

int main(int argc, char **argv)
{
  const char *binpath = "samplefat.bin";

  FILE *binfile = fopen(binpath, "r");
  if (!binfile)
  {
    printf("Failed to open file!\n");
    return 1;
  }
  else
    printf("Opened file.\n");

  struct fat12_bs *bootsector = NULL;

  int res;
  res = fat12_init(&binfile, &bootsector);

  if (res < 0)
  {
    printf("Bootsector read failed!\n");
    return 1;
  }
  else
    printf("Successfully read bootsector!\n");
  
  printf("Volume label: %.*s\n",(int)sizeof(bootsector->bsVolumeLabel), 
      bootsector->bsVolumeLabel);
  printf("Volume serial: %1X\n", bootsector->bsSerialNumber);

}
