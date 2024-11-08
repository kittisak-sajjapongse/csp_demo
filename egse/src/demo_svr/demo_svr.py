import logging
import random
import time
import uuid
import sys

logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - RequestID: %(request_id)s - %(message)s',
    handlers=[
        logging.StreamHandler(),
        # logging.FileHandler('demo_svr.log')
    ]
)

org_logger = logging.getLogger('demo_svr')

def simulate_activity():
    request_id = str(uuid.uuid4())
    logger = logging.LoggerAdapter(org_logger, {'request_id': request_id})
    time.sleep(random.uniform(1, 5))
    try:
        if random.random() < 0.2:
            logger.error('An error occurred while processing request')
            print(f"Error: An error occurred while processing request (RequestID: {request_id})", file=sys.stderr)
            raise RuntimeError('An error occurred while processing request')
        else:
            logger.info('Request processed successfully')
    except Exception as e:
        logger.error(f'An error occurred: {e}')

while True:
    simulate_activity()