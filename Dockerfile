FROM ubuntu:22.04

# Install build tools and bash
RUN apt-get update && apt-get install -y \
    build-essential \
    bash \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy everything into container
COPY . /app

# Compile the C program
RUN gcc -Wall -pthread p3190156-p3190289-res.c -o reservation

# Make scripts executable
RUN chmod +x test/run-tests.sh main/run-main.sh

# Default command (can be overridden by docker-compose)
CMD ["bash"]
