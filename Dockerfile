FROM ubuntu

ENV TZ=Europe/Kiev
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
RUN apt-get update
RUN apt-get install -y cmake xz-utils git wget python3 gcc build-essential python3-pip
WORKDIR /home
RUN ls -la
RUN pwd
RUN wget https://cmocka.org/files/1.1/cmocka-1.1.5.tar.xz
RUN tar -xf "cmocka-1.1.5.tar.xz"
WORKDIR /home/cmocka-1.1.5/build
RUN cmake ../ -DCMAKE_INSTALL_PREFIX=/usr
RUN make install
WORKDIR /home
RUN python3 -m pip install tinydb rich