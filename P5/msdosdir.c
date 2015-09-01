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

  struct fat12 *image = NULL;

  image = fat12_init(binfile);

  print_disk_information(image->bs);

  int i;
  for (i = 0; i < image->numRootEntries; i++)
    print_directory_entry((image->rootEntries)[i]);

  printf("%d root directory entries, totaling %d bytes.\n", image->numRootEntries, image->totalRootSize);

}
