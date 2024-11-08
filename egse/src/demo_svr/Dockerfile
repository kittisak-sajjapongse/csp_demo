FROM python:3.12.7-slim
RUN mkdir -p /app/logs
WORKDIR /app
COPY demo_svr/demo_svr.py /app
CMD ["python", "demo_svr.py"]