FROM postgres:15

RUN apt-get update && \
    apt-get install -y make gcc cmake postgresql-server-dev-15 libcmocka-dev && \
    rm -rf /var/lib/apt/lists/*

COPY . /pg_ais
WORKDIR /pg_ais

RUN make && make install
COPY docker/init.sql /docker-entrypoint-initdb.d/
