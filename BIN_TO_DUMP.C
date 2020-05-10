// compiled with gcc (Ubuntu 5.5.0-12ubuntu1~16.04) 5.5.0 20171010
// gcc -x c  OMC_MAIN.C -o omc
/*#define atariST*/
#define buflen 120
#define asem_col 19
#define _MAX_PATH 260        // this is a hack to replace win32 value
#define strnicmp strncasecmp // this is a hack to replace deprecated strnicmp
#define _getcwd getcwd       // ditto
#define _chdir chdir         // ditto

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <curses.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <termios.h>
#include <unistd.h>

FILE *fin, *fout;
uint8_t memory[65536];

void error_exit() {

  if (fin) {
    fclose(fin);
  }
  if (fout) {
    fclose(fout);
  }
  exit(1);
}
void error(const char *text) {

  fprintf(stderr, "%s", text);
  error_exit();
}

void usage(const char *argv[]) {

  fprintf(stderr, "usage:\n\t%s INPUT_ATARI_BIN_FILE OUTPUT_MEMORY_DUMP_FILE\n", argv[0]);
  exit(1);
}

int main(int argc, const char *argv[]) {

  uint8_t block_header[4];
  uint16_t offset;
  int ret;
  if (argc < 3) {
    usage(argv);
  }
  offset = 0;
  fin = fopen(argv[1], "rb");
  if (fin == NULL) {
    error("Can't ipen input file");
  }

  FILE *fout = fopen(argv[2], "wb");
  if (fout == NULL) {
    error("Can't open/create output file");
  }

  if (fread(&block_header[0], 2, 1, fin) != 1) {
    error("Can't read input's file header tag");
  }
  if (block_header[0] != 0xff && block_header[1] != 0xff) {
    error("incorrect input file header tag - expected 0xff, oxff");
  }
  offset = 2;

  while (true) {
    while ((ret = fread(&block_header[0], 1, 2, fin)) == 2 && block_header[0] == 0xff && block_header[1] == 0xff) {
      offset += 2;
    }
    offset += (ret > 0) ? ret : 0;
    if (ret == 0) {
      break;
    } else if (ret < 2) {
      fprintf(stderr, "Can't read block header start - offset=%d, ret=%d, errno=%d\n", offset, ret, errno);
      error_exit();
    }
    ret = fread(&block_header[2], 1, 2, fin);
    if (ret < 2) {
      fprintf(stderr, "Can't read block header end - offset=%d, ret=%d, errno=%d\n", offset, ret, errno);
      error_exit();
    }
    offset += 2;
    uint16_t start, end;

    start = block_header[1];
    start <<= 8;
    start |= block_header[0];

    end = block_header[3];
    end <<= 8;
    end |= block_header[2];

    if (start >= end) {
      fprintf(stderr, "Wrong block start/end addresses - expected start < end, start=%04x, end=%04x offset=%04x\n",
              (unsigned int)start, (unsigned int)end, (unsigned int)offset);
      error_exit();
    }
    uint16_t seg_size = end + 1 - start;
    if ((ret = fread(&memory[start], 1, seg_size, fin)) != seg_size) {
      fprintf(stderr, "When processing block start=%04x, end=%04x  - can't read start-end:%04x bytes offset=%04x\n",
              (unsigned int)start, (unsigned int)end, (unsigned int)(end - start), (unsigned int)offset);
      error_exit();
    }
    offset += seg_size;
  }
  if (ret > 0) {
    fprintf(stderr, "Can't read entire block spec - expected 4 bytes, got: %d - errno=%d offset=%04x", ret, errno,
            (unsigned int)offset);
    error_exit();
  } else if (ret < 0) {
    fprintf(stderr, "Can't read block spec - errno: %d offset=%04x", errno, (unsigned int)offset);
    error_exit();
  }
  ret = fwrite(memory, 1, sizeof(memory), fout);
  if (ret != sizeof(memory)) {
    fprintf(stderr, "Can't write output file = ret=%d errno=%d", ret, errno);
    error_exit();
  }
  fclose(fout);
  fclose(fin);
  return 0;
}
