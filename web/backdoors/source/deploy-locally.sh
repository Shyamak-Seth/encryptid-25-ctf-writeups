PORT=$1
if [ -n "$PORT" ]; then
    docker build -t encryptid-backdoors .
    echo "[*] RUNNING ON PORT $PORT"
    docker run -it --rm -p $PORT:80 encryptid-backdoors
else
    echo "Usage: ./deploy-locally.sh <PORT_NUMBER>"
fi
