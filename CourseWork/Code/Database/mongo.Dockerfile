FROM mongo:5.0

ADD ./mongo-init.js /docker-entrypoint-initdb.d