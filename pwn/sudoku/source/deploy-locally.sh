PORT=$1
if [ -n "$PORT" ]; then
    docker build -t encryptid-sudoku .
    echo "[*] RUNNING ON PORT $PORT"
    docker run -it --rm -p $PORT:3000 encryptid-sudoku
else
    echo "Usage: ./deploy-locally.sh <PORT_NUMBER>"
    echo "Example: ./deploy-locally.sh 7777"
fi