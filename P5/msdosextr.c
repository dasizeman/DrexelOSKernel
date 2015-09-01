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

  struct fat12 *fat = NULL;

  fat = fat12_init(binfile);

  //uint16_t samplecluster = (fat->rootEntries)[0]->starting_cluster;
  //printf("Testing FAT index %d\n", 3);
  //uint16_t fatval = lookup_fat_entry(3, fat, binfile);
  //printf("Got value %d\n", fatval);
  
  struct fat12_direntry *sample = (fat->rootEntries)[0];
  extract_fat12_file(sample, fat, binfile);
  

  return 0;
}
