This is a minimal demo using StereoKit and OpenCV to display a webcam view in XR! 

It's just a little educational resource I wanted to make - to show people some super basic multithreading (putting all the slow camera code on one thread and all the fast XR code on another) and how to hook up various C/C++ libraries using CMake.

It works in XR and on flatscreen, meaning all you need to try this is a Linux machine with a webcam, no HMDs necessary!

### Building
Currently, this only works on Linux.

First, follow the instructions at https://github.com/maluoi/StereoKit/blob/master/BUILDING.md to get StereoKit installed.

Next, get OpenCV's C++ development library and CMake - on Ubuntu, it'd be
```
sudo apt install libopencv-dev cmake
```
(There might be a lot more packages you need, but you'll probably have gotten them as you installed StereoKit)

Then, build this project itself with something like
```
git clone https://github.com/slitcch/sk-webcam-example
cd sk-webcam-example
mkdir build && cd build
cmake ..
make -j24
```

Then, to run it, do
```
./sk-webcam-example
```

### Getting help
Whoever you are reading this right now, these instructions probably won't work for you by themselves because there are a ton of other dependencies I'm forgetting about + don't have time to write down right here.

Open-source XR is a super new domain, and everybody who's working in it right now got help from somebody. Feel free to hit me up in the [StereoKit discord server](https://discord.gg/jtZpfS7nyK) if you want help or just want to discuss things!

### Contributing?

I don't really have anything I want to do with this; 'twas just a fun project. But I would take PRs for
* A UI for selecting a webcam and picking a resolution; not just letting OpenCV pick the first one at the default resolution
* Making the webcam window grabbable and more fun
* Windows build

### A little demo with my North Star:
https://diode.zone/w/3vgyZFRvaPazBukmxBtWKF
