import cv2

def main():
    # TCP/IP address and port
    stream_address = 'tcp://10.2.172.130:3000'

    # Create a VideoCapture object
    cap = cv2.VideoCapture(stream_address)

    while(cap.isOpened()):
        ret, frame = cap.read()
        if ret:
            # Display the frame
            cv2.imshow('frame', frame)

            if cv2.waitKey(1) & 0xFF == ord('q'):
                break
        else:
            break
    cap.release()
    cv2.destroyAllWindows()
if __name__ == "__main__":
    main()
