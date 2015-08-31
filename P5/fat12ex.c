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

  printf("First fat pos: 0x%1X\n", image->FATPos);
  printf("Root pos: %#1X\n", image->rootPos);
  printf("Data pos: %#1X\n", image->dataPos);
  printf("%d root directory entries.\n", image->numRootEntries);

  int i;
  for (i = 0; i < image->numRootEntries; i++)
    print_directory_entry((image->rootEntries)[i]);

}
