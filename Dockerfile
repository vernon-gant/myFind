# Use the official Ubuntu base image
FROM ubuntu:latest

# Set the working directory in the container
WORKDIR /myfind

# Install g++ and make
RUN apt-get update && \
    apt-get install -y g++ make

# Copy the current directory contents into the container at /myfind
COPY . /myfind

# Run make to compile the project
RUN make

# Start a shell to keep the container running
CMD ["/bin/bash"]
