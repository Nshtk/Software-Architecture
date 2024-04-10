FROM postgres:16

ADD ./Code/Database/commands.sql /docker-entrypoint-initdb.d