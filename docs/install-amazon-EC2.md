---
title: "Installing IBSimu Client on Amazon EC2 or Ubuntu"
permalink: /install-amazon-EC2/
---
# IBSimu Client installation guide - Amazon EC2 or any Ubuntu machine

This is a step-by-step guide to install IBSimu and IBSimu Client on a Ubuntu machine or a EC2 instance running Ubuntu. It was last updated on May 5, 2021.

## Amazon EC2

This section is relevant only if you choose to install IBSimu and its client on a Amazon EC2 instance.

What instance type you choose heavily depends on the simulation you plan to run. I found that c5d instances from Ohio, such as c5d.4xlarge or c5d.12xlarge, were a good compromise between CPU cores, memory and disk space for running [my lnl simulations](https://github.com/dgasparri/infn-lnl-ibsimu), but your needs might vary. As a rule of thumb, consider:
* IBSimu is relatively fast on modern machines
* IBSimu design is heavily multi-thread, and will use all the vCPU you provide to speed up the simulation
* Yet, even if a big part of the simulation can use multiple cores, some parts are sill single core, such as extracting Twiss parameters. If speed is your top concern, consider saving particle trajectories for later analysis
* I never ran out of memory (on a 32 GB machine or higher)
* I often ran out of disk space, because the particle trajectories database file can easily be a few GB.


The default user of your instance is: ubuntu . You will need to create a public/private key pair to access your instance. On Windows, I personaly use Putty as terminal, and FileZilla to transfer files.

Bigger instances such as the c5d will have one or more SSD NVMe disks. Specifically:

* nvme0n1 - root
* nvme1n1 - SSD NVMe disk 1 - optional, to be mounted
* nvme2n1 - SSD NVMe disk 2 - optional, to be mounted

The root disk is already mounted a /. To check if a disk is already mounted, you can use `parted -l`.

To mount the other(s) disk(s), repeat this process for each disk:

```
$ sudo parted /dev/nvme1n1
 print
 mklabel gpt 
 mkpart primary 2048s -1s 
 print 
```

 `mklabel gpt` creates a partition table, while `mkpart primary 2048s -1s` creates the primary partition in /dev/nvme1n1p1. If no error are raised, the second print command should show the disk with its partition.

Then, you need to format the partition and create a mount point, such as the folder `ibsimu-runs`:

```
$ lsblk -f
$ sudo mkfs.ext4 -L ssd1 /dev/nvme1n1p1
$ lsblk -f
$ mkdir -p ibsimu-runs
$ sudo mount -t auto /dev/nvme1n1p1 ibsimu-runs
$ sudo chown -R ubuntu:ubuntu ibsimu-runs
```

Further references:

* https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/EC2_GetStarted.html
* https://aws.amazon.com/marketplace/pp/B00JV9JBDS/ref=mkt_ste_catgtm_linuxlp
* https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/putty.html
* disk partitioning: https://opensource.com/article/18/6/how-partition-disk-linux
* default access to server: https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/connection-prereqs.html#connection-prereqs-get-info-about-instance


## Prerequisite packages

As of today (May 5, 2021), the IBSimu library requires the following packages: 

* pkg-config
* fontconfig 
* freetype2-demos
* libcairo2-dev
* libpng-dev
* zlib1g zlib1g-dev
* libpthread-stubs0-dev
* libgsl-dev
* libgtk-3-dev
* libgtkglext1-dev
* libumfpack5
* libsuitesparse-dev
* gcc-10 gcc-10-base gcc-10-doc - or higher
* g++-10 - or higher
* libstdc++-10-dev libstdc++-10-doc  - or higher


To get them, run the following commands:

```
$ sudo apt update
$ sudo apt-get install pkg-config fontconfig freetype2-demos libcairo2-dev libpng-dev zlib1g zlib1g-dev libpthread-stubs0-dev libgsl-dev libgtk-3-dev libgtkglext1-dev libumfpack5 libsuitesparse-dev
$ sudo apt install gcc-10 gcc-10-base gcc-10-doc g++-10 libstdc++-10-dev libstdc++-10-doc 
```

## IBSimu installation

This section relies on the official [IBSimu installation documentation](http://ibsimu.sourceforge.net/installation.html). You can reference to that for further details.

First, clone the Git repository of IBSimu in the ibsimu_lib directory and compile the library:

```
$ git clone git://ibsimu.git.sourceforge.net/gitroot/ibsimu/ibsimu ibsimu_lib
$ cd ibsimu_lib
$ ./reconf
$ ./configure
$ make clean
$ make
$ sudo make install
$ cd ..

```

## Boost C++ library installation:

To install the Boost C++ library, find the latest release of the library from the [Boost download page](https://www.boost.org/users/download/) in tar.bz2 format. 

```
wget https://dl.bintray.com/boostorg/release/1.76.0/source/boost_1_76_0.tar.bz2
tar --bzip2 -xf boost_1_75_0.tar.bz2
cd boost_1_75_0
./bootstrap.sh 
sudo ./b2 install
cd ..
sudo ldconfig
```

## IBSimu Client installation

To install the IBSimu client, clone the repository and build it.

```
git clone https://github.com/dgasparri/IBSimu_Client.git ibsimu_client
cd ibsimu_client
mkdir bin
mkdir bin/build
make clean
make
```

The client will be installed in the ibsimu_client directory, and the executables will be in ibsimu_client/bin .

