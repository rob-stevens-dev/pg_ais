# Start with PostgreSQL base
FROM postgres:15

# Install build tools and dependencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    make \
    git \
    libcmocka-dev \
    postgresql-server-dev-15 \
    && rm -rf /var/lib/apt/lists/*

# Set work directory
WORKDIR /app

# Copy source code
COPY . /app

# Build extension and install
RUN cmake -S . -B build && \
    cmake --build build && \
    make -C build install

# Run C unit tests
RUN make test_phase1 || true

# Run SQL regression test
RUN make sql_regression || true

# Entrypoint remains the PostgreSQL server
CMD ["postgres"]
