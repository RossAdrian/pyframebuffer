FROM mcr.microsoft.com/devcontainers/python:1-3.12-bullseye

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive
RUN apt-get install -y --fix-missing git gcc clang
RUN pip install flake8
