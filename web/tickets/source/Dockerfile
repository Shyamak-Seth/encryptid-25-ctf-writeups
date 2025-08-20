FROM python:3.11-slim

WORKDIR /app

COPY src /app

RUN useradd -m ctf

USER ctf

RUN pip install --no-cache-dir flask gunicorn

EXPOSE 5000

CMD ["python", "-m", "gunicorn", "app:app", "--bind", "0.0.0.0:5000", "--workers", "4"]
