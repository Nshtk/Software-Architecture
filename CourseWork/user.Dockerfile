FROM mcr.microsoft.com/devcontainers/cpp:1-ubuntu-22.04

COPY ./Code/reinstall-cmake.sh /tmp/
ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="3.22.2"
RUN if [ "${REINSTALL_CMAKE_VERSION_FROM_SOURCE}" != "none" ]; then \
        chmod +x /tmp/reinstall-cmake.sh && /tmp/reinstall-cmake.sh ${REINSTALL_CMAKE_VERSION_FROM_SOURCE}; \
    fi \
    && rm -f /tmp/reinstall-cmake.sh

ENV TZ=Europe/Moscow
RUN apt-get update && apt-get install -y git python3 pip iputils-ping cmake gcc-12 libpq-dev postgresql-client wrk libssl-dev zlib1g-dev librdkafka-dev mysql-client libmysqlclient-dev libboost-all-dev\
    && apt-get clean

RUN git clone -b poco-1.12.4-release https://github.com/pocoproject/poco.git &&\
    cd poco &&\
    mkdir cmake-build &&\
    cd cmake-build &&\
    cmake .. &&\
    cmake --build . --config Release -- -j 12 &&\
    cmake --build . --target install -- -j 12 &&\
    cd && rm poco/* -rf 
RUN ldconfig

WORKDIR /opt/Code
COPY ./Code/Common ./Common
COPY ./Code/Database ./Database
COPY ./Code/Services/UserService/src ./Services/UserService/src
COPY ./Code/Services/UserService/CMakeLists.txt ./Services/UserService
RUN mkdir ./Services/UserService/build
RUN cmake -B ./Services/UserService/build -S ./Services/UserService
RUN make -C ./Services/UserService/build
ENTRYPOINT [ "./Services/UserService/build/user_service" ]
#ENTRYPOINT ["tail", "-f", "/dev/null" ]
