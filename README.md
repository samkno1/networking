# Communication over UNIX domain sockets

Fetch the source
```
$ git clone "https://github.com/samkno1/networking.git"
$ cd networking/
```

## Build the docker container
Build the container in which the application will run
```
$ docker build -t networking .
```

## Build and run app
Enter the docker container
```
docker run -it networking /bin/bash
```
There are 2 applications: server and client.
```
$ cd networking/
$ g++ server.cpp -o server
$ g++ client.cpp -o client
```
To demo the app
```
$ ./server /tmp/socket &
$ ./client /tmp/socket VERSION
$ ./client /tmp/socket TEST
```

## Cross-compile app for aarch64
```
$ cd /opt/
$ aarch64-linux-g++ networking/server.cpp -o aarch64/server \
    --sysroot /opt/aarch64--glibc--stable-2025.08-1/aarch64-buildroot-linux-gnu/sysroot
$ aarch64-linux-g++ networking/client.cpp -o aarch64/client \
    --sysroot /opt/aarch64--glibc--stable-2025.08-1/aarch64-buildroot-linux-gnu/sysroot
```
