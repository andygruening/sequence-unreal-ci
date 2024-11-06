FROM ghcr.io/epicgames/unreal-engine:dev-slim-5.4.3

WORKDIR /

COPY . /SequencePlugin/

USER root
RUN chmod +x ./SequencePlugin/build.sh

CMD ["./SequencePlugin/build.sh"]