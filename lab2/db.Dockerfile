FROM postgres:16

ADD ./src/commands.sql /docker-entrypoint-initdb.d