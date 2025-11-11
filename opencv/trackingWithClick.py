import cv2
import sys

# Define available trackers
tracker_types = [
    "BOOSTING",
    "MIL",
    "KCF",
    "CSRT",
    "TLD",
    "MEDIANFLOW",
    "GOTURN",
    "MOSSE",
]

# Pick one tracker
tracker_type = tracker_types[3]  # e.g. CSRT (accurate but slower)

# Create tracker
if tracker_type == "BOOSTING":
    tracker = cv2.legacy.TrackerBoosting.create()
elif tracker_type == "MIL":
    tracker = cv2.legacy.TrackerMIL.create()
elif tracker_type == "KCF":
    tracker = cv2.TrackerKCF.create()
elif tracker_type == "CSRT":
    tracker = cv2.TrackerCSRT.create()
elif tracker_type == "TLD":
    tracker = cv2.legacy.TrackerTLD.create()
elif tracker_type == "MEDIANFLOW":
    tracker = cv2.legacy.TrackerMedianFlow.create()
elif tracker_type == "GOTURN":
    tracker = cv2.TrackerGOTURN.create()
else:
    tracker = cv2.legacy.TrackerMOSSE.create()

# Initialize webcam
cap = cv2.VideoCapture(1, cv2.CAP_DSHOW)
if not cap.isOpened():
    print("❌ Cannot open webcam")
    sys.exit()

# Read first frame
ok, frame = cap.read()
if not ok:
    print("❌ Cannot read video frame")
    sys.exit()

# Let user select the region to track
bbox = cv2.selectROI("Select Object to Track", frame, fromCenter=False)
cv2.destroyWindow("Select Object to Track")

# Initialize tracker with first frame and bounding box
ok = tracker.init(frame, bbox)

while True:
    ok, frame = cap.read()
    if not ok:
        break

    # Update tracker
    success, bbox = tracker.update(frame)

    if success:
        # Tracking success — draw bounding box
        p1 = (int(bbox[0]), int(bbox[1]))
        p2 = (int(bbox[0] + bbox[2]), int(bbox[1] + bbox[3]))
        cv2.rectangle(frame, p1, p2, (0, 255, 0), 2, 1)
        cv2.putText(frame, f"{tracker_type} Tracker", (10, 30),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.7, (50, 170, 50), 2)
    else:
        # Tracking failure
        cv2.putText(frame, "Tracking failure detected", (10, 30),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 255), 2)

    cv2.imshow("Object Tracking", frame)

    # Exit on 'q'
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
