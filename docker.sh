echo docker.sh: start building docker image
docker buildx build --progress=plain -t rms314 . || exit

echo docker.sh: start running
docker run -it rms314 || exit
