FROM debian:7

RUN mkdir -p /ft_ping

COPY Makefile /ft_ping/Makefile
COPY srcs /ft_ping/srcs
COPY includes /ft_ping/includes
COPY srcs.mk /ft_ping/srcs.mk
COPY entrypoint.sh /ft_ping/entrypoint.sh

RUN chmod +xw /ft_ping/entrypoint.sh

WORKDIR /ft_ping

RUN [ "/bin/sh entrypoint.sh" ]
