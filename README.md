# MPI
This repository has several MPI programs, which solves different kind of parallel problems.

## Pre-Requisites

	- C/C++ compiler
	- A working version of MPI installed on your system
	- Python package: mpi4py

## Installing and building MPI

#### Step 1) Download a stable version of OpenMPI 

```sh
wget -nc https://download.open-mpi.org/release/open-mpi/v4.0/openmpi-4.0.2.tar.gz
```

#### Step 2) Building OpenMPI from source

##### Create a folder to store the building files

```sh
cd ~
mkdir openmpi-build; mv Downloads/openmpi-4.0.2.tar.gz openmpi-build; cd openmpi-build
gunzip -c openmpi-4.0.2.tar.gz | tar xf -
cd openmpi-4.0.2
```
##### Run the configuration script by setting the instalation location of the MPI libraries and header files to /usr/local, so that CMake can easily find everything

```sh
./configure --prefix=/usr/local
```

##### Compile OpenMPI from source

```sh
sudo make all install
```
