FROM openjdk:8-jdk-alpine
WORKDIR /
COPY SpDbReader.jar /SpDbReader.jar
COPY SpDbReader.sh /SpDbReader.sh
RUN chmod +x /SpDbReader.sh
ENTRYPOINT ["/SpDbReader.sh"]
CMD [""]