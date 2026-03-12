// cnn_inference.h
#ifndef CNN_INFERENCE_H
#define CNN_INFERENCE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

float cnn_inference(uint8_t* image, int width, int height);

#ifdef __cplusplus
}
#endif

#endif
