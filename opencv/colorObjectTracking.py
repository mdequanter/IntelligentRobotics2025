import cv2
import numpy as np

def main():
    cap = cv2.VideoCapture(1)

    while True:
        ret, frame = cap.read()
        if not ret:
            break

        # Converteer het beeld naar HSV-kleurruimte
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

        # Definieer het bereik van groene kleur in HSV
        lower_green = np.array([40, 50, 50])
        upper_green = np.array([80, 255, 255])

        # Creëer een masker voor het groene object
        mask = cv2.inRange(hsv, lower_green, upper_green)

        # Zoek contouren in het masker
        contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

        # Als er contouren zijn gevonden
        if contours:
            # Bepaal het grootste contour (het groene object)
            max_contour = max(contours, key=cv2.contourArea)
            # Bepaal de begrenzende rechthoek van het contour
            x, y, w, h = cv2.boundingRect(max_contour)
            # Teken een rechthoek rond het grootste contour
            cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)

        # Toon het beeld met het gevolgde object
        cv2.imshow("Green Object Tracking", frame)

        # Wacht op een toetsaanslag en stop de lus als de 'q' toets wordt ingedrukt
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    # Beëindig de webcam en sluit alle vensters
    cap.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
