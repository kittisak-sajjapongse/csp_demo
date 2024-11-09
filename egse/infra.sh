#!/usr/bin/env bash

set -e

# File to store the deployed tag
DEPLOYED_TAG_FILE=".deployed_tag"

# Function to display usage information
usage() {
    echo "Usage:"
    echo "  $0 up <tag>    - Start the infrastructure with the specified tag"
    echo "  $0 down        - Stop the infrastructure"
    exit 1
}

# Check if at least one argument is provided
if [ $# -lt 1 ]; then
    usage
fi

# Handle commands
case "$1" in
    up)
        # Check if a tag is provided for the 'up' command
        if [ $# -ne 2 ]; then
            echo "Error: 'up' command requires a tag argument"
            usage
        fi
        
        # Set the tag as an environment variable
        export IMAGE_TAG="$2"
        
        echo "Starting infrastructure with tag: $IMAGE_TAG"
        cd src; ./build.sh demo_svr $IMAGE_TAG; cd ..
        docker compose -f infra/docker-compose.yaml up -d
        
        # Write the tag to the .deployed_tag file
        echo "$IMAGE_TAG" > "$DEPLOYED_TAG_FILE"
        echo "Deployed tag written to $DEPLOYED_TAG_FILE"
        ;;
    
    down)
        # 'down' command doesn't require additional arguments
        if [ $# -ne 1 ]; then
            echo "Error: 'down' command doesn't accept additional arguments"
            usage
        fi

        export IMAGE_TAG=$(cat $DEPLOYED_TAG_FILE)
        
        echo "Stopping infrastructure"
        docker compose  -f infra/docker-compose.yaml down
        
        # Delete the .deployed_tag file if it exists
        if [ -f "$DEPLOYED_TAG_FILE" ]; then
            rm "$DEPLOYED_TAG_FILE"
            echo "Removed $DEPLOYED_TAG_FILE"
        else
            echo "No $DEPLOYED_TAG_FILE file found"
        fi
        ;;
    
    *)
        echo "Error: Unknown command '$1'"
        usage
        ;;
esac
