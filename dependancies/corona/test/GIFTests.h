#ifndef GIF_TESTS_H
#define GIF_TESTS_H


#include "ImageTestCase.h"


class GIFTests : public ImageTestCase {
public:
  void testLoader();
  static Test* suite();
};


#endif
