import cv2
import datetime
import os

IP = "192.168.0.77"

def record_video_and_capture_images():
    # Start video capture
    # cap = cv2.VideoCapture(0)
    cap = cv2.VideoCapture(IP+":8080/?action=stream")

    # Define the codec for video recording
    fourcc = cv2.VideoWriter_fourcc(*'XVID')
    out = None

    while True:
        ret, frame = cap.read()
        if not ret:
            break

        # Initialize the VideoWriter object when the frame is available
        if out is None:
            timestamp = datetime.datetime.now().strftime("%Y%m%d%H%M%S")
            out = cv2.VideoWriter(f'{timestamp}.avi', fourcc, 20.0, (1280, 960))

        # Write the frame to the video file
        out.write(frame)

        # Display the frame
        cv2.imshow('frame', frame)

        # Check for key presses
        key = cv2.waitKey(1)

        # Save image when space key is pressed
        if key & 0xFF == 32:  # ASCII value of space key is 32
            img_timestamp = datetime.datetime.now().strftime("%Y%m%d%H%M%S")
            cv2.imwrite(f'{img_timestamp}.png', frame)

        # Break the loop when 'q' is pressed
        if key & 0xFF == ord('q'):
            break

    # Release everything when job is finished
    cap.release()
    out.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    record_video_and_capture_images()
