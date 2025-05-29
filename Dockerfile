FROM ros:humble-ros-core

RUN apt-get update \
    && apt-get upgrade -y 


RUN apt-get install -y \
    git \
    cmake \
    build-essential \
    nlohmann-json3-dev \
    python3-colcon-common-extensions

RUN apt-get install -y \
    libasound2-dev \
    libpulse-dev \
    libssl-dev \
    libcurl4-openssl-dev \
    libspeex-dev \
    libv4l-dev \
    libx11-dev 

# Install PJSIP
WORKDIR /home
RUN git clone https://github.com/pjsip/pjproject.git \
    && cd pjproject \
    && ./configure --enable-shared \
    && make dep \
    && make \
    && make install \
    && ldconfig

# Set up environment variables for PJSIP
ENV PKG_CONFIG_PATH=/usr/local/lib/pkgconfig
ENV LD_LIBRARY_PATH=/usr/local/lib

WORKDIR /home/workspace/src
COPY src/adapter_pabx adapter_pabx
RUN git clone https://github.com/KABAM-Robotics/kabam_msgs.git -b pbx

WORKDIR /home/workspace
RUN /ros_entrypoint.sh colcon build && \
    sed -i '$isource "/home/workspace/install/setup.bash"' /ros_entrypoint.sh

ENTRYPOINT [ "/ros_entrypoint.sh" ]

CMD ["ros2", "run",  "adapter_pabx", "announcement_server", "--ros-args", "-p", "console_level:=0"]