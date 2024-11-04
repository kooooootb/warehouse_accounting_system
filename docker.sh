echo docker.sh: start building docker image
docker buildx build --progress=plain -t was . || exit

echo docker.sh: start running
docker run -it was || exit
