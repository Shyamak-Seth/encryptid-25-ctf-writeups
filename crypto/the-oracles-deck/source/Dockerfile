FROM python:3.11-slim

RUN apt-get update && apt-get install -y socat && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY challenge.py /app/challenge.py

COPY flag.txt /app/flag.txt

RUN chmod 444 /app/flag.txt && useradd -m ctf

USER ctf

EXPOSE 3000

CMD ["socat", "-v", "TCP-LISTEN:3000,reuseaddr,fork", "EXEC:'python /app/challenge.py',pty,setsid,stderr"]
