# Gebruik deze code om de camerastream van DroidCam in Python te openen.
# Start eerst de DroidCam-app op je smartphone en noteer het IP-adres of verbind via USB.
# Pas het juiste camera-indexnummer of IP-adres aan in cv2.VideoCapture().
# Druk op 'q' om het videovenster te sluiten.

import cv2

url = "http://192.168.0.58:4747/video"
cap = cv2.VideoCapture(url)

while True:
    ret, frame = cap.read()
    if not ret:
        break

    cv2.imshow("DroidCam Stream", frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
