import cv2
from inference_sdk import InferenceHTTPClient
import numpy as np
import os

API_KEY = os.getenv("ROBOFLOW_API_KEY")

# Verbinding met lokale inference server
CLIENT = InferenceHTTPClient(
    api_url="http://localhost:9001",
    api_key=API_KEY
)

# Start webcam
cap = cv2.VideoCapture(1)

if not cap.isOpened():
    print("Kan de webcam niet openen.")
    exit()

while True:
    ret, frame = cap.read()
    if not ret:
        print("Kon frame niet lezen.")
        break

    # Sla tijdelijk frame op of stuur rechtstreeks als array
    cv2.imwrite("frame.jpg", frame)

    # Inference uitvoeren
    results = CLIENT.infer("frame.jpg", model_id="unrealsim/1")

    # Resultaat tonen in console
    # print(result)
    height, width = frame.shape[:2]  # neem echte framegrootte
    SCAN_HEIGHTS = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7]

    overlay = frame.copy()
    midpoints = []

    # results kan ofwel een dict {"predictions":[...]} zijn of meteen een list van preds
    preds = []
    if isinstance(results, dict) and "predictions" in results:
        preds = results["predictions"]
    elif isinstance(results, list):
        preds = results
    else:
        preds = []

    # 1) Maak een lege union-mask
    union_mask = np.zeros((height, width), dtype=np.uint8)

    # 2) Rasterizeer elk polygon naar union_mask
    for p in preds:
        if "points" in p and isinstance(p["points"], (list, tuple)) and len(p["points"]) >= 3:
            # lijst van dicts {'x':..,'y':..} -> Nx2 array int32
            pts = np.array([[pt["x"], pt["y"]] for pt in p["points"]], dtype=np.int32)

            # Optioneel clippen naar beeldgrenzen
            pts[:, 0] = np.clip(pts[:, 0], 0, width - 1)
            pts[:, 1] = np.clip(pts[:, 1], 0, height - 1)

            cv2.fillPoly(union_mask, [pts.reshape(-1, 1, 2)], 255)

    # 3) Visualisatie (groene overlay op mask)
    if union_mask.any():
        green_overlay = np.full_like(frame, (0, 255, 0))
        blended = cv2.addWeighted(frame, 0.3, green_overlay, 0.7, 0)
        overlay[union_mask > 0] = blended[union_mask > 0]

    # 4) Scanlijnen + midpoints
    for r in SCAN_HEIGHTS:
        y = int(height * r)
        if y >= height:
            continue

        row = union_mask[y, :]
        xs = np.where(row > 0)[0]

        if xs.size > 0:
            mid_x = int(xs.mean())
            midpoints.append((mid_x, y))
            cv2.circle(overlay, (mid_x, y), 5, (255, 0, 0), -1)

        cv2.line(overlay, (0, y), (width, y), (150, 150, 150), 1)

    # 5) (optioneel) pijl tekenen zoals bij jou
    if midpoints:
        avg_x = int(np.mean([pt[0] for pt in midpoints]))
        target_point = (avg_x, min(pt[1] for pt in midpoints))
        start_point = (width // 2, height)
        cv2.arrowedLine(overlay, start_point, target_point, (0, 0, 255), 5, tipLength=0.2)




    # Toon het beeld
    cv2.imshow("Webcam", frame)

    # Druk op 'q' om te stoppen
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
