#include "gtestInclude.h"



#define STB_IMAGE_IMPLEMENTATION
#include "../../third/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../third/stb_image_write.h"



int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}