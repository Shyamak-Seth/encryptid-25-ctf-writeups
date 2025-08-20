PORT=$1
if [ -n "$PORT" ]; then
    docker build -t encryptid-hex-converter .
    echo "[*] RUNNING ON PORT $PORT"
    docker run -it --rm -p $PORT:3000 encryptid-hex-converter
else
    echo "Usage: ./deploy-locally.sh <PORT_NUMBER>"
    echo "Example: ./deploy-locally.sh 7777"
fi