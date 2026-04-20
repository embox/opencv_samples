```markdown
# opencv_samples - Enhanced with Embedded Cascades and TinyCNN

This repository is a fork of the OpenCV samples adapted for Embox. It contains two major enhancements:

1. **Embedded Haar Cascades** - All XML files are converted to binary arrays, eliminating filesystem dependency
2. **Hybrid Haar-CNN Face Detector** - A novel approach combining classical Haar with a tiny CNN (34KB, 98.8% accuracy)

## Key Features of This Fork

### Embedded Cascades (Filesystem-Free)
All samples now load Haar cascade XML files directly from memory as binary arrays, crucial for embedded systems where storage is limited or filesystems are unavailable.

**Modified samples:**
- `facedetect` - Face and eye detection (now with embedded cascades)
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

Now templates are available:

```
$ make confload
...
ext_project/opencv_samples/stm32f746g-discovery
ext_project/opencv_samples/stm32f769i-discovery
ext_project/opencv_samples/stm32f769i-discovery-extmem
ext_project/opencv_samples/arm-qemu
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

### STM32F769I-Discovery & STM32F746G-Discovery

You can run samples on STM32F769I-Discovery from internal ROM (2 MB):

```
$ make confload-ext_project/opencv_samples/stm32f769i-discovery
$ make // you can optionally pass -j<n> to speed up build
```

You can also run samples STM32F769I-Discovery from internal QSPI (64 MB):

```
$ make confload-ext_project/opencv_samples/stm32f769i-discovery-extmem
$ make // you can optionally pass -j<n> to speed up build
```

STM32F746G-Discovery has 1 MB in-chip flash and 16 MB QSPI:

```
$ make confload-ext_project/opencv_samples/stm32f746g-discovery
$ make // you can optionally pass -j<n> to speed up build
```

#### Getting OpenOCD
You will need **OpenOCD >= 0.11.0**. You can download it [here](http://openocd.org/getting-openocd/).

#### Flash and run Embox

Start OpenOCD. If you built OpenOCD from sources:

```
$ cd <path to openocd>/tcl
```

Pass the corresponding `*.cfg` to openocd:

```
$ sudo ../src/openocd -f board/stm32f769i-disco.cfg
```

Then go to embox/ and run:

```
./scripts/gdb_run_stm32f7_qspi.sh
```

Embox will be loaded and you can call commands from conf/start_script.inc.

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
```