Author: Alhajali Jalal

Original work: March 2026

This implementation is part of research at [MIPT University]

Contact: Alkhazhali.zh@phystech.edu

# opencv_samples - Enhanced with Embedded Cascades and TinyCNN

This repository is a fork of the OpenCV samples adapted for Embox. It contains two major enhancements:

1. **Embedded Haar Cascades** - All XML files are converted to binary arrays, eliminating filesystem dependency
2. **Hybrid Haar-CNN Face Detector** - A novel approach combining classical Haar with a tiny CNN (34KB, 98.8% accuracy)

## Key Features of This Fork

### Embedded Cascades (Filesystem-Free)
All samples now load Haar cascade XML files directly from memory as binary arrays, crucial for embedded systems where storage is limited or filesystems are unavailable.

**Modified samples:**
- `embedded_facedetect` - Face and eye detection (now with embedded cascades)
- `haar_cnn_demo` - NEW: Hybrid Haar + CNN detector
- `cnn_test` - NEW: test images for CNN

### New: Hybrid Haar-CNN Face Detector (`haar_cnn_demo`)
Located in `cmds/haar_cnn_demo/`, this implementation combines classical Haar cascade with a tiny CNN:

```
Input Image → Haar Cascade → ROI Proposals (34 x 34) → TinyCNN (verifier) → Final Decision
```

### New: Test images (`cnn_test`)
Located in `data_samples/cnn_test/`, contains test images faces/non-faces


**Specifications:**
- TinyCNN architecture: 34KB model (int8 quantized), 34×34 input
- 98.8% accuracy on test set
- 95% reduction in Haar false positives
- Constant inference time per ROI (~40-60ms on Embox)
- Trained weights included as C headers (int8 quantized)

**Performance Results:**
```
Test Set (1,950 images: 1,050 faces + 900 non-faces)

Accuracy:  98.8%
Precision: 98.9%
Recall:    98.9%
F1-Score:  0.989

Confusion Matrix:
           Predicted
           Face  Non-face
Actual-Face 1038    12
Non-face     11    889

False Positive Rate: 1.22%
False Negative Rate: 1.14%
```

## Running on Embox

You need to have both repos cloned - embox and this opencv_samples fork.

```
$ cd embox/
$ make ext_conf EXT_PROJECT_PATH=<path-to>/opencv_samples
```

### QEMU

```
$ make confload-ext_project/opencv_samples/arm-qemu
$ make // you can optionally pass -j<n> to speed up build
```

Run:

```
./scripts/qemu/auto_qemu
```

Inside Embox, you can run:

```
# Original face detection (now with embedded cascades)
facedetect /data/faces/lena_512x512.png

# New hybrid CNN-Haar detector
haar_cnn_demo /data/cnn_test/faces/face_test1.png
```

## How It Works

### Embedded Cascades
The Haar cascade XML files are converted to C arrays using:

```
xxd -i haarcascade_frontalface_alt.xml > face_cascade.inc
```

### TinyCNN Architecture
```
Input: 34×34 grayscale
    ↓
Conv1 (8 filters) → ReLU → MaxPool (17×17)
    ↓
Conv2 (16 filters) → ReLU → MaxPool (8×8)
    ↓
FC1 (1024→32) → ReLU → FC2 (32→1) → Sigmoid
```

### Quantization
Weights are quantized to int8 with per-layer scaling, reducing size from 136KB to 34KB with **zero accuracy loss**.

## Building from Source (for your own training)

If you want to retrain the CNN with your own data:
check the link -> (link will be available soon)

## License

Same as original OpenCV samples (BSD license).

## Acknowledgments

- Original OpenCV samples team
- Embox developers
- LFW dataset for face images
- PASS dataset for hard negative mining
