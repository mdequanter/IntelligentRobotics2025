import cv2
import numpy as np
from inference_sdk import InferenceHTTPClient
import time

# ========== Config ==========
VIDEO_PATH = "input.mp4"              # <-- pad naar je video
API_URL = "http://localhost:9001"
API_KEY = "WgLYEMfa0WjwZWHJhwlO"
MODEL_ID = "unrealsim/1"
SCAN_HEIGHTS = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7]
DRAW_OVERLAY = True                   # zet op False voor alleen pijl
RESIZE_TO = None                      # bv. (640, 480) of None om originele resolutie te behouden
SHOW_FPS = True

# ========== Inference client ==========
CLIENT = InferenceHTTPClient(api_url=API_URL, api_key=API_KEY)

# ========== Helpers ==========
def infer_frame(frame_bgr):
    # OpenCV is BGR; de meeste libs verwachten RGB
    rgb = cv2.cvtColor(frame_bgr, cv2.COLOR_BGR2RGB)
    return CLIENT.infer(rgb, model_id=MODEL_ID)



def infer_bytes(frame_bgr):
    ok_enc, buffer = cv2.imencode(".jpg", frame_bgr, [cv2.IMWRITE_JPEG_QUALITY, 50])
    if not ok_enc:
        return None
    return CLIENT.infer(buffer.tobytes(), model_id=MODEL_ID)

def get_predictions(results):
    # results kan {"predictions":[...]} of list zijn
    if isinstance(results, dict) and "predictions" in results:
        return results["predictions"]
    if isinstance(results, list):
        return results
    return []

def polygons_to_union_mask(preds, width, height):
    mask = np.zeros((height, width), dtype=np.uint8)
    for p in preds:
        pts_list = p.get("points")
        if not (isinstance(pts_list, (list, tuple)) and len(pts_list) >= 3):
            continue
        # lijst van dicts {'x':..,'y':..} -> Nx2 array int32
        pts = np.array([[pt["x"], pt["y"]] for pt in pts_list], dtype=np.float32)
        # clip + cast naar int
        pts[:, 0] = np.clip(pts[:, 0], 0, width - 1)
        pts[:, 1] = np.clip(pts[:, 1], 0, height - 1)
        pts = pts.astype(np.int32).reshape(-1, 1, 2)
        cv2.fillPoly(mask, [pts], 255)
    return mask

# ========== Video-loop ==========
cap = cv2.VideoCapture(VIDEO_PATH)
if not cap.isOpened():
    raise RuntimeError(f"Kon de video niet openen: {VIDEO_PATH}")

prev_t = time.time()
while True:
    ret, frame = cap.read()
    if not ret:
        break

    if RESIZE_TO is not None:
        frame = cv2.resize(frame, RESIZE_TO)

    h, w = frame.shape[:2]
    overlay = frame.copy()

    t0 = time.time()
    #results = infer_bytes(frame)
    results = infer_frame(frame)
    t1 = time.time()
    preds = get_predictions(results)

    # Mask (unie van alle polygonen)
    union_mask = polygons_to_union_mask(preds, w, h)

    # Optionele groene overlay
    if DRAW_OVERLAY and union_mask.any():
        green_overlay = np.full_like(frame, (0, 255, 0))
        blended = cv2.addWeighted(frame, 0.3, green_overlay, 0.7, 0)
        overlay[union_mask > 0] = blended[union_mask > 0]

    # Scanlijnen en midpoints
    midpoints = []
    for r in SCAN_HEIGHTS:
        y = int(h * r)
        if y >= h:
            continue
        row = union_mask[y, :]
        xs = np.where(row > 0)[0]
        if xs.size > 0:
            mid_x = int(xs.mean())
            midpoints.append((mid_x, y))
            cv2.circle(overlay, (mid_x, y), 5, (255, 0, 0), -1)
        cv2.line(overlay, (0, y), (w, y), (150, 150, 150), 1)

    # Pijl tekenen
    if midpoints:
        avg_x = int(np.mean([pt[0] for pt in midpoints]))
        target_point = (avg_x, min(pt[1] for pt in midpoints))
        start_point = (w // 2, h)
        cv2.arrowedLine(overlay, start_point, target_point, (0, 0, 255), 5, tipLength=0.2)

    # Info overlay (FPS / inference tijd)
    if SHOW_FPS:
        now = time.time()
        loop_dt = now - prev_t
        prev_t = now
        fps = 1.0 / loop_dt if loop_dt > 0 else 0.0
        inf_ms = (t1 - t0) * 1000.0
        cv2.putText(overlay, f"FPS: {fps:.1f} | Inference: {inf_ms:.1f} ms",
                    (10, 25), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (10, 220, 10), 2, cv2.LINE_AA)

    cv2.imshow("Video + Local Inference (mask -> arrow)", overlay)
    key = cv2.waitKey(1) & 0xFF
    if key in (ord('q'), 27):  # q of ESC
        break

cap.release()
cv2.destroyAllWindows()
