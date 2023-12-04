FROM ubuntu:latest

WORKDIR /myfind

RUN apt-get update && \
    apt-get install -y g++ make

COPY . /myfind

RUN make

CMD ["/bin/bash"]
