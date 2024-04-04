FROM postgres:16

ADD ./commands.sql /docker-entrypoint-initdb.d