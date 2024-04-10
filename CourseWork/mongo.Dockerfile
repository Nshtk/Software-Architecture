FROM mongo:5.0

ADD ./Code/Database/mongo-init.js /docker-entrypoint-initdb.d