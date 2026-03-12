// TinyCNN for STM32 - int8 quantized
#ifndef CNN_MODEL_H
#define CNN_MODEL_H

#include <stdint.h>

#define CNN_INPUT_SIZE 34
#define CNN_INPUT_CHANNELS 1
#define CNN_OUTPUT_SIZE 1

#include "conv1_weight.h"
#include "conv1_bias.h"

#define CONV1_IN_CH 1
#define CONV1_OUT_CH 8
#define CONV1_KERNEL 3
#define CONV1_STRIDE 1
#define CONV1_PADDING 1

#include "conv2_weight.h"
#include "conv2_bias.h"

#define CONV2_IN_CH 8
#define CONV2_OUT_CH 16
#define CONV2_KERNEL 3
#define CONV2_STRIDE 1
#define CONV2_PADDING 1

#include "fc1_weight.h"
#include "fc1_bias.h"

#define FC1_IN_FEATURES 1024
#define FC1_OUT_FEATURES 32

#include "fc2_weight.h"
#include "fc2_bias.h"

#define FC2_IN_FEATURES 32
#define FC2_OUT_FEATURES 1

// Total model size: 34,081 bytes (int8)
#define CNN_TOTAL_SIZE 34081

#endif // CNN_MODEL_H
