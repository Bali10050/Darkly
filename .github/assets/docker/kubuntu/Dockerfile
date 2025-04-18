FROM ubuntu:24.10
MAINTAINER DeltaCopy
USER root
RUN apt-get update && \
    apt-get dist-upgrade -y --allow-downgrades && \
    apt-get install -y -qq console-setup- curl checkinstall git build-essential cmake kubuntu-desktop

