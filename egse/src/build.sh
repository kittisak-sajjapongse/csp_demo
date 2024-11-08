#!/usr/bin/env bash
USAGE_TEXT="{demo_svr}"

if [ $# -ne 2 ]; then
  echo "Usage: $0 $USAGE_TEXT {tag}" >&2
  exit 1
fi

if [ -z "$1" ] || [ -z "$2" ]; then
    echo "Usage: $0 $USAGE_TEXT {tag}" >&2
    exit 1
fi

function check_local_image_exists {
    local image="$1"
    local tag="$2"

    # Use docker images with filter to check for the specific image and tag
    if docker images --filter=reference="${image}:${tag}" --format "{{.Repository}}:{{.Tag}}" | grep -q "${image}:${tag}"; then
        echo "Image ${image}:${tag} exists locally"
        return 0
    else
        echo "Image ${image}:${tag} does not exist locally"
        return 1
    fi
}

function demo_svr {
    local DEMO_SVR_TAG=$1
    if check_local_image_exists demo_svr $DEMO_SVR_TAG; then
        echo "Stopping execution because the image exists locally."
        exit 0
    fi
    set -x
    docker build -t demo_svr:$DEMO_SVR_TAG -f demo_svr/Dockerfile .
    set +x
}

TAG=$2
case $1 in
    demo_svr)
        demo_svr $TAG
        ;;
    *)
        echo "Invalid argument. Usage: $0 $USAGE_TEXT {tag}"
        exit 1
        ;;
esac
