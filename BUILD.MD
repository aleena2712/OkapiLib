docker run -it   --mount type=bind,source=C:\vex-vscode-projects\OkapiLib,target=/usr/local/okapilib   d35f04d5c8b5   

docker exec -it 57d1125587f4 /bin/bash


arm-none-eabi-g++: not found

```sh
mkdir cmake-build-debug
cd cmake-build-debug
cmake -DCMAKE_BUILD_TYPE=Debug -G "CodeBlocks - Unix Makefiles" ..
cmake --build . --target OkapiLibV5 -- -j 2
chmod +x ./OkapiLibV5
./OkapiLibV5
```


```sh
mkdir cmake-build-debug
cd cmake-build-debug
cmake -DCMAKE_BUILD_TYPE=Debug -G "CodeBlocks - Unix Makefiles" ..
cmake --build . --target OkapiLibV5_lib -- -j 2
chmod +x ./OkapiLibV5
./OkapiLibV5
```

OkapiLibV5_lib