FROM ubuntu:22.04

RUN apt update && apt-get update && apt-get install -y socat

WORKDIR /app

COPY chall .

COPY flag.txt /

RUN chmod 444 /flag.txt

RUN chmod +x ./chall

RUN useradd ctf

USER ctf

EXPOSE 3000

CMD ["socat", "-v", "TCP-LISTEN:3000,reuseaddr,fork", "EXEC:'bash -c \"stty raw -echo; ./chall\"',pty,setsid,stderr"]