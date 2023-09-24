## Prerequisite

These exercises have a couple of dependencies, mainly LLVM and Z3.
They should already be installed on the system, so you shouldn't have anything to do for this.

## Get the exercises

```bash
# Do this just once ;)
cp /net/ens/csi/package.tar.gz .
tar -xf package.tar.gz
cd tp-obfu
```

We need to install a python dependency for the tests to work, and we will do that
through a virtual environment.
Start by creating your virtual environment in the `venv` folder:
```bash
python -m venv venv
```
(You need to do this only once)

Then activate it:
```bash
source ./venv/bin/activate
```

Then install the requirements:
```bash
pip install -r requirements.txt
```

**If you ever close your shell you will have to activate the virtual environment
again, but you don't need to create/install anything again**

## Compiling the exercises

Once you have extracted the exercises, you will need to create a build directory
and configure the project using CMake.

It should look like this:

```bash
mkdir build && cd build
cmake ..
make -j
```

### Compiling and running the tests

Running `make` in the build directory should succeeds, but running the tests with `lit` should fails (because the code isn't actually implemented :)).
The `lit` tool can be used by providing the path to the test directory in the build directory (for instance by running `lit tp1/ex/test` from within the build directory). Using `--filter` can filter tests based on a regexp, using `-v` will show the output for failing tests.

For reference, here is a sample output:

```
tp@adcbc7cb43ab:~/sources/build$ lit tp1/ex/test/ -v
-- Testing: 1 tests, 1 workers --
FAIL: tp1 :: add.c (1 of 1)
******************** TEST 'tp1 :: add.c' FAILED ********************
Script:
--
Exit Code: 1

Command Output (stderr):
--
/home/tp/sources/tp1/ex/test/add.c:25:11: error: CHECK: expected string not found in input
// CHECK: = {{and|xor|or}}
          ^
```

In this specific test we've told `lit` to look for a binary operator that would be introduced by the MBA, since it's not implemented yet it can't find it.

## Disassembling files

We suggest you install IDA free (or Ghidra) to be able to quickly take a look at the generated binary files.
You can download it here: https://www.hex-rays.com/products/ida/support/download_freeware/

## Using Docker

In case you haven't read the beginning of this file, this is not the recommended
way to build the exercises, it should only be used if you want to set them up
on your own computer.

### Pull from DockerHub

The latest image is available from DockerHub using `docker pull viroulep/master-csi:latest`

### Build from Dockerfile

You can use the Dockerfile provided to build the image with all the necessary dependencies for the tp.
Navigate to the `docker` folder and run `docker build -t master-csi .` (you may obviously pick another name for the image).

### Using the image

The recommended way to work on the exercises is to mount the folder with the sources when running the docker container.
This way you can use your preferred text editor from the host machine, and use the docker only to compile the projects and run the tests.

To start an interactive session in a container, you may use the following command line:
`docker run -it -v /path/to/sources:/home/tp/sources -i viroulep/master-csi /bin/bash` (you need to replace `/path/to/sources` with the actual path, and `viroulep/master-csi` by the image name if you changed it).

The default user is `tp` and has sudo rights without password.
