import cv2

# URL van de stream (pas aan indien nodig)
url = "http://192.168.0.81:8000"

# Open de video stream
cap = cv2.VideoCapture(url)

if not cap.isOpened():
    print("❌ Kan de stream niet openen. Controleer het IP-adres en poortnummer.")
    exit()

print("✅ Verbonden met stream. Druk op 'q' om te stoppen.")

while True:
    ret, frame = cap.read()
    if not ret:
        print("⚠️ Geen frame ontvangen (stream verbroken?).")
        break

    # Toon het beeld
    cv2.imshow("Live Stream", frame)

    # Stoppen met 'q'
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Alles afsluiten
cap.release()
cv2.destroyAllWindows()
