# opencv_samples

There are OpenCV samples copied from opencv/samples/cpp and adopted a little ro run on Embox. These samples are located in cmds/ folder.
Images for testing purposes are located in data_samples/. Embox's configurations are located in templates/.

You can run these sample on Embox (qemu, stm32) and on Linux (your PC).

## Running on Embox

You need to have both repos cloned - embox and opencv_samples.

```
$ cd embox/
$ make ext_conf EXT_PROJECT_PATH=<path-to>/opencv_samples
```

Now templates are available:

```
$ make confload
...
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

### STM32F769I-Discovery

You can run some samples from internal ROM (2 MB):
```
$ make confload-ext_project/opencv_samples/stm32f769i-discovery
$ make // you can optionally pass -j<n> to speed up build
```

You can also run samples from internal QSPI (64 MB):
```
$ make confload-ext_project/opencv_samples/stm32f769i-discovery-extmem
$ make // you can optionally pass -j<n> to speed up build
```

#### Install OpenOCD

#### Flash and run Embox
