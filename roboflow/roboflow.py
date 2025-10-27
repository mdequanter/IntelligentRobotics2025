
from inference_sdk import InferenceHTTPClient

CLIENT = InferenceHTTPClient(
    api_url="http://localhost:9001",
    api_key="WgLYEMfa0WjwZWHJhwlO"
)

image_url = "img1.png"
result = CLIENT.infer(image_url, model_id="unrealsim/1")
print(result)