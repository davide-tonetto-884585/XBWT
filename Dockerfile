FROM ubuntu:22.04

# Install essential build tools and dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    gdb \
    wget \
    sudo \
    valgrind \
    openssh-server \
    && rm -rf /var/lib/apt/lists/*

# Install SDSL library from source to the correct system-wide location
RUN git clone https://github.com/simongog/sdsl-lite.git /tmp/sdsl-lite \
    && cd /tmp/sdsl-lite \
    && cd build \
    && cmake .. \
    && make \
    && make install \
    && ldconfig \
    && rm -rf /tmp/sdsl-lite

# Configure SSH
RUN mkdir /var/run/sshd
RUN echo 'root:password' | chpasswd
RUN sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config
RUN sed -i 's/#PasswordAuthentication yes/PasswordAuthentication yes/' /etc/ssh/sshd_config

# Create a non-root user for development
RUN useradd -m -s /bin/bash developer && \
    echo 'developer:developer' | chpasswd && \
    usermod -aG sudo developer

# Set working directory
WORKDIR /app

# Copy your project files
COPY . /app/
RUN chown -R developer:developer /app

# Expose SSH port
EXPOSE 22

# Start SSH service
CMD ["/usr/sbin/sshd", "-D"]