#include <coopy/IntSheet.h>

#include <stdio.h>
#include <stdlib.h>

using namespace coopy::store;

int main(int argc, char *argv[]) {
  if (argc<3) return 1;
  IntSheet s;
  int h = atoi(argv[1]);
  int w = atoi(argv[2]);
  s.resize(w,h,0);
  int ct = 0;
  for (int y=0; y<h; y++) {
    for (int x=0; x<w; x++) {
      ct++;
      s.cell(x,y) = ct;
    }
  }
  SheetStyle style;
  printf("%s",s.encode(style).c_str());
  return 0;
}

