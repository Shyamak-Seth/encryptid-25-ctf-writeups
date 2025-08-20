PORT=$1
if [ -n "$PORT" ]; then
    docker build -t encryptid-heaps-of-bagels .
    echo "[*] RUNNING ON PORT $PORT"
    docker run -it --rm -p $PORT:3000 encryptid-heaps-of-bagels
else
    echo "Usage: ./deploy-locally.sh <PORT_NUMBER>"
    echo "Example: ./deploy-locally.sh 7777"
fi
