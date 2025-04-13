FROM ghcr.io/epicgames/unreal-engine:dev-slim-5.4.3

WORKDIR /
COPY . /sequence/

USER root
RUN chmod +x ./sequence/buildPlugin.sh
RUN chmod +x ./sequence/healthcheck.sh
