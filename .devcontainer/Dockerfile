FROM mcr.microsoft.com/devcontainers/python:1-3.12-bullseye

RUN rm -rf /var/lib/apt/lists/*
RUN apt-get clean
RUN apt-get update -o Acquire::CompressionTypes::Order::=gz
RUN export DEBIAN_FRONTEND=noninteractive
RUN apt-get install -y --fix-missing git gcc clang clang-format
RUN pip install flake8
