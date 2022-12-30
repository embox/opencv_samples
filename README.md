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

### STM32F769I-Discovery & STM32F746G-Discovery

You can run some samples on STM32F769I-Discovery from internal ROM (2 MB):
```
$ make confload-ext_project/opencv_samples/stm32f769i-discovery
$ make // you can optionally pass -j<n> to speed up build
```

You can also run samples STM32F769I-Discovery from internal QSPI (64 MB):
```
$ make confload-ext_project/opencv_samples/stm32f769i-discovery-extmem
$ make // you can optionally pass -j<n> to speed up build
```

STM32F746G-Discovery has 1 MB in-chip flash and 16 MB QSPI
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
