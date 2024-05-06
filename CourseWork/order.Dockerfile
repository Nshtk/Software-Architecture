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
RUN git clone https://github.com/tdv/redis-cpp.git  &&\
	cd redis-cpp &&\
	mkdir cmake-build  &&\
	cd cmake-build  &&\
	cmake ..  &&\
	make  &&\
	make install
RUN ldconfig

WORKDIR /opt/Code
COPY ./Code/Common ./Common
COPY ./Code/Database ./Database
COPY ./Code/Services/OrderService/src ./Services/OrderService/src
COPY ./Code/Services/OrderService/CMakeLists.txt ./Services/OrderService
RUN mkdir ./Services/OrderService/build
RUN cmake -B ./Services/OrderService/build -S ./Services/OrderService
RUN make -C ./Services/OrderService/build
ENTRYPOINT [ "./Services/OrderService/build/order_service" ]
#ENTRYPOINT ["tail", "-f", "/dev/null" ]
