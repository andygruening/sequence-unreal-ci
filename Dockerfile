FROM ghcr.io/epicgames/unreal-engine:dev-slim-5.4.3

WORKDIR /

COPY . /

RUN chmod +x /build.sh

CMD ["./build.sh"]