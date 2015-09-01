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

  int i;
  for (i = 0; i < fat->numRootEntries; i++)
  {
    struct fat12_direntry *this = (fat->rootEntries)[i];
    printf("Extracting %.*s.%.*s...\n", 8, this->filename, 3, this->ext);
    extract_fat12_file(this, fat, binfile);
  }
  

  return 0;
}
