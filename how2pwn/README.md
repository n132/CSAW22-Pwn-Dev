# how2pwn
how2pwn is a series of educational challenges(5 stages). I write this challenge for people have little experience on pwn. If you are new to pwn, don't hesitate to start from this challenge. And there would be many hints in the challenge, I am pretty sure most people, who have learned OS&C Language, could solve it. If you don't have an enviroment to start, please use mine: 

`docker push n132/pwn:20.04`


# Solution

All solvers are in this [folder][1]

# Pub

* MD5(./share.zip)= e6dec653fe13b6ba52932cbc5dc92449

This folder would be shown to the public, including the source code and binary. You can run the setup.py script to renew the file in pub.

`python3 ./setup.py release`

# Docker

I wrote a script to build the challenge. It opens 5 docker containers.


Usage:
```python
cd ./Dockerize
./setup clean
./setup build
./setup up
# ./setup down
# ./setup clean
```

[1]: ./solution


