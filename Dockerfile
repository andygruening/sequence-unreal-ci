FROM ghcr.io/epicgames/unreal-engine:dev-slim-5.4.3

WORKDIR /

COPY . /

USER root
RUN chmod +x ./build.sh

CMD ["./build.sh"]