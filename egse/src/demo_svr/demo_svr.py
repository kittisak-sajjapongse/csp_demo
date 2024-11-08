import json
import logging
import random
import time
import uuid
import sys

class JsonFormatter(logging.Formatter):
    def format(self, record):
        message = super().format(record)
        return json.dumps({
            'asctime': self.formatTime(record, self.datefmt),
            'name': record.name,
            'levelname': record.levelname,
            'request_id': record.request_id,
            'message': message
        })

# logging.basicConfig(
#     level=logging.INFO,
#     format='%(asctime)s - %(name)s - %(levelname)s - RequestID: %(request_id)s - %(message)s',
#     handlers=[
#         logging.StreamHandler(),
#         # logging.FileHandler('demo_svr.log')
#     ]
# )

org_logger = logging.getLogger('demo_svr')
org_logger.setLevel(logging.INFO)

stream_handler = logging.StreamHandler()
stream_handler.setFormatter(JsonFormatter())

org_logger.addHandler(stream_handler)

def simulate_activity():
    request_id = str(uuid.uuid4())
    logger = logging.LoggerAdapter(org_logger, {'request_id': request_id})
    time.sleep(random.uniform(1, 5))
    try:
        if random.random() < 0.2:
            logger.error('An error occurred while processing request')
            raise RuntimeError('An error occurred while processing request')
        else:
            logger.info('Request processed successfully')
    except Exception as e:
        logger.error(f'An error occurred: {e}')

while True:
    simulate_activity()